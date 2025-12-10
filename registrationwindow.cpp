#include "registrationwindow.h"
#include <QMessageBox>
#include <QApplication>
#include <QFrame>
#include "exceptionhandler.h" // Добавляем заголовок
#include <QRegularExpression>

RegistrationWindow::RegistrationWindow(UserManager *userManager, QWidget *parent)
    : QDialog(parent), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Регистрация нового пользователя");
    setFixedSize(500, 550);
    TRY_CATCH_END
}

void RegistrationWindow::setupUI()
{
    TRY_CATCH_BEGIN
        // Главный вертикальный layout
        QVBoxLayout *mainVerticalLayout = new QVBoxLayout(this);
    if (!mainVerticalLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный вертикальный макет");
    }
    mainVerticalLayout->setSpacing(3);
    mainVerticalLayout->setContentsMargins(15, 10, 15, 15);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ С КНОПКОЙ НАЗАД =====
    QWidget *topPanel = new QWidget();
    if (!topPanel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания верхней панели",
                        "Не удалось создать виджет верхней панели");
    }
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    if (!topLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета верхней панели",
                        "Не удалось создать горизонтальный макет для верхней панели");
    }
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    if (!backButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки 'Назад'",
                        "Не удалось создать кнопку возврата");
    }
    backButton->setFixedSize(100, 35);

    topLayout->addWidget(backButton);
    topLayout->addStretch();

    // ===== ОСНОВНОЙ КОНТЕНТ =====
    QWidget *contentWidget = new QWidget();
    if (!contentWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания виджета контента",
                        "Не удалось создать виджет для основного содержимого");
    }
    QHBoxLayout *mainLayout = new QHBoxLayout(contentWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания основного макета",
                        "Не удалось создать горизонтальный макет для содержимого");
    }
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // ===== ЛЕВАЯ КОЛОНКА =====
    leftColumn = new QWidget();
    if (!leftColumn) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания левой колонки",
                        "Не удалось создать виджет левой колонки");
    }
    QVBoxLayout *leftLayout = new QVBoxLayout(leftColumn);
    if (!leftLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета левой колонки",
                        "Не удалось создать вертикальный макет для левой колонки");
    }
    leftLayout->setSpacing(12);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *titleLabel = new QLabel("РЕГИСТРАЦИЯ ПОЛЬЗОВАТЕЛЯ");
    if (!titleLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания заголовка регистрации",
                        "Не удалось создать метку заголовка регистрации");
    }
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(35);

    QLabel *nameInfoLabel = new QLabel("Введите ваши данные:");
    if (!nameInfoLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания информационной метки",
                        "Не удалось создать метку с инструкцией");
    }
    nameInfoLabel->setAlignment(Qt::AlignCenter);
    nameInfoLabel->setMinimumHeight(25);

    lastNameEdit = new QLineEdit();
    if (!lastNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля фамилии",
                        "Не удалось создать поле ввода фамилии");
    }
    lastNameEdit->setPlaceholderText("Фамилия");
    lastNameEdit->setMinimumHeight(35);

    firstNameEdit = new QLineEdit();
    if (!firstNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля имени",
                        "Не удалось создать поле ввода имени");
    }
    firstNameEdit->setPlaceholderText("Имя");
    firstNameEdit->setMinimumHeight(35);

    middleNameEdit = new QLineEdit();
    if (!middleNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля отчества",
                        "Не удалось создать поле ввода отчества");
    }
    middleNameEdit->setPlaceholderText("Отчество");
    middleNameEdit->setMinimumHeight(35);

    confirmNameButton = new QPushButton("Подтвердить данные");
    if (!confirmNameButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки подтверждения",
                        "Не удалось создать кнопку подтверждения данных");
    }
    confirmNameButton->setMinimumHeight(40);

    statusLabel = new QLabel();
    if (!statusLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания метки статуса",
                        "Не удалось создать метку для отображения статуса");
    }
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
    if (!rightColumn) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания правой колонки",
                        "Не удалось создать виджет правой колонки");
    }
    QVBoxLayout *rightLayout = new QVBoxLayout(rightColumn);
    if (!rightLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета правой колонки",
                        "Не удалось создать вертикальный макет для правой колонки");
    }
    rightLayout->setSpacing(3);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    // Добавляем вертикальную линию-разделитель
    QFrame *verticalLine = new QFrame();
    if (!verticalLine) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания разделителя",
                        "Не удалось создать вертикальную линию-разделитель");
    }
    verticalLine->setFrameShape(QFrame::VLine);
    verticalLine->setFrameShadow(QFrame::Sunken);
    verticalLine->setStyleSheet("background-color: #cccccc;");

    QLabel *regTitleLabel = new QLabel("СОЗДАНИЕ УЧЕТНОЙ ЗАПИСИ");
    if (!regTitleLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания заголовка создания учетной записи",
                        "Не удалось создать метку заголовка создания учетной записи");
    }
    regTitleLabel->setAlignment(Qt::AlignCenter);
    regTitleLabel->setMinimumHeight(35);

    QLabel *regInfoLabel = new QLabel("Придумайте логин и пароль:");
    if (!regInfoLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания информационной метки регистрации",
                        "Не удалось создать метку с инструкцией регистрации");
    }
    regInfoLabel->setAlignment(Qt::AlignCenter);
    regInfoLabel->setMinimumHeight(25);

    loginEdit = new QLineEdit();
    if (!loginEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля логина",
                        "Не удалось создать поле ввода логина");
    }
    loginEdit->setPlaceholderText("Логин");
    loginEdit->setMinimumHeight(35);

    passwordEdit = new QLineEdit();
    if (!passwordEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля пароля",
                        "Не удалось создать поле ввода пароля");
    }
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    passwordEdit->setMinimumHeight(35);

    confirmPasswordEdit = new QLineEdit();
    if (!confirmPasswordEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля подтверждения пароля",
                        "Не удалось создать поле ввода подтверждения пароля");
    }
    confirmPasswordEdit->setPlaceholderText("Подтверждение пароля");
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setMinimumHeight(35);

    completeRegistrationButton = new QPushButton("Зарегистрироваться");
    if (!completeRegistrationButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки регистрации",
                        "Не удалось создать кнопку завершения регистрации");
    }
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
    TRY_CATCH_END
}

