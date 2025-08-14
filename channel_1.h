#ifndef CHANNEL_1_H
#define CHANNEL_1_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QDebug>
#include "basicclasschannel.h"
#include "cglobal.h"


struct WaitingReagentType
{
    bool AA_inducer;
    bool ADP_inducer;
    bool ADR_inducer;
    bool COL_inducer;
    bool RIS_inducer;
    WaitingReagentType() {
    AA_inducer = false;
    ADP_inducer = false;
    ADR_inducer = false;
    COL_inducer = false;
    RIS_inducer = false;
    }
};

class Channel_1 : public BasicClassChannel
{
public:
    Channel_1();
    ~Channel_1();

    void SetChannelRunning() override;

    bool GetChannelRunning() override;

    void Synchronize_Sample(const QString num, const QString name, const QString barcode) override; //同步样本信息

    void ExitChannel() override;

    void TestingRuuningValue(const double value) override;  //获取通道值 1S 一个

    void SavePoorBlood(bool); //保存贫血标志设置

    void SaveRichBlood(bool open); //保存富血设置

    void TestingReagentType(quint8); //在测试得试剂类型

    void WillTestReagentType(const QList<int> ) override;

signals:
    //void Exitthschannel();
private:
    bool m_channelRunning;
    QString m_number;    //样本编号
    QString m_name;     //样本姓名
    QString m_barcode;  //样本条形码

    bool m_saveinitpoorblood; //保存贫血值标志
    double m_initpoorblood; //初始贫血值

    bool m_saveinitRichBlood;   //保存富血值标志
    double  m_initrichblood;    //初始富血值

    quint8 m_Reagent_type;      //试剂类型
    bool m_startcalculating;

    QVector<double> m_TestingValue_AA; //试剂反应值
    QVector<double> m_TestingValue_ADP;
    QVector<double> m_TestingValue_ADR;
    QVector<double> m_TestingValue_COL;
    QVector<double> m_TestingValue_RIS;
    WaitingReagentType *m_Typainducer = nullptr;
};

#endif // CHANNEL_1_H
