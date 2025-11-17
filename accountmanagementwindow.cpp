#include "accountmanagementwindow.h"
#include "mainwindow.h"
#include "adduserwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>

AccountManagementWindow::AccountManagementWindow(UserManager *userManager, QWidget *parent)
    : QMainWindow(parent), m_userManager(userManager), contextMenu(nullptr), m_selectedRow(-1)
{
    setupUI();
    applyStyle();
    setWindowTitle("Управление аккаунтами");
    setFixedSize(900, 700);

    updateTables();
}

void AccountManagementWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 15, 20, 15);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ =====
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    QLabel *titleLabel = new QLabel("УПРАВЛЕНИЕ АККАУНТАМИ");
    titleLabel->setAlignment(Qt::AlignCenter);

    // Кнопка добавления в правом верхнем углу
    QPushButton *addButton = new QPushButton("+ Добавить");
    addButton->setFixedSize(120, 35);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(addButton);

    // ===== ОСНОВНОЙ КОНТЕНТ С ПРОКРУТКОЙ =====
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(25);

    // ===== ТАБЛИЦА СОТРУДНИКОВ =====
    QWidget *employeesWidget = new QWidget();
    QVBoxLayout *employeesLayout = new QVBoxLayout(employeesWidget);
    employeesLayout->setSpacing(10);
    employeesLayout->setContentsMargins(0, 0, 0, 0);

    employeesLabel = new QLabel("ЗАРЕГИСТРИРОВАННЫЕ СОТРУДНИКИ");
    employeesLabel->setAlignment(Qt::AlignCenter);

    // Панель поиска для сотрудников
    QWidget *employeesSearchPanel = new QWidget();
    QHBoxLayout *employeesSearchLayout = new QHBoxLayout(employeesSearchPanel);
    employeesSearchLayout->setContentsMargins(0, 0, 0, 0);
    employeesSearchLayout->setSpacing(10);

    QLabel *employeesSearchLabel = new QLabel("Поиск:");
    employeesSearchLabel->setFixedSize(50, 30);

    employeesSearchEdit = new QLineEdit();
    employeesSearchEdit->setPlaceholderText("Введите текст для поиска...");
    employeesSearchEdit->setFixedHeight(30);

    employeesSearchCombo = new QComboBox();
    employeesSearchCombo->addItem("Фамилия");
    employeesSearchCombo->addItem("Имя");
    employeesSearchCombo->addItem("Отчество");
    employeesSearchCombo->setFixedHeight(30);
    employeesSearchCombo->setFixedWidth(150);

    employeesSearchLayout->addStretch();
    employeesSearchLayout->addWidget(employeesSearchLabel);
    employeesSearchLayout->addWidget(employeesSearchEdit);
    employeesSearchLayout->addWidget(employeesSearchCombo);

    // Таблица сотрудников
    employeesTable = new QTableWidget();
    setupTable(employeesTable, {"Фамилия", "Имя", "Отчество", "Логин"});

    employeesLayout->addWidget(employeesLabel);
    employeesLayout->addWidget(employeesSearchPanel);
    employeesLayout->addWidget(employeesTable);

    // ===== ТАБЛИЦА АДМИНИСТРАТОРОВ =====
    QWidget *adminsWidget = new QWidget();
    QVBoxLayout *adminsLayout = new QVBoxLayout(adminsWidget);
    adminsLayout->setSpacing(10);
    adminsLayout->setContentsMargins(0, 0, 0, 0);

    adminsLabel = new QLabel("ЗАРЕГИСТРИРОВАННЫЕ АДМИНИСТРАТОРЫ");
    adminsLabel->setAlignment(Qt::AlignCenter);

    // Панель поиска для администраторов
    QWidget *adminsSearchPanel = new QWidget();
    QHBoxLayout *adminsSearchLayout = new QHBoxLayout(adminsSearchPanel);
    adminsSearchLayout->setContentsMargins(0, 0, 0, 0);
    adminsSearchLayout->setSpacing(10);

    QLabel *adminsSearchLabel = new QLabel("Поиск:");
    adminsSearchLabel->setFixedSize(50, 30);

    adminsSearchEdit = new QLineEdit();
    adminsSearchEdit->setPlaceholderText("Введите текст для поиска...");
    adminsSearchEdit->setFixedHeight(30);

    adminsSearchCombo = new QComboBox();
    adminsSearchCombo->addItem("Фамилия");
    adminsSearchCombo->addItem("Имя");
    adminsSearchCombo->addItem("Отчество");
    adminsSearchCombo->setFixedHeight(30);
    adminsSearchCombo->setFixedWidth(150);

    adminsSearchLayout->addStretch();
    adminsSearchLayout->addWidget(adminsSearchLabel);
    adminsSearchLayout->addWidget(adminsSearchEdit);
    adminsSearchLayout->addWidget(adminsSearchCombo);

    // Таблица администраторов
    adminsTable = new QTableWidget();
    setupTable(adminsTable, {"Фамилия", "Имя", "Отчество", "Логин"});

    adminsLayout->addWidget(adminsLabel);
    adminsLayout->addWidget(adminsSearchPanel);
    adminsLayout->addWidget(adminsTable);

    // ===== ТАБЛИЦА ОЖИДАЮЩИХ ПОЛЬЗОВАТЕЛЕЙ =====
    QWidget *pendingUsersWidget = new QWidget();
    QVBoxLayout *pendingUsersLayout = new QVBoxLayout(pendingUsersWidget);
    pendingUsersLayout->setSpacing(10);
    pendingUsersLayout->setContentsMargins(0, 0, 0, 0);

    pendingUsersLabel = new QLabel("НЕЗАРЕГИСТРИРОВАННЫЕ ПОЛЬЗОВАТЕЛИ");
    pendingUsersLabel->setAlignment(Qt::AlignCenter);

    // Панель поиска для ожидающих пользователей
    QWidget *pendingUsersSearchPanel = new QWidget();
    QHBoxLayout *pendingUsersSearchLayout = new QHBoxLayout(pendingUsersSearchPanel);
    pendingUsersSearchLayout->setContentsMargins(0, 0, 0, 0);
    pendingUsersSearchLayout->setSpacing(10);

    QLabel *pendingUsersSearchLabel = new QLabel("Поиск:");
    pendingUsersSearchLabel->setFixedSize(50, 30);

    pendingUsersSearchEdit = new QLineEdit();
    pendingUsersSearchEdit->setPlaceholderText("Введите текст для поиска...");
    pendingUsersSearchEdit->setFixedHeight(30);

    pendingUsersSearchCombo = new QComboBox();
    pendingUsersSearchCombo->addItem("Фамилия");
    pendingUsersSearchCombo->addItem("Имя");
    pendingUsersSearchCombo->addItem("Отчество");
    pendingUsersSearchCombo->setFixedHeight(30);
    pendingUsersSearchCombo->setFixedWidth(150);

    pendingUsersSearchLayout->addStretch();
    pendingUsersSearchLayout->addWidget(pendingUsersSearchLabel);
    pendingUsersSearchLayout->addWidget(pendingUsersSearchEdit);
    pendingUsersSearchLayout->addWidget(pendingUsersSearchCombo);

    // Таблица ожидающих пользователей
    pendingUsersTable = new QTableWidget();
    setupTable(pendingUsersTable, {"Фамилия", "Имя", "Отчество"});

    pendingUsersLayout->addWidget(pendingUsersLabel);
    pendingUsersLayout->addWidget(pendingUsersSearchPanel);
    pendingUsersLayout->addWidget(pendingUsersTable);

    // Собираем все таблицы в scroll area
    scrollLayout->addWidget(employeesWidget);
    scrollLayout->addWidget(adminsWidget);
    scrollLayout->addWidget(pendingUsersWidget);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);

    // Собираем главный layout
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(scrollArea);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &AccountManagementWindow::onBackClicked);
    connect(addButton, &QPushButton::clicked, this, &AccountManagementWindow::onAddUserClicked);

    connect(employeesSearchEdit, &QLineEdit::textChanged, this, &AccountManagementWindow::onSearchTextChanged);
    connect(employeesSearchCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AccountManagementWindow::onSearchCriteriaChanged);

    connect(adminsSearchEdit, &QLineEdit::textChanged, this, &AccountManagementWindow::onSearchTextChanged);
    connect(adminsSearchCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AccountManagementWindow::onSearchCriteriaChanged);

    connect(pendingUsersSearchEdit, &QLineEdit::textChanged, this, &AccountManagementWindow::onSearchTextChanged);
    connect(pendingUsersSearchCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &AccountManagementWindow::onSearchCriteriaChanged);

    // Настраиваем контекстные меню
    setupContextMenu(employeesTable, "employee");
    setupContextMenu(adminsTable, "admin");
    setupContextMenu(pendingUsersTable, "pending");
}

