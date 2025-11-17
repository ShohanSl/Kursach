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

ShipmentFormWindow::ShipmentFormWindow(const Product& product, int sectionNumber,
                                       bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_product(product), m_sectionNumber(sectionNumber),
    m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    applyStyle();
    setWindowTitle("Оформление отгрузки");
    setFixedSize(600, 650);
}

void ShipmentFormWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 15, 20, 15);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ =====
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ОТГРУЗКИ");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // ===== ФОРМА ДАННЫХ =====
    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(12);
    formLayout->setContentsMargins(0, 0, 0, 0);

    // Поля только для чтения (автоматически заполняются)
    productNameEdit = new QLineEdit(m_product.getName());
    productNameEdit->setReadOnly(true);

    productIndexEdit = new QLineEdit(m_product.getIndex());
    productIndexEdit->setReadOnly(true);

    supplierEdit = new QLineEdit(m_product.getSupplier());
    supplierEdit->setReadOnly(true);

    cellNumberEdit = new QLineEdit(QString::number(m_product.getCellNumber()));
    cellNumberEdit->setReadOnly(true);

    availableQuantityEdit = new QLineEdit(QString::number(m_product.getQuantity()));
    availableQuantityEdit->setReadOnly(true);

    // Поля для ввода
    customerEdit = new QLineEdit();
    customerEdit->setPlaceholderText("Введите название покупателя");

    shipmentQuantityEdit = new QLineEdit();
    shipmentQuantityEdit->setPlaceholderText(QString("Макс: %1").arg(m_product.getQuantity()));
    shipmentQuantityEdit->setValidator(new QIntValidator(1, m_product.getQuantity(), this));

    // Поле даты
    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    QLabel *dateOptionalLabel = new QLabel("(необязательно)");
    dateOptionalLabel->setStyleSheet("color: #7f8c8d; font-size: 12px;");

    QHBoxLayout *dateLayout = new QHBoxLayout();
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
    completeButton->setFixedHeight(45);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(completeButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &ShipmentFormWindow::onBackClicked);
    connect(completeButton, &QPushButton::clicked, this, &ShipmentFormWindow::onCompleteShipmentClicked);
}

void ShipmentFormWindow::applyStyle()
{
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
}

void ShipmentFormWindow::onCompleteShipmentClicked()
{
    // Валидация полей
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

    QMessageBox::information(this, "Успех", "Отгрузка успешно оформлена!");

    // Возвращаемся в главное меню
    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    mainWindow->show();
    this->close();
}

void ShipmentFormWindow::saveShipmentToFile()
{
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
    QDir().mkpath("operations_history");

    QList<Operation> operations;
    QFile hFile(historyFile);
    if (hFile.open(QIODevice::ReadOnly)) {
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

    if (hFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&hFile);
        out << static_cast<quint32>(operations.size());
        for (const Operation& op : operations) {
            out << op;
        }
        hFile.close();
    }

    // Обновляем товар в секции
    QString productsFile = QString("sections/section_%1.bin").arg(m_sectionNumber);
    QList<Product> products;
    QFile pFile(productsFile);
    if (pFile.open(QIODevice::ReadOnly)) {
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
            break;
        }
    }

    // Сохраняем обновленный список товаров
    if (pFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&pFile);
        out << static_cast<quint32>(products.size());
        for (const Product& prod : products) {
            out << prod;
        }
        pFile.close();
    }
}

void ShipmentFormWindow::onBackClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "shipment", m_userManager);
    warehouseWindow->show();
    this->close();
}
