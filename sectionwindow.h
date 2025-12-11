#ifndef SECTIONWINDOW_H
#define SECTIONWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "customlist.h"
#include <QLineEdit>
#include <QComboBox>
#include "product.h"
#include "operation.h"
#include "usermanager.h"
#include "deletionhistory.h"
#include <QKeyEvent>

class SectionWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SectionWindow(int sectionNumber, const QString& materialType,
                           bool isAdmin, const QString& mode = "view",
                           UserManager* userManager = nullptr, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;  // ✅ Уже есть

private slots:
    void onBackClicked();
    void onOperationsHistoryClicked();
    void onSearchTextChanged(const QString& text);
    void onSearchCriteriaChanged(int index);
    void onCellDoubleClicked(int row, int column);
    void onDeleteProduct();
    void onShipmentClicked(int row, int column);
    void onTransferClicked(int row, int column);

private:
    void setupUI();
    void loadProducts();
    void saveProducts();
    void loadOperationsHistory();
    void saveOperationsHistory();
    void updateTable();
    void filterTable(const QString& searchText, int searchCriteria);
    void updateProductData(int row, int column, const QString& newValue);
    void setupContextMenu();
    int getDataIndexFromVisualRow(int visualRow) const;

    // ✅ ДОБАВЬТЕ ЭТИ ДВА МЕТОДА:
    void undoLastDeletion();
    bool isCellOccupied(int cellNumber) const;

    int m_sectionNumber;
    QString m_materialType;
    bool m_isAdmin;
    QString m_mode;

    QTableWidget *productsTable;
    QLineEdit *searchEdit;
    QComboBox *searchComboBox;

    CustomList<Product> m_products;
    CustomList<Product> m_allProducts;
    CustomList<Operation> m_operationsHistory;

    QString m_productsFile;
    QString m_historyFile;

    QMenu *contextMenu;
    int selectedRow;
    UserManager* m_userManager;

    DeletionHistory* m_deletionHistory;

    static const int MAX_CELLS = 60;
};

#endif // SECTIONWINDOW_H
