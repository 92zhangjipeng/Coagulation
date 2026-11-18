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

    // 先取出再删，避免边遍历边 removeOne 导致漏删/下标错乱
    while (!m_qTimerList.isEmpty()) {
        QTimer *pt = m_qTimerList.takeFirst();
        if (!pt) {
            continue;
        }
        pt->stop();
        delete pt;
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
