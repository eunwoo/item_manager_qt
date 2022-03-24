#include "matchtable.h"
#include <QKeyEvent>
#include <QDebug>

MatchTable::MatchTable(QWidget *parent) : QTableWidget(parent)
{
}

void MatchTable::AddItem(QString range, QString item1, QString item2, int row)
{
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
    else QTableWidget::keyPressEvent(event);
}
