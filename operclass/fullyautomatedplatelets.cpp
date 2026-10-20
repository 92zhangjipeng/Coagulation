#include "fullyautomatedplatelets.h"
#include <QDesktopWidget>
#include "mainwindow.h"
#include "operclass/ccreatedump.h"
#include "ini_file.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

FullyAutomatedPlatelets::FullyAutomatedPlatelets(int &argc, char **argv) : QApplication(argc, argv)
{
    // 登录前就要创建大量界面/串口对象，先给等待光标，避免“假死”感
    setOverrideCursor(Qt::WaitCursor);

    QDesktopWidget* desktopWidget = QApplication::desktop();
    QRect deskRect = desktopWidget->availableGeometry();
    double availableScreenX = deskRect.width();
    double availableScreenY = deskRect.height();

    _ready = false;

    this->setOrganizationName("Decawave");
    this->setOrganizationDomain("decawave.com");
    this->setApplicationName(QString::fromUtf8("全自动血小板聚集分析系统"));

    pmainWindow = new MainWindow();
    pmainWindow->resize(availableScreenX,availableScreenY);

    mcontroldimming = new controldimming(this);

    _mreminderinfowidget = new Alarm();

    minstrumentConsumables.reset(new QualityControl());
    minquireSqldata.reset(new Inquire_Sql_Info());


    _mtestingwidget = new Testing();

    //添加样本
    maddtestsamplecase = new  Height_Data();
    //配置项目
    mpsetTestproject = new ConfigureProjectItem();

    _msuppilereminder = new instrumentAlarmPrompt(nullptr,0,"");

    _mwritecommand = new ConsumablesWrite();

    _mpsreialport = new SuoweiSerialPort();

    _mpSingleactive = new USB_InitConnect();

    _mLoadingLogfile = new displayLogsText();

    _mAdjustthecoordinates = nullptr;

    _mparsemainboard = new mainControlBoardProtocol();

    _mppatientinfo = new Calibrate();

    _mpobtainModuledata = new Monitor_TrayTest();

    _mequipmentconfig = new MachineSetting();

	_msqldata = new CustomCreatSql();

    _ready = true;
    restoreOverrideCursor();

     /*** 调光信号 **/
    qRegisterMetaType<QList<quint8> >("const QList<quint8> ");
    qRegisterMetaType<QMap<quint8, quint8> >("QMap<quint8,quint8>");

    QObject::connect(mcontroldimming,&controldimming::showDimmingFailedChn,pmainWindow,
                     &MainWindow::recvshowDimmingFailedChn,
                     Qt::QueuedConnection);

    QObject::connect(mcontroldimming, &controldimming::requestDimmingProgress,
                     pmainWindow, &MainWindow::dimmingprogress,
                     Qt::QueuedConnection);

    QObject::connect(mcontroldimming, &controldimming::requestSmartCupClear,
                     pmainWindow, &MainWindow::recvRequestSmartCupClear,
                     Qt::QueuedConnection);

    QObject::connect(mcontroldimming,&controldimming::writemodulLedData,_mpsreialport,
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

    QObject::connect(maddtestsamplecase,&Height_Data::ReminderTextOut,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(maddtestsamplecase, &Height_Data::requestAltimeterCamera,
                     pmainWindow, &MainWindow::setAltimeterCameraPower,
                     Qt::QueuedConnection);
    pmainWindow->bindAltimeterVision(maddtestsamplecase->vision());
    Q_EMIT maddtestsamplecase->requestAltimeterCamera(INI_File().GetWholeBloodModel());

    QObject::connect(mcontroldimming,&controldimming::reminderText,
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
             Qt::DirectConnection);





}

FullyAutomatedPlatelets::~FullyAutomatedPlatelets()
{
    // 1. 先断开信号，避免析构过程中队列信号打到半销毁对象
    disconnectAllConnections();
    _ready = false;

   // 3. 按照依赖关系逆序析构
   // 先析构UI界面和窗口（最上层）
   QLOG_DEBUG() << "start destroying UI";

   if (_mtestingwidget) {
       delete _mtestingwidget;
       _mtestingwidget = nullptr;
       QLOG_DEBUG() << "destroyed testing widget";
   }

   if (_mppatientinfo) {
       delete _mppatientinfo;
       _mppatientinfo = nullptr;
       QLOG_DEBUG() << "destroyed patient info";
   }

   if (mpsetTestproject) {
       delete mpsetTestproject;
       mpsetTestproject = nullptr;
       QLOG_DEBUG() << "destroyed project config";
   }

   if (maddtestsamplecase) {
       delete maddtestsamplecase;
       maddtestsamplecase = nullptr;
       QLOG_DEBUG() << "destroyed add-sample window";
   }

   if (_mequipmentconfig) {
       delete _mequipmentconfig;
       _mequipmentconfig = nullptr;
       QLOG_DEBUG() << "destroyed equipment config";
   }


   if (_mreminderinfowidget) {
       delete _mreminderinfowidget;
       _mreminderinfowidget = nullptr;
       QLOG_DEBUG() << "destroyed alarm widget";
   }

   if (_msuppilereminder) {
       delete _msuppilereminder;
       _msuppilereminder = nullptr;
       QLOG_DEBUG() << "destroyed consumable prompt";
   }

   if (_mLoadingLogfile) {
       delete _mLoadingLogfile;
       _mLoadingLogfile = nullptr;
       QLOG_DEBUG() << "destroyed log window";
   }

   if (_mAdjustthecoordinates) {
       delete _mAdjustthecoordinates;
       _mAdjustthecoordinates = nullptr;
       QLOG_DEBUG() << "destroyed coordinate window";
   }

    //析构业务逻辑组件
    QLOG_DEBUG() << "start destroying logic components";

    if(!minstrumentConsumables.isNull())
		minstrumentConsumables.reset();
    QLOG_DEBUG() << "destroyed consumable manager";
  



   if (mcontroldimming) {
       //delete mcontroldimming;
       mcontroldimming = nullptr;
       QLOG_DEBUG() << "destroyed dimming control";
   }

   if (_mpobtainModuledata) {
       delete _mpobtainModuledata;
       _mpobtainModuledata = nullptr;
       QLOG_DEBUG() << "destroyed module data";
   }

   if (!minquireSqldata.isNull()) {
       minquireSqldata.reset();
       QLOG_DEBUG() << "destroyed sql inquire";
   }

   if (_mpSingleactive) {
       delete _mpSingleactive;
       _mpSingleactive = nullptr;
       QLOG_DEBUG() << "destroyed usb init";
   }

   if (_mparsemainboard) {
       delete _mparsemainboard;
       _mparsemainboard = nullptr;
       QLOG_DEBUG() << "destroyed mainboard parser";
   }

   if (pmainWindow) {
       delete pmainWindow;
       pmainWindow = nullptr;
	   QLOG_DEBUG() << "destroyed main window";
   }

   // 5. 析构线程对象（确保线程已停止）
   QLOG_DEBUG() << "start destroying threads";

   if (_mwritecommand) {
       delete _mwritecommand;
       _mwritecommand = nullptr;
       QLOG_DEBUG() << "destroyed write-command thread";
   }

   if (_mpsreialport) {
       delete _mpsreialport;
       _mpsreialport = nullptr;
       QLOG_DEBUG() << "destroyed serial thread";
   }

   // 6. 最后析构数据持久层组件
   QLOG_DEBUG() << "start destroying data layer";

   if (_msqldata) {
       delete _msqldata;
       _msqldata = nullptr;
       QLOG_DEBUG() << "destroyed sql instance";
   }

#ifdef Q_OS_WIN
   CCreateDump::Instance()->Del_Instance();
   QLOG_DEBUG() << "cleared Windows dump instance";
#endif

   QLOG_DEBUG() << "FullyAutomatedPlatelets destroyed";
}


void FullyAutomatedPlatelets::disconnectAllConnections()
{
	QLOG_DEBUG() << "start disconnecting signals";

	int totalDisconnections = 0;

	// 获取所有需要断开连接的对象（只检查指针有效性）
	QList<QObject*> objects;
	if (minstrumentConsumables) objects << minstrumentConsumables.data();
	if (_mtestingwidget) objects << _mtestingwidget;
	if (_mppatientinfo) objects << _mppatientinfo;
	if (_msqldata) objects << _msqldata;
	if (_mpsreialport) objects << _mpsreialport;
	if (minquireSqldata) objects << minquireSqldata.data();

    if (mpsetTestproject) objects << mpsetTestproject;
    if (maddtestsamplecase) objects << maddtestsamplecase;
	if (_mequipmentconfig) objects << _mequipmentconfig;
	if (_mpobtainModuledata) objects << _mpobtainModuledata;
    if (mcontroldimming) objects << mcontroldimming;

	if (_mreminderinfowidget) objects << _mreminderinfowidget;
	if (_msuppilereminder) objects << _msuppilereminder;
	if (_mwritecommand) objects << _mwritecommand;
	if (_mpSingleactive) objects << _mpSingleactive;
	if (_mLoadingLogfile) objects << _mLoadingLogfile;
	if (_mAdjustthecoordinates) objects << _mAdjustthecoordinates;
	if (_mparsemainboard) objects << _mparsemainboard;
	if (pmainWindow) objects << pmainWindow;

	// 安全断开连接（含 Widget，避免退出期信号打到半销毁对象）
	for (QObject* obj : objects) {
		if (obj) {
			totalDisconnections += QObject::disconnect(obj, nullptr, nullptr, nullptr);
		}
	}

	// 断开当前对象的连接
	totalDisconnections += QObject::disconnect(this, nullptr, nullptr, nullptr);

	QLOG_DEBUG() << "disconnected slots:" << totalDisconnections;
}



FullyAutomatedPlatelets *FullyAutomatedPlatelets::instance()
{
    return qobject_cast<FullyAutomatedPlatelets *>(QCoreApplication::instance());
}

MainWindow *FullyAutomatedPlatelets::mainWindow()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->pmainWindow : nullptr;
}

controldimming *FullyAutomatedPlatelets::pinstancedimming()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->mcontroldimming : nullptr;
}





