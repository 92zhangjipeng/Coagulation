#include "moduletimerThread.h"
#include "QsLog/include/QsLog.h"

moduletimerThread::moduletimerThread(QObject* parent)
    : QObject(parent)
{

}

moduletimerThread::~moduletimerThread()
{
    stopAll();
    QMutexLocker locker(&m_mutex);
    for (auto it = m_moduleTimers.begin(); it != m_moduleTimers.end(); ++it) {
        if (it.value()) {
            it.value()->stop();
            delete it.value();
        }
    }
    m_moduleTimers.clear();
}

void moduletimerThread::setModuleCount(int count)
{
    QMutexLocker locker(&m_mutex);
    if (count < 0) count = 0;

    // 移除超出范围的定时器
    QList<int> toRemove;
    for (auto it = m_moduleTimers.begin(); it != m_moduleTimers.end(); ++it) {
        if (it.key() > count) {
            if (it.value()) {
                it.value()->stop();
                delete it.value();
            }
            toRemove.append(it.key());
        }
    }
    for (int idx : toRemove) {
        m_moduleTimers.remove(idx);
        QLOG_DEBUG() << "移除模组定时器:" << idx;
    }

    // 添加新的定时器
    for (int i = 1; i <= count; ++i) {
        if (!m_moduleTimers.contains(i)) {
            createTimerForModule(i);
            QLOG_DEBUG() << "创建模组定时器:" << i;
        }
    }
}

void moduletimerThread::createTimerForModule(int moduleIndex)
{
    // 定时器父对象设为 nullptr，手动管理生命周期（避免线程迁移问题）
    QTimer* timer = new QTimer(nullptr);
    timer->setInterval(100);   // 100 毫秒
    // 使用 lambda 捕获模组编号
    connect(timer, &QTimer::timeout, this, [this, moduleIndex]() {
        onModuleTimeout(moduleIndex);
    });
    timer->start();
    m_moduleTimers[moduleIndex] = timer;
}

void moduletimerThread::removeTimerForModule(int moduleIndex)
{
    auto it = m_moduleTimers.find(moduleIndex);
    if (it != m_moduleTimers.end()) {
        if (it.value()) {
            it.value()->stop();
            delete it.value();
        }
        m_moduleTimers.erase(it);
    }
}

void moduletimerThread::onModuleTimeout(int moduleIndex)
{
    if (m_paused.load() || m_stopped.load()) {
        return;
    }
    emit requestSendModuleData(moduleIndex);
}

void moduletimerThread::startAll()
{
    m_stopped = false;
    m_paused = false;
    QMutexLocker locker(&m_mutex);
    for (QTimer* timer : m_moduleTimers) {
        timer->start();
    }
    QLOG_DEBUG() << "所有模组定时器已启动";
}


void moduletimerThread::stopAll()
{
    m_stopped = true;
    QMutexLocker locker(&m_mutex);
    for (QTimer* timer : m_moduleTimers) {
        timer->stop();
    }
    QLOG_DEBUG() << "所有模组定时器已停止";
}

void moduletimerThread::pauseAll()
{
    m_paused = true;
    QLOG_DEBUG() << "模组数据发送已暂停（定时器仍在运行）";
}

void moduletimerThread::resumeAll()
{
    m_paused = false;
    QLOG_DEBUG() << "模组数据发送已恢复";
}

