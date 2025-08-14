#pragma execution_character_set("utf-8")
#include <QApplication>
#include <QDebug>
#include "functionclass.h"
#include "QsLog/include/QsLog.h"


functionclass * functionclass::m_pInstance(NULL);
ParamListVec paramListVec;

functionclass::functionclass()
{

}

functionclass::~functionclass()
{

}

functionclass * functionclass::GetInstance()
{
    if(NULL == m_pInstance)
    {
        static QMutex mutex;
        mutex.lock();
        if( NULL == m_pInstance)
        {
            static functionclass myclass;
            m_pInstance = &myclass;
        }
        mutex.unlock();
    }
    return m_pInstance;
}

void functionclass::startQualityTest(QVector<int> qualityChannel)
{

}
