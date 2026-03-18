#ifndef DISPLAYCHANNELDATA_H
#define DISPLAYCHANNELDATA_H
#pragma once

#pragma execution_character_set("utf-8")

#include "channelvaluepanel.h"
#include "cglobal.h"
#include "qtimerthread.h"
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QTextBrowser>
#include <QThread>
#include "testing.h"
#include "mydatabase.h"
#include "mycustomcurve.h"
#include <creatcurve_data/customcurveadp.h>
#include "dilag/custompppvalue.h"
#include "datalprocessor.h"




class displayChanneldata : public QObject
{
    Q_OBJECT

public:
    explicit displayChanneldata(QObject *parent = nullptr);
    ~displayChanneldata();

    void startModuleData();
    void UpdateChannelValue(const int Channel ,const int ChannelData);
    int  GetChannelValue(quint8);  //获取通道值

    void setCalibrationParameters();

signals:
    /*更新显示设置界面的通道数据*/
    void SetChannelValueUpdate(const int,const int);
     //显示测试值 进度条&&曲线
    void DisplayTestingValue(const QString& , const quint8 &, const quint8 &,
                                const float&, const int&,const int&,const  int&);
    //加样在测试后清洗哪根针
    void addSampmeTestCleanPin(const int ,const quint8 index_reagent);
    void CompleteGetAnemiaValue(quint8 );
    //在测试通道吐试剂
    void spitReagentTesting();
    void testComplete(QString ,const quint8,const quint8 );
    //单个样本完成项目清除曲线&保存
    void finishtestProgress(const quint8 Channel, const quint8 Reagents);
    void sampleTestingErr(const QString &sampleid,const quint8 &channelIdx);
    //质控的值
    void sendChannelQualitydata(int,int);
    //内部测试PPP
    void signalShowPPPError(int value);
	//测试PRP0 异常值
    void channelDataError(const QString &errorMsg);

public slots:
    void getqualityControlValue(const quint8 Channelnum, int active);
    void startthread(); //线程开始
    void Recv_syncModuleChannelData(const QStringList channelData);/*解析到模组数据*/
    void slotgetAnemiaValue();  //获取通道贫血值
    //读取通道测试富血值
    void slotreadbloodyInitValue(quint8 indexReagent, quint8 indexActive);
    //打开通道测试
    void slotopenTestChnTest(const int sampleId, const quint8 indexActive, const quint8 index_reagent);
    //断线清空模组内队列缓存数据
    void handleCleanModuleBuffData();

private:

    /*通道数据收集*/
    void ChannelDataLinedup(int index ,int ModuleData);
    void filteringStyle(); //滤波处理数据

    /* 替换更新接收模组数据*/
    void ReplaceModuleValue(quint8 Channel, int mean);


    //中位值平均滤波（又称防脉冲干扰平均滤波） 移除极值后求平均
    int Medianaveragefiltering(QVector<int> recvData, const int channel);


    /* 中值濾波 - 期數個書-排序-取中間*/
    int optimizedMedianFiltering(QVector<int> &data);
    int MedianEWMADynamicAdaptiveFilter(const QVector<int>& intVec);


    // 保存滤波数据到CSV文件的独立函数
    void saveFilterDataToCSV(const QVector<int>& data, int result,
                            const QString& filename = "filter_data.csv",
                            const QString& path = QCoreApplication::applicationDirPath());




    /** 输出在测试的通道值
    * @brief OutputModulTestResult
    */
    void outPutModuleTestData();
    // 提取子函数，降低复杂度
    void processChannelData(int channelIdx);


    bool sampleAbnormality(const int& initprp, const int anaemiaValue, const int&curprp,
                               const QString& sampleid, const quint8 &channelIdx,const quint8 indexReagent);

