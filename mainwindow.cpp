#include  "loginui.h"
#include  "mainwindow.h"
#include  "ui_mainwindow.h"
#include  "dilag/testopcv.h"
#include  <Dbt.h>
#include  <QCameraInfo>
#include  <QFontMetrics>
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
#include <custom_style/custommessagebox.h>
#include <QImage>
#include <QLinearGradient>
#include <QPainter>
#include <QPainterPath>
#include  <QPair>
#include  <QScreen>         // 包含QScreen头文件
#include  <QGuiApplication> // 包含QGuiApplication头文件
#include <QPointer>
#include <suoweiFileManager/filemanager.h>

#include "CalibratCoordinateLoc/coordinatepposit.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

namespace {

QIcon makeQcNavIcon()
{
    QImage image(QStringLiteral(":/Picture/backorigin.png"));
    if (image.isNull())
        return QIcon();
    image = image.convertToFormat(QImage::Format_ARGB32);

    int minX = image.width();
    int minY = image.height();
    int maxX = 0;
    int maxY = 0;
    for (int y = 0; y < image.height(); ++y) {
        const QRgb *line = reinterpret_cast<const QRgb *>(image.constScanLine(y));
        for (int x = 0; x < image.width(); ++x) {
            const QRgb px = line[x];
            if (qAlpha(px) < 220)
                continue;
            const int r = qRed(px);
            const int g = qGreen(px);
            const int b = qBlue(px);
            if (r < 70 && g < 90 && b > r + 20 && b < 190) {
                if (x < minX) minX = x;
                if (y < minY) minY = y;
                if (x > maxX) maxX = x;
                if (y > maxY) maxY = y;
            }
        }
    }
    if (maxX <= minX || maxY <= minY)
        return QIcon(QPixmap::fromImage(image));

    const int inset = qMax(4, (maxX - minX) / 18);
    const QRectF tile(minX + inset, minY + inset,
                      maxX - minX - inset * 2, maxY - minY - inset * 2);
    const int cx = (minX + maxX) / 2;
    const QColor top = image.pixel(cx, qMin(image.height() - 1, minY + inset + 2));
    const QColor bottom = image.pixel(cx, qMax(0, maxY - inset - 2));

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QLinearGradient fill(tile.topLeft(), tile.bottomLeft());
    fill.setColorAt(0, top);
    fill.setColorAt(1, bottom);
    painter.setPen(Qt::NoPen);
    painter.setBrush(fill);
    painter.drawRoundedRect(tile, tile.width() * 0.08, tile.height() * 0.08);

    painter.setBrush(Qt::white);
    const qreal barH = qMax(3.0, tile.height() * 0.09);
    const qreal left = tile.left() + tile.width() * 0.16;
    painter.drawRoundedRect(QRectF(left, tile.top() + tile.height() * 0.16,
                                   tile.width() * 0.68, barH), barH / 2, barH / 2);
    painter.drawRoundedRect(QRectF(left, tile.top() + tile.height() * 0.36,
                                   tile.width() * 0.50, barH), barH / 2, barH / 2);
    painter.drawRoundedRect(QRectF(left, tile.top() + tile.height() * 0.56,
                                   tile.width() * 0.34, barH), barH / 2, barH / 2);

    QPainterPath check;
    check.moveTo(tile.left() + tile.width() * 0.46, tile.top() + tile.height() * 0.70);
    check.lineTo(tile.left() + tile.width() * 0.58, tile.top() + tile.height() * 0.84);
    check.lineTo(tile.left() + tile.width() * 0.84, tile.top() + tile.height() * 0.52);
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::white, qMax(2.0, tile.width() * 0.07),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawPath(check);
    painter.end();
    return QIcon(QPixmap::fromImage(image));
}

void setFocusTaskStatusText(QLabel *lab, const QString &text)
{
    if (!lab)
        return;
    const QString shown = text.isEmpty() ? QObject::tr("待机") : text;
    const bool idle = (shown == QObject::tr("待机"));
    const bool alert = shown.contains(QObject::tr("暂停"))
            || shown.contains(QObject::tr("异常"))
            || shown.contains(QObject::tr("超时"))
            || shown.contains(QObject::tr("失败"));
    QString bg = QStringLiteral("#FFFFFF");
    QString fg = QStringLiteral("#1A6B8A");
    QString border = QStringLiteral("#E2EEF4");
    if (idle) {
        bg = QStringLiteral("transparent");
        fg = QStringLiteral("#5A7384");
        border = QStringLiteral("transparent");
    } else if (alert) {
        bg = QStringLiteral("#FFF4D6");
        fg = QStringLiteral("#9A6B00");
        border = QStringLiteral("#E8D39A");
    }
    lab->setStyleSheet(
        QStringLiteral("QLabel#label_focusTask{"
                       "background:%1;color:%2;border:1px solid %3;"
                       "border-radius:8px;padding:4px 10px;"
                       "font-family:'Microsoft YaHei';font-size:13px;}")
            .arg(bg).arg(fg).arg(border));
    QFontMetrics fm(lab->font());
    const int w = qMax(lab->width(), 220);
    lab->setToolTip(shown);
    lab->setText(fm.elidedText(shown, Qt::ElideRight, w - 20));
}

QLabel *ensureFocusTaskLabel(Ui::MainWindow *ui)
{
    QLabel *lab = ui->StatusBar->findChild<QLabel *>(QStringLiteral("label_focusTask"));
    if (!lab) {
        lab = new QLabel(ui->StatusBar);
        lab->setObjectName(QStringLiteral("label_focusTask"));
        lab->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        lab->setMinimumWidth(220);
        lab->setMaximumWidth(420);
        lab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        lab->setAttribute(Qt::WA_StyledBackground, true);
        setFocusTaskStatusText(lab, QObject::tr("待机"));
    }

    QHBoxLayout *lay = ui->horizontalLayout_status;
    if (lay->indexOf(lab) >= 0)
        lay->removeWidget(lab);
    const int reagentIdx = lay->indexOf(ui->widget_ReminderLiqune);
    lay->insertWidget(reagentIdx >= 0 ? reagentIdx + 1 : lay->count(), lab);
    return lab;
}

QTimer *usbUnplugDebounceTimer(QObject *owner)
{
    static QPointer<QTimer> timer;
    if (!timer) {
        timer = new QTimer(owner);
        timer->setSingleShot(true);
        timer->setInterval(1200);
    }
    return timer;
}

} // namespace

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    m_pauseIcon(":/Picture/pause.png")
    ,m_continueIcon(":/Picture/Start.png")
    ,ui(new Ui::MainWindow)
    ,m_shutdownClean(false)
    ,m_bmachineconfigureSignal(false)
    ,m_MachineAlreadyInitCleanned(false)
    ,m_benterapp(false)
    ,m_indexSec(0)
    ,m_relChannel(0)
    ,m_currentIndex(0)
    ,m_warmIndexReagent(-1)
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
        m_connectedIcon = m_connectedIcon.scaled(labelSize, Qt::KeepAspectRatio, Qt::FastTransformation);
        m_disconnectedIcon = m_disconnectedIcon.scaled(labelSize, Qt::KeepAspectRatio, Qt::FastTransformation);
    }


    // 状态栏初始化
    ui->StatusBar->setFixedHeight(64);
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
                                        Qt::FastTransformation);
    machinePixmap.setDevicePixelRatio(devicePixelRatioF());

    ui->Machine_type->setPixmap(machinePixmap);
    ui->Machine_type->setAlignment(Qt::AlignCenter);

    // 高DPI适配样式
    QString style = QString("QLabel {""border: none;""background: transparent;"
                           "min-width: %1px;""min-height: %2px;""}").arg(155).arg(150);
    ui->Machine_type->setStyleSheet(style);

    initErrorDisplayMap();

   // initScanBarcode();

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

    // 添加线程停止获取主板线程方法
    shutdownBoardThread();





    if (mThreaddotest.isRunning()) {
        mThreaddotest.quit();
        if (!mThreaddotest.wait(3000)) {
            QLOG_WARN() << "Test thread did not stop in time";
        }
    }

    if (m_pdoingTesting) {
        if (mThreaddotest.isRunning()) {
            // 调度对象还在测试线程里跑，这里 delete 会 UAF；退出过程宁可泄漏
            QLOG_WARN() << "Test thread still running, skip deleting TestProjectProcess";
            (void)m_pdoingTesting.take();
        } else {
            m_pdoingTesting.reset();
        }
    }

    // 最终状态验证
    logThreadStatus();
}

void MainWindow::logThreadStatus()
{
    QLOG_DEBUG() << "\n----- Thread Status Report -----";
    QLOG_DEBUG() << "Module Thread:" << (m_threadModule.isRunning() ? "Running" : "Stopped");
    QLOG_DEBUG() << "MainBoard Thread:" << (m_boardThread.isRunning() ? "Running" : "Stopped");
    QLOG_DEBUG() << "Test Thread:" << (mThreaddotest.isRunning() ? "Running" : "Stopped");
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
try{
    // 1. 首先停止并等待所有工作线程（最高优先级）
    stopAndWaitThreads();

    // 2. 停止定时器和异步操作
    if (m_experimentTimer) {
        m_experimentTimer->stop();
        delete m_experimentTimer;
        m_experimentTimer = nullptr;
    }

    // 3. 关闭硬件设备和串行通信（尽早释放硬件资源）
    DestructionSerialclass();

    // 4. 停止并删除可能还在运行的对象（如打印进程）
    if (m_processPrint) {
        delete m_processPrint;
        m_processPrint = nullptr;
    }

    // 5. 删除业务逻辑对象（依赖硬件和线程的对象）
    delete WriteParameter;
    WriteParameter = nullptr;

    if (mshowModuledata) {
        mshowModuledata->shutdown();
        delete mshowModuledata.data();
        mshowModuledata = nullptr;
    }

    // 6. 删除UI相关的非Widget对象
    delete m_graphplot;
    m_graphplot = nullptr;
    delete m_qcChartPage;
    m_qcChartPage = nullptr;

    // 7. 退出框只由这里释放（不要 WA_DeleteOnClose / closeEvent deleteLater 再删一次）
    if (preminder) {
        delete preminder.data();
        preminder = nullptr;
    }


    // 8. 清理单例（在大多数对象之后）
    cleanupSingletons();

    // 9. 处理剩余事件
    QCoreApplication::processEvents();

    // 10. 最后删除UI对象（最低优先级）
    delete ui;
    ui = nullptr;

    StructInstance::getInstance()->m_garbo;

    } catch (const std::exception& e) {
        QLOG_WARN() << "Exception in MainWindow destructor: " << e.what();
    } catch (...) {
        QLOG_WARN() << "Unknown exception in MainWindow destructor";
    }
}

void MainWindow::configReminderIcon(quint8 index_)
{
    if (!ui->sStatusInfo)
        return;
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


void MainWindow::dimmingprogress(bool isDimmingInProgress)
{
    if (cglobal::g_controldimmingfinished.load()) {
        initcreatprogressingDimming(false);
        return;
    }

    initcreatprogressingDimming(isDimmingInProgress);
    update();
}

void MainWindow::initStyleAll()
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

    serialConnection(); //串口实例化

    //Traverse_through_motherboard_information(); //遍历主板线程

    Traverse_through_ModuleReadInfo();  //遍历模组信息

    init_read_moduledata_thread(); //初始化实例获取到模组数据线程

    inittestmodule_data_thread(); //收到模组数据处理线程

    initmainboradthread(); //主板线程实例化函数&&***触发测高信号

    CreatActionExecution();  //单个动作完成状态信号

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

        QObject::connect(this,&MainWindow::sendcodeList,
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
                         &ConsumablesWrite::shutdownApp, this, []() {
            if (auto *mw = FullyAutomatedPlatelets::mainWindow()) {
                mw->deleteExitSoftware();
            }
        });
    }


    //所有测试完成复位
    connect(mptesting.data(),&Testing::tsetfinishedbackorigin,this,[=](){

        backOriginTestFinished();
		QLOG_DEBUG() << "仪器无测试样本或已完成,机器复位";

        if (!m_pProgress.isNull()) {
            m_pProgress->close();  // 自动触发 delete 并置空 m_pProgress
            qApp->processEvents(); // 可选：处理界面残留
        }

        //清空复查样本
        auto *paddTaskCol = FullyAutomatedPlatelets::pinstancepatientdata();
        if (paddTaskCol) {
            paddTaskCol->clearfollowUpSample();
        }
      


        emit allCurveClear();  //曲线界面复原
        mptesting.data()->TotalTaskProgress();//总任务进度试管杯
        update();
    });

    connect(m_graphplot.data(),
            &GraphPlot::DrawProgressbar,
            mptesting.data(),
            &Testing::DrawChannelProgress);

    connect(this,&MainWindow::allCurveClear,
            m_graphplot.data(),&GraphPlot::backallCurveClear);

    /*机器主界面试管使用同步耗材界面试管使用显示 &&质控使用试管*/
    connect(mptesting.data(),&Testing::SynchronizeEmptyTube,
            FullyAutomatedPlatelets::pinstanceinstrument(),
            &QualityControl::slotSynchronizeEmptyTube);

    connect(this, &MainWindow::SynchronizeEmptyTubeUsed,
            FullyAutomatedPlatelets::pinstanceinstrument(),
            &QualityControl::slotSynchronizeEmptyTube);


    //模组通道值显示在设置界面
    MachineSetting* pequipmentconfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();

    connect(mlocalSerial.data(),&SuoweiSerialPort::backHandssuck,pequipmentconfig,
            &MachineSetting::handlebackHandssuck,Qt::QueuedConnection);

    /** 登录工程师模式  **/
    connect(pequipmentconfig,
            &MachineSetting::LoginEngineerMode,
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
    connect(this, &MainWindow::instrumentCameOnline,
            mshowModuledata.data(), &displayChanneldata::handleInstrumentReconnected,
            Qt::QueuedConnection);


    //读耗材卡确认充值
    connect(this, &MainWindow::rechargesuccessful,
            FullyAutomatedPlatelets::pinstanceinstrument(),
            &QualityControl::handleRechargeSuccessful);

    //配置机器参数信号耗材修改限位值 、耗材余量
    connect(this,&MainWindow::sendmoduleactio,  mlocalSerial.data(),
            &SuoweiSerialPort::slotwrite_instructions_group);

    connect(this,&MainWindow::sendOneDirectives,
            mlocalSerial.data(),
            &SuoweiSerialPort::writedataToEquipment,Qt::QueuedConnection);

    //发送指令到仪器、指令信息
    connect(FullyAutomatedPlatelets::pinstanceinstrument(),
            &QualityControl::sendDirectives,
            mlocalSerial.data(),
            &SuoweiSerialPort::writedataToEquipment);

    connect(mshowModuledata.data(),&displayChanneldata::sampleTestingErr,
            FullyAutomatedPlatelets::pinstancepatientdata(),
            &Calibrate::recvSampleTestingErr);

    //查询界面删除样本记录
    auto *inquireData = FullyAutomatedPlatelets::pinstanceInquiredata();
    auto *patientData = FullyAutomatedPlatelets::pinstancepatientdata();
    if (inquireData && patientData) {
        connect(inquireData, &Inquire_Sql_Info::sycnDelOthertableWidgetRow,
                patientData, &Calibrate::onsycnDelOthertableWidgetRow);
    } else {
        // 可选：记录日志或处理错误
        QLOG_WARN() << "Failed to connect delete signal: instance is null";
    }


    // 连接通道数据错误信号，显示非阻塞提示给用户
    connect(mshowModuledata.data(), &displayChanneldata::channelDataError,
            this, [=](const QString &errorMsg) {
        // 创建非模态对话框
        QMessageBox *msgBox = new QMessageBox(this);
        msgBox->setWindowTitle("提示");
        msgBox->setText(errorMsg);
        msgBox->setStandardButtons(QMessageBox::Ok);
        msgBox->setAttribute(Qt::WA_DeleteOnClose); // 关闭时自动删除
        msgBox->show(); // 非阻塞显示
    });


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
    initializeAltimeterHardware();

    return;
}


