#ifdef _MSC_VER
#pragma warning(disable: 4819)
#pragma execution_character_set("utf-8")
#endif
#include "structinstance.h"
#include "testprojectprocess.h"
#include  "quiutils.h"
#include "loginui.h"
#include "globaldata.h"
#include <cstdint>
#include <QSemaphore>
#include <QThread>
#include <QDateTime>
#include <QPointer>
#include <memory>
#include <array>    // 若使用 std::array
#include <cstdint>  // quint8 类型支持
#include <operclass/fullyautomatedplatelets.h>
#include <LIS/curveimagegenerator.h>
#include <testing.h>
#include "loadequipmentpos.h"
#include "cglobal.h"

//#define POLLING_TIMER  800

namespace {

quint8 commandIndexOf(const QByteArray &cmd)
{
    if (cmd.size() <= GET_COMMAND_INDEX) {
        return 0;
    }
    bool ok = false;
    const quint8 index = static_cast<quint8>(
                cmd.mid(GET_COMMAND_INDEX, 1).toHex().toUShort(&ok, 16));
    return ok ? index : 0;
}

const char *recoveryStepName(int order)
{
    switch (order) {
    case 0: return "1/6 抓手Z复位";
    case 1: return "2/6 试剂针Z复位";
    case 2: return "3/6 样本针Z复位";
    case 3: return "4/6 XY回原点";
    case 4: return "5/6 XY走到原点";
    case 5: return "6/6 XY原点再确认";
    default: return "善后命令";
    }
}

}

// 定义静态成员变量
std::mutex TestProjectProcess::s_actionMutex;

const QHash<int, QString> TestProjectProcess::s_cleanActionMap = {
    {WASH_BLLODSAMPLE_NEDDLES, QString::fromUtf8("清洗样本针")},
    {WASH_REAGENT_NEDDLES,     QString::fromUtf8("清洗试剂针")},
    {WASH_DOUBLE_NEDDLES,      QString::fromUtf8("清洗样本针和试剂针")},
    {THROW_CUPS_END,           QString::fromUtf8("丢杯")}
};

const QHash<quint8, TestProjectProcess::ReagentWorkflow> TestProjectProcess::s_reagentWorkflow = {
    {AA_REAGENT,  {FOCUS_TESTING_AA,  FOCUS_SPIT_AA_TESTING,  FOCUS_THROW_AA_TUBE,  INDEX_AA_CONSUMABLE}},
    {ADP_REAGENT, {FOCUS_TESTING_ADP, FOCUS_SPIT_ADP_TESTING, FOCUS_THROW_ADP_TUBE, INDEX_ADP_CONSUMABLE}},
    {EPI_REAGENT, {FOCUS_TESTING_EPI, FOCUS_SPIT_EPI_TESTING, FOCUS_THROW_EPI_TUBE, INDEX_EPI_CONSUMABLE}},
    {COL_REAGENT, {FOCUS_TESTING_COL, FOCUS_SPIT_COL_TESTING, FOCUS_THROW_COL_TUBE, INDEX_COL_CONSUMABLE}},
    {RIS_REAGENT, {FOCUS_TESTING_RIS, FOCUS_SPIT_RIS_TESTING, FOCUS_THROW_RIS_TUBE, INDEX_RIS_CONSUMABLE}}
};

const QHash<quint8, QString> TestProjectProcess::s_focusActionMap = {
    {FOCUS_ADDING_SAMPLE,          QString::fromUtf8("加样")},
    {FOCUS_CLEAN_BLOODPIN,         QString::fromUtf8("清洗样本针")},
    {FOCUS_CLIP_ANEMIA_TO_CHN,     QString::fromUtf8("夹PPP到通道")},
    {FOCUS_THORW_ANEMIA,           QString::fromUtf8("弃PPP杯")},
    {FOCUS_TESTING_AA,             QString::fromUtf8("吸AA+抓PRP")},
    {FOCUS_TESTING_ADP,            QString::fromUtf8("吸ADP+抓PRP")},
    {FOCUS_TESTING_EPI,             QString::fromUtf8("吸EPI+抓PRP")},
    {FOCUS_TESTING_COL,             QString::fromUtf8("吸COL+抓PRP")},
    {FOCUS_TESTING_RIS,             QString::fromUtf8("吸RIS+抓PRP")},
    {FOCUS_SPIT_AA_TESTING,        QString::fromUtf8("吐AA试剂")},
    {FOCUS_SPIT_ADP_TESTING,       QString::fromUtf8("吐ADP试剂")},
    {FOCUS_SPIT_EPI_TESTING,       QString::fromUtf8("吐EPI试剂")},
    {FOCUS_SPIT_COL_TESTING,       QString::fromUtf8("吐COL试剂")},
    {FOCUS_SPIT_RIS_TESTING,       QString::fromUtf8("吐RIS试剂")},
    {FOCUS_CLEAN_DOUBLEPIN,        QString::fromUtf8("清洗双针")},
    {FOCUS_CLEAN_EREAGENTPIN_AA,   QString::fromUtf8("清洗试剂针(AA)")},
    {FOCUS_CLEAN_EREAGENTPIN_ADP,  QString::fromUtf8("清洗试剂针(ADP)")},
    {FOCUS_CLEAN_EREAGENTPIN_EPI,  QString::fromUtf8("清洗试剂针(EPI)")},
    {FOCUS_CLEAN_EREAGENTPIN_COL,  QString::fromUtf8("清洗试剂针(COL)")},
    {FOCUS_CLEAN_EREAGENTPIN_RIS,  QString::fromUtf8("清洗试剂针(RIS)")},
    {FOCUS_THROW_AA_TUBE,          QString::fromUtf8("弃AA测试杯")},
    {FOCUS_THROW_ADP_TUBE,         QString::fromUtf8("弃ADP测试杯")},
    {FOCUS_THROW_EPI_TUBE,         QString::fromUtf8("弃EPI测试杯")},
    {FOCUS_THROW_COL_TUBE,         QString::fromUtf8("弃COL测试杯")},
    {FOCUS_THROW_RIS_TUBE,         QString::fromUtf8("弃RIS测试杯")},
    {FOCUS_ERR_BACKORIGIN,         QString::fromUtf8("异常回原点")}
};

//抓手异常复位超时兜底阈值(ms):复位指令发出后若超过此时间prePatchActions仍未全部完成,
//视为设备回复数与队列数不匹配(如设备对相同指令去重),强制提示用户介入,避免机器卡死无弹窗
static const qint64 GRIPPER_ERR_EARLY_PROMPT_MS = 2500;
static const qint64 GRIPPER_ERR_TIMEOUT_MS = 30000;
static const int ACK_WATCHDOG_MS = 20000;
static const int RESET_FALLBACK_MS = 45000;
static const int OFFLINE_VOID_REAGENT_MS = 30000;

namespace {

QList<quint8> commandIndexListFromBytes(const QByteArrayList &commands)
{
    QList<quint8> indices;
    for (const QByteArray &command : commands) {
        if (command.isEmpty()) {
            continue;
        }
        const QByteArray indexField = command.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            continue;
        }
        bool ok = false;
        const quint8 indexCode = indexField.toHex().toUShort(&ok, HEX_SWITCH);
        if (ok) {
            indices.append(indexCode);
        }
    }
    return indices;
}

} // namespace

TestProjectProcess::TestProjectProcess(QObject *parent, quint8 equipmentIndex)
    : QObject(parent),
      m_ackWatchdog(nullptr),
      m_targetThread(nullptr),
      retryCount(0),
      mdataAArry{},
      m_Errorresetsfirst(false),
      mTestErrSampleid(0)
{
    m_finishTestData.clear();
    m_pendingtimeSample.clear();
    m_pendingContinueReagent.clear();
    m_pauseCommandQueue.clear();
    mbackOriginnumList.clear();
    m_totalChannel = (equipmentIndex + 1)*4;
    m_ackWatchdog = new QTimer(this);
    m_ackWatchdog->setSingleShot(true);
    connect(m_ackWatchdog, &QTimer::timeout, this, &TestProjectProcess::handleAckTimeout);

    m_offlineVoidTimer = new QTimer(this);
    m_offlineVoidTimer->setSingleShot(true);
    connect(m_offlineVoidTimer, &QTimer::timeout, this, &TestProjectProcess::handleOfflineVoidTimeout);

    m_gripperVacuumPollTimer = new QTimer(this);
    m_gripperVacuumPollTimer->setInterval(kGripperVacuumPollIntervalMs);
    connect(m_gripperVacuumPollTimer, &QTimer::timeout, this, &TestProjectProcess::onGripperVacuumPollTick);
}

TestProjectProcess::~TestProjectProcess()
{
    QLOG_DEBUG()<<"stop"<<__FILE__<<endl;
    disarmAckWatchdog();
    if (m_offlineVoidTimer) {
        m_offlineVoidTimer->stop();
    }
}
void TestProjectProcess::setTargetThread(QThread* thread) {
    m_targetThread = thread;
}

void TestProjectProcess::initTimer()
{
}

void TestProjectProcess::handleAckTimeout()
{
    if (!cglobal::g_StartTesting.load() || cglobal::g_PauseTesting.load()
            || !cglobal::gserialConnecStatus.load() || m_offlineWaitingReconnect) {
        m_outstandingAcks = 0;
        return;
    }
    if (m_gripperErrStartMs > 0) {
        return;
    }
    if (m_Errorresetsfirst) {
        QLOG_WARN() << QString::fromUtf8("全轴复位中忽略应答超时，等待复位完成或兜底续跑");
        return;
    }

    const int acksBeforeTimeout = m_outstandingAcks;
    const bool gripperPollActive = StructInstance::getInstance()->isGripperVacuumPollActive();
    QLOG_WARN() << "[GripperVac] 指令应答超时 焦点样本" << m_focus_sample_id
                << "超时前待应答数" << acksBeforeTimeout
                << "负压轮询中" << gripperPollActive
                << "待发缓存非空" << !mdataAArry.isEmpty();

    m_outstandingAcks = 0;
    if (m_waitingPppRead) {
        m_pendingPppReread = true;
        m_waitingPppRead = false;
        const QString tips = tr("读取PPP初值超时，已暂停，请处理后点「继续」重读");
        promptsareDisplayed(TESTINGLOG, tips, false);
        publishWorkflowStatus(tips);
        emit pauseTesting();
        return;
    }
    if (m_waitingPrpRead) {
        m_pendingPrpReread = true;
        m_waitingPrpRead = false;
        const QString tips = tr("读取PRP初值超时，已暂停，请处理后点「继续」重读");
        promptsareDisplayed(TESTINGLOG, tips, false);
        publishWorkflowStatus(tips);
        emit pauseTesting();
        return;
    }

    appendPauseQueueFromFocusRecovery();
    const QString tips = tr("指令应答超时，已暂停，请检查仪器后继续");
    logTestFsm(QStringLiteral("ack_timeout"),
               tr("待应答=%1 恢复队列=%2").arg(acksBeforeTimeout).arg(m_pauseCommandQueue.size()));
    publishWorkflowStatus(tips);
    emit pauseTesting();
    promptsareDisplayed(TESTINGLOG, tips, true);
}


void TestProjectProcess::promptsareDisplayed(const quint8 proIndex,const QString tips,const bool outwidgetTips){
    emit reminderTextOut(proIndex,tips);
    if(outwidgetTips){
         emit executeErrPrompt(tips);
    }
}


void TestProjectProcess::SampleReagTestFinish(QString finishid,const quint8 index_Chn,const quint8 indexReage)
{

    QString idDate;
    int completeSample = 0;
    GlobalData::apartSampleId(finishid,idDate,completeSample);


    Finish_Data_ kFinishChannel;
    kFinishChannel.CompleteSampleid = completeSample;
    kFinishChannel.index_Chn = index_Chn;
    kFinishChannel.index_reagent = indexReage;
    m_finishTestData.enqueue(kFinishChannel);
    QString finishLogs = QUIUtils::index_reagent_mapping_reagentName(indexReage,"试剂");
    logTestFsmInfo(QStringLiteral("reag_test_done_enqueue"),
                   tr("样本%1 %2 通道%3 队列=%4")
                       .arg(completeSample)
                       .arg(finishLogs)
                       .arg(index_Chn + 1)
                       .arg(m_finishTestData.size()));

    //通道测试完成如果有样本在加样就入队
    const bool sampleFocusActive = StructInstance::getInstance()->sampleHadFocus();

    if(!sampleFocusActive) {

        StructInstance::getInstance()->config_focusSample(completeSample,true);//++焦点
        m_focus_sample_id = completeSample;
        const quint8 focus_index_set = s_reagentWorkflow.value(indexReage).throwFocus;
        if (focus_index_set == 0) {
            QLOG_ERROR()<<"未找到完成试剂号"<<"行号"<<__LINE__<<endl;
        } else {
            StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
        }
        auto it = m_finishTestData.begin();
        while (it != m_finishTestData.end()) {
            if (it->index_Chn == index_Chn && it->index_reagent == indexReage
                    && it->CompleteSampleid == completeSample) {
                it = m_finishTestData.erase(it);
            } else {
                ++it;  // 未匹配则继续遍历
            }
        }

        emit setprogressinitboolwaittext(true,index_Chn,"");
        outputThrowTestChannelReagentFinish(m_focus_sample_id,indexReage);
    }
    return;
}

void TestProjectProcess::resetRuntimeForSessionStart()
{
    disarmAckWatchdog();
    m_outstandingAcks = 0;
    m_Errorresetsfirst = false;
    m_needHomeBeforeResume = false;
    mbackOriginnumList.clear();
    m_pauseCommandQueue.clear();
    m_waitingPppRead = false;
    m_waitingPrpRead = false;
    m_pendingPppReread = false;
    m_pendingPrpReread = false;
    m_offlineWaitingReconnect = false;
    m_offlineVoided = false;
    m_prepatchAckPaced = false;
    m_gripperRestartAwait = GripperRestartNone;
    m_gripperErrStartMs = 0;
    m_gripperFailPromptShown = false;
    m_resumeNextReagentAfterGripGiveup = false;
    m_hasPendingReagentDeduct = false;
    m_hasPendingCleanDeduct = false;
    mdataAArry.clear();
    m_lastOutboundCommands.clear();
    clearPendingConsumableDeducts();
}

void TestProjectProcess::prepareForNewTestRound()
{
    resetRuntimeForSessionStart();
    m_finishTestData.clear();
    m_pendingtimeSample.clear();
    m_pendingContinueReagent.clear();
}

void TestProjectProcess::logTestFsm(const QString &event, const QString &detail) const
{
    auto *instance = StructInstance::getInstance();
    quint8 focusActive = 0;
    if (m_focus_sample_id > 0 && instance) {
        focusActive = instance->read_focusSampleActive(m_focus_sample_id);
    }
    QLOG_WARN() << "[TestFsm]" << event
                << "sample" << m_focus_sample_id
                << "focus" << focusActionText(focusActive)
                << "outstanding" << m_outstandingAcks
                << "finishQ" << m_finishTestData.size()
                << "pppWait" << m_waitingPppRead
                << "prpWait" << m_waitingPrpRead
                << detail;
}

void TestProjectProcess::logTestFsmInfo(const QString &event, const QString &detail) const
{
    auto *instance = StructInstance::getInstance();
    quint8 focusActive = 0;
    if (m_focus_sample_id > 0 && instance) {
        focusActive = instance->read_focusSampleActive(m_focus_sample_id);
    }
    QLOG_INFO() << "[TestFsm]" << event
                << "sample" << m_focus_sample_id
                << "focus" << focusActionText(focusActive)
                << "outstanding" << m_outstandingAcks
                << detail;
}

void TestProjectProcess::setSampleFocus(int sampleId, quint8 newFocus, const QString &reason)
{
    if (sampleId <= 0) {
        return;
    }
    auto *instance = StructInstance::getInstance();
    const quint8 oldFocus = instance->read_focusSampleActive(sampleId);
    if (oldFocus != newFocus) {
        logTestFsmInfo(QStringLiteral("focus"),
                       tr("样本%1 %2 -> %3 (%4)")
                           .arg(sampleId)
                           .arg(focusActionText(oldFocus))
                           .arg(focusActionText(newFocus))
                           .arg(reason));
    }
    instance->write_focusSampleActive(sampleId, newFocus);
}

void TestProjectProcess::stallWithPause(const QString &event,
                                        const QString &userTips,
                                        const bool modalPrompt)
{
    logTestFsm(event, userTips);
    publishWorkflowStatus(userTips);
    promptsareDisplayed(TESTINGLOG, userTips, modalPrompt);
    if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
        emit pauseTesting();
    }
}

bool TestProjectProcess::shouldDeferSendForPause() const
{
    // 抓杯失败后的抬抓手/双针复位必须继续下发，不能被暂停缓存吞掉
    if (m_gripperErrStartMs > 0) {
        return false;
    }
    return cglobal::g_PauseTesting.load()
            && !m_Errorresetsfirst
            && StructInstance::getInstance()->sampleHadFocus();
}

void TestProjectProcess::appendPauseQueueFromFocusRecovery()
{
    auto *instance = StructInstance::getInstance();
    QByteArrayList recovery;
    if (instance && m_focus_sample_id > 0) {
        const quint8 focusActive = instance->read_focusSampleActive(m_focus_sample_id);
        recovery = instance->outputsIncompleteInstructionList(m_focus_sample_id, focusActive);
        if (recovery.isEmpty()) {
            const QByteArray first = instance->outputstheFirstIncompleteInstruction(
                        m_focus_sample_id, focusActive);
            if (!first.isEmpty()) {
                recovery.append(first);
            }
        }
    }
    if (recovery.isEmpty() && !m_lastOutboundCommands.isEmpty()) {
        recovery = m_lastOutboundCommands;
    } else if (recovery.isEmpty() && !mdataAArry.isEmpty()) {
        recovery.append(mdataAArry);
    }
    for (const QByteArray &cmd : recovery) {
        if (!cmd.isEmpty()) {
            m_pauseCommandQueue.append(cmd);
        }
    }
}

void TestProjectProcess::handleStartTheTestTask()
{
   resetRuntimeForSessionStart();
   StructInstance::getInstance()->SortemergencyattheTop();
   // 上一轮结束后的复位应答可能还在路上，先丢掉再发加样，避免命令号撞车后一直等应答。
   QPointer<TestProjectProcess> guard(this);
   QTimer::singleShot(300, this, [guard]() {
       if (!guard || !cglobal::g_StartTesting.load() || cglobal::g_PauseTesting.load()) {
           return;
       }
       guard->StartSampleAdditionWorkflow();
   });
}

void TestProjectProcess::slottestingaddsample()
{
    // 测试中加样也按急诊重排，下一份待吸样本才会插到普通待测前面
    StructInstance::getInstance()->SortemergencyattheTop();
    if(!StructInstance::getInstance()->sampleHadFocus())
    {
       StartSampleAdditionWorkflow();
       publishWorkflowStatus(tr("新样本加入，开始加样"));
    }
    else
    {
       publishWorkflowStatus(tr("新样本已加入，等待当前焦点完成"));
    }
}


void TestProjectProcess::remberSendPara(const QByteArray& outdata, Commandinfo& remember_data)
{
    // 添加长度校验防止越界访问
    if (outdata.size() < PROTOCOL_LENGTH) {
        QLOG_WARN() << "Invalid outdata length:" << outdata.size();
        return;
    }

    // 闭包函数提取重复逻辑
    auto extractByte = [&outdata](int position) -> quint8 {
        return static_cast<quint8>(outdata.at(position)); // 直接取二进制值
    };

    // 一次性提取所有字段
    remember_data.addr    = extractByte(SLAVE_ADDRESS);
    remember_data.func    = extractByte(FUNCTIONALMA);
    remember_data.numindex = extractByte(COMMANDNUMBER);

    // 优化日志输出（避免刷新流）
    QLOG_DEBUG() << QString::fromUtf8("下发串口指令：从机%1 功能码%2 命令号%3")
                    .arg(remember_data.addr)
                    .arg(remember_data.func)
                    .arg(remember_data.numindex);

    //m_timer->start(); // 启动定时
}


