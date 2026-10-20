#include "usb_initconnect.h"

#include <QCoreApplication>
#include <QThread>
#include <QDateTime>
#include <quiutils.h>
#include "testing.h"
#include "sampledataprocess.h"
#include <operclass/fullyautomatedplatelets.h>

#include <algorithm>
#include <exception>
#include <stdexcept>

// ============================================================================
// 构造函数与析构函数
// ============================================================================

USB_InitConnect::USB_InitConnect(QObject *parent) : QObject(parent)
  , m_runingAction(EQUIPMENT_FREETIME)
  , m_waitingForResponse(false)
  , m_taskFailed(false)
{
    moveToThread(&m_thread);
    QObject::connect(&m_thread, &QThread::started, this, &USB_InitConnect::pthreadStart);
}

USB_InitConnect::~USB_InitConnect()
{
    m_thread.quit();
    m_thread.wait();
    clearActionQueue();
    QLOG_DEBUG() << "USB_InitConnect 析构完成";
}

// ============================================================================
// 线程管理
// ============================================================================

void USB_InitConnect::_startthread()
{
    if (!m_thread.isRunning()) {
        m_thread.start();
    }
}

void USB_InitConnect::pthreadStart()
{
    QLOG_DEBUG() << "USB_InitConnect 线程启动，ID:" << QThread::currentThreadId();
    if (!m_pActionVec) {
        m_pActionVec = new ActionVec_;
        m_pActionVec->clear();
    }
}

// ============================================================================
// 槽函数实现
// ============================================================================

void USB_InitConnect::clearActionQueue()
{
    if (!m_pActionVec) return;
    for (auto* action : *m_pActionVec) {
        delete action;
    }
    m_pActionVec->clear();
}

void USB_InitConnect::abortPendingActions()
{
    QLOG_WARN() << QString::fromUtf8("中止未完成动作队列")
                << m_runingAction
                << QString::fromUtf8("等待应答") << m_waitingForResponse;
    clearActionQueue();
    m_waitingForResponse = false;
    m_taskFailed = false;
    m_runingAction = EQUIPMENT_FREETIME;
}

void USB_InitConnect::onCeratActionData(const int ACtionType, const QByteArrayList groupActions)
{
    if (!m_pActionVec) {
        QLOG_ERROR() << "onCeratActionData: m_pActionVec is null";
        return;
    }

    const bool homingBusy = m_waitingForResponse
            && (m_runingAction == EQUIPMENT_BTN_REAET
                || m_runingAction == AXIS_BACKRESET);
    if (homingBusy) {
        QLOG_WARN() << "复位进行中，忽略新动作以免中途清队列导致USB掉线, type=" << ACtionType;
        return;
    }

    // 如果当前有因错误中止的任务，或者队列中还有未完成的任务，则清空队列
    if (m_taskFailed || !m_pActionVec->empty()) {
        QLOG_DEBUG() << "收到新动作命令，清空现有任务队列（失败/未完成）";
        clearActionQueue();
        m_taskFailed = false;
        m_waitingForResponse = false;
        m_runingAction = EQUIPMENT_FREETIME;
    }

    ActionVec_ newActions;
    newActions.reserve(groupActions.size());

    try {
        for (qsizetype i = 0; i < groupActions.size(); ++i) {
            const QByteArray &actionData = groupActions.at(i);
            if (actionData.isEmpty()) {
                QLOG_WARN() << "接收到空指令数据";
                continue;
            }

            QByteArray indexField = actionData.mid(GET_COMMAND_INDEX, 1);
            if (indexField.isEmpty()) {
                QLOG_WARN() << "指令索引字段缺失，指令数据:" << actionData;
                continue;
            }

            bool ok;
            QString hexStr = indexField.toHex();
            quint8 index_code = hexStr.toUShort(&ok, HEX_SWITCH);
            if (!ok) {
                QLOG_WARN() << "无效的指令索引值:" << indexField << "原始指令:" << actionData;
                continue;
            }

            EquipmentActive_ *p_singleCommand = new EquipmentActive_;
            try {
                p_singleCommand->Command_number = index_code;
                p_singleCommand->CommamdArry = actionData;
                p_singleCommand->CompletionStatus = false;
                p_singleCommand->ACtionType = ACtionType;
                p_singleCommand->ActionCode = QUIUtils::QByteArrayToQStringList(actionData);
                p_singleCommand->lastSendTime = QDateTime();
                p_singleCommand->sendCount = 0;
                newActions.push_back(p_singleCommand);
            } catch (...) {
                delete p_singleCommand;
                throw;
            }
        }

        // 将新任务追加到队列末尾
        m_pActionVec->reserve(m_pActionVec->size() + newActions.size());
        for (auto* action : newActions) {
            m_pActionVec->push_back(action);
        }
        newActions.clear();

        // 如果当前没有正在等待响应的任务，则立即发送第一个任务
        if (!m_waitingForResponse && !m_pActionVec->empty()) {
            sendTaskHeader();
        }
    } catch (const std::exception& e) {
        for (auto* action : newActions) {
            delete action;
        }
        QLOG_ERROR() << "onCeratActionData 异常:" << e.what();
        throw;
    }
}

