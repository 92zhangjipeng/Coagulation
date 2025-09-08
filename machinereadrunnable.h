#ifndef MACHINEREADRUNNABLE_H
#define MACHINEREADRUNNABLE_H

#include "suoweiserialport.h"
#include "testing.h"
#include <QObject>
#include <QRunnable>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include "QsLog/include/QsLog.h"


class MachineReadRunnable : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit MachineReadRunnable(QObject *parent = nullptr);
    ~MachineReadRunnable();

protected:

signals:

public slots:

private:


};

//读取模组信息
class ReadModulInfo : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit ReadModulInfo(int id,QObject *parent = nullptr);
    ~ReadModulInfo();
protected:
    void run() override;  
public slots:



signals:
    void  ReadMachineState(const QByteArrayList &dataList,
                           const QString &info);  //读取模组数据消息
public:
    void sycnpSerialPort(SuoweiSerialPort *pSerialpotr); //同步串口线程
    void StopThread();

private:
    QByteArrayList m_modulcode;  //模组数据
    SuoweiSerialPort *m_SerialPort = nullptr;
    int m_idthread;
    bool mstop_;
    QMutex pauseLock;
};


///////////////////////////发送机器运动命令线程/////////////////////////////////////
class SendMachineSportInfo : public QObject ,public QRunnable
{
    Q_OBJECT
public:
    explicit SendMachineSportInfo(int id, QObject *parent = nullptr);
    ~SendMachineSportInfo();
     void sycnSerialclass(SuoweiSerialPort * Port);
protected:
    void run() override;

public slots:
    void willSendcode(const QByteArrayList codearry, const bool);
private:
    QByteArrayList m_machinespotr;  //数据
    SuoweiSerialPort *m_SerialPort_ = nullptr;
    int     m_idthread;
    QQueue<QByteArrayList> m_cachecode; //容量满时候缓存命令
signals:
    void  sendMachineState(const QByteArrayList &dataList,
                           const QString &info);

};



////////////////////////通讯配置机器参数//////////////////////////////
class ConfigureWriteParameter : public QObject ,public QRunnable
{
   Q_OBJECT
public:
   explicit ConfigureWriteParameter(int id, QObject *parent = nullptr);
    ~ConfigureWriteParameter();
    void sycnSerialclass(SuoweiSerialPort * port);
protected:
    void run() override;

public slots:
    void WriteCodearray(QByteArrayList);
    void slotDelayed_sending(const QByteArrayList , int delayedmS, bool needDelayed);
    void slotWhileReadChannelParam(const int Fromchannel);
private:
	void Delay_MSec_Suspend(unsigned int msec);
private:
   QByteArrayList m_machinespotr;  //数据
   SuoweiSerialPort *Serial_port = nullptr;
   int m_DelayedMS;
   bool m_bNeedDelayed;
   int m_id;
   int m_BeginChannel;


signals:
    void  WriteMachineAction(const QByteArrayList);


};
#endif // MACHINEREADRUNNABLE_H
