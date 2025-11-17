#include "administrator.h"

Administrator::Administrator()
    : Employee()
{
}

Administrator::Administrator(const QString& lastName, const QString& firstName, const QString& middleName,
                             const QString& login, const QString& password)
    : Employee(lastName, firstName, middleName, login, password)
{
}

QString Administrator::getRoleString() const {
    return "Администратор";
}

// Serialization
void Administrator::serialize(QDataStream& out) const {
    Employee::serialize(out);
    // Можно добавить дополнительные поля администратора в будущем
}

void Administrator::deserialize(QDataStream& in) {
    Employee::deserialize(in);
    // Можно добавить дополнительные поля администратора в будущем
}

// Global serialization operators
QDataStream& operator<<(QDataStream& out, const Administrator& admin) {
    admin.serialize(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Administrator& admin) {
    admin.deserialize(in);
    return in;
}
