#ifdef _MSC_VER
#pragma warning(disable: 4819)
#pragma execution_character_set("utf-8")
#endif
#include "machinesetting.h"
#include "ui_machinesetting.h"
#include "testing.h"
#include "loadequipmentpos.h"
#include "usbcodedispose.h"
#include "globaldata.h"
#include "quiutils.h"
#include "cglobal.h"
#include "custom_struct.h"

#include <QMessageBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QSizePolicy>
#include <QTimer>
#include <QVector>
#include <QFrame>
#include <QGroupBox>
#include <QScrollArea>
#include <QAbstractSpinBox>
#include <QPainter>
#include <QMouseEvent>
#include <QCheckBox>
#include <QTabBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLayoutItem>
#include <QApplication>
#include <QDesktopWidget>
#include <algorithm>

namespace {

const int kGripperAirStableCount = 5;
const int kGripperAirOriginStableCount = 3;
const int kGripperAirMaxSpread = 120;
const int kGripperAirOriginMaxSpread = 100;
const int kGripperAirPhaseMinMs = 600;
const int kGripperAirHoldDeltaMin = 80;
const int kGripperAirHoldCommitDelta = 250;
const int kGripperAirGrabGiveUpMs = 15000;
const int kGripperAirDropMin = 150;
const int kGripperAirReleaseTimeoutMs = 45000;
const int kGripperAirDumpMaxRetry = 2;
const int kGripperAirEmptyMatchMax = 100;
const int kGripperAirOriginSettleMs = 800;
const int kGripperAirObserveMinMs = 200;

int clampGripperAutoDelay(int elapsedMs)
{
    int withMargin = elapsedMs + 200;
    if (withMargin < 0) {
        withMargin = 0;
    }
    int rounded = ((withMargin + 49) / 50) * 50;
    if (rounded < 300) {
        rounded = 300;
    }
    if (rounded > 8000) {
        rounded = 8000;
    }
    return rounded;
}
const int kGripperAirThrowSettleMs = 3500;
const int kGripperGrabApplySlack = 60;
const int kGripperReleaseApplySlack = 60;
const int kGripperThresholdMinGap = 80;
const quint8 kGripperAirCalibChannel = 0;

const int kGripperMotionOrigin = 1;
const int kGripperMotionChannel = 2;
const int kGripperMotionDump = 3;
const int kGripperMotionReturn = 4;
const int kGripperMotionDumpRetreat = 5;
const int kGripperMotionCancel = 6;
const int kGripperAirVacuumOffBeforeMoveMs = 700;

// detect step: 1=origin empty(suck off)  2=ch1 hold(suck on)  3=throw release(suck off)

quint8 nextGripperCmdIndex(const QByteArrayList &cmds)
{
    return static_cast<quint8>(cmds.size() % 255);
}

quint8 cancelHomingCmdIndex(const QByteArrayList &cmds)
{
    return static_cast<quint8>((40 + cmds.size()) % 255);
}

void appendXYReposition(QByteArrayList &cmds, UsbCodeDispose *pActive)
{
    quint8 idx = nextGripperCmdIndex(cmds);
    const QByteArray repos = pActive->_DLLXYMoveReposition_(idx, MACHINEBACK_SPEED);
    if (!repos.isEmpty()) {
        cmds.push_back(repos);
    }
}

static inline QString gtr(const char *utf8Text)
{
    return QString::fromUtf8(utf8Text);
}

bool computeGripperAirThresholds(int holdSample, int emptyOrigin, int emptyAfterDump,
                                  int *grabLeq, int *releaseGeq, QString *err)
{
    if (!grabLeq || !releaseGeq) {
        return false;
    }
    if (holdSample < 0 || emptyAfterDump < 0) {
        if (err) {
            *err = QStringLiteral("missing hold or post-dump empty sample");
        }
        return false;
    }
    const int emptyRef = emptyOrigin >= 0 ? (emptyOrigin + emptyAfterDump) / 2 : emptyAfterDump;
    if (holdSample >= emptyRef) {
        if (err) {
            *err = QStringLiteral("hold sample should be lower than empty reference");
        }
        return false;
    }

    const int mid = holdSample + (emptyRef - holdSample) / 2;
    const int span = emptyRef - holdSample;
    // Grab line sits about one third of the way from the hold reading toward empty, and at least +60.
    // A fixed +60 (and the old pull back onto the hold average) made one live test sample fail while the cup was sealed.
    int grab = holdSample + qMax(kGripperGrabApplySlack, span / 3);
    int release = emptyAfterDump - kGripperReleaseApplySlack;

    if (grab >= mid) {
        grab = qMax(holdSample + kGripperGrabApplySlack, mid - kGripperThresholdMinGap / 2);
    }
    if (release <= mid) {
        release = mid + kGripperThresholdMinGap / 2;
    }
    if (grab >= release) {
        grab = holdSample + kGripperGrabApplySlack;
        release = qMax(emptyAfterDump, grab + kGripperThresholdMinGap);
    }
    if (grab >= release) {
        if (err) {
            *err = QStringLiteral("cannot split grab/release thresholds");
        }
        return false;
    }

    *grabLeq = grab;
    *releaseGeq = release;
    return true;
}

bool gripperAirPushSample(QVector<int> &samples, int value, int *outAvg,
                          int stableCount = kGripperAirStableCount,
                          int maxSpread = kGripperAirMaxSpread)
{
    if (stableCount < 1) {
        return false;
    }
    samples.push_back(value);
    while (samples.size() > stableCount) {
        samples.remove(0);
    }
    if (samples.size() < stableCount) {
        return false;
    }
    const int minVal = *std::min_element(samples.cbegin(), samples.cend());
    const int maxVal = *std::max_element(samples.cbegin(), samples.cend());
    if (maxVal - minVal > maxSpread) {
        return false;
    }
    int sum = 0;
    for (int v : samples) {
        sum += v;
    }
    *outAvg = sum / stableCount;
    return true;
}

} // namespace

