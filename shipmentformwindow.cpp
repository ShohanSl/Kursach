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
#include "fileexception.h"        // Добавляем
#include "validationexception.h"  // Добавляем
#include "inputvalidator.h"       // Добавляем

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
    try {
        QString customer = customerEdit->text().trimmed();
        QString quantityText = shipmentQuantityEdit->text().trimmed();

        // Проверка заполненности всех полей
        QList<QPair<QString, QString>> requiredFields = {
            {customer, "Покупатель"},
            {quantityText, "Количество для отгрузки"}
        };

        InputValidator::validateAllFieldsNotEmptyOrThrow(requiredFields);

        // Проверка числовых значений
        bool ok;
        int requestedQuantity = quantityText.toInt(&ok);
        if (!ok) {
            throw ValidationException("Количество должно быть числом");
        }

        if (requestedQuantity <= 0) {
            throw ValidationException("Количество должно быть больше 0");
        }

        int availableQuantity = m_product.getQuantity();

        if (requestedQuantity > availableQuantity) {
            throw ValidationException(
                QString("Запрошенное количество (%1) превышает доступное (%2)")
                    .arg(requestedQuantity)
                    .arg(availableQuantity));
        }

        // Валидация имени покупателя
        InputValidator::validateOrThrow(customer, InputValidator::Mode::NonEmpty);

        saveShipment();
        QMessageBox::information(this, "Успех", "Отгрузка успешно оформлена!");

        MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
        mainWindow->show();
        this->close();

    } catch (const ValidationException& e) {
        QMessageBox::warning(this, "Неверный ввод", e.qmessage());
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка", e.qmessage());
    }
}

void ShipmentFormWindow::saveShipment()
{
    try {
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
        if (hFile.exists()) {
            if (!hFile.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл истории операций для чтения:\n%1")
                                        .arg(hFile.errorString()));
            }

            QDataStream in(&hFile);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                hFile.close();
                throw FileException("Ошибка чтения размера истории операций");
            }

            for (quint32 i = 0; i < size; ++i) {
                Operation op;
                in >> op;

                if (in.status() != QDataStream::Ok) {
                    hFile.close();
                    throw FileException(QString("Ошибка чтения операции №%1 из истории")
                                            .arg(i + 1));
                }

                operations.append(op);
            }
            hFile.close();
        }

        operations.append(operation);

        if (!hFile.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл истории операций для записи:\n%1")
                                    .arg(hFile.errorString()));
        }

        QDataStream out(&hFile);
        out << static_cast<quint32>(operations.size());

        for (const Operation& op : operations) {
            out << op;

            if (out.status() != QDataStream::Ok) {
                hFile.close();
                throw FileException("Ошибка записи операции в историю");
            }
        }

        hFile.close();

        // Обновляем товар в секции
        QString productsFile = QString("sections/section_%1.bin").arg(m_sectionNumber);
        QList<Product> products;
        QFile pFile(productsFile);
        if (!pFile.exists()) {
            throw FileException(QString("Файл товаров секции %1 не найден")
                                    .arg(m_sectionNumber));
        }

        if (!pFile.open(QIODevice::ReadOnly)) {
            throw FileException(QString("Не удалось открыть файл товаров для чтения:\n%1")
                                    .arg(pFile.errorString()));
        }

        QDataStream in(&pFile);
        quint32 size;
        in >> size;

        if (in.status() != QDataStream::Ok) {
            pFile.close();
            throw FileException("Ошибка чтения размера списка товаров");
        }

        for (quint32 i = 0; i < size; ++i) {
            Product prod;
            in >> prod;

            if (in.status() != QDataStream::Ok) {
                pFile.close();
                throw FileException(QString("Ошибка чтения товара №%1")
                                        .arg(i + 1));
            }

            products.append(prod);
        }
        pFile.close();

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
            throw ValidationException("Товар не найден в базе данных");
        }

        // Сохраняем обновленный список товаров
        if (!pFile.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл товаров для записи:\n%1")
                                    .arg(pFile.errorString()));
        }

        QDataStream out2(&pFile);
        out2 << static_cast<quint32>(products.size());

        for (const Product& prod : products) {
            out2 << prod;

            if (out2.status() != QDataStream::Ok) {
                pFile.close();
                throw FileException(QString("Ошибка записи товара '%1'")
                                        .arg(prod.getName()));
            }
        }

        pFile.close();

    } catch (const AppException& e) {
        throw; // Пробрасываем дальше
    }
}

void ShipmentFormWindow::onBackClicked()
{
    try {
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "shipment", m_userManager);
        warehouseWindow->show();
        this->close();
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Не удалось вернуться к складу:\n%1").arg(e.qmessage()));
    }
}
