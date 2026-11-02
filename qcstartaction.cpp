#include "qcstartaction.h"

#include "cglobal.h"
#include "custom_struct.h"
#include "globaldata.h"
#include "ini_file.h"
#include "loadequipmentpos.h"
#include "mainwindow.h"
#include "quiutils.h"
#include "testing.h"
#include "Custommovements/performanceverification.h"
#include "operclass/fullyautomatedplatelets.h"
#include "QsLog/include/QsLog.h"
#include "suoweiserialport.h"

#include <QDate>
#include <QTime>
#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <algorithm>
#include <cmath>

namespace {

QString u8(const char *text)
{
    return QString::fromUtf8(text);
}

const int kWaterHole = 0;
const int kHighStdHole = 2;
const int kHighMatHole = 3;
const int kMidStdHole = 4;
const int kMidMatHole = 5;
const int kLowStdHole = 6;
const int kLowMatHole = 7;
const int kStableTimeoutMs = 45000;

QString channelName(int channel1)
{
    return u8("\xe9\x80\x9a\xe9\x81\x93") + QString::number(channel1);
}

QString recordName(int record)
{
    switch (record) {
    case 0: return u8("\xe6\xb0\xb4");
    case 1: return u8("\xe9\xab\x98\xe5\x80\xbc\xe6\xa0\x87\xe5\x87\x86");
    case 2: return u8("\xe9\xab\x98\xe5\x80\xbc");
    case 3: return u8("\xe4\xb8\xad\xe5\x80\xbc\xe6\xa0\x87\xe5\x87\x86");
    case 4: return u8("\xe4\xb8\xad\xe5\x80\xbc");
    case 5: return u8("\xe4\xbd\x8e\xe5\x80\xbc\xe6\xa0\x87\xe5\x87\x86");
    case 6: return u8("\xe4\xbd\x8e\xe5\x80\xbc");
    default: return QString();
    }
}

bool usablePoint(const QPoint &pt)
{
    return pt.x() != 0 || pt.y() != 0;
}

bool qcRatioPercent(double measured, double standard, double water, double &percent)
{
    const double eps = 1e-9;
    if (measured <= eps || standard <= eps || water <= eps)
        return false;
    const double numerBase = measured / standard;
    const double denomBase = water / standard;
    if (numerBase <= eps || denomBase <= eps)
        return false;
    const double denomLog = std::log10(denomBase);
    if (std::fabs(denomLog) <= eps)
        return false;
    percent = std::log10(numerBase) / denomLog * 100.0;
    return true;
}

quint8 trayGripHeight(int hole)
{
    const quint8 tary = SingletonAxis::witchoneindexTary(static_cast<quint8>(hole));
    return INI_File()._gethandsdownheightinTesttray(tary);
}

quint8 channelGripHeight(int channel0)
{
    return INI_File()._gethandsdownheiht(static_cast<quint8>(channel0));
}

} // namespace

Q_GLOBAL_STATIC(QcStartAction, qcStartActionInstance)

QcStartAction *QcStartAction::instance()
{
    return qcStartActionInstance();
}

QcStartAction::QcStartAction(QObject *parent)
    : QObject(parent)
    , m_wired(false)
    , m_running(false)
    , m_phase(PhaseIdle)
    , m_jobIndex(0)
    , m_expectIndex(0)
    , m_grabJudgeIndex(255)
    , m_grabOpenIndex(255)
    , m_grabLiftIndex(255)
    , m_grabDeepenTimes(0)
    , m_pickDownMm(0)
    , m_grabOk(false)
    , m_placing(false)
    , m_regrab(false)
    , m_promptingRegrab(false)
    , m_stepwise(false)
    , m_aborting(false)
    , m_pending()
    , m_pendingPos(0)
    , m_abortCmds()
    , m_abortPos(0)
    , m_waitTimer(new QTimer(this))
    , m_waitChannel1(0)
    , m_waitChannel0(0)
    , m_waitRecord(RecWater)
    , m_minSeq(0)
    , m_waitMs(0)
    , m_highLot()
    , m_midLot()
    , m_lowLot()
{
    m_waitTimer->setInterval(500);
    connect(m_waitTimer, &QTimer::timeout, this, &QcStartAction::onWaitTick);
    ensureWired();
}

void QcStartAction::ensureWired()
{
    if (m_wired)
        return;
    SuoweiSerialPort *serial = FullyAutomatedPlatelets::pinstanceserialusb();
    if (!serial)
        return;
    connect(this, &QcStartAction::sendOrderList,
            serial, &SuoweiSerialPort::slotwrite_instructions_group,
            Qt::QueuedConnection);
    connect(serial, &SuoweiSerialPort::serialQcData,
            this, &QcStartAction::onSerial,
            Qt::QueuedConnection);
    m_wired = true;
}

bool QcStartAction::takeCup(int &hole, QString *reason)
{
    hole = 0;
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (!sql) {
        if (reason)
            *reason = u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf");
        return false;
    }
    quint8 loc = 0;
    if (sql->inquire_test_free_tube_status(loc) <= 0) {
        if (reason)
            *reason = u8("\xe6\xb2\xa1\xe6\x9c\x89\xe7\xa9\xba\xe8\xaf\x95\xe6\x9d\xaf");
        return false;
    }
    hole = loc;
    sql->update_test_tube_status(static_cast<quint8>(hole), TESTTUBES_MARK, QStringLiteral("QC"), 0);
    Testing *testing = FullyAutomatedPlatelets::pinstanceTesting();
    if (testing)
        testing->tubeMarked(QStringLiteral("QC"), static_cast<quint8>(hole));
    m_claimed.append(hole);
    return true;
}

