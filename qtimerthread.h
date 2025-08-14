#ifndef QTIMERTHREAD_H
#define QTIMERTHREAD_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QTimer>

class QTimerThread : public QObject
{
    Q_OBJECT
public:
   QTimerThread(int iCount);
   ~QTimerThread();

    void createItem();
    void startMultThread();
    void stopThread();
public slots:
    void update();
private:
   int m_iThreadCount;//开启的线程个数
   QThread thread;
   QList<QTimer*> m_qTimerList;
signals:
   void OneSecSendData();
public slots:
};

#endif // QTIMERTHREAD_H
