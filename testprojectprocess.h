#ifndef TESTPROJECTPROCESS_H
#define TESTPROJECTPROCESS_H

#include <QFuture>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QThread>
#include <QtCore/QMutex>
#include "custom_struct.h"
#include "structinstance.h"

#include <atomic>
#include <array>
#include <functional>
#include <mutex>
#include <memory>
#include <QByteArray>
#include <QLoggingCategory>
#include <QHash>
#include <QMap>



struct Commandinfo{
    quint8 addr;
    quint8 func;
    quint8 numindex;
};


class TestProjectProcess : public QObject
{
    Q_OBJECT
public:
    explicit TestProjectProcess(QObject *parent = nullptr,quint8 equipmentIndex = 2);
    ~TestProjectProcess();

     void initTimer();
     void setTargetThread(QThread* thread);  // 公有方法设置线程






signals:
    void WriteaSingleCommand(const QByteArray ,QString);

    void write_instructions_group(const QByteArrayList &dataList,
                                  const QString &info); //写指令集

    void executeErrPrompt(QString);

    void pauseTesting(); //执行指令异常暂停
    void requestPauseUiSync(bool paused); // 与 g_PauseTesting 对齐主界面暂停按钮
    void abortPendingEquipmentIo(); //动作异常：丢掉未发出的串口写队列
    void sampleSuckPosInvalid(const QString& message); //吸样坐标落在原点

    void openChnMotor(const quint8, const bool);

    void getAnemiaValue();              //获取贫血初值

    void EmptyTubeCatched(quint8 );     //试管夹起变灰

    void readbloodyInitValue(quint8 indexReag,quint8 indexActive);  //读试管富血初值

    void openTestChnTest(const int ,const quint8, const quint8); //吐试剂完成打开测试通道测试

    void setprogressinitboolwaittext(bool iswait,quint8 index_chn, const QString &status);

    void sycn_SampleTestingChangInitColor(QPoint,quint8);
    void bloodSampleHintStage(quint8 anemiaHole, quint8 richHole, quint8 stage);

    void throwtesttube(); //弃杯成功

    void reminderTextOut(quint8 ReminderKind,const QString dataStr); //提示文字界面信号

    void theGripperFailed(const int, const QString,const quint8 );

    void pauseSycnMainuiUpdate(const quint8 lossReagent);

    /// 工作流状态（焦点样本 + 当前动作），供主界面展示
    void workflowStatusChanged(const QString& statusText);

    void bloodSuck2EmptyTube(bool banemia, quint8 IndexTube);
    void giveupSampleHolesChanged(const QList<quint8>& holeList);
    void giveupChannelFlash(bool isChannelNormal, quint8 indexChannel);
    void allSamplesFinished();
    void stopChannelAcquisition(quint8 indexChannel);
    void abortAllOpenChannelAcquisition(); // 掉线超时：停全部开测通道采数


public slots:
    void handleStartTheTestTask();
    void handlePppBaselineInvalid();
    void handlePrpBaselineInvalid();
    void handlePppZeroGiveUp(int sampleId, quint8 channel);
    void handlePrpZeroSkipReagent(int sampleId, quint8 reagent, quint8 focusActive);

    //抓手异常处理
    void handleGripErrGiveupSample(const bool , const quint8 indexAxtive);
    void handlePendingtimeoutSampleTest(const quint8 indexAxtive);

    void slottestingaddsample();

    void recvEquipmentdata(const QStringList); //收到仪器信号

    void equipment_err_continue();      //提示了异常后继续执行

    void throwAnemiaCup(quint8 testChn);   //收到通道获取贫血值完成

    void slotSpitReagentTesting();      //获取富血值后吐试剂到测试通道

    void slotaddSampmeTestCleanPin(const int cleanPin, const quint8 index_reagent); //清洗针

    void SampleReagTestFinish(QString finishid, const quint8 index_Chn, const quint8 indexReage); //测试完成通道

    //恢复测试任务
    void handleResumeaTestTask();

    /** PPP洗针后仍报挂液：确定后按失败高度液面探测吸PPP（不全轴复位） */
    void confirmPppHangLiquidProceed();

