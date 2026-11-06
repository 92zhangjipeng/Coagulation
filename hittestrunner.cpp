#include "hittestrunner.h"

#include "cglobal.h"
#include "custom_struct.h"
#include "customcreatsql.h"
#include "globaldata.h"
#include "ini_file.h"
#include "loadequipmentpos.h"
#include "mainwindow.h"
#include "monitor_traytest.h"
#include "operclass/fullyautomatedplatelets.h"
#include "quiutils.h"
#include "testing.h"
#include "Custommovements/performanceverification.h"
#include "QsLog/include/QsLog.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDialog>
#include <QDir>
#include <QElapsedTimer>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTimer>
#include <QVBoxLayout>
#include <QtMath>

namespace {

QSqlDatabase hitDb()
{
    return QSqlDatabase::database(QStringLiteral("hit-data"), false);
}

}

HitTestRunner::HitTestRunner(QObject *parent)
    : QObject(parent)
    , m_timer(new QTimer(this))
    , m_service(new QTimer(this))
    , m_clock(new QElapsedTimer)
{
    connect(m_timer, &QTimer::timeout, this, &HitTestRunner::onTimer);
    connect(m_service, &QTimer::timeout, this, &HitTestRunner::serviceLanes);
}

HitTestRunner::~HitTestRunner()
{
    cglobal::g_hitTesting = false;
    delete m_clock;
    m_clock = nullptr;
}

bool HitTestRunner::running() const
{
    return m_running;
}

bool HitTestRunner::paused() const
{
    return m_paused;
}

void HitTestRunner::setPaused(bool paused)
{
    if (m_paused == paused)
        return;
    m_paused = paused;
    if (paused || !m_running)
        return;
    if (m_holdAdvance) {
        m_holdAdvance = false;
        onBatchDone();
        return;
    }
    if (m_holdPos >= 0) {
        const int pos = m_holdPos;
        m_holdPos = -1;
        sendAt(pos);
        return;
    }
    pump();
}

QString HitTestRunner::u8(const char *text) const
{
    return QString::fromUtf8(text);
}

QString HitTestRunner::nowText() const
{
    return QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
}

int HitTestRunner::channel1() const
{
    if (m_sampleIndex < 0 || m_sampleIndex >= m_jobs.size())
        return 0;
    return m_jobs.at(m_sampleIndex).channel;
}

const HitMixSpec *HitTestRunner::currentMix() const
{
    if (m_mixIndex < 0 || m_mixIndex >= m_mix.size())
        return nullptr;
    return &m_mix.at(m_mixIndex);
}

void HitTestRunner::ensureWired()
{
    if (m_wired)
        return;
    SuoweiSerialPort *serial = FullyAutomatedPlatelets::pinstanceserialusb();
    if (!serial)
        return;
    connect(this, &HitTestRunner::sendOrderList,
            serial, &SuoweiSerialPort::slotwrite_instructions_group,
            Qt::QueuedConnection);
    connect(serial, &SuoweiSerialPort::serialHitData,
            this, &HitTestRunner::onSerial,
            Qt::QueuedConnection);
    m_wired = true;
}

void HitTestRunner::loadSettings()
{
    HitMixSpec spontaneous;
    spontaneous.key = "spontaneous";
    spontaneous.statusUtf8 = "\xe8\x87\xaa\xe5\x8f\x91\xe8\x81\x9a\xe9\x9b\x86";
    spontaneous.reagentHole = 0;
    spontaneous.reagentType = AA_REAGENT;
    spontaneous.serumUl = 35;
    spontaneous.prpUl = 280;
    spontaneous.reagentUl = 35;
    spontaneous.incubateSec = 1200;
    spontaneous.recordMin = 30;

    HitMixSpec low = spontaneous;
    low.key = "low";
    low.statusUtf8 = "\xe4\xbd\x8e\xe6\xb5\x93\xe5\xba\xa6";
    low.reagentHole = 1;
    low.reagentType = ADP_REAGENT;

    HitMixSpec high = spontaneous;
    high.key = "high";
    high.statusUtf8 = "\xe9\xab\x98\xe6\xb5\x93\xe5\xba\xa6";
    high.reagentHole = 2;
    high.reagentType = EPI_REAGENT;
    high.prpUl = 260;
    high.reagentUl = 55;

    HitMixSpec negative;
    negative.key = "negative";
    negative.statusUtf8 = "\xe9\x98\xb4\xe6\x80\xa7\xe5\xaf\xb9\xe7\x85\xa7";
    negative.reagentHole = 1;
    negative.reagentType = ADP_REAGENT;
    negative.serumUl = 0;
    negative.prpUl = 315;
    negative.reagentUl = 35;
    negative.incubateSec = 1200;
    negative.recordMin = 30;

    QSettings ini(QCoreApplication::applicationDirPath() + QStringLiteral("/HitConfig.ini"),
                  QSettings::IniFormat);
    ini.setIniCodec("UTF-8");
    ini.beginGroup(QStringLiteral("Ppp"));
    m_pppUl = ini.value(QStringLiteral("RefUl"), 150).toInt();
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Spontaneous"));
    spontaneous.serumUl = ini.value(QStringLiteral("SerumUl"), spontaneous.serumUl).toInt();
    spontaneous.prpUl = ini.value(QStringLiteral("PrpUl"), spontaneous.prpUl).toInt();
    spontaneous.reagentUl = ini.value(QStringLiteral("BufferUl"), spontaneous.reagentUl).toInt();
    spontaneous.incubateSec = ini.value(QStringLiteral("IncubateSec"), spontaneous.incubateSec).toInt();
    spontaneous.recordMin = ini.value(QStringLiteral("RecordMin"), spontaneous.recordMin).toInt();
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Low"));
    low.serumUl = ini.value(QStringLiteral("SerumUl"), low.serumUl).toInt();
    low.prpUl = ini.value(QStringLiteral("PrpUl"), low.prpUl).toInt();
    low.reagentUl = ini.value(QStringLiteral("HeparinUl"), low.reagentUl).toInt();
    low.incubateSec = ini.value(QStringLiteral("IncubateSec"), low.incubateSec).toInt();
    low.recordMin = ini.value(QStringLiteral("RecordMin"), low.recordMin).toInt();
    ini.endGroup();
    ini.beginGroup(QStringLiteral("High"));
    high.serumUl = ini.value(QStringLiteral("SerumUl"), high.serumUl).toInt();
    high.prpUl = ini.value(QStringLiteral("PrpUl"), high.prpUl).toInt();
    high.reagentUl = ini.value(QStringLiteral("HeparinUl"), high.reagentUl).toInt();
    high.incubateSec = ini.value(QStringLiteral("IncubateSec"), high.incubateSec).toInt();
    high.recordMin = ini.value(QStringLiteral("RecordMin"), high.recordMin).toInt();
    ini.endGroup();
    ini.beginGroup(QStringLiteral("Negative"));
    negative.prpUl = ini.value(QStringLiteral("PrpUl"), negative.prpUl).toInt();
    negative.reagentUl = ini.value(QStringLiteral("HeparinUl"), negative.reagentUl).toInt();
    negative.incubateSec = ini.value(QStringLiteral("IncubateSec"), negative.incubateSec).toInt();
    negative.recordMin = ini.value(QStringLiteral("RecordMin"), negative.recordMin).toInt();
    ini.endGroup();

    m_mix.clear();
    m_mix << spontaneous << low << high << negative;
}

bool HitTestRunner::start(const QVector<HitSampleJob> &jobs, QString *reason)
{
    if (m_running) {
        if (reason)
            *reason = u8("HIT\xe6\xad\xa3\xe5\x9c\xa8\xe6\xb5\x8b\xe8\xaf\x95");
        return false;
    }
    if (jobs.isEmpty()) {
        if (reason)
            *reason = u8("\xe6\xb2\xa1\xe6\x9c\x89\xe5\xbe\x85\xe6\xb5\x8b\xe6\xa0\xb7\xe6\x9c\xac");
        return false;
    }
    if (cglobal::g_StartTesting.load() || cglobal::g_quality_start.load()
            || (Performanceverification::instance()
                && Performanceverification::instance()->returnPerformanceTestFlag())) {
        if (reason)
            *reason = u8("\xe4\xbb\xaa\xe5\x99\xa8\xe6\xad\xa3\xe5\x9c\xa8\xe6\xb5\x8b\xe8\xaf\x95");
        return false;
    }
    if (!cglobal::gserialConnecStatus.load() || !Testing::getTaskDll()) {
        if (reason)
            *reason = u8(cglobal::gserialConnecStatus.load()
                         ? "\xe8\xbf\x90\xe5\x8a\xa8\xe5\xba\x93\xe6\x9c\xaa\xe5\x8a\xa0\xe8\xbd\xbd"
                         : "\xe4\xb8\xb2\xe5\x8f\xa3\xe6\x9c\xaa\xe8\xbf\x9e\xe6\x8e\xa5");
        return false;
    }
    ensureWired();
    if (!m_wired) {
        if (reason)
            *reason = u8("\xe4\xb8\xb2\xe5\x8f\xa3\xe6\x9c\xaa\xe8\xbf\x9e\xe6\x8e\xa5");
        return false;
    }

    loadSettings();
    m_jobs = jobs;
    m_lanes.clear();
    m_lanes.reserve(jobs.size());
    for (int i = 0; i < jobs.size(); ++i) {
        HitLane lane;
        lane.jobIndex = i;
        lane.stage = StageTakePpp;
        lane.needsMotion = true;
        m_lanes.append(lane);
    }
    QString cupReason;
    if (!planCups(&cupReason)) {
        m_lanes.clear();
        m_jobs.clear();
        if (reason)
            *reason = cupReason;
        return false;
    }
    m_motionLane = -1;
    m_sampleIndex = -1;
    m_stage = StageIdle;
    m_phase = PhaseIdle;
    m_failed = false;
    m_stopped = false;
    m_promptingGrab = false;
    m_paused = false;
    m_holdAdvance = false;
    m_holdPos = -1;
    m_failText.clear();
    m_running = true;
    cglobal::g_hitTesting = true;
    m_clock->start();
    m_service->start(200);
    QLOG_INFO() << "HIT start" << m_jobs.size();
    pump();
    return true;
}

void HitTestRunner::beginSample()
{
    m_mixIndex = 0;
    m_cupInChannel = false;
    m_ppp = 0;
    m_prp0 = 0;
    m_prpn = 0;
    m_runId = -1;
    m_readRetry = 0;
    m_recordSeq = 0;
    m_recordMs = 0;
    m_pppSeq = 0;
    m_startText = nowText();
    publish(u8("\xe6\xb5\x8b\xe8\xaf\x95\xe4\xb8\xad"), m_startText);
    insertRun();
    if (m_motionLane >= 0 && m_motionLane < m_lanes.size()) {
        HitLane &lane = m_lanes[m_motionLane];
        lane.started = true;
        lane.runId = m_runId;
        lane.startText = m_startText;
        lane.mixIndex = 0;
        lane.needsMotion = false;
    }
    enter(StageTakePpp);
}