void QcStartAction::releaseTray(int hole)
{
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql && hole >= 0)
        sql->update_test_tube_status(static_cast<quint8>(hole), TESTTUBES_FREETIME, QString(), 0);
    m_claimed.removeAll(hole);
}

void QcStartAction::markCupUsed(int hole)
{
    if (hole < 0)
        return;
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql)
        sql->UpdateEmptyTube_State(hole, TESTTUBES_CLIPPEDAWAY);
    Testing *testing = FullyAutomatedPlatelets::pinstanceTesting();
    if (testing)
        testing->InitUIEmptyTubeused(hole, true);
    m_claimed.removeAll(hole);
}

void QcStartAction::releaseAllClaimed()
{
    const QVector<int> holes = m_claimed;
    for (int i = 0; i < holes.size(); ++i)
        releaseTray(holes.at(i));
}

void QcStartAction::appendCupRoute(int trayHole, int record, bool discard)
{
    Job grip;
    grip.kind = JobGripTrayToCh;
    grip.trayHole = trayHole;
    grip.channel = m_channels.at(0);
    grip.record = record;
    m_jobs.append(grip);

    for (int i = 0; i < m_channels.size(); ++i) {
        if (i > 0) {
            Job move;
            move.kind = JobGripChToCh;
            move.channel = m_channels.at(i - 1);
            move.channelTo = m_channels.at(i);
            move.record = record;
            m_jobs.append(move);
        }
        Job read;
        read.kind = JobRead;
        read.channel = m_channels.at(i);
        read.record = record;
        m_jobs.append(read);
    }

    Job done;
    done.channel = m_channels.last();
    done.trayHole = trayHole;
    done.record = record;
    done.kind = discard ? JobThrow : JobGripChToTray;
    m_jobs.append(done);
}

bool QcStartAction::appendSuckRouteWash(int bloodHole, int record, QString *reason)
{
    int tray = 0;
    if (!takeCup(tray, reason))
        return false;
    Job suck;
    suck.kind = JobSuck;
    suck.bloodHole = bloodHole;
    suck.trayHole = tray;
    suck.record = record;
    m_jobs.append(suck);
    appendCupRoute(tray, record, true);
    Job wash;
    wash.kind = JobWash;
    wash.record = record;
    m_jobs.append(wash);
    return true;
}

bool QcStartAction::buildJobs(const QList<int> &levels, QString *reason)
{
    m_jobs.clear();
    bool wantHigh = levels.contains(0) || levels.contains(3);
    bool wantMid = levels.contains(1) || levels.contains(3);
    bool wantLow = levels.contains(2) || levels.contains(3);
    if (!wantHigh && !wantMid && !wantLow) {
        if (reason)
            *reason = u8("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe8\xb4\xa8\xe6\x8e\xa7\xe5\x80\xbc");
        return false;
    }

    if (!appendSuckRouteWash(kWaterHole, RecWater, reason))
        return false;

    struct LevelStep {
        int materialRecord;
        int stdHole;
        int matHole;
        int stdRecord;
    };
    QVector<LevelStep> steps;
    if (wantHigh)
        steps.append(LevelStep{RecHigh, kHighStdHole, kHighMatHole, RecHighStd});
    if (wantMid)
        steps.append(LevelStep{RecMid, kMidStdHole, kMidMatHole, RecMidStd});
    if (wantLow)
        steps.append(LevelStep{RecLow, kLowStdHole, kLowMatHole, RecLowStd});

    for (int i = 0; i < steps.size(); ++i) {
        const LevelStep step = steps.at(i);
        if (!appendSuckRouteWash(step.stdHole, step.stdRecord, reason))
            return false;
        if (!appendSuckRouteWash(step.matHole, step.materialRecord, reason))
            return false;
    }
    return true;
}

bool QcStartAction::gripJob(JobKind kind) const
{
    return kind == JobGripTrayToCh || kind == JobGripChToCh
            || kind == JobGripChToTray || kind == JobThrow;
}

