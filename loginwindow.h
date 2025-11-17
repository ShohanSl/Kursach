#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "usermanager.h"
#include "registrationwindow.h"
#include "employee.h"  // Добавляем новый заголовок

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    void setupUI();
    void applyStyle();

    QWidget *centralWidget;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *initTestDataButton;
    UserManager *userManager;
};

#endif // LOGINWINDOW_H
