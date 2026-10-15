#include "machinesetting.h"
#include "ui_machinesetting.h"
#include "testing.h"
#include "loadequipmentpos.h"
#include "usbcodedispose.h"
#include "globaldata.h"

#include <QMessageBox>
#include <QGridLayout>
#include <QSpinBox>
#include <QSizePolicy>
#include <algorithm>

namespace {

const int kGripperAirStableCount = 5;
const int kGripperAirMaxSpread = 120;
const int kGripperAirPhaseMinMs = 600;
const int kGripperAirHoldDeltaMin = 80;
const int kGripperAirDropMin = 150;
const int kGripperAirReleaseTimeoutMs = 45000;
const int kGripperAirDumpMaxRetry = 2;
const int kGripperAirEmptyMatchMax = 100;
const int kGripperAirOriginSettleMs = 2500;
const int kGripperAirChannelSettleMs = 3200;
const int kGripperAirThrowSettleMs = 3500;
const int kGripperGrabApplySlack = 60;
const int kGripperReleaseApplySlack = 60;
const int kGripperThresholdMinGap = 80;
const quint8 kGripperAirCalibChannel = 0;

// detect step: 1=origin pump empty  2=ch1 hold  3=throw release

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
    int grab = holdSample + kGripperGrabApplySlack;
    int release = emptyAfterDump - kGripperReleaseApplySlack;

