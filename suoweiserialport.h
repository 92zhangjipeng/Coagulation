#ifndef SUOWEISERIALPORT_H
#define SUOWEISERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QObject>
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QMessagebox>
#include <QFuture>
#include "testing.h"
#include "quiutils.h"
#include "thetestmoduleprotocol.h"      //测试模组消息解析线程
#include "mainControlBoardProtocol.h"   //主控板消息解析线程
#include "loadequipmentpos.h"


namespace ProtocolIndex {
    constexpr int CMD_NUM = 2;
    constexpr int REAGENT_LIMIT_START = 5;
    constexpr int REAGENT_LIMIT_COUNT = 5;
    constexpr int TUBE_LIMIT = 10;
    constexpr int CLEAN_LIMIT = 11;
    constexpr int CLEAN_TOTAL_LOW = 12;
    constexpr int CLEAN_TOTAL_HIGH = 13;
    constexpr int REAGENT_TOTAL_START = 5;
}

//仪器校准命令
typedef struct InstrumentCalibration
{
    QByteArray CommamdArry;
    QStringList ActionCode;
    bool Finished;
    InstrumentCalibration()
    {
        CommamdArry.clear();
        ActionCode.clear();
        Finished = false;
    }
}InstrumentCalibration;
typedef QVector<InstrumentCalibration* > InstrumentCalibration_Vec;

//仪器按键命令
typedef struct InstrumentReset
{
    QByteArray CommamdArry;
    QStringList ActionCode;
    bool Finished;
    int Activeindex;
    InstrumentReset()
    {
        CommamdArry.clear();
        ActionCode.clear();
        Finished = false;
        Activeindex = -1;
    }
}InstrumentReset;
typedef QVector<InstrumentReset* > InstrumentReset_Vec;


//#pragma pack(0)

class SuoweiSerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SuoweiSerialPort(QObject *parent = nullptr);
    ~SuoweiSerialPort();

    void suoweiportthreadbegin();

	void closePort();

private slots:
    void  startSerialthread();

public slots:

    void slotcloseseirport();


    /**@brief 解析模组数据*/
    void    slotReadData();

signals:
    void openequipmentCleanActive(); //连接成功开机清洗

    void writeRequested(const QByteArray &data, const QString &kindCommand);

    void moduleprotocolData(const int Slave_addr,const QStringList moduleData); /*模组数据*/

    void mainControlBoardProtocolData(const QStringList moduleData);           /*主控消息*/

    void ConsumableswriteState(QStringList moduleData); //耗材写入状态

    void outRminderWidget(QString,QString);//调用提示框提示信息

    void connectionStateChanged(bool);  //串口开关状态主界面显示
    void prepareReconnectGetData();

    void  serialrecvedata(const QStringList);  //接收消息逐条解析

    //性能验证消息
    void  serialPEdata(const QStringList& );

    /*命令执行异常报告*/
    void Execute_exception_prompt(const quint8 Index, const QString, const quint8 );

    void backHandssuck(bool,int);

    void outArmText(quint8 kinds,const QString detailedStr);


private slots:
   
    //测试托盘消息
    void    TestMoveTrayTube_recv(const QStringList backArry);

    /** 分析仪器校准动作
    * @brief ResolutionCalibrationOperation
    */
    void    ResolutionCalibrationOperation(const QStringList SportBuffer);

    /**解析析仪器按键动作*/
    void     ResolutionResetOperation(const QStringList SportBuffer);
    bool     FinishedActive(InstrumentReset_Vec *pStu);

    void     handleBytesWritten(qint64 bytes);  // 写入回调

private:
    bool    findAndValidateSerialPort();  //找到串口pid vip
    bool    openSerialPort(const QString portname);
    void    applySerialPortConfig(const QString &portname);
    void    handleOpenError(const QString &errorMsg);
    void    updateGlobalConnectionStatus(bool status);

    void    safeClosePort();

    /** 清除仪器校准命令
    * @brief DeleteInstrumentCalibrationCode
    */
    void   DeleteInstrumentCalibrationCode();

    /** 清除仪器复位命令
    * @brief DeleteInstrumentResetCode
    */
    void  DeleteInstrumentResetCode();

    //存贮的参数
    void StoredParameters(const QStringList& recvdata);
    void updateReagentLimit(const QStringList& data, int index,
                            int reagentType, ConsumablesOper* consumables);
    quint16 parseHexPair(const QStringList& data, int highIndex,
                         int lowIndex, bool& convertOk);
    void handleCmd3(const QStringList& recvdata, ConsumablesOper* consumables);
    void handleCmd5(const QStringList& recvdata, ConsumablesOper* consumables);

    void handleRecvdata();
	void processFrame(const QByteArray& frame);

    void clearWriteQueue();

