#include "obtainmainboarddata.h"
#include <QDebug>
#include "QUIUtils.h"      // 假设 Traverse_the_motherboard 在这里声明
#include "cglobal.h"       // 假设 gserialConnecStatus 在这里

ObtainMainBoardData::ObtainMainBoardData(QObject *parent)
    : QObject(parent)
{
    m_isPaused = 0;
    m_isStopped = 0;
}

ObtainMainBoardData::~ObtainMainBoardData()
{
    stop();
}

void ObtainMainBoardData::start()
{
    QMutexLocker locker(&m_mutex);
    if (m_isStopped.loadAcquire()) {
        qWarning() << "Already stopped, cannot start again";
        return;
    }
    if (m_timer) {
        qWarning() << "Timer already exists";
        return;
    }
    // 定时器将依附于当前线程（工作线程）
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ObtainMainBoardData::onTimeout);
    m_timer->setInterval(1000);
    m_timer->start();
    qDebug() << "Main board data acquisition started";
}

void ObtainMainBoardData::stop()
{
    QMutexLocker locker(&m_mutex);
    m_isStopped.storeRelease(1);
    if (m_timer) {
        if (m_timer->isActive())
            m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }
    QLOG_DEBUG() << "遍历主板 stopped";
}

void ObtainMainBoardData::pause()
{
    QMutexLocker locker(&m_mutex);
    if (!m_timer) {
        QLOG_WARN() << "Cannot pause: timer not created";
        return;
    }
    if (m_isPaused.fetchAndStoreRelaxed(1) == 0) {
        m_timer->stop();
        QLOG_DEBUG() << "Main board acquisition paused";
    }
}

void ObtainMainBoardData::resume()
{
    QMutexLocker locker(&m_mutex);
    if (!m_timer) {
        QLOG_WARN() << "Cannot resume: timer not created";
        return;
    }
    if (m_isPaused.fetchAndStoreRelaxed(0) == 1) {
        if (!m_isStopped.loadAcquire()) {
            m_timer->start();
            QLOG_DEBUG() << "Main board acquisition resumed";
        }
    }
}

void ObtainMainBoardData::onTimeout()
{
    // 检查停止或暂停标志
    if (m_isStopped.loadAcquire() || m_isPaused.loadAcquire())
        return;

    // ---- 以下代码运行在工作线程中 ----
    if (cglobal::gserialConnecStatus) {
        QByteArray readMainBoard;
        // 调用原有的主板数据读取函数（该函数不应涉及UI）
        QUIUtils::Traverse_the_motherboard(readMainBoard);
        // 通过信号将数据发送到主线程
        emit boardDataReady(readMainBoard);
    }
}
