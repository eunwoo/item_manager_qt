#include "cellitemspinbox.h"

CellItemSpinBox::CellItemSpinBox(QWidget *parent)
    : QWidget{parent}
{
    spinBox = new QSpinBox();
    spinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
//    spinBox->setStyleSheet("QSpinBox::up-button {subcontrol-origin: border;subcontrol-position: top right; width: 16px; border-image: url(./up.png) 1;border-width: 1px;}");
    spinBox->setStyleSheet("QSpinBox::up-button { width: 30px; } QSpinBox::down-button { width: 30px; }");
//    QVBoxLayout *vlayout = new QVBoxLayout(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
//    vlayout->addLayout(layout, 1);

    layout->addWidget(spinBox, 1);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);

}
CellItemSpinBox::~CellItemSpinBox()
{
    delete spinBox;
}

void CellItemSpinBox::SetValue(int value)
{
    spinBox->setValue(value);
}

void CellItemSpinBox::Increase()
{

}

void CellItemSpinBox::Decrease()
{

}