void MachineSetting::setupGripperVacuumSimpleUi()
{
    if (m_gripperAirUiReady) {
        return;
    }
    m_gripperAirUiReady = true;

    QGridLayout *grid = ui->gridLayout_8;
    if (grid) {
        grid->setVerticalSpacing(8);
        grid->setHorizontalSpacing(10);
        grid->setContentsMargins(12, 22, 12, 10);
        for (int col = 0; col < 4; ++col) {
            grid->setColumnStretch(col, 1);
        }
    }

    ui->groupBox_Airs->setTitle(gtr(u8"抓手放杯判定"));
    ui->label_2->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->label_8->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->labelsuckLateTimer->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->label_2->setText(gtr(u8"抓住判定: 负压 <="));
    ui->label_8->setText(gtr(u8"丢掉判定: 负压 >="));
    ui->labelsuckLateTimer->setText(gtr(u8"吸杯判定延时(ms)"));

    const QString gripperSpinStyle = m_setspinBoxQss + QStringLiteral(
                "\nQSpinBox#gripperThresholdSpin { padding-right: 8px; }"
                "\nQSpinBox#gripperThresholdSpin::up-button,"
                "\nQSpinBox#gripperThresholdSpin::down-button {"
                " width: 0; height: 0; margin: 0; border: none; }");
    const auto styleThresholdSpin = [&](QSpinBox *box) {
        if (!box) {
            return;
        }
        box->setObjectName(QStringLiteral("gripperThresholdSpin"));
        box->setButtonSymbols(QAbstractSpinBox::NoButtons);
        box->setReadOnly(false);
        box->setAlignment(Qt::AlignCenter);
        box->setMinimumHeight(32);
        box->setMaximumWidth(QWIDGETSIZE_MAX);
        box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        box->setStyleSheet(gripperSpinStyle);
    };
    styleThresholdSpin(ui->spinBoxSuckAirsMax);
    styleThresholdSpin(ui->spinBoxSplitAirsMin);
    styleThresholdSpin(ui->spinBox_suckLatetimer);

    const QString liveValStyle = QStringLiteral(
                "QLabel { background:transparent; border:none;"
                " padding:4px 8px; font-size:13px; color:#2C5F8A; }");
    ui->label_showsuckVal->setStyleSheet(liveValStyle);
    ui->label_splitval->setStyleSheet(liveValStyle);
    ui->label_showsuckVal->setMinimumHeight(32);
    ui->label_splitval->setMinimumHeight(32);
    ui->label_showsuckVal->setText(gtr(u8"吸住 -"));
    ui->label_splitval->setText(gtr(u8"空载 -"));

    ui->pushButtonopenSuck->setText(gtr(u8"(1) 仪器主动判定"));
    ui->pushButtonsplitAirs->setText(gtr(u8"取消判定"));
    ui->pushButtonopenSuck->setMinimumHeight(40);
    ui->pushButtonsplitAirs->setMinimumHeight(40);
    if (!m_settButtonQss.isEmpty()) {
        ui->pushButtonopenSuck->setStyleSheet(m_settButtonQss);
        ui->pushButtonsplitAirs->setStyleSheet(m_settButtonQss);
    }

    const auto gridReAdd = [grid](QWidget *w, int row, int col, int rowSpan = 1, int colSpan = 1) {
        if (!w || !grid) {
            return;
        }
        grid->removeWidget(w);
        grid->addWidget(w, row, col, rowSpan, colSpan);
    };
    if (grid) {
        gridReAdd(ui->pushButtonopenSuck, 0, 0, 1, 2);
        gridReAdd(ui->pushButtonsplitAirs, 0, 2, 1, 2);
        gridReAdd(ui->label_showsuckVal, 1, 0, 1, 2);
        gridReAdd(ui->label_splitval, 1, 2, 1, 2);
        gridReAdd(ui->label_2, 2, 0);
        gridReAdd(ui->spinBoxSuckAirsMax, 2, 1, 1, 3);
        gridReAdd(ui->label_8, 3, 0);
        gridReAdd(ui->spinBoxSplitAirsMin, 3, 1, 1, 3);
        gridReAdd(ui->labelsuckLateTimer, 4, 0);
        gridReAdd(ui->spinBox_suckLatetimer, 4, 1, 1, 3);
        if (!m_gripperThrowDelayLabel) {
            m_gripperThrowDelayLabel = new QLabel(ui->groupBox_Airs);
            m_gripperThrowDelayLabel->setText(gtr(u8"丢杯判定延时(ms)"));
        }
        if (!m_gripperThrowDelaySpin) {
            m_gripperThrowDelaySpin = new QSpinBox(ui->groupBox_Airs);
            m_gripperThrowDelaySpin->setRange(300, 10000);
            m_gripperThrowDelaySpin->setSingleStep(10);
            m_gripperThrowDelaySpin->setValue(1200);
            styleThresholdSpin(m_gripperThrowDelaySpin);
        }
        gridReAdd(m_gripperThrowDelayLabel, 5, 0);
        gridReAdd(m_gripperThrowDelaySpin, 5, 1, 1, 3);
        if (ui->horizontalSpacer_23) {
            grid->removeItem(ui->horizontalSpacer_23);
        }
    }
    if (ui->checkBox_Recapture) {
        ui->checkBox_Recapture->hide();
        if (grid) {
            grid->removeWidget(ui->checkBox_Recapture);
        }
    }

    if (!m_gripperAirStepLabel) {
        m_gripperAirStepLabel = new QLabel(ui->groupBox_Airs);
        m_gripperAirStepLabel->setWordWrap(true);
        m_gripperAirStepLabel->setTextFormat(Qt::RichText);
        m_gripperAirStepLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
        m_gripperAirStepLabel->setStyleSheet(QStringLiteral(
                    "QLabel { background:transparent; border:none;"
                    " padding:8px 10px; font-size:12px; color:#2C5F8A; }"));
    }
    if (grid) {
        gridReAdd(m_gripperAirStepLabel, 6, 0, 1, 4);
    }

    if (ui->verticalSpacer_2) {
        ui->verticalSpacer_2->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    if (!m_gripperAutoApplyBtn) {
        m_gripperAutoApplyBtn = new QPushButton(gtr(u8"(4) 自动设定"), ui->groupBox_Airs);
        connect(m_gripperAutoApplyBtn, &QPushButton::clicked, this, [this]() {
            askGripperAirApply();
        });
    }
    m_gripperAutoApplyBtn->setMinimumHeight(40);
    if (!m_settButtonQss.isEmpty()) {
        m_gripperAutoApplyBtn->setStyleSheet(m_settButtonQss);
    }
    if (grid) {
        gridReAdd(m_gripperAutoApplyBtn, 7, 0, 1, 4);
    }

    updateGripperAirStepHint(0);
}

void MachineSetting::updateGripperAirStepHint(int step)
{
    if (!m_gripperAirStepLabel) {
        return;
    }
    const QString step1 = gtr(u8"(1) 点[仪器主动判定], 在原点关泵读稳空载");
    const QString step2 = gtr(u8"(2) 通道1开负压后一直读, 记下吸住要多久");
    const QString step3 = gtr(u8"(3) 弃杯孔关负压后先不抬, 记下泄压要多久");
    const QString step4 = gtr(u8"(4) 点[自动设定], 再点本页[保存抓手参数]");
    QString s1 = step1;
    QString s2 = step2;
    QString s3 = step3;
    QString s4 = step4;
    QString now;
    if (step == 1) {
        s1 = QStringLiteral("<b style=\"color:#C0392B;\">") + step1 + QStringLiteral("</b>");
        now = gtr(u8"当前: 原点关泵, 连续稳定读数作为空载, 不用旧的抓住线。请勿放试杯。");
    } else if (step == 2) {
        s2 = QStringLiteral("<b style=\"color:#C0392B;\">") + step2 + QStringLiteral("</b>");
        now = gtr(u8"当前: 通道1已开负压。从落下完成开始计时, 读数吸住后再取稳定值。");
    } else if (step == 22) {
        s1 = QStringLiteral("<b style=\"color:#C0392B;\">") + step1 + QStringLiteral("</b>");
        now = gtr(u8"当前: 原点空载已读取, 前往通道1吸杯...");
    } else if (step == 3) {
        s3 = QStringLiteral("<b style=\"color:#C0392B;\">") + step3 + QStringLiteral("</b>");
        now = gtr(u8"当前: 弃杯位已关负压, 先不抬。从关泵完成开始计时, 读数回到空载附近才抬。");
    } else if (step == 4) {
        s4 = QStringLiteral("<b style=\"color:#C0392B;\">") + step4 + QStringLiteral("</b>");
        now = gtr(u8"当前: 负压线和吸杯、弃杯延时已填入。确认后点保存抓手参数。");
    } else if (step == 0) {
        now = gtr(u8"可手动修改上方阈值, 或按上方流程操作。");
    }
    m_gripperAirStepLabel->setText(
                QStringLiteral("<p>%1<br/>%2<br/>%3<br/>%4</p><p>%5</p>")
                .arg(s1, s2, s3, s4, now));
}

void MachineSetting::sendGripperVacuumProbe(bool suckOn)
{
    UsbCodeDispose *taskDll = Testing::getTaskDll();
    if (!taskDll) {
        return;
    }
    quint8 cmdIndex = suckOn ? static_cast<quint8>(HANDS_SUCK_NUM)
                             : static_cast<quint8>(HANDS_SPLIT_NUM);
    const int downMm = m_gripperAirProbeHeight > 0 ? m_gripperAirProbeHeight : 0;
    const bool gripperMode = suckOn ? GRABCUPWITHGRIPPER : GRIPPERNORMAL;
    const QByteArray cmd = QUIUtils::withGripperPump(
                taskDll->DLL_ZMoveSpecifiedPosition(
                MOTOR_HANDS_INDEX, downMm, 0, cmdIndex, false, downMm, false, gripperMode),
                suckOn ? GRIPPERMOVE_ABNORMALLY : GRIPPERMOVE_NORMAL);
    emit WriteArryCommand(cmd, suckOn ? gtr(u8"打开负压") : gtr(u8"关闭负压"));
}

void MachineSetting::startGripperAirPoll(bool hold)
{
    if (m_gripperAirAwaitingMotion) {
        return;
    }
    if (cglobal::g_StartTesting.load()) {
        QMessageBox::information(this, gtr(u8"运行暂停"),
                                 gtr(u8"测试中不能开启抓手负压。"));
        return;
    }
    if (!cglobal::gserialConnecStatus.load()) {
        QMessageBox::information(this, gtr(u8"未连接"),
                                 gtr(u8"请先连接仪器。"));
        return;
    }

    m_gripperAirPollHold = hold;
    sendGripperVacuumProbe(hold);

    if (!m_gripperAirPollTimer) {
        m_gripperAirPollTimer = new QTimer(this);
        m_gripperAirPollTimer->setInterval(200);
        connect(m_gripperAirPollTimer, &QTimer::timeout, this, [this]() {
            if (m_gripperAirAwaitingMotion) {
                return;
            }
            if (m_gripperAirPollHold) {
                sendGripperVacuumProbe(true);
            } else {
                sendGripperVacuumProbe(false);
            }
        });
    }
    m_gripperAirPollTimer->setInterval(200);
    if (!m_gripperAirPollTimer->isActive()) {
        m_gripperAirPollTimer->start();
    }
}

void MachineSetting::stopGripperAirPoll()
{
    if (m_gripperAirPollTimer) {
        m_gripperAirPollTimer->stop();
    }
    m_gripperAirPollHold = false;
}

void MachineSetting::queueGripperAirMotion(const QByteArrayList &cmds, int motionKind)
{
    if (cmds.isEmpty()) {
        return;
    }
    stopGripperAirPoll();
    m_gripperAirMotionKind = motionKind;
    m_gripperAirAwaitingMotion = true;
    emit testdownheight(cmds, EQUIPMENT_GRIPPER_AIR_CALIB);
}

void MachineSetting::onGripperAirMotionFinished()
{
    if (!m_gripperAirAwaitingMotion || m_gripperAirDetectStep == 0) {
        return;
    }
    m_gripperAirAwaitingMotion = false;
    const int kind = m_gripperAirMotionKind;
    m_gripperAirMotionKind = 0;

    switch (kind) {
    case kGripperMotionOrigin:
        m_gripperAirSettleMs = 0;
        m_gripperAirPhaseClock.restart();
        m_gripperAirSamples.clear();
        sendGripperVacuumProbe(false);
        scheduleGripperOriginEmptyPollFallback();
        updateGripperAirStepHint(1);
        break;
    case kGripperMotionChannel:
        m_gripperAirSettleMs = kGripperAirObserveMinMs;
        m_gripperGrabSeenMs = -1;
        m_gripperAirPhaseClock.restart();
        startGripperAirPoll(true);
        updateGripperAirStepHint(2);
        break;
    case kGripperMotionDump:
        m_gripperAirSettleMs = kGripperAirObserveMinMs;
        m_gripperThrowSeenMs = -1;
        m_gripperAirPhaseClock.restart();
        m_gripperAirSamples.clear();
        startGripperAirPoll(false);
        updateGripperAirStepHint(3);
        break;
    case kGripperMotionDumpRetreat:
        sendGripperReturnOriginAfterDetect();
        break;
    case kGripperMotionReturn:
        finishGripperAirDetect();
        break;
    case kGripperMotionCancel:
        updateGripperAirStepHint(0);
        break;
    default:
        break;
    }
}

void MachineSetting::cancelGripperAirDetect()
{
    const bool wasActive = (m_gripperAirDetectStep != 0) || m_gripperAirPollHold || m_gripperAirAwaitingMotion;
    stopGripperAirPoll();
    if (m_gripperAirOriginAckFallbackTimer) {
        m_gripperAirOriginAckFallbackTimer->stop();
    }
    m_gripperAirAwaitingMotion = false;
    m_gripperAirMotionKind = 0;
    m_gripperAirProbeHeight = 0;
    m_gripperAirDetectStep = 0;
    m_gripperAirEmptyBaseline = -1;
    m_gripperAirReleaseAfterDump = -1;
    m_gripperAirLockedHold = -1;
    m_gripperGrabSeenMs = -1;
    m_gripperThrowSeenMs = -1;
    m_gripperGrabDelayMeasured = -1;
    m_gripperThrowDelayMeasured = -1;
    m_gripperAirDumpRetry = 0;
    m_gripperAirSettleMs = kGripperAirPhaseMinMs;
    m_gripperAirSamples.clear();
    updateGripperAirStepHint(0);
    if (wasActive && cglobal::gserialConnecStatus.load() && !cglobal::g_StartTesting.load()) {
        sendGripperCancelHoming();
    }
}

void MachineSetting::finishGripperAirDetect()
{
    stopGripperAirPoll();
    sendGripperVacuumProbe(false);
    m_gripperAirDetectStep = 0;
    m_gripperAirSamples.clear();
    askGripperAirApply();
}

void MachineSetting::askGripperAirApply()
{
    const int holdSample = m_gripperAirLockedHold;
    const int emptyOrigin = m_gripperAirEmptyBaseline;
    const int emptyDump = m_gripperAirReleaseAfterDump;

    int grabThreshold = -1;
    int releaseThreshold = -1;
    QString err;
    if (!computeGripperAirThresholds(holdSample, emptyOrigin, emptyDump,
                                     &grabThreshold, &releaseThreshold, &err)) {
        const int holdFallback = holdSample >= 0 ? holdSample : m_liveHoldAir;
        const int releaseFallback = emptyDump >= 0 ? emptyDump
                                                   : (m_liveReleaseAir >= 0 ? m_liveReleaseAir : emptyOrigin);
        if (holdFallback < 0 || releaseFallback < 0) {
            QMessageBox::information(
                        this, gtr(u8"尚无读数"),
                        gtr(u8"请先完成仪器主动判定全流程, 或手动开/关负压等待稳定读数。"));
            return;
        }
        if (holdFallback >= releaseFallback) {
            QMessageBox::warning(
                        this, gtr(u8"参数异常"),
                        gtr(u8"吸住实测 %1 应低于空载实测 %2, 请重新判定。")
                        .arg(holdFallback).arg(releaseFallback));
            return;
        }
        grabThreshold = holdFallback + kGripperGrabApplySlack;
        releaseThreshold = releaseFallback - kGripperReleaseApplySlack;
        if (grabThreshold >= releaseThreshold) {
            grabThreshold = holdFallback;
            releaseThreshold = releaseFallback;
        }
    }

    m_liveHoldAir = holdSample >= 0 ? holdSample : m_liveHoldAir;
    m_liveReleaseAir = emptyDump >= 0 ? emptyDump : m_liveReleaseAir;

    ui->spinBoxSuckAirsMax->setValue(grabThreshold);
    ui->spinBoxSplitAirsMin->setValue(releaseThreshold);
    if (m_gripperGrabDelayMeasured >= 300) {
        ui->spinBox_suckLatetimer->setValue(m_gripperGrabDelayMeasured);
    }
    if (m_gripperThrowDelaySpin && m_gripperThrowDelayMeasured >= 300) {
        m_gripperThrowDelaySpin->setValue(m_gripperThrowDelayMeasured);
    }
    updateGripperAirStepHint(4);

    const quint32 grabU = static_cast<quint32>(grabThreshold);
    const quint32 releaseU = static_cast<quint32>(releaseThreshold);
    const bool holdWouldGrab = holdSample >= 0
            && static_cast<quint32>(holdSample) <= grabU;
    const bool originNotGrab = emptyOrigin < 0
            || static_cast<quint32>(emptyOrigin) > grabU;
    const bool dumpWouldRelease = emptyDump >= 0
            && static_cast<quint32>(emptyDump) >= releaseU;
    QString verifyNote;
    if (!holdWouldGrab || !originNotGrab || !dumpWouldRelease) {
        verifyNote = gtr(u8"\n[自检] 与测试判据对照: 吸住=%1 原点非吸住=%2 弃杯空载=%3")
                .arg(holdWouldGrab ? gtr(u8"通过") : gtr(u8"异常"))
                .arg(originNotGrab ? gtr(u8"通过") : gtr(u8"异常"))
                .arg(dumpWouldRelease ? gtr(u8"通过") : gtr(u8"异常"));
    } else {
        verifyNote = gtr(u8"\n[自检] 三段实测与测试判据(<=抓住 / >=丢掉)一致。");
    }
    Q_UNUSED(verifyNote);
}

void MachineSetting::sendGripperOriginSequence()
{
    if (cglobal::g_StartTesting.load() || !cglobal::gserialConnecStatus.load()) {
        cancelGripperAirDetect();
        return;
    }
    UsbCodeDispose *pActive = Testing::getTaskDll();
    if (!pActive) {
        cancelGripperAirDetect();
        return;
    }

    QPoint origin(0, 0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT, origin);
    if (origin.x() == 0 && origin.y() == 0) {
        QMessageBox::warning(this, gtr(u8"下发失败"),
                             gtr(u8"原点坐标无效, 请先校准坐标。"));
        cancelGripperAirDetect();
        return;
    }

    m_gripperAirProbeHeight = 0;
    QByteArrayList cmds;
    QUIUtils::allZAxisBackOrigin(cmds, cmds.size());
    appendXYReposition(cmds, pActive);
    const int safeZ = INI_File()._gethandsdownthrowcpus();
    if (safeZ > 0) {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(QUIUtils::withGripperPump(
                    pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, safeZ,
                                                TRAY_TESTCHANNEL, idx, false),
                    GRIPPERMOVE_NORMAL));
    }
    {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(origin, 0, 0, idx));
    }
    queueGripperAirMotion(cmds, kGripperMotionOrigin);
}