//初始化实例获取到模组数据线程
void MainWindow::init_read_moduledata_thread()
{
    if(mshowModuledata == nullptr)
        mshowModuledata = new displayChanneldata();

    connect(this,&MainWindow::syncModuleChannelData, mshowModuledata.data(),
                &displayChanneldata::Recv_syncModuleChannelData);

    connect(mshowModuledata.data(),
            &displayChanneldata::DisplayTestingValue,
            m_graphplot.data(),
            &GraphPlot::GetTestingValue);

    connect(mshowModuledata.data(),
            &displayChanneldata::pppInitRead,
            m_graphplot.data(),
            &GraphPlot::onPppInitRead,
            Qt::QueuedConnection);

    connect(this,&MainWindow::qualityChannelVal,mshowModuledata.data(),
            &displayChanneldata::getqualityControlValue);

    connect(FullyAutomatedPlatelets::pinstanceequipmentconfig(),&MachineSetting::getchanneldata,
            this,[=](const quint8 channel,int indexActive){
         emit qualityChannelVal(channel,indexActive);
    });

    return;
}

//收到模组数据处理线程
void  MainWindow::inittestmodule_data_thread()
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
             //ThreadSafeReminder(title_,errstr);
             CustomMessageBox::warning(this,title_,errstr);
        });


        //是否暂停模组通讯
        connect(mtestmoduleprotocol.data(),
                &theTestModuleProtocol::pauseConnectModule,this,[=](){
                pauseObtainmodulecommand("调光保存暂停模组通讯",true);
        });


        QObject::connect(mtestmoduleprotocol.data(),
                         &theTestModuleProtocol::resetconnectModule,this,[=](){
            emit aNewconnectMachine();
            resumeBoardAcquisition();
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


void MainWindow::handleNormalOperation(quint8 index)
{
    if (equipmentTipInfo::LinqueScrapFull == index) {
        m_warmwasteliquor = false;
        ui->show_wastlique->hide();
    } else if (equipmentTipInfo::LinqueCleanShortage == index) {
        m_cleanliquor = false;
        ui->showcleanliqur->hide();
    }
    if (!m_warmwasteliquor && !m_cleanliquor && m_consumablePulseTimer)
        m_consumablePulseTimer->stop();
}
void MainWindow::handleAbnormalCardSwipe(const QString& promptInfo)
{
    resumeMainboardPollAfterRecharge();
    CustomMessageBox::warning(this,tr("充值耗材异常"), promptInfo);
}
void MainWindow::initmainboradthread()
{
    auto mainBoardData = FullyAutomatedPlatelets::pinstanceMainboarddata();
    mainBoardData->setArgInterface(this);

    //收到串口主板消息
    QObject::connect(mlocalSerial.data(),&SuoweiSerialPort::mainControlBoardProtocolData,
                     mainBoardData,&mainControlBoardProtocol::recvmainControlBoardProtocol,
                     Qt::QueuedConnection);

    QObject::connect(mainBoardData,&mainControlBoardProtocol::normaloper,
                     this,[this](quint8 index){
        handleNormalOperation(index);
    });

    //废液、外部清洗液弹出提示后在信息栏提醒信号连接
    QObject::connect(mainBoardData, &mainControlBoardProtocol::reminderErrorInfo,
                     this, &MainWindow::handleErrorNotification);
    // 验证信号签名是否匹配
    static_assert(
        QtPrivate::FunctionPointer<decltype(&mainControlBoardProtocol::reminderErrorInfo)
                >::ArgumentCount == 2,
        "Signal signature mismatch!"
    );

    connect(this,&MainWindow::ConfigUsedBuzzerMark,
            mainBoardData,&mainControlBoardProtocol::RecvUsedBuzzerMark);

    //刷卡提示
    connect(mainBoardData,&mainControlBoardProtocol::cardSwipeSuccessful,
            this,&MainWindow::handlecardSwipeSuccessful);


    //读卡、充值成功信号
    connect(mainBoardData,&mainControlBoardProtocol::swipeCardSuccessfullyWritten,
            this,&MainWindow::handleswipeCardSuccessfullyWritten);

    // 读卡异常信息提示 - 使用成员函数替代lambda
    connect(mainBoardData, &mainControlBoardProtocol::Abnormalcardswipingprompt,
            this, &MainWindow::handleAbnormalCardSwipe);



    //提示试管盘
    m_ThreadReminderTsetTube = std::make_unique<ThreadReminderTsetTube>(this);
    //m_ThreadReminderTsetTube = new ThreadReminderTsetTube(this);

    //废液、外部清洗液、试管盘 弹出提示
    QObject::connect(mainBoardData,&mainControlBoardProtocol::equipmentReminder,
                     m_ThreadReminderTsetTube.get(),&ThreadReminderTsetTube::recvequipmentReminder);

    QObject::connect(m_ThreadReminderTsetTube.get(),&ThreadReminderTsetTube::reminderMainUi,this,
                     [=](quint8 index, QString datastr){
                emit ReminderTextOut(index,datastr);
    });

    //触发测高
    connect(mainBoardData,&mainControlBoardProtocol::MainBoardHeightTrigger,
        this,&MainWindow::recvTriggerAltimetrySignal);

    mainBoardData->_start(minstrumentType);
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

    connect(m_pdoingTesting.data(), &TestProjectProcess::workflowStatusChanged,
            this, [this](const QString& statusText) {
        if (QLabel *lab = ui->StatusBar->findChild<QLabel*>(QStringLiteral("label_focusTask")))
            setFocusTaskStatusText(lab, statusText);
    }, Qt::QueuedConnection);

    connect(m_pdoingTesting.data(), &TestProjectProcess::pauseTesting,
            this, [this]() {
        if (!cglobal::g_PauseTesting.load() && cglobal::g_StartTesting.load()) {
            cglobal::g_PauseTesting = true;
            QToolButton *pPauseBtn = ui->toolButton_quality_sample;
            if (pPauseBtn) {
                pPauseBtn->setText(tr("继续"));
                pPauseBtn->setIcon(m_continueIcon);
                pPauseBtn->setStyleSheet(mPauseSateQss);
            }
            emit ReminderTextOut(PROMPTLOG, tr("流程异常，测试已自动暂停"));
        }
    }, Qt::QueuedConnection);

    connect(m_pdoingTesting.data(), &TestProjectProcess::requestPauseUiSync,
            this, [this](bool paused) {
        if (!cglobal::g_StartTesting.load()) {
            return;
        }
        cglobal::g_PauseTesting = paused;
        QToolButton *pPauseBtn = ui->toolButton_quality_sample;
        if (!pPauseBtn) {
            return;
        }
        if (paused) {
            pPauseBtn->setText(tr("继续"));
            pPauseBtn->setIcon(m_continueIcon);
            pPauseBtn->setStyleSheet(mPauseSateQss);
        } else {
            pPauseBtn->setText(tr("暂停"));
            pPauseBtn->setIcon(m_pauseIcon);
            pPauseBtn->setStyleSheet(minitTopBtnQss);
        }
    }, Qt::QueuedConnection);

    connect(m_pdoingTesting.data(), &TestProjectProcess::sampleSuckPosInvalid,
            this, [this](const QString& msg) {
        CustomMessageBox::warning(this, tr("吸样位置错误"), msg);
    }, Qt::QueuedConnection);


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

    connect(m_pdoingTesting.data(), &TestProjectProcess::abortPendingEquipmentIo,
            mlocalSerial.data(), &SuoweiSerialPort::abortPendingWrites,
            Qt::QueuedConnection);
    if (USB_InitConnect *act = FullyAutomatedPlatelets::pinstanceSingleactive()) {
        connect(m_pdoingTesting.data(), &TestProjectProcess::abortPendingEquipmentIo,
                act, &USB_InitConnect::abortPendingActions,
                Qt::QueuedConnection);
    }


    //异常提示
    connect(m_pdoingTesting.data(),&TestProjectProcess::executeErrPrompt,this,
            [this](QString ERRInfo){
        const bool pppHangWipePrompt = ERRInfo.contains(
                    QString::fromUtf8("洗针后仍探测到挂液"));
        const QString footer = pppHangWipePrompt
                ? tr("\n\n确定：按失败高度液面探测吸PPP并吐PPP。\n取消：保持暂停，擦净针尖后点「继续」重做预探测。")
                : tr("\n\n确定：全轴复位后按当前步骤续跑。\n取消：保持暂停，处理好后再点「继续」。");
        const auto ret = CustomMessageBox::question(
                    this,
                    tr("动作执行异常"),
                    ERRInfo + footer,
                    QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok) {
            if (pppHangWipePrompt && m_pdoingTesting) {
                m_pdoingTesting->confirmPppHangLiquidProceed();
            } else {
                emit ProceedTask();
            }
        } else {
            if (!cglobal::g_PauseTesting.load() && cglobal::g_StartTesting.load()) {
                cglobal::g_PauseTesting = true;
                QToolButton *pPauseBtn = ui->toolButton_quality_sample;
                if (pPauseBtn) {
                    pPauseBtn->setText(tr("继续"));
                    pPauseBtn->setIcon(m_continueIcon);
                    pPauseBtn->setStyleSheet(mPauseSateQss);
                }
            }
            emit ReminderTextOut(PROMPTLOG, tr("动作异常已暂停，等待处理后继续"));
        }
    }, Qt::QueuedConnection);

    connect(this,&MainWindow::ProceedTask,m_pdoingTesting.data(),
            &TestProjectProcess::equipment_err_continue);

    //打开通道转子
    connect(m_pdoingTesting.data(),&TestProjectProcess::openChnMotor,
            this,[=](const quint8 indexchn,const bool bopen)
    {
        emit controlmotorrunning(indexchn,bopen);
    });



    //获取通道贫血值(抓贫血到测试通道后发送信号)
    connect(m_pdoingTesting.data(),
            &TestProjectProcess::getAnemiaValue,
            mshowModuledata.data(),
            &displayChanneldata::slotgetAnemiaValue,
            Qt::QueuedConnection);

    //获取样本PPP初值成功丢PPP
    connect(mshowModuledata.data(),
            &displayChanneldata::CompleteGetAnemiaValue,
            m_pdoingTesting.data(),
            &TestProjectProcess::throwAnemiaCup,
            Qt::QueuedConnection);

    connect(mshowModuledata.data(), &displayChanneldata::testingLogHint,
            this, [this](const QString& text) {
        emit ReminderTextOut(TESTINGLOG, text);
    }, Qt::QueuedConnection);

    //空试管被夹起
    connect(m_pdoingTesting.data(),&TestProjectProcess::EmptyTubeCatched,
            mptesting.data(), &Testing::EmptyTubeClipMoved,
            Qt::QueuedConnection);

    connect(m_pdoingTesting.data(), &TestProjectProcess::bloodSuck2EmptyTube,
            mptesting.data(), &Testing::recvBloodSuck2EmptyTube,
            Qt::QueuedConnection);
    connect(m_pdoingTesting.data(), &TestProjectProcess::giveupSampleHolesChanged,
            mptesting.data(), &Testing::giveupSampleShowHole,
            Qt::QueuedConnection);
    connect(m_pdoingTesting.data(), &TestProjectProcess::giveupChannelFlash,
            mptesting.data(), &Testing::giveupSampleChannelFlash,
            Qt::QueuedConnection);
    connect(m_pdoingTesting.data(), &TestProjectProcess::allSamplesFinished,
            mptesting.data(), &Testing::allSampleTested,
            Qt::QueuedConnection);
    connect(m_pdoingTesting.data(), &TestProjectProcess::allSamplesFinished,
            this, [this]() {
        if (QLabel *lab = ui->StatusBar->findChild<QLabel*>(QStringLiteral("label_focusTask")))
            setFocusTaskStatusText(lab, tr("待机"));
    }, Qt::QueuedConnection);

    connect(mshowModuledata.data(), &displayChanneldata::signalShowPPPError,
            m_pdoingTesting.data(), &TestProjectProcess::handlePppBaselineInvalid,
            Qt::QueuedConnection);
    connect(mshowModuledata.data(), &displayChanneldata::signalPrpReadFailed,
            m_pdoingTesting.data(), &TestProjectProcess::handlePrpBaselineInvalid,
            Qt::QueuedConnection);
    connect(mshowModuledata.data(), &displayChanneldata::signalPppZeroGiveUp,
            this, [this](int sampleId, quint8 channel) {
        emit ReminderTextOut(TESTINGLOG,
            tr("样本%1 PPP读初值连续8次为0，已停止该样本").arg(sampleId));
        CustomMessageBox box(
                    tr("PPP读初值异常"),
                    tr("样本%1 的PPP通道读数连续 8 次为 0。\n\n点击确定后将丢弃该通道PPP，并放弃该样本（后续PRP不再测试）。")
                        .arg(sampleId),
                    QMessageBox::Warning, QMessageBox::Ok, nullptr);
        box.setWindowModality(Qt::ApplicationModal);
        box.setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
        if (const QScreen *screen = QGuiApplication::primaryScreen()) {
            box.adjustSize();
            box.move(screen->availableGeometry().center() - box.rect().center());
        }
        box.raise();
        box.activateWindow();
        box.exec();
        if (m_pdoingTesting) {
            m_pdoingTesting->handlePppZeroGiveUp(sampleId, channel);
        }
    }, Qt::QueuedConnection);
    connect(mshowModuledata.data(), &displayChanneldata::signalPrpZeroSkipReagent,
            this, [this](int sampleId, quint8 reagent, quint8 focusActive) {
        const QString reagentName = GlobalData::mapIndexReagentnames(reagent);
        emit ReminderTextOut(TESTINGLOG,
            tr("样本%1 %2 PRP读初值连续8次为0，将丢弃当前试剂杯")
                .arg(sampleId).arg(reagentName));
        CustomMessageBox box(
                    tr("PRP读初值异常"),
                    tr("样本%1 的%2 PRP通道读数连续 8 次为 0。\n\n点击确定后将丢弃当前试剂杯；若该样本还有其他试剂，将继续测试。")
                        .arg(sampleId).arg(reagentName),
                    QMessageBox::Warning, QMessageBox::Ok, nullptr);
        box.setWindowModality(Qt::ApplicationModal);
        box.setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);
        if (const QScreen *screen = QGuiApplication::primaryScreen()) {
            box.adjustSize();
            box.move(screen->availableGeometry().center() - box.rect().center());
        }
        box.raise();
        box.activateWindow();
        box.exec();
        if (m_pdoingTesting) {
            m_pdoingTesting->handlePrpZeroSkipReagent(sampleId, reagent, focusActive);
        }
    }, Qt::QueuedConnection);
    connect(mshowModuledata.data(), &displayChanneldata::signalPrpReadFailed,
            this, [this]() {
        if (!cglobal::g_PauseTesting.load() && cglobal::g_StartTesting.load()) {
            cglobal::g_PauseTesting = true;
            QToolButton *pPauseBtn = ui->toolButton_quality_sample;
            if (pPauseBtn) {
                pPauseBtn->setText(tr("继续"));
                pPauseBtn->setIcon(m_continueIcon);
                pPauseBtn->setStyleSheet(mPauseSateQss);
            }
        }
        emit ReminderTextOut(TESTINGLOG,
            tr("PRP初值异常（须大于0且小于该通道PPP），测试已暂停，点「继续」后按当前焦点重读"));
        if (INI_File().getexperimentalMode()) {
            CustomMessageBox::warning(this, tr("PRP异常警告"),
                tr("未读到有效PRP初值（须大于0，且小于该通道PPP）。\n已暂停测试，请处理后再点「继续」重读。"));
        }
    }, Qt::QueuedConnection);


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
    connect(m_pdoingTesting.data(), &TestProjectProcess::stopChannelAcquisition,
            mshowModuledata.data(), &displayChanneldata::slotStopChannelAcquisition,
            Qt::QueuedConnection);

    //样本吐出试剂开始测试清洗针
    connect(mshowModuledata.data(), &displayChanneldata::addSampmeTestCleanPin,
            m_pdoingTesting.data(),&TestProjectProcess::slotaddSampmeTestCleanPin,
            Qt::QueuedConnection);

    //通道完成测试
    connect(mshowModuledata.data(),&displayChanneldata::testComplete,
            m_pdoingTesting.data(),&TestProjectProcess::SampleReagTestFinish,
            Qt::QueuedConnection);

    connect(this,&MainWindow::resumeaTestTask,m_pdoingTesting.data(),
            &TestProjectProcess::handleResumeaTestTask,
            Qt::QueuedConnection);

    //测试中添加样本
    connect(mptesting.data(),&Testing::testingaddsample,
            m_pdoingTesting.data(),&TestProjectProcess::slottestingaddsample);

    /*单个项目完成曲线图保存到数据库*/
    connect(mshowModuledata.data(),&displayChanneldata::finishtestProgress,
            m_graphplot.data(),&GraphPlot::CompleteOneSample);

    //查询删除样本 分配了任务没测试的也要删掉
    auto *inquireData = FullyAutomatedPlatelets::pinstanceInquiredata();
    if (inquireData) {
        connect(inquireData, &Inquire_Sql_Info::sycnDelTestSample,
                mptesting.data(),&Testing::onsycnDelTestSample);
    }



    connect(m_pdoingTesting.data(),&TestProjectProcess::setprogressinitboolwaittext,
            mptesting.data(),&Testing::updateChannelProgressAndStatus,
            Qt::QueuedConnection);

    connect(m_pdoingTesting.data(),&TestProjectProcess::sycn_SampleTestingChangInitColor,
            mptesting.data(),&Testing::SampleTestingChangInitColor,
            Qt::QueuedConnection);

    connect(m_pdoingTesting.data(),&TestProjectProcess::bloodSampleHintStage,
            mptesting.data(),&Testing::updateBloodHoleHintStage,
            Qt::QueuedConnection);

    connect(m_pdoingTesting.data(), &TestProjectProcess::instrumentMotionHint,
            mptesting.data(), &Testing::setInstrumentMotionHint,
            Qt::QueuedConnection);

    /*弃杯成功*/
    connect(m_pdoingTesting.data(),&TestProjectProcess::throwtesttube,
            mptesting.data(),&Testing::slotThrowtesttube,
            Qt::QueuedConnection);

    //断线重连
    connect(mlocalSerial.data(),&SuoweiSerialPort::connectEquipmentagin,
            m_pdoingTesting.data(),&TestProjectProcess::slot_connectEquipmentagin);
    connect(this, &MainWindow::instrumentWentOffline,
            m_pdoingTesting.data(), &TestProjectProcess::handleInstrumentOffline,
            Qt::QueuedConnection);
    connect(this, &MainWindow::instrumentCameOnline,
            m_pdoingTesting.data(), &TestProjectProcess::handleInstrumentCameOnline,
            Qt::QueuedConnection);
    connect(m_pdoingTesting.data(), &TestProjectProcess::abortAllOpenChannelAcquisition,
            mshowModuledata.data(), &displayChanneldata::slotAbortAllOpenChannelAcquisition,
            Qt::QueuedConnection);


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
        if (!cglobal::g_PauseTesting.load() && cglobal::g_StartTesting.load()) {
            cglobal::g_PauseTesting = true;
            QToolButton *pPauseBtn = ui->toolButton_quality_sample;
            if (pPauseBtn) {
                pPauseBtn->setText(tr("继续"));
                pPauseBtn->setIcon(m_continueIcon);
                pPauseBtn->setStyleSheet(mPauseSateQss);
            }
        }
        emit ReminderTextOut(TESTINGLOG,
            tr("PPP初值异常: %1（正常 %2-%3），测试已暂停，点「继续」后按当前焦点重读")
                .arg(value)
                .arg(GlobalData::kPppBaselineMin)
                .arg(GlobalData::kPppBaselineMax));
        if (INI_File().getexperimentalMode()) {
            CustomMessageBox::warning(this, tr("PPP异常警告"),
                tr("检测到PPP异常值: %1\n正常范围: %2-%3\n已暂停测试，请处理后再继续")
                    .arg(value)
                    .arg(GlobalData::kPppBaselineMin)
                    .arg(GlobalData::kPppBaselineMax));
        }
    }, Qt::QueuedConnection);


}


