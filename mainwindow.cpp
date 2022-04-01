#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAxObject>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTableWidgetSelectionRange>
#include <QCheckBox>
#include <QMessageBox>
#include <QStandardItemModel>
#include "newitemdialog.h"
#include "cellitemchkbox.h"
#include "cellitemspinbox.h"
#include "ItemTable.h"
#include "ExportTable.h"
#include "parser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(5);
    QStringList tableHeader;
    tableHeader << "품 명" << "가 격" << "재 고" << "활성화" << "태그";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget->setRowCount(1000);
    ui->tableWidget->setColumnWidth(0, 260);
    ui->tableWidget->setColumnWidth(1, 80);
    ui->tableWidget->setColumnWidth(2, 80);
    ui->tableWidget->setColumnWidth(3, 60);
    ui->tableWidget->setColumnWidth(3, 100);
    connect(ui->lineEdit_2, SIGNAL(FindValueChanged(QString)), this, SLOT(FindValueChanged(QString)));

    ui->matchTableWidget->setColumnCount(3);
    tableHeader.clear();
    tableHeader << "가격 범위" << "아이템 1" << "아이템 2";
    ui->matchTableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->matchTableWidget->setRowCount(1000);
    ui->matchTableWidget->setColumnWidth(0, 160);
    ui->matchTableWidget->setColumnWidth(1, 80);
    ui->matchTableWidget->setColumnWidth(2, 80);

    tableHeader.clear();
    tableHeader << "부분/검색어" << "txt 파일출력" << "글자크기" << "굵게" << "기울이기" << "밑줄" << "바탕색" << "글자색";
    ui->exportTableWidget->setColumnCount(tableHeader.length());
    ui->exportTableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->exportTableWidget->setRowCount(1000);
    ui->exportTableWidget->setColumnWidth(0, 160);
    ui->exportTableWidget->setColumnWidth(1, 300);
    ui->exportTableWidget->setColumnWidth(2, 80);
    ui->exportTableWidget->setColumnWidth(3, 80);
    ui->exportTableWidget->setColumnWidth(4, 80);
    ui->exportTableWidget->setColumnWidth(5, 80);
    ui->exportTableWidget->setColumnWidth(6, 80);
    ui->exportTableWidget->setColumnWidth(7, 80);


    ui->lineEdit->setText("1.0");
    ui->comboBox->addItem("그대로");
    ui->comboBox->addItem("아이템1");
    ui->comboBox->addItem("아이템2");

    setGeometry(100,100, 1200, 800);
    setWindowTitle(QString("아이템 관리자 - ") + QString(APP_VERSION));
//    setWindowIcon(QIcon("up.png"));

    LoadData();
    rowSelected = new QMap<int, int>();
}

MainWindow::~MainWindow()
{
    SaveData();
    delete ui;
    delete rowSelected;
}

void MainWindow::ClearTable(QTableWidget *table) {
    for(int i = 0; i < table->rowCount(); ++i) {
        table->setItem(i, 0, new QTableWidgetItem(""));
        table->setItem(i, 1, new QTableWidgetItem(""));
        table->setItem(i, 2, new QTableWidgetItem(""));
    }
}
void MainWindow::on_pushButton_clicked()    // 불러오기
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Excel File"), ".",
                                                    tr("Excel File (*.xlsx)"));
    if(QString::compare(filename, "", Qt::CaseInsensitive) == 0) {
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);

    QAxObject* excel = new QAxObject( "Excel.Application", 0 );
    QAxObject* workbooks = excel->querySubObject( "Workbooks" );
