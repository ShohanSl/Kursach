#include "accountmanagementwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include "mainwindow.h"
#include "adduserwindow.h"

AccountManagementWindow::AccountManagementWindow(UserManager *userManager, QWidget *parent)
    : QMainWindow(parent), m_userManager(userManager), m_selectedRow(-1)
{
    setupUI();
    setWindowTitle("Управление аккаунтами");
    setFixedSize(900, 700);
    updateTables();
}

void AccountManagementWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 15, 20, 15);

    // Верхняя панель
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 30);

    QLabel *titleLabel = new QLabel("УПРАВЛЕНИЕ АККАУНТАМИ");
    titleLabel->setAlignment(Qt::AlignCenter);

    QPushButton *addButton = new QPushButton("+ Добавить");
    addButton->setFixedSize(120, 30);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(addButton);

    // Таблица сотрудников
    QLabel *employeesLabel = new QLabel("ЗАРЕГИСТРИРОВАННЫЕ СОТРУДНИКИ");
    employeesLabel->setAlignment(Qt::AlignCenter);

    QWidget *employeesSearchPanel = new QWidget();
    QHBoxLayout *employeesSearchLayout = new QHBoxLayout(employeesSearchPanel);
    employeesSearchLayout->setContentsMargins(0, 0, 0, 0);
    employeesSearchLayout->setSpacing(10);

    QLabel *employeesSearchLabel = new QLabel("Поиск:");
    employeesSearchLabel->setFixedSize(40, 25);

    employeesSearchEdit = new QLineEdit();
    employeesSearchEdit->setPlaceholderText("Введите текст для поиска...");
    employeesSearchEdit->setFixedHeight(25);

    employeesSearchCombo = new QComboBox();
    employeesSearchCombo->addItem("Фамилия");
    employeesSearchCombo->addItem("Имя");
    employeesSearchCombo->addItem("Отчество");
    employeesSearchCombo->setFixedHeight(25);
    employeesSearchCombo->setFixedWidth(120);

    employeesSearchLayout->addStretch();
    employeesSearchLayout->addWidget(employeesSearchLabel);
    employeesSearchLayout->addWidget(employeesSearchEdit);
    employeesSearchLayout->addWidget(employeesSearchCombo);
    employeesSearchLayout->addStretch();

    employeesTable = new QTableWidget();
    setupTable(employeesTable, {"Фамилия", "Имя", "Отчество", "Логин"});

    // Таблица администраторов
    QLabel *adminsLabel = new QLabel("ЗАРЕГИСТРИРОВАННЫЕ АДМИНИСТРАТОРЫ");
    adminsLabel->setAlignment(Qt::AlignCenter);

    QWidget *adminsSearchPanel = new QWidget();
    QHBoxLayout *adminsSearchLayout = new QHBoxLayout(adminsSearchPanel);
    adminsSearchLayout->setContentsMargins(0, 0, 0, 0);
    adminsSearchLayout->setSpacing(10);

    QLabel *adminsSearchLabel = new QLabel("Поиск:");
    adminsSearchLabel->setFixedSize(40, 25);

    adminsSearchEdit = new QLineEdit();
    adminsSearchEdit->setPlaceholderText("Введите текст для поиска...");
    adminsSearchEdit->setFixedHeight(25);

    adminsSearchCombo = new QComboBox();
    adminsSearchCombo->addItem("Фамилия");
    adminsSearchCombo->addItem("Имя");
    adminsSearchCombo->addItem("Отчество");
    adminsSearchCombo->setFixedHeight(25);
    adminsSearchCombo->setFixedWidth(120);

    adminsSearchLayout->addStretch();
    adminsSearchLayout->addWidget(adminsSearchLabel);
    adminsSearchLayout->addWidget(adminsSearchEdit);
    adminsSearchLayout->addWidget(adminsSearchCombo);
    adminsSearchLayout->addStretch();

    adminsTable = new QTableWidget();
    setupTable(adminsTable, {"Фамилия", "Имя", "Отчество", "Логин"});

    // Таблица незарегистрированных пользователей
    QLabel *pendingUsersLabel = new QLabel("НЕЗАРЕГИСТРИРОВАННЫЕ ПОЛЬЗОВАТЕЛИ");
    pendingUsersLabel->setAlignment(Qt::AlignCenter);

    QWidget *pendingUsersSearchPanel = new QWidget();
    QHBoxLayout *pendingUsersSearchLayout = new QHBoxLayout(pendingUsersSearchPanel);
    pendingUsersSearchLayout->setContentsMargins(0, 0, 0, 0);
    pendingUsersSearchLayout->setSpacing(10);

    QLabel *pendingUsersSearchLabel = new QLabel("Поиск:");
    pendingUsersSearchLabel->setFixedSize(40, 25);

    pendingUsersSearchEdit = new QLineEdit();
    pendingUsersSearchEdit->setPlaceholderText("Введите текст для поиска...");
    pendingUsersSearchEdit->setFixedHeight(25);

    pendingUsersSearchCombo = new QComboBox();
    pendingUsersSearchCombo->addItem("Фамилия");
    pendingUsersSearchCombo->addItem("Имя");
    pendingUsersSearchCombo->addItem("Отчество");
    pendingUsersSearchCombo->addItem("Роль");
    pendingUsersSearchCombo->setFixedHeight(25);
    pendingUsersSearchCombo->setFixedWidth(120);

    pendingUsersSearchLayout->addStretch();
    pendingUsersSearchLayout->addWidget(pendingUsersSearchLabel);
    pendingUsersSearchLayout->addWidget(pendingUsersSearchEdit);
    pendingUsersSearchLayout->addWidget(pendingUsersSearchCombo);
    pendingUsersSearchLayout->addStretch();

    pendingUsersTable = new QTableWidget();
    setupTable(pendingUsersTable, {"Фамилия", "Имя", "Отчество", "Будущая роль"});

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(employeesLabel);
    mainLayout->addWidget(employeesSearchPanel);
    mainLayout->addWidget(employeesTable);
    mainLayout->addWidget(adminsLabel);
    mainLayout->addWidget(adminsSearchPanel);
    mainLayout->addWidget(adminsTable);
    mainLayout->addWidget(pendingUsersLabel);
    mainLayout->addWidget(pendingUsersSearchPanel);
    mainLayout->addWidget(pendingUsersTable);

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
    table->setFixedHeight(150);
}

