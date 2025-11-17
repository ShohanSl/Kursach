#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>
#include "loginwindow.h"
#include "warehousewindow.h"
#include "deliverywindow.h"
#include "accountmanagementwindow.h"
#include "usermanager.h"

MainWindow::MainWindow(bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    applyStyle();
    setWindowTitle("Складской учет - Главное меню");
    setFixedSize(650, 550);
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    titleLabel = new QLabel("ГЛАВНОЕ МЕНЮ");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(50);

    QString roleText = m_isAdmin ? "Администратор" : "Сотрудник";
    roleLabel = new QLabel("Роль: " + roleText);
    roleLabel->setAlignment(Qt::AlignCenter);
    roleLabel->setMinimumHeight(30);

    QWidget *contentWidget = new QWidget();
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    contentLayout->setSpacing(25);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *leftColumn = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftColumn);
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    warehouseButton = new QPushButton("Склад");
    warehouseButton->setMinimumSize(250, 50);
    warehouseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    deliveryButton = new QPushButton("Оформить поставку");
    deliveryButton->setMinimumSize(250, 50);
    deliveryButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    leftLayout->addWidget(warehouseButton);
    leftLayout->addWidget(deliveryButton);

    if (m_isAdmin) {
        manageAccountsButton = new QPushButton("Управление аккаунтами");
        manageAccountsButton->setMinimumSize(250, 50);
        manageAccountsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        leftLayout->addWidget(manageAccountsButton);
    } else {
        QWidget *adminSpace = new QWidget();
        adminSpace->setMinimumSize(250, 50);
        adminSpace->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        leftLayout->addWidget(adminSpace);
    }

    leftLayout->addStretch();

    QWidget *rightColumn = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightColumn);
    rightLayout->setSpacing(15);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    shipmentButton = new QPushButton("Оформить отгрузку");
    shipmentButton->setMinimumSize(250, 50);
    shipmentButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    transferButton = new QPushButton("Трансфер по складу");
    transferButton->setMinimumSize(250, 50);
    transferButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    logoutButton = new QPushButton("Выйти из аккаунта");
    logoutButton->setMinimumSize(250, 50);
    logoutButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    rightLayout->addWidget(shipmentButton);
    rightLayout->addWidget(transferButton);
    rightLayout->addWidget(logoutButton);
    rightLayout->addStretch();

    contentLayout->addWidget(leftColumn);
    contentLayout->addWidget(rightColumn);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(roleLabel);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(contentWidget);
    mainLayout->addStretch();

    connect(warehouseButton, &QPushButton::clicked, this, &MainWindow::onWarehouseClicked);
    connect(deliveryButton, &QPushButton::clicked, this, &MainWindow::onDeliveryClicked);
    connect(shipmentButton, &QPushButton::clicked, this, &MainWindow::onShipmentClicked);
    connect(transferButton, &QPushButton::clicked, this, &MainWindow::onTransferClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    if (m_isAdmin) {
        connect(manageAccountsButton, &QPushButton::clicked, this, &MainWindow::onManageAccountsClicked);
    }
}

void MainWindow::applyStyle()
{
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f0f0f0;
        }
        QWidget {
            background-color: #f0f0f0;
        }
        QLabel {
            color: #2c3e50;
            font-size: 16px;
            padding: 10px;
            margin: 5px;
        }
        QLabel[objectName="titleLabel"] {
            font-size: 24px;
            font-weight: bold;
            color: #2c3e50;
            padding: 15px;
        }
        QLabel[objectName="roleLabel"] {
            font-size: 14px;
            color: #7f8c8d;
            font-style: italic;
            padding: 8px;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 15px;
            padding: 25px 15px;
            font-size: 16px;
            font-weight: bold;
            margin: 0px;
            min-width: 250px;
            min-height: 50px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #2471a3;
        }
        QPushButton#logoutButton {
            background-color: #e74c3c;
        }
        QPushButton#logoutButton:hover {
            background-color: #c0392b;
        }
        QPushButton#manageAccountsButton {
            background-color: #9b59b6;
        }
        QPushButton#manageAccountsButton:hover {
            background-color: #8e44ad;
        }
        QPushButton#transferButton {
            background-color: #f39c12;
        }
        QPushButton#transferButton:hover {
            background-color: #e67e22;
        }
    )");

    titleLabel->setObjectName("titleLabel");
    roleLabel->setObjectName("roleLabel");
    logoutButton->setObjectName("logoutButton");
    transferButton->setObjectName("transferButton");

    if (m_isAdmin) {
        manageAccountsButton->setObjectName("manageAccountsButton");
    }
}

void MainWindow::onWarehouseClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "view", m_userManager);
    warehouseWindow->show();
    this->close();
}

void MainWindow::onDeliveryClicked()
{
    DeliveryWindow *deliveryWindow = new DeliveryWindow(m_isAdmin, m_userManager);
    deliveryWindow->show();
    this->close();
}

void MainWindow::onShipmentClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "shipment", m_userManager);
    warehouseWindow->show();
    this->close();
}

void MainWindow::onTransferClicked()
{
    WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "transfer", m_userManager);
    warehouseWindow->show();
    this->close();
}

void MainWindow::onManageAccountsClicked()
{
    if (!m_userManager) {
        QMessageBox::critical(this, "Ошибка", "UserManager не инициализирован");
        return;
    }

    AccountManagementWindow *accountWindow = new AccountManagementWindow(m_userManager);
    accountWindow->show();
    this->close();
}

void MainWindow::onLogoutClicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Выход");
    msgBox.setText("Вы уверены, что хотите выйти из аккаунта?");

    QPushButton *yesButton = msgBox.addButton("Да", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("Нет", QMessageBox::NoRole);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        LoginWindow *loginWindow = new LoginWindow();
        loginWindow->show();
        this->close();
    }
}
