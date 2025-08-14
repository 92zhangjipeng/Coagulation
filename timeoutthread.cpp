#pragma execution_character_set("utf-8")

#include "timeoutthread.h"
#include <QtConcurrent>
#include "usb_initconnect.h"


TimeoutThread::TimeoutThread(QObject *parent):
    QThread(parent)
{

}

TimeoutThread::~TimeoutThread(){

}







