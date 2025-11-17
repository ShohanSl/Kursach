#include "operationshistorywindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDir>
#include <QFile>
#include <QDataStream>
#include <QInputDialog>
#include <QMessageBox>
#include <QDateEdit>
#include "sectionwindow.h"
#include <QDebug>

OperationsHistoryWindow::OperationsHistoryWindow(int sectionNumber, const QString& materialType,
                                                 bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_sectionNumber(sectionNumber), m_materialType(materialType),
    m_isAdmin(isAdmin), m_userManager(userManager)
{
    m_historyFile = QString("operations_history/section_history_%1.bin").arg(sectionNumber);

    setupUI();
    applyStyle();
    setWindowTitle(QString("История операций - Секция %1").arg(sectionNumber));
    setFixedSize(900, 600);

    loadOperationsHistory();
    updateTable();

    if (m_isAdmin) {
        setupContextMenu();
    }
}

void OperationsHistoryWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    // ===== ВЕРХНЯЯ ПАНЕЛЬ =====
    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    titleLabel = new QLabel(QString("ИСТОРИЯ ОПЕРАЦИЙ - СЕКЦИЯ №%1").arg(m_sectionNumber));
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // ===== ТАБЛИЦА ОПЕРАЦИЙ =====
    operationsTable = new QTableWidget();
    operationsTable->setColumnCount(4);
    operationsTable->setHorizontalHeaderLabels({"Дата", "Тип операции", "Откуда", "Куда"});

    // Настройка таблицы
    operationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    operationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    operationsTable->setAlternatingRowColors(true);
    operationsTable->horizontalHeader()->setStretchLastSection(true);
    operationsTable->verticalHeader()->setVisible(false);
    operationsTable->setSortingEnabled(true);
    operationsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Настраиваем ширину колонок
    operationsTable->setColumnWidth(0, 150);
    operationsTable->setColumnWidth(1, 150);
    operationsTable->setColumnWidth(2, 250);

    // Устанавливаем фиксированную высоту таблицы с возможностью прокрутки
    operationsTable->setFixedHeight(400);
    operationsTable->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    // Собираем все вместе
    mainLayout->addWidget(topPanel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(operationsTable);

    // Подключаем сигналы
    connect(backButton, &QPushButton::clicked, this, &OperationsHistoryWindow::onBackClicked);

    if (m_isAdmin) {
        connect(operationsTable, &QTableWidget::cellDoubleClicked, this, &OperationsHistoryWindow::onCellDoubleClicked);
    }
}

void OperationsHistoryWindow::applyStyle()
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
}

void OperationsHistoryWindow::loadOperationsHistory()
{
    QDir().mkpath("operations_history");

    m_operationsHistory.clear();
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

    qDebug() << "Loaded" << m_operationsHistory.size() << "operations";
}

void OperationsHistoryWindow::saveOperationsHistory()
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

void OperationsHistoryWindow::updateTable()
{
    operationsTable->setRowCount(m_operationsHistory.size());

    for (int i = 0; i < m_operationsHistory.size(); ++i) {
        const Operation& operation = m_operationsHistory.at(i);

        QString dateString = operation.getDate().toString("dd.MM.yyyy");
        QString operationType = operation.getTypeString();
        QString fromLocation = operation.getFromLocation();
        QString toLocation = operation.getToLocation();

        QTableWidgetItem *dateItem = new QTableWidgetItem(dateString);
        QTableWidgetItem *typeItem = new QTableWidgetItem(operationType);
        QTableWidgetItem *fromItem = new QTableWidgetItem(fromLocation);
        QTableWidgetItem *toItem = new QTableWidgetItem(toLocation);

        operationsTable->setItem(i, 0, dateItem);
        operationsTable->setItem(i, 1, typeItem);
        operationsTable->setItem(i, 2, fromItem);
        operationsTable->setItem(i, 3, toItem);

        dateItem->setData(Qt::UserRole, operation.getDate());
    }

    operationsTable->sortByColumn(0, Qt::DescendingOrder);
}

void OperationsHistoryWindow::onBackClicked()
{
    SectionWindow *sectionWindow = new SectionWindow(m_sectionNumber, m_materialType, m_isAdmin, "view", m_userManager);
    sectionWindow->show();
    this->close();
}

