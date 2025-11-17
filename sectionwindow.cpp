#include "sectionwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QContextMenuEvent>
#include "mainwindow.h"
#include "warehousewindow.h"
#include "operationshistorywindow.h"
#include "shipmentformwindow.h"
#include "transferformwindow.h"

SectionWindow::SectionWindow(int sectionNumber, const QString& materialType,
                             bool isAdmin, const QString& mode, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_sectionNumber(sectionNumber), m_materialType(materialType),
    m_isAdmin(isAdmin), m_mode(mode), m_userManager(userManager)
{
    m_productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
    m_historyFile = QString("operations_history/section_history_%1.bin").arg(sectionNumber);

    setupUI();
    applyStyle();

    if (m_mode == "shipment") {
        setWindowTitle(QString("Отгрузка - Секция %1 - %2").arg(sectionNumber).arg(materialType));
    } else if (m_mode == "transfer") {
        setWindowTitle(QString("Трансфер - Секция %1 - %2").arg(sectionNumber).arg(materialType));
    } else {
        setWindowTitle(QString("Секция %1 - %2").arg(sectionNumber).arg(materialType));
    }

    setFixedSize(1000, 700);

    loadProducts();
    updateTable();
    updateOccupancyLabel();

    if (m_isAdmin && m_mode == "view") {
        setupContextMenu();
    }
}

void SectionWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    QWidget *titleWidget = new QWidget();
    QVBoxLayout *titleLayout = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(2);

    titleLabel = new QLabel(QString("СЕКЦИЯ №%1 - %2").arg(m_sectionNumber).arg(m_materialType));
    titleLabel->setAlignment(Qt::AlignCenter);

    occupancyLabel = new QLabel();
    occupancyLabel->setAlignment(Qt::AlignCenter);
    occupancyLabel->setMinimumHeight(20);

    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(occupancyLabel);

    QPushButton *operationsHistoryButton = new QPushButton("История операций");
    operationsHistoryButton->setFixedSize(180, 35);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleWidget);
    topLayout->addStretch();
    topLayout->addWidget(operationsHistoryButton);

    QWidget *searchPanel = new QWidget();
    QHBoxLayout *searchLayout = new QHBoxLayout(searchPanel);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(10);

    QLabel *searchLabel = new QLabel("Поиск:");
    searchLabel->setFixedSize(50, 30);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Введите текст для поиска...");
    searchEdit->setFixedHeight(30);

    searchComboBox = new QComboBox();
    searchComboBox->addItem("Номер ячейки");
    searchComboBox->addItem("Название товара");
    searchComboBox->addItem("Индекс товара");
    searchComboBox->addItem("Поставщик");
    searchComboBox->setFixedHeight(30);
    searchComboBox->setFixedWidth(150);

    searchLayout->addStretch();
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchComboBox);

    productsTable = new QTableWidget();
    productsTable->setColumnCount(5);
    productsTable->setHorizontalHeaderLabels({"Номер ячейки", "Название товара", "Индекс товара", "Количество", "Поставщик"});

    productsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    productsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    productsTable->setAlternatingRowColors(true);
    productsTable->horizontalHeader()->setStretchLastSection(true);
    productsTable->verticalHeader()->setVisible(false);
    productsTable->setSortingEnabled(true);
    productsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    productsTable->setColumnWidth(0, 120);
    productsTable->setColumnWidth(1, 200);
    productsTable->setColumnWidth(2, 150);
    productsTable->setColumnWidth(3, 100);

    productsTable->setFixedHeight(350);
    productsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(searchPanel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(productsTable);

    connect(backButton, &QPushButton::clicked, this, &SectionWindow::onBackClicked);
    connect(operationsHistoryButton, &QPushButton::clicked, this, &SectionWindow::onOperationsHistoryClicked);
    connect(searchEdit, &QLineEdit::textChanged, this, &SectionWindow::onSearchTextChanged);
    connect(searchComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &SectionWindow::onSearchCriteriaChanged);

    if (m_mode == "shipment") {
        connect(productsTable, &QTableWidget::cellDoubleClicked, this, &SectionWindow::onShipmentClicked);
    } else if (m_mode == "transfer") {
        connect(productsTable, &QTableWidget::cellDoubleClicked, this, &SectionWindow::onTransferClicked);
    } else if (m_isAdmin) {
        connect(productsTable, &QTableWidget::cellDoubleClicked, this, &SectionWindow::onCellDoubleClicked);
    }

    if (m_isAdmin && m_mode == "view") {
        QLabel *editHintLabel = new QLabel("💡 Двойной клик по ячейке для редактирования");
        editHintLabel->setAlignment(Qt::AlignCenter);
        editHintLabel->setStyleSheet("color: #7f8c8d; font-size: 12px; font-style: italic; padding: 5px;");
        mainLayout->insertWidget(3, editHintLabel);
    }

    if (m_mode == "shipment") {
        QLabel *modeHintLabel = new QLabel("💡 Двойной клик по товару для оформления отгрузки");
        modeHintLabel->setAlignment(Qt::AlignCenter);
        modeHintLabel->setStyleSheet("color: #e74c3c; font-size: 12px; font-weight: bold; padding: 5px;");
        mainLayout->insertWidget(3, modeHintLabel);
    } else if (m_mode == "transfer") {
        QLabel *modeHintLabel = new QLabel("💡 Двойной клик по товару для оформления трансфера");
        modeHintLabel->setAlignment(Qt::AlignCenter);
        modeHintLabel->setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold; padding: 5px;");
        mainLayout->insertWidget(3, modeHintLabel);
    }
}

