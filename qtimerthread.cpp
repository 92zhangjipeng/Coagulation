#include "qtimerthread.h"
#include <QDebug>

QTimerThread::QTimerThread(int iCount)
{
    m_iThreadCount = iCount;
}
QTimerThread::~QTimerThread()
{
    if(thread.isRunning()){
        thread.quit();
        thread.wait();
    }

    //对象的销毁顺序也很重要
	for (int i = 0; i < m_qTimerList.size(); i++) {
		QTimer *pt = m_qTimerList.at(i);
		pt->stop();
		m_qTimerList.removeOne(pt);
		delete pt;
		pt = nullptr;
	}
}

void QTimerThread::stopThread(){
    if(thread.isRunning()){
        thread.quit();
        thread.wait();
    }
}
void QTimerThread::createItem()
{
    for(int i = 0;i < m_iThreadCount;i++)
    {
        QTimer *timer = new QTimer();
        m_qTimerList.append(timer);
    }
}

void QTimerThread::startMultThread()
{
    for(int i = 0; i < m_qTimerList.size(); i++)
    {
        m_qTimerList.value(i)->start(1000);
        m_qTimerList.value(i)->moveToThread(&thread);
        QObject::connect(m_qTimerList.value(i),SIGNAL(timeout()),this,SLOT(update()),Qt::QueuedConnection);
		thread.start();
    }
}

void QTimerThread::update()
{
    emit OneSecSendData();
}
