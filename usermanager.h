// usermanager.h
#ifndef USERMANAGER_H
#define USERMANAGER_H

#include "user.h"
#include "employee.h"
#include "administrator.h"
#include "customlist.h"
#include <QString>

class UserManager
{
public:
    UserManager();

    // Аутентификация
    User* authenticateUser(const QString& login, const QString& password);

    // Регистрация pending пользователей
    bool registerPendingUser(const QString& lastName, const QString& firstName,
                             const QString& middleName, UserRole role);

    // Завершение регистрации
    bool completeRegistration(const QString& fullName, const QString& login,
                              const QString& password);

    // Вспомогательные методы
    void initializeTestData();
    bool isUserInPending(const QString& fullName);
    User* getPendingUser(const QString& fullName);
    UserRole getPendingUserRole(const QString& fullName);

    CustomList<QStringList> getEmployeesData() const;
    CustomList<QStringList> getAdminsData() const;
    CustomList<QStringList> getPendingUsersData() const;
    bool removeEmployeeByLogin(const QString& login);
    bool removeAdminByLogin(const QString& login);
    bool removePendingUserByName(const QString& fullName);

private:
    void loadUsers();
    void saveUsers();
    User* findPendingUserByName(const QString& fullName);
    Employee* findEmployeeByLogin(const QString& login);
    Administrator* findAdminByLogin(const QString& login);

    CustomList<User*> m_pendingUsers;      // Ожидающие пользователи (базовые User)
    CustomList<Employee*> m_employees;     // Зарегистрированные сотрудники
    CustomList<Administrator*> m_admins;   // Зарегистрированные администраторы

    QString m_pendingUsersFile = "pending_users.bin";
    QString m_employeesFile = "employees.bin";
    QString m_adminsFile = "admins.bin";
};

#endif // USERMANAGER_H
