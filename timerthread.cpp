#include "timerthread.h"
#include "QsLog/include/QsLog.h"

timerthread::timerthread(QObject* parent)
    : QThread(parent),
    pauseFlag(false),
    stopFlag(false)
{

}
timerthread::~timerthread()
{
    stop_th();
}

timerthread::State timerthread::state() const
{
    State s = Stoped;
    if (!timerthread::isRunning())
    {
        s = Stoped;
    }
    else if (timerthread::isRunning() && pauseFlag)
    {
        s = Paused;
    }
    else if (timerthread::isRunning() && (!pauseFlag))
    {
        s = Running;
    }
    return s;
}

void timerthread::start(Priority pri)
{
    QThread::start(pri);
    QLOG_DEBUG()<<"启动定时重连"<<timerthread::currentThreadId()<<endl;
}

void timerthread::stop_th()
{
    if (timerthread::isRunning())
    {
        stopFlag = true;
        condition.wakeAll();
		timerthread::quit();
		timerthread::wait();
    }
}

void timerthread::pause()
{
    if (timerthread::isRunning())
    {
        pauseFlag = true;
    }
}

void timerthread::resume()
{
    if (timerthread::isRunning())
    {
        pauseFlag = false;
        condition.wakeAll();
    }
}

void timerthread::run()
{
    QLOG_DEBUG() << QString("开始执行计时线程") << timerthread::currentThreadId();
    _timer = new QTimer();
    _timer->setInterval(50 * 20);
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
    QLOG_DEBUG() << QString("结束执行计时线程") <<timerthread::currentThreadId();
}

void timerthread::timerRun()
{
    emit sendCtrlSignal();
}
