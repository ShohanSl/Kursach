#pragma once
#include <QString>
#include <QStringList>

class InputValidator {
public:
    enum class Mode {
        LettersHyphensSpaces,        // рус/англ буквы, пробелы, дефисы
        LatinAlnumHyphensUnderscore, // латиница, цифры, -, _
        NonEmpty                     // строка не пустая
    };

    static void validateOrThrow(const QString& text, Mode mode);

    // Новые методы для удобства
    static void validateNotEmptyOrThrow(const QString& text, const QString& fieldName);
    static void validateMinLengthOrThrow(const QString& text, int minLength, const QString& fieldName);
    static void validateNameOrThrow(const QString& text, const QString& fieldName);

    // Метод для проверки нескольких полей на пустоту
    static void validateAllFieldsNotEmptyOrThrow(const QList<QPair<QString, QString>>& fields);
};
