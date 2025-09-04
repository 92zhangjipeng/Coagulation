#pragma execution_character_set("utf-8")
#include "customplot.h"
#include "loginui.h"
#include "loginui.h"
#include "ui_customplot.h"
#include <QAbstractItemView>
#include <qDebug>
#include <QSpinBox>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QTime>
#include <QElapsedTimer>
#include <QThread>
#include <QStandardItemModel>
#include <QVector>
#include <algorithm>



bool compairtool(QToolButton *holenum, QToolButton *endhole)
{
    QString objectIndex_s = holenum->objectName();
    int index_s = QUIUtils::StringFindintnum(objectIndex_s);
    QString objectIndex_e = endhole->objectName();
    int index_e = QUIUtils::StringFindintnum(objectIndex_e);
    return (index_s < index_e);
}

bool sortWidgetNumLoc(QWidget *fWidget,QWidget *eWidget)
{
    QString objectIndex_s = fWidget->objectName();
    int index_s = QUIUtils::StringFindintnum(objectIndex_s);
    QString objectIndex_e = eWidget->objectName();
    int index_e = QUIUtils::StringFindintnum(objectIndex_e);
    return (index_s < index_e);
}


CustomPlot::CustomPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomPlot),
    m_downhigh(0),
    mcodeNum(0),
    m_CalibrationArea(-1),
    mbstopTestadjust(false)
 {
    ui->setupUi(this);
    this->setWindowTitle(tr("坐标位置微调"));
    mfont.setFamily("楷体");
    mfont.setPointSize(10);//设置文字大小
    ui->widget_displayled->setColors(QSimpleLed::CUSTOM);
    ui->widget_displayled->setFixedSize(32, 32);
    ui->widget_displayled->setCustomOnColor0(QColor(0, 127, 0));
    ui->widget_displayled->setCustomOnColor1(QColor(0, 255, 0));
    ui->widget_displayled->setCustomOffColor0(QColor(127, 0, 0));
    ui->widget_displayled->setCustomOffColor1(QColor(255, 0, 0));
    ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::OFF);
    mcustFont.setFamily("楷体");
    mcustFont.setPointSize(12);//设置文字大小

    ui->pushButton_TrayHands->hide();
}

CustomPlot::~CustomPlot()
{
    if(mpTestCaseRun != nullptr)
    {
        delete mpTestCaseRun;
        mpTestCaseRun = nullptr;
    }
    if(m_CheckGroupBox)
    {
        delete m_CheckGroupBox;
        m_CheckGroupBox = nullptr;
    }
    if(mbloodBtnGroupbox)
    {
        delete mbloodBtnGroupbox;
        mbloodBtnGroupbox = nullptr;
    }
    delete ui;
}

void CustomPlot::initstyle()
{
    quint8 equipmentType = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);
    initCommboxView(equipmentType);

    m_CheckGroupBox = new  QButtonGroup(this);
    m_CheckGroupBox->setExclusive(true); //互斥
    m_CheckGroupBox->addButton(ui->checkBox_downTop,TheGripperDrops::GipperDown_top);
    m_CheckGroupBox->addButton(ui->checkBox_downmind,TheGripperDrops::GipperDown_mid);
    m_CheckGroupBox->addButton(ui->checkBox_downbottom,TheGripperDrops::GipperDown_bottom);
    m_CheckGroupBox->addButton(ui->checkBox_downcustom,TheGripperDrops::GipperDown_custom);
    connect(m_CheckGroupBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(CheckGipperDownHigh(QAbstractButton*)));

    InitdisplayPointTablewidget(ui->tableWidget_displayPos);

    InitBloodZoneNum(equipmentType);  //初始化血样区

    initEmptyTubeHole(equipmentType); //空试管区

    initTestChnHole(equipmentType);  //测试通道

   //试剂区
   QList<QToolButton *> ReagentsArea =  ui->widget_ReagentsArea->findChildren<QToolButton* >();
   std::sort(ReagentsArea.begin(),ReagentsArea.end(),compairtool);
   for(QToolButton* Reagentstube : ReagentsArea)
   {
       Reagentstube->setStyleSheet(m_Cssbtn);
       QObject::connect(Reagentstube,&QToolButton::clicked,this,&CustomPlot::ClickReagentsTube);
   }
   ui->toolButton_throwTube->setStyleSheet(m_Cssbtn);
   ui->toolButton_origin->setStyleSheet(m_Cssbtn);
   ui->toolButton_Clean_1->setStyleSheet(m_Cssbtn);

   mbloodBtnGroupbox->addButton(ui->toolButton_throwTube); //弃杯孔
   mbloodBtnGroupbox->addButton(ui->toolButton_origin);    //原点
   mbloodBtnGroupbox->addButton(ui->toolButton_Clean_1);
   mbloodBtnGroupbox->setExclusive(true);// 设置是否互斥

   //点击原点校验位置
   connect(ui->toolButton_origin,&QToolButton::clicked,this,[=](){
       UserClickOriginAxis();
   });

}

void CustomPlot::initCommboxView(quint8 indexType)
{
	QMap<int, QString> Instrument_Adjust;
    Instrument_Adjust.insert((int)Origin_bloodNeedle, "清洗区原点");
    Instrument_Adjust.insert((int)CleanLinque_bloodNeedle, "[样本针]清洗液");
    Instrument_Adjust.insert((int)Bloodsample_BloodNeedle, "[样本针]样本区");
    Instrument_Adjust.insert((int)EmptyTube_1_BloodNeedle, "[样本针]试杯区1");
    Instrument_Adjust.insert((int)EmptyTube_2_BloodNeedle, "[样本针]试杯区2");
    Instrument_Adjust.insert((int)EmptyTube_3_BloodNeedle, "[样本针]试杯区3");
    Instrument_Adjust.insert((int)EmptyTube_4_BloodNeedle, "[样本针]试杯区4");
    Instrument_Adjust.insert((int)CleanLinque_ReagentNeedle, "[试剂针]清洗液");
    Instrument_Adjust.insert((int)ReagentLinque_ReagentNeedle, "[试剂针]试剂区");
    Instrument_Adjust.insert((int)TestChannel_ReagentNeedle, "[试剂针]测试区");
    Instrument_Adjust.insert((int)TestChannel_Hands, "[抓手]测试区");
    Instrument_Adjust.insert((int)ThrowCup_Hands, "[抓手]弃杯孔");
    Instrument_Adjust.insert((int)EmptyTube_1_Hands, "[抓手]试杯区1");
    Instrument_Adjust.insert((int)EmptyTube_2_Hands, "[抓手]试杯区2");
    Instrument_Adjust.insert((int)EmptyTube_3_Hands, "[抓手]试杯区3");
    Instrument_Adjust.insert((int)EmptyTube_4_Hands, "[抓手]试杯区4");

    ui->comboBox_calibrationAarea->blockSignals(true);
    auto it = Instrument_Adjust.begin();
    while(it != Instrument_Adjust.end())
    {
        int keyIndex = it.key();
        ui->comboBox_calibrationAarea->insertItem(keyIndex,it.value());
        it++;
    }
    QListView* view = qobject_cast<QListView*>(ui->comboBox_calibrationAarea->view());
    switch(indexType)
    {
        case KS600:
            view->setRowHidden(EmptyTube_3_BloodNeedle, true);//隐藏为TRUE，显示为false
            view->setRowHidden(EmptyTube_4_BloodNeedle, true);
            view->setRowHidden(EmptyTube_3_Hands, true);
            view->setRowHidden(EmptyTube_4_Hands, true);
        break;
        case KS800:
            view->setRowHidden(EmptyTube_4_BloodNeedle, true);
            view->setRowHidden(EmptyTube_4_Hands, true);
        break;
        default:    break;
    }
    QStandardItemModel *pItemModel = qobject_cast<QStandardItemModel*>(ui->comboBox_calibrationAarea->model());
    for(int i = Origin_bloodNeedle ; i < CleanLinque_ReagentNeedle ; i++)
    {
        pItemModel->item(i)->setBackground(QColor(190, 190, 190));//修改某项背景颜色
    }
    for(int i = TestChannel_Hands ; i <= EmptyTube_4_Hands; i++)
    {
        pItemModel->item(i)->setBackground(QColor(211, 211, 211));
    }
    MyQStyledItemDelegate *pMyQStyledItemDelegate = new MyQStyledItemDelegate(30, this);  //构造的时候传入高度
    ui->comboBox_calibrationAarea->setItemDelegate(pMyQStyledItemDelegate);    //设置代理
    ui->comboBox_calibrationAarea->setCurrentIndex(-1);
    ui->comboBox_calibrationAarea->blockSignals(false);
    return;
}

