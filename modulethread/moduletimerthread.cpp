#include "moduletimerThread.h"
#include "QsLog/include/QsLog.h"
#include "mainwindow.h"
#include "operclass/fullyautomatedplatelets.h"

moduletimerThread::moduletimerThread(QObject* parent)
    : QObject(parent)
{

}

moduletimerThread::~moduletimerThread()
{
    stop();

   if (m_timer) {
       if (m_timer->isActive()) {
           m_timer->stop();
       }
       delete m_timer;
       m_timer = nullptr;
   }
}

void moduletimerThread::createTimer()
{
    QMutexLocker locker(&m_mutex);

    if (!m_timer) {
        m_timer = new QTimer(this);
        m_timer->setInterval(100);
        connect(m_timer, &QTimer::timeout, this, &moduletimerThread::onTimeout);
        m_timer->start();
    }
}

void moduletimerThread::onTimeout()
{
    if (m_pauseFlag || m_stopFlag) {
        return;
    }

    MainWindow* mainWindow = FullyAutomatedPlatelets::mainWindow();
    if (mainWindow) {
        mainWindow->ObtainModuleData();
    } else {
        QLOG_WARN() << "mainWindow is null in moduletimerThread::onTimeout";
    }
}

void moduletimerThread::stopObtainMachineInfo()
{
    QMutexLocker locker(&m_mutex);
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        QLOG_DEBUG() << "暂停模组遍历消息";
    }
}

void moduletimerThread::startObtainMachineInfo()
{
    QMutexLocker locker(&m_mutex);
    if (m_timer) {
        m_timer->start(100);
        QLOG_DEBUG() << "模组重新开始遍历消息";
    }
}

void moduletimerThread::stop()
{
    QMutexLocker locker(&m_mutex);
    m_stopFlag = true;
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void moduletimerThread::pause()
{
    QMutexLocker locker(&m_mutex);
    m_pauseFlag = true;
}

void moduletimerThread::resume()
{
    QMutexLocker locker(&m_mutex);
    m_pauseFlag = false;
}


