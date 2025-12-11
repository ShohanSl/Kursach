#include <QApplication>
#include "loginwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::Russia));

    LoginWindow loginWindow;
    loginWindow.show();

    return app.exec();
}