    if (grab >= mid) {
        grab = mid - kGripperThresholdMinGap / 2;
    }
    if (release <= mid) {
        release = mid + kGripperThresholdMinGap / 2;
    }
    if (grab >= release) {
        grab = holdSample;
        release = qMax(emptyAfterDump, holdSample + kGripperThresholdMinGap);
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

bool gripperAirPushSample(QVector<int> &samples, int value, int *outAvg)
{
    samples.push_back(value);
    while (samples.size() > kGripperAirStableCount) {
        samples.remove(0);
    }
    if (samples.size() < kGripperAirStableCount) {
        return false;
    }
    const int minVal = *std::min_element(samples.cbegin(), samples.cend());
    const int maxVal = *std::max_element(samples.cbegin(), samples.cend());
    if (maxVal - minVal > kGripperAirMaxSpread) {
        return false;
    }
    int sum = 0;
    for (int v : samples) {
        sum += v;
    }
    *outAvg = sum / kGripperAirStableCount;
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
        gridReAdd(m_gripperAirStepLabel, 5, 0, 1, 4);
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
        gridReAdd(m_gripperAutoApplyBtn, 6, 0, 1, 4);
    }

    updateGripperAirStepHint(0);
}

void MachineSetting::updateGripperAirStepHint(int step)
{
    if (!m_gripperAirStepLabel) {
        return;
    }
    const QString step1 = gtr(u8"(1) \u70b9[\u4eea\u5668\u4e3b\u52a8\u5224\u5b9a], \u5728\u539f\u70b9\u5f00\u6cf5\u8bfb\u7a33\u7a7a\u8f7d");
    const QString step2 = gtr(u8"(2) \u5230\u901a\u90531\u5f00\u8d1f\u538b\u5438\u676f, \u8bfb\u7a33\u5438\u4f4f\u8d1f\u538b");
    const QString step3 = gtr(u8"(3) \u5f03\u676f\u5b54\u4e0b\u964d\u5173\u8d1f\u538b, \u7b49\u7a7a\u8f7d\u540e\u590d\u4f4d\u56de\u539f\u70b9");
    const QString step4 = gtr(u8"(4) \u70b9[\u81ea\u52a8\u8bbe\u5b9a], \u518d\u70b9\u672c\u9875[\u4fdd\u5b58\u6293\u624b\u53c2\u6570]");
    QString s1 = step1;
    QString s2 = step2;
    QString s3 = step3;
    QString s4 = step4;
    QString now;
    if (step == 1) {
        s1 = QStringLiteral("<b style=\"color:#C0392B;\">") + step1 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u6293\u624b\u5728\u539f\u70b9\u5f00\u6cf5\u7a7a\u8f7d, \u8bf7\u52ff\u653e\u8bd5\u7ba1, \u7b49\u5f85\u8d1f\u538b\u7a33\u5b9a\u3002");
    } else if (step == 12) {
        s2 = QStringLiteral("<b style=\"color:#C0392B;\">") + step2 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u5df2\u5230\u901a\u90531, \u8bf7\u653e\u8bd5\u7ba1\u5438\u4f4f, \u7b49\u5f85\u8d1f\u538b\u7a33\u5b9a\u3002");
    } else if (step == 2) {
        s2 = QStringLiteral("<b style=\"color:#C0392B;\">") + step2 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u5438\u676f\u8d1f\u538b\u5df2\u7a33\u5b9a, \u6b63\u5728\u53bb\u5f03\u676f\u5b54...");
    } else if (step == 3) {
        s3 = QStringLiteral("<b style=\"color:#C0392B;\">") + step3 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u5df2\u5728\u5f03\u676f\u4f4d\u5173\u8d1f\u538b, \u7b49\u5f85\u6062\u590d\u7a7a\u8f7d\u503c...");
    } else if (step == 4) {
        s4 = QStringLiteral("<b style=\"color:#C0392B;\">") + step4 + QStringLiteral("</b>");
        now = gtr(u8"\u5f53\u524d: \u5224\u5b9a\u5b8c\u6210, \u786e\u8ba4\u9608\u503c\u540e\u4fdd\u5b58\u3002");
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
    const QByteArray cmd = taskDll->DLL_ZMoveSpecifiedPosition(
                MOTOR_HANDS_INDEX, downMm, 0, cmdIndex, false, downMm, false, gripperMode);
    emit WriteArryCommand(cmd, suckOn ? gtr(u8"\u6253\u5f00\u8d1f\u538b") : gtr(u8"\u5173\u95ed\u8d1f\u538b"));
}

void MachineSetting::startGripperAirPoll(bool hold)
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

    m_gripperAirPollHold = hold;
    sendGripperVacuumProbe(hold);

    if (!m_gripperAirPollTimer) {
        m_gripperAirPollTimer = new QTimer(this);
        m_gripperAirPollTimer->setInterval(500);
        connect(m_gripperAirPollTimer, &QTimer::timeout, this, [this]() {
            if (m_gripperAirPollHold) {
                sendGripperVacuumProbe(true);
            } else {
                sendGripperVacuumProbe(false);
            }
        });
    }
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

void MachineSetting::cancelGripperAirDetect()
{
    const bool wasHolding = m_gripperAirPollHold;
    stopGripperAirPoll();
    if (wasHolding || m_gripperAirDetectStep != 0) {
        sendGripperVacuumProbe(false);
    }
    m_gripperAirDetectStep = 0;
    m_gripperAirEmptyBaseline = -1;
    m_gripperAirReleaseAfterDump = -1;
    m_gripperAirLockedHold = -1;
    m_gripperAirDumpRetry = 0;
    m_gripperAirSettleMs = kGripperAirPhaseMinMs;
    m_gripperAirSamples.clear();
    updateGripperAirStepHint(0);
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

    QMessageBox::information(
                this, gtr(u8"\u5df2\u81ea\u52a8\u8bbe\u5b9a"),
                gtr(u8"\u6293\u4f4f\u5224\u5b9a <= %1 (\u5438\u676f\u7a33\u6001 %2)\n\u4e22\u6389\u5224\u5b9a >= %3 (\u5f03\u676f\u540e\u7a7a\u8f7d %4, \u539f\u70b9\u5f00\u6cf5\u7a7a\u8f7d %5)\n\u8bf7\u70b9\u672c\u9875[\u4fdd\u5b58\u6293\u624b\u53c2\u6570]\u5199\u5165\u4eea\u5668\u3002")
                .arg(grabThreshold)
                .arg(holdSample >= 0 ? QString::number(holdSample) : QStringLiteral("-"))
                .arg(releaseThreshold)
                .arg(emptyDump >= 0 ? QString::number(emptyDump) : QStringLiteral("-"))
                .arg(emptyOrigin >= 0 ? QString::number(emptyOrigin) : QStringLiteral("-"))
                + verifyNote);
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
    quint8 idx = 1;
    QByteArrayList cmds;
    const int safeZ = INI_File()._gethandsdownthrowcpus();
    if (safeZ > 0) {
        cmds.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, safeZ,
                                                TRAY_TESTCHANNEL, idx, false));
    }
    cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(origin, 0, 0, idx));
    emit SetParatoInstrument(cmds, gtr(u8"\u6293\u624b\u56de\u539f\u70b9\u8bfb\u7a7a\u8f7d\u8d1f\u538b"));
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

    quint8 idx = 1;
    QByteArrayList cmds;
    cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(chAxis, 0, 0, idx));
    cmds.push_back(pActive->DLL_ZMoveSpecifiedPosition(
                       MOTOR_HANDS_INDEX, grabH, 0, idx, false, grabH, false, GRABCUPWITHGRIPPER));
    emit SetParatoInstrument(cmds, gtr(u8"\u6293\u624b\u5230\u901a\u90531\u5438\u676f\u5224\u5b9a"));
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

    quint8 idx = 2;
    QByteArrayList cmds;
    cmds.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, throwH,
                                            TRAY_TESTCHANNEL, idx, GRABCUPWITHGRIPPER));
    cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(
                       throwAxis, OFFSET_ZHANDS, TRAY_TESTENPTY, idx));
    cmds.push_back(pActive->DLL_ZMoveSpecifiedPosition(
                       MOTOR_HANDS_INDEX, throwH, 0, idx, false, throwH, false, GRABCUPWITHGRIPPER));
    quint8 splitIdx = static_cast<quint8>(HANDS_SPLIT_NUM);
    cmds.push_back(pActive->DLL_ZMoveSpecifiedPosition(
                       MOTOR_HANDS_INDEX, throwH, 0, splitIdx, false, throwH, false, GRIPPERNORMAL));
    m_gripperAirProbeHeight = throwH;
    emit SetParatoInstrument(cmds, gtr(u8"\u6293\u624b\u5f03\u676f\u9636\u6bb5"));
    updateGripperAirStepHint(3);
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
    quint8 idx = 3;
    QByteArrayList cmds;
    if (throwH > 0) {
        cmds.push_back(pActive->DLL_ZAxis_Reset(MOTOR_HANDS_INDEX, throwH, TRAY_TESTCHANNEL, idx, false));
    }
    if (origin.x() != 0 || origin.y() != 0) {
        cmds.push_back(pActive->DLL_XYMoveSpecifiedPosition(origin, 0, 0, idx));
    }
    emit SetParatoInstrument(cmds, gtr(u8"\u6293\u624b\u590d\u4f4d\u5e76\u56de\u539f\u70b9"));
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

    sendGripperOriginSequence();
    startGripperAirPoll(true);
    updateGripperAirStepHint(1);
    QMessageBox::information(this, gtr(u8"\u6293\u624b\u8d1f\u538b"),
                             gtr(u8"\u5148\u5728\u539f\u70b9\u5f00\u6cf5\u8bfb\u7a7a\u8f7d\u8d1f\u538b\u5e76\u7b49\u5f85\u7a33\u5b9a, \u6b64\u9636\u6bb5\u8bf7\u52ff\u653e\u8bd5\u7ba1\u3002"));
}

