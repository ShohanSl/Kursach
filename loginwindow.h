#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QPushButton;
class UserManager;

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    void setupUI();

    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QPushButton *loginButton;
    QPushButton *registerButton;
    UserManager *userManager;
};

#endif // LOGINWINDOW_H