void MachineSetting::sendGripperChannelGrabSequence()
{
    if (cglobal::g_StartTesting.load() || !cglobal::gserialConnecStatus.load()) {
        cancelGripperAirDetect();
        return;
    }
    UsbCodeDispose *pActive = Testing::getTaskDll();
    if (!pActive) {
        cancelGripperAirDetect();
        return;
    }

    QPoint chAxis(0, 0);
    SingletonAxis::chnZoneAxisPos(READ_OPERRAT, kGripperAirCalibChannel,
                                  MOTOR_HANDS_INDEX, chAxis);
    const int grabH = INI_File()._gethandsdownheiht(kGripperAirCalibChannel);
    if ((chAxis.x() == 0 && chAxis.y() == 0) || grabH <= 0) {
        QMessageBox::warning(this, gtr(u8"下发失败"),
                             gtr(u8"通道1抓手下降高度无效。"));
        cancelGripperAirDetect();
        return;
    }
    m_gripperAirProbeHeight = grabH;

    QByteArrayList cmds;
    QUIUtils::allZAxisBackOrigin(cmds, cmds.size());
    appendXYReposition(cmds, pActive);
    {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(chAxis, 0, 0, idx));
    }
    QVector<QByteArray> tail;
    tail.reserve(4);
    QUIUtils::appendGripperDescendThenOpenVacuum(tail, grabH, INI_File().rConfigPara(REPEATGRABCUP).toBool());
    for (const QByteArray &cmd : tail) {
        if (!cmd.isEmpty()) {
            cmds.push_back(cmd);
        }
    }
    queueGripperAirMotion(cmds, kGripperMotionChannel);
}

void MachineSetting::sendGripperDumpSequence()
{
    UsbCodeDispose *pActive = Testing::getTaskDll();
    if (!pActive) {
        cancelGripperAirDetect();
        return;
    }
    QPoint throwAxis(0, 0);
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT, throwAxis);
    const int throwH = INI_File()._gethandsdownthrowcpus();
    if (throwH <= 0 || (throwAxis.x() == 0 && throwAxis.y() == 0)) {
        QMessageBox::warning(this, gtr(u8"下发失败"),
                             gtr(u8"弃杯区坐标或高度无效。"));
        cancelGripperAirDetect();
        return;
    }

    QByteArrayList cmds;
    {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(QUIUtils::withGripperPump(
                    pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, throwH,
                                                TRAY_TESTCHANNEL, idx, GRABCUPWITHGRIPPER),
                    GRIPPERMOVE_ABNORMALLY));
    }
    {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(
                           throwAxis, OFFSET_ZHANDS, TRAY_TESTENPTY, idx));
    }
    {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(QUIUtils::withGripperPump(
                    pActive->DLL_ZMoveSpecifiedPosition(
                           MOTOR_HANDS_INDEX, throwH, 0, idx, false, throwH, false, GRIPPERNORMAL),
                    GRIPPERMOVE_NORMAL));
    }
    m_gripperAirProbeHeight = throwH;
    queueGripperAirMotion(cmds, kGripperMotionDump);
}

void MachineSetting::sendGripperDumpZRetreat()
{
    UsbCodeDispose *pActive = Testing::getTaskDll();
    if (!pActive) {
        cancelGripperAirDetect();
        return;
    }
    const int throwH = INI_File()._gethandsdownthrowcpus();
    if (throwH <= 0) {
        sendGripperReturnOriginAfterDetect();
        return;
    }
    QByteArrayList cmds;
    quint8 idx = nextGripperCmdIndex(cmds);
    cmds.push_back(QUIUtils::withGripperPump(
                pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, throwH, TRAY_TESTCHANNEL, idx, false),
                GRIPPERMOVE_NORMAL));
    m_gripperAirProbeHeight = 0;
    queueGripperAirMotion(cmds, kGripperMotionDumpRetreat);
}

void MachineSetting::sendGripperCancelHoming()
{
    if (cglobal::g_StartTesting.load() || !cglobal::gserialConnecStatus.load()) {
        return;
    }
    UsbCodeDispose *pActive = Testing::getTaskDll();
    if (!pActive) {
        return;
    }
    QPoint origin(0, 0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT, origin);

    // 抓杯位置不能直接 XY 复位。先抓手复位并关掉吸气，再试剂针、血样针复位，最后才 XY。
    QByteArrayList cmds;
    {
        quint8 idx = cancelHomingCmdIndex(cmds);
        cmds.push_back(QUIUtils::withGripperPump(
                    pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, 0, TRAY_TESTCHANNEL, idx, false),
                    GRIPPERMOVE_NORMAL));
    }
    {
        quint8 idx = cancelHomingCmdIndex(cmds);
        cmds.push_back(pActive->DLL_ZAxis_Reset(MOTOR_REAGNET_INDEX, 0, TRAY_TESTCHANNEL, idx, false));
    }
    {
        quint8 idx = cancelHomingCmdIndex(cmds);
        cmds.push_back(pActive->DLL_ZAxis_Reset(MOTOR_BLOOD_INDEX, 0, TRAY_TESTCHANNEL, idx, false));
    }
    {
        quint8 idx = cancelHomingCmdIndex(cmds);
        const QByteArray repos = pActive->_DLLXYMoveReposition_(idx, MACHINEBACK_SPEED);
        if (!repos.isEmpty()) {
            cmds.push_back(repos);
        }
    }
    if (origin.x() != 0 || origin.y() != 0) {
        quint8 idx = cancelHomingCmdIndex(cmds);
        cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(origin, 0, 0, idx));
    }
    if (cmds.isEmpty()) {
        return;
    }
    stopGripperAirPoll();
    m_gripperAirMotionKind = kGripperMotionCancel;
    m_gripperAirAwaitingMotion = true;
    emit testdownheight(cmds, EQUIPMENT_GRIPPER_AIR_CALIB);
}

void MachineSetting::sendGripperDumpRetry()
{
    if (m_gripperAirDumpRetry >= kGripperAirDumpMaxRetry) {
        QMessageBox::warning(this, gtr(u8"下发失败"),
                             gtr(u8"弃杯阶段重试次数已达上限。"));
        cancelGripperAirDetect();
        return;
    }
    ++m_gripperAirDumpRetry;
    sendGripperDumpSequence();
}

void MachineSetting::sendGripperHomeAfterDump()
{
    sendGripperReturnOriginAfterDetect();
}

void MachineSetting::sendGripperReturnOriginAfterDetect()
{
    UsbCodeDispose *pActive = Testing::getTaskDll();
    if (!pActive) {
        return;
    }
    QPoint origin(0, 0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT, origin);
    const int throwH = INI_File()._gethandsdownthrowcpus();
    m_gripperAirProbeHeight = 0;
    QByteArrayList cmds;
    if (throwH > 0) {
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(QUIUtils::withGripperPump(
                    pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, throwH, TRAY_TESTCHANNEL, idx, false),
                    GRIPPERMOVE_NORMAL));
    }
    if (origin.x() != 0 || origin.y() != 0) {
        appendXYReposition(cmds, pActive);
        quint8 idx = nextGripperCmdIndex(cmds);
        cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(origin, 0, 0, idx));
    }
    if (cmds.isEmpty()) {
        finishGripperAirDetect();
        return;
    }
    queueGripperAirMotion(cmds, kGripperMotionReturn);
}

void MachineSetting::beginGripperAirDetect()
{
    if (cglobal::g_StartTesting.load()) {
        QMessageBox::information(this, gtr(u8"运行暂停"),
                                 gtr(u8"测试中不能开启抓手负压。"));
        return;
    }
    if (!cglobal::gserialConnecStatus.load()) {
        QMessageBox::information(this, gtr(u8"未连接"),
                                 gtr(u8"请先连接仪器。"));
        return;
    }

    QPoint ch1Axis(0, 0);
    SingletonAxis::chnZoneAxisPos(READ_OPERRAT, kGripperAirCalibChannel,
                                  MOTOR_HANDS_INDEX, ch1Axis);
    if (ch1Axis.x() == 0 && ch1Axis.y() == 0) {
        QMessageBox::warning(this, gtr(u8"下发失败"),
                             gtr(u8"没有通道1坐标, 请先校准坐标。"));
        return;
    }
    if (INI_File()._gethandsdownheiht(kGripperAirCalibChannel) <= 0) {
        QMessageBox::warning(this, gtr(u8"下发失败"),
                             gtr(u8"通道1抓手下降高度为0, 请先设置。"));
        return;
    }

    cancelGripperAirDetect();
    m_gripperAirDetectStep = 1;
    m_gripperAirEmptyBaseline = -1;
    m_gripperAirReleaseAfterDump = -1;
    m_gripperAirLockedHold = -1;
    m_gripperAirDumpRetry = 0;
    m_gripperAirSettleMs = kGripperAirOriginSettleMs;
    m_gripperAirSamples.clear();
    m_gripperAirPhaseClock.restart();

    updateGripperAirStepHint(1);
    sendGripperOriginSequence();
}

void MachineSetting::completeGripperOriginEmptyStep(int emptyVal)
{
    if (m_gripperAirDetectStep != 1) {
        return;
    }
    if (m_gripperAirOriginAckFallbackTimer) {
        m_gripperAirOriginAckFallbackTimer->stop();
    }
    m_gripperAirEmptyBaseline = emptyVal;
    m_liveReleaseAir = emptyVal;
    ui->label_splitval->setText(gtr(u8"原点空载 %1").arg(emptyVal));
    m_gripperAirSamples.clear();
    stopGripperAirPoll();
    m_gripperAirDetectStep = 0;
    updateGripperAirStepHint(22);
    QTimer::singleShot(kGripperAirVacuumOffBeforeMoveMs, this, [this]() {
        if (m_gripperAirEmptyBaseline < 0) {
            return;
        }
        m_gripperAirDetectStep = 2;
        m_gripperAirSettleMs = kGripperAirObserveMinMs;
        m_gripperGrabSeenMs = -1;
        m_gripperAirPhaseClock.restart();
        m_gripperAirSamples.clear();
        sendGripperChannelGrabSequence();
    });
}

void MachineSetting::scheduleGripperOriginEmptyPollFallback()
{
    if (!m_gripperAirOriginAckFallbackTimer) {
        m_gripperAirOriginAckFallbackTimer = new QTimer(this);
        m_gripperAirOriginAckFallbackTimer->setSingleShot(true);
        connect(m_gripperAirOriginAckFallbackTimer, &QTimer::timeout, this, [this]() {
            if (m_gripperAirDetectStep != 1 || m_gripperAirAwaitingMotion) {
                return;
            }
            m_gripperAirSettleMs = kGripperAirOriginSettleMs;
            m_gripperAirPhaseClock.restart();
            startGripperAirPoll(false);
        });
    }
    m_gripperAirOriginAckFallbackTimer->start(2500);
}