bool TestProjectProcess::comparedSendPara(const Commandinfo &remember_data, quint8 addr, quint8 func,  quint8 num){
    return (remember_data.addr == addr)
            && (remember_data.func == func)
            && (remember_data.numindex == num);
}



void TestProjectProcess::writeCommandHeader(const QByteArray data,const QString commandText,int latetime) {

    if(data.isEmpty()){
        QString outerr = QString::fromUtf8("指令%1为空,停止推进避免焦点悬挂").arg(commandText);
        QLOG_ERROR()<<outerr<<endl;
        publishWorkflowStatus(outerr);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        promptsareDisplayed(TESTINGLOG,outerr,true);
        return;
    }

    if (shouldDeferSendForPause()) {
        m_pauseCommandQueue.append(data);
        disarmAckWatchdog();
        QLOG_WARN() << "[抓放杯] 命令被暂停拦住，没有发给仪器 | " << commandText
                    << "命令号" << commandIndexOf(data)
                    << "缓存条数" << m_pauseCommandQueue.size();
        return;
    }

    publishWorkflowStatus(commandText);

    const int delay = std::max(0, latetime);
    QPointer<TestProjectProcess> guard(this);
    QTimer::singleShot(delay, this, [guard, data, commandText]() {
        if (!guard)
            return;
        if (guard->shouldDeferSendForPause()) {
            guard->m_pauseCommandQueue.append(data);
            return;
        }
        if (!cglobal::gserialConnecStatus.load()) {
            guard->logTestFsm(QStringLiteral("offline_drop_send"), commandText);
            if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
                emit guard->pauseTesting();
            }
            guard->publishWorkflowStatus(tr("仪器离线，已暂停发送"));
            return;
        }
        guard->m_lastOutboundCommands = {data};
        emit guard->WriteaSingleCommand(data,commandText);
        guard->noteCommandsSent(1);
    });

    mdataAArry = data;
}

void TestProjectProcess::missingReagentResetPause(){
    //先所有轴复位
    m_Errorresetsfirst = true;
    mbackOriginnumList.clear();
    QByteArrayList allAxisBackOrigin = GlobalData::letallAxisBackOrigin(mbackOriginnumList);
    sendInstructionsWithDelay(allAxisBackOrigin,DELAY_TIMENUM, "缺少试剂复位暂停");
    return;
}

void TestProjectProcess::sendInstructionsWithDelay(const QByteArrayList data_,int late_time,
                                                     const QString instructioninfo)
{
    if(data_.isEmpty()){
        return;
    }
    if (shouldDeferSendForPause()) {
        m_pauseCommandQueue.append(data_);
        disarmAckWatchdog();
        QLOG_DEBUG()<<"有样本焦点任务&&暂停指令数"<<m_pauseCommandQueue.size()<<endl;
        return;
    }

    QPointer<TestProjectProcess> guard(this);
    QTimer::singleShot(late_time, this, [guard, data_, instructioninfo]() {
        if (!guard || data_.isEmpty())
            return;
        if (guard->shouldDeferSendForPause()) {
            guard->m_pauseCommandQueue.append(data_);
            return;
        }
        if (!cglobal::gserialConnecStatus.load()) {
            guard->logTestFsm(QStringLiteral("offline_drop_batch"), instructioninfo);
            if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
                emit guard->pauseTesting();
            }
            guard->publishWorkflowStatus(tr("仪器离线，已暂停发送"));
            return;
        }
        guard->m_lastOutboundCommands = data_;
        emit guard->write_instructions_group(data_,instructioninfo);
        guard->promptsareDisplayed(TESTINGLOG,instructioninfo,false);
        guard->noteCommandsSent(data_.size());
    });
}




void TestProjectProcess::loadPedningtimeTask(){

    auto& structInstance = *StructInstance::getInstance();
    quint8 activeIndex = 0;
    const QList<int> pendingCopy = m_pendingtimeSample;
    for(int sampleId : pendingCopy){
         const bool hadPendtime = structInstance.continueTestPendTimeSample(activeIndex,sampleId);
         if(!hadPendtime) {
             QLOG_ERROR()<<"挂起样本"<<sampleId<<"启动失败";
             m_pendingtimeSample.removeOne(sampleId);
             continue;
         }
         const bool hadFocus = structInstance.sampleHadFocus();
         if(!hadFocus && m_finishTestData.isEmpty()){
             m_pendingtimeSample.removeOne(sampleId);
             structInstance.setSamplePendingtimeStatus(sampleId,false);
             structInstance.config_focusSample(sampleId ,true);
             m_focus_sample_id = sampleId;
             testSampleTryAgain(activeIndex,true);
             return;
         }
    }

    if (m_finishTestData.isEmpty() && !structInstance.sampleHadFocus()) {
        int remaining = 0;
        structInstance.SampleTotal(remaining);
        if (remaining == ALL_SAMPLE_TESTED) {
            prepareForNewTestRound();
            emit allSamplesFinished();
            structInstance.delalltaskinfo(false);
            structInstance.setemptynull();
        }
    }
}

void TestProjectProcess::handleNoPendingSamples()
{
    auto *instance = StructInstance::getInstance();
    const QString message = tr("无待加样样本");
    QLOG_INFO() << "[TestFsm]" << message;
    promptsareDisplayed(TESTINGLOG, message, false);

    if (!m_finishTestData.isEmpty() && instance && !instance->sampleHadFocus()) {
        logTestFsm(QStringLiteral("no_add_sample_drain_finish_q"),
                   tr("完成队列=%1").arg(m_finishTestData.size()));
        TraverseChnHadFinishTesting(THROW_CUPS_END);
        return;
    }

    loadPedningtimeTask();
}

// 分解子功能为独立方法
void TestProjectProcess::executeSampleAddition(int sampleId, StructInstance& instance)
{
    if (const auto *ps = instance.rfindIdSample(sampleId)) {
        if (QUIUtils::isBloodSampleSuckAtOrigin(ps->suckanaemia_axis)
                || QUIUtils::isBloodSampleSuckAtOrigin(ps->suckbloody_axis)) {
            const QString msg = QUIUtils::bloodSampleSuckOriginHint(
                        sampleId,
                        ps->suckanaemia_hole, ps->suckanaemia_axis,
                        ps->suckbloody_hole, ps->suckbloody_axis);
            QLOG_ERROR() << msg;
            promptsareDisplayed(TESTINGLOG, msg, false);
            publishWorkflowStatus(msg);
            emit sampleSuckPosInvalid(msg);
            emit pauseTesting();
            return;
        }
    }

    // 原子化状态更新
    instance.config_focusSample(sampleId, true);
    setSampleFocus(sampleId, FOCUS_ADDING_SAMPLE, tr("开始加样"));
    instance.updateSampleState(sampleId, ADDING_SAMPLE);
    logTestFsmInfo(QStringLiteral("add_sample_start"), tr("样本%1").arg(sampleId));
    if (const auto *ps = instance.rfindIdSample(sampleId)) {
        emit bloodSampleHintStage(ps->suckanaemia_hole, ps->suckbloody_hole,
                                  TestingConstants::BLOOD_HINT_ADDING);
    }

    const QByteArray  outdata =  instance.addSampleHeandCommand(sampleId);
    writeCommandHeader(outdata,tr("样本%1开始加样流程").arg(sampleId),DELAY_TIMENUM);
}


void TestProjectProcess::StartSampleAdditionWorkflow()
{
    // 获取单例实例并缓存
    auto& structInstance = *StructInstance::getInstance();

    const int sampleOpt  = structInstance.OutputPrepareAspirationNum();
    m_focus_sample_id = sampleOpt;

    if(sampleOpt <= 0){
        handleNoPendingSamples();
        return;
    }

    executeSampleAddition(sampleOpt,structInstance);
}


void TestProjectProcess::testSampleTryAgain(const quint8 indexActive,const std::atomic<bool>& ispendtime)
{
    auto* structInstance = StructInstance::getInstance();
    if (indexActive == FOCUS_TESTING_AA || indexActive == FOCUS_TESTING_ADP
            || indexActive == FOCUS_TESTING_EPI || indexActive == FOCUS_TESTING_COL
            || indexActive == FOCUS_TESTING_RIS) {
        if (ispendtime.load(std::memory_order_acquire)) {
            restartPrpAfterHome(reagentFromFocus(indexActive),
                                tr("挂起恢复后从试剂位重跑抓PRP"));
            return;
        }
    }

    // 抓杯失败重试：抓手复位且 XY 回原点后，再 XY→抓杯点→下抓（见 isGripperCupFailFocus）
    if (StructInstance::isGripperCupFailFocus(indexActive)) {
        if (!structInstance->nextIncompletePrepatch(m_focus_sample_id).isEmpty()) {
            const QString waitMsg = tr("抓手仍在复位中，请稍候再点「再次测试」");
            QLOG_WARN() << "[抓手异常] 样本" << m_focus_sample_id
                        << "| 善后还没做完就点了再次测试，已拒绝";
            promptsareDisplayed(TESTINGLOG, waitMsg, true);
            return;
        }
        const QByteArray gripRetry = structInstance->restartGripperFailTryAgain(
                    m_focus_sample_id, indexActive);
        if (gripRetry.isEmpty()) {
            const QString msg = tr("抓杯重试失败：无法从抓杯位置续跑，请放弃样本");
            QLOG_ERROR() << "[抓手异常] 样本" << m_focus_sample_id
                         << "| 再次测试重建失败，焦点步骤" << indexActive;
            promptsareDisplayed(TESTINGLOG, msg, true);
            emit theGripperFailed(m_focus_sample_id, msg, indexActive);
            return;
        }
        QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                    << "| 再次测试，重建抓杯/放杯，首条命令号"
                    << commandIndexOf(gripRetry)
                    << "焦点步骤" << indexActive;
        m_gripperRestartAwait = GripperRestartNone;
        m_needHomeBeforeResume = false;
        writeCommandHeader(gripRetry, tr("抓杯失败重试：先抬双针再至抓杯位"), DELAY_TIMENUM);
        return;
    }

    // 统一配置表：使用结构体封装处理逻辑
    struct ActionConfig {
        std::function<QByteArray(bool)> handler; // 使用lambda封装不同方法调用
        const char* description;
    };

    // 延迟初始化配置表
    QByteArray outdata;
    QString dataexplain;

    static const std::unordered_map<quint8,ActionConfig> configMap = {
    };

    // 统一查找逻辑
    std::lock_guard<std::mutex> lock(s_actionMutex);
	// 保证后续操作不重排必须实时读取最新值，改用强内存序约束
	const bool pending_loc = ispendtime.load(std::memory_order_acquire);
    QLOG_DEBUG() << "原子量" << pending_loc;
    std::unordered_map<quint8, ActionConfig>::const_iterator it = configMap.find(indexActive);
    if (it != configMap.cend()) {
        outdata = it->second.handler(pending_loc); // 调用lambda函数
        dataexplain = it->second.description;
    } else {
        const QString msg = tr("抓杯重试失败：当前步骤无法重试，请放弃样本");
        QLOG_ERROR() << msg << "焦点步骤" << indexActive << "样本" << m_focus_sample_id;
        promptsareDisplayed(TESTINGLOG, msg, true);
        emit theGripperFailed(m_focus_sample_id, msg, indexActive);
        return;
    }
    if (outdata.isEmpty()) {
        const QString msg = tr("抓杯重试失败：指令为空，请放弃样本");
        QLOG_ERROR() << msg << dataexplain << "样本" << m_focus_sample_id;
        promptsareDisplayed(TESTINGLOG, msg, true);
        emit theGripperFailed(m_focus_sample_id, msg, indexActive);
        return;
    }
    writeCommandHeader(outdata, dataexplain, DELAY_TIMENUM);
    return;
}



void TestProjectProcess::giveupSamplenum(const int sampleid){
     m_abandonAfterThrowPpp = false;
     m_prpZeroSkipAfterThrow = false;
     m_waitingPppRead = false;
     m_pendingPppReread = false;
     m_waitingPrpRead = false;
     m_pendingPrpReread = false;

     auto* instance = StructInstance::getInstance();

     const auto *itsample = instance->rfindIdSample(sampleid);

     if(itsample == nullptr) {
         QLOG_ERROR()<<"放弃样本"<<sampleid<<"失败";
         return ;
     }
    int idgiveupSample = itsample->indexTestingChn;

     //先把入队的任务踢出
     auto it = m_finishTestData.begin();
     while (it != m_finishTestData.end()) {
         if (it->index_Chn == idgiveupSample && it->CompleteSampleid == sampleid) {
             it = m_finishTestData.erase(it);
         }
         else {
             ++it;
         }
     }

     releaseSampleChannel(sampleid, true);

     //失去焦点
     dismissFocusSample(sampleid);
     removeSampleFromSchedulerQueues(sampleid);

     //标注取消样本选用试管
     QList<quint8> giveupHole;
     instance->labelCancelTubes(sampleid,giveupHole);
     emit giveupSampleHolesChanged(giveupHole);
     //+试管标记已使用掉
     for (quint8 hole : giveupHole) {
         Q_EMIT EmptyTubeCatched(hole);
     }

     //结构体删除样本
     instance->onetestend_del(sampleid);
     if (m_focus_sample_id == sampleid) {
         m_focus_sample_id = 0;
     }

     int remaining  = 0; //获取剩余样本数
     StructInstance::getInstance()->SampleTotal(remaining );

     if(remaining  == ALL_SAMPLE_TESTED){
         QLOG_DEBUG()<<"放弃样本后,测试列表为空!";
         prepareForNewTestRound();
         emit allSamplesFinished();
         instance->delalltaskinfo(false); //删除所有样本信息
         instance->setemptynull();
     }
     else {
         TraverseChnHadFinishTesting(THROW_CUPS_END);
     }

}

void TestProjectProcess::pendingTimeSample(const int sampleid){
    auto* instance = StructInstance::getInstance();
    auto *itsample = instance->wfindIdSample(sampleid);
    if(itsample == nullptr) {
        QLOG_ERROR()<<"挂起样本"<<sampleid<<"失败";
        return ;
    }
    int idgiveupSample = itsample->indexTestingChn;

    m_pendingtimeSample.push_back(sampleid);
    removeSampleFromSchedulerQueues(sampleid);
    //先把入队的任务踢出 暂存
    auto it = m_finishTestData.begin();
    while (it != m_finishTestData.end()) {
        if (it->index_Chn == idgiveupSample && it->CompleteSampleid == sampleid) {
            it = m_finishTestData.erase(it);
            break;
        }
        else {
            ++it;
        }
    }

    //失去焦点（杯子还在通道里，通道继续占用，等机械臂空闲再续跑）
    dismissFocusSample(sampleid);
    if (m_focus_sample_id == sampleid) {
        m_focus_sample_id = 0;
    }

    TraverseChnHadFinishTesting(THROW_CUPS_END);
}

void TestProjectProcess::handleGripErrGiveupSample(const bool isGiveupSample,const quint8 indexAxtive){
    auto* instance = StructInstance::getInstance();
    instance->clearRecvPrepatch(m_focus_sample_id);
    m_gripperErrStartMs = 0;
    m_gripperFailPromptShown = false;
    m_prepatchAckPaced = false;
    if(!isGiveupSample){
        instance->stopGripperVacuumPoll();
        stopGripperVacuumPollTimer();
        m_needHomeBeforeResume = false;
        m_outstandingAcks = 0;
        disarmAckWatchdog();
        m_pauseCommandQueue.clear();
        if (!m_gripperRecoveryFinished) {
            m_startClipAfterGripperRecovery = true;
            m_gripperFailPromptShown = false;
            m_prepatchAckPaced = false;
            instance->setHandsErrStatus(m_focus_sample_id, GRIPPERMOVE_ABNORMALLY);
            instance->clearRecvPrepatch(m_focus_sample_id);
            const QByteArrayList recovery =
                    GlobalData::errMachineBackOrigin(FOCUS_ERR_BACKORIGIN, MOTOR_HANDS_INDEX);
            instance->fillRecvPrepatch(m_focus_sample_id, recovery);
            m_gripperZStillDown = !recovery.isEmpty();
            m_gripperHomeCmdIndex = recovery.isEmpty() ? 0 : commandIndexOf(recovery.first());
            m_gripperErrStartMs = QDateTime::currentMSecsSinceEpoch();
            QLOG_WARN() << "[抓手异常] 样本" << m_focus_sample_id
                        << "| 再次测试时抓手善后还没做完(抓手可能还在下面)。"
                        << "先重做抓手复位，再抬双针，完成后再抓杯，这次不动XY";
            sendFirstPrepatchCommand(tr("再次测试前先完成抓手善后"));
            return;
        }
        instance->setHandsErrStatus(m_focus_sample_id, GRIPPERMOVE_NORMAL);
        cglobal::g_PauseTesting.store(false);
        emit requestPauseUiSync(false);
        testSampleTryAgain(indexAxtive, false);
    }else{
        QByteArrayList cleanData;
        QUIUtils::cleaningDoubleNeedleAction(cleanData,2);
        m_resumeNextReagentAfterGripGiveup = false;
        const quint8 failedReagent = reagentFromFocus(indexAxtive);
        if (failedReagent != 0) {
            quint8 failedHole = 0;
            if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
                for (Testing_reagents *item : ps->ptestReagent_active) {
                    if (item && item->index_Reagent == failedReagent) {
                        failedHole = item->indexHole;
                        break;
                    }
                }
            }
            markReagentSkipped(m_focus_sample_id, failedReagent);
            quint8 nextReagent = 0;
            if (instance->judge_hadWaitTestReag(m_focus_sample_id, nextReagent)) {
                m_resumeNextReagentAfterGripGiveup = true;
                if (failedHole > 0) {
                    emit EmptyTubeCatched(failedHole);
                }
                const QString failedName = GlobalData::mapIndexReagentnames(failedReagent);
                const QString nextName = GlobalData::mapIndexReagentnames(nextReagent);
                QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                            << "| 放弃当前" << failedName
                            << "，清洗后继续" << nextName;
                promptsareDisplayed(TESTINGLOG,
                                    tr("样本%1 当前%2抓杯失败已放弃，清洗后继续%3")
                                        .arg(m_focus_sample_id)
                                        .arg(failedName)
                                        .arg(nextName),
                                    false);
            }
        }
        if (!m_resumeNextReagentAfterGripGiveup) {
            instance->setSampleGiveupStatus(m_focus_sample_id,SAMPLE_GIVEUP);
        }
        instance->fillRecvPrepatch(m_focus_sample_id,cleanData);
        if (instance->nextIncompletePrepatch(m_focus_sample_id).isEmpty()) {
            QLOG_WARN() << QString::fromUtf8("放弃样本时双针清洗指令为空，直接结束当前步骤")
                        << m_focus_sample_id;
            instance->setHandsErrStatus(m_focus_sample_id, GRIPPERMOVE_NORMAL);
            FullyAutomatedPlatelets::pinstanceinstrument()->LosserOneReagentul(
                        INDEX_CLEANLINQUE_CONSUMABLE, 2);
            if (m_resumeNextReagentAfterGripGiveup) {
                m_resumeNextReagentAfterGripGiveup = false;
                cglobal::g_PauseTesting.store(false);
                emit requestPauseUiSync(false);
                if (!beginNextReagentForSample(m_focus_sample_id)) {
                    giveupSamplenum(m_focus_sample_id);
                }
            } else {
                giveupSamplenum(m_focus_sample_id);
            }
        } else {
            sendFirstPrepatchCommand(m_resumeNextReagentAfterGripGiveup
                                     ? QString::fromUtf8("放弃当前PRP，清洗后继续下一个试剂")
                                     : QString::fromUtf8("抓手异常取消测试任务"));
        }
    }
}

