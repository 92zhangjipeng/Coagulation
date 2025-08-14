#include "serialconnection.h"

#include "QsLog/include/QsLog.h"
#include <QSerialPortInfo>
#include <QMessageBox>

#define  DEVICE_VID   (1155)
#define  DEVICE_PID   (22336)

SerialConnection::SerialConnection(QObject *parent) : QObject(parent)
{
    _serial = new QSerialPort(this);

    connect(_serial,SIGNAL(error(QSerialPort::SerialPortError)),this,
            SLOT(handlError(QSerialPort::SerialPortError)));

    connect(_serial,SIGNAL(readyRead()),this,SLOT(readData()));

    _processingData = true;
}

SerialConnection::~SerialConnection()
{
    if(_serial->isOpen())
        _serial->close();

    delete _serial;
}

void SerialConnection::findSerialDevices()
{
    _portInfo.clear();
    _ports.clear();

    foreach (const QSerialPortInfo &port,QSerialPortInfo::availablePorts())
    {
        QLOG_DEBUG()<<port.portName()<<port.vendorIdentifier();
        int intHex_VID = info.vendorIdentifier();
        int intHex_PID = info.productIdentifier();
        if(intHex_VID == DEVICE_VID && intHex_PID == DEVICE_PID) {
            _portInfo += port;
            _ports += port.portName();
        }
    }
}

int SerialConnection::openSerialPort(QSerialPortInfo x)
{
    int error = 0;
    _serial->setPort(x);

    if(!_serial->isOpen())
    {
        if (_serial->open(QIODevice::ReadWrite))
        {
            _serial->setBaudRate(QSerialPort::Baud1000000);
            _serial->setDataBits(QSerialPort::Data8);
            _serial->setParity(QSerialPort::NoParity);
            _serial->setStopBits(QSerialPort::OneStop);
            _serial->setFlowControl(QSerialPort::NoFlowControl);
            QLOG_DEBUG()<< tr("Connected to %1").arg(x.portName())<<endl;
        }
        else
        {
            //QMessageBox::critical(NULL, tr("Error"), _serial->errorString());
            QLOG_DEBUG()<< tr("Open error")<<_serial->error()<<endl;
            _serial->close();
            emit serialError();
            error = 1;
        }
    }
    else
    {
        QLOG_DEBUG()<< "port already open!";
        error = 0;
    }

    return error;
}

int SerialConnection::openConnection()
{
    QSerialPortInfo x;
    int open = false;

    if(!_portInfo.isEmpty() && !_ports.isEmpty()){
        x =  _portInfo.at(0);
        open = true;
    }

    QLOG_DEBUG() << "is busy? " << x.isBusy()<<endl;

    if(!open) return -1;

    QLOG_DEBUG() << "open serial port " << x.portName()<<endl;

    //open serial port
    return openSerialPort(x);
}

void SerialConnection::closeConnection()
{
    _serial->close();
    QLOG_DEBUG()<<tr("COM port Disconnected");
    _processingData = true;
}

void SerialConnection::writeData(const QByteArray &data)
{
    if(_serial->isOpen())
    {
        _serial->write(data);
    }
    else
    {
        QLOG_DEBUG() << "not open - can't write?";
    }

}





void SerialConnection::readData(void)
{

}


void SerialConnection::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        //QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        _serial->close();

        _processingData = true;
    }
}
