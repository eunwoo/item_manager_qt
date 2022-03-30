#ifndef ITEMTABLE_H
#define ITEMTABLE_H

#include <QTableWidget>
#include <QStandardItemModel>
#include "cellitemchkbox.h"
#include "cellitemspinbox.h"
#include "spinboxdelegate.h"

class ItemTable : public QTableWidget
{
    Q_OBJECT

public:
    ItemTable(QWidget *parent = nullptr);
    ~ItemTable();
    SpinBoxDelegate delegate;

    void InsertItem(QString name, QString price, QString stock, bool isActivated, QString tag);
    void AddItem(QString name, QString price, QString stock, bool isActivated, QString tag, int row);
    void Find(QString text);

    // virtual function
    virtual void keyPressEvent(QKeyEvent *);

private slots:
private:
};
#endif // ITEMTABLE_H
