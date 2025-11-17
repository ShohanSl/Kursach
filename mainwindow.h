#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include "usermanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool isAdmin, UserManager* userManager, QWidget *parent = nullptr);

private slots:
    void onWarehouseClicked();
    void onDeliveryClicked();
    void onShipmentClicked();
    void onTransferClicked();
    void onLogoutClicked();
    void onManageAccountsClicked();

private:
    UserManager *m_userManager; // Добавляем указатель на UserManager

    void setupUI();
    void applyStyle();

    bool m_isAdmin;

    QWidget *centralWidget;
    QLabel *titleLabel;
    QLabel *roleLabel;

    QPushButton *warehouseButton;
    QPushButton *deliveryButton;
    QPushButton *manageAccountsButton;

    QPushButton *shipmentButton;
    QPushButton *transferButton;
    QPushButton *logoutButton;
};

#endif // MAINWINDOW_H
