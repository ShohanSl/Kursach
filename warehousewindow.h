#ifndef WAREHOUSEWINDOW_H
#define WAREHOUSEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "usermanager.h"

class WarehouseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WarehouseWindow(bool isAdmin = false, const QString& mode = "view",
                             UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onBackClicked();

private:
    void setupUI();
    void onSectionClicked(int sectionNumber);

    bool m_isAdmin;
    QString m_mode;
    UserManager* m_userManager;

    QPushButton *backButton;
    QPushButton *sectionButtons[12];
};

#endif // WAREHOUSEWINDOW_H
