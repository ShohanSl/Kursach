#include "loginwindow.h"
#include "mainwindow.h"
#include "registrationwindow.h"
#include "usermanager.h"
#include "user.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include "validationexception.h"  // Добавляем
#include "inputvalidator.h"       // Добавляем
#include "appexception.h"         // Добавляем
#include "fileexception.h"        // Добавляем

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), userManager(new UserManager())
{
    setupUI();
    setWindowTitle("Вход в систему");
    setFixedSize(300, 200);
}

LoginWindow::~LoginWindow()
{
    delete userManager;
}

void LoginWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *layout = new QVBoxLayout(central);
    layout->setContentsMargins(20, 20, 20, 20);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText("Логин");

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);

    loginButton = new QPushButton("Войти");
    registerButton = new QPushButton("Регистрация");

    layout->addWidget(loginEdit);
    layout->addWidget(passwordEdit);
    layout->addWidget(loginButton);
    layout->addWidget(registerButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
}

void LoginWindow::onLoginClicked()
{
    try {
        QString login = loginEdit->text().trimmed();
        QString password = passwordEdit->text().trimmed();

        // Проверка заполненности всех полей - одна ошибка для всех
        QList<QPair<QString, QString>> fields = {
            {login, "Логин"},
            {password, "Пароль"}
        };

        InputValidator::validateAllFieldsNotEmptyOrThrow(fields);

        User* user = userManager->authenticateUser(login, password);
        if (user) {
            try {
                MainWindow *mainWindow = new MainWindow(user->getRole() == UserRole::Administrator, userManager);
                mainWindow->show();
                this->close();
            } catch (const AppException& e) {
                QMessageBox::critical(this, "Ошибка создания главного окна",
                                      QString("Не удалось создать главное меню:\n%1").arg(e.qmessage()));
            }
        } else {
            throw ValidationException("Неверный логин или пароль");
        }

    } catch (const ValidationException& e) {
        QMessageBox::warning(this, "Неверный ввод", e.qmessage());
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка", e.qmessage());
    }
}

void LoginWindow::onRegisterClicked()
{
    try {
        RegistrationWindow regWindow(userManager, this);
        regWindow.exec();
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка открытия регистрации",
                              QString("Не удалось открыть окно регистрации:\n%1").arg(e.qmessage()));
    }
}
