#pragma execution_character_set("utf-8")

#include "genericfunctions.h"
#include "testing.h"
#include "ui_testing.h"
#include <QScrollBar>
#include <QMessageBox>
#include <QTreeWidgetItem>
#include <QCoreApplication>
#include <math.h>
#include <QTimer>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <string.h>
#include <QPainter>
#include <QMouseEvent>
#include <QQueue>
#include <QtConcurrent>
#include "globaldata.h"
#include "globaldata.h"
#include "loginui.h"
#include "quiutils.h"
#include <operclass/fullyautomatedplatelets.h>

using namespace std;
UsbCodeDispose* Testing::m_TaskDll = nullptr;

Testing::Testing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Testing),
    mInituiBloodArea(false),
    mReminderTube(-1),
    alreadyinitchannelui(false)
    ,m_ProTotalTube(0),
    m_ThrowTube(0),
    m_blinkState(false)

{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    QPixmap pixmap(":/Picture/test_hover.png");
    QCursor cursor(pixmap);
    this->setCursor(cursor);
    mflashingTubeList.clear();
    m_TaskDll = new UsbCodeDispose;
}

Testing::~Testing()
{

    delete m_TaskDll;
    m_TaskDll = nullptr;


    if (mtimer) {
        if (mtimer->isActive())
            mtimer->stop();
        delete mtimer;
        mtimer = nullptr;
    }
    delete ui;
}

void Testing::initequipmentKind(const quint8 indextype)
{
    mtimer = new QTimer(this);
    connect(mtimer, &QTimer::timeout, this, &Testing::toggleBlinkState);
    mtimer->start(500);

    minstrumentType = indextype;

    memset(mInitEmptyArea, false, sizeof(mInitEmptyArea));

    mEmptyTubeAssigned.clear();
    mEmptyTubeAbsorb_Poorblood.clear();
    mEmptyTubeAbsorb_Richblood.clear();
    m_TestingSample.clear();
    mEmptyText.clear();
    m_Blood_Tray_Checked.clear();
    m_Test_Tray_has_hole.clear();  //已使用得空试管孔

    initializeMachineUI(minstrumentType); /*初始化测试界面测试通道*/

    Initialize_the_Task_interface(); //初始化添加样本对话框
}

void  Testing::toggleBlinkState(){
    m_blinkState = !m_blinkState; // 切换状态
    update();
}

void Testing::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    mInituiBloodArea = false;

    //先获取大框的宽和高
    const int  appWidth = ui->widget_main->size().width();
    const int  appHeigh = ui->widget_main->size().height();

    //间距
    const int boardSpaceX = 5;
    const int boardSpaceY = 5;


    int ChnBarHeight = 155;                                     //测试通道高
    int ThrowCupWidth = 120;                                    //弃杯孔宽
    int ThrowCupHeight = boardSpaceY - 5;                    //弃杯孔高
    int Left_width = appWidth - boardSpaceX*2 - 2*2;


    //测试通道Loc
    int ChnModulePos_x = 0;
    ui->widget_channelarea->setGeometry(boardSpaceX, boardSpaceY, Left_width,ChnBarHeight);
    ui->DroptheCup->setGeometry(appWidth - 100, 5, ThrowCupHeight, ThrowCupWidth);  //弃杯孔位置
    //QLOG_DEBUG()<<"大框高"<<big_h<<"通道高"<<ChnBarHeight<<endl;

    //血样区和试管盘高度 = 大框高 - 测试通道高 - 2个间距 - 4个横线线宽
    int BloodAndTrayTubeHeight = appHeigh - ui->widget_channelarea->height() - boardSpaceY*3 - 4*2;
    //试管馆盘位置
    int TrayTubeHeight = BloodAndTrayTubeHeight/3*2 ;  //试管区高度
    int taryTopy = ui->widget_channelarea->geometry().bottomLeft().y() + boardSpaceY;
    ui->widget_testcup->setGeometry(boardSpaceX, taryTopy,Left_width, TrayTubeHeight);
    //QLOG_DEBUG()<<"试管馆盘位置"<<"topx"<<BoadrSpacing_x <<"topy"<<taryTopy<<"试管宽"<<Left_width<<"试管高"<<TrayTubeHeight<<endl;

    //血样管区域
    int bloodBottomLoc_topy  = ui->widget_testcup->geometry().bottomLeft().y() + boardSpaceY; //底部Y - 血样区高 - 间距
    int BloodSampleHeight = appHeigh - ui->widget_channelarea->height() - ui->widget_testcup->height() - 3*boardSpaceY; //血样区高
    ui->widget_Sample_1->setFixedHeight(BloodSampleHeight);
    ui->widget_Sample_1->setFixedWidth(Left_width);
    ui->widget_Sample_1->setGeometry(boardSpaceX,bloodBottomLoc_topy ,Left_width,BloodSampleHeight);
    //QLOG_DEBUG()<<"血样区和试管盘高度"<<BloodAndTrayTubeHeight<<"血样区高"<<BloodSampleHeight<<endl;



    //单个试管盘宽
    constexpr  int bigSpace = 29;
    constexpr  int kDefaultTraySpacingY = 15;
    int ColWidth = (Left_width - 5)/bigSpace;
    int TaryTubeWidth = ColWidth *6;


    ui->widget_TestCup_0->setFixedSize(TaryTubeWidth,TrayTubeHeight);
    ui->widget_TestCup_1->setFixedSize(TaryTubeWidth,TrayTubeHeight);
    ui->widget_TestCup_2->setFixedSize(TaryTubeWidth,TrayTubeHeight);
    ui->widget_TestCup_3->setFixedSize(TaryTubeWidth,TrayTubeHeight);
    //QLOG_DEBUG()<<"单个试管宽"<<TaryTubeWidth;

    //试管区高度
    int ibeginning = 0;
    int iending = 0;
    switch(minstrumentType)
    {
        case KS600:
        {
            ibeginning = 120;
            iending = 240;
            ui->widget_TestCup_2->hide();
            ui->widget_TestCup_3->hide();
            ChnModulePos_x = (appWidth - boardSpaceX - ui->widget_Module1->width() - ThrowCupWidth)/2;
            ui->widget_Module1->move(ChnModulePos_x ,kDefaultTraySpacingY);


            ui->widget_TestCup_0->setGeometry(ColWidth*1 + TaryTubeWidth/2, 0,TaryTubeWidth, TrayTubeHeight );
            ui->widget_TestCup_1->setGeometry(ColWidth*2+TaryTubeWidth*2+TaryTubeWidth/2, 0, TaryTubeWidth, TrayTubeHeight);
            for(;ibeginning < iending; ibeginning++)
                FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(ibeginning, TESTTUBES_CLIPPEDAWAY);
            break;
        }
        case KS800:
        {
            ibeginning = 180;
            iending = 240;
            ChnModulePos_x = (appWidth - boardSpaceX - ui->widget_Module1->width() - ui->widget_Module2->width() - ThrowCupWidth)/2;
            ui->widget_Module1->move(ChnModulePos_x ,kDefaultTraySpacingY);
            ChnModulePos_x = ChnModulePos_x + ui->widget_Module1->width() + boardSpaceX*2;
            ui->widget_Module2->move(ChnModulePos_x ,kDefaultTraySpacingY);

            ui->widget_TestCup_3->hide();
            ui->widget_TestCup_0->setGeometry(ColWidth*1+TaryTubeWidth/2, 0, TaryTubeWidth, TrayTubeHeight );
            ui->widget_TestCup_1->setGeometry(ColWidth*2+TaryTubeWidth/2+TaryTubeWidth, 0, TaryTubeWidth, TrayTubeHeight);
            ui->widget_TestCup_2->setGeometry(ColWidth*3+TaryTubeWidth/2+TaryTubeWidth*2, 0, TaryTubeWidth, TrayTubeHeight);
            for(;ibeginning < iending; ibeginning++)
                FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(ibeginning, TESTTUBES_CLIPPEDAWAY);
            break;
        }
        case KS1200:
        {
           ui->widget_TestCup_0->setGeometry(ColWidth *1, 0, TaryTubeWidth, TaryTubeWidth);
           ui->widget_TestCup_1->setGeometry(ColWidth *2 + TaryTubeWidth*1, 0, TaryTubeWidth, TrayTubeHeight );
           ui->widget_TestCup_2->setGeometry(ColWidth *3 + TaryTubeWidth*2, 0, TaryTubeWidth, TrayTubeHeight);
           ui->widget_TestCup_3->setGeometry(ColWidth *4 + TaryTubeWidth*3, 0, TaryTubeWidth, TrayTubeHeight);

           ChnModulePos_x = (appWidth - boardSpaceX - ui->widget_Module1->width() - ui->widget_Module2->width()- ui->widget_Module3->width() - ThrowCupWidth)/2;
           ui->widget_Module1->move(ChnModulePos_x ,kDefaultTraySpacingY);
           ChnModulePos_x = ChnModulePos_x + ui->widget_Module1->width() + boardSpaceX*2;
           ui->widget_Module2->move(ChnModulePos_x ,kDefaultTraySpacingY);
           ChnModulePos_x = ChnModulePos_x + ui->widget_Module2->width() + boardSpaceX*3;
           ui->widget_Module3->move(ChnModulePos_x ,kDefaultTraySpacingY);
           break;
        }
        default:{
            ibeginning = 120;
            iending = 240;
            ui->widget_TestCup_2->hide();
            ui->widget_TestCup_3->hide();
            ui->widget_TestCup_0->setGeometry(ColWidth*1+TaryTubeWidth/2+TaryTubeWidth, 0, TaryTubeWidth, TrayTubeHeight );
            ui->widget_TestCup_1->setGeometry(ColWidth*2+TaryTubeWidth*2+TaryTubeWidth/2, 0, TaryTubeWidth, TrayTubeHeight);
            for(;ibeginning < iending; ibeginning++)
                FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(ibeginning, TESTTUBES_CLIPPEDAWAY);
            break;

        }
    }
    return;
}

