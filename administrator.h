#ifndef ADMINISTRATOR_H
#define ADMINISTRATOR_H

#include "employee.h"

class Administrator : public Employee
{
public:
    Administrator();
    Administrator(const QString& lastName, const QString& firstName, const QString& middleName,
                  const QString& login, const QString& password);

    QString getRoleString() const override;

    // Методы администратора (будут добавлены позже)
    bool canManageAccounts() const { return true; }
    bool canViewAuditLog() const { return true; }
    bool canManageSystemSettings() const { return true; }

    // Serialization
    void serialize(QDataStream& out) const override;
    void deserialize(QDataStream& in) override;
};

// Сериализация для Administrator
QDataStream& operator<<(QDataStream& out, const Administrator& admin);
QDataStream& operator>>(QDataStream& in, Administrator& admin);

#endif // ADMINISTRATOR_H