void USB_InitConnect::recvSerialdata(const QStringList commandData)
{
    if (commandData.size() < 5) {
        QLOG_WARN() << "Invalid data format. Received:" << commandData.size() << "elements";
        return;
    }

    bool convertOk1, convertOk2, convertOk3, convertOk4;
    const int commandIndex = commandData.value(comd_num).toInt(&convertOk1, 16);
    const int deviceAddress = commandData.value(0).toInt(&convertOk2, 16);
    const int statusFlags = commandData.value(4).toInt(&convertOk3, 16);
    const int errorCode = commandData.last().toInt(&convertOk4, 16);

    if (!convertOk1 || !convertOk2 || !convertOk3 || !convertOk4) {
        QLOG_WARN() << "Invalid hex data in packet";
        return;
    }

    QString errorLog;
    const int runStatus = QUIUtils::_backmotorexecutionstatus(statusFlags, deviceAddress, errorCode, errorLog);

    // 开/关抓手泵命令不处理完成状态
    if (HANDS_SPLIT_NUM == commandIndex || HANDS_SUCK_NUM == commandIndex) {
        QLOG_DEBUG() << "测试坐标时，开/关抓手泵不解析";
        return;
    }

    if (runStatus == MOTOR_FINISH) {
        // 命令成功完成
        QMetaObject::invokeMethod(this, [this, commandIndex]() {
            deleteFinishCommand(commandIndex);
        }, Qt::QueuedConnection);
    } else {
        // 命令执行失败：记录错误，中止当前任务序列
        logCommandFailure(commandData, errorLog);
        m_taskFailed = true;
        m_waitingForResponse = false;
        // 发出错误信号，供上层提示
        emit taskErrorOccurred(QString("命令[%1]执行失败: %2").arg(commandIndex).arg(errorLog));

        // 注意：不调用 deleteFinishCommand，因此不会继续执行后续命令
        // 当新动作命令到来时，会清空队列并重新开始
    }
}

