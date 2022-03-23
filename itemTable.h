#ifndef ITEMTABLE_H
#define ITEMTABLE_H

#include <QTableWidget>

class ItemTable : public QTableWidget
{
    Q_OBJECT

public:
    ItemTable(QWidget *parent = nullptr);
    void AddItem(QString name, QString price, QString stock, int row);

private slots:

private:
};
#endif // ITEMTABLE_H
