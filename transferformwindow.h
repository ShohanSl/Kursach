#ifndef TRANSFERFORMWINDOW_H
#define TRANSFERFORMWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
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
                                const QString& materialType, bool isAdmin,
                                UserManager* userManager = nullptr, QWidget *parent = nullptr);

private slots:
    void onCompleteTransferClicked();
    void onBackClicked();
    void onTargetSectionChanged(int index);

private:
    void setupUI();
    void updateTargetCellsComboBox(int targetSectionNumber);
    void updateTargetSectionsComboBox(const QString& productType);
    void saveTransfer();

    Product m_product;
    int m_sourceSectionNumber;
    QString m_materialType;
    bool m_isAdmin;
    UserManager* m_userManager;

    QLineEdit *transferQuantityEdit;
    QComboBox *targetSectionCombo;
    QComboBox *targetCellCombo;
    QDateEdit *dateEdit;
};

#endif // TRANSFERFORMWINDOW_H
