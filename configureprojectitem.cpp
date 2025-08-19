#pragma execution_character_set("utf-8")
#include "configureprojectitem.h"
#include "ui_configureprojectitem.h"
#include <QScrollBar>
#include <QDebug>
#include "cglobal.h"
#include "loginui.h"
#include "testing.h"
#include <operclass/fullyautomatedplatelets.h>


ConfigureProjectItem::ConfigureProjectItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConfigureProjectItem)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("配置测试项目"));
    bgmcColor.setRgb(188,187,183);
    setWindowFlags(Qt::Tool |
                   Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);
    ui->label_title->setText(tr("配置测试项目"));
    ui->groupBox_group->setTitle(tr("测试项目"));

    ui->label_Image->setFixedSize(32,32);
    QPixmap *pixmap = new QPixmap(":/Picture/suowei.png");
    pixmap->scaled(ui->label_Image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_Image->setScaledContents(true);
    ui->label_Image->setPixmap(*pixmap);
    delete pixmap;
    pixmap = nullptr;


    m_checkBoxList.clear();
    if(parent)
    {
        QRect rect = parent->geometry();
        int x = rect.x() + rect.width()/2 - this->width() /2;
        int y = rect.y() + rect.height()/2 - this->height()/2;
        this->move(x, y);
    }
    m_ptestingradioList <<ui->radioButton_AA<<ui->radioButton_ADP<<ui->radioButton_EPI<<ui->radioButton_COL<<ui->radioButton_RIS;


    //-组合
	QObject::connect(ui->toolButton_LossGroup, &QToolButton::clicked, this, [=]() { DeleteReagentGroup(); });

    //+组合
    QObject::connect(ui->toolButton_addtable,&QToolButton::clicked,this,[=](){ AddGroupReagentTest(); });

    //退出
    QObject::connect(ui->toolButtonCancel,&QToolButton::clicked,this,[=](){ this->close(); });
    QObject::connect(ui->pushButton_Close,&QToolButton::clicked,this,[=](){ this->close(); });

    //确定
    QObject::connect(ui->toolButtonSaved,&QToolButton::clicked,this,[=](){ ConfigTestingReagnet(); });
}

ConfigureProjectItem::~ConfigureProjectItem()
{
    qDeleteAll(m_checkBoxList);

    QLOG_DEBUG()<<"析构添加任务对象"<<__FUNCTION__ <<endl;
    delete ui;
}

