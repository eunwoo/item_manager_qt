#include "lineedit.h"
#include <QDebug>
#include "mainwindow.h"
#include <QKeyEvent>

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{
}

void LineEdit::keyPressEvent(QKeyEvent *event)
{
    qInfo() << "LineEdit::keyPressEvent";
    qInfo() << text();
    QString filter_text;
    if(event->key() == Qt::Key_Return) {
        emit FindValueChanged(text().trimmed());
    }
    else if(event->key() == Qt::Key_Backspace) {
        emit FindValueChanged(text().left(text().length() - 1));
    }
    else {
        filter_text = text() + event->text();
        emit FindValueChanged(filter_text.trimmed());
    }
    QLineEdit::keyPressEvent(event);

}