void MainWindow::pauseObtainmodulecommand(const QString& info, bool pauseRequested)
{
    if (!m_moduletimerThread || !m_boardData) {
        QLOG_WARN() << "无法控制模块数据采集 - 定时器线程未初始化(模组和主板)";
        return;
    }

    //模组暂停||主板暂停
    if(pauseRequested){
        emit stopObatinMachineInfo();
        pauseBoardAcquisition();
    }
    else
    {
        emit aNewconnectMachine();
        resumeBoardAcquisition();
    }

    QString action = pauseRequested ? tr("暂停") : tr("恢复");
    QString message = QString("%1获取主板/模组数据").arg(action);
    emit ReminderTextOut(PROMPTLOG, message);
    QLOG_INFO() << info << " - " << message;
}

//提供一个切换监听状态的方法
void MainWindow::toggleUsbListening(bool enable)
{
    if (enable) {
        listentoUsb();
    } else {
        stopListenToUsb();
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

// 断开USB监听
void MainWindow::stopListenToUsb()
{
    if (m_USBListener) {
        // 断开所有信号连接
        disconnect(m_USBListener.data(), nullptr, this, nullptr);

        // 停止监听
        m_USBListener->stopListening();

        // 释放资源
        m_USBListener.reset();
    }
}


void MainWindow::onDeviceConnected(usbDevice dev)
{
    QLOG_DEBUG()<<"仪器重连类型:"<<dev.type;
    if (QTimer *debounce = usbUnplugDebounceTimer(this)) {
        debounce->stop();
        debounce->disconnect(this);
    }
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
    QTimer *debounce = usbUnplugDebounceTimer(this);
    debounce->disconnect(this);
    connect(debounce, &QTimer::timeout, this, [this, dev]() {
        QString usbDisconnect = QString("仪器断线:%1").arg(dev.type);
        const QString ErrOut = "仪器USB掉线,请检查接口!";
        slotconnectionStateChanged(false);
        emit disConnectCloseSerial();
        emit ReminderTextOut(ERRORLOG, ErrOut);
        CustomMessageBox::warning(this, usbDisconnect, ErrOut);
    });
    debounce->start();
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
    QLOG_WARN() << "退出软件：关闭主窗口并退出进程";
    if (m_systemTray) {
        m_systemTray->hide();
    }
    hide();
    QApplication::quit();
}




// 线程安全入口（MainWindow.cpp）
void MainWindow::ThreadSafeReminder(QString titleStr, QString outputText) {
    // 1. 线程安全检查
    if (QThread::currentThread() != this->thread()) {
        // 跨线程调用：安全转发到主线程
        QMetaObject::invokeMethod(this, "onReminderRequested",
            Qt::QueuedConnection,  // 异步队列连接
            Q_ARG(QString, titleStr),
            Q_ARG(QString, outputText));
        return;
    }

    // 主线程直接调用
    RealReminderImpl(titleStr, outputText);
}

// 安全槽函数（主线程执行）
void MainWindow::onReminderRequested(QString title_, QString outputText) {
    RealReminderImpl(title_, outputText);
}


void MainWindow::RealReminderImpl(QString title, QString outputText)
{
    {
        QWriteLocker locker(&m_reminderLock);
        //快速判断标题是否已存在（QSet优化查询效率）
        if (m_reminderTitleStr.contains(title)){ return;}
    }


    //使用QPointer跟踪弹窗对象生命周期（避免悬空指针）
    QPointer<warn_interface> pwarn = new warn_interface(title, outputText);


    //配置弹窗属性（使用Qt属性链式调用）
    pwarn->setWindowFlags(Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::WindowDoesNotAcceptFocus);
        pwarn->setAttribute(Qt::WA_DeleteOnClose);   // 关闭时自动销毁
        pwarn->setAttribute(Qt::WA_ShowWithoutActivating); // 不抢占焦点
        pwarn->setWindowModality(Qt::NonModal);     // 非模态窗口

    //集中初始化弹窗内容（减少冗余调用）
    pwarn->settitle(title);
    pwarn->setremtext(outputText);
    pwarn->replaceSupplyIndex(-1);

    //信号连接：用户确认后清理标题集合
    connect(pwarn, &warn_interface::makesure, this,
        [this](int,  QString title) {
            QWriteLocker lock(&m_reminderLock);
            m_reminderTitleStr.remove(title);
    },
    Qt::QueuedConnection);

    //自适应多屏幕居中（基于主窗口所在的屏幕）
    if (const QScreen* activeScreen = QGuiApplication::primaryScreen()) {
        const QRect screenGeometry = activeScreen->availableGeometry();
        pwarn->move(
            screenGeometry.center() - pwarn->rect().center()
        );
    }

    //显示弹窗并记录标题（原子操作避免竞态条件）
    pwarn->show();
    pwarn->raise(); // 确保窗口在最前面
    pwarn->activateWindow(); // 激活窗口
    m_reminderTitleStr.insert(title);
    return;
}



void  MainWindow::reminderFunctionWidget(QString title,QString outputtext_,QList<QString> btntext)
{
    int btn_ = 0;
    if(preminder == nullptr)
        preminder = new FunctionCustomWidget(title,outputtext_);
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
           if (auto *writer = FullyAutomatedPlatelets::pinstanceWirteBoard()) {
               emit writer->closeSerial();
           }
           if (auto *mw = FullyAutomatedPlatelets::mainWindow()) {
               mw->deleteExitSoftware();
           }
        }
        else
        {
            m_shutdownClean = true;
            equipmentinitActive(false,m_shutdownClean);  //开机关复位负速度运动模式清洗
        }
    });

    //直接退出
    connect(preminder.data(),&FunctionCustomWidget::sender_3function_,
            this,[=](){ writeConsumablesExit();
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
    CustomMessageBox::warning(this,"开机初始化失败", "未完成坐标同步，请重新联机后启动软件...");
    //ThreadSafeReminder("开机初始化失败", "未完成坐标同步，请重新联机后启动软件.");
}

void MainWindow::FirstConnectCleanEquipment()
{
    try {
        // 1. 检查清洗液是否充足
        bool hasEnoughCleanFluid = checkCleanFluidSufficiency();

        // 2. 检查坐标同步状态
        bool isAxisSyncFinished = getbAxisSycnFinished();
        if (!isAxisSyncFinished) {
           //ThreadSafeReminder("开机初始化失败","未连机同步坐标,请联机重新打开软件!");
           CustomMessageBox::warning(this,"开机初始化失败", "未完成坐标同步，请重新联机后启动软件...");
           return;
        }

        if(!m_MachineAlreadyInitCleanned){
           initProgressBar("开机清洗进度",false,true);
           equipmentinitActive(hasEnoughCleanFluid,m_shutdownClean);
        }

    } catch (const std::exception& e) {
        qCritical() << "Initialization failed:" << e.what();
        //ThreadSafeReminder("严重错误", "初始化过程中发生异常，请联系技术支持。");
        CustomMessageBox::warning(this,"严重错误", "初始化过程中发生异常，请联系技术支持。");
    }
    return;
}



void MainWindow::serialConnection()
{
    mlocalSerial = FullyAutomatedPlatelets::pinstanceserialusb();
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
        //ThreadSafeReminder("指令异常",reminder_);
        CustomMessageBox::warning(this,"指令异常", reminder_);
    });


    //串口开关状态
    connect(serial,&SuoweiSerialPort::connectionStateChanged,
            this,&MainWindow::slotconnectionStateChanged);

    connect(serial, &SuoweiSerialPort::prepareReconnectGetData,
            this, &MainWindow::recvprepareReconnectGetData);



    //串口异常消息提示
    connect(serial,&SuoweiSerialPort::outRminderWidget,
            this,[=](QString title, QString reminderInfo){
        //ThreadSafeReminder(title,reminderInfo);
        CustomMessageBox::warning(this,title, reminderInfo);
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

    // 先接信号再开线程，避免打开成功的 connectionStateChanged 被丢掉
    serial->suoweiportthreadbegin();
    return;
}

void MainWindow::creatbeginreadmodule()
{
    QLOG_DEBUG() << "获取模组数据线程ID:" << QThread::currentThreadId();

    m_moduletimerThread = new moduletimerThread();
    m_moduletimerThread->moveToThread(&m_threadModule);

    // 线程开始时：获取当前模组数量，设置定时器，启动所有模组
    connect(&m_threadModule, &QThread::started, this, [this]() {
        // 获取实际模组数量（根据您的硬件配置）
        quint8 moduleCount = 0,indexType = KS1200;
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, indexType);
        switch(indexType){
            case KS600: moduleCount = 1; break;
            case KS800: moduleCount = 2; break;
            case KS1200: moduleCount = 3; break;
            default: moduleCount = 3; break;
        }
        QMetaObject::invokeMethod(m_moduletimerThread, [this, moduleCount]() {
            m_moduletimerThread->setModuleCount(moduleCount);
            m_moduletimerThread->startAll();
        }, Qt::QueuedConnection);
    });

    // 线程结束时清理对象
    connect(&m_threadModule, &QThread::finished, m_moduletimerThread, &QObject::deleteLater);

    // 连接信号：工作线程请求发送某个模组的数据 -> 主线程实际执行
    connect(m_moduletimerThread, &moduletimerThread::requestSendModuleData,
            this, &MainWindow::sendOneModuleData, Qt::QueuedConnection);

    // 可选：外部停止/启动控制（保留原有信号接口）
    connect(this, &MainWindow::stopObatinMachineInfo,
            m_moduletimerThread, &moduletimerThread::stopAll);
    connect(this, &MainWindow::aNewconnectMachine,
            m_moduletimerThread, &moduletimerThread::startAll);

    m_threadModule.start();
}



