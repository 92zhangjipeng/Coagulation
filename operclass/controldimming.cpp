#ifdef _MSC_VER
#pragma warning(disable: 4819)
#pragma execution_character_set("utf-8")
#endif
#include "controldimming.h"

#include "QsLog/include/QsLog.h"
#include "cglobal.h"
#include <operclass/fullyautomatedplatelets.h>
#include <ini_file.h>
#include <structinstance.h>
#include <globaldata.h>
#include <QtGlobal>
#include <QDateTime>
#include <QTimer>
#include <cmath>

namespace {
constexpr int kEmptyChannelAdcTarget = 3300; // 空通道应对齐的 AD
constexpr int kDimmingAbsThreshold = 100;    // |空通道ADC - 3300|，约 3200~3400
constexpr int kSettleAfterReadyMs = 2000;    // 齐套后再跟最新 ADC 稳定 2 秒再判
constexpr int kMaxSmartCupClearRounds = 2;   // 未勾选「初始清空试杯」时自动丢杯轮数
qint64 g_dimmingSettleStartMs = 0;
}

controldimming::controldimming(QObject *parent)
    : QObject(parent)
    , m_startCollectChannelVal(false)
    , m_discardNextCompleteFrame(false)
    , m_frequency(0)
    , m_smartCupClearAttempts(0)
    , m_cupClearPending(false)
    , m_cupClearGen(0)
    , m_totalchn(0)
{
    m_testChnDimmingStu.clear();
}

controldimming::~controldimming()
{
    qDeleteAll(m_testChnDimmingStu);
    m_testChnDimmingStu.clear();
    QLOG_DEBUG() << "退出调光实例";
}

void controldimming::getdimming_status(bool &finished)
{
    finished = cglobal::g_controldimmingfinished.load();
}

void controldimming::sycnequipment(quint8 index)
{
    static const QHash<quint8, int> channelMap = {
        {KS600, 4},
        {KS800, 8},
        {KS1200, 12}
    };
    m_totalchn = static_cast<quint8>(channelMap.value(index, 0));
    QLOG_INFO() << "调光同步机型通道数:" << m_totalchn << "机型:" << index;
}

bool controldimming::moduleAlreadyRegistered(int module) const
{
    for (const ModuleChannelData *p : m_testChnDimmingStu) {
        if (p && p->ModuleNum == static_cast<quint8>(module)) {
            return true;
        }
    }
    return false;
}

void controldimming::markDimmingFinished(const QString &reason)
{
    cglobal::g_controldimmingfinished = true;
    m_smartCupClearAttempts = 0;
    m_cupClearPending = false;
    m_startCollectChannelVal = false;
    emit requestDimmingProgress(false);
    emit reminderText(PROMPTLOG, reason);
    QLOG_INFO() << reason;
}

void controldimming::recvmoduletemp(int module)
{
    if (m_totalchn == 0) {
        QLOG_ERROR() << "机型通道数未同步(m_totalchn=0)，忽略模组" << module << "控温调光";
        return;
    }
    if (cglobal::g_controldimmingfinished.load()) {
        QLOG_DEBUG() << "调光已完成，忽略模组" << module << "再次控温事件";
        return;
    }
    if (moduleAlreadyRegistered(module)) {
        QLOG_DEBUG() << "模组" << module << "已登记，跳过重复控温触发";
        return;
    }

    // 首个模组到达 37℃：开启本轮调光
    if (m_testChnDimmingStu.isEmpty()) {
        m_frequency = 0;
        m_smartCupClearAttempts = 0;
        dimmingModule.clear();
        m_startCollectChannelVal = false;
        m_discardNextCompleteFrame = false;
        cglobal::g_controldimmingfinished = false;
        QLOG_INFO() << "模组达控温阈值，开始空通道AD对齐检测(目标3300)";
    }

    constexpr int channelsPerModule = 4;
    const int startChannel = TEST_CHANNEL_1 + ((module - 1) * channelsPerModule);

    m_testChnDimmingStu.reserve(m_testChnDimmingStu.size() + channelsPerModule);
    for (int i = 0; i < channelsPerModule; ++i) {
        ModuleChannelData *pModuleData = new ModuleChannelData;
        pModuleData->ControlTemp = true;
        pModuleData->ModuleNum = static_cast<quint8>(module);
        pModuleData->ModuleDimmingVal = kEmptyChannelAdcTarget; // 空通道 AD 对齐 3300
        pModuleData->ledDriveVal = 0;
        pModuleData->indexChannel = static_cast<quint8>(startChannel + i);
        pModuleData->channdelData = 0;
        pModuleData->DimmingTimes = 0;
        pModuleData->bneedcontrastive = false;
        pModuleData->Dimmingsuccessful = false;
        m_testChnDimmingStu.push_back(pModuleData);
    }

    QLOG_INFO() << "模组" << module << "控温达标，登记空通道检测"
                << startChannel << "-" << (startChannel + channelsPerModule - 1)
                << "AD目标" << kEmptyChannelAdcTarget
                << "已登记" << m_testChnDimmingStu.size() << "/" << m_totalchn;

    if (m_testChnDimmingStu.size() >= m_totalchn) {
        m_startCollectChannelVal = true;
        m_discardNextCompleteFrame = true; // 丢掉刚到温的第一帧，避免未稳定就判失败
        emit requestDimmingProgress(true);
    }
}

