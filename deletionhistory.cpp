#include "deletionhistory.h"
#include <QFile>
#include <QDataStream>
#include <QDir>

DeletionHistory* DeletionHistory::m_instance = nullptr;

DeletionHistory::DeletionHistory(QObject* parent)
    : QObject(parent)
{
}

DeletionHistory::~DeletionHistory()
{
}

DeletionHistory* DeletionHistory::instance()
{
    if (!m_instance) {
        m_instance = new DeletionHistory();
    }
    return m_instance;
}

void DeletionHistory::addDeletion(int sectionNumber, int cellNumber, const Product& product)
{
    DeletionRecord record(sectionNumber, cellNumber, product);

    // Добавляем новую запись в начало
    m_history.insert(0, record);

    // Ограничиваем размер истории
    if (m_history.size() > MAX_HISTORY_SIZE) {
        m_history.removeAt(MAX_HISTORY_SIZE);
    }

    // Автоматически сохраняем историю
    saveToFile("deletion_history.bin");
}

bool DeletionHistory::undoLastDeletion(int& sectionNumber, int& cellNumber, Product& product)
{
    if (m_history.isEmpty()) {
        return false;
    }

    // Берем первую (самую свежую) запись
    DeletionRecord record = m_history.first();
    sectionNumber = record.getSectionNumber();
    cellNumber = record.getCellNumber();
    product = record.getProduct();

    // Удаляем запись из истории
    m_history.removeAt(0);

    // Сохраняем обновленную историю
    saveToFile("deletion_history.bin");

    return true;
}

bool DeletionHistory::canUndo() const
{
    return !m_history.isEmpty();
}

int DeletionHistory::historySize() const
{
    return m_history.size();
}

void DeletionHistory::clear()
{
    m_history.clear();
}

void DeletionHistory::saveToFile(const QString& filename)
{
    QDir().mkpath(".");
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << static_cast<quint32>(m_history.size());
        for (int i = 0; i < m_history.size(); ++i) {
            out << m_history[i];
        }
        file.close();
    }
}

void DeletionHistory::loadFromFile(const QString& filename)
{
    m_history.clear();

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> size;

        for (quint32 i = 0; i < size; ++i) {
            DeletionRecord record;
            in >> record;
            m_history.append(record);
        }
        file.close();
    }
}
