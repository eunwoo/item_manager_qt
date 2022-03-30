#include "ExportTable.h"
#include <QDebug>
#include <QKeyEvent>

ExportTable::ExportTable(QWidget *parent) : QTableWidget(parent)
{
//    setItemDelegate(&delegate);
}

ExportTable::~ExportTable() {
}

void ExportTable::InsertItem(QString name, QString txt, QString fontsize, QString isBold, QString isItalic, QString isUnderline, QString bgColor, QString fontColor)
{
    int currentRowNo = currentRow();
    if(currentRowNo == -1) currentRowNo = 0;    // 선택된 셀이 없을 경우 처음에 삽입
    insertRow(currentRowNo + 1);
    AddItem(name, txt, fontsize, isBold, isItalic, isUnderline, bgColor, fontColor, currentRowNo + 1);
}
void ExportTable::AddItem(QString name, QString txt, QString fontsize, QString isBold, QString isItalic, QString isUnderline, QString bgColor, QString fontColor, int row)
{
    if(row > rowCount() - 1) insertRow(row);
    setItem(row, 0, new QTableWidgetItem(name));
    setItem(row, 1, new QTableWidgetItem(txt));
    setItem(row, 2, new QTableWidgetItem(fontsize));
    setItem(row, 3, new QTableWidgetItem(isBold));
    setItem(row, 4, new QTableWidgetItem(isItalic));
    setItem(row, 5, new QTableWidgetItem(isUnderline));
    setItem(row, 6, new QTableWidgetItem(bgColor));
    setItem(row, 7, new QTableWidgetItem(fontColor));
//    setRowHeight(row, 30);
}

void ExportTable::keyPressEvent(QKeyEvent *event)
{
    qInfo() << "onKeyPressEvent";
    if(event->key() == Qt::Key_Return) {
        edit(currentIndex());
    }
    else QTableWidget::keyPressEvent(event);
}

void ExportTable::Find(QString filter_text)
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