void CustomPlot::InitBloodZoneNum(quint8 indexModels)
{
    int showNum = 0;
    mbloodBtnGroupbox = new QButtonGroup;
    switch(indexModels)
    {
        case KS600: showNum = 7;
        break;
        case KS800: showNum = 9;
        break;
        case KS1200: showNum = 12;
        break;
		default: showNum = 12; break;
    }
    QVector<QList<QToolButton*> > BloodAreasTube;
    BloodAreasTube.clear();
    //QList<QToolButton*> holeLineList;

   // QList<QWidget *> singleWidget;
	
	for (int i = 0; i < 12; i++)
	{
		QString findstr = QString("widget_Bloodchild_%1").arg(i +1);
		QWidget *pwidgt = ui->widget_BloodArea->findChild<QWidget *>(findstr);
        if(i >= showNum)
        {
            pwidgt->hide();
            continue;
        }
		QList<QToolButton *> list = pwidgt->findChildren<QToolButton *>();
		std::sort(list.begin(), list.end(), compairtool);
		int k = 1;
		for (auto it : list)
		{
			it->setText(QString("%1").arg(k+ i*10));
            it->setFont(mfont);
			it->setCheckable(true);
			it->setAutoExclusive(true);
			it->setStyleSheet(m_Cssbtn);
			mbloodBtnGroupbox->addButton(it);
			QObject::connect(it, &QToolButton::clicked, this, &CustomPlot::ClickBloodTube);
			k++;
		}
	}
    update();
	return;   
}

void CustomPlot::initEmptyTubeHole(quint8 indexType)
{
	int showNum = 0;
	switch (indexType)
	{
	case KS600: showNum = 2;
		break;
	case KS800: showNum = 3;
		break;
	case KS1200: showNum = 4;
		break;
	default: showNum = 4; break;
	}
	for (int i = 0; i < 4; i++)
	{
		QString findkey = QString("widget_testtubetray_%1").arg(i + 1);
		QWidget *pwidgt = ui->widget_trayall->findChild<QWidget *>(findkey);
		if (i >= showNum)
		{
			pwidgt->hide();
			continue;
		}
		QList<QToolButton *> list = pwidgt->findChildren<QToolButton *>();
		std::sort(list.begin(), list.end(), compairtool);
		int k = 1;
		for (auto iterbtn : list)
		{
			iterbtn->setCheckable(true);
			iterbtn->setAutoExclusive(true);
			iterbtn->setText(QString("%1").arg(k + 60* i));
            iterbtn->setFont(mfont);
			iterbtn->setStyleSheet(m_Cssbtn);
			mbloodBtnGroupbox->addButton(iterbtn);
			QObject::connect(iterbtn, &QToolButton::clicked, this, &CustomPlot::ClickEmptyTube);
			k++;
		}
	}
	return;
}

void CustomPlot::initTestChnHole(quint8 indexType)
{
    QList<QToolButton* > testChannelArea = ui->widget_testchannelArea->findChildren<QToolButton *>();
    std::sort(testChannelArea.begin(),testChannelArea.end(),compairtool);
    for(auto it_Chn : testChannelArea)
    {
        int index = QUIUtils::StringFindintnum(it_Chn->objectName());
        switch (indexType) {
        case KS600:
                    if(index > 4)
                    {
                        it_Chn->hide();
                    }
                    else
                    {
                        it_Chn->setStyleSheet(m_Cssbtn);
                        QObject::connect(it_Chn,&QToolButton::clicked,this,&CustomPlot::ClickTestChannelTube);
                    }
        break;
        case KS800:
                    if(index > 8)
                    {
                        it_Chn->hide();
                    }
                    else
                    {
                        it_Chn->setStyleSheet(m_Cssbtn);
                        QObject::connect(it_Chn,&QToolButton::clicked,this,&CustomPlot::ClickTestChannelTube);
                    }
        break;
        case KS1200:
                    it_Chn->setStyleSheet(m_Cssbtn);
                    QObject::connect(it_Chn,&QToolButton::clicked,this,&CustomPlot::ClickTestChannelTube);
        break;
        default:
                    it_Chn->setStyleSheet(m_Cssbtn);
                    QObject::connect(it_Chn,&QToolButton::clicked,this,&CustomPlot::ClickTestChannelTube);
        break;
        }

    }
    return;
}


void CustomPlot::InitdisplayPointTablewidget(QTableWidget* Tablewidgetpos)
{
    QStringList header;
    header <<tr("编号")<< tr("X坐标")<<tr("Y坐标")<<tr("保存");
    Tablewidgetpos->setColumnCount(header.size()); //初始化列
    Tablewidgetpos->setHorizontalHeaderLabels(header); //设置表头

    QFont font;
    font.setFamily("新宋体");
    font.setBold(true);
    font.setPixelSize(20);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 1);// 设置字符间距
    Tablewidgetpos->horizontalHeader()->setFont(font);
    Tablewidgetpos->horizontalHeader()->setHighlightSections(false);
    Tablewidgetpos->setFocusPolicy(Qt::NoFocus);

    Tablewidgetpos->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    Tablewidgetpos->verticalHeader()->setDefaultSectionSize(30); //设置行高
    Tablewidgetpos->horizontalHeader()->setDefaultSectionSize(30);
    Tablewidgetpos->setFrameShape(QFrame::Box); //设置边框
    Tablewidgetpos->setShowGrid(true); //设置不显示格子线
    Tablewidgetpos->verticalHeader()->setVisible(false); //设置垂直头不可见
    Tablewidgetpos->setEditTriggers(QAbstractItemView::CurrentChanged); //设置不可编辑
    Tablewidgetpos->setSelectionMode(QAbstractItemView::NoSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    Tablewidgetpos->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行

    Tablewidgetpos->horizontalHeader()->setFixedHeight(30); //设置表头的高度
    Tablewidgetpos->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    Tablewidgetpos->verticalHeader()->setResizeContentsPrecision(QHeaderView::Stretch);
    Tablewidgetpos->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
    Tablewidgetpos->setColumnWidth(TableIndexPos::IndexNum, 20);
    Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_xpos, 120);
    Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_ypos, 120);
    Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_save, 90);

    Tablewidgetpos->setStyleSheet(
        "QTableView::item:selected{color:white;background:rgb(34, 175, 75);}"
        "QTableView{border:none;background:white;  alternate-background-color: rgb(141, 163, 215); }"
        "QHeaderView {color: white;font: bold 10pt;background-color: rgb(108, 108, 108); border: 0px solid rgb(144, 144, 144); \
         border:0px solid rgb(191,191,191);border-left-color: rgba(255, 255, 255, 0);"
        "border-top-color: rgba(255, 255, 255, 0); border-radius:0px;min-height:29px; }"
        "QHeaderView::section {color: white; background-color: rgb(64, 64, 64);border: 5px solid #f6f7fa;\
         border-radius:0px;border-color: rgb(64, 64, 64);}");
         //设置水平、垂直滚动条样式
    Tablewidgetpos->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
         "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
         "QScrollBar::handle:hover{background:gray;}"
         "QScrollBar::sub-line{background:transparent;}"
         "QScrollBar::add-line{background:transparent;}");
    Tablewidgetpos->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 10px;}"
         "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
         "QScrollBar::handle:hover{background:gray;}"
         "QScrollBar::sub-line{background:transparent;}"
         "QScrollBar::add-line{background:transparent;}");
    return;
}


