#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include "warehousewindow.h"
#include "deliverywindow.h"
#include "accountmanagementwindow.h"
#include "loginwindow.h"

MainWindow::MainWindow(bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Складской учет - Главное меню");
    setFixedSize(400, 500);
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок
    titleLabel = new QLabel("ГЛАВНОЕ МЕНЮ");
    titleLabel->setAlignment(Qt::AlignCenter);

    QString roleText = m_isAdmin ? "Администратор" : "Сотрудник";
    roleLabel = new QLabel("Роль: " + roleText);
    roleLabel->setAlignment(Qt::AlignCenter);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(roleLabel);

    // Кнопки действий
    warehouseButton = new QPushButton("Склад");
    deliveryButton = new QPushButton("Оформить поставку");
    shipmentButton = new QPushButton("Оформить отгрузку");
    transferButton = new QPushButton("Трансфер по складу");

    mainLayout->addWidget(warehouseButton);
    mainLayout->addWidget(deliveryButton);
    mainLayout->addWidget(shipmentButton);
    mainLayout->addWidget(transferButton);

    // Кнопка управления аккаунтами (только для администратора)
    if (m_isAdmin) {
        manageAccountsButton = new QPushButton("Управление аккаунтами");
        mainLayout->addWidget(manageAccountsButton);
    }

    // Кнопка выхода
    logoutButton = new QPushButton("Выйти из аккаунта");
    mainLayout->addWidget(logoutButton);

    // Пространство
    mainLayout->addStretch();

    // Подключаем сигналы
    connect(warehouseButton, &QPushButton::clicked, this, &MainWindow::onWarehouseClicked);
    connect(deliveryButton, &QPushButton::clicked, this, &MainWindow::onDeliveryClicked);
    connect(shipmentButton, &QPushButton::clicked, this, &MainWindow::onShipmentClicked);
    connect(transferButton, &QPushButton::clicked, this, &MainWindow::onTransferClicked);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

    if (m_isAdmin) {
        connect(manageAccountsButton, &QPushButton::clicked, this, &MainWindow::onManageAccountsClicked);
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
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Выход",
        "Вы уверены, что хотите выйти из аккаунта?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        LoginWindow *loginWindow = new LoginWindow();
        loginWindow->show();
        this->close();
    }
}