void OperationsHistoryWindow::onCellDoubleClicked(int row, int column)
{
    if (!m_isAdmin) return;

    QTableWidgetItem* item = operationsTable->item(row, column);
    if (!item) return;

    QString currentValue = item->text();
    bool ok;
    QString newValue;

    QString columnName;
    switch(column) {
    case 0: columnName = "Дата"; break;
    case 1: columnName = "Тип операции"; break;
    case 2: columnName = "Откуда"; break;
    case 3: columnName = "Куда"; break;
    default: return;
    }

    if (column == 0) {
        QDate currentDate = QDate::fromString(currentValue, "dd.MM.yyyy");
        if (!currentDate.isValid()) currentDate = QDate::currentDate();

        QDialog dialog(this);
        dialog.setWindowTitle("Редактирование даты");
        QVBoxLayout layout(&dialog);

        QDateEdit dateEdit(&dialog);
        dateEdit.setDate(currentDate);
        dateEdit.setCalendarPopup(true);
        dateEdit.setDisplayFormat("dd.MM.yyyy");

        QDialogButtonBox buttons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);

        layout.addWidget(new QLabel("Выберите новую дату:"));
        layout.addWidget(&dateEdit);
        layout.addWidget(&buttons);

        connect(&buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
        connect(&buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

        if (dialog.exec() == QDialog::Accepted) {
            newValue = dateEdit.date().toString("dd.MM.yyyy");
            ok = true;
        } else {
            ok = false;
        }
    } else if (column == 1) {
        QStringList types = {"Поставка", "Отгрузка", "Перемещение"};
        QString selected = QInputDialog::getItem(this, "Редактирование", "Выберите тип операции:",
                                                 types, types.indexOf(currentValue), false, &ok);
        if (ok) newValue = selected;
    } else {
        newValue = QInputDialog::getText(this, "Редактирование",
                                         QString("Введите новое значение для '%1':").arg(columnName),
                                         QLineEdit::Normal, currentValue, &ok);
    }

    if (ok && !newValue.isEmpty()) {
        if (validateCellEdit(row, column, newValue)) {
            updateOperationData(row, column, newValue);
            saveOperationsHistory();
            QMessageBox::information(this, "Успех", "Данные успешно обновлены!");
        }
    }
}

bool OperationsHistoryWindow::validateCellEdit(int row, int column, const QString& newValue)
{
    if (row < 0 || row >= m_operationsHistory.size()) return false;

    switch(column) {
    case 0: {
        QDate date = QDate::fromString(newValue, "dd.MM.yyyy");
        if (!date.isValid()) {
            QMessageBox::warning(this, "Ошибка", "Неверный формат даты. Используйте: dd.MM.yyyy");
            return false;
        }
        break;
    }
    case 1: {
        if (newValue != "Поставка" && newValue != "Отгрузка" && newValue != "Перемещение") {
            QMessageBox::warning(this, "Ошибка", "Неверный тип операции. Допустимо: Поставка, Отгрузка, Перемещение");
            return false;
        }
        break;
    }
    case 2: case 3: {
        if (newValue.trimmed().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Поле не может быть пустым");
            return false;
        }
        break;
    }
    }

    return true;
}

void OperationsHistoryWindow::updateOperationData(int row, int column, const QString& newValue)
{
    Operation& operation = m_operationsHistory[row];

    switch(column) {
    case 0: {
        QDate newDate = QDate::fromString(newValue, "dd.MM.yyyy");
        if (newDate.isValid()) {
            operation.setDate(newDate);
        }
        break;
    }
    case 1: {
        Operation::OperationType newType;
        if (newValue == "Поставка") newType = Operation::DELIVERY;
        else if (newValue == "Отгрузка") newType = Operation::SHIPMENT;
        else newType = Operation::TRANSFER;
        operation.setType(newType);
        break;
    }
    case 2:
        operation.setFromLocation(newValue);
        break;
    case 3:
        operation.setToLocation(newValue);
        break;
    }

    saveOperationsHistory();
    updateTable();
}

void OperationsHistoryWindow::setupContextMenu()
{
    contextMenu = new QMenu(this);
    QAction *deleteAction = new QAction("Удалить", this);
    connect(deleteAction, &QAction::triggered, this, &OperationsHistoryWindow::onDeleteOperation);
    contextMenu->addAction(deleteAction);

    operationsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(operationsTable, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        if (!m_isAdmin) return;
        QTableWidgetItem *item = operationsTable->itemAt(pos);
        if (item) {
            selectedRow = item->row();
            contextMenu->exec(operationsTable->viewport()->mapToGlobal(pos));
        }
    });
}

void OperationsHistoryWindow::onDeleteOperation()
{
    if (selectedRow < 0 || selectedRow >= m_operationsHistory.size()) return;

    const Operation& operation = m_operationsHistory.at(selectedRow);
    QString operationInfo = QString("Дата: %1\nТип: %2\nОткуда: %3\nКуда: %4")
                                .arg(operation.getDate().toString("dd.MM.yyyy"))
                                .arg(operation.getTypeString())
                                .arg(operation.getFromLocation())
                                .arg(operation.getToLocation());

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить эту операцию?\n\n%1").arg(operationInfo),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_operationsHistory.removeAt(selectedRow);
        saveOperationsHistory();
        updateTable();
        QMessageBox::information(this, "Успех", "Операция успешно удалена!");
    }

    selectedRow = -1;
}