void SectionWindow::applyStyle()
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
            font-size: 24px;
            font-weight: bold;
            padding: 10px;
            margin: 5px;
        }
        QLabel[objectName="titleLabel"] {
            font-size: 24px;
            font-weight: bold;
            color: #2c3e50;
        }
        QLabel[objectName="occupancyLabel"] {
            font-size: 14px;
            color: #7f8c8d;
            font-style: italic;
            padding: 2px;
            margin: 2px;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 15px;
            padding: 8px 15px;
            font-size: 13px;
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
        QTableWidget {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 10px;
            gridline-color: #ecf0f1;
            font-size: 14px;
            color: #000000;
            alternate-background-color: #e8f4fd;
        }
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #ecf0f1;
            color: #000000;
        }
        QTableWidget::item:selected {
            background-color: #3498db;
            color: white;
        }
        QHeaderView::section {
            background-color: #34495e;
            color: white;
            padding: 10px;
            border: none;
            font-weight: bold;
        }
        QLineEdit {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 5px 10px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
        }
        QLineEdit:focus {
            border-color: #3498db;
        }
        QComboBox {
            background-color: white;
            border: 2px solid #bdc3c7;
            border-radius: 8px;
            padding: 5px 10px;
            font-size: 14px;
            min-height: 20px;
            color: #000000;
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
        QTableWidget::item:editable {
            background-color: #ffffe0;
        }
        QTableWidget::item:editable:focus {
            background-color: #fffacd;
            border: 2px solid #3498db;
        }
        QMenu {
            background-color: white;
            border: 1px solid #bdc3c7;
            border-radius: 5px;
            padding: 5px;
        }
        QMenu::item {
            padding: 5px 15px;
            color: #000000;
            font-size: 14px;
        }
        QMenu::item:selected {
            background-color: #3498db;
            color: white;
        }
    )");

    backButton->setObjectName("backButton");
    titleLabel->setObjectName("titleLabel");
    occupancyLabel->setObjectName("occupancyLabel");
}

void SectionWindow::updateOccupancyLabel()
{
    int occupied = m_allProducts.size();
    QString occupancyText = QString("Заполнено: %1/%2 ячеек").arg(occupied).arg(MAX_CELLS);

    double percentage = (double)occupied / MAX_CELLS * 100;
    if (percentage >= 90) {
        occupancyText += " 🟥";
    } else if (percentage >= 70) {
        occupancyText += " 🟨";
    } else {
        occupancyText += " 🟩";
    }

    occupancyLabel->setText(occupancyText);
}

