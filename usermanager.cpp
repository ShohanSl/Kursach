// usermanager.cpp
#include "usermanager.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDebug>

UserManager::UserManager()
{
    // Проверяем, есть ли уже данные
    if (!QFile::exists(m_adminsFile) && !QFile::exists(m_employeesFile)) {
        initializeTestData();
    } else {
        loadUsers();
    }
}

void UserManager::initializeTestData()
{
    // Очищаем все списки
    qDeleteAll(m_pendingUsers);
    m_pendingUsers.clear();
    qDeleteAll(m_employees);
    m_employees.clear();
    qDeleteAll(m_admins);
    m_admins.clear();

    // Создаем тестового администратора
    Administrator* admin = new Administrator("Иванов", "Админ", "Системович",
                                             "admin", "admin123");
    m_admins.append(admin);

    // Создаем тестового работника
    Employee* worker = new Employee("Петров", "Работник", "Складской",
                                    "worker", "work123");
    m_employees.append(worker);

    // Создаем незарегистрированных пользователей с разными ролями
    registerPendingUser("Сидоров", "Новый", "Администратор", UserRole::Administrator);
    registerPendingUser("Кузнецов", "Новый", "Сотрудник", UserRole::Employee);

    // Сохраняем тестовые данные
    saveUsers();

    qDebug() << "Тестовые данные инициализированы";
}

User* UserManager::authenticateUser(const QString& login, const QString& password)
{
    // Проверяем администраторов
    for (Administrator* admin : m_admins) {
        if (admin->authenticate(login, password)) {
            return admin; // Возвращаем как User* (upcast)
        }
    }

    // Проверяем сотрудников
    for (Employee* employee : m_employees) {
        if (employee->authenticate(login, password)) {
            return employee; // Возвращаем как User* (upcast)
        }
    }

    return nullptr;
}

bool UserManager::registerPendingUser(const QString& lastName, const QString& firstName,
                                      const QString& middleName, UserRole role)
{
    QString fullName = lastName + " " + firstName + " " + middleName;

    // Проверяем, нет ли уже пользователя с таким ФИО в ожидающих
    if (isUserInPending(fullName)) {
        return false;
    }

    // Создаем базового User с указанной ролью
    User* newUser = new User(lastName, firstName, middleName, role);
    m_pendingUsers.append(newUser);
    saveUsers();
    return true;
}

bool UserManager::completeRegistration(const QString& fullName, const QString& login,
                                       const QString& password)
{
    qDebug() << "Начало регистрации для:" << fullName;

    // Ищем пользователя в ожидающих
    User* pendingUser = findPendingUserByName(fullName);
    if (!pendingUser) {
        qDebug() << "Пользователь не найден в pending:" << fullName;
        return false;
    }

    // Получаем роль из ожидающего пользователя
    UserRole role = pendingUser->getRole();

    // Проверяем, не занят ли логин
    if (findEmployeeByLogin(login) || findAdminByLogin(login)) {
        qDebug() << "Логин уже занят:" << login;
        return false;
    }

    // Создаем пользователя в зависимости от роли
    if (role == UserRole::Administrator) {
        Administrator* newAdmin = new Administrator(pendingUser->getLastName(),
                                                    pendingUser->getFirstName(),
                                                    pendingUser->getMiddleName(),
                                                    login, password);
        m_admins.append(newAdmin);
        qDebug() << "Создан администратор:" << fullName;
    } else {
        Employee* newEmployee = new Employee(pendingUser->getLastName(),
                                             pendingUser->getFirstName(),
                                             pendingUser->getMiddleName(),
                                             login, password);
        m_employees.append(newEmployee);
        qDebug() << "Создан сотрудник:" << fullName;
    }

    // Удаляем из ожидания
    m_pendingUsers.removeOne(pendingUser);
    delete pendingUser;
    qDebug() << "Пользователь удален из pending:" << fullName;

    saveUsers();
    qDebug() << "Регистрация успешна для:" << fullName;

    return true;
}

