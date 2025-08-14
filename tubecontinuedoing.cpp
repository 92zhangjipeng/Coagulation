#include  "loginui.h"
#include  "tubecontinuedoing.h"
#include  "ui_tubecontinuedoing.h"
#include  "QsLog/include/QsLog.h"
#include  "testing.h"
#include  "genericfunctions.h"

#include <operclass/fullyautomatedplatelets.h>


TubeContinueDoing::TubeContinueDoing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TubeContinueDoing)
{
    ui->setupUi(this);
    this->setWindowTitle("调试测试");
    bgmcColor.setRgb(139, 137, 137);
    setWindowFlags(Qt::FramelessWindowHint);
    QUIUtils::QLabeldisplayIcon(ui->label_nametitle,":/Picture/Login.png","调试测试",6);
    QStringList SelReagent;
    SelReagent.append("AA");
    SelReagent.append("ADP");
    SelReagent.append("EPI");
    SelReagent.append("COL");
    SelReagent.append("RIS");
    ui->comboBox_selReagent->addItems(SelReagent);
    ui->comboBox_selReagent->setEditable(false);
    QStringList Channellist;
    Channellist<<"通道1"<<"通道2"<<"通道3"<<"通道4"<<"通道5"<<"通道6"<<"通道7"<<"通道8"<<"通道9"<<"通道10"<<"通道11"<<"通道12";
    ui->comboBox_Channel->addItems(Channellist);
    ui->comboBox_Channel->setEditable(false);
    QStringList absorbReagentlist;
    absorbReagentlist<<"2分之一空气"<<"归0"<<"2分之一空气+余量"<<"余量";
    ui->comboBox_falsesuck->addItems(absorbReagentlist);
    ui->comboBox_falsesuck->setEditable(false);

    QStringList suckhole;
    suckhole<<"0"<<"2"<<"4"<<"6"<<"8";
    ui->comboBox_suck_hole->addItems(suckhole);

    AllZaxisBack();
}

TubeContinueDoing::~TubeContinueDoing()
{
    delete ui;
}
void TubeContinueDoing::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = event->pos();
    }
}
void TubeContinueDoing::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() - mouseStartPoint;
        if (event->globalY() > desktopRc.height())
        {
            curPoint.setY(desktopRc.height() - mouseStartPoint.y());
        }
        move(curPoint);
    }
    QWidget::mouseMoveEvent(event);
}
void TubeContinueDoing::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}
void TubeContinueDoing::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = bgmcColor;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}
void TubeContinueDoing::closeEvent(QCloseEvent *event)
{
    event->accept();
    close();
    QLOG_TRACE()<<"关闭提示窗口";
}

void TubeContinueDoing::AllZaxisBack()
{
    mStarArry.clear();
    quint8 conde_num = 0;
    QByteArray back_z_hands = Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotortongs,0,TRAY_TESTCHANNEL,conde_num,false);
    mStarArry.push_back(back_z_hands);
    QByteArray back_z_reagentneedle = Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorReagent,0,TRAY_TESTCHANNEL,conde_num,false);
    mStarArry.push_back(back_z_reagentneedle);
    QByteArray back_z_bloodNeedle = Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorBlood,0,TRAY_TESTCHANNEL,conde_num,false);
    mStarArry.push_back(back_z_bloodNeedle);
}

