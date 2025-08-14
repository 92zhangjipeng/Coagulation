#include "modbus_customclass.h"
#include <QDebug>
#include <QThread>

Modbus_CustomClass::Modbus_CustomClass(QObject *parent) : QObject(parent)
{
    Tick = new QTimer(this);
    connect(Tick, SIGNAL(timeout()), this, SLOT(Tick_Server()));
    Read_TempLong = 0;//接收数据长度备份
    m_sendWrite = new QTimer(this);
    connect(m_sendWrite,SIGNAL(timeout()),this,SLOT(SendCode()));
    //m_sendWrite->start(1000);

}
Modbus_CustomClass::~Modbus_CustomClass()
{
//    delete m_modbusSerial;
//    m_modbusSerial = nullptr;
}

int Modbus_CustomClass::Init_Seiral_Port(QString portname)
{
    int Err=0;
     m_modbusSerial  = new QSerialPort;
    //设置串口名
    m_modbusSerial->setPortName(portname);
    //打开串口
    Err += m_modbusSerial->open(QIODevice::ReadWrite);
    //设置波特率
    Err += m_modbusSerial->setBaudRate(QSerialPort::Baud1000000);
    //设置数据位数
    Err += m_modbusSerial->setDataBits(QSerialPort::Data8);
     //设置奇偶校验
    Err += m_modbusSerial->setParity(QSerialPort::NoParity);
    //设置停止位
    Err += m_modbusSerial->setStopBits(QSerialPort::OneStop);
    //设置流控制
    Err += m_modbusSerial->setFlowControl(QSerialPort::NoFlowControl);
    //连接信号量
    connect(m_modbusSerial, &QSerialPort::readyRead, this, &Modbus_CustomClass::Read_Data);
    if(Err>5){
        Tick->start(5);
        m_serial_enable = 1;//标记当前已连接
        qDebug() << "串口初始话成功 ";
        qDebug() << "串口线程11111 ==thread: " << QThread::currentThread();
    }
    return Err;
}
void Modbus_CustomClass::SendCode()
{
    //qDebug() << "串口线程2222 ==thread: " << QThread::currentThread();
    /*arg(02).arg("0A").arg(01).arg(00).arg(00).arg(00).arg(00).arg(00).
    arg(00).arg(00).arg(00).arg(00).arg(00).arg(00).arg(00).arg(00);*/
   // Request(2,0x0a,);

}
//串口数据接收
void Modbus_CustomClass::Read_Data(){
    Read_Temp.append(m_modbusSerial->readAll());
    if(Read_Temp.length()>1000)Read_Temp.clear();
}
//关闭串口，释放资源
void Modbus_CustomClass::CloseUart(){
    if(m_serial_enable){
        m_modbusSerial->clear();
        m_modbusSerial->close();
        m_modbusSerial->deleteLater();
        Tick->stop();
    }
}

//分时处理消息
void Modbus_CustomClass::Tick_Server(){
    int i,LenTemp;
    unsigned int CRC16temp;
    unsigned short MBCRCreturn;
    if(Read_Temp.length()>0){
        if(Read_Temp.length()>Read_TempLong){
            Read_TempLong = Read_Temp.length();
        }else{
            //CRC校验
            LenTemp = Read_Temp.length();
			//unsigned char Read_TempP[LenTemp];  
			unsigned char * Read_TempP = (unsigned char*)malloc(sizeof(unsigned char) * LenTemp);
            for(i = 0; i < LenTemp; i++)
                Read_TempP[i] = Read_Temp[i];
            McMBCRC16(Read_TempP,LenTemp-2,&CRC16temp);
            MBCRCreturn = (Read_TempP[LenTemp-1]<<8)|((Read_TempP[LenTemp-2])&0xFF);
            if(CRC16temp!=MBCRCreturn){//CRC失败
                Read_TempLong = 0;
                Read_Temp.clear();
                return;
            }
            //数据接收
            Datatemp.clear();
            for(i=3;i<Read_Temp.length()-2;i++)
                Datatemp.append(Read_Temp[i]);
            //emit McModbusRTUMessage(Read_Temp[0],Read_Temp[1],Datatemp);
            Read_TempLong = 0;
            Read_Temp.clear();
			free(Read_TempP);
        }
    }
	
}
// CRC MODBUS 效验
// 输入参数: pDataIn: 数据地址
//           iLenIn: 数据长度
// 输出参数: pCRCOut: 2字节校验值
void Modbus_CustomClass::McMBCRC16(unsigned char *pDataIn, int iLenIn, unsigned int *pCRCOut){
    UCHAR ucCRCHi = 0xFF;
    UCHAR ucCRCLo = 0xFF;
    int iIndex;
    while(iLenIn-- )
    {
        iIndex = ucCRCLo ^ *( pDataIn++ );
        ucCRCLo = ( UCHAR )( ucCRCHi ^ aucCRCHi[iIndex] );
        ucCRCHi = aucCRCLo[iIndex];
    }
    *pCRCOut = ( ucCRCHi << 8 | ucCRCLo );
}
void Modbus_CustomClass::Request(unsigned char id, unsigned int address, unsigned short data)
{
    //if(m_serial_enable==0)return;
    unsigned char SenData[8];
    unsigned int CrcTemp;
    SenData[0]= id;
    SenData[1]= 06;
    SenData[2]= address>>8;
    SenData[3]= address&0xFF;
    SenData[4]= data>>8;
    SenData[5]= data&0xFF;
    McMBCRC16(SenData,6,&CrcTemp);
    SenData[6]= CrcTemp&0xFF;
    SenData[7]= CrcTemp>>8;
    m_modbusSerial->write((const char *)SenData,8);
}
void Modbus_CustomClass::Request16(unsigned char id, unsigned int address, int dLong, unsigned short *data){
    if(m_serial_enable == 0) return;
	unsigned char * SenData = (unsigned char*)malloc(sizeof(unsigned char) * 9 + dLong * 2);
    
    unsigned int CrcTemp;
    int i;

    SenData[0]=id;
    SenData[1]=16;
    SenData[2]= address>>8;
    SenData[3]= address&0xFF;

    SenData[4]= dLong>>8;
    SenData[5]= dLong&0xFF;

    SenData[6]= dLong*2;

    for(i=0;i<dLong;i++){
        SenData[7+2*i]=data[i]>>8;
        SenData[8+2*i]=data[i]&0xFF;
    }

    McMBCRC16(SenData,7+dLong*2,&CrcTemp);
    SenData[7+dLong*2]=CrcTemp&0xFF;
    SenData[8+dLong*2]=CrcTemp>>8;
    m_modbusSerial->write((const char *)SenData,9+dLong*2);
	free(SenData);
}








