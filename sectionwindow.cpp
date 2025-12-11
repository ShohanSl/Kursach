#include "sectionwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QKeyEvent>
#include "mainwindow.h"
#include "warehousewindow.h"
#include "operationshistorywindow.h"
#include "shipmentformwindow.h"
#include "transferformwindow.h"

SectionWindow::SectionWindow(int sectionNumber, const QString& materialType,
                             bool isAdmin, const QString& mode, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_sectionNumber(sectionNumber), m_materialType(materialType),
    m_isAdmin(isAdmin), m_mode(mode), m_userManager(userManager), selectedRow(-1)
{
    m_productsFile = QString("sections/section_%1.bin").arg(sectionNumber);
    m_historyFile = QString("operations_history/section_history_%1.bin").arg(sectionNumber);

    setupUI();

    if (m_mode == "shipment") {
        setWindowTitle(QString("Отгрузка - Секция %1 - %2").arg(sectionNumber).arg(materialType));
    } else if (m_mode == "transfer") {
        setWindowTitle(QString("Трансфер - Секция %1 - %2").arg(sectionNumber).arg(materialType));
    } else {
        setWindowTitle(QString("Секция %1 - %2").arg(sectionNumber).arg(materialType));
    }

    setFixedSize(1000, 600);

    loadProducts();
    updateTable();

    if (m_isAdmin && m_mode == "view") {
        setupContextMenu();
    }
    // Инициализируем историю удалений
    m_deletionHistory = DeletionHistory::instance();
    // Загружаем историю из файла
    m_deletionHistory->loadFromFile("deletion_history.bin");
}

void SectionWindow::setupUI()
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

    QLabel *titleLabel = new QLabel(QString("СЕКЦИЯ №%1 - %2").arg(m_sectionNumber).arg(m_materialType));
    titleLabel->setAlignment(Qt::AlignCenter);

    QPushButton *operationsHistoryButton = new QPushButton("История операций");
    operationsHistoryButton->setFixedSize(150, 30);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(operationsHistoryButton);

    // Панель поиска
    QWidget *searchPanel = new QWidget();
    QHBoxLayout *searchLayout = new QHBoxLayout(searchPanel);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(10);

    QLabel *searchLabel = new QLabel("Поиск:");
    searchLabel->setFixedSize(40, 25);

    searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("Введите текст для поиска...");
    searchEdit->setFixedHeight(25);

    searchComboBox = new QComboBox();
    searchComboBox->addItem("Номер ячейки");
    searchComboBox->addItem("Название товара");
    searchComboBox->addItem("Индекс товара");
    searchComboBox->addItem("Поставщик");
    searchComboBox->setFixedHeight(25);
    searchComboBox->setFixedWidth(140);

    searchLayout->addStretch();
    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchComboBox);
    searchLayout->addStretch();

    // Таблица товаров
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
    productsTable->setFixedHeight(400);

    // Добавление виджетов в главный макет
    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(searchPanel);
    mainLayout->addWidget(productsTable);

    // Подключение сигналов
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
    }
    // Убрана инициализация тестовыми данными

    m_allProducts = m_products;
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
    productsTable->setSortingEnabled(false);
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

    productsTable->setSortingEnabled(true);
    productsTable->sortByColumn(0, Qt::AscendingOrder);
}

void SectionWindow::onBackClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, m_mode, m_userManager);
    warehouseWindow->show();
    this->close();
}

void SectionWindow::onOperationsHistoryClicked()
{
    OperationsHistoryWindow *historyWindow = new OperationsHistoryWindow(m_sectionNumber, m_materialType, m_isAdmin, m_mode, m_userManager);
    historyWindow->show();
    this->close();
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

void SectionWindow::onCellDoubleClicked(int row, int column)
{
    if (!m_isAdmin || column == 0 || m_mode != "view") {
        return;
    }

    int dataIndex = getDataIndexFromVisualRow(row);
    if (dataIndex == -1) {
        QMessageBox::warning(this, "Ошибка", "Не удалось найти товар для редактирования");
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
        if (newValue.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Поле не может быть пустым");
            return;
        }

        if (column == 3) {
            bool conversionOk;
            int quantity = newValue.toInt(&conversionOk);
            if (!conversionOk || quantity < 0) {
                QMessageBox::warning(this, "Ошибка", "Количество должно быть неотрицательным числом");
                return;
            }
        }

        updateProductData(dataIndex, column, newValue);
        QMessageBox::information(this, "Успех", "Данные успешно обновлены!");
    }
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

    // Обновляем также в m_allProducts
    for (int i = 0; i < m_allProducts.size(); ++i) {
        if (m_allProducts[i].getCellNumber() == product.getCellNumber()) {
            m_allProducts[i] = product;
            break;
        }
    }

    saveProducts();
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
    if (selectedRow < 0 || selectedRow >= productsTable->rowCount()) {
        return;
    }

    int dataIndex = getDataIndexFromVisualRow(selectedRow);
    if (dataIndex == -1) {
        QMessageBox::warning(this, "Ошибка", "Не удалось найти товар для удаления");
        return;
    }

    const Product& product = m_products.at(dataIndex);
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
        // СОХРАНЯЕМ ТОВАР В ИСТОРИЮ УДАЛЕНИЙ
        m_deletionHistory->addDeletion(m_sectionNumber,
                                       product.getCellNumber(),
                                       product);

        int cellNumber = m_products[dataIndex].getCellNumber();
        m_products.removeAt(dataIndex);

        // Удаляем из m_allProducts по номеру ячейки
        for (int i = 0; i < m_allProducts.size(); ++i) {
            if (m_allProducts[i].getCellNumber() == cellNumber) {
                m_allProducts.removeAt(i);
                break;
            }
        }

        saveProducts();
        updateTable();

        // Показываем уведомление о возможности отмены
        if (m_deletionHistory->canUndo()) {
            QMessageBox::information(this, "Информация",
                                     "Товар удален. Для отмены нажмите Ctrl+Z.\n"
                                     "Доступно операций для отмены: " +
                                         QString::number(m_deletionHistory->historySize()));
        }
    }

    selectedRow = -1;
}

