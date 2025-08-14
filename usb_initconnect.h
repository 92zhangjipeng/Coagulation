#ifndef USB_INITCONNECT_H
#define USB_INITCONNECT_H

#include <QObject>
#include <QReadWriteLock>
#include "cglobal.h"

#include <QThread>

using namespace std;

typedef struct EquipmentActive
{
    quint8     Command_number;
    QByteArray CommamdArry;
    QStringList ActionCode;
    bool CompletionStatus;  //完成状态
    int  ACtionType;     //动作类别
}EquipmentActive_;
typedef QVector<EquipmentActive_ * > ActionVec_;


class USB_InitConnect : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit USB_InitConnect(QObject *parent = nullptr);
    ~USB_InitConnect();

    void _startthread();

public slots:

	void _pthreadstart();

    void Recv_serialdata(const QStringList commandData); //接收的串口消息

    void slotCeratActionDate(int ACtionType, const QByteArrayList groupActions);

    void slotDisconnectandreconnect(); //断线重连
signals:

    void writeCommand(const QByteArray,QString);         //发数据
    void bootInitCleanFinished();                       //开机初始化清洗完成
    void CleaningProgress(quint8 index,quint8 total);   //清洗进度
    void btn_resetComplete();                           //按钮复位
    void btn_CleanComplete();                           //按钮清洗完成
    void CoordinatefinetuningactionFinish();          //坐标微调动作

    void finishviewdownheigh(int ); //测试调整高度完成
private:

    //数据排序
    void dataSort();

    void deleteFinishCommand(quint8 indexcode);

    bool ProcessEquipmentActions(const ActionVec_ &actions);

    //执行的动作
    bool ActionsPerformed(const int indexActive, QString &aboutActive);

    void CompletedActions(const int indexActive); //完成的动作
    void sendTaskHeader(); //发送首个任务字节
private:
    ActionVec_* m_pActionVec = nullptr;
    const int comd_num =  2;    //命令编号
    int m_runingAction = EQUIPMENT_FREETIME;  //执行的动作

};

#endif // USB_INITCONNECT_H
