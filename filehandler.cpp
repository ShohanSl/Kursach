#include "fileHandler.h"
#include "fileException.h"

#include <QFile>
#include <QTextStream>

QString FileHandler::readAll(const QString& path) {
    QFile f(path);

    if (!f.exists()) {
        throw FileException(QString("Файл не найден: %1").arg(path));
    }

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw FileException(
            QString("Ошибка чтения файла %1: %2")
                .arg(path, f.errorString())
            );
    }

    QTextStream in(&f);
    return in.readAll();
}

void FileHandler::writeAll(const QString& path, const QString& text) {
    QFile f(path);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
        throw FileException(
            QString("Ошибка записи файла %1: %2")
                .arg(path, f.errorString())
            );
    }

    QTextStream out(&f);
    out << text;
}

void FileHandler::append(const QString& path, const QString& text) {
    QFile f(path);

    if (!f.open(QIODevice::Append | QIODevice::Text)) {
        throw FileException(
            QString("Ошибка дозаписи файла %1: %2")
                .arg(path, f.errorString())
            );
    }

    QTextStream out(&f);
    out << text;
}
