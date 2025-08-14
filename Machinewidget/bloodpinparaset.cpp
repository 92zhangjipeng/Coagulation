#include "bloodpinparaset.h"
#include "ui_bloodpinparaset.h"

BloodPinParaSet::BloodPinParaSet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BloodPinParaSet)
{
    ui->setupUi(this);
    ptable = ui->tableWidget_bloodpin;
}

BloodPinParaSet::~BloodPinParaSet()
{
    delete ui;
}

void BloodPinParaSet::initializeTable()
{
   ptable->setRowCount(14);     // 总行数
   ptable->setColumnCount(5);   // 5列

   // 设置字段表头
    QStringList headers;
    headers << "参数类型I" << "参数" << "   " << "参数类型II" << "参数";
    ptable->setHorizontalHeaderLabels(headers);
    ptable->verticalHeader()->setVisible(false); // 隐藏行号

    ptable->horizontalHeader()->setStyleSheet(
                "QHeaderView::section {"
                "   background-color: #005792;"  // 医疗蓝
                "   color: white;"
                "   padding: 12px 8px;"
                "   border: none;"
                "   font: bold 12pt 'Microsoft YaHei';"
                "}"
            );

    // 表格主体样式
    ptable->setStyleSheet(
                "QTableWidget {"
                "   background-color: #f0f9ff;"       // 主背景
                "   alternate-background-color: #e3f2fd;" // 交替行
                "   gridline-color: #b0d4f5;"         // 网格线颜色
                "}"
                "QTableWidget::item {"
                "   padding: 8px;"
                "   font: 11pt 'Segoe UI';"
                "   color: #333;"
                "}"
            );
    // 行高与列宽优化
    ptable->verticalHeader()->setDefaultSectionSize(40); // 固定行高

    // 列宽策略
    ptable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID列自适应
    ptable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
}

void BloodPinParaSet::insertParaBlood(QString types,int rows,int cols,
                      QVariant data,int datarows,int datacols){
    bloodPinData pdata;
    pdata.paraKind = types;
    pdata.paraKindRows = rows;
    pdata.paraKindCols = cols;

    pdata.paradata = data;
    pdata.paradataRows = datarows;
    pdata.paradataCols = datacols;
}

