#ifndef TRANSFERFORMWINDOW_H
#define TRANSFERFORMWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDateEdit>
#include "product.h"
#include "operation.h"
#include "usermanager.h"

class TransferFormWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TransferFormWindow(const Product& product, int sourceSectionNumber,
                                const QString& materialType, bool isAdmin, UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onCompleteTransferClicked();
    void onBackClicked();
    void onTargetSectionChanged(int index);

private:
    void setupUI();
    void applyStyle();
    void saveTransferToFile();
    void updateTargetCellsComboBox(int targetSectionNumber);
    void updateTargetSectionsComboBox(const QString& productType);

    Product m_product;
    int m_sourceSectionNumber;
    QString m_materialType;
    bool m_isAdmin;

    QWidget *centralWidget;

    QLineEdit *productNameEdit;
    QLineEdit *productIndexEdit;
    QLineEdit *supplierEdit;
    QLineEdit *cellNumberEdit;
    QLineEdit *availableQuantityEdit;
    QLineEdit *transferQuantityEdit;

    QComboBox *targetSectionCombo;
    QComboBox *targetCellCombo;
    QDateEdit *dateEdit;

    QPushButton *completeButton;
    QPushButton *backButton;

    UserManager* m_userManager;
};

#endif // TRANSFERFORMWINDOW_H