void Testing::initControlShowChannelProgress(quint8 &startChannel,QWidget * progressChannel,
                                                QWidget *ptestChannel,QPalette pa){
    QList<ProgressBar*> channelControl = progressChannel->findChildren<ProgressBar* >();
    QList<QLabel *> chnStateTextList = ptestChannel->findChildren<QLabel* >();
    for(int n = 0; n < channelControl.size(); n++)
    {
        channelControl.at(n)->flashingReminder(true);
        m_channelShowsTheProgress.push_back(channelControl.at(n));
        Channelreminder.push_back(chnStateTextList.at(n));
        channelControl.at(n)->setRange(0, 100);
        channelControl.at(n)->installEventFilter(this);
        bool usedChannel = INI_File().rConfigPara(QString("TestTheChanne1Opening_%1").arg(startChannel + 1)).toBool();
        if(!usedChannel)
           channelControl.at(n)->setChnTextindex(-1);
        else
           channelControl.at(n)->setChnTextindex(startChannel + 1);
        if (!alreadyinitchannelui)
        {
            chnStateTextList.at(n)->setPalette(pa);
            chnStateTextList.at(n)->hide();
        }
        startChannel++;
    }

}

void Testing::initializeMachineUI(const quint8 equipmentIndex)
{
    QPalette pa;
    pa.setColor(QPalette::WindowText, QColor(250, 128, 114));
    QList<QWidget *> widgetFilterList = { ui->widget_Reagents ,
                                            ui->widget_Abandoned_new,
                                            ui->widget_Sample_1,
                                            ui->widget_TestCup_3,
                                            ui->widget_TestCup_2,
                                            ui->widget_TestCup_1,
                                            ui->widget_TestCup_0,
                                            ui->DroptheCup,
                                            ui->widget_cleanagent
    };
    for (auto pwidget : widgetFilterList)
        pwidget->installEventFilter(this);//设置过滤器

    quint8 index = 0;
    initControlShowChannelProgress(index,ui->widget_Module1,ui->widget_Module1,pa);
    initControlShowChannelProgress(index,ui->widget_Module2,ui->widget_Module2,pa);
    initControlShowChannelProgress(index,ui->widget_Module3,ui->widget_Module3,pa);

    const std::unordered_map<int, ModuleVisibility> configMap = {
        {KS600,  {true,  false, false}},
        {KS800,  {true,  true,  false}},
        {KS1200, {true,  true,  true}}
    };
    auto it = configMap.find(equipmentIndex);
    if (it != configMap.end()) {
        const auto& config = it->second;
        ui->widget_Module1->setVisible(config.showModule1);
        ui->widget_Module2->setVisible(config.showModule2);
        ui->widget_Module3->setVisible(config.showModule3);
    } else {
        QLOG_WARN() << "Unknown equipment index:" << equipmentIndex;
    }
    alreadyinitchannelui = true;
    return;
}


void Testing::updateChannelProgressAndStatus(bool isWaitstate, quint8 channelIndex)
{
    // 防御性检查
    if (channelIndex >= m_channelShowsTheProgress.size() ||
        channelIndex >= Channelreminder.size()) {
        QLOG_WARN()<<("Invalid channel index: %d", channelIndex);
        return;
    }
    m_channelShowsTheProgress[channelIndex]->setValue(0);
    QLabel* reminderLabel = Channelreminder[channelIndex];
    QString prefix = (reminderLabel->text().indexOf(':')) ? reminderLabel->text().left(reminderLabel->text().indexOf(':'))
                                                            : reminderLabel->text();
    QString outText = (isWaitstate) ? prefix + tr("[等待]") : "";
    reminderLabel->setText(outText);
    // 强制刷新界面
    reminderLabel->repaint();
}






void Testing::slot_sycn_SampleTestingChangInitColor(QPoint maphole,quint8 indexChn)
{
    SampleTestingChangInitColor(maphole,indexChn);
}



/*通道禁用主界面显示*/
void  Testing::recv_NotifyChannleState(const quint8 channelIndexNum,const bool bUseing)
{
    int index = channelIndexNum - 1;
    if (bUseing) {
        m_channelShowsTheProgress.at(index)->setChnTextindex(channelIndexNum);
    } else {
        m_channelShowsTheProgress.at(index)->setChnTextindex(CHN_DISABLE);
    }
    return;
}

void Testing::showTestChannelInfo(const quint8& channelIndex,
                                   const QString&	sampleName,
                                   const quint8&	reagentIndex){

    // 1. 检查父对象状态
    if (!this) {
        QLOG_FATAL() << "Parent object destroyed!";
        return;
    }

    if (channelIndex >= Channelreminder.size() || Channelreminder.isEmpty()) {
        QLOG_WARN() << "Invalid channel index:" << channelIndex;
        return;
    }

    //智能指针检查
   QPointer<QLabel> channelLabel = Channelreminder[channelIndex - 1];
   if (channelLabel.isNull()) {  // 自动检测对象是否被销毁
       QLOG_DEBUG() << "Label at index" << channelIndex-1 << "already destroyed";
       return;
   }

    // 样本ID解析（移除未使用的idDate变量）
    int sampleNumber = 0;
    QString unusedDate;
    GlobalData::apartSampleId(sampleName, unusedDate, sampleNumber);


   // 试剂名称映射（添加默认值兜底）
   const QString reagentName = GlobalData::mapIndexReagentnames(reagentIndex).isEmpty()
                              ? QStringLiteral("Unknown_Reagent")
                              : GlobalData::mapIndexReagentnames(reagentIndex);

   // 格式化输出文本（预分配内存避免多次拼接）
   const QString displayText = tr("样本:%1[%2]")
                               .arg(sampleNumber)
                               .arg(reagentName);

   // 更新通道显示（安全访问容器）
   //QLabel* channelLabel = Channelreminder.at(channelIndex - 1);
   channelLabel->setText(displayText);
   channelLabel->show();

   // 更新样本-通道映射（使用insert直接覆盖旧值）
   m_TestingSample.insert(sampleName, channelIndex);

   // 限制界面刷新频率（避免频繁重绘）
   QTimer::singleShot(0, this, [this]() { update(); });

}



