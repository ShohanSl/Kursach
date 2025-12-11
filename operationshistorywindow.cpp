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
#include <QMenu>
#include <QAction>
#include "sectionwindow.h"
#include "fileexception.h"        // Добавляем
#include "validationexception.h"  // Добавляем
#include "inputvalidator.h"       // Добавляем

OperationsHistoryWindow::OperationsHistoryWindow(int sectionNumber, const QString& materialType,
                                                 bool isAdmin, const QString& mode, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_sectionNumber(sectionNumber), m_materialType(materialType),
    m_isAdmin(isAdmin), m_mode(mode), m_userManager(userManager), selectedRow(-1)
{
    m_historyFile = QString("operations_history/section_history_%1.bin").arg(sectionNumber);

    setupUI();
    setWindowTitle(QString("История операций - Секция %1").arg(sectionNumber));
    setFixedSize(900, 600);

    try {
        loadOperationsHistory();
        updateTable();
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка загрузки",
                              QString("Не удалось загрузить историю операций:\n%1").arg(e.qmessage()));
        // Возвращаемся к секции
        onBackClicked();
        return;
    }

    if (m_isAdmin) {
        setupContextMenu();
    }
}

void OperationsHistoryWindow::setupUI()
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

    QLabel *titleLabel = new QLabel(QString("ИСТОРИЯ ОПЕРАЦИЙ - СЕКЦИЯ №%1").arg(m_sectionNumber));
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    // Таблица операций
    operationsTable = new QTableWidget();
    operationsTable->setColumnCount(4);
    operationsTable->setHorizontalHeaderLabels({"Дата", "Тип операции", "Откуда", "Куда"});

    operationsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    operationsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    operationsTable->setAlternatingRowColors(true);
    operationsTable->horizontalHeader()->setStretchLastSection(true);
    operationsTable->verticalHeader()->setVisible(false);
    operationsTable->setSelectionMode(QAbstractItemView::SingleSelection);

    operationsTable->setColumnWidth(0, 150);
    operationsTable->setColumnWidth(1, 150);
    operationsTable->setColumnWidth(2, 250);
    operationsTable->setFixedHeight(400);

    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(operationsTable);

    // Подключение сигналов
    connect(backButton, &QPushButton::clicked, this, &OperationsHistoryWindow::onBackClicked);

    if (m_isAdmin) {
        connect(operationsTable, &QTableWidget::cellDoubleClicked, this, &OperationsHistoryWindow::onCellDoubleClicked);
    }
}

void OperationsHistoryWindow::loadOperationsHistory()
{
    try {
        QDir().mkpath("operations_history");

        m_operationsHistory.clear();

        QFile file(m_historyFile);
        if (!file.exists()) {
            // Файл может не существовать при первом открытии истории - это нормально
            return;
        }

        if (!file.open(QIODevice::ReadOnly)) {
            throw FileException(QString("Не удалось открыть файл истории операций '%1' для чтения\nОшибка: %2")
                                    .arg(m_historyFile, file.errorString()));
        }

        QDataStream in(&file);
        quint32 size;
        in >> size;

        if (in.status() != QDataStream::Ok) {
            file.close();
            throw FileException(QString("Ошибка чтения размера данных из файла истории операций '%1'")
                                    .arg(m_historyFile));
        }

        for (quint32 i = 0; i < size; ++i) {
            Operation operation;
            in >> operation;

            if (in.status() != QDataStream::Ok) {
                file.close();
                throw FileException(QString("Ошибка чтения операции №%1 из файла истории")
                                        .arg(i + 1));
            }

            // Валидация данных операции
            if (operation.getQuantity() < 0) {
                throw ValidationException(QString("Операция №%1 содержит некорректное количество: %2")
                                              .arg(i + 1).arg(operation.getQuantity()));
            }

            if (!operation.getDate().isValid()) {
                throw ValidationException(QString("Операция №%1 содержит некорректную дату")
                                              .arg(i + 1));
            }

            m_operationsHistory.append(operation);
        }

        file.close();

    } catch (const AppException& e) {
        throw; // Пробрасываем дальше
    }
}

