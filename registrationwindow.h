#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "usermanager.h"

class RegistrationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationWindow(UserManager *userManager, QWidget *parent = nullptr);

private slots:
    void onCompleteRegistrationClicked();
    void onBackClicked();

private:
    void setupUI();

    UserManager *m_userManager;

    QLineEdit *lastNameEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *completeButton;
    QPushButton *backButton;
    QLabel *statusLabel;
};

#endif // REGISTRATIONWINDOW_H
