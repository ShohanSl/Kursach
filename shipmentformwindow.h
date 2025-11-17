#ifndef SHIPMENTFORMWINDOW_H
#define SHIPMENTFORMWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
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
    void applyStyle();
    void saveShipmentToFile();

    Product m_product;
    int m_sectionNumber;
    bool m_isAdmin;

    QWidget *centralWidget;

    QLineEdit *productNameEdit;
    QLineEdit *productIndexEdit;
    QLineEdit *supplierEdit;
    QLineEdit *cellNumberEdit;
    QLineEdit *availableQuantityEdit;
    QLineEdit *customerEdit;
    QLineEdit *shipmentQuantityEdit;
    QDateEdit *dateEdit;

    QPushButton *completeButton;
    QPushButton *backButton;

    UserManager* m_userManager;
};

#endif // SHIPMENTFORMWINDOW_H
