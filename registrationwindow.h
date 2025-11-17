#ifndef REGISTRATIONWINDOW_H
#define REGISTRATIONWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "usermanager.h"

class RegistrationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RegistrationWindow(UserManager *userManager, QWidget *parent = nullptr);

private slots:
    void onConfirmNameClicked();
    void onCompleteRegistrationClicked();
    void onBackClicked();

private:
    void setupUI();
    void applyStyle();
    void showRegistrationFields();

    UserManager *m_userManager;

    // Левая колонка - ФИО
    QWidget *leftColumn;
    QLineEdit *lastNameEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QPushButton *confirmNameButton;

    // Правая колонка - регистрация (изначально скрыта)
    QWidget *rightColumn;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QPushButton *completeRegistrationButton;

    QPushButton *backButton;
    QLabel *statusLabel;
};

#endif // REGISTRATIONWINDOW_H
