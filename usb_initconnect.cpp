#pragma execution_character_set("utf-8")

#include  "usb_initconnect.h"
#include  <QCoreApplication>
#include  <quiutils.h>
#include  <QThread>
#include  "testing.h"

#include  "sampledataprocess.h"
#include <operclass/fullyautomatedplatelets.h>


USB_InitConnect::USB_InitConnect(QObject *parent) : QObject(parent)
{
    moveToThread(&m_thread);
    QObject::connect(&m_thread,&QThread::started,this,&USB_InitConnect::_pthreadstart);

    // 创建超时检查定时器（定时器也需要移动到线程中）
    m_timeoutTimer = new QTimer(this);
    m_timeoutTimer->setInterval(TIMEOUT_CHECK_INTERVAL);
    QObject::connect(m_timeoutTimer, &QTimer::timeout, this, &USB_InitConnect::checkTaskTimeout);
    m_timeoutTimer->moveToThread(&m_thread); // 定时器也需要在同一线程中运行

}

USB_InitConnect::~USB_InitConnect()
{
    // 停止超时定时器
    if (m_timeoutTimer) {
        m_timeoutTimer->stop();
        delete m_timeoutTimer;
        m_timeoutTimer = nullptr;
    }

    m_thread.quit();
    m_thread.wait();

    if (m_pActionVec != nullptr)
    {
        for (auto& action : *m_pActionVec){
            delete action;
            action = nullptr;
        }

        delete m_pActionVec;
        m_pActionVec = nullptr;
    }

    QLOG_DEBUG()<<"单个发送任务线程析构ID:"<< QThread::currentThreadId();
}

void USB_InitConnect::_startthread()
{
    if(!m_thread.isRunning())
    {
        m_thread.start();
    }
}

void USB_InitConnect::_pthreadstart()
{
    QLOG_DEBUG()<<"单个任务线程ID:"<< QThread::currentThreadId();
    if(!m_pActionVec)
    {
        m_pActionVec = new ActionVec_;
        m_pActionVec->clear();
    }

    // 启动超时检查定时器
    if (m_timeoutTimer) {
        m_timeoutTimer->start();
        QLOG_DEBUG() << "任务超时检查定时器已启动，检查间隔:" << TIMEOUT_CHECK_INTERVAL << "ms";
    }
}



void USB_InitConnect::slotCeratActionDate(int ACtionType,const QByteArrayList groupActions)
{
    if (!m_pActionVec) {
        QLOG_ERROR() << "slotCeratActionDate: m_pActionVec is null, action vector not initialized";
        return;
    }

    // 使用局部容器实现异常安全：如果分配失败，已分配的对象会被自动清理
    ActionVec_ newActions;
    newActions.reserve(groupActions.size());

    try {
        for (qsizetype i = 0; i < groupActions.size(); ++i) {
            const QByteArray &actionData = groupActions.at(i); // 缓存引用
            if(actionData.isEmpty()) {
                QLOG_WARN() << "接收到空指令数据"<<endl;
                continue;
            }
            QByteArray indexField = actionData.mid(GET_COMMAND_INDEX, 1);
            if(indexField.isEmpty()) {
                QLOG_WARN() << "指令索引字段缺失，指令数据:" << actionData;
                continue;
            }
            bool ok;
            QString hexStr = indexField.toHex();
            quint8 index_code = hexStr.toUShort(&ok, HEX_SWITCH);
            if(!ok) {
                QLOG_WARN() << "无效的指令索引值:" << indexField << "原始指令:" << actionData;
                continue;
            }
            EquipmentActive_ *p_singleCommand = new EquipmentActive_;
            // 如果后续设置字段时抛出异常，需要清理这个对象
            try {
                p_singleCommand->Command_number = index_code;
                p_singleCommand->CommamdArry = actionData;
                p_singleCommand->CompletionStatus = false;
                p_singleCommand->ACtionType = ACtionType;
                p_singleCommand->ActionCode = QUIUtils::QByteArrayToQStringList(actionData);
                // 初始化超时相关字段
                p_singleCommand->lastSendTime = QDateTime(); // 无效时间，表示尚未发送
                p_singleCommand->sendCount = 0;
                p_singleCommand->timeoutCount = 0;
                newActions.push_back(p_singleCommand);
            } catch (...) {
                // 如果设置字段时发生异常，清理当前对象并重新抛出
                delete p_singleCommand;
                throw;
            }
        }

        // 所有分配成功，将新动作追加到主容器
        m_pActionVec->reserve(m_pActionVec->size() + newActions.size());
        for (auto* action : newActions) {
            m_pActionVec->push_back(action);
        }
        // 清除局部容器的指针，避免双重删除（所有权已转移）
        newActions.clear();

        if (!m_pActionVec->empty()) {
            sendTaskHeader();
        }
    } catch (const std::exception& e) {
        // 发生异常，清理局部容器中已分配的对象
        for (auto* action : newActions) {
            delete action;
        }
        QLOG_ERROR() << "slotCeratActionDate异常:" << e.what();
        throw; // 重新抛出，让调用者处理
    }
    return;
}