void AccountManagementWindow::applyStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f0f0;
        }
        QWidget {
            background-color: #f0f0f0;
        }
        QLabel {
            color: #2c3e50;
            font-size: 18px;
            font-weight: bold;
            padding: 10px;
            margin: 5px;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #2471a3;
        }
        QPushButton#backButton {
            background-color: #95a5a6;
        }
        QPushButton#backButton:hover {
            background-color: #7f8c8d;
        }
        QTableWidget {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 10px;
            gridline-color: #ecf0f1;
            font-size: 14px;
            alternate-background-color: #e8f4fd;
        }
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #ecf0f1;
            color: #000000;
        }
        QTableWidget::item:selected {
            background-color: #3498db;
            color: white;
        }
        QHeaderView::section {
            background-color: #34495e;
            color: white;
            padding: 10px;
            border: none;
            font-weight: bold;
        }
        QLineEdit {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 5px 10px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
        }
        QLineEdit:focus {
            border-color: #3498db;
        }
        QComboBox {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 5px 10px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
        }
        QComboBox:focus {
            border-color: #3498db;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            border: 1px solid #bdc3c7;
            selection-background-color: #3498db;
            color: #000000;
        }
        QMenu {
            background-color: white;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            padding: 5px;
        }
        QMenu::item {
            padding: 5px 15px;
            color: #000000;
            font-size: 14px;
        }
        QMenu::item:selected {
            background-color: #3498db;
            color: white;
        }
    )");

    backButton->setObjectName("backButton");
}