bool Testing::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widget_Reagents && event->type() == QEvent::Paint)
    {
        plotReagentWellsCoordinates(REAGENT_TOTAL * 2); //参数为孔的数量
        showPaintReagents();                            //响应函数 绘制试剂
    }
    if(watched == ui->widget_Abandoned_new && event->type() == QEvent::Paint)
    {
        showCleaningbit();   /*绘制清洗位*/
    }
    if(watched == ui->widget_Sample_1 && event->type() == QEvent::Paint)
    {
        if (!mInituiBloodArea)
        {
            CreatBloodZoneAxisPos();
            mInituiBloodArea = true;
        }
        DrawBloodTopText(); //绘制血样区头部文字

        UpdateBloodHoleColors(TUBE_INIT,      m_BloodHoleInitUiAxis);
        UpdateBloodHoleColors(TUBE_CHECKED,   m_Blood_Tray_Checked);
        UpdateBloodHoleColors(TUBE_OUTRESULT, m_Blood_Tray_OutResult);
    }
    if(watched == ui->widget_TestCup_0 && event->type() == QEvent::Paint)
    {
        if (!mInitEmptyArea[EMPTYTRAY_1])
        {
            CreatTrayTestTubeUiAxis(EMPTYTRAY_1);
            mInitEmptyArea[EMPTYTRAY_1] = true;
            init_testtube_tray(EMPTYTRAY_1);
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_0,EMPTYTRAY_1,m_BigRadius,m_SmalleRadius);
    }
    if (watched == ui->widget_TestCup_1 && event->type() == QEvent::Paint)
    {
        if (!mInitEmptyArea[EMPTYTRAY_2])
        {
            CreatTrayTestTubeUiAxis(EMPTYTRAY_2);
            mInitEmptyArea[EMPTYTRAY_2] = true;
            init_testtube_tray(EMPTYTRAY_2);
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_1, EMPTYTRAY_2,m_BigRadius,m_SmalleRadius);
    }
    if (watched == ui->widget_TestCup_2 && event->type() == QEvent::Paint)
    {
        if (!mInitEmptyArea[EMPTYTRAY_3])
        {
            CreatTrayTestTubeUiAxis(EMPTYTRAY_3);
            mInitEmptyArea[EMPTYTRAY_3] = true;
            init_testtube_tray(EMPTYTRAY_3);
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_2, EMPTYTRAY_3,m_BigRadius,m_SmalleRadius);
    }
    if(watched == ui->widget_TestCup_3 && event->type() == QEvent::Paint)
    {
        if (!mInitEmptyArea[EMPTYTRAY_4])
        {
            CreatTrayTestTubeUiAxis(EMPTYTRAY_4);
            init_testtube_tray(EMPTYTRAY_4);
            mInitEmptyArea[EMPTYTRAY_4] = true;
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_3, EMPTYTRAY_4,m_BigRadius,m_SmalleRadius);
    }
    if(watched == ui->DroptheCup && event->type() == QEvent::Paint)
    {
        showDiscardTheCup(); /*弃杯孔*/
    }
    if(watched == ui->widget_cleanagent && event->type() == QEvent::Paint)
    {
        showCleanreagent();//清洗剂
    }
    return QWidget::eventFilter(watched,event);
}

void Testing::init_testtube_tray(const int index_tray)
{
    int status_tube = 0,sample_id = 0;
    QString reag_index;
    int from_tube = index_tray *ONETRAY_TOTALTUBE;
    int end_tube =  from_tube + ONETRAY_TOTALTUBE;
    for(;from_tube < end_tube; from_tube++)
    {
      FullyAutomatedPlatelets::pinstancesqlData()->FindOneEmptyTube(from_tube, status_tube, reag_index, sample_id);
      quint8 reagentindex = GlobalData::indexReagentMapReagnetNmaes(reag_index);
      if(status_tube  == TESTTUBES_SAMPLED)
      {
          InitTubeAddBlooded(from_tube, reagentindex, sample_id);      //试管已加血样
      }
      else if(status_tube  == TESTTUBES_CLIPPEDAWAY)
      {
          InitUIEmptyTubeused(from_tube);   //已使用的空试管变灰
      }
      else
      {
          FullyAutomatedPlatelets::pinstancesqlData()->UpdateTestTubeStateInfo(from_tube, TESTTUBES_FREETIME, "noone", 0);
      }
    }
    update();
    return;
}



//试管已被夹走用掉了
void  Testing::InitUIEmptyTubeused(int TestTubeUsed)
{
    if (m_Testcups.contains(TestTubeUsed))
    {
        auto hole =	m_Testcups.find(TestTubeUsed);
        mEmptyTubeClipMoved.insert(hole.key(),hole.value());
    }
    else
    {
       QLOG_ERROR()<<"试管:"<< TestTubeUsed<<"夹取失败";
    }
    update();
    return;
}



//初始化试管已加血样
void  Testing::InitTubeAddBlooded(int Tube,quint8 Reagetn,int SampleNumber)
{
    if(m_Testcups.contains(Tube))
    {
         auto iter = m_Testcups.find(Tube);
         if(Reagetn == 0)
             mEmptyTubeAbsorb_Poorblood.insert(Tube,iter.value());
         else
            mEmptyTubeAbsorb_Richblood.insert(Tube,iter.value());
         mEmptyText.insert(Tube,QString::number(SampleNumber));
    }
    update();
    return;
}


//清洗孔位置
void Testing::showCleaningbit()
{
    QPainter painter(ui->widget_Abandoned_new);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    constexpr int kMargin = 20;
    constexpr int kTextSpacing = 25;
    const QRect widgetRect = ui->widget_Abandoned_new->rect();
    m_CleanPos = {
        {0, QPointF(kMargin + Big_radius, kTextSpacing + Big_radius)},
        {1, QPointF(kMargin + Big_radius, widgetRect.height() - kTextSpacing - Big_radius)}
    };
    QMap<quint8, QPointF>::const_iterator iter;
    for (iter = m_CleanPos.begin(); iter != m_CleanPos.end(); ++iter) {
        QPalette palette;
        painter.setPen(cglobal::g_LineColor); //圆环外圈的颜色
        palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor); //内圆环背景颜色
        painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式外圆环的颜色
        painter.drawEllipse(iter.value(),Big_radius,Big_radius); //画大圆
        painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
        painter.drawEllipse(iter.value(),small_radius,small_radius); //画小圆
    }
    QFont font = this->font();
    font.setPointSize(14);
    painter.setFont(font);
    painter.setPen(cglobal::g_TextColorBlack);
    QRectF drawrect(m_CleanPos[1].x() - Big_radius*2 - 5 ,
        m_CleanPos[1].y() + kMargin,widgetRect.width(),kTextSpacing);
    painter.drawText(drawrect,Qt::AlignHCenter,"清洗位");
    painter.end();
    return;
}



/*初始化任务测高界面*/
void Testing::Initialize_the_Task_interface()
{
    FullyAutomatedPlatelets::pinstanceAddsampletest()->_initcreat();
    connect(FullyAutomatedPlatelets::pinstanceAddsampletest(),&Height_Data::Taskconfigcloe,this,[=](){
         m_opendedheight = false; //任务对话框关闭
    });
    connect(FullyAutomatedPlatelets::pinstanceAddsampletest(),&Height_Data::ReminderHole,this,[=](int richhole){
        mReminderTube = richhole;
        update();
    });
    return;
}

