#pragma execution_character_set("utf-8")

#include  "loginui.h"
#include  "mainwindow.h"
#include  "ui_mainwindow.h"
#include  <Dbt.h>
#include  <QCameraInfo>
#include  <QSerialPortInfo>
#include  <QVideoProbe>
#include  "genericfunctions.h"
#include  "globaldata.h"
#include  "mycustomcurve.h"
#include  <dilag/connecttoinstrument.h>
#include  <dilag/handoff_staff.h>
#include  <FunctionWidget/functioncustomwidget.h>
#include  <operclass/fullyautomatedplatelets.h>
#include  <custom_style/custombutton.h>
#include  <QPair>
#include  <QScreen>         // 包含QScreen头文件
#include  <QGuiApplication> // 包含QGuiApplication头文件



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
    ,m_MachineAlreadyInitCleanned(false)
    ,m_bmachineconfigureSignal(false)
    ,m_pauseIcon(":/Picture/pause.png")
    ,m_continueIcon(":/Picture/Start.png")
    ,m_indexSec(0)
    ,m_warmIndexReagent(-1)
    ,m_currentIndex(0)
    ,m_relChannel(0)
    ,m_shutdownClean(false)
    ,m_benterapp(false)
{
    ui->setupUi(this);

    // 窗口属性设置
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    // 预加载图标资源
    m_connectedIcon.load(":/Picture/SetPng/Serial_connect.png");
    m_disconnectedIcon.load(":/Picture/SetPng/Seril_error.png");


    // 验证资源加载
    if(m_connectedIcon.isNull() || m_disconnectedIcon.isNull()) {
        QLOG_ERROR() << "Connection status icons failed to load!";
        m_iconsLoaded = false;
    } else {
        m_iconsLoaded = true;
        // 初始缩放设置
        QSize labelSize = ui->label_machineStaus_show->size();
        m_connectedIcon = m_connectedIcon.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        m_disconnectedIcon = m_disconnectedIcon.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }



    // 状态栏初始化
    ui->StatusBar->setFixedHeight(60);
    m_reminderTitleStr.clear();

    //主标签页配置
    ui->tabWidget_Main->tabBar()->hide();
    ui->tabWidget_Main->setDocumentMode(true);  // 增强视觉表现


    //设备图标配置
    ui->Machine_type->setFixedSize(155, 150);

    // 使用栈对象避免内存管理问题
    QPixmap machinePixmap(":/Picture/suowei.png");
    if(machinePixmap.isNull()) {
        QLOG_WARN() << "Failed to load machine icon";
    }

    // 带缓存的智能缩放
    machinePixmap = machinePixmap.scaled(ui->Machine_type->size() * devicePixelRatioF(),
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);
    machinePixmap.setDevicePixelRatio(devicePixelRatioF());

    ui->Machine_type->setPixmap(machinePixmap);
    ui->Machine_type->setAlignment(Qt::AlignCenter);

    // 高DPI适配样式
    QString style = QString("QLabel {""border: none;""background: transparent;"
                           "min-width: %1px;""min-height: %2px;""}").arg(155).arg(150);
    ui->Machine_type->setStyleSheet(style);

    initErrorDisplayMap();

}

// 线程管理统一处理
void MainWindow::stopAndWaitThreads()
{
    if (m_threadModule.isRunning()) {
        m_threadModule.quit();
        if (!m_threadModule.wait(500)) {
            QLOG_WARN() << "Module thread termination timeout!";
            // 可选：强制终止（仅在必要时）
            // m_threadModule.terminate();
            // m_threadModule.wait();
        };
    }

    if (m_threadMainBoard.isRunning()) {
        m_threadMainBoard.quit();
        if (!m_threadMainBoard.wait(500)) {
            QLOG_WARN() << "MainBoard thread termination timeout!";
        }
    }

    if (mThreaddotest.isRunning()) {
        mThreaddotest.quit();
        if(!mThreaddotest.wait(3000)){
            qCritical() << "Failed to stop thread gracefully!";
            //mThreaddotest.terminate();  // 强制终止（慎用）
        }
    }
    m_pdoingTesting.reset();


    if (mTesthighThread.isRunning()){
        mTesthighThread.quit();
        if (!mTesthighThread.wait(500)) {
            QLOG_WARN() << "High-speed thread timeout, forcing termination";
        }
    }

    // 最终状态验证
    logThreadStatus();
}

void MainWindow::logThreadStatus()
{
    QLOG_DEBUG() << "\n----- Thread Status Report -----";
    QLOG_DEBUG() << "Module Thread:" << (m_threadModule.isRunning() ? "Running" : "Stopped");
    QLOG_DEBUG() << "MainBoard Thread:" << (m_threadMainBoard.isRunning() ? "Running" : "Stopped");
    QLOG_DEBUG() << "Test Thread:" << (mThreaddotest.isRunning() ? "Running" : "Stopped");
    QLOG_DEBUG() << "HighSpeed Thread:" << (mTesthighThread.isRunning() ? "Running" : "Stopped");
    QLOG_DEBUG() << "-------------------------------\n";
}


// 单例清理
void MainWindow::cleanupSingletons()
{
    if (SingletonAxis* instance = SingletonAxis::GetInstance()) {
        instance->deleteInstance();  // 确保在正确线程销毁
    }

    if (ConsumablesOper* instance = ConsumablesOper::GetpInstance()) {
        instance->del_Instance();
    }
}


MainWindow::~MainWindow()
{

    // 停止并等待所有工作线程
    stopAndWaitThreads();

    if(m_experimentTimer){
        delete m_experimentTimer;
        m_experimentTimer = nullptr;
    }


    delete preminder; //析构退出软件

    delete mshowModuledata;//析构接收显示模组数据

    if(mReminder)
        delete mReminder; //充值提示界面

   

    // 单例清理
    cleanupSingletons();

    delete WriteParameter; //析构命令写入仪器


    if (m_processPrint)
    {
        delete	m_processPrint;
        m_processPrint = nullptr;
    }

    StructInstance::getInstance()->m_garbo;

    delete m_ThreadReminderTsetTube;
    m_ThreadReminderTsetTube = NULL;

    delete m_graphplot;

    DestructionSerialclass();

    QsLogging::Logger::destroyInstance();

	

	// 确保所有资源已释放
	QCoreApplication::processEvents();

    delete ui;
}

void MainWindow::configReminderIcon(quint8 index_)
{
    static const std::array<QIcon*, 5> iconMap = {
            &m_iconNormal,  // NormalLog
            &m_iconNormal,  // PromptLog
            &m_iconNormal,  // UserActions
            &m_iconError,   // ErrorLog
            &m_iconReminder // MotherboardErr
    };

    const quint8 idx = index_;
    if (idx > iconMap.size()) return;

    QToolButton *pstauesInfo = ui->sStatusInfo;
    if(!pstauesInfo)  return;

    QIcon* targetIcon = iconMap[idx];
    if (m_currentIcon != targetIcon) { // 指针地址比对（O(1)）
        pstauesInfo->setIcon(*targetIcon);
        m_currentIcon = targetIcon;
        update(); // 触发界面重绘
    }
}


void MainWindow::_dimmingprogress(bool isDimmingInProgress)
{
	// 使用更清晰的变量名
	static const bool isDimmingFinished = cglobal::g_controldimmingfinished;

	// 提前终止条件放在最前面
	if (isDimmingFinished) {
		initcreatprogressingDimming(false); // 终止调光进度条
		return;
	}

	// 使用局部静态变量缓存模块值，避免重复读取INI文件
	static int cachedModValues[3] = { -1, -1, -1 };
	static bool valuesCached = false;

	if (!valuesCached) {
		cachedModValues[0] = INI_File().getModuledimmingVal(MODULE_1);
		cachedModValues[1] = INI_File().getModuledimmingVal(MODULE_2);
		cachedModValues[2] = INI_File().getModuledimmingVal(MODULE_3);
		valuesCached = true;
	}

	// 使用更清晰的逻辑判断
	bool anyModuleDisabled = (cachedModValues[0] == 0) ||
		(cachedModValues[1] == 0) ||
		(cachedModValues[2] == 0);

	if (anyModuleDisabled) {
		QLOG_ERROR() << "所有模组调光值设置为0 调光异常!";
		return;
	}

	// 更新进度条状态
	initcreatprogressingDimming(isDimmingInProgress);
	update();
}

void MainWindow::init_style_all()
{
    m_benterapp = true;
    quint8 indexequipment;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, indexequipment );//仪器型号
    minstrumentType = indexequipment;
    FullyAutomatedPlatelets::pinstancedimming()->sycnequipment(minstrumentType);

    switch(minstrumentType)
    {
        case KS600: this->setWindowTitle(KS600STR);     break;
        case KS800: this->setWindowTitle(KS800STR);     break;
        case KS1200:this->setWindowTitle(KS1200STR);    break;
        default:    this->setWindowTitle(KS1200STR);    break;
    }

    ui->MachineAxisValue->hide();       /*隐藏坐标信息*/

    InitMainUiLayout();//主界面布局

    initTrayFunction();//托盘显示

    initchnstate(); //初始化通道状态

    updateModuleVisibilityBasedOnInstrumentType();/*根据仪器型号初始化显示界面状态*/

    int maxthreadnum = QThreadPool::globalInstance()->maxThreadCount();
    QThreadPool::globalInstance()->setMaxThreadCount(maxthreadnum); //线程池
    QLOG_DEBUG()<<"线程池最大数"<<maxthreadnum<<endl;

    _serialConnection(); //串口实例化

    //Traverse_through_motherboard_information(); //遍历主板线程

    Traverse_through_ModuleReadInfo();  //遍历模组信息

    _init_read_moduledata_thread(); //初始化实例获取到模组数据线程

    _inittestmodule_data_thread(); //收到模组数据处理线程

    _initmainboradthread(); //主板线程实例化函数&&***触发测高信号

    CreatActionExecution();  //单个动作完成状态信号

    listentoUsb();

    initTestTaskThread(); //初始化启动任务线程

    //_opentes_hight_thread(); //打开测高线程
    {

        QObject::connect(this,&MainWindow::delInitfile,
                         FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::initdelfile);

        //清除7天外的log,清除样本详细信息非当天的信息(清洗完成才开启线程)
        emit delInitfile();

        QObject::connect(FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::_sendDirectives,
                         mlocalSerial.data(),&SuoweiSerialPort::writedataToEquipment,
                         Qt::QueuedConnection);

        //写入耗材状态
        QObject::connect(mlocalSerial.data(),&SuoweiSerialPort::ConsumableswriteState,
                         FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::_recvConsumableswriteState);

        QObject::connect(FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::_sendDirectivesList,
                         mlocalSerial.data(),
                         &SuoweiSerialPort::slotwrite_instructions_group);

        QObject::connect(this,&MainWindow::_sendcodeList,
                         FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::_slotsendcodeList);



        QObject::connect(this,&MainWindow::closeEquipmentconsumables,
                         FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::handlcloseEquipmentconsumables,
                         Qt::QueuedConnection);


       connect(FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::closeSerial,
                         mlocalSerial.data(),
                         &SuoweiSerialPort::slotcloseseirport
                        ,Qt::QueuedConnection);

        QObject::connect(FullyAutomatedPlatelets::pinstanceWirteBoard(),
                         &ConsumablesWrite::_ShutdownApp,this,[=](){
                FullyAutomatedPlatelets::mainWindow()->deleteExitSoftware();
        });
    }


    //所有测试完成复位
    connect(mptesting.data(),&Testing::tsetfinishedbackorigin,this,[=]()
    {
        _backOriginTestFinished();

        if (!m_pProgress.isNull()) {
            m_pProgress->close();  // 自动触发 delete 并置空 m_pProgress
            qApp->processEvents(); // 可选：处理界面残留
        }

        emit allCurveClear();  //曲线界面复原

        mptesting.data()->TotalTaskProgress();//总任务进度试管杯
        update();
    });

    connect(m_graphplot.data(),&GraphPlot::DrawProgressbar,
            mptesting.data(),&Testing::DrawChannelProgress);

    connect(this,&MainWindow::allCurveClear,
            m_graphplot.data(),&GraphPlot::backallCurveClear);

    /*机器主界面试管使用同步耗材界面试管使用显示 &&质控使用试管*/
    connect(mptesting.data(),&Testing::SynchronizeEmptyTube,
            minstrumentConsumables.data(),
            &QualityControl::slotSynchronizeEmptyTube);

    connect(this, &MainWindow::SynchronizeEmptyTubeUsed,
            minstrumentConsumables.data(),
            &QualityControl::slotSynchronizeEmptyTube);


    //模组通道值显示在设置界面
    MachineSetting* pequipmentconfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();

    connect(mlocalSerial.data(),&SuoweiSerialPort::backHandssuck,pequipmentconfig,
            &MachineSetting::handlebackHandssuck,Qt::QueuedConnection);

    /** 登录工程师模式  **/
    connect(pequipmentconfig,&MachineSetting::LoginEngineerMode,
            this,[=](const bool EnginnerMode){
        FullyAutomatedPlatelets::pinstanceInquiredata()->LoginEngineerMode(EnginnerMode);
    });

    connect(mshowModuledata.data(),&displayChanneldata::SetChannelValueUpdate,
            pequipmentconfig,&MachineSetting::SlotSetChannelValueUpdate,
            Qt::QueuedConnection);

    //断线清空模组缓存数据
    connect(this,&MainWindow::cleanModuleBuffData,
            mshowModuledata.data(),&displayChanneldata::handleCleanModuleBuffData
            ,Qt::QueuedConnection);


    //读耗材卡确认充值
    connect(this, &MainWindow::rechargesuccessful,
            minstrumentConsumables.data(),
            &QualityControl::handleRechargeSuccessful);

    //配置机器参数信号耗材修改限位值 、耗材余量
    connect(this,&MainWindow::sendmoduleactio,  mlocalSerial.data(),
            &SuoweiSerialPort::slotwrite_instructions_group);

    connect(this,&MainWindow::sendOneDirectives,
            mlocalSerial.data(),
            &SuoweiSerialPort::writedataToEquipment,Qt::QueuedConnection);

    //发送指令到仪器、指令信息
    connect(minstrumentConsumables.data(),&QualityControl::sendDirectives,
            mlocalSerial.data(),
            &SuoweiSerialPort::writedataToEquipment);

    connect(mshowModuledata.data(),&displayChanneldata::sampleTestingErr,
            FullyAutomatedPlatelets::pinstancepatientdata(),
            &Calibrate::recvSampleTestingErr);


    //机器参数配置
    WriteParameter = new ConfigureWriteParameter(5);
    WriteParameter.data()->setAutoDelete(false);
    WriteParameter.data()->sycnSerialclass(mlocalSerial.data());
    connect(this,&MainWindow::Thread_5_SendCode,WriteParameter.data(),&ConfigureWriteParameter::WriteCodearray);
    //测蒸馏水
    connect(this,&MainWindow::Delayed_sending,WriteParameter.data(),&ConfigureWriteParameter::slotDelayed_sending);

    //要读取通道的参数
    connect(this,&MainWindow::WhileReadChannelParam,WriteParameter.data(),
            &ConfigureWriteParameter::slotWhileReadChannelParam);


    slotconnectionStateChanged(cglobal::gserialConnecStatus);


    //初始化启动测高线程
    initTestPprHeight();

    return;
}


