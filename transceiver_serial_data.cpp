#include "transceiver_serial_data.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
/*
***串口数据的收发
*/
Transceiver_Serial_Data::Transceiver_Serial_Data(QObject *parent) : QObject(parent)
{
    m_Serialport = new QSerialPort(this);

}
void Transceiver_Serial_Data::SetPortName(QString name){
    m_setting.portName = name;
}
//初始化串口
void Transceiver_Serial_Data::Initialize_Serial()
{
    m_Serialport->setPortName(m_setting.portName);
    bool flag = m_Serialport->open(QIODevice::ReadWrite);//打开串口
    if (m_Serialport->isOpen())
    {
        m_Serialport->setBaudRate(m_setting.baud);
        m_Serialport->setDataBits(m_setting.dataBits);//设置数据位数
        m_Serialport->setParity(m_setting.parity); //设置奇偶校验
        m_Serialport->setStopBits(m_setting.stopBits);//设置停止位
        m_Serialport->setFlowControl(m_setting.FlowControl);  //设置流控制
    }
     //flag;
    return;


}
