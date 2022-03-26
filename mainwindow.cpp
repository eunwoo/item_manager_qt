#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAxObject>
#include <QDebug>
#include "ItemTable.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QKeyEvent>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTableWidgetSelectionRange>
#include <QCheckBox>
#include <QMessageBox>
#include "newitemdialog.h"
#include "cellitemchkbox.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(4);
    QStringList tableHeader;
    tableHeader << "품 명" << "가 격" << "재 고" << "활성화";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget->setRowCount(1000);
    ui->tableWidget->setColumnWidth(0, 260);
    ui->tableWidget->setColumnWidth(1, 80);
    ui->tableWidget->setColumnWidth(2, 80);
    ui->tableWidget->setColumnWidth(3, 60);
    connect(ui->lineEdit_2, SIGNAL(FindValueChanged(QString)), this, SLOT(FindValueChanged(QString)));

    ui->matchTableWidget->setColumnCount(3);
    tableHeader.clear();
    tableHeader << "가격 범위" << "아이템 1" << "아이템 2";
    ui->matchTableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->matchTableWidget->setRowCount(1000);
    ui->matchTableWidget->setColumnWidth(0, 160);
    ui->matchTableWidget->setColumnWidth(1, 80);
    ui->matchTableWidget->setColumnWidth(2, 80);


    ui->lineEdit->setText("1.0");
    ui->comboBox->addItem("그대로");
    ui->comboBox->addItem("아이템1");
    ui->comboBox->addItem("아이템2");

    setGeometry(100,100, 600, 800);
    setWindowTitle(QString("아이템 관리자 - ") + QString(APP_VERSION));
    setWindowIcon(QIcon("up.png"));

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
    qInfo() << "hi";
    QString filename = QFileDialog::getOpenFileName(this, tr("Open Excel File"), ".",
                                                    tr("Excel File (*.xlsx)"));
    if(QString::compare(filename, "", Qt::CaseInsensitive) == 0) {
        return;
    }
    QAxObject* excel = new QAxObject( "Excel.Application", 0 );
    QAxObject* workbooks = excel->querySubObject( "Workbooks" );
    QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", QString(filename));
    QAxObject* sheets = workbook->querySubObject( "Worksheets" );
    QList<QVariantList> data; //Data list from excel, each QVariantList is worksheet row

    ClearTable(ui->tableWidget);
    ClearTable(ui->matchTableWidget);
    //worksheets count
    auto sheet = sheets->querySubObject("Item(int)", 1);
    for (int r = 1; r <= 1000; ++r) {
        auto cCell1 = sheet->querySubObject("Cells(int,int)", r + 1, 1);
        auto cCell2 = sheet->querySubObject("Cells(int,int)", r + 1, 2);
        auto cCell3 = sheet->querySubObject("Cells(int,int)", r + 1, 3);
        QVariant data1 = cCell1->dynamicCall("Value()");
        QVariant data2 = cCell2->dynamicCall("Value()");
        QVariant data3 = cCell3->dynamicCall("Value()");
        if(QString::compare(data1.toString(), "", Qt::CaseInsensitive) == 0) {
            break;
        }
        QString strPrice = data2.toString();
        QStringList splitPrice = strPrice.split("또는");
        ui->tableWidget->AddItem(data1.toString(), splitPrice[0].trimmed(), data3.toString(), r - 1);

        auto cCell4 = sheet->querySubObject("Cells(int,int)", r + 1, 4);
        QVariant data4 = cCell4->dynamicCall("Value()");
        CellItemChkBox *cell_widget = new CellItemChkBox();
        if(data4.toString().length() > 0) {
            cell_widget->SetCheck(Qt::Checked);
        }
        else {
            cell_widget->SetCheck(Qt::Unchecked);
        }
        ui->tableWidget->setCellWidget(r-1, 3, cell_widget);
    }

    sheet = sheets->querySubObject("Item(int)", 2);
    for (int r = 1; r <= 1000; ++r) {
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

    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
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
    excelSetColumnWidth(sheet, 1, 30);
    excelSetColumnWidth(sheet, 2, 30);
    excelSetColumnWidth(sheet, 3, 10);
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
                qInfo() << QString::number((int)price_multiplied);
                QString strEquivItem = GetEquivalentItem(price_multiplied, export_option);
                qInfo() << strEquivItem;
                if(export_option > 0 && QString::compare(strEquivItem, "") != 0) {
                    strList << " 또는 " << strEquivItem;
                }
                sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value",strList.join(""));
            }
            sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value",ui->tableWidget->item(i, 2)->text());

            sheet->querySubObject("Cells(Int,Int)",row,4)->setProperty("Value",isActivated?"O":"");

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
        sheet->querySubObject("Cells(Int,Int)",row,1)->setProperty("Value","'"+ui->matchTableWidget->item(i, 0)->text());
        sheet->querySubObject("Cells(Int,Int)",row,2)->setProperty("Value",ui->matchTableWidget->item(i, 1)->text());
        sheet->querySubObject("Cells(Int,Int)",row,3)->setProperty("Value",ui->matchTableWidget->item(i, 2)->text());
        row++;
    }

    QString documentPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QStringList split_filename = filename.split("/");
    QString filename_only = split_filename.at(split_filename.length() - 1);

    split_filename.removeAt(split_filename.length() - 1);
    QString current_path = split_filename.join("/");
    qInfo() << current_path;

    QFile::remove(documentPath+"/"+filename_only);
    qInfo() << filename_only;
    qInfo() << QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    qInfo() << filename.replace("/", "\\");
    workbook->dynamicCall("SaveAs(QString&)", filename.replace("/", "\\"));
