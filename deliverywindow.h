#ifndef DELIVERYWINDOW_H
#define DELIVERYWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateEdit>
#include <QScrollArea>
#include "usermanager.h"

class DeliveryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit DeliveryWindow(bool isAdmin, UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onAddProductClicked();
    void onRemoveProductClicked();
    void onCompleteDeliveryClicked();
    void onBackClicked();
    void onProductTypeChanged(int index);

private:
    void setupUI();
    void applyStyle();
    void addProductFields();
    void updateSectionComboBox(QComboBox* sectionCombo, const QString& productType);
    void saveDeliveryToFiles();

    bool m_isAdmin;
    bool validateAndPrepareDelivery();

    QWidget *centralWidget;
    QScrollArea *scrollArea;
    QWidget *scrollContent;

    QLineEdit *supplierEdit;
    QDateEdit *dateEdit;

    QVBoxLayout *productsLayout;
    CustomList<QWidget*> productWidgets;

    QPushButton *completeButton;
    QPushButton *backButton;
    UserManager* m_userManager;
};

#endif // DELIVERYWINDOW_H