void HitTestRunner::captureMotionLane()
{
    if (m_motionLane < 0 || m_motionLane >= m_lanes.size())
        return;
    HitLane &lane = m_lanes[m_motionLane];
    lane.mixIndex = m_mixIndex;
    lane.cupHole = m_cupHole;
    for (int i = 0; i < HitCupSlots; ++i) {
        lane.cups[i] = m_cups[i];
        lane.cupFilled[i] = m_cupFilled[i];
    }
    lane.cupClaimed = m_cupClaimed;
    lane.cupInChannel = m_cupInChannel;
    lane.ppp = m_ppp;
    lane.prp0 = m_prp0;
    lane.prpn = m_prpn;
    lane.runId = m_runId;
    lane.startText = m_startText;
    lane.readRetry = m_readRetry;
    lane.recordSeq = m_recordSeq;
    lane.recordMs = m_recordMs;
    lane.pppSeq = m_pppSeq;
}

void HitTestRunner::restoreMotionLane(int index)
{
    m_motionLane = index;
    HitLane &lane = m_lanes[index];
    m_sampleIndex = lane.jobIndex;
    m_mixIndex = lane.mixIndex;
    m_cupHole = lane.cupHole;
    for (int i = 0; i < HitCupSlots; ++i) {
        m_cups[i] = lane.cups[i];
        m_cupFilled[i] = lane.cupFilled[i];
    }
    m_cupClaimed = lane.cupClaimed;
    m_cupInChannel = lane.cupInChannel;
    m_ppp = lane.ppp;
    m_prp0 = lane.prp0;
    m_prpn = lane.prpn;
    m_runId = lane.runId;
    m_startText = lane.startText;
    m_readRetry = lane.readRetry;
    m_recordSeq = lane.recordSeq;
    m_recordMs = lane.recordMs;
    m_pppSeq = lane.pppSeq;
}

void HitTestRunner::releaseForWait(int waitKind, int waitMs)
{
    if (m_motionLane < 0 || m_motionLane >= m_lanes.size())
        return;
    captureMotionLane();
    HitLane &lane = m_lanes[m_motionLane];
    lane.needsMotion = false;
    lane.waitKind = waitKind;
    lane.stage = (waitKind == TimerIncubate) ? StageSuckReagent : StageThrowMix;
    if (waitKind == TimerIncubate) {
        const bool keep = lane.stepTotalMs > 0 && lane.deadlineMs > m_clock->elapsed();
        if (!keep) {
            lane.stepOriginMs = m_clock->elapsed();
            lane.stepTotalMs = waitMs;
            lane.deadlineMs = lane.stepOriginMs + waitMs;
        }
        lane.readRetry = 0;
        lane.lastProgressSec = static_cast<int>((m_clock->elapsed() - lane.stepOriginMs) / 1000);
    } else if (waitKind == TimerStable) {
        lane.deadlineMs = m_clock->elapsed() + waitMs;
        lane.readRetry = 0;
    } else {
        lane.recordOriginMs = m_clock->elapsed();
        lane.recordSeq = m_recordSeq;
        lane.recordMs = m_recordMs;
        lane.nextRecordMs = lane.recordOriginMs + waitMs;
        lane.stepOriginMs = lane.recordOriginMs;
        lane.stepTotalMs = qMax(lane.recordMs, 1);
        lane.lastProgressSec = 0;
    }
    if (waitKind == TimerIncubate || waitKind == TimerRecord)
        emitStepProgress(lane);
    const int channel = channel1();
    QLOG_INFO() << "HIT yield channel" << channel << "wait" << waitKind;
    m_motionLane = -1;
    m_phase = PhaseIdle;
    m_stage = StageIdle;
    m_timer->stop();
    pump();
}

