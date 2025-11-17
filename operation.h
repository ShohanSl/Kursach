#ifndef OPERATION_H
#define OPERATION_H

#include <QString>
#include <QDate>  // Меняем QDateTime на QDate
#include <QDataStream>

class Operation
{
public:
    enum OperationType {
        DELIVERY,    // Поставка
        SHIPMENT,    // Отгрузка
        TRANSFER     // Перемещение
    };

    Operation();
    Operation(const QString& productName, const QString& productIndex, int quantity,
              const QString& fromLocation, const QString& toLocation,
              OperationType type, const QDate& date = QDate::currentDate()); // Меняем на QDate
    Operation(const Operation& other)
        : m_productName(other.m_productName),
        m_productIndex(other.m_productIndex),
        m_quantity(other.m_quantity),
        m_fromLocation(other.m_fromLocation),
        m_toLocation(other.m_toLocation),
        m_type(other.m_type),
        m_date(other.m_date)
    {
    }

    Operation& operator=(const Operation& other)
    {
        if (this != &other) {
            m_productName = other.m_productName;
            m_productIndex = other.m_productIndex;
            m_quantity = other.m_quantity;
            m_fromLocation = other.m_fromLocation;
            m_toLocation = other.m_toLocation;
            m_type = other.m_type;
            m_date = other.m_date;
        }
        return *this;
    }

    // Getters
    QString getProductName() const;
    QString getProductIndex() const;
    int getQuantity() const;
    QString getFromLocation() const;
    QString getToLocation() const;
    OperationType getType() const;
    QDate getDate() const; // Меняем на QDate
    QString getTypeString() const;

    // Setters
    void setProductName(const QString& name) { m_productName = name; }
    void setProductIndex(const QString& index) { m_productIndex = index; }
    void setQuantity(int quantity) { m_quantity = quantity; }
    void setFromLocation(const QString& location) { m_fromLocation = location; }
    void setToLocation(const QString& location) { m_toLocation = location; }
    void setType(OperationType type) { m_type = type; }
    void setDate(const QDate& date) { m_date = date; } // Меняем на QDate

    // Serialization
    friend QDataStream& operator<<(QDataStream& out, const Operation& operation);
    friend QDataStream& operator>>(QDataStream& in, Operation& operation);

private:
    QString m_productName;
    QString m_productIndex;
    int m_quantity;
    QString m_fromLocation;
    QString m_toLocation;
    OperationType m_type;
    QDate m_date; // Меняем на QDate
};

#endif // OPERATION_H