void controldimming::Collectchannelvalues(QVector<ModuleChannelData *> &ChannelVet,
                                          quint8 indexChannel, int indexChannelData)
{
    for (ModuleChannelData *pmoduleState : ChannelVet) {
        if (pmoduleState
            && pmoduleState->ControlTemp
            && pmoduleState->indexChannel == indexChannel) {
            // 0 是模组报上来的读数（无光/模组无信号），不能丢掉，否则永远等不到齐套
            if (indexChannelData < 0)
                indexChannelData = 0;
            pmoduleState->channdelData = indexChannelData;
            pmoduleState->bneedcontrastive = true;
            break;
        }
    }
}

void controldimming::handleFailedChannels(const QSet<quint8>& failedChannels)
{
    dimmingModule.clear();
    QList<quint8> onceDimmingChn;

    for (ModuleChannelData *pModule : m_testChnDimmingStu) {
        if (!pModule || !failedChannels.contains(pModule->indexChannel)) {
            continue;
        }
        pModule->DimmingTimes = static_cast<quint8>(qMin(m_frequency + 1, 3));
        if (!onceDimmingChn.contains(pModule->indexChannel)) {
            onceDimmingChn.append(pModule->indexChannel);
        }
        pModule->bneedcontrastive = false;
        QLOG_WARN() << "空通道AD未对齐 通道" << pModule->indexChannel
                    << "ADC" << pModule->channdelData
                    << "目标" << pModule->ModuleDimmingVal
                    << (pModule->channdelData <= 0
                        ? "（读数为0，模组无光信号，不按遗留试杯处理）"
                        : "（有测试杯挡光会明显低于3300，不会改LED去硬拉）");
    }

    emit requestDimmingProgress(false);
    emit showDimmingFailedChn(onceDimmingChn);
}

void controldimming::ProactiveComparisons(int differenceThreshold)
{
    QSet<quint8> failedChannels;
    for (ModuleChannelData *pmoduleinfo : qAsConst(m_testChnDimmingStu)) {
        if (!pmoduleinfo) {
            continue;
        }
        const QString chOpenKey = QString("TestTheChanne1Opening_%1").arg(pmoduleinfo->indexChannel);
        if (!INI_File().rConfigPara(chOpenKey).toBool()) {
            pmoduleinfo->Dimmingsuccessful = true;
            continue;
        }
        if (pmoduleinfo->ModuleDimmingVal <= 0) {
            pmoduleinfo->Dimmingsuccessful = false;
            failedChannels.insert(pmoduleinfo->indexChannel);
            QLOG_WARN() << "通道" << pmoduleinfo->indexChannel
                        << "模组" << pmoduleinfo->ModuleNum << "空通道AD目标为0";
            continue;
        }
        const int delta = std::abs(pmoduleinfo->channdelData - pmoduleinfo->ModuleDimmingVal);
        const bool ok = (delta <= differenceThreshold);
        pmoduleinfo->Dimmingsuccessful = ok;
        QLOG_INFO() << "空通道AD检测 通道" << pmoduleinfo->indexChannel
                    << "ADC" << pmoduleinfo->channdelData
                    << "目标" << pmoduleinfo->ModuleDimmingVal
                    << "偏差" << delta
                    << (ok ? "通过" : "未对齐(可能有杯或光路异常)");
        if (!ok) {
            failedChannels.insert(pmoduleinfo->indexChannel);
        }
    }

    if (failedChannels.isEmpty()) {
        markDimmingFinished(tr("所有空通道AD已对齐3300"));
        return;
    }

    const bool legacyBootClear = INI_File().rConfigPara(INITGRABCUPS).toBool();
    if (!legacyBootClear && m_smartCupClearAttempts < kMaxSmartCupClearRounds) {
        QList<quint8> toThrow;
        for (quint8 ch : failedChannels) {
            const QString wkey = QString("TestTheChanne1Opening_%1").arg(ch);
            if (!INI_File().rConfigPara(wkey).toBool())
                continue;
            int adc = -1;
            for (const ModuleChannelData *p : m_testChnDimmingStu) {
                if (p && p->indexChannel == ch) {
                    adc = p->channdelData;
                    break;
                }
            }
            // 整通道读数 0 不是杯子挡光，丢杯解决不了
            if (adc <= 0)
                continue;
            toThrow.append(ch);
        }
        if (!toThrow.isEmpty()) {
            ++m_smartCupClearAttempts;
            m_startCollectChannelVal = false;
            g_dimmingSettleStartMs = 0;
            for (ModuleChannelData *pModule : m_testChnDimmingStu) {
                if (!pModule || !failedChannels.contains(pModule->indexChannel)) {
                    continue;
                }
                pModule->bneedcontrastive = false;
                pModule->channdelData = 0;
            }
            QStringList chStrs;
            for (quint8 ch : toThrow) {
                chStrs.append(QString::number(ch));
            }
            const QString msg = tr("空通道AD未对齐，正在自动清理可能遗留的测试杯(通道 %1)，第 %2 轮")
                                    .arg(chStrs.join(','))
                                    .arg(m_smartCupClearAttempts);
            QLOG_INFO() << msg;
            emit reminderText(PROMPTLOG, msg);
            m_pendingFailed = failedChannels;
            m_cupClearPending = true;
            const int clearGen = ++m_cupClearGen;
            QTimer::singleShot(120000, this, [this, clearGen]() {
                if (!m_cupClearPending || clearGen != m_cupClearGen
                        || cglobal::g_controldimmingfinished.load()) {
                    return;
                }
                QLOG_WARN() << "自动丢杯超过120秒没有完成，停止等待并提示失败通道";
                m_cupClearPending = false;
                handleFailedChannels(m_pendingFailed);
            });
            emit requestSmartCupClear(toThrow);
            return;
        }
    }

    handleFailedChannels(failedChannels);
}