//初始化实例获取到模组数据线程
void MainWindow::_init_read_moduledata_thread()
{
    if(mshowModuledata == nullptr)
        mshowModuledata = new displayChanneldata();

    connect(this,&MainWindow::syncModuleChannelData, mshowModuledata.data(),
                &displayChanneldata::Recv_syncModuleChannelData);

    connect(mshowModuledata.data(), &displayChanneldata::DisplayTestingValue,
                m_graphplot.data(),&GraphPlot::GetTestingValue);

    connect(this,&MainWindow::qualityChannelVal,mshowModuledata.data(),
            &displayChanneldata::getqualityControlValue);

    connect(FullyAutomatedPlatelets::pinstanceequipmentconfig(),&MachineSetting::getchanneldata,
            this,[=](const quint8 channel,int indexActive){
         emit qualityChannelVal(channel,indexActive);
    });

    return;
}

//收到模组数据处理线程
void  MainWindow::_inittestmodule_data_thread()
{
    if(mtestmoduleprotocol == nullptr)
    {
        mtestmoduleprotocol = new theTestModuleProtocol();
        mtestmoduleprotocol.data()->SynchronizeInstrumentType(minstrumentType);
        mtestmoduleprotocol.data()->setArgInterface(this);
        mtestmoduleprotocol.data()->_start();//调用线程--模组处理数据

        //串口线程接收到模组的数据
        connect(mlocalSerial.data(),&SuoweiSerialPort::moduleprotocolData,mtestmoduleprotocol.data(),
                &theTestModuleProtocol::recrModuleprotocolData);

        //设置完成模组参数提示到设置界面
        MachineSetting*pequipmentconfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();

        connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::sendReminder,
                pequipmentconfig,&MachineSetting::slotsendReminder);

        //读取模组的速度
        connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::readModuleSpeed,
                pequipmentconfig,&MachineSetting::slotmoduleSpeedData);

        //读取模组的LED 值
        connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::readModuleLed,
                pequipmentconfig,&MachineSetting::slotmoduleLedData);

        //保存调光
        connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::_writeDataToSerial,
                this,[=](const QByteArray datasave,QString outtext){
            emit sendOneDirectives(datasave,outtext);
            QLOG_DEBUG()<<"收到调光完成,发送保存指令!"<<__FILE__<<endl;
        });


        QObject::connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::errmodule,
                         this,[=](QString title_,QString errstr){
             ThreadSafeReminder(title_,errstr);
        });


        //是否暂停模组通讯
        connect(mtestmoduleprotocol.data(),
                &theTestModuleProtocol::pauseConnectModule,this,[=](){
                pauseObtainmodulecommand("调光保存暂停模组通讯",true);
        });


        QObject::connect(mtestmoduleprotocol.data(),
                         &theTestModuleProtocol::resetconnectModule,this,[=](){
            emit aNewconnectMachine();
            emit ReminderTextOut(PROMPTLOG,tr("恢复继续获取模组和主板数据"));
        });

        //第一次主动调光后重新采集模组数据
        QObject::connect(mtestmoduleprotocol.data(),
                         &theTestModuleProtocol::FirstDimmingResult,this,[=](quint8 indexmodule)
        {
            FullyAutomatedPlatelets::pinstancedimming()->SecondDimmingModule(indexmodule,1);
        });


    }
    return;
}

void MainWindow::_initmainboradthread()
{
    FullyAutomatedPlatelets::pinstanceMainboarddata()->setArgInterface(this);
    mainControlBoardProtocol *_pparsemainboard = FullyAutomatedPlatelets::pinstanceMainboarddata();


    //收到串口主板消息
    QObject::connect(mlocalSerial.data(),&SuoweiSerialPort::mainControlBoardProtocolData,
                     _pparsemainboard,
                     &mainControlBoardProtocol::recvmainControlBoardProtocol,
                     Qt::QueuedConnection);



    QObject::connect(_pparsemainboard,&mainControlBoardProtocol::_normaloper,this,[=](quint8 index_){
        if(equipmentTipInfo::LinqueScrapFull == index_ )
        {
            ui->show_wastlique->hide();
        }
        if(equipmentTipInfo::LinqueCleanShortage == index_ )
        {
            ui->showcleanliqur->hide();
        }

    });

    //废液、外部清洗液弹出提示后在信息栏提醒信号连接
    QObject::connect(_pparsemainboard, &mainControlBoardProtocol::_reminderErrorInfo,
                     this, &MainWindow::handleErrorNotification);
    // 验证信号签名是否匹配
    static_assert(
        QtPrivate::FunctionPointer<decltype(&mainControlBoardProtocol::_reminderErrorInfo)>::ArgumentCount == 2,
        "Signal signature mismatch!"
    );

    connect(this,&MainWindow::ConfigUsedBuzzerMark,_pparsemainboard,
            &mainControlBoardProtocol::RecvUsedBuzzerMark);

    //刷卡提示
    connect(_pparsemainboard,&mainControlBoardProtocol::cardSwipeSuccessful,
            this,&MainWindow::handlecardSwipeSuccessful);


    //读卡、充值成功信号
    connect(_pparsemainboard,&mainControlBoardProtocol::swipeCardSuccessfullyWritten,
            this,&MainWindow::handleswipeCardSuccessfullyWritten);

    //读卡异常信息提示
    connect(_pparsemainboard,&mainControlBoardProtocol::Abnormalcardswipingprompt,
            this,[=](const QString promptInfo){
        ThreadSafeReminder(tr("充值耗材异常"),promptInfo);
        return;
    });

    //提示试管盘
    m_ThreadReminderTsetTube = new ThreadReminderTsetTube(this);

    //废液、外部清洗液、试管盘 弹出提示
    QObject::connect(_pparsemainboard,&mainControlBoardProtocol::equipmentReminder,m_ThreadReminderTsetTube,
                    &ThreadReminderTsetTube::recvequipmentReminder);

    QObject::connect(m_ThreadReminderTsetTube,
                     &ThreadReminderTsetTube::reminderMainUi,this,
                     [=](quint8 index, QString datastr){
                emit ReminderTextOut(index,datastr);
    });

    //触发测高
    connect(_pparsemainboard,&mainControlBoardProtocol::MainBoardHeightTrigger,
        this,&MainWindow::recv_MainBoardHeighTigger);

    _pparsemainboard->_start(minstrumentType);

    return;
}


void MainWindow::initTestTaskThread()
{
    if(mThreaddotest.isRunning()){
        QLOG_WARN()<<"工作线程已启动";
        return;
    }

    m_pdoingTesting.reset(new TestProjectProcess(nullptr,minstrumentType));

    // 绑定对象到工作线程
    m_pdoingTesting->moveToThread(&mThreaddotest);
    m_pdoingTesting->setTargetThread(&mThreaddotest);

    // 初始化线程配置
    mThreaddotest.setPriority(QThread::HighPriority);
    mThreaddotest.start();  // 显式启动线程


    //耗材提示暂停
    connect(m_pdoingTesting.data(),&TestProjectProcess::pauseSycnMainuiUpdate,
            this,&MainWindow::testingSuppileLoss,
            Qt::QueuedConnection);
    //S1清洗液不足 暂停
    connect(FullyAutomatedPlatelets::pinstanceinstrument(),
            &QualityControl::consumablesLackPauses,this,
            &MainWindow::testingSuppileLoss
            );


    connect(this,&MainWindow::startTheTestTask,m_pdoingTesting.data(),
            &TestProjectProcess::handleStartTheTestTask,
            Qt::QueuedConnection);

    connect(m_pdoingTesting.data(),&TestProjectProcess::reminderTextOut,
            FullyAutomatedPlatelets::pinstanceinfowidget(),
            &Alarm::handleReminderText,
            Qt::QueuedConnection);


    connect(mlocalSerial.data(),&SuoweiSerialPort::sendTestingRecvData,
            m_pdoingTesting.data(),
            &TestProjectProcess::recvEquipmentdata,
            Qt::QueuedConnection);


    connect(m_pdoingTesting.data(),&TestProjectProcess::WriteaSingleCommand,
            mlocalSerial.data(),
            &SuoweiSerialPort::writedataToEquipment,
            Qt::QueuedConnection);



    connect(m_pdoingTesting.data(),&TestProjectProcess::write_instructions_group,
            mlocalSerial.data(),&SuoweiSerialPort::slotwrite_instructions_group);


    //异常提示
    connect(m_pdoingTesting.data(),&TestProjectProcess::executeErrPrompt,this,
            [this](QString ERRInfo){
        ThreadSafeReminder("动作执行异常", ERRInfo);
        emit ProceedTask();//继续执行
    });

    connect(this,&MainWindow::ProceedTask,m_pdoingTesting.data(),
            &TestProjectProcess::equipment_err_continue);

    //打开通道转子
    connect(m_pdoingTesting.data(),&TestProjectProcess::openChnMotor,
            this,[=](const quint8 indexchn,const bool bopen)
    {
        emit _controlmotorrunning(indexchn,bopen);
    });



    //获取通道贫血值(抓贫血到测试通道后发送信号)
    connect(m_pdoingTesting.data(),
            &TestProjectProcess::getAnemiaValue,
            mshowModuledata.data(),
            &displayChanneldata::slotgetAnemiaValue);

    //获取样本PPP初值成功丢PPP
    connect(mshowModuledata.data(),
            &displayChanneldata::CompleteGetAnemiaValue,
            m_pdoingTesting.data(),
            &TestProjectProcess::throwAnemiaCup);

    //空试管被夹起
    connect(m_pdoingTesting.data(),&TestProjectProcess::EmptyTubeCatched,
            mptesting.data(), &Testing::EmptyTubeClipMoved,
            Qt::QueuedConnection);


    //获取通道富血的初值 ==>获取到富血值后 吐试剂
    connect(m_pdoingTesting.data(),&TestProjectProcess::readbloodyInitValue,
            mshowModuledata.data(),&displayChanneldata::slotreadbloodyInitValue,
            Qt::QueuedConnection);


    connect(mshowModuledata.data(),&displayChanneldata::spitReagentTesting,
            m_pdoingTesting.data(),&TestProjectProcess::slotSpitReagentTesting,
            Qt::QueuedConnection);

    //吐完试剂后测试
    connect(m_pdoingTesting.data(),&TestProjectProcess::openTestChnTest,
            mshowModuledata.data(),&displayChanneldata::slotopenTestChnTest,
            Qt::QueuedConnection);

    //样本吐出试剂开始测试清洗针
    connect(mshowModuledata.data(), &displayChanneldata::addSampmeTestCleanPin,
            m_pdoingTesting.data(),&TestProjectProcess::slotaddSampmeTestCleanPin,
            Qt::QueuedConnection);

    //通道完成测试
    connect(mshowModuledata.data(),&displayChanneldata::testComplete,
            m_pdoingTesting.data(),&TestProjectProcess::SampleReagTestFinish);

    connect(this,&MainWindow::resumeaTestTask,m_pdoingTesting.data(),
            &TestProjectProcess::handleResumeaTestTask);

    //测试中添加样本
    connect(mptesting.data(),&Testing::testingaddsample,
            m_pdoingTesting.data(),&TestProjectProcess::slottestingaddsample);

    /*单个项目完成曲线图保存到数据库*/
    connect(mshowModuledata.data(),&displayChanneldata::finishtestProgress,
            m_graphplot.data(),&GraphPlot::CompleteOneSample);


    connect(m_pdoingTesting.data(),&TestProjectProcess::setprogressinitboolwaittext,
            mptesting.data(),&Testing::updateChannelProgressAndStatus);

    connect(m_pdoingTesting.data(),&TestProjectProcess::sycn_SampleTestingChangInitColor,
            mptesting.data(),&Testing::slot_sycn_SampleTestingChangInitColor);

    /*弃杯成功*/
    connect(m_pdoingTesting.data(),&TestProjectProcess::throwtesttube,
            mptesting.data(),&Testing::slot_throwtesttube);

    //断线重连
    connect(mlocalSerial.data(),&SuoweiSerialPort::connectEquipmentagin,
            m_pdoingTesting.data(),&TestProjectProcess::slot_connectEquipmentagin);


    //测试过程中抓手吸 吐试杯失败
    connect(m_pdoingTesting.data(),&TestProjectProcess::theGripperFailed,
            this,&MainWindow::handletheGripperFailed);

    connect(this,&MainWindow::gripErrGiveupSample,
            m_pdoingTesting.data(),&TestProjectProcess::handleGripErrGiveupSample,
            Qt::QueuedConnection);

    connect(this,&MainWindow::pendingtimeoutSampleTest,
            m_pdoingTesting.data(),
            &TestProjectProcess::handlePendingtimeoutSampleTest,
            Qt::QueuedConnection);

    // 在主窗口类中连接信号
    connect(mshowModuledata.data(), &displayChanneldata::signalShowPPPError,
            this, [this](int value) {

        on_toolButton_quality_sample_clicked();//暂停
        QMessageBox::warning(this,  "PPP异常警告",
            QString("检测到PPP异常值: %1\n正常范围: 2000-3000\n机器已停止测试").arg(value));
    });


}


