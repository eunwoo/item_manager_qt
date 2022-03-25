#include "cellitemchkbox.h"

CellItemChkBox::CellItemChkBox(QWidget *parent)
    : QWidget{parent}
{
    chkBox = new QCheckBox();
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(chkBox);
    layout->setAlignment(Qt::AlignCenter);
    layout->setContentsMargins(0,0,0,0);
}

CellItemChkBox::~CellItemChkBox()
{
    delete chkBox;
}

void CellItemChkBox::SetCheck(Qt::CheckState state)
{
    chkBox->setCheckState(state);
}

Qt::CheckState CellItemChkBox::GetCheck()
{
    return chkBox->checkState();
}
