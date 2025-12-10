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
#include "exceptionhandler.h" // Добавляем заголовок для обработки исключений

TransferFormWindow::TransferFormWindow(const Product& product, int sourceSectionNumber,
                                       const QString& materialType, bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_product(product), m_sourceSectionNumber(sourceSectionNumber),
    m_materialType(materialType), m_isAdmin(isAdmin), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Оформление трансфера");
    setFixedSize(600, 650);
    TRY_CATCH_END
}

void TransferFormWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет окна трансфера");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет окна трансфера");
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

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ТРАНСФЕРА");
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

    cellNumberEdit = new QLineEdit(QString("Секция %1, Ячейка %2").arg(m_sourceSectionNumber).arg(m_product.getCellNumber()));
    if (!cellNumberEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля текущего расположения",
                        "Не удалось создать поле ввода для текущего расположения");
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
    transferQuantityEdit = new QLineEdit();
    if (!transferQuantityEdit) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания поля количества трансфера",
                        "Не удалось создать поле ввода для количества трансфера");
    }
    transferQuantityEdit->setPlaceholderText(QString("Макс: %1").arg(m_product.getQuantity()));
    transferQuantityEdit->setValidator(new QIntValidator(1, m_product.getQuantity(), this));

    // Поля целевой секции и ячейки
    targetSectionCombo = new QComboBox();
    if (!targetSectionCombo) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания комбобокса целевой секции",
                        "Не удалось создать комбобокс для выбора целевой секции");
    }
    targetSectionCombo->setEnabled(false); // Изначально недоступно

    targetCellCombo = new QComboBox();
    if (!targetCellCombo) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания комбобокса целевой ячейки",
                        "Не удалось создать комбобокс для выбора целевой ячейки");
    }
    targetCellCombo->setEnabled(false);

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
    formLayout->addRow("Текущее расположение:", cellNumberEdit);
    formLayout->addRow("Доступное количество:", availableQuantityEdit);
    formLayout->addRow("Количество для трансфера:*", transferQuantityEdit);
    formLayout->addRow("Целевая секция:*", targetSectionCombo);
    formLayout->addRow("Целевая ячейка:*", targetCellCombo);
    formLayout->addRow("Дата трансфера:", dateLayout);

    // ===== КНОПКА ОФОРМЛЕНИЯ =====
    completeButton = new QPushButton("Оформить трансфер");
    if (!completeButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки оформления",
                        "Не удалось создать кнопку оформления трансфера");
    }
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
    TRY_CATCH_END
}

void TransferFormWindow::applyStyle()
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
    TRY_CATCH_END
}

void TransferFormWindow::updateTargetSectionsComboBox(const QString& productType)
{
    TRY_CATCH_BEGIN
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
    TRY_CATCH_END
}

void TransferFormWindow::onTargetSectionChanged(int index)
{
    TRY_CATCH_BEGIN
        if (index < 0) return;

    int targetSectionNumber = targetSectionCombo->itemData(index).toInt();
    updateTargetCellsComboBox(targetSectionNumber);
    TRY_CATCH_END
}

void TransferFormWindow::updateTargetCellsComboBox(int targetSectionNumber)
{
    TRY_CATCH_BEGIN
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
    TRY_CATCH_END
}