Alarm *FullyAutomatedPlatelets::pinstanceinfowidget()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mreminderinfowidget : nullptr;
}

QualityControl *FullyAutomatedPlatelets::pinstanceinstrument()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->minstrumentConsumables.data() : nullptr;
}

Testing *FullyAutomatedPlatelets::pinstanceTesting()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mtestingwidget : nullptr;
}

ConfigureProjectItem *FullyAutomatedPlatelets::pinstanceTestproject()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->mpsetTestproject : nullptr;
}

Height_Data *FullyAutomatedPlatelets::pinstanceAddsampletest()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->maddtestsamplecase : nullptr;
}

instrumentAlarmPrompt* FullyAutomatedPlatelets::pinstancesuppilereminder()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_msuppilereminder : nullptr;
}

ConsumablesWrite* FullyAutomatedPlatelets::pinstanceWirteBoard()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mwritecommand : nullptr;
}

SuoweiSerialPort* FullyAutomatedPlatelets::pinstanceserialusb()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mpsreialport : nullptr;
}

USB_InitConnect* FullyAutomatedPlatelets::pinstanceSingleactive()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mpSingleactive : nullptr;
}

displayLogsText* FullyAutomatedPlatelets::pinstanceLogfile()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mLoadingLogfile : nullptr;
}

