#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//#include <Windows.h>
//#include <windowsx.h>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QCloseEvent>
#include <QtWidgets>
#include <QLabel>
#include <QMetaType>
#include "qualitycontrol.h" //质控
#include "calibrate.h"
#include "graphplot.h"
#include "result.h"
#include "testing.h"
#include "aboutmachine.h"
#include "machinesetting.h"
#include "alarm.h"
#include "customcreatsql.h"
//#include "test_module_class.h"
#include "monitor_traytest.h"       //监控测试试管盘
#include "testheight_module.h"      //测高模组
#include <QCamera>
#include <QVBoxLayout>
#include <QWheelEvent>
#include "cuestomtabstyle.h"
#include <QTabBar>
#include <QPrinter>
#include <QKeyEvent>
#include <QPrintDialog>
#include <QtPrintSupport/QPrintDialog>
#include <FunctionWidget/functioncustomwidget.h>

#include "sampledataprocess.h"      //样本数据处理
#include "suoweiserialport.h"
#include "displaychanneldata.h"
#include "machinereadrunnable.h"	//线程池
#include "customplot.h"             //位置校准
#include "CustomHighData.h"         //提示充值成功
#include "QsLog/include/QsLog.h"
#include "displayLogsText.h"
//#include "instrumentserialportmessage.h"  //串口消息
#include "instrumentalarmprompt.h"       /*仪器报警提示*/
#include "passparameterinterface.h"
#include  "pmessagebox.h"    //试管盘脱机插入
#include  "opencvfindheigh.h"
#include "USBListener.h"

#include  "commandexceptional.h"
#include  "thetestmoduleprotocol.h"
#include  "threadremindertsettube.h"
#include  "ui_handoff_staff.h"
#include  <QScopedPointer>
#include  "usb_initconnect.h"   //单个步骤执行
#include  "testprojectprocess.h"
#include  "controlthemoduletemp.h" //控温
#include  "warn_interface.h"
#include  <dilag/handoff_staff.h>
#include  "plotwidget.h" //主页
#include  <modulethread/moduletimerThread.h>
#include  <modulethread/obtainmainboarddata.h>
#include  <custom_style/customprogresscontrols.h>



// 在头文件中定义错误信息结构体
struct ErrorDisplayInfo {
    QString normalPath;
    QString alertPath;
    QString tooltip;
    QLabel* targetLabel;
    bool* stateFlag;
    QSize iconSize;

    ErrorDisplayInfo(QString normal, QString alert, QString tip,
                    QLabel* label, bool* flag, QSize size = {48,32}) :
        normalPath(std::move(normal)),
        alertPath(std::move(alert)),
        tooltip(std::move(tip)),
        targetLabel(label),
        stateFlag(flag),
        iconSize(size)
    {}
};

enum class MainInterfaceSubscripted
{
    FunctionBlock_Testing,
    FunctionBlock_PreviewResults,
    FunctionBlock_PreviewCurve,
    FunctionBlock_Inquire,
    FunctionBlock_Consumables,
    FunctionBlock_Setup,
    FunctionBlock_About
};

namespace Ui {
class MainWindow;
}

class moduletimerThread;
class mainboardThread;

class MainWindow : public QMainWindow, public PassparameterInterface
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void    init_style_all();

    void    ChannelValueshow(QStringList moduleData) override;

    void    displayPara(int Indexmodul, const double tempvalve) override;

    void    PromptInfo(const quint8 Index, const QString ReminderStr, const quint8 iActive) override;

    void    deleteExitSoftware(); //退出软件



    /**机器复位 所有样本测试完成
    * @brief Machine_reset
    * @param Location  true=位置模式
    */
    void    _backOriginTestFinished();
    void    ThreadSafeReminder(QString title_, QString outputText);
    //void    ReminderInfo(QString title_, QString outputText); //提示信息

    void    _reminderFunctionWidget(QString title_,QString outputtext_, QList<QString> btntext);//功能提示框
    void    _progressBarconfig(int data_, int max_);

    /**   控制模组和主板 暂停、读取状态
     * @brief _pauseObtainmodulecommand
     * @param info
     * @param connectedCotl  true = 暂停 false = 读取
     */
    void   pauseObtainmodulecommand(QString info, bool connectedCotl); //暂停读模组数据
    void   _displayHandoffUser(); //显示切换用户界面
    void   machineReposition();//复位
    void   _dimmingprogress(bool isDimmingInProgress); //调光中
    int    getChannelTestVal(const quint8& indexChannel);


