#include "deliverywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include "mainwindow.h"
#include "sectionwindow.h"
#include "operation.h"
#include "product.h"
#include <QDebug>
#include "exceptionhandler.h"

DeliveryWindow::DeliveryWindow(bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Оформление поставки");
    setFixedSize(800, 600);
    TRY_CATCH_END
}

void DeliveryWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет окна поставки");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет окна поставки");
    }
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 15, 20, 15);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ =====
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    QLabel *titleLabel = new QLabel("ОФОРМЛЕНИЕ ПОСТАВКИ");
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // ===== ОСНОВНОЙ КОНТЕНТ =====
    scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setSpacing(15);

    // ===== ПОЛЯ ПОСТАВЩИКА И ДАТЫ =====
    QWidget *supplierDateWidget = new QWidget();
    QFormLayout *supplierDateLayout = new QFormLayout(supplierDateWidget);
    supplierDateLayout->setSpacing(10);

    supplierEdit = new QLineEdit();
    supplierEdit->setPlaceholderText("Введите название поставщика");

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

    supplierDateLayout->addRow("Поставщик:*", supplierEdit);
    supplierDateLayout->addRow("Дата поставки:", dateLayout);

    // ===== КНОПКА ДОБАВЛЕНИЯ ТОВАРА =====
    QPushButton *addProductButton = new QPushButton("+ Добавить товар");
    addProductButton->setFixedSize(170, 35);

    // ===== ОБЛАСТЬ ДЛЯ ДИНАМИЧЕСКИХ ПОЛЕЙ ТОВАРОВ =====
    productsLayout = new QVBoxLayout();
    productsLayout->setSpacing(10);

    // ===== КНОПКА ОФОРМЛЕНИЯ =====
    completeButton = new QPushButton("Оформить поставку");
    completeButton->setFixedHeight(45);

    // Собираем все вместе
    scrollLayout->addWidget(supplierDateWidget);
    scrollLayout->addWidget(addProductButton, 0, Qt::AlignLeft);
    scrollLayout->addLayout(productsLayout);
    scrollLayout->addStretch();

    scrollArea->setWidget(scrollContent);

    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(completeButton);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &DeliveryWindow::onBackClicked);
    connect(addProductButton, &QPushButton::clicked, this, &DeliveryWindow::onAddProductClicked);
    connect(completeButton, &QPushButton::clicked, this, &DeliveryWindow::onCompleteDeliveryClicked);

    // Добавляем первый набор полей товара
    addProductFields();
    TRY_CATCH_END
}

void DeliveryWindow::applyStyle()
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
        QPushButton#removeButton {
            background-color: #e74c3c;
            font-size: 12px;
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА */
        }
        QPushButton#removeButton:hover {
            background-color: #c0392b;
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА ПРИ НАВЕДЕНИИ */
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
        QLineEdit:focus, QComboBox:focus, QDateEdit:focus {
            border-color: #3498db;
        }
        QComboBox QAbstractItemView {
            background-color: white;
            border: 1px solid #bdc3c7;
            selection-background-color: #3498db;
            color: #000000;
        }
        QGroupBox {
            background-color: white;
            border: 2px solid #3498db;
            border-radius: 8px;
            margin-top: 10px;
            padding-top: 10px;
            font-weight: bold;
            color: #2c3e50;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
/* СТИЛИ ДЛЯ КАЛЕНДАРЯ */
        QCalendarWidget {
            background-color: white;
            border: 1px solid #bdc3c7;
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА */
        }
        QCalendarWidget QWidget {
            alternate-background-color: white;
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА */
        }
        QCalendarWidget QToolButton {
            background-color: #3498db;
            color: white;
            font-size: 14px;
            font-weight: bold;
            border-radius: 5px;
            padding: 5px;
        }
        QCalendarWidget QToolButton:hover {
            background-color: #2980b9;
        }
        QCalendarWidget QMenu {
            background-color: white;
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА */
        }
        QCalendarWidget QSpinBox {
            background-color: white;
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА */
            border: 1px solid #bdc3c7;
            border-radius: 3px;
            padding: 3px;
        }
        QCalendarWidget QAbstractItemView:enabled {
            color: #000000; /* ЧЕРНЫЙ ЦВЕТ ТЕКСТА ДНЕЙ */
            background-color: white;
            selection-background-color: #3498db;
            selection-color: white;
        }
        QCalendarWidget QAbstractItemView:disabled {
            color: #7f8c8d; /* СЕРЫЙ ЦВЕТ ДЛЯ НЕАКТИВНЫХ ДНЕЙ */
        }
    )");

    backButton->setObjectName("backButton");
    completeButton->setObjectName("completeButton");
}

