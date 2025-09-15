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

private:


};

#endif // PERFORMANCETIMER_H
