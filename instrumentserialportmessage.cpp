#include "instrumentserialportmessage.h"
#include  <qdebug.h>
#include "genericfunctions.h"
#include "loadequipmentpos.h"

InstrumentSerialPortMessage::InstrumentSerialPortMessage(QObject *parent) : QObject(parent)
{


}

InstrumentSerialPortMessage::~InstrumentSerialPortMessage()
{

}

void InstrumentSerialPortMessage::timerEvent(QTimerEvent *event)
{


}


//控制某通道的磁珠旋转开关
void InstrumentSerialPortMessage::ControlWhichoneChannelrevolving(const int Channel ,const bool Openrevolving)
{
    QMutexLocker locker(&m_lock);
    QByteArray Modularray;
    Modularray.clear();
    if (Channel >= 1 && Channel < 5)
    {
        Modularray = Testing::m_TaskDll->_SetTestModeInfo(0, 0, Channel, Openrevolving);
    }
    else if (Channel >= 5 && Channel < 9)
    {
        Modularray = Testing::m_TaskDll->_SetTestModeInfo(1, 0, Channel, Openrevolving);
    }
    else if (Channel >= 9 && Channel <= 12)
    {
        Modularray = Testing::m_TaskDll->_SetTestModeInfo(2, 0, Channel, Openrevolving);
    }
    quint8 OpenChannel = Channel;
    QThread::msleep(1);

    return;
}