void AccountManagementWindow::loadEmployeesTable()
{
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

void AccountManagementWindow::loadAdminsTable()
{
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

void AccountManagementWindow::loadPendingUsersTable()
{
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

void AccountManagementWindow::filterTable(QTableWidget *table, const CustomList<QStringList> &allData,
                                          const QString& searchText, int searchCriteria)
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
        CustomList<QStringList> filteredData;
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

            QMenu *contextMenu = new QMenu(this);
            QAction *deleteAction = new QAction("Удалить", this);
            connect(deleteAction, &QAction::triggered, this, &AccountManagementWindow::onDeleteUser);
            contextMenu->addAction(deleteAction);

            contextMenu->exec(table->viewport()->mapToGlobal(pos));
            delete contextMenu;
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

void AccountManagementWindow::onDeleteUser()
{
    if (m_selectedRow < 0) {
        QMessageBox::warning(this, "Предупреждение", "Не выбрана строка для удаления");
        return;
    }

    QString userInfo;
    bool success = false;

    if (m_selectedUserType == "employee") {
        if (m_selectedRow >= m_employeesData.size()) {
            QMessageBox::warning(this, "Ошибка", "Неверный номер строки");
            return;
        }

        const QStringList &userData = m_employeesData.at(m_selectedRow);
        userInfo = QString("Сотрудник: %1 %2 %3\nЛогин: %4")
                       .arg(userData.at(0))
                       .arg(userData.at(1))
                       .arg(userData.at(2))
                       .arg(userData.at(3));

    } else if (m_selectedUserType == "admin") {
        if (m_selectedRow >= m_adminsData.size()) {
            QMessageBox::warning(this, "Ошибка", "Неверный номер строки");
            return;
        }

        const QStringList &userData = m_adminsData.at(m_selectedRow);
        userInfo = QString("Администратор: %1 %2 %3\nЛогин: %4")
                       .arg(userData.at(0))
                       .arg(userData.at(1))
                       .arg(userData.at(2))
                       .arg(userData.at(3));

    } else if (m_selectedUserType == "pending") {
        if (m_selectedRow >= m_pendingUsersData.size()) {
            QMessageBox::warning(this, "Ошибка", "Неверный номер строки");
            return;
        }

        const QStringList &userData = m_pendingUsersData.at(m_selectedRow);
        userInfo = QString("Ожидающий пользователь: %1 %2 %3\nРоль: %4")
                       .arg(userData.at(0))
                       .arg(userData.at(1))
                       .arg(userData.at(2))
                       .arg(userData.at(3));
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить этого пользователя?\n\n%1").arg(userInfo),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::No) {
        m_selectedRow = -1;
        m_selectedUserType = "";
        return;
    }

    if (m_selectedUserType == "employee") {
        const QStringList &userData = m_employeesData.at(m_selectedRow);
        QString login = userData.at(3);
        success = m_userManager->removeEmployeeByLogin(login);
    } else if (m_selectedUserType == "admin") {
        const QStringList &userData = m_adminsData.at(m_selectedRow);
        QString login = userData.at(3);
        success = m_userManager->removeAdminByLogin(login);
    } else if (m_selectedUserType == "pending") {
        const QStringList &userData = m_pendingUsersData.at(m_selectedRow);
        QString fullName = userData.at(0) + " " + userData.at(1) + " " + userData.at(2);
        success = m_userManager->removePendingUserByName(fullName);
    }

    if (success) {
        updateTables();
        QMessageBox::information(this, "Успех", "Пользователь успешно удален!");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось удалить пользователя");
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