void  Testing::SlotRemderbloodhole(int richhole)
{
    mReminderTube = richhole;
    update();
    return;
}

void Testing::DrawChannelProgress(quint8 Index ,double proportion)
{
    ProgressBar *PupdateTestProgress =  m_channelShowsTheProgress.at(Index);
    PupdateTestProgress->setValue(proportion);
    PupdateTestProgress->show();
    // 限制界面刷新频率（避免频繁重绘）
    QTimer::singleShot(0, this, [this]() { update(); });
    return;
}

/*********** 测高函数 *********************************************************/
//测高完成
void Testing::TiggerTestHighdone(QString ImagePath, double Heigh, bool isreplaceteshigh, QString numid)
{
    // 获取单例窗口实例
    auto* sampleTestWindow = FullyAutomatedPlatelets::pinstanceAddsampletest();

    if (m_opendedheight) {
        // 窗口首次打开时的初始化逻辑
        sampleTestWindow->setWindowFlags(Qt::Widget);
        sampleTestWindow->show();

    } else {
        // 关闭窗口时改用 hide() 避免资源释放问题
        sampleTestWindow->hide();
        sampleTestWindow->show();
    }

    // 统一管理窗口状态
    m_opendedheight = !m_opendedheight;  // 直接取反状态

    // 显式传递参数（必要时进行类型转换）
    sampleTestWindow->slotShowTestImageTube(
        ImagePath,
        Heigh,//static_cast<int>(std::round(Height)),  // 处理 double 转 int 的精度问题
        isreplaceteshigh,
        numid
    );

//    if(m_opendedheight == false)
//    {
//        FullyAutomatedPlatelets::pinstanceAddsampletest()->setWindowFlags(Qt::Widget);
//        FullyAutomatedPlatelets::pinstanceAddsampletest()->show();
//        m_opendedheight = true;
//    }
//    else
//    {
//        m_opendedheight = false;
//        FullyAutomatedPlatelets::pinstanceAddsampletest()->close();
//        FullyAutomatedPlatelets::pinstanceAddsampletest()->show();
//    }
//    FullyAutomatedPlatelets::pinstanceAddsampletest()->slotShowTestImageTube(ImagePath, Heigh,isreplaceteshigh,numid);
    return;
}



/*点击打开任务框*/
void Testing::_showaddsamplewidget()
{
    auto widget = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (widget->isMinimized()) {
        widget->showNormal();  // 先恢复窗口标准状态
        widget->raise();       // 确保窗口置顶
        widget->activateWindow(); // 激活窗口焦点
    }

    if(m_opendedheight == false){
        widget->setWindowFlags(Qt::Widget);
        widget->show();
        m_opendedheight = true;
    }
    else{
        m_opendedheight = false;
        widget->close();  // 隐藏窗口
        widget->show();   // 显示窗口
    }
    return;
}


void Testing::TotalTaskProgress()
{
    m_ThrowTube = 0;
    ui->widget_showtips->setValue(0);
    update();
    return;
}




void Testing::slot_throwtesttube()
{
    if(m_ProTotalTube == 0) return;
    m_ThrowTube++;
    if(m_ThrowTube >= m_ProTotalTube)
        ui->widget_showtips->setValue(100);
    else
        ui->widget_showtips->setValue(m_ThrowTube*100/m_ProTotalTube);
    QLOG_DEBUG()<<"丢杯个数"<<m_ThrowTube << "总数"<<m_ProTotalTube;
    update();
}


/*所有样本测试完成*/
void Testing::AllSampleTested()
{
    ui->widget_showtips->setValue(100);
    m_ProTotalTube = 0; //需要测试的样本数
    m_ThrowTube = 0;    //弃杯数

    //放弃样本血样孔闪烁状态回归
    mflashingTubeList.clear();
    mtimer->stop();
    for(auto *pProgress : m_channelShowsTheProgress)
        pProgress->flashingReminder(true);



    //试管选中状态复位 重新初始化血样下拉框血样孔
    FullyAutomatedPlatelets::pinstanceAddsampletest()->_initNumAnaemiaHole();
    ComplBackColorBloodArea();  //完成测试 血样区复原
    emit tsetfinishedbackorigin();

    update();
    return;
}


void Testing::sycn_changeui_status(QString sample_name,quint8 anemiahole,QList<quint8> marktube,int index_add,int all_add_task)
{
    m_ProTotalTube = m_ProTotalTube + marktube.size();
    if(m_ProTotalTube != 0)
        ui->widget_showtips->setValue(m_ThrowTube*100/m_ProTotalTube);
    VectorSelectedBloodAreaTube(anemiahole, sample_name);    //选中贫血 --改界面颜色
    VectorSelectedBloodAreaTube(anemiahole + 1, sample_name);//选中富血

    foreach (quint8 var, marktube)
    {
        bool contains_ = m_Testcups.contains(var);
        if(contains_)
        {
            auto tube_ = m_Testcups.find(var);
            int IndexHole = tube_.key();
            mEmptyTubeAssigned.insert(IndexHole, tube_.value()); //试管分配血样
            QString num_ = sample_name.split("-", QString::SkipEmptyParts).last();
            mEmptyText.insert(var, num_);
        }
    }
    if(cglobal::g_StartTesting && index_add == all_add_task)
    {
        emit testingaddsample();
    }
    update();
    return;
}

void Testing::ChaneColorEmptyAreaTray(QString SampleName)
{
    QList<quint8> EmptytubeTray;
    EmptytubeTray.clear();
    StructInstance::getInstance()->sycn_sample_all_test_tube(SampleName,EmptytubeTray);//找出单个样本的贫富血试管孔
    for (auto EmptyTube : EmptytubeTray)
    {
        EmptyTubeAssigned(EmptyTube);
    }
    update();
    return;
}


void Testing::tubeMarked(const QString& outtext,const quint8& tubeIndex){
    auto it = m_Testcups.find(tubeIndex);
    if (it != m_Testcups.end()) {
        // 移动试管到已使用容器（避免内存重复分配）
        mEmptyTubeAssigned.insert(tubeIndex, it.value());
        mEmptyText[tubeIndex] = outtext;  // 使用[]运算符直接写入
    }
    //同步耗材界面试管用掉
    emit SynchronizeEmptyTube(tubeIndex);
    update();
}

/*任务试管标记选中*/
void Testing::EmptyTubeAssigned(quint8 IndexTube)
{
    if(m_Testcups.contains(IndexTube))
    {
        auto emptytube = m_Testcups.find(IndexTube);
        int IndexHole = emptytube.key();
        mEmptyTubeAssigned.insert(IndexHole, emptytube.value()); //试管分配血样
        QLOG_DEBUG() << "试管:" << emptytube.key() << "标记选中";
        int sample_id = StructInstance::getInstance()->sycn_emptytube_get_sampleid(IndexTube);
        mEmptyText.insert(IndexTube, QString::number(sample_id));
    }
    update();
    return;
}

/*更换试管盘*/
void Testing::_replacEmptyTestTary(int index_tary)
{
    //耗材界面点更换试管盘
    //QtConcurrent::run(this, &Testing::replacementTray, index); //更换试管盘、线程
    int _statrHole,_endHole = 0;
    _statrHole = ONETRAY_TOTALTUBE * index_tary;
    _endHole = _statrHole + ONETRAY_TOTALTUBE;
    for(; _statrHole < _endHole; _statrHole++)
        VectorReplaceClear(_statrHole);
    FullyAutomatedPlatelets::pinstancesqlData()->UpadteTrayState(index_tary);  //替换数据库试管状态
    //FullyAutomatedPlatelets::pinstanceinfowidget()->InsertText(PROMPTLOG,"更换盘"+ QString::number(index_tary + 1)+"完成");
    QLOG_ERROR()<<"更换盘"<<QString::number(index_tary + 1)<<"完成"<<endl;
    //MachineLayoutTask->ReplaceTrayEmptyTube(index);
    update();
    return;
}

