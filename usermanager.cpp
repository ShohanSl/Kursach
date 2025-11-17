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
    clearAllUsers();

    // Создаем тестового администратора
    Administrator* admin = new Administrator("Иванов", "Админ", "Системович",
                                             "admin", "admin123");
    m_admins.append(admin);

    // Создаем тестового работника
    Employee* worker = new Employee("Петров", "Работник", "Складской",
                                    "worker", "work123");
    m_employees.append(worker);

    // Создаем незарегистрированных пользователей с разными ролями
    User* pendingAdmin = new User("Сидоров", "Новый", "Администратор");
    User* pendingWorker = new User("Кузнецов", "Новый", "Сотрудник");

    m_pendingUsers.append(pendingAdmin);
    m_pendingUsers.append(pendingWorker);

    // Сохраняем тестовые данные
    saveUsers();

    qDebug() << "Тестовые данные инициализированы";
}

void UserManager::clearAllUsers()
{
    // Очищаем все списки с освобождением памяти
    qDeleteAll(m_pendingUsers);
    m_pendingUsers.clear();

    qDeleteAll(m_employees);
    m_employees.clear();

    qDeleteAll(m_admins);
    m_admins.clear();

    // Удаляем файлы если существуют
    QFile::remove(m_pendingUsersFile);
    QFile::remove(m_employeesFile);
    QFile::remove(m_adminsFile);
}

Employee* UserManager::authenticateUser(const QString& login, const QString& password)
{
    // Проверяем администраторов
    for (Administrator* admin : m_admins) {
        if (admin->authenticate(login, password)) {
            return admin; // Возвращаем как Employee* (upcast)
        }
    }

    // Проверяем сотрудников
    for (Employee* employee : m_employees) {
        if (employee->authenticate(login, password)) {
            return employee;
        }
    }

    return nullptr;
}

bool UserManager::registerPendingUser(const QString& lastName, const QString& firstName,
                                      const QString& middleName, bool isAdmin)
{
    QString fullName = lastName + " " + firstName + " " + middleName;

    // Проверяем, нет ли уже пользователя с таким ФИО в ожидающих
    if (isUserInPending(fullName)) {
        return false;
    }

    User* newUser = new User(lastName, firstName, middleName);
    m_pendingUsers.append(newUser);
    saveUsers();
    return true;
}

User* UserManager::getPendingUser(const QString& fullName)
{
    return findPendingUserByName(fullName);
}

bool UserManager::completeRegistration(const QString& fullName, const QString& login,
                                       const QString& password)
{
    qDebug() << "Начало регистрации для:" << fullName;

    User* pendingUser = findPendingUserByName(fullName);
    if (!pendingUser) {
        qDebug() << "Пользователь не найден в pending:" << fullName;
        return false;
    }

    // Проверяем, не занят ли логин в employees
    for (Employee* employee : m_employees) {
        if (employee->getLogin() == login) {
            qDebug() << "Логин уже занят employee:" << login;
            return false;
        }
    }

    // Проверяем, не занят ли логин в admins
    for (Administrator* admin : m_admins) {
        if (admin->getLogin() == login) {
            qDebug() << "Логин уже занят admin:" << login;
            return false;
        }
    }

    // Создаем нового сотрудника или администратора на основе pending пользователя
    // В реальной системе здесь должна быть логика определения роли
    // Пока создаем как сотрудника по умолчанию
    Employee* newEmployee = new Employee(pendingUser->getLastName(),
                                         pendingUser->getFirstName(),
                                         pendingUser->getMiddleName(),
                                         login, password);

    // Удаляем из ожидания
    bool removed = false;
    for (int i = 0; i < m_pendingUsers.size(); ++i) {
        if (m_pendingUsers[i]->getFullName().trimmed() == fullName.trimmed()) {
            delete m_pendingUsers[i]; // Освобождаем память
            m_pendingUsers.removeAt(i);
            removed = true;
            qDebug() << "Пользователь удален из pending:" << fullName;
            break;
        }
    }

    if (!removed) {
        delete newEmployee; // Очищаем память в случае ошибки
        qDebug() << "Не удалось удалить пользователя из pending:" << fullName;
        return false;
    }

    // Добавляем в список сотрудников
    m_employees.append(newEmployee);
    qDebug() << "Пользователь добавлен в employees:" << fullName;

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
        // Точное сравнение полного имени
        if (user->getFullName().trimmed() == fullName.trimmed()) {
            return user;
        }
    }
    return nullptr;
}

bool UserManager::isUserInPending(const QString& fullName)
{
    return findPendingUserByName(fullName) != nullptr;
}

// usermanager.cpp
QList<QStringList> UserManager::getEmployeesData() const {
    QList<QStringList> data;
    for (const Employee* employee : m_employees) {
        data.append({employee->getLastName(),
                     employee->getFirstName(),
                     employee->getMiddleName(),
                     employee->getLogin()});
    }
    return data;
}

QList<QStringList> UserManager::getAdminsData() const {
    QList<QStringList> data;
    for (const Administrator* admin : m_admins) {
        data.append({admin->getLastName(),
                     admin->getFirstName(),
                     admin->getMiddleName(),
                     admin->getLogin()});
    }
    return data;
}

QList<QStringList> UserManager::getPendingUsersData() const {
    QList<QStringList> data;
    for (const User* user : m_pendingUsers) {
        data.append({user->getLastName(),
                     user->getFirstName(),
                     user->getMiddleName()});
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
