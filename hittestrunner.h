#ifndef HITTESTRUNNER_H
#define HITTESTRUNNER_H

#include <QByteArray>
#include <QByteArrayList>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QVector>

class QElapsedTimer;
class QTimer;

struct HitSampleJob {
    int row = -1;
    QString sampleId;
    int channel = 1;
    int pppHole = 1;
};

struct HitMixSpec {
    const char *key = "";
    const char *statusUtf8 = "";
    int serumUl = 0;
    int prpUl = 0;
    int reagentUl = 0;
    int reagentHole = 0;
    quint8 reagentType = 1;
    int incubateSec = 0;
    int recordMin = 0;
};

// HIT 独立测试：运动同一时间只服务一个样本。通道进入预孵育或记录后让出运动，
// 去给其它空闲通道加样。相同通道要等当前样本做完。不占用 AA 结果表，也不置 g_StartTesting。
class HitTestRunner : public QObject
{
    Q_OBJECT

public:
    explicit HitTestRunner(QObject *parent = nullptr);
    ~HitTestRunner();

    bool running() const;
    bool paused() const;
    void setPaused(bool paused);
    bool start(const QVector<HitSampleJob> &jobs, QString *reason);

signals:
    void rowChanged(int row, const QString &status, const QString &startTime, const QString &endTime,
                    const QString &ppp, const QString &spontaneous, const QString &low,
                    const QString &high, const QString &negative);
    void stepProgress(int row, int mixIndex, int elapsedMs, int totalMs, int phase);
    void runFinished(const QString &message);
    void placeHoleConsumed(int displayHole);
    void sendOrderList(const QByteArrayList &cmds, const QString &info);

private slots:
    void onSerial(const QStringList &recv);
    void onTimer();

private:
    enum Stage {
        StageIdle = 0,
        StageTakePpp,
        StageSuckPpp,
        StageGripPpp,
        StageThrowPpp,
        StageWashPpp,
        StagePrepPrp,
        StageTakeMix,
        StageSuckSerum,
        StageWashSerum,
        StageSuckPrp,
        StageWashPrp,
        StageSuckReagent,
        StageWashReagent,
        StageGripMix,
        StageRead0,
        StageThrowMix,
        StageHome,
        StageAbortThrow,
        StageAbortHome
    };
    enum Phase {
        PhaseIdle = 0,
        PhaseAck,
        PhaseWait
    };
    enum TimerMode {
        TimerNone = 0,
        TimerPppWait,
        TimerPppRetry,
        TimerIncubate,
        TimerRead0Retry,
        TimerRecord,
        TimerStable
    };
    enum { HitCupSlots = 5 };
    struct CmdStep {
        QByteArray cmd;
        int waitAfterMs = 0;
        bool grabOpen = false;
        bool grabJudge = false;
    };

    void ensureWired();
    void loadSettings();
    struct HitLane {
        int jobIndex = -1;
        int mixIndex = 0;
        int stage = StageTakePpp;
        int cupHole = -1;
        int cups[HitCupSlots] = {-1, -1, -1, -1, -1};
        bool cupFilled[HitCupSlots] = {};
        bool cupClaimed = false;
        bool cupInChannel = false;
        bool started = false;
        bool finished = false;
        bool needsMotion = true;
        int waitKind = TimerNone;
        int readRetry = 0;
        int recordSeq = 0;
        int recordMs = 0;
        qint64 deadlineMs = 0;
        qint64 recordOriginMs = 0;
        qint64 nextRecordMs = 0;
        qint64 stepOriginMs = 0;
        int stepTotalMs = 0;
        int lastProgressSec = -1;
        qint64 runId = -1;
        quint32 pppSeq = 0;
        double ppp = 0;
        double prp0 = 0;
        double prpn = 0;
        double peakPercent = 0;
        QString startText;
    };