void MachineSetting::feedGripperAirSample(bool issuck, int airval)
{
    if (m_gripperAirDetectStep == 0) {
        return;
    }
    if (m_gripperAirPhaseClock.elapsed() < m_gripperAirSettleMs) {
        return;
    }

    int avg = 0;
    if (!gripperAirPushSample(m_gripperAirSamples, airval, &avg)) {
        return;
    }

    if (m_gripperAirDetectStep == 1 && issuck) {
        m_gripperAirEmptyBaseline = avg;
        ui->label_splitval->setText(gtr(u8"\u539f\u70b9\u5f00\u6cf5\u7a7a\u8f7d %1").arg(avg));
        m_gripperAirDetectStep = 2;
        m_gripperAirSamples.clear();
        m_gripperAirSettleMs = kGripperAirChannelSettleMs;
        m_gripperAirPhaseClock.restart();
        sendGripperChannelGrabSequence();
        startGripperAirPoll(true);
        updateGripperAirStepHint(12);
        QMessageBox::information(this, gtr(u8"\u6293\u624b\u8d1f\u538b"),
                                 gtr(u8"\u7a7a\u8f7d\u8d1f\u538b\u5df2\u7a33\u5b9a, \u6293\u624b\u5c06\u5230\u901a\u90531, \u8bf7\u653e\u8bd5\u7ba1\u5438\u4f4f\u3002"));
        return;
    }

    if (m_gripperAirDetectStep == 2 && issuck) {
        if (m_gripperAirEmptyBaseline >= 0
                && m_gripperAirEmptyBaseline - avg < kGripperAirHoldDeltaMin) {
            return;
        }
        m_gripperAirLockedHold = avg;
        m_liveHoldAir = avg;
        ui->label_showsuckVal->setText(gtr(u8"\u5438\u4f4f %1").arg(avg));
        m_gripperAirDetectStep = 3;
        m_gripperAirSamples.clear();
        m_gripperAirSettleMs = kGripperAirThrowSettleMs;
        m_gripperAirPhaseClock.restart();
        stopGripperAirPoll();
        sendGripperDumpSequence();
        startGripperAirPoll(false);
        updateGripperAirStepHint(2);
        return;
    }

    if (m_gripperAirDetectStep == 3 && !issuck) {
        const bool risenFromHold = (m_gripperAirLockedHold < 0)
                || (avg - m_gripperAirLockedHold >= kGripperAirDropMin);
        const bool nearEmpty = (m_gripperAirEmptyBaseline < 0)
                || (avg >= m_gripperAirEmptyBaseline - kGripperAirEmptyMatchMax);
        if (!risenFromHold || !nearEmpty) {
            if (m_gripperAirPhaseClock.elapsed() > kGripperAirReleaseTimeoutMs) {
                sendGripperDumpRetry();
                m_gripperAirSamples.clear();
                m_gripperAirSettleMs = kGripperAirThrowSettleMs;
                m_gripperAirPhaseClock.restart();
            }
            return;
        }
        m_gripperAirReleaseAfterDump = avg;
        m_liveReleaseAir = avg;
        ui->label_splitval->setText(gtr(u8"\u5f03\u676f\u7a7a\u8f7d %1").arg(avg));
        sendGripperReturnOriginAfterDetect();
        finishGripperAirDetect();
    }
}