void USB_InitConnect::slotDisconnectandreconnect()
{
    QLOG_DEBUG() << "断线重连事件发生，尝试重发当前任务";

    // 如果任务因错误已停止，不再重发（等待新命令清空队列）
    if (m_taskFailed) {
        QLOG_DEBUG() << "任务因错误已停止，断线重连后不自动重发";
        return;
    }

    if (!m_pActionVec || m_pActionVec->empty()) {
        QLOG_DEBUG() << "任务队列为空，无需重发";
        return;
    }

    // 复位等待标志（断线时可能卡在等待状态）
    m_waitingForResponse = false;

    // 查找与当前执行动作类型匹配且未完成的任务
    auto iter = std::find_if(m_pActionVec->begin(), m_pActionVec->end(),
        [this](const EquipmentActive_* action) {
            return action && !action->CompletionStatus && action->ACtionType == m_runingAction;
        });

    if (iter != m_pActionVec->end()) {
        EquipmentActive_* cmd = *iter;
        QByteArray sendData = cmd->CommamdArry;
        quint8 index = cmd->Command_number;

        QLOG_DEBUG() << "重发命令: " << index << "，动作类型:" << m_runingAction;

        // 更新发送记录
        cmd->lastSendTime = QDateTime::currentDateTime();
        cmd->sendCount++;

        // 重新发送
        QString reminderStr;
        ActionsPerformed(m_runingAction, reminderStr);

        Q_EMIT writeCommand(sendData, reminderStr);

        m_waitingForResponse = true;
    } else {
        QLOG_DEBUG() << "未找到可重发的任务";
    }
}

// ============================================================================
// 任务队列管理
// ============================================================================

void USB_InitConnect::deleteFinishCommand(quint8 indexcode)
{
    if (!m_pActionVec) {
        QLOG_WARN() << "deleteFinishCommand: m_pActionVec is null";
        return;
    }

    // 查找当前正在执行的任务（通常队列中第一个未完成的任务）
    auto iter = std::find_if(m_pActionVec->begin(), m_pActionVec->end(),
        [indexcode](const EquipmentActive_* ptr) {
            return ptr && ptr->Command_number == indexcode && !ptr->CompletionStatus;
        });

    if (iter != m_pActionVec->end()) {
        (*iter)->CompletionStatus = true;
        QLOG_DEBUG() << "命令完成，命令号:" << indexcode;
    } else {
        QLOG_WARN() << "未找到匹配的未完成命令，命令号:" << indexcode;
    }

    // 清除等待标志，准备发送下一个任务
    m_waitingForResponse = false;

    // 继续处理队列（发送下一个未完成的任务）
    ProcessEquipmentActions(*m_pActionVec);
}

bool USB_InitConnect::ProcessEquipmentActions(ActionVec_ &actions)
{
    // 检查是否还有未完成的任务
    bool hasIncomplete = false;
    for (auto* action : actions) {
        if (action && !action->CompletionStatus) {
            hasIncomplete = true;
            break;
        }
    }

    if (hasIncomplete) {
        sendTaskHeader();   // 发送下一个任务
        return true;
    }

    // 所有任务已完成，清理队列
    try {
        CompletedActions(m_runingAction);
        m_runingAction = EQUIPMENT_FREETIME;

        ActionVec_ actionsToDelete;
        actionsToDelete.swap(actions);

        for (auto* action : actionsToDelete) {
            delete action;
        }
        return false;
    } catch (const std::exception& e) {
        QLOG_ERROR() << "ProcessEquipmentActions 异常:" << e.what();
        throw;
    }
}

void USB_InitConnect::dataSort()
{
    if (!m_pActionVec) return;
    auto actionCompare = [](const auto& a, const auto& b) {
        if (!a && !b) return false;
        if (!a) return true;
        if (!b) return false;
        return a->Command_number < b->Command_number;
    };
    if (!std::is_sorted(m_pActionVec->begin(), m_pActionVec->end(), actionCompare)) {
        std::sort(m_pActionVec->begin(), m_pActionVec->end(), actionCompare);
    }
}

namespace {

void bootCleanProgressSteps(const ActionVec_ *vec, int &outStep, int &outTotal)
{
    outTotal = 0;
    int done = 0;
    if (!vec) {
        outStep = 0;
        return;
    }
    for (const EquipmentActive_ *action : *vec) {
        if (!action || action->ACtionType != EQUIPMENT_BOOT_CLEAN) {
            continue;
        }
        ++outTotal;
        if (action->CompletionStatus) {
            ++done;
        }
    }
    outStep = (outTotal > 0) ? qMin(done + 1, outTotal) : 0;
}

} // namespace

