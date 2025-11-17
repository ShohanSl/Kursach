#ifndef USER_H
#define USER_H

#include <QString>
#include <QDataStream>

class User
{
public:
    User();
    User(const QString& lastName, const QString& firstName, const QString& middleName);
    virtual ~User() = default;

    // Getters
    QString getLastName() const;
    QString getFirstName() const;
    QString getMiddleName() const;
    QString getFullName() const;
    virtual QString getRoleString() const;

    // Setters
    void setLastName(const QString& lastName);
    void setFirstName(const QString& firstName);
    void setMiddleName(const QString& middleName);

    // Serialization
    virtual void serialize(QDataStream& out) const;
    virtual void deserialize(QDataStream& in);

    // Операторы сравнения
    bool operator==(const User& other) const;

protected:
    QString m_lastName;
    QString m_firstName;
    QString m_middleName;
};

// Сериализация для базового User
QDataStream& operator<<(QDataStream& out, const User& user);
QDataStream& operator>>(QDataStream& in, User& user);

#endif // USER_H