//下降高度
void CustomPlot::CheckGipperDownHigh(QAbstractButton *btn)
{
    // 当前点击的按钮
    int ClickedID = m_CheckGroupBox->id(btn);
    m_indexDown = ClickedID;
    switch(ClickedID)
    {
        case TheGripperDrops::GipperDown_top:   m_downhigh = 10;    break;
        case TheGripperDrops::GipperDown_mid:   m_downhigh = 30;    break;
        case TheGripperDrops::GipperDown_bottom: m_downhigh = 50;   break;
        case TheGripperDrops::GipperDown_custom: m_downhigh = ui->spinBox_downValue->value(); break;
        default:    m_downhigh = ui->spinBox_downValue->value();    break;
    }
}


//弃杯孔
void CustomPlot::displayThrowHole(bool bFindShow,bool bWrite_x,int notifyValue)
{
    auto* axis = SingletonAxis::GetInstance();
    if(bFindShow){
        // 显示弃杯孔位置
        QPoint throwHolePoint;
        axis->throwTubeHolePos(READ_OPERRAT, throwHolePoint);
        QMap<quint8, QPoint> points = {{0, throwHolePoint}};
        NotifyShowInstrumentPoint(points, "弃杯孔(抓手)微调");
    }
    else
    {
        axis->oper_ThrowTubeHolePos(bWrite_x,notifyValue);
    }
}



//查找原点坐标
void CustomPlot::displayOriginAxisPoint(bool bFindShow,bool bWrite_x,int notifyValue)
{
    auto* pconfAxis = SingletonAxis::GetInstance();
    if(bFindShow){

        QPoint originPoint;
		pconfAxis->originPos(READ_OPERRAT, originPoint);
        QMap<quint8, QPoint> points = {{0, originPoint}};
        NotifyShowInstrumentPoint(points, "微调原点坐标");
    }
    else
    {
        pconfAxis->oper_OriginAxis(bWrite_x,notifyValue);
    }
}

//清洗液区
void CustomPlot::displayCleanLinqueAxisPoint(bool bFindShow, quint8  indexZ,bool bWrite_x, int notifyValue)
{
    auto* pconfAxis = SingletonAxis::GetInstance();
    if(bFindShow)
    {
        QPoint cleanPoint;
		pconfAxis->cleanZoneAxisPos(READ_OPERRAT, indexZ, cleanPoint);

        QMap<quint8, QPoint> points = {{0, cleanPoint}};
        switch (indexZ) {
            case MOTOR_BLOOD_INDEX:
                NotifyShowInstrumentPoint(points, "清洗液(样本针)微调");
                break;
            case MOTOR_REAGNET_INDEX:
                NotifyShowInstrumentPoint(points, "清洗液(试剂针)微调");
                break;
        }
    }
    else{
        pconfAxis->oper_CleanZonePos(bWrite_x,indexZ,notifyValue);
    }
    return;
}

//试剂区
int CustomPlot::displayReagentAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QPoint backPoint(0,0);
    int offset_v = 0;
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();
        for(int i = 0 ;i < 10; i++)
        {
            SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,i,backPoint);
            readAxis.insert(i,backPoint);
        }
        InsertOneChangeOthersChange(readAxis,"试剂区(试剂针)微调");
    }
    else
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,0,backPoint);
        if(bWrite_x == NOTIFY_XPOINT)
            offset_v =  notifyValue - backPoint.x();
        else
            offset_v =  notifyValue - backPoint.y();
        for(int i = 0;  i < 10 ; i++)
        {
            SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,i,backPoint);
            if(bWrite_x == NOTIFY_XPOINT)
                SingletonAxis::GetInstance()->oper_ReagentZonePos(bWrite_x,i,backPoint.x() + offset_v);
            else
                SingletonAxis::GetInstance()->oper_ReagentZonePos(bWrite_x,i,backPoint.y() + offset_v);
        }
    }
    return offset_v;
}

//测试通道
void CustomPlot::displayTestChnAxisPoint(bool bFindShow, bool bWrite_x,quint8 indexZ, quint8 ChnNum,int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    quint8 Typtind = 0;
	SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,Typtind);
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();
        quint8 totalChn = 0;
        QPoint backPoint(0,0);
        switch (Typtind)
        {
            case KS600: totalChn = 4;  break;
            case KS800: totalChn = 8;   break;
            case KS1200: totalChn = 12; break;
            default: totalChn = 12;     break;
        }
        for(int i = 0 ;i < totalChn; i++)
        {
            SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,i,indexZ,backPoint);
            readAxis.insert(i,backPoint);
        }
        if(indexZ == MOTOR_REAGNET_INDEX)
            NotifyShowInstrumentPoint(readAxis,"测试区(试剂针)微调");
        else if(indexZ == MOTOR_HANDS_INDEX)
            NotifyShowInstrumentPoint(readAxis,"测试区(抓手)微调");
    }
    else
    {
        SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(bWrite_x,ChnNum,indexZ,notifyValue);
    }
    return;
}

//测试杯区
int CustomPlot::displayEmptyTrayAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ,quint8 indexTray,quint8 tubeNum, int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    quint8 totalHole_s = tubeNum;
    int offset_v = 0;
    quint8 totalHole_e = 0;
    QPoint backPoint(0,0);
    switch (indexTray)
    {
        case 1:
            totalHole_s = 0;
            totalHole_e = 60;
        break;
        case 2:
            totalHole_s = 60;
            totalHole_e = 120;
        break;
        case 3:
            totalHole_s = 120;
            totalHole_e = 180;
        break;
        case 4:
            totalHole_s = 180;
            totalHole_e = 240;
        break;
        default:
           break;
    }
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();
        for(int i = totalHole_s; i < totalHole_e; i++)
        {
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i,indexZ,backPoint);
            readAxis.insert(i,backPoint);
        }
        if(indexZ == MOTOR_BLOOD_INDEX)
            InsertOneChangeOthersChange(readAxis, QString("[样本针]试杯区(%1)偏移微调").arg(indexTray));
        else if(MOTOR_HANDS_INDEX == indexZ)
            InsertOneChangeOthersChange(readAxis, QString("[抓手]试杯区(%1)偏移微调").arg(indexTray));
    }
    else
    {
         SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,totalHole_s,indexZ,backPoint);
         if(bWrite_x == NOTIFY_XPOINT)
            offset_v = notifyValue - backPoint.x();
         else
            offset_v = notifyValue - backPoint.y();
         for(int k = totalHole_s; k < totalHole_e; k++)
         {
             SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,k,indexZ,backPoint);
             if(bWrite_x == NOTIFY_XPOINT)
                 SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_XPOINT,k,indexZ ,backPoint.x() + offset_v);
             else
                SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_YPOINT, k ,indexZ,backPoint.y() + offset_v);
         }
    }
    return offset_v;
}

int CustomPlot::displayBloodHoleAxisPoint(bool bFindShow, bool bWrite_x,quint8 indexhole, int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    quint8 Typtind = 0;
    int offset_v = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,Typtind);
    quint8 totalHole = 0;
    QPoint backPoint(0,0);
    switch (Typtind)
    {
        case KS600:  totalHole = 70; break;
        case KS800:  totalHole = 90; break;
        case KS1200: totalHole = 120; break;
        default: totalHole = 120;     break;
    }
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();

        for(int i = 0; i < totalHole; i++)
        {
            SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,i,backPoint);
            readAxis.insert(i,backPoint);
        }
        InsertOneChangeOthersChange(readAxis,tr("[样本针]样本区偏移微调"));
    }
    else
    {
        SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,indexhole,backPoint);
        if(bWrite_x == NOTIFY_XPOINT)
            offset_v = notifyValue - backPoint.x();
        else
            offset_v = notifyValue - backPoint.y();
        for(int i = 0; i < totalHole; i++)
        {
            SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,i,backPoint);
            if(bWrite_x == NOTIFY_XPOINT)
                SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_XPOINT,i,backPoint.x() + offset_v);
            else
               SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_YPOINT,i,backPoint.y() +  offset_v);
        }
    }
    return offset_v;
}

