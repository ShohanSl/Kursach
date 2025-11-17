#include "warehousewindow.h"
#include "mainwindow.h"
#include "sectionwindow.h"
#include <QMessageBox>

WarehouseWindow::WarehouseWindow(bool isAdmin, const QString& mode, UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_mode(mode), m_userManager(userManager)
{
    setupUI();
    applyStyle();

    if (m_mode == "shipment") {
        setWindowTitle("Складской учет - Оформление отгрузки");
    } else if (m_mode == "transfer") {
        setWindowTitle("Складской учет - Трансфер товаров");
    } else {
        setWindowTitle("Складской учет - Склад");
    }

    setFixedSize(800, 600);
}

void WarehouseWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 20, 30, 20);

    QWidget *topPanel = new QWidget();
    QHBoxLayout *topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(0, 0, 0, 0);

    backButton = new QPushButton("← Назад");
    backButton->setFixedSize(100, 35);

    if (m_mode == "shipment") {
        titleLabel = new QLabel("ОФОРМЛЕНИЕ ОТГРУЗКИ - ВЫБОР СЕКЦИИ");
    } else if (m_mode == "transfer") {
        titleLabel = new QLabel("ТРАНСФЕР ТОВАРОВ - ВЫБОР СЕКЦИИ");
    } else {
        titleLabel = new QLabel("СКЛАД - СЕКЦИИ");
    }
    titleLabel->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(backButton);
    topLayout->addStretch();
    topLayout->addWidget(titleLabel);
    topLayout->addStretch();

    QWidget *sectionsWidget = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(sectionsWidget);
    gridLayout->setSpacing(15);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    QString materials[12] = {
        "Дерево", "Дерево", "Дерево",
        "Металл", "Металл", "Металл",
        "Камень", "Камень", "Камень",
        "Пластмасса", "Пластмасса", "Пластмасса"
    };

    for (int i = 0; i < 12; i++) {
        int sectionNumber = i + 1;
        QString buttonText = QString("Секция №%1\n%2").arg(sectionNumber).arg(materials[i]);

        sectionButtons[i] = new QPushButton(buttonText);
        sectionButtons[i]->setMinimumSize(200, 80);
        sectionButtons[i]->setProperty("sectionNumber", sectionNumber);

        int row = i / 3;
        int col = i % 3;
        gridLayout->addWidget(sectionButtons[i], row, col);

        connect(sectionButtons[i], &QPushButton::clicked, this, [this, sectionNumber]() {
            onSectionClicked(sectionNumber);
        });
    }

    mainLayout->addWidget(topPanel);
    mainLayout->addWidget(sectionsWidget);
    mainLayout->addStretch();

    connect(backButton, &QPushButton::clicked, this, &WarehouseWindow::onBackClicked);
}

void WarehouseWindow::applyStyle()
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
            font-size: 24px;
            font-weight: bold;
            padding: 10px;
            margin: 5px;
        }
        QPushButton {
            background-color: #3498db;
            color: white;
            border: none;
            border-radius: 15px;
            padding: 15px;
            font-size: 14px;
            font-weight: bold;
            margin: 0px;
        }
        QPushButton:hover {
            background-color: #2980b9;
        }
        QPushButton:pressed {
            background-color: #2471a3;
        }
        QPushButton#backButton {
            background-color: #95a5a6;
            font-size: 13px;
            padding: 8px 15px;
        }
        QPushButton#backButton:hover {
            background-color: #7f8c8d;
        }
    )");

    backButton->setObjectName("backButton");
}

void WarehouseWindow::onSectionClicked(int sectionNumber)
{
    QString materialType;
    if (sectionNumber <= 3) materialType = "Дерево";
    else if (sectionNumber <= 6) materialType = "Металл";
    else if (sectionNumber <= 9) materialType = "Камень";
    else materialType = "Пластмасса";

    SectionWindow *sectionWindow = new SectionWindow(sectionNumber, materialType, m_isAdmin, m_mode, m_userManager);
    sectionWindow->show();
    this->close();
}

void WarehouseWindow::onBackClicked()
{
    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    mainWindow->show();
    this->close();
}
