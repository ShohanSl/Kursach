#include "transferformwindow.h"
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

TransferFormWindow::TransferFormWindow(const Product& product, int sourceSectionNumber,
                                       const QString& materialType, bool isAdmin,
                                       UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_product(product), m_sourceSectionNumber(sourceSectionNumber),
    m_materialType(materialType), m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Оформление трансфера");
    setFixedSize(500, 500);
}

void TransferFormWindow::setupUI()
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

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ТРАНСФЕРА");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // Поля только для чтения
    QLabel *productNameLabel = new QLabel(QString("Название: %1").arg(m_product.getName()));
    QLabel *productIndexLabel = new QLabel(QString("Индекс: %1").arg(m_product.getIndex()));
    QLabel *supplierLabel = new QLabel(QString("Поставщик: %1").arg(m_product.getSupplier()));
    QLabel *cellLabel = new QLabel(QString("Ячейка: %1, Секция: %2").arg(m_product.getCellNumber()).arg(m_sourceSectionNumber));
    QLabel *quantityLabel = new QLabel(QString("Доступно: %1").arg(m_product.getQuantity()));

    // Поля для ввода
    transferQuantityEdit = new QLineEdit();
    transferQuantityEdit->setPlaceholderText(QString("Макс: %1").arg(m_product.getQuantity()));
    transferQuantityEdit->setValidator(new QIntValidator(1, m_product.getQuantity(), this));

    targetSectionCombo = new QComboBox();
    targetCellCombo = new QComboBox();

    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setCalendarPopup(true);
    dateEdit->setDisplayFormat("dd.MM.yyyy");

    // Кнопка оформления
    QPushButton *completeButton = new QPushButton("Оформить трансфер");
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
    formLayout->addRow("Количество:*", transferQuantityEdit);
    formLayout->addRow("Целевая секция:*", targetSectionCombo);
    formLayout->addRow("Целевая ячейка:*", targetCellCombo);
    formLayout->addRow("Дата:", dateEdit);

    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(completeButton);

    // Подключение сигналов
    connect(backButton, &QPushButton::clicked, this, &TransferFormWindow::onBackClicked);
    connect(completeButton, &QPushButton::clicked, this, &TransferFormWindow::onCompleteTransferClicked);
    connect(targetSectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TransferFormWindow::onTargetSectionChanged);

    // Автоматически заполняем секции на основе типа материала
    updateTargetSectionsComboBox(m_materialType);
}

void TransferFormWindow::updateTargetSectionsComboBox(const QString& productType)
{
    targetSectionCombo->clear();
    targetSectionCombo->setEnabled(true);

    // Определяем диапазон секций по типу товара
    int startSection = 1, endSection = 12;
    if (productType == "Дерево") {
        startSection = 1; endSection = 3;
    } else if (productType == "Металл") {
        startSection = 4; endSection = 6;
    } else if (productType == "Камень") {
        startSection = 7; endSection = 9;
    } else if (productType == "Пластмасса") {
        startSection = 10; endSection = 12;
    }

    // Добавляем доступные секции
    for (int i = startSection; i <= endSection; i++) {
        targetSectionCombo->addItem(QString("Секция %1").arg(i), i);
    }

    // Автоматически выбираем первую доступную секцию (кроме текущей)
    if (targetSectionCombo->count() > 0) {
        for (int i = 0; i < targetSectionCombo->count(); ++i) {
            int sectionNumber = targetSectionCombo->itemData(i).toInt();
            if (sectionNumber != m_sourceSectionNumber) {
                targetSectionCombo->setCurrentIndex(i);
                break;
            }
        }

        if (targetSectionCombo->currentIndex() == -1 && targetSectionCombo->count() > 0) {
            targetSectionCombo->setCurrentIndex(0);
        }
    }

    // Обновляем список ячеек для выбранной секции
    if (targetSectionCombo->currentIndex() >= 0) {
        int targetSectionNumber = targetSectionCombo->currentData().toInt();
        updateTargetCellsComboBox(targetSectionNumber);
    }
}

