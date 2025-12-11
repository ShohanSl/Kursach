#include "deliverywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QIntValidator>
#include <QApplication>
#include "mainwindow.h"
#include "operation.h"
#include "product.h"
#include "fileexception.h"        // Добавляем
#include "validationexception.h"  // Добавляем
#include "inputvalidator.h"       // Добавляем

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
    try {
        // Получаем значения полей
        QString supplier = supplierEdit->text().trimmed();
        QString productName = nameEdit->text().trimmed();
        QString productIndex = indexEdit->text().trimmed();
        QString quantityText = quantityEdit->text().trimmed();
        QString cellText = cellEdit->text().trimmed();

        // Проверка заполненности всех обязательных полей
        QList<QPair<QString, QString>> requiredFields = {
            {supplier, "Поставщик"},
            {productName, "Название товара"},
            {productIndex, "Индекс товара"},
            {quantityText, "Количество"},
            {cellText, "Ячейка размещения"}
        };

        InputValidator::validateAllFieldsNotEmptyOrThrow(requiredFields);

        // Проверка, что выбрана секция
        if (sectionCombo->currentIndex() == -1) {
            throw ValidationException("Выберите секцию размещения");
        }

        // Проверка числовых значений
        bool ok;
        int quantity = quantityText.toInt(&ok);
        if (!ok || quantity <= 0) {
            throw ValidationException("Количество товара должно быть положительным числом");
        }

        int cellNumber = cellText.toInt(&ok);
        if (!ok || cellNumber < 1 || cellNumber > 60) {
            throw ValidationException("Номер ячейки должен быть числом от 1 до 60");
        }

        // Валидация текстовых полей
        InputValidator::validateOrThrow(supplier, InputValidator::Mode::NonEmpty);
        InputValidator::validateOrThrow(productName, InputValidator::Mode::NonEmpty);
        InputValidator::validateOrThrow(productIndex, InputValidator::Mode::NonEmpty);

        // Проверяем занятость ячейки
        int sectionNumber = sectionCombo->currentData().toInt();
        QString productsFile = QString("sections/section_%1.bin").arg(sectionNumber);

        QFile pFile(productsFile);
        if (pFile.exists()) {
            if (!pFile.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл секции для проверки:\n%1")
                                        .arg(pFile.errorString()));
            }

            QDataStream in(&pFile);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                pFile.close();
                throw FileException("Ошибка чтения данных секции для проверки");
            }

            for (quint32 i = 0; i < size; ++i) {
                Product existingProduct;
                in >> existingProduct;

                if (in.status() != QDataStream::Ok) {
                    pFile.close();
                    throw FileException("Ошибка чтения товара при проверке ячейки");
                }

                if (existingProduct.getCellNumber() == cellNumber) {
                    // Ячейка занята - проверяем совпадение данных
                    if (existingProduct.getName() != productName ||
                        existingProduct.getIndex() != productIndex ||
                        existingProduct.getSupplier() != supplier) {
                        pFile.close();
                        throw ValidationException(
                            QString("Ячейка %1 в секции %2 уже занята другим товаром.\n\n"
                                    "Измените ячейку размещения или данные товара.")
                                .arg(cellNumber).arg(sectionNumber));
                    }
                    break;
                }
            }
            pFile.close();
        }

        // Сохраняем данные
        saveDelivery();

    } catch (const ValidationException& e) {
        QMessageBox::warning(this, "Неверный ввод", e.qmessage());
    } catch (const FileException& e) {
        QMessageBox::critical(this, "Ошибка файла", e.qmessage());
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка", e.qmessage());
    }
}

void DeliveryWindow::saveDelivery()
{
    try {
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

        CustomList<Operation> operations;
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

        // Создаем/обновляем товар в секции
        QString productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
        QDir().mkpath("sections");

        CustomList<Product> products;
        QFile pFile(productsFile);
        if (pFile.exists()) {
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

        QMessageBox::information(this, "Успех", "Поставка успешно оформлена!");
        onBackClicked();

    } catch (const FileException& e) {
        QMessageBox::critical(this, "Ошибка сохранения",
                              QString("Не удалось сохранить данные поставки:\n%1").arg(e.qmessage()));
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка", e.qmessage());
    }
}

void DeliveryWindow::onBackClicked()
{
    try {
        MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
        mainWindow->show();
        this->close();

    } catch (const AppException& e) {
        // Критическая ошибка при создании главного окна
        QMessageBox::critical(this, "Критическая ошибка",
                              QString("Не удалось вернуться в главное меню:\n%1\nПриложение будет закрыто.")
                                  .arg(e.qmessage()));

        // Закрываем приложение после закрытия окна с ошибкой
        QMessageBox::StandardButton reply = QMessageBox::critical(
            this,
            "Закрытие приложения",
            "Произошла критическая ошибка. Приложение будет закрыто.",
            QMessageBox::Close
            );

        if (reply == QMessageBox::Close) {
            qApp->quit(); // Закрываем приложение полностью
        }
    }
}
