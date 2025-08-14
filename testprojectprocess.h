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

    void openChnMotor(const quint8, const bool);

    void getAnemiaValue();              //获取贫血初值

    void EmptyTubeCatched(quint8 );     //试管夹起变灰

    void readbloodyInitValue(quint8 indexReag,quint8 indexActive);  //读试管富血初值

    void openTestChnTest(const int ,const quint8, const quint8); //吐试剂完成打开测试通道测试

    void setprogressinitboolwaittext(bool iswait,quint8 index_chn);

    void sycn_SampleTestingChangInitColor(QPoint,quint8);

    void throwtesttube(); //弃杯成功

    void reminderTextOut(quint8 ReminderKind,const QString dataStr); //提示文字界面信号

    void theGripperFailed(const int, const QString,const quint8 );

    void pauseSycnMainuiUpdate(const quint8 lossReagent);


public slots:
    void handleStartTheTestTask();

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

    //断线重连
    void slot_connectEquipmentagin();

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

    //收到试剂针移动到的位置 试剂减少耗材
    void FindSuckReagentAxis(QPoint moveingloc, const int &indexSuckReagent);

    //更新测试界面试管颜色血样针吸吐样
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
     * @brief recv_spitReagent
     * @param indexActive
     * @param index_num
     * @param Dischargethetypeofreagent
     */
    void recv_spitReagent(quint8 indexActive, const int index_num, quint8 Dischargethetypeofreagent);


    void recv_cleanDoublePin(const int index_num);

    void recv_cleanReagentPin(const int index_num,const quint8 index_reag);

    void recv_cleanBloodPin(const int index_num);

    void recv_throwCup(quint8 indexActive,
                        const int commandIndex,
                        const int slaveAddr,
                        const int Action_motor,
                        const QStringList &recvdata);

    void throwcup_finish(); //丢杯完成后....

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

public:
    typedef QPair<quint8, quint8> FocusReagentPair;  // 定义类型别名
private:
    static const QMap<quint8, FocusReagentPair> FOCUS_MAPPING;  // 声明映射表
    void remberSendPara(const QByteArray& outdata, Commandinfo& remember_data); //保存发送的指令参数
    bool comparedSendPara(const Commandinfo &remember_data, quint8 addr, quint8 func, quint8 num);

private slots:
    void handleTimeout();


//接收指令
private:

    /** 动做未完成执行继续发送命令 或 异常先复位等待处理
     * @brief commandNotCompleteOrErr
     * @param isGripperError
     * @param senddata
     * @param tips
     * @param klatetimer
     */
    void commandNotCompleteOrErr(const bool isGripperError, const QByteArray senddata,
                                   const QString tips, const bool klatetimer);



    /**加样动作
     * @brief receiveInstructionsSampleactive
     * @param index_num  接收编号
     * @param axismovement 运动的轴
     * @param recvdata    收到的数据
     */
    void receiveInstructionsSampleactive(int index_num, int axismovement, const QStringList recvdata);


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
    void recv_throwAnemia(const int indexconde,
                              int slaveAddr,
                              int Action_motor,
                              const QStringList recvdata);

    /** 收测试AA命令
     * @brief recvNextTestAA
     * @param commandIndex
     * @param slaveAddr
     * @param Action_motor
     * @param recvdata
     */
    void  recvNextTestAA(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void  recvNextTestADP(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void recvNextTestEPI(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void recvNextTestCOL(const int commandIndex,const int slaveAddr,const int Action_motor,const QStringList &recvdata);

    void recvNextTestRIS(const int commandIndex,
                            const int slaveAddr,
                            const int Action_motor,
                            const QStringList &recvdata);




private:
    QTimer* m_timer;
    QThread* m_targetThread;
    int retryCount ;
    QByteArray mdataAArry;
    Commandinfo mcontrastCommand;

private:
    int m_focus_sample_id = 0; //焦点样本号

    QString m_gripperErrTips;

    QQueue<Finish_Data_> m_finishTestData; //完成测试通道
    QList<int> m_pendingtimeSample; //挂起样本

    //测试异常模组读书为0
    int mTestErrSampleid;

    QByteArrayList  m_pause_the_caching_instruction; //暂停缓存指令
    QByteArray  mpauseData;

    quint8 m_totalChannel; //通道总数
    QList<quint8> mbackOriginnumList;
    bool m_Errorresetsfirst; //发生异常先所有轴复位


    static const QHash<int, QString> s_cleanActionMap;      // 清洗类型映射表
    static const QHash<quint8, quint8> s_reagentFocusMap;  // 试剂与焦点关系映射表

    static std::mutex s_actionMutex; // 静态互斥锁



};

#endif // TESTPROJECTPROCESS_H