//区域
void CustomPlot::on_comboBox_calibrationAarea_activated(int index)
{
    if(index == -1 )     return;
    if(m_indexDown < 0)
    {
        QMessageBox msgBox;
        msgBox.setIconPixmap(QPixmap(":/Picture/SetPng/remindererroe.png"));
        msgBox.setText("选取失败,请选择下针高度");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Ok:     return;  break;
            case QMessageBox::Cancel: return;  break;
            default:
            break;
        }
    }
    //先清空表内容
    DelTableText();
    m_CalibrationArea = index;
    switch(index)
    {
        case Origin_bloodNeedle:
        {
            displayOriginAxisPoint(true,false,0);
            break;
        }
        case CleanLinque_bloodNeedle:
        {
            displayCleanLinqueAxisPoint(true,MOTOR_BLOOD_INDEX,false,0);
            break;
        }
        case CleanLinque_ReagentNeedle:
        {
            displayCleanLinqueAxisPoint(true,MOTOR_REAGNET_INDEX,false,0);
            break;
        }
        case ReagentLinque_ReagentNeedle:
        {
            displayReagentAxisPoint(true, false, 0);
            break;
        }
        case TestChannel_ReagentNeedle:
        {
            displayTestChnAxisPoint(true,false,MOTOR_REAGNET_INDEX,0,0);
            break;
        }
        case TestChannel_Hands:
        {
            displayTestChnAxisPoint(true,false,MOTOR_HANDS_INDEX,0,0);
            break;
        }
        case ThrowCup_Hands:
        {
            displayThrowHole(true,false,0);
            break;
        }
        case EmptyTube_1_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,1,0, 0);

            break;
        }

        case EmptyTube_1_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,1,0, 0);
            break;
        }

        case EmptyTube_2_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,2,0, 0);
            break;
        }

        case EmptyTube_2_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,2,0, 0);
            break;
        }

        case EmptyTube_3_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,3,0, 0);
            break;
        }

        case EmptyTube_3_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,3,0, 0);
            break;
        }
        case EmptyTube_4_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,4,0, 0);
            break;
        }
        case EmptyTube_4_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,4,0, 0);
            break;
        }
        case Bloodsample_BloodNeedle:
        {
            displayBloodHoleAxisPoint(true,false,0,0);
            break;
        }
        default:
        break;
    }
    return;
}


void CustomPlot::AddBtnSave_backValue(int rowIndex,bool NeedChaneOther)
{
    QPushButton *btn_save = new QPushButton("保存微调");
    btn_save->setFont(mcustFont);
    btn_save->setFixedSize(90,30);
    //编辑按钮样式
    if(NeedChaneOther == false)
    {
        connect(btn_save,&QPushButton::clicked,this,&CustomPlot::BtnClickSavePoint);//保存单个修改
    }
    else
    {
        connect(btn_save,&QPushButton::clicked,this,&CustomPlot::BtnClickOthersSavePoint);
    }
    //表格中添加Widget
    ui->tableWidget_displayPos->setCellWidget(rowIndex,TableIndexPos::Instrument_save,btn_save);
    return;
}




void CustomPlot::NotifyShowInstrumentPoint(QMap<quint8,QPoint>& displayPoint ,const QString& tableName)
{
    ui->label_title->setText(tableName);

    auto createSpinBox = [this](int value, const QString& style) -> QSpinBox* {
        auto* spinBox = new QSpinBox(this);
        spinBox->setStyleSheet(style);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMaximum(5500);
        spinBox->setFixedHeight(30);
        spinBox->setValue(value);
        return spinBox;
    };

   for(auto iter = displayPoint.constBegin(); iter != displayPoint.constEnd(); ++iter) {
       const int row = ui->tableWidget_displayPos->rowCount();
       ui->tableWidget_displayPos->insertRow(row);

       QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
       item->setFlags(item->flags() & ~Qt::ItemIsEditable);
       item->setTextAlignment(Qt::AlignCenter);
       item->setBackground(QColor(204, 204, 204));
       ui->tableWidget_displayPos->setItem(row, TableIndexPos::IndexNum, item);

       ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_xpos,
           createSpinBox(iter.value().x(), m_SpinboxsheetX));

       ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_ypos,
           createSpinBox(iter.value().y(), m_SpinboxsheetY));

       AddBtnSave_backValue(row, false);
   }


//    auto iter = displayPoint.begin();
//    while(iter != displayPoint.end())
//    {
//        int row = ui->tableWidget_displayPos->rowCount();
//        ui->tableWidget_displayPos->insertRow(row);

//        QTableWidgetItem *item = new QTableWidgetItem(QString("%1").arg(row + 1));
//        item->setFlags(item->flags() & (~Qt::ItemIsEditable));//不可编辑
//        item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        ui->tableWidget_displayPos->setItem(row,TableIndexPos::IndexNum,item);
//        ui->tableWidget_displayPos->item(row,TableIndexPos::IndexNum)->setBackground(QBrush(QColor(204,204,204)));//改变单元格颜色
//        //设置X
//        QSpinBox *DisplayXvalue = new QSpinBox(this);
//        DisplayXvalue->setStyleSheet(m_SpinboxsheetX);
//        DisplayXvalue->setAlignment(Qt::AlignCenter);
//        DisplayXvalue->setMaximum(5500);
//        DisplayXvalue->setFixedHeight(30);
//        DisplayXvalue->setValue(iter.value().x());
//        ui->tableWidget_displayPos->setCellWidget(row,TableIndexPos::Instrument_xpos,DisplayXvalue);//添加控件到tableWidget上
//        //设置Y
//        QSpinBox *DisplayYvalue = new QSpinBox(this);
//        DisplayYvalue->setMaximum(5500);
//        DisplayYvalue->setFixedHeight(30);
//        DisplayYvalue->setAlignment(Qt::AlignCenter);
//        DisplayYvalue->setStyleSheet(m_SpinboxsheetY);
//        DisplayYvalue->setValue(iter.value().y());
//        ui->tableWidget_displayPos->setCellWidget(row,TableIndexPos::Instrument_ypos, DisplayYvalue);
//        //保存按钮
//        AddBtnSave_backValue(row,false);
//        iter++;
//    }
}