void RegistrationWindow::applyStyle()
{
    TRY_CATCH_BEGIN
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
    TRY_CATCH_END
}

void RegistrationWindow::onBackClicked()
{
    TRY_CATCH_BEGIN
        if (!lastNameEdit->text().isEmpty() || !firstNameEdit->text().isEmpty() || !middleNameEdit->text().isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Подтверждение",
                                                                  "Вы уверены, что хотите вернуться? Все введенные данные будут потеряны.",
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    reject();
    TRY_CATCH_END
}

void RegistrationWindow::onConfirmNameClicked()
{
    TRY_CATCH_BEGIN
        QString lastName = lastNameEdit->text().trimmed();
    QString firstName = firstNameEdit->text().trimmed();
    QString middleName = middleNameEdit->text().trimmed();

    // Валидация полей ФИО
    if (lastName.isEmpty() || firstName.isEmpty() || middleName.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните все поля ФИО");
        return;
    }

    // Проверка минимальной длины
    if (lastName.length() < 2) {
        QMessageBox::warning(this, "Ошибка", "Фамилия должна содержать минимум 2 символа");
        return;
    }

    if (firstName.length() < 2) {
        QMessageBox::warning(this, "Ошибка", "Имя должно содержать минимум 2 символа");
        return;
    }

    // Проверка на допустимые символы (только буквы, дефисы, пробелы)
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
    TRY_CATCH_END
}

void RegistrationWindow::onCompleteRegistrationClicked()
{
    TRY_CATCH_BEGIN
        QString login = loginEdit->text().trimmed();
    QString password = passwordEdit->text().trimmed();
    QString confirmPassword = confirmPasswordEdit->text().trimmed();

    // Валидация полей (оставляем QMessageBox для пользовательских ошибок)
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

    if (login.length() < 3) {
        QMessageBox::warning(this, "Ошибка", "Логин должен содержать минимум 3 символа");
        return;
    }

    // Проверка на допустимые символы в логине
    QRegularExpression loginRegex("^[A-Za-z0-9_\\-]+$");
    if (!loginRegex.match(login).hasMatch()) {
        QMessageBox::warning(this, "Ошибка", "Логин может содержать только латинские буквы, цифры, дефисы и подчеркивания");
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
                              "- Пользователь с таким логином уже существует\n"
                              "- Ошибка при сохранении данных");
    }
    TRY_CATCH_END
}

void RegistrationWindow::showRegistrationFields()
{
    TRY_CATCH_BEGIN
        // Блокируем левую колонку
        lastNameEdit->setEnabled(false);
    firstNameEdit->setEnabled(false);
    middleNameEdit->setEnabled(false);
    confirmNameButton->setEnabled(false);

    // Показываем правую колонку и разделитель
    rightColumn->setVisible(true);

    // Показываем разделитель между колонками
    CustomList<QFrame*> frames;
    frames.fromQList(findChildren<QFrame*>());
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
    TRY_CATCH_END
}
