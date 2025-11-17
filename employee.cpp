#include "employee.h"

Employee::Employee()
    : User(), m_login(""), m_password("")
{
}

Employee::Employee(const QString& lastName, const QString& firstName, const QString& middleName,
                   const QString& login, const QString& password)
    : User(lastName, firstName, middleName), m_login(login), m_password(password)
{
}

// Getters
QString Employee::getLogin() const { return m_login; }
QString Employee::getPassword() const { return m_password; }

QString Employee::getRoleString() const {
    return "Сотрудник";
}

// Setters
void Employee::setLogin(const QString& login) { m_login = login; }
void Employee::setPassword(const QString& password) { m_password = password; }

// Аутентификация
bool Employee::authenticate(const QString& login, const QString& password) const {
    return m_login == login && m_password == password;
}

// Serialization
void Employee::serialize(QDataStream& out) const {
    User::serialize(out);
    out << m_login << m_password;
}

void Employee::deserialize(QDataStream& in) {
    User::deserialize(in);
    in >> m_login >> m_password;
}

// Операторы сравнения
bool Employee::operator==(const Employee& other) const {
    return User::operator==(other) &&
           m_login == other.m_login &&
           m_password == other.m_password;
}

// Global serialization operators
QDataStream& operator<<(QDataStream& out, const Employee& employee) {
    employee.serialize(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Employee& employee) {
    employee.deserialize(in);
    return in;
}
