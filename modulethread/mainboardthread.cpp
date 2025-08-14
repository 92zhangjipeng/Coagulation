#pragma execution_character_set("utf-8")
#include "mainboardthread.h"
#include "QsLog/include/QsLog.h"
#include <operclass/fullyautomatedplatelets.h>
#include <mainwindow.h>

mainboardThread::mainboardThread(QObject* parent) :QThread(parent),
    pauseFlag(false),
    stopFlag(false)
{

}
mainboardThread::~mainboardThread()
{
    stop_th();
    QLOG_DEBUG()<<__FUNCTION__ <<"[析构主板通讯线程]"<< endl;
	delete _timer;
	_timer = nullptr;
}

mainboardThread::StateMainBoard mainboardThread::state() const
{
    StateMainBoard s = Stoped;
    if(!QThread::isRunning())
    {
        s = Stoped;
    }
    else if(QThread::isRunning() && pauseFlag)
    {
        s = Paused;
    }
    else if(QThread::isRunning() && (!pauseFlag))
    {
        s = Running;
    }
    return s;
}
void mainboardThread::start(Priority pri)
{
    QThread::start(pri);
}

void mainboardThread::stop_th()
{
    if (mainboardThread::isRunning())
    {
        stopFlag = true;
        condition.wakeAll();
		mainboardThread::quit();
		mainboardThread::wait();
    }
    QLOG_DEBUG() << "退出同步主板状态线程" << mainboardThread::currentThreadId();
}

void mainboardThread::pause()
{
    if (mainboardThread::isRunning())
    {
        pauseFlag = true;
    }

}

void mainboardThread::resume()
{
    if(mainboardThread::isRunning())
    {
        pauseFlag = false;
        condition.wakeAll();
    }
}
void mainboardThread::run()
{
    QLOG_DEBUG() << QString("开始执行计时线程") << mainboardThread::currentThreadId();
    _timer = new QTimer();
    _timer->setInterval(1000);
    connect(_timer, &QTimer::timeout, [=] {
        if (!pauseFlag)
        {
            timerRun();
        }
        if (stopFlag)
        {
            _timer->stop();
        }
        });
    _timer->start();
    this->exec();

    pauseFlag = false;
    stopFlag = false;
    QLOG_DEBUG() << QString("结束执行计时线程") << mainboardThread::currentThreadId();
}

void mainboardThread::timerRun()
{
    emit sendCtrlSignal();
}

