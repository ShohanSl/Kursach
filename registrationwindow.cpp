#include "registrationwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QRegularExpression>
#include "fileexception.h"        // Добавляем
#include "appexception.h"         // Добавляем
#include "inputvalidator.h"
#include "validationexception.h"

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
    try {
        QString lastName = lastNameEdit->text().trimmed();
        QString firstName = firstNameEdit->text().trimmed();
        QString middleName = middleNameEdit->text().trimmed();
        QString login = loginEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();
        QString confirmPassword = confirmPasswordEdit->text().trimmed();

        // Валидация ВСЕГО ввода на уровне UI
        QList<QPair<QString, QString>> fields = {
            {lastName, "Фамилия"},
            {firstName, "Имя"},
            {middleName, "Отчество"},
            {login, "Логин"},
            {password, "Пароль"},
            {confirmPassword, "Подтверждение пароля"}
        };

        InputValidator::validateAllFieldsNotEmptyOrThrow(fields);
        InputValidator::validateNameOrThrow(lastName, "Фамилия");
        InputValidator::validateNameOrThrow(firstName, "Имя");
        InputValidator::validateNameOrThrow(middleName, "Отчество");

        if (password != confirmPassword) {
            throw ValidationException("Пароли не совпадают");
        }

        InputValidator::validateMinLengthOrThrow(password, 4, "Пароль");
        InputValidator::validateMinLengthOrThrow(login, 3, "Логин");
        InputValidator::validateOrThrow(login, InputValidator::Mode::LatinAlnumHyphensUnderscore);

        // ТОЛЬКО после валидации передаем данные в UserManager
        QString fullName = lastName + " " + firstName + " " + middleName;

        // UserManager проверяет ТОЛЬКО бизнес-логику
        if (m_userManager->completeRegistration(fullName, login, password)) {
            QMessageBox::information(this, "Успех",
                                     "Регистрация завершена успешно!\n\nТеперь вы можете войти в систему\nс вашим логином и паролем.");
            accept();
        } else {
            // Этот код не должен выполняться, если все проверки прошли
            throw ValidationException("Неизвестная ошибка при регистрации");
        }

    } catch (const ValidationException& e) {
        statusLabel->setText(e.qmessage());
    } catch (const FileException& e) {
        statusLabel->setText("Ошибка сохранения данных: " + e.qmessage());
    } catch (const AppException& e) {
        statusLabel->setText("Ошибка: " + e.qmessage());
    }
}

void RegistrationWindow::onBackClicked()
{
    // Проверяем, были ли введены данные
    bool hasData = !lastNameEdit->text().isEmpty() ||
                   !firstNameEdit->text().isEmpty() ||
                   !middleNameEdit->text().isEmpty() ||
                   !loginEdit->text().isEmpty() ||
                   !passwordEdit->text().isEmpty();

    if (hasData) {
        try {
            QMessageBox::StandardButton reply = QMessageBox::question(
                this,
                "Подтверждение",
                "Вы уверены, что хотите вернуться? Все введенные данные будут потеряны.",
                QMessageBox::Yes | QMessageBox::No
                );

            if (reply == QMessageBox::No) {
                return; // Пользователь передумал
            }
        } catch (const std::exception& e) {
            // Если возникла ошибка при показе диалога, просто закрываем окно
            qWarning() << "Ошибка при отображении диалога подтверждения:" << e.what();
        }
    }

    reject(); // Закрываем окно с результатом "отмена"
}
