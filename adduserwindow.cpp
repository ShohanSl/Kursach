#include "adduserwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include "accountmanagementwindow.h"

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
    QString lastName = lastNameEdit->text().trimmed();
    QString firstName = firstNameEdit->text().trimmed();
    QString middleName = middleNameEdit->text().trimmed();

    if (lastName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Фамилия' обязательно для заполнения");
        return;
    }

    if (firstName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Имя' обязательно для заполнения");
        return;
    }

    if (middleName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Поле 'Отчество' обязательно для заполнения");
        return;
    }

    if (lastName.length() < 2) {
        QMessageBox::warning(this, "Ошибка", "Фамилия должна содержать минимум 2 символа");
        return;
    }

    if (firstName.length() < 2) {
        QMessageBox::warning(this, "Ошибка", "Имя должно содержать минимум 2 символа");
        return;
    }

    QRegularExpression nameRegex("^[А-Яа-яЁёA-Za-z\\s\\-]+$");

    if (!nameRegex.match(lastName).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Фамилия может содержать только буквы, дефисы и пробелы");
        return;
    }

    if (!nameRegex.match(firstName).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Имя может содержать только буквы, дефисы и пробелы");
        return;
    }

    if (!middleName.isEmpty() && !nameRegex.match(middleName).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Отчество может содержать только буквы, дефисы и пробелы");
        return;
    }

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
