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
    this->setApplicationName("全自动血小板聚集仪");

    _mainWindow = new MainWindow();
    _mainWindow->resize(availableScreenX,availableScreenY);

    _mcontroldimming = new controldimming(this);

    _maboutequipment = new AboutMachine();

    _mreminderinfowidget = new Alarm();

    _minstrumentConsumables = new QualityControl();

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

    _minquiredata = new Inquire_Sql_Info();

    _mprintPdf =  new Printthereport();

    _ready = true;

     /*** 调光信号 **/
    qRegisterMetaType<QList<quint8> >("const QList<quint8> ");
    qRegisterMetaType<QMap<quint8, quint8> >("QMap<quint8,quint8>");

    QObject::connect(_mcontroldimming,&controldimming::showDimmingFailedChn,_mainWindow,
                     &MainWindow::recvshowDimmingFailedChn);


    QObject::connect(_mcontroldimming,&controldimming::writemodulLedData,_mpsreialport,
                     &SuoweiSerialPort::slotwrite_instructions_group,
                     Qt::QueuedConnection);


    //主界面显示耗材余量
   connect(_minstrumentConsumables,SIGNAL(SynclimitAlarmtheMainInterface(QMap<quint8,quint8>)),
                     _mainWindow,SLOT(DisplaysConsumablesRemaining(QMap<quint8,quint8>)));

   connect(
            _msuppilereminder,&instrumentAlarmPrompt::outSideCleanDepleteOne,
            _minstrumentConsumables,&QualityControl::handleoutSideCleanDepleteOne);

    //QObject::connect(_mainWindow,&MainWindow:: _writeSingleorder ,_mwritecommand,&ConsumablesWrite::_writeordertoboard);

    //测试界面


    QObject::connect(_minstrumentConsumables,&QualityControl::SycnMainUiLosserSuppile,
                     _mainWindow,&MainWindow::DisplaySycnMainUiLosserSuppile);

    QObject::connect(_minstrumentConsumables,&QualityControl::NoinitialCleaning,
                      _mainWindow,&MainWindow::recvNoinitialCleaning);

    QObject::connect(_mainWindow,&MainWindow::AlarmReminderSound,
                     _mreminderinfowidget,&Alarm::OnOffSound); //警示界面报警开关

    //插入提示文字
    QObject::connect(_mainWindow,&MainWindow::ReminderTextOut,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(_mpsreialport,&SuoweiSerialPort::outArmText,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(_maddtestsamplecase,&Height_Data::ReminderTextOut,
                     _mreminderinfowidget,&Alarm::InsertText);

    QObject::connect(_mcontroldimming,&controldimming::reminderText,
                     _mreminderinfowidget,&Alarm::InsertText);


    QObject::connect(_mreminderinfowidget,&Alarm::AlarmIconState,
                     _mainWindow,&MainWindow::configReminderIcon);


    //设置界面写命令到串口
    QObject::connect(_mequipmentconfig,&MachineSetting::WriteArryCommand,_mpsreialport,
                     &SuoweiSerialPort::writedataToEquipment,
                     Qt::QueuedConnection);


    //调整模组数据发送到串口
    QObject::connect(_mpobtainModuledata,&Monitor_TrayTest::writemodulData,
                     _mpsreialport,&SuoweiSerialPort::writedataToEquipment,
                     Qt::QueuedConnection);


    //测试时控制开关通道旋转电机
    connect(_mainWindow,&MainWindow::_controlmotorrunning,_mpobtainModuledata,
            &Monitor_TrayTest::controlChnMotorRotating);

    //设置界面设置温度
    connect(_mequipmentconfig, &MachineSetting::config_modul_temp,
             _mpobtainModuledata,&Monitor_TrayTest::configModulTEMPvalue,
             Qt::QueuedConnection);


    connect(_minquiredata,&Inquire_Sql_Info::writepdfprint,
            _mprintPdf,&Printthereport::slotwritePdf);

    connect(_minquiredata,&Inquire_Sql_Info::_printoutresult,
            _mprintPdf,&Printthereport::slotprintoutresult);

    _mprintPdf->_Start();

}

FullyAutomatedPlatelets::~FullyAutomatedPlatelets()
{
    if(_minstrumentConsumables){
        delete _minstrumentConsumables;
        _minstrumentConsumables = nullptr;
    }

	if (_mtestingwidget) {
	
		delete _mtestingwidget;
		_mtestingwidget = nullptr;
	}

	if (_mppatientinfo)
	{
		delete _mppatientinfo;
		_mppatientinfo = nullptr;
		QLOG_DEBUG() << "析构患者实例" << __FUNCTION__ << __LINE__ << endl;
	}
	
	delete _msqldata;
	_msqldata = nullptr;
	QLOG_DEBUG() << "析构数据库实例" << __FUNCTION__ << __LINE__ << endl;

	delete _mpsreialport;
	_mpsreialport = nullptr;
	QLOG_DEBUG() << "析构串口USB线程" << __FUNCTION__ << __LINE__ << endl;

    delete _minquiredata;
    _minquiredata = nullptr;

    delete _mprintPdf;
    _mprintPdf = nullptr;


    delete _mpsetTestproject;
    _mpsetTestproject = nullptr;
    QLOG_DEBUG()<<"析构退出配置项目widget"<<__FUNCTION__<<__LINE__;

    delete _maddtestsamplecase;
    _maddtestsamplecase = nullptr;
    QLOG_DEBUG()<<"析构退出添加任务widget"<<__FUNCTION__<<__LINE__;


    delete _mequipmentconfig;
    _mequipmentconfig = nullptr;
    QLOG_DEBUG()<<"析构仪器设置界面"<<__FUNCTION__<<__LINE__;

    delete _mainWindow;
    _mainWindow = nullptr;

    delete _mpobtainModuledata;
    _mpobtainModuledata = nullptr;

    delete _mcontroldimming;
    _mcontroldimming = nullptr;
    QLOG_DEBUG()<<"析构调光实例化"<<__FUNCTION__<<__LINE__;

    delete _maboutequipment;
    _maboutequipment = nullptr;
    QLOG_DEBUG()<<"析构关于界面实例化"<<__FUNCTION__<<__LINE__;


    delete _mreminderinfowidget;
    _mreminderinfowidget = nullptr;
    QLOG_DEBUG()<<"析构提示界面"<<__FUNCTION__<<__LINE__<<endl;

    delete _msuppilereminder;
    _msuppilereminder = nullptr;
    QLOG_DEBUG()<<"析构耗材缺少提示界面"<<__FUNCTION__<<__LINE__<<endl;

    delete _mwritecommand;
    _mwritecommand = nullptr;
    QLOG_DEBUG()<<"析构写入仪器耗材线程"<<__FUNCTION__<<__LINE__<<endl;

    delete _mpSingleactive;
    _mpSingleactive = nullptr;

    delete _mLoadingLogfile;
    _mLoadingLogfile = nullptr;
    QLOG_DEBUG()<<"析构加载日志框"<<__FUNCTION__<<__LINE__<<endl;

    delete _mAdjustthecoordinates;
    _mAdjustthecoordinates = nullptr;
    QLOG_DEBUG()<<"析构校准坐标"<<__FUNCTION__<<__LINE__<<endl;

    delete _mparsemainboard;
    _mparsemainboard = nullptr;
    QLOG_DEBUG()<<"析构解析主板实例"<<__FUNCTION__<<__LINE__<<endl;

#ifdef Q_OS_WIN
	CCreateDump::Instance()->Del_Instance();
#endif
   
}

FullyAutomatedPlatelets *FullyAutomatedPlatelets::instance()
{
    return qobject_cast<FullyAutomatedPlatelets *>(QCoreApplication::instance());
}

MainWindow *FullyAutomatedPlatelets::mainWindow()
{
    return instance()->_mainWindow;
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
    return instance()->_minstrumentConsumables;
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
    return instance()->_minquiredata;
}

Printthereport* FullyAutomatedPlatelets::pinstancePrintPdf()
{
    return instance()->_mprintPdf;
}