void AccountManagementWindow::setupTable(QTableWidget *table, const QStringList &headers)
{
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setSortingEnabled(true);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setFixedHeight(200);
    table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void AccountManagementWindow::loadEmployeesTable() {
    m_employeesData = m_userManager->getEmployeesData();
    employeesTable->setRowCount(0);

    employeesTable->setRowCount(m_employeesData.size());
    for (int i = 0; i < m_employeesData.size(); ++i) {
        const QStringList &userData = m_employeesData.at(i);
        for (int j = 0; j < userData.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(userData.at(j));
            employeesTable->setItem(i, j, item);
        }
    }
}

void AccountManagementWindow::loadAdminsTable() {
    m_adminsData = m_userManager->getAdminsData();
    adminsTable->setRowCount(0);

    adminsTable->setRowCount(m_adminsData.size());
    for (int i = 0; i < m_adminsData.size(); ++i) {
        const QStringList &userData = m_adminsData.at(i);
        for (int j = 0; j < userData.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(userData.at(j));
            adminsTable->setItem(i, j, item);
        }
    }
}

void AccountManagementWindow::loadPendingUsersTable() {
    m_pendingUsersData = m_userManager->getPendingUsersData();
    pendingUsersTable->setRowCount(0);

    pendingUsersTable->setRowCount(m_pendingUsersData.size());
    for (int i = 0; i < m_pendingUsersData.size(); ++i) {
        const QStringList &userData = m_pendingUsersData.at(i);
        for (int j = 0; j < userData.size(); ++j) {
            QTableWidgetItem *item = new QTableWidgetItem(userData.at(j));
            pendingUsersTable->setItem(i, j, item);
        }
    }
}

void AccountManagementWindow::updateTables()
{
    loadEmployeesTable();
    loadAdminsTable();
    loadPendingUsersTable();
}

void AccountManagementWindow::filterTable(QTableWidget *table, const QList<QStringList> &allData, const QString& searchText, int searchCriteria)
{
    if (searchText.isEmpty()) {
        table->setRowCount(allData.size());
        for (int i = 0; i < allData.size(); ++i) {
            const QStringList &userData = allData.at(i);
            for (int j = 0; j < userData.size(); ++j) {
                QTableWidgetItem *item = new QTableWidgetItem(userData.at(j));
                table->setItem(i, j, item);
            }
        }
    } else {
        QList<QStringList> filteredData;
        for (const QStringList &userData : allData) {
            bool match = false;
            QString searchLower = searchText.toLower();

            if (searchCriteria < userData.size()) {
                match = userData.at(searchCriteria).toLower().contains(searchLower);
            }

            if (match) {
                filteredData.append(userData);
            }
        }

        table->setRowCount(filteredData.size());
        for (int i = 0; i < filteredData.size(); ++i) {
            const QStringList &userData = filteredData.at(i);
            for (int j = 0; j < userData.size(); ++j) {
                QTableWidgetItem *item = new QTableWidgetItem(userData.at(j));
                table->setItem(i, j, item);
            }
        }
    }
}

void AccountManagementWindow::setupContextMenu(QTableWidget *table, const QString& userType)
{
    table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(table, &QTableWidget::customContextMenuRequested, this, [this, table, userType](const QPoint &pos) {
        QTableWidgetItem *item = table->itemAt(pos);
        if (item) {
            m_selectedRow = item->row();
            m_selectedUserType = userType;

            contextMenu = new QMenu(this);
            QAction *deleteAction = new QAction("Удалить", this);
            connect(deleteAction, &QAction::triggered, this, &AccountManagementWindow::onDeleteUser);
            contextMenu->addAction(deleteAction);

            contextMenu->exec(table->viewport()->mapToGlobal(pos));
            delete contextMenu;
            contextMenu = nullptr;
        }
    });
}

void AccountManagementWindow::onBackClicked()
{
    MainWindow *mainWindow = new MainWindow(true, m_userManager);
    mainWindow->show();
    this->close();
}

void AccountManagementWindow::onSearchTextChanged(const QString& text)
{
    QLineEdit *senderEdit = qobject_cast<QLineEdit*>(sender());

    if (senderEdit == employeesSearchEdit) {
        filterTable(employeesTable, m_employeesData, text, employeesSearchCombo->currentIndex());
    } else if (senderEdit == adminsSearchEdit) {
        filterTable(adminsTable, m_adminsData, text, adminsSearchCombo->currentIndex());
    } else if (senderEdit == pendingUsersSearchEdit) {
        filterTable(pendingUsersTable, m_pendingUsersData, text, pendingUsersSearchCombo->currentIndex());
    }
}

void AccountManagementWindow::onSearchCriteriaChanged(int index)
{
    QComboBox *senderCombo = qobject_cast<QComboBox*>(sender());

    if (senderCombo == employeesSearchCombo) {
        filterTable(employeesTable, m_employeesData, employeesSearchEdit->text(), index);
    } else if (senderCombo == adminsSearchCombo) {
        filterTable(adminsTable, m_adminsData, adminsSearchEdit->text(), index);
    } else if (senderCombo == pendingUsersSearchCombo) {
        filterTable(pendingUsersTable, m_pendingUsersData, pendingUsersSearchEdit->text(), index);
    }
}

void AccountManagementWindow::onDeleteUser() {
    if (m_selectedRow < 0) return;

    bool success = false;
    QString message;
    QString userInfo;

    if (m_selectedUserType == "employee") {
        if (m_selectedRow < m_employeesData.size()) {
            const QStringList &userData = m_employeesData.at(m_selectedRow);
            QString login = userData.at(3);
            userInfo = QString("Сотрудник: %1 %2 %3\nЛогин: %4")
                           .arg(userData.at(0))
                           .arg(userData.at(1))
                           .arg(userData.at(2))
                           .arg(login);

            success = m_userManager->removeEmployeeByLogin(login);
            message = "Сотрудник успешно удален!";
        }
    } else if (m_selectedUserType == "admin") {
        if (m_selectedRow < m_adminsData.size()) {
            const QStringList &userData = m_adminsData.at(m_selectedRow);
            QString login = userData.at(3);
            userInfo = QString("Администратор: %1 %2 %3\nЛогин: %4")
                           .arg(userData.at(0))
                           .arg(userData.at(1))
                           .arg(userData.at(2))
                           .arg(login);

            success = m_userManager->removeAdminByLogin(login);
            message = "Администратор успешно удален!";
        }
    } else if (m_selectedUserType == "pending") {
        if (m_selectedRow < m_pendingUsersData.size()) {
            const QStringList &userData = m_pendingUsersData.at(m_selectedRow);
            QString fullName = userData.at(0) + " " + userData.at(1) + " " + userData.at(2);
            userInfo = QString("Ожидающий пользователь: %1 %2 %3")
                           .arg(userData.at(0))
                           .arg(userData.at(1))
                           .arg(userData.at(2));

            success = m_userManager->removePendingUserByName(fullName);
            message = "Ожидающий пользователь успешно удален!";
        }
    }

    if (userInfo.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Не удалось получить данные пользователя");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить этого пользователя?\n\n%1").arg(userInfo),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        if (success) {
            updateTables();
            QMessageBox::information(this, "Успех", message);
        } else {
            QMessageBox::critical(this, "Ошибка", "Не удалось удалить пользователя");
        }
    }

    m_selectedRow = -1;
    m_selectedUserType = "";
}

void AccountManagementWindow::onAddUserClicked()
{
    AddUserWindow *addUserWindow = new AddUserWindow(m_userManager);
    addUserWindow->show();
    this->close();
}
