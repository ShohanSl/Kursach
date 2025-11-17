#ifndef ACCOUNTMANAGEMENTWINDOW_H
#define ACCOUNTMANAGEMENTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QMenu>
#include "usermanager.h"

class AccountManagementWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AccountManagementWindow(UserManager *userManager, QWidget *parent = nullptr);

private slots:
    void onBackClicked();
    void onAddUserClicked();
    void onSearchTextChanged(const QString& text);
    void onSearchCriteriaChanged(int index);
    void onDeleteUser();
    void updateTables();

private:
    void setupUI();
    void applyStyle();
    void setupTable(QTableWidget *table, const QStringList &headers);
    void loadEmployeesTable();
    void loadAdminsTable();
    void loadPendingUsersTable();
    void filterTable(QTableWidget *table, const QList<QStringList> &allData, const QString& searchText, int searchCriteria);
    void setupContextMenu(QTableWidget *table, const QString& userType);

    UserManager *m_userManager;

    QWidget *centralWidget;
    QScrollArea *scrollArea;
    QWidget *scrollContent;

    // Элементы для сотрудников
    QLabel *employeesLabel;
    QLineEdit *employeesSearchEdit;
    QComboBox *employeesSearchCombo;
    QTableWidget *employeesTable;

    // Элементы для администраторов
    QLabel *adminsLabel;
    QLineEdit *adminsSearchEdit;
    QComboBox *adminsSearchCombo;
    QTableWidget *adminsTable;

    // Элементы для ожидающих пользователей
    QLabel *pendingUsersLabel;
    QLineEdit *pendingUsersSearchEdit;
    QComboBox *pendingUsersSearchCombo;
    QTableWidget *pendingUsersTable;

    QPushButton *backButton;

    // Данные таблиц
    QList<QStringList> m_employeesData;
    QList<QStringList> m_adminsData;
    QList<QStringList> m_pendingUsersData;

    QMenu *contextMenu;
    QString m_selectedUserType;
    int m_selectedRow;
};

#endif // ACCOUNTMANAGEMENTWINDOW_H
