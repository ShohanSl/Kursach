#ifndef OPERATIONSHISTORYWINDOW_H
#define OPERATIONSHISTORYWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QList>
#include <QInputDialog>
#include <QMenu>
#include <QDate>
#include "operation.h"
#include "usermanager.h"

class OperationsHistoryWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit OperationsHistoryWindow(int sectionNumber, const QString& materialType,
                                     bool isAdmin, UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onBackClicked();
    void onCellDoubleClicked(int row, int column);
    void onDeleteOperation();

private:
    void setupUI();
    void applyStyle();
    void loadOperationsHistory();
    void saveOperationsHistory();
    void updateTable();
    bool validateCellEdit(int row, int column, const QString& newValue);
    void updateOperationData(int row, int column, const QString& newValue);
    void setupContextMenu();

    int m_sectionNumber;
    QString m_materialType;
    bool m_isAdmin;

    QWidget *centralWidget;
    QLabel *titleLabel;
    QPushButton *backButton;
    QTableWidget *operationsTable;

    QList<Operation> m_operationsHistory;
    QString m_historyFile;

    QMenu *contextMenu;
    int selectedRow;

    UserManager* m_userManager;
};

#endif // OPERATIONSHISTORYWINDOW_H