void USB_InitConnect::Recv_serialdata(const QStringList commandData)
{
    // 边界检查
    if (commandData.size() < 5) {
        QLOG_WARN() << "Invalid data format. Received:" << commandData.size() << "elements";
        return;
    }

    bool convertOk1, convertOk2, convertOk3, convertOk4;
    const int commandIndex =  commandData.value(comd_num).toInt(&convertOk1, 16);
    const int deviceAddress = commandData.value(0).toInt(&convertOk2, 16);
    const int statusFlags =   commandData.value(4).toInt(&convertOk3, 16);
    const int errorCode =     commandData.last().toInt(&convertOk4, 16);

    if (!convertOk1 || !convertOk2 || !convertOk3 || !convertOk4) {
        QLOG_WARN() << "Invalid hex data in packet";
        return;
    }


    QString errorLog;
    const int runStatus = QUIUtils::_backmotorexecutionstatus(statusFlags, deviceAddress, errorCode, errorLog);

    if (runStatus == MOTOR_FINISH) {
            QMetaObject::invokeMethod(this, [this, commandIndex]() {
                deleteFinishCommand(commandIndex);
            }, Qt::QueuedConnection);
    } else {
        const QString errorData = commandData.join("");
        QLOG_DEBUG().nospace() << "[CMD ERR] " << errorData << " | " << errorLog;
    }
    return;
}


void USB_InitConnect::deleteFinishCommand(quint8 indexcode)
{
    if (!m_pActionVec) {
        QLOG_WARN() << "deleteFinishCommand: m_pActionVec is null";
        return;
    }

    auto iter = std::find_if(m_pActionVec->begin(), m_pActionVec->end(),
        [this, indexcode](const EquipmentActive_* ptr) {
            return ptr && ptr->Command_number == indexcode;
        }
    );

    if (iter != m_pActionVec->end()) {
        (*iter)->CompletionStatus = true;  // 直接通过迭代器修改元素
    } else {
        QLOG_DEBUG() << "无效命令号: " << indexcode;  // 添加更多调试信息
    }


    //遍历是否所有都完成
   ProcessEquipmentActions(*m_pActionVec);
   return;
}

bool USB_InitConnect::ProcessEquipmentActions(ActionVec_ & actions){
    // 首先检查是否有未完成的任务
    const bool hasIncompleteActions = std::any_of(actions.cbegin(), actions.cend(),
            [](const EquipmentActive_* action) {
                return action && !action->CompletionStatus;
            });
    if (hasIncompleteActions) {
          sendTaskHeader();
          return true;
    }

    try {
        CompletedActions(m_runingAction); //完成的动作
        m_runingAction = EQUIPMENT_FREETIME;

        // 原子性删除：先将所有权转移到局部容器
        ActionVec_ actionsToDelete;
        actionsToDelete.swap(actions); // 交换内容，现在actions为空

        // 安全删除局部容器中的指针
        for (auto* action : actionsToDelete) {
            delete action;
        }
        // actionsToDelete离开作用域自动销毁，此时actions已经是空容器
        return false;
    }
    catch (const std::exception& e) {
        QLOG_ERROR() << "ProcessEquipmentActions exception: " << e.what();
        throw; // 重新抛出原始异常
    }
    catch (...) {
        QLOG_ERROR() << "ProcessEquipmentActions unknown exception";
        throw std::runtime_error("Failed to process equipment actions cleanup");
    }

}


bool USB_InitConnect::ActionsPerformed(const int indexActive,QString &aboutActive)
{
    switch(indexActive)
    {
        case EQUIPMENT_BOOT_CLEAN:
        {
            aboutActive = "开机初始化清洗";
            break;
        }
        case EQUIPMENT_BTN_REAET:
        {
            aboutActive = "按钮复位";
            break;
        }
        case EQUIPMENT_BTN_CLEAN:
        {
            aboutActive = "按钮清洗";
            break;
        }
        case COORDINATE_FINE_TUNING_TEST:
        {
            aboutActive ="坐标微调测试动作";
            break;
        }
        default: break;
    }
    return true;
}