//优化
private:

    // 线程管理统一处理
    void stopAndWaitThreads();

    // 单例清理
    void cleanupSingletons();

    // USB监听停止
    void stopUsbMonitoring();


    void initCameras();

    void initErrorDisplayMap();

    void centerWindow(QWidget* widget);

    void initTestPprHeight();

    // 封装信号连接逻辑
    void setupDimmingConnections(QSharedPointer<ControltheModuletemp> widget,
                                    const QList<quint8>& failedChn,
                                    QPointer<MainWindow> safeThis);


private:

    void    openSettingsDialog(); //打开设置界面
    void    FirstConnectCleanEquipment();//首次联机清洗
    bool    checkCleanFluidSufficiency();// 辅助函数：检查清洗液余量
    void    showAxisSyncErrorDialog();// 辅助函数：显示坐标同步错误
    void    initProgressBar(const QString &title, bool isModal,bool ishow);
    void    _begingTesting(); //开始测试
    void    _creatbeginreadmodule();//构建并开始读取模组数据
    void    listentoUsb();
    void    initchnstate();

     /*根据仪器类型配置界面*/
    void    updateModuleVisibilityBasedOnInstrumentType();

    void    _serialConnection();

    void   _init_read_moduledata_thread(); //初始化实例获取到模组数据线程

    void   _inittestmodule_data_thread();  //收到模组数据处理线程

    void   _initmainboradthread();  //主板线程实例化函数

    void   initTestTaskThread(); //初始化启动测试任务线程

    void    DestructionSerialclass();

    void    Traverse_through_motherboard_information(); //遍历主板

    void    Traverse_through_ModuleReadInfo(); //遍历模组数据

    //仪器初始化动作 true=试剂够.false=试剂不足只灌注
    void    equipmentinitActive(const bool enoughReagent, bool closeClean);

    void    equipment_will_test_num(bool &bgoto_testing); //点击测试待测项目样本数

    void    _testingchnEnough(bool &benoughChn); //有至少一个可用通道

    void    _commingfaileddisablechn(QList<quint8> dimmingFiledChnList);

    void    InitMainUiLayout(); //主界面布局

    void    initcreatprogressingDimming(bool bdimming); //构建进度条


    bool    requiresTemperatureControl(quint8 equipmentType); //开始测试时是否需要控温

    void    ReminderPauseConsumables_alarm(const int Index);  /*耗材报警提示*/

    void    CreatReminderWidget(char index,QString titleStr,QString reminderStr,quint8 indexFucn); //创建提示框 mesg

    bool    getbAxisSycnFinished(); //获取仪器坐标是否同步完成

    void    CreatActionExecution(); //开始测试的信号槽

protected:
    void    closeEvent(QCloseEvent *event);
    void    resizeEvent(QResizeEvent *event);
    bool    eventFilter(QObject *obj, QEvent *ev);
    void    paintEvent(QPaintEvent *event);
    bool    nativeEvent(const QByteArray &eventType,void *message,long *result);

public slots:

    /*耗材使用完 暂停*/
    void testingSuppileLoss(const quint8 index);
	//耗材暂停更新按钮状态
	void updateContinueButtonState();

    void experimentReadChannel(const quint8& peChannel, const int &index, bool isstart);

    //废液、外部清洗液弹出提示后在信息栏提醒
    void   handleErrorNotification(quint8 index_, const QString& errinfo);

    //采集通道数据对比失败提示
    void   recvshowDimmingFailedChn(const QList<quint8> &failedChn);

	void   recvprepareReconnectGetData();
    
    //测试过程中抓手吸/吐失败
    void   handletheGripperFailed(const int sampleid,
                                  const QString outstr,
                                  const quint8 indexActive);

    //PE测试打开关闭通道控制
    void handleControlChannelRevolve(const quint8&channelNum,const bool& isRevolve);
    void handleoutErrInfo(const QString titles, const QString errStr);

    //设置对话框图标
    void  configReminderIcon(quint8 );

    //添加样本同步患者界面
    void   _slotsycnPaintentInfo(QString id_, QString addtime, QString barcode_, QString testProject);

     //定时遍历模组
    void   _ObtainModuleData();

    //定时遍历主板
    void   timeoutObtainMainboadData();

    //串口连接状态提示
    void    slotconnectionStateChanged(bool connected);

    void    DisplaySycnMainUiLosserSuppile(int IndexReagent, double Remainingratio);

    //开始初始清洗液不足,更换清洗液触发信号
    void    recvNoinitialCleaning();

    //析构命令异常提示框 mesg
    void    closeReminder();

     /*串口提示消息显示log*/
    void    slotExecute_exception_prompt(const quint8 Index,const QString,const quint8 IndexError);

    //刷卡提示
    void    handlecardSwipeSuccessful(const QString tips,quint8 indexReagent,quint8 totalnum,quint16 datetime);

    void    handleswipeCardSuccessfullyWritten(QString tips, int addindexReag, quint8 addBottle);

     /*触发测高开关*/
    void    recv_MainBoardHeighTigger() ;

    void    Recv_Module_temperature(const quint8 IndexMode, const double tempValue);//模组温度

    void    DisplaysConsumablesRemaining();  /*更新主界面显示耗材余量*/

    void    slotbootInitCleanFinished(); //开机初始化完成

    void    slotCleaningProgress(quint8, quint8 total); //开机初始化进度

    void    SendCode_2_Serial(QByteArrayList arrycode); //调试模式发送到串口

    /*测高完成蜂鸣提示*/
    void    TestHeightFinish(const bool finished);

    void onDeviceConnected(usbDevice dev);
    void onDeviceDisconnected(usbDevice dev);
    void onListenerStarted();
    void onListenerStopped();
    void onListenerError(QString message);
    void onusbPowerContl(QString message, const quint8 istate);

    //void    on_toolButtonr_esult_tool_print_clicked();

