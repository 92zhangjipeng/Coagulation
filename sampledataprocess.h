#ifndef SAMPLEDATAPROCESS_H
#define SAMPLEDATAPROCESS_H

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QSqlDatabase>
#include <QThread>
#include <QTimer>
#include "machinetaskassignment.h"


#define WAITADDREAGENT     0		//已移动到测试通道等待添加试剂
#define ADDEDREAGENT       1		//试剂已添加
#define ADDEDREAGENTAND_CLEANING  2 //试剂加完并清洗了

#define TOTALDATANUM        300    //數據個數

#define   WASH_ALL_NEDDLE        0
#define   WASH_BLOOD_NEDDLE      1
#define   WASH_REAGENT_NEDDLE    2


class SampleDataProcess : public QObject
{
    Q_OBJECT
public:
    explicit SampleDataProcess(QObject *parent = nullptr);
	~SampleDataProcess();
private slots:

signals:

    bool            GroupWorkAction(QByteArrayList code);
	void            SenderTestChannelInfo(quint8,QByteArrayList);




public slots:




private:
    


public slots:


public:

private:

    QMutex          mutex;



};

#endif // SAMPLEDATAPROCESS_H
