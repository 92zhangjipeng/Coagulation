#pragma execution_character_set("utf-8")

#include "machinereadrunnable.h"
#include "cglobal.h"
#include <QElapsedTimer>
#include <QDir>
#include "ConnectionUtil.h"
#include "loadequipmentpos.h"


QMutex  mlockmutex;

MachineReadRunnable::MachineReadRunnable(QObject *parent) : QObject(parent)
{

}

MachineReadRunnable::~MachineReadRunnable()
{
    
}









/////////////////////模组线程 /////////////////////////////
/// \brief ReadModulInfo::ReadModulInfo
/// \param id
/// \param parent
///
ReadModulInfo::ReadModulInfo(int  id, QObject *parent) : QObject(parent)
{
    mstop_ = false;
    m_idthread = id; 
}
ReadModulInfo::~ReadModulInfo()
{
    QLOG_DEBUG()<<"析构线程池模组线程ID:"<<m_idthread;
}

void ReadModulInfo::run()
{
    while(!mstop_)
    {
        pauseLock.lock();
        if(m_modulcode.size() == 3)
        {
           emit ReadMachineState(m_modulcode,"获取模组数据");
//           for(int i = 0 ;i <m_modulcode.size(); i++)
//           {
//               QString framedata  = m_modulcode.at(i).toHex(' ').trimmed().toUpper();
//               QLOG_DEBUG()<<"读取模组数据："<< framedata<<endl;
//           }
        }
        QThread::msleep(100);
        pauseLock.unlock();
		if (mstop_)
				break;
    }
}

void ReadModulInfo::sycnpSerialPort(SuoweiSerialPort *pSerialpotr)
{
    m_SerialPort = pSerialpotr;
    connect(this, &ReadModulInfo::ReadMachineState,
            m_SerialPort, &SuoweiSerialPort::slotwrite_instructions_group);
    quint8 equipmentKind = 0;
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmentKind);
    m_modulcode.clear();
    m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(0, 0, 0, false));
    switch(equipmentKind)
    {
        case KS600:
        break;
        case KS800:
            m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(1, 0, 0, false));
        break;
        case KS1200:
            m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(1, 0, 0, false));
            m_modulcode.push_back(Testing::m_TaskDll->_SetTestModeInfo(2, 0, 0, false));
        break;
    }

}

QByteArray intToByte(int leng,int number)
{
    QByteArray abyte0;

    abyte0.resize(leng);

    if(leng == 1)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
    }
    else if(leng == 2)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
        abyte0[1] = (uchar) ((0x000000ff & number) >> 8);
    }
    else if(leng == 4)
    {
        abyte0[0] = (uchar) (0x000000ff & number);
        abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
        abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
        abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
    }

    return abyte0;
}




void ReadModulInfo::StopThread()
{
	mstop_ = true;
  
}


///////////////////////////发送机器运动命令线程/////////////////////////////////////
/// \brief SendMachineSportInfo::SendMachineSportInfo
/// \param id
/// \param parent
///
SendMachineSportInfo::SendMachineSportInfo(int  id, QObject *parent) : QObject(parent)
{
    m_idthread = id;
    m_cachecode.clear();
}
SendMachineSportInfo::~SendMachineSportInfo()
{
    QLOG_TRACE()<<"退出发送机器命令线程ID:"<<m_idthread;
}
void SendMachineSportInfo::sycnSerialclass(SuoweiSerialPort * Port)
{
    if(m_SerialPort_) return;
    if(m_SerialPort_ == nullptr)
    {
        m_SerialPort_ = Port;
        connect(this, &SendMachineSportInfo::sendMachineState,
                m_SerialPort_, &SuoweiSerialPort::slotwrite_instructions_group);
    }
    return;
}

void SendMachineSportInfo::willSendcode(const QByteArrayList codearry, const bool /*Delaytime*/)
{
    QMutexLocker locker(&mlockmutex);
    m_machinespotr = codearry;
    return;
}


