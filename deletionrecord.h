#ifndef DELETIONRECORD_H
#define DELETIONRECORD_H

#include "product.h"
#include <QDataStream>

class DeletionRecord
{
public:
    DeletionRecord();
    DeletionRecord(int sectionNumber, int cellNumber, const Product& product);

    // Геттеры
    int getSectionNumber() const { return m_sectionNumber; }
    int getCellNumber() const { return m_cellNumber; }
    Product getProduct() const { return m_product; }

    // Сериализация
    friend QDataStream& operator<<(QDataStream& out, const DeletionRecord& record);
    friend QDataStream& operator>>(QDataStream& in, DeletionRecord& record);

private:
    int m_sectionNumber;
    int m_cellNumber;
    Product m_product;
};

#endif // DELETIONRECORD_H
