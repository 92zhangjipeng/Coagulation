#ifndef MODULETIMERTHREAD_H
#define MODULETIMERTHREAD_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <QMap>
#include <atomic>

class moduletimerThread : public QObject
{
    Q_OBJECT
public:
    explicit moduletimerThread(QObject* parent = nullptr);
    ~moduletimerThread();

    // 设置模组总数（例如 8 表示模组 1~8），会动态增减定时器
    void setModuleCount(int count);

public slots:
    void startAll();      // 启动所有模组的定时器（清除停止标志）
    void stopAll();       // 停止所有定时器，设置停止标志
    void pauseAll();      // 暂停发送（定时器仍运行，但不会发射信号）
    void resumeAll();     // 恢复发送

signals:
    // 请求发送指定模组的数据（模组编号从 1 开始）
    void requestSendModuleData(int moduleIndex);

private slots:
    // 由每个模组定时器触发，发射请求信号
    void onModuleTimeout(int moduleIndex);

private:
    void createTimerForModule(int moduleIndex);
    void removeTimerForModule(int moduleIndex);

    QMap<int, QTimer*> m_moduleTimers;   // key: 模组编号 (1-based)
    QMutex m_mutex;
    std::atomic_bool m_paused{false};
    std::atomic_bool m_stopped{false};
};

#endif // MODULETIMERTHREAD_H
