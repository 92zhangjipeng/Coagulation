#ifndef OBTAINMAINBOARDDATA_H
#define OBTAINMAINBOARDDATA_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QAtomicInt>

class ObtainMainBoardData : public QObject
{
    Q_OBJECT
public:
    explicit ObtainMainBoardData(QObject *parent = nullptr);
    ~ObtainMainBoardData();

public slots:
    void start();           // 开始采集（在工作线程中调用）
    void stop();            // 停止采集并销毁定时器
    void pause();           // 暂停采集
    void resume();          // 恢复采集

signals:
    // 将采集到的原始数据发送回主线程
    void boardDataReady(const QByteArray &data);
    void errorOccurred(const QString &error);

private slots:
    void onTimeout();       // 定时器超时，执行实际采集

private:
    QTimer *m_timer = nullptr;
    QAtomicInt m_isPaused;  // 0: 运行中, 1: 暂停
    QAtomicInt m_isStopped; // 0: 活动中, 1: 已停止
    QMutex m_mutex;
};

#endif // OBTAINMAINBOARDDATA_H