bool HitTestRunner::channelBusy(int channel, int exceptLane) const
{
    for (int i = 0; i < m_lanes.size(); ++i) {
        if (i == exceptLane)
            continue;
        const HitLane &lane = m_lanes.at(i);
        if (!lane.started || lane.finished || lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
            continue;
        if (m_jobs.at(lane.jobIndex).channel == channel)
            return true;
    }
    return false;
}

bool HitTestRunner::reagentDue(int exceptLane) const
{
    if (!m_clock || !m_clock->isValid())
        return false;
    const qint64 now = m_clock->elapsed();
    for (int i = 0; i < m_lanes.size(); ++i) {
        if (i == exceptLane)
            continue;
        const HitLane &lane = m_lanes.at(i);
        if (lane.finished)
            continue;
        if (lane.needsMotion && lane.stage == StageSuckReagent)
            return true;
        if (lane.waitKind == TimerIncubate && now >= lane.deadlineMs)
            return true;
    }
    return false;
}

bool HitTestRunner::yieldIfReagentDue(int nextStage)
{
    if (m_motionLane < 0 || m_motionLane >= m_lanes.size() || !reagentDue(m_motionLane))
        return false;
    m_stage = nextStage;
    captureMotionLane();
    HitLane &lane = m_lanes[m_motionLane];
    lane.needsMotion = true;
    lane.stage = nextStage;
    if (lane.waitKind != TimerIncubate)
        lane.waitKind = TimerNone;
    QLOG_INFO() << "HIT yield for reagent channel" << channel1() << "resume" << nextStage;
    m_motionLane = -1;
    m_phase = PhaseIdle;
    m_stage = StageIdle;
    pump();
    return true;
}

int HitTestRunner::pickLane() const
{
    for (int i = 0; i < m_lanes.size(); ++i) {
        const HitLane &lane = m_lanes.at(i);
        if (!lane.finished && lane.needsMotion && lane.stage == StageSuckReagent)
            return i;
    }
    for (int i = 0; i < m_lanes.size(); ++i) {
        const HitLane &lane = m_lanes.at(i);
        if (lane.started || lane.finished || lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
            continue;
        if (!channelBusy(m_jobs.at(lane.jobIndex).channel, i))
            return i;
    }
    for (int i = 0; i < m_lanes.size(); ++i) {
        const HitLane &lane = m_lanes.at(i);
        if (!lane.finished && lane.needsMotion)
            return i;
    }
    return -1;
}

bool HitTestRunner::lanesWaiting() const
{
    for (int i = 0; i < m_lanes.size(); ++i) {
        if (!m_lanes.at(i).finished)
            return true;
    }
    return false;
}

void HitTestRunner::pump()
{
    if (!m_running || m_failed || m_stopped || m_paused || m_promptingGrab)
        return;
    if (m_motionLane >= 0)
        return;
    if (m_phase == PhaseAck || m_phase == PhaseWait)
        return;
    if (m_stage == StageHome || m_stage == StageAbortHome || m_stage == StageAbortThrow)
        return;
    const int next = pickLane();
    if (next < 0) {
        if (lanesWaiting())
            return;
        m_sampleIndex = -1;
        enter(StageHome);
        return;
    }
    restoreMotionLane(next);
    m_lanes[next].needsMotion = false;
    if (!m_lanes[next].started) {
        const int channel = channel1();
        QLOG_INFO() << "HIT dispatch new channel" << channel;
        beginSample();
        return;
    }
    QLOG_INFO() << "HIT dispatch continue channel" << channel1() << "stage" << m_lanes[next].stage;
    enter(m_lanes[next].stage);
}

void HitTestRunner::emitStepProgress(const HitLane &lane)
{
    if (!m_clock || lane.stepTotalMs <= 0 || lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
        return;
    int elapsed = static_cast<int>(m_clock->elapsed() - lane.stepOriginMs);
    if (elapsed < 0)
        elapsed = 0;
    if (elapsed > lane.stepTotalMs)
        elapsed = lane.stepTotalMs;
    const int phase = lane.waitKind == TimerRecord ? 1 : 0;
    emit stepProgress(m_jobs.at(lane.jobIndex).row, lane.mixIndex, elapsed, lane.stepTotalMs, phase);
}

void HitTestRunner::serviceLanes()
{
    if (!m_running || m_failed || m_stopped || m_promptingGrab || !m_clock || !m_clock->isValid())
        return;
    const qint64 now = m_clock->elapsed();
    for (int i = 0; i < m_lanes.size(); ++i) {
        if (!m_running || m_failed)
            return;
        HitLane &lane = m_lanes[i];
        if (lane.finished)
            continue;
        if (lane.waitKind == TimerIncubate && now >= lane.deadlineMs)
            serviceIncubate(lane);
        else if (lane.waitKind == TimerStable && now >= lane.deadlineMs)
            serviceStable(lane);
        else if (lane.waitKind == TimerRecord && now >= lane.nextRecordMs)
            serviceRecord(lane);
        if (!lane.finished
                && (lane.waitKind == TimerIncubate || lane.waitKind == TimerStable
                    || lane.waitKind == TimerRecord)
                && lane.stepTotalMs > 0) {
            const int sec = static_cast<int>((now - lane.stepOriginMs) / 1000);
            if (sec != lane.lastProgressSec) {
                lane.lastProgressSec = sec;
                emitStepProgress(lane);
            }
        }
    }
    if (m_running && !m_failed)
        pump();
}

void HitTestRunner::serviceIncubate(HitLane &lane)
{
    if (lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
        return;
    const HitMixSpec *mix = (lane.mixIndex >= 0 && lane.mixIndex < m_mix.size())
            ? &m_mix.at(lane.mixIndex) : nullptr;
    if (!mix) {
        lane.waitKind = TimerNone;
        lane.needsMotion = true;
        lane.stage = StageThrowMix;
        return;
    }
    emitStepProgress(lane);
    if (lane.needsMotion && lane.stage == StageWashSerum) {
        lane.waitKind = TimerNone;
        return;
    }
    if (mix->reagentUl <= 0) {
        parkStable(lane);
        return;
    }
    lane.waitKind = TimerNone;
    lane.needsMotion = true;
    lane.stage = StageSuckReagent;
}

void HitTestRunner::serviceStable(HitLane &lane)
{
    if (lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
        return;
    const HitMixSpec *mix = (lane.mixIndex >= 0 && lane.mixIndex < m_mix.size())
            ? &m_mix.at(lane.mixIndex) : nullptr;
    if (!mix) {
        lane.waitKind = TimerNone;
        lane.needsMotion = true;
        lane.stage = StageThrowMix;
        return;
    }
    int value = 0;
    const bool stable = readStableOf(lane, value);
    if (!stable && lane.readRetry < 15) {
        ++lane.readRetry;
        lane.deadlineMs = m_clock->elapsed() + 1000;
        return;
    }
    if (!stable)
        value = readChannelOf(m_jobs.at(lane.jobIndex).channel);
    lane.prp0 = value;
    lane.prpn = value;
    lane.readRetry = 0;
    const QString step = QString::fromLatin1(mix->key);
    savePointOf(lane, step, 0, 0, value, 0);
    saveStepOf(lane, step, lane.prp0, 0, 0);
    QLOG_INFO() << "HIT PRP0" << mix->key << value
                << "channel" << m_jobs.at(lane.jobIndex).channel;
    int recordMin = mix->recordMin;
    if (recordMin > 180)
        recordMin = 180;
    lane.recordMs = recordMin * 60 * 1000;
    lane.recordSeq = 0;
    lane.peakPercent = 0;
    if (lane.recordMs <= 0) {
        finishParkedRecord(lane);
        return;
    }
    lane.recordOriginMs = m_clock->elapsed();
    lane.nextRecordMs = lane.recordOriginMs + 1000;
    lane.waitKind = TimerRecord;
    lane.stage = StageThrowMix;
    lane.stepOriginMs = lane.recordOriginMs;
    lane.stepTotalMs = qMax(lane.recordMs, 1);
    lane.lastProgressSec = 0;
    emitStepProgress(lane);
}

void HitTestRunner::serviceRecord(HitLane &lane)
{
    if (lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
        return;
    const HitMixSpec *mix = (lane.mixIndex >= 0 && lane.mixIndex < m_mix.size())
            ? &m_mix.at(lane.mixIndex) : nullptr;
    if (!mix) {
        lane.waitKind = TimerNone;
        lane.needsMotion = true;
        lane.stage = StageThrowMix;
        return;
    }
    const int value = readChannelOf(m_jobs.at(lane.jobIndex).channel);
    if (value > 0 || lane.prpn <= 0)
        lane.prpn = value;
    ++lane.recordSeq;
    const int elapsed = static_cast<int>(m_clock->elapsed() - lane.recordOriginMs);
    const double pct = percentOfLane(lane, lane.prpn, lane.prp0);
    if (lane.recordSeq == 1)
        lane.peakPercent = pct;
    else
        lane.peakPercent = qMax(lane.peakPercent, pct);
    savePointOf(lane, QString::fromLatin1(mix->key), lane.recordSeq, elapsed, value, pct);
    if (elapsed >= lane.recordMs) {
        finishParkedRecord(lane);
        return;
    }
    lane.nextRecordMs = m_clock->elapsed() + 1000;
}

void HitTestRunner::finishParkedRecord(HitLane &lane)
{
    lane.waitKind = TimerNone;
    const HitMixSpec *mix = (lane.mixIndex >= 0 && lane.mixIndex < m_mix.size())
            ? &m_mix.at(lane.mixIndex) : nullptr;
    if (!mix) {
        lane.needsMotion = true;
        lane.stage = StageThrowMix;
        return;
    }
    const double pct = lane.peakPercent;
    saveStepOf(lane, QString::fromLatin1(mix->key), lane.prp0, lane.prpn, pct);
    const QString text = QString::number(pct, 'f', 1);
    QString spontaneous;
    QString low;
    QString high;
    QString negative;
    if (lane.mixIndex == 0)
        spontaneous = text;
    else if (lane.mixIndex == 1)
        low = text;
    else if (lane.mixIndex == 2)
        high = text;
    else
        negative = text;
    publishJob(lane.jobIndex, u8(mix->statusUtf8), QString(), QString(), QString(),
               spontaneous, low, high, negative);
    QLOG_INFO() << "HIT result" << mix->key << "channel" << m_jobs.at(lane.jobIndex).channel
                << "percent" << pct;
    lane.needsMotion = true;
    lane.stage = StageThrowMix;
}

void HitTestRunner::savePointOf(const HitLane &lane, const QString &step, int seq, int elapsedMs,
                                double raw, double percent)
{
    const int oldIndex = m_sampleIndex;
    const qint64 oldRun = m_runId;
    m_sampleIndex = lane.jobIndex;
    m_runId = lane.runId;
    savePoint(step, seq, elapsedMs, raw, percent);
    m_sampleIndex = oldIndex;
    m_runId = oldRun;
}

void HitTestRunner::saveStepOf(const HitLane &lane, const QString &step, double prp0, double prpn,
                               double percent)
{
    const qint64 oldRun = m_runId;
    m_runId = lane.runId;
    saveStep(step, prp0, prpn, percent);
    m_runId = oldRun;
}

double HitTestRunner::percentOfLane(const HitLane &lane, double prpn, double prp0) const
{
    if (lane.ppp <= 0 || prp0 <= 0 || prpn <= 0)
        return 0;
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    if (!mw)
        return 0;
    return mw->hitAggregationPercent(static_cast<float>(prpn),
                                     static_cast<float>(prp0),
                                     static_cast<float>(lane.ppp));
}

void HitTestRunner::stopAllStir()
{
    Monitor_TrayTest *monitor = FullyAutomatedPlatelets::pinstanceobtainModuledata();
    if (!monitor)
        return;
    for (int i = 0; i < m_lanes.size(); ++i) {
        const HitLane &lane = m_lanes.at(i);
        if (!lane.started || lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
            continue;
        const int ch0 = m_jobs.at(lane.jobIndex).channel - 1;
        if (ch0 < 0 || ch0 >= 12)
            continue;
        monitor->controlChnMotorRotating(static_cast<quint8>(ch0), false);
    }
}

void HitTestRunner::enter(int stage)
{
    if (!m_running)
        return;
    m_timer->stop();
    m_stage = stage;
    QLOG_INFO() << "HIT stage" << stage << "sample" << m_sampleIndex << "mix" << m_mixIndex;

    switch (stage) {
    case StageTakePpp: {
        if (m_pppUl <= 0) {
            fail(u8("\xe5\x90\xb8\xe6\xa0\xb7\xe9\x87\x8f\xe6\x97\xa0\xe6\x95\x88"));
            return;
        }
        if (m_cups[0] < 0) {
            fail(u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf"));
            return;
        }
        m_cupHole = m_cups[0];
        m_cupClaimed = true;
        publish(u8("\xe5\x90\xb8PPP\xe5\x8f\x82\xe7\x85\xa7"));
        enter(StageSuckPpp);
        break;
    }
    case StageSuckPpp: {
        QPoint suck;
        QPoint spit;
        QString err;
        if (!bloodPoint(m_jobs.at(m_sampleIndex).pppHole, suck, &err)
                || !trayPoint(m_cupHole, MOTOR_BLOOD_INDEX, spit, &err)
                || !buildBloodTransfer(suck, spit, m_pppUl, &err)) {
            fail(err.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : err);
            return;
        }
        sendAt(0);
        break;
    }
    case StageGripPpp:
    case StageGripMix: {
        QString err;
        if (!buildGrip(&err)) {
            fail(err.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : err);
            return;
        }
        sendAt(0);
        break;
    }
    case StageThrowPpp:
    case StageThrowMix:
    case StageAbortThrow: {
        QString err;
        if (!buildThrow(&err)) {
            m_cupInChannel = false;
            if (stage == StageAbortThrow)
                startAbortHome();
            else
                fail(err.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : err);
            return;
        }
        sendAt(0);
        break;
    }
    case StageWashPpp:
    case StageWashSerum:
    case StageWashPrp: {
        if (!GlobalData::cleaningFluidCoversNextUse(1)) {
            fail(u8("\xe6\xb8\x85\xe6\xb4\x97\xe6\xb6\xb2S1\xe4\xb8\x8d\xe5\xa4\x9f\xe5\xae\x8c\xe6\x88\x90\xe6\x9c\xac\xe6\xac\xa1\xe6\xb8\x85\xe6\xb4\x97\xef\xbc\x8c\xe8\xaf\xb7\xe6\x9b\xb4\xe6\x8d\xa2\xe5\x90\x8e\xe5\x86\x8d\xe7\xbb\xa7\xe7\xbb\xad"));
            return;
        }
        if (!buildWashBlood()) {
            fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
            return;
        }
        sendAt(0);
        break;
    }
    case StageWashReagent: {
        if (!GlobalData::cleaningFluidCoversNextUse(0)) {
            fail(u8("\xe6\xb8\x85\xe6\xb4\x97\xe6\xb6\xb2S1\xe4\xb8\x8d\xe5\xa4\x9f\xe5\xae\x8c\xe6\x88\x90\xe6\x9c\xac\xe6\xac\xa1\xe6\xb8\x85\xe6\xb4\x97\xef\xbc\x8c\xe8\xaf\xb7\xe6\x9b\xb4\xe6\x8d\xa2\xe5\x90\x8e\xe5\x86\x8d\xe7\xbb\xa7\xe7\xbb\xad"));
            return;
        }
        if (!buildWashReagent()) {
            fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
            return;
        }
        sendAt(0);
        break;
    }
    case StageTakeMix: {
        if (m_mixIndex >= m_mix.size()) {
            publish(u8("\xe5\xae\x8c\xe6\x88\x90"), QString(), nowText());
            closeRun(u8("\xe5\xae\x8c\xe6\x88\x90"));
            setStir(false);
            if (m_motionLane >= 0 && m_motionLane < m_lanes.size()) {
                captureMotionLane();
                HitLane &lane = m_lanes[m_motionLane];
                lane.finished = true;
                lane.needsMotion = false;
                lane.waitKind = TimerNone;
            }
            m_motionLane = -1;
            m_phase = PhaseIdle;
            m_stage = StageIdle;
            pump();
            return;
        }
        const HitMixSpec *mix = currentMix();
        if (!mix || (mix->serumUl <= 0 && mix->prpUl <= 0 && mix->reagentUl <= 0)) {
            fail(u8("\xe5\x90\xb8\xe6\xa0\xb7\xe9\x87\x8f\xe6\x97\xa0\xe6\x95\x88"));
            return;
        }
        if (m_mixIndex + 1 >= HitCupSlots || m_cups[m_mixIndex + 1] < 0) {
            fail(u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf"));
            return;
        }
        m_cupHole = m_cups[m_mixIndex + 1];
        publish(u8(mix->statusUtf8));
        if (mix->serumUl > 0)
            enter(StageSuckSerum);
        else
            enter(StageGripMix);
        break;
    }
    case StagePrepPrp: {
        if (m_mixIndex >= m_mix.size()) {
            enter(StageWashPpp);
            break;
        }
        const HitMixSpec *mix = currentMix();
        if (!mix || mix->prpUl <= 0) {
            fail(u8("\xe5\x90\xb8\xe6\xa0\xb7\xe9\x87\x8f\xe6\x97\xa0\xe6\x95\x88"));
            return;
        }
        if (m_mixIndex + 1 >= HitCupSlots || m_cups[m_mixIndex + 1] < 0) {
            fail(u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf"));
            return;
        }
        m_cupHole = m_cups[m_mixIndex + 1];
        const int displayHole = m_jobs.at(m_sampleIndex).pppHole + 2;
        QPoint suck;
        QPoint spit;
        QString err;
        if (!bloodPoint(displayHole, suck, &err)
                || !trayPoint(m_cupHole, MOTOR_BLOOD_INDEX, spit, &err)
                || !buildBloodTransfer(suck, spit, mix->prpUl, &err)) {
            fail(err.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : err);
            return;
        }
        publish(u8(mix->statusUtf8));
        sendAt(0);
        break;
    }
    case StageSuckSerum:
    case StageSuckPrp: {
        const HitMixSpec *mix = currentMix();
        if (!mix) {
            fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
            return;
        }
        const int displayHole = m_jobs.at(m_sampleIndex).pppHole
                + (stage == StageSuckSerum ? 1 : 2);
        const int ul = stage == StageSuckSerum ? mix->serumUl : mix->prpUl;
        QPoint suck;
        QPoint spit;
        QString err;
        if (!bloodPoint(displayHole, suck, &err)
                || !trayPoint(m_cupHole, MOTOR_BLOOD_INDEX, spit, &err)
                || !buildBloodTransfer(suck, spit, ul, &err)) {
            fail(err.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : err);
            return;
        }
        sendAt(0);
        break;
    }
    case StageSuckReagent: {
        const HitMixSpec *mix = currentMix();
        if (!mix) {
            fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
            return;
        }
        if (mix->reagentUl <= 0) {
            beginStableWait();
            break;
        }
        QPoint suck;
        QPoint spit;
        QString err;
        if (!reagentPoint(mix->reagentHole, suck, &err)
                || !channelReagentPoint(spit, &err)
                || !buildReagentTransfer(suck, spit, mix->reagentUl, mix->reagentType, &err)) {
            fail(err.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : err);
            return;
        }
        sendAt(0);
        break;
    }
    case StageRead0:
        readPrp0();
        break;
    case StageHome:
        stopAllStir();
        if (!buildHome()) {
            cleanup(m_failed ? m_failText : QString());
            return;
        }
        sendAt(0);
        break;
    default:
        break;
    }
}

void HitTestRunner::beginBatch()
{
    m_batch.clear();
    m_pos = -1;
    m_deepen = 0;
    m_phase = PhaseIdle;
}

int HitTestRunner::findCommand(quint8 index) const
{
    for (int i = 0; i < m_batch.size(); ++i) {
        const QByteArray &cmd = m_batch.at(i).cmd;
        if (cmd.size() > GET_COMMAND_INDEX
                && static_cast<quint8>(cmd.at(GET_COMMAND_INDEX)) == index) {
            return i;
        }
    }
    return -1;
}

void HitTestRunner::takeCommands(const QByteArrayList &cmds)
{
    for (int i = 0; i < cmds.size(); ++i) {
        if (cmds.at(i).size() <= GET_COMMAND_INDEX)
            continue;
        CmdStep step;
        step.cmd = cmds.at(i);
        m_batch.append(step);
    }
}

void HitTestRunner::sendAt(int pos)
{
    if (!m_running)
        return;
    if (m_paused) {
        m_holdPos = pos;
        m_holdAdvance = false;
        return;
    }
    while (pos < m_batch.size() && m_batch.at(pos).cmd.size() <= GET_COMMAND_INDEX)
        ++pos;
    if (pos >= m_batch.size()) {
        onBatchDone();
        return;
    }
    m_pos = pos;
    const QByteArray &cmd = m_batch.at(pos).cmd;
    m_expectIndex = static_cast<quint8>(cmd.at(GET_COMMAND_INDEX));
    m_phase = PhaseAck;
    QByteArrayList one;
    one.append(cmd);
    emit sendOrderList(one, QStringLiteral("HIT"));
}

void HitTestRunner::onBatchDone()
{
    if (!m_running)
        return;
    if (m_paused) {
        m_holdAdvance = true;
        m_holdPos = -1;
        return;
    }
    if (m_stage == StageAbortThrow) {
        markCupUsed();
        m_cupInChannel = false;
        setStir(false);
        startAbortHome();
        return;
    }
    if (m_stage == StageAbortHome || m_stage == StageHome) {
        cleanup(m_failed ? m_failText : QString());
        return;
    }

    const HitMixSpec *mix = currentMix();
    switch (m_stage) {
    case StageSuckPpp:
        m_cupFilled[0] = true;
        m_mixIndex = 0;
        notifySourceSucked(0);
        if (!yieldIfReagentDue(StagePrepPrp))
            enter(StagePrepPrp);
        break;
    case StagePrepPrp:
        if (m_mixIndex >= 0 && m_mixIndex + 1 < HitCupSlots)
            m_cupFilled[m_mixIndex + 1] = true;
        ++m_mixIndex;
        if (m_mixIndex >= m_mix.size())
            notifySourceSucked(2);
        if (!yieldIfReagentDue(StagePrepPrp))
            enter(StagePrepPrp);
        break;
    case StageGripPpp:
        m_cupInChannel = true;
        setStir(true);
        m_readRetry = 0;
        {
            MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
            m_pppSeq = mw ? mw->getChannelPositiveFilterSeq(static_cast<quint8>(channel1())) : 0;
        }
        armTimer(TimerPppWait, 5000, false);
        break;
    case StageThrowPpp:
        markCupUsed();
        m_cupInChannel = false;
        setStir(false);
        m_mixIndex = 0;
        if (!yieldIfReagentDue(StageTakeMix))
            enter(StageTakeMix);
        break;
    case StageWashPpp:
        deductClean(BLOOD_PIN);
        m_mixIndex = 0;
        if (m_cups[0] < 0) {
            fail(u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf"));
            break;
        }
        m_cupHole = m_cups[0];
        if (!yieldIfReagentDue(StageGripPpp))
            enter(StageGripPpp);
        break;
    case StageSuckSerum:
        if (m_mixIndex >= 0 && m_mixIndex + 1 < HitCupSlots)
            m_cupFilled[m_mixIndex + 1] = true;
        if (!yieldIfReagentDue(StageGripMix))
            enter(StageGripMix);
        break;
    case StageWashSerum:
        deductClean(BLOOD_PIN);
        startIncubate();
        break;
    case StageSuckPrp:
        enter(StageWashPrp);
        break;
    case StageWashPrp:
        deductClean(BLOOD_PIN);
        enter(StageGripMix);
        break;
    case StageGripMix:
        m_cupInChannel = true;
        setStir(true);
        m_readRetry = 0;
        beginIncubateClock();
        if (mix && mix->serumUl > 0) {
            if (!yieldIfReagentDue(StageWashSerum))
                enter(StageWashSerum);
        } else {
            startIncubate();
        }
        break;
    case StageSuckReagent:
        enter(StageWashReagent);
        break;
    case StageWashReagent:
        deductClean(REAGENT_PIN);
        beginStableWait();
        break;
    case StageThrowMix:
        markCupUsed();
        m_cupInChannel = false;
        setStir(false);
        ++m_mixIndex;
        if (!yieldIfReagentDue(StageTakeMix))
            enter(StageTakeMix);
        break;
    default:
        fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
        break;
    }
}

void HitTestRunner::onSerial(const QStringList &recv)
{
    if (!m_running || m_promptingGrab || m_phase != PhaseAck || m_pos < 0 || m_pos >= m_batch.size())
        return;

    int commandMode = 0;
    int runStatus = 0;
    int speedMode = 0;
    int motor = 0;
    GlobalData::Parse4thBytecommandstatus(recv, commandMode, runStatus, speedMode, motor);
    const quint8 index = GlobalData::analyzeCommandIndex(recv);
    if (runStatus == MOTOR_WAITING || runStatus == MOTOR_RUNNING)
        return;
    if (index != m_expectIndex)
        return;

    QString err;
    if (!GlobalData::commandStatus(runStatus, err)) {
        fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8 ") + err);
        return;
    }

    const CmdStep step = m_batch.at(m_pos);
    if (step.grabOpen) {
        m_phase = PhaseWait;
        const int openPos = m_pos;
        const int waitMs = GlobalData::gripperGrabDelayMs();
        QTimer::singleShot(waitMs, this, [this, openPos]() {
            if (!m_running || m_phase != PhaseWait || m_pos != openPos)
                return;
            sendAt(openPos + 1);
        });
        return;
    }
    if (step.grabJudge) {
        bool suction = false;
        quint32 air = 0;
        GlobalData::analyzeWhetherGripperSuction(recv, suction, air);
        if (!GlobalData::isHandsVacuumStillSealed(air)) {
            const bool gripping = m_stage == StageGripPpp || m_stage == StageGripMix;
            if (m_deepen >= 3) {
                if (gripping)
                    requestGrabPrompt();
                else
                    fail(u8("\xe6\x9c\xaa\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1"));
                return;
            }
            ++m_deepen;
            UsbCodeDispose *dll = Testing::getTaskDll();
            if (!dll || m_grabDownMm <= 0) {
                if (gripping)
                    requestGrabPrompt();
                else
                    fail(u8("\xe6\x9c\xaa\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1"));
                return;
            }
            const int down = m_grabDownMm + m_deepen;
            quint8 idx = index;
            const QByteArray deepen = QUIUtils::withGripperPump(
                        dll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, down, 0, idx, false,
                                                        down, true, GRABCUPWITHGRIPPER),
                        GRIPPERMOVE_ABNORMALLY);
            if (deepen.size() <= GET_COMMAND_INDEX) {
                if (gripping)
                    requestGrabPrompt();
                else
                    fail(u8("\xe6\x9c\xaa\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1"));
                return;
            }
            m_expectIndex = static_cast<quint8>(deepen.at(GET_COMMAND_INDEX));
            QByteArrayList one;
            one.append(deepen);
            emit sendOrderList(one, QStringLiteral("HIT"));
            return;
        }
        m_deepen = 0;
    }
    if (step.waitAfterMs > 0) {
        m_phase = PhaseWait;
        const int at = m_pos;
        const int waitMs = step.waitAfterMs;
        QTimer::singleShot(waitMs, this, [this, at]() {
            if (!m_running || m_phase != PhaseWait || m_pos != at)
                return;
            sendAt(at + 1);
        });
        return;
    }
    sendAt(m_pos + 1);
}

void HitTestRunner::onTimer()
{
    if (!m_running || m_failed)
        return;
    if (m_timerMode == TimerPppWait || m_timerMode == TimerPppRetry)
        readPpp();
    else if (m_timerMode == TimerIncubate) {
        m_readRetry = 0;
        enter(StageRead0);
    } else if (m_timerMode == TimerRead0Retry)
        readPrp0();
    else if (m_timerMode == TimerRecord)
        recordTick();
}

void HitTestRunner::armTimer(int mode, int ms, bool repeat)
{
    if (mode == TimerIncubate || mode == TimerRecord) {
        releaseForWait(mode, ms);
        return;
    }
    m_timerMode = mode;
    m_phase = PhaseWait;
    m_timer->stop();
    m_timer->setSingleShot(!repeat);
    m_timer->setInterval(qMax(1, ms));
    m_timer->start();
}

void HitTestRunner::readPpp()
{
    int value = 0;
    const bool stable = readStable(value);
    if (!stable && m_readRetry < 5) {
        ++m_readRetry;
        armTimer(TimerPppRetry, 1000, false);
        return;
    }
    if (!stable)
        value = readChannel();
    m_ppp = value;
    savePpp();
    publish(u8("\xe5\x90\xb8PPP\xe5\x8f\x82\xe7\x85\xa7"), QString(), QString(), QString::number(value));
    QLOG_INFO() << "HIT PPP" << value;
    enter(StageThrowPpp);
}

void HitTestRunner::readPrp0()
{
    const HitMixSpec *mix = currentMix();
    if (!mix) {
        fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
        return;
    }
    const int value = readChannel();
    if (value <= 0 && m_readRetry < 5) {
        ++m_readRetry;
        armTimer(TimerRead0Retry, 1000, false);
        return;
    }
    m_prp0 = value;
    m_prpn = value;
    savePoint(QString::fromLatin1(mix->key), 0, 0, value, 0);
    saveStep(QString::fromLatin1(mix->key), m_prp0, 0, 0);
    QLOG_INFO() << "HIT PRP0" << mix->key << value;
    int recordMin = mix->recordMin;
    if (recordMin > 180)
        recordMin = 180;
    m_recordMs = recordMin * 60 * 1000;
    m_recordSeq = 0;
    m_peakPercent = 0;
    if (m_recordMs <= 0) {
        finishRecord();
        return;
    }
    armTimer(TimerRecord, 1000, true);
}

void HitTestRunner::recordTick()
{
    const HitMixSpec *mix = currentMix();
    if (!mix) {
        fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
        return;
    }
    const int value = readChannel();
    if (value > 0 || m_prpn <= 0)
        m_prpn = value;
    ++m_recordSeq;
    const int elapsed = static_cast<int>(m_clock->elapsed());
    const double pct = percentOf(m_prpn, m_prp0);
    if (m_recordSeq == 1)
        m_peakPercent = pct;
    else
        m_peakPercent = qMax(m_peakPercent, pct);
    savePoint(QString::fromLatin1(mix->key), m_recordSeq, elapsed, value, pct);
    if (elapsed >= m_recordMs) {
        m_timer->stop();
        finishRecord();
    }
}

void HitTestRunner::finishRecord()
{
    m_timer->stop();
    const HitMixSpec *mix = currentMix();
    if (!mix) {
        fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8"));
        return;
    }
    const double pct = m_peakPercent;
    saveStep(QString::fromLatin1(mix->key), m_prp0, m_prpn, pct);
    const QString text = QString::number(pct, 'f', 1);
    QString spontaneous;
    QString low;
    QString high;
    QString negative;
    if (m_mixIndex == 0)
        spontaneous = text;
    else if (m_mixIndex == 1)
        low = text;
    else if (m_mixIndex == 2)
        high = text;
    else
        negative = text;
    publish(u8(mix->statusUtf8), QString(), QString(), QString(), spontaneous, low, high, negative);
    QLOG_INFO() << "HIT result" << mix->key << "PPP" << m_ppp << "PRP0" << m_prp0
                << "PRPn" << m_prpn << "percent" << pct;
    enter(StageThrowMix);
}

double HitTestRunner::percentOf(double prpn, double prp0) const
{
    if (m_ppp <= 0 || prp0 <= 0 || prpn <= 0)
        return 0;
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    if (!mw)
        return 0;
    return mw->hitAggregationPercent(static_cast<float>(prpn),
                                     static_cast<float>(prp0),
                                     static_cast<float>(m_ppp));
}

void HitTestRunner::requestGrabPrompt()
{
    if (!m_running || m_promptingGrab)
        return;
    m_promptingGrab = true;
    m_phase = PhaseIdle;
    QTimer::singleShot(0, this, [this] { promptGrab(); });
}

void HitTestRunner::promptGrab()
{
    if (!m_running || !m_promptingGrab)
        return;
    QWidget *parent = FullyAutomatedPlatelets::mainWindow();
    QDialog dlg(parent);
    dlg.setWindowTitle(u8("\xe6\x8a\x93\xe6\x9d\xaf\xe5\xa4\xb1\xe8\xb4\xa5"));
    dlg.setModal(true);
    dlg.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setAttribute(Qt::WA_StyledBackground, true);
    dlg.setMinimumWidth(460);
    dlg.setStyleSheet(QString::fromUtf8(
        "QDialog { background: #eef4f8; border: 1px solid #c5d4e2; border-radius: 14px; }"));

    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
    mainLayout->setContentsMargins(0, 0, 0, 18);
    mainLayout->setSpacing(0);
    QWidget *accent = new QWidget(&dlg);
    accent->setFixedHeight(6);
    accent->setStyleSheet(QString::fromUtf8(
        "background: #4a7ab5; border: none;"
        " border-top-left-radius: 14px; border-top-right-radius: 14px;"));
    mainLayout->addWidget(accent);

    QVBoxLayout *body = new QVBoxLayout();
    body->setContentsMargins(22, 18, 22, 0);
    body->setSpacing(14);
    mainLayout->addLayout(body);
    QLabel *title = new QLabel(u8("\xe6\x8a\x93\xe6\x9d\xaf\xe5\xa4\xb1\xe8\xb4\xa5"), &dlg);
    title->setStyleSheet(QString::fromUtf8(
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 24px; color: #1a4d6b;"
        " background: transparent; border: none; }"));
    body->addWidget(title);
    QLabel *message = new QLabel(
        u8("\xe8\xb4\x9f\xe5\x8e\x8b\xe6\xb2\xa1\xe5\x88\xb0\xef\xbc\x8c\xe5\xb7\xb2\xe7\xbb\x8f\xe5\x86\x8d\xe5\xbe\x80\xe4\xb8\x8b\xe5\x90\xb8\xe4\xba\x86"
           "3\xe6\xac\xa1\xef\xbc\x8c\xe8\xbf\x98\xe6\x98\xaf\xe6\xb2\xa1\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1\xe3\x80\x82\n"
           "\xe9\x87\x8d\xe8\xaf\x95\xe4\xbc\x9a\xe5\x85\x88\xe6\x8a\xac\xe8\xb5\xb7\xe6\x8a\x93\xe6\x89\x8b\xef\xbc\x8c\xe5\x86\x8d\xe4\xbb\x8e\xe8\xbf\x99\xe5\x8f\xaa\xe6\x9d\xaf\xe5\xad\x90\xe6\x8a\x93\xe4\xb8\x80\xe6\xac\xa1\xe3\x80\x82\n"
           "\xe6\x94\xbe\xe5\xbc\x83\xe5\x90\x8e\xe8\xbf\x99\xe5\x8f\xaa\xe7\xa9\xba\xe8\xaf\x95\xe7\xae\xa1\xe9\x80\x80\xe5\x9b\x9e\xef\xbc\x8c\xe4\xbb\xaa\xe5\x99\xa8\xe5\x9b\x9e\xe5\x8e\x9f\xe7\x82\xb9\xe3\x80\x82"),
        &dlg);
    message->setWordWrap(true);
    message->setMinimumWidth(400);
    message->setStyleSheet(QString::fromUtf8(
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 16px; color: #2c3a48;"
        " background: transparent; border: none; }"));
    body->addWidget(message);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->setSpacing(12);
    buttons->addStretch();
    QPushButton *giveUp = new QPushButton(u8("\xe6\x94\xbe\xe5\xbc\x83"), &dlg);
    QPushButton *again = new QPushButton(u8("\xe9\x87\x8d\xe8\xaf\x95"), &dlg);
    again->setDefault(true);
    giveUp->setCursor(Qt::PointingHandCursor);
    again->setCursor(Qt::PointingHandCursor);
    giveUp->setStyleSheet(QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px;"
        "  min-width: 96px; min-height: 40px; padding: 0 8px;"
        "  border: 1px solid #b7c5d3; border-radius: 8px;"
        "  background: #ffffff; color: #2c3a48;"
        "}"
        "QPushButton:hover { background: #f4f8fb; border-color: #8aa4be; }"));
    again->setStyleSheet(QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px;"
        "  min-width: 96px; min-height: 40px; padding: 0 8px;"
        "  border: 1px solid #4a7ab5; border-radius: 8px;"
        "  background: #4a7ab5; color: white;"
        "}"
        "QPushButton:hover { background: #3d6aa0; }"
        "QPushButton:pressed { background: #325884; }"));
    buttons->addWidget(giveUp);
    buttons->addWidget(again);
    body->addLayout(buttons);
    QObject::connect(again, &QPushButton::clicked, &dlg, &QDialog::accept);
    QObject::connect(giveUp, &QPushButton::clicked, &dlg, &QDialog::reject);
    dlg.adjustSize();

    const bool retry = dlg.exec() == QDialog::Accepted;
    if (!m_running)
        return;
    m_promptingGrab = false;
    if (retry) {
        m_deepen = 0;
        enter(m_stage);
        return;
    }
    fail(u8("\xe6\x9c\xaa\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1"));
}

void HitTestRunner::fail(const QString &why)
{
    m_timer->stop();
    QLOG_WARN() << "HIT fail" << why << "stage" << m_stage;
    if (m_stage == StageAbortHome || m_stage == StageHome) {
        if (m_failText.isEmpty())
            m_failText = why;
        m_failed = true;
        cleanup(m_failText);
        return;
    }
    if (m_stage == StageAbortThrow) {
        m_cupInChannel = false;
        if (m_failText.isEmpty())
            m_failText = why;
        startAbortHome();
        return;
    }
    m_failed = true;
    if (m_failText.isEmpty())
        m_failText = why;
    if (m_sampleIndex >= 0 && m_sampleIndex < m_jobs.size()) {
        publish(u8("\xe5\xa4\xb1\xe8\xb4\xa5"), QString(), nowText());
        closeRun(u8("\xe5\xa4\xb1\xe8\xb4\xa5"));
    }
    if (m_motionLane >= 0 && m_motionLane < m_lanes.size()) {
        captureMotionLane();
        m_lanes[m_motionLane].finished = true;
    }
    for (int i = 0; i < m_lanes.size(); ++i)
        retireLaneCups(m_lanes[i]);
    if (m_motionLane >= 0 && m_motionLane < m_lanes.size()) {
        const HitLane &lane = m_lanes.at(m_motionLane);
        for (int i = 0; i < HitCupSlots; ++i) {
            m_cups[i] = lane.cups[i];
            m_cupFilled[i] = lane.cupFilled[i];
        }
        m_cupClaimed = lane.cupClaimed;
    }
    setStir(false);
    if (m_cupInChannel) {
        m_stage = StageAbortThrow;
        QString err;
        if (!buildThrow(&err)) {
            m_cupInChannel = false;
            startAbortHome();
            return;
        }
        sendAt(0);
        return;
    }
    startAbortHome();
}

void HitTestRunner::startAbortHome()
{
    m_stage = StageAbortHome;
    if (!buildHome()) {
        cleanup(m_failText);
        return;
    }
    sendAt(0);
}

void HitTestRunner::cleanup(const QString &message)
{
    if (m_stopped)
        return;
    m_stopped = true;
    m_running = false;
    m_promptingGrab = false;
    m_paused = false;
    m_holdAdvance = false;
    m_holdPos = -1;
    m_phase = PhaseIdle;
    m_timer->stop();
    if (m_service)
        m_service->stop();
    cglobal::g_hitTesting = false;
    if (m_failed) {
        for (int i = 0; i < m_lanes.size(); ++i) {
            HitLane &lane = m_lanes[i];
            if (!lane.started || lane.finished || lane.runId < 0)
                continue;
            m_sampleIndex = lane.jobIndex;
            m_runId = lane.runId;
            publish(u8("\xe5\xa4\xb1\xe8\xb4\xa5"), QString(), nowText());
            closeRun(u8("\xe5\xa4\xb1\xe8\xb4\xa5"));
            lane.finished = true;
        }
    }
    stopAllStir();
    QString text = message;
    if (text.isEmpty() && !m_dbReady)
        text = u8("\xe6\x95\xb0\xe6\x8d\xae\xe5\xba\x93\xe6\x9c\xaa\xe5\x86\x99\xe5\x85\xa5\xef\xbc\x8c\xe7\xbb\x93\xe6\x9e\x9c\xe7\x95\x99\xe5\x9c\xa8\xe8\xa1\xa8\xe6\xa0\xbc");
    emit runFinished(text);
}

void HitTestRunner::publish(const QString &status, const QString &startTime, const QString &endTime,
                            const QString &ppp, const QString &spontaneous, const QString &low,
                            const QString &high, const QString &negative)
{
    publishJob(m_sampleIndex, status, startTime, endTime, ppp, spontaneous, low, high, negative);
}

void HitTestRunner::publishJob(int jobIndex, const QString &status, const QString &startTime,
                               const QString &endTime, const QString &ppp, const QString &spontaneous,
                               const QString &low, const QString &high, const QString &negative)
{
    if (jobIndex < 0 || jobIndex >= m_jobs.size())
        return;
    emit rowChanged(m_jobs.at(jobIndex).row, status, startTime, endTime,
                    ppp, spontaneous, low, high, negative);
}

void HitTestRunner::beginIncubateClock()
{
    if (!m_clock || !m_clock->isValid() || m_motionLane < 0 || m_motionLane >= m_lanes.size())
        return;
    const HitMixSpec *mix = currentMix();
    int sec = mix ? mix->incubateSec : 0;
    if (sec < 0)
        sec = 0;
    if (sec > 7200)
        sec = 7200;
    if (sec <= 0)
        return;
    HitLane &lane = m_lanes[m_motionLane];
    lane.stepOriginMs = m_clock->elapsed();
    lane.stepTotalMs = sec * 1000;
    lane.deadlineMs = lane.stepOriginMs + lane.stepTotalMs;
    lane.lastProgressSec = 0;
    lane.readRetry = 0;
    lane.waitKind = TimerIncubate;
    emitStepProgress(lane);
}

void HitTestRunner::startIncubate()
{
    const HitMixSpec *mix = currentMix();
    int sec = mix ? mix->incubateSec : 0;
    if (sec < 0)
        sec = 0;
    if (sec > 7200)
        sec = 7200;
    if (!m_clock || !m_clock->isValid() || m_motionLane < 0 || m_motionLane >= m_lanes.size())
        return;
    HitLane &lane = m_lanes[m_motionLane];
    const bool clockOn = lane.waitKind == TimerIncubate && lane.stepTotalMs > 0 && lane.deadlineMs > 0;
    if (!clockOn && sec > 0) {
        lane.stepOriginMs = m_clock->elapsed();
        lane.stepTotalMs = sec * 1000;
        lane.deadlineMs = lane.stepOriginMs + lane.stepTotalMs;
        lane.lastProgressSec = 0;
        lane.readRetry = 0;
        lane.waitKind = TimerIncubate;
        emitStepProgress(lane);
    }
    const int remain = lane.deadlineMs > 0
            ? static_cast<int>(lane.deadlineMs - m_clock->elapsed()) : 0;
    if (sec <= 0 || remain <= 0) {
        lane.waitKind = TimerNone;
        if (mix && mix->reagentUl > 0)
            enter(StageSuckReagent);
        else
            beginStableWait();
        return;
    }
    releaseForWait(TimerIncubate, remain);
}

void HitTestRunner::beginStableWait()
{
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    m_pppSeq = mw ? mw->getChannelPositiveFilterSeq(static_cast<quint8>(channel1())) : 0;
    m_readRetry = 0;
    releaseForWait(TimerStable, 1000);
}

void HitTestRunner::parkStable(HitLane &lane)
{
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    int channel = 0;
    if (lane.jobIndex >= 0 && lane.jobIndex < m_jobs.size())
        channel = m_jobs.at(lane.jobIndex).channel;
    lane.pppSeq = (mw && channel >= 1)
            ? mw->getChannelPositiveFilterSeq(static_cast<quint8>(channel)) : 0;
    lane.readRetry = 0;
    lane.waitKind = TimerStable;
    lane.needsMotion = false;
    lane.stage = StageThrowMix;
    lane.deadlineMs = (m_clock && m_clock->isValid()) ? m_clock->elapsed() + 1000 : 0;
}

bool HitTestRunner::readStableOf(const HitLane &lane, int &value) const
{
    value = 0;
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    if (!mw || lane.jobIndex < 0 || lane.jobIndex >= m_jobs.size())
        return false;
    const int channel = m_jobs.at(lane.jobIndex).channel;
    if (channel < 1)
        return false;
    quint32 seq = 0;
    return mw->getStablePositiveChannelVal(static_cast<quint8>(channel), lane.pppSeq, value, seq);
}

bool HitTestRunner::claimCup(QString *err)
{
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (!sql) {
        if (err)
            *err = u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf");
        return false;
    }
    quint8 loc = 0;
    if (sql->inquire_test_free_tube_status(loc) <= 0) {
        if (err)
            *err = u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf");
        return false;
    }
    m_cupHole = loc;
    sql->update_test_tube_status(loc, TESTTUBES_MARK, QStringLiteral("HIT"), 0);
    Testing *testing = FullyAutomatedPlatelets::pinstanceTesting();
    if (testing)
        testing->tubeMarked(QStringLiteral("HIT"), loc);
    m_cupClaimed = true;
    m_cupInChannel = false;
    return true;
}

bool HitTestRunner::claimAllCups(QString *err)
{
    for (int i = 0; i < HitCupSlots; ++i) {
        m_cups[i] = -1;
        m_cupFilled[i] = false;
    }
    for (int i = 0; i < HitCupSlots; ++i) {
        if (!claimCup(err)) {
            for (int j = 0; j < HitCupSlots; ++j) {
                if (m_cups[j] >= 0)
                    freeHole(m_cups[j]);
                m_cups[j] = -1;
                m_cupFilled[j] = false;
            }
            m_cupHole = -1;
            m_cupClaimed = false;
            m_cupInChannel = false;
            return false;
        }
        m_cups[i] = m_cupHole;
    }
    m_cupHole = m_cups[0];
    m_cupClaimed = true;
    m_cupInChannel = false;
    return true;
}

bool HitTestRunner::planCups(QString *reason)
{
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    quint8 ignored = 0;
    const int freeCount = sql ? sql->inquire_test_free_tube_status(ignored) : 0;
    const int need = m_lanes.size() * HitCupSlots;
    const QString shortage = u8(
        "\xe7\xa9\xba\xe8\xaf\x95\xe7\xae\xa1\xe4\xb8\x8d\xe8\xb6\xb3\xef\xbc\x8c"
        "\xe5\xbd\x93\xe5\x89\x8d\xe7\xa9\xba\xe9\x97\xb2%1\xe4\xb8\xaa\xef\xbc\x8c"
        "\xe6\x9c\xac\xe6\xac\xa1\xe9\x9c\x80\xe8\xa6\x81%2\xe4\xb8\xaa"
        "\xef\xbc\x88\xe6\xaf\x8f\xe4\xb8\xaa\xe6\xa0\xb7\xe6\x9c\xac""5""\xe4\xb8\xaa\xef\xbc\x89")
            .arg(freeCount).arg(need);
    if (!sql || freeCount < need) {
        if (reason)
            *reason = shortage;
        return false;
    }

    for (int laneIndex = 0; laneIndex < m_lanes.size(); ++laneIndex) {
        HitLane &lane = m_lanes[laneIndex];
        for (int cup = 0; cup < HitCupSlots; ++cup) {
            QString err;
            if (!claimCup(&err)) {
                for (int i = 0; i < m_lanes.size(); ++i) {
                    HitLane &held = m_lanes[i];
                    for (int c = 0; c < HitCupSlots; ++c) {
                        if (held.cups[c] >= 0)
                            freeHole(held.cups[c]);
                        held.cups[c] = -1;
                        held.cupFilled[c] = false;
                    }
                    held.cupHole = -1;
                    held.cupClaimed = false;
                    held.cupInChannel = false;
                }
                m_cupHole = -1;
                m_cupClaimed = false;
                m_cupInChannel = false;
                for (int c = 0; c < HitCupSlots; ++c) {
                    m_cups[c] = -1;
                    m_cupFilled[c] = false;
                }
                if (reason) {
                    quint8 again = 0;
                    const int left = sql->inquire_test_free_tube_status(again);
                    *reason = u8(
                        "\xe7\xa9\xba\xe8\xaf\x95\xe7\xae\xa1\xe4\xb8\x8d\xe8\xb6\xb3\xef\xbc\x8c"
                        "\xe5\xbd\x93\xe5\x89\x8d\xe7\xa9\xba\xe9\x97\xb2%1\xe4\xb8\xaa\xef\xbc\x8c"
                        "\xe6\x9c\xac\xe6\xac\xa1\xe9\x9c\x80\xe8\xa6\x81%2\xe4\xb8\xaa"
                        "\xef\xbc\x88\xe6\xaf\x8f\xe4\xb8\xaa\xe6\xa0\xb7\xe6\x9c\xac""5""\xe4\xb8\xaa\xef\xbc\x89")
                            .arg(left).arg(need);
                }
                return false;
            }
            lane.cups[cup] = m_cupHole;
            lane.cupFilled[cup] = false;
        }
        lane.cupHole = lane.cups[0];
        lane.cupClaimed = true;
        lane.cupInChannel = false;
    }
    m_cupHole = -1;
    m_cupClaimed = false;
    m_cupInChannel = false;
    for (int c = 0; c < HitCupSlots; ++c) {
        m_cups[c] = -1;
        m_cupFilled[c] = false;
    }
    QLOG_INFO() << "HIT plan cups" << need << "free" << freeCount;
    return true;
}

void HitTestRunner::releaseCup()
{
    if (m_cupHole < 0)
        return;
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql)
        sql->update_test_tube_status(static_cast<quint8>(m_cupHole), TESTTUBES_FREETIME, QString(), 0);
    m_cupClaimed = false;
    m_cupHole = -1;
}

void HitTestRunner::markHoleUsed(int hole)
{
    if (hole < 0)
        return;
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql)
        sql->UpdateEmptyTube_State(hole, TESTTUBES_CLIPPEDAWAY);
    Testing *testing = FullyAutomatedPlatelets::pinstanceTesting();
    if (testing)
        testing->InitUIEmptyTubeused(hole, true);
}

void HitTestRunner::freeHole(int hole)
{
    if (hole < 0)
        return;
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql)
        sql->update_test_tube_status(static_cast<quint8>(hole), TESTTUBES_FREETIME, QString(), 0);
    Testing *testing = FullyAutomatedPlatelets::pinstanceTesting();
    if (testing)
        testing->releaseMarkedTube(static_cast<quint8>(hole));
}

void HitTestRunner::retireLaneCups(HitLane &lane)
{
    const int skip = lane.cupInChannel ? lane.cupHole : -1;
    for (int i = 0; i < HitCupSlots; ++i) {
        const int hole = lane.cups[i];
        if (hole < 0 || hole == skip)
            continue;
        if (lane.cupFilled[i])
            markHoleUsed(hole);
        else
            freeHole(hole);
        lane.cups[i] = -1;
        lane.cupFilled[i] = false;
    }
    if (lane.cupInChannel)
        lane.cupClaimed = skip >= 0;
    else {
        lane.cupHole = -1;
        lane.cupClaimed = false;
    }
}

void HitTestRunner::markCupUsed()
{
    if (m_cupHole < 0)
        return;
    const int hole = m_cupHole;
    markHoleUsed(hole);
    for (int i = 0; i < HitCupSlots; ++i) {
        if (m_cups[i] == hole) {
            m_cups[i] = -1;
            m_cupFilled[i] = false;
        }
    }
    m_cupHole = -1;
    m_cupClaimed = false;
    for (int i = 0; i < HitCupSlots; ++i) {
        if (m_cups[i] >= 0) {
            m_cupClaimed = true;
            break;
        }
    }
}

void HitTestRunner::deductClean(quint8 kind)
{
    QualityControl *instrument = FullyAutomatedPlatelets::pinstanceinstrument();
    if (!instrument)
        return;
    instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE, kind);
    QLOG_INFO() << "HIT deduct clean" << kind;
}

void HitTestRunner::notifySourceSucked(int holeOffset)
{
    if (m_sampleIndex < 0 || m_sampleIndex >= m_jobs.size())
        return;
    const int hole = m_jobs.at(m_sampleIndex).pppHole + holeOffset;
    if (hole <= 0)
        return;
    emit placeHoleConsumed(hole);
}

void HitTestRunner::setStir(bool on)
{
    const int ch0 = channel1() - 1;
    Monitor_TrayTest *monitor = FullyAutomatedPlatelets::pinstanceobtainModuledata();
    if (!monitor || ch0 < 0 || ch0 >= 12)
        return;
    monitor->controlChnMotorRotating(static_cast<quint8>(ch0), on);
}

int HitTestRunner::readChannel() const
{
    return readChannelOf(channel1());
}

int HitTestRunner::readChannelOf(int channel) const
{
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    if (!mw || channel < 1)
        return 0;
    return mw->getChannelTestVal(static_cast<quint8>(channel));
}

bool HitTestRunner::readStable(int &value) const
{
    value = 0;
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    const int channel = channel1();
    if (!mw || channel < 1)
        return false;
    quint32 seq = 0;
    return mw->getStablePositiveChannelVal(static_cast<quint8>(channel), m_pppSeq, value, seq);
}

bool HitTestRunner::pointOk(const QPoint &point) const
{
    return !(point.x() == 0 && point.y() == 0);
}

bool HitTestRunner::bloodPoint(int displayHole, QPoint &out, QString *err) const
{
    out = QPoint();
    if (displayHole <= 0 || displayHole > 255) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    SingletonAxis::bloodSampleZonePos(READ_OPERRAT, static_cast<quint8>(displayHole - 1), out);
    if (!pointOk(out)) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    return true;
}

bool HitTestRunner::trayPoint(int hole, quint8 needle, QPoint &out, QString *err) const
{
    out = QPoint();
    if (hole < 0 || hole > 255) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    SingletonAxis::testTaryZoneAxisPos(READ_OPERRAT, static_cast<quint8>(hole), needle, out);
    if (!pointOk(out)) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    return true;
}

bool HitTestRunner::channelReagentPoint(QPoint &out, QString *err) const
{
    out = QPoint();
    const int ch0 = channel1() - 1;
    if (ch0 < 0 || ch0 > 255) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(ch0), MOTOR_REAGNET_INDEX, out);
    if (!pointOk(out)) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    return true;
}

bool HitTestRunner::reagentPoint(int index, QPoint &out, QString *err) const
{
    out = QPoint();
    if (index < 0 || index > 9) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    SingletonAxis::reagetZoneAxisPos(READ_OPERRAT, static_cast<quint8>(index), out);
    if (!pointOk(out)) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    return true;
}

bool HitTestRunner::buildBloodTransfer(const QPoint &suck, const QPoint &spit, int ul, QString *err)
{
    beginBatch();
    UsbCodeDispose *dll = Testing::getTaskDll();
    auto &ini = INI_File();
    if (!dll || !pointOk(suck) || !pointOk(spit)) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    const int failH = ini.GetFailedLinqueHigh();
    const int tubeDown = static_cast<int>(ini.GetEmptyTubeDownHigh());
    const double ratio = qRound(ini.GetPPPConversionScale() * 100.0) / 100.0;
    const int steps = static_cast<int>(qRound(ul * ratio));
    if (failH <= 0 || tubeDown <= 0 || steps <= 0) {
        if (err)
            *err = u8("\xe4\xb8\x8b\xe9\x99\x8d\xe9\xab\x98\xe5\xba\xa6\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }

    QByteArrayList cmds;
    QList<quint8> delays;
    QUIUtils::allZAxisBackOrigin(cmds, 0);
    quint8 num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_XYMoveSpecifiedPosition(suck, BloodNddle, 0, num));
    auto &manager = PumpPositionManager::instance();
    if (manager.getPosition().samplePumpLoc != BACK_SUCKAIRS) {
        num = static_cast<quint8>(cmds.count() % 255);
        cmds.push_back(dll->BigBenActive(true, BACK_SUCKAIRS, num, DIS_WASHES_PUMPS, 0));
        manager.setSamplePumpLoc(BACK_SUCKAIRS);
    }
    const int absPos = BACK_SUCKAIRS + steps;
    QUIUtils::appendHangLiquidBlindThenDetect(cmds, MOTOR_BLOOD_INDEX, failH);
    num = static_cast<quint8>(cmds.count() % 255);
    delays.append(num);
    cmds.push_back(dll->BigBenActive(true, absPos, num, DIS_WASHES_PUMPS, 0));
    manager.setSamplePumpLoc(absPos);
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX, 0, 0, num, false));
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_XYMoveSpecifiedPosition(spit, BloodNddle, 0, num));
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZMoveSpecifiedPosition(
                       MOTOR_BLOOD_INDEX, tubeDown, 0, num, false, tubeDown, false, GRIPPERNORMAL));
    num = static_cast<quint8>(cmds.count() % 255);
    delays.append(num);
    cmds.push_back(dll->BigBenActive(false, BACK_SUCKAIRS, num, DIS_WASHES_PUMPS, 0));
    manager.setSamplePumpLoc(BACK_SUCKAIRS);
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX, 0, 0, num, false));
    takeCommands(cmds);
    const int waitMs = ini.GetAfterSuckDelayMs();
    for (int i = 0; i < m_batch.size(); ++i) {
        const quint8 id = static_cast<quint8>(m_batch.at(i).cmd.at(GET_COMMAND_INDEX));
        if (delays.contains(id))
            m_batch[i].waitAfterMs = waitMs;
    }
    return !m_batch.isEmpty();
}

