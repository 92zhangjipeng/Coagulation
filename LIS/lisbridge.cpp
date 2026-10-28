#include "lisbridge.h"

#include "lisapiservice.h"
#include "lismessage.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QList>
#include <QMutex>
#include <QMutexLocker>
#include <QQueue>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTcpSocket>
#include <QTextCodec>
#include <QThread>
#include <QTimer>
#include <QWaitCondition>

#include <QsLog/include/QsLog.h>

struct LisJob
{
    QString sampleId;
    QString host;
    int port;
    int protocol;
    int timeoutMs;
    QByteArray hl7;
    QList<QByteArray> astmRecords;
};

static QByteArray encodeText(const QString &text, const QString &charset)
{
    const char *name = (charset == "GBK") ? "GBK" : "UTF-8";
    QTextCodec *codec = QTextCodec::codecForName(name);
    if (!codec && charset == "GBK") {
        codec = QTextCodec::codecForName("GB18030");
    }
    if (!codec) {
        codec = QTextCodec::codecForName("UTF-8");
    }
    if (!codec) {
        return text.toUtf8();
    }
    return codec->fromUnicode(text);
}

static QSqlDatabase outboxDb(const QString &dbPath)
{
    const QString name = "lis_outbox";
    QSqlDatabase db = QSqlDatabase::database(name, false);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", name);
        db.setDatabaseName(dbPath);
        db.setConnectOptions("QSQLITE_BUSY_TIMEOUT=3000");
    }
    if (!db.isOpen()) {
        db.open();
    }
    return db;
}

static int readCtrl(QTcpSocket &socket, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs) {
        const int left = timeoutMs - static_cast<int>(timer.elapsed());
        if (!socket.bytesAvailable() && !socket.waitForReadyRead(qMax(1, left))) {
            break;
        }
        const QByteArray chunk = socket.readAll();
        for (int i = 0; i < chunk.size(); ++i) {
            const unsigned char c = static_cast<unsigned char>(chunk.at(i));
            if (c == 0x06 || c == 0x15) {
                return c;
            }
        }
    }
    return -1;
}

static QByteArray astmFrame(int seq, const QByteArray &record)
{
    QByteArray body;
    body.append(static_cast<char>('0' + (seq % 8)));
    body.append(record);
    body.append('\r');
    body.append(static_cast<char>(0x03));
    int sum = 0;
    for (int i = 0; i < body.size(); ++i) {
        sum += static_cast<unsigned char>(body.at(i));
    }
    sum &= 0xFF;
    QByteArray frame;
    frame.append(static_cast<char>(0x02));
    frame.append(body);
    frame.append(QByteArray::number(sum, 16).rightJustified(2, '0').toUpper());
    frame.append("\r\n");
    return frame;
}

static bool sendHl7(QTcpSocket &socket, const LisJob &job, QString &error)
{
    QByteArray packet;
    packet.append(static_cast<char>(0x0B));
    packet.append(job.hl7);
    packet.append(static_cast<char>(0x1C));
    packet.append(static_cast<char>(0x0D));
    if (socket.write(packet) < 0 || !socket.waitForBytesWritten(job.timeoutMs)) {
        error = "hl7 write failed";
        return false;
    }
    QByteArray ack;
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < job.timeoutMs) {
        const int left = job.timeoutMs - static_cast<int>(timer.elapsed());
        if (!socket.waitForReadyRead(qMax(1, left))) {
            break;
        }
        ack += socket.readAll();
        if (ack.contains(static_cast<char>(0x1C))) {
            break;
        }
    }
    if (ack.contains("MSA|AA") || ack.contains("MSA|CA")) {
        return true;
    }
    if (ack.isEmpty()) {
        error = "no hl7 ack";
    } else {
        error = QString::fromLatin1(ack).remove(QChar(0x0B)).remove(QChar(0x1C)).trimmed().left(180);
    }
    return false;
}

static bool sendAstm(QTcpSocket &socket, const LisJob &job, QString &error)
{
    if (socket.write(QByteArray(1, static_cast<char>(0x05))) < 0
            || !socket.waitForBytesWritten(job.timeoutMs)) {
        error = "astm enq failed";
        return false;
    }
    if (readCtrl(socket, job.timeoutMs) != 0x06) {
        error = "no astm ack";
        return false;
    }
    for (int i = 0; i < job.astmRecords.size(); ++i) {
        const QByteArray frame = astmFrame(i + 1, job.astmRecords.at(i));
        bool acked = false;
        for (int attempt = 0; attempt < 2; ++attempt) {
            if (socket.write(frame) < 0 || !socket.waitForBytesWritten(job.timeoutMs)) {
                error = "astm write failed";
                return false;
            }
            const int ctrl = readCtrl(socket, job.timeoutMs);
            if (ctrl == 0x06) {
                acked = true;
                break;
            }
            if (ctrl != 0x15) {
                error = "astm frame rejected";
                return false;
            }
        }
        if (!acked) {
            error = "astm nak";
            return false;
        }
    }
    socket.write(QByteArray(1, static_cast<char>(0x04)));
    socket.waitForBytesWritten(job.timeoutMs);
    return true;
}

