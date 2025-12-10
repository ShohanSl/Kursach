// exceptionhandler.h
#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <QString>
#include <QDateTime>
#include <QException>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

enum class ErrorSeverity {
    DEBUG,      // Отладочная информация
    INFO,       // Информационное сообщение
    WARNING,    // Предупреждение
    ERROR,      // Ошибка, но программа может продолжать
    CRITICAL,   // Критическая ошибка, требуется вмешательство
    FATAL       // Фатальная ошибка, программа не может продолжать
};

enum class ErrorSource {
    USER_INPUT,     // Ошибка ввода пользователя
    FILE_OPERATION, // Ошибка работы с файлами
    DATABASE,       // Ошибка базы данных
    NETWORK,        // Сетевая ошибка
    AUTHENTICATION, // Ошибка аутентификации
    PERMISSION,     // Ошибка прав доступа
    VALIDATION,     // Ошибка валидации данных
    SYSTEM,         // Системная ошибка
    UNKNOWN         // Неизвестная ошибка
};

class ExceptionHandler : public QException
{
public:
    // Конструкторы
    ExceptionHandler(ErrorSeverity severity, ErrorSource source,
                     const QString& message, const QString& details = "",
                     const QString& file = "", int line = -1);

    ExceptionHandler(ErrorSeverity severity, ErrorSource source,
                     const QString& message, const QException& cause,
                     const QString& file = "", int line = -1);

    // Деструктор
    ~ExceptionHandler() noexcept override = default;

    // Методы для получения информации об ошибке
    ErrorSeverity severity() const { return m_severity; }
    ErrorSource source() const { return m_source; }
    QString message() const { return m_message; }
    QString details() const { return m_details; }
    QString file() const { return m_file; }
    int line() const { return m_line; }
    QDateTime timestamp() const { return m_timestamp; }
    QString formattedMessage() const;

    // QException интерфейс
    void raise() const override { throw *this; }
    ExceptionHandler* clone() const override { return new ExceptionHandler(*this); }

    // Статические методы для обработки ошибок
    static void handle(const ExceptionHandler& exception, QWidget* parent = nullptr);
    static void logToFile(const ExceptionHandler& exception, const QString& filename = "error_log.txt");
    static void showMessageBox(ErrorSeverity severity, const QString& title,
                               const QString& message, QWidget* parent = nullptr);

    // Утилиты для быстрого создания исключений
    static ExceptionHandler createFileError(const QString& filename,
                                            const QString& operation,
                                            const QString& details = "");

    static ExceptionHandler createAuthError(const QString& username,
                                            const QString& reason,
                                            const QString& details = "");

    static ExceptionHandler createValidationError(const QString& field,
                                                  const QString& rule,
                                                  const QString& details = "");

    static ExceptionHandler createPermissionError(const QString& action,
                                                  const QString& requiredRole,
                                                  const QString& details = "");

    static ExceptionHandler createDatabaseError(const QString& operation,
                                                const QString& details = "");

private:
    ErrorSeverity m_severity;
    ErrorSource m_source;
    QString m_message;
    QString m_details;
    QString m_file;
    int m_line;
    QDateTime m_timestamp;

    static QString severityToString(ErrorSeverity severity);
    static QString sourceToString(ErrorSource source);
};

// Макросы для удобного использования (ИСПРАВЛЕННЫЕ - используем do-while)
#define THROW_EXCEPTION(severity, source, message, details) \
throw ExceptionHandler(severity, source, message, details, __FILE__, __LINE__)

#define THROW_FILE_ERROR(filename, operation, details) \
    throw ExceptionHandler::createFileError(filename, operation, details)

#define THROW_AUTH_ERROR(username, reason, details) \
    throw ExceptionHandler::createAuthError(username, reason, details)

#define THROW_VALIDATION_ERROR(field, rule, details) \
    throw ExceptionHandler::createValidationError(field, rule, details)

#define THROW_PERMISSION_ERROR(action, requiredRole, details) \
    throw ExceptionHandler::createPermissionError(action, requiredRole, details)

// ИСПРАВЛЕННЫЙ макрос TRY_CATCH - используем do-while для безопасности
#define TRY_CATCH_BEGIN try {
#define TRY_CATCH_END \
} catch (const ExceptionHandler& e) { \
        ExceptionHandler::handle(e); \
} catch (const std::exception& e) { \
        ExceptionHandler::handle(ExceptionHandler(ErrorSeverity::ERROR, ErrorSource::UNKNOWN, \
                                                  "Стандартное исключение", e.what(), __FILE__, __LINE__)); \
} catch (...) { \
        ExceptionHandler::handle(ExceptionHandler(ErrorSeverity::ERROR, ErrorSource::UNKNOWN, \
                                                  "Неизвестное исключение", "", __FILE__, __LINE__)); \
}

#endif // EXCEPTIONHANDLER_H
