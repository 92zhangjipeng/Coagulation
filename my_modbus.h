#ifndef MY_MODBUS_H
#define MY_MODBUS_H
#include <QModbusClient>
#include <QModbusDataUnit>
#include <QObject>
#include <QSerialPort>
#include <QModbusRtuSerialMaster>
#include <QModbusTcpClient>
/*
 *
 *          modbus master 主站
 *
 *          1. modbusMaster * m_master  =  new modbusMaster(this);
 *
 *          2.初始化合适的模式 串口 网口
 *          initModbusSerialMaster(19200,QSeraiPort::DATA8,......)
 *          连接
 *
 *          3. connectDevice()
 *          写寄存器数据 01 02 03 地址开始为0 后面3个
 *
 *          4.writeRegisterData("01 02 03",0,3);
 *          读寄存器数据 0地址开始 后面3个
 *
 *          5.readRegisterData(0，3)；
 *          读出来的寄存器数据 会通过readData_signal 信号拿到
 *          address 地址 value 值
 *
 *
 *
*/
class my_modbus : public QObject
{
    Q_OBJECT
public:
    explicit my_modbus(QObject *parent = nullptr);

    enum modbusConnection
    {
        Serial,
        Tcp
    };

    /**
* @projectName   testMyClass
* @brief         初始化串口modbusMaster
* 其他参数 波特率 数据位 校验位 停止位
* @author        SMY
* @date          2019-03-27
*/
    bool initModbusSerialMaster(QString portName, qint32 baudRate, QSerialPort::DataBits dataBits,
                         QSerialPort::Parity parity, QSerialPort::StopBits stopBits);

    /**
* @projectName   testMyClass
* @brief         初始化网口modbusMaster
* ip地址,端口号
* @author        SMY
* @date          2019-03-27
*/
    bool initModbusNetworkMaster(QString address,int port);

    /**
* @projectName   testMyClass
* @brief         连接设备
* @author        SMY
* @date          2019-03-27
*/
    bool connectDevice();

    /**
* @projectName   testMyClass
* @brief         写寄存器数据,数据,开始地址,个数
* @author        SMY
* @date          2019-03-27
*/
    bool writeRegisterData(QString str,int startAddress,int num);
    /**
* @projectName   testMyClass
* @brief         读寄存器 开始地址,个数
* @author        SMY
* @date          2019-03-27
*/
    void readRegisterData(int startAddress,int num);
signals:
    //发生错误
    void error_signal(QString errorString);
    /*state :1 connect ,0:unconnect
    *状态发生改变
    */
    void stateChanged_signal(int state);

    /*读到寄存器的值信号*/
    void readData_signal(int address,int value);
public slots:
    /**
* @projectName   testMyClass
* @brief         device error
* @author        SMY
* @date          2019-03-27
*/
    void handleDeviceError(QModbusDevice::Error newError);
    /**
* @projectName   testMyClass
* @brief         连接状态改变
* @author        SMY
* @date          2019-03-27
*/
    void onStateChanged(int state);

    void readyRead();
private:
    QModbusClient* m_master;
    modbusConnection m_mode;
public:


private:
    //QModbusClient *modbusDevice = nullptr;

};

#endif // MODBUSMASTER_H