// ============================================================================
// 发送下一个任务（“收一个，发一个”核心）
// ============================================================================

void USB_InitConnect::sendTaskHeader()
{
    if (!m_pActionVec || m_pActionVec->empty()) {
        return;
    }

    // 如果任务因错误而中止，则不再发送任何命令（等待新任务清空队列）
    if (m_taskFailed) {
        QLOG_DEBUG() << "任务已因错误中止，等待新动作命令清空队列";
        return;
    }

    if (m_waitingForResponse) {
        QLOG_DEBUG() << "等待设备响应中，暂不发送新任务";
        return;
    }

    auto iter = std::find_if(m_pActionVec->cbegin(), m_pActionVec->cend(),
        [](const EquipmentActive_* action) {
            return action && !action->CompletionStatus;
        });

    if (iter == m_pActionVec->cend()) {
        return;
    }

    EquipmentActive_* cmd = *iter;
    QByteArray sendData = cmd->CommamdArry;
    quint8 index = cmd->Command_number;
    m_runingAction = cmd->ACtionType;

    cmd->lastSendTime = QDateTime::currentDateTime();
    cmd->sendCount++;
    QLOG_DEBUG() << "发送任务: 命令" << index << "，发送次数:" << cmd->sendCount;

    m_waitingForResponse = true;

    QString reminderStr;
    ActionsPerformed(m_runingAction, reminderStr);
    if (m_runingAction == EQUIPMENT_BOOT_CLEAN) {
        int step = 0;
        int total = 0;
        bootCleanProgressSteps(m_pActionVec, step, total);
        if (total > 0) {
            Q_EMIT CleaningProgress(static_cast<quint8>(step),
                                    static_cast<quint8>(qMin(total, 255)));
        }
    }
    Q_EMIT writeCommand(sendData, reminderStr);
}

// ============================================================================
// 动作执行与完成
// ============================================================================

bool USB_InitConnect::ActionsPerformed(const int indexActive, QString &aboutActive)
{
    switch (indexActive) {
        case EQUIPMENT_BOOT_CLEAN:    aboutActive = "开机初始化清洗"; break;
        case EQUIPMENT_BTN_REAET:     aboutActive = "按钮复位"; break;
        case EQUIPMENT_BTN_CLEAN:     aboutActive = "按钮清洗"; break;
        case EQUIPMENT_SMART_CUP_CLEAR: aboutActive = "自动清理测试杯"; break;
        case COORDINATE_FINE_TUNING_TEST: aboutActive = "坐标微调测试动作"; break;
        default: break;
    }
    return true;
}

void USB_InitConnect::CompletedActions(const int indexActive)
{
    switch (indexActive) {
        case EQUIPMENT_BOOT_CLEAN:
            QLOG_DEBUG() << "开机清洗完成";
            emit bootInitCleanFinished();
            break;
        case EQUIPMENT_SMART_CUP_CLEAR:
            QLOG_DEBUG() << "调光前自动丢杯完成";
            emit smartCupClearFinished();
            break;
        case EQUIPMENT_BTN_REAET:
            QLOG_DEBUG() << "复位按钮动作完成";
            emit btn_resetComplete();
            break;
        case EQUIPMENT_BTN_CLEAN:
            QLOG_DEBUG() << "清洗按钮动作完成";
            emit btn_CleanComplete();
            break;
        default:
            if (AXIS_TESTCATCHCUPS <= indexActive && AXIS_TEST_SUCKPRP >= indexActive)
                emit CoordinatefinetuningactionFinish(indexActive);
            else
                emit finishviewdownheigh(indexActive);
            break;
    }
}

// ============================================================================
// 辅助函数
// ============================================================================

void USB_InitConnect::logCommandFailure(const QStringList& commandData, const QString& errorLog)
{
    QString errorInfo = QString("命令执行失败: %1 | 错误信息: %2")
                            .arg(commandData.join(""), errorLog);
    QLOG_ERROR() << errorInfo;
}
