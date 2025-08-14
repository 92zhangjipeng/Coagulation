#ifndef DOSOMETINGTHREAD_H
#define DOSOMETINGTHREAD_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QThread>
#include <qDebug>
#include <qDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTimer>
#include "crc/JQChecksum.h"
#include "QSound"
#include <QVariant>
#include <QMetaType>
#include <QVariantList>
#include <QMetaType>
#include "cglobal.h"
#include <QMutex>
#include <test_module_class.h>


typedef struct Channel_data
{
   QList<int> channel_num;
   QMap<int,int> reagent_num;
   QMap<int,QString> Sample_num;
   QMap<int,QVector<double> > channe_data;
   Channel_data()
   {
       channel_num.clear();
       reagent_num.clear();
       Sample_num.clear();
       channe_data.clear();
   }

}Channel_data;

Q_DECLARE_METATYPE(Channel_data)



#define SEND_ID  0x00
#define SEND_FUNCTION_STEP_1 0x01

#define PROTOCOL_START_CODE  0xCC //协议起始码
#define PROTOCOL_DEVICE_CODE 0xA5 //协议设备码
#define AGREEMENT_END_CODE   0x5A //协议结束码

#define PULSE_2_MM_XY   64
#define PULSE_2_MM_Z    150


#define TIMED_INTERACTIVE_DATA 100  //定时交互消息

class DoSometingThread : public QObject
{
    Q_OBJECT
public:
    explicit DoSometingThread(QObject *parent = nullptr);
    ~DoSometingThread();

   
public: //CRC 校验
    static void GetCRC(quint8 *data, int len, quint16 &crc);
    static bool CheckCRC(quint8 *data, int len);
    QString Crc_add_Data_Get(QString,int); //数据+校验嘛
public:
    void Init_Seiral_Port(); //初始化串口

signals:







public slots:
    










	//modbus
	bool RecveDataCRC(QString temp_data); //收到数据校验CRC码

	void Int2HexString(int ID, int Fun, QStringList &ID_Fun);//要发送得从ID 和功能码 转换16进制
	QString Int_2_Hex_Data(QString data_str);


    void Task_Send_data_step_2(QString); //步骤2





    

private slots:

     void ReadData_Da();
	 void SerialRead();
	 void handleError(QSerialPort::SerialPortError);
private:
    
  
private:
    QSerialPort  *m_Serialport = nullptr;

    QTimer *m_timer = nullptr;
    QByteArray m_SendDataBuf;
	QMutex m_mutex;
	QList<QByteArrayList> m_Temp_AllData; //温度数据
	QMap<QString,int> m_TEMP;
    QString m_PortName;
    QStringList m_IDFucCode; //需要校验得 ID 跟 功能码
    /*要发送的数据  长度 */
    QByteArray m_data_array_send;
    int m_len_data_send;


	bool m_balready_connect; //已经连接上了串口有接收到数据，就不发送连接标志到主界面

   
    QString m_test_module_need_put;
    QString m_Reagent_num;
    QString m_name_code;
    Channel_data m_channel_data_struct;
	
};

#endif // DOSOMETINGTHREAD_H
