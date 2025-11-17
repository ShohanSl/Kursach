#ifndef ADDUSERWINDOW_H
#define ADDUSERWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
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
    void applyStyle();

    UserManager *m_userManager;

    QWidget *centralWidget;
    QLineEdit *lastNameEdit;
    QLineEdit *firstNameEdit;
    QLineEdit *middleNameEdit;
    QComboBox *roleCombo;
    QPushButton *addButton;
    QPushButton *backButton;
};

#endif // ADDUSERWINDOW_H
