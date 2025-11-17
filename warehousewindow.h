#ifndef WAREHOUSEWINDOW_H
#define WAREHOUSEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "usermanager.h"

class SectionWindow;

class WarehouseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit WarehouseWindow(bool isAdmin = false, const QString& mode = "view", UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onSectionClicked(int sectionNumber);
    void onBackClicked();

private:
    void setupUI();
    void applyStyle();

    bool m_isAdmin;
    QString m_mode;
    QWidget *centralWidget;
    QLabel *titleLabel;
    QPushButton *backButton;
    QPushButton *sectionButtons[12];
    UserManager* m_userManager;
};

#endif // WAREHOUSEWINDOW_H