void MainWindow::sendOneModuleData(int moduleIndex)
{
    if (!cglobal::gserialConnecStatus){
        return;
    }

    // 模组索引有效性检查（原协议中模组编号为 1~3）
    if (moduleIndex < 1 || moduleIndex > 3) {
        QLOG_WARN() << "Invalid module index:" << moduleIndex << ", expected 1~3";
        return;
    }

    Monitor_TrayTest* pobtainModuledata = FullyAutomatedPlatelets::pinstanceobtainModuledata();
    if (!pobtainModuledata) {
        QLOG_WARN() << "Failed to obtain Monitor_TrayTest instance";
        return;
    }

    QByteArray moduleData;
    // 注意：原 sycnmoduledata 第一个参数是模组编号（可能是 1-based）
    pobtainModuledata->sycnmoduledata(static_cast<quint8>(moduleIndex), moduleData);
    emit sendOneDirectives(moduleData, QString("获取模组%1数据").arg(moduleIndex));
}





void MainWindow::Traverse_through_ModuleReadInfo()
{
    // 获取单例实例（假设该方法返回有效指针）
    Monitor_TrayTest* pobtainModuledata = FullyAutomatedPlatelets::pinstanceobtainModuledata();
    if (!pobtainModuledata) {
        QLOG_WARN() << "Failed to obtain Monitor_TrayTest instance";
        return;
    }

    // 启动监控线程
    pobtainModuledata->_start();

    // 防止重复连接（避免多次调用导致槽被重复触发）
    static bool isConnected = false;
    if (!isConnected) {
        connect(this, &MainWindow::controlallchnstate,
                pobtainModuledata, &Monitor_TrayTest::open_orclose_chn);
        isConnected = true;
    }
}





/*主界面底栏：USB / 温度 / 耗材 统一成状态条*/
void MainWindow::applyStatusBarStyle()
{
    ensureFocusTaskLabel(ui);
    ui->StatusBar->setFixedHeight(64);
    ui->StatusBar->setStyleSheet(QStringLiteral(
        "QWidget#StatusBar{"
        "  background-color:#F4F8FB;"
        "  border:none;"
        "  border-top:1px solid #D4E4EE;"
        "  border-radius:0px;"
        "}"));
    ui->horizontalLayout_status->setSpacing(8);
    ui->horizontalLayout_status->setContentsMargins(8, 4, 8, 4);

    const QString cardStyle = QStringLiteral(
        "background:transparent; border:none;");
    ui->widget_usbstate->setAttribute(Qt::WA_StyledBackground, true);
    ui->widget_ModulI->setAttribute(Qt::WA_StyledBackground, true);
    ui->widget_ModulII->setAttribute(Qt::WA_StyledBackground, true);
    ui->widget_ModulIII->setAttribute(Qt::WA_StyledBackground, true);
    ui->widget_usbstate->setStyleSheet(cardStyle);
    ui->widget_ModulI->setStyleSheet(
        QStringLiteral("QWidget#widget_ModulI{background:#FFFFFF;border:1px solid #E2EEF4;border-radius:8px;}"));
    ui->widget_ModulII->setStyleSheet(
        QStringLiteral("QWidget#widget_ModulII{background:#FFFFFF;border:1px solid #E2EEF4;border-radius:8px;}"));
    ui->widget_ModulIII->setStyleSheet(
        QStringLiteral("QWidget#widget_ModulIII{background:#FFFFFF;border:1px solid #E2EEF4;border-radius:8px;}"));
    ui->widget_ReminderLiqune->setStyleSheet(
        QStringLiteral("QWidget#widget_ReminderLiqune{background:transparent;border:none;color:#334E5E;}"));
    ui->widget_ReminderLiqune->setMinimumWidth(380);

    ui->widget_usbstate->setMinimumSize(118, 32);
    ui->horizontalLayout_2->setContentsMargins(10, 4, 12, 4);
    ui->horizontalLayout_2->setSpacing(8);

    ui->label_machineStaus_show->setFixedSize(12, 12);
    ui->label_machineStaus_show->setScaledContents(false);
    ui->label_machineStaus_show->clear();

    const QString nameStyle = QStringLiteral(
        "border:none;background:transparent;color:#5A7384;"
        "font-family:'Microsoft YaHei';font-size:13px;padding:0px;");
    ui->label_Module1->setStyleSheet(nameStyle);
    ui->label_Module1_2->setStyleSheet(nameStyle);
    ui->label_Module1_3->setStyleSheet(nameStyle);
    ui->label_Module1->setText(tr("模组1"));
    ui->label_Module1_2->setText(tr("模组2"));
    ui->label_Module1_3->setText(tr("模组3"));
    ui->label_Module1->setMinimumWidth(44);
    ui->label_Module1->setMaximumWidth(56);
    ui->label_Module1_2->setMinimumWidth(44);
    ui->label_Module1_2->setMaximumWidth(56);
    ui->label_Module1_3->setMinimumWidth(44);
    ui->label_Module1_3->setMaximumWidth(56);

    ui->label_connectstate->setStyleSheet(QStringLiteral(
        "border:none;background:transparent;font-family:'Microsoft YaHei';"
        "font-size:14px;font-weight:bold;"));

    ui->gridLayout->setHorizontalSpacing(6);
    ui->gridLayout->setVerticalSpacing(3);
    ui->gridLayout->setContentsMargins(4, 2, 4, 2);

    if (ui->MachineAxisValue) {
        ui->MachineAxisValue->hide();
    }

    const int wasteIdx = ui->horizontalLayout_status->indexOf(ui->show_wastlique);
    if (wasteIdx >= 0) {
        bool hasStretch = false;
        for (int i = 0; i < ui->horizontalLayout_status->count(); ++i) {
            if (ui->horizontalLayout_status->itemAt(i)
                    && ui->horizontalLayout_status->itemAt(i)->spacerItem()) {
                hasStretch = true;
                break;
            }
        }
        if (!hasStretch) {
            ui->horizontalLayout_status->insertStretch(wasteIdx, 1);
        }
    }

    updateConnectionUI(cglobal::gserialConnecStatus);
}

QString MainWindow::consumableChipCaption(int reagentIndex) const
{
    switch (reagentIndex) {
    case INDEX_AA_CONSUMABLE:          return QStringLiteral("AA L");
    case INDEX_AA_1_CONSUMABLE:        return QStringLiteral("AA R");
    case INDEX_ADP_CONSUMABLE:         return QStringLiteral("ADP L");
    case INDEX_ADP_1_CONSUMABLE:       return QStringLiteral("ADP R");
    case INDEX_EPI_CONSUMABLE:         return QStringLiteral("EPI L");
    case INDEX_EPI_1_CONSUMABLE:       return QStringLiteral("EPI R");
    case INDEX_COL_CONSUMABLE:         return QStringLiteral("COL L");
    case INDEX_COL_1_CONSUMABLE:       return QStringLiteral("COL R");
    case INDEX_RIS_CONSUMABLE:         return QStringLiteral("RIS L");
    case INDEX_RIS_1_CONSUMABLE:       return QStringLiteral("RIS R");
    case INDEX_CLEANLINQUE_CONSUMABLE: return tr("清洗");
    default: return QString();
    }
}

void MainWindow::applyConsumableChipStyle(QLabel* label, double percent, quint8 threshold)
{
    if (!label) {
        return;
    }
    QString bg = QStringLiteral("#E6F6EA");
    QString fg = QStringLiteral("#1B7A3D");
    if (percent <= 0.0 || percent <= static_cast<double>(threshold)) {
        bg = QStringLiteral("#FDE8E8");
        fg = QStringLiteral("#C62828");
    } else if (percent <= 30.0) {
        bg = QStringLiteral("#FFF4D6");
        fg = QStringLiteral("#9A6B00");
    }

    label->setMinimumSize(58, 22);
    label->setMaximumHeight(26);
    label->setAlignment(Qt::AlignCenter);
    label->setStyleSheet(QStringLiteral(
        "QLabel{border:none;border-radius:8px;padding:1px 6px;"
        "font-family:'Microsoft YaHei';font-size:12px;font-weight:bold;"
        "background-color:%1;color:%2;}")
                             .arg(bg, fg));
}

/*主界面UI显示仪器连接状态*/
void  MainWindow::updateConnectionUI(bool connected)
{
    // 状态点 + 短文案
    ui->label_machineStaus_show->setPixmap(QPixmap());
    ui->label_machineStaus_show->setFixedSize(12, 12);
    ui->label_machineStaus_show->setStyleSheet(connected
        ? QStringLiteral("border:none;border-radius:6px;background-color:#2E9B57;")
        : QStringLiteral("border:none;border-radius:6px;background-color:#D32F2F;"));

    ui->widget_usbstate->setStyleSheet(connected
        ? QStringLiteral("QWidget#widget_usbstate{background:#E8F6EE;border:none;border-radius:16px;}")
        : QStringLiteral("QWidget#widget_usbstate{background:#FDECEC;border:none;border-radius:16px;}"));

    ui->label_connectstate->setStyleSheet(QStringLiteral(
        "border:none;background:transparent;font-family:'Microsoft YaHei';"
        "font-size:14px;font-weight:bold;color:%1;")
            .arg(connected ? QStringLiteral("#1B7A3D") : QStringLiteral("#C62828")));
    ui->label_connectstate->setText(connected ? tr("已连接") : tr("离线"));
}