void MachineSetting::feedGripperAirSample(bool issuck, int airval)
{
    if (m_gripperAirDetectStep == 0 || m_gripperAirAwaitingMotion) {
        return;
    }
    if (m_gripperAirDetectStep != 1 && m_gripperAirPhaseClock.elapsed() < m_gripperAirSettleMs) {
        return;
    }

    if (m_gripperAirDetectStep == 1 && !issuck) {
        int avg = 0;
        if (gripperAirPushSample(m_gripperAirSamples, airval, &avg,
                                 kGripperAirOriginStableCount, kGripperAirOriginMaxSpread)) {
            completeGripperOriginEmptyStep(avg);
        }
        return;
    }

    if (m_gripperAirPhaseClock.elapsed() < m_gripperAirSettleMs) {
        return;
    }

    if (m_gripperAirDetectStep == 2
            && m_gripperAirLockedHold < 0
            && m_gripperAirPhaseClock.elapsed() > kGripperAirGrabGiveUpMs) {
        cancelGripperAirDetect();
        QMessageBox::information(
                    this, gtr(u8"没有抓到杯子"),
                    gtr(u8"通道1没有抓到杯子，不丢杯。抓手、试剂针、血样针复位后再XY复位。"));
        return;
    }

    const auto finishDumpRelease = [this](int releaseVal) {
        m_gripperThrowDelayMeasured = clampGripperAutoDelay(
                    m_gripperThrowSeenMs >= 0
                    ? m_gripperThrowSeenMs
                    : static_cast<int>(m_gripperAirPhaseClock.elapsed()));
        m_gripperAirReleaseAfterDump = releaseVal;
        m_liveReleaseAir = releaseVal;
        ui->label_splitval->setText(gtr(u8"弃杯空载 %1").arg(releaseVal));
        stopGripperAirPoll();
        sendGripperDumpZRetreat();
    };

    if (m_gripperAirDetectStep == 2 && issuck && m_gripperGrabSeenMs < 0
            && m_gripperAirEmptyBaseline >= 0
            && m_gripperAirEmptyBaseline - airval >= kGripperAirHoldDeltaMin) {
        m_gripperGrabSeenMs = static_cast<int>(m_gripperAirPhaseClock.elapsed());
    }
    if (m_gripperAirDetectStep == 3 && !issuck && m_gripperThrowSeenMs < 0
            && m_gripperAirLockedHold >= 0
            && airval - m_gripperAirLockedHold >= kGripperAirDropMin
            && (m_gripperAirEmptyBaseline < 0
                || airval >= m_gripperAirEmptyBaseline - kGripperAirEmptyMatchMax)) {
        m_gripperThrowSeenMs = static_cast<int>(m_gripperAirPhaseClock.elapsed());
    }

    int avg = 0;
    if (!gripperAirPushSample(m_gripperAirSamples, airval, &avg)) {
        return;
    }

    if (m_gripperAirDetectStep == 2 && issuck) {
        const bool holdByDelta = (m_gripperAirEmptyBaseline >= 0
                                  && m_gripperAirEmptyBaseline - avg >= kGripperAirHoldCommitDelta);
        if (!holdByDelta) {
            return;
        }
        m_gripperAirLockedHold = avg;
        m_gripperGrabDelayMeasured = clampGripperAutoDelay(
                    m_gripperGrabSeenMs >= 0
                    ? m_gripperGrabSeenMs
                    : static_cast<int>(m_gripperAirPhaseClock.elapsed()));
        m_liveHoldAir = avg;
        ui->label_showsuckVal->setText(gtr(u8"吸住 %1").arg(avg));
        m_gripperAirDetectStep = 3;
        m_gripperAirSamples.clear();
        m_gripperAirSettleMs = kGripperAirObserveMinMs;
        m_gripperAirPhaseClock.restart();
        stopGripperAirPoll();
        sendGripperDumpSequence();
        return;
    }

    if (m_gripperAirDetectStep == 3 && !issuck) {
        const bool risenFromHold = (m_gripperAirLockedHold < 0)
                || (avg - m_gripperAirLockedHold >= kGripperAirDropMin);
        const bool nearEmpty = (m_gripperAirEmptyBaseline < 0)
                || (avg >= m_gripperAirEmptyBaseline - kGripperAirEmptyMatchMax);
        const bool throwReleased = risenFromHold && nearEmpty;
        if (!throwReleased) {
            if (m_gripperAirPhaseClock.elapsed() > kGripperAirReleaseTimeoutMs) {
                sendGripperDumpRetry();
                m_gripperAirSamples.clear();
                m_gripperThrowSeenMs = -1;
                m_gripperAirSettleMs = kGripperAirObserveMinMs;
                m_gripperAirPhaseClock.restart();
            }
            return;
        }
        finishDumpRelease(avg);
    }
}

