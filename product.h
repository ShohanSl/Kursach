#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>
#include <QDataStream>

class Product
{
public:
    Product();
    Product(const QString& name, const QString& index, int quantity,
            const QString& supplier, int cellNumber);

    // Getters
    QString getName() const;
    QString getIndex() const;
    int getQuantity() const;
    QString getSupplier() const;
    int getCellNumber() const;

    // Setters
    void setName(const QString& name);
    void setIndex(const QString& index);
    void setQuantity(int quantity);
    void setSupplier(const QString& supplier);
    void setCellNumber(int cellNumber);

    // Serialization
    friend QDataStream& operator<<(QDataStream& out, const Product& product);
    friend QDataStream& operator>>(QDataStream& in, Product& product);

private:
    QString m_name;
    QString m_index;
    int m_quantity;
    QString m_supplier;
    int m_cellNumber;
};

#endif // PRODUCT_H
