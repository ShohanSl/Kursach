#ifndef EMPLOYEE_H
#define EMPLOYEE_H

#include "user.h"

class Employee : public User
{
public:
    Employee();
    Employee(const QString& lastName, const QString& firstName, const QString& middleName,
             const QString& login, const QString& password);

    // Getters
    QString getLogin() const;
    QString getPassword() const;
    QString getRoleString() const override;

    // Setters
    void setLogin(const QString& login);
    void setPassword(const QString& password);

    // Аутентификация
    bool authenticate(const QString& login, const QString& password) const;

    // Serialization
    void serialize(QDataStream& out) const override;
    void deserialize(QDataStream& in) override;

    // Операторы сравнения
    bool operator==(const Employee& other) const;

private:
    QString m_login;
    QString m_password;
};

// Сериализация для Employee
QDataStream& operator<<(QDataStream& out, const Employee& employee);
QDataStream& operator>>(QDataStream& in, Employee& employee);

#endif // EMPLOYEE_H