void MachineSetting::installParameterHints()
{
    setToolTipDuration(30000);
    const QString tipStyle = QStringLiteral(
                "\nQToolTip { background-color:#F4FAFD; color:#16324A;"
                " border:1px solid #7EB0CC; padding:8px;"
                " font: 13px \"Microsoft YaHei\"; }");
    if (!styleSheet().contains(QStringLiteral("QToolTip"))) {
        setStyleSheet(styleSheet() + tipStyle);
    }

    auto tip = [](QWidget *a, QWidget *b, const QString &text) {
        if (a) {
            a->setToolTip(text);
        }
        if (b) {
            b->setToolTip(text);
        }
    };
    auto tips = [](const QList<QWidget *> &widgets, const QString &text) {
        for (QWidget *w : widgets) {
            if (w) {
                w->setToolTip(text);
            }
        }
    };

    const QString scanTip = gtr(u8"加样和测高时读试管上的条码。关掉就用手输样本号。");
    tip(ui->UseBarCode, nullptr, scanTip);

    const QString reserveTip = gtr(u8"同一种试剂可以放左边和右边两个位置。\n打开后：左边再吸一针会低于试剂报警限，并且右边同种试剂还不低于报警限，这一针就改吸右边，中间不暂停。两边都不够才暂停。\n报警限填 0 时，左边空了也不会切换。关掉则只用左边。");
    tip(ui->checkBox_UesSecondReagentHole, nullptr, reserveTip);

    ui->checkBox_catchcups->setText(gtr(u8"开机智能弃杯"));
    const QString bootCupTip = gtr(u8"勾选后，调光发现通道里可能还有试杯时，会自动把这些杯子丢掉，最多两轮。读数为 0 不丢，那是模组没有光。\n不勾选则调光时不丢杯，只在调光失败窗口里提示。");
    tip(ui->checkBox_catchcups, nullptr, bootCupTip);

    ui->checkBox_suck_offset->setText(gtr(u8"吸样前先吸空气"));
    const QString airFirstTip = gtr(u8"勾选后，吸血样前先吸一段空气，把针管里上次留下的液体隔开。保存仪器配置时写到主板。\n以前叫「校准吸样」，容易看成校准坐标。");
    tip(ui->checkBox_suck_offset, nullptr, airFirstTip);

    const QString channelTip = gtr(u8"勾上的通道可以排样、测试和调光。关掉的通道不会被分配样本。");
    if (ui->usechannel) {
        const QList<QCheckBox *> channels = ui->usechannel->findChildren<QCheckBox *>();
        for (QCheckBox *box : channels) {
            tip(box, nullptr, channelTip);
        }
        ui->usechannel->setToolTip(channelTip);
    }

    const QString temp1 = gtr(u8"模组 1 的目标温度，单位℃，管通道 1 到 4。点保存后写入主板，模组按这个温度重新锁温。");
    const QString temp2 = gtr(u8"模组 2 的目标温度，单位℃，管通道 5 到 8。点保存后写入主板，模组按这个温度重新锁温。");
    const QString temp3 = gtr(u8"模组 3 的目标温度，单位℃，管通道 9 到 12。点保存后写入主板，模组按这个温度重新锁温。");
    tip(ui->label_Moduletemperature_1, ui->doubleSpinBox_Moduletemperature_1, temp1);
    tip(ui->label_Moduletemperature_2, ui->doubleSpinBox_Moduletemperature_2, temp2);
    tip(ui->label_Moduletemperature_3, ui->doubleSpinBox_Moduletemperature_3, temp3);
    tip(ui->Machine_conf_save, nullptr, gtr(u8"把通道开关、上面几个勾选和三块模组温度写到本机，并下发主板。"));

    ui->label_EmptyTubeHeigh->setText(gtr(u8"试杯吐样下针"));
    tip(ui->label_EmptyTubeHeigh, ui->EmptyHeigh, gtr(u8"样本针把血样吐进空试杯时的固定下降高度，单位毫米，不做液面探测。\n点击后到试管盘四角各下一次这个高度，看会不会扎底或够不着。"));

    ui->label_failedlinque_3->setText(gtr(u8"洗液探不到时下针"));
    tip(ui->label_failedlinque_3, ui->spinBox_CleanLinqueFailedHigh, gtr(u8"洗样本针、吸内部清洗液时做液面探测。探到液面就停；一直探不到才下到这个高度。\n不是平时吐样的深度。点击后到清洗位按这个高度试一次。"));

    ui->label_3->setText(gtr(u8"血浆模式固定下针"));
    tip(ui->label_3, ui->Testheighdownheigh, gtr(u8"血浆模式不探液面，样本针直接下到这个固定高度。批量加样时的默认下针高度也是它。\n点击后到血样孔 2 按这个高度试一次。"));

    ui->label_failedlinque->setText(gtr(u8"吸血探液失败高度"));
    tip(ui->label_failedlinque, ui->spinBox_faliedlinque, gtr(u8"吸血样时样本针做液面探测，探到液面就停。探不到就下到这个高度为止，避免扎穿试管。血浆模式不用这项。\n主板上这一项也叫贫血探测失败高度。点击后到血样孔 1 按这个高度试一次。"));

    tip(ui->label_SecurityValue, ui->SecurityValue_box, gtr(u8"样本泵换向时空走的补偿，界面上仍叫空回值。吐样时按 空回值/0.347 折成步数补上。偏小容易吐不净，偏大就多吐空气。"));
    tip(ui->Poor_blood_changliang, ui->poorBlood_changliang, gtr(u8"每一份血样要吸的微升数。真正的泵步数还要再乘 PPP 系数或 PRP 系数。"));
    tip(ui->label_ratio_ben, ui->doubleSpinBox_Ratio_ben, gtr(u8"把 PPP 样本量换算成样本泵步数。步数大约等于 样本量 × 本系数。只改泵走多少，不改上面的微升数。\n只做一个项目时，吸 PRP 也走这个系数。"));
    tip(ui->PRPratiolable, ui->doubleSpinBox_PRPratio, gtr(u8"一次抽血、多个试剂吸 PRP 时，用这个系数把样本量换算成泵步数。只做一个项目时不走这项，走 PPP 系数。"));
    tip(ui->labelPeRatio, ui->doubleSpinBoxAddRatio, gtr(u8"性能评估加标准品时，在按 PPP 系数算出的吸量之外，再多吸这个百分比。填 10 就是多吸 10%。病人正式测试不走这项。"));
    tip(ui->label_fixedhigh, ui->FixedHighvalue, gtr(u8"全血测高用的基准高度，就是针到基准物的距离，单位毫米。和测高偏移、模块底部距离一起算出该下多深。"));
    tip(ui->OffsetTestHeight, ui->OffsetTestHeightValue, gtr(u8"全血测高的安全余量，单位毫米。算出来的下针深度会再减掉它，避免针扎进红细胞层。"));
    tip(ui->labelRefToBottom, ui->doubleSpinBoxBottomHeigh, gtr(u8"测高模块到试管底的参照距离，单位毫米。和全血测高高度、偏移一起算针该下多深。保存后写到主板扩展参数。"));
    tip(ui->label_AbsorbX2, ui->spinBoxAbsorbX2, gtr(u8"每洗一次样本针，从内部清洗液瓶里吸走的微升。界面上的清洗液百分比按这个数扣，不按泵步数扣。"));
    tip(ui->labelwashesTime, ui->spinBox_WashesTime, gtr(u8"样本针或试剂针把清洗液吐回清洗池时，泵保持冲洗的毫秒数。洗双针、只洗样本针、只洗试剂针都用这一项。"));
    tip(ui->label_suckairs, ui->spinBox_suckairs, gtr(u8"吸 PRP 前要额外吸入的空气量，点保存会写回主板，用来隔开针管里上次的残液。\n上位机现在组吸 PRP 动作时，隔开残液用的是固定回吸，改这个数不会马上改变那一段泵步。"));
    tip(ui->label_PPPdelay, ui->spinBox_PPPdelay, gtr(u8"血样吐进试杯之后，等这么多毫秒再发下一条抬针。PPP 和 PRP 吐样都用它，避免液体还没落下就抬针。保存后写到主板。"));
    tip(ui->label_PRPdelay, ui->spinBox_PRPdelay, gtr(u8"血样吸完之后，等这么多毫秒再发下一条抬针。PPP 和 PRP 吸样都用它。保存后写到主板。"));
    tip(ui->pushButtonBloodPinParasave, nullptr, gtr(u8"把本页样本针参数保存，并写入主板。"));

    const QString coefTip = gtr(u8"只修正试剂泵步数，让机器真正吸到你填的微升。1.02 表示泵多走 2% 的步。\n不参与瓶子剩余百分比。百分比只看吸试剂量，再加上基础吸量添加比。");
    tips(QList<QWidget *>()
         << ui->label_11
         << ui->doubleSpinBox_AA_Ratio << ui->doubleSpinBox_ADP_Ratio
         << ui->doubleSpinBox_ADR_Ratio << ui->doubleSpinBox_COL_Ratio
         << ui->doubleSpinBox_RIS_Ratio, coefTip);

    const QString addTip = gtr(u8"在标称微升之外真正多吸的比例。0.02 表示多吸 2%，瓶子余量会把多出来的液体扣掉。\n和吸试剂系数不是一回事：系数只修泵步，这一项才是多吸液体。");
    tips(QList<QWidget *>()
         << ui->label_12
         << ui->doubleSpinBox_Add_AA_Ratio << ui->doubleSpinBox_Add_ADP_Ratio
         << ui->doubleSpinBox_Add_ADR_Ratio << ui->doubleSpinBox_Add_COL_Ratio
         << ui->doubleSpinBox_Add_RIS_Ratio, addTip);

    const QString volTip = gtr(u8"这种试剂每次从瓶里吸走的标称微升。瓶子百分比按这个数扣；若基础吸量添加比大于 0，再把多吸的液体一并扣掉。");
    tips(QList<QWidget *>()
         << ui->label_absorbVol
         << ui->spinBox_AA_Suck_Vol << ui->spinBox_ADP_Suck_Vol
         << ui->spinBox_ADR_Suck_Vol << ui->spinBox_COL_Suck_Vol
         << ui->spinBox_RIS_Suck_Vol, volTip);

    const QString downTip = gtr(u8"试剂针把这种试剂吐进测试通道时的固定高度，单位毫米，不探液面。\n点左侧蓝色试剂名，会到 1 号通道按这一行的高度试下针。");
    tips(QList<QWidget *>()
         << ui->label_13
         << ui->spinBox_down_AA << ui->spinBox_down_ADP
         << ui->spinBox_down_ADR << ui->spinBox_down_COL
         << ui->spinBox_down_RIS, downTip);

    const QString capTip = gtr(u8"一瓶能装多少微升。同一种试剂左边和右边共用这个容量。剩余百分比 = 剩下的微升 / 容量。\n清洗液那一行是内部清洗液，不是试剂。");
    tips(QList<QWidget *>()
         << ui->label_capacity
         << ui->spinBox_capacity_aa << ui->spinBox_capacity_adp
         << ui->spinBox_capacity_epi << ui->spinBox_capacity_col
         << ui->spinBox_capacity_ris << ui->spinBox_capacity_cleanlinque, capTip);

    const QString limitTip = gtr(u8"剩余百分比低于这个数就报警。\n开了预留试剂位时：左边再吸一针会低于这个数，右边还够，才改吸右边。填 0 时左边空了也不会切换。清洗液用同一套百分比报警。");
    tips(QList<QWidget *>()
         << ui->label_limit
         << ui->spinBox_limit_aa << ui->spinBox_limit_adp
         << ui->spinBox_limit_epi << ui->spinBox_limit_col
         << ui->spinBox_limit_ris << ui->spinBox_limit_cleanlinque, limitTip);
    tip(ui->label_cleanlinque, nullptr, gtr(u8"这一行是内部清洗液。容量是整瓶微升，报警限是剩余百分比。洗样本针、洗试剂针都从这里扣。"));

    ui->label_failedhighreagentneedle->setText(gtr(u8"吸试剂探液失败高度"));
    tip(ui->label_failedhighreagentneedle, ui->spinBoxReagentNeedleFailehigh, gtr(u8"吸试剂不再做液面探测，试剂针直接下到这个高度再吸，避免扎到瓶底。\n点击后到试剂位按这个高度试一次。"));

    ui->label_failedlinque_2->setText(gtr(u8"洗液探不到时下针"));
    tip(ui->label_failedlinque_2, ui->spinBox_TestReagentFailedhigh, gtr(u8"洗试剂针、吸内部清洗液时做液面探测。探到液面就停；探不到才下到这个高度。\n点击后到清洗位按这个高度试一次。"));

    ui->label_AbsorbX1->setText(gtr(u8"试剂针清洗量"));
    tip(ui->label_AbsorbX1, ui->spinBoxAbsorbX1, gtr(u8"每洗一次试剂针，从内部清洗液瓶吸走的微升。清洗液百分比按这个数扣。"));

    ui->label_delpms->setText(gtr(u8"读初值前等待"));
    tip(ui->label_delpms, ui->spinBox_deptime, gtr(u8"杯子放到通道后，先等这段毫秒再判断PRP是否异常、再读PRP0。五种试剂都等。读到PRP0后，再等同样的毫秒，才把对应试剂吐进通道里的杯子。读PPP初值也会加上这段等待。不是吸完试剂后的抬针时间。"));

    ui->label_reagentSuckLiftDelay->setText(gtr(u8"吸吐完再抬针"));
    tip(ui->label_reagentSuckLiftDelay, ui->spinBox_reagentSuckLiftDelay, gtr(u8"试剂针吸完试剂、或把试剂吐进通道杯子后，都等这么多毫秒再抬针，让液滴落下，避免还挂在针尖上。\n改完离开输入框就会记下；点保存试剂针参数时也会再写一次。存在本机配置里，测试直接用。"));
    tip(ui->pushButton_saved, nullptr, gtr(u8"把吸试剂量、系数、添加比、吐试剂高度、容量、报警限，以及吸完再抬针的时间保存，并把试剂参数下发主板。"));

    if (ui->groupBox_gripperDown) {
        ui->groupBox_gripperDown->setTitle(gtr(u8"抓手下降高度"));
        ui->groupBox_gripperDown->setToolTip(gtr(u8"抓手在试管盘、测试通道和弃杯孔下到多深。蓝色标签可以点，按旁边的毫米数试一次动作。"));
    }
    tip(ui->label_handthrowcups, ui->spinBox_downthrowcups, gtr(u8"抓手在弃杯孔松手时的下降深度，单位毫米。\n点击后只到弃杯孔下到这个高度，再抬起来。"));
    tip(ui->label_testtray_0, ui->spinBox_testtray_0, gtr(u8"抓手从试管盘 1 抓杯子时的下降深度。点击后到这只盘的四角各抓一次，再到弃杯孔按弃杯高度试放，最后回原点。"));
    tip(ui->label_testtray_1, ui->spinBox_testtray_1, gtr(u8"抓手从试管盘 2 抓杯子时的下降深度。点击后到这只盘的四角各抓一次，再到弃杯孔按弃杯高度试放，最后回原点。"));
    tip(ui->label_testtray_2, ui->spinBox_testtray_2, gtr(u8"抓手从试管盘 3 抓杯子时的下降深度。点击后到这只盘的四角各抓一次，再到弃杯孔按弃杯高度试放，最后回原点。"));
    tip(ui->label_testtray_3, ui->spinBox_testtray_3, gtr(u8"抓手从试管盘 4 抓杯子时的下降深度。点击后到这只盘的四角各抓一次，再到弃杯孔按弃杯高度试放，最后回原点。"));
    tip(ui->label_handsdownchn_0, ui->spinBox_handsdownchn_0, gtr(u8"抓手把杯子放进模组 1（通道 1 到 4）时的下降深度，这四个通道共用。\n点击后到通道 1 下到这个深度，再到弃杯孔试放，然后回原点。"));
    tip(ui->label_handsdownchn_1, ui->spinBox_handsdownchn_1, gtr(u8"抓手把杯子放进模组 2（通道 5 到 8）时的下降深度，这四个通道共用。\n点击后到通道 5 下到这个深度，再到弃杯孔试放，然后回原点。"));
    tip(ui->label_handsdownchn_2, ui->spinBox_handsdownchn_2, gtr(u8"抓手把杯子放进模组 3（通道 9 到 12）时的下降深度，这四个通道共用。\n点击后到通道 9 下到这个深度，再到弃杯孔试放，然后回原点。"));

    const QString grabTip = gtr(u8"气压读数小于等于这个值，才算杯子吸在抓手上。数值越小，要求吸得越狠。\n自动判定会把测到的数填进来。要点保存抓手参数，正式测试才会用新值。");
    tip(ui->label_2, ui->spinBoxSuckAirsMax, grabTip);
    const QString dropTip = gtr(u8"气压读数大于等于这个值，才算杯子已经松开。必须明显高于抓住判定，否则没丢掉也会当成已丢，抬抓手时把杯子带起来。\n自动判定会填这个数。保存后正式测试才使用。");
    tip(ui->label_8, ui->spinBoxSplitAirsMin, dropTip);
    const QString grabDelayTip = gtr(u8"抓手下到杯子上、负压打开之后，等这么多毫秒再读气压。太短会在还没吸牢时就判定失败，然后加深重抓。");
    tip(ui->labelsuckLateTimer, ui->spinBox_suckLatetimer, grabDelayTip);
    const QString throwDelayTip = gtr(u8"关掉负压之后，等这么多毫秒再确认杯子松开。太短会在杯子还粘着时就抬抓手，把试杯带起来。\n保存抓手参数时，这个时间会和试剂扩展参数一起写到主板。");
    tip(m_gripperThrowDelayLabel, m_gripperThrowDelaySpin, throwDelayTip);
    tip(ui->checkBox_Recapture, nullptr, gtr(u8"吸杯判定没过时，再加深重抓。关掉则失败后走异常复位，不再试第二次。"));
    tip(ui->pushButtonopenSuck, nullptr, gtr(u8"仪器自己走一遍空载、吸住、丢掉，把抓住判定、丢掉判定、吸杯延时和丢杯延时填进框里。填完仍要点保存抓手参数，才会写进主板。"));
    tip(ui->pushButtonsplitAirs, nullptr, gtr(u8"停下正在进行的自动判定。判定没在跑时，用来关闭负压并看当前空载气压。"));
    tip(ui->label_showsuckVal, nullptr, gtr(u8"最近一次吸住时读到的气压。数值越小，吸力越大。"));
    tip(ui->label_splitval, nullptr, gtr(u8"最近一次空载或丢掉时读到的气压。应明显高于吸住时的数。"));
    tip(ui->pushButtonHandsParaSave, nullptr, gtr(u8"把下降高度、负压判定、吸杯延时和丢杯延时保存，并写入主板。"));

    relayoutEquipmentPages();
}

class ChannelToggle : public QWidget
{
public:
    explicit ChannelToggle(QCheckBox *box, QWidget *parent = 0)
        : QWidget(parent), m_box(box)
    {
        setFixedSize(78, 36);
        setCursor(Qt::PointingHandCursor);
        setAttribute(Qt::WA_Hover, true);
    }

protected:
    void paintEvent(QPaintEvent *)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        const bool on = m_box && m_box->isChecked();
        const QRectF track(1.0, 4.0, width() - 2.0, 28.0);
        painter.setPen(QPen(on ? QColor(44, 127, 176) : QColor(197, 208, 218), 1));
        painter.setBrush(on ? QColor(58, 155, 212) : QColor(213, 222, 230));
        painter.drawRoundedRect(track, 14.0, 14.0);

        const qreal knob = 22.0;
        const qreal knobY = track.center().y() - knob / 2.0;
        const qreal knobX = on ? track.right() - knob - 3.0 : track.left() + 3.0;
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 36));
        painter.drawEllipse(QRectF(knobX, knobY + 1.0, knob, knob));
        painter.setBrush(QColor(255, 255, 255));
        painter.drawEllipse(QRectF(knobX, knobY, knob, knob));

        painter.setPen(on ? QColor(232, 244, 253) : QColor(90, 112, 128));
        QFont font(QStringLiteral("Microsoft YaHei"));
        font.setPixelSize(12);
        font.setBold(true);
        painter.setFont(font);
        const QRectF word = on
                ? QRectF(track.left() + 4.0, track.top(), 28.0, track.height())
                : QRectF(track.right() - 32.0, track.top(), 28.0, track.height());
        painter.drawText(word, Qt::AlignCenter, on ? gtr(u8"开") : gtr(u8"关"));
    }

    void mouseReleaseEvent(QMouseEvent *event)
    {
        if (event && event->button() == Qt::LeftButton && m_box) {
            m_box->setChecked(!m_box->isChecked());
            update();
        }
        QWidget::mouseReleaseEvent(event);
    }

private:
    QCheckBox *m_box;
};