void SectionWindow::onShipmentClicked(int row, int column)
{
    int dataIndex = getDataIndexFromVisualRow(row);
    if (dataIndex == -1) {
        QMessageBox::warning(this, "Ошибка", "Не удалось найти товар для отгрузки");
        return;
    }

    const Product& product = m_products.at(dataIndex);
    ShipmentFormWindow *shipmentForm = new ShipmentFormWindow(product, m_sectionNumber, m_isAdmin, m_userManager);
    shipmentForm->show();
    this->close();
}

void SectionWindow::onTransferClicked(int row, int column)
{
    int dataIndex = getDataIndexFromVisualRow(row);
    if (dataIndex == -1) {
        QMessageBox::warning(this, "Ошибка", "Не удалось найти товар для трансфера");
        return;
    }

    const Product& product = m_products.at(dataIndex);
    TransferFormWindow *transferForm = new TransferFormWindow(product, m_sectionNumber,
                                                              m_materialType, m_isAdmin, m_userManager);
    transferForm->show();
    this->close();
}

int SectionWindow::getDataIndexFromVisualRow(int visualRow) const
{
    if (visualRow < 0 || visualRow >= productsTable->rowCount())
        return -1;

    QTableWidgetItem *cellItem = productsTable->item(visualRow, 0);
    if (!cellItem) return -1;

    int cellNumber = cellItem->text().toInt();

    for (int i = 0; i < m_products.size(); ++i) {
        if (m_products[i].getCellNumber() == cellNumber) {
            return i;
        }
    }

    return -1;
}

void SectionWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Z && event->modifiers() == Qt::ControlModifier) {
        // Пытаемся отменить последнее удаление
        undoLastDeletion();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

// Новый метод для отмены удаления
void SectionWindow::undoLastDeletion()
{
    if (!m_deletionHistory->canUndo()) {
        QMessageBox::information(this, "Отмена", "Нет операций для отмены");
        return;
    }

    int sectionNumber;
    int cellNumber;
    Product product;

    if (m_deletionHistory->undoLastDeletion(sectionNumber, cellNumber, product)) {
        // Проверяем, относится ли отменяемая операция к текущей секции
        if (sectionNumber != m_sectionNumber) {
            QMessageBox::warning(this, "Внимание",
                                 QString("Отменяемая операция относится к секции %1.\n"
                                         "Текущая секция: %2.\n"
                                         "Перейдите в нужную секцию для восстановления.")
                                     .arg(sectionNumber)
                                     .arg(m_sectionNumber));

            // Возвращаем запись в историю, так как отмена не удалась
            m_deletionHistory->addDeletion(sectionNumber, cellNumber, product);
            return;
        }

        // ✅ ИСПРАВЛЕНО: используем вызов метода вместо переменной
        if (isCellOccupied(cellNumber)) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Ячейка %1 занята другим товаром.\n"
                                         "Восстановление невозможно.")
                                     .arg(cellNumber));

            // Возвращаем запись в историю
            m_deletionHistory->addDeletion(sectionNumber, cellNumber, product);
            return;
        }

        // ✅ ИСПРАВЛЕНО: удаляем условие if(!cellOccupied) - оно уже проверено выше
        // Восстанавливаем товар
        m_products.append(product);
        m_allProducts.append(product);
        saveProducts();

        // Применяем текущий фильтр
        filterTable(searchEdit->text(), searchComboBox->currentIndex());
        updateTable();

        QMessageBox::information(this, "Восстановление",
                                 "Товар \"" + product.getName() +
                                     "\" восстановлен в ячейке " + QString::number(cellNumber));
    }
}

bool SectionWindow::isCellOccupied(int cellNumber) const
{
    for (int i = 0; i < m_allProducts.size(); ++i) {
        if (m_allProducts[i].getCellNumber() == cellNumber) {
            return true;
        }
    }
    return false;
}
