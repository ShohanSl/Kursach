#include "mainwindow.h"
#include <QMessageBox>
#include <QApplication>
#include "loginwindow.h"
#include "warehousewindow.h"
#include "deliverywindow.h"
#include "accountmanagementwindow.h"
#include "usermanager.h"
#include "exceptionhandler.h" // Добавляем заголовок

MainWindow::MainWindow(bool isAdmin, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_userManager(userManager)
{
    TRY_CATCH_BEGIN
    setupUI();
    applyStyle();
    setWindowTitle("Складской учет - Главное меню");
    setFixedSize(650, 550);
    TRY_CATCH_END
}

void MainWindow::setupUI()
{
    TRY_CATCH_BEGIN
        centralWidget = new QWidget(this);
    if (!centralWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания центрального виджета",
                        "Не удалось создать центральный виджет главного окна");
    }
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    if (!mainLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания главного макета",
                        "Не удалось создать главный макет главного окна");
    }
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    titleLabel = new QLabel("ГЛАВНОЕ МЕНЮ");
    if (!titleLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания заголовка",
                        "Не удалось создать метку заголовка");
    }
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setMinimumHeight(50);

    QString roleText = m_isAdmin ? "Администратор" : "Сотрудник";
    roleLabel = new QLabel("Роль: " + roleText);
    if (!roleLabel) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания метки роли",
                        "Не удалось создать метку отображения роли");
    }
    roleLabel->setAlignment(Qt::AlignCenter);
    roleLabel->setMinimumHeight(30);

    QWidget *contentWidget = new QWidget();
    if (!contentWidget) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания виджета контента",
                        "Не удалось создать виджет для основного содержимого");
    }
    QHBoxLayout *contentLayout = new QHBoxLayout(contentWidget);
    if (!contentLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета контента",
                        "Не удалось создать горизонтальный макет для содержимого");
    }
    contentLayout->setSpacing(25);
    contentLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *leftColumn = new QWidget();
    if (!leftColumn) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания левой колонки",
                        "Не удалось создать виджет левой колонки");
    }
    QVBoxLayout *leftLayout = new QVBoxLayout(leftColumn);
    if (!leftLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета левой колонки",
                        "Не удалось создать вертикальный макет для левой колонки");
    }
    leftLayout->setSpacing(15);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    warehouseButton = new QPushButton("Склад");
    if (!warehouseButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки склада",
                        "Не удалось создать кнопку перехода к складу");
    }
    warehouseButton->setMinimumSize(250, 50);
    warehouseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    deliveryButton = new QPushButton("Оформить поставку");
    if (!deliveryButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки поставки",
                        "Не удалось создать кнопку оформления поставки");
    }
    deliveryButton->setMinimumSize(250, 50);
    deliveryButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    leftLayout->addWidget(warehouseButton);
    leftLayout->addWidget(deliveryButton);

    if (m_isAdmin) {
        manageAccountsButton = new QPushButton("Управление аккаунтами");
        if (!manageAccountsButton) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания кнопки управления аккаунтами",
                            "Не удалось создать кнопку управления аккаунтами");
        }
        manageAccountsButton->setMinimumSize(250, 50);
        manageAccountsButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        leftLayout->addWidget(manageAccountsButton);
    } else {
        QWidget *adminSpace = new QWidget();
        if (!adminSpace) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания пустого виджета",
                            "Не удалось создать пустой виджет для заполнения пространства");
        }
        adminSpace->setMinimumSize(250, 50);
        adminSpace->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        leftLayout->addWidget(adminSpace);
    }

    leftLayout->addStretch();

    QWidget *rightColumn = new QWidget();
    if (!rightColumn) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания правой колонки",
                        "Не удалось создать виджет правой колонки");
    }
    QVBoxLayout *rightLayout = new QVBoxLayout(rightColumn);
    if (!rightLayout) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания макета правой колонки",
                        "Не удалось создать вертикальный макет для правой колонки");
    }
    rightLayout->setSpacing(15);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    shipmentButton = new QPushButton("Оформить отгрузку");
    if (!shipmentButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки отгрузки",
                        "Не удалось создать кнопку оформления отгрузки");
    }
    shipmentButton->setMinimumSize(250, 50);
    shipmentButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    transferButton = new QPushButton("Трансфер по складу");
    if (!transferButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки трансфера",
                        "Не удалось создать кнопку трансфера товаров");
    }
    transferButton->setMinimumSize(250, 50);
    transferButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    logoutButton = new QPushButton("Выйти из аккаунта");
    if (!logoutButton) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания кнопки выхода",
                        "Не удалось создать кнопку выхода из аккаунта");
    }
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
    TRY_CATCH_END
}

void MainWindow::applyStyle()
{
    TRY_CATCH_BEGIN
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
    TRY_CATCH_END
}

void MainWindow::onWarehouseClicked()
{
    TRY_CATCH_BEGIN
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "view", m_userManager);
    if (!warehouseWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна склада",
                        "Не удалось создать экземпляр WarehouseWindow");
    }
    warehouseWindow->show();
    this->close();
    TRY_CATCH_END
}

void MainWindow::onDeliveryClicked()
{
    TRY_CATCH_BEGIN
        DeliveryWindow *deliveryWindow = new DeliveryWindow(m_isAdmin, m_userManager);
    if (!deliveryWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна поставки",
                        "Не удалось создать экземпляр DeliveryWindow");
    }
    deliveryWindow->show();
    this->close();
    TRY_CATCH_END
}

void MainWindow::onShipmentClicked()
{
    TRY_CATCH_BEGIN
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "shipment", m_userManager);
    if (!warehouseWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна отгрузки",
                        "Не удалось создать экземпляр WarehouseWindow для отгрузки");
    }
    warehouseWindow->show();
    this->close();
    TRY_CATCH_END
}

void MainWindow::onTransferClicked()
{
    TRY_CATCH_BEGIN
        WarehouseWindow *warehouseWindow = new WarehouseWindow(m_isAdmin, "transfer", m_userManager);
    if (!warehouseWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна трансфера",
                        "Не удалось создать экземпляр WarehouseWindow для трансфера");
    }
    warehouseWindow->show();
    this->close();
    TRY_CATCH_END
}

void MainWindow::onManageAccountsClicked()
{
    TRY_CATCH_BEGIN
        if (!m_userManager) {
        // Оставляем QMessageBox для пользовательской ошибки (неправильная конфигурация)
        QMessageBox::critical(this, "Ошибка", "UserManager не инициализирован");
        return;
    }

    AccountManagementWindow *accountWindow = new AccountManagementWindow(m_userManager);
    if (!accountWindow) {
        THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                        "Ошибка создания окна управления аккаунтами",
                        "Не удалось создать экземпляр AccountManagementWindow");
    }
    accountWindow->show();
    this->close();
    TRY_CATCH_END
}

void MainWindow::onLogoutClicked()
{
    TRY_CATCH_BEGIN
        QMessageBox msgBox;
    msgBox.setWindowTitle("Выход");
    msgBox.setText("Вы уверены, что хотите выйти из аккаунта?");

    QPushButton *yesButton = msgBox.addButton("Да", QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton("Нет", QMessageBox::NoRole);

    msgBox.exec();

    if (msgBox.clickedButton() == yesButton) {
        LoginWindow *loginWindow = new LoginWindow();
        if (!loginWindow) {
            THROW_EXCEPTION(ErrorSeverity::ERROR, ErrorSource::SYSTEM,
                            "Ошибка создания окна входа",
                            "Не удалось создать экземпляр LoginWindow");
        }
        loginWindow->show();
        this->close();
    }
    TRY_CATCH_END
}