void MainWindow::pauseObtainmodulecommand(QString info,bool connectedCotl)
{
    if(m_moduletimerThread && m_ObtainMainBoardData)
    {
        //模组暂停||主板暂停
        if(connectedCotl){
            emit stopObatinMachineInfo();
            emit ReminderTextOut(PROMPTLOG,tr("暂停 获取模组和主板数据"));
            QLOG_DEBUG()<<info<<"暂停 获取模组数据 && 主板数据";
        }
        else
        {
            emit aNewconnectMachine();
            emit ReminderTextOut(PROMPTLOG,tr("恢复继续获取模组和主板数据"));
            QLOG_DEBUG()<<info<<"恢复 获取模组数据 && 主板数据";
        }
    }
}


void MainWindow::listentoUsb()
{
    //初始化USB监听 (目标设备VID=0x0483, PID=0x5740)
    m_USBListener.reset(new USBListener(HEX_VID, HEX_PID,this));
    if (!this->window()->winId()) {
        this->window()->createWinId(); // 确保有窗口ID
    }
    HWND hwnd = reinterpret_cast<HWND>(this->window()->winId());
    m_USBListener->registerDevice(hwnd); //注册设备


    // 连接信号
    connect(m_USBListener.data(), &USBListener::deviceConnected,
            this, &MainWindow::onDeviceConnected);
    connect(m_USBListener.data(), &USBListener::deviceDisconnected,
            this, &MainWindow::onDeviceDisconnected);

    connect(m_USBListener.data(), &USBListener::listenerStarted,
            this, &MainWindow::onListenerStarted);
    connect(m_USBListener.data(), &USBListener::listenerStopped,
            this, &MainWindow::onListenerStopped);
    connect(m_USBListener.data(), &USBListener::errorOccurred,
            this, &MainWindow::onListenerError);
    connect(m_USBListener.data(), &USBListener::usbPowerContl,
            this, &MainWindow::onusbPowerContl);

    // 启动线程
    m_USBListener->startListening();
}


void MainWindow::onDeviceConnected(usbDevice dev)
{
    QLOG_DEBUG()<<"仪器重连类型:"<<dev.type;
    emit requestReconnect();
    emit ReminderTextOut(PROMPTLOG, tr("USB仪器设备连接!"));
    return;

}
void MainWindow::recvprepareReconnectGetData() {
	pauseObtainmodulecommand("USB仪器设备连接", false);
}

void MainWindow::onDeviceDisconnected(usbDevice dev)
{
    QLOG_DEBUG()<<"仪器掉线类型:"<<dev.type;
    QString usbDisconnect = QString("仪器断线:%1").arg(dev.type);
    const QString ErrOut = "仪器USB掉线,请检查接口!";
	ThreadSafeReminder(usbDisconnect, ErrOut);
	//设置状态 关闭串口
	slotconnectionStateChanged(false);
	emit cleanModuleBuffData(); //清空模组缓存数据
	emit disConnectCloseSerial();
    emit ReminderTextOut(ERRORLOG, ErrOut);
    return;
}


void MainWindow::onListenerStarted() {
     emit ReminderTextOut(NORMALLOG, tr("USB监听已启动!"));
}

void MainWindow::onListenerStopped() {
    emit ReminderTextOut(PROMPTLOG, tr("USB监听已停止!"));
}

void MainWindow::onListenerError(QString message) {
    const QString err =  "USB监听错误" + message;
    emit ReminderTextOut(ERRORLOG, err);
}

void MainWindow::onusbPowerContl(QString message, const quint8 istate) {

    emit ReminderTextOut(istate, message);
}



void MainWindow::initchnstate()
{
    //使用容器初始化列表替代重复拼接操作
    const QList<quint8> operChnList = [this]() -> QList<quint8> {
       switch(minstrumentType) {
           case KS600:  return {TEST_CHANNEL_1, TEST_CHANNEL_2, TEST_CHANNEL_3, TEST_CHANNEL_4};
           case KS800:  return {TEST_CHANNEL_1, TEST_CHANNEL_2, TEST_CHANNEL_3, TEST_CHANNEL_4,
                                TEST_CHANNEL_5, TEST_CHANNEL_6, TEST_CHANNEL_7, TEST_CHANNEL_8};
           case KS1200: return {TEST_CHANNEL_1, TEST_CHANNEL_2, TEST_CHANNEL_3, TEST_CHANNEL_4,
                                TEST_CHANNEL_5, TEST_CHANNEL_6, TEST_CHANNEL_7, TEST_CHANNEL_8,
                                TEST_CHANNEL_9, TEST_CHANNEL_10, TEST_CHANNEL_11, TEST_CHANNEL_12};
           default:     return {}; // 2. 处理未定义仪器类型
       }
    }();

   //空容器防御检查
   if (operChnList.isEmpty()) {
       qWarning() << "Unsupported instrument type:" << minstrumentType;
       return;
   }

    StructInstance::getInstance()->addEquipmentChn(operChnList);

   //使用范围循环替代索引遍历
   for (int i = 0; i < operChnList.size(); ++i) {
       //集中管理配置键名生成逻辑
       const QString configKey = QString("TestTheChanne1Opening_%1").arg(i + 1);

       // 减少临时变量，直接使用三元运算符
       const bool isOpen = INI_File().rConfigPara(configKey).toBool();
       const quint8 status = isOpen ? CHN_STATUS_FREE : CHN_STATUS_DISABLE;

       StructInstance::getInstance()->config_testChn_State(i, status);
   }

}


void MainWindow::deleteExitSoftware()
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    QApplication::exit(0);
    return;
}




// 线程安全入口（MainWindow.cpp）
void MainWindow::ThreadSafeReminder(QString title_, QString outputText) {
    // 1. 线程安全检查
    if (QThread::currentThread() != this->thread()) {
        // 跨线程调用：安全转发到主线程
        QMetaObject::invokeMethod(this, "onReminderRequested",
            Qt::QueuedConnection,  // 异步队列连接
            Q_ARG(QString, title_),
            Q_ARG(QString, outputText));
        return;
    }

    // 主线程直接调用
    RealReminderImpl(title_, outputText);
}

// 安全槽函数（主线程执行）
void MainWindow::onReminderRequested(QString title_, QString outputText) {
    RealReminderImpl(title_, outputText);
}

void MainWindow::RealReminderImpl(QString title_, QString outputText)
{
    QWriteLocker locker(&m_reminderLock);
    // 1. 快速判断标题是否已存在（QSet优化查询效率）
    if (m_reminderTitleStr.contains(title_)) return;


    // 2. 使用QPointer跟踪弹窗对象生命周期（避免悬空指针）
    QPointer<warn_interface> pwarn = new warn_interface(title_, outputText);


    //3. 配置弹窗属性（使用Qt属性链式调用）
    pwarn->setWindowFlags(Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::WindowDoesNotAcceptFocus);
        pwarn->setAttribute(Qt::WA_DeleteOnClose);   // 关闭时自动销毁
        pwarn->setAttribute(Qt::WA_ShowWithoutActivating); // 不抢占焦点
        pwarn->setWindowModality(Qt::NonModal);     // 非模态窗口

    // 4. 集中初始化弹窗内容（减少冗余调用）
    pwarn->settitle(title_);
    pwarn->setremtext(outputText);
    pwarn->replaceSupplyIndex(-1);

    // 5. 信号连接：用户确认后清理标题集合
    connect(pwarn, &warn_interface::makesure, this,
        [this](int,  QString title) {
            QWriteLocker lock(&m_reminderLock);
            m_reminderTitleStr.remove(title);
    },
    Qt::QueuedConnection);

    //6. 自适应多屏幕居中（基于主窗口所在的屏幕）
    if (const QScreen* activeScreen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = activeScreen->availableGeometry();
        pwarn->move(
            screenGeometry.center() - pwarn->rect().center()
        );
    }

    // 7. 显示弹窗并记录标题（原子操作避免竞态条件）
    pwarn->show();
    pwarn->raise(); // 确保窗口在最前面
    pwarn->activateWindow(); // 激活窗口
    m_reminderTitleStr.insert(title_);
    return;
}



void  MainWindow::_reminderFunctionWidget(QString title_,QString outputtext_,QList<QString> btntext)
{
    int btn_ = 0;
    if(preminder == nullptr)
        preminder = new FunctionCustomWidget(title_,outputtext_);
    preminder.data()->setremimdertext(outputtext_);
    preminder.data()->setAttribute(Qt::WA_QuitOnClose,false);
    // 设置窗口始终位于最前端
    preminder.data()->setWindowFlags(preminder.data()->windowFlags() | Qt::WindowStaysOnTopHint);
    // 移动窗口到屏幕中心
    int screenWidth = QApplication::desktop()->width();
    int screenHeight = QApplication::desktop()->height();
    int windowWidth = preminder.data()->width();
    int windowHeight = preminder.data()->height();
    preminder.data()->move((screenWidth - windowWidth) / 2, (screenHeight - windowHeight) / 2);
    foreach (QString data_, btntext){
        preminder.data()->setTextbtnfunction(btn_,data_);
        btn_++;
    }

    connect(preminder.data(),&FunctionCustomWidget::sender_1function_,this,[=]()
    {
        emit ReminderTextOut(NORMALLOG,"取消退出软件");
    });

    connect(preminder.data(),&FunctionCustomWidget::sender_2function_,this,[=]()
    {
        emit controlallchnstate(false); //关闭所有通道旋转
        if(m_MachineAlreadyInitCleanned == false)
        {
           emit  FullyAutomatedPlatelets::pinstanceWirteBoard()->closeSerial();
           FullyAutomatedPlatelets::mainWindow()->deleteExitSoftware();
        }
        else
        {
            m_shutdownClean = true;
            equipmentinitActive(false,m_shutdownClean);  //开机关复位负速度运动模式清洗
        }
    });

    connect(preminder.data(),&FunctionCustomWidget::sender_3function_,this,[=](){
        writeConsumablesExit();
    });
    if(preminder.data()->isMinimized()){
        preminder->showNormal();
        preminder->raise();
        preminder->activateWindow();
    }
    preminder.data()->setWindowModality(Qt::NonModal);
    preminder.data()->update();
    preminder.data()->show();
}


// 辅助函数：检查清洗液余量
bool MainWindow::checkCleanFluidSufficiency()
{
    double lastRatio = 0.0;
    quint16 updateRatio = 0;
    quint8 limitRatio = 0;

    QUIUtils::CalculateTheConsumptionOfCleaningFluid(0, lastRatio, updateRatio);
    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT, INDEX_CLEANLINQUE_CONSUMABLE, limitRatio);

    double remainingRatio = lastRatio * 100.0;
    return (remainingRatio > limitRatio);
}


// 辅助函数：显示坐标同步错误
void MainWindow::showAxisSyncErrorDialog()
{
    ThreadSafeReminder("开机初始化失败", "未完成坐标同步，请重新联机后启动软件.");
}

void MainWindow::FirstConnectCleanEquipment()
{
    try {
        // 1. 检查清洗液是否充足
        bool hasEnoughCleanFluid = checkCleanFluidSufficiency();

        // 2. 检查坐标同步状态
        bool isAxisSyncFinished = getbAxisSycnFinished();
        if (!isAxisSyncFinished) {
           ThreadSafeReminder("开机初始化失败","未连机同步坐标,请联机重新打开软件!");
           return;
        }

        if(!m_MachineAlreadyInitCleanned){
           initProgressBar("开机清洗进度",false,true);
           equipmentinitActive(hasEnoughCleanFluid,m_shutdownClean);
        }

    } catch (const std::exception& e) {
        qCritical() << "Initialization failed:" << e.what();
        ThreadSafeReminder("严重错误", "初始化过程中发生异常，请联系技术支持。");
    }
    return;
}



void MainWindow::_serialConnection()
{
    mlocalSerial = FullyAutomatedPlatelets::pinstanceserialusb();
    mlocalSerial.data()->suoweiportthreadbegin();

    const auto serial = mlocalSerial.data();

    //重新连接
    connect(this,&MainWindow::requestReconnect,
            serial, &SuoweiSerialPort::prepareReconnect);
	//断开连接 关闭窗口
	connect(this, &MainWindow::disConnectCloseSerial,
			serial, &SuoweiSerialPort::recvdisConnectCloseSerial);



    //连接开始灌注动作
    connect(serial,&SuoweiSerialPort::openequipmentCleanActive,this,[=](){
        if(!m_MachineAlreadyInitCleanned){
            FirstConnectCleanEquipment(); //仪器首次连接&&未清洗
        }
    });


    connect(serial,&SuoweiSerialPort::BufferFullState,this,[=](int _addr){
        QString reminder_;
        QUIUtils::reminderfullcoder(_addr,reminder_);
        ThreadSafeReminder("指令异常",reminder_);
    });


    //串口开关状态
    connect(serial,&SuoweiSerialPort::connectionStateChanged,
            this,&MainWindow::slotconnectionStateChanged);

	connect(serial, &SuoweiSerialPort::prepareReconnectGetData,
            this, &MainWindow::recvprepareReconnectGetData);
	


    //串口异常消息提示
    connect(serial,&SuoweiSerialPort::outRminderWidget,
            this,[=](QString title, QString reminderInfo){
        ThreadSafeReminder(title,reminderInfo);
        return;
    });



    //动作异常弹出提示框
    connect(serial,&SuoweiSerialPort::Execute_exception_prompt,this,[=](const quint8 Index,
            const QString ReminderStr, const quint8 IndexError)
    {
        emit ReminderTextOut(Index,ReminderStr);
        if(Index == ERRORLOG)
            CreatReminderWidget(2,"命令异常提示",ReminderStr,IndexError);
        return;

    });
    return;
}

