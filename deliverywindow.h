#ifndef DELIVERYWINDOW_H
#define DELIVERYWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include "usermanager.h"

class DeliveryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DeliveryWindow(bool isAdmin, UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onCompleteDeliveryClicked();
    void onBackClicked();
    void onProductTypeChanged(int index);

private:
    void setupUI();
    void updateSectionComboBox();
    void saveDelivery();

    bool m_isAdmin;
    UserManager* m_userManager;

    QLineEdit *supplierEdit;
    QDateEdit *dateEdit;
    QComboBox *typeCombo;
    QLineEdit *nameEdit;
    QLineEdit *indexEdit;
    QLineEdit *quantityEdit;
    QComboBox *sectionCombo;
    QLineEdit *cellEdit;
};

#endif // DELIVERYWINDOW_H
