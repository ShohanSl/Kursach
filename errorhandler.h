#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QString>
#include <exception>

class ErrorHandler : public std::exception
{
public:
    enum ErrorType {
        FILE_ERROR,
        DATA_CORRUPTION,
        USER_NOT_FOUND,
        INVALID_CREDENTIALS,
        USER_ALREADY_EXISTS,
        PERMISSION_DENIED
    };

    explicit ErrorHandler(ErrorType type, const QString& message = "");
    const char* what() const noexcept override;
    ErrorType getType() const;
    QString getMessage() const;

private:
    ErrorType m_type;
    QString m_message;
};

#endif // ERRORHANDLER_H