bool QcStartAction::buildGripLeg(const QPoint &target, int downMm, bool picking, QByteArrayList &out)
{
    out.clear();
    UsbCodeDispose *dll = Testing::getTaskDll();
    if (!dll || downMm <= 0 || !usablePoint(target))
        return false;

    QVector<QByteArray> steps;
    if (picking && m_regrab) {
        quint8 resetIndex = static_cast<quint8>(steps.size() % 255);
        steps.push_back(dll->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, 0, 0, resetIndex, false));
    }
    QUIUtils::appendBloodReagentNeedleResetBeforeGripperXy(steps, 0);
    quint8 idx = static_cast<quint8>(steps.size() % 255);
    steps.push_back(dll->DLL_XYMoveSpecifiedPosition(target, 0, 0, idx));
    const int gripAt = steps.size();
    quint8 endNo = 0;
    if (picking) {
        const bool repeat = m_regrab || INI_File().rConfigPara(REPEATGRABCUP).toBool();
        QUIUtils::appendGripperDescendThenVacuumHold(steps, downMm, repeat, endNo, 0);
        if (steps.size() < gripAt + 4 || steps.at(gripAt + 2).size() <= GET_COMMAND_INDEX)
            return false;
        m_pickDownMm = downMm;
        m_grabOpenIndex = static_cast<quint8>(steps.at(gripAt + 1).at(GET_COMMAND_INDEX));
        m_grabJudgeIndex = static_cast<quint8>(steps.at(gripAt + 2).at(GET_COMMAND_INDEX));
        m_grabLiftIndex = endNo;
        m_grabOk = false;
    } else {
        m_grabJudgeIndex = 255;
        m_grabOpenIndex = 255;
        m_grabLiftIndex = 255;
        m_pickDownMm = 0;
        QUIUtils::appendGripperDescendThenVacuumRelease(steps, downMm, endNo, 0);
    }
    for (int i = 0; i < steps.size(); ++i)
        out.append(steps.at(i));
    return !out.isEmpty();
}

void QcStartAction::requestRegrabPrompt()
{
    if (!m_running || m_promptingRegrab)
        return;
    m_promptingRegrab = true;
    m_phase = PhaseIdle;
    QTimer::singleShot(0, this, [this]() { promptRegrab(); });
}

void QcStartAction::promptRegrab()
{
    if (!m_running || !m_promptingRegrab)
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
    const QString giveUpTip = u8(
        "\xe6\x94\xbe\xe5\xbc\x83\xe5\x90\x8e\xe5\x85\x88\xe6\x8a\xac\xe8\xb5\xb7\xe6\x8a\x93\xe6\x89\x8b\xe5\x92\x8c\xe5\x8f\x8c\xe9\x92\x88\xef\xbc\x8c"
        "\xe5\x9b\x9e\xe5\x88\xb0\xe5\x8e\x9f\xe7\x82\xb9\xef\xbc\x8c\xe5\x86\x8d\xe6\xb8\x85\xe6\xb4\x97\xe8\xa1\x80\xe6\xa0\xb7\xe9\x92\x88\xe3\x80\x82");
    const QString tip = (m_grabDeepenTimes >= 3)
            ? u8("\xe8\xb4\x9f\xe5\x8e\x8b\xe6\xb2\xa1\xe5\x88\xb0\xef\xbc\x8c\xe5\xb7\xb2\xe7\xbb\x8f\xe5\x86\x8d\xe5\xbe\x80\xe4\xb8\x8b\xe5\x90\xb8\xe4\xba\x86")
              + QStringLiteral("3")
              + u8("\xe6\xac\xa1\xef\xbc\x8c"
                 "\xe8\xbf\x98\xe6\x98\xaf\xe6\xb2\xa1\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1\xe3\x80\x82\n"
                 "\xe9\x87\x8d\xe6\x8a\x93\xe4\xbc\x9a\xe4\xbb\x8e\xe5\xa4\xb4\xe5\x86\x8d\xe6\x8a\x93\xe4\xb8\x80\xe6\xac\xa1\xe3\x80\x82") + giveUpTip
            : u8("\xe6\xb2\xa1\xe6\x9c\x89\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1\xe3\x80\x82\n"
                 "\xe9\x87\x8d\xe6\x8a\x93\xe4\xbc\x9a\xe5\x86\x8d\xe6\x8a\x93\xe4\xb8\x80\xe6\xac\xa1\xe3\x80\x82") + giveUpTip;
    QLabel *message = new QLabel(tip, &dlg);
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
    QPushButton *again = new QPushButton(u8("\xe9\x87\x8d\xe6\x8a\x93"), &dlg);
    again->setDefault(true);
    again->setCursor(Qt::PointingHandCursor);
    giveUp->setCursor(Qt::PointingHandCursor);
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
    m_promptingRegrab = false;
    if (retry) {
        m_regrab = true;
        m_placing = false;
        m_grabOk = false;
        m_grabDeepenTimes = 0;
        runJob();
        return;
    }
    beginAbort();
}

void QcStartAction::beginAbort()
{
    m_waitTimer->stop();
    m_promptingRegrab = false;
    m_stepwise = false;
    m_pending.clear();
    m_phase = PhaseIdle;
    for (int i = 0; i < m_channels.size(); ++i)
        setSpin(m_channels.at(i) - 1, false);

    m_abortCmds = GlobalData::errMachineBackOrigin(200, MOTOR_HANDS_INDEX);
    QByteArrayList wash;
    QUIUtils::CleanBloodPinActionCommd(1, wash);
    for (int i = 0; i < wash.size(); ++i) {
        if (!wash.at(i).isEmpty())
            m_abortCmds.append(wash.at(i));
    }
    m_aborting = true;
    emit progressText(u8("\xe6\x8a\x93\xe6\x9d\xaf\xe5\xa4\xb1\xe8\xb4\xa5\xef\xbc\x8c\xe6\xad\xa3\xe5\x9c\xa8\xe5\xa4\x8d\xe4\xbd\x8d\xe5\xb9\xb6\xe6\xb8\x85\xe6\xb4\x97"));
    if (m_abortCmds.isEmpty()) {
        fail(u8("\xe6\x8a\x93\xe6\x9d\xaf\xe5\xa4\xb1\xe8\xb4\xa5"));
        return;
    }
    sendAbortAt(0);
}