class LisSendThread : public QThread
{
public:
    explicit LisSendThread(LisBridge *bridge)
        : QThread(bridge), m_bridge(bridge), m_stop(false) {}

    void post(const LisJob &job)
    {
        QMutexLocker locker(&m_mutex);
        m_queue.enqueue(job);
        m_wait.wakeOne();
    }

    void requestStop()
    {
        QMutexLocker locker(&m_mutex);
        m_stop = true;
        m_wait.wakeOne();
    }

protected:
    void run() override
    {
        for (;;) {
            LisJob job;
            {
                QMutexLocker locker(&m_mutex);
                while (m_queue.isEmpty() && !m_stop) {
                    m_wait.wait(&m_mutex);
                }
                if (m_stop && m_queue.isEmpty()) {
                    return;
                }
                job = m_queue.dequeue();
            }
            QString error;
            bool ok = false;
            QTcpSocket socket;
            socket.connectToHost(job.host, static_cast<quint16>(job.port));
            if (!socket.waitForConnected(job.timeoutMs)) {
                error = socket.errorString();
            } else if (job.protocol == 2) {
                ok = sendAstm(socket, job, error);
            } else {
                ok = sendHl7(socket, job, error);
            }
            socket.disconnectFromHost();
            QMetaObject::invokeMethod(m_bridge, "onJobDone", Qt::QueuedConnection,
                                      Q_ARG(QString, job.sampleId),
                                      Q_ARG(int, ok ? 1 : 0),
                                      Q_ARG(QString, error));
        }
    }

private:
    LisBridge *m_bridge;
    QMutex m_mutex;
    QWaitCondition m_wait;
    QQueue<LisJob> m_queue;
    bool m_stop;
};

LisBridge *LisBridge::instance()
{
    static LisBridge *bridge = new LisBridge(qApp);
    return bridge;
}

LisBridge::LisBridge(QObject *parent)
    : QObject(parent)
    , m_http(nullptr)
    , m_sender(nullptr)
    , m_timer(nullptr)
    , m_busy(false)
{
    m_cfg = LisConfig::load();
    m_sender = new LisSendThread(this);
    m_sender->start();
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &LisBridge::pumpQueue);
    m_timer->start(5000);
}

LisBridge::~LisBridge()
{
    if (m_timer) {
        m_timer->stop();
    }
    if (m_http) {
        m_http->stop();
    }
    if (m_sender) {
        m_sender->requestStop();
        m_sender->wait(12000);
    }
}

void LisBridge::start(const QString &dbPath)
{
    m_dbPath = dbPath;
    ensureOutbox();
    applyConfig();
}

void LisBridge::ensureOutbox()
{
    if (m_dbPath.isEmpty()) return;
    QSqlDatabase db = outboxDb(m_dbPath);
    if (!db.isOpen()) {
        QLOG_WARN() << "LIS outbox open failed" << db.lastError().text();
        return;
    }
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS LisOutbox ("
               "sample_id TEXT PRIMARY KEY,"
               "status TEXT,"
               "tries INTEGER,"
               "last_error TEXT,"
               "updated_at TEXT)");
    query.exec("UPDATE LisOutbox SET status='pending' WHERE status='sending'");
}

void LisBridge::applyConfig()
{
    m_cfg = LisConfig::load();
    if (m_http) {
        m_http->stop();
        m_http->deleteLater();
        m_http = nullptr;
    }
    if (!m_cfg.enabled || !m_cfg.pullEnabled || m_dbPath.isEmpty()) {
        m_pullStatus = "off";
        return;
    }
    m_http = new LisApiService(static_cast<quint16>(m_cfg.pullPort), m_dbPath, this);
    m_http->setImageSaveDirectory("./CurveImages");
    m_http->setImageRetentionDays(60);
    m_http->start();
    if (m_http->isListening()) {
        m_pullStatus = "ok";
        QLOG_INFO() << "LIS pull listening" << m_cfg.pullPort;
    } else {
        m_pullStatus = m_http->errorString();
        QLOG_WARN() << "LIS pull listen failed" << m_pullStatus;
    }
}

LisConfigData LisBridge::config() const
{
    return m_cfg;
}

QString LisBridge::pullStatus() const
{
    return m_pullStatus;
}

QString LisBridge::dbPath() const
{
    return m_dbPath;
}

void LisBridge::notifyTestFinished(const QString &sampleId)
{
    if (!m_cfg.enabled || !m_cfg.pushEnabled || !m_cfg.sendOnTestComplete || m_cfg.sendOnReview) {
        return;
    }
    enqueue(sampleId, false);
}

void LisBridge::notifyReviewed(const QString &sampleId)
{
    if (!m_cfg.enabled || !m_cfg.pushEnabled || !m_cfg.sendOnReview) {
        return;
    }
    enqueue(sampleId, true);
}

void LisBridge::resend(const QString &sampleId)
{
    enqueue(sampleId, true);
}

