#include "my_modbus.h"
#include "qdebug.h"
#include <QDebug>

my_modbus::my_modbus(QObject *parent) : QObject(parent)
{

}
bool my_modbus::initModbusSerialMaster(QString portName, qint32 baudRate,
                                         QSerialPort::DataBits dataBits,
                                         QSerialPort::Parity parity,
                                         QSerialPort::StopBits stopBits)
{

    m_master = new QModbusRtuSerialMaster(this);

    if(!m_master)
    {
        qDebug()<<"init master error";
        return 0;
    }

    m_master->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                                    portName);
    m_master->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                                    baudRate);
    m_master->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                                    dataBits);
    m_master->setConnectionParameter(QModbusDevice::SerialParityParameter,
                                    parity);
    m_master->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                                    stopBits);

    return 1;
}

bool my_modbus::initModbusNetworkMaster(QString address, int port)
{
    m_master = new QModbusTcpClient(this);

    m_mode = Tcp;
    if(!m_master)
    {
        qDebug()<<"init master error";
        return 0;
    }

    m_master->setConnectionParameter(QModbusDevice::NetworkAddressParameter,
                                    address);
    m_master->setConnectionParameter(QModbusDevice::NetworkPortParameter,
                                    port);
    return 1;
}

bool my_modbus::connectDevice()
{
    return m_master->connectDevice();
    qDebug()<<"0000999"<<m_master->connectDevice();
}

bool my_modbus::writeRegisterData(QString str, int startAddress, int num)
{
    QByteArray byte = QByteArray::fromHex(str.toLatin1().data());
    QString hexStr = byte.toHex().data();

    //发送的数据信息
    QModbusDataUnit writeUnit(QModbusDataUnit::HoldingRegisters
                              ,startAddress,num);

    //得到的寄存器的类型传给table
    QModbusDataUnit::RegisterType table = writeUnit.registerType();
    for(int i = 0;i < writeUnit.valueCount(); i++)
    {
        int j = 2*i;
        QString str = hexStr.mid(j,2);

        bool ok;
        int hex = str.toInt(&ok,16);

        writeUnit.setValue(i,hex);
    }
    //1 代表server address sendWriteRequest 是想服务器写数据
    if(auto* reply = m_master->sendWriteRequest(writeUnit,1))
    {
        if(!reply->isFinished())
        {
            connect(reply,&QModbusReply::finished,this,[this,reply]()
            {
                if(reply->error() == QModbusDevice::ProtocolError)
                {
                    qDebug()<<"write response ProtocolError:"<<reply->errorString();
                    return 0;
                }
                else if(reply->error() != QModbusDevice::NoError)
                {
                    qDebug()<<"write response error:"<<reply->errorString();
                    return 0;
                }

                reply->deleteLater();
            });
        }
        else
        {
            reply->deleteLater();
            return 1;
        }
    }
    else
    {
        qDebug()<<"write error: "<<m_master->errorString();
        return 0;
    }


}

void my_modbus::readRegisterData(int startAddress, int num)
{
    if(!m_master)
        return;

    QModbusDataUnit readUnit(QModbusDataUnit::HoldingRegisters,startAddress,num);

    if(auto* reply = m_master->sendReadRequest(readUnit,1))
    {
        if(!reply->isFinished())
        {
            connect(reply,&QModbusReply::finished,this,&my_modbus::readyRead);

        }
        else
        {
            delete reply;
        }

    }
    else
    {
        qDebug()<<m_master->errorString();
    }
}

void my_modbus::handleDeviceError(QModbusDevice::Error newError)
{
    if(newError == QModbusDevice::NoError || !m_master)
        return;
    emit error_signal(m_master->errorString());
}

void my_modbus::onStateChanged(int state)
{
    if(state == QModbusDevice::UnconnectedState)
        emit stateChanged_signal(0);
    else if(state == QModbusDevice::ConnectedState)
        emit stateChanged_signal(1);
}

void my_modbus::readyRead()
{
    auto reply = qobject_cast<QModbusReply*>(sender());

    if(!reply)
        return;
    if(reply->error() == QModbusDevice::NoError)
    {

        const QModbusDataUnit unit = reply->result();

        for(uint i = 0; i< unit.valueCount();i++)
        {
            const QString entry = tr("Address:%1,Value:%2").arg(unit.startAddress()+i)
                    .arg(QString::number(unit.value(i),unit.registerType()
                                         <= QModbusDataUnit::Coils?10:16));

            //qDebug()<<entry;

            emit readData_signal(unit.startAddress()+i,unit.value(i));
        }
    }
    else if(reply->error() == QModbusDevice::ProtocolError)
    {
        qDebug()<<"error protocolError"<<reply->errorString();
    }
    else
    {
        qDebug()<<"error:"<<reply->errorString();
    }

    reply->deleteLater();
}





