#ifndef QCSTARTACTION_H
#define QCSTARTACTION_H

#include <QByteArray>
#include <QByteArrayList>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>

class QTimer;

// QC: water at blood hole 1 (index 0); each level has its own standard and
// sample at holes 3/4 high, 5/6 mid, 7/8 low (indexes 2-7). One empty cup per
// suck, visit all selected channels, discard, wash blood needle.
class QcStartAction : public QObject
{
    Q_OBJECT

public:
    explicit QcStartAction(QObject *parent = nullptr);
    static QcStartAction *instance();

    // level: 0 high, 1 mid, 2 low, 3 all. channels are 1-based.
    bool start(int level, const QList<int> &channels,
               const QString &highLot, const QString &midLot, const QString &lowLot,
               QString *reason);
    bool isRunning() const { return m_running; }

signals:
    void progressText(const QString &text);
    void finished(bool ok, const QString &text);
    void sendOrderList(const QByteArrayList &cmds, const QString &info);

private slots:
    void onSerial(const QStringList &recv);
    void onWaitTick();

private:
    enum JobKind {
        JobSuck = 1,
        JobGripTrayToCh,
        JobGripChToCh,
        JobGripChToTray,
        JobWash,
        JobThrow,
        JobRead
    };
    enum RecordKind {
        RecWater = 0,
        RecHighStd = 1,
        RecHigh = 2,
        RecMidStd = 3,
        RecMid = 4,
        RecLowStd = 5,
        RecLow = 6
    };
    enum Phase {
        PhaseIdle = 0,
        PhaseAck,
        PhaseStable
    };
    struct Job {
        JobKind kind;
        int bloodHole;
        int trayHole;
        int channel;
        int channelTo;
        int record;
        Job()
            : kind(JobSuck)
            , bloodHole(0)
            , trayHole(0)
            , channel(0)
            , channelTo(0)
            , record(RecWater)
        {}
    };
    struct Reading {
        int water;
        int highStd;
        int high;
        int midStd;
        int mid;
        int lowStd;
        int low;
        Reading()
            : water(-1)
            , highStd(-1)
            , high(-1)
            , midStd(-1)
            , mid(-1)
            , lowStd(-1)
            , low(-1)
        {}
    };

    void ensureWired();
    bool takeCup(int &hole, QString *reason);
    void releaseTray(int hole);
    void markCupUsed(int hole);
    void releaseAllClaimed();
    void appendCupRoute(int trayHole, int record, bool discard);
    bool appendSuckRouteWash(int bloodHole, int record, QString *reason);
    bool buildJobs(int level, QString *reason);
    bool commandForJob(const Job &job, QByteArrayList &out, QString *reason);
    void runJob();
    void advance();
    void fail(const QString &text);
    void succeed();
    void storeReading(int channel1, int record, int value);
    void setSpin(int channel0, bool on);
    QString jobText(const Job &job) const;

    bool m_wired;
    bool m_running;
    Phase m_phase;
    QVector<Job> m_jobs;
    QVector<int> m_channels;
    QVector<int> m_claimed;
    QMap<int, Reading> m_readings;
    int m_jobIndex;
    quint8 m_expectIndex;
    QVector<quint8> m_batchIndexes;
    QTimer *m_waitTimer;
    int m_waitChannel1;
    int m_waitChannel0;
    int m_waitRecord;
    quint32 m_minSeq;
    int m_waitMs;
    QString m_highLot;
    QString m_midLot;
    QString m_lowLot;
};

#endif
