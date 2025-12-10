// user.h
#ifndef USER_H
#define USER_H

#include <QString>
#include <QDataStream>

enum class UserRole {
    Employee,     // Сотрудник
    Administrator // Администратор
};

class User
{
public:
    User();
    User(const QString& lastName, const QString& firstName, const QString& middleName, UserRole role = UserRole::Employee);
    virtual ~User() = default;

    // Getters
    QString getLastName() const;
    QString getFirstName() const;
    QString getMiddleName() const;
    QString getFullName() const;
    UserRole getRole() const;
    virtual QString getRoleString() const;

    // Setters
    void setLastName(const QString& lastName);
    void setFirstName(const QString& firstName);
    void setMiddleName(const QString& middleName);
    void setRole(UserRole role);

    // Serialization
    virtual void serialize(QDataStream& out) const;
    virtual void deserialize(QDataStream& in);

    // Операторы сравнения
    bool operator==(const User& other) const;

protected:
    QString m_lastName;
    QString m_firstName;
    QString m_middleName;
    UserRole m_role;
};

// Сериализация для базового User
QDataStream& operator<<(QDataStream& out, const User& user);
QDataStream& operator>>(QDataStream& in, User& user);

// Сериализация для UserRole
QDataStream& operator<<(QDataStream& out, const UserRole& role);
QDataStream& operator>>(QDataStream& in, UserRole& role);

#endif // USER_H
