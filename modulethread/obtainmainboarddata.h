#ifndef OBTAINMAINBOARDDATA_H
#define OBTAINMAINBOARDDATA_H

#include <QObject>
#include <QMutex>
#include <QTimer>
#include <atomic>

class ObtainMainBoardData : public QObject
{
    Q_OBJECT
public:
    explicit ObtainMainBoardData(QObject* parent = nullptr);
    ~ObtainMainBoardData() override;

public slots:
    void stop();
    void pause();
    void resume();
    void stopDataAcquisition();
    void startDataAcquisition();
    void createTimer();

private slots:
    void onTimeout();

private:
    QMutex m_mutex;
    std::atomic_bool m_isStopped{false};
    std::atomic_bool m_isPaused{false};
    QTimer* m_timer{nullptr};

};

#endif // OBTAINMAINBOARDDATA_H
