// usermanager.cpp
#include "usermanager.h"
#include <QFile>
#include <QDataStream>
#include <QDir>
#include <QDebug>
#include "fileexception.h"        // Добавляем
#include "validationexception.h"  // Добавляем
#include "appexception.h"         // Добавляем

UserManager::UserManager()
{
    try {
        loadUsers();
    } catch (const AppException& e) {
        qCritical() << "Критическая ошибка при загрузке пользователей:" << e.qmessage();
        throw;
    }
}

User* UserManager::authenticateUser(const QString& login, const QString& password)
{
    // Простой поиск, без исключений
    for (Administrator* admin : m_admins) {
        if (admin->authenticate(login, password)) {
            return admin;
        }
    }

    for (Employee* employee : m_employees) {
        if (employee->authenticate(login, password)) {
            return employee;
        }
    }

    return nullptr;
}

bool UserManager::registerPendingUser(const QString& lastName, const QString& firstName,
                                      const QString& middleName, UserRole role)
{
    QString fullName = lastName + " " + firstName + " " + middleName;

    // ТОЛЬКО бизнес-проверка: нет ли уже пользователя с таким ФИО
    if (isUserInPending(fullName)) {
        throw ValidationException(QString("Пользователь с ФИО '%1' уже существует в списке ожидания")
                                      .arg(fullName));
    }

    User* newUser = new User(lastName, firstName, middleName, role);
    m_pendingUsers.append(newUser);

    try {
        saveUsers();
        return true;
    } catch (const FileException& e) {
        // Откатываем изменения в памяти при ошибке сохранения
        m_pendingUsers.removeOne(newUser);
        delete newUser;
        throw;
    }
}

bool UserManager::completeRegistration(const QString& fullName, const QString& login,
                                       const QString& password)
{
    // ТОЛЬКО бизнес-проверки

    User* pendingUser = findPendingUserByName(fullName);
    if (!pendingUser) {
        throw ValidationException(QString("Пользователь с ФИО '%1' не найден в списке ожидания")
                                      .arg(fullName));
    }

    UserRole role = pendingUser->getRole();

    // Проверка уникальности логина
    if (findEmployeeByLogin(login) || findAdminByLogin(login)) {
        throw ValidationException(QString("Логин '%1' уже занят").arg(login));
    }

    User* newUser = nullptr;

    if (role == UserRole::Administrator) {
        Administrator* newAdmin = new Administrator(pendingUser->getLastName(),
                                                    pendingUser->getFirstName(),
                                                    pendingUser->getMiddleName(),
                                                    login, password);
        m_admins.append(newAdmin);
        newUser = newAdmin;
    } else {
        Employee* newEmployee = new Employee(pendingUser->getLastName(),
                                             pendingUser->getFirstName(),
                                             pendingUser->getMiddleName(),
                                             login, password);
        m_employees.append(newEmployee);
        newUser = newEmployee;
    }

    m_pendingUsers.removeOne(pendingUser);
    delete pendingUser;

    try {
        saveUsers();
        return true;
    } catch (const FileException& e) {
        // Откатываем изменения при ошибке сохранения
        if (newUser) {
            if (role == UserRole::Administrator) {
                m_admins.removeOne(static_cast<Administrator*>(newUser));
            } else {
                m_employees.removeOne(static_cast<Employee*>(newUser));
            }
            delete newUser;
        }
        throw;
    }
}

void UserManager::loadUsers()
{
    try {
        // Загружаем ожидающих пользователей (базовые User)
        QFile file(m_pendingUsersFile);
        if (file.exists()) {
            if (!file.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл ожидающих пользователей '%1' для чтения\nОшибка: %2")
                                        .arg(m_pendingUsersFile, file.errorString()));
            }

            QDataStream in(&file);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                file.close();
                throw FileException(QString("Ошибка чтения размера данных из файла ожидающих пользователей '%1'")
                                        .arg(m_pendingUsersFile));
            }

            for (quint32 i = 0; i < size; ++i) {
                User* user = new User();
                in >> *user;

                if (in.status() != QDataStream::Ok) {
                    file.close();
                    throw FileException(QString("Ошибка чтения ожидающего пользователя №%1 из файла")
                                            .arg(i + 1));
                }

                m_pendingUsers.append(user);
            }
            file.close();
        }

        // Загружаем сотрудников
        file.setFileName(m_employeesFile);
        if (file.exists()) {
            if (!file.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл сотрудников '%1' для чтения\nОшибка: %2")
                                        .arg(m_employeesFile, file.errorString()));
            }

            QDataStream in(&file);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                file.close();
                throw FileException(QString("Ошибка чтения размера данных из файла сотрудников '%1'")
                                        .arg(m_employeesFile));
            }

            for (quint32 i = 0; i < size; ++i) {
                Employee* employee = new Employee();
                in >> *employee;

                if (in.status() != QDataStream::Ok) {
                    file.close();
                    throw FileException(QString("Ошибка чтения сотрудника №%1 из файла")
                                            .arg(i + 1));
                }

                m_employees.append(employee);
            }
            file.close();
        }

        // Загружаем администраторов
        file.setFileName(m_adminsFile);
        if (file.exists()) {
            if (!file.open(QIODevice::ReadOnly)) {
                throw FileException(QString("Не удалось открыть файл администраторов '%1' для чтения\nОшибка: %2")
                                        .arg(m_adminsFile, file.errorString()));
            }

            QDataStream in(&file);
            quint32 size;
            in >> size;

            if (in.status() != QDataStream::Ok) {
                file.close();
                throw FileException(QString("Ошибка чтения размера данных из файла администраторов '%1'")
                                        .arg(m_adminsFile));
            }

            for (quint32 i = 0; i < size; ++i) {
                Administrator* admin = new Administrator();
                in >> *admin;

                if (in.status() != QDataStream::Ok) {
                    file.close();
                    throw FileException(QString("Ошибка чтения администратора №%1 из файла")
                                            .arg(i + 1));
                }

                m_admins.append(admin);
            }
            file.close();
        }

    } catch (const AppException& e) {
        throw; // Пробрасываем дальше
    }
}