void MainWindow::_creatbeginreadmodule()
{
    QLOG_DEBUG() << "UI thread id:" << QThread::currentThreadId();
    m_moduletimerThread = new moduletimerThread();
    m_moduletimerThread->moveToThread(&m_threadModule); //使用moveToThread创建子线程的
    connect(&m_threadModule, SIGNAL(started()), m_moduletimerThread, SLOT(onCreateTimer())); //在线程启动的时候创建定时器
    connect(&m_threadModule, &QThread::finished, m_moduletimerThread, &QObject::deleteLater);
    //connect(m_moduletimerThread,&moduletimerThread::sendCtrlSignal,this,&MainWindow::_ObtainModuleData,Qt::QueuedConnection);
    connect(this,&MainWindow::stopObatinMachineInfo,
            m_moduletimerThread,&moduletimerThread::recvStopObatinMachineInfo);

    connect(this,&MainWindow::aNewconnectMachine,
            m_moduletimerThread,&moduletimerThread::recvaNewconnectMachine);
    m_threadModule.start();
    return;
}

void MainWindow::_ObtainModuleData()
{
    if(!cglobal::gserialConnecStatus) return;

    quint8 index_equipment = 0;
    QByteArray* _pModuleDataarry = NULL;
    Monitor_TrayTest*_pobtainModuledata = FullyAutomatedPlatelets::pinstanceobtainModuledata();
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,index_equipment);
    _pModuleDataarry = new QByteArray[index_equipment + 1];
    for(int i = 0; i <= index_equipment; i++)
    {
        _pobtainModuledata->_sycnmoduledata(i + 1,_pModuleDataarry[i]);
        QThread::msleep(TIME_SEND_MODULECODER);
        emit sendOneDirectives(_pModuleDataarry[i],"获取模组数据");
        //QLOG_DEBUG()<<"定时获取模组数据线程id:" << QThread::currentThreadId();
    }
    delete [] _pModuleDataarry;
}

//定时读取模组消息
void MainWindow::Traverse_through_ModuleReadInfo()
{
    //定时检测模组数据MODULE_DATA
    Monitor_TrayTest*_pobtainModuledata = FullyAutomatedPlatelets::pinstanceobtainModuledata();
    _pobtainModuledata->_start(); //开启线程

    connect(_pobtainModuledata,&Monitor_TrayTest::sendreadModuleCommd,
            mlocalSerial.data(),
            &SuoweiSerialPort::slotwrite_instructions_group);

    //MachineSetting* pequipmentconfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();

    connect(this,&MainWindow::controlallchnstate,
            _pobtainModuledata,
            &Monitor_TrayTest::open_orclose_chn);

    return;
}


/*主界面UI显示仪器连接状态*/
void  MainWindow::updateConnectionUI(bool connected)
{
    if(!m_iconsLoaded) return;

    // 设置状态图标
    ui->label_machineStaus_show->setPixmap(connected ? m_connectedIcon : m_disconnectedIcon);

    // 配置文本样式
    QPalette palette;
    const QString stateText = connected ? tr("仪器状态:已连接")
                                        : tr("仪器状态:离线");
    const QColor textColor = connected ? Qt::black : Qt::red;

    // 应用样式设置
    palette.setColor(QPalette::WindowText, textColor);
    ui->label_connectstate->setPalette(palette);
    ui->label_connectstate->setText(stateText);
}

void  MainWindow::slotconnectionStateChanged(bool connected)
{
    // 优先更新全局状态（如果需要）
    cglobal::gserialConnecStatus = connected;

    // 更新UI元素
    updateConnectionUI(connected);

    // 触发声音提醒
    emit AlarmReminderSound(!connected);
}


bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    // 定义Windows消息类型标识符（Qt内部使用）
    static const QByteArray sWindowsEventType = QByteArrayLiteral("windows_generic_MSG");

    // 仅处理Windows平台消息
    if (eventType != sWindowsEventType) {
        return QMainWindow::nativeEvent(eventType, message, result);
    }

    // 安全类型转换
    const MSG* const windowsMsg = reinterpret_cast<MSG*>(message);

    // 处理WM_SHOWWINDOW消息
    if (windowsMsg->message == WM_SHOWWINDOW) {
        // 显式转换BOOL类型参数（确保正确处理所有Windows定义值）
        const bool shouldShow = (windowsMsg->wParam != FALSE);

        /* 同步窗口状态前检查当前状态
         * 避免不必要的状态变更导致的递归调用 */
        if (isVisible() != shouldShow) {
            setVisible(shouldShow);
        }

        // 标记消息已处理
        return true;
    }

    // 其他消息交由基类处理
    return QMainWindow::nativeEvent(eventType, message, result);

}

bool MainWindow::getbAxisSycnFinished()
{
    bool baxisSycnFinished = false; //坐标同步成功
    SingletonAxis::GetInstance()->sycnAxisState(READ_OPERRAT,baxisSycnFinished);
    return baxisSycnFinished;
}


void MainWindow::CreatActionExecution()
{

    USB_InitConnect* mpSingleactive = FullyAutomatedPlatelets::pinstanceSingleactive();
    SuoweiSerialPort* mpserialusbinfo = FullyAutomatedPlatelets::pinstanceserialusb();

    connect(mpSingleactive,&USB_InitConnect::writeCommand,
            mpserialusbinfo,&SuoweiSerialPort::writedataToEquipment,
            Qt::QueuedConnection);

    connect(this,&MainWindow::CeratActionDate,
            mpSingleactive,&USB_InitConnect::slotCeratActionDate,
            Qt::QueuedConnection);

    connect(mpserialusbinfo,&SuoweiSerialPort::serialrecvedata,
            mpSingleactive,&USB_InitConnect::Recv_serialdata,
            Qt::QueuedConnection);

    //开机清洗
    connect(mpSingleactive,&USB_InitConnect::bootInitCleanFinished,
            this,&MainWindow::slotbootInitCleanFinished,
            Qt::QueuedConnection);

     //清洗进度
    connect(mpSingleactive,&USB_InitConnect::CleaningProgress,
            this,&MainWindow::slotCleaningProgress);

     //单个任务断线重连
     connect(mpserialusbinfo,&SuoweiSerialPort::Disconnectandreconnect,
             mpSingleactive,&USB_InitConnect::slotDisconnectandreconnect);

     //复位按钮完成复位
     connect(mpSingleactive,&USB_InitConnect::btn_resetComplete,this,[=](){
         ui->toolButton_quality_reset->setEnabled(true);
         QLOG_DEBUG()<<"按钮复位完成!";
     });

     connect(mpSingleactive,&USB_InitConnect::finishviewdownheigh,
             this,[this](int index_){
                FullyAutomatedPlatelets::pinstanceequipmentconfig()
                        ->_finishmovetestdownhigh(index_);
     });

     //按钮清洗完成
     connect(mpSingleactive,&USB_InitConnect::btn_CleanComplete,this,[=]()
     {
         //按钮清洗完成
         FullyAutomatedPlatelets::pinstanceinstrument()->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);

     });

     FullyAutomatedPlatelets::pinstanceSingleactive()->_startthread();
}


void MainWindow::checkTrayAvailability()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(this,
                            tr("System Tray Unavailable"),
                            tr("This application requires system tray support."));
        QCoreApplication::exit(EXIT_FAILURE);
    }
}
void MainWindow::setupTrayMenuStyle()
{
    m_trayMenu->setStyleSheet(R"(
        QMenu {
            background: rgb(253,253,254);
            padding: 10px;
            border-radius: 5px;
            font-family: "Microsoft Yahei", "楷体", serif;
            font-size: 14pt;
        }
        QMenu::item {
            padding: 10px 35px 8px 18px;
            background-color: transparent;
            color: #333333;
            min-width: 120px;
        }
        QMenu::item:hover, QMenu::item:selected {
            background-color: rgb(220,220,220);
        }
    )");
}
void MainWindow::createTrayActions()
{
    // 使用动作容器管理生命周期
    QList<QAction*> actions;

    // 消息通知
    auto* messageAction = new QAction(tr("系统消息状态"), this);
    connect(messageAction, &QAction::triggered, [this] {
        m_systemTray->showMessage(tr("System Status"),
                                tr("Automated Analysis System Ready"),
                                QSystemTrayIcon::Information,
                                TRAY_MESSAGE_DURATION);
    });
    actions.append(messageAction);

    // 窗口控制动作
    struct WindowAction {
        QString text;
        std::function<void()> callback;
    };

    const QList<WindowAction> windowActions = {
        { tr("显示窗口"), [this]{ showMaximized(); } },
        { tr("最小化到托盘"), [this]{ hide(); } },
        { tr("打开设置"), [this]{ openSettingsDialog(); } }
    };

    // 批量创建动作
    for (const auto& action : windowActions) {
        auto* act = new QAction(action.text, this);
        connect(act, &QAction::triggered, action.callback);
        actions.append(act);
    }

    // 退出动作
    auto* exitAction = new QAction(tr("退出"), this);
    connect(exitAction, &QAction::triggered, [] {
        qApp->closeAllWindows();
    });
    actions.append(exitAction);

    // 构建菜单结构
    m_trayMenu->addActions(actions.mid(0, 1));       // 消息通知
    m_trayMenu->addSeparator();
    m_trayMenu->addActions(actions.mid(1, 2));       // 窗口控制
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(actions[3]);               // 设置
    m_trayMenu->addSeparator();
    m_trayMenu->addAction(actions.last());           // 退出

    m_systemTray->setContextMenu(m_trayMenu);
}
void MainWindow::connectTraySignals()
{
    // 托盘激活事件
    connect(m_systemTray, &QSystemTrayIcon::activated, this,
        [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick) {
                showMaximized();
            }
        });

    // 消息点击处理
    connect(m_systemTray, &QSystemTrayIcon::messageClicked, this, [] {
        QDesktopServices::openUrl(QUrl("app:notifications"));
    });
}
void MainWindow::initTrayFunction()
{
    // 检查系统托盘可用性
    checkTrayAvailability();

    // 初始化托盘图标
    m_systemTray = new QSystemTrayIcon(QIcon(":/Picture/suowei.png"), this);
    m_systemTray->setToolTip(tr("全自动血小板聚集分析系统"));

    // 创建托盘菜单和动作
    m_trayMenu = new QMenu(this);
    setupTrayMenuStyle();
    createTrayActions();

    // 事件连接
    connectTraySignals();

    // 显示托盘和主窗口
    m_systemTray->show();
    showMaximized();
    return;
}



void MainWindow::resizeEvent(QResizeEvent *event)
{
   QWidget::resizeEvent(event);
}

void MainWindow::InitMainUiLayout()
{
    //2025/07/18 加载样式表初始状态
    QFile styleFile(":/Picture/SetPng/initToolBtn.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
      minitTopBtnQss = QLatin1String(styleFile.readAll());
      styleFile.close();
    }
    for (auto* btn : ui->widget_top->findChildren<QToolButton*>()) {
        btn->setStyleSheet(minitTopBtnQss); // 仅应用到特定按钮
    }
    //暂停状态
    QFile styleFilePause(":/Picture/SetPng/continueState.qss");
    if(styleFilePause.open(QIODevice::ReadOnly)) {
      mPauseSateQss = QLatin1String(styleFilePause.readAll());
      styleFilePause.close();
    }


    /*隐藏添加任务进度条*/
    initProgressBar("初始加载进度:",false,false);

    /*仪器耗材初始化*/
    minstrumentConsumables = FullyAutomatedPlatelets::pinstanceinstrument();

    //测试界面
    mptesting = FullyAutomatedPlatelets::pinstanceTesting();
    mptesting.data()->initequipmentKind(minstrumentType);

    //设置界面
    FullyAutomatedPlatelets::pinstanceequipmentconfig()->sycnequipmentKind(minstrumentType);
    FullyAutomatedPlatelets::pinstanceequipmentconfig()->_initpara();

    //测试曲线
    m_graphplot = new GraphPlot(this);
    m_graphplot.data()->innitKindequipment();

    MachineSetting*pequipmentconfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();
    connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::sendReminder,pequipmentconfig,&MachineSetting::slotsendReminder);


    connect(FullyAutomatedPlatelets::pinstanceequipmentconfig(),&MachineSetting::sycnViewCurvePara,
            m_graphplot.data(),&GraphPlot::recvsycnViewCurvePara);


    FullyAutomatedPlatelets::pinstancepatientdata()->initstyle();


    //开始按钮
    QToolButton *pbeginTest = ui->toolButton_quality_start;

    QObject::connect(pbeginTest,&QToolButton::clicked,this,[=](){

        emit ReminderTextOut(PROMPTLOG,"点击开始测试按键");
        if(!cglobal::gserialConnecStatus){
            ThreadSafeReminder("开始失败","仪器未连接!");
            return;
        }
        bool hadwaittesttasknum = false; //有待测任务
        equipment_will_test_num(hadwaittesttasknum);
        if(!hadwaittesttasknum) return;
        _begingTesting();
    });

    _setupMainInterface();

    ui->tabWidget_Main->setCurrentIndex((int)MainInterfaceSubscripted::FunctionBlock_Testing); //默认显示主页
    GlobalData::mainseledview(ui->toolButton_sampleTest_1,true);
    return;
}

//控温时显示模组温度
void MainWindow::displayPara(int Indexmodul, const double tempvalve){
    Recv_Module_temperature(Indexmodul,tempvalve);
}


void MainWindow::ChannelValueshow(QStringList moduleData)
{
    emit syncModuleChannelData(moduleData);
}


