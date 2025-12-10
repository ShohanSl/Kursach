#include "adduserwindow.h"
#include "accountmanagementwindow.h"
#include "exceptionhandler.h"
#include <QFormLayout>

AddUserWindow::AddUserWindow(UserManager *userManager, QWidget *parent)
    : QMainWindow(parent), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Добавление пользователя");
    setFixedSize(500, 400);
    TRY_CATCH_END
}

void AddUserWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет окна добавления пользователя");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет для окна добавления пользователя");
    }
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ =====
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
                        "Не удалось создать макет для верхней панели");
    }
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    if (!backButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки 'Назад'",
                        "Не удалось создать кнопку возврата");
    }
    backButton->setFixedSize(100, 35);

    QLabel *titleLabel = new QLabel("ДОБАВЛЕНИЕ ПОЛЬЗОВАТЕЛЯ");
    if (!titleLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания заголовка",
                        "Не удалось создать метку заголовка");
    }
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // ===== ФОРМА ДАННЫХ =====
    QWidget *formWidget = new QWidget();
    if (!formWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания виджета формы",
                        "Не удалось создать виджет для формы данных");
    }
    QFormLayout *formLayout = new QFormLayout(formWidget);
    if (!formLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета формы",
                        "Не удалось создать макет формы данных");
    }
    formLayout->setSpacing(12);
    formLayout->setContentsMargins(0, 0, 0, 0);

    lastNameEdit = new QLineEdit();
    if (!lastNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля ввода фамилии",
                        "Не удалось создать поле ввода для фамилии");
    }
    lastNameEdit->setPlaceholderText("Введите фамилию");

    firstNameEdit = new QLineEdit();
    if (!firstNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля ввода имени",
                        "Не удалось создать поле ввода для имени");
    }
    firstNameEdit->setPlaceholderText("Введите имя");

    middleNameEdit = new QLineEdit();
    if (!middleNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля ввода отчества",
                        "Не удалось создать поле ввода для отчества");
    }
    middleNameEdit->setPlaceholderText("Введите отчество");

    roleCombo = new QComboBox();
    if (!roleCombo) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания комбобокса роли",
                        "Не удалось создать комбобокс для выбора роли");
    }
    roleCombo->addItem("Сотрудник");
    roleCombo->addItem("Администратор");

    formLayout->addRow("Фамилия:*", lastNameEdit);
    formLayout->addRow("Имя:*", firstNameEdit);
    formLayout->addRow("Отчество:*", middleNameEdit);
    formLayout->addRow("Роль:*", roleCombo);

    // ===== КНОПКА ДОБАВЛЕНИЯ =====
    addButton = new QPushButton("Добавить пользователя");
    if (!addButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки добавления",
                        "Не удалось создать кнопку добавления пользователя");
    }
    addButton->setFixedHeight(45);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(addButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &AddUserWindow::onBackClicked);
    connect(addButton, &QPushButton::clicked, this, &AddUserWindow::onAddUserClicked);
    TRY_CATCH_END
}

void AddUserWindow::applyStyle()
{
    TRY_CATCH_BEGIN
        setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f0f0;
        }
        QWidget {
            background-color: #f0f0f0;
        }
        QLabel {
            color: #2c3e50;
            font-size: 16px;
            font-weight: bold;
            padding: 5px;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 14px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #2471a3;
        }
        QPushButton#backButton {
            background-color: #95a5a6;
        }
        QPushButton#backButton:hover {
            background-color: #7f8c8d;
        }
        QPushButton#addButton {
            background-color: #27ae60;
            font-size: 16px;
        }
        QPushButton#addButton:hover {
            background-color: #219a52;
        }
        QLineEdit, QComboBox {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 5px;
            padding: 8px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
        }
        QLineEdit:focus, QComboBox:focus {
            border-color: #3498db;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            border: 1px solid #bdc3c7;
            selection-background-color: #3498db;
            color: #000000;
        }
    )");

    backButton->setObjectName("backButton");
    addButton->setObjectName("addButton");
    TRY_CATCH_END
}