void UserManager::saveUsers()
{
    try {
        // Создаем папку если нужно
        QDir().mkpath(".");

        // Сохраняем ожидающих пользователей
        QFile file(m_pendingUsersFile);
        if (!file.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл ожидающих пользователей '%1' для записи\nОшибка: %2")
                                    .arg(m_pendingUsersFile, file.errorString()));
        }

        QDataStream out(&file);
        out << static_cast<quint32>(m_pendingUsers.size());
        for (const User* user : m_pendingUsers) {
            out << *user;

            if (out.status() != QDataStream::Ok) {
                file.close();
                throw FileException("Ошибка записи ожидающего пользователя в файл");
            }
        }
        file.close();

        // Сохраняем сотрудников
        file.setFileName(m_employeesFile);
        if (!file.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл сотрудников '%1' для записи\nОшибка: %2")
                                    .arg(m_employeesFile, file.errorString()));
        }

        QDataStream out2(&file);
        out2 << static_cast<quint32>(m_employees.size());
        for (const Employee* employee : m_employees) {
            out2 << *employee;

            if (out2.status() != QDataStream::Ok) {
                file.close();
                throw FileException("Ошибка записи сотрудника в файл");
            }
        }
        file.close();

        // Сохраняем администраторов
        file.setFileName(m_adminsFile);
        if (!file.open(QIODevice::WriteOnly)) {
            throw FileException(QString("Не удалось открыть файл администраторов '%1' для записи\nОшибка: %2")
                                    .arg(m_adminsFile, file.errorString()));
        }

        QDataStream out3(&file);
        out3 << static_cast<quint32>(m_admins.size());
        for (const Administrator* admin : m_admins) {
            out3 << *admin;

            if (out3.status() != QDataStream::Ok) {
                file.close();
                throw FileException("Ошибка записи администратора в файл");
            }
        }
        file.close();

    } catch (const AppException& e) {
        throw; // Пробрасываем дальше
    }
}

User* UserManager::findPendingUserByName(const QString& fullName)
{
    // Простой поиск, исключений быть не должно
    for (User* user : m_pendingUsers) {
        if (user->getFullName() == fullName) {
            return user;
        }
    }
    return nullptr;
}

Employee* UserManager::findEmployeeByLogin(const QString& login)
{
    // Простой поиск
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
    try {
        CustomList<QStringList> data;
        for (const Employee* employee : m_employees) {
            data.append({employee->getLastName(),
                         employee->getFirstName(),
                         employee->getMiddleName(),
                         employee->getLogin()});
        }
        return data;
    } catch (const std::exception& e) {
        qWarning() << "Ошибка при получении данных сотрудников:" << e.what();
        return CustomList<QStringList>();
    }
}

CustomList<QStringList> UserManager::getAdminsData() const {
    try {
        CustomList<QStringList> data;
        for (const Administrator* admin : m_admins) {
            data.append({admin->getLastName(),
                         admin->getFirstName(),
                         admin->getMiddleName(),
                         admin->getLogin()});
        }
        return data;
    } catch (const std::exception& e) {
        qWarning() << "Ошибка при получении данных администраторов:" << e.what();
        return CustomList<QStringList>();
    }
}

CustomList<QStringList> UserManager::getPendingUsersData() const {
    try {
        CustomList<QStringList> data;
        for (const User* user : m_pendingUsers) {
            data.append({user->getLastName(),
                         user->getFirstName(),
                         user->getMiddleName(),
                         user->getRoleString()});
        }
        return data;
    } catch (const std::exception& e) {
        qWarning() << "Ошибка при получении данных ожидающих пользователей:" << e.what();
        return CustomList<QStringList>();
    }
}

bool UserManager::removeEmployeeByLogin(const QString& login) {
    try {
        for (int i = 0; i < m_employees.size(); ++i) {
            if (m_employees[i]->getLogin() == login) {
                delete m_employees[i];
                m_employees.removeAt(i);
                saveUsers();
                return true;
            }
        }
        return false;
    } catch (const FileException& e) {
        // При ошибке сохранения возвращаем false
        qWarning() << "Ошибка при сохранении после удаления сотрудника:" << e.qmessage();
        return false;
    }
}

bool UserManager::removeAdminByLogin(const QString& login) {
    try {
        for (int i = 0; i < m_admins.size(); ++i) {
            if (m_admins[i]->getLogin() == login) {
                delete m_admins[i];
                m_admins.removeAt(i);
                saveUsers();
                return true;
            }
        }
        return false;
    } catch (const FileException& e) {
        qWarning() << "Ошибка при сохранении после удаления администратора:" << e.qmessage();
        return false;
    }
}

bool UserManager::removePendingUserByName(const QString& fullName) {
    try {
        for (int i = 0; i < m_pendingUsers.size(); ++i) {
            if (m_pendingUsers[i]->getFullName() == fullName) {
                delete m_pendingUsers[i];
                m_pendingUsers.removeAt(i);
                saveUsers();
                return true;
            }
        }
        return false;
    } catch (const FileException& e) {
        qWarning() << "Ошибка при сохранении после удаления ожидающего пользователя:" << e.qmessage();
        return false;
    }
}