void MachineSetting::relayoutEquipmentPages()
{
    if (ui->usechannel && ui->usechannel->property("modelCards").toBool()) {
        return;
    }
    if (ui->usechannel) {
        ui->usechannel->setProperty("modelCards", true);
    }
    int modules = 3;
    if (m_typedequipment == KS600) {
        modules = 1;
    } else if (m_typedequipment == KS800) {
        modules = 2;
    }

    QCheckBox *boxes[12] = {
        ui->Channel_State_1, ui->Channel_State_2, ui->Channel_State_3, ui->Channel_State_4,
        ui->Channel_State_5, ui->Channel_State_6, ui->Channel_State_7, ui->Channel_State_8,
        ui->Channel_State_9, ui->Channel_State_10, ui->Channel_State_11, ui->Channel_State_12
    };
    QLabel *tempLab[3] = {
        ui->label_Moduletemperature_1, ui->label_Moduletemperature_2, ui->label_Moduletemperature_3
    };
    QDoubleSpinBox *tempSp[3] = {
        ui->doubleSpinBox_Moduletemperature_1,
        ui->doubleSpinBox_Moduletemperature_2,
        ui->doubleSpinBox_Moduletemperature_3
    };

    const QString cardQss = QStringLiteral(
                "QFrame#moduleChannelCard {"
                " background: transparent;"
                " border: none;"
                "}");
    const QString headQss = QStringLiteral(
                "QFrame#moduleHeadBar {"
                " background: #D4EAF5;"
                " border: none;"
                " border-top-left-radius: 8px;"
                " border-top-right-radius: 8px;"
                "}");
    const QString titleQss = QStringLiteral(
                "color:#2C5F8A; font-size:15px; font-weight:700; background:transparent; border:none;");
    const QString subQss = QStringLiteral(
                "color:#4A7FA3; font-size:13px; background:transparent; border:none;");

    if (ui->gridLayout_11 && ui->usechannel) {
        while (QLayoutItem *old = ui->gridLayout_11->takeAt(0)) {
            delete old;
        }
        ui->gridLayout_11->setContentsMargins(4, 2, 4, 4);
        ui->gridLayout_11->setHorizontalSpacing(10);
        ui->gridLayout_11->setVerticalSpacing(0);
        ui->usechannel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        if (ui->gridLayout_12) {
            ui->gridLayout_12->setAlignment(ui->usechannel, Qt::AlignTop);
            ui->gridLayout_12->setRowStretch(1, 0);
        }

        for (int m = 0; m < modules; ++m) {
            QFrame *card = new QFrame(ui->usechannel);
            card->setObjectName(QStringLiteral("moduleChannelCard"));
            card->setStyleSheet(cardQss);
            card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
            QVBoxLayout *v = new QVBoxLayout(card);
            v->setContentsMargins(0, 0, 0, 8);
            v->setSpacing(0);

            QFrame *headBar = new QFrame(card);
            headBar->setObjectName(QStringLiteral("moduleHeadBar"));
            headBar->setStyleSheet(headQss);
            QHBoxLayout *headLay = new QHBoxLayout(headBar);
            headLay->setContentsMargins(12, 8, 12, 8);
            headLay->setSpacing(8);
            QLabel *title = new QLabel(headBar);
            title->setText(gtr(u8"模组 %1").arg(m + 1));
            title->setStyleSheet(titleQss);
            title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            QLabel *sub = new QLabel(headBar);
            sub->setText(gtr(u8"通道 %1–%2").arg(m * 4 + 1).arg(m * 4 + 4));
            sub->setStyleSheet(subQss);
            sub->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            sub->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            headLay->addWidget(title);
            headLay->addStretch(1);
            headLay->addWidget(sub);
            v->addWidget(headBar);

            QBoxLayout *chLay = 0;
            if (modules == 1) {
                QHBoxLayout *row = new QHBoxLayout;
                row->setContentsMargins(12, 8, 12, 4);
                row->setSpacing(16);
                chLay = row;
            } else {
                QVBoxLayout *col = new QVBoxLayout;
                col->setContentsMargins(12, 6, 12, 2);
                col->setSpacing(0);
                chLay = col;
            }
            for (int i = 0; i < 4; ++i) {
                QCheckBox *box = boxes[m * 4 + i];
                if (!box) {
                    continue;
                }
                QWidget *line = new QWidget(card);
                line->setObjectName(QStringLiteral("channelSwitchLine"));
                line->setStyleSheet(QStringLiteral(
                            "QWidget#channelSwitchLine { background: transparent; }"));
                line->setMinimumHeight(44);
                line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                QHBoxLayout *lineLay = new QHBoxLayout(line);
                lineLay->setContentsMargins(0, 2, 0, 2);
                lineLay->setSpacing(8);
                QLabel *name = new QLabel(box->text(), line);
                name->setStyleSheet(QStringLiteral(
                            "color:#2C5F8A; font-size:16px; font-weight:600;"
                            " background:transparent; border:none;"));
                box->setParent(line);
                box->hide();
                ChannelToggle *toggle = new ChannelToggle(box, line);
                QObject::connect(box, &QCheckBox::toggled, toggle, [toggle](bool) {
                    toggle->update();
                });
                lineLay->addWidget(name, 1);
                lineLay->addWidget(toggle, 0, Qt::AlignVCenter);
                chLay->addWidget(line);
            }
            v->addLayout(chLay);

            if (tempLab[m] && tempSp[m]) {
                tempLab[m]->setParent(card);
                tempSp[m]->setParent(card);
                tempLab[m]->setText(gtr(u8"目标温度"));
                tempLab[m]->setStyleSheet(subQss);
                tempLab[m]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
                tempLab[m]->setMaximumWidth(QWIDGETSIZE_MAX);
                tempLab[m]->show();
                tempSp[m]->setMinimumWidth(120);
                tempSp[m]->setMaximumWidth(180);
                tempSp[m]->show();
                QHBoxLayout *tempRow = new QHBoxLayout;
                tempRow->setContentsMargins(12, 6, 12, 0);
                tempRow->setSpacing(8);
                tempRow->addWidget(tempLab[m]);
                tempRow->addWidget(tempSp[m]);
                tempRow->addStretch(1);
                v->addLayout(tempRow);
            }
            ui->gridLayout_11->addWidget(card, 0, m, Qt::AlignTop);
            ui->gridLayout_11->setColumnStretch(m, 1);
        }
        for (int i = modules * 4; i < 12; ++i) {
            if (boxes[i]) {
                boxes[i]->hide();
            }
        }
        for (int m = modules; m < 3; ++m) {
            if (tempLab[m]) {
                tempLab[m]->hide();
            }
            if (tempSp[m]) {
                tempSp[m]->hide();
            }
        }

        QString model = QStringLiteral("KS1200");
        if (m_typedequipment == KS600) {
            model = QStringLiteral("KS600");
        } else if (m_typedequipment == KS800) {
            model = QStringLiteral("KS800");
        }
        ui->usechannel->setTitle(model + gtr(u8"  使用通道"));
    }
    if (ui->groupBox) {
        ui->groupBox->hide();
    }

    QGridLayout *down = ui->gridLayout_2;
    if (!down || !ui->groupBox_gripperDown) {
        return;
    }
    while (QLayoutItem *old = down->takeAt(0)) {
        delete old;
    }
    down->setContentsMargins(12, 22, 12, 12);
    down->setHorizontalSpacing(10);
    down->setVerticalSpacing(8);

    const QString heightQss = QStringLiteral(
                "QFrame#heightTuneCard {"
                " background: #FFFFFF;"
                " border: 1px solid #D4EAF5;"
                " border-radius: 8px;"
                "}");
    const QString sectionQss = QStringLiteral(
                "color:#2C5F8A; font-weight:700; background:transparent; border:none; padding-top:4px;");

    QLabel *trayLab[4] = {
        ui->label_testtray_0, ui->label_testtray_1, ui->label_testtray_2, ui->label_testtray_3
    };
    QSpinBox *traySp[4] = {
        ui->spinBox_testtray_0, ui->spinBox_testtray_1, ui->spinBox_testtray_2, ui->spinBox_testtray_3
    };
    QLabel *modLab[3] = {
        ui->label_handsdownchn_0, ui->label_handsdownchn_1, ui->label_handsdownchn_2
    };
    QSpinBox *modSp[3] = {
        ui->spinBox_handsdownchn_0, ui->spinBox_handsdownchn_1, ui->spinBox_handsdownchn_2
    };

    int trayCount = 4;
    int modCount = modules;
    if (m_typedequipment == KS600) {
        trayCount = 2;
    } else if (m_typedequipment == KS800) {
        trayCount = 3;
    }
    for (int i = trayCount; i < 4; ++i) {
        if (trayLab[i]) {
            trayLab[i]->hide();
        }
        if (traySp[i]) {
            traySp[i]->hide();
        }
    }
    for (int i = modCount; i < 3; ++i) {
        if (modLab[i]) {
            modLab[i]->hide();
        }
        if (modSp[i]) {
            modSp[i]->hide();
        }
    }
    int cols = trayCount;
    if (modCount > cols) {
        cols = modCount;
    }
    if (cols < 1) {
        cols = 1;
    }

    int row = 0;
    if (ui->label_handthrowcups && ui->spinBox_downthrowcups) {
        ui->label_handthrowcups->setText(gtr(u8"弃杯孔"));
        QFrame *bar = new QFrame(ui->groupBox_gripperDown);
        bar->setObjectName(QStringLiteral("heightTuneCard"));
        bar->setStyleSheet(heightQss);
        QHBoxLayout *h = new QHBoxLayout(bar);
        h->setContentsMargins(12, 8, 12, 8);
        h->setSpacing(12);
        ui->label_handthrowcups->setParent(bar);
        ui->spinBox_downthrowcups->setParent(bar);
        ui->label_handthrowcups->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        ui->label_handthrowcups->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        ui->label_handthrowcups->show();
        ui->spinBox_downthrowcups->show();
        ui->spinBox_downthrowcups->setMinimumWidth(96);
        ui->spinBox_downthrowcups->setMaximumWidth(140);
        h->addWidget(ui->label_handthrowcups);
        h->addWidget(ui->spinBox_downthrowcups);
        h->addStretch(1);
        down->addWidget(bar, row, 0, 1, cols);
        ++row;
    }

    if (trayCount > 0) {
        QLabel *sec = new QLabel(ui->groupBox_gripperDown);
        sec->setText(gtr(u8"试管盘"));
        sec->setStyleSheet(sectionQss);
        down->addWidget(sec, row, 0, 1, cols);
        ++row;
        int col = 0;
        for (int i = 0; i < trayCount; ++i) {
            if (!trayLab[i] || !traySp[i]) {
                continue;
            }
            trayLab[i]->setText(gtr(u8"试管盘 %1").arg(i + 1));
            QFrame *card = new QFrame(ui->groupBox_gripperDown);
            card->setObjectName(QStringLiteral("heightTuneCard"));
            card->setStyleSheet(heightQss);
            QVBoxLayout *v = new QVBoxLayout(card);
            v->setContentsMargins(8, 8, 8, 8);
            v->setSpacing(6);
            trayLab[i]->setParent(card);
            traySp[i]->setParent(card);
            trayLab[i]->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            trayLab[i]->setAlignment(Qt::AlignCenter);
            trayLab[i]->setWordWrap(true);
            trayLab[i]->show();
            traySp[i]->show();
            traySp[i]->setMinimumWidth(88);
            traySp[i]->setMaximumWidth(160);
            v->addWidget(trayLab[i]);
            v->addWidget(traySp[i], 0, Qt::AlignHCenter);
            down->addWidget(card, row, col);
            down->setColumnStretch(col, 1);
            ++col;
        }
        ++row;
    }

    if (modCount > 0) {
        QLabel *sec = new QLabel(ui->groupBox_gripperDown);
        sec->setText(gtr(u8"放到测试通道"));
        sec->setStyleSheet(sectionQss);
        down->addWidget(sec, row, 0, 1, cols);
        ++row;
        int col = 0;
        for (int i = 0; i < modCount; ++i) {
            if (!modLab[i] || !modSp[i]) {
                continue;
            }
            modLab[i]->setText(gtr(u8"模组 %1").arg(i + 1));
            QFrame *card = new QFrame(ui->groupBox_gripperDown);
            card->setObjectName(QStringLiteral("heightTuneCard"));
            card->setStyleSheet(heightQss);
            QVBoxLayout *v = new QVBoxLayout(card);
            v->setContentsMargins(8, 8, 8, 8);
            v->setSpacing(6);
            modLab[i]->setParent(card);
            modSp[i]->setParent(card);
            modLab[i]->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
            modLab[i]->setAlignment(Qt::AlignCenter);
            modLab[i]->setWordWrap(true);
            modLab[i]->show();
            modSp[i]->show();
            modSp[i]->setMinimumWidth(88);
            modSp[i]->setMaximumWidth(160);
            v->addWidget(modLab[i]);
            v->addWidget(modSp[i], 0, Qt::AlignHCenter);
            down->addWidget(card, row, col);
            down->setColumnStretch(col, 1);
            ++col;
        }
    }
}

static QFrame *makeNeedleItemCard(QWidget *parent, QLabel *label, QWidget *editor)
{
    QFrame *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("heightTuneCard"));
    card->setStyleSheet(QStringLiteral(
                "QFrame#heightTuneCard {"
                " background: #FFFFFF;"
                " border: 1px solid #D4EAF5;"
                " border-radius: 8px;"
                "}"));
    card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    card->setMinimumWidth(108);
    QVBoxLayout *lay = new QVBoxLayout(card);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(6);
    if (label) {
        label->setParent(card);
        label->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        label->setMinimumWidth(0);
        label->setAlignment(Qt::AlignCenter);
        label->setWordWrap(true);
        label->setStyleSheet(QStringLiteral(
                    "QLabel { color:#2C5F8A; background:transparent; border:none; }"));
        label->show();
        lay->addWidget(label);
    }
    if (editor) {
        editor->setParent(card);
        editor->setMinimumWidth(88);
        editor->setMaximumWidth(160);
        editor->show();
        lay->addWidget(editor, 0, Qt::AlignHCenter);
    }
    return card;
}

