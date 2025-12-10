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
#include "exceptionhandler.h" // Добавляем заголовок

OperationsHistoryWindow::OperationsHistoryWindow(int sectionNumber, const QString& materialType,
                                                 bool isAdmin, const QString& mode, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_sectionNumber(sectionNumber), m_materialType(materialType),
    m_isAdmin(isAdmin), m_mode(mode), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
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
    TRY_CATCH_END
}

void OperationsHistoryWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет окна истории операций");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет окна истории операций");
    }
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(30, 20, 30, 20);

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

    titleLabel = new QLabel(QString("ИСТОРИЯ ОПЕРАЦИЙ - СЕКЦИЯ №%1").arg(m_sectionNumber));
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

    // ===== ТАБЛИЦА ОПЕРАЦИЙ =====
    operationsTable = new QTableWidget();
    if (!operationsTable) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания таблицы операций",
                        "Не удалось создать таблицу для отображения истории операций");
    }
    operationsTable->setColumnCount(4);
    operationsTable->setHorizontalHeaderLabels({"Дата", "Тип операции", "Откуда", "Куда"});

    // Настройка таблицы (УБИРАЕМ сортировку)
    operationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    operationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    operationsTable->setAlternatingRowColors(true);
    operationsTable->horizontalHeader()->setStretchLastSection(true);
    operationsTable->verticalHeader()->setVisible(false);
    // УБИРАЕМ: operationsTable->setSortingEnabled(true);
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
    TRY_CATCH_END
}

void OperationsHistoryWindow::applyStyle()
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
    TRY_CATCH_END
}

void OperationsHistoryWindow::loadOperationsHistory()
{
    TRY_CATCH_BEGIN
        QDir historyDir("operations_history");
    if (!historyDir.exists() && !historyDir.mkpath(".")) {
        THROW_FILE_ERROR("operations_history", "создания директории",
                         "Не удалось создать директорию для истории операций");
    }

    m_operationsHistory.clear();
    QFile file(m_historyFile);
    if (file.exists()) {
        if (!file.open(QIODevice::ReadOnly)) {
            THROW_FILE_ERROR(m_historyFile, "открытия для чтения",
                             "Не удалось прочитать историю операций");
        }
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
    TRY_CATCH_END
}

void OperationsHistoryWindow::saveOperationsHistory()
{
    TRY_CATCH_BEGIN
        QFile file(m_historyFile);
    if (!file.open(QIODevice::WriteOnly)) {
        THROW_FILE_ERROR(m_historyFile, "открытия для записи",
                         "Не удалось сохранить историю операций");
    }
    QDataStream out(&file);
    out << static_cast<quint32>(m_operationsHistory.size());
    for (const Operation& operation : m_operationsHistory) {
        out << operation;
    }
    file.close();
    TRY_CATCH_END
}

void OperationsHistoryWindow::updateTable()
{
    TRY_CATCH_BEGIN
        // Сортируем операции по дате (новые сверху)
        std::sort(m_operationsHistory.begin(), m_operationsHistory.end(),
                  [](const Operation& a, const Operation& b) {
                      return a.getDate() > b.getDate(); // По убыванию - новые сначала
                  });

    operationsTable->setRowCount(m_operationsHistory.size());

    for (int i = 0; i < m_operationsHistory.size(); ++i) {
        const Operation& operation = m_operationsHistory.at(i);

        QString dateString = operation.getDate().toString("dd.MM.yyyy");
        QString operationType = operation.getTypeString();
        QString fromLocation = operation.getFromLocation();
        QString toLocation = operation.getToLocation();

        QTableWidgetItem *dateItem = new QTableWidgetItem(dateString);
        if (!dateItem) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания элемента таблицы",
                            "Не удалось создать элемент для даты операции");
        }

        QTableWidgetItem *typeItem = new QTableWidgetItem(operationType);
        if (!typeItem) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания элемента таблицы",
                            "Не удалось создать элемент для типа операции");
        }

        QTableWidgetItem *fromItem = new QTableWidgetItem(fromLocation);
        if (!fromItem) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания элемента таблицы",
                            "Не удалось создать элемент для места отправления");
        }

        QTableWidgetItem *toItem = new QTableWidgetItem(toLocation);
        if (!toItem) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания элемента таблицы",
                            "Не удалось создать элемент для места назначения");
        }

        operationsTable->setItem(i, 0, dateItem);
        operationsTable->setItem(i, 1, typeItem);
        operationsTable->setItem(i, 2, fromItem);
        operationsTable->setItem(i, 3, toItem);
    }

    // УБИРАЕМ: operationsTable->setSortingEnabled(false);
    TRY_CATCH_END
}

