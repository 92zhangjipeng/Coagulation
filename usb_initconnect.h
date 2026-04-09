#ifndef USB_INITCONNECT_H
#define USB_INITCONNECT_H

// Qt核心库
#include <QObject>
#include <QThread>

// Qt其他模块
#include <QReadWriteLock>
#include <QDateTime>
#include <QTimer>

// 项目头文件
#include "cglobal.h"

// 标准库
#include <vector>
#include <memory>

// 前向声明（如果需要）

// ============================================================================
// 数据结构定义
// ============================================================================

/**
 * @brief 设备动作结构体
 *
 * 描述一个设备执行的任务，包含命令信息、状态和超时控制
 */
typedef struct EquipmentActive
{
    quint8     Command_number;     ///< 命令编号
    QByteArray CommamdArry;        ///< 命令数据数组
    QStringList ActionCode;        ///< 动作代码列表
    bool       CompletionStatus;   ///< 完成状态（true=已完成，false=未完成）
    int        ACtionType;         ///< 动作类别
    QDateTime  lastSendTime;       ///< 最后发送时间（用于超时检测）
    int        sendCount;          ///< 发送次数统计
    int        timeoutCount;       ///< 超时次数统计
} EquipmentActive_;

/// 设备动作指针向量类型定义
typedef QVector<EquipmentActive_*> ActionVec_;

// ============================================================================
// 类声明
// ============================================================================

/**
 * @brief USB设备连接和任务管理类
 *
 * 负责USB设备的通信、任务队列管理、超时重试和状态维护
 */
class USB_InitConnect : public QObject
{
    Q_OBJECT

public:
    // ========================================================================
    // 构造与析构
    // ========================================================================
    explicit USB_InitConnect(QObject *parent = nullptr);
    ~USB_InitConnect();

    // ========================================================================
    // 公共接口
    // ========================================================================
    void _startthread();   ///< 启动管理线程

public slots:
    // ========================================================================
    // 公共槽函数
    // ========================================================================
    void _pthreadstart();                           ///< 线程启动初始化
    void Recv_serialdata(const QStringList commandData);  ///< 接收串口数据
    void slotCeratActionDate(int ACtionType, const QByteArrayList groupActions); ///< 创建动作数据
    void slotDisconnectandreconnect();              ///< 断线重连处理

signals:
    // ========================================================================
    // 信号
    // ========================================================================
    void writeCommand(const QByteArray, QString);          ///< 发送命令信号
    void bootInitCleanFinished();                         ///< 开机初始化清洗完成
    void CleaningProgress(quint8 index, quint8 total);    ///< 清洗进度更新
    void btn_resetComplete();                             ///< 按钮复位完成
    void btn_CleanComplete();                             ///< 按钮清洗完成
    void CoordinatefinetuningactionFinish();              ///< 坐标微调动作完成
    void finishviewdownheigh(int);                        ///< 测试调整高度完成

private:
    // ========================================================================
    // 私有辅助函数
    // ========================================================================

    // 任务队列管理
    void dataSort();                                   ///< 数据排序（按命令编号）
    void deleteFinishCommand(quint8 indexcode);        ///< 删除完成命令
    bool ProcessEquipmentActions(ActionVec_ &actions); ///< 处理设备动作队列

    // 动作执行与完成
    bool ActionsPerformed(const int indexActive, QString &aboutActive); ///< 动作执行处理
    void CompletedActions(const int indexActive);       ///< 动作完成回调

    // 超时与任务调度
    void checkTaskTimeout();                           ///< 检查任务超时
    void sendTaskHeader();                             ///< 发送首个任务字节

private:
    // ========================================================================
    // 成员变量
    // ========================================================================

    // 线程管理
    QThread m_thread;                                  ///< 任务管理线程

    // 任务队列
    ActionVec_* m_pActionVec = nullptr;                ///< 动作队列指针

    // 定时器
    QTimer* m_timeoutTimer = nullptr;                  ///< 超时检查定时器

    // 状态变量
    const int comd_num = 2;                            ///< 命令编号在数据包中的位置
    int m_runingAction = EQUIPMENT_FREETIME;           ///< 当前执行的动作类型

    // ========================================================================
    // 静态配置常量（类级别共享）
    // ========================================================================
    static const int TASK_TIMEOUT_MS = 30000;          ///< 任务超时时间（30秒）
    static const int MAX_RETRY_COUNT = 3;              ///< 最大重试次数
    static const int TIMEOUT_CHECK_INTERVAL = 5000;    ///< 超时检查间隔（5秒）
};

#endif // USB_INITCONNECT_H
