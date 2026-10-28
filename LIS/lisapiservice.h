#ifndef LISAPISERVICE_H
#define LISAPISERVICE_H

#include <QTcpServer>
#include <QSqlDatabase>
#include <QJsonObject>
#include <QTimer>

class LisApiService : public QTcpServer
{
    Q_OBJECT
public:
    explicit LisApiService(quint16 port, const QString &dbPath, QObject *parent = nullptr);
    ~LisApiService();

    void start();
    void stop();

    QString getLocalIpAddress();                         // 获取本机IPv4地址
    static QJsonObject loadSampleDetail(const QString &dbPath, const QString &sampleId);

    void setImageSaveDirectory(const QString &dir);
    void setImageRetentionDays(int days);
    void setSharePrefix(const QString &prefix);   // 设置共享路径前缀，如 "\\\\192.168.3.106\\CurveImages"

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private slots:
    void cleanOldImages();

private:
    struct HttpRequest {
        QString method;
        QString path;
        QMap<QString, QString> queryParams;
        QMap<QString, QString> headers;
        QString body;
    };

    struct HttpResponse {
        int statusCode = 200;
        QString contentType = "application/json";
        QByteArray body;
    };

    HttpRequest parseRequest(const QByteArray &data);
    QByteArray buildResponse(const HttpResponse &resp);
    QMap<QString, QString> parseQueryString(const QString &query);

    HttpResponse handleRequest(const HttpRequest &req);
    HttpResponse handleHealth();
    HttpResponse handleSamples(const QMap<QString, QString> &params);
    HttpResponse handleSample(const QString &sampleId);
    HttpResponse handleCurves(const QString &sampleId);
    HttpResponse handleCurvesImage(const QString &sampleId);
    HttpResponse handleConfig();   // 新增配置接口

    QJsonArray querySamplesByDate(const QString &startDate, const QString &endDate);
    QJsonObject querySampleDetail(const QString &sampleId);
    QJsonObject querySampleCurves(const QString &sampleId);

    QString formatDateForSql(const QString &apiDate);
    bool isValidSampleId(const QString &sampleId);

    QByteArray generateCurvesImage(const QString &sampleId);
    QString ensureCurveImage(const QString &sampleId);   // 确保图片存在，返回本地绝对路径


    quint16 m_port;
    QString m_dbPath;
    bool m_running;

    QString m_imageSaveDir;      // 图片保存本地目录
    QString m_sharePrefix;       // 网络共享前缀，如 "\\\\192.168.3.106\\CurveImages"
    int m_retentionDays;         // 保留天数
    QTimer *m_cleanTimer;

};

#endif // LISAPISERVICE_H