bool HitTestRunner::buildReagentTransfer(const QPoint &suck, const QPoint &spit, int ul,
                                         quint8 reagentType, QString *err)
{
    beginBatch();
    UsbCodeDispose *dll = Testing::getTaskDll();
    auto &ini = INI_File();
    if (!dll || !pointOk(suck) || !pointOk(spit)) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    const int failH = ini.getFailedCleanLinqueReagNeedle();
    const int tubeDown = ini.getTypesReagentNeedleDownHigh(reagentType);
    const double ratio = ini.getTypesReagentSuckRatio(reagentType);
    const double add = ini.getTypesReagentSuckAdd_Ratio(reagentType);
    int steps = static_cast<int>(qRound(ul * ratio * SMALL_BEN_RATIO * (1.0 + add)));
    if (steps < 0)
        steps = 0;
    int absPos = SMALL_BEN_INHALE_ARI + steps;
    if (absPos > 7200)
        absPos = 7200;
    if (failH <= 0 || tubeDown <= 0 || steps <= 0) {
        if (err)
            *err = u8("\xe4\xb8\x8b\xe9\x99\x8d\xe9\xab\x98\xe5\xba\xa6\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }

    QByteArrayList cmds;
    QList<quint8> delays;
    QUIUtils::allZAxisBackOrigin(cmds, 0);
    quint8 num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_XYMoveSpecifiedPosition(suck, ReagentNeddle, TRAY_TESTCHANNEL, num));
    auto &manager = PumpPositionManager::instance();
    if (manager.getPosition().reagentPumpLoc != SMALL_BEN_INHALE_ARI) {
        num = static_cast<quint8>(cmds.count() % 255);
        cmds.push_back(dll->SmallBenActive(true, SMALL_BEN_INHALE_ARI, DIS_WASHES_PUMPS, num, 0));
        manager.setReagentPumpLoc(SMALL_BEN_INHALE_ARI);
    }
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX, 0, TRAY_TESTCHANNEL, num, false));
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZMoveSpecifiedPosition(
                       MOTOR_REAGNET_INDEX, failH, 0, num,
                       false, failH, false, GRIPPERNORMAL));
    num = static_cast<quint8>(cmds.count() % 255);
    delays.append(num);
    cmds.push_back(dll->SmallBenActive(true, absPos, DIS_WASHES_PUMPS, num, 0));
    manager.setReagentPumpLoc(absPos);
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX, 0, TRAY_TESTCHANNEL, num, false));
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_XYMoveSpecifiedPosition(spit, ReagentNeddle, TRAY_TESTCHANNEL, num));
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZMoveSpecifiedPosition(
                       MOTOR_REAGNET_INDEX, tubeDown, TRAY_TESTCHANNEL, num,
                       false, tubeDown, false, GRIPPERNORMAL));
    num = static_cast<quint8>(cmds.count() % 255);
    delays.append(num);
    cmds.push_back(dll->SmallBenActive(false, SMALL_BEN_INHALE_ARI, DIS_WASHES_PUMPS, num, 0));
    manager.setReagentPumpLoc(SMALL_BEN_INHALE_ARI);
    num = static_cast<quint8>(cmds.count() % 255);
    cmds.push_back(dll->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX, 0, TRAY_TESTCHANNEL, num, false));
    takeCommands(cmds);
    const int waitMs = ini.GetAfterSuckDelayMs();
    for (int i = 0; i < m_batch.size(); ++i) {
        const quint8 id = static_cast<quint8>(m_batch.at(i).cmd.at(GET_COMMAND_INDEX));
        if (delays.contains(id))
            m_batch[i].waitAfterMs = waitMs;
    }
    return !m_batch.isEmpty();
}

