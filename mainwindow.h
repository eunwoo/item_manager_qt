#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonArray>
#include <QTableWidget>
#include <QStandardItemModel>

#define APP_VERSION "Ver. 27.0"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum {
    HTML_EXPORT_TABLE,
    HTML_EXPORT_LIST
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QJsonObject jsonObj;
    QMap<int, int> *rowSelected;
    QJsonArray json_export;
    int HtmlExportMode;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool LoadData();
    bool SaveData();
    void ClearTable(QTableWidget *table);
    void DisableTableRow(QTableWidget *tablewidget, int row);
    void TableToggleItemEditAttribute(QTableWidgetItem *item);
    bool SwapTableItem(QTableWidget *tablewidget, int rowSrc, int rowDest);
    bool CheckSwappable(QTableWidget *tablewidget, int rowSrc, int rowDest);
    void exportToExcel(QString filename, bool is_only_editable, int export_option);
    void exportToTxt(QString filename, bool is_only_editable, int export_option);
    void exportToHtml(QString filename, bool is_only_editable, int export_option);
    void exportToHtmlTable(QTextStream &out, bool is_only_editable, int price_row, int stock_row, int tag_row, QMap<QString, int> &exportTag, int export_option);
    void exportToHtmlList(QTextStream &out, bool is_only_editable, int price_row, int stock_row, int tag_row, QMap<QString, int> &exportTag, int export_option);
    QString GetEquivalentItem(float price, int option);
    QString convertToHtml(QString strInput);
    void MakeExportJson();
    int GetRowByNameInExportTable(QString str);
    void GenerateStyleTag(int row_export, QTextStream &out, QString text, int colspan = 0);
    void GenerateStyleTagForList(int row_export, QStringList &out, QString text, int colspan = 0);
    int FindExportTag(QMap<QString, int> &map, QString srcText);
    int FindListLevel(QMap<QString, int> &map, QString srcText);
    QString StripTag(QString str);

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

    void on_action_2_triggered();

    void on_action_3_triggered();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
