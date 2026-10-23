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
#include <algorithm>

namespace {

const int kGripperAirStableCount = 5;
const int kGripperAirOriginStableCount = 3;
const int kGripperAirMaxSpread = 120;
const int kGripperAirOriginMaxSpread = 100;
const int kGripperAirPhaseMinMs = 600;
const int kGripperAirHoldDeltaMin = 80;
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

    ui->groupBox_Airs->setTitle(gtr(u8"\u6293\u624b\u653e\u676f\u5224\u5b9a"));
    ui->label_2->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->label_8->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->labelsuckLateTimer->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->label_2->setText(gtr(u8"\u6293\u4f4f\u5224\u5b9a: \u8d1f\u538b <="));
    ui->label_8->setText(gtr(u8"\u4e22\u6389\u5224\u5b9a: \u8d1f\u538b >="));
    ui->labelsuckLateTimer->setText(gtr(u8"\u5438\u676f\u5224\u5b9a\u5ef6\u65f6(ms)"));

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
                "QLabel { background:#FFFFFF; border:1px solid #AACDE2; border-radius:6px;"
                " padding:4px 8px; font-size:13px; color:#1A4D6B; }");
    ui->label_showsuckVal->setStyleSheet(liveValStyle);
    ui->label_splitval->setStyleSheet(liveValStyle);
    ui->label_showsuckVal->setMinimumHeight(32);
    ui->label_splitval->setMinimumHeight(32);
    ui->label_showsuckVal->setText(gtr(u8"\u5438\u4f4f -"));
    ui->label_splitval->setText(gtr(u8"\u7a7a\u8f7d -"));

    ui->pushButtonopenSuck->setText(gtr(u8"(1) \u4eea\u5668\u4e3b\u52a8\u5224\u5b9a"));
    ui->pushButtonsplitAirs->setText(gtr(u8"\u53d6\u6d88\u5224\u5b9a"));
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
            m_gripperThrowDelayLabel->setText(gtr(u8"\u4e22\u676f\u5224\u5b9a\u5ef6\u65f6(ms)"));
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
                    "QLabel { background:#F2F9FC; border:1px solid #C8E0F0; border-radius:8px;"
                    " padding:8px 10px; font-size:12px; color:#1A4D6B; }"));
    }
    if (grid) {
        gridReAdd(m_gripperAirStepLabel, 6, 0, 1, 4);
    }

    if (ui->verticalSpacer_2) {
        ui->verticalSpacer_2->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    }

    if (!m_gripperAutoApplyBtn) {
        m_gripperAutoApplyBtn = new QPushButton(gtr(u8"(4) \u81ea\u52a8\u8bbe\u5b9a"), ui->groupBox_Airs);
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
    const QString step1 = gtr(u8"(1) \u70b9[\u4eea\u5668\u4e3b\u52a8\u5224\u5b9a], \u5728\u539f\u70b9\u5173\u6cf5\u8bfb\u7a33\u7a7a\u8f7d");
    const QString step2 = gtr(u8"(2) \u901a\u90531\u5f00\u8d1f\u538b\u540e\u4e00\u76f4\u8bfb, \u8bb0\u4e0b\u5438\u4f4f\u8981\u591a\u4e45");
    const QString step3 = gtr(u8"(3) \u5f03\u676f\u5b54\u5173\u8d1f\u538b\u540e\u5148\u4e0d\u62ac, \u8bb0\u4e0b\u6cc4\u538b\u8981\u591a\u4e45");
    const QString step4 = gtr(u8"(4) \u70b9[\u81ea\u52a8\u8bbe\u5b9a], \u518d\u70b9\u672c\u9875[\u4fdd\u5b58\u6293\u624b\u53c2\u6570]");
    QString s1 = step1;
    QString s2 = step2;
    QString s3 = step3;
    QString s4 = step4;
    QString now;
    if (step == 1) {
        s1 = QStringLiteral("<b style=\"color:#C0392B;\">") + step1 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u539f\u70b9\u5173\u6cf5, \u8fde\u7eed\u7a33\u5b9a\u8bfb\u6570\u4f5c\u4e3a\u7a7a\u8f7d, \u4e0d\u7528\u65e7\u7684\u6293\u4f4f\u7ebf\u3002\u8bf7\u52ff\u653e\u8bd5\u676f\u3002");
    } else if (step == 2) {
        s2 = QStringLiteral("<b style=\"color:#C0392B;\">") + step2 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u901a\u90531\u5df2\u5f00\u8d1f\u538b\u3002\u4ece\u843d\u4e0b\u5b8c\u6210\u5f00\u59cb\u8ba1\u65f6, \u8bfb\u6570\u5438\u4f4f\u540e\u518d\u53d6\u7a33\u5b9a\u503c\u3002");
    } else if (step == 22) {
        s1 = QStringLiteral("<b style=\"color:#C0392B;\">") + step1 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u539f\u70b9\u7a7a\u8f7d\u5df2\u8bfb\u53d6, \u524d\u5f80\u901a\u90531\u5438\u676f...");
    } else if (step == 3) {
        s3 = QStringLiteral("<b style=\"color:#C0392B;\">") + step3 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u5f03\u676f\u4f4d\u5df2\u5173\u8d1f\u538b, \u5148\u4e0d\u62ac\u3002\u4ece\u5173\u6cf5\u5b8c\u6210\u5f00\u59cb\u8ba1\u65f6, \u8bfb\u6570\u56de\u5230\u7a7a\u8f7d\u9644\u8fd1\u624d\u62ac\u3002");
    } else if (step == 4) {
        s4 = QStringLiteral("<b style=\"color:#C0392B;\">") + step4 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u8d1f\u538b\u7ebf\u548c\u5438\u676f\u3001\u5f03\u676f\u5ef6\u65f6\u5df2\u586b\u5165\u3002\u786e\u8ba4\u540e\u70b9\u4fdd\u5b58\u6293\u624b\u53c2\u6570\u3002");
    } else if (step == 0) {
        now = gtr(u8"\u53ef\u624b\u52a8\u4fee\u6539\u4e0a\u65b9\u9608\u503c, \u6216\u6309\u4e0a\u65b9\u6d41\u7a0b\u64cd\u4f5c\u3002");
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
    emit WriteArryCommand(cmd, suckOn ? gtr(u8"\u6253\u5f00\u8d1f\u538b") : gtr(u8"\u5173\u95ed\u8d1f\u538b"));
}

void MachineSetting::startGripperAirPoll(bool hold)
{
    if (m_gripperAirAwaitingMotion) {
        return;
    }
    if (cglobal::g_StartTesting.load()) {
        QMessageBox::information(this, gtr(u8"\u8fd0\u884c\u6682\u505c"),
                                 gtr(u8"\u6d4b\u8bd5\u4e2d\u4e0d\u80fd\u5f00\u542f\u6293\u624b\u8d1f\u538b\u3002"));
        return;
    }
    if (!cglobal::gserialConnecStatus.load()) {
        QMessageBox::information(this, gtr(u8"\u672a\u8fde\u63a5"),
                                 gtr(u8"\u8bf7\u5148\u8fde\u63a5\u4eea\u5668\u3002"));
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
    const bool wasHolding = m_gripperAirPollHold;
    stopGripperAirPoll();
    if (m_gripperAirOriginAckFallbackTimer) {
        m_gripperAirOriginAckFallbackTimer->stop();
    }
    m_gripperAirAwaitingMotion = false;
    m_gripperAirMotionKind = 0;
    if (wasHolding || m_gripperAirDetectStep != 0) {
        sendGripperVacuumProbe(false);
    }
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
                        this, gtr(u8"\u5c1a\u65e0\u8bfb\u6570"),
                        gtr(u8"\u8bf7\u5148\u5b8c\u6210\u4eea\u5668\u4e3b\u52a8\u5224\u5b9a\u5168\u6d41\u7a0b, \u6216\u624b\u52a8\u5f00/\u5173\u8d1f\u538b\u7b49\u5f85\u7a33\u5b9a\u8bfb\u6570\u3002"));
            return;
        }
        if (holdFallback >= releaseFallback) {
            QMessageBox::warning(
                        this, gtr(u8"\u53c2\u6570\u5f02\u5e38"),
                        gtr(u8"\u5438\u4f4f\u5b9e\u6d4b %1 \u5e94\u4f4e\u4e8e\u7a7a\u8f7d\u5b9e\u6d4b %2, \u8bf7\u91cd\u65b0\u5224\u5b9a\u3002")
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
        verifyNote = gtr(u8"\n[\u81ea\u68c0] \u4e0e\u6d4b\u8bd5\u5224\u636e\u5bf9\u7167: \u5438\u4f4f=%1 \u539f\u70b9\u975e\u5438\u4f4f=%2 \u5f03\u676f\u7a7a\u8f7d=%3")
                .arg(holdWouldGrab ? gtr(u8"\u901a\u8fc7") : gtr(u8"\u5f02\u5e38"))
                .arg(originNotGrab ? gtr(u8"\u901a\u8fc7") : gtr(u8"\u5f02\u5e38"))
                .arg(dumpWouldRelease ? gtr(u8"\u901a\u8fc7") : gtr(u8"\u5f02\u5e38"));
    } else {
        verifyNote = gtr(u8"\n[\u81ea\u68c0] \u4e09\u6bb5\u5b9e\u6d4b\u4e0e\u6d4b\u8bd5\u5224\u636e(<=\u6293\u4f4f / >=\u4e22\u6389)\u4e00\u81f4\u3002");
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
        QMessageBox::warning(this, gtr(u8"\u4e0b\u53d1\u5931\u8d25"),
                             gtr(u8"\u539f\u70b9\u5750\u6807\u65e0\u6548, \u8bf7\u5148\u6821\u51c6\u5750\u6807\u3002"));
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
        QMessageBox::warning(this, gtr(u8"\u4e0b\u53d1\u5931\u8d25"),
                             gtr(u8"\u901a\u90531\u6293\u624b\u4e0b\u964d\u9ad8\u5ea6\u65e0\u6548\u3002"));
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
        QMessageBox::warning(this, gtr(u8"\u4e0b\u53d1\u5931\u8d25"),
                             gtr(u8"\u5f03\u676f\u533a\u5750\u6807\u6216\u9ad8\u5ea6\u65e0\u6548\u3002"));
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
    QByteArrayList cmds = QUIUtils::backZAxisandmoveOrigin(origin);
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
        QMessageBox::warning(this, gtr(u8"\u4e0b\u53d1\u5931\u8d25"),
                             gtr(u8"\u5f03\u676f\u9636\u6bb5\u91cd\u8bd5\u6b21\u6570\u5df2\u8fbe\u4e0a\u9650\u3002"));
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
        QMessageBox::information(this, gtr(u8"\u8fd0\u884c\u6682\u505c"),
                                 gtr(u8"\u6d4b\u8bd5\u4e2d\u4e0d\u80fd\u5f00\u542f\u6293\u624b\u8d1f\u538b\u3002"));
        return;
    }
    if (!cglobal::gserialConnecStatus.load()) {
        QMessageBox::information(this, gtr(u8"\u672a\u8fde\u63a5"),
                                 gtr(u8"\u8bf7\u5148\u8fde\u63a5\u4eea\u5668\u3002"));
        return;
    }

    QPoint ch1Axis(0, 0);
    SingletonAxis::chnZoneAxisPos(READ_OPERRAT, kGripperAirCalibChannel,
                                  MOTOR_HANDS_INDEX, ch1Axis);
    if (ch1Axis.x() == 0 && ch1Axis.y() == 0) {
        QMessageBox::warning(this, gtr(u8"\u4e0b\u53d1\u5931\u8d25"),
                             gtr(u8"\u6ca1\u6709\u901a\u90531\u5750\u6807, \u8bf7\u5148\u6821\u51c6\u5750\u6807\u3002"));
        return;
    }
    if (INI_File()._gethandsdownheiht(kGripperAirCalibChannel) <= 0) {
        QMessageBox::warning(this, gtr(u8"\u4e0b\u53d1\u5931\u8d25"),
                             gtr(u8"\u901a\u90531\u6293\u624b\u4e0b\u964d\u9ad8\u5ea6\u4e3a0, \u8bf7\u5148\u8bbe\u7f6e\u3002"));
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
    ui->label_splitval->setText(gtr(u8"\u539f\u70b9\u7a7a\u8f7d %1").arg(emptyVal));
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

    const auto finishDumpRelease = [this](int releaseVal) {
        m_gripperThrowDelayMeasured = clampGripperAutoDelay(
                    m_gripperThrowSeenMs >= 0
                    ? m_gripperThrowSeenMs
                    : static_cast<int>(m_gripperAirPhaseClock.elapsed()));
        m_gripperAirReleaseAfterDump = releaseVal;
        m_liveReleaseAir = releaseVal;
        ui->label_splitval->setText(gtr(u8"\u5f03\u676f\u7a7a\u8f7d %1").arg(releaseVal));
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
                                  && m_gripperAirEmptyBaseline - avg >= kGripperAirHoldDeltaMin);
        if (!holdByDelta) {
            return;
        }
        m_gripperAirLockedHold = avg;
        m_gripperGrabDelayMeasured = clampGripperAutoDelay(
                    m_gripperGrabSeenMs >= 0
                    ? m_gripperGrabSeenMs
                    : static_cast<int>(m_gripperAirPhaseClock.elapsed()));
        m_liveHoldAir = avg;
        ui->label_showsuckVal->setText(gtr(u8"\u5438\u4f4f %1").arg(avg));
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
