#include "legacytaskcontinue.h"
#include "ui_legacytaskcontinue.h"

LegacyTaskContinue::LegacyTaskContinue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LegacyTaskContinue)
{
    ui->setupUi(this);
    //Initialize_the_interface(ui->tableWidget_LegacyTask);
}

LegacyTaskContinue::~LegacyTaskContinue()
{
    delete ui;
   
}

/*初始化界面*/
void LegacyTaskContinue::Initialize_the_interface(QTableWidget *Table)
{
    QStringList header;
    header<<tr("样本号")<<tr("项目 ")<<tr("数据")<<tr("异常 ")<<tr("参考值");
    Table->setHorizontalHeaderLabels(header);
    QFont font;
    font.setFamily("宋体");
    //设置文字大小为50像素
    font.setPixelSize(14);
    font.setBold(true);
    Table->horizontalHeader()->setFont(font);
    Table->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    Table->verticalHeader()->setVisible(false); //隐藏行表头(行号)
    Table->verticalHeader()->setDefaultSectionSize(30); //设置行高
    Table->setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    Table->horizontalHeader()->setFixedHeight(40); //设置表头的高度
    Table->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    Table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 将根据整个列或行的内容自动调整区段的大小
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);    //x先自适应宽度
}

/*继续任务*/
void LegacyTaskContinue::on_toolButton_continue_clicked()
{

}

/*异常样本== 已吸了血样的样本*/
void LegacyTaskContinue::SlotExcptionSampleNum(const int /*SampleNum*/){

}
