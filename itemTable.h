#ifndef ITEMTABLE_H
#define ITEMTABLE_H

#include <QTableWidget>

class ItemTable : public QTableWidget
{
    Q_OBJECT

public:
    ItemTable(QWidget *parent = nullptr);
    void InsertItem(QString name, QString price, QString stock);
    void AddItem(QString name, QString price, QString stock, int row);
    void Find(QString text);

    // virtual function
    virtual void keyPressEvent(QKeyEvent *);

private slots:
private:
};
#endif // ITEMTABLE_H
