#include "adduserwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include "accountmanagementwindow.h"
#include "validationexception.h"  // Добавляем заголовок
#include "inputvalidator.h"       // Добавляем заголовок

AddUserWindow::AddUserWindow(UserManager *userManager, QWidget *parent)
    : QMainWindow(parent), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Добавление пользователя");
    setFixedSize(400, 400);
}

void AddUserWindow::setupUI()
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

    QLabel *titleLabel = new QLabel("ДОБАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯ");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // Форма данных
    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

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

    // Кнопка добавления
    QPushButton *addButton = new QPushButton("Добавить пользователя");
    addButton->setFixedHeight(35);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(addButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &AddUserWindow::onBackClicked);
    connect(addButton, &QPushButton::clicked, this, &AddUserWindow::onAddUserClicked);
}

void AddUserWindow::onAddUserClicked()
{
    try {
        QString lastName = lastNameEdit->text().trimmed();
        QString firstName = firstNameEdit->text().trimmed();
        QString middleName = middleNameEdit->text().trimmed();

        // Проверка всех полей на заполненность - одна ошибка для всех
        QList<QPair<QString, QString>> fields = {
            {lastName, "Фамилия"},
            {firstName, "Имя"},
            {middleName, "Отчество"}
        };

        InputValidator::validateAllFieldsNotEmptyOrThrow(fields);

        // Валидация каждого поля отдельно
        InputValidator::validateNameOrThrow(lastName, "Фамилия");
        InputValidator::validateNameOrThrow(firstName, "Имя");
        InputValidator::validateNameOrThrow(middleName, "Отчество");

        // Определяем роль
        UserRole role;
        QString roleText = roleCombo->currentText();

        if (roleText == "Администратор") {
            role = UserRole::Administrator;
        } else {
            role = UserRole::Employee;
        }

        // Добавляем пользователя в ожидание с указанной ролью
        bool success = m_userManager->registerPendingUser(lastName, firstName, middleName, role);

        if (success) {
            QMessageBox::information(this, "Успех", "Пользователь успешно добавлен в список ожидания!");
            onBackClicked();
        } else {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Пользователь с ФИО '%1 %2 %3' уже существует в списке ожидания")
                                     .arg(lastName).arg(firstName).arg(middleName));
        }

    } catch (const ValidationException& e) {
        // Изменяем заголовок на более понятный
        QMessageBox::warning(this, "Неверный ввод", e.qmessage());
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка", e.qmessage());
    }
}

void AddUserWindow::onBackClicked()
{
    bool hasData = !lastNameEdit->text().isEmpty() ||
                   !firstNameEdit->text().isEmpty() ||
                   !middleNameEdit->text().isEmpty();

    if (hasData) {
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение",
            "Вы уверены, что хотите вернуться? Все введенные данные будут потеряны.",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return;
        }
    }

    AccountManagementWindow *accountWindow = new AccountManagementWindow(m_userManager);
    accountWindow->show();
    this->close();
}