void Testing::VectorReplaceClear(quint8 Index)
{
    //耗材更换试管盘--处理测试界面显示
    if (mEmptyTubeAssigned.contains(Index))
    {
        auto EmptyTube = mEmptyTubeAssigned.find(Index);
        mEmptyTubeAssigned.erase(EmptyTube);
    }
    if (mEmptyTubeAbsorb_Poorblood.contains(Index))
    {
        auto EmptyTube = mEmptyTubeAbsorb_Poorblood.find(Index);
        mEmptyTubeAbsorb_Poorblood.erase(EmptyTube);
    }
    if (mEmptyTubeAbsorb_Richblood.contains(Index))
    {
        auto EmptyTube = mEmptyTubeAbsorb_Richblood.find(Index);
        mEmptyTubeAbsorb_Richblood.erase(EmptyTube);
    }
    if (mEmptyTubeClipMoved.contains(Index))
    {
        auto EmptyTube = mEmptyTubeClipMoved.find(Index);
        mEmptyTubeClipMoved.erase(EmptyTube);
    }
    if (mEmptyText.contains(Index))
    {
        auto Text = mEmptyText.find(Index);
        mEmptyText.erase(Text);
    }
    return;
}

/****************-血样孔绘制 ******************/
QFont ConfigureTextstyle(int size)
{
    QFont Textfont;
    Textfont.setFamily("楷体");
    Textfont.setPixelSize(size);
    Textfont.setLetterSpacing(QFont::AbsoluteSpacing, 1);// 设置字符间距
    return Textfont;
}



void Testing::CreatBloodZoneAxisPos()
{
    m_BloodHoleInitUiAxis.clear();
    m_BloodHoleNum.clear();

    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

    QRect bloodRect = ui->widget_Sample_1->rect();
    constexpr  int kSpaceTopY  = 30;
    int BloodZoneHeigh = bloodRect.height()  - kSpaceTopY ;

    int BloodAreasWidth = 0; //血样区界面总宽度
    int SpaceLeft = ui->widget_TestCup_0->geometry().bottomLeft().x(); //距离左边边界
    int TotalRows = 5, TotalCols = 0;

    switch(equipmentKind)
    {
        case KS600:
            TotalCols = 6 ;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
        case KS800:
            TotalCols = 8 ;
            BloodAreasWidth = ui->widget_TestCup_2->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
        case KS1200:
            TotalCols = 12 ;
            BloodAreasWidth = ui->widget_TestCup_3->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
        default:
            TotalCols = 6 ;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
    }
    int oneWidth =0;
    int oneHeight = 0;
    QList<QRect> BloodOnceZoneList_Left;
    QList<QRect> BloodOnceZoneList_Right;
    int drawWidth = (BloodAreasWidth - TotalCols * 5)/TotalCols; //均分血样区宽度 间隔是 5
    for(int n = 0; n < TotalCols; n++)
    {
        int x_ = SpaceLeft+ n*drawWidth + n * 5;
        int y_ = 30;
        QRect rectone(QPoint(x_,y_),QSize(drawWidth,BloodZoneHeigh));
        //QLOG_DEBUG()<<"单个="<<rectone.width()<<"hhh"<<rectone.height();
        oneWidth = rectone.width()/2;
        oneHeight = rectone.height()/5;

        int right_w = drawWidth/2;
        QRect Leftrect(QPoint(x_,30),QSize(right_w,BloodZoneHeigh));
        int right_one = Leftrect.bottomRight().x() ;
        QRect Rightrect(QPoint(right_one,30),QSize(right_w,BloodZoneHeigh));
        BloodOnceZoneList_Left.push_back(Leftrect);
        BloodOnceZoneList_Right.push_back(Rightrect);
    }

    int diameter = 0; //直径
    (oneWidth > oneHeight)? diameter = oneHeight : diameter = oneWidth;
    m_BloodBigRadius = (diameter - 2)/2;
    m_BloodSmallRadius = qAbs(m_BloodBigRadius - 2);
    //QLOG_DEBUG()<<"直径===="<<diameter<<"半"<<m_BloodBigRadius<< BloodOnceZoneList_Left;


    int num =  0;
    for(int n = 0 ; n < BloodOnceZoneList_Left.size(); n++)
    {
        QRect rect_ = BloodOnceZoneList_Left.at(n);
        for(int c = 0 ; c < TotalRows; c++)
        {
           int xpos = rect_.topLeft().x();
           int heightBlood = rect_.height()/TotalRows;
           int ypos = rect_.topLeft().y() + heightBlood * c;
           QRect BloodHole(QPoint(xpos,ypos),QSize(rect_.width(),heightBlood));
           num = 2*c + 10 * n;
           QPoint CenterPos = BloodHole.center();
           m_BloodHoleInitUiAxis.insert(num,CenterPos);
           m_BloodHoleNum.insert(num,QString("%1").arg(num + 1));
        }
    }
    for(int n = 0 ; n < BloodOnceZoneList_Right.size(); n++)
    {
        QRect rect_ = BloodOnceZoneList_Right.at(n);
        for(int c = 0 ; c < TotalRows; c++)
        {
           int xpos = rect_.topLeft().x() + m_BloodBigRadius/3*2;  //右边位置
           int heightBlood = rect_.height()/TotalRows;
           int ypos = rect_.topLeft().y() + heightBlood * c;
           QRect BloodHole(QPoint(xpos,ypos),QSize(rect_.width(),heightBlood));
           num = 2*c+1 + 10 * n;
           QPoint CenterPos(BloodHole.topLeft().x(),BloodHole.center().y());
           m_BloodHoleInitUiAxis.insert(num,CenterPos);
           m_BloodHoleNum.insert(num,QString("%1").arg(num + 1));
        }
    }
    return;
}

void Testing::DrawBloodTopText()
{
    QPainter painter(ui->widget_Sample_1);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);
    QFont font = ConfigureTextstyle(20);
    painter.setFont(font);
    QList<QString> ReminderToptext;
    ReminderToptext<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L";
    int TextHeight = 30;
    int BloodAreasWidth = 0; //血样区界面总宽度
    int SpaceLeft = ui->widget_TestCup_0->geometry().bottomLeft().x(); //距离左边边界
    int TotalCols = 0;
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);
    switch(equipmentKind)
    {
        case KS600:
            TotalCols = 6 ;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() -
                    ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
        case KS800:
            TotalCols = 8 ;
            BloodAreasWidth = ui->widget_TestCup_2->geometry().bottomRight().x() -
                    ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
        case KS1200:
            TotalCols = 12 ;
            BloodAreasWidth = ui->widget_TestCup_3->geometry().bottomRight().x() -
                    ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
        default:
            TotalCols = 6 ;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
    }
    painter.setPen(cglobal::g_TextColorBlack);
    int oneWidth = (BloodAreasWidth - TotalCols*5)/TotalCols;
    for(int n = 0; n < TotalCols; n++)
    {
       int x_ = SpaceLeft+ n*oneWidth + n*5;
       int y_ = 0;
       QRect _Rect(QPoint(x_,y_),QSize(oneWidth,TextHeight));
       painter.drawText(QPoint(_Rect.center().x() - m_BloodBigRadius,_Rect.center().y() + 5), ReminderToptext.at(n));
    }
    return;
}

void Testing::DrawBloodHoleInnerText(const QMap<quint8, QPoint>& BloodHoleMap)
{
	QPainter painter(ui->widget_Sample_1);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                           | QPainter::Qt4CompatiblePainting);
	painter.setFont(ConfigureTextstyle(16));

	static const QMap<quint8, QString> numberMap = {
		{ 0, "1" },
		{ 2, "2" },
		{ 4, "3" },
		{ 1, "4" },
		{ 3, "5" }
	};

	for (auto iter = BloodHoleMap.constBegin(); iter != BloodHoleMap.constEnd(); ++iter) {
		if (iter.key() % 2 == 0) {
			const QPoint& pos = iter.value();
			QPoint textPos(pos.x() - m_BloodBigRadius * 2 + 7, pos.y() + 5);

			auto it = numberMap.find(iter.key() % 5);
			if (it != numberMap.end()) {
				painter.drawText(textPos, it.value());
			}
		}
	}
}