//My_Modbus::My_Modbus()
//{
//    modbusDevice = new QModbusRtuSerialMaster(nullptr);
//    connect(modbusDevice,SIGNAL(errorOccurred()),this,SLOT(modbusMessage()));
//    if(modbusDevice)
//    {
//        qDebug("创建 Modbus Master 成功。");
//        connect(modbusDevice, &QModbusClient::stateChanged, this, &My_Modbus::modbusStateChanged);
//    }
//    else
//    {
//        qDebug("创建 Modbus Master 失败。");
//    }
//}
//My_Modbus::~My_Modbus()
//{
//    delete modbusDevice;
//}
//void My_Modbus::modbusConnect(void)
//{
//    if(!modbusDevice) return;
//    if(modbusDevice->state() != QModbusDevice::ConnectedState)
//    {
//        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter, m_settings.portName);
//        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, m_settings.baud);
//        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,   m_settings.parity);
//        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, m_settings.dataBits);
//        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, m_settings.stopBits);
//        modbusDevice->setTimeout(m_settings.responseTime);
//        modbusDevice->setNumberOfRetries(m_settings.numberOfRetries);
//        if(modbusDevice->connectDevice())
//        {
//            qDebug("Modbus 连接成功。");
//        }
//        else
//        {
//            qDebug("Modbus 连接失败。");
//        }
//    }
//}
//void My_Modbus::modbusDisconnect(void)
//{
//    if (modbusDevice)   modbusDevice->disconnectDevice();
//    delete modbusDevice;
//    modbusDevice = nullptr;
//}
//void My_Modbus::modbusStateChanged(int state)
//{
//    if(state == QModbusDevice::UnconnectedState)
//    {
//        qDebug("Modbus 已断开。");
//    }
//    else if(state == QModbusDevice::ConnectedState)
//    {
//        qDebug("Modbus 已连接。");
//    }
//}
//void My_Modbus::modbusMessage(void)
//{
//    qDebug() << modbusDevice->errorString();
//}

//void My_Modbus::modbusMessage(QString mess)
//{
//    qDebug() << mess;
//}
//void My_Modbus::modbusRead(int serverAddress, QModbusDataUnit::RegisterType table, int startAddress, int numOfEntries)
//{
//        if(!modbusDevice || modbusDevice->state() != QModbusDevice::ConnectedState)
//        {
//                qDebug("Modbus Device is not connected!");
//                return;
//        }

//        QModbusDataUnit dataUnit = QModbusDataUnit(table, startAddress, numOfEntries);

//        if(auto *reply = modbusDevice->sendReadRequest(dataUnit, serverAddress))
//        {
//                if(reply->isFinished())
//                {
//                    connect(reply, &QModbusReply::finished, this, &My_Modbus::modbusReadData);
//                }
//        }
//        else
//        {
//             modbusMessage(tr("Write error:") + modbusDevice->errorString());
//        }
//}
//void My_Modbus::modbusReadData()
//{
//    QModbusDataUnit dataUnit;
//    auto reply = qobject_cast<QModbusReply *>(sender());
//    if(!reply) return;

//    if(reply->error() == QModbusDevice::NoError)
//    {
//        dataUnit = reply->result();
//        emit modbusReadReady(dataUnit);
//    }
//    else if(reply->error() == QModbusDevice::ProtocolError)
//    {
//        modbusMessage(tr("Read response error: %1 (Modbus exception :0x%2)")
//                                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1 ,16));
//    }
//    else
//    {
//        modbusMessage(tr("Read response: %1 (code :0x%2)")
//                                        .arg(reply->errorString()).arg(reply->rawResult().exceptionCode(), -1 ,16));
//    }
//    reply->deleteLater();
//}

////服务器地址、寄存器类型、起始地址、变量数量、写入数据
//void My_Modbus::modbusWrite(int serverAddress, QModbusDataUnit::RegisterType table, int startAddress, int numOfEntries, QVector<quint16> data)
//{
//    if(!modbusDevice || modbusDevice->state() != QModbusDevice::ConnectedState)
//    {
//            qDebug("Modbus Device is not connected!");
//            return;
//    }
//     //存储接收和发送数据的类，数据类型为1bit和16bit QModbusDataUnit
//    //QModbusDataUnit，是用来处理通过串口一次传输的数据
//    QModbusDataUnit dataunit = QModbusDataUnit(table, startAddress, numOfEntries);
//    for(uint i = 0; i < dataunit.valueCount(); i++)
//    {
//            dataunit.setValue( i,  data.at(i));
//    }
//    if(auto *reply = modbusDevice->sendWriteRequest(dataunit,  serverAddress))
//    {
//        connect(reply, &QModbusReply::finished, this, [this, reply](){
//                if(reply->error() == QModbusDevice::ProtocolError)
//                {
//                    QString str_temp = QString("Write response error:%1 (modbus exception:0x%2)").arg(reply->errorString())
//                                        .arg(reply->rawResult().exceptionCode()/*, -1, 16), 5000*/);
//                    modbusMessage(str_temp);
//                }else{
//                    QString temp = QString("Write response:%1 (code:0x%2)").arg(reply->errorString())
//                            .arg(reply->rawResult().exceptionCode()/*, -1, 16), 5000*/);
//                    modbusMessage(temp);
//                    }
//        reply->deleteLater(); });
//    }
//    else{
//            //statusBar()->showMessage(tr("Write error:") + modbusDevice->errorString(), 5000);
//    }
//}
//void My_Modbus::SetPortName(QString name)
//{
//    m_settings.portName = name;
//}
