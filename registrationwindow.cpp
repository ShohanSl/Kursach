#include "registrationwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>

RegistrationWindow::RegistrationWindow(UserManager *userManager, QWidget *parent)
    : QDialog(parent), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Регистрация");
    setFixedSize(380, 450);
}

void RegistrationWindow::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Верхняя панель с кнопкой "Назад"
    QHBoxLayout *topLayout = new QHBoxLayout();
    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 30);
    topLayout->addWidget(backButton);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    // Заголовок
    QLabel *titleLabel = new QLabel("РЕГИСТРАЦИЯ");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Поля формы
    lastNameEdit = new QLineEdit();
    lastNameEdit->setPlaceholderText("Фамилия*");

    firstNameEdit = new QLineEdit();
    firstNameEdit->setPlaceholderText("Имя*");

    middleNameEdit = new QLineEdit();
    middleNameEdit->setPlaceholderText("Отчество*");

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText("Логин*");

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Пароль*");
    passwordEdit->setEchoMode(QLineEdit::Password);

    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("Подтвердите пароль*");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    // Добавляем все поля
    mainLayout->addWidget(lastNameEdit);
    mainLayout->addWidget(firstNameEdit);
    mainLayout->addWidget(middleNameEdit);
    mainLayout->addWidget(loginEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(confirmPasswordEdit);

    // Кнопка регистрации
    completeButton = new QPushButton("Зарегистрироваться");
    mainLayout->addWidget(completeButton);

    // Статус
    statusLabel = new QLabel("");
    statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(statusLabel);

    // Подключаем сигналы
    connect(completeButton, &QPushButton::clicked,
            this, &RegistrationWindow::onCompleteRegistrationClicked);
    connect(backButton, &QPushButton::clicked,
            this, &RegistrationWindow::onBackClicked);
}

void RegistrationWindow::onCompleteRegistrationClicked()
{
    QString lastName = lastNameEdit->text().trimmed();
    QString firstName = firstNameEdit->text().trimmed();
    QString middleName = middleNameEdit->text().trimmed();
    QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    // ВАЛИДАЦИЯ ФИО (как в старой версии)
    if (lastName.isEmpty() || firstName.isEmpty() || middleName.isEmpty()) {
        statusLabel->setText("Заполните все поля ФИО");
        return;
    }

    // Проверка минимальной длины (как в старой версии)
    if (lastName.length() < 2) {
        statusLabel->setText("Фамилия должна содержать минимум 2 символа");
        return;
    }

    if (firstName.length() < 2) {
        statusLabel->setText("Имя должно содержать минимум 2 символа");
        return;
    }

    // Проверка на допустимые символы (как в старой версии)
    QRegularExpression nameRegex("^[А-Яа-яЁёA-Za-z\\s\\-]+$");
    if (!nameRegex.match(lastName).hasMatch()) {
        statusLabel->setText("Фамилия может содержать только буквы, дефисы и пробелы");
        return;
    }

    if (!nameRegex.match(firstName).hasMatch()) {
        statusLabel->setText("Имя может содержать только буквы, дефисы и пробелы");
        return;
    }

    if (!middleName.isEmpty() && !nameRegex.match(middleName).hasMatch()) {
        statusLabel->setText("Отчество может содержать только буквы, дефисы и пробелы");
        return;
    }

    // ВАЛИДАЦИЯ ЛОГИНА И ПАРОЛЯ (как в старой версии)
    if (login.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        statusLabel->setText("Заполните все поля");
        return;
    }

    if (password != confirmPassword) {
        statusLabel->setText("Пароли не совпадают");
        return;
    }

    if (password.length() < 4) {
        statusLabel->setText("Пароль должен содержать минимум 4 символа");
        return;
    }

    if (login.length() < 3) {
        statusLabel->setText("Логин должен содержать минимум 3 символа");
        return;
    }

    // Проверка на допустимые символы в логине (как в старой версии)
    QRegularExpression loginRegex("^[A-Za-z0-9_\\-]+$");
    if (!loginRegex.match(login).hasMatch()) {
        statusLabel->setText("Логин может содержать только латинские буквы, цифры, дефисы и подчеркивания");
        return;
    }

    // Проверка существования пользователя в ожидании (как в старой версии)
    QString fullName = lastName + " " + firstName + " " + middleName;
    if (!m_userManager->isUserInPending(fullName)) {
        statusLabel->setText("Пользователь с такими ФИО не найден в списке ожидания");
        return;
    }

    // Проверка, что пользователь уже не зарегистрирован (через completeRegistration)
    // и регистрация
    if (m_userManager->completeRegistration(fullName, login, password)) {
        QMessageBox::information(this, "Успех",
                                 "Регистрация завершена успешно!\n\nТеперь вы можете войти в систему\nс вашим логином и паролем.");
        accept(); // Закрываем окно с успешным результатом
    } else {
        statusLabel->setText("Не удалось завершить регистрацию.\nВозможная причина:\n- Пользователь с таким логином уже существует\n- Ошибка при сохранении данных");
    }
}

void RegistrationWindow::onBackClicked()
{
    // Проверяем, были ли введены данные (как в старой версии)
    bool hasData = !lastNameEdit->text().isEmpty() ||
                   !firstNameEdit->text().isEmpty() ||
                   !middleNameEdit->text().isEmpty() ||
                   !loginEdit->text().isEmpty() ||
                   !passwordEdit->text().isEmpty();

    if (hasData) {
        // Показываем предупреждение о потере данных (как в старой версии)
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "Подтверждение",
            "Вы уверены, что хотите вернуться? Все введенные данные будут потеряны.",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) {
            return; // Пользователь передумал
        }
    }

    reject(); // Закрываем окно с результатом "отмена"
}
