#ifndef EXPORTTABLE_H
#define EXPORTTABLE_H

#include <QTableWidget>
#include <QStandardItemModel>
#include "cellitemchkbox.h"
#include "cellitemspinbox.h"
#include "spinboxdelegate.h"

class ExportTable : public QTableWidget
{
    Q_OBJECT

public:
    ExportTable(QWidget *parent = nullptr);
    ~ExportTable();
    SpinBoxDelegate delegate;

    void InsertItem(QString name, QString txt, QString fontsize, QString isBold, QString isItalic, QString isUnderline, QString bgColor, QString fontColor);
    void AddItem(QString name, QString txt, QString fontsize, QString isBold, QString isItalic, QString isUnderline, QString bgColor, QString fontColor, int row);
    void Find(QString text);

    // virtual function
    virtual void keyPressEvent(QKeyEvent *);

private slots:
private:
};

#endif // EXPORTTABLE_H
