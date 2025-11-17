#include "errorhandler.h"

ErrorHandler::ErrorHandler(ErrorType type, const QString& message)
    : m_type(type), m_message(message)
{
}

const char* ErrorHandler::what() const noexcept
{
    return m_message.toStdString().c_str();
}

ErrorHandler::ErrorType ErrorHandler::getType() const
{
    return m_type;
}

QString ErrorHandler::getMessage() const
{
    return m_message;
}