void MainWindow::initTestPprHeight(){

    static QPointer<opencvfindHeigh> safeAltimeterTrigger = mAltimetertrigger;

    if (!mAltimetertrigger/*safeAltimeterTrigger.isNull()*/) {
        mAltimetertrigger = new opencvfindHeigh();
    }

    safeAltimeterTrigger = mAltimetertrigger;

    mAltimetertrigger->moveToThread(&mTesthighThread);

    connect(&mTesthighThread,&QThread::started,mAltimetertrigger,&opencvfindHeigh::Start);
    connect(&mTesthighThread,&QThread::finished,mAltimetertrigger,&opencvfindHeigh::deleteLater);

    connect(this,&MainWindow::triggerTestHeight,
            mAltimetertrigger,&opencvfindHeigh::handleTriggerTestHeight
            ,Qt::QueuedConnection);

    connect(mAltimetertrigger,&opencvfindHeigh::FindFailed,this,
            [=](QString titles,QString contentstr){ThreadSafeReminder(titles,contentstr);});

    connect(this,&MainWindow::OpenInstrumentCamera,mAltimetertrigger,
            &opencvfindHeigh::RecvOpenInstrumentCamera
            ,Qt::QueuedConnection);

    connect(mAltimetertrigger,&opencvfindHeigh::FinishTestHigh,
            FullyAutomatedPlatelets::pinstanceTesting(),&Testing::TiggerTestHighdone
            ,Qt::QueuedConnection);

    //测高蜂鸣器提示
    connect(mAltimetertrigger,&opencvfindHeigh::Testheightfinish,
             this,&MainWindow::TestHeightFinish
            ,Qt::QueuedConnection);

    //重测
    connect(FullyAutomatedPlatelets::pinstanceTesting(),&Testing::sendRepPrpheight,
            mAltimetertrigger,&opencvfindHeigh::handleRepPrpheight
            ,Qt::QueuedConnection);

    mTesthighThread.start();

    QEventLoop loop;
    connect(&mTesthighThread, &QThread::started, &loop, &QEventLoop::quit);
    loop.exec();

    initCameras();
}


void MainWindow::recv_MainBoardHeighTigger()
{
    if(!INI_File().GetWholeBloodModel())
    {
        emit ReminderTextOut(PROMPTLOG,tr("血浆模式不支持测高"));
        ThreadSafeReminder("提示异常","血浆模式不支持测高!");
        return;
    }


    QList<QCameraInfo> camerasList = QCameraInfo::availableCameras();
    if(camerasList.isEmpty())
    {
        ThreadSafeReminder("识别异常","仪器无摄像设备!");
        return;
    }

    // 安全发射信号
    if (mAltimetertrigger) {
        emit triggerTestHeight();
    } else {
        QLOG_ERROR() << "测高模块未正确初始化";
    }
    return;

}

void MainWindow::initCameras() {
    QList<QCameraInfo> availableCameras;
    availableCameras = QCameraInfo::availableCameras();
    QCameraInfo instrumentCamera;

    foreach (const QCameraInfo &cameraInfo, availableCameras) {
        QString devName = cameraInfo.deviceName().toLower();
        QString desc = cameraInfo.description().toLower();
        const QStringList keys = {"hd camera" };
        bool isUSB = std::any_of(keys.begin(), keys.end(), [&](const QString& key) {
            return  desc.contains(key);
        });
        if (isUSB) {
            instrumentCamera = cameraInfo;
            break;
        }
    }

    //QLOG_DEBUG()<<"可用摄像头"<<availableCameras<<"QCameraInfo"<<instrumentCamera;
    if(availableCameras.size() == 1){
       emit OpenInstrumentCamera(OPEN_DEFAULT);
    }else if(availableCameras.size() >= 2){
        if(!instrumentCamera.isNull()){
            emit OpenInstrumentCamera(OPEN_EXTERNAL);
        }
    }else{
        QCameraInfo defaultCam = QCameraInfo::defaultCamera();
        foreach (const QCameraInfo &cam, availableCameras) {
           if (cam != defaultCam) {
               emit OpenInstrumentCamera(OPEN_EXTERNAL);
               break;
           }
        }
    }
}



void MainWindow::PromptInfo(const quint8 Index, const QString ReminderStr, const quint8 iActive)
{
    slotExecute_exception_prompt(Index, ReminderStr, iActive);
}


void MainWindow::handlecardSwipeSuccessful(const QString tips,quint8 indexReagent,quint8 totalnum,quint16 datetime){
    if(mReminder == nullptr){
        mReminder  = new CustomHighData(tips,  CONSUMABLES_READ_SUCESSFULLY,indexReagent, totalnum);
    }
    connect(mReminder.data(),&CustomHighData::AddSupplies,minstrumentConsumables.data(),
                             &QualityControl::sltoAddConsumables);
    connect(mReminder.data(),&CustomHighData::closeDelWidget,this,[=](){
        if(mReminder)
            delete mReminder;
    });
    QLOG_TRACE()<<"刷卡有效期"<<datetime;
    mReminder.data()->show();
}

//提示文字，刷卡试剂、刷卡值
void MainWindow::handleswipeCardSuccessfullyWritten(QString tips, int addindexReag, quint8 addBottle)
{
    ThreadSafeReminder("充值成功",tips);
    emit rechargesuccessful(addindexReag,addBottle);
    return;
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();
    QString outputtext;
    QList<QString> btnText;
    btnText<<"取消退出"<<"清洗后退出"<<"确定退出";
    if(cglobal::g_StartTesting)
       outputtext = ("样本测试中...强行退出可能导致测试异常,请等待测试完成!");
    else
       outputtext = ("确定退出并关闭软件?");
    _reminderFunctionWidget("关闭软件",outputtext,btnText);
}


bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    return QMainWindow::eventFilter(obj, ev);
}




void MainWindow::_displayHandoffUser()
{
    QScopedPointer<handoff_staff> dialog(new handoff_staff());
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    if (!dialog.data() || !QApplication::desktop() ) return;
    QRect screenGeometry = QApplication::desktop()->availableGeometry(this);
    dialog.data()->move((screenGeometry.width() - dialog.data()->width()) / 2,
        (screenGeometry.height() - dialog.data()->height()) / 2
    );

    dialog->setWindowModality(Qt::ApplicationModal);

    dialog->show();
    dialog->raise();
    dialog->activateWindow();

    // 释放所有权，由WA_DeleteOnClose和QScopedPointer共同管理生命周期
    dialog.take(); // ✅ 此处take()是QScopedPointer的合法操作
}


void  MainWindow::paintEvent(QPaintEvent *event)
{
    QMainWindow::paintEvent(event);
}


void MainWindow::_setupMainInterface()
{
    // 初始化选项卡
    setupTabWidget();

    // 配置功能按钮
    setupFunctionButtons();

    // 配置辅助按钮
    setupUtilityButtons();


}

void MainWindow::setupTabWidget()
{
    struct TabInfo {
        MainInterfaceSubscripted index;
        QWidget* widget;
    };

    const QList<TabInfo> tabs = {
        {MainInterfaceSubscripted::FunctionBlock_Testing,       mptesting.data()},
        {MainInterfaceSubscripted::FunctionBlock_PreviewResults, FullyAutomatedPlatelets::pinstancepatientdata()},
        {MainInterfaceSubscripted::FunctionBlock_PreviewCurve,   m_graphplot.data()},
        {MainInterfaceSubscripted::FunctionBlock_Inquire,        FullyAutomatedPlatelets::pinstanceInquiredata()},
        {MainInterfaceSubscripted::FunctionBlock_Consumables,    FullyAutomatedPlatelets::pinstanceinstrument()}
    };

    foreach (const auto& tab, tabs) {
        QWidget* container = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(container);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(tab.widget);
        ui->tabWidget_Main->insertTab(static_cast<int>(tab.index), container, "");
    }
}



void MainWindow::setupFunctionButtons()
{


    const QList<QPair<QToolButton*, QString>> buttonInfo = {
        {ui->toolButton_sampleTest_1,    tr("样本测试")},
        {ui->toolButton_resultView_2,    tr("结果预览")},
        {ui->toolButton_testingCurve_3, tr("测试曲线")},
        {ui->toolButton_TestingResult_4,tr("结果查询")},
        {ui->toolButton_ControlSuppile_5,tr("耗材管理")}
    };

    for (int i = 0; i < buttonInfo.size(); ++i) {
        QToolButton* btn = buttonInfo[i].first;
        const QString& text = buttonInfo[i].second;



        // 设置按钮状态
        GlobalData::mainseledview(btn, false);

        // 设置显示文本
        btn->setText(text);

        // 连接信号（直接绑定索引）
        connect(btn, &QToolButton::clicked, this, [this, i] {
            switchMainTab(static_cast<MainInterfaceSubscripted>(i));
            handleFunctionButtonClick(static_cast<int>(i));
        });
    }
}

void MainWindow::switchMainTab(MainInterfaceSubscripted index)
{
    const int tabIndex = static_cast<int>(index);
    if (tabIndex >= 0 && tabIndex < ui->tabWidget_Main->count()) {
        ui->tabWidget_Main->setCurrentIndex(tabIndex);
        highlightActiveButton(tabIndex);
    }
}

void MainWindow::highlightActiveButton(int index)
{
    // 获取所有功能按钮的列表
    const QList<QToolButton*> buttons = {
        ui->toolButton_sampleTest_1,
        ui->toolButton_resultView_2,
        ui->toolButton_testingCurve_3,
        ui->toolButton_TestingResult_4,
        ui->toolButton_ControlSuppile_5
    };

    // 更新按钮高亮状态
    for (int i = 0; i < buttons.size(); ++i) {
        GlobalData::mainseledview(buttons[i], i == index);

    }
}

void MainWindow::setupUtilityButtons()
{
    // 设备设置按钮
    connect(ui->toolButton_EquipmentSetting, &QPushButton::clicked,this,
            [this]{ openSettingsDialog(); });

    // 关于按钮
    connect(ui->toolButton_about, &QPushButton::clicked, this, [this] {
        auto aboutDialog = FullyAutomatedPlatelets::paboutinstance();
        aboutDialog->sycnMd5Value(QCoreApplication::applicationFilePath());
        aboutDialog->sycnVersion(VERSION_RELEASE);
        aboutDialog->sycnEquipment(minstrumentType);

        aboutDialog->sycnSerialname(cglobal::gserialPortName);
        aboutDialog->show();
        aboutDialog->raise();
        aboutDialog->activateWindow();
    });

    //退出
    connect(ui->toolButton_Exit,&QToolButton::clicked,this,[this]{
        emit FullyAutomatedPlatelets::pinstancepatientdata()->hideCurveUi();
        this->close();
    });


    QObject::connect(ui->toolButton_quality_stop ,&QToolButton::clicked,this,[this]{

        handleFunctionButtonClick(static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Testing));
        mptesting.data()->_showaddsamplewidget();
    });
}

void MainWindow::updateButtonSelectionState(int activeIndex)
{
    const QList<QToolButton*> functionButtons = {
        ui->toolButton_sampleTest_1,
        ui->toolButton_resultView_2,
        ui->toolButton_testingCurve_3,
        ui->toolButton_TestingResult_4,
        ui->toolButton_ControlSuppile_5
    };

    // 安全索引检查
    if (activeIndex < 0 || activeIndex >= functionButtons.size()) {
        QLOG_ERROR() << tr("Invalid button index:") << activeIndex;
        return;
    }

    // 批量更新按钮状态
    for (int i = 0; i < functionButtons.size(); ++i) {
        GlobalData::mainseledview(functionButtons[i], i == activeIndex);
    }

    if (static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Consumables) == activeIndex) {
        FullyAutomatedPlatelets::pinstanceinstrument()->displayConsumablesInteger();
        FullyAutomatedPlatelets::pinstanceinstrument()->ShowConsumablesLimitArm();
        //只要打开耗材界面 当总耗材数小于等于报警限 就弹提示
        FullyAutomatedPlatelets::pinstanceinstrument()->TotalConsumablesAlarm();
    }
}

void MainWindow::logTabSwitchEvent(int index)
{
    static const QMap<int, QString> tabLogMessages = {
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Testing),     tr("显示测试界面")},
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewResults), tr("打开结果预览")},
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewCurve),  tr("打开曲线界面")},
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Inquire),      tr("打开查询界面")},
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Consumables),  tr("进入耗材管理")}
    };

    if (tabLogMessages.contains(index)) {
        QLOG_DEBUG() << tabLogMessages[index];
    } else {
        QLOG_WARN() << tr("未知标签页索引:") << index;
    }
}

void MainWindow::handleFunctionButtonClick(const int indexed)
{

    // 验证索引有效性
    if (indexed < 0 || indexed >= ui->tabWidget_Main->count()) {
        QLOG_WARN() << tr("Invalid tab index:") << indexed;
        return;
    }

    // 设置当前标签页
    ui->tabWidget_Main->setCurrentIndex(indexed);

    // 更新按钮状态
    updateButtonSelectionState(indexed);

    // 记录操作日志
    logTabSwitchEvent(indexed);
}

void MainWindow::openSettingsDialog()
{
	auto* pEquipment = FullyAutomatedPlatelets::pinstanceequipmentconfig();
	if (!pEquipment) return;

	// Ensure dialog is properly shown
	if (pEquipment->isHidden() || pEquipment->isMinimized()) {
		pEquipment->showNormal();
		pEquipment->raise();
		pEquipment->activateWindow();
	}
	else {
		// If already visible but not focused, just bring to front
		pEquipment->raise();
		pEquipment->activateWindow();
	}

	pEquipment->updatepara(cglobal::g_StartTesting);
	pEquipment->show();

	// Connect signals only once
	if (!m_bmachineconfigureSignal) {
		auto* pPatient = FullyAutomatedPlatelets::pinstancepatientdata();
		if (!pPatient || !mlocalSerial) return;

		// Connect all signals in one clear block
		connect(pEquipment, &MachineSetting::SetParatoInstrument,
			this, [this](const QByteArrayList& d, QString s) {
			emit _sendcodeList(d, s);
		});

        connect(pEquipment, &MachineSetting::testdownheight,
			this, [this](QByteArrayList d, int i) {
			emit CeratActionDate(i, d);
		});

		// State synchronization connections
		connect(pEquipment, &MachineSetting::Synchronizeupdates,
			pPatient, &Calibrate::updatecommboxInfo);

		connect(pEquipment, &MachineSetting::OpenChannelMotor,
			this, [this](quint8 i, bool b) {
			emit _controlmotorrunning(i, b);
		});

		connect(pEquipment, &MachineSetting::controlallchn,
			this, [this](bool b) {
			emit controlallchnstate(b);
		});

		connect(pEquipment, &MachineSetting::pauseConnectModule,
			this, [this](bool b) {
			pauseObtainmodulecommand("设置界面保存模组", b);
		});

		pEquipment->openKeyboard();
		m_bmachineconfigureSignal = true;
	}
}