static void styleNeedleSection(QLabel *label)
{
    if (!label) {
        return;
    }
    label->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    label->setMinimumHeight(22);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setStyleSheet(QStringLiteral(
                "color:#2C5F8A; font-weight:700; background:transparent; border:none; padding-top:6px;"));
    label->show();
}

static QScrollArea *makeNeedleScroll(QWidget *parent, QWidget *inner)
{
    inner->setObjectName(QStringLiteral("needlePageBody"));
    inner->setStyleSheet(QStringLiteral(
                "QWidget#needlePageBody { background: transparent; }"));
    QScrollArea *scroll = new QScrollArea(parent);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setStyleSheet(QStringLiteral(
                "QScrollArea { background: transparent; border: none; }"
                "QScrollArea > QWidget > QWidget { background: transparent; }"));
    scroll->setWidget(inner);
    return scroll;
}

void MachineSetting::frameNeedleConfigPages()
{
    if (property("needlePagesFramed").toBool()) {
        return;
    }
    setProperty("needlePagesFramed", true);

    struct Cell {
        QWidget *widget;
        int row;
        int col;
    };

    if (ui->gridLayout_5 && ui->tab_configbloodpin) {
        QVector<Cell> cells;
        QGridLayout *grid = ui->gridLayout_5;
        for (int i = 0; i < grid->count(); ++i) {
            QLayoutItem *item = grid->itemAt(i);
            if (!item || !item->widget()) {
                continue;
            }
            Cell cell;
            cell.widget = item->widget();
            int rowSpan = 1;
            int colSpan = 1;
            grid->getItemPosition(i, &cell.row, &cell.col, &rowSpan, &colSpan);
            cells.append(cell);
        }
        while (QLayoutItem *old = grid->takeAt(0)) {
            delete old;
        }

        QWidget *body = new QWidget;
        QVBoxLayout *bodyLay = new QVBoxLayout(body);
        bodyLay->setContentsMargins(0, 0, 0, 0);
        bodyLay->setSpacing(12);

        QGroupBox *upper = new QGroupBox(gtr(u8"下针高度"), body);
        upper->setObjectName(QStringLiteral("groupBox_bloodPinUpper"));
        QGridLayout *upperGrid = new QGridLayout(upper);
        upperGrid->setHorizontalSpacing(10);
        upperGrid->setVerticalSpacing(8);
        upperGrid->setContentsMargins(8, 8, 8, 8);

        QGroupBox *lower = new QGroupBox(gtr(u8"吸样和等待"), body);
        lower->setObjectName(QStringLiteral("groupBox_bloodPinLower"));
        QGridLayout *lowerGrid = new QGridLayout(lower);
        lowerGrid->setHorizontalSpacing(10);
        lowerGrid->setVerticalSpacing(8);
        lowerGrid->setContentsMargins(8, 8, 8, 8);

        QWidget *save = ui->widgetbloodsave;
        QVector<QWidget *> used;
        for (int i = 0; i < cells.size(); ++i) {
            QLabel *label = qobject_cast<QLabel *>(cells.at(i).widget);
            if (!label || cells.at(i).widget == save) {
                continue;
            }
            QWidget *editor = nullptr;
            for (int j = 0; j < cells.size(); ++j) {
                if (cells.at(j).row == cells.at(i).row + 1
                        && cells.at(j).col == cells.at(i).col
                        && qobject_cast<QAbstractSpinBox *>(cells.at(j).widget)) {
                    editor = cells.at(j).widget;
                    break;
                }
            }
            if (!editor) {
                continue;
            }
            const int cardCol = cells.at(i).col / 2;
            QGridLayout *dest = upperGrid;
            int cardRow = 0;
            if (cells.at(i).row <= 6) {
                cardRow = (cells.at(i).row - 1) / 2;
            } else {
                dest = lowerGrid;
                cardRow = (cells.at(i).row - 9) / 2;
                if (cardRow < 0) {
                    cardRow = 0;
                }
            }
            dest->addWidget(makeNeedleItemCard(dest->parentWidget(), label, editor), cardRow, cardCol);
            used.append(label);
            used.append(editor);
        }
        for (int col = 0; col < 4; ++col) {
            upperGrid->setColumnStretch(col, 1);
            lowerGrid->setColumnStretch(col, 1);
        }
        upper->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        lower->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        bodyLay->addWidget(upper);
        bodyLay->addWidget(lower);
        bodyLay->addStretch(1);
        grid->addWidget(makeNeedleScroll(ui->tab_configbloodpin, body), 0, 0, 1, 8);
        if (save) {
            grid->addWidget(save, 1, 0, 1, 8);
        }
        for (int i = 0; i < cells.size(); ++i) {
            QWidget *widget = cells.at(i).widget;
            if (!widget || widget == save || used.contains(widget)) {
                continue;
            }
            widget->hide();
        }
        grid->setRowStretch(0, 1);
        grid->setRowStretch(1, 0);
    }

    if (ui->gridLayout_Reagent && ui->groupBox_RetagentBox) {
        QVector<Cell> cells;
        QGridLayout *grid = ui->gridLayout_Reagent;
        for (int i = 0; i < grid->count(); ++i) {
            QLayoutItem *item = grid->itemAt(i);
            if (!item || !item->widget()) {
                continue;
            }
            Cell cell;
            cell.widget = item->widget();
            int rowSpan = 1;
            int colSpan = 1;
            grid->getItemPosition(i, &cell.row, &cell.col, &rowSpan, &colSpan);
            cells.append(cell);
        }
        while (QLayoutItem *old = grid->takeAt(0)) {
            delete old;
        }
        grid->setHorizontalSpacing(10);
        grid->setVerticalSpacing(8);
        grid->setContentsMargins(8, 8, 8, 8);

        QLabel *header[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        QLabel *rowTitle[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        QWidget *editor[8][8];
        for (int r = 0; r < 8; ++r) {
            for (int c = 0; c < 8; ++c) {
                editor[r][c] = nullptr;
            }
        }
        for (int i = 0; i < cells.size(); ++i) {
            QWidget *widget = cells.at(i).widget;
            const int row = cells.at(i).row;
            const int col = cells.at(i).col;
            if (!widget || row < 0 || row >= 8 || col < 0 || col >= 8) {
                if (widget) {
                    widget->hide();
                }
                continue;
            }
            QLabel *label = qobject_cast<QLabel *>(widget);
            if (row == 0 && label) {
                header[col] = label;
            } else if (col == 0 && label) {
                rowTitle[row] = label;
            } else if (!label) {
                editor[row][col] = widget;
            } else {
                label->hide();
            }
        }
        for (int c = 0; c < 8; ++c) {
            if (header[c]) {
                header[c]->hide();
            }
        }

        int outRow = 0;
        for (int row = 1; row < 8; ++row) {
            bool any = false;
            for (int col = 1; col < 8; ++col) {
                if (editor[row][col]) {
                    any = true;
                    break;
                }
            }
            if (!any) {
                if (rowTitle[row]) {
                    rowTitle[row]->hide();
                }
                continue;
            }
            if (rowTitle[row]) {
                styleNeedleSection(rowTitle[row]);
                grid->addWidget(rowTitle[row], outRow, 0, 1, 6);
                ++outRow;
            }
            for (int col = 1; col < 8; ++col) {
                if (!editor[row][col]) {
                    continue;
                }
                QLabel *field = new QLabel(ui->groupBox_RetagentBox);
                if (header[col]) {
                    field->setText(header[col]->text());
                    field->setFont(header[col]->font());
                }
                field->setToolTip(editor[row][col]->toolTip());
                grid->addWidget(makeNeedleItemCard(ui->groupBox_RetagentBox, field, editor[row][col]),
                                outRow, col - 1);
                grid->setColumnStretch(col - 1, 1);
            }
            ++outRow;
        }
        ui->groupBox_RetagentBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }

    if (ui->gridLayout_6 && ui->tab_reagpin) {
        QWidget *save = ui->widget_makesure;
        QVector<Cell> stripCells;
        if (ui->gridLayout_reagPinParams) {
            QGridLayout *strip = ui->gridLayout_reagPinParams;
            for (int i = 0; i < strip->count(); ++i) {
                QLayoutItem *item = strip->itemAt(i);
                if (!item || !item->widget()) {
                    continue;
                }
                Cell cell;
                cell.widget = item->widget();
                int rowSpan = 1;
                int colSpan = 1;
                strip->getItemPosition(i, &cell.row, &cell.col, &rowSpan, &colSpan);
                stripCells.append(cell);
            }
            while (QLayoutItem *old = strip->takeAt(0)) {
                delete old;
            }
        }

        QGroupBox *lower = new QGroupBox(gtr(u8"清洗和下针"), ui->tab_reagpin);
        lower->setObjectName(QStringLiteral("groupBox_reagPinLower"));
        QGridLayout *lowerGrid = new QGridLayout(lower);
        lowerGrid->setHorizontalSpacing(10);
        lowerGrid->setVerticalSpacing(8);
        lowerGrid->setContentsMargins(8, 8, 8, 8);
        for (int i = 0; i < stripCells.size(); ++i) {
            QLabel *label = qobject_cast<QLabel *>(stripCells.at(i).widget);
            if (!label || stripCells.at(i).row != 0) {
                continue;
            }
            QWidget *editor = nullptr;
            for (int j = 0; j < stripCells.size(); ++j) {
                if (stripCells.at(j).row == 1 && stripCells.at(j).col == stripCells.at(i).col
                        && qobject_cast<QAbstractSpinBox *>(stripCells.at(j).widget)) {
                    editor = stripCells.at(j).widget;
                    break;
                }
            }
            if (!editor) {
                continue;
            }
            lowerGrid->addWidget(makeNeedleItemCard(lower, label, editor), 0, stripCells.at(i).col);
            lowerGrid->setColumnStretch(stripCells.at(i).col, 1);
        }
        lower->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

        if (ui->widgetFucn) {
            ui->gridLayout_6->removeWidget(ui->widgetFucn);
            ui->widgetFucn->hide();
        }
        if (ui->groupBox_RetagentBox) {
            ui->gridLayout_6->removeWidget(ui->groupBox_RetagentBox);
        }
        if (save) {
            ui->gridLayout_6->removeWidget(save);
        }

        QWidget *body = new QWidget;
        QVBoxLayout *bodyLay = new QVBoxLayout(body);
        bodyLay->setContentsMargins(0, 0, 0, 0);
        bodyLay->setSpacing(12);
        if (ui->groupBox_RetagentBox) {
            bodyLay->addWidget(ui->groupBox_RetagentBox);
        }
        bodyLay->addWidget(lower);
        bodyLay->addStretch(1);
        ui->gridLayout_6->addWidget(makeNeedleScroll(ui->tab_reagpin, body), 0, 0, 1, 5);
        if (save) {
            ui->gridLayout_6->addWidget(save, 1, 0, 1, 5);
        }
        ui->gridLayout_6->setRowStretch(0, 1);
        ui->gridLayout_6->setRowStretch(1, 0);
    }
}

static void clearLayoutItems(QLayout *layout)
{
    if (!layout) {
        return;
    }
    while (QLayoutItem *old = layout->takeAt(0)) {
        delete old;
    }
}

static void markCurveCard(QFrame *card, bool chosen)
{
    if (!card) {
        return;
    }
    card->setProperty("chosen", chosen);
    card->style()->unpolish(card);
    card->style()->polish(card);
    card->update();
}

static QFrame *makeCurveOptionCard(QWidget *parent, QCheckBox *box)
{
    QFrame *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("curveOptionCard"));
    card->setStyleSheet(QStringLiteral(
                "QFrame#curveOptionCard {"
                " background: transparent;"
                " border: none;"
                "}"
                "QFrame#curveOptionCard[chosen=\"true\"] {"
                " background: transparent;"
                " border: none;"
                "}"));
    card->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    card->setMinimumHeight(44);
    QHBoxLayout *lay = new QHBoxLayout(card);
    lay->setContentsMargins(12, 8, 12, 8);
    if (box) {
        box->setParent(card);
        GlobalData::QCheckboxSheet(box, box->text());
        box->setMinimumHeight(28);
        box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        box->show();
        lay->addWidget(box, 0, Qt::AlignVCenter);
        lay->addStretch(1);
        markCurveCard(card, box->isChecked());
        QObject::connect(box, &QCheckBox::toggled, card, [card](bool on) {
            markCurveCard(card, on);
        });
    }
    return card;
}

void MachineSetting::relayoutCurveModePage()
{
    if (property("curveModeFramed").toBool() || !ui->groupBox_curvepara) {
        return;
    }
    setProperty("curveModeFramed", true);

    ui->groupBox_curvepara->setMinimumHeight(0);
    ui->groupBox_curvepara->setMaximumWidth(QWIDGETSIZE_MAX);
    ui->groupBox_curvepara->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    if (ui->verticalLayout_7) {
        ui->verticalLayout_7->setSpacing(10);
        ui->verticalLayout_7->setContentsMargins(8, 8, 8, 8);
    }
    if (ui->gridLayout_7) {
        ui->gridLayout_7->setAlignment(ui->groupBox_curvepara, Qt::AlignTop);
    }

    if (ui->groupBox_4 && ui->horizontalLayout) {
        ui->groupBox_4->setMinimumHeight(0);
        ui->groupBox_4->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        clearLayoutItems(ui->horizontalLayout);
        ui->horizontalLayout->setSpacing(10);
        ui->horizontalLayout->setContentsMargins(8, 8, 8, 8);
        if (ui->label_basecutNum) {
            ui->label_basecutNum->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        }
        QFrame *cutCard = makeNeedleItemCard(ui->groupBox_4, ui->label_basecutNum, ui->comboBox_CutNum);
        if (ui->comboBox_CutNum) {
            ui->comboBox_CutNum->setMinimumWidth(140);
            ui->comboBox_CutNum->setMaximumWidth(200);
        }
        QFrame *avgCard = makeCurveOptionCard(ui->groupBox_4, ui->checkBox_Avg_cutnum);
        ui->horizontalLayout->addWidget(cutCard, 1);
        ui->horizontalLayout->addWidget(avgCard, 1);
    }

    if (ui->groupBox_3 && ui->verticalLayout_6) {
        ui->groupBox_3->setMinimumHeight(0);
        ui->groupBox_3->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        clearLayoutItems(ui->verticalLayout_6);
        ui->verticalLayout_6->setContentsMargins(8, 8, 8, 8);
        QGridLayout *grid = new QGridLayout();
        grid->setHorizontalSpacing(10);
        grid->setVerticalSpacing(8);
        QCheckBox *filters[4] = {
            ui->checkBox_originTestData,
            ui->checkBox_average,
            ui->checkBox_median,
            ui->checkBoxTriple
        };
        for (int i = 0; i < 4; ++i) {
            if (!filters[i]) {
                continue;
            }
            grid->addWidget(makeCurveOptionCard(ui->groupBox_3, filters[i]), i / 2, i % 2);
            grid->setColumnStretch(i % 2, 1);
        }
        ui->verticalLayout_6->addLayout(grid);
    }

    if (ui->groupBox_6 && ui->verticalLayout_9) {
        ui->groupBox_6->setMinimumHeight(0);
        ui->groupBox_6->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
        clearLayoutItems(ui->verticalLayout_9);
        ui->verticalLayout_9->setContentsMargins(8, 8, 8, 8);
        QGridLayout *grid = new QGridLayout();
        grid->setHorizontalSpacing(10);
        grid->setVerticalSpacing(8);
        QCheckBox *outputs[3] = {
            ui->checkBox_absorbance,
            ui->checkBoxExperimental,
            ui->checkBoxshowPrintAdhesion
        };
        for (int i = 0; i < 3; ++i) {
            if (!outputs[i]) {
                continue;
            }
            grid->addWidget(makeCurveOptionCard(ui->groupBox_6, outputs[i]), 0, i);
            grid->setColumnStretch(i, 1);
        }
        ui->verticalLayout_9->addLayout(grid);
    }

    if (ui->gridLayout_15 && ui->widget) {
        clearLayoutItems(ui->gridLayout_15);
        ui->widget->setMinimumHeight(0);
        ui->widget->setMaximumHeight(56);
        ui->gridLayout_15->setContentsMargins(0, 4, 0, 0);
        ui->gridLayout_15->setHorizontalSpacing(10);
        ui->gridLayout_15->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 0);
        if (ui->toolButton_SaveCutnum) {
            ui->toolButton_SaveCutnum->setMinimumSize(128, 36);
            ui->toolButton_SaveCutnum->setMaximumSize(160, 40);
            ui->toolButton_SaveCutnum->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            ui->gridLayout_15->addWidget(ui->toolButton_SaveCutnum, 0, 1);
        }
        if (ui->pushButton_Adjustcoordinates) {
            ui->pushButton_Adjustcoordinates->setMinimumSize(128, 36);
            ui->pushButton_Adjustcoordinates->setMaximumSize(160, 40);
            ui->pushButton_Adjustcoordinates->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            ui->gridLayout_15->addWidget(ui->pushButton_Adjustcoordinates, 0, 2);
        }
        ui->gridLayout_15->setColumnStretch(0, 1);
    }

    if (ui->verticalLayout_7) {
        ui->verticalLayout_7->addStretch(1);
    }
}

