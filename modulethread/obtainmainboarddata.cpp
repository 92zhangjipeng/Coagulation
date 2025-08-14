#include "obtainmainboarddata.h"
#include "QsLog/include/QsLog.h"
#include "operclass/fullyautomatedplatelets.h"
#include <mainwindow.h>

ObtainMainBoardData::ObtainMainBoardData(QObject *parent):QObject(parent)
{
	m_ispause = false; //标记非暂停
}

ObtainMainBoardData::~ObtainMainBoardData(){

	if (timer->isActive())
		timer->stop();
	delete timer;
	timer = nullptr;
}

void ObtainMainBoardData::onCreateTimer()
{

    timer = new QTimer();
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
    timer->start();
}

void ObtainMainBoardData::onTimeout()
{
    FullyAutomatedPlatelets::mainWindow()->timeoutObtainMainboadData();

    //emit sendReadMainboardData();
    //QLOG_DEBUG() <<"work thread id:" << QThread::currentThreadId(); //打印出线程ID，看看是否UI线程的ID不同
}

void ObtainMainBoardData::recvStopObatinMachineInfo()
{
	if (timer)
	{
		if (timer->isActive())
		{
			timer->stop();
			QLOG_DEBUG() << "暂停主板遍历消息";
		}
	}
   
}
void ObtainMainBoardData::recvaNewconnectMachine()
{
    if(timer)
    {
        if(!timer->isActive())
        {
            timer->start(1000);
            QLOG_DEBUG()<<"主板从新开始遍历消息";
        }
    }

}

void ObtainMainBoardData::stopImmediately()
{
    QMutexLocker locker(&m_lock);
    m_isCanRun = false;
}
void ObtainMainBoardData::pauseImmediately()
{
	QMutexLocker locker(&m_lock);
	m_ispause = true;
}

void ObtainMainBoardData::resetImmediately()
{
	QMutexLocker locker(&m_lock);
	m_ispause = false;
}