void LisBridge::enqueue(const QString &sampleId, bool force)
{
    const QString id = sampleId.trimmed();
    if (id.isEmpty() || m_dbPath.isEmpty() || !m_cfg.enabled || !m_cfg.pushEnabled) {
        return;
    }
    ensureOutbox();
    QSqlDatabase db = outboxDb(m_dbPath);
    if (!db.isOpen()) return;

    const QString now = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QSqlQuery query(db);
    query.prepare("SELECT status FROM LisOutbox WHERE sample_id=?");
    query.addBindValue(id);
    if (!query.exec()) {
        QLOG_WARN() << "LIS enqueue read failed" << query.lastError().text();
        return;
    }
    if (!query.next()) {
        QSqlQuery insertQuery(db);
        insertQuery.prepare("INSERT INTO LisOutbox(sample_id,status,tries,last_error,updated_at) VALUES(?,?,0,'',?)");
        insertQuery.addBindValue(id);
        insertQuery.addBindValue("pending");
        insertQuery.addBindValue(now);
        if (!insertQuery.exec()) {
            QLOG_WARN() << "LIS enqueue insert failed" << insertQuery.lastError().text();
        }
        return;
    }
    const QString status = query.value(0).toString();
    if (status == "sending") {
        m_again = id;
        return;
    }
    if (!force && (status == "pending" || status == "sent")) {
        return;
    }
    QSqlQuery updateQuery(db);
    updateQuery.prepare("UPDATE LisOutbox SET status='pending', tries=0, last_error='', updated_at=? WHERE sample_id=?");
    updateQuery.addBindValue(now);
    updateQuery.addBindValue(id);
    updateQuery.exec();
}

void LisBridge::markRow(const QString &sampleId, const QString &status, int tries, const QString &error)
{
    QSqlDatabase db = outboxDb(m_dbPath);
    if (!db.isOpen()) return;
    QSqlQuery query(db);
    query.prepare("UPDATE LisOutbox SET status=?, tries=?, last_error=?, updated_at=? WHERE sample_id=?");
    query.addBindValue(status);
    query.addBindValue(tries);
    query.addBindValue(error.left(300));
    query.addBindValue(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"));
    query.addBindValue(sampleId);
    query.exec();
}

void LisBridge::pumpQueue()
{
    if (m_busy || m_dbPath.isEmpty() || !m_cfg.enabled || !m_cfg.pushEnabled) {
        return;
    }
    QSqlDatabase db = outboxDb(m_dbPath);
    if (!db.isOpen()) return;

    QSqlQuery query(db);
    query.prepare("SELECT sample_id, tries FROM LisOutbox WHERE status='pending' AND tries < ? "
                  "ORDER BY updated_at LIMIT 1");
    query.addBindValue(m_cfg.retryMax);
    if (!query.exec() || !query.next()) {
        return;
    }
    const QString sampleId = query.value(0).toString();
    const int tries = query.value(1).toInt();

    if (m_cfg.host.isEmpty()) {
        markRow(sampleId, "failed", m_cfg.retryMax, "host empty");
        return;
    }

    const QJsonObject detail = LisApiService::loadSampleDetail(m_dbPath, sampleId);
    const bool withCurve = m_cfg.pullEnabled;
    const LisOutboundText text = LisMessage::build(m_cfg, detail, LisConfig::localIpv4(), withCurve);
    if (!text.error.isEmpty()) {
        markRow(sampleId, "failed", m_cfg.retryMax, text.error);
        QLOG_WARN() << "LIS build failed" << sampleId << text.error;
        return;
    }

    LisJob job;
    job.sampleId = sampleId;
    job.host = m_cfg.host;
    job.port = m_cfg.pushPort;
    job.protocol = (m_cfg.protocol == "ASTM") ? 2 : 1;
    job.timeoutMs = m_cfg.timeoutSec * 1000;
    if (job.protocol == 2) {
        for (int i = 0; i < text.astmRecords.size(); ++i) {
            job.astmRecords.append(encodeText(text.astmRecords.at(i), m_cfg.charset));
        }
    } else {
        job.hl7 = encodeText(text.hl7, m_cfg.charset);
    }

    markRow(sampleId, "sending", tries + 1, "");
    m_busy = true;
    m_sender->post(job);
}

void LisBridge::onJobDone(QString sampleId, int ok, QString error)
{
    m_busy = false;
    QSqlDatabase db = outboxDb(m_dbPath);
    int tries = m_cfg.retryMax;
    if (db.isOpen()) {
        QSqlQuery query(db);
        query.prepare("SELECT tries FROM LisOutbox WHERE sample_id=?");
        query.addBindValue(sampleId);
        if (query.exec() && query.next()) {
            tries = query.value(0).toInt();
        }
    }
    if (ok) {
        markRow(sampleId, "sent", tries, "");
        QLOG_INFO() << "LIS sent" << sampleId;
    } else if (tries >= m_cfg.retryMax) {
        markRow(sampleId, "failed", tries, error);
        QLOG_WARN() << "LIS failed" << sampleId << error;
    } else {
        markRow(sampleId, "pending", tries, error);
        QLOG_WARN() << "LIS retry" << sampleId << error;
    }
    if (m_again == sampleId) {
        m_again.clear();
        markRow(sampleId, "pending", 0, "");
    }
}