bool MainWindow::requiresTemperatureControl(quint8 equipmentType){

    constexpr float kBaseTemp = 36.50f;
    const std::unordered_map<quint8, std::vector<QString>> kDeviceModules {
        {KS600,  { "label_showModule1_value" }},
        {KS800,  { "label_showModule1_value", "label_showModule2_value_2" }},
        {KS1200, { "label_showModule1_value", "label_showModule2_value_2", "label_showModule3_value_2" }}
    };

    const auto& modules = kDeviceModules.find(equipmentType);
    if (modules == kDeviceModules.end()) return false;

    auto checkTemp = [this, kBaseTemp](const QString& label) {
        return ui->StatusBar->findChild<QLabel*>(label)->text().toDouble() < kBaseTemp;
    };

    return std::any_of(modules->second.begin(), modules->second.end(), checkTemp);
}


//打印
//void MainWindow::on_toolButtonr_esult_tool_print_clicked()
//{
//    PrintPatientInfo InfoPatient;
//    QCustomPlot *pCurveWidget = nullptr;
//    QMap<quint8,QString> ntestResult;


//    Calibrate* _ppatientdata = FullyAutomatedPlatelets::pinstancepatientdata();
//    _ppatientdata->sycnPatientInfoStuPrint(InfoPatient,pCurveWidget,ntestResult);

//    QMap<quint8,QStringList> filterStr;
//    filterStr.clear();
//    if(pCurveWidget != nullptr)
//    {
//        auto iter = ntestResult.begin();
//        while(iter != ntestResult.end())
//        {
//            QStringList _strList = iter.value().split(",");
//            QStringList _ssList;
//            _ssList.clear();
//            for(int j = 0 ; j < _strList.size() ; j++)
//            {
//                QString tmpstr = _strList.at(j);
//                QUIUtils::parseDataratio(tmpstr);
//                double testvalue = tmpstr.toDouble();
//                if(testvalue == 0){
//                   _ssList.push_back(_strList.at(j));
//                }
//            }
//            int index = _ssList.size();
//            if(index != 4)
//            {
//                filterStr.insert(iter.key()/4 + 1,_strList);
//            }
//            iter++;
//        }
//        OutPrintReport::getPrinterInstance()->creat_PdfView(InfoPatient,pCurveWidget,filterStr);
//    }
//    else
//    {
//        QLOG_ERROR()<<"输出PDF失败!";
//    }
//    OutPrintReport::getPrinterInstance()->delPrinterInstance();
//    return;

//    bool canPrint = false;  /*emit *///PrintReport();
//    if(canPrint)
//    {
//        QString fileName = QString("%1/%2/%3.exe").arg(QCoreApplication::applicationDirPath()).arg("PrintReport").arg("QtReportTest");
//        if(m_processPrint == nullptr)
//            m_processPrint = new QProcess(this);
//        m_processPrint->setProgram(fileName);
//        m_processPrint->start();
//    }
//    return;
//}


QWidget* findTopWidgetUnderCursor(const QPoint &logicalPos) {
    QWidgetList topLevels = QApplication::topLevelWidgets();
    foreach (QWidget *top, topLevels) {
        if (top->geometry().contains(logicalPos) && top->isVisible()) {
            // 在顶层窗口内继续查找子控件
            QWidget *child = top->childAt(logicalPos - top->geometry().topLeft());
            return child ? child : top;
        }
    }
    return nullptr;
}

void MainWindow::on_sStatusInfo_clicked()
{
   QPoint globalClickPos = ui->sStatusInfo->mapToGlobal(QPoint(0,0));
   QPoint movepos;
   movepos.setX(globalClickPos.x() - OUT_UI_SIZE_WIDTH - 1 + ui->sStatusInfo->width() + 5);
   movepos.setY(globalClickPos.y() - OUT_UI_SIZE_HEIGHT - 10);
   FullyAutomatedPlatelets::pinstanceinfowidget()->move(movepos);
   FullyAutomatedPlatelets::pinstanceinfowidget()->show();
   update();
   return;
}




//按钮==清洗
void MainWindow::on_toolButton_quality_cleaning_clicked()
{
    if(!cglobal::gserialConnecStatus) return;

    QByteArrayList CleanBtnarry; CleanBtnarry.clear();
    QUIUtils::cleaningDoubleNeedleAction(CleanBtnarry,1);  //清洗(吸清洗剂吐出)
    emit  CeratActionDate(EQUIPMENT_BTN_CLEAN,CleanBtnarry);// 按钮清洗
    return;
}

//复位
void MainWindow::on_toolButton_quality_reset_clicked()
{
    if(!cglobal::gserialConnecStatus) return;
    QByteArrayList btn_originloc;
    QUIUtils::_equipmentbackoriginloc(btn_originloc);
    emit CeratActionDate(EQUIPMENT_BTN_REAET,btn_originloc);
    return;
}


void MainWindow::machineReposition()
{
    on_toolButton_quality_reset_clicked();
}


//查看日志
void MainWindow::on_sStatusInfo_text_clicked()
{
    // 获取日志窗口单例
    if (auto* logWindow = FullyAutomatedPlatelets::pinstanceLogfile())
    {
        // 使用现代屏幕API替代废弃的desktop()
        const auto* screen = QGuiApplication::primaryScreen();
        const QRect screenGeometry = screen->availableGeometry();

        // 计算居中坐标
        const int x = (screenGeometry.width() - logWindow->width()) / 2;
        const int y = (screenGeometry.height() - logWindow->height()) / 2;

        // 设置窗口属性
        logWindow->move(x, y);
        logWindow->setWindowModality(Qt::NonModal);  // 显式设置非模态
        logWindow->show();

        // 激活窗口（可选）
        logWindow->activateWindow();
        logWindow->raise();
    }
    else
    {
        QLOG_ERROR() << "Log window instance is not available";
    }
    return;
}


/*耗材试剂消耗同步界面显示*/
void MainWindow::DisplaySycnMainUiLosserSuppile(int IndexReagent,double Remainingratio)
{
    // 常量定义
    constexpr quint8 kCriticalThreshold = 0; // 预设阈值
    constexpr int kPrecision = 2;           // 显示精度
    const QColor kNormalColor =  Qt::black;  // 常规颜色
    const QColor kWarningColor = Qt::red;    // 警告颜色

    // 获取试剂阈值配置
    quint8 threshold = kCriticalThreshold;
    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,IndexReagent,threshold);
    QLOG_DEBUG()<<"试剂:"<<GlobalData::mapReagentNames(IndexReagent)<<"报警限制值"<<threshold;

    // 查找对应显示控件
    auto labelIt = displayRatioLab.constFind(IndexReagent);
    if (labelIt != displayRatioLab.constEnd() && *labelIt) {
        QLabel* displayLabel = *labelIt;

        bool isCritical = false;
        if (Remainingratio > 0 && Remainingratio >= threshold)
            isCritical = false;
        else if (qFuzzyIsNull(Remainingratio) || Remainingratio < threshold)
            isCritical = true;

        // 原子化UI更新
        QSignalBlocker blocker(displayLabel);

        QPalette palette = displayLabel->palette();
        palette.setColor(QPalette::WindowText, isCritical ? kWarningColor : kNormalColor);
        displayLabel->setPalette(palette);

        // 构建显示文本
        const QString originalText = displayLabel->text();
        const QString baseText = originalText.section(':', 0, 0).trimmed();
        const QString displayValue = QString::number(Remainingratio, 'f', kPrecision);

        const QString displayText = baseText.isEmpty() ?
            QString("%1%").arg(displayValue) :
            QString("%1: %2%").arg(baseText, displayValue);
            displayLabel->setText(displayText);
    }
    return;
}


void MainWindow::recvNoinitialCleaning()
{
    if(!m_MachineAlreadyInitCleanned)
        FirstConnectCleanEquipment();
    return;
}


//耗材报警提示
void  MainWindow::ReminderPauseConsumables_alarm(const int Index)
{
    // 定义耗材报警信息表
    static const QHash<int, QPair<QString, QString>> consumableAlarmMap = {
        {INDEX_AA_CONSUMABLE,          {tr("AA报警"),      tr("AA(L)试剂余量不足,请更换试剂再开始!")}},
        {INDEX_AA_1_CONSUMABLE,        {tr("AA报警"),      tr("AA(R)试剂余量不足,请更换AA试剂!")}},
        {INDEX_ADP_CONSUMABLE,         {tr("ADP报警"),     tr("ADP(L)试剂余量不足,请更换ADP试剂!")}},
        {INDEX_ADP_1_CONSUMABLE,       {tr("ADP报警"),     tr("ADP(R)试剂余量不足,请更换ADP试剂!")}},  // 修复：原代码中的"AA(R)"错误
        {INDEX_EPI_CONSUMABLE,         {tr("EPI报警"),     tr("EPI(L)试剂余量不足,请更换EPI试剂!")}},
        {INDEX_EPI_1_CONSUMABLE,       {tr("EPI报警"),     tr("EPI(R)试剂余量不足,请更换EPI试剂!")}},
        {INDEX_COL_CONSUMABLE,         {tr("COL报警"),     tr("COL(L)试剂余量不足,请更换COL试剂!")}},  // 修复：原代码中的"更COL换试剂"错误
        {INDEX_COL_1_CONSUMABLE,       {tr("COL报警"),     tr("COL(R)试剂余量不足,请更换试剂!")}},
        {INDEX_RIS_CONSUMABLE,         {tr("RIS报警"),     tr("RIS(L)试剂余量不足,请更换RIS试剂!")}},
        {INDEX_RIS_1_CONSUMABLE,       {tr("RIS报警"),     tr("RIS(R)试剂不足,请更换RIS试剂!")}},
        {INDEX_CLEANLINQUE_CONSUMABLE, {tr("清洗液不足报警"), tr("清洗液余量不足,请更换!")}}
    };

    // 查找报警信息
    if (consumableAlarmMap.contains(Index)) {
        const auto& info = consumableAlarmMap[Index];
        ThreadSafeReminder(info.first, info.second);
    } else {
        // 处理未知耗材类型
        QLOG_WARN() << "Unknown consumable index:" << Index;
    }
    return;
}

/*初始化更新耗材显示*/
void MainWindow::DisplaysConsumablesRemaining()
{
    // 常量定义
    constexpr float kMaxRatioSwitch = 100.0f;
    const QColor kWarningColor(178,34, 34);
    const QColor kNormalColor(Qt::black);

    // 初始化控件映射表
    const QVector<QPair<int, QLabel*>> consumableWidgets {
        {INDEX_AA_CONSUMABLE,          ui->label_Dipaly_AA_1},
        {INDEX_AA_1_CONSUMABLE,        ui->label_Dipaly_AA_2},
        {INDEX_ADP_CONSUMABLE,         ui->label_Dipaly_ADP_1},
        {INDEX_ADP_1_CONSUMABLE,       ui->label_Dipaly_ADP_2},
        {INDEX_EPI_CONSUMABLE,         ui->label_Dipaly_ADR_1},  // EPI
        {INDEX_EPI_1_CONSUMABLE,       ui->label_Dipaly_ADR_2},
        {INDEX_COL_CONSUMABLE,         ui->label_Dipaly_COL_1},
        {INDEX_COL_1_CONSUMABLE,       ui->label_Dipaly_COL_2},
        {INDEX_RIS_CONSUMABLE,         ui->label_Dipaly_RIS_1},
        {INDEX_RIS_1_CONSUMABLE,       ui->label_Dipaly_RIS_2},
        {INDEX_CLEANLINQUE_CONSUMABLE, ui->label_Dipaly_cleanlinque}
    };

    // 构建显示控件映射
    for(const auto& pair : consumableWidgets){
        int index = pair.first;
        QLabel* widget = pair.second; // 通过 second 访问值
        if (widget) {
            displayRatioLab.insert(index, widget);
        }
    }

    // 更新显示内容
    for (auto it = displayRatioLab.constBegin(); it != displayRatioLab.constEnd(); ++it) {
           const int reagentIndex = it.key();
           QLabel* displayLabel = it.value();

           if (!displayLabel) continue;

           // 获取耗材数据
           quint8 threshold = 0;
           quint16 currentRatio = 0;
           ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT, reagentIndex, threshold);
           ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT, reagentIndex, currentRatio);

           // 计算显示比例
           const float ratio = static_cast<float>(currentRatio) * kMaxRatioSwitch / MAX_RATIO_SWITCT;
           const bool isWarning = ratio <= threshold;

           // 构建显示文本
           const QString baseText = displayLabel->text().section(':', 0, 0).trimmed();
           const QString displayValue = QString::number(ratio, 'f', 2);
           const QString fullText = baseText.isEmpty()
                                   ? QString("%1%").arg(displayValue)
                                   : QString("%1: %2%").arg(baseText, displayValue);

           // 更新UI
           QPalette palette = displayLabel->palette();
           palette.setColor(QPalette::WindowText, isWarning ? kWarningColor : kNormalColor);

           QSignalBlocker blocker(displayLabel);
           displayLabel->setPalette(palette);
           displayLabel->setText(fullText);
    }
    return;
}


//机器位置复位
void MainWindow::_backOriginTestFinished()
{
    cglobal::g_StartTesting = false;
    ui->toolButton_quality_start->setEnabled(true);
    ui->toolButton_quality_reset->setEnabled(true);
    ui->toolButton_quality_cleaning->setEnabled(true);


    QByteArrayList btn_originloc;
    QUIUtils::_equipmentbackoriginloc(btn_originloc);
    emit CeratActionDate(EQUIPMENT_BTN_REAET,btn_originloc);

    return;
}

//调试模式发送命令
void MainWindow::SendCode_2_Serial(QByteArrayList arrycode)
{
    emit Thread_5_SendCode(arrycode);
    QThreadPool::globalInstance()->start(WriteParameter.data());
    return;
}

