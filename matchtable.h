#ifndef MATCHTABLE_H
#define MATCHTABLE_H

#include <QTableWidget>

class MatchTable : public QTableWidget
{
    Q_OBJECT

public:
    MatchTable(QWidget *parent = nullptr);
    void AddItem(QString name, QString price, QString stock, int row);

    // virtual function
    virtual void keyPressEvent(QKeyEvent *);

private slots:
private:
};
#endif // MATCHTABLE_H
