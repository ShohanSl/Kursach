#include "transferformwindow.h"
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

TransferFormWindow::TransferFormWindow(const Product& product, int sourceSectionNumber,
                                       const QString& materialType, bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_product(product), m_sourceSectionNumber(sourceSectionNumber),
    m_materialType(materialType), m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    applyStyle();
    setWindowTitle("Оформление трансфера");
    setFixedSize(600, 650);
}

void TransferFormWindow::setupUI()
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

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ТРАНСФЕРА");
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

    cellNumberEdit = new QLineEdit(QString("Секция %1, Ячейка %2").arg(m_sourceSectionNumber).arg(m_product.getCellNumber()));
    cellNumberEdit->setReadOnly(true);

    availableQuantityEdit = new QLineEdit(QString::number(m_product.getQuantity()));
    availableQuantityEdit->setReadOnly(true);

    // Поля для ввода
    transferQuantityEdit = new QLineEdit();
    transferQuantityEdit->setPlaceholderText(QString("Макс: %1").arg(m_product.getQuantity()));
    transferQuantityEdit->setValidator(new QIntValidator(1, m_product.getQuantity(), this));

    // Поля целевой секции и ячейки
    targetSectionCombo = new QComboBox();
    targetSectionCombo->setEnabled(false); // Изначально недоступно

    targetCellCombo = new QComboBox();
    targetCellCombo->setEnabled(false);

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
    formLayout->addRow("Текущее расположение:", cellNumberEdit);
    formLayout->addRow("Доступное количество:", availableQuantityEdit);
    formLayout->addRow("Количество для трансфера:*", transferQuantityEdit);
    formLayout->addRow("Целевая секция:*", targetSectionCombo);
    formLayout->addRow("Целевая ячейка:*", targetCellCombo);
    formLayout->addRow("Дата трансфера:", dateLayout);

    // ===== КНОПКА ОФОРМЛЕНИЯ =====
    completeButton = new QPushButton("Оформить трансфер");
    completeButton->setFixedHeight(45);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(formWidget);
    mainLayout->addStretch();
    mainLayout->addWidget(completeButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &TransferFormWindow::onBackClicked);
    connect(completeButton, &QPushButton::clicked, this, &TransferFormWindow::onCompleteTransferClicked);
    connect(targetSectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TransferFormWindow::onTargetSectionChanged);

    // Автоматически заполняем секции на основе типа материала
    updateTargetSectionsComboBox(m_materialType);
}

void TransferFormWindow::applyStyle()
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
            background-color: #f39c12;
            font-size: 16px;
        }
        QPushButton#completeButton:hover {
            background-color: #e67e22;
        }
        QLineEdit, QComboBox, QDateEdit {
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
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus {
            border-color: #3498db;
        }
        QComboBox:focus {
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
    completeButton->setObjectName("completeButton");
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
        // Ищем секцию, отличную от текущей
        for (int i = 0; i < targetSectionCombo->count(); ++i) {
            int sectionNumber = targetSectionCombo->itemData(i).toInt();
            if (sectionNumber != m_sourceSectionNumber) {
                targetSectionCombo->setCurrentIndex(i);
                break;
            }
        }

        // Если все секции совпадают с текущей, выбираем первую
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
    // Валидация полей
    if (transferQuantityEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Заполните поле 'Количество для трансфера'");
        return;
    }

    if (targetSectionCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите целевую секцию");
        return;
    }

    if (targetCellCombo->currentIndex() == -1) {
        QMessageBox::warning(this, "Ошибка", "Выберите целевую ячейку");
        return;
    }

    int requestedQuantity = transferQuantityEdit->text().toInt();
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

    int targetSectionNumber = targetSectionCombo->currentData().toInt();
    int targetCellNumber = targetCellCombo->currentData().toInt();

    // Проверяем, не пытаемся ли переместить в ту же секцию и ячейку
    if (m_sourceSectionNumber == targetSectionNumber && m_product.getCellNumber() == targetCellNumber) {
        QMessageBox::warning(this, "Ошибка", "Нельзя переместить товар в ту же ячейку");
        return;
    }

    // Проверяем занятость целевой ячейки
    QString targetProductsFile = QString("sections/section_%1.bin").arg(targetSectionNumber);
    QFile pFile(targetProductsFile);
    if (pFile.exists() && pFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&pFile);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Product existingProduct;
            in >> existingProduct;
            if (existingProduct.getCellNumber() == targetCellNumber) {
                // Проверяем, можно ли объединить товары (одинаковые название, индекс, поставщик)
                if (existingProduct.getName() != m_product.getName() ||
                    existingProduct.getIndex() != m_product.getIndex() ||
                    existingProduct.getSupplier() != m_product.getSupplier()) {
                    QMessageBox::warning(this, "Ошибка",
                                         QString("Целевая ячейка %1 в секции %2 уже занята товаром '%3'")
                                             .arg(targetCellNumber)
                                             .arg(targetSectionNumber)
                                             .arg(existingProduct.getName()));
                    pFile.close();
                    return;
                }
            }
        }
        pFile.close();
    }

    // Сохраняем данные
    saveTransferToFile();

    QMessageBox::information(this, "Успех", "Трансфер успешно оформлен!");

    // Возвращаемся в главное меню
    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    mainWindow->show();
    this->close();
}