//执行的动作
void USB_InitConnect::CompletedActions(const int indexActive)
{
    switch(indexActive)
    {
        case EQUIPMENT_BOOT_CLEAN:
        {
            QLOG_DEBUG()<<"开机清洗完成";
            emit bootInitCleanFinished();
            break;
        }
        case EQUIPMENT_BTN_REAET:
        {
            emit btn_resetComplete();
            break;
        }
        case EQUIPMENT_BTN_CLEAN:
        {
            emit btn_CleanComplete();
            break;
        }
        case COORDINATE_FINE_TUNING_TEST:
        {
            emit CoordinatefinetuningactionFinish();
            break;
        }
        case BLOODPINDOWNHEIGH:
        case BLOODPINDOWNHEIGH_CLEANLINQUEFAILED:
        case BLOODPINDOWNHEIGH_SERUMMODEL:
        case BLOODPINDOWNHEIGH_ANEMIALINQUEFAILED:

        case REAGPIN_CLEANLINQUE_DOWN:
        case REAGPIN_REAGLINQUE_FAILEDDOWN:
        case REAGPIN_DOWNHEIGH_IN_AA:
        case REAGPIN_DOWNHEIGH_IN_ADP:
        case REAGPIN_DOWNHEIGH_IN_EPI:
        case REAGPIN_DOWNHEIGH_IN_COL:
        case REAGPIN_DOWNHEIGH_IN_RIS:

       case HANDSDOWN_THROWCUP:
       case HANDSDOWN_TRAYTUBE_1:
       case HANDSDOWN_TRAYTUBE_2:
       case HANDSDOWN_TRAYTUBE_3:
       case HANDSDOWN_TRAYTUBE_4:

       case HANDSDOWN_CHN_1:
       case HANDSDOWN_CHN_2:
       case HANDSDOWN_CHN_3:
       case HANDSDOWN_CHN_4:
       case HANDSDOWN_CHN_5:
       case HANDSDOWN_CHN_6:
       case HANDSDOWN_CHN_7:
       case HANDSDOWN_CHN_8:
       case HANDSDOWN_CHN_9:
       case HANDSDOWN_CHN_10:
       case HANDSDOWN_CHN_11:
       case HANDSDOWN_CHN_12:
        {
            emit finishviewdownheigh(indexActive);
        }
        default: break;
    }
    return;
}


//断线后连
void USB_InitConnect::slotDisconnectandreconnect()
{
    if (!m_pActionVec || m_pActionVec->empty())
    {
        return;
    }

    auto iter = m_pActionVec->begin();
    while(iter != m_pActionVec->end())
    {
        EquipmentActive_ *pcommand = *iter;
        if (!pcommand) {
            QLOG_WARN() << "slotDisconnectandreconnect: null pointer in action vector";
            ++iter;
            continue;
        }
        if(pcommand->ACtionType == m_runingAction && !pcommand->CompletionStatus)
        {
            QLOG_DEBUG()<<"重连发命令成功";
            QString reminderStr = "";
            ActionsPerformed(m_runingAction,reminderStr);
            emit writeCommand(pcommand->CommamdArry,reminderStr);
            break;
        }
        ++iter;
    }
    return;
}

void USB_InitConnect::dataSort(){
    if (!m_pActionVec) {
        return;
    }

    auto actionCompare = [](const auto& a, const auto& b) {
        // 处理空指针
        if (!a && !b) return false; // 两者都为空，相等
        if (!a) return true;        // a 为空，b 不为空，a < b
        if (!b) return false;       // a 不为空，b 为空，a > b
        // 两者都非空，比较 Command_number
        return a->Command_number < b->Command_number;
    };
    // 3. 仅在未排序时进行排序
    if (!std::is_sorted(m_pActionVec->begin(), m_pActionVec->end(), actionCompare)) {
        std::sort(m_pActionVec->begin(), m_pActionVec->end(), actionCompare);
    }
}

