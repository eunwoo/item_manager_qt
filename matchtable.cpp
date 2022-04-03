#include "matchtable.h"
#include <QKeyEvent>
#include <QDebug>

MatchTable::MatchTable(QWidget *parent) : QTableWidget(parent)
{
}

void MatchTable::InsertItem(QString range, QString item1, QString item2)
{
    int currentRowNo = currentRow();
    if(currentRowNo == -1) currentRowNo = 0;    // 선택된 셀이 없을 경우 처음에 삽입
    insertRow(currentRowNo + 1);
    AddItem(range, item1, item2, currentRowNo + 1);
}

void MatchTable::AddItem(QString range, QString item1, QString item2, int row)
{
    if(row > rowCount() - 1) insertRow(row);
    setItem(row, 0, new QTableWidgetItem(range));
    setItem(row, 1, new QTableWidgetItem(item1));
    setItem(row, 2, new QTableWidgetItem(item2));
}

void MatchTable::keyPressEvent(QKeyEvent *event)
{
    qInfo() << "MatchTable::onKeyPressEvent";
    if(event->key() == Qt::Key_Return) {
        edit(currentIndex());
    }
    else if(event->key() == Qt::Key_Plus) {
        InsertItem("", "", "");
    }
    else QTableWidget::keyPressEvent(event);
}
