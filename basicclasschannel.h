#ifndef BASICCLASSCHANNEL_H
#define BASICCLASSCHANNEL_H
#pragma execution_character_set("utf-8")
#include <QObject>
#include <QDebug>





class BasicClassChannel : public QObject
{
    Q_OBJECT
public:
    explicit BasicClassChannel(QObject *parent = nullptr);
    ~BasicClassChannel();

    virtual void ExitChannel();     //通道完成测试退出

    virtual void SetChannelRunning();

    virtual bool GetChannelRunning();

    virtual void Synchronize_Sample(const QString num, const QString name, const QString barcode); //同步样本信息

    virtual void TestingRuuningValue(const double); //通道反应值

    virtual void WillTestReagentType(const QList<int> ); //要反应得试剂种类

signals:

public slots:
};

#endif // BASICCLASSCHANNEL_H
