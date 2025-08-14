#include "moduletimerThread.h"
#include "QsLog/include/QsLog.h"
#include "operclass/fullyautomatedplatelets.h"
#include <mainwindow.h>

moduletimerThread::moduletimerThread(QObject* parent): QObject(parent),
    pauseFlag(false)
{
}

moduletimerThread::~moduletimerThread()
{
    //timer->stop();
    //timer->deleteLater();

	if(timer->isActive())
		timer->stop();
	delete timer;
	timer = nullptr;

}

void moduletimerThread::onCreateTimer()
{
    //关键点：在子线程中创建QTimer的对象
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
}

void moduletimerThread::onTimeout()
{
    FullyAutomatedPlatelets::mainWindow()->_ObtainModuleData();
    //emit sendCtrlSignal();
    //QLOG_DEBUG() << " work thread idModule:" << QThread::currentThreadId();
}

void moduletimerThread::recvStopObatinMachineInfo()
{
    if(timer)
    {
        if(timer->isActive())
        {
            timer->stop();
            QLOG_DEBUG()<<"暂停模组遍历消息";
        }
    }
}
void moduletimerThread::recvaNewconnectMachine()
{
    if(timer)
    {
        timer->start(100);
        QLOG_DEBUG()<<"模组从新开始遍历消息";
    }

}

void moduletimerThread::stop_th()
{
    QMutexLocker locker(&mutex);
    stopFlag = false;
}

void moduletimerThread::pause_Module()
{
    QMutexLocker locker(&mutex);
    pauseFlag = true;
}

void moduletimerThread::resume()
{
    QMutexLocker locker(&mutex);
    pauseFlag = false;
}