QByteArrayList TubeContinueDoing::creatAbsorbReagentCommand()
{
    AllZaxisBack();
    quint8 conde_num = mStarArry.size() % 255;
    int milliliter = ui->spinBox_ReagentVol->value();   //(int)INI_File().GetLearnReagentvolume();  //吸试剂的量 ul
    double AbsorbRatio = 0;
    int falilinquehigh = INI_File().GetFailedReagentsLinqueHigh();
    QVector<double> MaxRatio;
    for(int i = 0 ; i < 5; i++)
    {
       MaxRatio.push_back(INI_File().getTypesReagentSuckRatio(i+1));
    }
    auto maxvalue = std::max_element(std::begin(MaxRatio), std::end(MaxRatio));
    AbsorbRatio = *maxvalue;
    (AbsorbRatio == 0.00)? AbsorbRatio = 1 : AbsorbRatio += 0;
    int currindex = ui->comboBox_selReagent->currentIndex() ;
    bool bUesdAsideLoc = false; //预留位试剂
    QPoint ReagentPosLoc(0,0);

    int SuckAirVol = ui->spinBox_suckAir->value();

    //获取准备吸取的试剂的余量
    bool  bReagentUsedEnd = GenericFunctions::ReagentUsingFinished(currindex);
    //判断试剂余量&&使用预留试剂
    GenericFunctions::Determinethereagentbalance(bUesdAsideLoc,currindex,bReagentUsedEnd,ReagentPosLoc);

    QByteArray OnceCommand;
    OnceCommand = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(ReagentPosLoc, ReagentNeddle , TRAY_TESTCHANNEL,conde_num);
    mStarArry.push_back(OnceCommand);

    OnceCommand = Testing::m_TaskDll->SmallBenActive(true,SuckAirVol,DIS_WASHES_PUMPS,conde_num,0); //吸一段空气
    mStarArry.push_back(OnceCommand);

    OnceCommand = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(ZMotorReagent, falilinquehigh,TRAY_TESTCHANNEL,
                                                                 conde_num,true,falilinquehigh,false,GRIPPERNORMAL); //(吸试剂液面探测)
    mStarArry.push_back(OnceCommand);
    //SMALL_BEN_INHALE_ARI

    int suck_ul = SuckAirVol + milliliter * (SMALL_BEN_RATIO* AbsorbRatio) + (milliliter * ui->doubleSpinBox_addratio->value()*SMALL_BEN_RATIO * AbsorbRatio);
    QLOG_DEBUG()<<"吸取量:"<<milliliter<<"*"<<"720"<<"转换比例"<<AbsorbRatio<<"+"<<720;
    QString str = QString("吸试剂量(步数): %1*%2*%3+(%4*%5*%6*%7) = %8").arg(milliliter).arg(SMALL_BEN_RATIO).arg(AbsorbRatio).arg(milliliter).
            arg(ui->doubleSpinBox_addratio->value()).arg(SMALL_BEN_RATIO).arg(AbsorbRatio).arg(suck_ul);
    ui->label_suck->setText(str);

    (suck_ul > 7200)? suck_ul = 0 : suck_ul+= 0;

    OnceCommand = Testing::m_TaskDll->SmallBenActive(true,suck_ul,DIS_WASHES_PUMPS,conde_num,0);
    mStarArry.push_back(OnceCommand);


    OnceCommand = Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorReagent,0,TRAY_TESTCHANNEL,conde_num,false);
    mStarArry.push_back(OnceCommand);
    //吐试剂
    int Channelnum = ui->comboBox_Channel->currentIndex();
    int Downmm = ui->spinBox_spitheigh->value(); //试剂针在空试管区下降高度
    int absorbIndex = ui->comboBox_falsesuck->currentIndex();
    int absorbValue = 0;
    switch(absorbIndex)
    {
        case 0:  absorbValue = SuckAirVol/2; break;
        case 1:  absorbValue = 0; break;
        case 2:  absorbValue = SuckAirVol/2 + ((milliliter * ui->doubleSpinBox_addratio->value()*SMALL_BEN_RATIO * AbsorbRatio)); break;
        case 3:  absorbValue = (milliliter * ui->doubleSpinBox_addratio->value()*SMALL_BEN_RATIO * AbsorbRatio); break;
        default:
            absorbValue = SuckAirVol/2;
        break;
    }
    QString absorbstr = QString("吐试剂量(步数): %1").arg(absorbValue);
    ui->label_absorb->setText(absorbstr);

	QPoint spitreagentposChannel(0,0);
    SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX, Channelnum, spitreagentposChannel);

    mStarArry.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(spitreagentposChannel, ReagentNeddle , TRAY_TESTCHANNEL,conde_num));
    mStarArry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(ZMotorReagent,Downmm,TRAY_TESTCHANNEL,
                                                                       conde_num,false,Downmm,false,GRIPPERNORMAL));
    mStarArry.push_back(Testing::m_TaskDll->SmallBenActive(false,absorbValue,DIS_WASHES_PUMPS,conde_num,0));
    mStarArry.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorReagent,0,TRAY_TESTCHANNEL,conde_num,false));



    QByteArrayList commandBack = mStarArry;
    return commandBack;
}

