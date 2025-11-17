#include "registrationwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QFrame>

RegistrationWindow::RegistrationWindow(UserManager *userManager, QWidget *parent)
    : QDialog(parent), m_userManager(userManager)
{
    setupUI();
    applyStyle();
    setWindowTitle("Регистрация нового пользователя");
    setFixedSize(500, 550);
}

void RegistrationWindow::setupUI()
{
    // Главный вертикальный layout
    QVBoxLayout *mainVerticalLayout = new QVBoxLayout(this);
    mainVerticalLayout->setSpacing(3);
    mainVerticalLayout->setContentsMargins(15, 10, 15, 15);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ С КНОПКОЙ НАЗАД =====
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    topLayout->addWidget(backButton);
    topLayout->addStretch();

    // ===== ОСНОВНОЙ КОНТЕНТ =====
    QWidget *contentWidget = new QWidget();
    QHBoxLayout *mainLayout = new QHBoxLayout(contentWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ===== ЛЕВАЯ КОЛОНКА =====
    leftColumn = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftColumn);
    leftLayout->setSpacing(12);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *titleLabel = new QLabel("РЕГИСТРАЦИЯ ПОЛЬЗОВАТЕЛЯ");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(35);

    QLabel *nameInfoLabel = new QLabel("Введите ваши данные:");
    nameInfoLabel->setAlignment(Qt::AlignCenter);
    nameInfoLabel->setMinimumHeight(25);

    lastNameEdit = new QLineEdit();
    lastNameEdit->setPlaceholderText("Фамилия");
    lastNameEdit->setMinimumHeight(35);

    firstNameEdit = new QLineEdit();
    firstNameEdit->setPlaceholderText("Имя");
    firstNameEdit->setMinimumHeight(35);

    middleNameEdit = new QLineEdit();
    middleNameEdit->setPlaceholderText("Отчество");
    middleNameEdit->setMinimumHeight(35);

    confirmNameButton = new QPushButton("Подтвердить данные");
    confirmNameButton->setMinimumHeight(40);

    statusLabel = new QLabel();
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    statusLabel->setMinimumHeight(80);
    statusLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    leftLayout->addWidget(titleLabel);
    leftLayout->addWidget(nameInfoLabel);
    leftLayout->addWidget(lastNameEdit);
    leftLayout->addWidget(firstNameEdit);
    leftLayout->addWidget(middleNameEdit);
    leftLayout->addWidget(confirmNameButton);
    leftLayout->addWidget(statusLabel);

    // ===== ПРАВАЯ КОЛОНКА =====
    rightColumn = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setSpacing(3);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // Добавляем вертикальную линию-разделитель
    QFrame *verticalLine = new QFrame();
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);
    verticalLine->setStyleSheet("background-color: #cccccc;");

    QLabel *regTitleLabel = new QLabel("СОЗДАНИЕ УЧЕТНОЙ ЗАПИСИ");
    regTitleLabel->setAlignment(Qt::AlignCenter);
    regTitleLabel->setMinimumHeight(35);

    QLabel *regInfoLabel = new QLabel("Придумайте логин и пароль:");
    regInfoLabel->setAlignment(Qt::AlignCenter);
    regInfoLabel->setMinimumHeight(25);

    loginEdit = new QLineEdit();
    loginEdit->setPlaceholderText("Логин");
    loginEdit->setMinimumHeight(35);

    passwordEdit = new QLineEdit();
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);

    confirmPasswordEdit = new QLineEdit();
    confirmPasswordEdit->setPlaceholderText("Подтверждение пароля");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setMinimumHeight(35);

    completeRegistrationButton = new QPushButton("Зарегистрироваться");
    completeRegistrationButton->setMinimumHeight(40);

    rightLayout->addWidget(regTitleLabel);
    rightLayout->addWidget(regInfoLabel);
    rightLayout->addWidget(loginEdit);
    rightLayout->addWidget(passwordEdit);
    rightLayout->addWidget(confirmPasswordEdit);
    rightLayout->addWidget(completeRegistrationButton);

    // Изначально скрываем правую колонку
    rightColumn->setVisible(false);
    verticalLine->setVisible(false);

    // Добавляем в основной layout
    mainLayout->addWidget(leftColumn);
    mainLayout->addWidget(verticalLine);
    mainLayout->addWidget(rightColumn);

    // Собираем все вместе
    mainVerticalLayout->addWidget(topPanel);
    mainVerticalLayout->addWidget(contentWidget);

    connect(confirmNameButton, &QPushButton::clicked, this, &RegistrationWindow::onConfirmNameClicked);
    connect(completeRegistrationButton, &QPushButton::clicked, this, &RegistrationWindow::onCompleteRegistrationClicked);
    connect(backButton, &QPushButton::clicked, this, &RegistrationWindow::onBackClicked);
}

