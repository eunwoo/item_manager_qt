#ifndef CELLITEMSPINBOX_H
#define CELLITEMSPINBOX_H

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>

class CellItemSpinBox : public QWidget
{
    Q_OBJECT
public:
    explicit CellItemSpinBox(QWidget *parent = nullptr);
    ~CellItemSpinBox();

    QSpinBox *spinBox;

    void SetValue(int value);
    void Increase();
    void Decrease();

signals:

};

#endif // CELLITEMSPINBOX_H