void SectionWindow::loadProducts()
{
    QDir().mkpath("sections");
    QDir().mkpath("operations_history");

    m_products.clear();
    QFile file(m_productsFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Product product;
            in >> product;
            m_products.append(product);
        }
        file.close();
    } else {
        if (m_sectionNumber == 1 && m_products.isEmpty()) {
            createInitialTestData();
        }
    }
    m_allProducts = m_products;
    updateOccupancyLabel();
}

void SectionWindow::saveProducts()
{
    QFile file(m_productsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << static_cast<quint32>(m_products.size());
        for (const Product& product : m_products) {
            out << product;
        }
        file.close();
    }
}

void SectionWindow::loadOperationsHistory()
{
    QFile file(m_historyFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Operation operation;
            in >> operation;
            m_operationsHistory.append(operation);
        }
        file.close();
    }
}

void SectionWindow::saveOperationsHistory()
{
    QFile file(m_historyFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << static_cast<quint32>(m_operationsHistory.size());
        for (const Operation& operation : m_operationsHistory) {
            out << operation;
        }
        file.close();
    }
}

void SectionWindow::updateTable()
{
    productsTable->setRowCount(m_products.size());

    for (int i = 0; i < m_products.size(); ++i) {
        const Product& product = m_products.at(i);

        QTableWidgetItem *cellItem = new QTableWidgetItem();
        cellItem->setData(Qt::DisplayRole, product.getCellNumber());

        QTableWidgetItem *nameItem = new QTableWidgetItem(product.getName());

        QTableWidgetItem *indexItem = new QTableWidgetItem(product.getIndex());

        QTableWidgetItem *quantityItem = new QTableWidgetItem();
        quantityItem->setData(Qt::DisplayRole, product.getQuantity());

        QTableWidgetItem *supplierItem = new QTableWidgetItem(product.getSupplier());

        productsTable->setItem(i, 0, cellItem);
        productsTable->setItem(i, 1, nameItem);
        productsTable->setItem(i, 2, indexItem);
        productsTable->setItem(i, 3, quantityItem);
        productsTable->setItem(i, 4, supplierItem);
    }
}

int SectionWindow::countOccupiedCells() const
{
    return m_allProducts.size();
}

void SectionWindow::onSearchTextChanged(const QString& text)
{
    filterTable(text, searchComboBox->currentIndex());
}

void SectionWindow::onSearchCriteriaChanged(int index)
{
    filterTable(searchEdit->text(), index);
}

void SectionWindow::filterTable(const QString& searchText, int searchCriteria)
{
    if (searchText.isEmpty()) {
        m_products = m_allProducts;
    } else {
        m_products.clear();
        for (const Product& product : m_allProducts) {
            bool match = false;
            QString searchLower = searchText.toLower();

            switch (searchCriteria) {
            case 0:
                match = QString::number(product.getCellNumber()).contains(searchText);
                break;
            case 1:
                match = product.getName().toLower().contains(searchLower);
                break;
            case 2:
                match = product.getIndex().toLower().contains(searchLower);
                break;
            case 3:
                match = product.getSupplier().toLower().contains(searchLower);
                break;
            }

            if (match) {
                m_products.append(product);
            }
        }
    }

    updateTable();
}

void SectionWindow::onBackClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, m_mode, m_userManager);
    warehouseWindow->show();
    this->close();
}

void SectionWindow::onOperationsHistoryClicked()
{
    OperationsHistoryWindow *historyWindow = new OperationsHistoryWindow(m_sectionNumber, m_materialType, m_isAdmin, m_userManager);
    historyWindow->show();
    this->close();
}

