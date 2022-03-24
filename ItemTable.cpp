#include "ItemTable.h"
#include <QDebug>
#include <QKeyEvent>

ItemTable::ItemTable(QWidget *parent) : QTableWidget(parent)
{
}

void ItemTable::InsertItem(QString name, QString price, QString stock)
{
    int currentRowNo = currentRow();
    if(currentRowNo == -1) currentRowNo = 0;
    insertRow(currentRowNo);
    AddItem(name, price, stock, currentRowNo);
}
void ItemTable::AddItem(QString name, QString price, QString stock, int row)
{
    setItem(row, 0, new QTableWidgetItem(name));
    setItem(row, 1, new QTableWidgetItem(price));
    setItem(row, 2, new QTableWidgetItem(stock));
}


void ItemTable::keyPressEvent(QKeyEvent *event)
{
    qInfo() << "onKeyPressEvent";
    if(event->key() == Qt::Key_Return) {
        edit(currentIndex());
    }
    else QTableWidget::keyPressEvent(event);
}

void ItemTable::Find(QString filter_text)
{
    for(int i = 0; i<rowCount(); ++i) {
        setRowHidden(i, false);
    }
    for(int i = 0; i<rowCount(); ++i) {
        if(item(i, 0) == nullptr) break;
        QString cell = item( i, 0 )->text();
        if (cell.indexOf(filter_text) == -1) {
            setRowHidden(i, true);
        }
        else {
            setRowHidden(i, false);
        }
    }
}
