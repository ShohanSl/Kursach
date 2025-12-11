#include "deliverywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QIntValidator>
#include "mainwindow.h"
#include "operation.h"
#include "product.h"

DeliveryWindow::DeliveryWindow(bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Оформление поставки");
    setFixedSize(500, 500);
}

void DeliveryWindow::setupUI()
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

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ПОСТАВКИ");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // Поля формы
    QWidget *formWidget = new QWidget();
    QFormLayout *formLayout = new QFormLayout(formWidget);
    formLayout->setSpacing(10);

    supplierEdit = new QLineEdit();
    supplierEdit->setPlaceholderText("Введите название поставщика");

    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    typeCombo = new QComboBox();
    typeCombo->addItem("Дерево");
    typeCombo->addItem("Металл");
    typeCombo->addItem("Камень");
    typeCombo->addItem("Пластмасса");

    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите название товара");

    indexEdit = new QLineEdit();
    indexEdit->setPlaceholderText("Введите индекс товара");

    quantityEdit = new QLineEdit();
    quantityEdit->setPlaceholderText("Введите количество");
    quantityEdit->setValidator(new QIntValidator(1, 10000, this));

    sectionCombo = new QComboBox();
    sectionCombo->setEnabled(false);

    cellEdit = new QLineEdit();
    cellEdit->setPlaceholderText("Введите номер ячейки");
    cellEdit->setEnabled(false);
    cellEdit->setValidator(new QIntValidator(1, 60, this));

    // Добавляем поля в форму
    formLayout->addRow("Поставщик:*", supplierEdit);
    formLayout->addRow("Дата поставки:", dateEdit);
    formLayout->addRow("Тип товара:*", typeCombo);
    formLayout->addRow("Название товара:*", nameEdit);
    formLayout->addRow("Индекс товара:*", indexEdit);
    formLayout->addRow("Количество:*", quantityEdit);
    formLayout->addRow("Секция размещения:*", sectionCombo);
    formLayout->addRow("Ячейка размещения:*", cellEdit);

    // Кнопка оформления
    QPushButton *completeButton = new QPushButton("Оформить поставку");
    completeButton->setFixedHeight(35);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addWidget(completeButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &DeliveryWindow::onBackClicked);
    connect(completeButton, &QPushButton::clicked, this, &DeliveryWindow::onCompleteDeliveryClicked);
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeliveryWindow::onProductTypeChanged);

    // Инициализируем секции для текущего типа
    onProductTypeChanged(0);
}

void DeliveryWindow::onProductTypeChanged(int index)
{
    Q_UNUSED(index);
    updateSectionComboBox();
}

void DeliveryWindow::updateSectionComboBox()
{
    sectionCombo->clear();

    // Определяем диапазон секций по типу товара
    int startSection = 1, endSection = 12;
    QString productType = typeCombo->currentText();

    if (productType == "Дерево") {
        startSection = 1; endSection = 3;
    } else if (productType == "Металл") {
        startSection = 4; endSection = 6;
    } else if (productType == "Камень") {
        startSection = 7; endSection = 9;
    } else if (productType == "Пластмасса") {
        startSection = 10; endSection = 12;
    }

    for (int i = startSection; i <= endSection; i++) {
        sectionCombo->addItem(QString("Секция %1").arg(i), i);
    }

    // Активируем комбобокс и поле ячейки
    sectionCombo->setEnabled(true);
    cellEdit->setEnabled(sectionCombo->count() > 0);
}

void DeliveryWindow::onCompleteDeliveryClicked()
{
    // Валидация основных полей
    if (supplierEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Поставщик'");
        return;
    }

    if (nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Название товара'");
        return;
    }

    if (indexEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Индекс товара'");
        return;
    }

    if (quantityEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Количество'");
        return;
    }

    if (sectionCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите секцию размещения");
        return;
    }

    if (cellEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Ячейка размещения'");
        return;
    }

    // Проверка числовых полей
    bool ok;
    int quantity = quantityEdit->text().toInt(&ok);
    if (!ok || quantity <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество товара должно быть положительным числом");
        return;
    }

    int cellNumber = cellEdit->text().toInt(&ok);
    if (!ok || cellNumber < 1 || cellNumber > 60) {
        QMessageBox::warning(this, "Ошибка", "Номер ячейки должен быть числом от 1 до 60");
        return;
    }

    // Проверяем занятость ячейки
    int sectionNumber = sectionCombo->currentData().toInt();
    QString productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
    QString supplier = supplierEdit->text().trimmed();
    QString productName = nameEdit->text().trimmed();
    QString productIndex = indexEdit->text().trimmed();

    QFile pFile(productsFile);
    if (pFile.exists() && pFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&pFile);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Product existingProduct;
            in >> existingProduct;

            if (existingProduct.getCellNumber() == cellNumber) {
                // Ячейка занята - проверяем совпадение данных
                if (existingProduct.getName() != productName ||
                    existingProduct.getIndex() != productIndex ||
                    existingProduct.getSupplier() != supplier) {
                    // Данные не совпадают - конфликт
                    QMessageBox::critical(this, "Ошибка",
                                          QString("Ячейка %1 в секции %2 уже занята другим товаром.\n\n"
                                                  "Измените ячейку размещения или данные товара.")
                                              .arg(cellNumber).arg(sectionNumber));
                    pFile.close();
                    return;
                }
                break;
            }
        }
        pFile.close();
    }

    // Сохраняем данные
    saveDelivery();
}

void DeliveryWindow::saveDelivery()
{
    QString supplier = supplierEdit->text().trimmed();
    QDateTime deliveryDate = dateEdit->dateTime();
    if (!dateEdit->date().isValid()) {
        deliveryDate = QDateTime::currentDateTime();
    }

    QString productName = nameEdit->text().trimmed();
    QString productIndex = indexEdit->text().trimmed();
    int quantity = quantityEdit->text().toInt();
    int sectionNumber = sectionCombo->currentData().toInt();
    int cellNumber = cellEdit->text().toInt();

    // Создаем операцию поставки
    Operation operation(productName, productIndex, quantity,
                        supplier, QString("Ячейка №%1").arg(cellNumber),
                        Operation::DELIVERY, deliveryDate.date());

    // Сохраняем операцию в историю
    QString historyFile = QString("operations_history/section_history_%1.bin").arg(sectionNumber);
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

    // Создаем/обновляем товар в секции
    QString productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
    QDir().mkpath("sections");

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

    // Проверяем, есть ли уже товар в этой ячейке
    bool productExists = false;
    for (int i = 0; i < products.size(); ++i) {
        if (products[i].getCellNumber() == cellNumber) {
            // Обновляем существующий товар
            products[i].setQuantity(products[i].getQuantity() + quantity);
            productExists = true;
            break;
        }
    }

    if (!productExists) {
        // Добавляем новый товар
        products.append(Product(productName, productIndex, quantity, supplier, cellNumber));
    }

    if (pFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&pFile);
        out << static_cast<quint32>(products.size());
        for (const Product& prod : products) {
            out << prod;
        }
        pFile.close();
    }

    QMessageBox::information(this, "Успех", "Поставка успешно оформлена!");
    onBackClicked();
}

void DeliveryWindow::onBackClicked()
{
    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    mainWindow->show();
    this->close();
}
