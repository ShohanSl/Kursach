#include "validationexception.h"

ValidationException::ValidationException(const QString& msg)
    : AppException(msg) {}
