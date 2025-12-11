#ifndef ADDUSERWINDOW_H
#define ADDUSERWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "usermanager.h"

class AddUserWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddUserWindow(UserManager *userManager, QWidget *parent = nullptr);

private slots:
    void onAddUserClicked();
    void onBackClicked();

private:
    void setupUI();

    UserManager *m_userManager;

    QLineEdit *lastNameEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QComboBox *roleCombo;
};

#endif // ADDUSERWINDOW_H