void MachineSetting::applyClinicalSettingsChrome()
{
    if (property("clinicalChrome").toBool()) {
        return;
    }
    setProperty("clinicalChrome", true);
    setStyleSheet(styleSheet() + QStringLiteral(
        "\nQLabel { background-color: transparent; }"));
    setWindowTitle(gtr(u8"仪器设置"));
    if (m_titleBar) {
        m_titleBar->setTitle(windowTitle());
    }

    ui->MachineSetList->setMinimumWidth(200);
    ui->MachineSetList->setMaximumWidth(228);
    ui->MachineSetList->setIconSize(QSize(18, 18));
    ui->MachineSetList->setIndentation(18);
    ui->MachineSetList->setStyleSheet(QStringLiteral(
        "QTreeWidget {"
        " background: #E8F4FD;"
        " border: none;"
        " border-right: 1px solid #D4EAF5;"
        " font-size: 16px;"
        " color: #2C5F8A;"
        " outline: none;"
        " padding: 8px 0;"
        "}"
        "QTreeWidget::item {"
        " padding: 8px 10px;"
        " margin: 2px 8px;"
        " border-radius: 6px;"
        " background: transparent;"
        "}"
        "QTreeWidget::item:hover { background: #D4EAF5; color: #2C5F8A; }"
        "QTreeWidget::item:selected { background: #B8DFF7; color: #2C5F8A; }"
        "QTreeWidget::branch { background: #E8F4FD; }"));

    QWidget *work = new QWidget(ui->widgetcenter);
    work->setObjectName(QStringLiteral("settingsWork"));
    work->setStyleSheet(QStringLiteral(
        "QWidget#settingsWork { background: #E8F4FD; }"));
    QVBoxLayout *workLay = new QVBoxLayout(work);
    workLay->setContentsMargins(0, 0, 0, 0);
    workLay->setSpacing(0);

    QFrame *head = new QFrame(work);
    head->setObjectName(QStringLiteral("settingsPageHead"));
    head->setStyleSheet(QStringLiteral(
        "QFrame#settingsPageHead { background: #E8F4FD; border: none; border-bottom: 1px solid #D4EAF5; }"
        "QLabel { background: transparent; border: none; }"));
    QVBoxLayout *headLay = new QVBoxLayout(head);
    headLay->setContentsMargins(20, 10, 20, 10);
    headLay->setSpacing(2);
    QLabel *path = new QLabel(head);
    path->setObjectName(QStringLiteral("settingsPagePath"));
    path->setStyleSheet(QStringLiteral("color:#4A7FA3; font-size:12px;"));
    QLabel *title = new QLabel(head);
    title->setObjectName(QStringLiteral("settingsPageTitle"));
    title->setStyleSheet(QStringLiteral("color:#2C5F8A; font-size:20px; font-weight:700;"));
    QLabel *hint = new QLabel(head);
    hint->setStyleSheet(QStringLiteral("color:#4A7FA3; font-size:12px;"));
    hint->setText(gtr(u8"改完请保存。测试进行中不要改动作参数。"));
    headLay->addWidget(path);
    headLay->addWidget(title);
    headLay->addWidget(hint);

    ui->horizontalLayout_center->removeWidget(ui->tabWidgetSetconfigure);
    ui->tabWidgetSetconfigure->setParent(work);
    ui->tabWidgetSetconfigure->setDocumentMode(true);
    ui->tabWidgetSetconfigure->setStyleSheet(QStringLiteral(
        "QTabWidget::pane { border: none; background: #E8F4FD; }"));
    workLay->addWidget(head);
    workLay->addWidget(ui->tabWidgetSetconfigure, 1);
    ui->horizontalLayout_center->addWidget(work, 1);

    const int pages = ui->tabWidgetSetconfigure->count();
    for (int i = 0; i < pages; ++i) {
        QWidget *page = ui->tabWidgetSetconfigure->widget(i);
        if (page && page->layout()) {
            page->layout()->setContentsMargins(16, 12, 16, 16);
            if (page->layout()->spacing() < 10) {
                page->layout()->setSpacing(12);
            }
        }
    }

    if (ui->tabWidget_config) {
        ui->tabWidget_config->setDocumentMode(true);
        if (QTabBar *configBar = ui->tabWidget_config->tabBar()) {
            configBar->setDrawBase(false);
        }
        ui->tabWidget_config->setStyleSheet(QStringLiteral(
            "QTabWidget::pane { border: none; background: transparent; }"
            "QTabBar { border: none; }"
            "QTabBar::tab {"
            " min-width: 112px; height: 34px; margin: 0 4px 0 0; padding: 0 12px;"
            " background: #E7EEF3; border: 1px solid #D3E0E8; border-radius: 6px;"
            " color: #3E5C6E;"
            "}"
            "QTabBar::tab:selected {"
            " background: #FFFFFF; color: #2C5F8A; border: 1px solid #7EC8F0; font-weight: 700;"
            "}"
            "QTabBar::tab:hover:!selected { background: #F7FBFD; }"
            "QLabel { color: #1A3344; background-color: transparent; border: none; }"
            "QSpinBox, QDoubleSpinBox {"
            " background: #FFFFFF; border: 1px solid #C5D7E4; border-radius: 6px;"
            " padding: 4px; min-height: 28px; color: #16384A;"
            "}"
            "QPushButton, QToolButton {"
            " background: #1A6F99; border: 1px solid #14597C; border-radius: 6px;"
            " color: white; min-height: 32px; padding: 4px 14px;"
            "}"
            "QPushButton:hover, QToolButton:hover { background: #14597C; }"));
    }

    const QString groupQss = QStringLiteral(
        "QGroupBox {"
        " background: transparent;"
        " border: 1px solid #D4EAF5;"
        " border-radius: 8px;"
        " margin-top: 4px;"
        " padding: 22px 8px 8px 8px;"
        " font-weight: 700;"
        " color: #2C5F8A;"
        "}"
        "QGroupBox QCheckBox { background: transparent; border: none; }"
        "QGroupBox::title {"
        " subcontrol-origin: padding;"
        " subcontrol-position: top left;"
        " left: 12px;"
        " top: 2px;"
        " padding: 0 4px;"
        " background: transparent;"
        " color: #2C5F8A;"
        "}"
        "QGroupBox QLabel { background-color: transparent; border: none; }"
        "QGroupBox QWidget#widgetFucn { background-color: transparent; border: none; }");
    frameNeedleConfigPages();
    relayoutCurveModePage();

    const QList<QGroupBox *> groups = findChildren<QGroupBox *>();
    for (QGroupBox *box : groups) {
        if (!box) {
            continue;
        }
        box->setFlat(true);
        box->setStyleSheet(groupQss);
    }

    if (m_childItem_0_1) {
        ui->MachineSetList->setCurrentItem(m_childItem_0_1);
        refreshSettingsPageTitle(m_childItem_0_1);
    }

    placeSettingsWindow();
}

void MachineSetting::placeSettingsWindow()
{
    if (!QApplication::desktop()) {
        return;
    }
    const QRect desk = QApplication::desktop()->availableGeometry(this);
    const int wantH = qBound(760, desk.height() - 140, desk.height());
    const int wantW = qBound(1300, qMax(width(), 1400), qMin(1600, qMax(1300, desk.width() - 48)));
    resize(wantW, wantH);
    setMinimumHeight(qMin(760, qMax(640, desk.height() - 160)));
    move(desk.left() + (desk.width() - width()) / 2,
         desk.top() + (desk.height() - height()) / 2);
}


