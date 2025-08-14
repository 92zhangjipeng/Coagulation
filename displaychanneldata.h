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



struct gatherdata
{
   quint8 chn_;
   int id_;
   int data_;
   quint8 indexreag_;
};

class displayChanneldata : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit displayChanneldata(QObject *parent = nullptr);
    ~displayChanneldata();

    void    _start();

    void    UpdateChannelValue(const int Channel ,const int ChannelData);

    int     GetChannelValue(quint8);  //获取通道值
signals:
    void    SetChannelValueUpdate(const int,const int); /*更新显示设置界面的通道数据*/

     //显示测试值 进度条&&曲线
    void    DisplayTestingValue(const QString& , const quint8 &, const quint8 &,
                                    const float&, const int&,const int&,const  int&);

    void    addSampmeTestCleanPin(const int ,const quint8 index_reagent); //加样在测试后清洗哪根针

    void    CompleteGetAnemiaValue(quint8 );

    void    spitReagentTesting(); //在测试通道吐试剂

    void    testComplete(QString ,const quint8,const quint8 );

    //单个样本完成项目清除曲线&保存
    void    finishtestProgress(const quint8 Channel, const quint8 Reagents);

    void    sampleTestingErr(const QString &sampleid,const quint8 &channelIdx);

    //质控的值
    void    sendChannelQualitydata(int,int);

public slots:
    void   getqualityControlValue(const quint8 Channelnum, int active);

    void   _startthread(); //线程开始

    void    Recv_syncModuleChannelData(const QStringList channelData);/*解析到模组数据*/

    void   slotgetAnemiaValue();  //获取通道贫血值

    //读取通道测试富血值
    void   slotreadbloodyInitValue(quint8 indexReagent, quint8 indexActive);

    //打开通道测试
    void   slotopenTestChnTest(const int sampleId, const quint8 indexActive, const quint8 index_reagent);

    //断线清空模组内队列缓存数据
    void   handleCleanModuleBuffData();

private:

    /*通道数据收集*/
    void    ChannelDataLinedup(int index ,int ModuleData);
    void    filteringStyle(); //滤波处理数据


    /* 替换更新接收模组数据*/
    void    ReplaceModuleValue(quint8 Channel, int mean);

    /*中位值平均滤波法*/
    int     Medianaveragefiltering(QVector<int> RecvData);

    /* 中值濾波 - 期數個書-排序-取中間*/
    int     optimizedMedianFiltering(QVector<int> &data);

    void    OutputModulTestResult();/*输出在测试的结果*/

    bool    sampleAbnormality(const int& initprp, const int anaemiaValue, const int&curprp,
                               const QString& sampleid, const quint8 &channelIdx);

    /**
     * @brief Calculation_formula
     * @param sampleNum
     * @param reagentIndex
     * @param currentRichValue
     * @param baselinePoor
     * @param baselineRich
     * @param channelIdx
     */
    void Calculation_formula(const QString& sampleNum, quint8 reagentIndex,
                                int currentRichValue,
                                int baselinePoor, int baselineRich,
                                int channelIdx);
    float calculateAggregationRate(const bool isLogMode,float PRPn, float PRP0, float PPP);


    void    savedrandData(const QString samplenum,const quint8 indexReagent);

    float   creatvalue(const float testvalue, int num, const QString samplenum, const quint8 indexReagent, float &backvalue);

    void   initializeOneTimeChannelUpdate(bool& allChannelsReceived);//启动每秒更新通道数据

    void   processChannelData(int moduleIndex,
                               const std::array<int, FOUR_CHANNELS_PERMODULE>& data,
                               const  QStringList &receiveOriginalData);

private:

    QHash<quint8 ,int> m_ChnRealtimeData;//通道的实时值
	bool m_AllModulAccept;
    quint8 m_configChannelCount;
    QQueue<int> m_queueChannel[MACHINE_SETTING_CHANNEL];

    // 使用标准容器//12个通道数据列队
    //std::array<QQueue<int>, MACHINE_SETTING_CHANNEL> m_queueChannel;

    std::unique_ptr<QTimerThread> m_timerThread; // 修改为智能指针
    bool mOpneChnTest[MACHINE_SETTING_CHANNEL];
    const int  dataLengthGroup = 9; //每秒钟采集处理的数据个数


    QList<gatherdata* > m_channelRawData;//data_write;
    QMultiMap<quint8,QString> m_processedChannelData;//mchngetchardata;
    QSet<QString> sentSamples; // 用于存储已发送信号的样本ID

    QMutex queueMutex;

};

#endif // DISPLAYCHANNELDATA_H
