#include "basicclasschannel.h"

BasicClassChannel::BasicClassChannel(QObject *parent) : QObject(parent)
{

}
BasicClassChannel::~BasicClassChannel(){

}

//通道状态
void BasicClassChannel::SetChannelRunning()
{

}

//获取通道状态
bool BasicClassChannel::GetChannelRunning(){
    return false;
}

void BasicClassChannel::Synchronize_Sample(const QString num,const QString name, const QString barcode)
{
    qDebug()<<num<<name<<barcode;
}

void BasicClassChannel::ExitChannel()
{

}

void BasicClassChannel::TestingRuuningValue(const double)
{

}

void BasicClassChannel::WillTestReagentType(const QList<int> )
{

}