//样本任务挂起
void TestProjectProcess::handlePendingtimeoutSampleTest(const quint8 indexAxtive) {
    auto* instance = StructInstance::getInstance();
    instance->clearRecvPrepatch(m_focus_sample_id);
    m_gripperErrStartMs = 0;
    m_gripperFailPromptShown = false;

    //挂起后没判断在做什么就清洗双针
    QByteArrayList cleanData;
    QUIUtils::cleaningDoubleNeedleAction(cleanData,2);


    instance->setSamplePendingtimeStatus(m_focus_sample_id,true); //设置挂起状态
    instance->fillRecvPrepatch(m_focus_sample_id,cleanData);
    if (instance->nextIncompletePrepatch(m_focus_sample_id).isEmpty()) {
        QLOG_WARN() << QString::fromUtf8("挂起样本时双针清洗指令为空，直接挂起") << m_focus_sample_id;
        instance->setHandsErrStatus(m_focus_sample_id, GRIPPERMOVE_NORMAL);
        FullyAutomatedPlatelets::pinstanceinstrument()->LosserOneReagentul(
                    INDEX_CLEANLINQUE_CONSUMABLE, 2);
        pendingTimeSample(m_focus_sample_id);
    } else {
        sendFirstPrepatchCommand(QString::fromUtf8("抓手挂起测试任务动作编号:%1").arg(indexAxtive));
    }
}


bool TestProjectProcess::gripperExceptionHandling(const quint8 commandIndex,const quint8 indexActive ){
    auto* instance = StructInstance::getInstance();
    auto* instrument = FullyAutomatedPlatelets::pinstanceinstrument();
    const bool gripperError = instance->getHandsStatus(m_focus_sample_id);
    if(gripperError == GRIPPERMOVE_NORMAL) return false;

    if (m_gripperZStillDown && commandIndex == m_gripperHomeCmdIndex) {
        m_gripperZStillDown = false;
        QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                    << "| 抓手Z复位应答已到，命令号" << commandIndex;
    }

    QByteArray nextPrepatch;
    const bool activeFinished = instance->recvPrepatchActions(m_focus_sample_id,commandIndex, &nextPrepatch);
    if(activeFinished){
        m_gripperErrStartMs = 0;
        m_prepatchAckPaced = false;
        const bool sampleGiveUp = instance->getSampleGiveupStatus(m_focus_sample_id);
        const bool samplePendingtime = instance->getSamplePendingtimeStatus(m_focus_sample_id);
        m_gripperRecoveryFinished = true;
        if (m_resumeNextReagentAfterGripGiveup && !samplePendingtime) {
            m_resumeNextReagentAfterGripGiveup = false;
            m_gripperFailPromptShown = true;
            instance->setHandsErrStatus(m_focus_sample_id, GRIPPERMOVE_NORMAL);
            m_pauseCommandQueue.clear();
            cglobal::g_PauseTesting.store(false);
            emit requestPauseUiSync(false);
            const int sampleId = m_focus_sample_id;
            QLOG_INFO() << "[抓手异常] 样本" << sampleId
                        << "| 放弃当前PRP后清洗完成，继续下一个试剂";
            if (!beginNextReagentForSample(sampleId)) {
                giveupSamplenum(sampleId);
            }
            return true;
        }
        if (m_startClipAfterGripperRecovery && !sampleGiveUp && !samplePendingtime) {
            m_startClipAfterGripperRecovery = false;
            m_gripperFailPromptShown = true;
            instance->setHandsErrStatus(m_focus_sample_id, GRIPPERMOVE_NORMAL);
            m_pauseCommandQueue.clear();
            cglobal::g_PauseTesting.store(false);
            emit requestPauseUiSync(false);
            QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                        << "| 善后完成(抓手已抬、双针已复位)，开始再次抓杯";
            testSampleTryAgain(indexActive, false);
            return true;
        }
        if(!sampleGiveUp && !samplePendingtime){
            if (!m_gripperFailPromptShown) {
                m_gripperFailPromptShown = true;
                QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                            << "| 善后完成(抓手已抬、双针已复位、XY已回)，弹出人工确认 | 原因:" << m_gripperErrTips;
                emit theGripperFailed(m_focus_sample_id, m_gripperErrTips, indexActive);
            }
            return true;
        }
        else if(sampleGiveUp && !samplePendingtime){
            QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                        << "| 善后完成，执行放弃样本";
            instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_NORMAL);
            instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);
            giveupSamplenum(m_focus_sample_id);//放弃样本
        }else if(!sampleGiveUp && samplePendingtime){
            QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                        << "| 善后完成，执行挂起样本";
            instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_NORMAL);
            instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);
            pendingTimeSample(m_focus_sample_id);
        }
    } else if (m_prepatchAckPaced && !nextPrepatch.isEmpty()) {
        const QString tips = m_gripperErrTips.isEmpty()
                ? QString::fromUtf8("抓手异常清洗")
                : m_gripperErrTips;
        m_gripperErrStartMs = QDateTime::currentMSecsSinceEpoch();
        QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                    << "| 善后应答命令号" << commandIndex
                    << "→ 下发下一步命令号" << commandIndexOf(nextPrepatch);
        writeCommandHeader(nextPrepatch, tips, DELAY_TIMENUM);
        return true;
    }
    if (m_gripperFailPromptShown) {
        return true;
    }
    if (!instance->nextIncompletePrepatch(m_focus_sample_id).isEmpty()) {
        return true;
    }
    QLOG_WARN() << "[抓手异常] 样本" << m_focus_sample_id
                << "| 收到对不上的应答，命令号" << commandIndex
                << "，已忽略，善后继续等";
    return true;
}


