#include "adduserwindow.h"
#include "accountmanagementwindow.h"
#include <QFormLayout>

AddUserWindow::AddUserWindow(UserManager *userManager, QWidget *parent)
    : QMainWindow(parent), m_userManager(userManager)
{
    setupUI();
    applyStyle();
    setWindowTitle("Добавление пользователя");
    setFixedSize(500, 400);
}

void AddUserWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ =====
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    QLabel *titleLabel = new QLabel("ДОБАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯ");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // ===== ФОРМА ДАННЫХ =====
    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(12);
    formLayout->setContentsMargins(0, 0, 0, 0);

    lastNameEdit = new QLineEdit();
    lastNameEdit->setPlaceholderText("Введите фамилию");

    firstNameEdit = new QLineEdit();
    firstNameEdit->setPlaceholderText("Введите имя");

    middleNameEdit = new QLineEdit();
    middleNameEdit->setPlaceholderText("Введите отчество");

    roleCombo = new QComboBox();
    roleCombo->addItem("Сотрудник");
    roleCombo->addItem("Администратор");

    formLayout->addRow("Фамилия:*", lastNameEdit);
    formLayout->addRow("Имя:*", firstNameEdit);
    formLayout->addRow("Отчество:*", middleNameEdit);
    formLayout->addRow("Роль:*", roleCombo);

    // ===== КНОПКА ДОБАВЛЕНИЯ =====
    addButton = new QPushButton("Добавить пользователя");
    addButton->setFixedHeight(45);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(addButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &AddUserWindow::onBackClicked);
    connect(addButton, &QPushButton::clicked, this, &AddUserWindow::onAddUserClicked);
}

void AddUserWindow::applyStyle()
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
            font-size: 16px;
            font-weight: bold;
            padding: 5px;
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
        QPushButton#addButton {
            background-color: #27ae60;
            font-size: 16px;
        }
        QPushButton#addButton:hover {
            background-color: #219a52;
        }
        QLineEdit, QComboBox {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 5px;
            padding: 8px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
        }
        QLineEdit:focus, QComboBox:focus {
            border-color: #3498db;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            border: 1px solid #bdc3c7;
            selection-background-color: #3498db;
            color: #000000;
        }
    )");

    backButton->setObjectName("backButton");
    addButton->setObjectName("addButton");
}

void AddUserWindow::onAddUserClicked()
{
    QString lastName = lastNameEdit->text().trimmed();
    QString firstName = firstNameEdit->text().trimmed();
    QString middleName = middleNameEdit->text().trimmed();

    if (lastName.isEmpty() || firstName.isEmpty() || middleName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля ФИО");
        return;
    }

    // Определяем, администратор или сотрудник
    bool isAdmin = (roleCombo->currentText() == "Администратор");

    // Добавляем пользователя в ожидающие
    if (m_userManager->registerPendingUser(lastName, firstName, middleName, isAdmin)) {
        QMessageBox::information(this, "Успех", "Пользователь успешно добавлен в список ожидания!");
        onBackClicked();
    } else {
        QMessageBox::critical(this, "Ошибка", "Не удалось добавить пользователя. Возможно, пользователь с такими ФИО уже существует.");
    }
}

void AddUserWindow::onBackClicked()
{
    // Возвращаемся в управление аккаунтами
    AccountManagementWindow *accountWindow = new AccountManagementWindow(m_userManager);
    accountWindow->show();
    this->close();
}