void SectionWindow::createInitialTestData()
{
    m_products.clear();
    m_operationsHistory.clear();

    if (m_sectionNumber == 1) {
        m_products.append(Product("Доска сосновая", "WOOD-001", 150, "Лесопилка 'Северная'", 1));
        m_products.append(Product("Брус дубовый", "WOOD-002", 80, "Мебельная фабрика 'Дубок'", 2));
        m_products.append(Product("Фанера березовая", "WOOD-003", 200, "Комбинат 'Тайга'", 3));
        m_products.append(Product("Вагонка лиственница", "WOOD-004", 120, "Лесхоз 'Сибирский'", 4));
        m_products.append(Product("Брусок сосновый", "WOOD-005", 300, "Лесопилка 'Северная'", 5));
        m_products.append(Product("Доска еловая", "WOOD-006", 180, "Лесхоз 'Сибирский'", 6));
        m_products.append(Product("Паркетная доска", "WOOD-007", 95, "Мебельная фабрика 'Дубок'", 7));
        m_products.append(Product("ОСП плита", "WOOD-008", 250, "Комбинат 'Тайга'", 8));
        m_products.append(Product("ДВП плита", "WOOD-009", 175, "Лесопилка 'Северная'", 9));
        m_products.append(Product("МДФ панель", "WOOD-010", 140, "Мебельная фабрика 'Дубок'", 10));

        m_operationsHistory.append(Operation("Доска сосновая", "WOOD-001", 150,
                                             "Лесопилка 'Северная'", "Ячейка №1", Operation::DELIVERY,
                                             QDate::currentDate().addDays(-10)));
        m_operationsHistory.append(Operation("Брус дубовый", "WOOD-002", 80,
                                             "Мебельная фабрика 'Дубок'", "Ячейка №2", Operation::DELIVERY,
                                             QDate::currentDate().addDays(-5)));
        m_operationsHistory.append(Operation("Фанера березовая", "WOOD-003", 50,
                                             "Ячейка №3", "Ячейка №15", Operation::TRANSFER,
                                             QDate::currentDate().addDays(-3)));
        m_operationsHistory.append(Operation("Паркетная доска", "WOOD-007", 30,
                                             "Ячейка №7", "Строительная фирма 'Дом'", Operation::SHIPMENT,
                                             QDate::currentDate().addDays(-1)));
    }

    m_allProducts = m_products;
    saveProducts();
    saveOperationsHistory();
    updateTable();
    updateOccupancyLabel();
}

void SectionWindow::onCellDoubleClicked(int row, int column)
{
    if (!m_isAdmin || column == 0 || m_mode != "view") {
        return;
    }

    QTableWidgetItem* item = productsTable->item(row, column);
    if (!item) return;

    QString currentValue = item->text();

    bool ok;
    QString newValue;

    QString columnName;
    switch(column) {
    case 1: columnName = "Название товара"; break;
    case 2: columnName = "Индекс товара"; break;
    case 3: columnName = "Количество"; break;
    case 4: columnName = "Поставщик"; break;
    default: return;
    }

    if (column == 3) {
        int intValue = QInputDialog::getInt(this, "Редактирование",
                                            QString("Введите новое значение для '%1':").arg(columnName),
                                            currentValue.toInt(), 0, 10000, 1, &ok);
        if (ok) {
            newValue = QString::number(intValue);
        }
    } else {
        newValue = QInputDialog::getText(this, "Редактирование",
                                         QString("Введите новое значение для '%1':").arg(columnName),
                                         QLineEdit::Normal, currentValue, &ok);
    }

    if (ok && !newValue.isEmpty()) {
        if (validateCellEdit(row, column, newValue)) {
            updateProductData(row, column, newValue);
            saveProducts();
            QMessageBox::information(this, "Успех", "Данные успешно обновлены!");
        }
    }
}