void DeliveryWindow::addProductFields()
{
    QWidget *productWidget = new QWidget();
    productWidget->setStyleSheet("background-color: white; border: 1px solid #bdc3c7; border-radius: 5px; padding: 10px;");

    QFormLayout *formLayout = new QFormLayout(productWidget);
    formLayout->setSpacing(8);

    // Тип товара
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("Дерево");
    typeCombo->addItem("Металл");
    typeCombo->addItem("Камень");
    typeCombo->addItem("Пластмасса");

    // Название товара
    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Введите название товара");

    // Индекс товара
    QLineEdit *indexEdit = new QLineEdit();
    indexEdit->setPlaceholderText("Введите индекс товара");

    // Количество
    QLineEdit *quantityEdit = new QLineEdit();
    quantityEdit->setPlaceholderText("Введите количество");
    quantityEdit->setValidator(new QIntValidator(1, 10000, this));

    // Секция размещения
    QComboBox *sectionCombo = new QComboBox();
    sectionCombo->setEnabled(false); // Изначально недоступно

    // Ячейка размещения
    QLineEdit *cellEdit = new QLineEdit();
    cellEdit->setPlaceholderText("Введите номер ячейки");
    cellEdit->setEnabled(false); // Изначально недоступно
    cellEdit->setValidator(new QIntValidator(1, 60, this));

    // Кнопка удаления
    QPushButton *removeButton = new QPushButton("Удалить");
    removeButton->setFixedSize(80, 50);
    removeButton->setObjectName("removeButton");

    // Добавляем поля в форму
    formLayout->addRow("Тип товара:*", typeCombo);
    formLayout->addRow("Название товара:*", nameEdit);
    formLayout->addRow("Индекс товара:*", indexEdit);
    formLayout->addRow("Количество:*", quantityEdit);
    formLayout->addRow("Секция размещения:*", sectionCombo);
    formLayout->addRow("Ячейка размещения:*", cellEdit);
    formLayout->addRow("", removeButton);

    // Сохраняем виджет в список
    productWidgets.append(productWidget);
    productsLayout->addWidget(productWidget);

    // Подключаем сигналы
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DeliveryWindow::onProductTypeChanged);
    connect(removeButton, &QPushButton::clicked, this, [this, productWidget]() {
        productsLayout->removeWidget(productWidget);
        productWidgets.removeOne(productWidget);
        productWidget->deleteLater();
    });

    // Обновляем секции для текущего типа
    // Обновляем секции для текущего типа и активируем поле ячейки
    updateSectionComboBox(sectionCombo, typeCombo->currentText());
    sectionCombo->setEnabled(true);

    // Активируем поле ячейки если секция уже выбрана
    if (sectionCombo->currentIndex() >= 0) {
        cellEdit->setEnabled(true);
    }
}

void DeliveryWindow::onProductTypeChanged(int index)
{
    QComboBox *typeCombo = qobject_cast<QComboBox*>(sender());
    if (!typeCombo) return;

    // Находим родительский виджет и комбобокс секции
    QWidget *productWidget = typeCombo->parentWidget();
    if (!productWidget) return;

    QComboBox *sectionCombo = productWidget->findChild<QComboBox*>();
    QLineEdit *cellEdit = nullptr;

    // Ищем второй комбобокс (секцию) и поле ячейки
    CustomList<QComboBox*> combos;
    combos.fromQList(productWidget->findChildren<QComboBox*>());
    if (combos.size() > 1) {
        sectionCombo = combos[1];
    }

    CustomList<QLineEdit*> edits = productWidget->findChildren<QLineEdit*>();
    if (edits.size() > 4) { // Пятое поле - ячейка
        cellEdit = edits[4];
    }

    if (sectionCombo) {
        updateSectionComboBox(sectionCombo, typeCombo->currentText());
        sectionCombo->setEnabled(true);

        if (cellEdit) {
            cellEdit->setEnabled(sectionCombo->currentIndex() >= 0);
        }
    }
}

