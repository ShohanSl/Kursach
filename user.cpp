// user.cpp
#include "user.h"

User::User()
    : m_role(UserRole::Employee) // По умолчанию сотрудник
{
}

User::User(const QString& lastName, const QString& firstName, const QString& middleName, UserRole role)
    : m_lastName(lastName), m_firstName(firstName), m_middleName(middleName), m_role(role)
{
    // Роль устанавливается через параметр
}

// Getters
QString User::getLastName() const { return m_lastName; }
QString User::getFirstName() const { return m_firstName; }
QString User::getMiddleName() const { return m_middleName; }

QString User::getFullName() const {
    return m_lastName + " " + m_firstName + " " + m_middleName;
}

UserRole User::getRole() const { return m_role; }

QString User::getRoleString() const {
    switch(m_role) {
    case UserRole::Employee: return "Сотрудник";
    case UserRole::Administrator: return "Администратор";
    default: return "Неизвестно";
    }
}

// Setters
void User::setLastName(const QString& lastName) { m_lastName = lastName; }
void User::setFirstName(const QString& firstName) { m_firstName = firstName; }
void User::setMiddleName(const QString& middleName) { m_middleName = middleName; }
void User::setRole(UserRole role) { m_role = role; }

// Serialization
void User::serialize(QDataStream& out) const {
    out << m_lastName << m_firstName << m_middleName << m_role;
}

void User::deserialize(QDataStream& in) {
    in >> m_lastName >> m_firstName >> m_middleName >> m_role;
}

// Операторы сравнения
bool User::operator==(const User& other) const {
    return m_lastName == other.m_lastName &&
           m_firstName == other.m_firstName &&
           m_middleName == other.m_middleName &&
           m_role == other.m_role;
}

// Global serialization operators
QDataStream& operator<<(QDataStream& out, const User& user) {
    user.serialize(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, User& user) {
    user.deserialize(in);
    return in;
}

// Сериализация для UserRole
QDataStream& operator<<(QDataStream& out, const UserRole& role) {
    out << static_cast<quint32>(role);
    return out;
}

QDataStream& operator>>(QDataStream& in, UserRole& role) {
    quint32 value;
    in >> value;
    role = static_cast<UserRole>(value);
    return in;
}