//接收解消息
void TestProjectProcess::recvEquipmentdata(const QStringList recvData)
{
    quint8 slaveAddr =   GlobalData::analyzeSlave_addr(recvData,false);//从机地址
    quint8 commandIndex = GlobalData::analyzeCommandIndex(recvData); //命令编号

    if (!cglobal::g_StartTesting.load()) {
        return;
    }

    if (tryRecvGripperVacuumProbeAck(recvData, commandIndex)) {
        return;
    }

    if (m_outstandingAcks <= 0 && !m_waitingPppRead && !m_waitingPrpRead
            && !m_Errorresetsfirst) {
        logTestFsm(QStringLiteral("ignore_stale_ack"),
                   tr("cmdIndex=%1").arg(commandIndex));
        return;
    }

    if (m_Errorresetsfirst) {
        noteAckReceived();
        if (!mbackOriginnumList.removeOne(commandIndex)) {
            QLOG_DEBUG() << QString::fromUtf8("复位期间忽略残留应答 命令号") << commandIndex;
            return;
        }
        if (mbackOriginnumList.isEmpty()) {
            m_Errorresetsfirst = false;
            exceptionRetest(QString::fromUtf8("全轴复位完成继续测试"));
        }
        return;
    }


    //完成状态
    int commandMode, run_status,speedMode ,Action_motor;
    GlobalData::Parse4thBytecommandstatus(recvData,commandMode,run_status,speedMode,Action_motor);

    if (run_status == MOTOR_WAITING || run_status == MOTOR_RUNNING) {
        return;
    }

    QString showErrorlog;
    if (run_status != MOTOR_FINISH) {
        QString error_code = recvData.join(" ");
        GlobalData::commandStatus(run_status, showErrorlog);
        int errorbit = 0;
        if (!recvData.isEmpty()) {
            errorbit = recvData.last().toInt(nullptr, 16);
        }
        QString motorHint = QUIUtils::MotorStateMapStr(
                    static_cast<quint8>(run_status), errorbit);
        if (errorbit == 2 && m_gripperZStillDown) {
            motorHint = QString::fromUtf8("执行异常:抓手Z未复位(抓手还在下面),xy执行错误");
        }
        QString tips = showErrorlog;
        const int hintColon = motorHint.indexOf(QLatin1Char(':'));
        if (hintColon >= 0) {
            tips += motorHint.mid(hintColon);
        }
        logTestFsm(QStringLiteral("motor_error_ack"), tips);
        publishWorkflowStatus(tr("命令异常已暂停: %1").arg(tips));
        m_hasPendingCleanDeduct = false;
        m_outstandingAcks = 0;
        m_needHomeBeforeResume = true;
        m_pauseCommandQueue.clear();
        clearPppPreProbeRecoveryState();
        disarmAckWatchdog();
        emit abortPendingEquipmentIo();
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        promptsareDisplayed(TESTINGLOG, tips, true);
        return;
    }

    noteAckReceived();

    const quint8 indexActive = StructInstance::getInstance()->read_focusSampleActive(m_focus_sample_id); 

    if(gripperExceptionHandling(commandIndex,indexActive)){
        return;
    }


    switch(indexActive){
        case FOCUS_ADDING_SAMPLE:{
            //收到加样动作指令
            receiveInstructionsSampleactive(commandIndex, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLIP_ANEMIA_TO_CHN:{
            receiveInstructionsPPPCuptoChannel(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
        case FOCUS_THORW_ANEMIA:{
            recv_throwAnemia(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }

       case FOCUS_TESTING_AA:{
            recvNextTestAA(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_ADP:{
            recvNextTestADP(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_EPI:{
            recvNextTestEPI(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_COL:{
            recvNextTestCOL(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_RIS:{
            recvNextTestRIS(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }


        case FOCUS_SPIT_AA_TESTING: {
            recvSpitReagent(indexActive,commandIndex,AA_REAGENT);
            break;
        }
        case FOCUS_SPIT_ADP_TESTING: {
            recvSpitReagent(indexActive,commandIndex,ADP_REAGENT);
            break;
        }

        case FOCUS_SPIT_EPI_TESTING: {
            recvSpitReagent(indexActive,commandIndex,EPI_REAGENT);
            break;
        }
        case FOCUS_SPIT_COL_TESTING: {
            recvSpitReagent(indexActive,commandIndex,COL_REAGENT);
            break;
        }
        case FOCUS_SPIT_RIS_TESTING:{
            recvSpitReagent(indexActive,commandIndex,RIS_REAGENT);
            break;
        }
        case FOCUS_CLEAN_DOUBLEPIN:{
            recv_cleanDoublePin(commandIndex, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_AA:{
            recv_cleanReagentPin(commandIndex, AA_REAGENT, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_ADP:{
            recv_cleanReagentPin(commandIndex, ADP_REAGENT, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_EPI:{
            recv_cleanReagentPin(commandIndex, EPI_REAGENT, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_COL:{
            recv_cleanReagentPin(commandIndex, COL_REAGENT, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_RIS:{
            recv_cleanReagentPin(commandIndex, RIS_REAGENT, slaveAddr, Action_motor, recvData);
            break;
        }
        case FOCUS_CLEAN_BLOODPIN:{
            recv_cleanBloodPin(commandIndex, slaveAddr, Action_motor, recvData);
            break;
        }

        case FOCUS_THROW_AA_TUBE:
        case FOCUS_THROW_ADP_TUBE:
        case FOCUS_THROW_EPI_TUBE:
        case FOCUS_THROW_COL_TUBE:
        case FOCUS_THROW_RIS_TUBE:
        {
            recv_throwCup(indexActive,commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
            default:
                break;
        }


}




void TestProjectProcess::getFreeTimeTestChn()
{
    auto* instance = StructInstance::getInstance();
    quint8 freeChannel = 0;
    //const bool hasFreeChannel = instance->hadFreeTestChn(freeChannel);
    const bool hasFreeChannel = instance->getNextFreeChnAfterTesting(freeChannel);

    if (hasFreeChannel) {
        instance->setTestingChn(m_focus_sample_id, freeChannel);
        emit openChnMotor(freeChannel, true);
        logTestFsmInfo(QStringLiteral("ppp_to_chn"),
                       tr("通道%1").arg(freeChannel + 1));
        setSampleFocus(m_focus_sample_id, FOCUS_CLIP_ANEMIA_TO_CHN, tr("加样完成夹PPP"));
        instance->stopGripperVacuumPoll();
        stopGripperVacuumPollTimer();
        writeCommandHeader(instance->sendPPP2ChannelHeanderCommand(m_focus_sample_id,false),
                    tr("吸PPP试管到测试通道首命令"),DELAY_TIMENUM);
    } else {
        logTestFsmInfo(QStringLiteral("no_free_chn_wash_blood"),
                       tr("等待通道，先洗样本针"));
        setSampleFocus(m_focus_sample_id, FOCUS_CLEAN_BLOODPIN, tr("无空闲通道"));
        markPendingCleanDeduct(1); // BLOOD_PIN
        startCleanNeedleAckMode(QString::fromUtf8("洗样本针"));
    }
    return;
}




void TestProjectProcess::throwAnemiaCup(quint8 testChn)
{
    // 实验模式才拦 PPP 范围；正常模式读到值就弃杯继续。PPP 连续为 0 放弃样本时强制丢杯
    int anemiaValue = 0;
    StructInstance::getInstance()->oper_AnemiaValue(READ_OPERRAT, testChn, anemiaValue);
    if (!m_abandonAfterThrowPpp
            && INI_File().getexperimentalMode()
            && !GlobalData::isPppBaselineValid(anemiaValue)) {
        QLOG_ERROR() << "throwAnemiaCup: PPP异常" << anemiaValue
                     << "通道" << (testChn + 1) << "拒绝弃杯";
        m_pendingPppReread = true;
        m_waitingPppRead = false;
        disarmAckWatchdog();
        const QString tips = tr("实验模式 PPP 初值异常，已暂停，请处理后点「继续」重读");
        publishWorkflowStatus(tips);
        promptsareDisplayed(TESTINGLOG, tips, false);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        return;
    }
    m_waitingPppRead = false;
    m_pendingPppReread = false;
    disarmAckWatchdog();

    setSampleFocus(m_focus_sample_id, FOCUS_THORW_ANEMIA, tr("PPP初值OK弃杯"));
    logTestFsmInfo(QStringLiteral("throw_ppp"), tr("通道%1").arg(testChn + 1));

    StructInstance::getInstance()->stopGripperVacuumPoll();
    stopGripperVacuumPollTimer();

    QByteArray sendHeandArry = StructInstance::getInstance()->outputHeadnThrowCups(m_focus_sample_id,false);
    writeCommandHeader(sendHeandArry,tr("弃杯动作发送首命令"),DELAY_TIMENUM);
    return;
}


void TestProjectProcess::FindSuckReagentAxis(QPoint moveingloc, const int &indexSuckReagent)
{
	static const int kReagentIndices[] = {
        INDEX_AA_CONSUMABLE,  INDEX_ADP_CONSUMABLE, INDEX_EPI_CONSUMABLE,
        INDEX_COL_CONSUMABLE, INDEX_RIS_CONSUMABLE, INDEX_AA_1_CONSUMABLE,
        INDEX_ADP_1_CONSUMABLE, INDEX_EPI_1_CONSUMABLE,
        INDEX_COL_1_CONSUMABLE, INDEX_RIS_1_CONSUMABLE
    };
	constexpr int MAX_REAGENT_INDEX = sizeof(kReagentIndices) / sizeof(kReagentIndices[0]) - 1;

	// 获取试剂索引
	quint8 testingReag = SingletonAxis::GetInstance()->outPutLossReagentIndex(moveingloc);

	const QString logPrefix = QStringLiteral("[FindSuckReagentAxis]");
	if (testingReag > MAX_REAGENT_INDEX) {
		QLOG_ERROR() << logPrefix << "无效试剂索引:" << testingReag
            << "，最大允许值:" << MAX_REAGENT_INDEX<<"赋值"<<indexSuckReagent;
        testingReag = indexSuckReagent - 1;
	}

    // P1：仅记录待扣瓶位，等吸试剂+抓杯流程成功完成后再扣减
    m_pendingConsumableIndex = static_cast<quint8>(kReagentIndices[testingReag]);
    m_hasPendingReagentDeduct = true;
    QLOG_DEBUG() << logPrefix << "记录待扣耗材索引:" << m_pendingConsumableIndex
                 << "试剂逻辑号:" << indexSuckReagent;
}

void TestProjectProcess::deductPendingReagentConsumable()
{
    if (!m_hasPendingReagentDeduct) {
        return;
    }
    if (auto* instrument = FullyAutomatedPlatelets::pinstanceinstrument()) {
        instrument->LosserOneReagentul(m_pendingConsumableIndex, 0);
        QLOG_DEBUG() << "吸试剂流程成功，扣减耗材索引:" << m_pendingConsumableIndex;
    } else {
        QLOG_ERROR() << "扣减耗材失败：仪器单例为空";
    }
    m_hasPendingReagentDeduct = false;
}

void TestProjectProcess::markPendingCleanDeduct(quint8 depleteType)
{
    m_pendingCleanDepleteType = depleteType;
    m_hasPendingCleanDeduct = true;
    QLOG_DEBUG() << "记录待扣清洗液类型:" << depleteType;
}

void TestProjectProcess::deductPendingCleanConsumable()
{
    if (!m_hasPendingCleanDeduct) {
        return;
    }
    if (auto* instrument = FullyAutomatedPlatelets::pinstanceinstrument()) {
        instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE, m_pendingCleanDepleteType);
        QLOG_DEBUG() << "洗针完成，扣减清洗液类型:" << m_pendingCleanDepleteType;
    } else {
        QLOG_ERROR() << "扣减清洗液失败：仪器单例为空";
    }
    m_hasPendingCleanDeduct = false;
}

void TestProjectProcess::clearPendingConsumableDeducts()
{
    m_hasPendingReagentDeduct = false;
    m_hasPendingCleanDeduct = false;
}

QString TestProjectProcess::focusActionText(quint8 focusActive) const
{
    return s_focusActionMap.value(focusActive, tr("未知FOCUS(%1)").arg(focusActive));
}

void TestProjectProcess::publishWorkflowStatus(const QString& detail)
{
    QString status;
    auto* instance = StructInstance::getInstance();
    quint8 focusActive = 0;
    if (m_focus_sample_id > 0 && instance)
        focusActive = instance->read_focusSampleActive(m_focus_sample_id);

    if (m_focus_sample_id > 0 && focusActive != 0) {
        const QString action = focusActionText(focusActive);
        status = tr("样本%1  %2").arg(m_focus_sample_id).arg(action);
        const bool alertDetail = detail.contains(tr("超时"))
                || detail.contains(tr("异常"))
                || detail.contains(tr("暂停"))
                || detail.contains(tr("失败"))
                || detail.contains(tr("为空"));
        if (alertDetail && !detail.isEmpty() && !status.contains(detail))
            status += QString::fromLatin1("  ") + detail;
    } else if (cglobal::g_StartTesting.load()) {
        status = detail.isEmpty() ? tr("调度中") : detail;
    } else {
        status = tr("待机");
    }

    if (cglobal::g_PauseTesting.load() && !status.contains(tr("暂停")))
        status += tr("  （已暂停）");

    if (status != m_lastWorkflowStatusLog) {
        m_lastWorkflowStatusLog = status;
        emit workflowStatusChanged(status);
    }
}

void TestProjectProcess::captureReconnectSnapshot()
{
    auto* instance = StructInstance::getInstance();
    m_reconnectSnapshotValid = true;
    m_reconnectHadFocus = instance && instance->sampleHadFocus();
    m_reconnectSampleId = m_focus_sample_id;
    m_reconnectFocusActive = 0;
    m_reconnectTestChn = 0;
    if (m_reconnectHadFocus && m_reconnectSampleId > 0 && instance) {
        m_reconnectFocusActive = instance->read_focusSampleActive(m_reconnectSampleId);
        instance->sycn_sampleneed_data(m_reconnectSampleId, m_reconnectTestChn);
    }
    QLOG_DEBUG() << "重连快照: sample=" << m_reconnectSampleId
                 << " focus=" << m_reconnectFocusActive
                 << " chn=" << static_cast<int>(m_reconnectTestChn)
                 << " hadFocus=" << m_reconnectHadFocus;
}

void TestProjectProcess::applyReconnectSnapshotAfterReset(const QString& tips)
{
    if (!m_reconnectSnapshotValid) {
        return;
    }

    // 复位后尽量恢复通道搅拌，避免光学采数阶段掉速
    if (m_reconnectHadFocus && m_reconnectSampleId > 0) {
        if (m_focus_sample_id != m_reconnectSampleId) {
            m_focus_sample_id = m_reconnectSampleId;
        }
        if (m_reconnectTestChn < m_totalChannel) {
            emit openChnMotor(m_reconnectTestChn, true);
        }
        publishWorkflowStatus(tr("重连续跑: %1 (FOCUS=%2)")
                              .arg(tips)
                              .arg(focusActionText(m_reconnectFocusActive)));
    } else {
        publishWorkflowStatus(tr("重连后续调度: %1").arg(tips));
    }

    m_reconnectSnapshotValid = false;
}

quint8 TestProjectProcess::reagentFromFocus(quint8 focusActive) const
{
    for (auto it = s_reagentWorkflow.cbegin(); it != s_reagentWorkflow.cend(); ++it) {
        if (it.value().testingFocus == focusActive
                || it.value().spitFocus == focusActive
                || it.value().throwFocus == focusActive) {
            return it.key();
        }
    }
    return 0;
}

void TestProjectProcess::enqueueOfflineVoidedReagent(int sampleId, quint8 chn, quint8 reagent)
{
    if (sampleId <= 0 || reagent == 0 || reagent == ANEMIA) {
        return;
    }
    for (const Finish_Data_ &item : m_finishTestData) {
        if (item.CompleteSampleid == sampleId
                && item.index_Chn == chn
                && item.index_reagent == reagent) {
            return;
        }
    }
    markReagentSkipped(sampleId, reagent);
    Finish_Data_ task;
    task.CompleteSampleid = sampleId;
    task.index_Chn = chn;
    task.index_reagent = reagent;
    m_finishTestData.enqueue(task);
    emit stopChannelAcquisition(chn);
    emit setprogressinitboolwaittext(true, chn, tr("[作废]"));
    promptsareDisplayed(TESTINGLOG,
                        tr("样本%1 通道%2 %3 掉线超时已作废")
                            .arg(sampleId)
                            .arg(chn + 1)
                            .arg(GlobalData::mapIndexReagentnames(reagent)),
                        false);
}

void TestProjectProcess::handleInstrumentOffline()
{
    disarmAckWatchdog();
    if (m_offlineVoidTimer) {
        m_offlineVoidTimer->stop();
    }
    m_offlineWaitingReconnect = true;

    if (m_waitingPppRead) {
        m_waitingPppRead = false;
        m_pendingPppReread = true;
    }
    if (m_waitingPrpRead) {
        m_waitingPrpRead = false;
        m_pendingPrpReread = true;
    }

    if (!cglobal::g_StartTesting.load()) {
        return;
    }
    m_offlineVoided = false;
    const QString tips = tr("仪器掉线，测试通道已清值，等待重连。超过30秒将作废当前试剂（杯子留在孔内，重连后再弃杯）");
    promptsareDisplayed(TESTINGLOG, tips, false);
    publishWorkflowStatus(tips);
    if (m_offlineVoidTimer) {
        m_offlineVoidTimer->start(OFFLINE_VOID_REAGENT_MS);
    }
}

void TestProjectProcess::handleInstrumentCameOnline()
{
    if (m_offlineVoidTimer) {
        m_offlineVoidTimer->stop();
    }
    if (!cglobal::g_StartTesting.load()) {
        m_offlineWaitingReconnect = false;
        m_offlineVoided = false;
    }
}

void TestProjectProcess::handleOfflineVoidTimeout()
{
    if (!cglobal::g_StartTesting.load()) {
        return;
    }
    m_offlineVoided = true;
    emit abortAllOpenChannelAcquisition();

    const QString tips = tr("掉线超过30秒，作废当前试剂（杯子留在孔内，重连后再弃杯）");
    promptsareDisplayed(TESTINGLOG, tips, true);
    publishWorkflowStatus(tips);

    auto* instance = StructInstance::getInstance();
    if (!instance) {
        return;
    }

    for (TESTCHNSTAUSINFO *chnInfo : instance->m_testChnStructvec_) {
        if (!chnInfo || chnInfo->Chn_Status != CHN_STATUS_TESTING) {
            continue;
        }
        const quint8 chnReag = chnInfo->Reagent;
        if (chnReag == ANEMIA) {
            continue;
        }
        QString sampleName;
        int sampleId = 0;
        instance->rootTestChnGetSampleInfo(chnInfo->index_Chn, sampleName, sampleId);
        if (sampleId > 0) {
            enqueueOfflineVoidedReagent(sampleId, chnInfo->index_Chn, chnReag);
        }
    }

    const quint8 focus = (m_focus_sample_id > 0)
            ? instance->read_focusSampleActive(m_focus_sample_id) : 0;
    if (m_pendingPppReread || m_waitingPppRead || focus == FOCUS_CLIP_ANEMIA_TO_CHN) {
        m_waitingPppRead = false;
        m_pendingPppReread = false;
        m_abandonAfterThrowPpp = true;
        promptsareDisplayed(TESTINGLOG,
                            tr("样本%1 掉线超时，通道PPP作废，重连后弃杯并停止该样本")
                                .arg(m_focus_sample_id),
                            false);
    }

    quint8 prpReagent = m_pendingPrpReagent;
    if (prpReagent == 0) {
        prpReagent = reagentFromFocus(focus);
    }
    const bool prpCupInChannel = m_pendingPrpReread || m_waitingPrpRead
            || focus == FOCUS_TESTING_AA || focus == FOCUS_TESTING_ADP
            || focus == FOCUS_TESTING_EPI || focus == FOCUS_TESTING_COL
            || focus == FOCUS_TESTING_RIS
            || focus == FOCUS_SPIT_AA_TESTING || focus == FOCUS_SPIT_ADP_TESTING
            || focus == FOCUS_SPIT_EPI_TESTING || focus == FOCUS_SPIT_COL_TESTING
            || focus == FOCUS_SPIT_RIS_TESTING;
    if (prpCupInChannel && prpReagent != 0 && m_focus_sample_id > 0) {
        m_waitingPrpRead = false;
        m_pendingPrpReread = false;
        quint8 chn = 0;
        instance->sycn_sampleneed_data(m_focus_sample_id, chn);
        enqueueOfflineVoidedReagent(m_focus_sample_id, chn, prpReagent);
        m_pendingPrpReagent = 0;
        m_prpZeroSkipAfterThrow = true;
    }
}

void TestProjectProcess::recoverAfterOfflineVoid(const QString &tips)
{
    Q_UNUSED(tips);
    m_offlineVoided = false;
    m_offlineWaitingReconnect = false;
    m_pauseCommandQueue.clear();
    promptsareDisplayed(TESTINGLOG, tr("重连：处理掉线超时作废的试剂，开始弃杯"), false);

    if (m_abandonAfterThrowPpp) {
        if (m_focus_sample_id <= 0 && m_reconnectSampleId > 0) {
            m_focus_sample_id = m_reconnectSampleId;
        }
        quint8 chn = 0;
        if (m_focus_sample_id > 0) {
            StructInstance::getInstance()->sycn_sampleneed_data(m_focus_sample_id, chn);
        }
        throwAnemiaCup(chn);
        return;
    }

    if (startPendingContinueReagent()) {
        return;
    }
    if (!m_finishTestData.isEmpty()) {
        processFinishedChannelTask();
        return;
    }

    TraverseChnHadFinishTesting(THROW_CUPS_END);
}


bool TestProjectProcess::controlsuckReagentLoc(quint8 primaryReagentIndex,quint8 testingReagent)
{
    // 配置缓存优化（类初始化时加载）
    const bool useAlternateReagent = INI_File().rConfigPara(USINGASIDEREAGENTLOC).toBool();

    // 试剂仓映射表（可扩展设计）
    static const QMap<quint8, quint8> REAGENT_MAPPING {
        {INDEX_AA_CONSUMABLE,  INDEX_AA_1_CONSUMABLE},
        {INDEX_ADP_CONSUMABLE, INDEX_ADP_1_CONSUMABLE},
        {INDEX_EPI_CONSUMABLE, INDEX_EPI_1_CONSUMABLE},
        {INDEX_COL_CONSUMABLE, INDEX_COL_1_CONSUMABLE},
        {INDEX_RIS_CONSUMABLE, INDEX_RIS_1_CONSUMABLE}
    };

    // 安全获取备用仓索引
    if (!REAGENT_MAPPING.contains(primaryReagentIndex)) {
        QLOG_ERROR() << "Invalid primary reagent index:" << primaryReagentIndex;
        return false;
    }
    const quint8 alternateReagentIndex = REAGENT_MAPPING.value(primaryReagentIndex);

    // 统一余量检测接口
    auto checkReagentSufficiency = [](quint8 reagentIndex, quint8 required) {
        return GlobalData::determineReagentontheleftIsSufficient(reagentIndex, required);
    };

    const bool isPrimarySufficient = checkReagentSufficiency(primaryReagentIndex, testingReagent);
    const bool isAlternateSufficient = checkReagentSufficiency(alternateReagentIndex, testingReagent);

    // 决策矩阵处理
    if(useAlternateReagent) {
       bool  EnoughsSuppile = handleAlternateReagentStrategy(primaryReagentIndex, testingReagent,
                                      isPrimarySufficient, isAlternateSufficient);
       return EnoughsSuppile;
    } else {
        //默认左边试剂位置 -先复位-再提示
        bool isEnoughSuppile =  handlePrimaryOnlyStrategy(isPrimarySufficient, primaryReagentIndex);
        return isEnoughSuppile;
    }
    return true;
}

bool  TestProjectProcess::handlePrimaryOnlyStrategy(bool isPrimarySufficient,const quint8& index){
    if(!isPrimarySufficient){
        missingReagentResetPause();
        emit pauseSycnMainuiUpdate(index);
        return false;
    }
    return true;
}

// 分离策略处理函数
bool TestProjectProcess::handleAlternateReagentStrategy(quint8 primary,
                                                        quint8 alternate,
                                                        bool isPrimarySufficient,
                                                        bool isAlternateSufficient)
{
    if(!isPrimarySufficient && !isAlternateSufficient) {
        triggerConsumablesPause(primary, alternate);
        QLOG_WARN()<<"试剂和预留位置剩余试剂不足"<<primary;
        return false;
    } else if(!isPrimarySufficient) {
       const bool switched = StructInstance::getInstance()->changesuckReagentFirstCommandLocation(
                   m_focus_sample_id, alternate);
       if (!switched) {
           QLOG_ERROR() << "左侧试剂不足，改吸右侧失败，已暂停" << primary;
           triggerConsumablesPause(primary, alternate);
           return false;
       }
       promptsareDisplayed(TESTINGLOG,
                           tr("%1左侧低于报警限，本针改吸右侧预留位")
                           .arg(GlobalData::mapIndexReagentnames(alternate)),
                           false);
    }
    return true;
}

// 统一暂停触发机制
void TestProjectProcess::triggerConsumablesPause(quint8 primary, quint8 alternate)
{
    Q_UNUSED(alternate);
    missingReagentResetPause();
    emit pauseSycnMainuiUpdate(primary);
}

bool TestProjectProcess::isHandsControl(int slaveAddr, int Action_motor) const {
    return (slaveAddr == Z_AXIS && Action_motor == MOTOR_HANDS_INDEX);
}
bool TestProjectProcess::isMainControlMotorXY(int slaveAddr, int Action_motor) const {
    return (slaveAddr == MAIN_CONTROL && Action_motor == MOTOR_XY);
}

const QMap<quint8, TestProjectProcess::FocusReagentPair> TestProjectProcess::FOCUS_MAPPING = []{
    QMap<quint8, FocusReagentPair> map;  // C++11 Lambda 初始化静态常量
    map.insert(FOCUS_TESTING_AA,  qMakePair(FOCUS_SPIT_AA_TESTING,  AA_REAGENT));
    map.insert(FOCUS_TESTING_ADP, qMakePair(FOCUS_SPIT_ADP_TESTING, ADP_REAGENT));
    map.insert(FOCUS_TESTING_EPI, qMakePair(FOCUS_SPIT_EPI_TESTING, EPI_REAGENT));
    map.insert(FOCUS_TESTING_COL, qMakePair(FOCUS_SPIT_COL_TESTING, COL_REAGENT));
    map.insert(FOCUS_TESTING_RIS, qMakePair(FOCUS_SPIT_RIS_TESTING, RIS_REAGENT));
    return map;
}();



void TestProjectProcess::slotSpitReagentTesting()
{
    m_waitingPrpRead = false;
    m_pendingPrpReread = false;
    disarmAckWatchdog();

    StructInstance* instance = StructInstance::getInstance();  // 获取单例
    const quint8 currentFocus = instance->read_focusSampleActive(m_focus_sample_id);

    // 查找状态映射
    auto it = FOCUS_MAPPING.find(currentFocus);
    if (it == FOCUS_MAPPING.constEnd()) {
        stallWithPause(QStringLiteral("spit_reagent_bad_focus"),
                       tr("吐试剂时焦点异常(FOCUS=%1)，已暂停").arg(currentFocus),
                       true);
        return;
    }

    const quint8 newFocus = it.value().first;
    const quint8 reagentType = it.value().second;

    setSampleFocus(m_focus_sample_id, newFocus, tr("PRP初值OK吐试剂"));

    const QByteArray firstComm = instance->addReagentIntoChannel(m_focus_sample_id, reagentType);
    logTestFsmInfo(QStringLiteral("spit_reagent_start"),
                   GlobalData::mapIndexReagentnames(reagentType));
    writeCommandHeader(firstComm,"吐试剂到测试通道",DELAY_TIMENUM);
}

void TestProjectProcess::recvSpitReagent(quint8 indexActive, const int indexNum, quint8 reagentIndex)
{
    QByteArray nextCmd;
    int nextDelayMs = DELAY_TIMENUM;
    StructInstance* instance = StructInstance::getInstance();
    const bool finished = instance->addReagentIntoChannelFinish(m_focus_sample_id,
                                                                 reagentIndex,
                                                                 indexNum,
                                                                 nextCmd,
                                                                 &nextDelayMs);
    if (!finished) {
        if (!nextCmd.isEmpty()) {
            writeCommandHeader(nextCmd, tr("吐试剂到测试通道"), nextDelayMs);
        } else {
            QLOG_ERROR() << "样本ID" << m_focus_sample_id << "试剂" << reagentIndex
                         << "未完成但无下一个指令，停止推进避免悬挂";
            if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
                emit pauseTesting();
            }
            promptsareDisplayed(TESTINGLOG, tr("吐试剂指令缺失，已暂停"), true);
        }
    } else {
        quint8 indexReagentForAbsorbent, consumablesIndex;
        GlobalData::outputtheAbsorbentIndex(indexActive, indexReagentForAbsorbent, consumablesIndex);
        const int delayMs = std::max(DELAY_TIMENUM, nextDelayMs);
        QPointer<TestProjectProcess> guard(this);
        const int sampleId = m_focus_sample_id;
        QTimer::singleShot(delayMs, this, [guard, sampleId, indexActive, indexReagentForAbsorbent]() {
            if (!guard)
                return;
            guard->logTestFsmInfo(QStringLiteral("open_chn_test_emit"),
                                  tr("样本%1 试剂索引%2").arg(sampleId).arg(indexReagentForAbsorbent));
            emit guard->openTestChnTest(sampleId, indexActive, indexReagentForAbsorbent);
        });
    }

//    bool finished_spitreagent = StructInstance::getInstance()->recv_spitReagentData(m_focus_sample_id,
//                                                                                    reagentIndex,
//                                                                                    indexNum);
//    if(finished_spitreagent == true)
//    {
//        //吐试剂完成==>打开通道开始测试-->清洗针
//        quint8 indexreagent,Consumablesindex;
//        GlobalData::outputtheAbsorbentIndex(indexActive,indexreagent,Consumablesindex);
//        //吐完试剂后延时再去洗针和读数据
//        emit openTestChnTest(m_focus_sample_id,indexActive,indexreagent);
//    }
    return;
}


void TestProjectProcess::slotaddSampmeTestCleanPin(const int cleanPin ,const quint8 index_reagent)
{
    quint8 focus_index_set = 0;
    QString outtext;
    if(cleanPin == WASH_DOUBLE_NEDDLES)
    {
        focus_index_set = FOCUS_CLEAN_DOUBLEPIN;
        StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
        outtext = QString::fromUtf8("清洗双针");
        markPendingCleanDeduct(2); // BOTH_PINS
    }
    else if(cleanPin == WASH_REAGENT_NEDDLES)
    {
        switch(index_reagent)
        {
            case AA_REAGENT:  focus_index_set = FOCUS_CLEAN_EREAGENTPIN_AA; break;
            case ADP_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_ADP; break;
            case EPI_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_EPI; break;
            case COL_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_COL; break;
            case RIS_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_RIS; break;
            default:break;
        }
        QLOG_DEBUG()<<"样本(清洗试剂针):"<<m_focus_sample_id<<GlobalData::mapIndexReagentnames(index_reagent);
        StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
        outtext = QString::fromUtf8("清洗试剂针");
        markPendingCleanDeduct(0); // REAGENT_PIN
    }
    startCleanNeedleAckMode(outtext);
}


void TestProjectProcess::startCleanNeedleAckMode(const QString &tips)
{
    if (tips.isEmpty()) {
        QLOG_ERROR() << QString::fromUtf8("清洗针类型无效，无法启动应答发送");
        return;
    }
    promptsareDisplayed(TESTINGLOG, tips, false);
    const quint8 focus = StructInstance::getInstance()->read_focusSampleActive(m_focus_sample_id);
    const QByteArray firstCmd = StructInstance::getInstance()->outputstheFirstIncompleteInstruction(
                m_focus_sample_id, focus);
    if (firstCmd.isEmpty()) {
        QLOG_ERROR() << tips << QString::fromUtf8("无未完成清洗指令 样本") << m_focus_sample_id;
        promptsareDisplayed(TESTINGLOG,
                            tr("清洗序列无有效指令（常见原因：清洗位未标定为(0,0)或与原点相同），请检查维护参数/坐标"),
                            true);
        return;
    }
    writeCommandHeader(firstCmd, tips, DELAY_TIMENUM);
}

void TestProjectProcess::continueCleanNeedleAckMode(bool finished, const QByteArray &nextCmd,
                                                    int cleanKind, const QString &tips)
{
    if (finished) {
        deductPendingCleanConsumable();
        dismissFocusSample(m_focus_sample_id);
        TraverseChnHadFinishTesting(cleanKind);
        return;
    }
    if (nextCmd.isEmpty()) {
        return;
    }
    writeCommandHeader(nextCmd, tips, DELAY_TIMENUM);
}

void TestProjectProcess::sendFirstPrepatchCommand(const QString &tips)
{
    m_prepatchAckPaced = true;
    const QByteArray firstCmd = StructInstance::getInstance()->nextIncompletePrepatch(m_focus_sample_id);
    if (firstCmd.isEmpty()) {
        m_prepatchAckPaced = false;
        QLOG_ERROR() << "[抓手异常] 样本" << m_focus_sample_id
                     << "| 善后队列是空的，无法开始 | " << tips;
        return;
    }
    QLOG_INFO() << "[抓手异常] 样本" << m_focus_sample_id
                << "| 发出善后第一条，命令号" << commandIndexOf(firstCmd);
    writeCommandHeader(firstCmd, tips, DELAY_TIMENUM);
}


void TestProjectProcess::recv_cleanDoublePin(const int index_num, const int slaveAddr,
                                             const int actionMotor, const QStringList &recvdata)
{
    notifyHangLiquidWipeIfNeeded(slaveAddr, actionMotor, recvdata);
    QByteArray nextCmd;
    const bool finished_clean = StructInstance::getInstance()->recv_cleanDobulePin(
                index_num, m_focus_sample_id, nextCmd);
    continueCleanNeedleAckMode(finished_clean, nextCmd, WASH_DOUBLE_NEDDLES,
                               QString::fromUtf8("清洗双针"));
}

void TestProjectProcess::recv_cleanReagentPin(const int index_num, const quint8 index_reag,
                                              const int slaveAddr, const int actionMotor,
                                              const QStringList &recvdata)
{
    notifyHangLiquidWipeIfNeeded(slaveAddr, actionMotor, recvdata);
    QByteArray nextCmd;
    const bool finished_clean = StructInstance::getInstance()->recv_cleanReagentPin(
                index_num, index_reag, m_focus_sample_id, nextCmd);
    continueCleanNeedleAckMode(finished_clean, nextCmd, WASH_REAGENT_NEDDLES,
                               QString::fromUtf8("清洗试剂针"));
}

void TestProjectProcess::recv_cleanBloodPin(const int index_num, const int slaveAddr,
                                            const int actionMotor, const QStringList &recvdata)
{
    notifyHangLiquidWipeIfNeeded(slaveAddr, actionMotor, recvdata);
    QByteArray nextCmd;
    const bool finished_clean = StructInstance::getInstance()->recv_cleanbloodPin(
                index_num, m_focus_sample_id, nextCmd);
    continueCleanNeedleAckMode(finished_clean, nextCmd, WASH_BLLODSAMPLE_NEDDLES,
                               QString::fromUtf8("洗样本针"));
}


// 处理完成通道任务
void TestProjectProcess::processFinishedChannelTask()
{
    const Finish_Data_ task = m_finishTestData.dequeue();

    // 调试日志输出
    QLOG_DEBUG() << "出队任务通道 " << task.index_Chn
                          << " | " << QUIUtils::index_reagent_mapping_reagentName(task.index_reagent, "试剂");

    auto *instance = StructInstance::getInstance();
    QString sampleName;
    int sampleId = 0;
    if (task.CompleteSampleid > 0) {
        const DATASAMPLESTRUCT *ps = instance->rfindIdSample(task.CompleteSampleid);
        if (ps && ps->indexTestingChn == static_cast<int>(task.index_Chn)) {
            sampleId = task.CompleteSampleid;
            sampleName = ps->samplestr;
        } else {
            QLOG_WARN() << "完成入队任务与通道占用不一致，延后 任务样本"
                        << task.CompleteSampleid << "任务通道" << (task.index_Chn + 1);
            m_finishTestData.enqueue(task);
            if (startPendingContinueReagent()) {
                return;
            }
            TraverseChnHadFinishTesting(THROW_CUPS_END);
            return;
        }
    }
    if (sampleId <= 0) {
        instance->rootTestChnGetSampleInfo(task.index_Chn, sampleName, sampleId);
    }
    if (sampleId <= 0) {
        QLOG_ERROR() << "完成队列通道" << (task.index_Chn + 1) << "未找到样本，跳过弃杯";
        TraverseChnHadFinishTesting(THROW_CUPS_END);
        return;
    }

    // 设置焦点样本
    setupFocusSample(sampleId, task.index_reagent);

    // 发送控制命令
    sendControlCommands(task.index_Chn, sampleId, task.index_reagent);
}

// 设置焦点样本
void TestProjectProcess::setupFocusSample(int sampleId, quint8 reagentType)
{
    StructInstance::getInstance()->config_focusSample(sampleId, true);
    m_focus_sample_id = sampleId;

    // 获取焦点类型
    const quint8 focusType = s_reagentWorkflow.value(reagentType).throwFocus;
    //Q_ASSERT_X(focusType != FOCUS_UNKNOWN, Q_FUNC_INFO, "无效试剂类型");

    // 写入焦点状态
    StructInstance::getInstance()->write_focusSampleActive(sampleId, focusType);
}

// 发送控制命令
void TestProjectProcess::sendControlCommands(quint8 chn, int sampleId, quint8 reagent)
{
    emit setprogressinitboolwaittext(true, chn,"");

    outputThrowTestChannelReagentFinish(sampleId,reagent);
}

// 处理空闲通道
void TestProjectProcess::handleIdleChannelTasks()
{
    if (!m_pendingtimeSample.isEmpty()) {
        loadPedningtimeTask();
        if (StructInstance::getInstance()->sampleHadFocus()) {
            return;
        }
    }

    int waitSampleId = -1;
    if (!StructInstance::getInstance()->alreadyadded_wait_chn(waitSampleId)){
        StartSampleAdditionWorkflow();
        return;
    }


    quint8 freeChn = 0;
    const bool hasFreeChannel = StructInstance::getInstance()->getNextFreeChnAfterTesting(freeChn);
    //if (!StructInstance::getInstance()->hadFreeTestChn(freeChn)) {
    if(!hasFreeChannel){
        StartSampleAdditionWorkflow();
        return;
    }

    m_focus_sample_id = waitSampleId;
    StructInstance::getInstance()->config_focusSample(waitSampleId, true);
    StructInstance::getInstance()->write_focusSampleActive(
        waitSampleId, FOCUS_CLIP_ANEMIA_TO_CHN);
    StructInstance::getInstance()->setTestingChn(waitSampleId, freeChn);

    emit openChnMotor(freeChn, true);

    const QByteArray cmd = StructInstance::getInstance()->sendPPP2ChannelHeanderCommand(waitSampleId,
                                                                                        GRIPPERMOVE_NORMAL);
    writeCommandHeader(cmd, tr("吸待测PPP试管到测试通道首命令"), DELAY_TIMENUM);
}


//清洗完针 和 样本全部测试完成后调用
void TestProjectProcess::TraverseChnHadFinishTesting(int clean_indexPin)
{
     const QString& out_text = s_cleanActionMap.value(clean_indexPin, "");
     if (out_text.isEmpty()) {
         QLOG_ERROR() << "TraverseChnHadFinishTesting: invalid clean_indexPin:" << clean_indexPin;
         return;
     }
     logTestFsmInfo(QStringLiteral("traverse_chn"),
                    tr("%1 finishQ=%2").arg(out_text).arg(m_finishTestData.size()));
     promptsareDisplayed(TESTINGLOG,out_text,false);

    if (startPendingContinueReagent()) {
        return;
    }

    if (!m_finishTestData.isEmpty()) {
        processFinishedChannelTask();
        return;
    }

    handleIdleChannelTasks();
}




void TestProjectProcess::recv_throwCup(quint8 indexActive,const int commandIndex,const int slaveAddr,
                                        const int Action_motor,const QStringList &recvdata) {

    static const QMap<quint8, quint8> reagentMapping = {
        {FOCUS_THROW_AA_TUBE,  AA_REAGENT},
        {FOCUS_THROW_ADP_TUBE, ADP_REAGENT},
        {FOCUS_THROW_EPI_TUBE, EPI_REAGENT},
        {FOCUS_THROW_COL_TUBE, COL_REAGENT},
        {FOCUS_THROW_RIS_TUBE, RIS_REAGENT}
    };
    quint8 index_reagent = reagentMapping.value(indexActive, AA_REAGENT);

    auto* instance = StructInstance::getInstance();
    if (ignoreRestartLeftoverAck(Action_motor, recvdata)) {
        return;
    }
    if (!instance->throwReagentContainsCommandIndex(
                m_focus_sample_id, index_reagent, static_cast<quint8>(commandIndex))) {
        QLOG_WARN() << "丢试剂杯忽略非本序列应答 样本" << m_focus_sample_id
                    << "试剂" << index_reagent
                    << "命令号" << commandIndex << "从机" << slaveAddr
                    << "电机" << Action_motor;
        return;
    }

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
        GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
        if (tryFeedActiveGripperVacuumPoll(airValue, isSuction,
                                           tr("继续发送丢测试通道测试杯指令"))) {
            return;
        }
    }

    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    QByteArray outdata;
    bool finished_throwcup = instance->recvThrowCupPara(m_focus_sample_id,commandIndex,index_reagent, handsControlActive,
                                                        isSuction, airValue,outdata,kGripperFailed,klaterTimer);

    if(finished_throwcup)
    {
        emit throwtesttube();
        throwCupFinish();
        return;
    }
    else
    {
        if (kGripperFailed == GRIPPERMOVE_NORMAL && outdata.isEmpty()) {
            if (tryStartGripperPollIfWaiting(outdata)) {
                return;
            }
            stallWithPause(QStringLiteral("throw_reag_cup_stall"),
                           tr("丢试剂杯无法推进(cmd=%1)，已暂停").arg(commandIndex),
                           true);
            return;
        }
        const QString illustrateText = (isSuction ? tr("通道抓试杯失败") : tr("通道放试杯失败"));
        QString tips = kGripperFailed ? illustrateText : tr("继续发送丢测试通道测试杯指令");
        if (klaterTimer && handsControlActive && kGripperFailed == GRIPPERMOVE_NORMAL) {
            quint8 suckCheck = 0;
            quint8 channel = 0;
            if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
                if (ps->indexTestingChn >= 0) {
                    channel = static_cast<quint8>(ps->indexTestingChn);
                }
                for (Testing_reagents *reag : ps->ptestReagent_active) {
                    if (reag && reag->index_Reagent == index_reagent
                            && reag->throwPRRsucknum >= 2) {
                        suckCheck = static_cast<quint8>(reag->throwPRRsucknum - 1);
                        break;
                    }
                }
            }
            armGripperReadAfterOpenVacuum(suckCheck, channel, false, 0, index_reagent,
                                          tr("丢试剂杯"),
                                          illustrateText + tr("（下扎加深仍吸不牢）"));
        }
        commandNotCompleteOrErr(kGripperFailed,outdata,tips,klaterTimer,false);
   }

}

void TestProjectProcess::throwCupFinish()
{
     quint8 waittest_reag = 0;
     auto* instance = StructInstance::getInstance();
     const bool bhand_waittest_reag = instance->judge_hadWaitTestReag(m_focus_sample_id,waittest_reag);
     const bool skipFailedPrp = m_prpZeroSkipAfterThrow;
     m_prpZeroSkipAfterThrow = false;
     if(bhand_waittest_reag)
     {
        if (skipFailedPrp) {
            promptsareDisplayed(TESTINGLOG,
                                tr("样本%1 当前PRP已丢弃，继续测试试剂%2")
                                    .arg(m_focus_sample_id)
                                    .arg(GlobalData::mapIndexReagentnames(waittest_reag)),
                                false);
        }
        enqueueContinueReagent(m_focus_sample_id);
        dismissFocusSample(m_focus_sample_id);
        // PRP 读 0 丢杯后须先续跑本样本下一试剂，不能被其它样本的「完成入队」抢通道
        if (skipFailedPrp && startPendingContinueReagent()) {
            return;
        }
        TraverseChnHadFinishTesting(THROW_CUPS_END);
     }
     else
     {
         if (skipFailedPrp) {
             promptsareDisplayed(TESTINGLOG,
                                 tr("样本%1 无待测试剂，放弃该样本").arg(m_focus_sample_id),
                                 false);
             giveupSamplenum(m_focus_sample_id);
         } else {
             theTestChannelStopTesting(false);
         }
     }
}



/**
 * @brief TestProjectProcess::
 * 1.清洗完针该样本失去 焦点 遍历通道内 测试样本状态
 * 2.再测试中添加了样本 遍历样本库有无样本有焦点
 * 3.样本测试完成，且无测试试剂继续测试的  需要
 */

void TestProjectProcess::equipment_err_continue()
{
    m_needHomeBeforeResume = false;
    m_pauseCommandQueue.clear();
    m_outstandingAcks = 0;
    disarmAckWatchdog();
    cglobal::g_PauseTesting.store(false);
    emit requestPauseUiSync(false);
    m_Errorresetsfirst = true;
    mbackOriginnumList.clear();
    QByteArrayList allAxisBackOrigin = GlobalData::letallAxisBackOrigin(mbackOriginnumList);
    sendInstructionsWithDelay(allAxisBackOrigin, DELAY_TIMENUM,
                              QString::fromUtf8("动作异常全轴复位"));

    QPointer<TestProjectProcess> guard(this);
    QTimer::singleShot(RESET_FALLBACK_MS, this, [guard]() {
        if (!guard || !guard->m_Errorresetsfirst) {
            return;
        }
        QLOG_WARN() << QString::fromUtf8("全轴复位应答未齐，强制继续调度");
        guard->m_Errorresetsfirst = false;
        guard->mbackOriginnumList.clear();
        guard->m_outstandingAcks = 0;
        guard->disarmAckWatchdog();
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            guard->exceptionRetest(QString::fromUtf8("复位应答未齐强制续跑"));
        }
    });
}


void TestProjectProcess::updatetubeColorState(const QStringList data)
{
    QPoint movedAxis = QUIUtils::coordinatePositionXYMotion(data);
    quint8 backHole = 250;
    bool bemptyzone = false;
    bool  oper_anemia = StructInstance::getInstance()->MoveAxisMapingHole(movedAxis,backHole,bemptyzone);
    if(bemptyzone == true)
    {
        QLOG_DEBUG()<<"改变空试管颜色标识:"<<backHole<<"吐样"<<__FILE__<<__LINE__<<endl;
        emit bloodSuck2EmptyTube(oper_anemia,backHole);
    }
    else
    {
        QLOG_DEBUG()<<"样本针在血样区吸样孔号"<<backHole<<endl;
    }
    return;
}


QPoint TestProjectProcess::updateEmptyClipAway(const QStringList data)
{
    QPoint movedAxis = QUIUtils::coordinatePositionXYMotion(data);
    quint8 backHole = 0;
    if(StructInstance::getInstance()->EmptytubeClipAway(movedAxis,backHole))
    {
        QString outtext = QString::fromUtf8("抓手将试管%1夹走测试").arg(backHole);
        promptsareDisplayed(TESTINGLOG,outtext,false);
        emit EmptyTubeCatched(backHole);
        QLOG_DEBUG() << "试管区夹走试管编号:"<< backHole<<endl;
    }
    return movedAxis;
}


/** 断线重连
* @brief TestProjectProcess::slot_connectEquipmentagin
* 判断有无焦点样本
* 有就先继续做完
* 无就看完成的容器
*/
void TestProjectProcess::slot_connectEquipmentagin()
{
    if (m_offlineVoidTimer) {
        m_offlineVoidTimer->stop();
    }
    promptsareDisplayed(TESTINGLOG,"重连继续遍历测试指令",false);

    if (!cglobal::g_StartTesting.load()) {
        m_offlineWaitingReconnect = false;
        m_offlineVoided = false;
        return;
    }

    // P2：保存焦点快照；未完成动作的待扣耗材作废，避免重连续跑误扣
    captureReconnectSnapshot();
    clearPendingConsumableDeducts();
    if (m_offlineVoided) {
        publishWorkflowStatus(tr("断线重连：全轴复位后处理作废试剂"));
    } else {
        publishWorkflowStatus(tr("断线重连：全轴复位后按快照续跑"));
    }

    //先所有轴复位
    m_Errorresetsfirst = true;
    mbackOriginnumList.clear();
    QByteArrayList allAxisBackOrigin = GlobalData::letallAxisBackOrigin(mbackOriginnumList);
    sendInstructionsWithDelay(allAxisBackOrigin,DELAY_TIMENUM, "重连继续遍历测试指令");

}





//测试通道无继续待测试剂停止测试完成
void TestProjectProcess::theTestChannelStopTesting(const bool handError)
{
    //新增：生成曲线图片+数据（异步，线程安全）
    const QString thieDate = GlobalData::ObatinCreatSampleTime();
    const QString curveId = GlobalData::groupDateAndID(thieDate, m_focus_sample_id);
    CurveImageGenerator::instance()->generateCurveImageAsync(curveId);


    // 获取单例实例一次
    auto* instance = StructInstance::getInstance();
    const auto *psample = instance->rfindIdSample(m_focus_sample_id);
    quint8 kstopTestChannel = 0;
    quint8 finishedPppHole = 0;
    quint8 finishedPrpHole = 0;
    if (psample) {
        finishedPppHole = psample->suckanaemia_hole;
        finishedPrpHole = psample->suckbloody_hole;
    }
    if (psample && psample->indexTestingChn >= 0) {
        kstopTestChannel = static_cast<quint8>(psample->indexTestingChn);
        emit openChnMotor(kstopTestChannel, false);
        emit setprogressinitboolwaittext(false,kstopTestChannel,"[完成]");
        instance->config_testChn_State(kstopTestChannel,CHN_STATUS_FREE);
        instance->_deleteOneSampleTesteddata(kstopTestChannel,m_focus_sample_id);
        emit stopChannelAcquisition(kstopTestChannel);
    }

    if (finishedPppHole != 0 || finishedPrpHole != 0) {
        emit sycn_SampleTestingChangInitColor(QPoint(finishedPppHole, finishedPrpHole),
                                              static_cast<quint8>(kstopTestChannel + 1));
    }

    dismissFocusSample(m_focus_sample_id);

    //结构体删除样本
    const int finishedSampleId = m_focus_sample_id;
    instance->onetestend_del(finishedSampleId);
    removeSampleFromSchedulerQueues(finishedSampleId);
    m_focus_sample_id = 0;

    const QString actionDesc = handError
            ? QString::fromUtf8("抓手异常取消测试样本")
            : QString::fromUtf8("完成所有试剂测试");
    const QString message = QString::fromUtf8("样本%1%2，关闭通道%3搅拌电机")
                               .arg(finishedSampleId)
                               .arg(actionDesc)
                               .arg(kstopTestChannel + 1);
    promptsareDisplayed(TESTINGLOG, message,false);



    int remaining  = 0; //获取剩余样本数
    StructInstance::getInstance()->SampleTotal(remaining );



    if(remaining  == ALL_SAMPLE_TESTED){
        prepareForNewTestRound();
        emit allSamplesFinished();

        instance->delalltaskinfo(false); //删除所有样本信息

        instance->setemptynull();
    }
    else {
        TraverseChnHadFinishTesting(THROW_CUPS_END);
    }
    return;
}


/**
 * @brief 恢复测试任务
 */
void TestProjectProcess::handleResumeaTestTask()
{
    StructInstance* instance = StructInstance::getInstance();
    const bool hasFocus = instance->sampleHadFocus();

    if (m_needHomeBeforeResume) {
        m_pauseCommandQueue.clear();
        equipment_err_continue();
        return;
    }

    if (m_pendingPppReread) {
        requestPppBaselineRead();
        return;
    }
    if (m_pendingPrpReread) {
        requestPrpBaselineRead();
        return;
    }

    if (!hasFocus && !m_pauseCommandQueue.isEmpty()) {
        logTestFsm(QStringLiteral("resume_no_focus_stash"),
                   tr("暂停队列=%1，改由调度续跑").arg(m_pauseCommandQueue.size()));
        exceptionRetest(tr("无焦点恢复"));
        return;
    }

    if (hasFocus) {
        if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
            if (ps->pppAwaitManualWipe && !ps->pppAddSamplePendingTail.isEmpty()) {
                ps->pppAwaitManualWipe = false;
                ps->pppPreProbeCompleted = false;
                ps->pppManualPreProbeRetry = true;
                QByteArrayList cmds;
                quint8 preProbeIndex = 0;
                QUIUtils::appendPppPositionAndPreProbe(cmds, ps->suckanaemia_axis, preProbeIndex);
                ps->pppPreProbeCmdIndex = preProbeIndex;
                beginPppPreProbeRecovery(cmds, preProbeIndex);
                promptsareDisplayed(TESTINGLOG,
                                    tr("擦针完成，从PPP预探测继续"), false);
                return;
            }
        }
    }

    if (hasFocus && !m_pauseCommandQueue.isEmpty()) {
        if (m_pauseCommandQueue.size() == 1) {
            const QByteArray cmd = m_pauseCommandQueue.takeFirst();
            m_lastOutboundCommands = {cmd};
            emit WriteaSingleCommand(cmd, tr("继续发送指令"));
            noteCommandsSent(1);
        } else {
            const QByteArrayList cmds = m_pauseCommandQueue;
            m_pauseCommandQueue.clear();
            m_lastOutboundCommands = cmds;
            emit write_instructions_group(cmds, tr("继续任务指令群"));
            noteCommandsSent(cmds.size());
        }
        return;
    }

    exceptionRetest(tr("恢复测试任务"));
}


void TestProjectProcess::outputThrowTestChannelReagentFinish(const int finishId,const quint8 finishReagent)
{
    const QByteArray outHeadnArry = StructInstance::getInstance()->sendCommandThrowCupFirst(finishId,
                                                                                      finishReagent,
                                                                                      false);
    writeCommandHeader(outHeadnArry,"丢测试通道内完成试剂杯首索引指令",DELAY_TIMENUM);
}




void TestProjectProcess::clearPppPreProbeRecoveryState()
{
    m_pppRecoveryInProgress = false;
    m_pppRecoveryCommands.clear();
    m_pppRecoveryAckIndices.clear();
    m_pppRecoveryAckPos = 0;
}

void TestProjectProcess::sendNextPppRecoveryCommand()
{
    while (m_pppRecoveryAckPos < m_pppRecoveryCommands.size()) {
        const QByteArray cmd = m_pppRecoveryCommands.at(m_pppRecoveryAckPos);
        if (cmd.isEmpty()) {
            ++m_pppRecoveryAckPos;
            continue;
        }
        writeCommandHeader(cmd, tr("PPP挂液处理/预探测"), DELAY_TIMENUM);
        return;
    }
}

void TestProjectProcess::notifyHangLiquidWipeIfNeeded(const int slaveAddr, const int actionMotor,
                                                    const QStringList &recvdata,
                                                    const bool showModalPrompt)
{
    if (slaveAddr != Z_AXIS) {
        return;
    }
    const quint8 motorIndex = static_cast<quint8>(actionMotor);
    if (motorIndex != MOTOR_BLOOD_INDEX && motorIndex != MOTOR_REAGNET_INDEX) {
        return;
    }
    if (!QUIUtils::isNeedleZHangPreProbeResetAck(recvdata, motorIndex)) {
        return;
    }
    if (!QUIUtils::isZAxisLiquidLevelDetected(recvdata)) {
        return;
    }
    const QString needleName = (motorIndex == MOTOR_BLOOD_INDEX)
            ? tr("样本针") : tr("试剂针");
    if (showModalPrompt) {
        promptsareDisplayed(TESTINGLOG, tr("请擦拭%1针尖").arg(needleName), true);
    } else {
        promptsareDisplayed(TESTINGLOG,
                            tr("检测到%1尖可能挂液，请擦拭针尖；随后将按设定失败高度液面探测并继续吸样")
                            .arg(needleName),
                            false);
    }
}

void TestProjectProcess::beginPppPreProbeRecovery(const QByteArrayList &commands,
                                                  quint8 /*preProbeCmdIndex*/)
{
    m_pppRecoveryCommands = commands;
    m_pppRecoveryAckIndices = commandIndexListFromBytes(commands);
    m_pppRecoveryAckPos = 0;
    m_pppRecoveryInProgress = !m_pppRecoveryCommands.isEmpty();
    if (!m_pppRecoveryInProgress) {
        QLOG_ERROR() << QString::fromUtf8("PPP挂液恢复序列为空，无法洗针");
        return;
    }
    publishWorkflowStatus(tr("PPP挂液处理/预探测"));
    promptsareDisplayed(TESTINGLOG, tr("PPP挂液：开始洗样本针并回PPP预探测"), false);
    sendNextPppRecoveryCommand();
}

void TestProjectProcess::confirmPppHangLiquidProceed()
{
    if (!cglobal::g_StartTesting.load()) {
        return;
    }
    m_needHomeBeforeResume = false;
    m_pauseCommandQueue.clear();
    m_outstandingAcks = 0;
    disarmAckWatchdog();
    cglobal::g_PauseTesting.store(false);
    emit requestPauseUiSync(false);

    auto *instance = StructInstance::getInstance();
    DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id);
    if (!ps || ps->pppAddSamplePendingTail.isEmpty()) {
        QLOG_WARN() << QString::fromUtf8("PPP挂液确定续跑时无待追加加样尾段 样本") << m_focus_sample_id;
        return;
    }

    ps->pppAwaitManualWipe = false;
    ps->pppManualPreProbeRetry = false;
    ps->pppPreProbeCompleted = false;
    clearPppPreProbeRecoveryState();
    publishWorkflowStatus(tr("按液面探测失败高度吸PPP"));
    promptsareDisplayed(TESTINGLOG,
                        tr("已确认擦针：跳过预探测，按失败高度液面探测吸PPP并吐PPP"),
                        false);
    advanceAddSampleAfterPppPreProbePassed(ps->pppPreProbeCmdIndex);
}

void TestProjectProcess::advanceAddSampleAfterPppPreProbePassed(quint8 /*preProbeIndex*/)
{
    auto *instance = StructInstance::getInstance();
    instance->appendPendingPppAddSampleTail(m_focus_sample_id);
    quint8 handshakeIndex = 0;
    if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
        ps->pppPreProbeCompleted = true;
        handshakeIndex = ps->pppPreProbePrefixCmdIndex;
    }

    bool iscompleteAddSample = false;
    QByteArray outdata = instance->recvAddSampleIndexCommand(m_focus_sample_id,
                                                             handshakeIndex,
                                                             iscompleteAddSample);
    if (!iscompleteAddSample) {
        writeCommandHeader(outdata, tr("加样指令"), DELAY_TIMENUM);
    } else {
        instance->updateSampleState(m_focus_sample_id, ADDED_COMPLETE_SAMPLE);
        if (const auto *ps = instance->rfindIdSample(m_focus_sample_id)) {
            emit bloodSampleHintStage(ps->suckanaemia_hole, ps->suckbloody_hole,
                                      TestingConstants::BLOOD_HINT_TESTING);
        }
        getFreeTimeTestChn();
    }
}

void TestProjectProcess::finishPppPreProbeDecision(const QStringList &recvdata)
{
    auto *ps = StructInstance::getInstance()->wfindIdSample(m_focus_sample_id);
    if (!ps || ps->pppAddSamplePendingTail.isEmpty()) {
        return;
    }

    const quint8 preProbeIndex = ps->pppPreProbeCmdIndex;
    if (!QUIUtils::isZAxisLiquidLevelDetected(recvdata)) {
        ps->pppManualPreProbeRetry = false;
        promptsareDisplayed(TESTINGLOG, tr("PPP预探测未检测到挂液，继续液面探测吸样"), false);
        advanceAddSampleAfterPppPreProbePassed(preProbeIndex);
        return;
    }

    if (ps->pppManualPreProbeRetry) {
        ps->pppManualPreProbeRetry = false;
        ps->pppAwaitManualWipe = false;
        clearPppPreProbeRecoveryState();
        promptsareDisplayed(TESTINGLOG,
                            tr("擦针后预探测仍报挂液，按液面探测失败高度吸PPP"),
                            false);
        advanceAddSampleAfterPppPreProbePassed(preProbeIndex);
        return;
    }

    if (ps->pppPreProbeWashCount == 0) {
        ps->pppPreProbeWashCount = 1;
        ps->pppPreProbeCompleted = false;
        QByteArrayList recovery;
        QUIUtils::CleanBloodPinActionCommd(1, recovery);
        quint8 newPreProbeIndex = 0;
        QUIUtils::appendPppPositionAndPreProbe(recovery, ps->suckanaemia_axis, newPreProbeIndex);
        ps->pppPreProbeCmdIndex = newPreProbeIndex;
        beginPppPreProbeRecovery(recovery, newPreProbeIndex);
        promptsareDisplayed(TESTINGLOG,
                            tr("PPP位预探测到挂液，自动洗样本针一次后从预探测重试"),
                            false);
        return;
    }

    ps->pppAwaitManualWipe = true;
    clearPppPreProbeRecoveryState();
    publishWorkflowStatus(tr("PPP预探测挂液：请擦净样本针后继续"));
    if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
        emit pauseTesting();
    }
    promptsareDisplayed(TESTINGLOG,
                        tr("洗针后仍探测到挂液，请擦净样本针尖。"
                           "确定：按失败高度液面探测吸PPP；取消：暂停后点「继续」重做预探测。"),
                        true);
}

bool TestProjectProcess::tryHandlePppPreProbeAck(int index_num, int slaveAddr, int axismovement,
                                                 const QStringList &recvdata)
{
    if (m_pppRecoveryInProgress) {
        if (m_pppRecoveryAckPos >= m_pppRecoveryCommands.size()) {
            clearPppPreProbeRecoveryState();
            return false;
        }
        if (m_pppRecoveryAckPos < m_pppRecoveryAckIndices.size()
                && static_cast<quint8>(index_num) != m_pppRecoveryAckIndices.at(m_pppRecoveryAckPos)) {
            QLOG_WARN() << QString::fromUtf8("PPP恢复序列应答命令号不符 期望")
                        << m_pppRecoveryAckIndices.at(m_pppRecoveryAckPos)
                        << QString::fromUtf8("收到") << index_num;
        }
        notifyHangLiquidWipeIfNeeded(slaveAddr, axismovement, recvdata, false);

        ++m_pppRecoveryAckPos;
        if (m_pppRecoveryAckPos < m_pppRecoveryCommands.size()) {
            sendNextPppRecoveryCommand();
            return true;
        }
        m_pppRecoveryInProgress = false;
        m_pppRecoveryCommands.clear();
        m_pppRecoveryAckIndices.clear();
        m_pppRecoveryAckPos = 0;
        finishPppPreProbeDecision(recvdata);
        return true;
    }

    auto *ps = StructInstance::getInstance()->wfindIdSample(m_focus_sample_id);
    if (!ps || ps->pppAddSamplePendingTail.isEmpty()) {
        return false;
    }
    if (ps->pppPreProbeCompleted) {
        return false;
    }
    if (static_cast<quint8>(index_num) != ps->pppPreProbeCmdIndex) {
        return false;
    }
    if (slaveAddr != Z_AXIS || axismovement != MOTOR_BLOOD_INDEX) {
        return false;
    }
    if (!QUIUtils::isBloodNeedleZPreProbeResetAck(recvdata)) {
        return false;
    }

    finishPppPreProbeDecision(recvdata);
    return true;
}

//收到加样动作指令
void TestProjectProcess::receiveInstructionsSampleactive(int index_num, int slaveAddr,
                                                         int axismovement,
                                                         const QStringList recvdata)
{
    if (tryHandlePppPreProbeAck(index_num, slaveAddr, axismovement, recvdata)) {
        if (axismovement == MOTOR_XY) {
            updatetubeColorState(recvdata);
        }
        return;
    }

    if(axismovement == MOTOR_XY)
        updatetubeColorState(recvdata);

    bool iscompleteAddSample = false;
    QByteArray outdata = StructInstance::getInstance()->recvAddSampleIndexCommand(m_focus_sample_id,
                                                                                  index_num,
                                                                                  iscompleteAddSample);
    if(!iscompleteAddSample){

        writeCommandHeader(outdata,"加样指令",DELAY_TIMENUM);

    }else{
        //设置加样完成状态
        StructInstance::getInstance()->updateSampleState(m_focus_sample_id,
                                                         ADDED_COMPLETE_SAMPLE);
        if (const auto *ps = StructInstance::getInstance()->rfindIdSample(m_focus_sample_id)) {
            emit bloodSampleHintStage(ps->suckanaemia_hole, ps->suckbloody_hole,
                                      TestingConstants::BLOOD_HINT_TESTING);
        }
        logTestFsmInfo(QStringLiteral("add_sample_done"), tr("样本%1").arg(m_focus_sample_id));
        getFreeTimeTestChn();
    }
    return;
}



void TestProjectProcess::receiveInstructionsPPPCuptoChannel(const int indexcode
                                                               ,const int slaveAddr
                                                               ,const int Action_motor
                                                               ,const QStringList& recvdata)
{
   auto *instance = StructInstance::getInstance();
   if (ignoreRestartLeftoverAck(Action_motor, recvdata)) {
       return;
   }
   if (!instance->pppClipContainsCommandIndex(
               m_focus_sample_id, static_cast<quint8>(indexcode))) {
       QLOG_WARN() << "夹PPP忽略非本序列应答 样本" << m_focus_sample_id
                   << "命令号" << indexcode << "从机" << slaveAddr
                   << "电机" << Action_motor;
       return;
   }

   if(Action_motor == MOTOR_XY)
       updateEmptyClipAway(recvdata);

   quint32 airValue  = 0;
   bool isSuction = false;
   const bool isHandsControl = (slaveAddr == Z_AXIS && Action_motor == MOTOR_HANDS_INDEX);

   if(isHandsControl) {
      QString outputTips = GlobalData::analyzeWhetherGripperSuction(recvdata,isSuction,airValue);
      promptsareDisplayed(TESTINGLOG,outputTips + "(加PPP动作)",false);
      if (tryFeedActiveGripperVacuumPoll(airValue, isSuction, tr("移动PPP试管到测试通道"))) {
          return;
      }
   }

   QByteArray sendData;
   sendData.clear();
   bool errorHands = GRIPPERMOVE_NORMAL;
   bool klateTimer = false;
   const bool operationSuccess  = StructInstance::getInstance()->processPPPCupToChannel(m_focus_sample_id
                                                                                        ,indexcode
                                                                                        ,isHandsControl
                                                                                        ,isSuction,
                                                                                        airValue
                                                                                        ,sendData,
                                                                                        errorHands
                                                                                        ,klateTimer);
   if(operationSuccess) {
       instance->stopGripperVacuumPoll();
       stopGripperVacuumPollTimer();
       QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"PPP吸到测试通道成功,开始延时1S准备读取PPP初值"<<endl;
       const int configDelay  = INI_File()._getdelayedtime();
       QPointer<TestProjectProcess> guard(this);
       QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + configDelay , this, [guard]() {
            if (guard)
                guard->requestPppBaselineRead();
       });
       return;
   }else{
       const QString illustrateText = (isSuction ? tr("吸PPP试杯失败")
                                                   : tr("放PPP试杯到通道失败"));
       QString tips = errorHands ? illustrateText : tr("移动PPP试管到测试通道");
       if (klateTimer && isHandsControl && errorHands == GRIPPERMOVE_NORMAL) {
           quint8 suckCheck = 0;
           quint8 hole = 0;
           if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
               if (ps->suctionPPPreset >= 2) {
                   suckCheck = static_cast<quint8>(ps->suctionPPPreset - 1);
               }
               hole = ps->spitanaemia_hole;
           }
           armGripperReadAfterOpenVacuum(suckCheck, hole, true, 1, 0,
                                         tr("夹PPP"),
                                         illustrateText + tr("（下扎加深仍吸不牢）"));
           commandNotCompleteOrErr(errorHands, sendData, tips, klateTimer, false);
           return;
       }
       if (errorHands == GRIPPERMOVE_NORMAL && sendData.isEmpty()) {
           if (tryStartGripperPollIfWaiting(sendData)) {
               return;
           }
           instance->forceAdvancePppClipQueue(m_focus_sample_id, sendData);
           if (!sendData.isEmpty()) {
               commandNotCompleteOrErr(errorHands, sendData, tr("移动PPP试管到测试通道"),
                                       klateTimer, false);
               return;
           }
           stallWithPause(QStringLiteral("ppp_clip_stall"),
                          tr("夹PPP步骤无法推进(cmd=%1)，已暂停").arg(indexcode),
                          true);
           return;
       }
       commandNotCompleteOrErr(errorHands, sendData, tips, klateTimer, false);
   }
   return;
}



bool TestProjectProcess::tryRecvGripperVacuumProbeAck(const QStringList &recvData,
                                                      quint8 commandIndex)
{
    auto *instance = StructInstance::getInstance();
    if (!instance->isGripperVacuumPollActive()) {
        return false;
    }
    if (cglobal::g_StartTesting.load()) {
        QLOG_WARN() << "[GripperVac] 测试中忽略probe命令号" << commandIndex;
        instance->stopGripperVacuumPoll();
        stopGripperVacuumPollTimer();
        return false;
    }
    if (commandIndex != HANDS_SUCK_NUM && commandIndex != HANDS_SPLIT_NUM) {
        return false;
    }

    int commandMode = 0;
    int run_status = 0;
    int speedMode = 0;
    int actionMotor = 0;
    GlobalData::Parse4thBytecommandstatus(recvData, commandMode, run_status, speedMode, actionMotor);
    if (run_status == MOTOR_WAITING || run_status == MOTOR_RUNNING) {
        return true;
    }
    if (run_status != MOTOR_FINISH) {
        return true;
    }

    noteAckReceived();
    bool isSuction = false;
    quint32 airValue = 0;
    GlobalData::analyzeWhetherGripperSuction(recvData, isSuction, airValue);
    QLOG_DEBUG() << "[GripperVac] probe应答 命令号" << commandIndex
                 << "焦点样本" << m_focus_sample_id << "负压" << airValue
                 << "吸杯" << isSuction;
    applyGripperVacuumPollResult(instance->feedGripperVacuumPollSample(airValue, isSuction),
                                 tr("抓手负压检测"));
    return true;
}

bool TestProjectProcess::tryFeedActiveGripperVacuumPoll(quint32 /*airValue*/,
                                                        bool /*isSuctionBit*/,
                                                        const QString &/*workflowTips*/)
{
    auto *instance = StructInstance::getInstance();
    if (instance->isGripperVacuumPollActive()) {
        QLOG_WARN() << "[GripperVac] 测试流程清除残留负压轮询，改由经典抬Z应答推进";
        instance->stopGripperVacuumPoll();
        stopGripperVacuumPollTimer();
    }
    return false;
}

bool TestProjectProcess::tryStartGripperPollIfWaiting(const QByteArray &sendData)
{
    Q_UNUSED(sendData);
    auto *instance = StructInstance::getInstance();
    if (instance->isGripperVacuumPollActive()) {
        instance->stopGripperVacuumPoll();
        stopGripperVacuumPollTimer();
    }
    return false;
}

void TestProjectProcess::startGripperVacuumPoll()
{
    if (!m_gripperVacuumPollTimer) {
        return;
    }
    if (!m_gripperVacuumPollTimer->isActive()) {
        m_gripperVacuumPollTimer->start();
    }
    onGripperVacuumPollTick();
}

void TestProjectProcess::stopGripperVacuumPollTimer()
{
    if (m_gripperVacuumPollTimer) {
        m_gripperVacuumPollTimer->stop();
    }
}

void TestProjectProcess::onGripperVacuumPollTick()
{
    auto *instance = StructInstance::getInstance();
    if (!instance->isGripperVacuumPollActive()) {
        stopGripperVacuumPollTimer();
        return;
    }

    const StructInstance::GripperVacuumPollResult deadlineHit = instance->tickGripperVacuumPollDeadline();
    if (deadlineHit.gripperFailed
            || deadlineHit.allFinished
            || !deadlineHit.sendData.isEmpty()) {
        applyGripperVacuumPollResult(deadlineHit, tr("抓手负压检测"));
        return;
    }

    if (m_outstandingAcks > 0) {
        QLOG_DEBUG() << "[GripperVac] 轮询tick跳过probe 待应答数" << m_outstandingAcks
                     << "焦点样本" << m_focus_sample_id;
        return;
    }

    const QByteArray probe = instance->gripperVacuumProbeCommand();
    if (!probe.isEmpty()) {
        QLOG_DEBUG() << "[GripperVac] 轮询tick发送probe 焦点样本" << m_focus_sample_id
                     << "probe字节长" << probe.size();
        writeCommandHeader(probe, tr("读抓手负压"), DELAY_TIMENUM);
    }
}

void TestProjectProcess::applyGripperVacuumPollResult(
        const StructInstance::GripperVacuumPollResult &result,
        const QString &defaultTips)
{
    if (result.gripperFailed) {
        stopGripperVacuumPollTimer();
        StructInstance::getInstance()->stopGripperVacuumPoll();
        commandNotCompleteOrErr(GRIPPERMOVE_ABNORMALLY,
                                QByteArray(),
                                defaultTips + QString::fromUtf8("复位等待处理"),
                                false,
                                false);
        return;
    }

    if (result.allFinished) {
        stopGripperVacuumPollTimer();
    }

    if (!result.sendData.isEmpty()) {
        stopGripperVacuumPollTimer();
        const QString tips = result.tips.isEmpty() ? defaultTips : result.tips;
        quint8 nextIndex = 255;
        if (result.sendData.size() > GET_COMMAND_INDEX) {
            bool ok = false;
            nextIndex = static_cast<quint8>(
                        result.sendData.mid(GET_COMMAND_INDEX, 1).toHex().toUShort(&ok, HEX_SWITCH));
        }
        QLOG_INFO() << "[GripperVac] 轮询结果推进流程 样本" << result.sampleId
                    << "下发命令号" << nextIndex << "lateTimer" << result.lateTimer
                    << "tips" << tips;
        commandNotCompleteOrErr(GRIPPERMOVE_NORMAL, result.sendData, tips, result.lateTimer, false);
    }
}

void TestProjectProcess::armGripperReadAfterOpenVacuum(quint8 suckCheckIndex,
                                                       quint8 positionIndex,
                                                       bool grabAtTray,
                                                       quint8 holdQueue,
                                                       quint8 reagentIndex,
                                                       const QString &label,
                                                       const QString &failTips)
{
    if (!StructInstance::getInstance()->gripperNeedsDelayBeforeRead()) {
        return;
    }
    m_gripperVacuumHoldEval.pending = true;
    m_gripperVacuumHoldEval.sampleId = m_focus_sample_id;
    m_gripperVacuumHoldEval.holdQueue = holdQueue;
    m_gripperVacuumHoldEval.reagentIndex = reagentIndex;
    m_gripperVacuumHoldEval.suckCheckIndex = suckCheckIndex;
    m_gripperVacuumHoldEval.positionIndex = positionIndex;
    m_gripperVacuumHoldEval.grabAtTray = grabAtTray;
    m_gripperVacuumHoldEval.actionLabel = label;
    m_gripperVacuumHoldEval.failTips = failTips;
}

void TestProjectProcess::commandNotCompleteOrErr(const bool isGripperError,const QByteArray senddata,
                                                   const QString tips,const bool klatetimer,
                                                   const bool isBackReagentNeedle)
{
    if(isGripperError == GRIPPERMOVE_NORMAL && klatetimer)
    {
        if (m_gripperVacuumHoldEval.pending) {
            m_gripperVacuumHoldEval.pending = false;
        }
        int waitMs = StructInstance::getInstance()->takeGripperWaitMs();
        if (waitMs <= 0) {
            waitMs = GlobalData::gripperGrabDelayMs();
        }
        QPointer<TestProjectProcess> guard(this);
        QTimer::singleShot(waitMs, this, [guard, senddata, tips, waitMs]() {
            if (!guard) {
                return;
            }
            QLOG_INFO() << "[抓放杯] 延时" << waitMs << "ms到，发出命令号"
                        << commandIndexOf(senddata) << tips;
            guard->writeCommandHeader(senddata, tips, DELAY_TIMENUM);
        });
    }
    //不是抓手部分的指令继续发送
    else if(isGripperError == GRIPPERMOVE_NORMAL && !klatetimer)
    {
        if(isBackReagentNeedle){
            const int suckDelayMs  = INI_File().GetReagentAfterSuckLiftDelayMs();
            QPointer<TestProjectProcess> guard(this);
            QTimer::singleShot(suckDelayMs , this, [guard, senddata, tips,suckDelayMs]() {
                if (!guard)
                    return;
                guard->writeCommandHeader(senddata, tips,DELAY_TIMENUM);
                QLOG_DEBUG() << "试剂泵吸完，延时" << suckDelayMs << "ms后复位试剂针(抬针)";
            });
        }
        //正常继续发送
        else{
            writeCommandHeader(senddata,tips,DELAY_TIMENUM);
        }
    }
    else
    {
        clearPendingConsumableDeducts();
        auto* instance = StructInstance::getInstance();
        m_gripperErrTips = tips;
        instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_ABNORMALLY);
        instance->clearRecvPrepatch(m_focus_sample_id);
        m_gripperFailPromptShown = false;
        m_gripperRecoveryFinished = false;
        m_startClipAfterGripperRecovery = false;
        m_resumeNextReagentAfterGripGiveup = false;
        m_pauseCommandQueue.clear();
        m_gripperErrStartMs = QDateTime::currentMSecsSinceEpoch();
        const QByteArrayList outBackOriginCommand =
                GlobalData::errMachineBackOrigin(FOCUS_ERR_BACKORIGIN, MOTOR_HANDS_INDEX);
        instance->fillRecvPrepatch(m_focus_sample_id, outBackOriginCommand);
        m_gripperZStillDown = !outBackOriginCommand.isEmpty();
        m_gripperHomeCmdIndex = outBackOriginCommand.isEmpty()
                ? 0
                : commandIndexOf(outBackOriginCommand.first());
        QLOG_WARN() << "[抓手异常] 样本" << m_focus_sample_id
                    << "| 进入善后 | 原因:" << tips;
        for (int i = 0; i < outBackOriginCommand.size(); ++i) {
            QLOG_WARN() << "[抓手异常] 善后队列"
                        << recoveryStepName(i)
                        << "命令号" << commandIndexOf(outBackOriginCommand.at(i));
        }
        QLOG_WARN() << "[抓手异常] 样本" << m_focus_sample_id
                    << "| 开始逐步执行：先抓手复位，再双针，最后XY";
        const QString statusTips = tips + tr("：正在抬抓手，然后复位双针、XY回原点，请稍候");
        publishWorkflowStatus(statusTips);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        sendFirstPrepatchCommand(statusTips);
        QPointer<TestProjectProcess> gripGuard(this);
        QTimer::singleShot(GRIPPER_ERR_TIMEOUT_MS, this, [gripGuard]() {
            if (!gripGuard || gripGuard->m_gripperErrStartMs <= 0) {
                return;
            }
            if (gripGuard->m_gripperFailPromptShown || gripGuard->m_gripperRecoveryFinished) {
                return;
            }
            auto *inst = StructInstance::getInstance();
            const quint8 indexActive =
                    inst->read_focusSampleActive(gripGuard->m_focus_sample_id);
            QLOG_WARN() << "[抓手异常] 样本" << gripGuard->m_focus_sample_id
                        << "| 善后超过30秒还没做完，弹出人工确认。抓手可能还在下面。"
                        << "点再次测试会先重做抬抓手，不会直接动XY | 原因:"
                        << gripGuard->m_gripperErrTips;
            gripGuard->m_gripperFailPromptShown = true;
            emit gripGuard->theGripperFailed(gripGuard->m_focus_sample_id,
                                             gripGuard->m_gripperErrTips,
                                             indexActive);
        });
    }
}


void TestProjectProcess::handlePppBaselineInvalid()
{
    m_waitingPppRead = false;
    m_pendingPppReread = true;
    disarmAckWatchdog();
    publishWorkflowStatus(tr("PPP初值异常，等待处理后继续重读"));
}

void TestProjectProcess::requestPppBaselineRead()
{
    if (m_offlineVoided && m_abandonAfterThrowPpp) {
        return;
    }
    if (!cglobal::gserialConnecStatus.load() || m_offlineWaitingReconnect) {
        m_pendingPppReread = true;
        m_waitingPppRead = false;
        logTestFsm(QStringLiteral("ppp_read_deferred"), tr("离线/重连等待"));
        return;
    }
    if (cglobal::g_PauseTesting.load()) {
        m_pendingPppReread = true;
        logTestFsmInfo(QStringLiteral("ppp_read_deferred"), tr("已暂停"));
        return;
    }
    m_pendingPppReread = false;
    m_waitingPppRead = true;
    logTestFsmInfo(QStringLiteral("ppp_read_start"), QString());
    emit getAnemiaValue();
    armAckWatchdog();
}

void TestProjectProcess::handlePrpBaselineInvalid()
{
    m_waitingPrpRead = false;
    m_pendingPrpReread = true;
    disarmAckWatchdog();
    publishWorkflowStatus(tr("PRP初值异常，等待处理后继续重读"));
}

void TestProjectProcess::markReagentSkipped(int sampleId, quint8 reagent)
{
    auto *sample = StructInstance::getInstance()->wfindIdSample(sampleId);
    if (!sample) {
        return;
    }
    for (auto *item : sample->ptestReagent_active) {
        if (item && item->index_Reagent == reagent) {
            item->testfinished = true;
            break;
        }
    }
}

void TestProjectProcess::handlePppZeroGiveUp(int sampleId, quint8 channel)
{
    m_waitingPppRead = false;
    m_pendingPppReread = false;
    disarmAckWatchdog();
    if (sampleId <= 0) {
        QLOG_ERROR() << "handlePppZeroGiveUp: 无效样本号";
        return;
    }
    if (m_focus_sample_id <= 0) {
        m_focus_sample_id = sampleId;
    } else if (m_focus_sample_id != sampleId) {
        QLOG_WARN() << "handlePppZeroGiveUp: 焦点样本" << m_focus_sample_id
                    << "与PPP异常样本" << sampleId << "不一致，按PPP样本处理";
        m_focus_sample_id = sampleId;
    }

    const QString tips = tr("样本%1 PPP读初值异常，丢弃通道PPP并放弃该样本").arg(sampleId);
    promptsareDisplayed(TESTINGLOG, tips, false);
    publishWorkflowStatus(tips);
    m_abandonAfterThrowPpp = true;
    throwAnemiaCup(channel);
}

void TestProjectProcess::handlePrpZeroSkipReagent(int sampleId, quint8 reagent, quint8 /*focusActive*/)
{
    m_waitingPrpRead = false;
    m_pendingPrpReread = false;
    disarmAckWatchdog();
    if (sampleId <= 0 || reagent == 0) {
        QLOG_ERROR() << "handlePrpZeroSkipReagent: 样本或试剂无效";
        return;
    }

    markReagentSkipped(sampleId, reagent);

    const QString tips = tr("样本%1 %2 PRP读初值异常，丢弃当前试剂杯")
                             .arg(sampleId)
                             .arg(GlobalData::mapIndexReagentnames(reagent));
    promptsareDisplayed(TESTINGLOG, tips, false);
    publishWorkflowStatus(tips);

    m_prpZeroSkipAfterThrow = true;
    setupFocusSample(sampleId, reagent);
    outputThrowTestChannelReagentFinish(sampleId, reagent);
}

void TestProjectProcess::requestPrpBaselineRead()
{
    requestPrpBaselineRead(m_pendingPrpReagent, m_pendingPrpFocus);
}

void TestProjectProcess::requestPrpBaselineRead(quint8 reagentIndex, quint8 focusActive)
{
    if (reagentIndex == 0 || focusActive == 0) {
        QLOG_ERROR() << "requestPrpBaselineRead: 试剂或焦点无效";
        return;
    }
    m_pendingPrpReagent = reagentIndex;
    m_pendingPrpFocus = focusActive;
    if (m_offlineVoided) {
        m_pendingPrpReread = true;
        m_waitingPrpRead = false;
        return;
    }
    if (!cglobal::gserialConnecStatus.load() || m_offlineWaitingReconnect) {
        m_pendingPrpReread = true;
        m_waitingPrpRead = false;
        logTestFsm(QStringLiteral("prp_read_deferred"), tr("离线/重连等待"));
        return;
    }
    if (cglobal::g_PauseTesting.load()) {
        m_pendingPrpReread = true;
        logTestFsmInfo(QStringLiteral("prp_read_deferred"), tr("已暂停"));
        return;
    }
    m_pendingPrpReread = false;
    m_waitingPrpRead = true;
    logTestFsmInfo(QStringLiteral("prp_read_start"),
                   GlobalData::mapIndexReagentnames(reagentIndex));
    emit readbloodyInitValue(reagentIndex, focusActive);
    armAckWatchdog();
}

void TestProjectProcess::enqueueContinueReagent(int sampleId)
{
    if (sampleId <= 0) {
        return;
    }
    if (!m_pendingContinueReagent.contains(sampleId)) {
        m_pendingContinueReagent.enqueue(sampleId);
    }
}

void TestProjectProcess::removeSampleFromSchedulerQueues(int sampleId)
{
    m_pendingContinueReagent.removeAll(sampleId);
}

void TestProjectProcess::dismissFocusSample(int sampleId)
{
    if (m_focus_sample_id == sampleId) {
        m_pauseCommandQueue.clear();
    }
    StructInstance::getInstance()->config_dismissFocusSample(sampleId);
}

void TestProjectProcess::releaseSampleChannel(int sampleId, bool flashAsError)
{
    auto* instance = StructInstance::getInstance();
    const auto *itsample = instance->rfindIdSample(sampleId);
    if (!itsample || itsample->indexTestingChn < 0) {
        return;
    }
    const quint8 chn = static_cast<quint8>(itsample->indexTestingChn);
    emit openChnMotor(chn, false);
    emit stopChannelAcquisition(chn);
    emit setprogressinitboolwaittext(false, chn, flashAsError ? tr("[异常]") : QString());
    instance->config_testChn_State(chn, CHN_STATUS_FREE);
    instance->config_testChn_test_SampleID(chn, 0);
    instance->_deleteOneSampleTesteddata(chn, sampleId);
    if (flashAsError) {
        emit giveupChannelFlash(false, chn);
    }
}

bool TestProjectProcess::startPendingContinueReagent()
{
    while (!m_pendingContinueReagent.isEmpty()) {
        const int sampleId = m_pendingContinueReagent.dequeue();
        if (beginNextReagentForSample(sampleId)) {
            return true;
        }
    }
    return false;
}

bool TestProjectProcess::beginNextReagentForSample(int sampleId)
{
    auto* instance = StructInstance::getInstance();
    if (!instance->rfindIdSample(sampleId)) {
        return false;
    }

    quint8 waittest_reag = 0;
    if (!instance->judge_hadWaitTestReag(sampleId, waittest_reag)) {
        return false;
    }

    const ReagentWorkflow workflow = s_reagentWorkflow.value(waittest_reag);
    if (workflow.testingFocus == 0) {
        QLOG_ERROR() << "beginNextReagentForSample: 未知试剂" << waittest_reag;
        return false;
    }

    const QString outLog = GlobalData::mapIndexReagentnames(waittest_reag);
    const int previousFocusId = m_focus_sample_id;
    m_focus_sample_id = sampleId;
    if (!controlsuckReagentLoc(workflow.consumableIndex, waittest_reag)) {
        m_focus_sample_id = previousFocusId;
        enqueueContinueReagent(sampleId);
        QLOG_DEBUG() << "待测试剂" << outLog << "不足，未占用焦点，样本放回继续队列";
        return true;
    }

    instance->config_focusSample(sampleId, true);
    m_focus_sample_id = sampleId;
    instance->write_focusSampleActive(sampleId, workflow.testingFocus);

    const QString continueLog = QString::fromUtf8("样本%1继续吸试剂:%2")
            .arg(sampleId).arg(outLog);
    promptsareDisplayed(TESTINGLOG, continueLog, false);
    writeCommandHeader(instance->sendsuckReagentCatchCuptoTestChannelHead(sampleId, waittest_reag),
                       continueLog,
                       DELAY_TIMENUM);
    return true;
}

void TestProjectProcess::noteCommandsSent(int count)
{
    if (count <= 0) {
        return;
    }
    m_outstandingAcks += count;
    logTestFsmInfo(QStringLiteral("ack_sent"), tr("+%1 -> %2").arg(count).arg(m_outstandingAcks));
    armAckWatchdog();
}

void TestProjectProcess::noteAckReceived()
{
    if (m_outstandingAcks > 0) {
        --m_outstandingAcks;
    }
    logTestFsmInfo(QStringLiteral("ack_recv"), tr("-> %1").arg(m_outstandingAcks));
    if (m_outstandingAcks > 0 || m_waitingPppRead || m_waitingPrpRead) {
        armAckWatchdog();
    } else {
        disarmAckWatchdog();
    }
}

void TestProjectProcess::armAckWatchdog()
{
    if (!m_ackWatchdog || !cglobal::g_StartTesting.load() || cglobal::g_PauseTesting.load()
            || !cglobal::gserialConnecStatus.load() || m_offlineWaitingReconnect) {
        return;
    }
    int waitMs = ACK_WATCHDOG_MS;
    const int washMs = static_cast<int>(INI_File().getWashesTime());
    if (washMs > 0) {
        waitMs = qMax(waitMs, washMs + 10000);
    }
    if (m_Errorresetsfirst) {
        waitMs = qMax(waitMs, RESET_FALLBACK_MS);
    }
    m_ackWatchdog->start(waitMs);
}

void TestProjectProcess::disarmAckWatchdog()
{
    if (m_ackWatchdog) {
        m_ackWatchdog->stop();
    }
}




//接收丟PPP命令 命令号，从机地址，运动电机，数据
void TestProjectProcess::recv_throwAnemia(const int indexconde,int slaveAddr,int Action_motor,
                                           const QStringList recvdata)
{
    auto* instance = StructInstance::getInstance();
    if (ignoreRestartLeftoverAck(Action_motor, recvdata)) {
        return;
    }
    if (!instance->throwPppContainsCommandIndex(
                m_focus_sample_id, static_cast<quint8>(indexconde))) {
        QLOG_WARN() << "丢PPP忽略非本序列应答 样本" << m_focus_sample_id
                    << "命令号" << indexconde << "从机" << slaveAddr
                    << "电机" << Action_motor;
        return;
    }

    QByteArray sendData;
    sendData.clear();
    quint32 airvale = 0;
    bool isSuction = false;

    const bool isHandsControl = (slaveAddr == Z_AXIS && Action_motor == MOTOR_HANDS_INDEX);
    if(isHandsControl) {
       QString outputTips = GlobalData::analyzeWhetherGripperSuction(recvdata,isSuction,airvale);
       promptsareDisplayed(TESTINGLOG,outputTips + "(丟PPP动作)",false);
    }

    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    const bool kthrowCupFinish = instance->recv_throwCupcommd(m_focus_sample_id,indexconde,isHandsControl,isSuction,airvale,
                                                              sendData,kGripperFailed,klaterTimer);
    if(kthrowCupFinish){

        //计数杯子
        emit throwtesttube();

        if (m_abandonAfterThrowPpp) {
            const int abandonId = m_focus_sample_id;
            m_abandonAfterThrowPpp = false;
            promptsareDisplayed(TESTINGLOG,
                                tr("样本%1 PPP已丢弃，放弃该样本，后续PRP不测").arg(abandonId),
                                false);
            giveupSamplenum(abandonId);
            return;
        }

        quint8 testingReag = 0;
        instance->reagentTestintotheStructure(m_focus_sample_id,testingReag);
        Q_UNUSED(testingReag);

        enqueueContinueReagent(m_focus_sample_id);
        dismissFocusSample(m_focus_sample_id);
        TraverseChnHadFinishTesting(THROW_CUPS_END);
        return;

    }else{
        const QString illustrateText = (isSuction ? tr("丢PPP杯3次失败")
                                                  : tr("PPP丢到弃杯孔3次失败"));
        const QString tips = kGripperFailed
                ? illustrateText + tr("复位等待处理")
                : tr("丢PPP试管到弃杯孔");
        if (klaterTimer && isHandsControl && kGripperFailed == GRIPPERMOVE_NORMAL) {
            quint8 suckCheck = 0;
            quint8 channel = 0;
            if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
                if (ps->throwPPPsuckBacknum >= 2) {
                    suckCheck = static_cast<quint8>(ps->throwPPPsuckBacknum - 1);
                }
                if (ps->indexTestingChn >= 0) {
                    channel = static_cast<quint8>(ps->indexTestingChn);
                }
            }
            armGripperReadAfterOpenVacuum(suckCheck, channel, false, 0, 0,
                                          tr("丢PPP"),
                                          illustrateText + tr("（下扎加深仍吸不牢）"));
        }
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer,false);
    }
    return;
}


void TestProjectProcess::recvNextTestReagent(quint8 reagentIndex, quint8 focusActive,
                                             const int commandIndex, const int slaveAddr,
                                             const int Action_motor, const QStringList &recvdata)
{
    const int latencyTime = INI_File()._getdelayedtime();
    const QString reagentName = GlobalData::mapIndexReagentnames(reagentIndex);

    auto* instance = StructInstance::getInstance();
    if (ignoreRestartLeftoverAck(Action_motor, recvdata)) {
        return;
    }
    if (!instance->prpClipContainsCommandIndex(
                m_focus_sample_id, reagentIndex, static_cast<quint8>(commandIndex))) {
        QLOG_WARN() << "抓PRP忽略非本序列应答 样本" << m_focus_sample_id
                    << reagentName << "命令号" << commandIndex
                    << "从机" << slaveAddr << "电机" << Action_motor;
        return;
    }

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
        GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
        if (tryFeedActiveGripperVacuumPoll(
                airValue, isSuction,
                tr("继续发送吸%1(PRP)到通道测试指令").arg(reagentName))) {
            return;
        }
    } else {
        notifyHangLiquidWipeIfNeeded(slaveAddr, Action_motor, recvdata, false);
    }
    if (!handsControlActive && isMainControlMotorXY(slaveAddr, Action_motor)) {
        QPoint moveingAxis = updateEmptyClipAway(recvdata);
        if (SingletonAxis::GetInstance()->ismoveXYsuckReagent(moveingAxis)) {
            FindSuckReagentAxis(moveingAxis, reagentIndex);
        }
    }

    QByteArray sendData;
    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    bool bnextBackReagentNeedle = false;
    const bool actionCompleted = instance->recvSuckReagentClipTube(
                m_focus_sample_id, commandIndex, reagentIndex,
                handsControlActive, isSuction, airValue, sendData,
                kGripperFailed, klaterTimer, bnextBackReagentNeedle);

    if (actionCompleted) {
        deductPendingReagentConsumable();
        QLOG_DEBUG() << "样本" << m_focus_sample_id << reagentName
                     << "开始延时准备读取PRP初值";
        publishWorkflowStatus(tr("读取%1初值").arg(reagentName));
        const int delayMs = DELAY_READ_TUBE_INITVALUE
                + ((reagentIndex == AA_REAGENT) ? 0 : latencyTime);
        QPointer<TestProjectProcess> guard(this);
        QTimer::singleShot(delayMs, this, [guard, reagentIndex, focusActive]() {
            if (guard)
                guard->requestPrpBaselineRead(reagentIndex, focusActive);
        });
        return;
    }

    const QString illustrateText = isSuction
            ? tr("吸%1(PRP)试杯失败").arg(reagentName)
            : tr("放%1(PRP)试杯到通道失败").arg(reagentName);
    const QString tips = kGripperFailed
            ? illustrateText
            : tr("继续发送吸%1(PRP)到通道测试指令").arg(reagentName);
    if (klaterTimer && handsControlActive && kGripperFailed == GRIPPERMOVE_NORMAL
            && !bnextBackReagentNeedle) {
        quint8 suckCheck = 0;
        quint8 prpHole = 0;
        if (DATASAMPLESTRUCT *ps = instance->wfindIdSample(m_focus_sample_id)) {
            for (const BLOODY_INFO &bh : ps->bloody_tube) {
                if (bh.index_reagent == reagentIndex) {
                    prpHole = bh.bloody_hole;
                    break;
                }
            }
            for (Testing_reagents *reag : ps->ptestReagent_active) {
                if (reag && reag->index_Reagent == reagentIndex && reag->testPRPsucknum >= 2) {
                    suckCheck = static_cast<quint8>(reag->testPRPsucknum - 1);
                    break;
                }
            }
        }
        armGripperReadAfterOpenVacuum(suckCheck, prpHole, true, 2, reagentIndex,
                                      tr("抓%1").arg(reagentName),
                                      illustrateText + tr("（下扎加深仍吸不牢）"));
        commandNotCompleteOrErr(kGripperFailed, sendData, tips, klaterTimer, bnextBackReagentNeedle);
        return;
    }

    if (kGripperFailed == GRIPPERMOVE_NORMAL && sendData.isEmpty()) {
        if (tryStartGripperPollIfWaiting(sendData)) {
            return;
        }
        stallWithPause(QStringLiteral("prp_clip_stall"),
                       tr("抓PRP(%1)无法推进(cmd=%2)，已暂停").arg(reagentName).arg(commandIndex),
                       true);
        return;
    }

    commandNotCompleteOrErr(kGripperFailed, sendData, tips, klaterTimer, bnextBackReagentNeedle);
}

void TestProjectProcess::recvNextTestAA(const int commandIndex,const int slaveAddr,const int Action_motor,
                                           const QStringList &recvdata){
    recvNextTestReagent(AA_REAGENT, FOCUS_TESTING_AA, commandIndex, slaveAddr, Action_motor, recvdata);
}


void TestProjectProcess::recvNextTestADP(const int commandIndex,const int slaveAddr,const int Action_motor,
                                          const QStringList &recvdata)
{
    recvNextTestReagent(ADP_REAGENT, FOCUS_TESTING_ADP, commandIndex, slaveAddr, Action_motor, recvdata);
}


void TestProjectProcess::recvNextTestEPI(const int commandIndex,const int slaveAddr,const int Action_motor,
                                          const QStringList &recvdata)
{
    recvNextTestReagent(EPI_REAGENT, FOCUS_TESTING_EPI, commandIndex, slaveAddr, Action_motor, recvdata);
}


void TestProjectProcess::recvNextTestCOL(const int commandIndex,const int slaveAddr,const int Action_motor,
                                         const QStringList &recvdata)
{
    recvNextTestReagent(COL_REAGENT, FOCUS_TESTING_COL, commandIndex, slaveAddr, Action_motor, recvdata);
}


void TestProjectProcess::recvNextTestRIS(const int commandIndex,const int slaveAddr,const int Action_motor,
                                           const QStringList &recvdata){
    recvNextTestReagent(RIS_REAGENT, FOCUS_TESTING_RIS, commandIndex, slaveAddr, Action_motor, recvdata);
}






bool TestProjectProcess::restartClipPppAfterHome(const QString& tips)
{
    m_gripperRestartAwait = GripperRestartPppTray;
    m_restartReagent = 0;
    const QByteArray firstCmd =
            StructInstance::getInstance()->restartPppClipFromXy(m_focus_sample_id);
    if (firstCmd.isEmpty()) {
        m_gripperRestartAwait = GripperRestartNone;
        const QString outerr = tr("复位后无法重建夹PPP指令，停止推进");
        QLOG_ERROR() << outerr << "样本" << m_focus_sample_id;
        publishWorkflowStatus(outerr);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        promptsareDisplayed(TESTINGLOG, outerr, true);
        return false;
    }
    promptsareDisplayed(TESTINGLOG, tips, false);
    writeCommandHeader(firstCmd, tips, DELAY_TIMENUM);
    return true;
}

bool TestProjectProcess::restartThrowPppAfterHome(const QString& tips)
{
    m_gripperRestartAwait = GripperRestartThrowChannel;
    m_restartReagent = 0;
    const QByteArray firstCmd =
            StructInstance::getInstance()->restartThrowPppFromXy(m_focus_sample_id);
    if (firstCmd.isEmpty()) {
        m_gripperRestartAwait = GripperRestartNone;
        const QString outerr = tr("复位后无法重建丢PPP指令，停止推进");
        QLOG_ERROR() << outerr << "样本" << m_focus_sample_id;
        publishWorkflowStatus(outerr);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        promptsareDisplayed(TESTINGLOG, outerr, true);
        return false;
    }
    promptsareDisplayed(TESTINGLOG, tips, false);
    writeCommandHeader(firstCmd, tips, DELAY_TIMENUM);
    return true;
}

bool TestProjectProcess::restartThrowReagentAfterHome(quint8 reagent, const QString& tips)
{
    m_gripperRestartAwait = GripperRestartThrowChannel;
    m_restartReagent = reagent;
    const QByteArray firstCmd =
            StructInstance::getInstance()->restartThrowReagentFromXy(m_focus_sample_id, reagent);
    if (firstCmd.isEmpty()) {
        m_gripperRestartAwait = GripperRestartNone;
        const QString outerr = tr("复位后无法重建丢试剂杯指令，停止推进");
        QLOG_ERROR() << outerr << "样本" << m_focus_sample_id << "试剂" << reagent;
        publishWorkflowStatus(outerr);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        promptsareDisplayed(TESTINGLOG, outerr, true);
        return false;
    }
    promptsareDisplayed(TESTINGLOG, tips, false);
    writeCommandHeader(firstCmd, tips, DELAY_TIMENUM);
    return true;
}

bool TestProjectProcess::restartPrpAfterHome(quint8 reagent, const QString& tips)
{
    auto *inst = StructInstance::getInstance();
    m_restartReagent = reagent;
    const bool suckAlreadyDone = inst->prpReagentPortionCompleted(m_focus_sample_id, reagent);
    m_gripperRestartAwait = suckAlreadyDone ? GripperRestartPrpTray : GripperRestartPrpReagent;
    QByteArray firstCmd =
            inst->restartPrpClipResumePoint(m_focus_sample_id, reagent);
    if (firstCmd.isEmpty()) {
        m_gripperRestartAwait = GripperRestartNone;
        const QString outerr = tr("复位后无法重建抓PRP指令，停止推进");
        QLOG_ERROR() << outerr << "样本" << m_focus_sample_id << "试剂" << reagent;
        publishWorkflowStatus(outerr);
        if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
            emit pauseTesting();
        }
        promptsareDisplayed(TESTINGLOG, outerr, true);
        return false;
    }
    if (!suckAlreadyDone) {
        const ReagentWorkflow workflow = s_reagentWorkflow.value(reagent);
        if (workflow.consumableIndex == 255) {
            m_gripperRestartAwait = GripperRestartNone;
            const QString outerr = tr("复位后无法重建抓PRP指令，停止推进");
            QLOG_ERROR() << outerr << "样本" << m_focus_sample_id << "试剂" << reagent;
            publishWorkflowStatus(outerr);
            if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
                emit pauseTesting();
            }
            promptsareDisplayed(TESTINGLOG, outerr, true);
            return false;
        }
        if (!controlsuckReagentLoc(workflow.consumableIndex, reagent)) {
            m_gripperRestartAwait = GripperRestartNone;
            return false;
        }
        firstCmd = inst->sendsuckReagentCatchCuptoTestChannelHead(m_focus_sample_id, reagent);
        if (firstCmd.isEmpty()) {
            m_gripperRestartAwait = GripperRestartNone;
            const QString outerr = tr("复位后无法重建抓PRP指令，停止推进");
            QLOG_ERROR() << outerr << "样本" << m_focus_sample_id << "试剂" << reagent;
            publishWorkflowStatus(outerr);
            if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
                emit pauseTesting();
            }
            promptsareDisplayed(TESTINGLOG, outerr, true);
            return false;
        }
    }
    promptsareDisplayed(TESTINGLOG, tips, false);
    writeCommandHeader(firstCmd, tips, DELAY_TIMENUM);
    return true;
}

bool TestProjectProcess::restartGripperFocusAfterHome(quint8 indexActive, const QString& tips)
{
    if (indexActive == FOCUS_CLIP_ANEMIA_TO_CHN) {
        if (StructInstance::getInstance()->pppClipFinishedAwaitingRead(m_focus_sample_id)) {
            return false;
        }
        restartClipPppAfterHome(tips);
        return true;
    }
    if (indexActive == FOCUS_THORW_ANEMIA) {
        restartThrowPppAfterHome(tips);
        return true;
    }
    const quint8 reagent = reagentFromFocus(indexActive);
    if (indexActive == FOCUS_THROW_AA_TUBE || indexActive == FOCUS_THROW_ADP_TUBE
            || indexActive == FOCUS_THROW_EPI_TUBE || indexActive == FOCUS_THROW_COL_TUBE
            || indexActive == FOCUS_THROW_RIS_TUBE) {
        restartThrowReagentAfterHome(reagent, tips);
        return true;
    }
    if (indexActive == FOCUS_TESTING_AA || indexActive == FOCUS_TESTING_ADP
            || indexActive == FOCUS_TESTING_EPI || indexActive == FOCUS_TESTING_COL
            || indexActive == FOCUS_TESTING_RIS) {
        if (StructInstance::getInstance()->prpClipFinishedAwaitingRead(m_focus_sample_id, reagent)) {
            return false;
        }
        restartPrpAfterHome(reagent, tips);
        return true;
    }
    return false;
}

bool TestProjectProcess::ignoreRestartLeftoverAck(int Action_motor, const QStringList &recvdata)
{
    if (m_gripperRestartAwait == GripperRestartNone) {
        return false;
    }
    if (Action_motor != MOTOR_XY) {
        QLOG_WARN() << "复位后忽略残留轴应答 样本" << m_focus_sample_id
                    << "电机" << Action_motor;
        return true;
    }
    const QPoint arrived = QUIUtils::coordinatePositionXYMotion(recvdata);
    bool atTarget = false;
    StructInstance *instance = StructInstance::getInstance();
    switch (m_gripperRestartAwait) {
    case GripperRestartPppTray:
        atTarget = instance->pppClipXyAtTray(m_focus_sample_id, arrived);
        break;
    case GripperRestartThrowChannel:
        atTarget = instance->gripperXyAtChannel(m_focus_sample_id, arrived);
        break;
    case GripperRestartPrpReagent:
        atTarget = instance->prpClipXyAtReagent(m_focus_sample_id, m_restartReagent, arrived);
        break;
    case GripperRestartPrpTray:
        atTarget = instance->prpClipXyAtTrayHole(m_focus_sample_id, m_restartReagent, arrived);
        break;
    default:
        break;
    }
    if (!atTarget) {
        QLOG_WARN() << "复位后忽略非目标XY 样本" << m_focus_sample_id
                    << "坐标" << arrived.x() << "," << arrived.y();
        return true;
    }
    m_gripperRestartAwait = GripperRestartNone;
    return false;
}

void TestProjectProcess::exceptionRetest(QString tips){

    if (!cglobal::g_StartTesting.load()) {
        return;
    }

    m_offlineWaitingReconnect = false;

    // 重连复位完成后：按快照恢复通道搅拌并更新状态条
    applyReconnectSnapshotAfterReset(tips);

    if (m_offlineVoided) {
        recoverAfterOfflineVoid(tips);
        return;
    }

    auto* instance = StructInstance::getInstance();
    const bool hadTestingFocus = instance->sampleHadFocus();
    if(hadTestingFocus){
        quint8  indexActive = instance->read_focusSampleActive(m_focus_sample_id);
        if (restartGripperFocusAfterHome(indexActive, tr("复位后从XY重跑当前抓手动作"))) {
            return;
        }
        QByteArray outputData = instance->outputstheFirstIncompleteInstruction(m_focus_sample_id,indexActive);
        if(!outputData.isEmpty()){
            writeCommandHeader(outputData,tips,DELAY_TIMENUM);
        } else {
            QByteArrayList outputDatalist = instance->outputsIncompleteInstructionList(m_focus_sample_id,indexActive);
            if(!outputDatalist.isEmpty()){
                sendInstructionsWithDelay(outputDatalist,DELAY_TIMENUM,tips);
            } else if (indexActive == FOCUS_CLIP_ANEMIA_TO_CHN) {
                promptsareDisplayed(TESTINGLOG, tr("夹PPP已完成，延时后重新读取PPP初值"), false);
                QPointer<TestProjectProcess> pppGuard(this);
                QTimer::singleShot(DELAY_READ_TUBE_INITVALUE, this, [pppGuard]() {
                    if (pppGuard) {
                        pppGuard->requestPppBaselineRead();
                    }
                });
            } else if (indexActive == FOCUS_TESTING_AA || indexActive == FOCUS_TESTING_ADP
                       || indexActive == FOCUS_TESTING_EPI || indexActive == FOCUS_TESTING_COL
                       || indexActive == FOCUS_TESTING_RIS) {
                quint8 reagent = 0;
                for (auto it = s_reagentWorkflow.cbegin(); it != s_reagentWorkflow.cend(); ++it) {
                    if (it.value().testingFocus == indexActive) {
                        reagent = it.key();
                        break;
                    }
                }
                promptsareDisplayed(TESTINGLOG, tr("抓PRP已完成，延时后重新读取PRP初值"), false);
                QPointer<TestProjectProcess> prpGuard(this);
                QTimer::singleShot(DELAY_READ_TUBE_INITVALUE, this, [prpGuard, reagent, indexActive]() {
                    if (prpGuard) {
                        prpGuard->requestPrpBaselineRead(reagent, indexActive);
                    }
                });
            } else if (startPendingContinueReagent()) {
                return;
            } else {
                QLOG_DEBUG()<<tips<<"无未发送指令"<<endl;
                promptsareDisplayed(TESTINGLOG,QString("%1无未发送指令").arg(tips),false);
                if (cglobal::g_StartTesting.load() && !cglobal::g_PauseTesting.load()) {
                    emit pauseTesting();
                }
            }
        }

    }else
    {
        if(!m_finishTestData.isEmpty())
        {
            Finish_Data_ complete_ = m_finishTestData.dequeue();
            quint8 test_finish_chn = complete_.index_Chn;
            quint8 finish_reag =  complete_.index_reagent;

            QString samplename = "";
            int sampleId = 0;
            auto *instance = StructInstance::getInstance();
            if (complete_.CompleteSampleid > 0) {
                const DATASAMPLESTRUCT *ps = instance->rfindIdSample(complete_.CompleteSampleid);
                if (ps && ps->indexTestingChn == static_cast<int>(test_finish_chn)) {
                    sampleId = complete_.CompleteSampleid;
                    samplename = ps->samplestr;
                } else {
                    m_finishTestData.enqueue(complete_);
                    if (startPendingContinueReagent()) {
                        return;
                    }
                    StartSampleAdditionWorkflow();
                    return;
                }
            }
            if (sampleId <= 0) {
                instance->rootTestChnGetSampleInfo(test_finish_chn, samplename, sampleId);
            }
            if (sampleId <= 0) {
                QLOG_ERROR() << "完成队列通道" << (test_finish_chn + 1) << "未找到样本，跳过弃杯";
                if (!startPendingContinueReagent()) {
                    StartSampleAdditionWorkflow();
                }
                return;
            }
            StructInstance::getInstance()->config_focusSample(sampleId,true);//++焦点
            m_focus_sample_id = sampleId;
            const quint8 focus_index_set = s_reagentWorkflow.value(finish_reag).throwFocus;
            StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
            emit setprogressinitboolwaittext(true,test_finish_chn,"");

            outputThrowTestChannelReagentFinish(m_focus_sample_id,finish_reag);
        }
        else
        {
            QString outtext = tips + QString::fromUtf8("测试通道内无完成测试样本,继续遍历加样样本");
            promptsareDisplayed(TESTINGLOG,outtext,false);
            if (!startPendingContinueReagent()) {
                StartSampleAdditionWorkflow();
            }
        }
    }
}