void ConfigureProjectItem::_initstyle()
{
    int n = 0;
    QStringList _projectname;
    _projectname<<tr("AA")<<tr("ADP")<<tr("EPI")<<tr("COL")<<tr("RIS");
    foreach (QRadioButton *pvar, m_ptestingradioList)
    {
        pvar->setText(_projectname.at(n));
        pvar->setAutoExclusive(false);
        connect(pvar,&QRadioButton::clicked,this,[=](){
            if(pvar->isChecked()== true){
                CancelReagentGroup();
            }
        });
        n++;
    }



    QString pattern_str_ScrollBar = "QScrollBar{background:transparent; height:8px;}"
                          "QScrollBar::handle{background:lightgray; border:1px solid transparent; border-radius:3px;}"
                          "QScrollBar::handle:hover{background:gray;}"
                          "QScrollBar::sub-line{background:transparent;}"
                          "QScrollBar::add-line{background:transparent;}";

    QStringList TableTitleList;
    TableTitleList<<"组合试剂"<<"确认";
    ui->tableWidget_group->setFont(QFont("楷体", 14));
    ui->tableWidget_group->setColumnCount(2);
    QFont font =  ui->tableWidget_group->horizontalHeader()->font();
    font.setBold(true);
    ui->tableWidget_group->horizontalHeader()->setFont(font);
    ui->tableWidget_group->setHorizontalHeaderLabels(TableTitleList);


    ui->tableWidget_group->verticalHeader()->setVisible(false); //设置垂直头不可见
    ui->tableWidget_group->setSelectionMode(QAbstractItemView::ExtendedSelection); //可多选（Ctrl、Shift、  Ctrl+A都可以）
    ui->tableWidget_group->setSelectionBehavior(QAbstractItemView::SelectRows); //设置选择行为时每次选择一行
    ui->tableWidget_group->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑



     QString TableCss ="QTableWidget::item:hover{background-color:rgb(92,188,227,200)}"
    "QTableWidget::item:selected{background-color:#1B89A1}"
    "QHeaderView::section,QTableCornerButton:section{ \
    padding:3px; margin:0px; color:#DCDCDC;  border:1px solid #242424;font:16pt '楷体'; \
    border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
    background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
    "QTableWidget{background-color: rgb(188,187,183,200);border:none;}";

    ui->tableWidget_group->setStyleSheet(TableCss);

    ui->tableWidget_group->setColumnWidth((int)Table_TestReagentroup, 180);
    ui->tableWidget_group->horizontalScrollBar()->setStyleSheet(pattern_str_ScrollBar);
    ui->tableWidget_group->verticalScrollBar()->setStyleSheet(pattern_str_ScrollBar);



    //设置单元格大小
    ui->tableWidget_group->horizontalHeader()->setDefaultSectionSize(180);  //设置默认宽度
    ui->tableWidget_group->verticalHeader()->setDefaultSectionSize(40);   //设置一行默认高度
    ui->tableWidget_group->setColumnWidth((int)Table_TestReagentroup,350);
    ui->tableWidget_group->setColumnWidth((int)Table_CheckState,80);
    ui->tableWidget_group->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    CreatTableNumTab();
    return;
}

void ConfigureProjectItem::CreatTableNumTab()
{
    int TotalRows = 0;
    QMap<int, QString> AllSaveReagentTask;
    AllSaveReagentTask.clear();
    QTableWidgetItem * Reagentinfo_item;
    FullyAutomatedPlatelets::pinstancesqlData()->GetTestReagentGroup(AllSaveReagentTask);
    auto iter = AllSaveReagentTask.constBegin();
    while(iter != AllSaveReagentTask.constEnd())
    {
        TotalRows = ui->tableWidget_group->rowCount();
        ui->tableWidget_group->insertRow(TotalRows);

        Reagentinfo_item = new QTableWidgetItem(iter.value());
		Reagentinfo_item->setTextAlignment(Qt::AlignCenter);
        Reagentinfo_item->setFont(QFont("楷体", 14));
        ui->tableWidget_group->setItem(TotalRows,Table_TestReagentroup,Reagentinfo_item);

        AddCheckBoxInntoTable(TotalRows); //复选框

        iter++;
    }
}



void ConfigureProjectItem::RadioButtonAllNoChecked()
{
    for(auto radiobtn : m_ptestingradioList)
    {
        radiobtn->setChecked(false);
    }
}


void ConfigureProjectItem::AddCheckBoxInntoTable(int Rows)
{
    //++复选框
    QCheckBox * pCheckBox ;
    QWidget * pBoxWidget;
    QHBoxLayout * hBox ;

    pCheckBox = new QCheckBox();
    pBoxWidget = new QWidget(this);
    hBox = new QHBoxLayout();
    //调整框的大小,添加选中图片
    pCheckBox->setStyleSheet("QCheckBox::indicator{width:40px;height:40px}"
                             "QCheckBox::indicator:unchecked{image: url(:/Picture/SetPng/checkbox-blank.png);}"
                             "QCheckBox::indicator:checked {image: url(:/Picture/SetPng/checkbox-fill.png);}");
    hBox->addWidget(pCheckBox);

    //设置默认选中状态
    pCheckBox->setChecked(false);
    hBox->setMargin(0);
    hBox->setAlignment(pCheckBox,Qt::AlignCenter);
    pBoxWidget->setLayout(hBox);
    ui->tableWidget_group->setCellWidget(Rows,Table_CheckState,pBoxWidget);
    QObject::connect(pCheckBox,&QCheckBox::clicked,this,[=](){
       bool pchecked = pCheckBox->isChecked();
       ClickCheckBox(pchecked);
    });
    return;
}