void TransferFormWindow::onCompleteTransferClicked()
{
    TRY_CATCH_BEGIN
        // Валидация полей (пользовательские ошибки - оставляем QMessageBox)
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

    // Используем ExceptionHandler для информационного сообщения
    ExceptionHandler::showMessageBox(ErrorSeverity::INFO, "Успех",
                                     "Трансфер успешно оформлен!", this);

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

void TransferFormWindow::saveTransferToFile()
{
    TRY_CATCH_BEGIN
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
    QDir historyDir("operations_history");
    if (!historyDir.exists() && !historyDir.mkpath(".")) {
        THROW_FILE_ERROR("operations_history", "создания директории",
                         "Не удалось создать директорию для истории операций");
    }

    CustomList<Operation> sourceOperations;
    QFile hFile(sourceHistoryFile);
    if (hFile.exists()) {
        if (!hFile.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(sourceHistoryFile, "открытия для чтения",
                             "Не удалось прочитать историю операций исходной секции");
        }
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

    if (!hFile.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(sourceHistoryFile, "открытия для записи",
                         "Не удалось сохранить историю операций исходной секции");
    }
    QDataStream outHistory(&hFile);
    outHistory << static_cast<quint32>(sourceOperations.size());
    for (const Operation& op : sourceOperations) {
        outHistory << op;
    }
    hFile.close();

    // Сохраняем операцию в историю ЦЕЛЕВОЙ секции
    QString targetHistoryFile = QString("operations_history/section_history_%1.bin").arg(targetSectionNumber);
    CustomList<Operation> targetOperations;
    QFile hFile2(targetHistoryFile);
    if (hFile2.exists()) {
        if (!hFile2.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(targetHistoryFile, "открытия для чтения",
                             "Не удалось прочитать историю операций целевой секции");
        }
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

    if (!hFile2.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(targetHistoryFile, "открытия для записи",
                         "Не удалось сохранить историю операций целевой секции");
    }
    QDataStream outHistory2(&hFile2);
    outHistory2 << static_cast<quint32>(targetOperations.size());
    for (const Operation& op : targetOperations) {
        outHistory2 << op;
    }
    hFile2.close();

    // Обновляем товар в ИСХОДНОЙ секции (уменьшаем количество или удаляем)
    QString sourceProductsFile = QString("sections/section_%1.bin").arg(m_sourceSectionNumber);
    CustomList<Product> sourceProducts;
    QFile pFile(sourceProductsFile);
    if (pFile.exists()) {
        if (!pFile.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(sourceProductsFile, "открытия для чтения",
                             "Не удалось прочитать список товаров исходной секции");
        }
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
    bool sourceProductFound = false;
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
            sourceProductFound = true;
            break;
        }
    }

    if (!sourceProductFound) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::VALIDATION,
                        "Товар не найден в исходной секции",
                        QString("Товар '%1' с индексом '%2' в ячейке %3 не найден в секции %4")
                            .arg(m_product.getName())
                            .arg(m_product.getIndex())
                            .arg(m_product.getCellNumber())
                            .arg(m_sourceSectionNumber));
    }

    // Сохраняем обновленный список товаров исходной секции
    if (!pFile.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(sourceProductsFile, "открытия для записи",
                         "Не удалось сохранить обновленный список товаров исходной секции");
    }
    QDataStream outProducts(&pFile);
    outProducts << static_cast<quint32>(sourceProducts.size());
    for (const Product& prod : sourceProducts) {
        outProducts << prod;
    }
    pFile.close();

    // Добавляем товар в ЦЕЛЕВУЮ секцию
    QString targetProductsFile = QString("sections/section_%1.bin").arg(targetSectionNumber);
    CustomList<Product> targetProducts;
    QFile pFile2(targetProductsFile);
    if (pFile2.exists()) {
        if (!pFile2.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(targetProductsFile, "открытия для чтения",
                             "Не удалось прочитать список товаров целевой секции");
        }
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
    if (!pFile2.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(targetProductsFile, "открытия для записи",
                         "Не удалось сохранить обновленный список товаров целевой секции");
    }
    QDataStream outProducts2(&pFile2);
    outProducts2 << static_cast<quint32>(targetProducts.size());
    for (const Product& prod : targetProducts) {
        outProducts2 << prod;
    }
    pFile2.close();
    TRY_CATCH_END
}

void TransferFormWindow::onBackClicked()
{
    TRY_CATCH_BEGIN
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "transfer", m_userManager);
    if (!warehouseWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна склада",
                        "Не удалось создать экземпляр WarehouseWindow");
    }
    warehouseWindow->show();
    this->close();
    TRY_CATCH_END
}
