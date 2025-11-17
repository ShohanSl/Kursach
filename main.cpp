#include <QApplication>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Установка русского языка и кодировки
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));

    // Настройка стиля приложения
    app.setStyle("Fusion");

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
