#ifndef TRANSCEIVER_SERIAL_DATA_H
#define TRANSCEIVER_SERIAL_DATA_H

#include <QObject>
#include <QTimer>
#include "custom_struct.h"
#include "qdebug.h"
#include <QDebug>

class Transceiver_Serial_Data : public QObject
{
    Q_OBJECT
public:
    explicit Transceiver_Serial_Data(QObject *parent = nullptr);

signals:


public:
    Settings m_setting;
public slots:
    void SetPortName(QString name);
private:
    void Initialize_Serial();//初始化串口
private:
    QSerialPort  *m_Serialport = nullptr;
    QTimer *m_timer;

};

#endif // TRANSCEIVER_SERIAL_DATA_H