void  MainWindow::slotconnectionStateChanged(bool connected)
{
    const bool wasConnected = cglobal::gserialConnecStatus.load();
    cglobal::gserialConnecStatus = connected;

    // 更新UI元素
    updateConnectionUI(connected);

    // 触发声音提醒
    emit AlarmReminderSound(!connected);

    if (!connected && wasConnected) {
        emit cleanModuleBuffData();
        emit instrumentWentOffline();
    } else if (connected) {
        // 重连时串口线程可能已先把 gserialConnecStatus 置 true，不能只靠边沿
        emit instrumentCameOnline();
    }
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
    static bool initialized = false;
    if (initialized) return;
    initialized = true;

    USB_InitConnect* mpSingleactive = FullyAutomatedPlatelets::pinstanceSingleactive();
    SuoweiSerialPort* mpserialusbinfo = FullyAutomatedPlatelets::pinstanceserialusb();

    auto equipmentConfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();
    auto instrument = FullyAutomatedPlatelets::pinstanceinstrument();


    // 线程间命令发送
    connect(mpSingleactive, &USB_InitConnect::writeCommand,
            mpserialusbinfo, &SuoweiSerialPort::writedataToEquipment,
            Qt::QueuedConnection);

    // 界面动作触发数据处理
    connect(this, &MainWindow::CeratActionDate,
            mpSingleactive, &USB_InitConnect::onCeratActionData,
            Qt::QueuedConnection);

    // 串口数据接收转发
    connect(mpserialusbinfo, &SuoweiSerialPort::serialrecvedata,
            mpSingleactive, &USB_InitConnect::recvSerialdata,
            Qt::QueuedConnection);

    // 开机清洗完成通知
    connect(mpSingleactive, &USB_InitConnect::bootInitCleanFinished,
            this, &MainWindow::slotbootInitCleanFinished,
            Qt::QueuedConnection);

    connect(mpSingleactive, &USB_InitConnect::smartCupClearFinished,
            this, &MainWindow::slotSmartCupClearFinished,
            Qt::QueuedConnection);

    // 清洗进度
    connect(mpSingleactive, &USB_InitConnect::CleaningProgress,
            this, &MainWindow::slotCleaningProgress,
            Qt::QueuedConnection);

    // 断线重连
    connect(mpserialusbinfo, &SuoweiSerialPort::Disconnectandreconnect,
            mpSingleactive, &USB_InitConnect::slotDisconnectandreconnect,
            Qt::QueuedConnection);

    // 复位按钮完成
    connect(mpSingleactive, &USB_InitConnect::btn_resetComplete, this,
            [this]() {
                ui->toolButton_quality_reset->setEnabled(true);
                QLOG_DEBUG() << "按钮复位完成!";
            },
            Qt::QueuedConnection);

    // 运动下位高度完成
    connect(mpSingleactive, &USB_InitConnect::finishviewdownheigh, this,
            [equipmentConfig](int index_) {
                if (index_ == EQUIPMENT_GRIPPER_AIR_CALIB) {
                    equipmentConfig->onGripperAirMotionFinished();
                } else {
                    equipmentConfig->_finishmovetestdownhigh(index_);
                }
            },
            Qt::QueuedConnection);

    // 按钮清洗完成（扣除试剂）
    connect(mpSingleactive, &USB_InitConnect::btn_CleanComplete, this,
            [instrument]() {
                instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE, 2);
            },
            Qt::QueuedConnection);

    // 任务执行失败提示
    connect(mpSingleactive, &USB_InitConnect::taskErrorOccurred, this,
            [this](const QString& errorMsg) {
                CustomMessageBox::warning(this, "动作执行失败", errorMsg);
            },
            Qt::QueuedConnection);

    // 启动工作线程
    mpSingleactive->_startthread();
}


void MainWindow::showTrayMessage(const QString& title, const QString& message,
                               QSystemTrayIcon::MessageIcon icon, int timeout)
{
    if (!m_systemTray || !m_systemTray->isVisible()) {
        QLOG_WARN() << "无法显示消息：托盘图标未初始化或不可见";
        return;
    }

    if (!QSystemTrayIcon::supportsMessages()) {
        QLOG_WARN() << "系统不支持托盘消息功能";
        return;
    }

    // 确保在主线程中执行
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "showTrayMessage",
                                Qt::QueuedConnection,
                                Q_ARG(QString, title),
                                Q_ARG(QString, message),
                                Q_ARG(QSystemTrayIcon::MessageIcon, icon),
                                Q_ARG(int, timeout));
        return;
    }

    m_systemTray->showMessage(title, message, icon, timeout);
}

void MainWindow::toggleWindowVisibility()
{
    if (isVisible() && !isMinimized()) {
        hide();
    } else {
        showNormal();
        raise();
        activateWindow();
    }
}
void MainWindow::setTrayIcon()
{
    QIcon trayIcon;
    QString customIconPath = ":/Picture/suowei.png";

    if (QFile::exists(customIconPath)) {
        trayIcon = QIcon(customIconPath);
        QLOG_DEBUG() << "成功加载自定义图标:" << customIconPath;
    } else {
        QLOG_WARN() << "自定义图标不存在:" << customIconPath;
        // 尝试使用内置图标
        trayIcon = QApplication::style()->standardIcon(QStyle::SP_ComputerIcon);
    }

    m_systemTray->setIcon(trayIcon);
    QLOG_DEBUG() << "托盘图标设置" << (trayIcon.isNull() ? "失败" : "成功");
}
void MainWindow::checkTrayAvailability()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        CustomMessageBox::warning(this,
                            tr("系统托盘"),
                            tr("系统不支持托盘功能！\n请确保系统托盘服务正在运行."));
        QCoreApplication::exit(EXIT_FAILURE);
    }

    // 额外检查消息系统支持
    if (!QSystemTrayIcon::supportsMessages()) {
        QLOG_WARN() << "系统不支持托盘消息提示功能";
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
    // 清空现有菜单项（如果之前已创建）
    m_trayMenu->clear();

    // 创建并添加消息通知动作
    auto* messageAction = new QAction(tr("关于"), m_trayMenu);
    connect(messageAction, &QAction::triggered, this, [this] {
        ui->toolButton_about->click(); // 触发信号
    });
    m_trayMenu->addAction(messageAction);
    m_trayMenu->addSeparator();

     // 使用函数添加动作，避免结构体初始化问题
     auto addTrayAction = [this](const QString& text, std::function<void()> handler, bool addSeparator = false) {
         auto* action = new QAction(text, m_trayMenu);
         connect(action, &QAction::triggered, this, handler);
         m_trayMenu->addAction(action);
         if (addSeparator) {
             m_trayMenu->addSeparator();
         }
     };

     // 添加窗口控制动作
     addTrayAction(tr("显示主窗口"), [this] {
         showNormal();
         raise();
         activateWindow();
     });

     addTrayAction(tr("最小化到托盘"), [this] {
         hide();
     });

     addTrayAction(tr("打开设置"), [this] {
         openSettingsDialog();
     }, true); // 添加分隔符

     addTrayAction(tr("退出系统"), [this] {
         m_forceQuit = true;
         writeConsumablesExit();
     });
}

void MainWindow::connectTraySignals()
{
    // 托盘激活事件
    connect(m_systemTray, &QSystemTrayIcon::activated, this,
        [this](QSystemTrayIcon::ActivationReason reason) {
            if (reason == QSystemTrayIcon::DoubleClick) {
                toggleWindowVisibility();
            }else if(reason == QSystemTrayIcon::Trigger){

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
    m_systemTray = new QSystemTrayIcon(this);

    // 设置托盘图标
    setTrayIcon();

    m_systemTray->setToolTip(tr("全自动血小板聚集分析系统"));

    // 创建托盘菜单
    m_trayMenu = new QMenu(this);
    setupTrayMenuStyle();
    createTrayActions();

    // 设置上下文菜单
    m_systemTray->setContextMenu(m_trayMenu);

    // 事件连接
    connectTraySignals();

    // 显示托盘图标
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


    //测试界面
    auto *testFucn = FullyAutomatedPlatelets::pinstanceTesting();
    if (testFucn)
        mptesting = testFucn;
    mptesting.data()->initequipmentKind(minstrumentType);

    //设置界面
    FullyAutomatedPlatelets::pinstanceequipmentconfig()->sycnequipmentKind(minstrumentType);
    FullyAutomatedPlatelets::pinstanceequipmentconfig()->_initpara();

    //测试曲线
    m_graphplot = new GraphPlot(this);
    m_graphplot.data()->innitKindequipment();
    m_qcChartPage = new QcChartPage(this);


    MachineSetting*pequipmentconfig = FullyAutomatedPlatelets::pinstanceequipmentconfig();
    connect(mtestmoduleprotocol.data(),&theTestModuleProtocol::sendReminder,
            pequipmentconfig,&MachineSetting::slotsendReminder);


    connect(FullyAutomatedPlatelets::pinstanceequipmentconfig(),
            &MachineSetting::sycnViewCurvePara,
            m_graphplot.data(),&GraphPlot::recvsycnViewCurvePara);


    FullyAutomatedPlatelets::pinstancepatientdata()->initstyle();

    //开始按钮
    connect(ui->toolButton_quality_start, &QToolButton::clicked,
                this, &MainWindow::onStartTestClicked);

    setupMainInterface();
    applyStatusBarStyle();

    ui->tabWidget_Main->setCurrentIndex((int)MainInterfaceSubscripted::FunctionBlock_Testing); //默认显示主页
    GlobalData::mainseledview(ui->toolButton_sampleTest_1,true);
    return;
}

//控温时显示模组温度
void MainWindow::displayPara(int Indexmodul, const double tempvalve){
    recvModuleTemperature(Indexmodul,tempvalve);
}


void MainWindow::ChannelValueshow(QStringList moduleData)
{
    emit syncModuleChannelData(moduleData);
}




void MainWindow::initializeAltimeterHardware()
{
    if (!m_altimeterDelayTimer) {
        m_altimeterDelayTimer = new QTimer(this);
        m_altimeterDelayTimer->setSingleShot(true);
        connect(m_altimeterDelayTimer, &QTimer::timeout, this, [this]() {
            Q_EMIT triggerTestHeight();
            QLOG_INFO() << "[测高拍照] 延时结束，开始取图";
        });
    }
}

void MainWindow::bindAltimeterVision(TestOpcv *vision)
{
    if (!vision) {
        return;
    }
    m_altimeter = vision;
    setupAltimeterConnections();
    initCameras();
}

void MainWindow::setupAltimeterConnections()
{
    if (!m_altimeter || m_altimeterBound) {
        return;
    }
    m_altimeterBound = true;

    connect(this, &MainWindow::triggerTestHeight,
            m_altimeter, &TestOpcv::triggerCaptureAndRecognize,
            Qt::QueuedConnection);

    connect(m_altimeter, &TestOpcv::captureFailed, this,
            [this](QString titles, QString contentstr) {
                CustomMessageBox::warning(this, titles, contentstr);
            });

    connect(this, &MainWindow::findCameraIndexByDevicePath, m_altimeter,
            &TestOpcv::recvfindCameraIndexByDevicePath,
            Qt::QueuedConnection);

    connect(m_altimeter, &TestOpcv::captureFinished,
            this, &MainWindow::TestHeightFinish,
            Qt::QueuedConnection);

    connect(FullyAutomatedPlatelets::pinstanceTesting(), &Testing::sendRepPrpheight,
            m_altimeter, &TestOpcv::handleRepPrpheight,
            Qt::QueuedConnection);

    connect(this, &MainWindow::altimeterCameraPower,
            m_altimeter, &TestOpcv::setCameraPower,
            Qt::QueuedConnection);
}

void MainWindow::setAltimeterCameraPower(bool enable)
{
    emit altimeterCameraPower(enable);
}

void MainWindow::initCameras()
{
    QList<QCameraInfo> availableCameras = QCameraInfo::availableCameras();
    if(availableCameras.isEmpty()){
        emit ReminderTextOut(ERRORLOG, "无可用摄像头!");
        return;
    }

    // 查找仪器摄像头
    QCameraInfo instrumentCamera;

    foreach (const QCameraInfo &cameraInfo, availableCameras) {
        QString devName = cameraInfo.deviceName();
        QString desc = cameraInfo.description();

        // 检查是否是目标设备 (VID_1E45&PID_80228M)
        if (devName.contains("vid_1e45", Qt::CaseInsensitive) &&
            devName.contains("pid_8022", Qt::CaseInsensitive)) {
            instrumentCamera = cameraInfo;
            QLOG_DEBUG() << "发现目标摄像头:" << devName << "描述:" << desc;
            break;
        }

        // 同时保留原有的USB HD摄像头检测逻辑
        const QStringList keys = {"hd camera"};
        bool isUSB = std::any_of(keys.begin(), keys.end(), [&](const QString& key) {
            return desc.contains(key, Qt::CaseInsensitive);
        });

        if (isUSB && instrumentCamera.isNull()) {
            instrumentCamera = cameraInfo;
            QLOG_DEBUG() << "发现目标摄像头 HD camera:" << devName << "Description:" << desc;
        }
    }


    if (!instrumentCamera.isNull()) {
        QLOG_DEBUG() << "使用仪器专用摄像头:" << instrumentCamera.description();
        emit findCameraIndexByDevicePath(instrumentCamera.deviceName());
    }
}



void MainWindow::PromptInfo(const quint8 Index, const QString ReminderStr, const quint8 iActive)
{
    slotExecute_exception_prompt(Index, ReminderStr, iActive);
}


void MainWindow::handlecardSwipeSuccessful(const QString tips,
                                        quint8 indexReagent,
                                        quint8 totalnum,
                                        const QString datetime){
    if (mReminder) {
        disconnect(mReminder.data(), nullptr, nullptr, nullptr);
        mReminder.reset();
    }

    mReminder.reset(new CustomHighData(tips, CONSUMABLES_READ_SUCESSFULLY,
                                          indexReagent, totalnum, this));

    connect(mReminder.data(), &CustomHighData::closeDelWidget, this, [this]() {
         cleanupReminder();
    });

    saveWritingconsumablebatchnumber(indexReagent, datetime);
    mReminder->show();

    // 主板 RFID 写窗口很短：立刻发确认，并暂停 0xFE 轮询避免读命令冲掉写卡
    pauseMainboardPollForRecharge();
    const int rechargeGen = m_rechargePollGeneration;
    if (QualityControl *instrument = FullyAutomatedPlatelets::pinstanceinstrument()) {
        instrument->sltoAddConsumables(indexReagent, totalnum);
    }
    QTimer::singleShot(4000, this, [this, rechargeGen]() {
        if (m_rechargePollGeneration == rechargeGen)
            resumeMainboardPollAfterRecharge();
    });
}
void MainWindow::cleanupReminder() {
    if (mReminder) {
        // 断开所有连接
        disconnect(mReminder.data(), nullptr, nullptr, nullptr);
        // 关闭窗口
        mReminder->close();
        // 释放资源
        mReminder.reset();
    }
}


//保存写入耗材批号信息
void MainWindow::saveWritingconsumablebatchnumber(const quint8 indexConsu,const QString& batchDateNum)
{
    FileManager fileManager;
    const QString filefolder = "ConsumableBatchNumber";
    const QString consumableName = GenericFunctions::BiteMapingConsumablesName(indexConsu) +".txt";//耗材类型

    // 创建配置文件夹和文件
    if (fileManager.createDataFile(filefolder, consumableName, "")) {
        QLOG_DEBUG() << "文件创建成功"<<consumableName;
        // 追加数据
        if(fileManager.appendToFile(filefolder, consumableName, batchDateNum)){
            QLOG_DEBUG() << "批号数据写入成功:" << batchDateNum;
        } else{
            QLOG_DEBUG() << "批号数据写入失败:" << batchDateNum;
        }
    }else {
        QLOG_ERROR() << "文件创建失败:" << consumableName;
    }
}



//提示文字，刷卡试剂、刷卡值
void MainWindow::handleswipeCardSuccessfullyWritten(QString tips, int addindexReag, quint8 addBottle)
{
    QLOG_DEBUG()<<"确认充值成功"<<tips;
    resumeMainboardPollAfterRecharge();
    emit rechargesuccessful(addindexReag,addBottle);
    return;
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_forceQuit) {
        event->accept();
        return;
    }

    if (preminder && preminder->isVisible()) {
        event->ignore();
        return;
    }

    event->ignore();

    QString warningText, titleText;
    if (cglobal::g_StartTesting) {
       warningText = tr("样本测试中...强行退出可能导致测试异常，请等待测试完成！");
       titleText = tr("操作提示");
    } else {
       warningText = tr("确定退出并关闭软件？");
       titleText = tr("关闭软件");
    }

    if (preminder) {
        delete preminder.data();
        preminder = nullptr;
    }
    preminder = new FunctionCustomWidget(titleText, warningText, this);

    QList<QString> btnText;
    btnText << tr("取消退出") << tr("清洗后退出") << tr("确定退出");
    for (int i = 0; i < btnText.size(); ++i) {
        preminder->setTextbtnfunction(i, btnText[i]);
    }

    connect(preminder, &FunctionCustomWidget::sender_1function_, this, [this]() {
        QLOG_WARN() << "退出软件：用户取消退出";
        emit ReminderTextOut(NORMALLOG, tr("取消退出软件"));
        if (preminder) {
            preminder->close();
        }
    });

    connect(preminder, &FunctionCustomWidget::sender_2function_, this, [this]() {
        QLOG_WARN() << "退出软件：用户选择清洗后退出，已初始化清洗"
                    << m_MachineAlreadyInitCleanned;
        emit ReminderTextOut(NORMALLOG, tr("清洗后退出软件"));
        if (preminder) {
            preminder->close();
        }
        emit controlallchnstate(false);
        if (!m_MachineAlreadyInitCleanned) {
            m_forceQuit = true;
            writeConsumablesExit();
        } else {
            m_shutdownClean = true;
            equipmentinitActive(false, m_shutdownClean);
        }
    });

    connect(preminder, &FunctionCustomWidget::sender_3function_, this, [this]() {
        QLOG_WARN() << "退出软件：用户确定退出";
        emit ReminderTextOut(NORMALLOG, tr("确定退出软件"));
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
        if (preminder) {
            preminder->close();
        }
        m_forceQuit = true;
        writeConsumablesExit();
    });

    preminder->setWindowModality(Qt::ApplicationModal);
    preminder->move(QApplication::desktop()->screen()->rect().center() - preminder->rect().center());
    preminder->show();
}


bool MainWindow::eventFilter(QObject *obj, QEvent *ev)
{
    return QMainWindow::eventFilter(obj, ev);
}




void MainWindow::displayHandoffUser()
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


void MainWindow::setupMainInterface()
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
        {MainInterfaceSubscripted::FunctionBlock_Consumables,    FullyAutomatedPlatelets::pinstanceinstrument()},
        {MainInterfaceSubscripted::FunctionBlock_Qc,             m_qcChartPage.data()}
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


    struct NavButton {
        QToolButton *button;
        QString text;
        int tabIndex;
    };
    const NavButton buttonInfo[] = {
        {ui->toolButton_sampleTest_1, tr("样本测试"),
         static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Testing)},
        {ui->toolButton_resultView_2, tr("结果预览"),
         static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewResults)},
        {ui->toolButton_testingCurve_3, tr("测试曲线"),
         static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewCurve)},
        {ui->toolButton_TestingResult_4, tr("结果查询"),
         static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Inquire)},
        {ui->toolButton_ControlSuppile_5, tr("耗材管理"),
         static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Consumables)}
    };

    const int buttonCount = static_cast<int>(sizeof(buttonInfo) / sizeof(buttonInfo[0]));
    for (int i = 0; i < buttonCount; ++i) {
        QToolButton *btn = buttonInfo[i].button;
        const int tabIndex = buttonInfo[i].tabIndex;
        GlobalData::mainseledview(btn, false);
        btn->setText(buttonInfo[i].text);
        connect(btn, &QToolButton::clicked, this, [this, tabIndex] {
            switchMainTab(static_cast<MainInterfaceSubscripted>(tabIndex));
            handleFunctionButtonClick(tabIndex);
        });
    }
}

