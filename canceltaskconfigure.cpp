#pragma execution_character_set("utf-8")

#include "canceltaskconfigure.h"
#include "globaldata.h"
#include "ui_canceltaskconfigure.h"
#include <QHBoxLayout>
#include <QScrollBar>


CancelTaskConfigure::CancelTaskConfigure(QWidget *parent, QList<int> click_sampleid) :
    QDialog(parent),
    ui(new Ui::CancelTaskConfigure)
{
    ui->setupUi(this);
    setAcceptDrops(true);//默认控件不支持拖拽，需在构造函数中设置接受拖拽事件
    // 仅保留关闭按钮
    this->setWindowTitle("取消样本任务");
    this->setWindowFlags(Qt::Dialog| Qt::WindowCloseButtonHint);


    mclickHole = click_sampleid;

    deleteitem.clear();

    initLayout();

    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
      settButtonQss = QLatin1String(styleFile.readAll());
      styleFile.close();
    }
    QHash<QPushButton*, QString> pushButtonList = {
         {ui->Cancel,tr("取消&&退出")},
         {ui->ok,tr("确定&&退出")}
    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
         QPushButton* button = it.key();
         QString text = it.value();
         // 使用button和text进行操作
         button->setStyleSheet(settButtonQss);
         button->setText(text);
    }

}

CancelTaskConfigure::~CancelTaskConfigure()
{
    delete ui;
}

void CancelTaskConfigure::initLayout(){
    ui->Cancel->setFocus();
    ui->ok->setFocus();
    ui->tableWidgetCancelTask->setColumnCount(2);
    QStringList header{tr("样本号"),tr("删除")};
    ui->tableWidgetCancelTask->setHorizontalHeaderLabels(header);
    QFont font;
    font.setFamily("楷体");
    //设置文字大小为50像素
    font.setPixelSize(14);
    font.setBold(true);
    ui->tableWidgetCancelTask->horizontalHeader()->setFont(font);
    ui->tableWidgetCancelTask->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui->tableWidgetCancelTask->verticalHeader()->setDefaultSectionSize(32); //设置行高
    ui->tableWidgetCancelTask->setSelectionMode(QAbstractItemView::SingleSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    ui->tableWidgetCancelTask->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    ui->tableWidgetCancelTask->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->tableWidgetCancelTask->horizontalHeader()->setFixedHeight(32); //设置表头的高度
    ui->tableWidgetCancelTask->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    ui->tableWidgetCancelTask->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 将根据整个列或行的内容自动调整区段的大小
    ui->tableWidgetCancelTask->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);    //x先自适应宽度
    ui->tableWidgetCancelTask->horizontalHeader()->setFixedHeight(40); //设置表头的高度
    ui->tableWidgetCancelTask->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    ui->tableWidgetCancelTask->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 将根据整个列或行的内容自动调整区段的大小
    ui->tableWidgetCancelTask->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);    //x先自适应宽度
    ui->tableWidgetCancelTask->setColumnWidth(0, 100);
    ui->tableWidgetCancelTask->setColumnWidth(1, 150);

    ui->tableWidgetCancelTask->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(188, 187, 186); font:14pt '楷体';color: black;};");
    ui->tableWidgetCancelTask->setStyleSheet(CancelTableWidgetCss);
    ui->tableWidgetCancelTask->verticalScrollBar()->setStyleSheet(CancelVScroBarCss); //垂直
    ui->tableWidgetCancelTask->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
         "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
         "QScrollBar::handle:hover{background:gray;}"
         "QScrollBar::sub-line{background:transparent;}"
         "QScrollBar::add-line{background:transparent;}");

    connect(ui->tableWidgetCancelTask,&QTableWidget::itemClicked,this,&CancelTaskConfigure::delenter);
    QPixmap Litterpix(":/Picture/SetPng/Litter.png");
    for(int Index : mclickHole)
    {
        int iRow = ui->tableWidgetCancelTask->rowCount();
        ui->tableWidgetCancelTask->insertRow(iRow);

        SetColumnText(ui->tableWidgetCancelTask,iRow,0,QString::number(Index));


        QWidget *widget = new QWidget;
        QHBoxLayout *hLayout = new QHBoxLayout();// 水平布局
        QLabel * DelTask =  new QLabel(this);
        DelTask->setPixmap(Litterpix);
        DelTask->setAttribute(Qt::WA_DeleteOnClose);
        DelTask->resize(Litterpix.size());
        DelTask->setMaximumHeight(32);

        hLayout->setMargin(0);      // 与窗体边无距离 尽量占满
        hLayout->addWidget(DelTask);     // 加入控件
        hLayout->setAlignment(DelTask, Qt::AlignCenter); // 控件居中
        widget->setLayout(hLayout);
        ui->tableWidgetCancelTask->setCellWidget(iRow,1,widget);
    }


}

void CancelTaskConfigure::delenter(QTableWidgetItem *item)
{
    QString delstr = item->text();
    deleteitem.push_back(delstr.toInt());
    ui->tableWidgetCancelTask->removeRow(item->row());
    return;
}


void CancelTaskConfigure::SetColumnText(QTableWidget * tablewiget,int row,int col,QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(QIcon(":/Picture/SetPng/tube.ico"),text);
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    tablewiget->setItem(row,col,item);
}


void CancelTaskConfigure::on_ok_clicked()
{
    emit  makesureCacelIdList(deleteitem);
	close();
    return;
}

void CancelTaskConfigure::on_Cancel_clicked()
{
    close();
}
