#pragma once
#include <QString>
#include <exception>

/*
 * Базовое исключение для всего проекта.
 * Использует QString — удобно для Qt.
 */
class AppException : public std::exception {
public:
    explicit AppException(const QString& msg);

    const char* what() const noexcept override;
    QString qmessage() const;

protected:
    QString message;
    mutable QByteArray utf8;
};
