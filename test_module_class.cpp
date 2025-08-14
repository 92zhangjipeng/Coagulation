#include "test_module_class.h"
#include <QThread>
#include <qDebug>
#include "ini_file.h"
#include "crc/vld.h"
#include "QsLog/include/QsLog.h"


Test_Module_Class::Test_Module_Class(QObject *parent) : QObject(parent)
{
    mMachineTestChannel = new TotalChannelStatusVec;
    for(int totalChannel = 0 ; totalChannel < MACHINE_SETTING_CHANNEL; totalChannel++)
    {
        //std::shared_ptr<SingleChannelStatus> ChannelInfo = std::make_shared<SingleChannelStatus>();
        SingleChannelStatus* ChannelInfo = new SingleChannelStatus;
        ChannelInfo->IndexChannel = totalChannel +1;
        ChannelInfo->bChannelState = !INI_File().rConfigPara(QString("TestTheChanne1Opening_%1").arg(totalChannel + 1)).toBool();
        ChannelInfo->BindSample = "";
        ChannelInfo->TestingReagent = 0;
        mMachineTestChannel->push_back(ChannelInfo);
    }
}

Test_Module_Class::~Test_Module_Class()
{
    auto iter = mMachineTestChannel->begin();
    while(iter != mMachineTestChannel->end())
    {
        if((*iter)!= nullptr)
        {
            delete (*iter);
            (*iter) = nullptr;
        }
        iter++;
    }
    mMachineTestChannel->clear();
    QVector<SingleChannelStatus* >().swap(*mMachineTestChannel);
    if(mMachineTestChannel != nullptr)
    {
        delete mMachineTestChannel;
        mMachineTestChannel = nullptr;
    }
}







