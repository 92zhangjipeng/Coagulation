#pragma execution_character_set("utf-8")

#include "dosometingthread.h"
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include<QUrl>
#include<QTime>
#include<QMessageBox>
#include<QEventLoop>
#include<QCoreApplication>
#include<QThread>
#include <QTimer>
#include <string>

//#if defined(_MSC_VER) && (_MSC_VER >= 1900)
//#pragma execution_character_set("utf-8")
//#endif

DoSometingThread::DoSometingThread(QObject *parent) : QObject(parent)
{
    
	m_balready_connect = false;
    m_test_module_need_put ="";
    m_Reagent_num ="";
    m_name_code = "";

}
DoSometingThread::~DoSometingThread()
{
    if (m_Serialport->isOpen())
    {
		m_Serialport->close();
	}
    if (m_Serialport != nullptr)
    {
		delete m_Serialport;
		m_Serialport = nullptr;
	}
    if (m_timer)
    {
		delete m_timer;
		m_timer = nullptr;
	}
}


void DoSometingThread::Init_Seiral_Port()
{
    if(m_Serialport == nullptr)
    {
        m_Serialport  = new QSerialPort();
    }
    //如果串口已经打开了 先给他关闭了
    if (m_Serialport->isOpen())
    {
        m_Serialport->clear();
		m_Serialport->close();
	}
    m_Serialport->setPortName(m_PortName);
    bool bopen = m_Serialport->open(QIODevice::ReadWrite);//打开串口
    if (!bopen)
    {
		qDebug() << tr("串口打开失败!");
		return;
	}	
    if(m_Serialport->isOpen())
    {
        m_Serialport->setBaudRate(QSerialPort::Baud1000000);
		m_Serialport->setDataBits(QSerialPort::Data8);//设置数据位数
		m_Serialport->setParity(QSerialPort::NoParity); //设置奇偶校验
		m_Serialport->setStopBits(QSerialPort::OneStop);//设置停止位
		m_Serialport->setFlowControl(QSerialPort::NoFlowControl);  //设置流控制
	}
    m_timer = new QTimer();
    m_timer ->setInterval(TIMED_INTERACTIVE_DATA);
    connect(m_Serialport, SIGNAL(readyRead()),this,SLOT(ReadData_Da()),Qt::QueuedConnection);
    connect(m_Serialport, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),this, &DoSometingThread::handleError);//连接槽,串口出现问题连接到错误处理函数
    connect(m_timer, SIGNAL(timeout()), this, SLOT(SerialRead()),Qt::QueuedConnection);
	return;
}
void DoSometingThread::handleError(QSerialPort::SerialPortError error)
{
	if (error == QSerialPort::ResourceError) 
	{
		if (m_Serialport->isOpen())
			m_Serialport->close(); //关闭串口
	}
	return;
}
void DoSometingThread::ReadData_Da()
{
    m_timer->start();
    //m_RequestData.append(m_Serialport->readAll());
	//SerialRead(); 
	return;
}

void DoSometingThread::SerialRead( )
{
    //接收到数据,才改变标志为true
 //   cglobal::g_ConnectUSB = false;
 //   if (!m_balready_connect)
 //   {
	//	m_balready_connect = true;

	//}
 //   int sender_error = 0;
	//m_timer->stop();
 //   QString temp_data ="";
 //   if(m_RequestData.length() != 0)
 //   {
 //       QDataStream out(&m_RequestData,QIODevice::ReadWrite);
 //       while (!out.atEnd())
 //       {
 //           qint8 outchar = 0;
 //           out>>outchar;
 //           QString str = QString("%1").arg(outchar&0xFF,2,16,QLatin1Char('0')).toUpper() + QString(tr(" "));
 //           temp_data +=str;
 //       }
 //   }
 //   qDebug()<<QString("16进制接收字符:")<<temp_data;
	//m_RequestData.clear();

 //   /*机器状态信息 功能码 0A*/
 //   QStringList Info = temp_data.split(" ");
	//if (Info.at(1) == "0A") {

	//	return;
	//}
 //   if(Info.at(1) == "0B")
 //   {

 //   } 
 //   //判断校验是否正确
 //   if(RecveDataCRC(temp_data) == true)
 //   {



 //   }
 //   else
 //   {   /*接收不对在重新发送 最多5次*/
 //       if(sender_error < 5 )
 //       {
 //          m_Serialport->write(m_data_array_send, m_len_data_send);
 //          sender_error++;
 //       }
 //       else
 //       {
 //           qDebug() << tr("接收消息异常!");
 //       }

 //   }
 //   //qDebug() << QString("接收机器数据线程ID:") << QThread::currentThreadId();
 //   return;
}

