#include "product.h"

Product::Product()
    : m_quantity(0), m_cellNumber(0)
{
}

Product::Product(const QString& name, const QString& index, int quantity,
                 const QString& supplier, int cellNumber)
    : m_name(name), m_index(index), m_quantity(quantity),
    m_supplier(supplier), m_cellNumber(cellNumber)
{
}

// Getters
QString Product::getName() const { return m_name; }
QString Product::getIndex() const { return m_index; }
int Product::getQuantity() const { return m_quantity; }
QString Product::getSupplier() const { return m_supplier; }
int Product::getCellNumber() const { return m_cellNumber; }

// Setters
void Product::setName(const QString& name) { m_name = name; }
void Product::setIndex(const QString& index) { m_index = index; }
void Product::setQuantity(int quantity) { m_quantity = quantity; }
void Product::setSupplier(const QString& supplier) { m_supplier = supplier; }
void Product::setCellNumber(int cellNumber) { m_cellNumber = cellNumber; }

// Serialization
QDataStream& operator<<(QDataStream& out, const Product& product) {
    out << product.m_name << product.m_index << product.m_quantity
        << product.m_supplier << product.m_cellNumber;
    return out;
}

QDataStream& operator>>(QDataStream& in, Product& product) {
    in >> product.m_name >> product.m_index >> product.m_quantity
        >> product.m_supplier >> product.m_cellNumber;
    return in;
}