signals:
    //质控获取通道值
    void	qualityChannelVal(const quint8, int);

    void    ReminderTextOut(quint8 ReminderKind,const QString dataStr); //提示文字界面信号

    void    AlarmReminderSound(bool bsound); //警报声

    void    delInitfile();

    void    requestReconnect();  //重新连接
	void    disConnectCloseSerial();//断线关闭串口

    void    stopObatinMachineInfo(); //停止读取主板模组信息

    void    aNewconnectMachine();//模组 主板从新开始

    void    _controlmotorrunning(quint8 indexchn,const bool bopen); //控制通道搅拌电机

    void    controlallchnstate(bool bopen); //控制所有通道开关状态

    void    CeratActionDate(int ACtionType, const QByteArrayList GroupAction);

    //加样动作未完成--继续执行
    void    ProceedTask();

    void    resumeaTestTask();

    //抓手异常处理是否放弃样本
    void    gripErrGiveupSample(const bool ,const quint8);
    void    pendingtimeoutSampleTest(const quint8);

    void    startTheTestTask();

    //关机写所有耗材
    void    closeEquipmentconsumables();

    //设置界面耗材参数写入主板
    void    _sendcodeList(const QByteArrayList, QString);

    void    ExcptionSampleNum(const int);

    void    Thread_5_SendCode(QByteArrayList);  //线程池5 发送机器的配置

    void    Delayed_sending(const QByteArrayList , int delayedmS, bool needDelayed);

    void    WhileReadChannelParam(const int Fromchannel);


    //刷卡充值成功
    void    rechargesuccessful(int index, quint8 num);

    //断线清空模组缓存数据
    void    cleanModuleBuffData();

    /***测试任务处理***/
    void    syncModuleChannelData(const QStringList); /*解析到模组通道数据*/

    void    ConfigUsedBuzzerMark(const bool UsedBuzzer); /** 蜂鸣器设为启用*/

    void    SynchronizeEmptyTubeUsed(quint8); /*质控使用试管同步耗材界面*/

    void    OpenInstrumentCamera(const quint8);

    //触发到测高开始解析图片
    void    triggerTestHeight();

    //所有样本测试完成曲线界面复原
    void    allCurveClear();

    //模组控制
    void    sendmoduleactio(const QByteArrayList &dataList,
                            const QString &info);

    void    sendOneDirectives(const QByteArray arry, QString kindCommad);

private slots:
    void    onReminderRequested(QString, QString);
    void    on_sStatusInfo_clicked();
    void    on_toolButton_quality_reset_clicked();
    void    on_toolButton_quality_cleaning_clicked();
    void    on_toolButton_quality_sample_clicked();
    void    on_sStatusInfo_text_clicked();

private:
    QIcon m_pauseIcon;
    QIcon m_continueIcon;
	void logThreadStatus();

private:
    void _setupMainInterface();
    void setupTabWidget();
    void setupFunctionButtons(); //主界面初始显示
    void switchMainTab(MainInterfaceSubscripted index);
    void highlightActiveButton(int index);
    void setupUtilityButtons();

    void updateButtonSelectionState(int activeIndex);
    void logTabSwitchEvent(int index);
    void handleFunctionButtonClick(const int indexed); //点击跳转页面

    

    //写入耗材余量 退出关闭软件
    void writeConsumablesExit();

    //更换试剂确认
    bool reagentReplacementComplete(const int& indexReagent);
