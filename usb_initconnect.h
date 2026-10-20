#ifndef USB_INITCONNECT_H
#define USB_INITCONNECT_H

#include <QObject>
#include <QThread>
#include <QDateTime>
#include "cglobal.h"
#include <vector>

typedef struct EquipmentActive
{
    quint8     Command_number;   ///< 命令编号
    QByteArray CommamdArry;      ///< 命令数据
    QStringList ActionCode;      ///< 动作代码列表
    bool       CompletionStatus; ///< 完成状态（true=已完成/失败，false=未完成）
    int        ACtionType;       ///< 动作类别
    QDateTime  lastSendTime;     ///< 最后发送时间（仅用于调试）
    int        sendCount;        ///< 发送次数（仅用于调试）
} EquipmentActive_;

typedef QVector<EquipmentActive_*> ActionVec_;

class USB_InitConnect : public QObject
{
    Q_OBJECT

public:
    explicit USB_InitConnect(QObject *parent = nullptr);
    ~USB_InitConnect();

    void _startthread();   ///< 启动管理线程

public slots:
    void pthreadStart();                           ///< 线程启动初始化
    void recvSerialdata(const QStringList commandData);  ///< 接收串口数据
    void onCeratActionData(const int ACtionType, const QByteArrayList groupActions); ///< 创建动作数据
    void abortPendingActions();                     ///< 中止未完成复位/清洗队列
    void slotDisconnectandreconnect();              ///< 断线重连处理（已空实现）

signals:
    void writeCommand(const QByteArray, QString);          ///< 发送命令信号
    void bootInitCleanFinished();                         ///< 开机初始化清洗完成
    void smartCupClearFinished();                           ///< 调光前自动丢杯完成
    void CleaningProgress(quint8 index, quint8 total);    ///< 清洗进度更新
    void btn_resetComplete();                             ///< 按钮复位完成
    void btn_CleanComplete();                             ///< 按钮清洗完成
    void CoordinatefinetuningactionFinish(const int indexActFinished);  ///< 坐标微调动作完成
    void finishviewdownheigh(int);                        ///< 测试调整高度完成
    void taskErrorOccurred(const QString& errorMsg);      ///< 命令执行错误信号，供上层提示

private:
    void deleteFinishCommand(quint8 indexcode);        ///< 标记命令完成
    bool ProcessEquipmentActions(ActionVec_ &actions); ///< 处理设备动作队列（发送下一个）
    bool ActionsPerformed(const int indexActive, QString &aboutActive); ///< 动作执行处理
    void CompletedActions(const int indexActive);      ///< 动作完成回调
    void sendTaskHeader();                             ///< 发送下一个待执行任务
    void clearActionQueue();                           ///< 清空任务队列并释放内存

    void dataSort();                                   ///< 按命令编号排序（可选）
    void logCommandFailure(const QStringList& commandData, const QString& errorLog); ///< 记录失败

private:
    QThread m_thread;                    ///< 任务管理线程
    ActionVec_* m_pActionVec = nullptr;  ///< 动作队列指针
    const int comd_num = 2;              ///< 命令编号在数据包中的位置
    int m_runingAction;                  ///< 当前执行的动作类型
    bool m_waitingForResponse = false;   ///< 是否正在等待设备响应（防止并发发送）
    bool m_taskFailed = false;           ///< 当前任务是否因错误而中止
};

#endif // USB_INITCONNECT_H