void DeliveryWindow::updateSectionComboBox(QComboBox* sectionCombo, const QString& productType)
{
    sectionCombo->clear();

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

    for (int i = startSection; i <= endSection; i++) {
        sectionCombo->addItem(QString("Секция %1").arg(i), i);
    }

    // Автоматически выбираем первую доступную секцию и активируем поле ячейки
    if (sectionCombo->count() > 0) {
        sectionCombo->setCurrentIndex(0);

        // Находим и активируем поле ячейки
        QWidget *productWidget = sectionCombo->parentWidget();
        if (productWidget) {
            CustomList<QLineEdit*> edits = productWidget->findChildren<QLineEdit*>();
            if (edits.size() > 4) { // Пятое поле - ячейка
                QLineEdit *cellEdit = edits[4];
                cellEdit->setEnabled(true);
            }
        }
    }

    // Подключаем сигнал изменения секции для активации поля ячейки
    connect(sectionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this, sectionCombo]() {
        QWidget *productWidget = sectionCombo->parentWidget();
        if (!productWidget) return;

        CustomList<QLineEdit*> edits = productWidget->findChildren<QLineEdit*>();
        if (edits.size() > 4) { // Пятое поле - ячейка
            QLineEdit *cellEdit = edits[4];
            cellEdit->setEnabled(sectionCombo->currentIndex() >= 0);
        }
    });
}

void DeliveryWindow::onAddProductClicked()
{
    addProductFields();
}

void DeliveryWindow::onRemoveProductClicked()
{
    // Уже реализовано через лямбду в addProductFields()
}

void DeliveryWindow::onCompleteDeliveryClicked()
{
    TRY_CATCH_BEGIN
        // Валидация основных полей
        if (supplierEdit->text().trimmed().isEmpty()) {
        THROW_VALIDATION_ERROR("Поставщик", "не может быть пустым",
                               "Заполните поле 'Поставщик'");
    }

    // Валидация товаров
    if (productWidgets.isEmpty()) {
        THROW_VALIDATION_ERROR("товары", "отсутствуют",
                               "Добавьте хотя бы один товар");
    }

    for (QWidget *productWidget : productWidgets) {
        CustomList<QLineEdit*> edits = productWidget->findChildren<QLineEdit*>();
        CustomList<QComboBox*> combos;
        combos.fromQList(productWidget->findChildren<QComboBox*>());

        // Проверяем наличие всех необходимых элементов
        if (edits.size() < 4 || combos.size() < 2) {
            THROW_VALIDATION_ERROR("данные товара", "неполные",
                                   "Некорректные данные товара");
        }

        // Безопасная проверка полей
        bool hasEmptyFields = false;
        QStringList emptyFields;

        // Проверяем текстовые поля
        for (int i = 0; i < 3; ++i) { // Первые 3 поля: название, индекс, количество
            if (i < edits.size() && edits[i]->text().trimmed().isEmpty()) {
                hasEmptyFields = true;
                if (i == 0) emptyFields << "Название товара";
                else if (i == 1) emptyFields << "Индекс товара";
                else if (i == 2) emptyFields << "Количество";
            }
        }

        // Проверяем комбобоксы
        if (combos[0]->currentIndex() == -1) {
            hasEmptyFields = true;
            emptyFields << "Тип товара";
        }
        if (combos[1]->currentIndex() == -1) {
            hasEmptyFields = true;
            emptyFields << "Секция размещения";
        }

        // Проверяем поле ячейки (4-е поле)
        if (edits.size() > 3 && edits[3]->text().trimmed().isEmpty()) {
            hasEmptyFields = true;
            emptyFields << "Ячейка размещения";
        }

        if (hasEmptyFields) {
            THROW_VALIDATION_ERROR("поля товара", "не все заполнены",
                                   QString("Заполните все обязательные поля для товара.\n"
                                           "Не заполнены: %1").arg(emptyFields.join(", ")));
        }

        // Проверка числовых полей
        bool ok;
        int quantity = edits[2]->text().toInt(&ok);
        if (!ok || quantity <= 0) {
            THROW_VALIDATION_ERROR("количество", "должно быть положительным числом",
                                   "Количество товара должно быть положительным числом");
        }

        if (edits.size() > 3) {
            int cellNumber = edits[3]->text().toInt(&ok);
            if (!ok || cellNumber < 1 || cellNumber > 60) {
                THROW_VALIDATION_ERROR("номер ячейки", "должен быть от 1 до 60",
                                       "Номер ячейки должен быть числом от 1 до 60");
            }
        }
    }

    // Сохраняем данные (внутри saveDeliveryToFiles будет проверка ячеек)
    saveDeliveryToFiles();
    // Не вызываем onBackClicked() здесь - он вызывается внутри saveDeliveryToFiles() при успехе
    TRY_CATCH_END
}