void TubeContinueDoing::on_toolButton_runTest_clicked()
{
    QByteArrayList  SendTestCommand = creatAbsorbReagentCommand();
    emit scynTestCommand(0,SendTestCommand);
    return;
}

QByteArrayList TubeContinueDoing::creatAnaemiaCommand()
{
    AllZaxisBack();
    quint8 conde_num =  mStarArry.size() % 255;
    int LeveldetectionFailed = INI_File().GetFailedLinqueHigh(); /*液面探测失败下降高度*/
    int suckmiter = ui->spinBox_PPPVol->value();//INI_File().GetLearnSamplevolume();          /*吸贫血样本量*/
    double   proportionalValue   = INI_File().GetPPPConversionScale();/*转换比例*/

    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QPoint BloodAreaPos(0,0); //血样针到血样区的坐标
    SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,ui->spinBox_Hole->value(),BloodAreaPos);


    /*XY 移动到血样区对应位置*/
    QByteArray EncodedArray = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(BloodAreaPos, BloodNddle , TRAY_BLOOD,conde_num);
    mStarArry.push_back(EncodedArray);

    /*吸一段空气*/
    EncodedArray = Testing::m_TaskDll->BigBenActive(true, BIG_BEN_INHALE_ARI,conde_num,DIS_WASHES_PUMPS,0); //true 吸 false 吐
    mStarArry.push_back(EncodedArray);

    /*下血样针液面探测吸贫血*/
    EncodedArray = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(ZMotorBlood,LeveldetectionFailed,TRAY_BLOOD,
                                                                  conde_num,true,LeveldetectionFailed,false,GRIPPERNORMAL);
    mStarArry.push_back(EncodedArray);

    //吸贫血量 + 空气量
    int suck_ul = suckmiter * proportionalValue + BIG_BEN_INHALE_ARI;
    EncodedArray = Testing::m_TaskDll->BigBenActive(true,suck_ul,conde_num,DIS_WASHES_PUMPS,0);
    mStarArry.push_back(EncodedArray);

    EncodedArray = Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorBlood,0,TRAY_BLOOD,conde_num,false);
    mStarArry.push_back(EncodedArray);

    //吐PPP 到试管
    int emptyAreaDownmm = INI_File().GetEmptyTubeDownHigh();                /*血样针在空试管区下降高度*/

    QPoint EmptytubeAreaPos(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,ui->spinBox_emptyHole->value(),MOTOR_BLOOD_INDEX,EmptytubeAreaPos);



    QByteArray spitOutAnemia = Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(EmptytubeAreaPos, BloodNddle , TRAY_TESTENPTY,conde_num);
    mStarArry.push_back(spitOutAnemia);

    spitOutAnemia = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(ZMotorBlood,emptyAreaDownmm,TRAY_TESTENPTY,
                                                                   conde_num,false,emptyAreaDownmm,false,GRIPPERNORMAL);
    mStarArry.push_back(spitOutAnemia);

    int spit_ul =  BIG_BEN_INHALE_ARI/2; //吐出贫血剩一半空气
    spitOutAnemia = Testing::m_TaskDll->BigBenActive(false,spit_ul,conde_num,DIS_WASHES_PUMPS,0);
    mStarArry.push_back(spitOutAnemia);

    spitOutAnemia = Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorBlood,0,TRAY_TESTENPTY,conde_num,false);
    mStarArry.push_back(spitOutAnemia);

    QByteArrayList SuckAction = mStarArry;
    return SuckAction;
}

