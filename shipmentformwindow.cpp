#include "shipmentformwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include "mainwindow.h"
#include "sectionwindow.h"
#include "warehousewindow.h"
#include "exceptionhandler.h" // Добавляем заголовок для обработки исключений

ShipmentFormWindow::ShipmentFormWindow(const Product& product, int sectionNumber,
                                       bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_product(product), m_sectionNumber(sectionNumber),
    m_isAdmin(isAdmin), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Оформление отгрузки");
    setFixedSize(600, 650);
    TRY_CATCH_END
}

void ShipmentFormWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет окна отгрузки");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет окна отгрузки");
    }
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 15, 20, 15);

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

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ОТГРУЗКИ");
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

    // Поля только для чтения (автоматически заполняются)
    productNameEdit = new QLineEdit(m_product.getName());
    if (!productNameEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля названия товара",
                        "Не удалось создать поле ввода для названия товара");
    }
    productNameEdit->setReadOnly(true);

    productIndexEdit = new QLineEdit(m_product.getIndex());
    if (!productIndexEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля индекса товара",
                        "Не удалось создать поле ввода для индекса товара");
    }
    productIndexEdit->setReadOnly(true);

    supplierEdit = new QLineEdit(m_product.getSupplier());
    if (!supplierEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля поставщика",
                        "Не удалось создать поле ввода для поставщика");
    }
    supplierEdit->setReadOnly(true);

    cellNumberEdit = new QLineEdit(QString::number(m_product.getCellNumber()));
    if (!cellNumberEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля номера ячейки",
                        "Не удалось создать поле ввода для номера ячейки");
    }
    cellNumberEdit->setReadOnly(true);

    availableQuantityEdit = new QLineEdit(QString::number(m_product.getQuantity()));
    if (!availableQuantityEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля доступного количества",
                        "Не удалось создать поле ввода для доступного количества");
    }
    availableQuantityEdit->setReadOnly(true);

    // Поля для ввода
    customerEdit = new QLineEdit();
    if (!customerEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля покупателя",
                        "Не удалось создать поле ввода для покупателя");
    }
    customerEdit->setPlaceholderText("Введите название покупателя");

    shipmentQuantityEdit = new QLineEdit();
    if (!shipmentQuantityEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля количества отгрузки",
                        "Не удалось создать поле ввода для количества отгрузки");
    }
    shipmentQuantityEdit->setPlaceholderText(QString("Макс: %1").arg(m_product.getQuantity()));
    shipmentQuantityEdit->setValidator(new QIntValidator(1, m_product.getQuantity(), this));

    // Поле даты
    dateEdit = new QDateEdit();
    if (!dateEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля даты",
                        "Не удалось создать поле ввода даты");
    }
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    QLabel *dateOptionalLabel = new QLabel("(необязательно)");
    if (!dateOptionalLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания метки даты",
                        "Не удалось создать метку для даты");
    }
    dateOptionalLabel->setStyleSheet("color: #7f8c8d; font-size: 12px;");

    QHBoxLayout *dateLayout = new QHBoxLayout();
    if (!dateLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета даты",
                        "Не удалось создать макет для даты");
    }
    dateLayout->addWidget(dateEdit);
    dateLayout->addWidget(dateOptionalLabel);
    dateLayout->addStretch();

    // Добавляем поля в форму
    formLayout->addRow("Название товара:", productNameEdit);
    formLayout->addRow("Индекс товара:", productIndexEdit);
    formLayout->addRow("Поставщик:", supplierEdit);
    formLayout->addRow("Номер ячейки:", cellNumberEdit);
    formLayout->addRow("Доступное количество:", availableQuantityEdit);
    formLayout->addRow("Покупатель:*", customerEdit);
    formLayout->addRow("Количество для отгрузки:*", shipmentQuantityEdit);
    formLayout->addRow("Дата отгрузки:", dateLayout);

    // ===== КНОПКА ОФОРМЛЕНИЯ =====
    completeButton = new QPushButton("Оформить отгрузку");
    if (!completeButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки оформления",
                        "Не удалось создать кнопку оформления отгрузки");
    }
    completeButton->setFixedHeight(45);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(completeButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &ShipmentFormWindow::onBackClicked);
    connect(completeButton, &QPushButton::clicked, this, &ShipmentFormWindow::onCompleteShipmentClicked);
    TRY_CATCH_END
}

void ShipmentFormWindow::applyStyle()
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
        QPushButton#completeButton {
            background-color: #27ae60;
            font-size: 16px;
        }
        QPushButton#completeButton:hover {
            background-color: #219a52;
        }
        QLineEdit, QDateEdit {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 5px;
            padding: 8px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
        }
        QLineEdit:read-only {
            background-color: #f8f9fa;
            color: #6c757d;
        }
        QLineEdit:focus, QDateEdit:focus {
            border-color: #3498db;
        }
        QDateEdit:focus {
            border-color: #3498db;
        }
    )");

    backButton->setObjectName("backButton");
    completeButton->setObjectName("completeButton");
    TRY_CATCH_END
}