bool DeliveryWindow::validateAndPrepareDelivery()
{
    QString supplier = supplierEdit->text().trimmed();

    // Собираем информацию о всех товарах для проверки
    CustomList<QPair<int, int>> occupiedCells; // секция, ячейка
    CustomList<QPair<int, int>> conflictingCells; // секция, ячейка

    for (QWidget *productWidget : productWidgets) {
        // Безопасное получение полей
        QComboBox *typeCombo = nullptr;
        QLineEdit *nameEdit = nullptr;
        QLineEdit *indexEdit = nullptr;
        QLineEdit *quantityEdit = nullptr;
        QComboBox *sectionCombo = nullptr;
        QLineEdit *cellEdit = nullptr;

        CustomList<QComboBox*> combos;
        combos.fromQList(productWidget->findChildren<QComboBox*>());
        CustomList<QLineEdit*> edits = productWidget->findChildren<QLineEdit*>();

        if (combos.size() >= 2) {
            typeCombo = combos[0];
            sectionCombo = combos[1];
        }

        for (QLineEdit *edit : edits) {
            QString placeholder = edit->placeholderText().toLower();
            if (placeholder.contains("название")) {
                nameEdit = edit;
            } else if (placeholder.contains("индекс")) {
                indexEdit = edit;
            } else if (placeholder.contains("количество")) {
                quantityEdit = edit;
            } else if (placeholder.contains("номер ячейки")) {
                cellEdit = edit;
            }
        }

        if (!nameEdit && edits.size() > 0) nameEdit = edits[0];
        if (!indexEdit && edits.size() > 1) indexEdit = edits[1];
        if (!quantityEdit && edits.size() > 2) quantityEdit = edits[2];
        if (!cellEdit && edits.size() > 3) cellEdit = edits[3];

        if (!typeCombo || !nameEdit || !indexEdit || !quantityEdit || !sectionCombo || !cellEdit) {
            continue;
        }

        int sectionNumber = sectionCombo->currentData().toInt();
        int cellNumber = cellEdit->text().toInt();

        // Проверяем занятость ячейки
        QString productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
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
                    if (existingProduct.getName() != nameEdit->text().trimmed() ||
                        existingProduct.getIndex() != indexEdit->text().trimmed() ||
                        existingProduct.getSupplier() != supplier) {
                        // Данные не совпадают - конфликт
                        conflictingCells.append(qMakePair(sectionNumber, cellNumber));
                    } else {
                        // Данные совпадают - можно обновить количество
                        occupiedCells.append(qMakePair(sectionNumber, cellNumber));
                    }
                    break;
                }
            }
            pFile.close();
        }
    }

    // Если есть конфликтные ячейки - отменяем сохранение
    if (!conflictingCells.isEmpty()) {
        QString errorMessage = "Следующие ячейки уже заняты другими товарами:\n";
        for (const auto& cell : conflictingCells) {
            errorMessage += QString("• Секция %1, Ячейка %2\n").arg(cell.first).arg(cell.second);
        }
        errorMessage += "\nИзмените ячейки размещения или данные товаров.";
        QMessageBox::critical(this, "Ошибка", errorMessage);
        return false;
    }

    return true;
}