bool HitTestRunner::buildWashBlood()
{
    beginBatch();
    QByteArrayList cmds;
    QUIUtils::CleanBloodPinActionCommd(1, cmds);
    takeCommands(cmds);
    return !m_batch.isEmpty();
}

bool HitTestRunner::buildWashReagent()
{
    beginBatch();
    QVector<QByteArray> steps;
    QUIUtils::CleanReagentsPinActionCommd(steps);
    QByteArrayList cmds;
    for (int i = 0; i < steps.size(); ++i)
        cmds.push_back(steps.at(i));
    takeCommands(cmds);
    return !m_batch.isEmpty();
}

bool HitTestRunner::buildGrip(QString *err)
{
    beginBatch();
    const int ch0 = channel1() - 1;
    QPoint from;
    QPoint to;
    QString localErr;
    if (ch0 < 0 || !trayPoint(m_cupHole, MOTOR_HANDS_INDEX, from, &localErr)) {
        if (err)
            *err = localErr.isEmpty() ? u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88") : localErr;
        return false;
    }
    SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(ch0), MOTOR_HANDS_INDEX, to);
    const quint8 trayDown = INI_File()._gethandsdownheightinTesttray(
                SingletonAxis::witchoneindexTary(static_cast<quint8>(m_cupHole)));
    const quint8 channelDown = INI_File()._gethandsdownheiht(static_cast<quint8>(ch0));
    if (!pointOk(to) || trayDown == 0 || channelDown == 0) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    QByteArrayList cmds;
    quint8 judge = 0;
    quint8 lift = 0;
    QUIUtils::suckCupMoveChannelPut(from, to, trayDown, channelDown, cmds, judge, lift);
    takeCommands(cmds);
    const int judgePos = findCommand(judge);
    if (judgePos >= 0)
        m_batch[judgePos].grabJudge = true;
    if (judgePos > 0)
        m_batch[judgePos - 1].grabOpen = true;
    m_grabDownMm = trayDown;
    if (m_batch.isEmpty() && err)
        *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
    return !m_batch.isEmpty();
}

