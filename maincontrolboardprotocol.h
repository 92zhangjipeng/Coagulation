#ifndef MAINCONTROLBOARDPROTOCOL_H
#define MAINCONTROLBOARDPROTOCOL_H

#include <QObject>
#include <QThread>
#include "cglobal.h"
#include "passparameterinterface.h"
#include <QtEndian>


// 状态枚举定义
enum CommandType {
    CMD_WRITE = 0,
    CMD_READ = 1
};

enum CommandStatus {
    STATUS_PENDING_B = 0,
    STATUS_RUNNING = 1,
    STATUS_COMPLETED = 2,
    STATUS_ERROR = 3
};

// 解析结果结构体
struct DeviceStatus {
    CommandType type;
    CommandStatus cmdStatus;
    bool isCleanNormal; // true=正常，false=清洗液不足
    bool cup1Present;   // true=正常存在
    bool cup2Present;
    bool cup3Present;
    bool cup4Present;
};



class mainControlBoardProtocol : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit mainControlBoardProtocol(QObject *parent = nullptr);
    ~mainControlBoardProtocol();

    void _start(quint8 Equipment);

    void  setArgInterface(PassparameterInterface* pInterface);

 private slots:
    void _threadstart();

signals:
    void  equipmentReminder(const int );
    void  MainBoardHeightTrigger(); //触发开关


    /** 刷卡提示
     * @brief cardSwipeSuccessful
     * @param tips    提示内容
     * @param indexReagent 刷卡试剂编号
     * @param totalnum     刷卡量
     * @param datetime     有效期
     */
    void cardSwipeSuccessful(const QString tips,quint8 indexReagent,quint8 totalnum,quint16 datetime);

    /** 刷卡写入成功
     * @brief swipeCardSuccessfullyWritten
     * @param tips   提示
     * @param indexReagent 刷卡试剂索引
     * @param addBottle    写入瓶数
     */
    void  swipeCardSuccessfullyWritten(QString tips,quint8 indexReagent,quint8 addBottle);

    //刷卡异常提示
    void  Abnormalcardswipingprompt(const QString);


    void  _reminderErrorInfo(quint8 ,const QString& errinfo); //提示异常
    void  _normaloper(quint8); //耗材正常操作
public slots:
    void recvmainControlBoardProtocol(const QStringList mainControlBoardData);
    void RecvUsedBuzzerMark(const bool BuzzerStae);


private:

   /*测高触发*/
   void TriggerAltimetryState(const int bitAltimetryHex);
   void triggerHeightEvent();
   //void TriggerAltimemetryState(const int BitAltimemetryHex);

   //检测抓板耗材刷卡状态
   void configreadCardflag(); //修改读卡状态标志
   void DetectCardStatusofScratchingBoardConsumables(quint8 mainControlBoardData_suppileBit,
                                                        quint8 mainControlBoardData_suppileTotal,
                                                        quint16 suppileDate);

   DeviceStatus parseDeviceStatus(quint8 statusByte);
   //试管盘和S2清洗液
   void th4mainControlBoardData(const DeviceStatus &status);

   void Waste_detection(const int WastLinqueHex);  /*废液检测*/
   void Detect_Externalcleaningfluid(const QStringList Boardmessages); /*主板第4个字节状态*/

   /** S2 清洗液状态
    * @brief triggeredOutCleanInfo
    * @param ctrigger
    */
   void triggeredOutCleanInfo(const bool ctrigger);


   void triggeredTary_1_info(const bool isTriggered);

   void triggeredTary_2_info(const bool ctrigger);
   void triggeredTary_3_info(const bool ctrigger);
   void triggeredTary_4_info(const bool ctrigger);

private:
    const quint8 IndexByte_4 = 4;
    const quint8 IndexByte_5 = 5;
    const quint8 IndexByte_8 = 8;
    const quint8 IndexByte_13 = 13;
    const quint8 IndexByte_14 = 14;

    const quint8 SwitchLength = 8;
    const quint8 SwitchBinary = 2;
    bool  binitCreatSignals;
    quint8 mEquipment;
    bool m_TriggerTestHeight = false;         //触发测高
    bool m_BuzzerState;                      //启用蜂鸣器状态

    bool m_btheWasteTankIsFull = false;     /*废液罐满状态*/

    bool m_boutsideCleanLinqueNull = false; /*外部清洗液已空*/

    bool m_isTray1Normal    = false; /*空使馆盘状态*/
    bool m_bEmptyCupTrayIINormal   = false;
    bool m_bEmptyCupTrayIIINormal  = false;
    bool m_bEmptyCupTrayIVNormal = false;

    PassparameterInterface *m_parameterInterface = nullptr;

    bool mreadCardEvent; //读卡状态标志

};

#endif // MAINCONTROLBOARDPROTOCOL_H
