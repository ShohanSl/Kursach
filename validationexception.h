#pragma once
#include "appexception.h"

/*
 * Исключение ошибок пользовательского ввода.
 */
class ValidationException : public AppException {
public:
    explicit ValidationException(const QString& msg);
};