private:
    void updateConnectionUI(bool connected);
    QPixmap m_connectedIcon;
    QPixmap m_disconnectedIcon;
    bool m_iconsLoaded = false;

private:
    static constexpr int TRAY_MESSAGE_DURATION = 5000; // 托盘消息显示时长
    QSystemTrayIcon *m_systemTray = nullptr; //托盘
    QMenu*   m_trayMenu; //托盘目录
    void initTrayFunction();   //初始化托盘功能
    void createTrayActions();
    void setupTrayMenuStyle();
    void connectTraySignals();
    void checkTrayAvailability();

private:
    Ui::MainWindow *ui;

    //USB监听
    QScopedPointer<USBListener> m_USBListener;


    QPointer<QualityControl> minstrumentConsumables;//耗材管理
    QPointer<Testing> mptesting; //测试界面
    QPointer<GraphPlot> m_graphplot; //测试曲线
    QPointer<SuoweiSerialPort> mlocalSerial; //串口类线程对象
    QPointer<FunctionCustomWidget> preminder;//退出软件对话框
    QPointer<CustomHighData> mReminder; /*提示充值界面*/
    QPointer<displayChanneldata> mshowModuledata; //接收显示模组通道数据
    QPointer<ConfigureWriteParameter> WriteParameter;//命令参数写入仪器
    QPointer<CommandExceptional> m_pReminderExceptional;//提示框
    QPointer<ControltheModuletemp> m_popwidgetTips; //调光提示
    QPointer<theTestModuleProtocol> mtestmoduleprotocol; //模组数据解析
    QPointer<customProgresscontrols> m_pProgress; //进度条



    //theTestModuleProtocol *mtestmoduleprotocol = nullptr;


    //QSharedPointer <CustomPlot> m_calibrationLoca;  //位置校准

    //执行动作命令线程
    QThread mThreaddotest;
    QScopedPointer<TestProjectProcess> m_pdoingTesting;


    bool m_shutdownClean;//关机清洗标志

    QMap<quint8, QLabel*> displayRatioLab;  //耗材Label

    /*异常启动后任务处理*/
    //QSharedPointer<LegacyTaskContinue> m_ExceptionTaskHandling;

    /*多次点击打开设置界面*/
    bool m_bmachineconfigureSignal;

    bool m_MachineAlreadyInitCleanned; //机器已经初始化清洗


    ThreadReminderTsetTube *m_ThreadReminderTsetTube = nullptr;

    //Heightmeasurement * Altimetertrigger = nullptr; /*测高线程*/
    QThread mTesthighThread;
    opencvfindHeigh *mAltimetertrigger = nullptr;

    //SendMachineSportInfo *Machinesport = nullptr;

    bool  m_benterapp; //输入密码进入软件标志

    quint8  minstrumentType;  /*读取到仪器类型*/

    QVector<quint8 > m_QualitylastChannel; //测试质控通道

    QProcess *m_processPrint = nullptr; /**测试用例  打印已未用**/

    //模组读取
    QThread m_threadModule;
    moduletimerThread* m_moduletimerThread = nullptr;

    //主板
    QThread m_threadMainBoard;
    ObtainMainBoardData* m_ObtainMainBoardData = nullptr;


    bool m_warmwasteliquor = false;     //提示闪烁废液
    bool m_cleanliquor = false;         //外部清洗液闪烁

    QSet<QString> m_reminderTitleStr;
    mutable QReadWriteLock m_reminderLock;  // 读写锁保护共享数据
    void RealReminderImpl(QString title_, QString outputText); // 实际实现

    QMap<quint8, ErrorDisplayInfo> m_errorInfoMap;

    bool m_suppressGripperPrompt = false; //抓手异常弹窗


    QTimer *m_experimentTimer = nullptr;
	int m_indexSec;
    int m_relChannel;
    int m_currentIndex;  // 存储当前index

    QMutex m_mutex; // 互斥量保护状态


   

    //缓存QSS样式
    QString minitTopBtnQss;
    QString mPauseSateQss;
    int m_warmIndexReagent;


    static constexpr auto ICON_NORMAL = ":/Picture/icon_title/talk_nor.png";
    static constexpr auto ICON_ERROR = ":/Picture/icon_title/talk_err.png";
    static constexpr auto ICON_REMINDER = ":/Picture/icon_title/talk_reminder.png";

    // 预加载资源
    QIcon m_iconNormal{ICON_NORMAL};
    QIcon m_iconError{ICON_ERROR};
    QIcon m_iconReminder{ICON_REMINDER};
    QIcon* m_currentIcon = nullptr; // 内存占用8字节

};

#endif // MAINWINDOW_H