    //断线重连
    void slot_connectEquipmentagin();
    void handleInstrumentOffline();
    void handleInstrumentCameOnline();

private:
    /** 写指令到仪器
     * @brief writeCommandHeader
     * @param data
     * @param commandText
     * @param latetime
     */
    void writeCommandHeader(const QByteArray data, const QString commandText, int latetime);



    bool gripperExceptionHandling(const quint8 commandIndex,const quint8 indexActive);

    //向仪器发送指令
    void sendInstructionsWithDelay(const QByteArrayList data_,
                                   int late_time,
                                   const QString instructioninfo);
    //缺少试剂复位暂停
    void missingReagentResetPause();

    //开始样本加样指令
    void StartSampleAdditionWorkflow();
    void resetRuntimeForSessionStart();
    void prepareForNewTestRound();
    void appendPauseQueueFromFocusRecovery();
    void logTestFsm(const QString &event, const QString &detail = QString()) const;
    void logTestFsmInfo(const QString &event, const QString &detail = QString()) const;
    void setSampleFocus(int sampleId, quint8 newFocus, const QString &reason);
    void stallWithPause(const QString &event, const QString &userTips, bool modalPrompt = true);
    bool shouldDeferSendForPause() const;

    void handleNoPendingSamples();

    //载入挂起任务
    void loadPedningtimeTask();

    void executeSampleAddition(int sampleId, StructInstance& instance) ;

    //逻辑控制吸试剂的位置
    bool controlsuckReagentLoc(quint8 primaryReagentIndex, quint8 testingReagent);

    void triggerConsumablesPause(quint8 primary, quint8 alternate);
    bool handlePrimaryOnlyStrategy(bool isPrimarySufficient, const quint8 &index);
    bool handleAlternateReagentStrategy(quint8 primary, quint8 alternate,
                                                            bool isPrimarySufficient,
                                                            bool isAlternateSufficient);

    //收到试剂针移动到的位置：记录待扣耗材索引（成功完成吸试剂流程后再扣）
    void FindSuckReagentAxis(QPoint moveingloc, const int &indexSuckReagent);

    // 表驱动：五种试剂吸试剂+抓PRP 共用实现
    void recvNextTestReagent(quint8 reagentIndex, quint8 focusActive,
                             const int commandIndex, const int slaveAddr,
                             const int Action_motor, const QStringList &recvdata);

    // 吸试剂+抓杯成功后扣减对应瓶耗材
    void deductPendingReagentConsumable();

    // 清洗液：发洗针时只记待扣，ACK 成功后再扣
    void markPendingCleanDeduct(quint8 depleteType);
    void deductPendingCleanConsumable();
    void clearPendingConsumableDeducts();

    // FOCUS 中文 + 广播当前焦点/动作到 UI
    QString focusActionText(quint8 focusActive) const;
    void publishWorkflowStatus(const QString& detail);

    // 断线重连快照
    void captureReconnectSnapshot();
    void applyReconnectSnapshotAfterReset(const QString& tips);
    void handleOfflineVoidTimeout();
    void recoverAfterOfflineVoid(const QString& tips);
    void enqueueOfflineVoidedReagent(int sampleId, quint8 chn, quint8 reagent);
    quint8 reagentFromFocus(quint8 focusActive) const;

    //更新测试界面试管颜色样本针吸吐样
    void updatetubeColorState(const QStringList data);

    //更新空试管被抓走
    QPoint updateEmptyClipAway(const QStringList data);


    //每个样本加样完成 ==>后遍历通道内有没有完成测试的通道
    void TraverseChnHadFinishTesting(int clean_indexPin);


    //找到空闲通道
    void getFreeTimeTestChn();


    bool isHandsControl(int slaveAddr, int Action_motor) const;
    bool isMainControlMotorXY(int slaveAddr, int Action_motor) const;


    /** 收吐试剂命令
     * @brief recvSpitReagent
     * @param indexActive
     * @param index_num
     * @param Dischargethetypeofreagent
     */
    void recvSpitReagent(quint8 indexActive, const int indexNum, quint8 reagentIndex);


    void recv_cleanDoublePin(const int index_num, const int slaveAddr, const int actionMotor,
                             const QStringList &recvdata);