    /**公式部分
     * @brief displayChanneldata::calculationFormula
     * @param sampleNum                     样本id
     * @param reagentIndex                  试剂编号
     * @param currentRichValue              实时PRP
     * @param baselinePoor                  初始PPP
     * @param baselineRich                  初始PRP
     * @param channelIdx                    测试通道从1开始
     */
    void calculationFormula(const QString& sampleNum,
                                quint8 reagentIndex,
                                int currentRichValue,
                                int baselinePoor,
                                int baselineRich,
                                int channelIdx);


    /** 测试完成
     * @brief handleTestCompletion
     * @param sampleNum     样本ID
     * @param reagentIndex  试剂号
     * @param channelIdx    通道号
     */
    void handleTestCompletion(const QString& sampleNum,
                              quint8 reagentIndex,
                              int channelIdx);

    /** 正在测试
     * @brief processTestData
     * @param sampleNum
     * @param reagentIndex
     * @param currentRichValue
     * @param baselinePoor
     * @param baselineRich
     * @param channelIdx
     * @param totalDataPoints
     */
    void processTestData(const QString& sampleNum,
                           quint8 reagentIndex,
                           int currentRichValue,
                           int baselinePoor,
                           int baselineRich,
                           int channelIdx,
                           int totalDataPoints);

    int getBaselinePoorValue(int channelIdx, int currentBaseline);


    double calculateTestAggregationRate(double prpn, double prpMax, double prp0);

    float calculateAggregationRate(const bool isLogMode,float PRPn, float PRP0, float PPP);

    float getRandomFactor(float min, float max);


    void savedrandData(const QString samplenum,const quint8 indexReagent);
    float creatvalue(const float testvalue, int num, const QString samplenum, const quint8 indexReagent, float &backvalue);
    void initializeOneTimeChannelUpdate(bool& allChannelsReceived);//启动每秒更新通道数据
    void processChannelData(int moduleIndex,
                               const std::array<int, FOUR_CHANNELS_PERMODULE>& data,
                               const  QStringList &receiveOriginalData);



//缓存算法
private:
    // 配置参数缓存
    mutable bool m_absorbanceAlgorithmCache = false;
    // 缓存获取算法模式
    bool getAbsorbanceAlgorithm() const {
        if(!m_absorbanceAlgorithmCache) {
            m_absorbanceAlgorithmCache  = INI_File().rConfigPara("AbsorbanceAlgorithm").toBool();
        }
        return m_absorbanceAlgorithmCache;
    }

    // 清除缓存（当配置可能改变时调用）
    void clearAlgorithmCache() {
        m_absorbanceAlgorithmCache = false;
        QLOG_DEBUG() << "对数模式缓存清除";
    }

private:

    QHash<quint8 ,int> m_ChnRealtimeData;//通道的实时值
    bool m_allModulesAccepted;
    quint8 m_configChannelCount;
    QQueue<int> m_queueChannel[MACHINE_SETTING_CHANNEL];

    // 使用标准容器//12个通道数据列队
    //std::array<QQueue<int>, MACHINE_SETTING_CHANNEL> m_queueChannel;

    std::unique_ptr<QTimerThread> m_timerThread; // 修改为智能指针
    static constexpr int dataLengthGroup = 9; //每秒钟采集处理的数据个数

    QSet<QString> sentSamples; // 用于存储已发送信号的样本ID
    QMutex queueMutex;
    float m_prevPRPn;

    QThread m_workerThread;
    bool  m_experimenttestData;

    static constexpr size_t kMaxChannels = MACHINE_SETTING_CHANNEL;
    //std::array<bool ,kMaxChannels> mOpenChnTest;
    bool mOpenChnTest[MACHINE_SETTING_CHANNEL];

    DataProcessor processor;

    double mk1 = 2.3;

    double mk2min = 2.3;
    double mk2max = 4;

    double mk3min = 4.0;
    double mk3max = 6.5;

    double mk4 = 6.5;

    double mcalibrationFactor1 = 2.3;
    double mcalibrationFactor3 = 0.6;
    double mcalibrationFactor4 = 0.5;

};

#endif // DISPLAYCHANNELDATA_H
