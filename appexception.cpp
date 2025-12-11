#include "appexception.h"

AppException::AppException(const QString& msg)
    : message(msg) {}

const char* AppException::what() const noexcept {
    utf8 = message.toUtf8();
    return utf8.constData();
}

QString AppException::qmessage() const {
    return message;
}