void SendMachineSportInfo::run()
{
    QByteArrayList SentCommand;
    SentCommand.clear();
    if( m_cachecode.isEmpty())
    {
        SentCommand = m_machinespotr;
        emit sendMachineState(SentCommand,"未定义指令");
        QThread::msleep(10);
    }
    else if(!m_cachecode.isEmpty())
    {
        SentCommand = m_cachecode.dequeue();
        emit sendMachineState(SentCommand,"未定义指令"); //出队
        QThread::msleep(10);
        m_cachecode.enqueue(m_machinespotr);
    }

}





///////////////////////////////配置机器参数//////////////////////////////////////////////
ConfigureWriteParameter::ConfigureWriteParameter(int id, QObject *parent) : QObject(parent)
{
    m_id = id;
    m_DelayedMS = 0;
    m_bNeedDelayed = false;
    m_BeginChannel = 0;

    m_machinespotr.clear();
}

ConfigureWriteParameter::~ConfigureWriteParameter()
{
    QLOG_DEBUG()<<"退出发送机器命令线程ID:"<<m_id;
}

QStringList Char2String(char *data, int size)
{
    QByteArray byArr;
    QString back_data;
    byArr.resize(size);
    for (int i = 0; i<size; i++)
        byArr[i] = data[i];
    QDataStream out(&byArr, QIODevice::ReadWrite);
    while (!out.atEnd())
    {
        qint8 outchar = 0;
        out >> outchar;
        QString str = QString("%1").arg(outchar & 0xFF, 2, 16, QLatin1Char('0')).toUpper() + QString((" "));
        back_data += str;
    }
    back_data.simplified();
    QStringList outlist = back_data.split(" ");
    if (outlist.last().isEmpty())
        outlist.removeLast();
    return outlist;
}

void bytearry_2_String(QByteArray arry,QStringList &Machinedata){

    char buf[PROTOCOL_LENGTH];//数组
    int len_array = arry.size();
    int len_buf = sizeof(buf);
    int len = qMin(len_array, len_buf);
    memcpy(buf, arry, len);// 转化
    Machinedata = Char2String(buf, PROTOCOL_LENGTH);
    return;
}
void OutPrintSendCode(QByteArrayList Shaparry)
{
    QStringList OutPrintcode;
    OutPrintcode.clear();
    auto iter = Shaparry.constBegin();
    while (iter != Shaparry.constEnd())
    {
        bytearry_2_String(*iter, OutPrintcode);
        qDebug() << ("发送到机器命令:") << QThread::currentThread() << "ID" << OutPrintcode;
        iter++;
    }
    return;
}

void ConfigureWriteParameter::Delay_MSec_Suspend(unsigned int msec)
{
    qDebug()<<msec;
    emit WriteMachineAction(m_machinespotr);

    m_machinespotr.clear();

}

void ConfigureWriteParameter::run()
{

    QMutexLocker locker(&mlockmutex);
    if (!m_machinespotr.isEmpty() && m_machinespotr.size() >= 1)
    {
        if(m_bNeedDelayed)
        {
            Delay_MSec_Suspend(m_DelayedMS); //死等
        }
        else
        {
            QThread::msleep(1);
            emit WriteMachineAction(m_machinespotr);
            m_machinespotr.clear();
        }
    }
}

/*开始测试后机器运动发送数据终端*/
void ConfigureWriteParameter::sycnSerialclass(SuoweiSerialPort * port)
{
    if(Serial_port) return;
    if(Serial_port == nullptr)
    {
        Serial_port = port;
        connect(this, &ConfigureWriteParameter::WriteMachineAction, Serial_port, &SuoweiSerialPort::write_data);
    }
    return;
}

void ConfigureWriteParameter::WriteCodearray(QByteArrayList arraycode)
{
    QMutexLocker locker(&mlockmutex);
    m_machinespotr = arraycode;
    m_DelayedMS = 0;
    m_bNeedDelayed = false;
    return;
}

void ConfigureWriteParameter::slotDelayed_sending(const QByteArrayList arraycode,int delayedmS, bool needDelayed )
{
    QMutexLocker locker(&mlockmutex);
    m_machinespotr = arraycode;
    m_DelayedMS = delayedmS;
    m_bNeedDelayed = needDelayed;
    return;
}

void ConfigureWriteParameter::slotWhileReadChannelParam(const int Fromchannel)
{
    QMutexLocker locker(&mlockmutex);
    m_BeginChannel = Fromchannel;
    return;
}