void Testing::UpdateBloodHoleColors(int State, QMap<quint8, QPoint>& MapBloodHole)
{
    QPainter painter(ui->widget_Sample_1);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    QFont font = ConfigureTextstyle(12);
    painter.setFont(font);

    QPen pen;
    pen.setColor(cglobal::g_LineColor);//圆环外圈的颜色
    pen.setWidthF(1);

    // 提前设置渲染提示和画笔，避免在循环中重复设置
    painter.setRenderHint(QPainter::Antialiasing, true);
   


    for (auto iter = MapBloodHole.constBegin(); iter != MapBloodHole.constEnd(); ++iter) {
            const quint8 holeId = iter.key();
            const QPoint& center = iter.value();
            const QString& showText = m_BloodHoleNum[holeId];
            QPalette palette;
			painter.setPen(pen);
            //确定孔的颜色（根据State和奇偶性）
            switch (State) {
                case TUBE_INIT:    palette.setColor(QPalette::Background, cglobal::g_SamllBgmColor); break;
                case TUBE_CHECKED: (holeId % 2 != 0 ) ? palette.setColor(QPalette::Background, cglobal::g_RichBloodColor)
                                                 :palette.setColor(QPalette::Background, cglobal::g_PoorBloodColor);
                break;
                case TUBE_OUTRESULT: palette.setColor(QPalette::Background, cglobal::g_OutResult); break;
                default: break;
            }


			bool isReminderHole = (mReminderTube - 1 == holeId || mReminderTube == holeId);
			painter.setBrush(isReminderHole
				? QBrush(cglobal::g_ReminderBloodHoleColor, Qt::SolidPattern)
				: QBrush(cglobal::g_OuterRingColor, Qt::SolidPattern));
			painter.drawEllipse(center, m_BloodBigRadius, m_BloodBigRadius); //画大圆

            // 3. 绘制小圆（内圈）
            painter.setBrush(QBrush(palette.brush(QPalette::Background)));
            painter.drawEllipse(center, m_BloodSmallRadius, m_BloodSmallRadius);

            QColor textColor = (holeId % 2 == 0) ? cglobal::g_TextColorRoorblood : cglobal::g_TextColorRichblood;
            painter.setPen(textColor);
            if(showText.size() == 1){
                painter.drawText(QPoint(center.x() - 2 , center.y() + 5), showText);
            } else if(showText.size() == 2){
                painter.drawText(QPoint(center.x() - 6, center.y() + 5), showText);
            }else if(showText.size()>= 3){
                painter.drawText(QPoint(center.x() - 11, center.y() + 5), showText);
            }
    }

    DrawBloodHoleInnerText(MapBloodHole); //绘制血样孔内文字和前提醒数字
    return;
}

void Testing::mousePressEvent(QMouseEvent *event)
{
    int mouse_posx = event->x() ;
    int mouse_posy = event->y() ;
    if (event->button() == Qt::RightButton /*&& !cglobal::g_StartTesting*/)
    {
       ClickCanelTask(mouse_posx, mouse_posy);
    }
    return;
}

void Testing::ToggletheReagentPosition(int mouse_posx , int mouse_posy)
{
    auto iter = m_Reagent_Tray.constBegin();
    while(iter != m_Reagent_Tray.constEnd()){
        QPoint posReagent = ui->widget_Reagents->mapToParent(QPoint(5 + iter.value().x(),iter.value().y()));
        int Leftpos =  posReagent.x() - Big_radius;
        int Rightpos = posReagent.x() + Big_radius;
        int Toppos =  posReagent.y() - Big_radius;
        int bottompos = posReagent.y() + Big_radius;
        if((mouse_posx >= Leftpos && mouse_posx <= Rightpos) && (mouse_posy >= Toppos && mouse_posy <= bottompos))
        {
            if (cglobal::g_StartTesting) {
                FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("切换试剂失败", "样本测试中请勿切换试剂位置!");
                return;
            }
            break;
        }
        iter++;
    }
    update();
    return;
}

//点击孔取消任务
void Testing::ClickCanelTask(int mouse_posx , int mouse_posy)
{
    QPoint bloodpos;
    int Cliclhole = -1;
    QList<int> emptytube;
    emptytube.clear();
    int Leftpos,Rightpos,Toppos,bottompos;
    auto iter = m_Blood_Tray_Checked.constBegin();
    while(iter != m_Blood_Tray_Checked.constEnd())
    {
        int left_w = ui->widget_Machine->width() + 20;
        bloodpos = ui->widget_Sample_1->mapToParent(QPoint(left_w + iter.value().x(),iter.value().y()));
        Leftpos =  bloodpos.x() - Big_radius;
        Rightpos = bloodpos.x() + Big_radius;
        Toppos =  bloodpos.y() - Big_radius;
        bottompos = bloodpos.y() + Big_radius;
        if((mouse_posx >= Leftpos && mouse_posx <= Rightpos) && (mouse_posy >= Toppos && mouse_posy <= bottompos))
        {
            if (cglobal::g_StartTesting)
            {
                FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("取消失败", "样本测试中请勿取消测试样本!");
                return;
            }
            Cliclhole = iter.key();
            if(Cliclhole % 2 == 0)   //点击的是血孔号可以取消任务
            {
                QList<int> sample_tasklist; //获取贫血的血样区孔号
                QList<int> sampleidList;
                StructInstance::getInstance()->cancel_sample_task(sample_tasklist,sampleidList);
                clickAnaemiaHoleCancelTestTask(sampleidList); //取消任务点击贫血孔号
            }
            update();
            break;
        }
        iter++;
    }
    update();
    return;
}

void Testing::clickAnaemiaHoleCancelTestTask(QList<int> waitTestSampleId)
{
    CancelTaskConfigure *pCancelwaitTestTask = new CancelTaskConfigure(this, waitTestSampleId);
    connect(pCancelwaitTestTask,&CancelTaskConfigure::makesureCacelIdList,this,[=](QList<int> cancelsampleid)
    {
        QString date = GlobalData::ObatinCreatSampleTime();
        foreach (int cancelwaittestsampleid, cancelsampleid)
        {
            int cancelTubeTotalnum = 0;
            quint8 cancelAnemiaHole = 0; //贫血样孔号
            QList<quint8> CancelEmptyTubeList;
            CancelEmptyTubeList.clear();
            quint8 cancelBackHole = 0; //富血样孔号

            QString sampleid =  GlobalData::groupDateAndID(date,cancelwaittestsampleid);
            FullyAutomatedPlatelets::pinstancesqlData()->deleteTestResultTable(sampleid);
            FullyAutomatedPlatelets::pinstancesqlData()->deleteTestCurveDataTale(sampleid);

            StructInstance::getInstance()->cancelsampletask(cancelwaittestsampleid,cancelTubeTotalnum,cancelAnemiaHole,
                                                            CancelEmptyTubeList,cancelBackHole);

            m_ProTotalTube = m_ProTotalTube - cancelTubeTotalnum;
            FullyAutomatedPlatelets::pinstanceAddsampletest()->_backcancelhole(cancelBackHole);
            CanceltaskbackTubecolor(cancelAnemiaHole, cancelBackHole, CancelEmptyTubeList);

            FullyAutomatedPlatelets::pinstancepatientdata()->cancelSampleResultItem(sampleid);
        }

    });
    pCancelwaitTestTask->setAttribute(Qt::WA_DeleteOnClose);
    pCancelwaitTestTask->exec();
    return;
}