void QcStartAction::sendOne(const QByteArray &cmd, const QString &info)
{
    if (cmd.size() <= GET_COMMAND_INDEX) {
        fail(u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88"));
        return;
    }
    m_batchIndexes.clear();
    m_batchIndexes.append(static_cast<quint8>(cmd.at(GET_COMMAND_INDEX)));
    m_expectIndex = m_batchIndexes.last();
    m_stepwise = true;
    m_phase = PhaseAck;
    QByteArrayList one;
    one.append(cmd);
    emit sendOrderList(one, info);
}

void QcStartAction::sendPendingAt(int pos)
{
    if (!m_running || m_aborting || m_promptingRegrab)
        return;
    if (pos < 0 || pos >= m_pending.size()) {
        requestRegrabPrompt();
        return;
    }
    m_pendingPos = pos;
    sendOne(m_pending.at(pos), QStringLiteral("QC"));
}

void QcStartAction::sendAbortAt(int pos)
{
    while (pos < m_abortCmds.size() && m_abortCmds.at(pos).size() <= GET_COMMAND_INDEX)
        ++pos;
    if (pos >= m_abortCmds.size()) {
        m_aborting = false;
        fail(u8("\xe6\x9c\xaa\xe6\x8a\x93\xe5\x88\xb0\xe8\xaf\x95\xe7\xae\xa1\xef\xbc\x8c\xe5\xb7\xb2\xe5\xa4\x8d\xe4\xbd\x8d\xe5\xb9\xb6\xe6\xb8\x85\xe6\xb4\x97"));
        return;
    }
    m_abortPos = pos;
    sendOne(m_abortCmds.at(pos), QStringLiteral("QC abort"));
}

QByteArray QcStartAction::buildGrabDeepen(quint8 commandIndex) const
{
    UsbCodeDispose *dll = Testing::getTaskDll();
    if (!dll || m_pickDownMm <= 0)
        return QByteArray();
    int extra = m_grabDeepenTimes;
    if (extra < 1)
        extra = 1;
    if (extra > 3)
        extra = 3;
    const int down = m_pickDownMm + extra;
    return QUIUtils::withGripperPump(
                dll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, down, 0, commandIndex, false,
                                           down, true, GRABCUPWITHGRIPPER),
                GRIPPERMOVE_ABNORMALLY);
}

bool QcStartAction::commandForJob(const Job &job, QByteArrayList &out, QString *reason)
{
    out.clear();
    UsbCodeDispose *dll = Testing::getTaskDll();
    if (!dll) {
        if (reason)
            *reason = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }

    if (job.kind == JobSuck) {
        QPoint suck(0, 0);
        QPoint spit(0, 0);
        SingletonAxis::bloodSampleZonePos(READ_OPERRAT, static_cast<quint8>(job.bloodHole), suck);
        SingletonAxis::testTaryZoneAxisPos(READ_OPERRAT, static_cast<quint8>(job.trayHole),
                                           MOTOR_BLOOD_INDEX, spit);
        if (!usablePoint(suck) || !usablePoint(spit)) {
            if (reason)
                *reason = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
            return false;
        }
        QUIUtils::qualitysucksampleact(suck, spit, out);
        return !out.isEmpty();
    }

    if (job.kind == JobWash) {
        QUIUtils::CleanBloodPinActionCommd(1, out);
        return !out.isEmpty();
    }

    if (job.kind == JobThrow) {
        QPoint channelPt(0, 0);
        QPoint throwPt(0, 0);
        const int channel0 = job.channel - 1;
        SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(channel0),
                                      MOTOR_HANDS_INDEX, channelPt);
        SingletonAxis::throwTubeHolePos(READ_OPERRAT, throwPt);
        const quint8 down = channelGripHeight(channel0);
        const quint8 throwDown = INI_File()._gethandsdownthrowcpus();
        if (!usablePoint(channelPt) || !usablePoint(throwPt) || down == 0 || throwDown == 0) {
            if (reason)
                *reason = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
            return false;
        }
        const bool ok = m_placing
                ? buildGripLeg(throwPt, throwDown, false, out)
                : buildGripLeg(channelPt, down, true, out);
        if (!ok && reason)
            *reason = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return ok;
    }

    QPoint from(0, 0);
    QPoint to(0, 0);
    quint8 downPick = 0;
    quint8 downPut = 0;
    if (job.kind == JobGripTrayToCh) {
        const int channel0 = job.channel - 1;
        SingletonAxis::testTaryZoneAxisPos(READ_OPERRAT, static_cast<quint8>(job.trayHole),
                                           MOTOR_HANDS_INDEX, from);
        SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(channel0),
                                      MOTOR_HANDS_INDEX, to);
        downPick = trayGripHeight(job.trayHole);
        downPut = channelGripHeight(channel0);
        if (downPick == 0)
            downPick = downPut;
        if (downPut == 0)
            downPut = downPick;
    } else if (job.kind == JobGripChToCh) {
        const int from0 = job.channel - 1;
        const int to0 = job.channelTo - 1;
        SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(from0), MOTOR_HANDS_INDEX, from);
        SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(to0), MOTOR_HANDS_INDEX, to);
        downPick = channelGripHeight(from0);
        downPut = channelGripHeight(to0);
        if (downPick == 0)
            downPick = downPut;
        if (downPut == 0)
            downPut = downPick;
    } else if (job.kind == JobGripChToTray) {
        const int channel0 = job.channel - 1;
        SingletonAxis::chnZoneAxisPos(READ_OPERRAT, static_cast<quint8>(channel0), MOTOR_HANDS_INDEX, from);
        SingletonAxis::testTaryZoneAxisPos(READ_OPERRAT, static_cast<quint8>(job.trayHole),
                                           MOTOR_HANDS_INDEX, to);
        downPick = channelGripHeight(channel0);
        downPut = trayGripHeight(job.trayHole);
        if (downPick == 0)
            downPick = downPut;
        if (downPut == 0)
            downPut = downPick;
    } else {
        return true;
    }

    if (!usablePoint(from) || !usablePoint(to) || downPick == 0 || downPut == 0) {
        if (reason)
            *reason = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
        return false;
    }

    const bool ok = m_placing
            ? buildGripLeg(to, downPut, false, out)
            : buildGripLeg(from, downPick, true, out);
    if (!ok && reason)
        *reason = u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88");
    return ok;
}

