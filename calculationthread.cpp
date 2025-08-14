#include "calculationthread.h"
#include <QDebug>
#include <QThread>
#include<QMessageBox>
#include "cglobal.h"

/************************************************************************************
*                                                                                   *
*                             定时读取机器状态线程类                                *
*                                                                                   *
*************************************************************************************/
CalculationThread::CalculationThread(QObject *parent) : QObject(parent)
  //,m_Portconnect(false)
{
    //qDebug()<<"Threa线程接收机器状态构造函数ID:"<<QThread::currentThreadId();
    m_Readed_Temp.clear();

}

CalculationThread::~CalculationThread()
{
	if (m_time_send != nullptr) {
		if (m_time_send->isActive()){
			m_time_send->stop();
			delete m_time_send;
			m_time_send = NULL;
		}
	}	
}



void CalculationThread::Connect_state(bool connect_Ser)
{
	//connect_Ser == false 表示已连接串口
	if(connect_Ser == false){
		m_time_send = new QTimer();
        connect(m_time_send, SIGNAL(timeout()), this, SLOT(onTime_Send_code()),Qt::DirectConnection);
        m_time_send->start(TIME_OUT_READ_MACHINE_STATE);//,获取机器状态
	}
    qDebug() << "定时发送Code线程 ==thread: " << QThread::currentThread();
    return;
}

void CalculationThread::onTime_Send_code()
{

    return;
}



