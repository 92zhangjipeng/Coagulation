#ifndef PASSPARAMETERINTERFACE_H
#define PASSPARAMETERINTERFACE_H

#pragma once
#include <QString>
#include <QStringList>

class PassparameterInterface
{
public:
    PassparameterInterface();
    ~PassparameterInterface();

    virtual void displayPara(int, const double); //显示模组温度数值
    virtual void ChannelValueshow(QStringList);    //显示模组通道数据
    virtual void PromptInfo(const quint8, const QString, const quint8);

};

#endif // PASSPARAMETERINTERFACE_H