void USB_InitConnect::sendTaskHeader()
{
    // 空容器检查（关键安全防护)
    QString reminderStr;
    if (!m_pActionVec || m_pActionVec->empty()) {
        qCritical() << "Attempted to send task header with empty action vector";
        return;
    }
    try {
        dataSort();

        // 使用智能选择算法：优先选择未发送的任务，然后是发送时间最早的任务
        auto iter = std::min_element(m_pActionVec->cbegin(), m_pActionVec->cend(),
            [this](const EquipmentActive_* a, const EquipmentActive_* b) {
                // 处理空指针
                if (!a) return false; // a无效，b优先
                if (!b) return true;  // b无效，a优先

                // 排除已完成或超过重试次数的任务
                bool aValid = !a->CompletionStatus && a->timeoutCount < MAX_RETRY_COUNT;
                bool bValid = !b->CompletionStatus && b->timeoutCount < MAX_RETRY_COUNT;

                if (!aValid && !bValid) return false; // 都无效，任意
                if (!aValid) return false; // a无效，b优先
                if (!bValid) return true;  // b无效，a优先

                // 优先选择未发送过的任务（lastSendTime无效）
                bool aNotSent = !a->lastSendTime.isValid();
                bool bNotSent = !b->lastSendTime.isValid();
                if (aNotSent && !bNotSent) return true;  // a未发送，b已发送，a优先
                if (!aNotSent && bNotSent) return false; // a已发送，b未发送，b优先

                // 都发送过，选择发送时间更早的（等待时间更长的）
                if (a->lastSendTime.isValid() && b->lastSendTime.isValid()) {
                    return a->lastSendTime < b->lastSendTime;
                }

                // 默认情况：选择发送次数更少的
                return a->sendCount < b->sendCount;
            }
        );
        if (iter != m_pActionVec->cend()) {
            EquipmentActive_* cmd = *iter;
            // 验证任务是否有效
            if (!cmd || cmd->CompletionStatus || cmd->timeoutCount >= MAX_RETRY_COUNT) {
                QLOG_WARN() << "找到无效任务，跳过发送。命令:" << (cmd ? cmd->Command_number : 0)
                           << "完成状态:" << (cmd ? cmd->CompletionStatus : true)
                           << "超时次数:" << (cmd ? cmd->timeoutCount : 0);
                return; // 任务无效，直接返回
            }

            QByteArray sendData = cmd->CommamdArry;
            quint8 index = cmd->Command_number;
            m_runingAction = cmd->ACtionType;

            // 更新任务发送状态
            cmd->lastSendTime = QDateTime::currentDateTime();
            cmd->sendCount++;
            QLOG_DEBUG() << "发送任务: 命令" << index << "发送次数:" << cmd->sendCount
                        << "超时次数:" << cmd->timeoutCount;

            ActionsPerformed(m_runingAction, reminderStr);
            if(m_runingAction == EQUIPMENT_BOOT_CLEAN)
                Q_EMIT CleaningProgress(index, m_pActionVec->count());
            Q_EMIT writeCommand(sendData, reminderStr);
        }

    } catch (const std::exception& e) {
        qFatal("Task header processing failed: %s", e.what());
    }
    return;
}

void USB_InitConnect::checkTaskTimeout()
{
    if (!m_pActionVec || m_pActionVec->empty()) {
        return;
    }

    QDateTime now = QDateTime::currentDateTime();
    bool foundTimeout = false;

    for (auto* action : *m_pActionVec) {
        if (!action || action->CompletionStatus) {
            continue;
        }

        // 检查任务是否已发送过（lastSendTime有效）且已超时
        if (action->lastSendTime.isValid()) {
            qint64 elapsed = action->lastSendTime.msecsTo(now);
            if (elapsed > TASK_TIMEOUT_MS) {
                // 任务超时
                action->timeoutCount++;
                QLOG_WARN() << "任务超时检测: 命令" << action->Command_number
                              << "超时" << elapsed << "ms, 超时次数:" << action->timeoutCount;

                if (action->timeoutCount >= MAX_RETRY_COUNT) {
                    // 超过最大重试次数，标记为失败
                    QLOG_ERROR() << "任务超过最大重试次数，标记为失败: 命令" << action->Command_number;
                    action->CompletionStatus = true; // 标记为完成（失败）
                    foundTimeout = true;
                } else {
                    // 重置发送时间，允许重新发送
                    action->lastSendTime = QDateTime(); // 重置为无效时间，等待重新发送
                    QLOG_DEBUG() << "任务准备重试: 命令" << action->Command_number;
                }
            }
        }
    }

    // 如果有任务超时失败，重新检查任务队列
    if (foundTimeout) {
        QLOG_DEBUG() << "检测到超时任务，重新检查任务队列状态";
        ProcessEquipmentActions(*m_pActionVec);
    }
}