void MainWindow::switchMainTab(MainInterfaceSubscripted index)
{
    const int tabIndex = static_cast<int>(index);
    if (tabIndex >= 0 && tabIndex < ui->tabWidget_Main->count()) {
        ui->tabWidget_Main->setCurrentIndex(tabIndex);
        highlightActiveButton(tabIndex);
        if (index == MainInterfaceSubscripted::FunctionBlock_PreviewResults) {
            FullyAutomatedPlatelets::pinstancepatientdata()->onPreviewPageShown();
        }
    }
}

void MainWindow::highlightActiveButton(int index)
{
    // 获取所有功能按钮的列表
    const QList<QPair<QToolButton*, int> > buttons = {
        qMakePair(ui->toolButton_sampleTest_1, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Testing)),
        qMakePair(ui->toolButton_resultView_2, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewResults)),
        qMakePair(ui->toolButton_testingCurve_3, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewCurve)),
        qMakePair(ui->toolButton_TestingResult_4, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Inquire)),
        qMakePair(ui->toolButton_ControlSuppile_5, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Consumables))
    };

    for (int i = 0; i < buttons.size(); ++i)
        GlobalData::mainseledview(buttons.at(i).first, buttons.at(i).second == index);
}




void MainWindow::setupUtilityButtons()
{
    // 设备设置按钮
    connect(ui->toolButton_EquipmentSetting, &QPushButton::clicked,this,
            [this]{ openSettingsDialog(); });

    // 关于按钮
    connect(ui->toolButton_about, &QPushButton::clicked, this, [this] {
        if (m_aboutMachine.isNull()) {
            m_aboutMachine = new AboutMachine(this);
        }
        //m_aboutMachine->sycnMd5Value(QCoreApplication::applicationFilePath());
        m_aboutMachine->sycnVersion(VERSIONALLNUM);
        m_aboutMachine->sycnEquipment(minstrumentType);
        m_aboutMachine->sycnSerialname(cglobal::gserialPortName);

        m_aboutMachine->show();
        m_aboutMachine->raise();
        m_aboutMachine->activateWindow();

    });

    //退出
    connect(ui->toolButton_Exit,&QToolButton::clicked,this,[this]{
        emit FullyAutomatedPlatelets::pinstancepatientdata()->hideCurveUi();
        this->close();
    });


    ui->toolButton_quality_qc->setText(tr("质控"));
    ui->toolButton_quality_qc->setIcon(makeQcNavIcon());
    connect(ui->toolButton_quality_qc, &QToolButton::clicked, this, [this] {
        switchMainTab(MainInterfaceSubscripted::FunctionBlock_Qc);
        logTabSwitchEvent(static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Qc));
    });

    QObject::connect(ui->toolButton_quality_stop ,&QToolButton::clicked,this,[this]{

        handleFunctionButtonClick(static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Testing));
        mptesting.data()->_showaddsamplewidget();
    });
}

void MainWindow::updateButtonSelectionState(int activeIndex)
{
    const QList<QPair<QToolButton*, int> > functionButtons = {
        qMakePair(ui->toolButton_sampleTest_1, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Testing)),
        qMakePair(ui->toolButton_resultView_2, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewResults)),
        qMakePair(ui->toolButton_testingCurve_3, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewCurve)),
        qMakePair(ui->toolButton_TestingResult_4, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Inquire)),
        qMakePair(ui->toolButton_ControlSuppile_5, static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Consumables))
    };

    if (activeIndex < 0) {
        QLOG_ERROR() << tr("Invalid button index:") << activeIndex;
        return;
    }

    for (int i = 0; i < functionButtons.size(); ++i)
        GlobalData::mainseledview(functionButtons.at(i).first, functionButtons.at(i).second == activeIndex);

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
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Consumables),  tr("进入耗材管理")},
        {static_cast<int>(MainInterfaceSubscripted::FunctionBlock_Qc),           tr("打开质控界面")}
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

    if (indexed == static_cast<int>(MainInterfaceSubscripted::FunctionBlock_PreviewResults)) {
        FullyAutomatedPlatelets::pinstancepatientdata()->onPreviewPageShown();
    }

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

    pEquipment->updatepara(cglobal::g_StartTesting.load());
    pEquipment->placeSettingsWindow();
    pEquipment->show();

    // Dropdown refresh does not need the serial port. UniqueConnection so a
    // later open (after the port exists) does not stack a second slot.
    auto* pPatient = FullyAutomatedPlatelets::pinstancepatientdata();
    if (pPatient) {
        connect(pEquipment, &MachineSetting::Synchronizeupdates,
            pPatient, &Calibrate::updatecommboxInfo, Qt::UniqueConnection);
    }

    // Connect signals only once
    if (!m_bmachineconfigureSignal) {
        if (!pPatient || !mlocalSerial) return;

        // Connect all signals in one clear block
        connect(pEquipment, &MachineSetting::SetParatoInstrument,
            this, [this](const QByteArrayList& d, QString s) {
            emit sendcodeList(d, s);
        });

        connect(pEquipment, &MachineSetting::testdownheight,
            this, [this](QByteArrayList d, int i) {
            emit CeratActionDate(i, d);
        });

        connect(pEquipment, &MachineSetting::OpenChannelMotor,
            this, [this](quint8 i, bool b) {
            emit controlmotorrunning(i, b);
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
    if (cglobal::g_StartTesting) {
        CustomMessageBox::warning(this, tr("操作禁止"),
                                  tr("仪器正在测试中，不能清洗。"));
        return;
    }

    QByteArrayList CleanBtnarry; CleanBtnarry.clear();
    QUIUtils::cleaningDoubleNeedleAction(CleanBtnarry,1);  //清洗(吸清洗剂吐出)
    emit  CeratActionDate(EQUIPMENT_BTN_CLEAN,CleanBtnarry);// 按钮清洗
    return;
}

//复位
void MainWindow::on_toolButton_quality_reset_clicked()
{
    if(!cglobal::gserialConnecStatus) return;
    if (cglobal::g_StartTesting) {
        CustomMessageBox::warning(this, tr("操作禁止"),
                                  tr("仪器正在测试中，不能复位。"));
        return;
    }

    QPoint locAxis(0,0);
    auto *pconfAxis = SingletonAxis::GetInstance();
    if(!pconfAxis)
    {
        QLOG_WARN()<<"调试界面复位坐标系为NULL";
    }
    pconfAxis->originPos(READ_OPERRAT,locAxis);

    Q_EMIT CeratActionDate(EQUIPMENT_BTN_REAET,QUIUtils::backZAxisandmoveOrigin(locAxis));

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
    quint8 threshold = 0;
    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,IndexReagent,threshold);
    QLOG_DEBUG()<<"试剂:"<<GlobalData::mapReagentNames(IndexReagent)<<"报警限制值"<<threshold;

    auto labelIt = displayRatioLab.constFind(IndexReagent);
    if (labelIt != displayRatioLab.constEnd() && *labelIt) {
        QLabel* displayLabel = *labelIt;
        QSignalBlocker blocker(displayLabel);
        const QString caption = consumableChipCaption(IndexReagent);
        const QString displayValue = QString::number(Remainingratio, 'f', 1);
        const QString preciseValue = QString::number(Remainingratio, 'f', 2);
        displayLabel->setText(QString("%1  %2%").arg(caption, displayValue));
        displayLabel->setToolTip(tr("%1 余量 %2%").arg(caption).arg(preciseValue));
        applyConsumableChipStyle(displayLabel, Remainingratio, threshold);
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
        //ThreadSafeReminder(info.first, info.second);
        CustomMessageBox::warning(this,info.first, info.second);
    } else {
        // 处理未知耗材类型
        QLOG_WARN() << "Unknown consumable index:" << Index;
    }
    return;
}

/*初始化更新耗材显示*/
void MainWindow::DisplaysConsumablesRemaining()
{
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
           ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT, reagentIndex, threshold);

           const double ratio = ConsumablesOper::displayedPercent(static_cast<quint8>(reagentIndex));
           const QString caption = consumableChipCaption(reagentIndex);
           const QString displayValue = QString::number(ratio, 'f', 1);
           const QString preciseValue = QString::number(ratio, 'f', 2);

           QSignalBlocker blocker(displayLabel);
           displayLabel->setText(QString("%1  %2%").arg(caption, displayValue));
           displayLabel->setToolTip(tr("%1 余量 %2%").arg(caption).arg(preciseValue));
           applyConsumableChipStyle(displayLabel, static_cast<double>(ratio), threshold);
    }
    return;
}


//机器位置复位
void MainWindow::backOriginTestFinished()
{
    ui->toolButton_quality_start->setEnabled(true);
    ui->toolButton_quality_reset->setEnabled(true);
    ui->toolButton_quality_cleaning->setEnabled(true);
    cglobal::g_PauseTesting = false;
    if (QToolButton *pPauseBtn = ui->toolButton_quality_sample) {
        pPauseBtn->setText(tr("暂停"));
        pPauseBtn->setIcon(m_pauseIcon);
        pPauseBtn->setStyleSheet(minitTopBtnQss);
    }

    QPoint locAxis(0,0);
    auto *pconfAxis = SingletonAxis::GetInstance();
    if(!pconfAxis)
    {
        QLOG_WARN()<<"调试界面复位坐标系为NULL";
    }
    pconfAxis->originPos(READ_OPERRAT,locAxis);

    Q_EMIT CeratActionDate(EQUIPMENT_BTN_REAET,QUIUtils::backZAxisandmoveOrigin(locAxis));
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
    auto *ptask = Testing::getTaskDll();
    QByteArray openarry = ptask->_buzzerControl(true);
    QByteArray closearry = ptask->_buzzerControl(false);

    // 封装蜂鸣动作
    auto beepAction = [=](const QString& log, int duration, int times, int gapMs) {
        int* count = new int(0);
        auto timer = new QTimer;
        timer->setSingleShot(true);
        QObject::connect(timer, &QTimer::timeout, [=]() mutable {
               if (*count >= times) {
                   emit ReminderTextOut(PROMPTLOG, log);
                   emit ConfigUsedBuzzerMark(false);
                   delete count;
                   timer->deleteLater();
                   return;
               }
            emit sendOneDirectives(openarry, "打开蜂鸣");
            QTimer::singleShot(duration, [=]() {
                emit sendOneDirectives(closearry, "关闭蜂鸣");
                (*count)++;
                timer->start(gapMs);
            });
        });
        timer->start(0);
    };

    if (finished) {
        beepAction(tr("测高成功！"), 80, 1, 120);
    } else {
        beepAction(tr("测高失败！"), 120, 3, 180);
        emit ReminderTextOut(PROMPTLOG, tr("测高失败!"));
    }
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
        CustomMessageBox::warning(this,"开始失败","样本任务为空,请添加测试样本!");
        //ThreadSafeReminder("开始失败","样本任务为空,请添加测试样本!");
    }
    else
        bgoto_testing = true;
}
void MainWindow::testingchnEnough(bool &benoughChn){
    quint8 freeChn = 0;
    benoughChn = StructInstance::getInstance()->hadFreeTestChn(freeChn);//找空闲通道
}