void UserManager::loadUsers()
{
    // Загружаем ожидающих пользователей (базовые User)
    QFile file(m_pendingUsersFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            User* user = new User();
            in >> *user;
            m_pendingUsers.append(user);
        }
        file.close();
    }

    // Загружаем сотрудников
    file.setFileName(m_employeesFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Employee* employee = new Employee();
            in >> *employee;
            m_employees.append(employee);
        }
        file.close();
    }

    // Загружаем администраторов
    file.setFileName(m_adminsFile);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            Administrator* admin = new Administrator();
            in >> *admin;
            m_admins.append(admin);
        }
        file.close();
    }
}

void UserManager::saveUsers()
{
    // Создаем папку если нужно
    QDir().mkpath(".");

    // Сохраняем ожидающих пользователей
    QFile file(m_pendingUsersFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << static_cast<quint32>(m_pendingUsers.size());
        for (const User* user : m_pendingUsers) {
            out << *user;
        }
        file.close();
    }

    // Сохраняем сотрудников
    file.setFileName(m_employeesFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << static_cast<quint32>(m_employees.size());
        for (const Employee* employee : m_employees) {
            out << *employee;
        }
        file.close();
    }

    // Сохраняем администраторов
    file.setFileName(m_adminsFile);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << static_cast<quint32>(m_admins.size());
        for (const Administrator* admin : m_admins) {
            out << *admin;
        }
        file.close();
    }
}

User* UserManager::findPendingUserByName(const QString& fullName)
{
    for (User* user : m_pendingUsers) {
        if (user->getFullName() == fullName) {
            return user;
        }
    }
    return nullptr;
}

Employee* UserManager::findEmployeeByLogin(const QString& login)
{
    for (Employee* employee : m_employees) {
        if (employee->getLogin() == login) {
            return employee;
        }
    }
    return nullptr;
}

Administrator* UserManager::findAdminByLogin(const QString& login)
{
    for (Administrator* admin : m_admins) {
        if (admin->getLogin() == login) {
            return admin;
        }
    }
    return nullptr;
}

bool UserManager::isUserInPending(const QString& fullName)
{
    return findPendingUserByName(fullName) != nullptr;
}

User* UserManager::getPendingUser(const QString& fullName)
{
    return findPendingUserByName(fullName);
}

UserRole UserManager::getPendingUserRole(const QString& fullName)
{
    User* user = findPendingUserByName(fullName);
    if (user) {
        return user->getRole();
    }
    return UserRole::Employee; // По умолчанию
}

CustomList<QStringList> UserManager::getEmployeesData() const {
    CustomList<QStringList> data;
    for (const Employee* employee : m_employees) {
        data.append({employee->getLastName(),
                     employee->getFirstName(),
                     employee->getMiddleName(),
                     employee->getLogin()});
    }
    return data;
}

CustomList<QStringList> UserManager::getAdminsData() const {
    CustomList<QStringList> data;
    for (const Administrator* admin : m_admins) {
        data.append({admin->getLastName(),
                     admin->getFirstName(),
                     admin->getMiddleName(),
                     admin->getLogin()});
    }
    return data;
}

CustomList<QStringList> UserManager::getPendingUsersData() const {
    CustomList<QStringList> data;
    for (const User* user : m_pendingUsers) {
        data.append({user->getLastName(),
                     user->getFirstName(),
                     user->getMiddleName(),
                     user->getRoleString()}); // Показываем роль в таблице
    }
    return data;
}

bool UserManager::removeEmployeeByLogin(const QString& login) {
    for (int i = 0; i < m_employees.size(); ++i) {
        if (m_employees[i]->getLogin() == login) {
            delete m_employees[i];
            m_employees.removeAt(i);
            saveUsers();
            return true;
        }
    }
    return false;
}

bool UserManager::removeAdminByLogin(const QString& login) {
    for (int i = 0; i < m_admins.size(); ++i) {
        if (m_admins[i]->getLogin() == login) {
            delete m_admins[i];
            m_admins.removeAt(i);
            saveUsers();
            return true;
        }
    }
    return false;
}

bool UserManager::removePendingUserByName(const QString& fullName) {
    for (int i = 0; i < m_pendingUsers.size(); ++i) {
        if (m_pendingUsers[i]->getFullName() == fullName) {
            delete m_pendingUsers[i];
            m_pendingUsers.removeAt(i);
            saveUsers();
            return true;
        }
    }
    return false;
}