//取消任务孔号
void Testing::CanceltaskbackTubecolor(int PPPHole, int PRPHoles, QList<quint8> emptytubehole)
{

    auto processHole = [this](int hole){
        auto it = m_Blood_Tray_Checked.find(hole);
        if(it != m_Blood_Tray_Checked.end()){
            m_BloodHoleInitUiAxis.insert(it.key(),it.value());
            m_Blood_Tray_Checked.erase(it);

            m_BloodHoleNum[hole] = QString("%1").arg(hole + 1);
        }
    };

    // 处理PPPHole和PRPHoles
    processHole(PPPHole);
    processHole(PRPHoles);

    for(int tube : emptytubehole){
        FullyAutomatedPlatelets::pinstancesqlData()->update_test_tube_status(tube,
                                                                             TESTTUBES_FREETIME,
                                                                             "noone",
                                                                              0);
        // 删除相关映射条目：直接使用find检查
        auto emptyIt = mEmptyTubeAssigned.find(tube);
        if (emptyIt != mEmptyTubeAssigned.end()) {
            mEmptyTubeAssigned.erase(emptyIt);
        }

        auto textIt = mEmptyText.find(tube);
        if (textIt != mEmptyText.end()) {
            mEmptyText.erase(textIt);
        }
    }
    return;
}


void Testing::VectorSelectedBloodAreaTube(quint8 IndexTube,QString Samplenum)
{
   auto itInit = m_BloodHoleInitUiAxis.find(IndexTube);  //直接find
   if (itInit != m_BloodHoleInitUiAxis.end())
   {
        m_Blood_Tray_Checked.insert(itInit.key(), itInit.value());
        m_BloodHoleInitUiAxis.erase(itInit);


        QString numStr = Samplenum.split("-", QString::SkipEmptyParts).last();
        auto TextNum = m_BloodHoleNum.find(IndexTube);
        TextNum.value().swap(numStr);
   }
   return;
}



void Testing::SampleTestingChangInitColor(QPoint BloodPoint ,quint8 FinishChannel)
{
    const quint8 pppHole = BloodPoint.x();
    const quint8 prpHole = BloodPoint.y();
    QLOG_DEBUG()<<"PPP血样孔号:"<<pppHole<<"PRP血样孔号:"<<prpHole;

    // 使用lambda函数处理重复的map操作
    auto processHole = [this](quint8 hole) {
        auto it = m_Blood_Tray_Checked.find(hole);
        if (it != m_Blood_Tray_Checked.end()) {
            m_Blood_Tray_OutResult.insert(it.key(), it.value());
            m_Blood_Tray_Checked.erase(it);
        }
    };

    processHole(pppHole);
    processHole(prpHole);

    // 处理通道显示
    const quint8 completedChannel = FinishChannel - 1;
    DrawChannelProgress(completedChannel, 0);      // 通道显示变0
    Channelreminder.at(completedChannel)->hide();  // 隐藏通道显示信息

	for (auto it = m_TestingSample.begin(); it != m_TestingSample.end(); ) {
		if (it.value() == FinishChannel) {
			it = m_TestingSample.erase(it);
		}
		else {
			++it;
		}
	}
    return;
}

void Testing::ComplBackColorBloodArea()
{
    m_Blood_Tray_OutResult.clear();
    m_Blood_Tray_Checked.clear();
    CreatBloodZoneAxisPos();
    return;
}






/*******************画空试管区***********************/
void  Testing::DrawTrayTestTubeUiAxis(QWidget* pTrayWidget,quint8 IndexTray,quint8 BigRadius,
                                        quint8 SmalleRadius)
{
    QPainter painter(pTrayWidget);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = ConfigureTextstyle(12);
    painter.setFont(font);

    quint8 FirstHole = 0;
    quint8 EndHoleNum = 0;
    QMap<quint8, QPointF>::iterator  TrayTubeFirst;

    switch(IndexTray)
    {
        case EMPTYTRAY_1:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_oneHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_oneHole;
                EndHoleNum =  TestTrayHole::Tray_twoHole;
            }
        break;

        case EMPTYTRAY_2:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_twoHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_twoHole;
                EndHoleNum =  TestTrayHole::Tray_threeHole;
            }
        break;
        case EMPTYTRAY_3:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_threeHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_threeHole;
                EndHoleNum =  TestTrayHole::Tray_fourHole;
            }
        break;
        case EMPTYTRAY_4:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_fourHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_fourHole;
                EndHoleNum =  TestTrayHole::Tray_totalHole;
            }
        break;
        default:break;
    }

    if(TrayTubeFirst == m_Testcups.end())
        return;


    while(TrayTubeFirst != m_Testcups.end())
    {
        QPalette palette;
        painter.setPen(cglobal::g_LineColor); //圆环外圈的颜色
        const quint8 holeNum = TrayTubeFirst.key();

        if (mEmptyTubeAssigned.contains(holeNum)){
            palette.setColor(QPalette::Background, cglobal::g_InnerRing_Checked); //分配任务
        }
        else if (mEmptyTubeAbsorb_Poorblood.contains(holeNum)){
            palette.setColor(QPalette::Background, cglobal::g_PoorBloodColor);
        }
        else if (mEmptyTubeAbsorb_Richblood.contains(holeNum)){
            palette.setColor(QPalette::Background, cglobal::g_RichBloodColor);   //PRP加完血样内圈背景色
        }
        else if (mEmptyTubeClipMoved.contains(holeNum)){
             palette.setColor(QPalette::Background, cglobal::g_OutResult);       //夹走
        }
        else{
            palette.setColor(QPalette::Background, cglobal::g_SamllBgmColor);
        }

        if(mflashingTubeList.contains(holeNum)){
            if (m_blinkState)
                palette.setColor(QPalette::Background, QColor(255 ,255 ,255));
            else
                palette.setColor(QPalette::Background, QColor(255,0,0));
        }
        painter.setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿
        painter.setBrush(QBrush(cglobal::g_OuterRingColor, Qt::SolidPattern)); //设置画刷形式 -- 外圆环的颜色
        painter.drawEllipse(TrayTubeFirst.value(), BigRadius, BigRadius); //画大圆
        painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
        painter.drawEllipse(TrayTubeFirst.value(), SmalleRadius, SmalleRadius); //画小圆
        painter.setPen(cglobal::g_TextColorWhite);
        QString SampleText = mEmptyText[TrayTubeFirst.key()];
        if (SampleText.length() >= 3)
            painter.drawText(QPoint(TrayTubeFirst.value().x() - 9, TrayTubeFirst.value().y() + 5), SampleText);
        else if (SampleText.length() == 2)
            painter.drawText(QPoint(TrayTubeFirst.value().x() - 6, TrayTubeFirst.value().y() + 5), SampleText);
        else if (SampleText.length() == 1)
            painter.drawText(QPoint(TrayTubeFirst.value().x() - 2, TrayTubeFirst.value().y() + 5), SampleText);

        if(EndHoleNum == (FirstHole + 1))
            break;
        TrayTubeFirst++;
        FirstHole++;
    }
    return;
}



//血样区的血样被加到空试管区
void Testing::_RecvBloodSuck2EmptyTube(bool banemia, quint8 IndexTube)
{
    bool findit = mEmptyTubeAssigned.contains(IndexTube);
    if(findit)
    {
        auto it = mEmptyTubeAssigned.find(IndexTube);
        if(banemia)
            mEmptyTubeAbsorb_Poorblood.insert(IndexTube, it.value());
        else
            mEmptyTubeAbsorb_Richblood.insert(IndexTube, it.value());
        mEmptyTubeAssigned.erase(it);
        FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(IndexTube,TESTTUBES_SAMPLED);
    }
    update();
    return;
}

void Testing::giveupSampleShowHole(QList<quint8> holeList){

    for(quint8 hole : holeList){
        if(!mflashingTubeList.contains(hole)){
            mflashingTubeList.append(hole);
        }
    }
}

