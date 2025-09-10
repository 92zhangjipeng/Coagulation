#include "obtainmainboarddata.h"
#include "QsLog/include/QsLog.h"
#include "operclass/fullyautomatedplatelets.h"
#include "mainwindow.h"

ObtainMainBoardData::ObtainMainBoardData(QObject *parent):QObject(parent)
{

}

ObtainMainBoardData::~ObtainMainBoardData()
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

void ObtainMainBoardData::createTimer()
{
    QMutexLocker locker(&m_mutex);

    if (!m_timer) {
        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, &ObtainMainBoardData::onTimeout);
        m_timer->start();
        QLOG_DEBUG() << "Main board data timer created and started";
    }
}

void ObtainMainBoardData::onTimeout()
{
    if (m_isStopped || m_isPaused) {
        return;
    }

    MainWindow* mainWindow = FullyAutomatedPlatelets::mainWindow();
    if (mainWindow) {
        mainWindow->timeoutObtainMainboadData();
    } else {
        QLOG_WARN() << "mainWindow is null in ObtainMainBoardData::onTimeout";
    }
}

void ObtainMainBoardData::stopDataAcquisition()
{
    QMutexLocker locker(&m_mutex);
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
        QLOG_DEBUG() << "暂停主板数据采集";
    }
}

void ObtainMainBoardData::startDataAcquisition()
{
    QMutexLocker locker(&m_mutex);
    if (m_timer && !m_timer->isActive()) {
        m_timer->start(1000);
        QLOG_DEBUG() << "恢复主板数据采集";
    }
}

void ObtainMainBoardData::stop()
{
    QMutexLocker locker(&m_mutex);
    m_isStopped = true;
    if (m_timer && m_timer->isActive()) {
        m_timer->stop();
    }
}

void ObtainMainBoardData::pause()
{
    QMutexLocker locker(&m_mutex);
    m_isPaused = true;
}

void ObtainMainBoardData::resume()
{
    QMutexLocker locker(&m_mutex);
    m_isPaused = false;
}

