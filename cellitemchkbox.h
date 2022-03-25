#ifndef CELLITEMCHKBOX_H
#define CELLITEMCHKBOX_H

#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>

class CellItemChkBox : public QWidget
{
    Q_OBJECT
public:
    explicit CellItemChkBox(QWidget *parent = nullptr);
    ~CellItemChkBox();

    QCheckBox *chkBox;
    void SetCheck(Qt::CheckState state);
    Qt::CheckState GetCheck();
signals:

};

#endif // CELLITEMCHKBOX_H
