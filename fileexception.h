#pragma once
#include "appexception.h"

/*
 * Исключение ошибок файловой системы:
 * - файл не найден
 * - нельзя открыть
 * - нельзя прочитать / записать
 */
class FileException : public AppException {
public:
    explicit FileException(const QString& msg);
};
