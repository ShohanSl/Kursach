#include "deletionhistory.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include "fileexception.h"  // Добавляем заголовок

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
    try {
        saveToFile("deletion_history.bin");
    } catch (const FileException& e) {
        // В данном случае не прерываем операцию, только логируем
        // Пользователь мог продолжать работу даже если история не сохранилась
        qWarning() << "Не удалось сохранить историю удалений:" << e.qmessage();
    }
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
    try {
        saveToFile("deletion_history.bin");
        return true;
    } catch (const FileException& e) {
        qWarning() << "Не удалось сохранить историю после отмены:" << e.qmessage();
        return false;
    }
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

    if (!file.open(QIODevice::WriteOnly)) {
        throw FileException(QString("Не удалось открыть файл для записи: %1\nОшибка: %2")
                                .arg(filename, file.errorString()));
    }

    QDataStream out(&file);
    out << static_cast<quint32>(m_history.size());
    for (int i = 0; i < m_history.size(); ++i) {
        out << m_history[i];
    }

    if (file.error() != QFile::NoError) {
        file.close();
        throw FileException(QString("Ошибка записи в файл: %1\nОшибка: %2")
                                .arg(filename, file.errorString()));
    }

    file.close();
}

void DeletionHistory::loadFromFile(const QString& filename)
{
    m_history.clear();

    QFile file(filename);
    if (!file.exists()) {
        // Если файл не существует, это нормально (первый запуск)
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        throw FileException(QString("Не удалось открыть файл для чтения: %1\nОшибка: %2")
                                .arg(filename, file.errorString()));
    }

    QDataStream in(&file);
    quint32 size;
    in >> size;

    for (quint32 i = 0; i < size; ++i) {
        DeletionRecord record;
        in >> record;
        m_history.append(record);
    }

    if (file.error() != QFile::NoError) {
        file.close();
        throw FileException(QString("Ошибка чтения файла: %1\nОшибка: %2")
                                .arg(filename, file.errorString()));
    }

    file.close();
}
