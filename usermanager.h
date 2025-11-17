#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "user.h"
#include "employee.h"
#include "administrator.h"
#include <QList>
#include <QString>

class UserManager
{
public:
    UserManager();

    // Аутентификация - возвращает указатель на Employee или Administrator
    Employee* authenticateUser(const QString& login, const QString& password);

    // Регистрация pending пользователей
    bool registerPendingUser(const QString& lastName, const QString& firstName,
                             const QString& middleName, bool isAdmin = false);

    // Завершение регистрации
    bool completeRegistration(const QString& fullName, const QString& login,
                              const QString& password);

    // Вспомогательные методы
    void initializeTestData();
    bool isUserInPending(const QString& fullName);
    User* getPendingUser(const QString& fullName); // Возвращает базового User

    QList<QStringList> getEmployeesData() const;
    QList<QStringList> getAdminsData() const;
    QList<QStringList> getPendingUsersData() const;
    bool removeEmployeeByLogin(const QString& login);
    bool removeAdminByLogin(const QString& login);
    bool removePendingUserByName(const QString& fullName);

private:
    void loadUsers();
    void saveUsers();
    User* findPendingUserByName(const QString& fullName);
    void clearAllUsers();

    QList<User*> m_pendingUsers;      // Базовые User (только ФИО)
    QList<Employee*> m_employees;     // Зарегистрированные сотрудники
    QList<Administrator*> m_admins;   // Зарегистрированные администраторы

    QString m_pendingUsersFile = "pending_users.bin";
    QString m_employeesFile = "employees.bin";
    QString m_adminsFile = "admins.bin";
};

#endif // USERMANAGER_H