void TransferFormWindow::onTargetSectionChanged(int index)
{
    if (index < 0) return;

    int targetSectionNumber = targetSectionCombo->itemData(index).toInt();
    updateTargetCellsComboBox(targetSectionNumber);
}

void TransferFormWindow::updateTargetCellsComboBox(int targetSectionNumber)
{
    Q_UNUSED(targetSectionNumber);
    targetCellCombo->clear();
    targetCellCombo->setEnabled(true);

    // Добавляем доступные ячейки (1-60)
    for (int i = 1; i <= 60; i++) {
        targetCellCombo->addItem(QString("Ячейка %1").arg(i), i);
    }

    // Автоматически выбираем первую доступную ячейку
    if (targetCellCombo->count() > 0) {
        targetCellCombo->setCurrentIndex(0);
    }
}

void TransferFormWindow::onCompleteTransferClicked()
{
    try {
        QString quantityText = transferQuantityEdit->text().trimmed();

        // Проверка заполненности всех полей
        QList<QPair<QString, QString>> requiredFields = {
            {quantityText, "Количество для трансфера"}
        };

        InputValidator::validateAllFieldsNotEmptyOrThrow(requiredFields);

        // Проверка выбора секции и ячейки
        if (targetSectionCombo->currentIndex() == -1) {
            throw ValidationException("Выберите целевую секцию");
        }

        if (targetCellCombo->currentIndex() == -1) {
            throw ValidationException("Выберите целевую ячейку");
        }

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

        int targetSectionNumber = targetSectionCombo->currentData().toInt();
        int targetCellNumber = targetCellCombo->currentData().toInt();

        // Проверка, не пытаемся ли переместить в ту же секцию и ячейку
        if (m_sourceSectionNumber == targetSectionNumber && m_product.getCellNumber() == targetCellNumber) {
            throw ValidationException("Нельзя переместить товар в ту же ячейку");
        }

        // Проверяем занятость целевой ячейки
        QString targetProductsFile = QString("sections/section_%1.bin").arg(targetSectionNumber);
        QFile pFile(targetProductsFile);
        if (pFile.exists()) {
            if (!pFile.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл целевой секции для проверки:\n%1")
                                        .arg(pFile.errorString()));
            }

            QDataStream in(&pFile);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                pFile.close();
                throw FileException("Ошибка чтения данных целевой секции для проверки");
            }

            for (quint32 i = 0; i < size; ++i) {
                Product existingProduct;
                in >> existingProduct;

                if (in.status() != QDataStream::Ok) {
                    pFile.close();
                    throw FileException("Ошибка чтения товара при проверке целевой ячейки");
                }

                if (existingProduct.getCellNumber() == targetCellNumber) {
                    // Проверяем, можно ли объединить товары
                    if (existingProduct.getName() != m_product.getName() ||
                        existingProduct.getIndex() != m_product.getIndex() ||
                        existingProduct.getSupplier() != m_product.getSupplier()) {
                        pFile.close();
                        throw ValidationException(
                            QString("Целевая ячейка %1 в секции %2 уже занята товаром '%3'")
                                .arg(targetCellNumber)
                                .arg(targetSectionNumber)
                                .arg(existingProduct.getName()));
                    }
                }
            }
            pFile.close();
        }

        // Сохраняем данные
        saveTransfer();
        QMessageBox::information(this, "Успех", "Трансфер успешно оформлен!");

        MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
        mainWindow->show();
        this->close();

    } catch (const ValidationException& e) {
        QMessageBox::warning(this, "Неверный ввод", e.qmessage());
    } catch (const FileException& e) {
        QMessageBox::critical(this, "Ошибка файла", e.qmessage());
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка", e.qmessage());
    }
}

