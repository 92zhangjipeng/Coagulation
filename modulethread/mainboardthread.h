#ifndef MAINBOARDTHREAD_H
#define MAINBOARDTHREAD_H

#pragma once

#include <QObject>
#include <qthread.h>
#include <atomic>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>

class mainboardThread : public QThread
{
    Q_OBJECT

public:
    mainboardThread(QObject* parent = nullptr);
    ~mainboardThread() override;

    enum StateMainBoard
    {
        Stoped,     ///<停止状态，包括从未启动过和启动后被停止
        Running,    ///<运行状态
        Paused      ///<暂停状态
    };
    StateMainBoard state() const;


private:
    std::atomic_bool pauseFlag;
    std::atomic_bool stopFlag;
    QMutex mutex;
    QWaitCondition condition;

    QTimer* _timer = nullptr;
public slots:
    void timerRun();//用于反复跑的内容
    void start(Priority pri = InheritPriority);
    void stop_th();
    void pause();
    void resume();

protected:
    virtual void run() override final;
signals:
    void sendCtrlSignal();//和主线程交互触发的signal


};

#endif // MAINBOARDTHREAD_H