void CustomPlot::InsertOneChangeOthersChange(QMap<quint8,QPoint> displayPoint, const QString Tablename)
{
    ui->label_title->setText(Tablename);
    int InsertRow = 0;
    auto iter = displayPoint.begin();
    while(iter != displayPoint.end())
    {
        ui->tableWidget_displayPos->insertRow(InsertRow);
        QTableWidgetItem *itemIndex = new QTableWidgetItem();
        itemIndex->setText(QString("%1").arg(InsertRow + 1));
        itemIndex->setFlags(itemIndex->flags() & (~Qt::ItemIsEditable));//不可编辑
        itemIndex->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::IndexNum, itemIndex);
        ui->tableWidget_displayPos->item(InsertRow,TableIndexPos::IndexNum)->setBackground(QBrush(QColor(204,204,204)));//改变单元格颜色
        if(InsertRow == 0)
        {
            //设置X
            QSpinBox *DisplayXvalue = new QSpinBox(this);
            DisplayXvalue->setStyleSheet(m_SpinboxsheetX);
            DisplayXvalue->setAlignment(Qt::AlignCenter);
            DisplayXvalue->setMaximum(5000);
            DisplayXvalue->setFixedHeight(30);
            DisplayXvalue->setValue(iter.value().x());
            ui->tableWidget_displayPos->setCellWidget(InsertRow,TableIndexPos::Instrument_xpos,DisplayXvalue);//添加控件到tableWidget上
            connect(DisplayXvalue, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int value)
            {
                DisplayXvalue->setEnabled(false);
                UpdateBaseValue_x(value, DisplayXvalue);
                DisplayXvalue->setEnabled(true);
            });
            //设置Y
            QSpinBox *DisplayYvalue = new QSpinBox(this);
            DisplayYvalue->setMaximum(5000);
            DisplayYvalue->setFixedHeight(30);
            DisplayYvalue->setAlignment(Qt::AlignCenter);
            DisplayYvalue->setStyleSheet(m_SpinboxsheetY);
            DisplayYvalue->setValue(iter.value().y());
            ui->tableWidget_displayPos->setCellWidget(InsertRow,TableIndexPos::Instrument_ypos, DisplayYvalue);
            connect(DisplayYvalue, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int value)
            {
                DisplayYvalue->setEnabled(false);
                UpdateBaseValue_y(value);
                DisplayYvalue->setEnabled(true);
            });
            //保存按钮
            AddBtnSave_backValue(InsertRow,true);
        }
        else
        {
            QTableWidgetItem *item_x = new QTableWidgetItem();
            item_x->setFont(mcustFont);
            item_x->setText(QString("%1").arg(iter.value().x()));
            item_x->setFlags(item_x->flags() & (~Qt::ItemIsEditable));//不可编辑
            item_x->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::Instrument_xpos,item_x);

            QTableWidgetItem *item_y = new QTableWidgetItem(QString("%1").arg(iter.value().y()));
            item_y->setFont(mcustFont);
            item_y->setFlags(item_y->flags() & (~Qt::ItemIsEditable));//不可编辑
            item_y->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::Instrument_ypos,item_y);

            QTableWidgetItem *item_btn = new QTableWidgetItem();
            item_btn->setText("");
            item_btn->setFlags(item_btn->flags() & (~Qt::ItemIsEditable));//不可编辑
            item_btn->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::Instrument_save,item_btn);
            ui->tableWidget_displayPos->item(InsertRow,TableIndexPos::Instrument_save)->setBackground(QBrush(QColor(205, 201, 201)));
        }
        iter++;
        InsertRow++;
    }
    update();
}


//保存修改的单个坐标
void CustomPlot::BtnClickSavePoint()
{
    QByteArrayList sendcommd;
    QByteArray modifyParaarry_x;
    QByteArray modifyParaarry_y;
    QPushButton *btn = (QPushButton *)sender();
    int x = btn->mapToParent(QPoint(0,0)).x();
    int y = btn->mapToParent(QPoint(0,0)).y();
    QModelIndex index = ui->tableWidget_displayPos->indexAt(QPoint(x,y));
    int row = index.row(); //行

    QSpinBox * Point_x = (QSpinBox *)ui->tableWidget_displayPos->cellWidget(row,TableIndexPos::Instrument_xpos);
    int config_xpos = Point_x->value();

    QSpinBox * Point_y = (QSpinBox *)ui->tableWidget_displayPos->cellWidget(row,TableIndexPos::Instrument_ypos);
    int config_ypos = Point_y->value();

    switch(m_CalibrationArea)
    {

        case Origin_bloodNeedle:
                displayOriginAxisPoint(false,NOTIFY_XPOINT,config_xpos);
                displayOriginAxisPoint(false,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                QLOG_DEBUG()<<QString("修改原点坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case CleanLinque_bloodNeedle:
                displayCleanLinqueAxisPoint(false,MOTOR_BLOOD_INDEX,NOTIFY_XPOINT,config_xpos);
                displayCleanLinqueAxisPoint(false,MOTOR_BLOOD_INDEX,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                QLOG_DEBUG()<<QString("修改清洗液相对血样针坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case CleanLinque_ReagentNeedle:
                displayCleanLinqueAxisPoint(false,MOTOR_REAGNET_INDEX,NOTIFY_XPOINT,config_xpos);
                displayCleanLinqueAxisPoint(false,MOTOR_REAGNET_INDEX,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                QLOG_DEBUG()<<QString("修改清洗液相对试剂针坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case TestChannel_ReagentNeedle:
                displayTestChnAxisPoint(false,NOTIFY_XPOINT,MOTOR_REAGNET_INDEX,row,config_xpos);
                displayTestChnAxisPoint(false,NOTIFY_YPOINT,MOTOR_REAGNET_INDEX,row,config_ypos);
                writeboard(MOTOR_REAGNET_INDEX,row,sendcommd);
                QLOG_DEBUG()<<QString("修改测试通道%1offset试剂针坐标:[%2,%3]").arg(row+1).arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case TestChannel_Hands:
                displayTestChnAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,row,config_xpos);
                displayTestChnAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,row,config_ypos);
                writeboard(MOTOR_HANDS_INDEX,row,sendcommd);
                QLOG_DEBUG()<<QString("修改测试通道%1offset抓手坐标:[%2,%3]").arg(row+1).arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case ThrowCup_Hands:
                displayThrowHole(false,NOTIFY_XPOINT,config_xpos);
                displayThrowHole(false,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QLOG_DEBUG()<<QString("修改弃杯孔坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        default:
            break;
    }
    emit writdAxisata(sendcommd,"缓存坐标指令");
    //btn->setText("已保存");
    return;
}

void  CustomPlot::UpdateBaseValue_x(int NotifyValue_x,QSpinBox *pbox)
{
    pbox->setEnabled(false);
    int offsetValue = 0;
    switch(m_CalibrationArea)
    {
        case ReagentLinque_ReagentNeedle:   offsetValue = displayReagentAxisPoint(false,NOTIFY_XPOINT,NotifyValue_x);                             break;
        case EmptyTube_1_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,1,0,NotifyValue_x);     break;
        case EmptyTube_1_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,1,0,NotifyValue_x);     break;
        case EmptyTube_2_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,2,60,NotifyValue_x);    break;
        case EmptyTube_2_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,2,60,NotifyValue_x);    break;
        case EmptyTube_3_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,3,120,NotifyValue_x);   break;
        case EmptyTube_3_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,3,120,NotifyValue_x);   break;
        case EmptyTube_4_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,4,180,NotifyValue_x);   break;
        case EmptyTube_4_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,4,180,NotifyValue_x);   break;
        case Bloodsample_BloodNeedle:       offsetValue = displayBloodHoleAxisPoint(false,NOTIFY_XPOINT,0,NotifyValue_x);                         break;
        default:
            break;
    }
    updateNotifyOffsetValue(NOTIFY_XPOINT,offsetValue);
    pbox->setEnabled(true);

}
void  CustomPlot::UpdateBaseValue_y(int NotifyValue_y)
{
    int offsetValue = 0;
    switch(m_CalibrationArea)
    {
        case ReagentLinque_ReagentNeedle:  offsetValue = displayReagentAxisPoint(false,NOTIFY_YPOINT,NotifyValue_y);                             break;
        case EmptyTube_1_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,1,0,NotifyValue_y);     break;
        case EmptyTube_1_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,1,0,NotifyValue_y);     break;
        case EmptyTube_2_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,2,60,NotifyValue_y);    break;
        case EmptyTube_2_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,2,60,NotifyValue_y);    break;
        case EmptyTube_3_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,3,120,NotifyValue_y);   break;
        case EmptyTube_3_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,3,120,NotifyValue_y);   break;
        case EmptyTube_4_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,4,180,NotifyValue_y);   break;
        case EmptyTube_4_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,4,180,NotifyValue_y);   break;
        case Bloodsample_BloodNeedle:      offsetValue = displayBloodHoleAxisPoint(false,NOTIFY_YPOINT,0,NotifyValue_y);                         break;
        default:
            break;
    }
    updateNotifyOffsetValue(NOTIFY_YPOINT,offsetValue);
}

