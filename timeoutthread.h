#ifndef TIMEOUTTHREAD_H
#define TIMEOUTTHREAD_H

//#pragma once

#include <QObject>
#include <QQueue>
#include <QThread>
#include <QTimer>
#include "suoweiserialport.h"

class TimeoutThread : public QThread
{
    Q_OBJECT
public:
    TimeoutThread(QObject *parent);
	~TimeoutThread();
		
private slots:

signals:

public slots:

public:
 
private:

};

#endif // TIMEOUTTHREAD_H
