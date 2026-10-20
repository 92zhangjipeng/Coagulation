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
    Q_UNUSED(event);
}


//控制某通道的磁珠旋转开关
void InstrumentSerialPortMessage::ControlWhichoneChannelrevolving(const int Channel ,const bool Openrevolving)
{
    if (Channel < 1 || Channel > 12) {
        QLOG_FATAL() << "无效通道:" << Channel;
        return;
    }

    const int module = (Channel - 1) / 4;
    // 锁内只做必要调用
    {
        QMutexLocker locker(&m_lock);
        Testing::getTaskDll()->_SetTestModeInfo(module, 0, Channel, Openrevolving);
    }

    // 延迟放在锁外，避免阻塞其他线程
    QThread::msleep(1);
    return;
}





