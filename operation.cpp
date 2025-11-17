#include "operation.h"

Operation::Operation()
    : m_quantity(0), m_type(DELIVERY), m_date(QDate::currentDate()) // Меняем на QDate
{
}

Operation::Operation(const QString& productName, const QString& productIndex, int quantity,
                     const QString& fromLocation, const QString& toLocation,
                     OperationType type, const QDate& date) // Меняем на QDate
    : m_productName(productName), m_productIndex(productIndex), m_quantity(quantity),
    m_fromLocation(fromLocation), m_toLocation(toLocation), m_type(type), m_date(date)
{
}

// Getters
QString Operation::getProductName() const { return m_productName; }
QString Operation::getProductIndex() const { return m_productIndex; }
int Operation::getQuantity() const { return m_quantity; }
QString Operation::getFromLocation() const { return m_fromLocation; }
QString Operation::getToLocation() const { return m_toLocation; }
Operation::OperationType Operation::getType() const { return m_type; }
QDate Operation::getDate() const { return m_date; } // Меняем на QDate

QString Operation::getTypeString() const {
    switch(m_type) {
    case DELIVERY: return "Поставка";
    case SHIPMENT: return "Отгрузка";
    case TRANSFER: return "Перемещение";
    default: return "Неизвестно";
    }
}

// Serialization
QDataStream& operator<<(QDataStream& out, const Operation& operation) {
    out << operation.m_productName << operation.m_productIndex << operation.m_quantity
        << operation.m_fromLocation << operation.m_toLocation
        << static_cast<quint32>(operation.m_type) << operation.m_date; // Меняем на QDate
    return out;
}

QDataStream& operator>>(QDataStream& in, Operation& operation) {
    quint32 type;
    in >> operation.m_productName >> operation.m_productIndex >> operation.m_quantity
        >> operation.m_fromLocation >> operation.m_toLocation >> type >> operation.m_date; // Меняем на QDate
    operation.m_type = static_cast<Operation::OperationType>(type);
    return in;
}