//    workbook->dynamicCall("SaveAs (const QString&,int,const QString&,const QString&,bool,bool)",filename,56,QString(""),QString(""),false,false);
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");

//    QFile::remove(filename);
//    QFile::copy(current_path+, filename);
//    workbook->dynamicCall("SaveAs(QString&)",filename);

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
                strCell1.sprintf("%s", ui->tableWidget->item(i, 0)->text().toUtf8().constData());
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
                qInfo() << QString::number((int)price_multiplied);
                QString strEquivItem = GetEquivalentItem(price_multiplied, export_option);
                qInfo() << strEquivItem;
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
            if(QString::compare(ui->tableWidget->item(i,2)->text(), "") == 0) {
                out << "0";
            }
            else {
                strCell1.sprintf("%s", ui->tableWidget->item(i, 2)->text().toUtf8().constData());
                out << strCell1;
            }
            strOut.sprintf("%s", QString::fromUtf8("개)\n").toUtf8().constData());
            out << strOut;
        }
    }
    file.flush();
    file.close();

}

void MainWindow::on_pushButton_2_clicked()  // 내보내기(모두)
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Excel File"), ".",
                                                    tr("Excel File (*.xlsx)"));
    if(QString::compare(filename, "", Qt::CaseInsensitive) == 0) {
        return;
    }
    qInfo() << filename;

    exportToExcel(filename, false, ui->comboBox->currentIndex());
    exportToTxt(filename, false, ui->comboBox->currentIndex());
}

void MainWindow::on_pushButton_3_clicked()  // 내보내기(활성화만)
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save Excel File"), ".",
                                                    tr("Excel File (*.xlsx)"));
    if(QString::compare(filename, "", Qt::CaseInsensitive) == 0) {
        return;
    }
    qInfo() << filename;

    exportToExcel(filename, true, ui->comboBox->currentIndex());
    exportToTxt(filename, true, ui->comboBox->currentIndex());
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
        int row = ui->tableWidget->currentRow();
        if(row == -1) return;
        QMessageBox msgBox;
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setText("지우시겠습니까?");
        if(msgBox.exec() == QMessageBox::Ok) {
            QList<QTableWidgetSelectionRange> range = ui->tableWidget->selectedRanges();
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
                ui->tableWidget->removeRow(*pos);
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
    qInfo() << items.toArray();
    int row = 0;
    foreach(const QJsonValue & val, items.toArray()){
        QJsonArray cellValue = val.toArray();
        ui->tableWidget->AddItem(cellValue[0].toString(), cellValue[1].toString(), cellValue[2].toString(), row);
        if(cellValue.size() > 3) {
            CellItemChkBox *cell_widget = new CellItemChkBox();
            if(QString::compare(cellValue[3].toString(), "true") == 0) {
                cell_widget->SetCheck(Qt::Checked);
            }
            else {
                cell_widget->SetCheck(Qt::Unchecked);
            }
            ui->tableWidget->setCellWidget(row, 3, cell_widget);
        }
        row++;
    }

    items = jsonObj.value("matching");
    qInfo() << items.toArray();
    row = 0;
    foreach(const QJsonValue & val, items.toArray()){
        QJsonArray cellValue = val.toArray();
        ui->matchTableWidget->AddItem(cellValue[0].toString(), cellValue[1].toString(), cellValue[2].toString(), row);
        row++;
    }
    return true;
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
        item.push_back(ui->tableWidget->item(i, 1)->text());
        item.push_back(ui->tableWidget->item(i, 2)->text());
        CellItemChkBox *chk = (CellItemChkBox *)ui->tableWidget->cellWidget(i, 3);
        item.push_back(chk->GetCheck()?"true":"false");
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

    QByteArray ba = QJsonDocument(root).toJson();
    QTextStream ts(stdout);
    ts << "rendered JSON" << endl;
    ts << ba;
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
        ui->tableWidget->InsertItem(uiDialog.lineEdit->text(), uiDialog.lineEdit_2->text(), uiDialog.lineEdit_3->text());
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
        ui->tableWidget->InsertItem(uiDialog.lineEdit->text(), uiDialog.lineEdit_2->text(), uiDialog.lineEdit_3->text());
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
    itemSrc = tablewidget->takeItem(rowSrc, 2);
    itemDst = tablewidget->takeItem(rowDest, 2);
    tablewidget->setItem(rowSrc, 2, itemDst);
    tablewidget->setItem(rowDest, 2, itemSrc);

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