void MainWindow::begingTesting()
{
    if (cglobal::g_StartTesting.load()) {
        CustomMessageBox::information(this, tr("开始无效"), tr("测试已在进行中，请勿重复开始。"));
        return;
    }
    // P0 开测门禁
    if (!cglobal::gserialConnecStatus.load()) {
        CustomMessageBox::warning(this, tr("开始失败"), tr("仪器离线，请先连接串口!"));
        return;
    }

    int sampleTotal = 0;
    StructInstance::getInstance()->SampleTotal(sampleTotal);
    if (sampleTotal <= 0) {
        CustomMessageBox::warning(this, tr("开始失败"), tr("无待测样本，请先添加任务!"));
        return;
    }

    if (!cglobal::g_controldimmingfinished.load()) {
        bool dimmingOk = false;
        if (auto *dim = FullyAutomatedPlatelets::pinstancedimming()) {
            dim->getdimming_status(dimmingOk);
        }
        if (!dimmingOk) {
            CustomMessageBox::warning(this, tr("开始失败"), tr("调光未完成，请等待调光结束后再开始!"));
            return;
        }
    }

    // 清洗液余量门禁
    quint16 cleanRatio = 0;
    ConsumablesOper::getCleanLinqueAllowance(cleanRatio);
    quint8 cleanLimit = 0;
    ConsumablesOper::updateReagentLimit(READ_OPERRAT, INDEX_CLEANLINQUE_CONSUMABLE, cleanLimit);
    if (cleanRatio < cleanLimit) {
        CustomMessageBox::warning(this, tr("开始失败"),
            tr("清洗液余量不足(当前%1%，限位%2%)，请更换后再开始!").arg(cleanRatio).arg(cleanLimit));
        return;
    }

    bool benoughChn_ = false; //用通道空闲
    testingchnEnough(benoughChn_);
    if(!benoughChn_){
        CustomMessageBox::warning(this,"开始失败","模组无可用通道!");
        return;
    }
    mptesting.data()->TotalTaskProgress(); //总任务进度试管杯
    cglobal::g_PauseTesting = false;
    if (QToolButton *pPauseBtn = ui->toolButton_quality_sample) {
        pPauseBtn->setText(tr("暂停"));
        pPauseBtn->setIcon(m_pauseIcon);
        pPauseBtn->setStyleSheet(minitTopBtnQss);
    }
    if (USB_InitConnect *act = FullyAutomatedPlatelets::pinstanceSingleactive()) {
        QMetaObject::invokeMethod(act, "abortPendingActions", Qt::BlockingQueuedConnection);
    }
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
        //ThreadSafeReminder(tr("无效暂停操作"),tr("未开始测试暂停无效"));
        CustomMessageBox::warning(this,tr("无效暂停操作"),tr("未开始测试暂停无效"));
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
    ConsumablesOper::updateReplaceLocRatio(READ_OPERRAT, indexReagent, allowanceRatio);
    return (allowanceRatio >= MAX_RATIO_SWITCT)? true: false;
}



void MainWindow::testingSuppileLoss(const quint8 index){
   const bool alreadyWarned = (m_warmIndexReagent == static_cast<int>(index))
           && cglobal::g_PauseTesting.load();
   QString warmmReagent =  GlobalData::mapReagentNames(index);
   m_warmIndexReagent = index;
   if (!alreadyWarned) {
       QString outtext = "耗材:"+ warmmReagent +"余量报警!请更换"+ warmmReagent + "继续测试";
       warn_interface::showTransientWarning(tr("暂停测试"),outtext);
   }

   // 集中UI状态修改
   updateContinueButtonState();
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
    Q_UNUSED(closeClean);
    uint perfusion_leng = 0;

    //开机灌注清洗双针。弃杯不在这里全通道抓，勾选开机智能弃杯后改到调光时按通道丢。
    int cleanntime = INI_File().getWashesTime();
    QByteArrayList perfusion = QUIUtils::perfusionAction(perfusion_leng,enoughReagent,cleanntime);

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

quint32 MainWindow::getChannelPositiveFilterSeq(const quint8& indexChannel)
{
    if (mshowModuledata) {
        return mshowModuledata.data()->GetPositiveFilterSeq(indexChannel);
    }
    return 0;
}

bool MainWindow::getStablePositiveChannelVal(const quint8& indexChannel, quint32 minSeq,
                                             int& value, quint32& currentSeq)
{
    value = 0;
    currentSeq = 0;
    if (mshowModuledata) {
        return mshowModuledata.data()->GetStablePositiveChannelValue(
                    indexChannel, minSeq, value, currentSeq);
    }
    return false;
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
            recvModuleTemperature(moduleId, 0.00); // 初始化温度
        }
    }

    DisplaysConsumablesRemaining(); // 更新耗材显示

    return;
}


void MainWindow::DestructionSerialclass()
{
    QLOG_DEBUG()<<"析构串口类";

    if(mtestmoduleprotocol){ delete mtestmoduleprotocol; mtestmoduleprotocol = nullptr;}

    //if(m_ThreadReminderTsetTube){ delete m_ThreadReminderTsetTube; m_ThreadReminderTsetTube = nullptr;}

}



/* 定时检测机器主板信息获取刷卡耗材 MAINBOARD_CONSUMABLES*/
void MainWindow::initBoardDataAcquisition()
{
    if (m_boardData) {
        QLOG_WARN() << "Board data acquisition already initialized";
        return;
    }

    m_boardData = new ObtainMainBoardData();
    m_boardData->moveToThread(&m_boardThread);

    // 线程启动时在工作线程中调用 start()
    connect(&m_boardThread, &QThread::started, m_boardData, &ObtainMainBoardData::start);
    // 线程结束时安全销毁工作对象
    connect(&m_boardThread, &QThread::finished, m_boardData, &QObject::deleteLater);

    // 接收数据信号（在主线程中处理）
    connect(m_boardData, &ObtainMainBoardData::boardDataReady,
            this, &MainWindow::handleBoardData);

    // 可选错误处理
    //connect(m_boardData, &ObtainMainBoardData::errorOccurred,
            //this, &MainWindow::onBoardError);

    m_boardThread.start();
}


void MainWindow::handleBoardData(const QByteArray &data)
{
    // 此函数在主线程中运行，可以安全地更新 UI、发送信号等
    if (!data.isEmpty()) {
        emit sendOneDirectives(data, "同步主板信息");
    }
}

void MainWindow::pauseBoardAcquisition()
{
    if (m_boardData) {
        // 跨线程安全调用 pause 槽
        QMetaObject::invokeMethod(m_boardData, "pause", Qt::QueuedConnection);
    }
}

void MainWindow::resumeBoardAcquisition()
{
    if (m_boardData) {
        QMetaObject::invokeMethod(m_boardData, "resume", Qt::QueuedConnection);
    }
}

void MainWindow::pauseMainboardPollForRecharge()
{
    ++m_rechargePollGeneration;
    if (m_rechargePollPaused)
        return;
    m_rechargePollPaused = true;
    pauseBoardAcquisition();
    QLOG_INFO() << "刷卡充值：暂停主板状态轮询";
}

void MainWindow::resumeMainboardPollAfterRecharge()
{
    ++m_rechargePollGeneration;
    if (!m_rechargePollPaused)
        return;
    m_rechargePollPaused = false;
    resumeBoardAcquisition();
    QLOG_INFO() << "刷卡充值：恢复主板状态轮询";
}
void MainWindow::shutdownBoardThread()
{
    if (m_boardThread.isRunning()) {
        // 停止工作对象
        if (m_boardData) {
            QMetaObject::invokeMethod(m_boardData, "stop", Qt::BlockingQueuedConnection);
        }
        // 退出线程事件循环（禁止 terminate，避免堆损坏/UAF）
        m_boardThread.quit();
        if (!m_boardThread.wait(5000)) {
            QLOG_WARN() << "Board thread still running after wait; skip terminate to avoid heap corruption";
        }
    }
}

void MainWindow::writeConsumablesExit(){

    m_forceQuit = true;
    QLOG_WARN() << "退出软件：写入耗材后关闭串口";
    emit ReminderTextOut(NORMALLOG, tr("正在保存耗材并退出"));
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    if (m_systemTray) {
        m_systemTray->hide();
    }
    hide();

    //停止USB 监听
    toggleUsbListening(false);

    pauseObtainmodulecommand("退出程序", true);

    emit controlallchnstate(false); //关闭所有通道旋转
    QByteArray buffer =  GlobalData::writeBuffer2Equipment();

    QByteArray bufferLimitBottle = GlobalData::writeBottleLimit2Equipment();
    emit sendOneDirectives(bufferLimitBottle,"关闭写入试剂整体单位限位");
    QLOG_DEBUG()<<"关闭写入试剂整体单位限位"<<bufferLimitBottle.toHex(' ').toUpper();


    emit sendOneDirectives(buffer,"写入缓存耗材数据");
    QTimer::singleShot(1000, this, [this]() {
        if (auto *writer = FullyAutomatedPlatelets::pinstanceWirteBoard()) {
            emit writer->closeSerial();
        }
        if (auto *mw = FullyAutomatedPlatelets::mainWindow()) {
            mw->deleteExitSoftware();
        }
    });
}

void MainWindow::slotbootInitCleanFinished()
{
    if(m_shutdownClean){
        if (!m_pProgress.isNull()) {
            m_pProgress->close();
            qApp->processEvents();
        }
        writeConsumablesExit();
        return;
    }

    //机器已经初始化清洗
    m_MachineAlreadyInitCleanned = true;

    FullyAutomatedPlatelets::pinstanceWirteBoard()->Start();

    //开始每秒读取主板耗材信息
    initBoardDataAcquisition(); //遍历主板线程

    creatbeginreadmodule();//开始读取模组线程数据

    mshowModuledata.data()->startModuleData();//开启接收模组数据线程

    QLOG_DEBUG()<<"清洗初始化完成[开始读取 主板命令- 模组- 接收模组数据]"<<endl;

    //开机初始化清洗完成
    FullyAutomatedPlatelets::pinstanceinstrument()->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);

    showModuleWarmupWait();
    return;
}



void MainWindow::slotsycnPaintentInfo(QString id, QString addtime, QString barcode,
                                      QString testProject, bool isEmergency)
{
    auto *paddTaskCol = FullyAutomatedPlatelets::pinstancepatientdata();
    if(paddTaskCol)
        paddTaskCol->addpatientsqltable(id, addtime, barcode, testProject, isEmergency);
}

