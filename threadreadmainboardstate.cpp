#include "threadreadmainboardstate.h"
#include "testing.h"
#include "suoweiserialport.h"

ThreadReadMainBoardState::ThreadReadMainBoardState(QObject *parent) : QObject(parent)
{

}

ThreadReadMainBoardState::~ThreadReadMainBoardState()
{
    if(m_timer)
    {
        m_timer->stop();
        delete  m_timer;
        m_timer = nullptr;
    }
}


void ThreadReadMainBoardState::onTimeout()
{
   //连接状态&&获取主板状态 ==true 发送
   if(cglobal::gserialConnecStatus)
   {
	   QString info = "连接状态&&获取主板状态";
       emit this->ReadMachineState(m_readMainBoard,info);
   }
}

void ThreadReadMainBoardState::recv_beginReadSuppliesAllowance()
{
    QLOG_DEBUG()<<"读主板信息线程ID:"<<QThread::currentThreadId();
    QUIUtils::Traverse_the_motherboard(m_readMainBoard);
    m_timer = new QTimer();
    m_timer->setInterval(1000);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout())); //一秒读取读取主板信息
    m_timer->start();
}

void ThreadReadMainBoardState::control_read_state(bool _stop)
{
    //m_timer->isActive()
    if(!_stop )
        m_timer->stop();
    else
       m_timer->start();
    return;
}
