// exceptionhandler.cpp
#include "exceptionhandler.h"
#include <QDir>

ExceptionHandler::ExceptionHandler(ErrorSeverity severity, ErrorSource source,
                                   const QString& message, const QString& details,
                                   const QString& file, int line)
    : m_severity(severity), m_source(source), m_message(message),
    m_details(details), m_file(file), m_line(line),
    m_timestamp(QDateTime::currentDateTime())
{
}

ExceptionHandler::ExceptionHandler(ErrorSeverity severity, ErrorSource source,
                                   const QString& message, const QException& cause,
                                   const QString& file, int line)
    : m_severity(severity), m_source(source), m_message(message),
    m_file(file), m_line(line), m_timestamp(QDateTime::currentDateTime())
{
    m_details = QString("Причина: %1").arg(cause.what());
}

QString ExceptionHandler::formattedMessage() const
{
    QString formatted;

    formatted += QString("[%1] %2\n")
                     .arg(m_timestamp.toString("dd.MM.yyyy HH:mm:ss"))
                     .arg(severityToString(m_severity));

    formatted += QString("Источник: %1\n").arg(sourceToString(m_source));
    formatted += QString("Сообщение: %1\n").arg(m_message);

    if (!m_details.isEmpty()) {
        formatted += QString("Детали: %1\n").arg(m_details);
    }

    if (!m_file.isEmpty() && m_line != -1) {
        formatted += QString("Место: %1:%2\n").arg(m_file).arg(m_line);
    }

    return formatted;
}

void ExceptionHandler::handle(const ExceptionHandler& exception, QWidget* parent)
{
    // Логируем ошибку
    logToFile(exception);

    // Определяем, показывать ли сообщение пользователю
    if (exception.severity() >= ErrorSeverity::ERROR) {
        QString title;
        switch (exception.severity()) {
        case ErrorSeverity::ERROR: title = "Ошибка"; break;
        case ErrorSeverity::CRITICAL: title = "Критическая ошибка"; break;
        case ErrorSeverity::FATAL: title = "Фатальная ошибка"; break;
        default: title = "Сообщение"; break;
        }

        showMessageBox(exception.severity(), title, exception.message(), parent);
    }

    // Если фатальная ошибка - завершаем программу
    if (exception.severity() == ErrorSeverity::FATAL) {
        QCoreApplication::exit(1);
    }
}

void ExceptionHandler::logToFile(const ExceptionHandler& exception, const QString& filename)
{
    QFile file(filename);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << exception.formattedMessage() << "\n";
        stream << "----------------------------------------\n";
        file.close();
    }
}

void ExceptionHandler::showMessageBox(ErrorSeverity severity, const QString& title,
                                      const QString& message, QWidget* parent)
{
    QMessageBox msgBox(parent);
    msgBox.setWindowTitle(title);
    msgBox.setText(message);

    // Настраиваем иконку в зависимости от серьезности
    switch (severity) {
    case ErrorSeverity::INFO:
        msgBox.setIcon(QMessageBox::Information);
        break;
    case ErrorSeverity::WARNING:
        msgBox.setIcon(QMessageBox::Warning);
        break;
    case ErrorSeverity::ERROR:
    case ErrorSeverity::CRITICAL:
        msgBox.setIcon(QMessageBox::Critical);
        break;
    case ErrorSeverity::FATAL:
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setStandardButtons(QMessageBox::Ok);
        break;
    default:
        msgBox.setIcon(QMessageBox::NoIcon);
    }

    msgBox.exec();
}

ExceptionHandler ExceptionHandler::createFileError(const QString& filename,
                                                   const QString& operation,
                                                   const QString& details)
{
    QString message = QString("Ошибка %1 файла: %2").arg(operation).arg(filename);
    return ExceptionHandler(ErrorSeverity::ERROR, ErrorSource::FILE_OPERATION,
                            message, details, __FILE__, __LINE__);
}

ExceptionHandler ExceptionHandler::createAuthError(const QString& username,
                                                   const QString& reason,
                                                   const QString& details)
{
    QString message = QString("Ошибка аутентификации для пользователя '%1': %2")
                          .arg(username).arg(reason);
    return ExceptionHandler(ErrorSeverity::WARNING, ErrorSource::AUTHENTICATION,
                            message, details, __FILE__, __LINE__);
}

ExceptionHandler ExceptionHandler::createValidationError(const QString& field,
                                                         const QString& rule,
                                                         const QString& details)
{
    QString message = QString("Ошибка валидации поля '%1': %2").arg(field).arg(rule);
    return ExceptionHandler(ErrorSeverity::WARNING, ErrorSource::VALIDATION,
                            message, details, __FILE__, __LINE__);
}

ExceptionHandler ExceptionHandler::createPermissionError(const QString& action,
                                                         const QString& requiredRole,
                                                         const QString& details)
{
    QString message = QString("Отказано в доступе к '%1'. Требуется роль: %2")
                          .arg(action).arg(requiredRole);
    return ExceptionHandler(ErrorSeverity::WARNING, ErrorSource::PERMISSION,
                            message, details, __FILE__, __LINE__);
}

ExceptionHandler ExceptionHandler::createDatabaseError(const QString& operation,
                                                       const QString& details)
{
    QString message = QString("Ошибка базы данных при %1").arg(operation);
    return ExceptionHandler(ErrorSeverity::ERROR, ErrorSource::DATABASE,
                            message, details, __FILE__, __LINE__);
}

QString ExceptionHandler::severityToString(ErrorSeverity severity)
{
    switch (severity) {
    case ErrorSeverity::DEBUG: return "DEBUG";
    case ErrorSeverity::INFO: return "INFO";
    case ErrorSeverity::WARNING: return "WARNING";
    case ErrorSeverity::ERROR: return "ERROR";
    case ErrorSeverity::CRITICAL: return "CRITICAL";
    case ErrorSeverity::FATAL: return "FATAL";
    default: return "UNKNOWN";
    }
}

QString ExceptionHandler::sourceToString(ErrorSource source)
{
    switch (source) {
    case ErrorSource::USER_INPUT: return "USER_INPUT";
    case ErrorSource::FILE_OPERATION: return "FILE_OPERATION";
    case ErrorSource::DATABASE: return "DATABASE";
    case ErrorSource::NETWORK: return "NETWORK";
    case ErrorSource::AUTHENTICATION: return "AUTHENTICATION";
    case ErrorSource::PERMISSION: return "PERMISSION";
    case ErrorSource::VALIDATION: return "VALIDATION";
    case ErrorSource::SYSTEM: return "SYSTEM";
    default: return "UNKNOWN";
    }
}
