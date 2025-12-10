#include "loginwindow.h"
#include "mainwindow.h"
#include "user.h"
#include <QMessageBox>
#include <QApplication>
#include <QHBoxLayout>
#include "exceptionhandler.h" // Добавляем заголовок

LoginWindow::LoginWindow(QWidget *parent)
    : QMainWindow(parent), userManager(new UserManager())
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Складской учет - Вход");
    setFixedSize(500, 500);
    TRY_CATCH_END
}

void LoginWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет окна входа");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет окна входа");
    }
    mainLayout->setSpacing(25);
    mainLayout->setContentsMargins(40, 30, 40, 30);

    QLabel *titleLabel = new QLabel("СИСТЕМА СКЛАДСКОГО УЧЕТА");
    if (!titleLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания заголовка",
                        "Не удалось создать метку заголовка");
    }
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(50);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    loginEdit = new QLineEdit();
    if (!loginEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля логина",
                        "Не удалось создать поле ввода логина");
    }
    loginEdit->setPlaceholderText("Логин");
    loginEdit->setMinimumHeight(45);

    passwordEdit = new QLineEdit();
    if (!passwordEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля пароля",
                        "Не удалось создать поле ввода пароля");
    }
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(45);

    loginButton = new QPushButton("Войти");
    if (!loginButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки входа",
                        "Не удалось создать кнопку входа");
    }
    loginButton->setMinimumHeight(50);

    registerButton = new QPushButton("Регистрация");
    if (!registerButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки регистрации",
                        "Не удалось создать кнопку регистрации");
    }
    registerButton->setMinimumHeight(50);

    initTestDataButton = new QPushButton("Инициализировать тестовые данные");
    if (!initTestDataButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки тестовых данных",
                        "Не удалось создать кнопку инициализации тестовых данных");
    }
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
        TRY_CATCH_BEGIN
            userManager->initializeTestData();
        QMessageBox::information(this, "Тестовые данные", "Тестовые данные успешно инициализированы!\n\n"
                                                          "Админ: admin / admin123\n"
                                                          "Работник: worker / work123\n"
                                                          "Незарегистрированный: Сидоров Новый Пользователь");
        TRY_CATCH_END
    });
    TRY_CATCH_END
}

void LoginWindow::applyStyle()
{
    TRY_CATCH_BEGIN
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
    TRY_CATCH_END
}

void LoginWindow::onLoginClicked()
{
    TRY_CATCH_BEGIN
        QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();

    // Валидация ввода (оставляем QMessageBox для пользовательских ошибок)
    if (login.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    // Проверка минимальной длины (можно добавить, если требуется)
    if (login.length() < 3) {
        QMessageBox::warning(this, "Ошибка", "Логин должен содержать минимум 3 символа");
        return;
    }

    if (password.length() < 4) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать минимум 4 символа");
        return;
    }

    User* authenticatedUser = userManager->authenticateUser(login, password);
    if (authenticatedUser) {
        QString roleStr = authenticatedUser->getRoleString();
        QMessageBox::information(this, "Успех",
                                 QString("Вход выполнен успешно!\nРоль: %1").arg(roleStr));

        // Определяем, является ли пользователь администратором
        bool isAdmin = (authenticatedUser->getRole() == UserRole::Administrator);

        // Создаем и показываем главное окно
        MainWindow *mainWindow = new MainWindow(isAdmin, userManager);
        if (!mainWindow) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания главного окна",
                            "Не удалось создать экземпляр MainWindow");
        }
        mainWindow->show();
        this->close();

    } else {
        // Оставляем QMessageBox для ошибки аутентификации (пользовательская ошибка)
        QMessageBox::critical(this, "Ошибка", "Неверный логин или пароль");
    }
    TRY_CATCH_END
}

void LoginWindow::onRegisterClicked()
{
    TRY_CATCH_BEGIN
        RegistrationWindow regWindow(userManager, this);
    if (regWindow.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Успех", "Регистрация завершена! Теперь вы можете войти в систему.");
    }
    TRY_CATCH_END
}
