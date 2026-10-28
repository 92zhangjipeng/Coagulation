#ifndef LISBRIDGE_H
#define LISBRIDGE_H

#include "lisconfig.h"

#include <QObject>
#include <QString>

class LisApiService;
class LisSendThread;
class QTimer;

class LisBridge : public QObject
{
    Q_OBJECT
public:
    static LisBridge *instance();

    void start(const QString &dbPath);
    void applyConfig();
    void notifyTestFinished(const QString &sampleId);
    void notifyReviewed(const QString &sampleId);
    void resend(const QString &sampleId);

    LisConfigData config() const;
    QString pullStatus() const;
    QString dbPath() const;

private slots:
    void pumpQueue();
    void onJobDone(QString sampleId, int ok, QString error);

private:
    explicit LisBridge(QObject *parent = nullptr);
    ~LisBridge();

    void ensureOutbox();
    void enqueue(const QString &sampleId, bool force);
    void markRow(const QString &sampleId, const QString &status, int tries, const QString &error);

    QString m_dbPath;
    LisConfigData m_cfg;
    LisApiService *m_http;
    LisSendThread *m_sender;
    QTimer *m_timer;
    bool m_busy;
    QString m_again;
    QString m_pullStatus;

    friend class LisSendThread;
};

#endif
