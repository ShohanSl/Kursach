#include "shipmentformwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QIntValidator>
#include "mainwindow.h"
#include "warehousewindow.h"

ShipmentFormWindow::ShipmentFormWindow(const Product& product, int sectionNumber,
                                       bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_product(product), m_sectionNumber(sectionNumber),
    m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Оформление отгрузки");
    setFixedSize(500, 500);
}

void ShipmentFormWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 15, 20, 15);

    // Верхняя панель
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QPushButton *backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 30);

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ОТГРУЗКИ");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // Поля только для чтения
    QLabel *productNameLabel = new QLabel(QString("Название: %1").arg(m_product.getName()));
    QLabel *productIndexLabel = new QLabel(QString("Индекс: %1").arg(m_product.getIndex()));
    QLabel *supplierLabel = new QLabel(QString("Поставщик: %1").arg(m_product.getSupplier()));
    QLabel *cellLabel = new QLabel(QString("Ячейка: %1").arg(m_product.getCellNumber()));
    QLabel *quantityLabel = new QLabel(QString("Доступно: %1").arg(m_product.getQuantity()));

    // Поля для ввода
    customerEdit = new QLineEdit();
    customerEdit->setPlaceholderText("Введите название покупателя");

    shipmentQuantityEdit = new QLineEdit();
    shipmentQuantityEdit->setPlaceholderText(QString("Макс: %1").arg(m_product.getQuantity()));
    shipmentQuantityEdit->setValidator(new QIntValidator(1, m_product.getQuantity(), this));

    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    // Кнопка оформления
    QPushButton *completeButton = new QPushButton("Оформить отгрузку");
    completeButton->setFixedHeight(35);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(productNameLabel);
    mainLayout->addWidget(productIndexLabel);
    mainLayout->addWidget(supplierLabel);
    mainLayout->addWidget(cellLabel);
    mainLayout->addWidget(quantityLabel);

    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);
    formLayout->addRow("Покупатель:*", customerEdit);
    formLayout->addRow("Количество:*", shipmentQuantityEdit);
    formLayout->addRow("Дата:", dateEdit);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(completeButton);

    // Подключение сигналов
    connect(backButton, &QPushButton::clicked, this, &ShipmentFormWindow::onBackClicked);
    connect(completeButton, &QPushButton::clicked, this, &ShipmentFormWindow::onCompleteShipmentClicked);
}

void ShipmentFormWindow::onCompleteShipmentClicked()
{
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

    saveShipment();
    QMessageBox::information(this, "Успех", "Отгрузка успешно оформлена!");

    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    mainWindow->show();
    this->close();
}

void ShipmentFormWindow::saveShipment()
{
    QString customer = customerEdit->text().trimmed();
    int shipmentQuantity = shipmentQuantityEdit->text().toInt();
    QDateTime shipmentDate = dateEdit->dateTime();

    // Создаем операцию отгрузки
    Operation operation(m_product.getName(), m_product.getIndex(), shipmentQuantity,
                        QString("Ячейка №%1").arg(m_product.getCellNumber()), customer,
                        Operation::SHIPMENT, shipmentDate.date());

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
    bool productFound = false;
    for (int i = 0; i < products.size(); ++i) {
        if (products[i].getCellNumber() == m_product.getCellNumber() &&
            products[i].getIndex() == m_product.getIndex()) {

            int newQuantity = products[i].getQuantity() - shipmentQuantity;

            if (newQuantity <= 0) {
                products.removeAt(i);
            } else {
                products[i].setQuantity(newQuantity);
            }
            productFound = true;
            break;
        }
    }

    if (!productFound) {
        QMessageBox::warning(this, "Ошибка", "Товар не найден в базе данных");
        return;
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
