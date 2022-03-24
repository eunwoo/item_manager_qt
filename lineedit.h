#ifndef LINEEDIT_H
#define LINEEDIT_H

#include <QLineEdit>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LineEdit(QWidget *parent = nullptr);
    virtual void keyPressEvent(QKeyEvent *event);
signals:
    void FindValueChanged(QString text);
};

#endif // LINEEDIT_H
