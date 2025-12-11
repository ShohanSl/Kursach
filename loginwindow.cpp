#include "loginwindow.h"
#include "mainwindow.h"
#include "registrationwindow.h"
#include "usermanager.h"
#include "user.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>

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
    QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    User* user = userManager->authenticateUser(login, password);
    if (user) {
        MainWindow *mainWindow = new MainWindow(user->getRole() == UserRole::Administrator, userManager);
        mainWindow->show();
        this->close();
    } else {
        QMessageBox::critical(this, "Ошибка", "Неверный логин или пароль");
    }
}

void LoginWindow::onRegisterClicked()
{
    RegistrationWindow regWindow(userManager, this);
    regWindow.exec();
}
