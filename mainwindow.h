#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QTableWidget>

#define APP_VERSION "Ver. 20.0"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QJsonObject jsonObj;
    QMap<int, int> *rowSelected;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool LoadData();
    bool SaveData();
    void ClearTable(QTableWidget *table);
    void DisableTableRow(QTableWidget *tablewidget, int row);
    void TableToggleItemEditAttribute(QTableWidgetItem *item);
    bool SwapTableItem(QTableWidget *tablewidget, int rowSrc, int rowDest);
    void exportToExcel(QString filename, bool is_only_editable, int export_option);
    void exportToTxt(QString filename, bool is_only_editable, int export_option);
    QString GetEquivalentItem(float price, int option);

private slots:
    virtual void changeEvent(QEvent *event);
    virtual void keyPressEvent(QKeyEvent *);
    void FindValueChanged(QString text);
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_action_triggered();

    void on_action_U_triggered();

    void on_action_E_triggered();

    void on_action_Q_triggered();

    void on_action_A_triggered();

    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