QString QcStartAction::jobText(const Job &job) const
{
    switch (job.kind) {
    case JobSuck:
        if (job.record == RecWater)
            return u8("\xe5\x90\xb8\xe6\xb0\xb4 1\xe5\x8f\xb7\xe5\xad\x94");
        if (job.record == RecHighStd)
            return u8("\xe5\x90\xb8\xe9\xab\x98\xe5\x80\xbc\xe6\xa0\x87\xe5\x87\x86 3\xe5\x8f\xb7\xe5\xad\x94");
        if (job.record == RecHigh)
            return u8("\xe5\x90\xb8\xe9\xab\x98\xe5\x80\xbc 4\xe5\x8f\xb7\xe5\xad\x94");
        if (job.record == RecMidStd)
            return u8("\xe5\x90\xb8\xe4\xb8\xad\xe5\x80\xbc\xe6\xa0\x87\xe5\x87\x86 5\xe5\x8f\xb7\xe5\xad\x94");
        if (job.record == RecMid)
            return u8("\xe5\x90\xb8\xe4\xb8\xad\xe5\x80\xbc 6\xe5\x8f\xb7\xe5\xad\x94");
        if (job.record == RecLowStd)
            return u8("\xe5\x90\xb8\xe4\xbd\x8e\xe5\x80\xbc\xe6\xa0\x87\xe5\x87\x86 7\xe5\x8f\xb7\xe5\xad\x94");
        return u8("\xe5\x90\xb8\xe4\xbd\x8e\xe5\x80\xbc 8\xe5\x8f\xb7\xe5\xad\x94");
    case JobGripTrayToCh:
    case JobGripChToCh:
        return channelName(job.kind == JobGripChToCh ? job.channelTo : job.channel)
                + u8(" \xe6\x94\xbe\xe5\x85\xa5") + recordName(job.record);
    case JobGripChToTray:
        return recordName(job.record) + u8(" \xe6\x94\xbe\xe5\x9b\x9e\xe8\xaf\x95\xe6\x9d\xaf");
    case JobWash:
        return u8("\xe6\xb8\x85\xe6\xb4\x97\xe8\xa1\x80\xe6\xa0\xb7\xe9\x92\x88");
    case JobThrow:
        return channelName(job.channel) + u8(" \xe4\xb8\xa2\xe6\x9d\xaf");
    case JobRead:
        return channelName(job.channel) + QLatin1Char(' ') + u8("\xe8\xaf\xbb") + recordName(job.record);
    }
    return QString();
}

void QcStartAction::storeReading(int channel1, int record, int value)
{
    Reading reading = m_readings.value(channel1);
    switch (record) {
    case RecWater: reading.water = value; break;
    case RecHighStd: reading.highStd = value; break;
    case RecHigh: reading.high = value; break;
    case RecMidStd: reading.midStd = value; break;
    case RecMid: reading.mid = value; break;
    case RecLowStd: reading.lowStd = value; break;
    case RecLow: reading.low = value; break;
    default: break;
    }
    m_readings.insert(channel1, reading);
}

void QcStartAction::setSpin(int channel0, bool on)
{
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    if (mw && channel0 >= 0)
        mw->handleControlChannelRevolve(static_cast<quint8>(channel0), on);
}