////////////////////////////////////////////////////////////////////////////
//modbus  ---  ID fun 转16进制部分
void DoSometingThread::Int2HexString(int ID ,int Fun ,QStringList &ID_Fun)
{
	ID_Fun.clear();
	QString code_id;
	QString code_fuc;
	code_id = QString("%1").arg(ID, 4, 16, QLatin1Char('0'));//value为int型或char型都可 
	code_fuc = QString("%1").arg(Fun, 4, 16, QLatin1Char('0'));
	ID_Fun.append(code_id);
	ID_Fun.append(code_fuc);
	return;
}
QString DoSometingThread::Int_2_Hex_Data(QString data_str)
{
	QString back_str;
	back_str.clear();
    int value =  0 ;
	QString temp_data;
	QStringList result_str = data_str.split("-");
	for (int j = 0; j < result_str.size(); j++)
	{
        QString temp_= result_str.at(j);
		if (j == 1) {
			value = temp_.toInt(nullptr, 16);
		}    
		else {
			value = temp_.toInt(nullptr, 10);
		}
		temp_data = QString("%1").arg(value, 2, 16, QLatin1Char('0')).toUpper();
		back_str += temp_data;
	}
	return back_str;
}












void DoSometingThread::Task_Send_data_step_2(QString Data)
{
    if (!m_Serialport->isOpen()) return;
    QString send_data=  Int_2_Hex_Data(Data);
    QString DataAddCRC = Crc_add_Data_Get(send_data,2);
    //qDebug()<<"步骤二:"<<temp;
    bool OK;
    QByteArray data_array_send;
    data_array_send.clear();
    QMutexLocker m_lock(&m_mutex);
    int len_data_send = DataAddCRC.length() / 2;
    for (int i = 0; i<len_data_send; i++){
        QString code = DataAddCRC.mid(i * 2, 2);
        data_array_send[i] = code.toInt(&OK, 16);
    }
    m_Serialport->write(data_array_send, len_data_send);
    return;
}


/*--------------------CRC-------------------------------*/
bool DoSometingThread::CheckCRC(quint8 *data, int len)
{
    //检验CRC 时要把高地位掉回原来位置
    if (len < 2) return false;
    quint16 crcLow = data[len - 1];
    quint16 crcHigh = data[len - 2];
    quint16 crc_received = static_cast<quint16>(crcHigh << 8 | crcLow);
    quint16 crc_new;
    GetCRC(data, len - 2, crc_new);
    if (crc_new == crc_received) {
        return true;
    }
    return false;
}
//获取校验码
void DoSometingThread::GetCRC(quint8 *data, int len, quint16 &crc)
{
    quint16 i,j,carry_flag, temp;
    crc = 0xffff;
    for (i = 0; i < len; i++) {
        crc = crc ^ data[i];
        for (j = 0; j < 8; j ++) {
            temp = crc;
            carry_flag = temp & 0x0001;
            crc = crc >> 1;
            if (carry_flag == 1) {
                 crc = crc ^ 0xa001;
            }
         }
    }
}
QString DoSometingThread::Crc_add_Data_Get(QString data_str, int cut_pot)
{
    quint16 Crc_bit;
    Crc_bit = 0;
    bool OK;
    QString transData = data_str;
    int length = transData.length() / cut_pot;
    unsigned char * data = (unsigned char*)malloc(sizeof(unsigned char) * length);
    for (int i = 0; i<length; i++)
    {
        QString temp = transData.mid(i * cut_pot, cut_pot);
        data[i] = temp.toInt(&OK, 16);
    }
    GetCRC(data, length, Crc_bit);
    //保留四位,不足补零
    QString hex = QString("%1").arg(Crc_bit, 4, 16, QLatin1Char('0')).toUpper();
    QString H_crc = hex.mid(2, 2);
    QString L_crc = hex.mid(0, 2);
    QString Data = QString("%1%2%3").arg(transData).arg(H_crc).arg(L_crc);
    //qDebug() << tr("数据+生成得校验码:") << Data;
    free(data);
    return Data;
}
bool DoSometingThread::RecveDataCRC(QString data)
{
    bool ok;
    //int len_str;
    QStringList datalist;
    datalist.clear();
    datalist = data.split(" ");
    int len = datalist.size();
    if (NULL == datalist.at(len-1))
        datalist.removeLast();
    len = datalist.size();
    //校验码互换位置
    QString h_crc = datalist.at(len -1);
    QString l_crc = datalist.at(len -2);
    datalist.replace(len - 1,l_crc);
    datalist.replace(len - 2, h_crc);
    unsigned char * prt = (unsigned char*)malloc(sizeof( unsigned char) * len);
    for(int i = 0 ; i<len;i++)
    {
       QString temp;
       temp = datalist.at(i);
       prt[i] = temp.toInt(&ok,16);
    }
    bool b_Crc = false;
    //int ID = datalist.at(0).toInt();
    //int Fuc = datalist.at(1).toInt();
    b_Crc = CheckCRC(prt,len);
    //qDebug()<<tr("接收数据校验码是否正确:")<<b_Crc;
    free(prt);
    //if (m_IDFucCode.at(0).toInt() == ID && m_IDFucCode.at(1).toInt() == Fuc && b_Crc)
    return b_Crc;
    //else
        //return false;
}
