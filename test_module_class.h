#ifndef TEST_MODULE_CLASS_H
#define TEST_MODULE_CLASS_H
#pragma execution_character_set("utf-8")
#include <QVector>
#include <QObject>
#include <QTimer>
#include <QVariant>
#include <QMutex>
#include "cglobal.h"
#include "custom_struct.h"

#pragma once
//#pragma pack(pop)
typedef struct SingleChannelStatus
{
    QString BindSample;      //通道绑定样本号
    quint8  IndexChannel;    //通道号
    quint8  TestingReagent;  //通道在测试的试剂
    volatile bool    bChannelState;   //通道状态

}SingleChannelStatus;
typedef QVector<SingleChannelStatus*> TotalChannelStatusVec;
Q_DECLARE_METATYPE(SingleChannelStatus)


class Test_Module_Class : public QObject
{
    Q_OBJECT
public:
    explicit Test_Module_Class(QObject *parent = nullptr);
    ~Test_Module_Class();
signals:


public slots:

private:
    QMutex m_Mutex;
    TotalChannelStatusVec* mMachineTestChannel = nullptr;
};

#endif // TEST_MODULE_CLASS_H