void TransferFormWindow::saveTransfer()
{
    try {
        int transferQuantity = transferQuantityEdit->text().toInt();
        int targetSectionNumber = targetSectionCombo->currentData().toInt();
        int targetCellNumber = targetCellCombo->currentData().toInt();
        QDateTime transferDate = dateEdit->dateTime();

        // Создаем операцию трансфера
        Operation operation(m_product.getName(), m_product.getIndex(), transferQuantity,
                            QString("Секция %1, Ячейка №%2").arg(m_sourceSectionNumber).arg(m_product.getCellNumber()),
                            QString("Секция %1, Ячейка №%2").arg(targetSectionNumber).arg(targetCellNumber),
                            Operation::TRANSFER, transferDate.date());

        // Сохраняем операцию в историю исходной секции
        QString sourceHistoryFile = QString("operations_history/section_history_%1.bin").arg(m_sourceSectionNumber);
        QDir().mkpath("operations_history");

        QList<Operation> sourceOperations;
        QFile hFile(sourceHistoryFile);
        if (hFile.exists()) {
            if (!hFile.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл истории исходной секции для чтения:\n%1")
                                        .arg(hFile.errorString()));
            }

            QDataStream in(&hFile);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                hFile.close();
                throw FileException("Ошибка чтения размера истории исходной секции");
            }

            for (quint32 i = 0; i < size; ++i) {
                Operation op;
                in >> op;

                if (in.status() != QDataStream::Ok) {
                    hFile.close();
                    throw FileException(QString("Ошибка чтения операции №%1 из истории исходной секции")
                                            .arg(i + 1));
                }

                sourceOperations.append(op);
            }
            hFile.close();
        }

        sourceOperations.append(operation);

        if (!hFile.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл истории исходной секции для записи:\n%1")
                                    .arg(hFile.errorString()));
        }

        QDataStream out(&hFile);
        out << static_cast<quint32>(sourceOperations.size());

        for (const Operation& op : sourceOperations) {
            out << op;

            if (out.status() != QDataStream::Ok) {
                hFile.close();
                throw FileException("Ошибка записи операции в историю исходной секции");
            }
        }

        hFile.close();

        // Сохраняем операцию в историю целевой секции
        QString targetHistoryFile = QString("operations_history/section_history_%1.bin").arg(targetSectionNumber);
        QList<Operation> targetOperations;
        QFile hFile2(targetHistoryFile);
        if (hFile2.exists()) {
            if (!hFile2.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл истории целевой секции для чтения:\n%1")
                                        .arg(hFile2.errorString()));
            }

            QDataStream in(&hFile2);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                hFile2.close();
                throw FileException("Ошибка чтения размера истории целевой секции");
            }

            for (quint32 i = 0; i < size; ++i) {
                Operation op;
                in >> op;

                if (in.status() != QDataStream::Ok) {
                    hFile2.close();
                    throw FileException(QString("Ошибка чтения операции №%1 из истории целевой секции")
                                            .arg(i + 1));
                }

                targetOperations.append(op);
            }
            hFile2.close();
        }

        targetOperations.append(operation);

        if (!hFile2.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл истории целевой секции для записи:\n%1")
                                    .arg(hFile2.errorString()));
        }

        QDataStream out2(&hFile2);
        out2 << static_cast<quint32>(targetOperations.size());

        for (const Operation& op : targetOperations) {
            out2 << op;

            if (out2.status() != QDataStream::Ok) {
                hFile2.close();
                throw FileException("Ошибка записи операции в историю целевой секции");
            }
        }

        hFile2.close();

        // Обновляем товар в исходной секции
        QString sourceProductsFile = QString("sections/section_%1.bin").arg(m_sourceSectionNumber);
        QList<Product> sourceProducts;
        QFile pFile(sourceProductsFile);

        if (!pFile.exists()) {
            throw FileException(QString("Файл исходной секции %1 не найден")
                                    .arg(m_sourceSectionNumber));
        }

        if (!pFile.open(QIODevice::ReadOnly)) {
            throw FileException(QString("Не удалось открыть файл исходной секции для чтения:\n%1")
                                    .arg(pFile.errorString()));
        }

        QDataStream in(&pFile);
        quint32 size;
        in >> size;

        if (in.status() != QDataStream::Ok) {
            pFile.close();
            throw FileException("Ошибка чтения размера списка товаров исходной секции");
        }

        for (quint32 i = 0; i < size; ++i) {
            Product prod;
            in >> prod;

            if (in.status() != QDataStream::Ok) {
                pFile.close();
                throw FileException(QString("Ошибка чтения товара №%1 из исходной секции")
                                        .arg(i + 1));
            }

            sourceProducts.append(prod);
        }
        pFile.close();

        // Находим и обновляем товар в исходной секции
        bool sourceProductFound = false;
        for (int i = 0; i < sourceProducts.size(); ++i) {
            if (sourceProducts[i].getCellNumber() == m_product.getCellNumber() &&
                sourceProducts[i].getIndex() == m_product.getIndex()) {

                int newQuantity = sourceProducts[i].getQuantity() - transferQuantity;

                if (newQuantity <= 0) {
                    sourceProducts.removeAt(i);
                } else {
                    sourceProducts[i].setQuantity(newQuantity);
                }
                sourceProductFound = true;
                break;
            }
        }

        if (!sourceProductFound) {
            throw ValidationException("Товар не найден в исходной секции");
        }

        // Сохраняем обновленный список товаров исходной секции
        if (!pFile.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл исходной секции для записи:\n%1")
                                    .arg(pFile.errorString()));
        }

        QDataStream out3(&pFile);
        out3 << static_cast<quint32>(sourceProducts.size());

        for (const Product& prod : sourceProducts) {
            out3 << prod;

            if (out3.status() != QDataStream::Ok) {
                pFile.close();
                throw FileException(QString("Ошибка записи товара '%1' в исходную секцию")
                                        .arg(prod.getName()));
            }
        }

        pFile.close();

        // Добавляем товар в целевую секцию
        QString targetProductsFile = QString("sections/section_%1.bin").arg(targetSectionNumber);
        QList<Product> targetProducts;
        QFile pFile2(targetProductsFile);
        if (pFile2.exists()) {
            if (!pFile2.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл целевой секции для чтения:\n%1")
                                        .arg(pFile2.errorString()));
            }

            QDataStream in2(&pFile2);
            quint32 size2;
            in2 >> size2;

            if (in2.status() != QDataStream::Ok) {
                pFile2.close();
                throw FileException("Ошибка чтения размера списка товаров целевой секции");
            }

            for (quint32 i = 0; i < size2; ++i) {
                Product prod;
                in2 >> prod;

                if (in2.status() != QDataStream::Ok) {
                    pFile2.close();
                    throw FileException(QString("Ошибка чтения товара №%1 из целевой секции")
                                            .arg(i + 1));
                }

                targetProducts.append(prod);
            }
            pFile2.close();
        }

        // Проверяем, есть ли уже такой товар в целевой секции
        bool productExists = false;
        for (int i = 0; i < targetProducts.size(); ++i) {
            if (targetProducts[i].getCellNumber() == targetCellNumber) {
                if (targetProducts[i].getName() == m_product.getName() &&
                    targetProducts[i].getIndex() == m_product.getIndex() &&
                    targetProducts[i].getSupplier() == m_product.getSupplier()) {
                    targetProducts[i].setQuantity(targetProducts[i].getQuantity() + transferQuantity);
                    productExists = true;
                    break;
                }
            }
        }

        if (!productExists) {
            targetProducts.append(Product(m_product.getName(), m_product.getIndex(),
                                          transferQuantity, m_product.getSupplier(), targetCellNumber));
        }

        // Сохраняем обновленный список товаров целевой секции
        if (!pFile2.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл целевой секции для записи:\n%1")
                                    .arg(pFile2.errorString()));
        }

        QDataStream out4(&pFile2);
        out4 << static_cast<quint32>(targetProducts.size());

        for (const Product& prod : targetProducts) {
            out4 << prod;

            if (out4.status() != QDataStream::Ok) {
                pFile2.close();
                throw FileException(QString("Ошибка записи товара '%1' в целевую секцию")
                                        .arg(prod.getName()));
            }
        }

        pFile2.close();

    } catch (const AppException& e) {
        throw; // Пробрасываем дальше
    }
}

void TransferFormWindow::onBackClicked()
{
    try {
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "transfer", m_userManager);
        warehouseWindow->show();
        this->close();
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Не удалось вернуться к складу:\n%1").arg(e.qmessage()));
    }
}