void AddUserWindow::onAddUserClicked()
{
    TRY_CATCH_BEGIN
        QString lastName = lastNameEdit->text().trimmed();
    QString firstName = firstNameEdit->text().trimmed();
    QString middleName = middleNameEdit->text().trimmed();

    // Валидация полей ФИО с визуальным выделением ошибок
    bool hasError = false;

    // Сбрасываем стили полей
    lastNameEdit->setStyleSheet("");
    firstNameEdit->setStyleSheet("");
    middleNameEdit->setStyleSheet("");

    if (lastName.isEmpty()) {
        hasError = true;
        // Визуально выделяем поле с ошибкой
        lastNameEdit->setStyleSheet("border: 2px solid #e74c3c; background-color: #ffe6e6;");
        THROW_VALIDATION_ERROR("фамилия", "не может быть пустой",
                               "Поле 'Фамилия' обязательно для заполнения");
    }

    if (firstName.isEmpty()) {
        hasError = true;
        firstNameEdit->setStyleSheet("border: 2px solid #e74c3c; background-color: #ffe6e6;");
        THROW_VALIDATION_ERROR("имя", "не может быть пустым",
                               "Поле 'Имя' обязательно для заполнения");
    }

    if (middleName.isEmpty()) {
        hasError = true;
        middleNameEdit->setStyleSheet("border: 2px solid #e74c3c; background-color: #ffe6e6;");
        THROW_VALIDATION_ERROR("отчество", "не может быть пустым",
                               "Поле 'Отчество' обязательно для заполнения");
    }

    // Если есть ошибки - показываем сообщение и выходим
    if (hasError) {
        ExceptionHandler::showMessageBox(ErrorSeverity::WARNING, "Внимание",
                                         "Заполните все обязательные поля ФИО", this);
        return;
    }

    // Проверка на минимальную длину
    if (lastName.length() < 2) {
        lastNameEdit->setStyleSheet("border: 2px solid #e67e22; background-color: #fff3e6;");
        THROW_VALIDATION_ERROR("фамилия", "слишком короткая",
                               "Фамилия должна содержать минимум 2 символа");
    }

    if (firstName.length() < 2) {
        firstNameEdit->setStyleSheet("border: 2px solid #e67e22; background-color: #fff3e6;");
        THROW_VALIDATION_ERROR("имя", "слишком короткое",
                               "Имя должно содержать минимум 2 символа");
    }

    // Используем QRegularExpression для Qt6
    QRegularExpression nameRegex("^[А-Яа-яЁёA-Za-z\\s\\-]+$");

    if (!nameRegex.match(lastName).hasMatch()) {
        lastNameEdit->setStyleSheet("border: 2px solid #e74c3c; background-color: #ffe6e6;");
        THROW_VALIDATION_ERROR("фамилия", "содержит недопустимые символы",
                               "Фамилия может содержать только буквы, дефисы и пробелы");
    }

    if (!nameRegex.match(firstName).hasMatch()) {
        firstNameEdit->setStyleSheet("border: 2px solid #e74c3c; background-color: #ffe6e6;");
        THROW_VALIDATION_ERROR("имя", "содержит недопустимые символы",
                               "Имя может содержать только буквы, дефисы и пробелы");
    }

    if (!middleName.isEmpty() && !nameRegex.match(middleName).hasMatch()) {
        middleNameEdit->setStyleSheet("border: 2px solid #e74c3c; background-color: #ffe6e6;");
        THROW_VALIDATION_ERROR("отчество", "содержит недопустимые символы",
                               "Отчество может содержать только буквы, дефисы и пробелы");
    }

    // Определяем роль
    UserRole role;
    QString roleText = roleCombo->currentText();

    if (roleText == "Администратор") {
        role = UserRole::Administrator;
    } else if (roleText == "Сотрудник") {
        role = UserRole::Employee;
    } else {
        THROW_VALIDATION_ERROR("роль", "недопустимое значение",
                               QString("Получена недопустимая роль: %1").arg(roleText));
    }

    // Добавляем пользователя в ожидание с указанной ролью
    bool success = m_userManager->registerPendingUser(lastName, firstName, middleName, role);

    if (success) {
        // Успешное добавление - информационное сообщение
        ExceptionHandler::showMessageBox(ErrorSeverity::INFO, "Успех",
                                         "Пользователь успешно добавлен в список ожидания!", this);

        // Возвращаемся назад
        onBackClicked();
    } else {
        // Ошибка при добавлении пользователя
        THROW_EXCEPTION(ErrorSeverity::WARNING, ErrorSource::VALIDATION,
                        "Не удалось добавить пользователя",
                        QString("Пользователь с ФИО '%1 %2 %3' уже существует в списке ожидания")
                            .arg(lastName).arg(firstName).arg(middleName));
    }
    TRY_CATCH_END
}

void AddUserWindow::onBackClicked()
{
    TRY_CATCH_BEGIN
        // Проверяем, были ли введены данные
        bool hasData = !lastNameEdit->text().isEmpty() ||
          !firstNameEdit->text().isEmpty() ||
          !middleNameEdit->text().isEmpty();

    if (hasData) {
        // Показываем предупреждение о потере данных
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

    // Создаем окно управления аккаунтами
    AccountManagementWindow *accountWindow = new AccountManagementWindow(m_userManager);
    if (!accountWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна управления аккаунтами",
                        "Не удалось создать экземпляр AccountManagementWindow");
    }

    // Показываем новое окно и закрываем текущее
    accountWindow->show();
    this->close();
    TRY_CATCH_END
}