void ConfigureProjectItem::ClickCheckBox(bool pchecked)
{
    QCheckBox *pIndexCheckBox = qobject_cast<QCheckBox *>(sender());
    if(pchecked == true)
    {
        RadioButtonAllNoChecked();
        int Rows = ui->tableWidget_group->rowCount();
        for(int n = 0; n < Rows; n++)
        {
           QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
           QCheckBox* pCheck = widget->findChild<QCheckBox *>();
           if(pCheck != pIndexCheckBox)
           {
               pCheck->blockSignals(true);
               pCheck->setChecked(false);
               pCheck->blockSignals(false);
           }
        }
    }
    update();
    return;
}

void ConfigureProjectItem::CancelReagentGroup()
{
    int Rows = ui->tableWidget_group->rowCount();
    for(int n = 0; n < Rows; n++)
    {
       QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
       QCheckBox* pCheck = widget->findChild<QCheckBox *>();
       if(pCheck->isChecked())
       {
            pCheck->blockSignals(true);
            pCheck->setChecked(false);
            pCheck->blockSignals(false);
       }
    }
    update();
    return;
}


void ConfigureProjectItem::AddGroupReagentTest()
{
    QStringList AddGroupReagent;
    QString SavedGroup = "";
    AddGroupReagent.clear();
    for(auto radiobtn : m_ptestingradioList)
    {
        bool bchecked = radiobtn->isChecked();
        if(bchecked)
            AddGroupReagent.push_back(radiobtn->text());
    }
    if(AddGroupReagent.size() == 0){
		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint); //取消置顶
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("组合试剂失败","请选择要编组的试剂!");
        return ;
    }
    ShowTestGroupText(AddGroupReagent, SavedGroup);
    QMap<int ,QString> HadGroup;
    FullyAutomatedPlatelets::pinstancesqlData()->GetTestReagentGroup(HadGroup);
    auto  it = HadGroup.begin();
    while(it != HadGroup.end())
    {
        if(it.value() == SavedGroup)
        {
			this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
            FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("组合试剂失败","组合已存在!");
            return;
        }
        it++;
    }

    //添加
    int Rows = ui->tableWidget_group->rowCount();
    ui->tableWidget_group->insertRow(Rows);

    QTableWidgetItem *Reagentinfo_item = new QTableWidgetItem(SavedGroup);
    Reagentinfo_item->setTextAlignment(Qt::AlignCenter);
    Reagentinfo_item->setFont(QFont("楷体", 14));
    ui->tableWidget_group->setItem(Rows,(int)Table_TestReagentroup,Reagentinfo_item);

    AddCheckBoxInntoTable(Rows); //复选框



    //保存到数据库中
    FullyAutomatedPlatelets::pinstancesqlData()->AddTestReagentGroup(ui->tableWidget_group->rowCount(),SavedGroup);

    RadioButtonAllNoChecked();
    return;

}

void ConfigureProjectItem::DeleteReagentGroup()
{
    int Rows = ui->tableWidget_group->rowCount();
    for(int n = 0; n < Rows; n++)
    {
       QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
	   QCheckBox* pCheck = widget->findChild<QCheckBox *>();
       if(pCheck->isChecked() == true)
       {
           QString reagentstr = ui->tableWidget_group->item(n,Table_TestReagentroup)->text();
           FullyAutomatedPlatelets::pinstancesqlData()->DeleteTestReagentGroup(reagentstr);
           ui->tableWidget_group->removeRow(n);
           m_checkBoxList.removeOne(pCheck);
		   break;
       }
    }

    Rows = ui->tableWidget_group->rowCount();
    for(int i = 0; i < Rows; i++)
    {
        int index = i + 1;
        QString reagentstr = ui->tableWidget_group->item(i,Table_TestReagentroup)->text();
        FullyAutomatedPlatelets::pinstancesqlData()->UpdateGroupReagent(index,reagentstr);
    }
    return;
}