bool SectionWindow::validateCellEdit(int row, int column, const QString& newValue)
{
    if (row < 0 || row >= m_products.size()) {
        QMessageBox::warning(this, "Ошибка", "Неверный номер строки");
        return false;
    }

    switch(column) {
    case 1:
        if (newValue.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Название товара не может быть пустым");
            return false;
        }
        break;

    case 2:
        if (newValue.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Индекс товара не может быть пустым");
            return false;
        }
        for (int i = 0; i < m_products.size(); ++i) {
            if (i != row && m_products[i].getIndex() == newValue) {
                QMessageBox::warning(this, "Ошибка", "Товар с таким индексом уже существует");
                return false;
            }
        }
        break;

    case 3:
    {
        bool ok;
        int quantity = newValue.toInt(&ok);
        if (!ok || quantity < 0) {
            QMessageBox::warning(this, "Ошибка", "Количество должно быть неотрицательным числом");
            return false;
        }
    }
    break;

    case 4:
        if (newValue.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Поставщик не может быть пустым");
            return false;
        }
        break;

    default:
        return false;
    }

    return true;
}

void SectionWindow::updateProductData(int row, int column, const QString& newValue)
{
    Product& product = m_products[row];

    switch(column) {
    case 1:
        product.setName(newValue);
        break;
    case 2:
        product.setIndex(newValue);
        break;
    case 3:
        product.setQuantity(newValue.toInt());
        break;
    case 4:
        product.setSupplier(newValue);
        break;
    }

    for (int i = 0; i < m_allProducts.size(); ++i) {
        if (m_allProducts[i].getCellNumber() == product.getCellNumber()) {
            m_allProducts[i] = product;
            break;
        }
    }

    updateTable();
}

void SectionWindow::setupContextMenu()
{
    contextMenu = new QMenu(this);
    QAction *deleteAction = new QAction("Удалить товар", this);
    connect(deleteAction, &QAction::triggered, this, &SectionWindow::onDeleteProduct);
    contextMenu->addAction(deleteAction);

    productsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(productsTable, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        if (!m_isAdmin || m_mode != "view") return;

        QTableWidgetItem *item = productsTable->itemAt(pos);
        if (item) {
            selectedRow = item->row();
            contextMenu->exec(productsTable->viewport()->mapToGlobal(pos));
        }
    });
}

void SectionWindow::onDeleteProduct()
{
    if (selectedRow < 0 || selectedRow >= m_products.size()) {
        return;
    }

    const Product& product = m_products.at(selectedRow);
    QString productInfo = QString("Ячейка: %1\nНазвание: %2\nИндекс: %3\nКоличество: %4\nПоставщик: %5")
                              .arg(product.getCellNumber())
                              .arg(product.getName())
                              .arg(product.getIndex())
                              .arg(product.getQuantity())
                              .arg(product.getSupplier());

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить этот товар?\n\n%1").arg(productInfo),
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        int cellNumber = m_products[selectedRow].getCellNumber();
        m_products.removeAt(selectedRow);

        for (int i = 0; i < m_allProducts.size(); ++i) {
            if (m_allProducts[i].getCellNumber() == cellNumber) {
                m_allProducts.removeAt(i);
                break;
            }
        }

        saveProducts();
        updateTable();
        updateOccupancyLabel();

        QMessageBox::information(this, "Успех", "Товар успешно удален!");
    }

    selectedRow = -1;
}

void SectionWindow::onShipmentClicked(int row, int column)
{
    if (row < 0 || row >= m_products.size()) {
        return;
    }

    const Product& product = m_products.at(row);

    ShipmentFormWindow *shipmentForm = new ShipmentFormWindow(product, m_sectionNumber, m_isAdmin, m_userManager);
    shipmentForm->show();
    this->close();
}

void SectionWindow::onTransferClicked(int row, int column)
{
    if (row < 0 || row >= m_products.size()) {
        return;
    }

    const Product& product = m_products.at(row);

    // Исправляем m_sourceSectionNumber на m_sectionNumber
    TransferFormWindow *transferForm = new TransferFormWindow(product, m_sectionNumber,
                                                              m_materialType, m_isAdmin, m_userManager);
    transferForm->show();
    this->close();
}