void OperationsHistoryWindow::saveOperationsHistory()
{
    try {
        QFile file(m_historyFile);

        if (!file.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл истории операций '%1' для записи\nОшибка: %2")
                                    .arg(m_historyFile, file.errorString()));
        }

        QDataStream out(&file);
        out << static_cast<quint32>(m_operationsHistory.size());

        for (const Operation& operation : m_operationsHistory) {
            out << operation;

            if (out.status() != QDataStream::Ok) {
                file.close();
                throw FileException(QString("Ошибка записи операции '%1' в файл истории")
                                        .arg(operation.getProductName()));
            }
        }

        file.close();

    } catch (const AppException& e) {
        throw; // Пробрасываем дальше
    }
}

void OperationsHistoryWindow::updateTable()
{
    try {
        // Сортируем операции по дате (новые сверху)
        std::sort(m_operationsHistory.begin(), m_operationsHistory.end(),
                  [](const Operation& a, const Operation& b) {
                      return a.getDate() > b.getDate();
                  });

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
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка обновления таблицы",
                             QString("Произошла ошибка при обновлении таблицы:\n%1").arg(e.what()));
    }
}

void OperationsHistoryWindow::onBackClicked()
{
    try {
        SectionWindow *sectionWindow = new SectionWindow(m_sectionNumber, m_materialType, m_isAdmin, m_mode, m_userManager);
        sectionWindow->show();
        this->close();
    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Не удалось вернуться к секции:\n%1").arg(e.qmessage()));
    }
}

void OperationsHistoryWindow::onCellDoubleClicked(int row, int column)
{
    if (!m_isAdmin) return;

    if (row < 0 || row >= m_operationsHistory.size()) {
        QMessageBox::warning(this, "Ошибка", "Неверный номер строки");
        return;
    }

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
        try {
            // Валидация ввода
            InputValidator::validateNotEmptyOrThrow(newValue, columnName);

            if (column == 0) {
                QDate date = QDate::fromString(newValue, "dd.MM.yyyy");
                if (!date.isValid()) {
                    throw ValidationException("Неверный формат даты. Используйте: дд.мм.гггг");
                }
                if (date > QDate::currentDate()) {
                    throw ValidationException("Дата операции не может быть в будущем");
                }
            } else if (column == 1) {
                QStringList validTypes = {"Поставка", "Отгрузка", "Перемещение"};
                if (!validTypes.contains(newValue)) {
                    throw ValidationException("Недопустимый тип операции. Допустимые значения: Поставка, Отгрузка, Перемещение");
                }
            }

            updateOperationData(row, column, newValue);
            QMessageBox::information(this, "Успех", "Данные успешно обновлены!");

        } catch (const ValidationException& e) {
            QMessageBox::warning(this, "Неверный ввод", e.qmessage());
        } catch (const AppException& e) {
            QMessageBox::critical(this, "Ошибка", e.qmessage());
        }
    }
}

void OperationsHistoryWindow::updateOperationData(int row, int column, const QString& newValue)
{
    try {
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

    } catch (const AppException& e) {
        QMessageBox::critical(this, "Ошибка сохранения",
                              QString("Не удалось сохранить изменения:\n%1").arg(e.qmessage()));
        // Перезагружаем данные, чтобы откатить изменения
        try {
            loadOperationsHistory();
            updateTable();
        } catch (const AppException& reloadError) {
            QMessageBox::critical(this, "Критическая ошибка",
                                  "Не удалось восстановить данные после ошибки сохранения");
        }
    }
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
    if (selectedRow < 0 || selectedRow >= operationsTable->rowCount()) {
        return;
    }

    if (selectedRow >= m_operationsHistory.size()) {
        QMessageBox::warning(this, "Ошибка", "Неверный номер строки");
        return;
    }

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
        try {
            m_operationsHistory.removeAt(selectedRow);
            saveOperationsHistory();
            updateTable();
            QMessageBox::information(this, "Успех", "Операция успешно удалена!");

        } catch (const FileException& e) {
            QMessageBox::critical(this, "Ошибка сохранения",
                                  QString("Не удалось сохранить изменения после удаления:\n%1").arg(e.qmessage()));
            // Восстанавливаем удаленную операцию в списке
            // (не можем восстановить, т.к. она уже удалена, нужно перезагрузить)
            try {
                loadOperationsHistory();
                updateTable();
            } catch (const AppException& reloadError) {
                QMessageBox::critical(this, "Критическая ошибка",
                                      "Не удалось восстановить данные после ошибки удаления");
            }
        } catch (const AppException& e) {
            QMessageBox::critical(this, "Ошибка", e.qmessage());
        }
    }

    selectedRow = -1;
}