void TubeContinueDoing::on_toolButton_close_clicked()
{
    close();
}

void TubeContinueDoing::on_toolButton_PPPok_clicked()
{
    QByteArrayList  SendTestCommand = creatAnaemiaCommand();
    emit scynTestCommand(1,SendTestCommand);
    return;
}

//构造曲线数据
void TubeContinueDoing::on_toolButton_cruveCreat_clicked()
{
    int suck_hole,suck_time,suck_vol;
    suck_hole =  ui->comboBox_suck_hole->currentText().toInt();
    suck_time = ui->spinBox_suck_times->value();
    suck_vol = ui->spinBox_suck_vol->value();

    AllZaxisBack();
    quint8   conde_num                   =  mStarArry.size() % 255;
    int      LeveldetectionFailed        =  INI_File().GetFailedLinqueHigh(); /*液面探测失败下降高度*/
    double   proportionalValue           =  INI_File().GetPPPConversionScale();  /*转换比例*/
    int      down_spit                   =  INI_File().GetEmptyTubeDownHigh();   /*血样针在空试管区下降高度*/

    QPoint spit_pos(0,0);

    //血样针到血样区的坐标
    QPoint suck_pos(0,0);
    SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,suck_hole,suck_pos);

    QList<quint8> spit_holeList;
	quint8 index_ = 0;
    for(int n = 1; n <= suck_time; n++)
    {
        if(n%2 == 0)
			index_ = 10*(n/2) + suck_hole;
        else
			index_ = 10 * ((n-1)/2) + 1 + suck_hole;
		spit_holeList.push_back(index_);
    }
    QLOG_DEBUG()<<"吐样孔号"<<spit_holeList<<endl;
    auto iter_ = spit_holeList.begin();
    while(iter_ != spit_holeList.end()){
        quint8 spithole = *iter_;
        mStarArry.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(suck_pos, BloodNddle , TRAY_BLOOD,conde_num)); /*XY 移动到血样区对应位置*/

        mStarArry.push_back(Testing::m_TaskDll->BigBenActive(true, BIG_BEN_INHALE_ARI,conde_num,DIS_WASHES_PUMPS,0));//吸一段空气true 吸 false 吐

        /*下血样针液面探测吸贫血*/
        mStarArry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(ZMotorBlood,LeveldetectionFailed,TRAY_BLOOD,
                                                                           conde_num,true,LeveldetectionFailed,false,GRIPPERNORMAL));

        //吸贫血量 + 空气量
        int suck_ul = suck_vol * proportionalValue + BIG_BEN_INHALE_ARI;
        QString printText = QString("吸样量:%1*%2+720 = %3").arg(suck_vol).arg(proportionalValue).arg(suck_ul);
        QLOG_WARN()<<printText<<endl;
        //FullyAutomatedPlatelets::pinstanceinfowidget()->InsertText(USERACTIONS,printText);

        mStarArry.push_back(Testing::m_TaskDll->BigBenActive(true,suck_ul,conde_num,DIS_WASHES_PUMPS,0));

        mStarArry.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorBlood,0,TRAY_BLOOD,conde_num,false));

        //吐出的孔号坐标
        SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,spithole,spit_pos);

        mStarArry.push_back(Testing::m_TaskDll->DLL_XYMoveSpecifiedPosition(spit_pos, BloodNddle , TRAY_TESTENPTY,conde_num));

        mStarArry.push_back(Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(ZMotorBlood,down_spit,TRAY_TESTENPTY,
                                                                           conde_num,false,down_spit,false,GRIPPERNORMAL));

        int spit_ul =  0; //吐出贫血剩一半空气
        mStarArry.push_back(Testing::m_TaskDll->BigBenActive(false,spit_ul,conde_num,DIS_WASHES_PUMPS,0));

        mStarArry.push_back(Testing::m_TaskDll->DLL_ZAxis_Reset(ZMotorBlood,0,TRAY_TESTENPTY,conde_num,false));

        iter_++;
    }
    emit  this->sendcode(mStarArry);
    return;
}
