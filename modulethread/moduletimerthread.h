#ifndef MODULETIMERTHREAD_H
#define MODULETIMERTHREAD_H

#pragma once
#include <qthread.h>
#include <atomic>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>
#include <QObject>



class moduletimerThread : public QObject
{
    Q_OBJECT
public:
    moduletimerThread(QObject* parent = nullptr);
    ~moduletimerThread();

private:
    std::atomic_bool pauseFlag;
    std::atomic_bool stopFlag;
    QMutex mutex;
    QTimer* timer = nullptr;

public slots:
	void stop_th();
    void pause_Module();
    void resume();

    void recvStopObatinMachineInfo();
    void recvaNewconnectMachine();

    void onCreateTimer();
    void onTimeout();

signals:
    //void sendCtrlSignal();//和主线程交互触发的signal
};

#endif // MODULETIMERTHREAD_H