bool QcStartAction::start(const QList<int> &levels, const QList<int> &channels,
                           const QString &highLot, const QString &midLot, const QString &lowLot,
                           QString *reason)
{
    ensureWired();
    if (m_running) {
        if (reason)
            *reason = u8("\xe8\xb4\xa8\xe6\x8e\xa7\xe8\xbf\x9b\xe8\xa1\x8c\xe4\xb8\xad");
        return false;
    }
    if (cglobal::g_StartTesting.load()) {
        if (reason)
            *reason = u8("\xe4\xbb\xaa\xe5\x99\xa8\xe6\xad\xa3\xe5\x9c\xa8\xe6\xb5\x8b\xe8\xaf\x95");
        return false;
    }
    if (Performanceverification::instance()->returnPerformanceTestFlag()) {
        if (reason)
            *reason = u8("\xe6\x80\xa7\xe8\x83\xbd\xe9\xaa\x8c\xe8\xaf\x81\xe8\xbf\x9b\xe8\xa1\x8c\xe4\xb8\xad");
        return false;
    }
    if (!m_wired) {
        if (reason)
            *reason = u8("\xe4\xb8\xb2\xe5\x8f\xa3\xe6\x9c\xaa\xe5\xb0\xb1\xe7\xbb\xaa");
        return false;
    }
    bool levelOk = false;
    for (int i = 0; i < levels.size(); ++i) {
        const int level = levels.at(i);
        if (level >= 0 && level <= 3)
            levelOk = true;
    }
    if (!levelOk) {
        if (reason)
            *reason = u8("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe8\xb4\xa8\xe6\x8e\xa7\xe5\x80\xbc");
        return false;
    }

    m_channels.clear();
    for (int i = 0; i < channels.size(); ++i) {
        const int ch = channels.at(i);
        if (ch >= 1 && ch <= MACHINE_SETTING_CHANNEL && !m_channels.contains(ch))
            m_channels.append(ch);
    }
    std::sort(m_channels.begin(), m_channels.end());
    if (m_channels.isEmpty()) {
        if (reason)
            *reason = u8("\xe8\xaf\xb7\xe5\x85\x88\xe5\x8b\xbe\xe9\x80\x89\xe6\xb5\x8b\xe8\xaf\x95\xe9\x80\x9a\xe9\x81\x93");
        return false;
    }

    m_claimed.clear();
    m_readings.clear();
    QString buildReason;
    if (!buildJobs(levels, &buildReason)) {
        releaseAllClaimed();
        if (reason)
            *reason = buildReason;
        return false;
    }

    for (int i = 0; i < m_jobs.size(); ++i) {
        if (m_jobs.at(i).kind == JobRead)
            continue;
        QByteArrayList probe;
        QString cmdReason;
        if (!commandForJob(m_jobs.at(i), probe, &cmdReason)) {
            releaseAllClaimed();
            m_jobs.clear();
            if (reason)
                *reason = cmdReason;
            return false;
        }
    }

    m_jobIndex = 0;
    m_placing = false;
    m_grabOk = false;
    m_regrab = false;
    m_promptingRegrab = false;
    m_aborting = false;
    m_stepwise = false;
    m_grabDeepenTimes = 0;
    m_grabJudgeIndex = 255;
    m_grabOpenIndex = 255;
    m_grabLiftIndex = 255;
    m_pending.clear();
    m_abortCmds.clear();
    m_highLot = highLot.trimmed();
    m_midLot = midLot.trimmed();
    m_lowLot = lowLot.trimmed();
    cglobal::g_quality_start.store(true);
    m_running = true;
    for (int i = 0; i < m_channels.size(); ++i)
        setSpin(m_channels.at(i) - 1, true);
    QLOG_INFO() << "QC start levels" << levels << "channels" << m_channels.size();
    emit progressText(u8("\xe8\xb4\xa8\xe6\x8e\xa7\xe8\xbf\x9b\xe8\xa1\x8c\xe4\xb8\xad"));
    runJob();
    return true;
}

void QcStartAction::runJob()
{
    if (!m_running || m_jobIndex < 0 || m_jobIndex >= m_jobs.size()) {
        succeed();
        return;
    }
    const Job job = m_jobs.at(m_jobIndex);
    emit progressText(jobText(job));
    QLOG_DEBUG() << "QC step" << m_jobIndex << job.kind << "channel" << job.channel;

    if (job.kind == JobRead) {
        MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
        if (!mw) {
            fail(u8("\xe7\xad\x89\xe5\xbe\x85\xe7\xa8\xb3\xe5\xae\x9a\xe8\xb6\x85\xe6\x97\xb6"));
            return;
        }
        m_waitChannel1 = job.channel;
        m_waitChannel0 = job.channel - 1;
        m_waitRecord = job.record;
        m_waitMs = 0;
        m_minSeq = mw->getChannelPositiveFilterSeq(static_cast<quint8>(m_waitChannel1));
        setSpin(m_waitChannel0, true);
        m_phase = PhaseStable;
        m_waitTimer->start();
        return;
    }

    QByteArrayList cmds;
    QString cmdReason;
    if (!commandForJob(job, cmds, &cmdReason)) {
        fail(cmdReason);
        return;
    }
    m_batchIndexes.clear();
    for (int i = 0; i < cmds.size(); ++i) {
        if (cmds.at(i).size() > GET_COMMAND_INDEX)
            m_batchIndexes.append(static_cast<quint8>(cmds.at(i).at(GET_COMMAND_INDEX)));
    }
    if (m_batchIndexes.isEmpty()) {
        fail(u8("\xe5\x9d\x90\xe6\xa0\x87\xe6\x97\xa0\xe6\x95\x88"));
        return;
    }
    const bool pickStep = gripJob(job.kind) && !m_placing;
    if (pickStep) {
        m_pending = cmds;
        m_grabDeepenTimes = 0;
        sendPendingAt(0);
        return;
    }
    m_stepwise = false;
    m_pending.clear();
    m_expectIndex = m_batchIndexes.last();
    m_phase = PhaseAck;
    emit sendOrderList(cmds, QStringLiteral("QC"));
}