void OperationsHistoryWindow::onBackClicked()
{
    TRY_CATCH_BEGIN
        SectionWindow *sectionWindow = new SectionWindow(m_sectionNumber, m_materialType, m_isAdmin, m_mode, m_userManager);
    if (!sectionWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна секции",
                        "Не удалось создать экземпляр SectionWindow");
    }
    sectionWindow->show();
    this->close();
    TRY_CATCH_END
}

void OperationsHistoryWindow::onCellDoubleClicked(int row, int column)
{
    TRY_CATCH_BEGIN
        if (!m_isAdmin) return;

    if (row < 0 || row >= m_operationsHistory.size()) {
        THROW_VALIDATION_ERROR("номер строки", "выходит за пределы списка операций",
                               QString("Выбранная строка %1, всего строк %2").arg(row).arg(m_operationsHistory.size()));
    }

    QTableWidgetItem* item = operationsTable->item(row, column);
    if (!item) {
        THROW_EXCEPTION(ErrorSeverity::WARNING, ErrorSource::VALIDATION,
                        "Ошибка редактирования", "Не удалось получить элемент таблицы для редактирования");
        return;
    }

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
        QDate currentDate = m_operationsHistory[row].getDate();

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
        int currentIndex = types.indexOf(currentValue);
        if (currentIndex == -1) currentIndex = 0;

        QString selected = QInputDialog::getItem(this, "Редактирование", "Выберите тип операции:",
                                                 types, currentIndex, false, &ok);
        if (ok) newValue = selected;
    } else {
        newValue = QInputDialog::getText(this, "Редактирование",
                                         QString("Введите новое значение для '%1':").arg(columnName),
                                         QLineEdit::Normal, currentValue, &ok);
    }

    if (ok && !newValue.isEmpty()) {
        if (validateCellEdit(row, column, newValue)) {
            updateOperationData(row, column, newValue);
            // Используем ExceptionHandler для информационного сообщения
            ExceptionHandler::showMessageBox(ErrorSeverity::INFO, "Успех",
                                             "Данные успешно обновлены!", this);
        }
    }
    TRY_CATCH_END
}

bool OperationsHistoryWindow::validateCellEdit(int row, int column, const QString& newValue)
{
    // Этот метод оставляем с QMessageBox, так как это пользовательские ошибки валидации
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
    TRY_CATCH_BEGIN
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
    // Обновляем таблицу
    updateTable();
    TRY_CATCH_END
}

void OperationsHistoryWindow::setupContextMenu()
{
    TRY_CATCH_BEGIN
        contextMenu = new QMenu(this);
    if (!contextMenu) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания контекстного меню",
                        "Не удалось создать контекстное меню для таблицы операций");
    }
    QAction *deleteAction = new QAction("Удалить", this);
    if (!deleteAction) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания действия меню",
                        "Не удалось создать действие удаления для контекстного меню");
    }
    connect(deleteAction, &QAction::triggered, this, &OperationsHistoryWindow::onDeleteOperation);
    contextMenu->addAction(deleteAction);

    operationsTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(operationsTable, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
        if (!m_isAdmin) return;
        QTableWidgetItem *item = operationsTable->itemAt(pos);
        if (item) {
            selectedRow = item->row(); // Сохраняем визуальную строку
            contextMenu->exec(operationsTable->viewport()->mapToGlobal(pos));
        }
    });
    TRY_CATCH_END
}

void OperationsHistoryWindow::onDeleteOperation()
{
    TRY_CATCH_BEGIN
        if (selectedRow < 0 || selectedRow >= operationsTable->rowCount()) {
        THROW_VALIDATION_ERROR("номер строки", "не выбран или выходит за пределы",
                               "Не выбрана строка для удаления или некорректный номер строки");
    }

    // Поскольку операции уже отсортированы вручную, визуальная строка соответствует индексу в m_operationsHistory
    if (selectedRow >= m_operationsHistory.size()) {
        THROW_VALIDATION_ERROR("индекс операции", "выходит за пределы списка операций",
                               QString("Выбранная строка %1, всего операций %2").arg(selectedRow).arg(m_operationsHistory.size()));
    }

    const Operation& operation = m_operationsHistory.at(selectedRow);
    QString operationInfo = QString("Дата: %1\nТип: %2\nОткуда: %3\nКуда: %4")
                                .arg(operation.getDate().toString("dd.MM.yyyy"))
                                .arg(operation.getTypeString())
                                .arg(operation.getFromLocation())
                                .arg(operation.getToLocation());

    // Оставляем QMessageBox для пользовательского подтверждения
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Подтверждение удаления",
        QString("Вы уверены, что хотите удалить эту операцию?\n\n%1").arg(operationInfo),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        m_operationsHistory.removeAt(selectedRow);
        saveOperationsHistory();
        updateTable();
        // Используем ExceptionHandler для информационного сообщения
        ExceptionHandler::showMessageBox(ErrorSeverity::INFO, "Успех",
                                         "Операция успешно удалена!", this);
    }

    selectedRow = -1;
    TRY_CATCH_END
}