/*测高完成蜂鸣提示*/
void  MainWindow::TestHeightFinish(const bool finished)
{
    emit ConfigUsedBuzzerMark(true);
    auto *ptask = Testing::m_TaskDll;
    QByteArray openarry = ptask->_buzzerControl(true);
    QByteArray closearry = ptask->_buzzerControl(false);

    // 封装蜂鸣动作
    auto beepAction = [=](const QString& log, int duration, int times) {
        int* count = new int(0); // 动态分配计数器
        auto timer = new QTimer;
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [=]() mutable {
               if (*count >= times) {
                   emit ReminderTextOut(PROMPTLOG, log);
                   delete count;
                   timer->deleteLater();
                   return;
               }
            emit sendOneDirectives(openarry, "打开蜂鸣");
            QTimer::singleShot(duration, [=]() {
                emit sendOneDirectives(closearry, "关闭蜂鸣");
                (*count)++;
                timer->start(100); // 间隔100ms后触发下一次蜂鸣
            });
        });
        timer->start(0);
    };

    if (finished) {
        // 成功：单次蜂鸣
        beepAction(tr("测高成功！"), 30,1);
    }
    else {
        beepAction(tr("测高失败！"), 30,3);
        emit ReminderTextOut(PROMPTLOG, tr("测高失败!"));

    }
    emit ConfigUsedBuzzerMark(false);
    return;
}
//TestHeightFinish（）



//点击测试待测项目样本数
void MainWindow::equipment_will_test_num(bool &bgoto_testing)
{
    int had_wait_testsample = 0;
    StructInstance::getInstance()->SampleTotal(had_wait_testsample);
    if(had_wait_testsample <= 0)
    {
        bgoto_testing = false;
        ThreadSafeReminder("开始失败","样本任务为空,请添加测试样本!");
    }
    else
        bgoto_testing = true;
}
void MainWindow::_testingchnEnough(bool &benoughChn){
    quint8 _freeChn = 0;
    benoughChn = StructInstance::getInstance()->hadFreeTestChn(_freeChn);//找空闲通道
}

void MainWindow::_begingTesting()
{
    bool benoughChn_ = false; //用通道空闲
    _testingchnEnough(benoughChn_);
    if(!benoughChn_){
        ThreadSafeReminder("开始失败","模组无可用通道!");
        return;
    }
    mptesting.data()->TotalTaskProgress(); //总任务进度试管杯
    cglobal::g_StartTesting = true; //开始测试标志


    ui->toolButton_quality_start->setEnabled(false);
    ui->toolButton_quality_reset->setEnabled(false);
    ui->toolButton_quality_cleaning->setEnabled(false);
    emit  startTheTestTask();
    return;
}

/*暂停按钮*/
void MainWindow::on_toolButton_quality_sample_clicked()
{
    if(!cglobal::g_StartTesting){
        ThreadSafeReminder(tr("无效暂停操作"),tr("未开始测试暂停无效"));
        return;
    }

    QToolButton *pPauseBtn = ui->toolButton_quality_sample;
    const bool isPaused = cglobal::g_PauseTesting;

    if(m_warmIndexReagent >= INDEX_AA_CONSUMABLE)
    {
        //更换试剂是否完成
        if(!reagentReplacementComplete(m_warmIndexReagent)){
           warn_interface::showTransientWarning(tr("操作失败"),tr("%1未更换新试剂请更换后重试").arg(
                                                    GlobalData::mapReagentNames(m_warmIndexReagent)));
           return;
        }else{
           m_warmIndexReagent = -1;
        }
    }

    cglobal::g_PauseTesting = !isPaused;  // 状态直接反转

    if (isPaused) {  // 当前暂停 → 切换到继续
        pPauseBtn->setText(tr("暂停"));
        pPauseBtn->setIcon(m_pauseIcon);
        pPauseBtn->setStyleSheet(minitTopBtnQss);

        emit resumeaTestTask();
        emit ReminderTextOut(PROMPTLOG, tr("测试继续"));
    } else {  // 当前继续 → 切换到暂停
        pPauseBtn->setText(tr("继续"));
        pPauseBtn->setIcon(m_continueIcon);
        pPauseBtn->setStyleSheet(mPauseSateQss);
        emit ReminderTextOut(PROMPTLOG, tr("测试暂停"));
    }
    return;
}

bool MainWindow::reagentReplacementComplete(const int& indexReagent){

    quint16 allowanceRatio = 0; //剩余比例
    ConsumablesOper* consumables = ConsumablesOper::GetpInstance();
    consumables->updateReplaceLocRatio(READ_OPERRAT, indexReagent, allowanceRatio);
    return (allowanceRatio >= MAX_RATIO_SWITCT)? true: false;
}



void MainWindow::testingSuppileLoss(const quint8 index){
   QString warmmReagent =  GlobalData::mapReagentNames(index);
   m_warmIndexReagent = index;
   QString outtext = "耗材:"+ warmmReagent +"余量报警!请更换"+ warmmReagent + "继续测试";
   warn_interface::showTransientWarning(tr("暂停测试"),outtext);

   // 集中UI状态修改
   updateContinueButtonState();
   return;
}


void MainWindow::updateContinueButtonState()
{
    const bool pauseState = cglobal::g_PauseTesting;
    const bool isTesting = cglobal::g_StartTesting;
    QToolButton *isPauseBtn =  ui->toolButton_quality_sample;
    QMutexLocker locker(&m_mutex); // 加锁确保线程安全
    if(isTesting && !pauseState){
        cglobal::g_PauseTesting = true;
        isPauseBtn->setStyleSheet(mPauseSateQss);
        isPauseBtn->setText(tr("继续"));
        isPauseBtn->setIcon(m_continueIcon);
        isPauseBtn->setUpdatesEnabled(true);
    }
}


/*仪器初始化动作 true = 开机初始化|false = 关机清洗*/
void MainWindow::equipmentinitActive(const bool enoughReagent,bool closeClean)
{
    uint perfusion_leng = 0;

    //初始化抓通道杯子
    static const bool binitGrabcups = INI_File().rConfigPara(INITGRABCUPS).toBool();

    //开机灌注清洗双针
    int cleanntime = INI_File().getWashesTime();
    QByteArrayList perfusion = QUIUtils::perfusionAction(perfusion_leng,enoughReagent,cleanntime);

    if(!closeClean && binitGrabcups)
    {
        quint8 index_equipment = KS1200;
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,index_equipment);
        QUIUtils::initequipmentgrabcups(index_equipment,perfusion);
    }
    emit CeratActionDate(EQUIPMENT_BOOT_CLEAN,perfusion);
    return;
}

//串口提示消息
void MainWindow::slotExecute_exception_prompt(const quint8 Index, const QString ReminderStr, const quint8 IndexError)
{
    emit ReminderTextOut(Index,ReminderStr);
    if(Index == ERRORLOG)
    {
        CreatReminderWidget(2,"命令异常提示",ReminderStr,IndexError);
    }
    return;
}

//创建提示框
void MainWindow::CreatReminderWidget(char index,QString titleStr,QString reminderStr,quint8 indexFucn)
{
    if (!m_pReminderExceptional)
    {
        m_pReminderExceptional = new CommandExceptional(index,titleStr);
        connect(m_pReminderExceptional.data(),&CommandExceptional::closeDel,this,&MainWindow::closeReminder);
        if(index == 2){
             //connect(m_pReminderExceptional,&CommandExceptional::ReplaceSend,mlocal_serial,&SuoweiSerialPort::RecveErrorMotorDoneContinue);
        }
        m_pReminderExceptional.data()->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                                                      Qt::WindowStaysOnTopHint |
                                                      Qt::WindowDoesNotAcceptFocus);
        m_pReminderExceptional.data()->setErrInfo(indexFucn,reminderStr);
        m_pReminderExceptional.data()->show();
    }
    return;
}

//关闭提示窗口
void MainWindow::closeReminder()
{
    if(m_pReminderExceptional)
    {
       delete m_pReminderExceptional;
    }
    return;
}

void MainWindow::experimentReadChannel(const quint8& peChannel,const int &index,bool isstart){
    if (isstart) {
        m_indexSec = 0;         // 重置计数器
        m_currentIndex = index;
        m_relChannel = peChannel + 1;

        if (!m_experimentTimer) {
            m_experimentTimer = new QTimer(this);
            m_experimentTimer->setInterval(1000);
            m_experimentTimer->setTimerType(Qt::PreciseTimer);  // 高精度模式
            connect(m_experimentTimer, &QTimer::timeout, this, [=]() {
                int outdata = getChannelTestVal(m_relChannel);
                m_indexSec++;
                switch (m_currentIndex) {
                case THROWPEWATER:  QLOG_DEBUG() << "通道" << m_relChannel << "蒸馏水第" << m_indexSec << "S=" << outdata; break;
                case THROWHEIGHVAL: QLOG_DEBUG() << "通道" << m_relChannel << "高值第" << m_indexSec << "S=" << outdata; break;
                case THROWMIDVAL:   QLOG_DEBUG() << "通道" << m_relChannel << "中值第" << m_indexSec << "S=" << outdata; break;
                case THROWLOWVAL:   QLOG_DEBUG() << "通道" << m_relChannel << "低值第" << m_indexSec << "S=" << outdata; break;
                case THROWBASIC:    QLOG_DEBUG() << "通道" << m_relChannel << "浊度物值第" << m_indexSec << "S=" << outdata; break;

                }
            });
        }
        m_experimentTimer->start();  // 统一启动
    }
    else {
        if (m_experimentTimer) {
            m_experimentTimer->stop();  // 安全停止
        }
    }
}

int MainWindow::getChannelTestVal(const quint8& indexChannel){
    if(mshowModuledata){
       return  mshowModuledata.data()->GetChannelValue(indexChannel);
    }
    return 0;
}

void MainWindow::updateModuleVisibilityBasedOnInstrumentType()
{
    const QMap<int, QWidget*> moduleWidgets = {
        {MODULE_1, ui->widget_ModulI},
        {MODULE_2, ui->widget_ModulII},
        {MODULE_3, ui->widget_ModulIII}
    };

    // 根据设备类型确定可见模块
    QSet<int> visibleModules;
    switch(minstrumentType) {
    case KS600:
        visibleModules = {MODULE_1};
        break;
    case KS800:
        visibleModules = {MODULE_1, MODULE_2};
        break;
    default:  // 其他设备显示所有模块
        visibleModules = {MODULE_1, MODULE_2, MODULE_3};
        break;
    }

    // 统一更新模块状态
    for (auto it = moduleWidgets.cbegin(); it != moduleWidgets.cend(); ++it) {
        const int moduleId = it.key();
        QWidget* widget = it.value();

        const bool shouldShow = visibleModules.contains(moduleId);
        widget->setVisible(shouldShow);

        if (shouldShow) {
            Recv_Module_temperature(moduleId, 0.00); // 初始化温度
        }
    }

    DisplaysConsumablesRemaining(); // 更新耗材显示

    return;
}


void MainWindow::DestructionSerialclass()
{
    QLOG_DEBUG()<<"析构串口类";

    if(mtestmoduleprotocol){ delete mtestmoduleprotocol; mtestmoduleprotocol = nullptr;}

    if(m_ThreadReminderTsetTube){ delete m_ThreadReminderTsetTube; m_ThreadReminderTsetTube = nullptr;}

}



/* 定时检测机器主板信息获取刷卡耗材 MAINBOARD_CONSUMABLES*/
void MainWindow::Traverse_through_motherboard_information()
{
    if(nullptr == m_ObtainMainBoardData){
        m_ObtainMainBoardData = new ObtainMainBoardData();
        m_ObtainMainBoardData->moveToThread(&m_threadMainBoard); //使用moveToThread创建子线程的
        connect(&m_threadMainBoard, SIGNAL(started()), m_ObtainMainBoardData, SLOT(onCreateTimer())); //在线程启动的时候创建定时器
        connect(&m_threadMainBoard, &QThread::finished, m_ObtainMainBoardData, &QObject::deleteLater);
        //connect(m_ObtainMainBoardData,&ObtainMainBoardData::sendReadMainboardData,this,&MainWindow::timeoutObtainMainboadData,Qt::QueuedConnection);
        connect(this,&MainWindow::stopObatinMachineInfo,m_ObtainMainBoardData,&ObtainMainBoardData::recvStopObatinMachineInfo);
        connect(this,&MainWindow::aNewconnectMachine,m_ObtainMainBoardData,&ObtainMainBoardData::recvaNewconnectMachine);
        m_threadMainBoard.start();
    }
    //QLOG_DEBUG() << "主线程Id:" << QThread::currentThreadId();
    return;
}

void  MainWindow::timeoutObtainMainboadData()
{
    if(cglobal::gserialConnecStatus){
        QByteArray _readMainBoard;
        QUIUtils::Traverse_the_motherboard(_readMainBoard);
        emit sendOneDirectives(_readMainBoard,"同步主板信息");
        //QLOG_DEBUG() << "读主板信息线程mianId:" << QThread::currentThreadId();
    }
    return;
}


void MainWindow::writeConsumablesExit(){
    emit controlallchnstate(false); //关闭所有通道旋转
    QByteArray buffer =  GlobalData::writeBuffer2Equipment();

    QByteArray bufferLimitBottle = GlobalData::writeBottleLimit2Equipment();
    emit sendOneDirectives(bufferLimitBottle,"关闭写入试剂整体单位限位");
    QLOG_DEBUG()<<"关闭写入试剂整体单位限位"<<bufferLimitBottle.toHex(' ').toUpper();


    emit sendOneDirectives(buffer,"写入缓存耗材数据");
    QTimer::singleShot(1000, this, [=]() { // 1000毫秒=1秒延时
           emit FullyAutomatedPlatelets::pinstanceWirteBoard()->closeSerial();
           FullyAutomatedPlatelets::mainWindow()->deleteExitSoftware();
    });
}
void MainWindow::slotbootInitCleanFinished()
{
    if (!m_pProgress.isNull()) {
        m_pProgress->close();  // 自动触发 delete 并置空 m_pProgress
        qApp->processEvents(); // 可选：处理界面残留
    }

    if(m_shutdownClean){
        writeConsumablesExit();
        return;
    }

    //机器已经初始化清洗
    m_MachineAlreadyInitCleanned = true;

    FullyAutomatedPlatelets::pinstanceWirteBoard()->Start();

    //开始每秒读取主板耗材信息
    Traverse_through_motherboard_information(); //遍历主板线程

    _creatbeginreadmodule();//开始读取模组线程数据

    mshowModuledata.data()->_start();//开启接收模组数据线程

    QLOG_DEBUG()<<"清洗初始化完成[开始读取 主板命令- 模组- 接收模组数据]"<<endl;

    //开机初始化清洗完成
    FullyAutomatedPlatelets::pinstanceinstrument()->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);

    return;
}