void QcStartAction::advance()
{
    if (!m_running)
        return;
    if (m_jobIndex >= 0 && m_jobIndex < m_jobs.size()) {
        const Job done = m_jobs.at(m_jobIndex);
        if (done.kind == JobGripTrayToCh)
            markCupUsed(done.trayHole);
    }
    m_placing = false;
    m_grabOk = false;
    m_regrab = false;
    m_grabJudgeIndex = 255;
    ++m_jobIndex;
    if (m_jobIndex >= m_jobs.size())
        succeed();
    else
        runJob();
}

void QcStartAction::fail(const QString &text)
{
    m_waitTimer->stop();
    m_promptingRegrab = false;
    m_aborting = false;
    m_stepwise = false;
    m_pending.clear();
    m_abortCmds.clear();
    m_placing = false;
    m_grabJudgeIndex = 255;
    m_grabOpenIndex = 255;
    m_grabLiftIndex = 255;
    for (int i = 0; i < m_channels.size(); ++i)
        setSpin(m_channels.at(i) - 1, false);
    m_phase = PhaseIdle;
    m_running = false;
    cglobal::g_quality_start.store(false);
    QLOG_WARN() << "QC failed" << text;
    emit progressText(text);
    emit finished(false, text);
}

void QcStartAction::succeed()
{
    m_waitTimer->stop();
    for (int i = 0; i < m_channels.size(); ++i)
        setSpin(m_channels.at(i) - 1, false);
    m_phase = PhaseIdle;
    m_running = false;
    cglobal::g_quality_start.store(false);

    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    const QString today = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    QStringList lines;
    for (QMap<int, Reading>::const_iterator it = m_readings.cbegin(); it != m_readings.cend(); ++it) {
        const Reading &reading = it.value();
        QLOG_INFO() << "QC channel" << it.key()
                    << "water" << reading.water
                    << "highStd" << reading.highStd
                    << "high" << reading.high
                    << "midStd" << reading.midStd
                    << "mid" << reading.mid
                    << "lowStd" << reading.lowStd
                    << "low" << reading.low;
        const double water = reading.water;
        double highPct = 0.0;
        double midPct = 0.0;
        double lowPct = 0.0;
        const bool hasHigh = reading.high > 0 && reading.highStd > 0
                && qcRatioPercent(reading.high, reading.highStd, water, highPct);
        const bool hasMid = reading.mid > 0 && reading.midStd > 0
                && qcRatioPercent(reading.mid, reading.midStd, water, midPct);
        const bool hasLow = reading.low > 0 && reading.lowStd > 0
                && qcRatioPercent(reading.low, reading.lowStd, water, lowPct);
        if ((hasHigh || hasMid || hasLow) && sql) {
            const QString now = QTime::currentTime().toString(QStringLiteral("HH:mm:ss"));
            if (hasHigh && !sql->insertQcRun(it.key(), today, now, 0, highPct, m_highLot))
                QLOG_WARN() << "QC run save failed channel" << it.key() << "high";
            if (hasMid && !sql->insertQcRun(it.key(), today, now, 1, midPct, m_midLot))
                QLOG_WARN() << "QC run save failed channel" << it.key() << "mid";
            if (hasLow && !sql->insertQcRun(it.key(), today, now, 2, lowPct, m_lowLot))
                QLOG_WARN() << "QC run save failed channel" << it.key() << "low";
        }
        QStringList parts;
        parts.append(channelName(it.key()));
        if (hasHigh) {
            QString item = QLatin1String("  ") + recordName(RecHigh) + QLatin1Char(' ')
                    + QString::number(highPct, 'f', 1) + QLatin1Char('%');
            if (!m_highLot.isEmpty())
                item += QLatin1String("  ") + u8("\xe6\x89\xb9\xe5\x8f\xb7 ") + m_highLot;
            parts.append(item);
        }
        if (hasMid) {
            QString item = QLatin1String("  ") + recordName(RecMid) + QLatin1Char(' ')
                    + QString::number(midPct, 'f', 1) + QLatin1Char('%');
            if (!m_midLot.isEmpty())
                item += QLatin1String("  ") + u8("\xe6\x89\xb9\xe5\x8f\xb7 ") + m_midLot;
            parts.append(item);
        }
        if (hasLow) {
            QString item = QLatin1String("  ") + recordName(RecLow) + QLatin1Char(' ')
                    + QString::number(lowPct, 'f', 1) + QLatin1Char('%');
            if (!m_lowLot.isEmpty())
                item += QLatin1String("  ") + u8("\xe6\x89\xb9\xe5\x8f\xb7 ") + m_lowLot;
            parts.append(item);
        }
        if (!hasHigh && !hasMid && !hasLow)
            parts.append(QLatin1String("  ") + u8("\xe6\x97\xa0\xe6\xb3\x95\xe8\xae\xa1\xe7\xae\x97"));
        const QString line = parts.join(QLatin1Char('\n'));
        QLOG_INFO() << "QC result" << line;
        lines.append(line);
    }
    const QString done = lines.isEmpty()
            ? u8("\xe8\xb4\xa8\xe6\x8e\xa7\xe5\xae\x8c\xe6\x88\x90")
            : lines.join(QStringLiteral("\n\n"));
    emit progressText(done);
    emit finished(true, done);
}

