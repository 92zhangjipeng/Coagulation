#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "timeoutthread.h"
#include <QtConcurrent>
#include "usb_initconnect.h"


TimeoutThread::TimeoutThread(QObject *parent):
    QThread(parent)
{

}

TimeoutThread::~TimeoutThread(){

}