void TransferFormWindow::saveTransferToFile()
{
    int transferQuantity = transferQuantityEdit->text().toInt();
    int targetSectionNumber = targetSectionCombo->currentData().toInt();
    int targetCellNumber = targetCellCombo->currentData().toInt();

    QDateTime transferDate = dateEdit->dateTime();
    if (!dateEdit->date().isValid()) {
        transferDate = QDateTime::currentDateTime();
    }

    // Создаем операцию трансфера
    Operation operation(m_product.getName(), m_product.getIndex(), transferQuantity,
                        QString("Секция %1, Ячейка №%2").arg(m_sourceSectionNumber).arg(m_product.getCellNumber()),
                        QString("Секция %1, Ячейка №%2").arg(targetSectionNumber).arg(targetCellNumber),
                        Operation::TRANSFER, transferDate.date());

    // Сохраняем операцию в историю ИСХОДНОЙ секции
    QString sourceHistoryFile = QString("operations_history/section_history_%1.bin").arg(m_sourceSectionNumber);
    QDir().mkpath("operations_history");

    QList<Operation> sourceOperations;
    QFile hFile(sourceHistoryFile);
    if (hFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&hFile);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Operation op;
            in >> op;
            sourceOperations.append(op);
        }
        hFile.close();
    }

    sourceOperations.append(operation);

    if (hFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&hFile);
        out << static_cast<quint32>(sourceOperations.size());
        for (const Operation& op : sourceOperations) {
            out << op;
        }
        hFile.close();
    }

    // Сохраняем операцию в историю ЦЕЛЕВОЙ секции
    QString targetHistoryFile = QString("operations_history/section_history_%1.bin").arg(targetSectionNumber);
    QList<Operation> targetOperations;
    QFile hFile2(targetHistoryFile);
    if (hFile2.open(QIODevice::ReadOnly)) {
        QDataStream in(&hFile2);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Operation op;
            in >> op;
            targetOperations.append(op);
        }
        hFile2.close();
    }

    targetOperations.append(operation);

    if (hFile2.open(QIODevice::WriteOnly)) {
        QDataStream out(&hFile2);
        out << static_cast<quint32>(targetOperations.size());
        for (const Operation& op : targetOperations) {
            out << op;
        }
        hFile2.close();
    }

    // Обновляем товар в ИСХОДНОЙ секции (уменьшаем количество или удаляем)
    QString sourceProductsFile = QString("sections/section_%1.bin").arg(m_sourceSectionNumber);
    QList<Product> sourceProducts;
    QFile pFile(sourceProductsFile);
    if (pFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&pFile);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Product prod;
            in >> prod;
            sourceProducts.append(prod);
        }
        pFile.close();
    }

    // Находим и обновляем товар в исходной секции
    for (int i = 0; i < sourceProducts.size(); ++i) {
        if (sourceProducts[i].getCellNumber() == m_product.getCellNumber() &&
            sourceProducts[i].getIndex() == m_product.getIndex()) {

            int newQuantity = sourceProducts[i].getQuantity() - transferQuantity;

            if (newQuantity <= 0) {
                // Удаляем товар, если количество стало 0 или меньше
                sourceProducts.removeAt(i);
            } else {
                // Обновляем количество
                sourceProducts[i].setQuantity(newQuantity);
            }
            break;
        }
    }

    // Сохраняем обновленный список товаров исходной секции
    if (pFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&pFile);
        out << static_cast<quint32>(sourceProducts.size());
        for (const Product& prod : sourceProducts) {
            out << prod;
        }
        pFile.close();
    }

    // Добавляем товар в ЦЕЛЕВУЮ секцию
    QString targetProductsFile = QString("sections/section_%1.bin").arg(targetSectionNumber);
    QList<Product> targetProducts;
    QFile pFile2(targetProductsFile);
    if (pFile2.open(QIODevice::ReadOnly)) {
        QDataStream in(&pFile2);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Product prod;
            in >> prod;
            targetProducts.append(prod);
        }
        pFile2.close();
    }

    // Проверяем, есть ли уже такой товар в целевой секции
    bool productExists = false;
    for (int i = 0; i < targetProducts.size(); ++i) {
        if (targetProducts[i].getCellNumber() == targetCellNumber) {
            // Если это тот же товар - обновляем количество
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
        // Добавляем новый товар в целевую секцию
        targetProducts.append(Product(m_product.getName(), m_product.getIndex(),
                                      transferQuantity, m_product.getSupplier(), targetCellNumber));
    }

    // Сохраняем обновленный список товаров целевой секции
    if (pFile2.open(QIODevice::WriteOnly)) {
        QDataStream out(&pFile2);
        out << static_cast<quint32>(targetProducts.size());
        for (const Product& prod : targetProducts) {
            out << prod;
        }
        pFile2.close();
    }
}

void TransferFormWindow::onBackClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "transfer", m_userManager);
    warehouseWindow->show();
    this->close();
}