    void recv_cleanReagentPin(const int index_num, const quint8 index_reag, const int slaveAddr,
                              const int actionMotor, const QStringList &recvdata);

    void recv_cleanBloodPin(const int index_num, const int slaveAddr, const int actionMotor,
                            const QStringList &recvdata);

    void startCleanNeedleAckMode(const QString &tips);
    void continueCleanNeedleAckMode(bool finished, const QByteArray &nextCmd,
                                    int cleanKind, const QString &tips);
    void sendFirstPrepatchCommand(const QString &tips);

    void recv_throwCup(quint8 indexActive,
                        const int commandIndex,
                        const int slaveAddr,
                        const int Action_motor,
                        const QStringList &recvdata);

    void throwCupFinish(); //丢杯完成后....

    //抓手失败再次尝试
    void testSampleTryAgain(const quint8 indexActive, const std::atomic<bool>& ispendtime);

    void theTestChannelStopTesting(const bool handError);

    //抓手异常放弃当前样本
    void giveupSamplenum(const int sampleid);

    //挂起
    void pendingTimeSample(const int sampleid);


    /** 丢测试通道内完成试剂杯首索引指令
     * @brief outputThrowTestChannelReagentFinish
     * @param finishId   完成试剂测试样本号
     * @param finishReagent 完成试剂号
     */
    void outputThrowTestChannelReagentFinish(const int finishId,const quint8 finishReagent);


    // 处理完成通道任务
    void processFinishedChannelTask();

    void setupFocusSample(int sampleId, quint8 reagentType);

    void sendControlCommands(quint8 chn, int sampleId, quint8 reagent);

    // 处理空闲通道
    void handleIdleChannelTasks();

    //提示显示到主界面
    void promptsareDisplayed(const quint8 proIndex, const QString tips, const bool outwidgetTips);

    //异常重新测试
    void exceptionRetest(QString tips);
    bool restartClipPppAfterHome(const QString& tips);
    bool restartThrowPppAfterHome(const QString& tips);
    bool restartThrowReagentAfterHome(quint8 reagent, const QString& tips);
    bool restartPrpAfterHome(quint8 reagent, const QString& tips);
    bool restartGripperFocusAfterHome(quint8 indexActive, const QString& tips);
    bool ignoreRestartLeftoverAck(int Action_motor, const QStringList &recvdata);

    void enqueueContinueReagent(int sampleId);
    bool startPendingContinueReagent();
    bool beginNextReagentForSample(int sampleId);
    void removeSampleFromSchedulerQueues(int sampleId);
    void dismissFocusSample(int sampleId);
    void releaseSampleChannel(int sampleId, bool flashAsError);
    void noteCommandsSent(int count);
    void noteAckReceived();
    void armAckWatchdog();
    void disarmAckWatchdog();
    void requestPppBaselineRead();
    void requestPrpBaselineRead();
    void requestPrpBaselineRead(quint8 reagentIndex, quint8 focusActive);
    void markReagentSkipped(int sampleId, quint8 reagent);

public:
    typedef QPair<quint8, quint8> FocusReagentPair;
    struct ReagentWorkflow {
        quint8 testingFocus;
        quint8 spitFocus;
        quint8 throwFocus;
        quint8 consumableIndex;
        ReagentWorkflow()
            : testingFocus(0), spitFocus(0), throwFocus(0), consumableIndex(255) {}
        ReagentWorkflow(quint8 testing, quint8 spit, quint8 throwFocus_, quint8 consumable)
            : testingFocus(testing), spitFocus(spit), throwFocus(throwFocus_), consumableIndex(consumable) {}
    };
private:
    static const QMap<quint8, FocusReagentPair> FOCUS_MAPPING;
    static const QHash<quint8, ReagentWorkflow> s_reagentWorkflow;
    void remberSendPara(const QByteArray& outdata, Commandinfo& remember_data);
    bool comparedSendPara(const Commandinfo &remember_data, quint8 addr, quint8 func, quint8 num);

private slots:
    void handleAckTimeout();


//接收指令
private:

    /** 动做未完成执行继续发送命令 或 异常先复位等待处理
     * @brief commandNotCompleteOrErr
     * @param isGripperError
     * @param senddata
     * @param tips
     * @param klatetimer
     * @param isBackReagentNeedle 是否延时复位试剂针
     */
    void commandNotCompleteOrErr(const bool isGripperError, const QByteArray senddata,
                                   const QString tips, const bool klatetimer
                                  ,const bool isBackReagentNeedle);

    // 仅在「开负压下扎已应答、等待同高度读负压」时置位；吐杯延时抬Z不走这里
    void armGripperReadAfterOpenVacuum(quint8 suckCheckIndex,
                                       quint8 positionIndex,
                                       bool grabAtTray,
                                       quint8 holdQueue,
                                       quint8 reagentIndex,
                                       const QString &label,
                                       const QString &failTips);

    void startGripperVacuumPoll();
    void stopGripperVacuumPollTimer();
    void onGripperVacuumPollTick();
    void applyGripperVacuumPollResult(const StructInstance::GripperVacuumPollResult &result,
                                      const QString &defaultTips);
    bool tryRecvGripperVacuumProbeAck(const QStringList &recvData, quint8 commandIndex);
    bool tryFeedActiveGripperVacuumPoll(quint32 airValue,
                                        bool isSuctionBit,
                                        const QString &workflowTips);
    bool tryStartGripperPollIfWaiting(const QByteArray &sendData);

    static const int kGripperVacuumPollIntervalMs = 400;



    /**加样动作
     * @brief receiveInstructionsSampleactive
     * @param index_num  接收编号
     * @param axismovement 运动的轴
     * @param recvdata    收到的数据
     */
    void receiveInstructionsSampleactive(int index_num, int slaveAddr, int axismovement,
                                         const QStringList recvdata);

    /** PPP位Z复位预探测及洗针/擦针后重做 */
    bool tryHandlePppPreProbeAck(int index_num, int slaveAddr, int axismovement,
                                 const QStringList &recvdata);
    void finishPppPreProbeDecision(const QStringList &recvdata);
    void advanceAddSampleAfterPppPreProbePassed(quint8 preProbeIndex);
    void beginPppPreProbeRecovery(const QByteArrayList &commands, quint8 preProbeCmdIndex);
    void sendNextPppRecoveryCommand();
    void clearPppPreProbeRecoveryState();

    void notifyHangLiquidWipeIfNeeded(int slaveAddr, int actionMotor, const QStringList &recvdata,
                                      bool showModalPrompt = false);


    /** 吸PPP试管到测试通道
     * @brief receiveInstructionsPPPCuptoChannel
     * @param indexcode  接收编号
     * @param slaveAddr  从机地址区分 xy 和 Z轴
     * @param Action_motor 运动的轴
     * @param recvdata    收到的数据
     */
    void receiveInstructionsPPPCuptoChannel(const int indexcode,
                                               const int slaveAddr,
                                               const  int Action_motor,
                                               const QStringList &recvdata);


    /** 丟通道内的PPP
     * @brief recv_throwAnemia
     * @param indexconde
     * @param slaveAddr
     * @param Action_motor
     * @param recvdata
     */
    void recv_throwAnemia(const int indexconde,int slaveAddr,int Action_motor, const QStringList recvdata);