void RegistrationWindow::applyStyle()
{
    // Стили остаются без изменений
    setStyleSheet(R"(
        QDialog {
            background-color: #f0f0f0;
        }
        QLabel {
            color: #333333;
            font-size: 14px;
            padding: 3px;
            margin: 1px;
        }
        QLineEdit {
            background-color: #ffffff;
            border: 2px solid #cccccc;
            border-radius: 15px;
            padding: 10px;
            font-size: 14px;
            color: #333333;
            margin: 3px;
        }
        QLineEdit:focus {
            border-color: #3498db;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 15px;
            padding: 10px;
            font-size: 14px;
            font-weight: bold;
            margin: 3px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #2471a3;
        }
        QPushButton#backButton {
            background-color: #95a5a6;
            font-size: 13px;
            padding: 8px 15px;
        }
        QPushButton#backButton:hover {
            background-color: #7f8c8d;
        }
    )");

    backButton->setObjectName("backButton");

    statusLabel->setStyleSheet(R"(
        QLabel {
            color: #d35400;
            font-weight: bold;
            padding: 15px;
            margin: 5px;
            background-color: #f8f9fa;
            border: 2px solid #e9ecef;
            border-radius: 12px;
            font-size: 14px;
            line-height: 1.4;
            min-height: 80px;
        }
    )");
}

void RegistrationWindow::onBackClicked()
{
    if (!lastNameEdit->text().isEmpty() || !firstNameEdit->text().isEmpty() || !middleNameEdit->text().isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
                                                                  "Вы уверены, что хотите вернуться? Все введенные данные будут потеряны.",
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    reject();
}

void RegistrationWindow::onConfirmNameClicked()
{
    QString lastName = lastNameEdit->text().trimmed();
    QString firstName = firstNameEdit->text().trimmed();
    QString middleName = middleNameEdit->text().trimmed();

    if (lastName.isEmpty() || firstName.isEmpty() || middleName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля ФИО");
        return;
    }

    QString fullName = lastName + " " + firstName + " " + middleName;

    // Проверяем, есть ли такой пользователь в ожидании
    if (m_userManager->isUserInPending(fullName)) {
        // Получаем базового пользователя для отображения информации
        User* pendingUser = m_userManager->getPendingUser(fullName);
        if (pendingUser) {
            statusLabel->setText(QString("✓ Данные подтверждены!\n"
                                         "Пользователь найден в списке ожидания\n"
                                         "Теперь создайте учетную запись"));
            showRegistrationFields();
        } else {
            statusLabel->setText("❌ Ошибка при получении данных пользователя");
        }
    } else {
        statusLabel->setText("❌ Пользователь с такими ФИО\nне найден в списке ожидания");
    }
}

void RegistrationWindow::onCompleteRegistrationClicked()
{
    QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    if (login.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля");
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "Ошибка", "Пароли не совпадают");
        return;
    }

    if (password.length() < 4) {
        QMessageBox::warning(this, "Ошибка", "Пароль должен содержать минимум 4 символа");
        return;
    }

    QString fullName = lastNameEdit->text().trimmed() + " " +
                       firstNameEdit->text().trimmed() + " " +
                       middleNameEdit->text().trimmed();

    if (m_userManager->completeRegistration(fullName, login, password)) {
        QString message = QString("Регистрация завершена успешно!\n\n"
                                  "Теперь вы можете войти в систему\n"
                                  "с вашим логином и паролем.");
        QMessageBox::information(this, "Успех", message);
        accept();
    } else {
        QMessageBox::critical(this, "Ошибка",
                              "Не удалось завершить регистрацию.\n"
                              "Возможная причина:\n"
                              "- Пользователь с таким логином уже существует");
    }
}

void RegistrationWindow::showRegistrationFields()
{
    // Блокируем левую колонку
    lastNameEdit->setEnabled(false);
    firstNameEdit->setEnabled(false);
    middleNameEdit->setEnabled(false);
    confirmNameButton->setEnabled(false);

    // Показываем правую колонку и разделитель
    rightColumn->setVisible(true);

    // Показываем разделитель между колонками
    QList<QFrame*> frames = findChildren<QFrame*>();
    for (QFrame* frame : frames) {
        if (frame->frameShape() == QFrame::VLine) {
            frame->setVisible(true);
            break;
        }
    }

    // Увеличиваем размер окна для размещения двух колонок
    setFixedSize(800, 450);

    // Устанавливаем фокус на поле логина
    loginEdit->setFocus();
}
