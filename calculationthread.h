#ifndef CALCULATIONTHREAD_H
#define CALCULATIONTHREAD_H

#include <QMap>
#include <QObject>
#include <QSerialPort>
#include <QTimer>
#include <QVector>


#define TIME_OUT_READ_MACHINE_STATE  1000
#define ALL_PASSAGE_NUMBER  12

class CalculationThread : public QObject
{
    Q_OBJECT
public:
    explicit CalculationThread(QObject *parent = nullptr);
    ~CalculationThread();

public:
    //void Clear_PassageData(int witch_passage,bool all); //清除通道数据
signals:

    void Show_PassageTemp(QList<QString> ); //测试通道温度

    void SendTestModulData(QMap<quint8,quint32>);



public slots:
    void Connect_state(bool);


private slots:
	void onTime_Send_code();

private slots:

private:
     QTimer *m_time_send = nullptr;
     QList<QString> m_Readed_Temp;//温度


signals:

};

#endif // CALCULATIONTHREAD_H
