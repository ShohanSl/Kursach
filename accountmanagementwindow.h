#ifndef ACCOUNTMANAGEMENTWINDOW_H
#define ACCOUNTMANAGEMENTWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
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

private:
    void setupUI();
    void setupTable(QTableWidget *table, const QStringList &headers);
    void loadEmployeesTable();
    void loadAdminsTable();
    void loadPendingUsersTable();
    void filterTable(QTableWidget *table, const CustomList<QStringList> &allData,
                     const QString& searchText, int searchCriteria);
    void setupContextMenu(QTableWidget *table, const QString& userType);
    void updateTables();

    UserManager *m_userManager;

    QTableWidget *employeesTable;
    QTableWidget *adminsTable;
    QTableWidget *pendingUsersTable;

    QLineEdit *employeesSearchEdit;
    QComboBox *employeesSearchCombo;
    QLineEdit *adminsSearchEdit;
    QComboBox *adminsSearchCombo;
    QLineEdit *pendingUsersSearchEdit;
    QComboBox *pendingUsersSearchCombo;

    CustomList<QStringList> m_employeesData;
    CustomList<QStringList> m_adminsData;
    CustomList<QStringList> m_pendingUsersData;

    QString m_selectedUserType;
    int m_selectedRow;
};

#endif // ACCOUNTMANAGEMENTWINDOW_H
