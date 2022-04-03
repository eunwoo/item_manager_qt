#include "ItemTable.h"
#include <QDebug>
#include <QKeyEvent>

ItemTable::ItemTable(QWidget *parent) : QTableWidget(parent)
{
//    setItemDelegate(&delegate);
}

ItemTable::~ItemTable() {
}

void ItemTable::InsertItem(QString name, QString price, QString stock, bool isActivated, QString tag)
{
    int currentRowNo = currentRow();
    if(currentRowNo == -1) currentRowNo = 0;    // 선택된 셀이 없을 경우 처음에 삽입
    insertRow(currentRowNo + 1);
    AddItem(name, price, stock, isActivated, tag, currentRowNo + 1);
}
void ItemTable::AddItem(QString name, QString price, QString stock, bool isActivated, QString tag, int row)
{
    if(row > rowCount() - 1) insertRow(row);
    setItem(row, 0, new QTableWidgetItem(name));
    setItem(row, 1, new QTableWidgetItem(price));

    CellItemSpinBox *cell_spin_widget = new CellItemSpinBox();
    cell_spin_widget->SetValue(stock.toInt());
    setCellWidget(row, 2, cell_spin_widget);

    CellItemChkBox *cell_widget = new CellItemChkBox();
    if(isActivated) {
        cell_widget->SetCheck(Qt::Checked);
    }
    else {
        cell_widget->SetCheck(Qt::Unchecked);
    }
    setCellWidget(row, 3, cell_widget);

    setItem(row, 4, new QTableWidgetItem(tag));
}

void ItemTable::keyPressEvent(QKeyEvent *event)
{
    qInfo() << "ItemTable::onKeyPressEvent";
    if(event->key() == Qt::Key_Return) {
        if(item(currentRow(), currentColumn()) == nullptr) {
            insertRow(currentRow());
            AddItem("", "", "", true, "", currentRow());
        }
        else {
            edit(currentIndex());
        }
        edit(currentIndex());
        qInfo() << "Enter";
    }
    else if(event->key() == Qt::Key_Plus){
        InsertItem("-", "", "", false, "");
        event->ignore();
        return;
    }
    else {
        if(item(currentRow(), currentColumn()) == nullptr) {
//            insertRow(currentRow());
            AddItem("-", "", "", true, "", currentRow());
        }
        QTableWidget::keyPressEvent(event);
    }
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