bool HitTestRunner::buildThrow(QString *err)
{
    beginBatch();
    const int ch0 = channel1() - 1;
    if (ch0 < 0) {
        if (err)
            *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }
    QVector<QByteArray> steps;
    quint8 lift = 0;
    quint8 split = 0;
    QUIUtils::ThrowTestChnCup(static_cast<quint8>(ch0), steps, lift, split);
    QByteArrayList cmds;
    for (int i = 0; i < steps.size(); ++i)
        cmds.push_back(steps.at(i));
    takeCommands(cmds);
    const int liftPos = findCommand(lift);
    const int judgePos = liftPos - 1;
    if (judgePos >= 0)
        m_batch[judgePos].grabJudge = true;
    if (judgePos > 0)
        m_batch[judgePos - 1].grabOpen = true;
    m_grabDownMm = INI_File()._gethandsdownheiht(static_cast<quint8>(ch0));
    Q_UNUSED(split);
    if (m_batch.isEmpty() && err)
        *err = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
    return !m_batch.isEmpty();
}

bool HitTestRunner::buildHome()
{
    beginBatch();
    QByteArrayList cmds;
    QUIUtils::appendPostCleanMachineXyHome(cmds);
    takeCommands(cmds);
    return !m_batch.isEmpty();
}

void HitTestRunner::ensureDb()
{
    if (m_dbReady)
        return;
    QSqlDatabase db = hitDb();
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), QStringLiteral("hit-data"));
        const QString dirPath = QCoreApplication::applicationDirPath() + QStringLiteral("/SQLFile");
        QDir().mkpath(dirPath);
        db.setDatabaseName(dirPath + QStringLiteral("/HitData.db"));
    }
    if (!db.open()) {
        QLOG_WARN() << "HIT db open" << db.lastError().text();
        return;
    }
    QSqlQuery query(db);
    const bool runOk = query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS HitRun ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "sample_id TEXT,"
        "channel INTEGER,"
        "sample_tag INTEGER,"
        "start_time TEXT,"
        "end_time TEXT,"
        "ppp REAL,"
        "spontaneous_prp0 REAL,"
        "spontaneous_prpn REAL,"
        "spontaneous_percent REAL,"
        "low_prp0 REAL,"
        "low_prpn REAL,"
        "low_percent REAL,"
        "high_prp0 REAL,"
        "high_prpn REAL,"
        "high_percent REAL,"
        "negative_prp0 REAL,"
        "negative_prpn REAL,"
        "negative_percent REAL,"
        "status TEXT)"));
    const bool pointOk = query.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS HitPoint ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "run_id INTEGER,"
        "sample_id TEXT,"
        "step TEXT,"
        "seq INTEGER,"
        "elapsed_ms INTEGER,"
        "raw_value REAL,"
        "agg_percent REAL)"));
    if (!runOk || !pointOk) {
        QLOG_WARN() << "HIT db create" << query.lastError().text();
        return;
    }
    m_dbReady = true;
}

