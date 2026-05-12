#include "fullyautomatedplatelets.h"
#include <QDesktopWidget>
#include "mainwindow.h"
#include "operclass/ccreatedump.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

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





}

FullyAutomatedPlatelets::~FullyAutomatedPlatelets()
{
    // 1. 断开所有信号槽连接
    //disconnectAllConnections();

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

	int totalDisconnections = 0;

	// 获取所有需要断开连接的对象（只检查指针有效性）
	QList<QObject*> objects;
	if (minstrumentConsumables) objects << minstrumentConsumables.data();
	if (_mtestingwidget) objects << _mtestingwidget;
	if (_mppatientinfo) objects << _mppatientinfo;
	if (_msqldata) objects << _msqldata;
	if (_mpsreialport) objects << _mpsreialport;
	if (minquireSqldata) objects << minquireSqldata.data();

	if (_mpsetTestproject) objects << _mpsetTestproject;
	if (_maddtestsamplecase) objects << _maddtestsamplecase;
	if (_mequipmentconfig) objects << _mequipmentconfig;
	if (_mpobtainModuledata) objects << _mpobtainModuledata;
	if (_mcontroldimming) objects << _mcontroldimming;

	if (_mreminderinfowidget) objects << _mreminderinfowidget;
	if (_msuppilereminder) objects << _msuppilereminder;
	if (_mwritecommand) objects << _mwritecommand;
	if (_mpSingleactive) objects << _mpSingleactive;
	if (_mLoadingLogfile) objects << _mLoadingLogfile;
	if (_mAdjustthecoordinates) objects << _mAdjustthecoordinates;
	if (_mparsemainboard) objects << _mparsemainboard;
	if (pmainWindow) objects << pmainWindow;

	// 安全断开连接：不使用metaObject
	for (QObject* obj : objects) {
		if (obj && !obj->isWidgetType()) {  // 避免对已部分析构的widget调用metaObject
											// 尝试断开所有连接，但不依赖metaObject
											// 使用static_cast确保类型安全
			totalDisconnections += QObject::disconnect(obj, nullptr, nullptr, nullptr);
		}
	}

	// 断开当前对象的连接
	totalDisconnections += QObject::disconnect(this, nullptr, nullptr, nullptr);

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