    void beginSample();
    void enter(int stage);
    void captureMotionLane();
    void restoreMotionLane(int index);
    void releaseForWait(int waitKind, int waitMs);
    void pump();
    int pickLane() const;
    bool reagentDue(int exceptLane) const;
    bool yieldIfReagentDue(int nextStage);
    bool channelBusy(int channel, int exceptLane) const;
    bool lanesWaiting() const;
    void serviceLanes();
    void emitStepProgress(const HitLane &lane);
    void serviceIncubate(HitLane &lane);
    void serviceStable(HitLane &lane);
    void serviceRecord(HitLane &lane);
    void finishParkedRecord(HitLane &lane);
    void savePointOf(const HitLane &lane, const QString &step, int seq, int elapsedMs,
                     double raw, double percent);
    void saveStepOf(const HitLane &lane, const QString &step, double prp0, double prpn, double percent);
    void publishJob(int jobIndex, const QString &status, const QString &startTime = QString(),
                    const QString &endTime = QString(), const QString &ppp = QString(),
                    const QString &spontaneous = QString(), const QString &low = QString(),
                    const QString &high = QString(), const QString &negative = QString());
    int readChannelOf(int channel) const;
    double percentOfLane(const HitLane &lane, double prpn, double prp0) const;
    bool readStableOf(const HitLane &lane, int &value) const;
    void beginIncubateClock();
    void startIncubate();
    void beginStableWait();
    void parkStable(HitLane &lane);
    void stopAllStir();
    void beginBatch();
    void takeCommands(const QByteArrayList &cmds);
    int findCommand(quint8 index) const;
    void sendAt(int pos);
    void onBatchDone();
    void fail(const QString &why);
    void requestGrabPrompt();
    void promptGrab();
    void startAbortHome();
    void cleanup(const QString &message);
    void publish(const QString &status, const QString &startTime = QString(),
                 const QString &endTime = QString(), const QString &ppp = QString(),
                 const QString &spontaneous = QString(), const QString &low = QString(),
                 const QString &high = QString(), const QString &negative = QString());
    bool claimCup(QString *err);
    bool claimAllCups(QString *err);
    bool planCups(QString *reason);
    void releaseCup();
    void markCupUsed();
    void markHoleUsed(int hole);
    void freeHole(int hole);
    void retireLaneCups(HitLane &lane);
    void setStir(bool on);
    void deductClean(quint8 kind);
    void notifySourceSucked(int holeOffset);
    int channel1() const;
    int readChannel() const;
    bool readStable(int &value) const;
    bool pointOk(const QPoint &point) const;
    bool bloodPoint(int displayHole, QPoint &out, QString *err) const;
    bool trayPoint(int hole, quint8 needle, QPoint &out, QString *err) const;
    bool reagentPoint(int index, QPoint &out, QString *err) const;
    bool channelReagentPoint(QPoint &out, QString *err) const;
    bool buildBloodTransfer(const QPoint &suck, const QPoint &spit, int ul, QString *err);
    bool buildReagentTransfer(const QPoint &suck, const QPoint &spit, int ul,
                              quint8 reagentType, QString *err);
    bool buildWashBlood();
    bool buildWashReagent();
    bool buildGrip(QString *err);
    bool buildThrow(QString *err);
    bool buildHome();
    void ensureDb();
    void insertRun();
    void savePpp();
    void saveStep(const QString &step, double prp0, double prpn, double percent);
    void savePoint(const QString &step, int seq, int elapsedMs, double raw, double percent);
    void closeRun(const QString &status);
    double percentOf(double prpn, double prp0) const;
    void readPpp();
    void readPrp0();
    void recordTick();
    void finishRecord();
    void armTimer(int mode, int ms, bool repeat);
    const HitMixSpec *currentMix() const;
    QString nowText() const;
    QString u8(const char *text) const;

    QTimer *m_timer = nullptr;
    QTimer *m_service = nullptr;
    QElapsedTimer *m_clock = nullptr;
    QVector<HitSampleJob> m_jobs;
    QVector<HitLane> m_lanes;
    int m_motionLane = -1;
    QVector<HitMixSpec> m_mix;
    QVector<CmdStep> m_batch;
    int m_pppUl = 150;
    int m_sampleIndex = 0;
    int m_mixIndex = 0;
    int m_stage = StageIdle;
    int m_phase = PhaseIdle;
    int m_timerMode = TimerNone;
    int m_pos = -1;
    int m_cupHole = -1;
    int m_cups[HitCupSlots] = {-1, -1, -1, -1, -1};
    bool m_cupFilled[HitCupSlots] = {};
    int m_grabDownMm = 0;
    int m_deepen = 0;
    bool m_promptingGrab = false;
    int m_readRetry = 0;
    int m_recordSeq = 0;
    int m_recordMs = 0;
    qint64 m_runId = -1;
    quint32 m_pppSeq = 0;
    quint8 m_expectIndex = 0;
    double m_ppp = 0;
    double m_prp0 = 0;
    double m_prpn = 0;
    double m_peakPercent = 0;
    bool m_running = false;
    bool m_paused = false;
    bool m_holdAdvance = false;
    int m_holdPos = -1;
    bool m_wired = false;
    bool m_failed = false;
    bool m_stopped = false;
    bool m_dbReady = false;
    bool m_cupClaimed = false;
    bool m_cupInChannel = false;
    QString m_startText;
    QString m_failText;
};

#endif