    /** 收测试AA命令
     * @brief recvNextTestAA
     * @param commandIndex
     * @param slaveAddr
     * @param Action_motor
     * @param recvdata
     */
    void  recvNextTestAA(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void  recvNextTestADP(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void  recvNextTestEPI(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void  recvNextTestCOL(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void  recvNextTestRIS(const int commandIndex,
                            const int slaveAddr,
                            const int Action_motor,
                            const QStringList &recvdata);




private:
    QTimer* m_ackWatchdog;
    QThread* m_targetThread;
    int retryCount ;
    QByteArray mdataAArry;
    QByteArrayList m_lastOutboundCommands;
    Commandinfo mcontrastCommand;

private:
    int m_focus_sample_id = 0; //焦点样本号

    QString m_gripperErrTips;

    struct GripperVacuumHoldEval {
        bool pending = false;
        int sampleId = 0;
        quint8 holdQueue = 0; // 1=夹PPP 2=抓PRP
        quint8 reagentIndex = 0;
        quint32 airValue = 0;
        bool isSuction = false;
        quint8 suckCheckIndex = 0;
        quint8 positionIndex = 0;
        bool grabAtTray = true;
        QString actionLabel;
        QString failTips;
    };
    GripperVacuumHoldEval m_gripperVacuumHoldEval;

    //抓手异常复位起始时间戳(ms),用于复位指令未全部回完时的超时兜底;0表示未计时
    qint64 m_gripperErrStartMs = 0;
    // 善后真正做完才允许再次测试去动XY。超时弹窗时抓手可能还在下面。
    bool m_gripperRecoveryFinished = true;
    // 抓手复位应答回来之前，XY 的「Z未复位」按抓手报，不按双针报。
    bool m_gripperZStillDown = false;
    quint8 m_gripperHomeCmdIndex = 0;
    bool m_startClipAfterGripperRecovery = false;
    // 当前PRP抓杯失败并放弃后，清洗完继续同一样本的下一个试剂，不删整份样本。
    bool m_resumeNextReagentAfterGripGiveup = false;
    bool m_gripperFailPromptShown = false;

    QQueue<Finish_Data_> m_finishTestData; //完成测试通道
    QList<int> m_pendingtimeSample; //挂起样本
    QQueue<int> m_pendingContinueReagent; //弃杯后等待调度的下一试剂

    //测试异常模组读书为0
    int mTestErrSampleid;

    QByteArrayList m_pauseCommandQueue;

    int m_outstandingAcks = 0;
    bool m_waitingPppRead = false;
    bool m_pendingPppReread = false;
    bool m_waitingPrpRead = false;
    bool m_pendingPrpReread = false;
    bool m_abandonAfterThrowPpp = false;
    bool m_prpZeroSkipAfterThrow = false;
    quint8 m_pendingPrpReagent = 0;
    quint8 m_pendingPrpFocus = 0;

    quint8 m_totalChannel; //通道总数
    QList<quint8> mbackOriginnumList;
    bool m_Errorresetsfirst; //发生异常先所有轴复位
    bool m_needHomeBeforeResume = false; //电机异常后点「继续」须先全轴复位
    bool m_prepatchAckPaced = false; // 预填指令按应答逐条发（清洗针）

    // P1：待扣试剂耗材（XY 到试剂位时记录，流程成功完成后再扣）
    bool m_hasPendingReagentDeduct = false;
    quint8 m_pendingConsumableIndex = 0;

    // P2：待扣清洗液（发洗针时记录，清洗 ACK 完成后再扣）
    bool m_hasPendingCleanDeduct = false;
    quint8 m_pendingCleanDepleteType = 0; // CleaningFluidDepleteType

    bool m_pppRecoveryInProgress = false;
    QByteArrayList m_pppRecoveryCommands;
    QList<quint8> m_pppRecoveryAckIndices;
    int m_pppRecoveryAckPos = 0;

    QTimer* m_gripperVacuumPollTimer = nullptr;
    QTimer* m_offlineVoidTimer = nullptr;
    bool m_offlineWaitingReconnect = false;
    bool m_offlineVoided = false;
    enum GripperRestartAwait {
        GripperRestartNone = 0,
        GripperRestartPppTray,
        GripperRestartThrowChannel,
        GripperRestartPrpReagent,
        GripperRestartPrpTray
    };
    GripperRestartAwait m_gripperRestartAwait = GripperRestartNone;
    quint8 m_restartReagent = 0;

    // 断线重连快照（全轴复位后用于续跑与状态展示）
    bool m_reconnectSnapshotValid = false;
    int m_reconnectSampleId = 0;
    quint8 m_reconnectFocusActive = 0;
    quint8 m_reconnectTestChn = 0;
    bool m_reconnectHadFocus = false;
    QString m_lastWorkflowStatusLog;

    static const QHash<int, QString> s_cleanActionMap;      // 清洗类型映射表
    static const QHash<quint8, QString> s_focusActionMap;  // FOCUS 中文描述

    static std::mutex s_actionMutex; // 静态互斥锁

};

#endif // TESTPROJECTPROCESS_H