void MainWindow::Recv_Module_temperature(const quint8 IndexMode, const double tempValue)
{
    QString originallyText = QString::number(tempValue, 'f', 2);
    switch(IndexMode)
    {
        case MODULE_1:
            ui->label_showModule1_value->setText(originallyText);
        break;
        case MODULE_2:
            ui->label_showModule2_value_2->setText(originallyText);
        break;
        case MODULE_3:
            ui->label_showModule3_value_2->setText(originallyText);
        break;
        default: break;
    }
    update();
    return;
}

void MainWindow::_slotsycnPaintentInfo(QString id_,QString addtime,QString barcode_,QString testProject)
{
    Calibrate* paddTaskCol = FullyAutomatedPlatelets::pinstancepatientdata();
    paddTaskCol->_addpatientsqltable(id_,addtime,barcode_,testProject);
}

void MainWindow::initcreatprogressingDimming(bool bdimming)
{
    //空指针防护 + 延迟初始化
    if (m_pProgress.isNull()) {
        m_pProgress = new customProgresscontrols(this); // 设置父对象

    }

    // 跨线程安全：切换到主线程执行
    QMetaObject::invokeMethod(this, [this, bdimming]() {
            if (m_pProgress.isNull()) return;

            // 3. 原子操作读取全局状态（避免竞态）
            const bool isFinished = /*qAtomicLoad(&*/cglobal::g_controldimmingfinished/*)*/;

            if (bdimming) {
                m_pProgress->_settitle(tr("等待仪器调光"));
                m_pProgress->_setprogressRand(0, 0);
                m_pProgress->_setlabletext(tr("请等待仪器控温调光中..."));

                // 4. 状态驱动显示逻辑
                isFinished ? m_pProgress->close() : m_pProgress->show();
            } else {
                m_pProgress->close();
            }
    });
}


void MainWindow::slotCleaningProgress(quint8 index, quint8 total)
{
    // 确保在主线程执行 UI 操作
   QMetaObject::invokeMethod(this, [this, index, total]() {
       if (!m_pProgress.isNull()) {
           m_pProgress->_setprogresstotalnum(total);
           m_pProgress->_setprogressvalue("初始清洗进度:", index + 1);
       }
   });
   return;
}

//添加任务进度条
void MainWindow::initProgressBar(const QString& title, bool isModal,bool ishow)
{
    if(m_pProgress.isNull()){
        m_pProgress = new customProgresscontrols(this);
    }

    m_pProgress->setWindowModality(isModal ? Qt::ApplicationModal : Qt::NonModal);
    m_pProgress->_settitle(title);
    m_pProgress->_setprogressRand(0,100);
    m_pProgress->_setprogressvalue(title, 0);
    m_pProgress->setVisible(ishow);
    update(); // 请求界面更新
    return;
}

void MainWindow::_progressBarconfig(int data_,int max_)
{
    //空指针防护
    if (m_pProgress.isNull()) {
        QLOG_WARN() << "Progress bar is null!";
        return;
    }

    //跨线程安全：确保在主线程执行UI操作
    QMetaObject::invokeMethod(this, [this, data_, max_]() {
        if (m_pProgress.isNull()) return;

        //优化显示逻辑：仅需首次显示
        if (!m_pProgress->isVisible())
            m_pProgress->show();

        //更新进度值
        m_pProgress->_setprogresstotalnum(max_);
        m_pProgress->_setprogressvalue("添加测试样本进度:", data_);

        //安全关闭与生命周期管理
        if (data_ == max_) {
            m_pProgress->close();
        }
    });
}




// 初始化错误信息映射表
void MainWindow::initErrorDisplayMap()
{
    m_errorInfoMap.clear();

    // 定义错误类型与显示信息的映射表
    const QVector<QPair<quint8, ErrorDisplayInfo>> errorInfoList = {
        {
            static_cast<quint8>(equipmentTipInfo::LinqueScrapFull),
            ErrorDisplayInfo(
                ":/Picture/SetPng/outside_wasteliquor.png",
                ":/Picture/SetPng/outside_wasteliquor_warming.png",
                tr("废液满"),
                ui->show_wastlique,
                &m_warmwasteliquor
            )
        },
        {
			static_cast<quint8>(equipmentTipInfo::LinqueCleanShortage),
            ErrorDisplayInfo(
                ":/Picture/SetPng/outside_clean.png",
                ":/Picture/SetPng/outside_clean_warming.png",
                tr("外部清洗液缺少"),
                ui->showcleanliqur,
                &m_cleanliquor
            )
        }
            // 可以继续添加其他错误类型
        };

	for (const auto &pair : errorInfoList) {
		m_errorInfoMap.insert(pair.first, pair.second);
	}
}

//优化
void MainWindow::handleErrorNotification(quint8 index_, const QString& errinfo)
{
    auto it = m_errorInfoMap.find(index_);
    if(it != m_errorInfoMap.end()) {
        auto& info = it.value();

        if(!info.targetLabel) {
            qCritical() << "Invalid label pointer for error code:" << index_;
            return;
        }

        // 状态切换
        if(info.stateFlag) {
            *info.stateFlag = !(*info.stateFlag);
        }

        // 图片路径选择
        const QString& path = (info.stateFlag && *info.stateFlag)
                            ? info.alertPath
                            : info.normalPath;
        if(path.isEmpty()) {
            qWarning() << "Empty image path for error code:" << index_;
            return;
        }

        // 带缓存机制的图像加载
        QPixmap cachedPix;
        if(!QPixmapCache::find(path, &cachedPix)) {
            if(!cachedPix.load(path)) {
                qWarning() << "Failed to load image:" << path;
                return;
            }
            cachedPix = cachedPix.scaled(info.iconSize * devicePixelRatioF(),
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);
            cachedPix.setDevicePixelRatio(devicePixelRatioF());
            QPixmapCache::insert(path, cachedPix);
        }

        // 线程安全的UI更新
        QMetaObject::invokeMethod(info.targetLabel, [=](){
            info.targetLabel->show();
            info.targetLabel->setToolTip(info.tooltip);
            info.targetLabel->setPixmap(cachedPix);
        });
    }
    else {
        //QLOG_WARN()<< "Unhandled error code:" << index_ << "Info:" << errinfo;
    }
}




// 兼容Qt5全版本的窗口居中方法
void MainWindow::centerWindow(QWidget* widget)
{
    if (!widget) return;

    // 方法1：适用于Qt5.14+（推荐）
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
    QScreen* targetScreen = widget->screen();
    if (!targetScreen) {
        targetScreen = QGuiApplication::primaryScreen();
    }

    // 方法2：兼容Qt5.0+的旧方法
#else
    QDesktopWidget* desktop = QApplication::desktop();
    QScreen* targetScreen = QGuiApplication::screens().value(desktop->screenNumber(widget));
#endif
    if (targetScreen) {
        QRect screenGeometry = targetScreen->availableGeometry();
        widget->move(
            (screenGeometry.width() - widget->width()) / 2,
            (screenGeometry.height() - widget->height()) / 2
            );
    }
    else {
        // 回退到主屏幕居中
        widget->move(
            (QApplication::desktop()->width() - widget->width()) / 2,
            (QApplication::desktop()->height() - widget->height()) / 2
        );
    }
}


// 封装信号连接逻辑
void MainWindow::setupDimmingConnections(
    QSharedPointer<ControltheModuletemp> widget,
    const QList<quint8>& failedChn,
    QPointer<MainWindow> safeThis)
{
    // 重新调光信号
    QObject::connect(widget.data(), &ControltheModuletemp::re_dimming, [safeThis]() {
        if(safeThis) {
            FullyAutomatedPlatelets::pinstancedimming()->theEndDimmingActive();
        }
    });

    // 删除提示信号
    QObject::connect(widget.data(), &ControltheModuletemp::deletedimming,
        [widget, safeThis](const QString& title) {
            if(safeThis) {
                safeThis->m_reminderTitleStr.remove(title);
            }
            widget->close();
    });

    // 禁用通道信号
    QObject::connect(widget.data(), &ControltheModuletemp::disableusechn,
        [widget, failedChn, safeThis]() {
            if(!safeThis) return;

            safeThis->_commingfaileddisablechn(failedChn);
            safeThis->_dimmingprogress(false);
            cglobal::g_controldimmingfinished = true;

            QLOG_DEBUG() << "调光失败直接禁用通道(调光完成):" << failedChn;
            emit safeThis->ReminderTextOut(PROMPTLOG, "模组调光完成end");

            widget->close();
    });
}


void MainWindow::recvshowDimmingFailedChn(const QList<quint8>& failedChn)
{
    // 优化1: 使用QStringList和join优化字符串拼接
    QStringList failedChnStrs;
    std::transform(failedChn.cbegin(), failedChn.cend(), std::back_inserter(failedChnStrs),
                   [](quint8 chn) { return QString::number(chn); });
    const QString failedList = failedChnStrs.join(',');


    // 优化2: 避免重复创建提示窗口
    const QString dimmingTitle = tr("温控调光提示");
    if(m_reminderTitleStr.contains(dimmingTitle))
        return;

    // 优化3: 使用智能指针管理弹窗对象
    QSharedPointer<ControltheModuletemp> popWidget(new ControltheModuletemp(),
        [this](ControltheModuletemp* ptr) {
        if(m_reminderTitleStr.contains(ptr->windowTitle())) {
            m_reminderTitleStr.remove(ptr->windowTitle());
        }
        ptr->deleteLater();
    });

    // 优化4: 统一界面设置逻辑
    popWidget->settipstext(QString("调光失败通道%1是否直接禁用").arg(failedList), dimmingTitle);
    centerWindow(popWidget.data()); // 封装窗口居中逻辑
    popWidget->setWindowModality(Qt::NonModal);
    popWidget->setAttribute(Qt::WA_DeleteOnClose);

    // 优化5: 使用QPointer安全捕获this指针
    QPointer<MainWindow> safeThis(this);

    // 优化6: 集中管理信号连接
    setupDimmingConnections(popWidget, failedChn, safeThis);

    // 更新状态管理
    m_reminderTitleStr.insert(dimmingTitle);
    popWidget->show();
    popWidget->raise(); // 确保窗口在最前面
    popWidget->activateWindow(); // 激活窗口
    m_popwidgetTips = popWidget.data(); // 转换为弱引用存储

    return;
}

void MainWindow::_commingfaileddisablechn(QList<quint8> dimmingFiledChnList)
{

    // 1. 使用std::transform转换容器
    QVector<quint8> disableChn;
    disableChn.reserve(dimmingFiledChnList.size());
    std::transform(dimmingFiledChnList.cbegin(), dimmingFiledChnList.cend(),
                       std::back_inserter(disableChn),
                       [](quint8 chn) { return chn - 1; });


    //2. 同步设置界面通道状态
    FullyAutomatedPlatelets::pinstanceequipmentconfig()->disablechn(disableChn);

    // 3. 使用范围for循环处理每个通道
    for (auto indexchn : disableChn) {
        QString wkey = QString("TestTheChanne1Opening_%1").arg(indexchn + 1);
        INI_File().wConfigPara(wkey,false);
        //INI_File().SetChannelState(indexchn + 1, false);                     // 设置INI文件状态
        StructInstance::getInstance()->config_testChn_State(                 // 更新配置状态
            indexchn, CHN_STATUS_DISABLE);
        mptesting.data()->recv_NotifyChannleState(indexchn + 1, false);     // 通知测试模块
    }
    return;
}



//测试过程中抓手吸 吐试杯失败
void MainWindow::handletheGripperFailed(const int sampleid, const QString outstr,const quint8 indexActive){

    if (m_suppressGripperPrompt) {
        QLOG_DEBUG() << "Gripper prompt suppressed for sample" << sampleid;
        return;
    }

    QString titlename = tr("抓杯异常");
    QString dispalyWarmming = QString("样本%1%2请擦拭抓手洗盘后重试,或放弃当前样本!").arg(sampleid).arg(outstr);

    QPointer<ConnectToInstrument> phansErrWidget = new ConnectToInstrument(titlename, dispalyWarmming);
    m_suppressGripperPrompt = true;

    phansErrWidget->setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    phansErrWidget->setAttribute(Qt::WA_DeleteOnClose);   // 关闭时自动销毁
    phansErrWidget->setAttribute(Qt::WA_ShowWithoutActivating); // 不抢占焦点
    phansErrWidget->setWindowModality(Qt::NonModal);     // 非模态窗口

    phansErrWidget->notifybuttonDisplayName(tr("放弃样本"),tr("再次测试"));

    connect(phansErrWidget, &ConnectToInstrument::giveupTestSample, this,
        [this, phansErrWidget,indexActive]() {
        emit gripErrGiveupSample(true, indexActive);
        if (phansErrWidget) {
            m_suppressGripperPrompt = false;
            delete phansErrWidget;
        }
    });


    connect(phansErrWidget, &ConnectToInstrument::tryAgainTestSample, this,
            [this,phansErrWidget, indexActive]() {
        emit gripErrGiveupSample(false,indexActive);
        if (phansErrWidget){
            m_suppressGripperPrompt = false;
            delete phansErrWidget;
        }

    });

    connect(phansErrWidget, &ConnectToInstrument::pendingTimeout,this,
        [this,phansErrWidget, indexActive](){
        emit pendingtimeoutSampleTest(indexActive);
        if (phansErrWidget){
            m_suppressGripperPrompt = false;
            delete phansErrWidget;
        }
    });

    if (const QScreen* activeScreen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = activeScreen->availableGeometry();
        phansErrWidget->move(screenGeometry.center() - phansErrWidget->rect().center()
        );
    }

    phansErrWidget->show();
    phansErrWidget->raise(); // 确保窗口在最前面
    phansErrWidget->activateWindow(); // 激活窗口
    return;
}


//PE测试打开关闭通道控制
void MainWindow::handleControlChannelRevolve(const quint8&channelNum,const bool& isRevolve){
    emit _controlmotorrunning(channelNum,isRevolve);
}
void MainWindow::handleoutErrInfo(const QString titles,const QString errStr){
    ThreadSafeReminder(titles,errStr);
}