bool ConfigureProjectItem::SelectRadittom()
{
    for(int i = 0 ; i < m_ptestingradioList.size() ; i++)
    {
       if(m_ptestingradioList.at(i)->isChecked() == true)
           return false;
    }
    return true ;
}

void ConfigureProjectItem::ShowTestGroupText(QStringList GroupReagent,QString &ShowText)
{
    for(int i = 0 ; i < GroupReagent.size() ; i++)
    {
        if( i == GroupReagent.size() - 1)
            ShowText = ShowText + GroupReagent.at(i);
        else
            ShowText = ShowText + GroupReagent.at(i) + "|";
    }
    return;
}

void ConfigureProjectItem::ConfigTestingReagnet()
{
    QString reagent_str ="";
    QStringList  ReagnetGroup;
    ReagnetGroup.clear();
    for(auto radiobtn : m_ptestingradioList)
    {
        if(radiobtn->isChecked() == true)
           ReagnetGroup.push_back(radiobtn->text());
    }

    QStringList ReagentGroupCheck;
    ReagentGroupCheck.clear();
    int Rows = ui->tableWidget_group->rowCount();
    for(int n = 0; n < Rows; n++)
    {
       QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
	   QCheckBox* pCheck = widget->findChild<QCheckBox *>();
       if(pCheck->isChecked() == true){
           QString reagentstr = ui->tableWidget_group->item(n,Table_TestReagentroup)->text();
           ReagentGroupCheck.push_back(reagentstr);
       }
    }

    if(ReagnetGroup.size() != 0 && ReagentGroupCheck.size() == 0)
    {
        ShowTestGroupText(ReagnetGroup, reagent_str);
    }
    else if(ReagnetGroup.size() == 0 && ReagentGroupCheck.size() == 0)
    {
		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(tr("保存试剂失败"),tr("请选择测试试剂种类!"));
    }
    else if(ReagnetGroup.size() != 0 && ReagentGroupCheck.size() != 0)
    {
		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(tr("保存试剂失败"),tr("请选择单项或试剂组测试种类!"));
    }
    else if(ReagnetGroup.size() == 0 && ReagentGroupCheck.size() != 0)
    {
        reagent_str = ReagentGroupCheck.first();
    }
    if(!reagent_str.isEmpty())
    {
        if(m_singleitem == false)
            emit _setallsampletestproject(reagent_str);
        else
            emit _setsinglesampleproject(m_rows,m_clos, reagent_str);
        this->close();
    }

    return;
}

void ConfigureProjectItem::Slot_ChangeSingleItem(int rows, int clos, bool signal)
{
    m_rows = rows;
    m_clos = clos;
    m_singleitem = signal;
}


void ConfigureProjectItem::mousePressEvent(QMouseEvent *event)
{
	this->windowsPos = this->pos();       // 获得部件当前位置
	this->mousePos = event->globalPos(); // 获得鼠标位置
	this->dPos = mousePos - windowsPos;   // 移动后部件所在的位置
}

void ConfigureProjectItem::mouseMoveEvent(QMouseEvent *event)
{
	this->move(event->globalPos() - this->dPos);
}

void ConfigureProjectItem::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	QColor colorBackGround = bgmcColor;
	p.setRenderHint(QPainter::Antialiasing);//抗锯齿
	p.setBrush(colorBackGround);
	p.setPen(Qt::NoPen);
	p.drawRoundedRect(0, 0, width() - 1, height() - 1, 2, 2);
}
void ConfigureProjectItem::closeEvent(QCloseEvent *event)
{
	event->accept();
	close();
}

void ConfigureProjectItem::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
	{
		this->close();
	}
}