void CustomPlot::updateNotifyOffsetValue(bool bupdate_x, const int offsetValue)
{
    int rowscount = ui->tableWidget_displayPos->rowCount();
    for(int i = 0; i < rowscount; i++)
    {
        if(bupdate_x == NOTIFY_XPOINT)
        {
            if(ui->tableWidget_displayPos->item(i,TableIndexPos::Instrument_xpos) == nullptr || ui->tableWidget_displayPos->item(i,TableIndexPos::Instrument_xpos)->text().isEmpty())
            {
                continue;
            }
            int tablevalue = ui->tableWidget_displayPos->item(i,TableIndexPos::Instrument_xpos)->text().toInt();
            ui->tableWidget_displayPos->item(i, TableIndexPos::Instrument_xpos)->setText(QString("%1").arg(tablevalue + offsetValue));
        }
        else
        {
            if(ui->tableWidget_displayPos->item(i,TableIndexPos::Instrument_ypos) == nullptr || ui->tableWidget_displayPos->item(i,TableIndexPos::Instrument_ypos)->text().isEmpty())
            {
                continue;
            }
            int tablevalue = ui->tableWidget_displayPos->item(i,TableIndexPos::Instrument_ypos)->text().toInt();
            ui->tableWidget_displayPos->item(i, TableIndexPos::Instrument_ypos)->setText(QString("%1").arg(tablevalue + offsetValue));
        }
    }
    update();
    return;
}

void CustomPlot::writeboard(quint8 intdexZ,quint8 row, QByteArrayList &sendcommd)
{
    QByteArray x_arry;
    QByteArray y_arry;
    sendcommd.clear();
    if(intdexZ == MOTOR_REAGNET_INDEX)
    {
        if(row >= 0 && row < 5)
        {
             QUIUtils::get89NumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else if(row >= 5 && row < 10)
        {
            QUIUtils::get0a0bNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else
        {
            QUIUtils::get0c0dNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
    }
    else if(intdexZ == MOTOR_HANDS_INDEX)
    {
        if(row >= 0 && row < 5)
        {
             QUIUtils::get0c0dNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else if(row >= 5 && row < 10)
        {
             QUIUtils::get0e0fNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else
        {
           QUIUtils::get1011NumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
    }
    sendcommd.push_back(x_arry);
    sendcommd.push_back(y_arry);
}




//保存一个整体已偏移此刻发送命令就行
void CustomPlot::BtnClickOthersSavePoint()
{
    QByteArrayList sendcommd;
    QByteArray modifyParaarry_x;
    QByteArray modifyParaarry_y;
    switch(m_CalibrationArea)
    {
        case ReagentLinque_ReagentNeedle:
                    sendcommd.clear();
                    QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                    sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        case EmptyTube_1_Hands:
        case EmptyTube_1_BloodNeedle:
        case EmptyTube_2_Hands:
        case EmptyTube_2_BloodNeedle:
        case EmptyTube_3_Hands:
        case EmptyTube_3_BloodNeedle:
        case EmptyTube_4_Hands:
        case EmptyTube_4_BloodNeedle:
                     sendcommd.clear();
                     QUIUtils::get1213NumberCodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                     sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                     QUIUtils::get1415NumberCodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                     sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        case Bloodsample_BloodNeedle:
                    sendcommd.clear();
                    QUIUtils::get1011NumberCodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                    sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        default:
            break;
    }
    emit writdAxisata(sendcommd,"缓存坐标指令");
    return;
}



//清空列表内容
void CustomPlot::DelTableText()
{
    ui->tableWidget_displayPos->setRowCount(0);
    ui->label_movedPos->setText("校验位置: ");
}

//复位
void CustomPlot::on_toolButton_Backorigin_clicked()
{
    emit Resetmaneuver();
    auto *pconfAxis = SingletonAxis::GetInstance();
    QPoint locAxis(0,0);
    pconfAxis->originPos(READ_OPERRAT,locAxis);
    ui->label_movedPos->setText(QString("复位位置: X[%2]  Y[%3]").arg(locAxis.x()).arg(locAxis.y()));
    return;
}

void CustomPlot::closeEvent(QCloseEvent* event)
{
    if (QMessageBox::question(this, "修正完成", "是否要关闭?",
                           QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
       if (cglobal::gserialConnecStatus) {
           emit Resetmaneuver();
           ExitCoordinateSaving();
       }
       event->accept();
   } else {
       event->ignore();
   }
}


void CustomPlot::ExitCoordinateSaving()
{
    quint8 indexHole = 60;
    QPoint savingAxis(0,0);
    QMap<quint8,quint16> ExitSaveCoordin;
    QByteArrayList send_data_;
    ExitSaveCoordin.clear();
    send_data_.clear();
    for(int n = 0; n < 3; n++){
        int indexTray = n + 1;
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,indexHole*indexTray,MOTOR_HANDS_INDEX,savingAxis);
        ExitSaveCoordin.insert(n,savingAxis.y());
    }
    ExitSaveCoordin.insert(3,0);
    ExitSaveCoordin.insert(4,0);

    QByteArray writeinboard = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_HANDSY,AXISPOS_WRITE,ExitSaveCoordin);
    send_data_.push_back(writeinboard);

    QLOG_DEBUG()<<"退出坐标调试保存坐标尾端"<< writeinboard.toHex(' ').trimmed().toUpper()<<"leng="<<writeinboard.size()<<endl;
    emit writdAxisata(send_data_,"保存坐标微调界面坐标");
    return;
}

bool CustomPlot::QuerytoModifyCoordinates(QMap<int,QPoint> PointGather,int FindIndexKey,QPoint &FindedCoordinates)
{
    bool bContainsKey = false;
    bContainsKey = PointGather.contains(FindIndexKey);
    if(!bContainsKey) return false;
    auto iter = PointGather.find(FindIndexKey);
    FindedCoordinates = iter.value();
    return bContainsKey;
}


void CustomPlot::SelectHoleChangebgm(bool changeColor, int selectedIndex, int indexTray)
{
    if (selectedIndex <= 0) return;

    auto updateItem = [this, changeColor](int row, int column) {
        QTableWidgetItem* item = ui->tableWidget_displayPos->item(row, column);
        if (item && !item->text().isEmpty()) {
            item->setBackground(changeColor ? QColor(255, 0, 0) : QColor(255, 255, 255));
        }
    };

    updateItem(selectedIndex, Instrument_xpos);
    updateItem(selectedIndex, Instrument_ypos);

    if (changeColor) {
        ui->tableWidget_displayPos->selectRow(selectedIndex);
        m_selectRows = selectedIndex + indexTray * 60;
    }
}

void CustomPlot::ChangeControlColors(bool changeColor,int selectedIndex)
{
    static const QColor SELECTED_COLOR(255, 0, 0);
    static const QColor DEFAULT_COLOR(204, 204, 204);

    QTableWidgetItem* item = ui->tableWidget_displayPos->item(selectedIndex, IndexNum);
    if (!item || item->text().isEmpty()) {
        return;
    }

    if (changeColor) {
        item->setBackground(SELECTED_COLOR);
        m_selectRows = selectedIndex;
    } else {
        item->setBackground(DEFAULT_COLOR);
    }
}

//血样区BtnClick
void  CustomPlot::ClickBloodTube()
{
    if(m_CalibrationArea != Bloodsample_BloodNeedle)
    {
        QMessageBox::about(this,"血样区","请选择匹配的校准区域!");
        return;
    }
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QObject *object = QObject::sender();
    QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
    QString object_name = pushbtn->text();
    int index = object_name.toInt() - 1;
    if(m_selectRows != index && m_selectRows > 0)
    {
        SelectHoleChangebgm(false,m_selectRows,0);
    }
    mcodeNum = 0;
    mbtnClickPos.setX(0);
    mbtnClickPos.setY(0);
    SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,index,mbtnClickPos);

    SelectHoleChangebgm(true,index,0);
    QByteArrayList modifyarry = Testing:: m_TaskDll->XYLocation(mbtnClickPos,MOTOR_BLOOD_INDEX,OFFSET_BLOODAREA,m_downhigh,mcodeNum,m_downhigh);
    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry);
    ui->label_movedPos->setText(QString("校验位置: X[%2]  Y[%3]").arg(mbtnClickPos.x()).arg(mbtnClickPos.y()));
    return;
}

//Click 试管区
void CustomPlot::ClickEmptyTube()
{
	QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
	if (!button) return;

	bool ok = false;
	int index = button->text().toInt(&ok) - 1;
	if (!ok || index < 0) return;

	quint8 trayId = (index / 60) + 1;;
	QPoint targetPoint;
	int needleIndex = 0;
	QString statusText;
	QString operationType;

	// 定义校准区域类型
	bool isBloodNeedleArea = (m_CalibrationArea >= EmptyTube_1_BloodNeedle &&
		m_CalibrationArea <= EmptyTube_4_BloodNeedle);
	bool isHandsArea = (m_CalibrationArea >= EmptyTube_1_Hands &&
		m_CalibrationArea <= EmptyTube_4_Hands);

	if (!isBloodNeedleArea && !isHandsArea) {
		QMessageBox::about(this, "试管区", "请选择匹配的校准区域!");
		return;
	}

	// 确定针类型和操作类型
	if (isBloodNeedleArea) {
		needleIndex = MOTOR_BLOOD_INDEX;
		operationType = "血样针";
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, index, needleIndex, targetPoint);
	}
	else {
		needleIndex = MOTOR_HANDS_INDEX;
		operationType = "抓手";
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, index, needleIndex, targetPoint);
	}

	// 根据枚举值计算期望的托盘ID
	int expectedTrayId = 0;
	if (isBloodNeedleArea) {
		expectedTrayId = m_CalibrationArea - EmptyTube_1_BloodNeedle + 1;
	}
	else {
		expectedTrayId = m_CalibrationArea - EmptyTube_1_Hands + 1;
	}

	// 验证托盘ID匹配
	if (trayId != expectedTrayId) {
		QMessageBox::about(this, "试管区", "请选择匹配的校准区域!");
		return;
	}

	// 生成状态文本
	statusText = QString("试杯区%1(%2)校验位置: [%3,%4]")
		.arg(trayId)
		.arg(operationType)
		.arg(targetPoint.x())
		.arg(targetPoint.y());

	// 更新选择的行
	if (m_selectRows != index && m_selectRows > 0) {
		SelectHoleChangebgm(false, m_selectRows - 60 * (trayId - 1), trayId - 1);
	}

	// 生成运动指令
	mcodeNum = 0;
	QByteArrayList modifyArray = Testing::m_TaskDll->XYLocation(targetPoint, needleIndex,
		1, m_downhigh, mcodeNum, m_downhigh);

	// 更新UI并发送信号
	ui->label_movedPos->setText(statusText);
	emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);
}