void ShipmentFormWindow::onCompleteShipmentClicked()
{
    TRY_CATCH_BEGIN
        // Валидация полей (пользовательские ошибки - оставляем QMessageBox)
        if (customerEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Покупатель'");
        return;
    }

    if (shipmentQuantityEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Количество для отгрузки'");
        return;
    }

    int requestedQuantity = shipmentQuantityEdit->text().toInt();
    int availableQuantity = m_product.getQuantity();

    if (requestedQuantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество должно быть больше 0");
        return;
    }

    if (requestedQuantity > availableQuantity) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Запрошенное количество (%1) превышает доступное (%2)")
                                 .arg(requestedQuantity)
                                 .arg(availableQuantity));
        return;
    }

    // Сохраняем данные
    saveShipmentToFile();

    // Используем ExceptionHandler для информационного сообщения
    ExceptionHandler::showMessageBox(ErrorSeverity::INFO, "Успех",
                                     "Отгрузка успешно оформлена!", this);

    // Возвращаемся в главное меню
    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    if (!mainWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного окна",
                        "Не удалось создать экземпляр MainWindow");
    }
    mainWindow->show();
    this->close();
    TRY_CATCH_END
}

void ShipmentFormWindow::saveShipmentToFile()
{
    TRY_CATCH_BEGIN
        QString customer = customerEdit->text().trimmed();
    int shipmentQuantity = shipmentQuantityEdit->text().toInt();

    QDateTime shipmentDate = dateEdit->dateTime();
    if (!dateEdit->date().isValid()) {
        shipmentDate = QDateTime::currentDateTime();
    }

    // Создаем операцию отгрузки
    Operation operation(m_product.getName(), m_product.getIndex(), shipmentQuantity,
                        QString("Ячейка №%1").arg(m_product.getCellNumber()), customer,
                        Operation::SHIPMENT, shipmentDate.date()); // Берем только дату

    // Сохраняем операцию в историю
    QString historyFile = QString("operations_history/section_history_%1.bin").arg(m_sectionNumber);
    QDir historyDir("operations_history");
    if (!historyDir.exists() && !historyDir.mkpath(".")) {
        THROW_FILE_ERROR("operations_history", "создания директории",
                         "Не удалось создать директорию для истории операций");
    }

    CustomList<Operation> operations;
    QFile hFile(historyFile);
    if (hFile.exists()) {
        if (!hFile.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(historyFile, "открытия для чтения",
                             "Не удалось прочитать историю операций");
        }
        QDataStream in(&hFile);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Operation op;
            in >> op;
            operations.append(op);
        }
        hFile.close();
    }

    operations.append(operation);

    if (!hFile.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(historyFile, "открытия для записи",
                         "Не удалось сохранить историю операций");
    }
    QDataStream outHistory(&hFile);
    outHistory << static_cast<quint32>(operations.size());
    for (const Operation& op : operations) {
        outHistory << op;
    }
    hFile.close();

    // Обновляем товар в секции
    QString productsFile = QString("sections/section_%1.bin").arg(m_sectionNumber);
    CustomList<Product> products;
    QFile pFile(productsFile);
    if (pFile.exists()) {
        if (!pFile.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(productsFile, "открытия для чтения",
                             "Не удалось прочитать список товаров");
        }
        QDataStream in(&pFile);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Product prod;
            in >> prod;
            products.append(prod);
        }
        pFile.close();
    }

    // Находим и обновляем товар
    bool productFound = false;
    for (int i = 0; i < products.size(); ++i) {
        if (products[i].getCellNumber() == m_product.getCellNumber() &&
            products[i].getIndex() == m_product.getIndex()) {

            int newQuantity = products[i].getQuantity() - shipmentQuantity;

            if (newQuantity <= 0) {
                // Удаляем товар, если количество стало 0 или меньше
                products.removeAt(i);
            } else {
                // Обновляем количество
                products[i].setQuantity(newQuantity);
            }
            productFound = true;
            break;
        }
    }

    if (!productFound) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::VALIDATION,
                        "Товар не найден в базе данных",
                        QString("Товар '%1' с индексом '%2' в ячейке %3 не найден в секции %4")
                            .arg(m_product.getName())
                            .arg(m_product.getIndex())
                            .arg(m_product.getCellNumber())
                            .arg(m_sectionNumber));
    }

    // Сохраняем обновленный список товаров
    if (!pFile.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(productsFile, "открытия для записи",
                         "Не удалось сохранить обновленный список товаров");
    }
    QDataStream outProducts(&pFile);
    outProducts << static_cast<quint32>(products.size());
    for (const Product& prod : products) {
        outProducts << prod;
    }
    pFile.close();
    TRY_CATCH_END
}

void ShipmentFormWindow::onBackClicked()
{
    TRY_CATCH_BEGIN
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "shipment", m_userManager);
    if (!warehouseWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна склада",
                        "Не удалось создать экземпляр WarehouseWindow");
    }
    warehouseWindow->show();
    this->close();
    TRY_CATCH_END
}
