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

}

USB_InitConnect::~USB_InitConnect()
{
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
}



void USB_InitConnect::slotCeratActionDate(int ACtionType,const QByteArrayList groupActions)
{
    // 预分配内存避免多次扩容
    m_pActionVec->reserve(m_pActionVec->size() + groupActions.size());

    QVector<EquipmentActive_> actionPool(groupActions.size());
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
        p_singleCommand->Command_number = index_code;
        p_singleCommand->CommamdArry = actionData;
        p_singleCommand->CompletionStatus = false;
        p_singleCommand->ACtionType = ACtionType;
        p_singleCommand->ActionCode = QUIUtils::QByteArrayToQStringList(actionData);
        m_pActionVec->push_back(p_singleCommand);
    }
    if (!m_pActionVec->empty()) {
		sendTaskHeader();
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

    bool convertOk;
    const int commandIndex =  commandData.value(comd_num).toInt(&convertOk, 16);
    const int deviceAddress = commandData.value(0).toInt(&convertOk, 16);
    const int statusFlags =   commandData.value(4).toInt(&convertOk, 16);
    const int errorCode =     commandData.last().toInt(&convertOk, 16);

    if (!convertOk) {
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

    auto iter = std::find_if(m_pActionVec->begin(), m_pActionVec->end(),
        [this, indexcode](const EquipmentActive_* ptr) {
            return ptr->Command_number == indexcode;
        }
    );

    if (iter != m_pActionVec->end()) {
        (*iter)->CompletionStatus = true;  // 直接通过迭代器修改元素
    } else {
        QLOG_DEBUG() << "无效命令号: " << indexcode;  // 添加更多调试信息
    }


    //遍历是否所有都完成
   bool hadIncomplete = ProcessEquipmentActions(*m_pActionVec);
   if (!hadIncomplete) {
       m_pActionVec->clear();
   }
   return;
}

bool USB_InitConnect::ProcessEquipmentActions(const ActionVec_ & actions){
    const bool hasIncompleteActions = std::any_of(actions.cbegin(), actions.cend(),
            [](const EquipmentActive_* action) {
                return !action->CompletionStatus;
            });
    if (hasIncompleteActions) {
          sendTaskHeader();
          return true;
    }
    try {
        CompletedActions(m_runingAction); //完成的动作
        m_runingAction = EQUIPMENT_FREETIME;
        // Clean up resources
        for (auto* action : actions) {
            delete action;
        }
        return false;
    }
    catch (...) {
            // Log error or handle exception
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
    if(!m_pActionVec->empty())
    {
        auto iter = m_pActionVec->begin();
        while(iter != m_pActionVec->end())
        {
            EquipmentActive_ *pcommand = *iter;
            if(pcommand->ACtionType == m_runingAction && !pcommand->CompletionStatus)
            {
                QLOG_DEBUG()<<"重连发命令成功";
				QString reminderStr = "";
                ActionsPerformed(m_runingAction,reminderStr);
                emit writeCommand(pcommand->CommamdArry,reminderStr);
                break;
            }
            iter++;
        }
    }
    return;
}

void USB_InitConnect::dataSort(){
    auto actionCompare = [](const auto& a, const auto& b) {
        // 示例：按优先级降序排序
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

        auto iter = std::find_if(m_pActionVec->cbegin(), m_pActionVec->cend(),
            [](const EquipmentActive_* ptr) {
                return ptr->CompletionStatus == false;  // 检查状态是否为False
            }
        );
        if (iter != m_pActionVec->cend()) {
            EquipmentActive_* cmd = *iter;
            QByteArray sendData = cmd->CommamdArry;
            quint8 index = cmd->Command_number;
            m_runingAction = cmd->ACtionType;
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
