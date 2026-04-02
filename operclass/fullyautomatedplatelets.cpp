
#pragma execution_character_set("utf-8")
#include "fullyautomatedplatelets.h"
#include <QDesktopWidget>
#include "mainwindow.h"
#include "operclass/ccreatedump.h"

FullyAutomatedPlatelets::FullyAutomatedPlatelets(int &argc, char **argv) : QApplication(argc, argv)
{
    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskRect = desktopWidget->availableGeometry();
    double availableScreenX = deskRect.width();
    double availableScreenY = deskRect.height();

    _ready = false;

    this->setOrganizationName("Decawave");
    this->setOrganizationDomain("decawave.com");
    this->setApplicationName("全自动血小板聚集分析系统");

    pmainWindow = new MainWindow();
    pmainWindow->resize(availableScreenX,availableScreenY);

    _mcontroldimming = new controldimming(this);

    _maboutequipment = new AboutMachine();

    _mreminderinfowidget = new Alarm();

    minstrumentConsumables.reset(new QualityControl());
    minquireSqldata.reset(new Inquire_Sql_Info());


    _mtestingwidget = new Testing();

    _mpsetTestproject = new ConfigureProjectItem();

    _maddtestsamplecase = new  Height_Data();

    _msuppilereminder = new instrumentAlarmPrompt(nullptr,0,"");

    _mwritecommand = new ConsumablesWrite();

    _mpsreialport = new SuoweiSerialPort();

    _mpSingleactive = new USB_InitConnect();

    _mLoadingLogfile = new displayLogsText();

    _mAdjustthecoordinates = new CustomPlot();

    _mparsemainboard = new mainControlBoardProtocol();

    _mppatientinfo = new Calibrate();

    _mpobtainModuledata = new Monitor_TrayTest();

    _mequipmentconfig = new MachineSetting();

	_msqldata = new CustomCreatSql();

    _mprintPdf =  new Printthereport();

    _ready = true;

     /*** 调光信号 **/
    qRegisterMetaType<QList<quint8> >("const QList<quint8> ");
    qRegisterMetaType<QMap<quint8, quint8> >("QMap<quint8,quint8>");

    QObject::connect(_mcontroldimming,&controldimming::showDimmingFailedChn,pmainWindow,
                     &MainWindow::recvshowDimmingFailedChn);


    QObject::connect(_mcontroldimming,&controldimming::writemodulLedData,_mpsreialport,
                     &SuoweiSerialPort::slotwrite_instructions_group,
                     Qt::QueuedConnection);


    //主界面显示耗材余量
   //connect(minstrumentConsumables.data(),SIGNAL(SynclimitAlarmtheMainInterface(QMap<quint8,quint8>)),
           // pmainWindow,SLOT(DisplaysConsumablesRemaining(QMap<quint8,quint8>)));

   connect(_msuppilereminder,&instrumentAlarmPrompt::outSideCleanDepleteOne,
            minstrumentConsumables.data(),
           &QualityControl::handleoutSideCleanDepleteOne);



    //测试界面
    QObject::connect(minstrumentConsumables.data(),&QualityControl::SycnMainUiLosserSuppile,
                     pmainWindow,&MainWindow::DisplaySycnMainUiLosserSuppile);

    QObject::connect(minstrumentConsumables.data(),&QualityControl::NoinitialCleaning,
                      pmainWindow,&MainWindow::recvNoinitialCleaning);

    QObject::connect(pmainWindow,&MainWindow::AlarmReminderSound,
                     _mreminderinfowidget,&Alarm::OnOffSound); //警示界面报警开关

    //插入提示文字
    QObject::connect(pmainWindow,&MainWindow::ReminderTextOut,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(_mpsreialport,&SuoweiSerialPort::outArmText,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(_maddtestsamplecase,&Height_Data::ReminderTextOut,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(_mcontroldimming,&controldimming::reminderText,
                     _mreminderinfowidget,&Alarm::InsertText);


    QObject::connect(_mreminderinfowidget,&Alarm::AlarmIconState,
                     pmainWindow,&MainWindow::configReminderIcon);


    //设置界面写命令到串口
    QObject::connect(_mequipmentconfig,&MachineSetting::WriteArryCommand,_mpsreialport,
                     &SuoweiSerialPort::writedataToEquipment,
                     Qt::QueuedConnection);


    //调整模组数据发送到串口
    QObject::connect(_mpobtainModuledata,&Monitor_TrayTest::writemodulData,
                     _mpsreialport,&SuoweiSerialPort::writedataToEquipment,
                     Qt::QueuedConnection);


    //测试时控制开关通道旋转电机
    connect(pmainWindow,&MainWindow::controlmotorrunning,_mpobtainModuledata,
            &Monitor_TrayTest::controlChnMotorRotating);

    //设置界面设置温度
    connect(_mequipmentconfig, &MachineSetting::config_modul_temp,
             _mpobtainModuledata,&Monitor_TrayTest::configModulTEMPvalue,
             Qt::QueuedConnection);


    connect(minquireSqldata.data(),
            &Inquire_Sql_Info::writepdfprint,
            _mprintPdf,
            &Printthereport::slotwritePdf);

    connect(minquireSqldata.data(),
            &Inquire_Sql_Info::_printoutresult,
            _mprintPdf,
            &Printthereport::slotprintoutresult);

    _mprintPdf->_Start();

}

FullyAutomatedPlatelets::~FullyAutomatedPlatelets()
{
    // 1. 断开所有信号槽连接
    disconnectAllConnections();

    // 2. 停止所有运行的线程
    //stopAllThreads();

   // 3. 按照依赖关系逆序析构
   // 先析构UI界面和窗口（最上层）
   QLOG_DEBUG() << "开始析构UI组件...";

   if (_mtestingwidget) {
       delete _mtestingwidget;
       _mtestingwidget = nullptr;
       QLOG_DEBUG() << "已析构测试窗口";
   }

   if (_mppatientinfo) {
       delete _mppatientinfo;
       _mppatientinfo = nullptr;
       QLOG_DEBUG() << "已析构患者信息实例";
   }

   if (_mpsetTestproject) {
       delete _mpsetTestproject;
       _mpsetTestproject = nullptr;
       QLOG_DEBUG() << "已析构测试项目配置";
   }

   if (_maddtestsamplecase) {
       delete _maddtestsamplecase;
       _maddtestsamplecase = nullptr;
       QLOG_DEBUG() << "已析构添加测试样本窗口";
   }

   if (_mequipmentconfig) {
       delete _mequipmentconfig;
       _mequipmentconfig = nullptr;
       QLOG_DEBUG() << "已析构设备配置界面";
   }

   if (_maboutequipment) {
       delete _maboutequipment;
       _maboutequipment = nullptr;
       QLOG_DEBUG() << "已析构关于设备界面";
   }

   if (_mreminderinfowidget) {
       delete _mreminderinfowidget;
       _mreminderinfowidget = nullptr;
       QLOG_DEBUG() << "已析构提醒信息窗口";
   }

   if (_msuppilereminder) {
       delete _msuppilereminder;
       _msuppilereminder = nullptr;
       QLOG_DEBUG() << "已析构耗材提醒界面";
   }

   if (_mLoadingLogfile) {
       delete _mLoadingLogfile;
       _mLoadingLogfile = nullptr;
       QLOG_DEBUG() << "已析构日志加载窗口";
   }

   if (_mAdjustthecoordinates) {
       delete _mAdjustthecoordinates;
       _mAdjustthecoordinates = nullptr;
       QLOG_DEBUG() << "已析构坐标调整界面";
   }

    //析构业务逻辑组件
    QLOG_DEBUG() << "开始析构业务逻辑组件...";

    if(!minstrumentConsumables.isNull())
		minstrumentConsumables.reset();
    QLOG_DEBUG() << "已析构仪器耗材管理";
  

   if (_mprintPdf) {
       delete _mprintPdf;
       _mprintPdf = nullptr;
       QLOG_DEBUG() << "已析构PDF打印组件";
   }

   if (_mcontroldimming) {
       delete _mcontroldimming;
       _mcontroldimming = nullptr;
       QLOG_DEBUG() << "已析构调光控制组件";
   }

   if (_mpobtainModuledata) {
       delete _mpobtainModuledata;
       _mpobtainModuledata = nullptr;
       QLOG_DEBUG() << "已析构模块数据获取组件";
   }

   if (!minquireSqldata.isNull()) {
       minquireSqldata.reset();
       QLOG_DEBUG() << "已析构数据查询组件";
   }

   if (_mpSingleactive) {
       delete _mpSingleactive;
       _mpSingleactive = nullptr;
       QLOG_DEBUG() << "已析构单激活组件";
   }

   if (_mparsemainboard) {
       delete _mparsemainboard;
       _mparsemainboard = nullptr;
       QLOG_DEBUG() << "已析构主板解析组件";
   }

   if (pmainWindow) {
       delete pmainWindow;
       pmainWindow = nullptr;
	   QLOG_DEBUG() << "已析构主窗口";
   }

   // 5. 析构线程对象（确保线程已停止）
   QLOG_DEBUG() << "开始析构线程对象...";

   if (_mwritecommand) {
       delete _mwritecommand;
       _mwritecommand = nullptr;
       QLOG_DEBUG() << "已析构写入命令线程";
   }

   if (_mpsreialport) {
       delete _mpsreialport;
       _mpsreialport = nullptr;
       QLOG_DEBUG() << "已析构串口USB线程";
   }

   // 6. 最后析构数据持久层组件
   QLOG_DEBUG() << "开始析构数据层组件...";

   if (_msqldata) {
       delete _msqldata;
       _msqldata = nullptr;
       QLOG_DEBUG() << "已析构数据库实例";
   }

#ifdef Q_OS_WIN
   CCreateDump::Instance()->Del_Instance();
   QLOG_DEBUG() << "已清理Windows dump实例";
#endif

   QLOG_DEBUG() << "FullyAutomatedPlatelets 析构完成";
}


void FullyAutomatedPlatelets::disconnectAllConnections()
{
    QLOG_DEBUG() << "开始断开所有信号槽连接";

    // 获取所有需要断开连接的对象
    QVector<QObject*> objects = {
        minstrumentConsumables.data(),
        _mtestingwidget,
        _mppatientinfo,
        _msqldata,
        _mpsreialport,
        minquireSqldata.data(),
        _mprintPdf,
        _mpsetTestproject,
        _maddtestsamplecase,
        _mequipmentconfig,
        _mpobtainModuledata,
        _mcontroldimming,
        _maboutequipment,
        _mreminderinfowidget,
        _msuppilereminder,
        _mwritecommand,
        _mpSingleactive,
        _mLoadingLogfile,
        _mAdjustthecoordinates,
        _mparsemainboard,
        pmainWindow
    };

    int totalDisconnections = 0;

    // 断开所有对象的连接
    for (QObject* obj : objects) {
        if (obj) {
            int count = disconnect(obj, nullptr, nullptr, nullptr);
            if (count > 0) {
                QLOG_DEBUG() << "断开" << obj->metaObject()->className()
                           << "对象的" << count << "个连接";
                totalDisconnections += count;
            }
        }
    }

    // 断开与this对象相关的所有连接
    int selfDisconnections = disconnect(this, nullptr, nullptr, nullptr);
    if (selfDisconnections > 0) {
        QLOG_DEBUG() << "断开当前对象的" << selfDisconnections << "个连接";
        totalDisconnections += selfDisconnections;
    }

    QLOG_DEBUG() << "总共断开" << totalDisconnections << "个信号槽连接";
}



FullyAutomatedPlatelets *FullyAutomatedPlatelets::instance()
{
    return qobject_cast<FullyAutomatedPlatelets *>(QCoreApplication::instance());
}

MainWindow *FullyAutomatedPlatelets::mainWindow()
{
    return instance()->pmainWindow;
}

controldimming *FullyAutomatedPlatelets::pinstancedimming()
{
    return instance()->_mcontroldimming;
}


AboutMachine *FullyAutomatedPlatelets::paboutinstance()
{
    return instance()->_maboutequipment;
}


Alarm *FullyAutomatedPlatelets::pinstanceinfowidget()
{
    return  instance()->_mreminderinfowidget;
}

QualityControl *FullyAutomatedPlatelets::pinstanceinstrument()
{
    return instance()->minstrumentConsumables.data();
}

Testing *FullyAutomatedPlatelets::pinstanceTesting()
{
    return instance()->_mtestingwidget;
}

ConfigureProjectItem *FullyAutomatedPlatelets::pinstanceTestproject()
{
    return instance()->_mpsetTestproject;
}

Height_Data *FullyAutomatedPlatelets::pinstanceAddsampletest()
{
    return instance()->_maddtestsamplecase;
}

instrumentAlarmPrompt* FullyAutomatedPlatelets::pinstancesuppilereminder()
{
    return instance()->_msuppilereminder;
}

ConsumablesWrite* FullyAutomatedPlatelets::pinstanceWirteBoard()
{
    return instance()->_mwritecommand;
}

SuoweiSerialPort* FullyAutomatedPlatelets::pinstanceserialusb()
{
    return instance()->_mpsreialport;
}

USB_InitConnect* FullyAutomatedPlatelets::pinstanceSingleactive()
{
    return instance()->_mpSingleactive;
}

displayLogsText* FullyAutomatedPlatelets::pinstanceLogfile()
{
    return instance()->_mLoadingLogfile;
}

CustomPlot* FullyAutomatedPlatelets::pinstanceadjustcoordinates()
{
    return instance()->_mAdjustthecoordinates;
}

mainControlBoardProtocol* FullyAutomatedPlatelets::pinstanceMainboarddata()
{
    return instance()->_mparsemainboard;
}

Calibrate* FullyAutomatedPlatelets::pinstancepatientdata()
{
    return instance()->_mppatientinfo;
}


Monitor_TrayTest* FullyAutomatedPlatelets::pinstanceobtainModuledata()
{
    return instance()->_mpobtainModuledata;
}

MachineSetting* FullyAutomatedPlatelets::pinstanceequipmentconfig()
{
    return instance()->_mequipmentconfig;
}

CustomCreatSql* FullyAutomatedPlatelets::pinstancesqlData()
{
	return instance()->_msqldata;
}

Inquire_Sql_Info* FullyAutomatedPlatelets::pinstanceInquiredata()
{
    return instance()->minquireSqldata.data();
}

Printthereport* FullyAutomatedPlatelets::pinstancePrintPdf()
{
    return instance()->_mprintPdf;
}