void DeliveryWindow::saveDeliveryToFiles()
{
    TRY_CATCH_BEGIN
        QString supplier = supplierEdit->text().trimmed();
    QDateTime deliveryDate = dateEdit->dateTime();
    if (!dateEdit->date().isValid()) {
        deliveryDate = QDateTime::currentDateTime();
    }

    // Сначала проверяем все ячейки на конфликты
    if (!validateAndPrepareDelivery()) {
        return; // Прерываем сохранение при конфликтах
    }

    // Если проверка пройдена - сохраняем данные
    for (QWidget *productWidget : productWidgets) {
        // Безопасное получение полей
        QComboBox *typeCombo = nullptr;
        QLineEdit *nameEdit = nullptr;
        QLineEdit *indexEdit = nullptr;
        QLineEdit *quantityEdit = nullptr;
        QComboBox *sectionCombo = nullptr;
        QLineEdit *cellEdit = nullptr;

        CustomList<QComboBox*> combos;
        combos.fromQList(productWidget->findChildren<QComboBox*>());
        CustomList<QLineEdit*> edits = productWidget->findChildren<QLineEdit*>();

        if (combos.size() >= 2) {
            typeCombo = combos[0];
            sectionCombo = combos[1];
        }

        for (QLineEdit *edit : edits) {
            QString placeholder = edit->placeholderText().toLower();
            if (placeholder.contains("название")) {
                nameEdit = edit;
            } else if (placeholder.contains("индекс")) {
                indexEdit = edit;
            } else if (placeholder.contains("количество")) {
                quantityEdit = edit;
            } else if (placeholder.contains("номер ячейки")) {
                cellEdit = edit;
            }
        }

        if (!nameEdit && edits.size() > 0) nameEdit = edits[0];
        if (!indexEdit && edits.size() > 1) indexEdit = edits[1];
        if (!quantityEdit && edits.size() > 2) quantityEdit = edits[2];
        if (!cellEdit && edits.size() > 3) cellEdit = edits[3];

        if (!typeCombo || !nameEdit || !indexEdit || !quantityEdit || !sectionCombo || !cellEdit) {
            qDebug() << "Не удалось найти все поля для товара";
            continue;
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
        QDataStream outHistory(&hFile);  // ИЗМЕНЕНО: out -> outHistory
        outHistory << static_cast<quint32>(operations.size());
        for (const Operation& op : operations) {
            outHistory << op;
        }
        hFile.close();

        // Создаем/обновляем товар в секции
        QString productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
        QDir sectionsDir("sections");
        if (!sectionsDir.exists() && !sectionsDir.mkpath(".")) {
            THROW_FILE_ERROR("sections", "создания директории",
                             "Не удалось создать директорию для секций");
        }

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

        // Проверяем, есть ли уже товар в этой ячейке
        bool productExists = false;
        for (int i = 0; i < products.size(); ++i) {
            if (products[i].getCellNumber() == cellNumber) {
                // Обновляем существующий товар (данные уже проверены)
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
            THROW_FILE_ERROR(productsFile, "открытия для записи",
                             "Не удалось сохранить список товаров");
        }
        QDataStream outProducts(&pFile);  // ИЗМЕНЕНО: out -> outProducts
        outProducts << static_cast<quint32>(products.size());
        for (const Product& prod : products) {
            outProducts << prod;
        }
        pFile.close();

        qDebug() << "Сохранен товар:" << productName << "в секцию" << sectionNumber << "ячейка" << cellNumber;
    }

    // Используем ExceptionHandler для показа информационного сообщения
    ExceptionHandler::showMessageBox(ErrorSeverity::INFO, "Успех",
                                     "Поставка успешно оформлена!", this);

    onBackClicked();
    TRY_CATCH_END
}

void DeliveryWindow::onBackClicked()
{
    TRY_CATCH_BEGIN
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