void HitTestRunner::insertRun()
{
    ensureDb();
    if (!m_dbReady || m_sampleIndex < 0 || m_sampleIndex >= m_jobs.size())
        return;
    const HitSampleJob &job = m_jobs.at(m_sampleIndex);
    QSqlQuery query(hitDb());
    query.prepare(QStringLiteral(
        "INSERT INTO HitRun(sample_id, channel, sample_tag, start_time, status) "
        "VALUES(?,?,?,?,?)"));
    query.addBindValue(job.sampleId);
    query.addBindValue(job.channel);
    query.addBindValue(job.pppHole);
    query.addBindValue(m_startText);
    query.addBindValue(u8("\xe6\xb5\x8b\xe8\xaf\x95\xe4\xb8\xad"));
    if (!query.exec()) {
        QLOG_WARN() << "HIT insert" << query.lastError().text();
        return;
    }
    m_runId = query.lastInsertId().toLongLong();
}

void HitTestRunner::savePpp()
{
    if (!m_dbReady || m_runId < 0)
        return;
    QSqlQuery query(hitDb());
    query.prepare(QStringLiteral("UPDATE HitRun SET ppp=? WHERE id=?"));
    query.addBindValue(m_ppp);
    query.addBindValue(m_runId);
    if (!query.exec())
        QLOG_WARN() << "HIT save ppp" << query.lastError().text();
}

