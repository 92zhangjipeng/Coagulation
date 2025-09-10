#ifndef MODULETIMERTHREAD_H
#define MODULETIMERTHREAD_H

#include <QObject>
#include <atomic>
#include <QMutex>
#include <QTimer>



class moduletimerThread : public QObject
{
    Q_OBJECT
public:
    explicit moduletimerThread(QObject* parent = nullptr);
    ~moduletimerThread() override;

public slots:
    void stop();
    void pause();
    void resume();
    void stopObtainMachineInfo();
    void startObtainMachineInfo();
    void createTimer();
private slots:
	void onTimeout();
private:
    std::atomic_bool m_pauseFlag{false};
    std::atomic_bool m_stopFlag{false};
    QMutex m_mutex;
	QTimer* m_timer{ nullptr };
};

#endif // MODULETIMERTHREAD_H
