#include "inputValidator.h"
#include "validationexception.h"

#include <QRegularExpression>

void InputValidator::validateOrThrow(const QString& text, Mode mode) {

    switch (mode) {

    case Mode::LettersHyphensSpaces: {
        QRegularExpression re(
            "^[A-Za-zА-Яа-яЁё\\-\\s]+$",
            QRegularExpression::UseUnicodePropertiesOption
            );
        if (text.isEmpty() || !re.match(text).hasMatch()) {
            throw ValidationException(
                "Разрешены только русские/английские буквы, пробелы и дефисы."
                );
        }
        return;
    }

    case Mode::LatinAlnumHyphensUnderscore: {
        QRegularExpression re("^[A-Za-z0-9\\-_]+$");
        if (text.isEmpty() || !re.match(text).hasMatch()) {
            throw ValidationException(
                "Разрешены только латинские буквы, цифры, дефис и подчёркивание."
                );
        }
        return;
    }

    case Mode::NonEmpty: {
        if (text.trimmed().isEmpty()) {
            throw ValidationException("Поле не может быть пустым.");
        }
        return;
    }
    }
}

void InputValidator::validateNotEmptyOrThrow(const QString& text, const QString& fieldName) {
    if (text.trimmed().isEmpty()) {
        throw ValidationException(QString("Поле '%1' не может быть пустым").arg(fieldName));
    }
}

void InputValidator::validateMinLengthOrThrow(const QString& text, int minLength, const QString& fieldName) {
    if (text.trimmed().length() < minLength) {
        throw ValidationException(QString("Поле '%1' должно содержать минимум %2 символа(ов)")
                                      .arg(fieldName).arg(minLength));
    }
}

void InputValidator::validateNameOrThrow(const QString& text, const QString& fieldName) {
    validateNotEmptyOrThrow(text, fieldName);
    validateMinLengthOrThrow(text, 2, fieldName);

    QRegularExpression nameRegex("^[А-Яа-яЁёA-Za-z\\s\\-]+$");
    if (!nameRegex.match(text).hasMatch()) {
        throw ValidationException(QString("Поле '%1' может содержать только буквы, дефисы и пробелы")
                                      .arg(fieldName));
    }
}

void InputValidator::validateAllFieldsNotEmptyOrThrow(const QList<QPair<QString, QString>>& fields) {
    QStringList emptyFields;

    for (const auto& field : fields) {
        if (field.first.trimmed().isEmpty()) {
            emptyFields.append(field.second);
        }
    }

    if (!emptyFields.isEmpty()) {
        throw ValidationException("Заполните все поля");
    }
}