public slots:
    void handleSerialError(QSerialPort::SerialPortError error);

     //断线重连
    void prepareReconnect();

    //断线关闭串口
    void recvdisConnectCloseSerial();

    void slotwrite_instructions_group(const QByteArrayList &dataList,
                                         const QString &info);

    /*发送数据*/
    void write_data(const QByteArrayList );

    /**
    * @brief writedataToEquipment  发送命令到仪器
    * @param arry                  发送数据
    * @param kindCommad            发送的功能描述
    */
    void writedataToEquipment(const QByteArray arry, QString kindCommad);
    void processWriteQueue();    // 新增队列处理函数

    //测试托盘命令
    void Recv_TrayMoveTest(const QByteArrayList,const bool,const quint8);

private:

    //解析返回抓手的负压值
    void    toReadSuckAirsValBack(const QStringList &recv_data);

    //实例化串口并打开
    void    InstancingSerialandOpen(const bool initconnect);

    //解析到电机运动状态
    int     Recve_Motor_motion_state(const QStringList ,const int);

    //解析运动到的位置
    int     ParseSportLocation(const QStringList recv_data ,const int Index);

    //重连恢复测试
    void    prepareReconnectTesting();

    void cleanupSerialPort();
    QString getSerialPortErrorString(QSerialPort* port);
    void  handleInitializationFailure(const QString& error);

    // 端口验证
    bool isPortValid() const;

    //仪器校准
    InstrumentCalibration_Vec *mpInstrumentCalibration = nullptr;

    //仪器复位
    InstrumentReset_Vec *mpInstrumentReset = nullptr;


signals:

    //继续发送托盘运动测试命令
    void continueSendArryTray();

signals:
    void  Disconnectandreconnect();  //断线从连继续任务

    void  sendTestingRecvData(const QStringList); //测试时接收的信号数据

    void  connectEquipmentagin();  //重连分析任务

    void  BufferFullState(int _addr);  //提示缓存满

private:
    void Parsing_received_messages(const QStringList); //解析收到的消息

    bool _bufferfull(const int buffer_num, const int _addr); //命令内存状态

    bool processModuleInfo(int moduleAddress, const QStringList &receivedData); //模组数据

    bool _machinesportinfo(const int _addr,const int func_code,const QStringList _recvdata);

    //仪器运动
    bool processMachineData(int deviceAddress,
                             int functionCode,
                             const QStringList& receivedData);

    bool processMainBoardInfo(int deviceAddress,
                        int functionCode,
                        const QStringList &receivedData); //主板

private:
    QMutex mutex;
	QThread m_thread;
    QMutex m_serialMutex;
    QMutex m_bufferMutex;


    QSerialPort *mserialPort = nullptr;

    QByteArray m_buffer;
    QThreadPool *m_threadPool;             // 解析线程池
    const int  MAX_BUFFER_SIZE = 15*256;     // 最大缓存限制


    const quint16 mprotocolLength = 15;

    bool mreminderbufferfull = false; //数据缓存满已提示

    bool mbCalibration = false;     //接收动作为仪器校准动作
    bool mbInstrumentreset = false; //复位&&清洗按钮


    bool mTestMoveTrayTube;
    quint8 mTotalCommd;
    quint8 mTestMoveTrayTubeFinished;



    bool m_errorSignalConnected = false;

    // 新增成员
    qint64 m_lastWriteTime = 0;  // 上一次写入完成时间
    QMutex m_writeMutex;         // 写入专用锁
    QQueue<QPair<QByteArray, QString>> m_writeQueue; // 写入队列
    QTimer *m_writeTimer;         // 写入调度定时器
    QMetaObject::Connection m_bytesWrittenConnection; // 持久化连接

};

#endif // SUOWEISERIALPORT_H
