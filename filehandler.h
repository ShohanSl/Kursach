#pragma once
#include <QString>

/*
 * Централизованная работа с файлами.
 */
class FileHandler {
public:
    static QString readAll(const QString& path);
    static void writeAll(const QString& path, const QString& text);
    static void append(const QString& path, const QString& text);
};
