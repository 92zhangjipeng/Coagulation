#ifndef OBTAINMAINBOARDDATA_H
#define OBTAINMAINBOARDDATA_H

#include <QObject>
#include <QMutex>
#include <QThread>
#include <QTimer>

class ObtainMainBoardData : public QObject
{
    Q_OBJECT
public:
    ObtainMainBoardData(QObject* parent = nullptr);
    ~ObtainMainBoardData();

public slots:
        void stopImmediately();
		void pauseImmediately();
		void resetImmediately();
private:
        QMutex m_lock;
        bool m_isCanRun;
		bool m_ispause;
        QTimer* timer = nullptr;

public slots:
    void onCreateTimer();
    void onTimeout();
    void recvStopObatinMachineInfo();
    void recvaNewconnectMachine();

signals:
      //void  sendReadMainboardData();

};

#endif // OBTAINMAINBOARDDATA_H