void Testing::giveupSampleChannelFlash(const bool &isChannelNormal, const quint8 indexChannel){
    Q_ASSERT(indexChannel < m_channelShowsTheProgress.size());
    m_channelShowsTheProgress[indexChannel]->flashingReminder(isChannelNormal);
    QColor colors;
    (isChannelNormal)?  colors.setRgb(0,0,0): colors.setRgb(255,0,0);
    QPalette palette =  Channelreminder[indexChannel]->palette();
    palette.setColor(QPalette::WindowText, colors);
    Channelreminder[indexChannel]->setPalette(palette);
}


//试管被夹走 再同步耗材界面
void Testing::EmptyTubeClipMoved(quint8 IndexTube)
{
    if (mEmptyTubeAbsorb_Poorblood.contains(IndexTube))
    {
        auto emptytube = mEmptyTubeAbsorb_Poorblood.find(IndexTube);
        mEmptyTubeClipMoved.insert(emptytube.key(), emptytube.value());
        emit SynchronizeEmptyTube(emptytube.key());
        mEmptyTubeAbsorb_Poorblood.erase(emptytube);
    }
    if (mEmptyTubeAbsorb_Richblood.contains(IndexTube))
    {
        auto emptytube = mEmptyTubeAbsorb_Richblood.find(IndexTube);
        mEmptyTubeClipMoved.insert(emptytube.key(), emptytube.value());
        emit SynchronizeEmptyTube(emptytube.key());  //同步质控界面试管用掉
        mEmptyTubeAbsorb_Richblood.erase(emptytube);
    }
    FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(IndexTube,TESTTUBES_CLIPPEDAWAY);

    repaint();
    return;
}



//绘制试剂&&坐标
void Testing::plotReagentWellsCoordinates(quint8 totalReagents)
{
    m_Reagent_Tray.clear();
    QPoint ReagentWellpos(0,0);
    for(int i = 0 ; i < totalReagents ; i++)
    {
        (i%2 == 0) ? ReagentWellpos.setX(Big_radius+3) : ReagentWellpos.setX(Big_radius + (2 * Big_radius + 10));
        ReagentWellpos.setY(Top_Bottom_space + Big_radius + (i/2)*(Top_Bottom_space+2*Big_radius));
        m_Reagent_Tray.insert(i,ReagentWellpos);
    }
}

void Testing::showPaintReagents()
{
    QPainter painter(ui->widget_Reagents);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QStringList NameText{"AA","ADP","EPI","COL","RIS"};
    QFont font = ConfigureTextstyle(13);
    painter.setFont(font);
    QMap<quint8,QPoint>::iterator iter = m_Reagent_Tray.begin();
    quint8 index = 0;
    QColor WaitUseReagent(0,0,0);
    while(iter != m_Reagent_Tray.end())
    {
        QPalette palette;
        painter.setPen(cglobal::g_LineColor); // -- 圆环外圈的颜色
        //内圆环背景颜色
         palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor);
        //设置渲染,启动反锯齿
        painter.setRenderHint(QPainter::Antialiasing, true);
        //设置画刷形式 -- 外圆环的颜色
        painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern));
        //画大圆
        painter.drawEllipse(iter.value(),Big_radius,Big_radius);
        //设置画刷为背景色
        painter.setBrush(QBrush(palette.brush(QPalette::Background)));
        //画小圆
        painter.drawEllipse(iter.value(),small_radius,small_radius);

        quint8 indexreag = iter.key()/2 + 1;
        painter.setPen(GlobalData::customCurveColor(indexreag));

        QString name =  NameText.at(index/2);
        if(name.length() >= 3)
            painter.drawText(QPoint(iter.value().x()-11 ,iter.value().y() + 5),name);
        else
            painter.drawText(QPoint(iter.value().x()-8,iter.value().y() + 5),name);
        iter++;
        index++;
    }
    update();
    return;
}

/**清洗剂*/
void Testing::showCleanreagent()
{
    QPainter painter(ui->widget_cleanagent);
    QFont font = ConfigureTextstyle(12);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing,true);
    m_CleanningReagent.setX((ui->widget_cleanagent->width() >> 1));
    m_CleanningReagent.setY((ui->widget_cleanagent->height() >> 1));
    QPalette palette;
    painter.setPen(cglobal::g_LineColor); // -- 圆环外圈的颜色
    palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor); //-----内圆环背景颜色
    painter.setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿
    painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式 -- 外圆环的颜色
    painter.drawEllipse(m_CleanningReagent,TopDraw_Bigradius,TopDraw_Bigradius); //画大圆
    painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
    painter.drawEllipse(m_CleanningReagent,TopDraw_Smallradius,TopDraw_Smallradius); //画小圆
    painter.setPen(cglobal::g_TextColorBlack);
    painter.drawText(QPoint(m_CleanningReagent.x() - 22,m_CleanningReagent.y() + 5),tr("清洗剂S1"));
}


/*显示弃杯孔*/
void Testing::showDiscardTheCup()
{
    QPainter painter(ui->DroptheCup);
    QFont font = ConfigureTextstyle(14);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing,true);
    m_DiscardCupHole.setX((ui->DroptheCup->width() >> 1));
    m_DiscardCupHole.setY((ui->DroptheCup->height() >> 1));
    QPalette palette;
    painter.setPen(cglobal::g_LineColor); // -- 圆环外圈的颜色
    palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor); //-----内圆环背景颜色
    painter.setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿
    painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式 -- 外圆环的颜色
    painter.drawEllipse(m_DiscardCupHole,TopDraw_Bigradius,TopDraw_Bigradius); //画大圆
    painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
    painter.drawEllipse(m_DiscardCupHole,TopDraw_Smallradius,TopDraw_Smallradius); //画小圆
    painter.setPen(QColor(0,0,0));
    painter.drawText(QPoint(m_DiscardCupHole.x()- 20 ,m_DiscardCupHole.y() + 5),tr("弃杯孔"));
    return;
}

/*测试试管显示初始坐标*/
void Testing::CreatTrayTestTubeUiAxis(int indexTray) {
    constexpr quint8 Total_Cols = 6;
    constexpr quint8 Total_Rows = 10;
    constexpr int spaceing = 5;

    // 预定义控件和起始孔映射
    QWidget* trayWidgets[] = {ui->widget_TestCup_0, ui->widget_TestCup_1,
                             ui->widget_TestCup_2, ui->widget_TestCup_3};
    quint8 trayHoles[] = {Tray_oneHole, Tray_twoHole, Tray_threeHole, Tray_fourHole};

    if (indexTray < EMPTYTRAY_1 || indexTray > EMPTYTRAY_4) return;
    int idx = indexTray - EMPTYTRAY_1;
    quint8 FirstIndexHole = trayHoles[idx];
    int AloneWidth = trayWidgets[idx]->width();
    int AloneHeight = trayWidgets[idx]->height();

    const int Width_OnceWidth = AloneWidth / Total_Cols;
    const int Width_OnceHeight = AloneHeight / Total_Rows;
    const int setdiameter = std::min(Width_OnceWidth, Width_OnceHeight);

    m_BigRadius = (setdiameter - spaceing) / 2;
    m_SmalleRadius = std::abs(m_BigRadius - m_BigRadius/7*3);

    //m_Testcups.reserve(FirstIndexHole + Total_Rows * Total_Cols); // 预分配内存

    for (int r = 0; r < Total_Rows; ++r) {
        const int y_base = r * Width_OnceHeight;
        for (int c = 0; c < Total_Cols; ++c) {
            const QPoint center(
                c * Width_OnceWidth + Width_OnceWidth / 2,
                y_base + Width_OnceHeight / 2
            );
            m_Testcups.insert(FirstIndexHole++, center);
        }
    }
}

//重测高度
void Testing::repPrpheight(const QString idnum,bool falgrep){
    emit sendRepPrpheight(idnum,falgrep);
}

