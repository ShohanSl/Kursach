#ifndef DELETIONHISTORY_H
#define DELETIONHISTORY_H

#include "deletionrecord.h"
#include "customlist.h"
#include <QObject>

class DeletionHistory : public QObject
{
    Q_OBJECT

public:
    static DeletionHistory* instance();

    // Добавление операции удаления
    void addDeletion(int sectionNumber, int cellNumber, const Product& product);

    // Отмена последней операции удаления
    bool undoLastDeletion(int& sectionNumber, int& cellNumber, Product& product);

    // Проверка наличия операций для отмены
    bool canUndo() const;

    // Получение количества операций в истории
    int historySize() const;

    // Очистка истории
    void clear();

    // Сохранение и загрузка истории
    void saveToFile(const QString& filename);
    void loadFromFile(const QString& filename);

private:
    DeletionHistory(QObject* parent = nullptr);
    ~DeletionHistory();

    static DeletionHistory* m_instance;
    CustomList<DeletionRecord> m_history;
    static const int MAX_HISTORY_SIZE = 5;
};

#endif // DELETIONHISTORY_H