void MainWindow::showModuleWarmupWait()
{
    if (cglobal::g_controldimmingfinished.load()) {
        if (!m_pProgress.isNull()) {
            m_pProgress->close();
        }
        return;
    }

    if (m_pProgress.isNull()) {
        m_pProgress = new customProgresscontrols(this);
    }
    m_pProgress->_settitle(tr("模组升温"));
    m_pProgress->setBusyWait(tr("模组正在升温，请稍候..."));
    if (!m_pProgress->isVisible()) {
        m_pProgress->show();
    }
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
            const bool isFinished = cglobal::g_controldimmingfinished.load();

            if (bdimming) {
                m_pProgress->_settitle(tr("等待仪器调光"));
                m_pProgress->setBusyWait(tr("开始调光"));

                // 4. 状态驱动显示逻辑
                if (isFinished) {
                    m_pProgress->close();
                } else {
                    m_pProgress->show();
                }
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
           const int safeTotal = qMax(1, static_cast<int>(total));
           const int safeStep = qBound(0, static_cast<int>(index), safeTotal);
           m_pProgress->_setprogresstotalnum(safeTotal);
           QString progressLabel = m_pProgress->windowTitle();
           if (progressLabel.isEmpty()) {
               progressLabel = tr("开机清洗进度");
           }
           if (!progressLabel.endsWith(QLatin1Char(':'))) {
               progressLabel.append(QLatin1Char(':'));
           }
           m_pProgress->_setprogressvalue(progressLabel, safeStep);
           static bool isusbListen = false;
           if(index > 10 && index < 40 && !isusbListen){
               isusbListen = true;
               toggleUsbListening(true);
           }
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

void MainWindow::progressBarconfig(int data_,int max_)
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

    ui->show_wastlique->setAlignment(Qt::AlignCenter);
    ui->showcleanliqur->setAlignment(Qt::AlignCenter);
    ui->show_wastlique->setScaledContents(false);
    ui->showcleanliqur->setScaledContents(false);

    if (!m_consumablePulseTimer) {
        m_consumablePulseTimer = new QTimer(this);
        m_consumablePulseTimer->setInterval(560);
        connect(m_consumablePulseTimer, &QTimer::timeout, this, [this]() {
            m_consumablePulseBright = !m_consumablePulseBright;
            refreshConsumableStatusIcons();
        });
    }
}

void MainWindow::refreshConsumableStatusIcons()
{
    const qreal dpr = devicePixelRatioF();
    auto showGlyph = [&](QLabel *label, bool on, int kind) {
        if (!label || !on)
            return;
        label->setPixmap(instrumentAlarmPrompt::consumableGlyph(
            kind, 32, dpr, m_consumablePulseBright));
        label->show();
    };
    showGlyph(ui->show_wastlique, m_warmwasteliquor, equipmentTipInfo::LinqueScrapFull);
    showGlyph(ui->showcleanliqur, m_cleanliquor, equipmentTipInfo::LinqueCleanShortage);
}

//优化
void MainWindow::handleErrorNotification(quint8 index_, const QString& errinfo)
{
    Q_UNUSED(errinfo);
    auto it = m_errorInfoMap.find(index_);
    if(it != m_errorInfoMap.end()) {
        auto& info = it.value();

        if(!info.targetLabel) {
            qCritical() << "Invalid label pointer for error code:" << index_;
            return;
        }

        const bool wasOn = info.stateFlag && *info.stateFlag;
        if (info.stateFlag)
            *info.stateFlag = true;
        if (!wasOn)
            m_consumablePulseBright = true;

        info.targetLabel->setToolTip(info.tooltip);
        refreshConsumableStatusIcons();
        if (m_consumablePulseTimer && !m_consumablePulseTimer->isActive())
            m_consumablePulseTimer->start();
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
    auto finishByDisable = [widget, failedChn, safeThis]() {
        if (!safeThis) {
            return;
        }
        if (!cglobal::g_controldimmingfinished.load()) {
            safeThis->commingfaileddisablechn(failedChn);
            cglobal::g_controldimmingfinished = true;
            safeThis->dimmingprogress(false);
            QLOG_DEBUG() << "调光失败禁用通道并结束:" << failedChn;
            emit safeThis->ReminderTextOut(PROMPTLOG,
                MainWindow::tr("模组调光完成(已禁用失败通道)"));
        }
        widget->close();
    };

    // 取消=取出杯子后重检空通道 AD（不改 LED）
    QObject::connect(widget.data(), &ControltheModuletemp::re_dimming, [safeThis]() {
        if (safeThis) {
            FullyAutomatedPlatelets::pinstancedimming()->theEndDimmingActive();
        }
    });

    // 仅清理标题集合，结束逻辑由 disableusechn / close 兜底
    QObject::connect(widget.data(), &ControltheModuletemp::deletedimming,
        [safeThis](const QString& title) {
            if (safeThis) {
                safeThis->m_reminderTitleStr.remove(title);
            }
    });

    // 确定 / 关闭：禁用失败通道并结束
    QObject::connect(widget.data(), &ControltheModuletemp::disableusechn, finishByDisable);
}

void MainWindow::recvRequestSmartCupClear(const QList<quint8>& failedChn1Based)
{
    controldimming *dimming = FullyAutomatedPlatelets::pinstancedimming();
    if (cglobal::g_StartTesting.load()) {
        QLOG_WARN() << "测试进行中，不执行调光丢杯";
        if (dimming)
            dimming->cupClearNotSent();
        return;
    }
    QByteArrayList actions;
    QUIUtils::appendThrowCupsForChannels(failedChn1Based, actions);
    if (actions.isEmpty()) {
        QLOG_WARN() << "调光丢杯没有可执行动作";
        if (dimming)
            dimming->cupClearNotSent();
        return;
    }
    emit ReminderTextOut(PROMPTLOG, tr("正在自动清理遗留测试杯，请勿操作抓手…"));
    emit CeratActionDate(EQUIPMENT_SMART_CUP_CLEAR, actions);
}

void MainWindow::slotSmartCupClearFinished()
{
    emit ReminderTextOut(PROMPTLOG, tr("自动清理测试杯完成，重新检测空通道AD…"));
    FullyAutomatedPlatelets::pinstancedimming()->theEndDimmingActive();
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
    const bool smartDiscard = INI_File().rConfigPara(INITGRABCUPS).toBool();
    const QString dimmingTip = smartDiscard
            ? tr("通道 %1 空杯AD未对齐3300。\n已勾选开机智能弃杯：有读数的通道会先自动丢杯，读数为 0 不丢。\n取消：重新检测    确定 / 关闭：禁用这些通道")
            : tr("通道 %1 空杯AD未对齐3300。\n未勾选开机智能弃杯，这次没有自动丢杯。\n取消：重新检测    确定 / 关闭：禁用这些通道");
    popWidget->settipstext(dimmingTip.arg(failedList), dimmingTitle);
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

void MainWindow::commingfaileddisablechn(QList<quint8> dimmingFiledChnList)
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
        QLOG_WARN() << "[抓手异常] 样本" << sampleid
                    << "| 弹窗已经在，这次不再重复弹出 | " << outstr;
        return;
    }
    QLOG_WARN() << "[抓手异常] 样本" << sampleid
                << "| 弹出人工确认 | 焦点步骤" << indexActive
                << "| 原因:" << outstr;

    QString titlename = tr("抓杯异常");
    QString dispalyWarmming = tr("样本%1 %2。请擦拭抓手吸盘后再点「再次测试」，或「放弃样本」。")
            .arg(sampleid).arg(outstr);

    QPointer<ConnectToInstrument> phansErrWidget = new ConnectToInstrument(titlename, dispalyWarmming);
    m_suppressGripperPrompt = true;

    phansErrWidget->setWindowFlags(Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    phansErrWidget->setAttribute(Qt::WA_DeleteOnClose);   // 关闭时自动销毁
    phansErrWidget->setAttribute(Qt::WA_ShowWithoutActivating); // 不抢占焦点
    phansErrWidget->setWindowModality(Qt::NonModal);     // 非模态窗口

    phansErrWidget->notifybuttonDisplayName(tr("放弃样本"),tr("再次测试"));

    connect(phansErrWidget, &ConnectToInstrument::giveupTestSample, this,
        [this, phansErrWidget,indexActive]() {
        QLOG_WARN() << "[抓手异常] 人工选择:放弃样本 焦点步骤" << indexActive;
        m_suppressGripperPrompt = false;
        emit gripErrGiveupSample(true, indexActive);
        if (phansErrWidget) {
            delete phansErrWidget;
        }
    });


    connect(phansErrWidget, &ConnectToInstrument::tryAgainTestSample, this,
            [this,phansErrWidget, indexActive]() {
        QLOG_INFO() << "[抓手异常] 人工选择:再次测试 焦点步骤" << indexActive;
        m_suppressGripperPrompt = false;
        emit gripErrGiveupSample(false,indexActive);
        if (phansErrWidget){
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
    emit controlmotorrunning(channelNum,isRevolve);
}

void MainWindow::handleoutErrInfo(const QString titles,const QString errStr){
    //ThreadSafeReminder(titles,errStr);
    CustomMessageBox::warning(this,titles, errStr);
}








/**   更新主界面状态栏模组温度
 * @brief MainWindow::recvModuleTemperature
 * @param IndexMode
 * @param tempValue
 */
void MainWindow::recvModuleTemperature(const quint8 IndexMode, const double tempValue)
{

    // 参数验证
    if (IndexMode < MODULE_1 || IndexMode > MODULE_3) {
        QLOG_WARN() << "无效的模块索引:" << IndexMode;
        return;
    }

    if (tempValue < -50.0 || tempValue > 150.0) {
        QLOG_WARN() << "温度值异常:" << tempValue << "，模块:" << IndexMode;
        return;
    }

    // 格式化温度值（未控温时显示占位）
    const QString temperatureText = (tempValue < 1.0)
            ? QStringLiteral("--")
            : QString::number(tempValue, 'f', 1) + QChar(0x2103);

    // 根据模块索引更新对应的UI标签
    updateTemperatureDisplay(IndexMode, temperatureText, tempValue);


}


void MainWindow::updateTemperatureDisplay(quint8 moduleIndex, const QString& displayText, double tempValue)
{
    // 使用映射表代替switch，更易维护
    static const QMap<quint8, QLabel*> moduleLabelMap = {
        {MODULE_1, ui->label_showModule1_value},
        {MODULE_2, ui->label_showModule2_value_2},
        {MODULE_3, ui->label_showModule3_value_2}
    };

    QLabel* targetLabel = moduleLabelMap.value(moduleIndex, nullptr);
    if (!targetLabel) {
        QLOG_ERROR() << "无效的模块索引:" << moduleIndex;
        return;
    }
    targetLabel->setText(displayText);
    setTemperatureColorSafe(targetLabel, tempValue);
}






void MainWindow::setTemperatureColorSafe(QLabel* label, double temperature)
{
    const QString color = getTemperatureColor(temperature);
    const bool abnormal = (temperature >= 1.0) && (temperature < 36.5 || temperature > 37.5);
    const QString newStyle = QStringLiteral(
                "QLabel{color:%1;font-weight:%2;border:none;background:transparent;"
                "font-family:'Microsoft YaHei';font-size:%3px;}")
            .arg(color)
            .arg(abnormal ? QStringLiteral("bold") : QStringLiteral("normal"))
            .arg(abnormal ? 17 : 15);
    if (m_labelStyleCache.value(label) != newStyle) {
        label->setStyleSheet(newStyle);
        m_labelStyleCache[label] = newStyle;
    }
}

QString MainWindow::getTemperatureColor(double temperature)
{
    if (temperature < 1.0)  return QStringLiteral("#8AA0B0");
    if (temperature < 35.0) return QStringLiteral("#2F6FED");
    if (temperature < 36.5) return QStringLiteral("#4A8CFF");
    if (temperature <= 37.5) return QStringLiteral("#1B8A4A");
    if (temperature <= 38.0) return QStringLiteral("#D48A00");
    if (temperature <= 40.0) return QStringLiteral("#E85D04");
    return QStringLiteral("#C62828");
}
int MainWindow::getFontSizeForTemperature(double temperature)
{
    // 异常温度使用更大字号提醒
    if (temperature < 35.0 || temperature > 38.0) {
        return 18; // 异常温度，更大字号
    }
    return 16; // 正常温度，标准大字
}






/**  触发测高信号
 * @brief MainWindow::recvTriggerAltimetrySignal
 */
void MainWindow::recvTriggerAltimetrySignal()
{
    QLOG_DEBUG() << "接收到主板高度触发信号";

    // 检查工作模式
   if (!isWholeBloodMode()) {
       handleNonWholeBloodMode();
       return;
   }

   // 检查摄像头设备
   if (!checkCameraAvailability()) {
       return;
   }

   // 检查测高模块状态
   if (!checkAltimeterStatus()) {
       return;
   }

   // 触发测高测试
   triggerHeightMeasurement();

}


bool MainWindow::isWholeBloodMode()
{
    bool isWholeBlood = INI_File().GetWholeBloodModel();
    if (!isWholeBlood) {
        QLOG_INFO() << "当前为血浆模式，不支持测高功能";
    }
    return isWholeBlood;
}


void MainWindow::handleNonWholeBloodMode()
{
    QString message = tr("血浆模式不支持测高");
    QString title = tr("模式限制");

    emit ReminderTextOut(PROMPTLOG, message);
    CustomMessageBox::warningNonModal(this, title, message);
    QLOG_WARN() << message;
}


bool MainWindow::checkCameraAvailability()
{
    if (m_altimeter && m_altimeter->hasOpenedCamera()) {
        return true;
    }

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    if (cameras.isEmpty()) {
        QString errorMsg = tr("未检测到可用的摄像设备");
        QString title = tr("硬件错误");

        CustomMessageBox::warning(this,title, errorMsg);
        QLOG_ERROR() << errorMsg;
        return false;
    }

    QLOG_DEBUG() << "检测到" << cameras.size() << "个摄像设备";
    return true;
}


bool MainWindow::checkAltimeterStatus()
{
    if (!m_altimeter) {
        if (Height_Data *hd = FullyAutomatedPlatelets::pinstanceAddsampletest()) {
            bindAltimeterVision(hd->vision());
        }
    }
    if (!m_altimeter) {
        QString errorMsg = tr("测高模块未初始化或初始化失败");
        QString title = tr("模块错误");
        CustomMessageBox::warning(this,title, errorMsg);
        QLOG_ERROR() << errorMsg;
        return false;
    }
    return true;
}

bool MainWindow::initializeAltimeter()
{
    if (Height_Data *hd = FullyAutomatedPlatelets::pinstanceAddsampletest()) {
        bindAltimeterVision(hd->vision());
    }
    if (m_altimeter) {
        QLOG_INFO() << "测高模块初始化成功";
        return true;
    }
    QLOG_ERROR() << "测高模块初始化失败";
    return false;
}


void MainWindow::triggerHeightMeasurement()
{
    try {
        if (mptesting) {
            mptesting.data()->ensureAddSampleWindowVisible();
        }
        if (!m_altimeterDelayTimer) {
            m_altimeterDelayTimer = new QTimer(this);
            m_altimeterDelayTimer->setSingleShot(true);
            connect(m_altimeterDelayTimer, &QTimer::timeout, this, [this]() {
                Q_EMIT triggerTestHeight();
                QLOG_INFO() << "[测高拍照] 延时结束，开始取图";
            });
        }
        // 重复触发则重新计时，等试管插稳再拍。
        m_altimeterDelayTimer->start(500);
        QLOG_INFO() << "[测高拍照] 收到触发，500ms 后再取图";
    } catch (const std::exception& e) {
        QLOG_ERROR() << "触发测高过程中发生异常:" << e.what();
        CustomMessageBox::warning(this,tr("系统错误"), tr("测高触发失败"));
    }
}



/**  开始测试按钮
* @brief MainWindow::onStartTestClicked
*/
void MainWindow::onStartTestClicked()
{
    emit ReminderTextOut(PROMPTLOG,"点击开始测试按键");

    if(!cglobal::gserialConnecStatus){
        //ThreadSafeReminder("开始失败","仪器未连接!");
        CustomMessageBox::warning(this,"开始失败","仪器未连接!");
        return;
    }

    // 检查测试任务
    bool hasTestTask = false;
    equipment_will_test_num(hasTestTask);
    if(!hasTestTask){
        return;
    }

    // 开始测试
    begingTesting();
}


//测试摄像头扫码
void MainWindow::initScanBarcode(){
    m_readerCode = std::make_unique<QRCodeReader>(nullptr); // 不设置父对象
    connect(m_readerCode.get(), &QRCodeReader::decodingFinished,
        this, &MainWindow::onDecodingFinished);
	m_readerCode->readFromFile("C:/barcode.jpg");
}

void MainWindow::onDecodingFinished(bool success, const QString &result)
{
	if (success)
		CustomMessageBox::information(this,"识别成功", result);
    else
        CustomMessageBox::warning(this, "识别失败", result);
}
