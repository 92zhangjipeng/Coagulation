#ifndef MYTIMERWORKER_H
#define MYTIMERWORKER_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include <QsLog/include/QsLog.h>


// 1. 增强定时器工作者类（支持安全关闭）
class MyTimerWorker : public QObject {
    Q_OBJECT
public:
    explicit MyTimerWorker(QObject *parent = nullptr)
        : QObject(parent), m_timer(new QTimer(this)) {
        // 配置定时器
        m_timer->setInterval(5000);
        connect(m_timer, &QTimer::timeout, this, &MyTimerWorker::onTimeout);
    }

    void start() {
        if (m_timer && !m_timer->isActive()) {
            m_timer->start();
            QLOG_DEBUG() << "定时Worker线程ID:" << QThread::currentThreadId();
        }
    }

    // 新增：安全停止定时器
    void stop() {
        if (m_timer && m_timer->isActive()) {
            m_timer->stop();
            qDebug() << "定时器已停止";
        }
    }

signals:
    void taskCompleted(const QString &result);
    void stopped();  // 新增：停止完成信号

private slots:
    void onTimeout() {
        if (m_stopRequested) return;  // 停止请求检查

        // 执行耗时任务
        QThread::msleep(300);
        emit taskCompleted(QTime::currentTime().toString());
    }

private:
    QTimer *m_timer;
    std::atomic<bool> m_stopRequested{false};  // 原子停止标志
};
#endif // MYTIMERWORKER_H
