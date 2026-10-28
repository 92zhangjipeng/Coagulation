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
                "QLabel { background:transparent; border:none;"
                " padding:4px 8px; font-size:13px; color:#2C5F8A; }");
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

    const QString scanTip = gtr(u8"\u52a0\u6837\u548c\u6d4b\u9ad8\u65f6\u8bfb\u8bd5\u7ba1\u4e0a\u7684\u6761\u7801\u3002\u5173\u6389\u5c31\u7528\u624b\u8f93\u6837\u672c\u53f7\u3002");
    tip(ui->UseBarCode, nullptr, scanTip);

    const QString reserveTip = gtr(u8"\u540c\u4e00\u79cd\u8bd5\u5242\u53ef\u4ee5\u653e\u5de6\u8fb9\u548c\u53f3\u8fb9\u4e24\u4e2a\u4f4d\u7f6e\u3002\n\u6253\u5f00\u540e\uff1a\u5de6\u8fb9\u518d\u5438\u4e00\u9488\u4f1a\u4f4e\u4e8e\u8bd5\u5242\u62a5\u8b66\u9650\uff0c\u5e76\u4e14\u53f3\u8fb9\u540c\u79cd\u8bd5\u5242\u8fd8\u4e0d\u4f4e\u4e8e\u62a5\u8b66\u9650\uff0c\u8fd9\u4e00\u9488\u5c31\u6539\u5438\u53f3\u8fb9\uff0c\u4e2d\u95f4\u4e0d\u6682\u505c\u3002\u4e24\u8fb9\u90fd\u4e0d\u591f\u624d\u6682\u505c\u3002\n\u62a5\u8b66\u9650\u586b 0 \u65f6\uff0c\u5de6\u8fb9\u7a7a\u4e86\u4e5f\u4e0d\u4f1a\u5207\u6362\u3002\u5173\u6389\u5219\u53ea\u7528\u5de6\u8fb9\u3002");
    tip(ui->checkBox_UesSecondReagentHole, nullptr, reserveTip);

    ui->checkBox_catchcups->setText(gtr(u8"\u5f00\u673a\u667a\u80fd\u5f03\u676f"));
    const QString bootCupTip = gtr(u8"\u52fe\u9009\u540e\uff0c\u8c03\u5149\u53d1\u73b0\u901a\u9053\u91cc\u53ef\u80fd\u8fd8\u6709\u8bd5\u676f\u65f6\uff0c\u4f1a\u81ea\u52a8\u628a\u8fd9\u4e9b\u676f\u5b50\u4e22\u6389\uff0c\u6700\u591a\u4e24\u8f6e\u3002\u8bfb\u6570\u4e3a 0 \u4e0d\u4e22\uff0c\u90a3\u662f\u6a21\u7ec4\u6ca1\u6709\u5149\u3002\n\u4e0d\u52fe\u9009\u5219\u8c03\u5149\u65f6\u4e0d\u4e22\u676f\uff0c\u53ea\u5728\u8c03\u5149\u5931\u8d25\u7a97\u53e3\u91cc\u63d0\u793a\u3002");
    tip(ui->checkBox_catchcups, nullptr, bootCupTip);

    ui->checkBox_suck_offset->setText(gtr(u8"\u5438\u6837\u524d\u5148\u5438\u7a7a\u6c14"));
    const QString airFirstTip = gtr(u8"\u52fe\u9009\u540e\uff0c\u5438\u8840\u6837\u524d\u5148\u5438\u4e00\u6bb5\u7a7a\u6c14\uff0c\u628a\u9488\u7ba1\u91cc\u4e0a\u6b21\u7559\u4e0b\u7684\u6db2\u4f53\u9694\u5f00\u3002\u4fdd\u5b58\u4eea\u5668\u914d\u7f6e\u65f6\u5199\u5230\u4e3b\u677f\u3002\n\u4ee5\u524d\u53eb\u300c\u6821\u51c6\u5438\u6837\u300d\uff0c\u5bb9\u6613\u770b\u6210\u6821\u51c6\u5750\u6807\u3002");
    tip(ui->checkBox_suck_offset, nullptr, airFirstTip);

    const QString channelTip = gtr(u8"\u52fe\u4e0a\u7684\u901a\u9053\u53ef\u4ee5\u6392\u6837\u3001\u6d4b\u8bd5\u548c\u8c03\u5149\u3002\u5173\u6389\u7684\u901a\u9053\u4e0d\u4f1a\u88ab\u5206\u914d\u6837\u672c\u3002");
    if (ui->usechannel) {
        const QList<QCheckBox *> channels = ui->usechannel->findChildren<QCheckBox *>();
        for (QCheckBox *box : channels) {
            tip(box, nullptr, channelTip);
        }
        ui->usechannel->setToolTip(channelTip);
    }

    const QString temp1 = gtr(u8"\u6a21\u7ec4 1 \u7684\u76ee\u6807\u6e29\u5ea6\uff0c\u5355\u4f4d\u2103\uff0c\u7ba1\u901a\u9053 1 \u5230 4\u3002\u70b9\u4fdd\u5b58\u540e\u5199\u5165\u4e3b\u677f\uff0c\u6a21\u7ec4\u6309\u8fd9\u4e2a\u6e29\u5ea6\u91cd\u65b0\u9501\u6e29\u3002");
    const QString temp2 = gtr(u8"\u6a21\u7ec4 2 \u7684\u76ee\u6807\u6e29\u5ea6\uff0c\u5355\u4f4d\u2103\uff0c\u7ba1\u901a\u9053 5 \u5230 8\u3002\u70b9\u4fdd\u5b58\u540e\u5199\u5165\u4e3b\u677f\uff0c\u6a21\u7ec4\u6309\u8fd9\u4e2a\u6e29\u5ea6\u91cd\u65b0\u9501\u6e29\u3002");
    const QString temp3 = gtr(u8"\u6a21\u7ec4 3 \u7684\u76ee\u6807\u6e29\u5ea6\uff0c\u5355\u4f4d\u2103\uff0c\u7ba1\u901a\u9053 9 \u5230 12\u3002\u70b9\u4fdd\u5b58\u540e\u5199\u5165\u4e3b\u677f\uff0c\u6a21\u7ec4\u6309\u8fd9\u4e2a\u6e29\u5ea6\u91cd\u65b0\u9501\u6e29\u3002");
    tip(ui->label_Moduletemperature_1, ui->doubleSpinBox_Moduletemperature_1, temp1);
    tip(ui->label_Moduletemperature_2, ui->doubleSpinBox_Moduletemperature_2, temp2);
    tip(ui->label_Moduletemperature_3, ui->doubleSpinBox_Moduletemperature_3, temp3);
    tip(ui->Machine_conf_save, nullptr, gtr(u8"\u628a\u901a\u9053\u5f00\u5173\u3001\u4e0a\u9762\u51e0\u4e2a\u52fe\u9009\u548c\u4e09\u5757\u6a21\u7ec4\u6e29\u5ea6\u5199\u5230\u672c\u673a\uff0c\u5e76\u4e0b\u53d1\u4e3b\u677f\u3002"));

    ui->label_EmptyTubeHeigh->setText(gtr(u8"\u8bd5\u676f\u5410\u6837\u4e0b\u9488"));
    tip(ui->label_EmptyTubeHeigh, ui->EmptyHeigh, gtr(u8"\u6837\u672c\u9488\u628a\u8840\u6837\u5410\u8fdb\u7a7a\u8bd5\u676f\u65f6\u7684\u56fa\u5b9a\u4e0b\u964d\u9ad8\u5ea6\uff0c\u5355\u4f4d\u6beb\u7c73\uff0c\u4e0d\u505a\u6db2\u9762\u63a2\u6d4b\u3002\n\u70b9\u51fb\u540e\u5230\u8bd5\u7ba1\u76d8\u56db\u89d2\u5404\u4e0b\u4e00\u6b21\u8fd9\u4e2a\u9ad8\u5ea6\uff0c\u770b\u4f1a\u4e0d\u4f1a\u624e\u5e95\u6216\u591f\u4e0d\u7740\u3002"));

    ui->label_failedlinque_3->setText(gtr(u8"\u6d17\u6db2\u63a2\u4e0d\u5230\u65f6\u4e0b\u9488"));
    tip(ui->label_failedlinque_3, ui->spinBox_CleanLinqueFailedHigh, gtr(u8"\u6d17\u6837\u672c\u9488\u3001\u5438\u5185\u90e8\u6e05\u6d17\u6db2\u65f6\u505a\u6db2\u9762\u63a2\u6d4b\u3002\u63a2\u5230\u6db2\u9762\u5c31\u505c\uff1b\u4e00\u76f4\u63a2\u4e0d\u5230\u624d\u4e0b\u5230\u8fd9\u4e2a\u9ad8\u5ea6\u3002\n\u4e0d\u662f\u5e73\u65f6\u5410\u6837\u7684\u6df1\u5ea6\u3002\u70b9\u51fb\u540e\u5230\u6e05\u6d17\u4f4d\u6309\u8fd9\u4e2a\u9ad8\u5ea6\u8bd5\u4e00\u6b21\u3002"));

    ui->label_3->setText(gtr(u8"\u8840\u6d46\u6a21\u5f0f\u56fa\u5b9a\u4e0b\u9488"));
    tip(ui->label_3, ui->Testheighdownheigh, gtr(u8"\u8840\u6d46\u6a21\u5f0f\u4e0d\u63a2\u6db2\u9762\uff0c\u6837\u672c\u9488\u76f4\u63a5\u4e0b\u5230\u8fd9\u4e2a\u56fa\u5b9a\u9ad8\u5ea6\u3002\u6279\u91cf\u52a0\u6837\u65f6\u7684\u9ed8\u8ba4\u4e0b\u9488\u9ad8\u5ea6\u4e5f\u662f\u5b83\u3002\n\u70b9\u51fb\u540e\u5230\u8840\u6837\u5b54 2 \u6309\u8fd9\u4e2a\u9ad8\u5ea6\u8bd5\u4e00\u6b21\u3002"));

    ui->label_failedlinque->setText(gtr(u8"\u5438\u8840\u63a2\u6db2\u5931\u8d25\u9ad8\u5ea6"));
    tip(ui->label_failedlinque, ui->spinBox_faliedlinque, gtr(u8"\u5438\u8840\u6837\u65f6\u6837\u672c\u9488\u505a\u6db2\u9762\u63a2\u6d4b\uff0c\u63a2\u5230\u6db2\u9762\u5c31\u505c\u3002\u63a2\u4e0d\u5230\u5c31\u4e0b\u5230\u8fd9\u4e2a\u9ad8\u5ea6\u4e3a\u6b62\uff0c\u907f\u514d\u624e\u7a7f\u8bd5\u7ba1\u3002\u8840\u6d46\u6a21\u5f0f\u4e0d\u7528\u8fd9\u9879\u3002\n\u4e3b\u677f\u4e0a\u8fd9\u4e00\u9879\u4e5f\u53eb\u8d2b\u8840\u63a2\u6d4b\u5931\u8d25\u9ad8\u5ea6\u3002\u70b9\u51fb\u540e\u5230\u8840\u6837\u5b54 1 \u6309\u8fd9\u4e2a\u9ad8\u5ea6\u8bd5\u4e00\u6b21\u3002"));

    tip(ui->label_SecurityValue, ui->SecurityValue_box, gtr(u8"\u6837\u672c\u6cf5\u6362\u5411\u65f6\u7a7a\u8d70\u7684\u8865\u507f\uff0c\u754c\u9762\u4e0a\u4ecd\u53eb\u7a7a\u56de\u503c\u3002\u5410\u6837\u65f6\u6309 \u7a7a\u56de\u503c/0.347 \u6298\u6210\u6b65\u6570\u8865\u4e0a\u3002\u504f\u5c0f\u5bb9\u6613\u5410\u4e0d\u51c0\uff0c\u504f\u5927\u5c31\u591a\u5410\u7a7a\u6c14\u3002"));
    tip(ui->Poor_blood_changliang, ui->poorBlood_changliang, gtr(u8"\u6bcf\u4e00\u4efd\u8840\u6837\u8981\u5438\u7684\u5fae\u5347\u6570\u3002\u771f\u6b63\u7684\u6cf5\u6b65\u6570\u8fd8\u8981\u518d\u4e58 PPP \u7cfb\u6570\u6216 PRP \u7cfb\u6570\u3002"));
    tip(ui->label_ratio_ben, ui->doubleSpinBox_Ratio_ben, gtr(u8"\u628a PPP \u6837\u672c\u91cf\u6362\u7b97\u6210\u6837\u672c\u6cf5\u6b65\u6570\u3002\u6b65\u6570\u5927\u7ea6\u7b49\u4e8e \u6837\u672c\u91cf \u00d7 \u672c\u7cfb\u6570\u3002\u53ea\u6539\u6cf5\u8d70\u591a\u5c11\uff0c\u4e0d\u6539\u4e0a\u9762\u7684\u5fae\u5347\u6570\u3002\n\u53ea\u505a\u4e00\u4e2a\u9879\u76ee\u65f6\uff0c\u5438 PRP \u4e5f\u8d70\u8fd9\u4e2a\u7cfb\u6570\u3002"));
    tip(ui->PRPratiolable, ui->doubleSpinBox_PRPratio, gtr(u8"\u4e00\u6b21\u62bd\u8840\u3001\u591a\u4e2a\u8bd5\u5242\u5438 PRP \u65f6\uff0c\u7528\u8fd9\u4e2a\u7cfb\u6570\u628a\u6837\u672c\u91cf\u6362\u7b97\u6210\u6cf5\u6b65\u6570\u3002\u53ea\u505a\u4e00\u4e2a\u9879\u76ee\u65f6\u4e0d\u8d70\u8fd9\u9879\uff0c\u8d70 PPP \u7cfb\u6570\u3002"));
    tip(ui->labelPeRatio, ui->doubleSpinBoxAddRatio, gtr(u8"\u6027\u80fd\u8bc4\u4f30\u52a0\u6807\u51c6\u54c1\u65f6\uff0c\u5728\u6309 PPP \u7cfb\u6570\u7b97\u51fa\u7684\u5438\u91cf\u4e4b\u5916\uff0c\u518d\u591a\u5438\u8fd9\u4e2a\u767e\u5206\u6bd4\u3002\u586b 10 \u5c31\u662f\u591a\u5438 10%\u3002\u75c5\u4eba\u6b63\u5f0f\u6d4b\u8bd5\u4e0d\u8d70\u8fd9\u9879\u3002"));
    tip(ui->label_fixedhigh, ui->FixedHighvalue, gtr(u8"\u5168\u8840\u6d4b\u9ad8\u7528\u7684\u57fa\u51c6\u9ad8\u5ea6\uff0c\u5c31\u662f\u9488\u5230\u57fa\u51c6\u7269\u7684\u8ddd\u79bb\uff0c\u5355\u4f4d\u6beb\u7c73\u3002\u548c\u6d4b\u9ad8\u504f\u79fb\u3001\u6a21\u5757\u5e95\u90e8\u8ddd\u79bb\u4e00\u8d77\u7b97\u51fa\u8be5\u4e0b\u591a\u6df1\u3002"));
    tip(ui->OffsetTestHeight, ui->OffsetTestHeightValue, gtr(u8"\u5168\u8840\u6d4b\u9ad8\u7684\u5b89\u5168\u4f59\u91cf\uff0c\u5355\u4f4d\u6beb\u7c73\u3002\u7b97\u51fa\u6765\u7684\u4e0b\u9488\u6df1\u5ea6\u4f1a\u518d\u51cf\u6389\u5b83\uff0c\u907f\u514d\u9488\u624e\u8fdb\u7ea2\u7ec6\u80de\u5c42\u3002"));
    tip(ui->labelRefToBottom, ui->doubleSpinBoxBottomHeigh, gtr(u8"\u6d4b\u9ad8\u6a21\u5757\u5230\u8bd5\u7ba1\u5e95\u7684\u53c2\u7167\u8ddd\u79bb\uff0c\u5355\u4f4d\u6beb\u7c73\u3002\u548c\u5168\u8840\u6d4b\u9ad8\u9ad8\u5ea6\u3001\u504f\u79fb\u4e00\u8d77\u7b97\u9488\u8be5\u4e0b\u591a\u6df1\u3002\u4fdd\u5b58\u540e\u5199\u5230\u4e3b\u677f\u6269\u5c55\u53c2\u6570\u3002"));
    tip(ui->label_AbsorbX2, ui->spinBoxAbsorbX2, gtr(u8"\u6bcf\u6d17\u4e00\u6b21\u6837\u672c\u9488\uff0c\u4ece\u5185\u90e8\u6e05\u6d17\u6db2\u74f6\u91cc\u5438\u8d70\u7684\u5fae\u5347\u3002\u754c\u9762\u4e0a\u7684\u6e05\u6d17\u6db2\u767e\u5206\u6bd4\u6309\u8fd9\u4e2a\u6570\u6263\uff0c\u4e0d\u6309\u6cf5\u6b65\u6570\u6263\u3002"));
    tip(ui->labelwashesTime, ui->spinBox_WashesTime, gtr(u8"\u6837\u672c\u9488\u6216\u8bd5\u5242\u9488\u628a\u6e05\u6d17\u6db2\u5410\u56de\u6e05\u6d17\u6c60\u65f6\uff0c\u6cf5\u4fdd\u6301\u51b2\u6d17\u7684\u6beb\u79d2\u6570\u3002\u6d17\u53cc\u9488\u3001\u53ea\u6d17\u6837\u672c\u9488\u3001\u53ea\u6d17\u8bd5\u5242\u9488\u90fd\u7528\u8fd9\u4e00\u9879\u3002"));
    tip(ui->label_suckairs, ui->spinBox_suckairs, gtr(u8"\u5438 PRP \u524d\u8981\u989d\u5916\u5438\u5165\u7684\u7a7a\u6c14\u91cf\uff0c\u70b9\u4fdd\u5b58\u4f1a\u5199\u56de\u4e3b\u677f\uff0c\u7528\u6765\u9694\u5f00\u9488\u7ba1\u91cc\u4e0a\u6b21\u7684\u6b8b\u6db2\u3002\n\u4e0a\u4f4d\u673a\u73b0\u5728\u7ec4\u5438 PRP \u52a8\u4f5c\u65f6\uff0c\u9694\u5f00\u6b8b\u6db2\u7528\u7684\u662f\u56fa\u5b9a\u56de\u5438\uff0c\u6539\u8fd9\u4e2a\u6570\u4e0d\u4f1a\u9a6c\u4e0a\u6539\u53d8\u90a3\u4e00\u6bb5\u6cf5\u6b65\u3002"));
    tip(ui->label_PPPdelay, ui->spinBox_PPPdelay, gtr(u8"\u8840\u6837\u5410\u8fdb\u8bd5\u676f\u4e4b\u540e\uff0c\u7b49\u8fd9\u4e48\u591a\u6beb\u79d2\u518d\u53d1\u4e0b\u4e00\u6761\u62ac\u9488\u3002PPP \u548c PRP \u5410\u6837\u90fd\u7528\u5b83\uff0c\u907f\u514d\u6db2\u4f53\u8fd8\u6ca1\u843d\u4e0b\u5c31\u62ac\u9488\u3002\u4fdd\u5b58\u540e\u5199\u5230\u4e3b\u677f\u3002"));
    tip(ui->label_PRPdelay, ui->spinBox_PRPdelay, gtr(u8"\u8840\u6837\u5438\u5b8c\u4e4b\u540e\uff0c\u7b49\u8fd9\u4e48\u591a\u6beb\u79d2\u518d\u53d1\u4e0b\u4e00\u6761\u62ac\u9488\u3002PPP \u548c PRP \u5438\u6837\u90fd\u7528\u5b83\u3002\u4fdd\u5b58\u540e\u5199\u5230\u4e3b\u677f\u3002"));
    tip(ui->pushButtonBloodPinParasave, nullptr, gtr(u8"\u628a\u672c\u9875\u6837\u672c\u9488\u53c2\u6570\u4fdd\u5b58\uff0c\u5e76\u5199\u5165\u4e3b\u677f\u3002"));

    const QString coefTip = gtr(u8"\u53ea\u4fee\u6b63\u8bd5\u5242\u6cf5\u6b65\u6570\uff0c\u8ba9\u673a\u5668\u771f\u6b63\u5438\u5230\u4f60\u586b\u7684\u5fae\u5347\u30021.02 \u8868\u793a\u6cf5\u591a\u8d70 2% \u7684\u6b65\u3002\n\u4e0d\u53c2\u4e0e\u74f6\u5b50\u5269\u4f59\u767e\u5206\u6bd4\u3002\u767e\u5206\u6bd4\u53ea\u770b\u5438\u8bd5\u5242\u91cf\uff0c\u518d\u52a0\u4e0a\u57fa\u7840\u5438\u91cf\u6dfb\u52a0\u6bd4\u3002");
    tips(QList<QWidget *>()
         << ui->label_11
         << ui->doubleSpinBox_AA_Ratio << ui->doubleSpinBox_ADP_Ratio
         << ui->doubleSpinBox_ADR_Ratio << ui->doubleSpinBox_COL_Ratio
         << ui->doubleSpinBox_RIS_Ratio, coefTip);

    const QString addTip = gtr(u8"\u5728\u6807\u79f0\u5fae\u5347\u4e4b\u5916\u771f\u6b63\u591a\u5438\u7684\u6bd4\u4f8b\u30020.02 \u8868\u793a\u591a\u5438 2%\uff0c\u74f6\u5b50\u4f59\u91cf\u4f1a\u628a\u591a\u51fa\u6765\u7684\u6db2\u4f53\u6263\u6389\u3002\n\u548c\u5438\u8bd5\u5242\u7cfb\u6570\u4e0d\u662f\u4e00\u56de\u4e8b\uff1a\u7cfb\u6570\u53ea\u4fee\u6cf5\u6b65\uff0c\u8fd9\u4e00\u9879\u624d\u662f\u591a\u5438\u6db2\u4f53\u3002");
    tips(QList<QWidget *>()
         << ui->label_12
         << ui->doubleSpinBox_Add_AA_Ratio << ui->doubleSpinBox_Add_ADP_Ratio
         << ui->doubleSpinBox_Add_ADR_Ratio << ui->doubleSpinBox_Add_COL_Ratio
         << ui->doubleSpinBox_Add_RIS_Ratio, addTip);

    const QString volTip = gtr(u8"\u8fd9\u79cd\u8bd5\u5242\u6bcf\u6b21\u4ece\u74f6\u91cc\u5438\u8d70\u7684\u6807\u79f0\u5fae\u5347\u3002\u74f6\u5b50\u767e\u5206\u6bd4\u6309\u8fd9\u4e2a\u6570\u6263\uff1b\u82e5\u57fa\u7840\u5438\u91cf\u6dfb\u52a0\u6bd4\u5927\u4e8e 0\uff0c\u518d\u628a\u591a\u5438\u7684\u6db2\u4f53\u4e00\u5e76\u6263\u6389\u3002");
    tips(QList<QWidget *>()
         << ui->label_absorbVol
         << ui->spinBox_AA_Suck_Vol << ui->spinBox_ADP_Suck_Vol
         << ui->spinBox_ADR_Suck_Vol << ui->spinBox_COL_Suck_Vol
         << ui->spinBox_RIS_Suck_Vol, volTip);

    const QString downTip = gtr(u8"\u8bd5\u5242\u9488\u628a\u8fd9\u79cd\u8bd5\u5242\u5410\u8fdb\u6d4b\u8bd5\u901a\u9053\u65f6\u7684\u56fa\u5b9a\u9ad8\u5ea6\uff0c\u5355\u4f4d\u6beb\u7c73\uff0c\u4e0d\u63a2\u6db2\u9762\u3002\n\u70b9\u5de6\u4fa7\u84dd\u8272\u8bd5\u5242\u540d\uff0c\u4f1a\u5230 1 \u53f7\u901a\u9053\u6309\u8fd9\u4e00\u884c\u7684\u9ad8\u5ea6\u8bd5\u4e0b\u9488\u3002");
    tips(QList<QWidget *>()
         << ui->label_13
         << ui->spinBox_down_AA << ui->spinBox_down_ADP
         << ui->spinBox_down_ADR << ui->spinBox_down_COL
         << ui->spinBox_down_RIS, downTip);

    const QString capTip = gtr(u8"\u4e00\u74f6\u80fd\u88c5\u591a\u5c11\u5fae\u5347\u3002\u540c\u4e00\u79cd\u8bd5\u5242\u5de6\u8fb9\u548c\u53f3\u8fb9\u5171\u7528\u8fd9\u4e2a\u5bb9\u91cf\u3002\u5269\u4f59\u767e\u5206\u6bd4 = \u5269\u4e0b\u7684\u5fae\u5347 / \u5bb9\u91cf\u3002\n\u6e05\u6d17\u6db2\u90a3\u4e00\u884c\u662f\u5185\u90e8\u6e05\u6d17\u6db2\uff0c\u4e0d\u662f\u8bd5\u5242\u3002");
    tips(QList<QWidget *>()
         << ui->label_capacity
         << ui->spinBox_capacity_aa << ui->spinBox_capacity_adp
         << ui->spinBox_capacity_epi << ui->spinBox_capacity_col
         << ui->spinBox_capacity_ris << ui->spinBox_capacity_cleanlinque, capTip);

    const QString limitTip = gtr(u8"\u5269\u4f59\u767e\u5206\u6bd4\u4f4e\u4e8e\u8fd9\u4e2a\u6570\u5c31\u62a5\u8b66\u3002\n\u5f00\u4e86\u9884\u7559\u8bd5\u5242\u4f4d\u65f6\uff1a\u5de6\u8fb9\u518d\u5438\u4e00\u9488\u4f1a\u4f4e\u4e8e\u8fd9\u4e2a\u6570\uff0c\u53f3\u8fb9\u8fd8\u591f\uff0c\u624d\u6539\u5438\u53f3\u8fb9\u3002\u586b 0 \u65f6\u5de6\u8fb9\u7a7a\u4e86\u4e5f\u4e0d\u4f1a\u5207\u6362\u3002\u6e05\u6d17\u6db2\u7528\u540c\u4e00\u5957\u767e\u5206\u6bd4\u62a5\u8b66\u3002");
    tips(QList<QWidget *>()
         << ui->label_limit
         << ui->spinBox_limit_aa << ui->spinBox_limit_adp
         << ui->spinBox_limit_epi << ui->spinBox_limit_col
         << ui->spinBox_limit_ris << ui->spinBox_limit_cleanlinque, limitTip);
    tip(ui->label_cleanlinque, nullptr, gtr(u8"\u8fd9\u4e00\u884c\u662f\u5185\u90e8\u6e05\u6d17\u6db2\u3002\u5bb9\u91cf\u662f\u6574\u74f6\u5fae\u5347\uff0c\u62a5\u8b66\u9650\u662f\u5269\u4f59\u767e\u5206\u6bd4\u3002\u6d17\u6837\u672c\u9488\u3001\u6d17\u8bd5\u5242\u9488\u90fd\u4ece\u8fd9\u91cc\u6263\u3002"));

    ui->label_failedlinque_2->setText(gtr(u8"\u6d17\u6db2\u63a2\u4e0d\u5230\u65f6\u4e0b\u9488"));
    tip(ui->label_failedlinque_2, ui->spinBoxReagentNeedleFailehigh, gtr(u8"\u6d17\u8bd5\u5242\u9488\u3001\u5438\u5185\u90e8\u6e05\u6d17\u6db2\u65f6\u505a\u6db2\u9762\u63a2\u6d4b\u3002\u63a2\u5230\u6db2\u9762\u5c31\u505c\uff1b\u63a2\u4e0d\u5230\u624d\u4e0b\u5230\u8fd9\u4e2a\u9ad8\u5ea6\u3002\n\u4ee5\u524d\u8fd9\u4e2a\u540d\u5b57\u5199\u6210\u4e86\u8bd5\u5242\u63a2\u6d4b\u5e95\u90e8\uff0c\u5b9e\u9645\u7ba1\u7684\u662f\u6e05\u6d17\u6db2\u3002\u70b9\u51fb\u540e\u5230\u6e05\u6d17\u4f4d\u6309\u8fd9\u4e2a\u9ad8\u5ea6\u8bd5\u4e00\u6b21\u3002"));

    ui->label_failedhighreagentneedle->setText(gtr(u8"\u5438\u8bd5\u5242\u63a2\u6db2\u5931\u8d25\u9ad8\u5ea6"));
    tip(ui->label_failedhighreagentneedle, ui->spinBox_TestReagentFailedhigh, gtr(u8"\u5438\u8bd5\u5242\u65f6\u8bd5\u5242\u9488\u505a\u6db2\u9762\u63a2\u6d4b\uff0c\u63a2\u5230\u6db2\u9762\u5c31\u505c\u3002\u63a2\u4e0d\u5230\u5c31\u4e0b\u5230\u8fd9\u4e2a\u9ad8\u5ea6\u4e3a\u6b62\uff0c\u907f\u514d\u624e\u5230\u74f6\u5e95\u3002\n\u4ee5\u524d\u8fd9\u4e2a\u540d\u5b57\u5199\u6210\u4e86\u6e05\u6d17\u63a2\u6d4b\u9ad8\u5ea6\uff0c\u5b9e\u9645\u7ba1\u7684\u662f\u5438\u8bd5\u5242\u3002\u70b9\u51fb\u540e\u5230\u8bd5\u5242\u4f4d\u6309\u8fd9\u4e2a\u9ad8\u5ea6\u8bd5\u4e00\u6b21\u3002"));

    ui->label_AbsorbX1->setText(gtr(u8"\u8bd5\u5242\u9488\u6e05\u6d17\u91cf"));
    tip(ui->label_AbsorbX1, ui->spinBoxAbsorbX1, gtr(u8"\u6bcf\u6d17\u4e00\u6b21\u8bd5\u5242\u9488\uff0c\u4ece\u5185\u90e8\u6e05\u6d17\u6db2\u74f6\u5438\u8d70\u7684\u5fae\u5347\u3002\u6e05\u6d17\u6db2\u767e\u5206\u6bd4\u6309\u8fd9\u4e2a\u6570\u6263\u3002"));

    ui->label_delpms->setText(gtr(u8"\u8bfb\u521d\u503c\u524d\u7b49\u5f85"));
    tip(ui->label_delpms, ui->spinBox_deptime, gtr(u8"\u676f\u5b50\u653e\u5230\u901a\u9053\u540e\u3001\u8bfb\u805a\u96c6\u521d\u503c\u4e4b\u524d\uff0c\u989d\u5916\u518d\u7b49\u7684\u6beb\u79d2\u3002\n\u8bfb PPP \u521d\u503c\u603b\u4f1a\u52a0\u4e0a\u5b83\u3002\u8bfb PRP \u521d\u503c\u65f6 AA \u4e0d\u52a0\uff0cADP\u3001EPI\u3001COL\u3001RIS \u52a0\u3002\u4e0d\u662f\u5410\u8bd5\u5242\u540e\u7684\u62ac\u9488\u65f6\u95f4\u3002"));

    ui->label_reagentSuckLiftDelay->setText(gtr(u8"\u5438\u5b8c\u8bd5\u5242\u518d\u62ac\u9488"));
    tip(ui->label_reagentSuckLiftDelay, ui->spinBox_reagentSuckLiftDelay, gtr(u8"\u8bd5\u5242\u9488\u5438\u5b8c\u8bd5\u5242\u540e\uff0c\u7b49\u8fd9\u4e48\u591a\u6beb\u79d2\u518d\u62ac\u9488\uff0c\u907f\u514d\u9488\u5c16\u8fd8\u6302\u7740\u8bd5\u5242\u3002\n\u6539\u5b8c\u79bb\u5f00\u8f93\u5165\u6846\u5c31\u4f1a\u8bb0\u4e0b\uff1b\u70b9\u4fdd\u5b58\u8bd5\u5242\u9488\u53c2\u6570\u65f6\u4e5f\u4f1a\u518d\u5199\u4e00\u6b21\u3002\u5b58\u5728\u672c\u673a\u914d\u7f6e\u91cc\uff0c\u6d4b\u8bd5\u76f4\u63a5\u7528\u3002"));
    tip(ui->pushButton_saved, nullptr, gtr(u8"\u628a\u5438\u8bd5\u5242\u91cf\u3001\u7cfb\u6570\u3001\u6dfb\u52a0\u6bd4\u3001\u5410\u8bd5\u5242\u9ad8\u5ea6\u3001\u5bb9\u91cf\u3001\u62a5\u8b66\u9650\uff0c\u4ee5\u53ca\u5438\u5b8c\u518d\u62ac\u9488\u7684\u65f6\u95f4\u4fdd\u5b58\uff0c\u5e76\u628a\u8bd5\u5242\u53c2\u6570\u4e0b\u53d1\u4e3b\u677f\u3002"));

    if (ui->groupBox_gripperDown) {
        ui->groupBox_gripperDown->setTitle(gtr(u8"\u6293\u624b\u4e0b\u964d\u9ad8\u5ea6"));
        ui->groupBox_gripperDown->setToolTip(gtr(u8"\u6293\u624b\u5728\u8bd5\u7ba1\u76d8\u3001\u6d4b\u8bd5\u901a\u9053\u548c\u5f03\u676f\u5b54\u4e0b\u5230\u591a\u6df1\u3002\u84dd\u8272\u6807\u7b7e\u53ef\u4ee5\u70b9\uff0c\u6309\u65c1\u8fb9\u7684\u6beb\u7c73\u6570\u8bd5\u4e00\u6b21\u52a8\u4f5c\u3002"));
    }
    tip(ui->label_handthrowcups, ui->spinBox_downthrowcups, gtr(u8"\u6293\u624b\u5728\u5f03\u676f\u5b54\u677e\u624b\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\uff0c\u5355\u4f4d\u6beb\u7c73\u3002\n\u70b9\u51fb\u540e\u53ea\u5230\u5f03\u676f\u5b54\u4e0b\u5230\u8fd9\u4e2a\u9ad8\u5ea6\uff0c\u518d\u62ac\u8d77\u6765\u3002"));
    tip(ui->label_testtray_0, ui->spinBox_testtray_0, gtr(u8"\u6293\u624b\u4ece\u8bd5\u7ba1\u76d8 1 \u6293\u676f\u5b50\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\u3002\u70b9\u51fb\u540e\u5230\u8fd9\u53ea\u76d8\u7684\u56db\u89d2\u5404\u6293\u4e00\u6b21\uff0c\u518d\u5230\u5f03\u676f\u5b54\u6309\u5f03\u676f\u9ad8\u5ea6\u8bd5\u653e\uff0c\u6700\u540e\u56de\u539f\u70b9\u3002"));
    tip(ui->label_testtray_1, ui->spinBox_testtray_1, gtr(u8"\u6293\u624b\u4ece\u8bd5\u7ba1\u76d8 2 \u6293\u676f\u5b50\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\u3002\u70b9\u51fb\u540e\u5230\u8fd9\u53ea\u76d8\u7684\u56db\u89d2\u5404\u6293\u4e00\u6b21\uff0c\u518d\u5230\u5f03\u676f\u5b54\u6309\u5f03\u676f\u9ad8\u5ea6\u8bd5\u653e\uff0c\u6700\u540e\u56de\u539f\u70b9\u3002"));
    tip(ui->label_testtray_2, ui->spinBox_testtray_2, gtr(u8"\u6293\u624b\u4ece\u8bd5\u7ba1\u76d8 3 \u6293\u676f\u5b50\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\u3002\u70b9\u51fb\u540e\u5230\u8fd9\u53ea\u76d8\u7684\u56db\u89d2\u5404\u6293\u4e00\u6b21\uff0c\u518d\u5230\u5f03\u676f\u5b54\u6309\u5f03\u676f\u9ad8\u5ea6\u8bd5\u653e\uff0c\u6700\u540e\u56de\u539f\u70b9\u3002"));
    tip(ui->label_testtray_3, ui->spinBox_testtray_3, gtr(u8"\u6293\u624b\u4ece\u8bd5\u7ba1\u76d8 4 \u6293\u676f\u5b50\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\u3002\u70b9\u51fb\u540e\u5230\u8fd9\u53ea\u76d8\u7684\u56db\u89d2\u5404\u6293\u4e00\u6b21\uff0c\u518d\u5230\u5f03\u676f\u5b54\u6309\u5f03\u676f\u9ad8\u5ea6\u8bd5\u653e\uff0c\u6700\u540e\u56de\u539f\u70b9\u3002"));
    tip(ui->label_handsdownchn_0, ui->spinBox_handsdownchn_0, gtr(u8"\u6293\u624b\u628a\u676f\u5b50\u653e\u8fdb\u6a21\u7ec4 1\uff08\u901a\u9053 1 \u5230 4\uff09\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\uff0c\u8fd9\u56db\u4e2a\u901a\u9053\u5171\u7528\u3002\n\u70b9\u51fb\u540e\u5230\u901a\u9053 1 \u4e0b\u5230\u8fd9\u4e2a\u6df1\u5ea6\uff0c\u518d\u5230\u5f03\u676f\u5b54\u8bd5\u653e\uff0c\u7136\u540e\u56de\u539f\u70b9\u3002"));
    tip(ui->label_handsdownchn_1, ui->spinBox_handsdownchn_1, gtr(u8"\u6293\u624b\u628a\u676f\u5b50\u653e\u8fdb\u6a21\u7ec4 2\uff08\u901a\u9053 5 \u5230 8\uff09\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\uff0c\u8fd9\u56db\u4e2a\u901a\u9053\u5171\u7528\u3002\n\u70b9\u51fb\u540e\u5230\u901a\u9053 5 \u4e0b\u5230\u8fd9\u4e2a\u6df1\u5ea6\uff0c\u518d\u5230\u5f03\u676f\u5b54\u8bd5\u653e\uff0c\u7136\u540e\u56de\u539f\u70b9\u3002"));
    tip(ui->label_handsdownchn_2, ui->spinBox_handsdownchn_2, gtr(u8"\u6293\u624b\u628a\u676f\u5b50\u653e\u8fdb\u6a21\u7ec4 3\uff08\u901a\u9053 9 \u5230 12\uff09\u65f6\u7684\u4e0b\u964d\u6df1\u5ea6\uff0c\u8fd9\u56db\u4e2a\u901a\u9053\u5171\u7528\u3002\n\u70b9\u51fb\u540e\u5230\u901a\u9053 9 \u4e0b\u5230\u8fd9\u4e2a\u6df1\u5ea6\uff0c\u518d\u5230\u5f03\u676f\u5b54\u8bd5\u653e\uff0c\u7136\u540e\u56de\u539f\u70b9\u3002"));

    const QString grabTip = gtr(u8"\u6c14\u538b\u8bfb\u6570\u5c0f\u4e8e\u7b49\u4e8e\u8fd9\u4e2a\u503c\uff0c\u624d\u7b97\u676f\u5b50\u5438\u5728\u6293\u624b\u4e0a\u3002\u6570\u503c\u8d8a\u5c0f\uff0c\u8981\u6c42\u5438\u5f97\u8d8a\u72e0\u3002\n\u81ea\u52a8\u5224\u5b9a\u4f1a\u628a\u6d4b\u5230\u7684\u6570\u586b\u8fdb\u6765\u3002\u8981\u70b9\u4fdd\u5b58\u6293\u624b\u53c2\u6570\uff0c\u6b63\u5f0f\u6d4b\u8bd5\u624d\u4f1a\u7528\u65b0\u503c\u3002");
    tip(ui->label_2, ui->spinBoxSuckAirsMax, grabTip);
    const QString dropTip = gtr(u8"\u6c14\u538b\u8bfb\u6570\u5927\u4e8e\u7b49\u4e8e\u8fd9\u4e2a\u503c\uff0c\u624d\u7b97\u676f\u5b50\u5df2\u7ecf\u677e\u5f00\u3002\u5fc5\u987b\u660e\u663e\u9ad8\u4e8e\u6293\u4f4f\u5224\u5b9a\uff0c\u5426\u5219\u6ca1\u4e22\u6389\u4e5f\u4f1a\u5f53\u6210\u5df2\u4e22\uff0c\u62ac\u6293\u624b\u65f6\u628a\u676f\u5b50\u5e26\u8d77\u6765\u3002\n\u81ea\u52a8\u5224\u5b9a\u4f1a\u586b\u8fd9\u4e2a\u6570\u3002\u4fdd\u5b58\u540e\u6b63\u5f0f\u6d4b\u8bd5\u624d\u4f7f\u7528\u3002");
    tip(ui->label_8, ui->spinBoxSplitAirsMin, dropTip);
    const QString grabDelayTip = gtr(u8"\u6293\u624b\u4e0b\u5230\u676f\u5b50\u4e0a\u3001\u8d1f\u538b\u6253\u5f00\u4e4b\u540e\uff0c\u7b49\u8fd9\u4e48\u591a\u6beb\u79d2\u518d\u8bfb\u6c14\u538b\u3002\u592a\u77ed\u4f1a\u5728\u8fd8\u6ca1\u5438\u7262\u65f6\u5c31\u5224\u5b9a\u5931\u8d25\uff0c\u7136\u540e\u52a0\u6df1\u91cd\u6293\u3002");
    tip(ui->labelsuckLateTimer, ui->spinBox_suckLatetimer, grabDelayTip);
    const QString throwDelayTip = gtr(u8"\u5173\u6389\u8d1f\u538b\u4e4b\u540e\uff0c\u7b49\u8fd9\u4e48\u591a\u6beb\u79d2\u518d\u786e\u8ba4\u676f\u5b50\u677e\u5f00\u3002\u592a\u77ed\u4f1a\u5728\u676f\u5b50\u8fd8\u7c98\u7740\u65f6\u5c31\u62ac\u6293\u624b\uff0c\u628a\u8bd5\u676f\u5e26\u8d77\u6765\u3002\n\u4fdd\u5b58\u6293\u624b\u53c2\u6570\u65f6\uff0c\u8fd9\u4e2a\u65f6\u95f4\u4f1a\u548c\u8bd5\u5242\u6269\u5c55\u53c2\u6570\u4e00\u8d77\u5199\u5230\u4e3b\u677f\u3002");
    tip(m_gripperThrowDelayLabel, m_gripperThrowDelaySpin, throwDelayTip);
    tip(ui->checkBox_Recapture, nullptr, gtr(u8"\u5438\u676f\u5224\u5b9a\u6ca1\u8fc7\u65f6\uff0c\u518d\u52a0\u6df1\u91cd\u6293\u3002\u5173\u6389\u5219\u5931\u8d25\u540e\u8d70\u5f02\u5e38\u590d\u4f4d\uff0c\u4e0d\u518d\u8bd5\u7b2c\u4e8c\u6b21\u3002"));
    tip(ui->pushButtonopenSuck, nullptr, gtr(u8"\u4eea\u5668\u81ea\u5df1\u8d70\u4e00\u904d\u7a7a\u8f7d\u3001\u5438\u4f4f\u3001\u4e22\u6389\uff0c\u628a\u6293\u4f4f\u5224\u5b9a\u3001\u4e22\u6389\u5224\u5b9a\u3001\u5438\u676f\u5ef6\u65f6\u548c\u4e22\u676f\u5ef6\u65f6\u586b\u8fdb\u6846\u91cc\u3002\u586b\u5b8c\u4ecd\u8981\u70b9\u4fdd\u5b58\u6293\u624b\u53c2\u6570\uff0c\u624d\u4f1a\u5199\u8fdb\u4e3b\u677f\u3002"));
    tip(ui->pushButtonsplitAirs, nullptr, gtr(u8"\u505c\u4e0b\u6b63\u5728\u8fdb\u884c\u7684\u81ea\u52a8\u5224\u5b9a\u3002\u5224\u5b9a\u6ca1\u5728\u8dd1\u65f6\uff0c\u7528\u6765\u5173\u95ed\u8d1f\u538b\u5e76\u770b\u5f53\u524d\u7a7a\u8f7d\u6c14\u538b\u3002"));
    tip(ui->label_showsuckVal, nullptr, gtr(u8"\u6700\u8fd1\u4e00\u6b21\u5438\u4f4f\u65f6\u8bfb\u5230\u7684\u6c14\u538b\u3002\u6570\u503c\u8d8a\u5c0f\uff0c\u5438\u529b\u8d8a\u5927\u3002"));
    tip(ui->label_splitval, nullptr, gtr(u8"\u6700\u8fd1\u4e00\u6b21\u7a7a\u8f7d\u6216\u4e22\u6389\u65f6\u8bfb\u5230\u7684\u6c14\u538b\u3002\u5e94\u660e\u663e\u9ad8\u4e8e\u5438\u4f4f\u65f6\u7684\u6570\u3002"));
    tip(ui->pushButtonHandsParaSave, nullptr, gtr(u8"\u628a\u4e0b\u964d\u9ad8\u5ea6\u3001\u8d1f\u538b\u5224\u5b9a\u3001\u5438\u676f\u5ef6\u65f6\u548c\u4e22\u676f\u5ef6\u65f6\u4fdd\u5b58\uff0c\u5e76\u5199\u5165\u4e3b\u677f\u3002"));

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
        painter.drawText(word, Qt::AlignCenter, on ? gtr(u8"\u5f00") : gtr(u8"\u5173"));
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
            title->setText(gtr(u8"\u6a21\u7ec4 %1").arg(m + 1));
            title->setStyleSheet(titleQss);
            title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            QLabel *sub = new QLabel(headBar);
            sub->setText(gtr(u8"\u901a\u9053 %1\u2013%2").arg(m * 4 + 1).arg(m * 4 + 4));
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
                tempLab[m]->setText(gtr(u8"\u76ee\u6807\u6e29\u5ea6"));
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
        ui->usechannel->setTitle(model + gtr(u8"  \u4f7f\u7528\u901a\u9053"));
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
                " background: transparent;"
                " border: none;"
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
        ui->label_handthrowcups->setText(gtr(u8"\u5f03\u676f\u5b54"));
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
        sec->setText(gtr(u8"\u8bd5\u7ba1\u76d8"));
        sec->setStyleSheet(sectionQss);
        down->addWidget(sec, row, 0, 1, cols);
        ++row;
        int col = 0;
        for (int i = 0; i < trayCount; ++i) {
            if (!trayLab[i] || !traySp[i]) {
                continue;
            }
            trayLab[i]->setText(gtr(u8"\u8bd5\u7ba1\u76d8 %1").arg(i + 1));
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
        sec->setText(gtr(u8"\u653e\u5230\u6d4b\u8bd5\u901a\u9053"));
        sec->setStyleSheet(sectionQss);
        down->addWidget(sec, row, 0, 1, cols);
        ++row;
        int col = 0;
        for (int i = 0; i < modCount; ++i) {
            if (!modLab[i] || !modSp[i]) {
                continue;
            }
            modLab[i]->setText(gtr(u8"\u6a21\u7ec4 %1").arg(i + 1));
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
                " background: transparent;"
                " border: none;"
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

        QGroupBox *upper = new QGroupBox(gtr(u8"\u4e0b\u9488\u9ad8\u5ea6"), body);
        upper->setObjectName(QStringLiteral("groupBox_bloodPinUpper"));
        QGridLayout *upperGrid = new QGridLayout(upper);
        upperGrid->setHorizontalSpacing(10);
        upperGrid->setVerticalSpacing(8);
        upperGrid->setContentsMargins(8, 8, 8, 8);

        QGroupBox *lower = new QGroupBox(gtr(u8"\u5438\u6837\u548c\u7b49\u5f85"), body);
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

        QGroupBox *lower = new QGroupBox(gtr(u8"\u6e05\u6d17\u548c\u4e0b\u9488"), ui->tab_reagpin);
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
    setWindowTitle(gtr(u8"\u4eea\u5668\u8bbe\u7f6e"));
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
    hint->setText(gtr(u8"\u6539\u5b8c\u8bf7\u4fdd\u5b58\u3002\u6d4b\u8bd5\u8fdb\u884c\u4e2d\u4e0d\u8981\u6539\u52a8\u4f5c\u53c2\u6570\u3002"));
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