void CustomPlot::ReminderTable(int TableRow)
{
    static const QColor SELECTED_COLOR(220, 44, 44);
    static const QColor DEFAULT_COLOR(204, 204, 204);

    const int rowCount = ui->tableWidget_displayPos->rowCount();

    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem* item = ui->tableWidget_displayPos->item(i, TableIndexPos::IndexNum);
        if (!item) continue;

        bool ok = false;
        const int rowNum = item->text().toInt(&ok) - 1;
        if (!ok) continue;

        item->setBackground(TableRow == rowNum ? SELECTED_COLOR : DEFAULT_COLOR);
    }
}

//点击测试通道
void  CustomPlot::ClickTestChannelTube()
{
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (!button) return;

    bool ok = false;
    int index = button->text().toInt(&ok) - 1;
    if (!ok || index < 0) return;

    if (m_selectRows != index) {
        ChangeControlColors(false, m_selectRows);
    }

    quint8 codeNum = 0;
    QPoint targetPoint;
    int motorIndex = MOTOR_HANDS_INDEX;
    QByteArrayList modifyArray;

    if (m_CalibrationArea == TestChannel_ReagentNeedle || m_CalibrationArea == TestChannel_Hands) {
        motorIndex = (m_CalibrationArea == TestChannel_ReagentNeedle) ? MOTOR_REAGNET_INDEX : MOTOR_HANDS_INDEX;
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, index, motorIndex, targetPoint);
        ChangeControlColors(true, index);
    } else {
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, index, MOTOR_HANDS_INDEX, targetPoint);
        motorIndex = MOTOR_HANDS_INDEX;
    }

    modifyArray = Testing::m_TaskDll->XYLocation(targetPoint, motorIndex, 0, m_downhigh, codeNum, m_downhigh);

    ui->label_movedPos->setText(QString("测试通道校验位置:[%1,%2]").arg(targetPoint.x()).arg(targetPoint.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);



//    quint8  codenum = 0%255;
//    QObject *object = QObject::sender();
//    QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
//    QString object_name = pushbtn->text();
//    int index = object_name.toInt() - 1;
//    if(m_selectRows != index)
//        ChangeControlColors(false,m_selectRows);

//    QByteArrayList modifyarry;
//    modifyarry.clear();

//    QPoint getOutPoint(0,0);
//    if(m_CalibrationArea == TestChannel_ReagentNeedle || m_CalibrationArea == TestChannel_Hands)
//    {
//        if(m_CalibrationArea == TestChannel_ReagentNeedle)
//        {
//            SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,index,MOTOR_REAGNET_INDEX,getOutPoint);
//            modifyarry = Testing:: m_TaskDll->XYLocation(getOutPoint, MOTOR_REAGNET_INDEX, 0, m_downhigh, codenum, m_downhigh);
//        }
//        else if(m_CalibrationArea == TestChannel_Hands)
//        {
//            SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,index,MOTOR_HANDS_INDEX,getOutPoint);
//            modifyarry =  Testing:: m_TaskDll->XYLocation(getOutPoint, MOTOR_HANDS_INDEX, 0, m_downhigh, codenum, m_downhigh);
//        }
//        ChangeControlColors(true,index);
//    }
//    else
//    {
//         SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,index,MOTOR_HANDS_INDEX,getOutPoint);
//         modifyarry =  Testing:: m_TaskDll->XYLocation(getOutPoint, MOTOR_HANDS_INDEX, 0, m_downhigh, codenum, m_downhigh);
//    }
//    ui->label_movedPos->setText(QString("测试通道校验位置:[%1,%2]").arg(getOutPoint.x()).arg(getOutPoint.y()));
//    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry);
//    return;
}

//点击试剂区
void  CustomPlot::ClickReagentsTube()
{
    if(m_CalibrationArea != ReagentLinque_ReagentNeedle)
    {
        QMessageBox::about(this,"试剂区","请选择匹配的校准区域!");
        return;
    }
    quint8  codenum = 0;
    QByteArrayList  Notifyarry;
    Notifyarry.clear();
    QObject *object = QObject::sender();
    QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
    QStringList object_name = pushbtn->objectName().split("_");
    int index = object_name.last().toInt() - 20; //20起
    if(m_selectRows != index && m_selectRows > 0)
    {
        SelectHoleChangebgm(false,m_selectRows,0);
    }


    QPoint getOutPoint(0,0);
    SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,index,getOutPoint);
    QLOG_DEBUG()<<"click试剂区"<<index<<"坐标"<<getOutPoint<<endl;

    SelectHoleChangebgm(true,index,0);
    QByteArrayList modifyarry = Testing::m_TaskDll->XYLocation(getOutPoint,MOTOR_REAGNET_INDEX, 0, m_downhigh,codenum, m_downhigh);
    ui->label_movedPos->setText(QString("试剂区校验位置:[%1,%2]").arg(getOutPoint.x()).arg(getOutPoint.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry); //试剂区
    return;
}


