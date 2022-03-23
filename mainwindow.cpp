#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAxObject>
#include <QDebug>
#include "ItemTable.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(3);
    QStringList tableHeader;
    tableHeader << "아이템" << "가격" << "재고";
    ui->tableWidget->setHorizontalHeaderLabels(tableHeader);
    ui->tableWidget->setRowCount(1000);
    ui->tableWidget->setColumnWidth(0, 160);
    ui->tableWidget->setColumnWidth(1, 80);
    ui->tableWidget->setColumnWidth(2, 80);

    setGeometry(100,100, 500, 600);
    setWindowTitle(QString("아이템 관리자 - ") + QString(APP_VERSION));
    setWindowIcon(QIcon("up.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()    // 불러오기
{
    qInfo() << "hi";
    QAxObject* excel = new QAxObject( "Excel.Application", 0 );
    QAxObject* workbooks = excel->querySubObject( "Workbooks" );
    QAxObject* workbook = workbooks->querySubObject( "Open(const QString&)", QString("D:\\prog\\itemManager\\itemManagerQt\\itemManager\\data.xlsx"));
    QAxObject* sheets = workbook->querySubObject( "Worksheets" );
    QList<QVariantList> data; //Data list from excel, each QVariantList is worksheet row

    //worksheets count
//    int count = sheets->dynamicCall("Count()").toInt();
    auto sheet = sheets->querySubObject("Item(int)", 1);
    for (int r = 1; r <= 3; ++r) {
        auto cCell1 = sheet->querySubObject("Cells(int,int)", r + 1, 1);
        auto cCell2 = sheet->querySubObject("Cells(int,int)", r + 1, 2);
        auto cCell3 = sheet->querySubObject("Cells(int,int)", r + 1, 3);
        QVariant data1 = cCell1->dynamicCall("Value()");
        QVariant data2 = cCell2->dynamicCall("Value()");
        QVariant data3 = cCell3->dynamicCall("Value()");
        qInfo() << data1.typeName();
        qInfo() << cCell1->dynamicCall("Value()");
        qInfo() << data2.typeName();
        qInfo() << cCell2->dynamicCall("Value()");
        qInfo() << data3.typeName();
        qInfo() << cCell3->dynamicCall("Value()");
        ui->tableWidget->AddItem(data1.toString(), data2.toString(), data3.toString(), r - 1);
    }
//    count = sheets->property("Count").toInt();
//    for (int i=1; i<=count; i++) //cycle through sheets
//     {
//        //sheet pointer
//        QAxObject* sheet = sheets->querySubObject( "Item( int )", i );

//        QAxObject* rows = sheet->querySubObject( "Rows" );
//        int rowCount = rows->dynamicCall( "Count()" ).toInt(); //unfortunately, always returns 255, so you have to check somehow validity of cell values
//        QAxObject* columns = sheet->querySubObject( "Columns" );
//        int columnCount = columns->property("Count").toInt();




//        for (int row=1; row <= rowCount; row++)
//        {
//            QVariantList dataRow;
//            bool isEmpty = true; //when all the cells of row are empty, it means that file is at end (of course, it maybe not right for different excel files. it's just criteria to calculate somehow row count for my file)
//            for (int column=1; column <= columnCount; column++)
//            {
//                //Do something usefule here
//            }
//            if (isEmpty) //criteria to get out of cycle
//                break;
//            data.append(dataRow);

//        }
//    }

    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
}


void MainWindow::on_pushButton_2_clicked()
{
    auto excel     = new QAxObject("Excel.Application");
    auto workbooks = excel->querySubObject("Workbooks");
    auto workbook  = workbooks->querySubObject("Add");
    auto sheets    = workbook->querySubObject("Worksheets");
    auto sheet     = sheets->querySubObject("Item(int)",1);

    auto row       = 1;
    auto column    = 1;
    sheet->querySubObject("Cells(Int,Int)",row++,column)->setProperty("Value","Hello");
    sheet->querySubObject("Cells(Int,Int)",row++,column)->setProperty("Value","Excel");

    workbook->dynamicCall("SaveAs(QString&)",QString("D:\\prog\\itemManager\\itemManagerQt\\itemManager\\output.xlsx"));
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
}


