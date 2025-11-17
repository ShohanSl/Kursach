#include "user.h"

User::User()
    : m_lastName(""), m_firstName(""), m_middleName("")
{
}

User::User(const QString& lastName, const QString& firstName, const QString& middleName)
    : m_lastName(lastName), m_firstName(firstName), m_middleName(middleName)
{
}

// Getters
QString User::getLastName() const { return m_lastName; }
QString User::getFirstName() const { return m_firstName; }
QString User::getMiddleName() const { return m_middleName; }

QString User::getFullName() const {
    return m_lastName + " " + m_firstName + " " + m_middleName;
}

QString User::getRoleString() const {
    return "Пользователь (ожидает регистрации)";
}

// Setters
void User::setLastName(const QString& lastName) { m_lastName = lastName; }
void User::setFirstName(const QString& firstName) { m_firstName = firstName; }
void User::setMiddleName(const QString& middleName) { m_middleName = middleName; }

// Serialization
void User::serialize(QDataStream& out) const {
    out << m_lastName << m_firstName << m_middleName;
}

void User::deserialize(QDataStream& in) {
    in >> m_lastName >> m_firstName >> m_middleName;
}

// Операторы сравнения
bool User::operator==(const User& other) const {
    return m_lastName == other.m_lastName &&
           m_firstName == other.m_firstName &&
           m_middleName == other.m_middleName;
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
