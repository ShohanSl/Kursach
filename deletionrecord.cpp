#include "deletionrecord.h"

DeletionRecord::DeletionRecord()
    : m_sectionNumber(0), m_cellNumber(0)
{
}

DeletionRecord::DeletionRecord(int sectionNumber, int cellNumber, const Product& product)
    : m_sectionNumber(sectionNumber), m_cellNumber(cellNumber), m_product(product)
{
}

QDataStream& operator<<(QDataStream& out, const DeletionRecord& record)
{
    out << record.m_sectionNumber << record.m_cellNumber << record.m_product;
    return out;
}

QDataStream& operator>>(QDataStream& in, DeletionRecord& record)
{
    in >> record.m_sectionNumber >> record.m_cellNumber >> record.m_product;
    return in;
}
