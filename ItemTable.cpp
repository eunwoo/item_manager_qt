#include "ItemTable.h"

ItemTable::ItemTable(QWidget *parent) : QTableWidget(parent)
{
}

void ItemTable::AddItem(QString name, QString price, QString stock, int row)
{
    setItem(row, 0, new QTableWidgetItem(name));
    setItem(row, 1, new QTableWidgetItem(price));
    setItem(row, 2, new QTableWidgetItem(stock));
}