//    QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", QString(filename));
    QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", filename.replace("/", "\\"));
    QAxObject* sheets = workbook->querySubObject( "Worksheets" );
    QList<QVariantList> data; //Data list from excel, each QVariantList is worksheet row

    ClearTable(ui->tableWidget);
    ClearTable(ui->matchTableWidget);
    //worksheets count
    auto sheet = sheets->querySubObject("Item(int)", 1);
    for (int r = 1; true ; ++r) {
        auto cCell1 = sheet->querySubObject("Cells(int,int)", r + 1, 1);    // 이름
        auto cCell2 = sheet->querySubObject("Cells(int,int)", r + 1, 2);    // 가격
        auto cCell3 = sheet->querySubObject("Cells(int,int)", r + 1, 3);    // 재고
        auto cCell4 = sheet->querySubObject("Cells(int,int)", r + 1, 4);    // 활성화
        auto cCell5 = sheet->querySubObject("Cells(int,int)", r + 1, 5);    // 태그

        QVariant data1 = cCell1->dynamicCall("Value()");
        QVariant data2 = cCell2->dynamicCall("Value()");
        QVariant data3 = cCell3->dynamicCall("Value()");
        QVariant data4 = cCell4->dynamicCall("Value()");
        QVariant data5 = cCell5->dynamicCall("Value()");

        if(QString::compare(data1.toString(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        QString strPrice = data2.toString();
        QStringList splitPrice = strPrice.split("또는");

        bool isActivated;
        if(data4.toString().length() > 0) {
            isActivated = true;
        }
        else {
            isActivated = false;
        }
        ui->tableWidget->AddItem(data1.toString(), splitPrice[0].trimmed(), data3.toString(), isActivated, data5.toString(), r - 1);
    }

    sheet = sheets->querySubObject("Item(int)", 2);
    for (int r = 1; r <= true; ++r) {
        auto cCell1 = sheet->querySubObject("Cells(int,int)", r + 1, 1);
        auto cCell2 = sheet->querySubObject("Cells(int,int)", r + 1, 2);
        auto cCell3 = sheet->querySubObject("Cells(int,int)", r + 1, 3);
        QVariant data1 = cCell1->dynamicCall("Value()");
        QVariant data2 = cCell2->dynamicCall("Value()");
        QVariant data3 = cCell3->dynamicCall("Value()");
        if(QString::compare(data1.toString(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        ui->matchTableWidget->AddItem(data1.toString(), data2.toString(), data3.toString(), r - 1);
    }

    // 내보내기 테이블 초기화
    ClearTable(ui->exportTableWidget);
    sheet = sheets->querySubObject("Item(int)", 3);
    for (int r = 1; r <= true; ++r) {
        auto cCell1 = sheet->querySubObject("Cells(int,int)", r + 1, 1);    // 태그
        auto cCell2 = sheet->querySubObject("Cells(int,int)", r + 1, 2);    // txt 파일변환
        auto cCell3 = sheet->querySubObject("Cells(int,int)", r + 1, 3);    // 글자크기
        auto cCell4 = sheet->querySubObject("Cells(int,int)", r + 1, 4);    // 굵게
        auto cCell5 = sheet->querySubObject("Cells(int,int)", r + 1, 5);    // 이탤릭체
        auto cCell6 = sheet->querySubObject("Cells(int,int)", r + 1, 6);    // 밑줄
        auto cCell7 = sheet->querySubObject("Cells(int,int)", r + 1, 7);    // 배경색
        auto cCell8 = sheet->querySubObject("Cells(int,int)", r + 1, 8);    // 글자색

        QVariant data1 = cCell1->dynamicCall("Value()");
        QVariant data2 = cCell2->dynamicCall("Value()");
        QVariant data3 = cCell3->dynamicCall("Value()");
        QVariant data4 = cCell4->dynamicCall("Value()");
        QVariant data5 = cCell5->dynamicCall("Value()");
        QVariant data6 = cCell6->dynamicCall("Value()");
        QVariant data7 = cCell7->dynamicCall("Value()");
        QVariant data8 = cCell8->dynamicCall("Value()");

        if(QString::compare(data1.toString(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        ui->exportTableWidget->AddItem(data1.toString(), data2.toString(), data3.toString(), data4.toString(),
                                       data5.toString(), data6.toString(), data7.toString(), data8.toString(), r - 1);
    }

    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;

    QApplication::restoreOverrideCursor();

    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setText("완료되었습니다");
    QApplication::alert(this);
    msgBox.exec();
}

void excelSetColumnWidth(QAxObject *sheet, int column, int width)
{
    QString columnName;
    columnName.append(QChar(column - 1 + 'A'));
    columnName.append(":");
    columnName.append(QChar(column - 1 + 'A'));

    QAxObject * col = sheet->querySubObject("Columns(const QString&)", columnName);
    col->setProperty("ColumnWidth", width);
}

QString MainWindow::GetEquivalentItem(float price, int option)
{
    QString item_text = "";
    for(int i = 0; i < ui->matchTableWidget->rowCount(); ++i)
    {
        if(ui->matchTableWidget->item(i, option) == nullptr) break;
        QString price_range_str = ui->matchTableWidget->item(i, 0)->text();
        if(QString::compare(price_range_str, "") == 0) break;
        QStringList price_range = price_range_str.split('-');
        if(price_range.length() == 1) {
            if(price == price_range[0].trimmed().toFloat()) {
                item_text = ui->matchTableWidget->item(i, option)->text();
                break;
            }
        }
        else {
            if(price >= price_range[0].trimmed().toFloat() && price <= price_range[1].trimmed().toFloat()) {
                item_text = ui->matchTableWidget->item(i, option)->text();
                break;
             }
        }
    }
    return item_text;
}
/*
 * https://wiki.qt.io/Handling_Microsoft_Excel_file_format
 * https://cpp.hotexamples.com/de/examples/-/QAxObject/-/cpp-qaxobject-class-examples.html
 */

void MainWindow::exportToExcel(QString filename, bool is_only_editable, int export_option)  // 엑셀로 내보내기
{
    auto excel     = new QAxObject("Excel.Application");
    auto workbooks = excel->querySubObject("Workbooks");
    auto workbook  = workbooks->querySubObject("Add");
    auto sheets    = workbook->querySubObject("Worksheets");
    auto sheet     = sheets->querySubObject("Item(int)",1);

    auto row       = 1;
    sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value","품 명");
    sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value","가 격");
    sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value","재 고");
    sheet->querySubObject("Cells(Int,Int)",row,4)->setProperty("Value","활성화");
    sheet->querySubObject("Cells(Int,Int)",row,5)->setProperty("Value","태 그");
    excelSetColumnWidth(sheet, 1, 30);
    excelSetColumnWidth(sheet, 2, 30);
    excelSetColumnWidth(sheet, 3, 10);
    excelSetColumnWidth(sheet, 4, 10);
    excelSetColumnWidth(sheet, 5, 50);
    row++;
    for(int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if(ui->tableWidget->item(i, 0) == nullptr) break;
        // 행이 비어있으면 더이상 아이템이 없다고 간주
        if(QString::compare(ui->tableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        CellItemChkBox *chk = (CellItemChkBox *)ui->tableWidget->cellWidget(i, 3);
        bool isActivated = chk->GetCheck() == Qt::Checked;
        if(!is_only_editable || isActivated) {
            QAxObject *range = sheet->querySubObject("Cells(int,int)", row, 1);
            range = range->querySubObject("Font");
            range->setProperty("Size", 10);
            range->setProperty("Name", "Segoe UI Emoji");

            sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value",ui->tableWidget->item(i, 0)->text());
            if(QString::compare(ui->tableWidget->item(i,1)->text(), "") == 0) {
                sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value","");
            }
            else {
                float price_multiplied = ui->tableWidget->item(i,1)->text().toFloat() * ui->lineEdit->text().toFloat();
                QStringList strList;
                strList << QString::number((int)price_multiplied);
//                qInfo() << QString::number((int)price_multiplied);
                QString strEquivItem = GetEquivalentItem(price_multiplied, export_option);
//                qInfo() << strEquivItem;
                if(export_option > 0 && QString::compare(strEquivItem, "") != 0) {
                    strList << " 또는 " << strEquivItem;
                }
                sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value",strList.join(""));
            }
            // 재고
            CellItemSpinBox *spin = (CellItemSpinBox *)ui->tableWidget->cellWidget(i, 2);
            if(spin != nullptr) {
                sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value",QString::number(spin->spinBox->value()).toUtf8().constData());
            }
            // 활성화
            sheet->querySubObject("Cells(Int,Int)",row,4)->setProperty("Value",isActivated?"O":"");

            // 태그
            sheet->querySubObject("Cells(Int,Int)",row,5)->setProperty("Value",ui->tableWidget->item(i, 4)->text());

            row++;
        }
    }

    // Sheets number
    int intCount = sheets->property("Count").toInt();

    // Capture last sheet and add new sheet
    QAxObject* lastSheet = sheets->querySubObject("Item(int)", intCount);
    sheets->dynamicCall("Add(QVariant)", lastSheet->asVariant());

    // Capture the new sheet and move to after last sheet
    QAxObject* newSheet = sheets->querySubObject("Item(int)", intCount);
    lastSheet->dynamicCall("Move(QVariant)", newSheet->asVariant());

    sheet = newSheet;
    row       = 1;
    sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value","가격 범위");
    sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value","아이템 1");
    sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value","아이템 2");
    excelSetColumnWidth(sheet, 1, 20);
    excelSetColumnWidth(sheet, 2, 20);
    excelSetColumnWidth(sheet, 3, 20);
    row++;
    for(int i = 0; i < ui->matchTableWidget->rowCount(); ++i) {
        if(ui->matchTableWidget->item(i, 0) == nullptr) break;
        if(QString::compare(ui->matchTableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value","'"+ui->matchTableWidget->item(i, 0)->text());   // "'"는 셀이 텍스트 속정을 갖게 하기 위함
        sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value",ui->matchTableWidget->item(i, 1)->text());
        sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value",ui->matchTableWidget->item(i, 2)->text());
        row++;
    }

    // 내보내기 테이블
    // Sheets number
    intCount = sheets->property("Count").toInt();

    // Capture last sheet and add new sheet
    lastSheet = sheets->querySubObject("Item(int)", intCount);
    sheets->dynamicCall("Add(QVariant)", lastSheet->asVariant());

    // Capture the new sheet and move to after last sheet
    newSheet = sheets->querySubObject("Item(int)", intCount);
    lastSheet->dynamicCall("Move(QVariant)", newSheet->asVariant());

    sheet = newSheet;
    row       = 1;
    sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value","태그/항목");
    sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value","txt 내보내기");
    sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value","글자크기");
    sheet->querySubObject("Cells(Int,Int)",row,4)->setProperty("Value","굵게");
    sheet->querySubObject("Cells(Int,Int)",row,5)->setProperty("Value","기울이기");
    sheet->querySubObject("Cells(Int,Int)",row,6)->setProperty("Value","밑줄");
    sheet->querySubObject("Cells(Int,Int)",row,7)->setProperty("Value","바탕색");
    sheet->querySubObject("Cells(Int,Int)",row,8)->setProperty("Value","글자색");
    excelSetColumnWidth(sheet, 1, 20);
    excelSetColumnWidth(sheet, 2, 20);
    excelSetColumnWidth(sheet, 3, 20);
    excelSetColumnWidth(sheet, 4, 20);
    excelSetColumnWidth(sheet, 5, 20);
    excelSetColumnWidth(sheet, 6, 20);
    excelSetColumnWidth(sheet, 7, 20);
    excelSetColumnWidth(sheet, 8, 20);
    row++;
    for(int i = 0; i < ui->exportTableWidget->rowCount(); ++i) {
        if(ui->exportTableWidget->item(i, 0) == nullptr) break;
        if(QString::compare(ui->exportTableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value",ui->exportTableWidget->item(i, 0)->text());
        sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value",ui->exportTableWidget->item(i, 1)->text());
        sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value",ui->exportTableWidget->item(i, 2)->text());
        sheet->querySubObject("Cells(Int,Int)",row,4)->setProperty("Value",ui->exportTableWidget->item(i, 3)->text());
        sheet->querySubObject("Cells(Int,Int)",row,5)->setProperty("Value",ui->exportTableWidget->item(i, 4)->text());
        sheet->querySubObject("Cells(Int,Int)",row,6)->setProperty("Value",ui->exportTableWidget->item(i, 5)->text());
        sheet->querySubObject("Cells(Int,Int)",row,7)->setProperty("Value",ui->exportTableWidget->item(i, 6)->text());
        sheet->querySubObject("Cells(Int,Int)",row,8)->setProperty("Value",ui->exportTableWidget->item(i, 7)->text());
        row++;
    }





    QString documentPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QStringList split_filename = filename.split("/");
    QString filename_only = split_filename.at(split_filename.length() - 1);

    split_filename.removeAt(split_filename.length() - 1);
    QString current_path = split_filename.join("/");
//    qInfo() << current_path;

    QFile::remove(documentPath+"/"+filename_only);
//    qInfo() << filename_only;
//    qInfo() << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
//    qInfo() << filename.replace("/", "\\");
    QFile::remove(filename);
    workbook->dynamicCall("SaveAs(QString&)", filename.replace("/", "\\"));
//    workbook->dynamicCall("SaveAs (const QString&,int,const QString&,const QString&,bool,bool)",filename,56,QString(""),QString(""),false,false);
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;

//    QFile::remove(filename);
//    QFile::copy(current_path+, filename);
//    workbook->dynamicCall("SaveAs(QString&)",filename);

}
QString MainWindow::StripTag(QString str)
{
    Parser parse(str);
    parse.Parse();
    return parse.toTxt();
}

void MainWindow::exportToTxt(QString filename, bool is_only_editable, int export_option)
{
    QStringList splitFilename = filename.split(".");
    splitFilename.removeAt(splitFilename.length() - 1);
    splitFilename << ".txt";
    QFile file(splitFilename.join(""));
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qInfo() << "error in opening file";
        return;
    }
    QTextStream out(&file);
    out.setCodec("UTF-8");
    QString strOut;
    for(int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if(ui->tableWidget->item(i, 0) == nullptr) break;
        // 행이 비어있으면 더이상 아이템이 없다고 간주
        if(QString::compare(ui->tableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        CellItemChkBox *chk = (CellItemChkBox *)ui->tableWidget->cellWidget(i, 3);
        bool isActivated = chk->GetCheck() == Qt::Checked;
        if(!is_only_editable || isActivated) {
            QString strCell1;
            if(QString::compare(ui->tableWidget->item(i, 0)->text(), "@@") == 0) {
                out << "\n";
                continue;
            }
            else {
                QString stripped = StripTag(ui->tableWidget->item(i, 0)->text());
                strCell1.sprintf("%s", stripped.toUtf8().constData());
                out << strCell1;
            }
            // 가격 출력
            if(QString::compare(ui->tableWidget->item(i,1)->text(), "") == 0) {
                strCell1.sprintf("%s", "");
                out << strCell1;
                out << "\n";
                continue;
            }
            else {
                float price_multiplied = ui->tableWidget->item(i,1)->text().toFloat() * ui->lineEdit->text().toFloat();
                QStringList strList;
                strList << QString::number((int)price_multiplied);
//                qInfo() << QString::number((int)price_multiplied);
                QString strEquivItem = GetEquivalentItem(price_multiplied, export_option);
//                qInfo() << strEquivItem;
                if(export_option > 0 && QString::compare(strEquivItem, "") != 0) {
                    strList << " 또는 " << strEquivItem;
                }
                strCell1.sprintf("%s",strList.join("").toUtf8().constData());
                out << " - ";
                out << strCell1;
            }
            // 재고 출력
            strOut.sprintf("%s", QString::fromUtf8(" (재고: ").toUtf8().constData());
            out << strOut;
            CellItemSpinBox *spin = (CellItemSpinBox *)ui->tableWidget->cellWidget(i, 2);
            if(spin == nullptr) {
                out << "0";
            }
            else {
                strCell1.sprintf("%s", QString::number(spin->spinBox->value()).toUtf8().constData());
                out << strCell1;
            }
            strOut.sprintf("%s", QString::fromUtf8("개)\n").toUtf8().constData());
            out << strOut;
        }
    }
    file.flush();
    file.close();

}

QString MainWindow::convertToHtml(QString strInput)
{
    Parser parse(strInput);
    parse.Parse();
    return parse.toHtml();
}
int MainWindow::GetRowByNameInExportTable(QString str)
{
    int row = 0;
    foreach(const QJsonValue & arr, json_export){
        foreach(const QJsonValue & v, arr.toArray()){
            if(v.toString().contains(str)) {
                return row;
            }
        }
        row++;
    }
    return -1;  // not found
}
void MainWindow::GenerateStyleTag(int row_export, QTextStream &out, QString text, int colspan)
{
    QString color;
    if(ui->exportTableWidget->item(row_export, 6) == nullptr) {
        color = "";
    }
    else {
        color = ui->exportTableWidget->item(row_export, 6)->text();
    }
    if(row_export != -1) {
        if(QString::compare(color, "") != 0) {
            if(colspan > 0) {
                out << tr("<td bgcolor=\"") << color
                    << tr("\" colspan=\"") << QString::number(colspan)
                    << "\" style=\"text-align:center\">";
            }
            else {
                out << tr("<td bgcolor=\"") << color << tr("\">");
            }
        }
        else {
            if(colspan > 0) {
                out << tr("<td colspan=\"") << QString::number(colspan) << "\" style=\"text-align:center\">";
            }
            else {
                out << tr("<td>");
            }
        }
    }
    else {
        out << tr("<td>");
    }
    if(row_export != -1) {
        QString fontsize;
        QString fontweight;
        QString fontstyle;
        QString text_decoration;
        QString fontcolor;
        QString style;
        if(QString::compare(ui->exportTableWidget->item(row_export, 2)->text(), "") != 0) {  // 글자크기
            fontsize += ui->exportTableWidget->item(row_export, 2)->text() + "rem";
        }
        if(QString::compare(ui->exportTableWidget->item(row_export, 3)->text(), "") != 0) {
            fontweight += " bold";
        }
        if(QString::compare(ui->exportTableWidget->item(row_export, 4)->text(), "") != 0) {
            fontstyle += " italic";
        }
        if(QString::compare(ui->exportTableWidget->item(row_export, 5)->text(), "") != 0) {
            text_decoration += "underline";
        }
        if(QString::compare(ui->exportTableWidget->item(row_export, 7)->text(), "") != 0) {
            fontcolor += ui->exportTableWidget->item(row_export, 7)->text();
        }
        if(fontsize.length() > 0) {
            style += "font-size: " + fontsize + ";";
        }
        if(fontweight.length() > 0) {
            style += "font-weight: bold;";
        }
        if(text_decoration.length() > 0) {  // underline
            style += "text-decoration: underline;";
        }
        if(fontcolor.length() > 0) {
            style += "color: " + fontcolor + ";";
        }
        if(style.length() > 0) {
            out << "<div style=\"" << style << "\">";
            out << text << "</div>";
        }
        else {
            out << "<div>" << text << "</div>";
        }
    }

}
int MainWindow::FindExportTag(QMap<QString, int> &map, QString srcText)
{
    QList<QString> tags = map.keys();
    for(QList<QString>::iterator itr = tags.begin(); itr < tags.end(); ++itr)
    {
        if(srcText.contains(*itr)) {
            return map.value(*itr);
        }
    }
    return -1;
}
void MainWindow::exportToHtml(QString filename, bool is_only_editable, int export_option)
{
    QStringList splitFilename = filename.split(".");
    splitFilename.removeAt(splitFilename.length() - 1);
    splitFilename << ".html";
    QFile file(splitFilename.join(""));
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        qInfo() << "error in opening file";
        return;
    }

    MakeExportJson();
    int price_row = GetRowByNameInExportTable("* 가격");
    int stock_row = GetRowByNameInExportTable("* 재고");
    int tag_row = GetRowByNameInExportTable("* 검색어");
    QMap<QString, int> exportTag;
    for(int i = 0; i<ui->exportTableWidget->rowCount(); ++i) {
        QTableWidgetItem *item = ui->exportTableWidget->item(i, 0);
        if(item == nullptr) break;
        exportTag.insert(item->text().mid(item->text().indexOf("<")), i);
    }
    qInfo() << exportTag;

    QTextStream out(&file);
//    QTextStream out(stderr);
    out.setCodec("UTF-8");
    QString strOut;

    out << tr("<html>\n");
    out << tr("<head></head>\n");
    out << tr("<body>\n");
    out << tr("<table border=\"1\" width=\"100%\" style=\"border-collapse:collapse\">\n");

    out << tr("<tr>\n");
    out << tr("<td>품명</td>\n");
    out << tr("<td>가격</td>\n");
    out << tr("<td>재고</td>\n");
    out << tr("<td>검색어</td>\n");
    out << tr("</tr>\n");

    for(int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        if(ui->tableWidget->item(i, 0) == nullptr) break;
        // 행이 비어있으면 더이상 아이템이 없다고 간주
        if(QString::compare(ui->tableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        CellItemChkBox *chk = (CellItemChkBox *)ui->tableWidget->cellWidget(i, 3);
        bool isActivated = chk->GetCheck() == Qt::Checked;
        if(!is_only_editable || isActivated) {
            QString strCell1;
            int exportRow;
            if(QString::compare(ui->tableWidget->item(i, 0)->text(), "@@") == 0) {
                continue;
            }
            else if((exportRow = FindExportTag(exportTag, ui->tableWidget->item(i, 0)->text())) != -1) {
                strCell1.sprintf("%s", ui->tableWidget->item(i, 0)->text().toUtf8().constData());
                out << tr("<tr>\n");
                GenerateStyleTag(exportRow, out, strCell1.replace(exportTag.key(exportRow), ""), 4);
                out << tr("</td></tr>\n");
                continue;
            }
            else {
                // 품명
                strCell1.sprintf("%s", ui->tableWidget->item(i, 0)->text().toUtf8().constData());
                out << tr("<tr>\n");
                out << tr("<td>") << convertToHtml(strCell1) << tr("</td>\n");
            }
            // 가격 출력
            if(QString::compare(ui->tableWidget->item(i,1)->text(), "") == 0) {
                out << tr("<td></td>\n");
            }
            else {
                float price_multiplied = ui->tableWidget->item(i,1)->text().toFloat() * ui->lineEdit->text().toFloat();
                QStringList strList;
                strList << QString::number((int)price_multiplied);
//                qInfo() << QString::number((int)price_multiplied);
                QString strEquivItem = GetEquivalentItem(price_multiplied, export_option);
//                qInfo() << strEquivItem;
                if(export_option > 0 && QString::compare(strEquivItem, "") != 0) {
                    strList << " 또는 " << strEquivItem;
                }
                strCell1.sprintf("%s",strList.join("").toUtf8().constData());
                GenerateStyleTag(price_row, out, strCell1);
                out << tr("</td>\n");
            }
            // 재고 출력
            CellItemSpinBox *spin = (CellItemSpinBox *)ui->tableWidget->cellWidget(i, 2);
            if(spin == nullptr) {
                out << "0" ;
            }
            else {
                QString strStock = QString::number(spin->spinBox->value()).toUtf8().constData();
//                strCell1.arg("%s", strStock);
                GenerateStyleTag(stock_row, out, strStock);
            }
            out << tr("</td>\n");
            // 검색어
            if(ui->tableWidget->item(i,4) != nullptr) {
                GenerateStyleTag(tag_row, out, ui->tableWidget->item(i,4)->text());
            }
            out << tr("</td>\n");

        }
        out << tr("</tr>\n");
    }

    out << tr("</table>\n");
    out << tr("</body>\n");
    out << tr("</html>\n");

    file.flush();
    file.close();

}

void MainWindow::on_pushButton_2_clicked()  // 내보내기(모두)
{
//    QString testStr = "ddd<b>bo<b>l</b>d</b>";
//    convertToHtml(testStr);
//    convertToHtml(ui->tableWidget->item(5, 0)->text());
//    return;

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Excel File"), ".",
                                                    tr("Excel File (*.xlsx)"));
    if(QString::compare(filename, "", Qt::CaseInsensitive) == 0) {
        return;
    }
//    qInfo() << filename;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    exportToExcel(filename, false, ui->comboBox->currentIndex());
    exportToTxt(filename, false, ui->comboBox->currentIndex());
    exportToHtml(filename, false, ui->comboBox->currentIndex());

    QApplication::restoreOverrideCursor();

    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setText("완료되었습니다");
    QApplication::alert(this);
    msgBox.exec();
}

void MainWindow::on_pushButton_3_clicked()  // 내보내기(활성화만)
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Excel File"), ".",
                                                    tr("Excel File (*.xlsx)"));
    if(QString::compare(filename, "", Qt::CaseInsensitive) == 0) {
        return;
    }
//    qInfo() << filename;
    QApplication::setOverrideCursor(Qt::WaitCursor);

    exportToExcel(filename, true, ui->comboBox->currentIndex());
    exportToTxt(filename, true, ui->comboBox->currentIndex());
    exportToHtml(filename, true, ui->comboBox->currentIndex());

    QApplication::restoreOverrideCursor();

    QMessageBox msgBox;
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setText("완료되었습니다");
    QApplication::alert(this);
    msgBox.exec();
}


void MainWindow::changeEvent(QEvent *event)
{
    qInfo() << event;
}
struct str_num_compare {
  bool operator() (const QString& lhs, const QString& rhs) const
  {return lhs.toInt()<rhs.toInt();}
};
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    qInfo() << "MainWindow::onKeyPressEvent";
    if(event->key() == Qt::Key_Return) {
    }
    else if(event->key() == Qt::Key_Delete) {
        QTableWidget *widget;
        switch(ui->tabWidget->currentIndex()) {
        case 0:
            widget = ui->tableWidget;
            break;
        case 1:
            widget = ui->matchTableWidget;
            break;
        case 2:
            widget = ui->exportTableWidget;
            break;
        }

        int row = widget->currentRow();
        if(row == -1) return;
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setText("지우시겠습니까?");
        if(msgBox.exec() == QMessageBox::Ok) {
            QList<QTableWidgetSelectionRange> range = widget->selectedRanges();
            qInfo() << "다중삭제 " << range.length();
            rowSelected->clear();
            for(QList<QTableWidgetSelectionRange>::iterator selectionRange = range.begin();
                selectionRange != range.end(); selectionRange++) {
                qInfo() << selectionRange->topRow();
                qInfo() << selectionRange->bottomRow();
                for(int i = selectionRange->bottomRow(); i>=selectionRange->topRow() ; --i) {
                    if(rowSelected->contains(i)) continue;
                    rowSelected->insert(i, i);
                }
            }

            qDebug() << rowSelected->keys();
            QList<int> rows = rowSelected->keys();
            for (QList<int>::iterator pos = rows.end() - 1; pos>=rows.begin(); pos--) {
                qDebug() << QString::number(*pos);
                widget->removeRow(*pos);
            }

        }
    }
    else QMainWindow::keyPressEvent(event);
}
void MainWindow::FindValueChanged(QString text)
{
    qInfo() << "MainWindow::FindValueChanged, " << text;
    ui->tableWidget->Find(text);
}
bool MainWindow::LoadData()
{
//    QFile loadFile(QStringLiteral("D:\\prog\\itemManager\\itemManagerQt\\itemManager\\data.json"));
    QFile loadFile(QStringLiteral("data.json"));

    if(!loadFile.open(QIODevice::ReadOnly)){
        qWarning("Could not open json file to read");
        return false;
    }

    QByteArray loadData = loadFile.readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(loadData));
    jsonObj = loadDoc.object();

    QJsonValue items = jsonObj.value("items");
//    qInfo() << items.toArray();
    int row = 0;
    foreach(const QJsonValue & val, items.toArray()){
        QJsonArray cellValue = val.toArray();
        bool isActivated;
        if(cellValue[3].toString().length() > 0) {
            isActivated = true;
        }
        else {
            isActivated = false;
        }
        if(cellValue.size() < 5) {
            ui->tableWidget->AddItem(cellValue[0].toString(), cellValue[1].toString(), cellValue[2].toString(), isActivated, "", row);
        }
        else {
            ui->tableWidget->AddItem(cellValue[0].toString(), cellValue[1].toString(), cellValue[2].toString(), isActivated, cellValue[4].toString(), row);
        }
        row++;
    }

    items = jsonObj.value("matching");
//    qInfo() << items.toArray();
    row = 0;
    foreach(const QJsonValue & val, items.toArray()){
        QJsonArray cellValue = val.toArray();
        ui->matchTableWidget->AddItem(cellValue[0].toString(), cellValue[1].toString(), cellValue[2].toString(), row);
        row++;
    }


    items = jsonObj.value("export");
//    qInfo() << items.toArray();
    row = 0;
    foreach(const QJsonValue & val, items.toArray()){
        QJsonArray cellValue = val.toArray();
        ui->exportTableWidget->AddItem(cellValue[0].toString(), cellValue[1].toString(), cellValue[2].toString(),
                cellValue[3].toString(), cellValue[4].toString(), cellValue[5].toString(),
                cellValue[6].toString(), cellValue[7].toString(), row);
        row++;
    }

    items = jsonObj.value("row_height");
    int height = items.toString().toInt();
    for(int i=0; i<ui->tableWidget->rowCount(); ++i) {
        ui->tableWidget->setRowHeight(i, height);
    }

    // restore column width
    items = jsonObj.value("column_width");
    if(items != QJsonValue::Null) {
        {
            QJsonArray json_arr = items.toArray()[0].toArray();
            for(int i=0; i<ui->tableWidget->columnCount(); ++i) {
                ui->tableWidget->setColumnWidth(i, json_arr[i].toString().toInt());
            }
        }

        {
            QJsonArray json_arr = items.toArray()[1].toArray();
            for(int i=0; i<ui->matchTableWidget->columnCount(); ++i) {
                ui->matchTableWidget->setColumnWidth(i, json_arr[i].toString().toInt());
            }
        }

        {
            QJsonArray json_arr = items.toArray()[2].toArray();
            for(int i=0; i<ui->exportTableWidget->columnCount(); ++i) {
                ui->exportTableWidget->setColumnWidth(i, json_arr[i].toString().toInt());
            }
        }
    }


    return true;
}
void MainWindow::MakeExportJson()
{
    while(json_export.count()) {
        json_export.pop_back();
    }
    for(int i = 0; i < ui->exportTableWidget->rowCount(); ++i) {
        QJsonArray item;
        if(ui->exportTableWidget->item(i, 0) == nullptr) break;
        if(QString::compare(ui->exportTableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        for(int j = 0; j < ui->exportTableWidget->columnCount(); ++j) {
            QTableWidgetItem *cellitem = ui->exportTableWidget->item(i, j);
            if(cellitem == nullptr) {
                item.push_back("");
            }
            else {
                item.push_back(cellitem->text());
            }
        }
        json_export.push_back(item);
    }
}
bool MainWindow::SaveData()
{
    QJsonObject root;

    QJsonArray items;
    for(int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        QJsonArray item;
        if(ui->tableWidget->item(i, 0) == nullptr) break;
        if(QString::compare(ui->tableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        item.push_back(ui->tableWidget->item(i, 0)->text());

        // 가격
        item.push_back(ui->tableWidget->item(i, 1)->text());

        // 재고
//        item.push_back(ui->tableWidget->item(i, 2)->text());
        CellItemSpinBox *spin = (CellItemSpinBox *)ui->tableWidget->cellWidget(i, 2);
        if(spin != nullptr) {
            item.push_back(QString::number(spin->spinBox->value()));
        }
        else {
            item.push_back("");
        }

        CellItemChkBox *chk = (CellItemChkBox *)ui->tableWidget->cellWidget(i, 3);
        item.push_back(chk->GetCheck()?"true":"false");

        // 검색태그
        item.push_back(ui->tableWidget->item(i, 4)->text());
//        item.push_back("");

        items.push_back(item);
    }
    root["items"] = items;

    QJsonArray items1;
    for(int i = 0; i < ui->matchTableWidget->rowCount(); ++i) {
        QJsonArray item;
        if(ui->matchTableWidget->item(i, 0) == nullptr) break;
        if(QString::compare(ui->matchTableWidget->item(i, 0)->text(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        item.push_back(ui->matchTableWidget->item(i, 0)->text());
        item.push_back(ui->matchTableWidget->item(i, 1)->text());
        item.push_back(ui->matchTableWidget->item(i, 2)->text());
        items1.push_back(item);
    }
    root["matching"] = items1;


    MakeExportJson();
    root["export"] = json_export;
    root["row_height"] = QString::number(ui->tableWidget->rowHeight(0));

    QJsonArray json_column_width;
    {
        QJsonArray item;
        for(int i = 0; i < ui->tableWidget->columnCount(); ++i) {
            item.push_back(QString::number(ui->tableWidget->columnWidth(i)));
        }
        json_column_width.push_back(item);
    }
    {
        QJsonArray item;
        for(int i = 0; i < ui->matchTableWidget->columnCount(); ++i) {
            item.push_back(QString::number(ui->matchTableWidget->columnWidth(i)));
        }
        json_column_width.push_back(item);
    }
    {
        QJsonArray item;
        for(int i = 0; i < ui->exportTableWidget->columnCount(); ++i) {
            item.push_back(QString::number(ui->exportTableWidget->columnWidth(i)));
        }
        json_column_width.push_back(item);
    }
    root["column_width"] = json_column_width;

    QByteArray ba = QJsonDocument(root).toJson();
//    QTextStream ts(stdout);
//    ts << "rendered JSON" << endl;
//    ts << ba;
    {
        QFile fout("data.json");
        fout.open(QIODevice::WriteOnly);
        fout.write(ba);
    }


    return true;
}


void MainWindow::on_action_triggered()  // 새로만들기
{
    qInfo() << "MainWindow::on_action_triggered";
    NewItemDialog *dialog = new NewItemDialog();
    Ui_NewItemDialog uiDialog;
    uiDialog.setupUi(dialog);
//    dialog->layout()->setSizeConstraint(QLayout::SetFixedSize);
    dialog->setWindowTitle("새로만들기");
    dialog->setModal(true);
    if(dialog->exec() == QDialog::Accepted) {
        qInfo() << "accepted";
        qInfo() << uiDialog.lineEdit->text();
        ui->tableWidget->InsertItem(uiDialog.lineEdit->text(), uiDialog.lineEdit_2->text(), uiDialog.lineEdit_3->text(), true, "");
    }

}


void MainWindow::on_action_U_triggered()    // 복제하기
{
    qInfo() << "MainWindow::on_action_U_triggered";
    NewItemDialog *dialog = new NewItemDialog();
    Ui_NewItemDialog uiDialog;
    int row = ui->tableWidget->currentRow();
    if(row == -1) row = 0;
    uiDialog.setupUi(dialog);
    // 이름과 가격은 있고 재고는 비어있게
    uiDialog.lineEdit->setText(ui->tableWidget->item(row, 0)->text());
    uiDialog.lineEdit_2->setText(ui->tableWidget->item(row, 1)->text());
//    dialog->layout()->setSizeConstraint(QLayout::SetFixedSize);
    dialog->setWindowTitle("복제하기");
    dialog->setModal(true);
    if(dialog->exec() == QDialog::Accepted) {
        qInfo() << "accepted";
        qInfo() << uiDialog.lineEdit->text();
        ui->tableWidget->InsertItem(uiDialog.lineEdit->text(), uiDialog.lineEdit_2->text(), uiDialog.lineEdit_3->text(), true, "");
    }
}

void MainWindow::TableToggleItemEditAttribute(QTableWidgetItem *item)
{
    if(item->flags() & Qt::ItemIsEditable) {
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setBackground(QColor(200,200,200));
        item->setForeground(QBrush(QColor(255,255,255)));
    }
    else {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        item->setBackground(QColor(255,255,255));
        item->setForeground(QBrush(QColor(0,0,0)));
    }
}
void MainWindow::DisableTableRow(QTableWidget *tablewidget, int row)
{
//    TableToggleItemEditAttribute(tablewidget->item(row, 0));
//    TableToggleItemEditAttribute(tablewidget->item(row, 1));
//    TableToggleItemEditAttribute(tablewidget->item(row, 2));
    CellItemChkBox *chk = (CellItemChkBox *)tablewidget->cellWidget(row, 3);
    if(chk->GetCheck() == Qt::Checked) {
        chk->SetCheck(Qt::Unchecked);
    }
    else {
        chk->SetCheck(Qt::Checked);
    }
}

void MainWindow::on_action_E_triggered()    // 활성화 변경
{
    if(ui->tabWidget->currentIndex() == 0) {
        QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges();
        qInfo() << "활성화변경 " << range.length();
        rowSelected->clear();
        for(QList<QTableWidgetSelectionRange>::iterator selectionRange = range.begin();
            selectionRange != range.end(); selectionRange++) {
            qInfo() << selectionRange->topRow();
            qInfo() << selectionRange->bottomRow();
            for(int i = selectionRange->topRow(); i<=selectionRange->bottomRow() ; ++i) {
                if(rowSelected->contains(i)) continue;
                DisableTableRow(ui->tableWidget, i);
                rowSelected->insert(i, i);
            }
        }

    }
//    else {
//        QList<QTableWidgetSelectionRange> range = ui->matchTableWidget->selectedRanges();
//        for(QList<QTableWidgetSelectionRange>::iterator selectionRange = range.begin();
//            selectionRange != range.end(); selectionRange++) {
//            qInfo() << selectionRange->topRow();
//            qInfo() << selectionRange->bottomRow();
//            for(int i = selectionRange->topRow(); i<=selectionRange->bottomRow() ; ++i) {
//                DisableTableRow(ui->matchTableWidget, i);
//            }
//        }
//    }
}

bool MainWindow::SwapTableItem(QTableWidget *tablewidget, int rowSrc, int rowDest)
{
    CellItemChkBox *chkWidgetSrc = (CellItemChkBox *)tablewidget->cellWidget(rowSrc, 3);
    CellItemChkBox *chkWidgetDst = (CellItemChkBox *)tablewidget->cellWidget(rowDest, 3);
    if(chkWidgetSrc == nullptr || chkWidgetDst == nullptr) return false;

    QTableWidgetItem *itemSrc = tablewidget->takeItem(rowSrc, 0);
    QTableWidgetItem *itemDst = tablewidget->takeItem(rowDest, 0);
    tablewidget->setItem(rowSrc, 0, itemDst);
    tablewidget->setItem(rowDest, 0, itemSrc);
    itemSrc = tablewidget->takeItem(rowSrc, 1);
    itemDst = tablewidget->takeItem(rowDest, 1);
    tablewidget->setItem(rowSrc, 1, itemDst);
    tablewidget->setItem(rowDest, 1, itemSrc);

    // 재고
//    itemSrc = tablewidget->takeItem(rowSrc, 2);
//    itemDst = tablewidget->takeItem(rowDest, 2);
//    tablewidget->setItem(rowSrc, 2, itemDst);
//    tablewidget->setItem(rowDest, 2, itemSrc);
    CellItemSpinBox *spinWidgetSrc = (CellItemSpinBox *)tablewidget->cellWidget(rowSrc, 2);
    CellItemSpinBox *spinWidgetDst = (CellItemSpinBox *)tablewidget->cellWidget(rowDest, 2);
    int spinWidgetSrcValue = spinWidgetSrc->spinBox->value();
    int spinWidgetDstValue = spinWidgetDst->spinBox->value();
    spinWidgetSrc->spinBox->setValue(spinWidgetDstValue);
    spinWidgetDst->spinBox->setValue(spinWidgetSrcValue);

    // 활성화 체크 상태 swap
    Qt::CheckState chkSrc = chkWidgetSrc->GetCheck();
    Qt::CheckState chkDst = chkWidgetDst->GetCheck();
    chkWidgetSrc->SetCheck(chkDst);
    chkWidgetDst->SetCheck(chkSrc);
    return true;
}
bool MainWindow::CheckSwappable(QTableWidget *tablewidget, int rowSrc, int rowDest)
{
    CellItemChkBox *chkWidgetSrc = (CellItemChkBox *)tablewidget->cellWidget(rowSrc, 3);
    CellItemChkBox *chkWidgetDst = (CellItemChkBox *)tablewidget->cellWidget(rowDest, 3);
    if(chkWidgetSrc == nullptr || chkWidgetDst == nullptr) return false;
    return true;
}
void MainWindow::on_action_Q_triggered()    // 위로 이동
{
    int row = ui->tableWidget->currentRow();
    if(row == 0) return;

    if(ui->tabWidget->currentIndex() == 0) {

        QList<QModelIndex> *selectAfter = new QList<QModelIndex>();
        QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges();
        rowSelected->clear();
        qInfo() << "move up";
        for(QList<QTableWidgetSelectionRange>::iterator selectionRange = range.begin();
            selectionRange != range.end(); selectionRange++) {

            qInfo() << selectionRange->topRow();
            qInfo() << selectionRange->bottomRow();

            for(int i = selectionRange->topRow(); i<=selectionRange->bottomRow() ; ++i) {
                if(i == 0) continue;
                if(rowSelected->contains(i)) continue;
                rowSelected->insert(i, i);
                selectAfter->append(ui->tableWidget->model()->index(i-1, 0));
                selectAfter->append(ui->tableWidget->model()->index(i-1, 1));
                selectAfter->append(ui->tableWidget->model()->index(i-1, 2));
                selectAfter->append(ui->tableWidget->model()->index(i-1, 3));
            }
        }

        QList<int> keyValue = rowSelected->keys();
        for(QList<int>::iterator itr = keyValue.begin(); itr < keyValue.end(); ++itr) {
            qInfo() << QString::number(*itr);
            SwapTableItem(ui->tableWidget, *itr, *itr - 1);
        }

        ui->tableWidget->clearSelection();
        for(QList<QModelIndex>::iterator index = selectAfter->begin();
            index != selectAfter->end(); index++) {
            ui->tableWidget->selectionModel()->select(*index, QItemSelectionModel::Select);
        }
    }
}


void MainWindow::on_action_A_triggered()    // 아래로 이동
{
    int row = ui->tableWidget->currentRow();
    if(row == ui->tableWidget->rowCount() - 1) return;

    if(ui->tabWidget->currentIndex() == 0) {

        QList<QModelIndex> *selectAfter = new QList<QModelIndex>();
        QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges();
        rowSelected->clear();
        qInfo() << "move down";
        for(QList<QTableWidgetSelectionRange>::iterator selectionRange = range.begin();
            selectionRange != range.end(); selectionRange++) {
            qInfo() << selectionRange->topRow();
            qInfo() << selectionRange->bottomRow();

            for(int i = selectionRange->bottomRow(); i >= selectionRange->topRow() ; --i) {
                if(i > ui->tableWidget->rowCount() - 1) continue;
                if(rowSelected->contains(i)) continue;
                if(CheckSwappable(ui->tableWidget, i, i + 1) == true) {
                    selectAfter->append(ui->tableWidget->model()->index(i+1, 0));
                    selectAfter->append(ui->tableWidget->model()->index(i+1, 1));
                    selectAfter->append(ui->tableWidget->model()->index(i+1, 2));
                    selectAfter->append(ui->tableWidget->model()->index(i+1, 3));
                }
                rowSelected->insert(i, i);
            }
        }

        QList<int> keyValue = rowSelected->keys();
        for(QList<int>::iterator itr = keyValue.end()-1; itr >= keyValue.begin(); --itr) {
            qInfo() << QString::number(*itr);
            SwapTableItem(ui->tableWidget, *itr, *itr + 1);
        }

        ui->tableWidget->clearSelection();
        for(QList<QModelIndex>::iterator index = selectAfter->begin();
            index != selectAfter->end(); index++) {
            ui->tableWidget->selectionModel()->select(*index, QItemSelectionModel::Select);
        }
    }
}