void QcStartAction::onSerial(const QStringList &recv)
{
    if (!m_running || m_promptingRegrab)
        return;

    int commandMode = 0;
    int runStatus = 0;
    int speedMode = 0;
    int motor = 0;
    GlobalData::Parse4thBytecommandstatus(recv, commandMode, runStatus, speedMode, motor);
    const quint8 index = GlobalData::analyzeCommandIndex(recv);
    if (runStatus == MOTOR_WAITING || runStatus == MOTOR_RUNNING)
        return;

    if (m_aborting) {
        if (m_phase != PhaseAck || index != m_expectIndex)
            return;
        QString err;
        if (!GlobalData::commandStatus(runStatus, err)) {
            QLOG_WARN() << "QC abort step failed" << err;
            fail(u8("\xe5\xa4\x8d\xe4\xbd\x8d\xe5\xa4\xb1\xe8\xb4\xa5 ") + err);
            return;
        }
        sendAbortAt(m_abortPos + 1);
        return;
    }

    if (m_phase != PhaseAck || !m_batchIndexes.contains(index))
        return;
    const bool picking = m_jobIndex >= 0 && m_jobIndex < m_jobs.size()
            && gripJob(m_jobs.at(m_jobIndex).kind) && !m_placing;
    QString err;
    if (!GlobalData::commandStatus(runStatus, err)) {
        if (picking)
            requestRegrabPrompt();
        else
            fail(u8("\xe5\x8a\xa8\xe4\xbd\x9c\xe5\xbc\x82\xe5\xb8\xb8 ") + err);
        return;
    }

    if (m_stepwise && picking) {
        if (index == m_grabOpenIndex) {
            m_phase = PhaseIdle;
            const int waitMs = GlobalData::gripperGrabDelayMs();
            const int openPos = m_pendingPos;
            QLOG_INFO() << "QC grab vacuum open, wait" << waitMs;
            QTimer::singleShot(waitMs, this, [this, openPos]() {
                if (!m_running || m_aborting || m_promptingRegrab || !m_stepwise)
                    return;
                if (m_pendingPos != openPos)
                    return;
                sendPendingAt(openPos + 1);
            });
            return;
        }
        if (index == m_grabJudgeIndex) {
            bool suction = false;
            quint32 air = 0;
            GlobalData::analyzeWhetherGripperSuction(recv, suction, air);
            const bool held = GlobalData::isHandsVacuumStillSealed(air);
            QLOG_INFO() << "QC grab judge" << index << "air" << air
                        << "held" << held << "deepen" << m_grabDeepenTimes;
            if (!held) {
                if (m_grabDeepenTimes >= 3) {
                    QLOG_WARN() << "QC grab deepen exhausted";
                    requestRegrabPrompt();
                    return;
                }
                ++m_grabDeepenTimes;
                const QByteArray deepen = buildGrabDeepen(index);
                if (deepen.isEmpty()) {
                    requestRegrabPrompt();
                    return;
                }
                QLOG_WARN() << "QC grab deepen" << m_grabDeepenTimes << "mm, pump stays on";
                sendOne(deepen, QStringLiteral("QC grab deepen"));
                return;
            }
            m_grabDeepenTimes = 0;
            m_grabOk = true;
            sendPendingAt(m_pendingPos + 1);
            return;
        }
        if (index == m_grabLiftIndex) {
            bool suction = false;
            quint32 air = 0;
            GlobalData::analyzeWhetherGripperSuction(recv, suction, air);
            const bool held = GlobalData::isHandsLiftStillHolding(air, suction);
            QLOG_INFO() << "QC grab lift" << index << "air" << air << "held" << held;
            if (!held) {
                requestRegrabPrompt();
                return;
            }
            m_placing = true;
            m_regrab = false;
            m_grabJudgeIndex = 255;
            m_grabOpenIndex = 255;
            m_grabLiftIndex = 255;
            m_stepwise = false;
            runJob();
            return;
        }
        sendPendingAt(m_pendingPos + 1);
        return;
    }

    if (index != m_expectIndex)
        return;
    m_phase = PhaseIdle;
    advance();
}

void QcStartAction::onWaitTick()
{
    if (!m_running || m_phase != PhaseStable)
        return;
    m_waitMs += m_waitTimer->interval();
    MainWindow *mw = FullyAutomatedPlatelets::mainWindow();
    int value = 0;
    quint32 seq = 0;
    const bool stable = mw && mw->getStablePositiveChannelVal(
                static_cast<quint8>(m_waitChannel1), m_minSeq, value, seq);
    if (!stable && m_waitMs < kStableTimeoutMs)
        return;

    m_waitTimer->stop();
    storeReading(m_waitChannel1, m_waitRecord, value);
    QString text = channelName(m_waitChannel1) + QLatin1Char(' ')
            + recordName(m_waitRecord) + QLatin1Char(' ') + QString::number(value);
    if (!stable) {
        text += u8(" \xe7\xad\x89\xe5\xbe\x85\xe7\xa8\xb3\xe5\xae\x9a\xe8\xb6\x85\xe6\x97\xb6");
        QLOG_WARN() << "QC stable timeout channel" << m_waitChannel1 << "value" << value;
    } else {
        QLOG_INFO() << "QC stable channel" << m_waitChannel1
                    << "record" << m_waitRecord << "value" << value;
    }
    emit progressText(text);
    m_phase = PhaseIdle;
    advance();
}