void CustomPlot::_sendmoveActiveToEquipment(AreasCalibration indexZone, QPoint MovingAxis,quint8 indexZ)
{
    QString outputText ="";
    quint8 codenum = 0;
    QByteArrayList modifyarry;
    modifyarry.clear();

    switch(indexZone)
    {
        case origin:
            outputText = "原点校验位置:";
            QUIUtils::_Locatetheorigin(modifyarry, MovingAxis ,m_downhigh);
        break;
        case detergent:
            outputText = "清洗剂校验位置:";
            modifyarry = Testing::m_TaskDll->XYLocation(MovingAxis, indexZ, 0, m_downhigh, codenum, m_downhigh);

        break;
    }
    ui->label_movedPos->setText(QString("%1[%2,%3]").arg(outputText).arg(MovingAxis.x()).arg(MovingAxis.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry);
    return;
}


//点击原点
void CustomPlot::UserClickOriginAxis()
{
    if(m_CalibrationArea != Origin_bloodNeedle)
    {
        QMessageBox::about(this,"清洗区原点","请选择匹配的校准区域!");
        return;
    }
    QPoint getOutPoint(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,getOutPoint);

    ChangeControlColors(true,0);

    _sendmoveActiveToEquipment(origin,getOutPoint,0);
    return;
}

//点击清洗孔
void CustomPlot::on_toolButton_Clean_1_clicked()
{
    if(CleanLinque_bloodNeedle != m_CalibrationArea && CleanLinque_ReagentNeedle != m_CalibrationArea)
    {
        QMessageBox::about(this,"清洗液区","请选择匹配的校准区域!");
        return;
    }
    quint8  indexZ = 0;
    QPoint  getOutPoint(0,0);
    if(CleanLinque_bloodNeedle == m_CalibrationArea)
    {
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX,getOutPoint);
        indexZ = MOTOR_BLOOD_INDEX;
    }
    else
    {
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX,getOutPoint);
        indexZ = MOTOR_REAGNET_INDEX;
    }

    ChangeControlColors(true,0);

    _sendmoveActiveToEquipment(detergent,getOutPoint,indexZ);

    return;

}

//点击弃杯孔
void CustomPlot::on_toolButton_throwTube_clicked()
{
    quint8 codeNum = 0;
    QPoint throwPoint;
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT, throwPoint);

    QByteArrayList modifyArray;
    QString statusText;

    if (ThrowCup_Hands != m_CalibrationArea) {
        int throwCupsDown = INI_File()._gethandsdownthrowcpus();
        modifyArray = Testing::m_TaskDll->AdjustModeThrowTube(throwPoint, MOTOR_HANDS_INDEX, 0, throwCupsDown, codeNum);
        statusText = QString("弃杯:[%1,%2]").arg(throwPoint.x()).arg(throwPoint.y());
    } else {
        modifyArray = Testing::m_TaskDll->XYLocation(throwPoint, MOTOR_HANDS_INDEX, 0, m_downhigh, codeNum, m_downhigh);
        ChangeControlColors(true, 0);
        statusText = QString("弃杯校验位置:[%1,%2]").arg(throwPoint.x()).arg(throwPoint.y());
    }

    ui->label_movedPos->setText(statusText);
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);
}


void CustomPlot::Recv_CalibrationMoved()
{
    QString text = ui->label_movedPos->text();
    if (text.indexOf("完成") == -1) {
        ui->label_movedPos->setText(text + " [完成]");
    }
}





void Generate_random_numbers(QVector<int> & Randomnum ,int spacevalue)
{
    int i,j;
    QList<int> numbersList;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(i = 0; i < 60; i++)
    {
        numbersList.append(qrand()%60);
        bool flag = true;
        while(flag){
         for(j = 0; j < i;j++){
            if(numbersList[i] == numbersList[j])
            {
                break;
            }
         }
         if(j < i)
         {
            numbersList[i] = rand()%60;
          }
         if(j == i){
            flag=!flag;
        }
      }
   }
    for(i = 0; i<numbersList.size(); i++)
    {
       Randomnum.append(numbersList[i] + spacevalue);;
    }
    return ;
}


void CustomPlot::recv_scynTestCommand(int index, const QByteArrayList commdArry)
{
     switch(index)
     {
         case 0: QLOG_DEBUG()<<"测试吸试剂"; break;
         case 1: QLOG_DEBUG()<<"测试吸PPP"; break;
     }
     emit this->SportActive(COORDINATE_FINE_TUNING_TEST,commdArry);
     QThread::sleep(1);
     emit Resetmaneuver(); //调试界面机器复位
     return;
}

/*测试抓手*/
void CustomPlot::on_pushButton_TrayHands_clicked()
{
    if(mpTestCaseRun == nullptr)
    {
        mpTestCaseRun = new TubeContinueDoing;
        connect(mpTestCaseRun,&TubeContinueDoing::scynTestCommand,this,&CustomPlot::recv_scynTestCommand);
        connect(mpTestCaseRun,&TubeContinueDoing::sendcode,this,&CustomPlot::slotsendcode);
    }
    mpTestCaseRun->show();
    return; 
}
void CustomPlot::slotsendcode(QByteArrayList data_)
{
     emit writdAxisata(data_,"吸吐");
     //QLOG_DEBUG()<<"=============";
}

void CustomPlot::on_spinBox_downValue_valueChanged(int arg1)
{
    m_downhigh = arg1;
    update();
}

void CustomPlot::on_pushButton_TrayHands_2_clicked()
{
    if(m_indexDown < 0 ){
        QMessageBox::about(this,tr("测试失败"),tr("请配试抓手下降高度!"));
        return;
    }
    mfromtray = ui->spinBox_from_tray->value() - 1;
    mendtray =  ui->spinBox_endtray->value() - 1;
    if(mfromtray != mendtray){
        mfromHole.clear();
        mendHole.clear();
        mtest_catch_put_commad.clear();
        for(int i = 0; i < total_tube_num ; i++){
            mfromHole.push_back(mfromtray*total_tube_num + i);
            mendHole.push_back(mendtray*total_tube_num + i);
        }
        ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::BLINK);
        //打乱抓取试管盘的试管号(from)
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(mfromHole.begin(),mfromHole.end(),std::default_random_engine(seed));
        //从小到大放置孔号
        std::sort(mendHole.begin(),mendHole.end());
        //输出抓取孔号放到托盘孔号
        int index_ = 0;
        auto it = mfromHole.constBegin();
        while(it != mfromHole.constEnd()){
            int hole_catch = *it;
            int hole_put = mendHole.at(index_);
            test_catch_putdown_cup(hole_catch,hole_put,mtest_catch_put_commad);
            index_++;
            it++;
        }
        send_test_cups_accurate();
    }else{
        QMessageBox::about(this,tr("测试失败"),tr("相同试管盘请勿移动!"));
    }
    return;
}

void CustomPlot::test_catch_putdown_cup(const int form_,const int end_,QMap<int,QByteArrayList> &group_directives)
{
    QByteArrayList once_directives;
    QPoint from_tube_axis(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,form_,MOTOR_HANDS_INDEX,from_tube_axis);
    QPoint end_tube_axis(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,end_,MOTOR_HANDS_INDEX,end_tube_axis);
    QUIUtils::test_hands_catchtube_precise(from_tube_axis,end_tube_axis,m_downhigh,once_directives);
    group_directives.insert(end_,once_directives);
    return;
}

void CustomPlot::send_test_cups_accurate()
{
    foreach(int insex_ ,mendHole)
    {
        if(mtest_catch_put_commad.contains(insex_))
        {
             QMap<int, QByteArrayList>::iterator iter = mtest_catch_put_commad.find(insex_);
             int key = iter.key();
             QByteArrayList value = iter.value();
             //测试抓被准确性
             emit this->TrayMoveTest(value,true,value.size());
             mtest_catch_put_commad.remove(key);
        }
    }
    if(mtest_catch_put_commad.isEmpty()){
        ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::OFF);
        emit Resetmaneuver();
    }
    return;
}


//移动到摄像头区域下样本针
void CustomPlot::on_toolButton_video_clicked()
{
    static const QPoint locVideo(190, 3140);
    quint8 codeNum = 0;

    auto modifyArray = Testing::m_TaskDll->XYLocation(locVideo, Blood_z, 2, m_downhigh, codeNum, m_downhigh);

    ui->label_movedPos->setText(QString("信息录入坐标:[%1,%2]").arg(locVideo.x()).arg(locVideo.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);
}
