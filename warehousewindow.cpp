#include "warehousewindow.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include "sectionwindow.h"
#include "mainwindow.h"

WarehouseWindow::WarehouseWindow(bool isAdmin, const QString& mode,
                                 UserManager* userManager, QWidget *parent)
    : QMainWindow(parent), m_isAdmin(isAdmin), m_mode(mode), m_userManager(userManager)
{
    setupUI();
    setWindowTitle("Склад");
    setFixedSize(500, 500);
}

void WarehouseWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout *mainLayout = new QVBoxLayout(central);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Заголовок
    QString titleText;
    if (m_mode == "shipment")
        titleText = "ОТГРУЗКА - Выберите секцию";
    else if (m_mode == "transfer")
        titleText = "ТРАНСФЕР - Выберите исходную секцию";
    else
        titleText = "СКЛАД - Выберите секцию";

    QLabel *titleLabel = new QLabel(titleText);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Кнопка "Назад"
    backButton = new QPushButton("← Назад");
    mainLayout->addWidget(backButton);

    // Группа секций "Дерево"
    QGroupBox *woodGroup = new QGroupBox("Дерево");
    QGridLayout *woodLayout = new QGridLayout(woodGroup);
    for (int i = 0; i < 3; i++) {
        int sectionNum = i + 1;
        sectionButtons[i] = new QPushButton("Секция " + QString::number(sectionNum));
        woodLayout->addWidget(sectionButtons[i], 0, i);
        connect(sectionButtons[i], &QPushButton::clicked,
                [this, sectionNum]() { onSectionClicked(sectionNum); });
    }
    mainLayout->addWidget(woodGroup);

    // Группа секций "Металл"
    QGroupBox *metalGroup = new QGroupBox("Металл");
    QGridLayout *metalLayout = new QGridLayout(metalGroup);
    for (int i = 0; i < 3; i++) {
        int sectionNum = i + 4;
        sectionButtons[i+3] = new QPushButton("Секция " + QString::number(sectionNum));
        metalLayout->addWidget(sectionButtons[i+3], 0, i);
        connect(sectionButtons[i+3], &QPushButton::clicked,
                [this, sectionNum]() { onSectionClicked(sectionNum); });
    }
    mainLayout->addWidget(metalGroup);

    // Группа секций "Камень"
    QGroupBox *stoneGroup = new QGroupBox("Камень");
    QGridLayout *stoneLayout = new QGridLayout(stoneGroup);
    for (int i = 0; i < 3; i++) {
        int sectionNum = i + 7;
        sectionButtons[i+6] = new QPushButton("Секция " + QString::number(sectionNum));
        stoneLayout->addWidget(sectionButtons[i+6], 0, i);
        connect(sectionButtons[i+6], &QPushButton::clicked,
                [this, sectionNum]() { onSectionClicked(sectionNum); });
    }
    mainLayout->addWidget(stoneGroup);

    // Группа секций "Пластмасса"
    QGroupBox *plasticGroup = new QGroupBox("Пластмасса");
    QGridLayout *plasticLayout = new QGridLayout(plasticGroup);
    for (int i = 0; i < 3; i++) {
        int sectionNum = i + 10;
        sectionButtons[i+9] = new QPushButton("Секция " + QString::number(sectionNum));
        plasticLayout->addWidget(sectionButtons[i+9], 0, i);
        connect(sectionButtons[i+9], &QPushButton::clicked,
                [this, sectionNum]() { onSectionClicked(sectionNum); });
    }
    mainLayout->addWidget(plasticGroup);

    mainLayout->addStretch();

    connect(backButton, &QPushButton::clicked, this, &WarehouseWindow::onBackClicked);
}

void WarehouseWindow::onSectionClicked(int sectionNumber)
{
    QString materialType;
    if (sectionNumber >= 1 && sectionNumber <= 3)
        materialType = "Дерево";
    else if (sectionNumber >= 4 && sectionNumber <= 6)
        materialType = "Металл";
    else if (sectionNumber >= 7 && sectionNumber <= 9)
        materialType = "Камень";
    else if (sectionNumber >= 10 && sectionNumber <= 12)
        materialType = "Пластмасса";
    else
        materialType = "Неизвестно";

    SectionWindow *sectionWindow = new SectionWindow(sectionNumber, materialType,
                                                     m_isAdmin, m_mode, m_userManager);
    sectionWindow->show();
    this->close();
}

void WarehouseWindow::onBackClicked()
{
    MainWindow *mainWindow = new MainWindow(m_isAdmin, m_userManager);
    mainWindow->show();
    this->close();
}