void HitTestRunner::saveStep(const QString &step, double prp0, double prpn, double percent)
{
    if (!m_dbReady || m_runId < 0)
        return;
    QString prp0Col;
    QString prpnCol;
    QString percentCol;
    if (step == QLatin1String("spontaneous")) {
        prp0Col = QStringLiteral("spontaneous_prp0");
        prpnCol = QStringLiteral("spontaneous_prpn");
        percentCol = QStringLiteral("spontaneous_percent");
    } else if (step == QLatin1String("low")) {
        prp0Col = QStringLiteral("low_prp0");
        prpnCol = QStringLiteral("low_prpn");
        percentCol = QStringLiteral("low_percent");
    } else if (step == QLatin1String("high")) {
        prp0Col = QStringLiteral("high_prp0");
        prpnCol = QStringLiteral("high_prpn");
        percentCol = QStringLiteral("high_percent");
    } else if (step == QLatin1String("negative")) {
        prp0Col = QStringLiteral("negative_prp0");
        prpnCol = QStringLiteral("negative_prpn");
        percentCol = QStringLiteral("negative_percent");
    } else {
        return;
    }
    QSqlQuery query(hitDb());
    query.prepare(QStringLiteral("UPDATE HitRun SET %1=?, %2=?, %3=? WHERE id=?")
                  .arg(prp0Col, prpnCol, percentCol));
    query.addBindValue(prp0);
    query.addBindValue(prpn);
    query.addBindValue(percent);
    query.addBindValue(m_runId);
    if (!query.exec())
        QLOG_WARN() << "HIT save step" << query.lastError().text();
}

void HitTestRunner::savePoint(const QString &step, int seq, int elapsedMs, double raw, double percent)
{
    if (!m_dbReady || m_runId < 0 || m_sampleIndex < 0 || m_sampleIndex >= m_jobs.size())
        return;
    QSqlQuery query(hitDb());
    query.prepare(QStringLiteral(
        "INSERT INTO HitPoint(run_id, sample_id, step, seq, elapsed_ms, raw_value, agg_percent) "
        "VALUES(?,?,?,?,?,?,?)"));
    query.addBindValue(m_runId);
    query.addBindValue(m_jobs.at(m_sampleIndex).sampleId);
    query.addBindValue(step);
    query.addBindValue(seq);
    query.addBindValue(elapsedMs);
    query.addBindValue(raw);
    query.addBindValue(percent);
    if (!query.exec())
        QLOG_WARN() << "HIT save point" << query.lastError().text();
}

void HitTestRunner::closeRun(const QString &status)
{
    if (!m_dbReady || m_runId < 0)
        return;
    QSqlQuery query(hitDb());
    query.prepare(QStringLiteral("UPDATE HitRun SET end_time=?, status=? WHERE id=?"));
    query.addBindValue(nowText());
    query.addBindValue(status);
    query.addBindValue(m_runId);
    if (!query.exec())
        QLOG_WARN() << "HIT close" << query.lastError().text();
}
