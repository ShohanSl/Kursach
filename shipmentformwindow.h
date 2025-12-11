#ifndef SHIPMENTFORMWINDOW_H
#define SHIPMENTFORMWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QDateEdit>
#include "product.h"
#include "operation.h"
#include "usermanager.h"

class ShipmentFormWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ShipmentFormWindow(const Product& product, int sectionNumber,
                                bool isAdmin, UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onCompleteShipmentClicked();
    void onBackClicked();

private:
    void setupUI();
    void saveShipment();

    Product m_product;
    int m_sectionNumber;
    bool m_isAdmin;
    UserManager* m_userManager;

    QLineEdit *customerEdit;
    QLineEdit *shipmentQuantityEdit;
    QDateEdit *dateEdit;
};

#endif // SHIPMENTFORMWINDOW_H
