#ifndef INSTRUMENTSERIALPORTMESSAGE_H
#define INSTRUMENTSERIALPORTMESSAGE_H

#include "displaychanneldata.h"
#include "test_module_class.h"
#include <QObject>
#include <QMutexLocker>
#include <QTimerEvent>
#include <suoweiserialport.h>
#include <testing.h>
#include <customcreatsql.h>
#include "QsLog/include/QsLog.h"
#include "cglobal.h"

class InstrumentSerialPortMessage : public QObject
{
    Q_OBJECT
public:
    explicit InstrumentSerialPortMessage(QObject *parent = nullptr);
    ~InstrumentSerialPortMessage();
protected:
    void timerEvent(QTimerEvent *event) override;
signals:





    void  LoserCleanReagent(double ); //清洗剂减少


private:
    QMutex m_lock;


public slots:


    void  ControlWhichoneChannelrevolving(const int Channel ,const bool Openrevolving);










};

#endif // INSTRUMENTSERIALPORTMESSAGE_H