CustomPlot* FullyAutomatedPlatelets::pinstanceadjustcoordinates()
{
    FullyAutomatedPlatelets *app = instance();
    if (!app) {
        return nullptr;
    }
    if (!app->_mAdjustthecoordinates) {
        app->_mAdjustthecoordinates = new CustomPlot();
        QObject::connect(app->_mAdjustthecoordinates, &QObject::destroyed, app, [app]() {
            app->_mAdjustthecoordinates = nullptr;
        });
    }
    return app->_mAdjustthecoordinates;
}

mainControlBoardProtocol* FullyAutomatedPlatelets::pinstanceMainboarddata()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mparsemainboard : nullptr;
}

Calibrate* FullyAutomatedPlatelets::pinstancepatientdata()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mppatientinfo : nullptr;
}


Monitor_TrayTest* FullyAutomatedPlatelets::pinstanceobtainModuledata()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mpobtainModuledata : nullptr;
}

MachineSetting* FullyAutomatedPlatelets::pinstanceequipmentconfig()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_mequipmentconfig : nullptr;
}

CustomCreatSql* FullyAutomatedPlatelets::pinstancesqlData()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->_msqldata : nullptr;
}

Inquire_Sql_Info* FullyAutomatedPlatelets::pinstanceInquiredata()
{
    FullyAutomatedPlatelets *app = instance();
    return app ? app->minquireSqldata.data() : nullptr;
}


