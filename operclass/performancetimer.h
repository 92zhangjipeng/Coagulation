#ifndef PERFORMANCETIMER_H
#define PERFORMANCETIMER_H

#include <qt_windows.h>
#include <QtCore/QObject>

class PerformanceTimer : public QObject
{
    Q_OBJECT
public:
    explicit PerformanceTimer(QObject *parent = nullptr);
    ~PerformanceTimer();
signals:
      void timeout();
public slots:
      void start(int);
      void stop();
      friend   void WINAPI   CALLBACK PeriodCycle(uint,uint,DWORD_PTR,DWORD_PTR,DWORD_PTR);
private:
      int m_interval;
      int m_id;

};

#endif // PERFORMANCETIMER_H