void controldimming::startEmptyChannelRecheck()
{
    if (cglobal::g_controldimmingfinished.load()) {
        return;
    }
    m_cupClearPending = false;
    g_dimmingSettleStartMs = 0;
    m_startCollectChannelVal = true;
    m_discardNextCompleteFrame = true;
    ++m_frequency;
    emit requestDimmingProgress(true);
    QLOG_INFO() << "空通道AD重检，第" << m_frequency << "轮";
}

void controldimming::theEndDimmingActive()
{
    if (cglobal::g_controldimmingfinished.load()) {
        QLOG_DEBUG() << "调光已结束，忽略再次重检";
        return;
    }
    startEmptyChannelRecheck();
}

void controldimming::cupClearNotSent()
{
    m_cupClearPending = false;
    if (cglobal::g_controldimmingfinished.load()) {
        return;
    }
    QLOG_WARN() << "调光丢杯没有发出，直接提示失败通道";
    handleFailedChannels(m_pendingFailed);
}

void controldimming::SecondDimmingModule(quint8 notifyModule, int times_)
{
    Q_UNUSED(notifyModule);
    Q_UNUSED(times_);
    QLOG_DEBUG() << "初始化调光不再根据LED保存应答重检";
}

void controldimming::_obtainChndata(const quint8 indexChannel, const int indexChannelData)
{
    if (!m_startCollectChannelVal || cglobal::g_controldimmingfinished.load()) {
        return;
    }

    Collectchannelvalues(m_testChnDimmingStu, indexChannel + 1, indexChannelData);

    bool allReady = !m_testChnDimmingStu.isEmpty();
    for (ModuleChannelData *pChnVal : m_testChnDimmingStu) {
        if (!pChnVal || !pChnVal->bneedcontrastive) {
            allReady = false;
            break;
        }
    }

    if (allReady) {
        if (m_discardNextCompleteFrame) {
            m_discardNextCompleteFrame = false;
            for (ModuleChannelData *pChnVal : m_testChnDimmingStu) {
                if (pChnVal) {
                    pChnVal->bneedcontrastive = false;
                    pChnVal->channdelData = 0;
                }
            }
            g_dimmingSettleStartMs = 0;
            QLOG_INFO() << "丢弃未稳定的第一组ADC，等待下一组齐套后再稳定采集";
            return;
        }
        if (g_dimmingSettleStartMs == 0) {
            g_dimmingSettleStartMs = QDateTime::currentMSecsSinceEpoch();
            QLOG_INFO() << "通道ADC已齐套，稳定采集" << kSettleAfterReadyMs << "ms后再对比";
            return;
        }
        const qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - g_dimmingSettleStartMs;
        if (elapsed < kSettleAfterReadyMs) {
            return; // 继续用最新 ADC，对齐设置页上看到的 3300
        }
        g_dimmingSettleStartMs = 0;
        m_startCollectChannelVal = false;
        ProactiveComparisons(kDimmingAbsThreshold);
    }
}

quint8 controldimming::_chnParentModule(const quint8 index_chn)
{
    return static_cast<quint8>(index_chn / 4 + 1);
}
