#include "loginwindow.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QHBoxLayout>

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), userManager(new UserManager())
{
    setupUI();
    applyStyle();
    setWindowTitle("Складской учет - Вход");
    setFixedSize(500, 500);
}

void LoginWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(25);
    mainLayout->setContentsMargins(40, 30, 40, 30);

    QLabel *titleLabel = new QLabel("СИСТЕМА СКЛАДСКОГО УЧЕТА");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(50);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText("Логин");
    loginEdit->setMinimumHeight(45);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(45);

    loginButton = new QPushButton("Войти");
    loginButton->setMinimumHeight(50);

    registerButton = new QPushButton("Регистрация");
    registerButton->setMinimumHeight(50);

    initTestDataButton = new QPushButton("Инициализировать тестовые данные");
    initTestDataButton->setMinimumHeight(50);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(loginEdit);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(registerButton);
    mainLayout->addWidget(initTestDataButton);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &LoginWindow::onRegisterClicked);
    connect(initTestDataButton, &QPushButton::clicked, this, [this]() {
        userManager->initializeTestData();
        QMessageBox::information(this, "Тестовые данные", "Тестовые данные успешно инициализированы!\n\n"
                                                          "Админ: admin / admin123\n"
                                                          "Работник: worker / work123\n"
                                                          "Незарегистрированный: Сидоров Новый Пользователь");
    });
}

void LoginWindow::applyStyle()
{
    // Стили остаются без изменений
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f0f0;
        }
        QWidget {
            background-color: #f0f0f0;
        }
        QLabel {
            color: #2c3e50;
            font-size: 20px;
            font-weight: bold;
            padding: 15px;
            margin: 0px;
        }
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #cccccc;
            border-radius: 15px;
            padding: 15px;
            font-size: 14px;
            color: #333333;
            margin: 5px;
            min-height: 20px;
        }
        QLineEdit:focus {
            border-color: #4CAF50;
        }
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 15px;
            padding: 15px;
            font-size: 14px;
            font-weight: bold;
            margin: 5px;
            min-height: 20px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
        QPushButton:pressed {
            background-color: #3d8b40;
        }
        QPushButton#initTestDataButton {
            background-color: #e67e22;
        }
        QPushButton#initTestDataButton:hover {
            background-color: #d35400;
        }
    )");

    initTestDataButton->setObjectName("initTestDataButton");
}

void LoginWindow::onLoginClicked()
{
    QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    Employee* authenticatedUser = userManager->authenticateUser(login, password);
    if (authenticatedUser) {
        QString roleStr = authenticatedUser->getRoleString();
        QMessageBox::information(this, "Успех",
                                 QString("Вход выполнен успешно!\nРоль: %1").arg(roleStr));

        // Определяем, является ли пользователь администратором
        bool isAdmin = (dynamic_cast<Administrator*>(authenticatedUser) != nullptr);

        // Создаем и показываем главное окно
        MainWindow *mainWindow = new MainWindow(isAdmin, userManager);
        mainWindow->show();
        this->close();

    } else {
        QMessageBox::critical(this, "Ошибка", "Неверный логин или пароль");
    }
}

void LoginWindow::onRegisterClicked()
{
    RegistrationWindow regWindow(userManager, this);
    if (regWindow.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Успех", "Регистрация завершена! Теперь вы можете войти в систему.");
    }
}
