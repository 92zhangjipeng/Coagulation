#include "lisapiservice.h"
#include <QTcpSocket>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>
#include <QDateTime>
#include <QTimer>
#include <QDebug>
#include <QsLog/include/QsLog.h>
#include <QThread>
#include <QDir>
#include <QBuffer>
#include <QNetworkInterface>
#include "qcustomplot.h"

LisApiService::LisApiService(quint16 port, const QString &dbPath, QObject *parent)
    : QTcpServer(parent)
    , m_port(port)
    , m_dbPath(dbPath)
    , m_running(false)
    , m_imageSaveDir("./CurveImages")
    , m_retentionDays(60)
    , m_cleanTimer(nullptr)
{
    // 启动定时清理器（每天一次）
    m_cleanTimer = new QTimer(this);
    connect(m_cleanTimer, &QTimer::timeout, this, &LisApiService::cleanOldImages);
    m_cleanTimer->start(24 * 60 * 60 * 1000); // 24小时
    // 启动后立即清理一次
    QTimer::singleShot(0, this, &LisApiService::cleanOldImages);
}

LisApiService::~LisApiService()
{
    stop();

}

void LisApiService::start()
{
    if (m_running) return;
    if (!listen(QHostAddress::Any, m_port)) {
        QLOG_WARN() << "LIS API服务启动失败:" << errorString();
        return;
    }
    m_running = true;
    QLOG_DEBUG() << "LIS API服务已启动，端口:" << m_port;

}

void LisApiService::stop()
{
    if (!m_running) return;
    m_running = false;
    close();
}

void LisApiService::setImageSaveDirectory(const QString &dir)
{
    m_imageSaveDir = dir;
}

void LisApiService::setImageRetentionDays(int days)
{
    if (days >= 1) m_retentionDays = days;
}

void LisApiService::setSharePrefix(const QString &prefix)
{
    m_sharePrefix = prefix;
}

QString LisApiService::getLocalIpAddress()
{
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    for (const QHostAddress &address : QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
            return address.toString();
    }
    return QString();
}

void LisApiService::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    if (!socket->setSocketDescriptor(socketDescriptor)) {
        QLOG_WARN() << "setSocketDescriptor failed:" << socket->errorString();
        delete socket;
        return;
    }

    QLOG_DEBUG() << "Client connected:" << socket->peerAddress().toString();

    QTimer *timeoutTimer = new QTimer(socket);
    timeoutTimer->setSingleShot(true);
    timeoutTimer->start(5000);
    connect(timeoutTimer, &QTimer::timeout, [socket, timeoutTimer]() {
        QLOG_WARN() << "Request timeout, closing connection";
        timeoutTimer->deleteLater();
        socket->disconnectFromHost();
        socket->deleteLater();
    });

    connect(socket, &QTcpSocket::readyRead, this, [this, socket, timeoutTimer]() {
        QByteArray requestData = socket->readAll();
        timeoutTimer->stop();
        timeoutTimer->deleteLater();

        HttpRequest req = parseRequest(requestData);
        HttpResponse resp = handleRequest(req);
        QByteArray responseData = buildResponse(resp);

        socket->write(responseData);
        socket->flush();
        socket->disconnectFromHost();
        socket->deleteLater();
    });

    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

LisApiService::HttpRequest LisApiService::parseRequest(const QByteArray &data)
{
    HttpRequest req;
    QStringList lines = QString::fromUtf8(data).split("\r\n");
    if (lines.isEmpty()) return req;

    QStringList requestLine = lines[0].split(' ');
    if (requestLine.size() >= 2) {
        req.method = requestLine[0];
        QString fullPath = requestLine[1];
        int queryPos = fullPath.indexOf('?');
        if (queryPos != -1) {
            req.path = fullPath.left(queryPos);
            req.queryParams = parseQueryString(fullPath.mid(queryPos + 1));
        } else {
            req.path = fullPath;
        }
    }

    int i = 1;
    while (i < lines.size() && !lines[i].isEmpty()) {
        int colonPos = lines[i].indexOf(':');
        if (colonPos != -1) {
            QString key = lines[i].left(colonPos).trimmed();
            QString value = lines[i].mid(colonPos + 1).trimmed();
            req.headers.insert(key, value);
        }
        i++;
    }

    if (i + 1 < lines.size()) {
        req.body = lines.mid(i + 1).join("\r\n");
    }
    return req;
}

QByteArray LisApiService::buildResponse(const HttpResponse &resp)
{
    QString statusText;
    switch (resp.statusCode) {
    case 200: statusText = "OK"; break;
    case 400: statusText = "Bad Request"; break;
    case 404: statusText = "Not Found"; break;
    case 500: statusText = "Internal Server Error"; break;
    default: statusText = "Unknown";
    }

    QString response = QString("HTTP/1.1 %1 %2\r\n").arg(resp.statusCode).arg(statusText);
    response += QString("Content-Type: %1\r\n").arg(resp.contentType);
    response += QString("Content-Length: %1\r\n").arg(resp.body.size());
    response += "Connection: close\r\n\r\n";
    return response.toUtf8() + resp.body;
}

QMap<QString, QString> LisApiService::parseQueryString(const QString &query)
{
    QMap<QString, QString> params;
    QUrlQuery urlQuery(query);
    QList<QPair<QString, QString>> items = urlQuery.queryItems();
    for (const auto &item : items) {
        params[item.first] = item.second;
    }
    return params;
}

LisApiService::HttpResponse LisApiService::handleRequest(const HttpRequest &req)
{
    if (req.path == "/v1/health") {
        return handleHealth();
    }
    else if (req.path == "/v1/config") {
        return handleConfig();
    }
    else if (req.path == "/v1/samples") {
        return handleSamples(req.queryParams);
    }
    else if (req.path.startsWith("/v1/sample/")) {
        QString rest = req.path.mid(11);
        if (rest.endsWith("/curves.png")) {
            QString sampleId = rest.left(rest.length() - 11);
            return handleCurvesImage(sampleId);
        }
        else if (rest.endsWith("/curves")) {
            QString sampleId = rest.left(rest.length() - 7);
            return handleCurves(sampleId);
        }
        else {
            return handleSample(rest);
        }
    }

    HttpResponse resp;
    resp.statusCode = 404;
    QJsonObject obj; obj["error"] = "Not Found";
    resp.body = QJsonDocument(obj).toJson();
    return resp;
}

LisApiService::HttpResponse LisApiService::handleHealth()
{
    HttpResponse resp;
    QJsonObject obj;
    obj["status"] = "ok";
    obj["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    resp.body = QJsonDocument(obj).toJson();
    return resp;
}

LisApiService::HttpResponse LisApiService::handleConfig()
{
    HttpResponse resp;
    QJsonObject obj;
    obj["curve_image_url_prefix"] = QString("http://%1:%2/v1/sample/").arg(getLocalIpAddress()).arg(m_port);
    if (!m_sharePrefix.isEmpty()) {
        obj["curve_image_share_prefix"] = m_sharePrefix;
    } else {
        QString ip = getLocalIpAddress();
        if (!ip.isEmpty()) {
            obj["curve_image_share_prefix"] = QString("\\\\%1\\CurveImages").arg(ip);
        } else {
            obj["curve_image_share_prefix"] = QJsonValue::Null;
        }
    }
    obj["retention_days"] = m_retentionDays;
    resp.body = QJsonDocument(obj).toJson();
    return resp;
}

LisApiService::HttpResponse LisApiService::handleSamples(const QMap<QString, QString> &params)
{
    QString startDate = params.value("start_date");
    QString endDate = params.value("end_date");
    if (startDate.isEmpty() || endDate.isEmpty()) {
        HttpResponse resp;
        resp.statusCode = 400;
        QJsonObject obj;
        obj["error"] = "Missing required parameters: start_date, end_date (format: YYYY-MM-DD)";
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    QJsonArray samples = querySamplesByDate(startDate, endDate);
    QJsonObject result;
    result["code"] = 200;
    result["data"] = samples;
    result["total"] = samples.size();

    HttpResponse resp;
    resp.body = QJsonDocument(result).toJson();
    return resp;
}

QString LisApiService::ensureCurveImage(const QString &sampleId)
{
    // 本地文件路径
    QString localPath = QDir(m_imageSaveDir).filePath(sampleId + ".png");
    // 如果图片不存在，则生成
    if (!QFile::exists(localPath)) {
        QByteArray imageData = generateCurvesImage(sampleId);
        if (imageData.isEmpty()) {
            return QString(); // 无曲线数据，生成失败
        }
    }
    return localPath; // 返回本地绝对路径
}

LisApiService::HttpResponse LisApiService::handleSample(const QString &sampleId)
{
    if (!isValidSampleId(sampleId)) {
        HttpResponse resp;
        resp.statusCode = 400;
        QJsonObject obj; obj["error"] = "Invalid sample ID format";
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    QJsonObject detail = querySampleDetail(sampleId);
    if (detail.isEmpty()) {
        HttpResponse resp;
        resp.statusCode = 404;
        QJsonObject obj; obj["error"] = "Sample not found: " + sampleId;
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    // 确保曲线图片存在，并添加访问路径
    QString localPath = ensureCurveImage(sampleId);
    if (!localPath.isEmpty()) {
        // HTTP URL
        QString url = QString("http://%1:%2/v1/sample/%3/curves.png")
                          .arg(getLocalIpAddress()).arg(m_port).arg(sampleId);
        detail["curve_image_url"] = url;
        // 共享路径（UNC）
        if (!m_sharePrefix.isEmpty()) {
            detail["curve_image_path"] = QDir(m_sharePrefix).filePath(sampleId + ".png");
        } else {
            // 尝试自动构造：使用本机IP + 默认共享名 "CurveImages"
            QString ip = getLocalIpAddress();
            if (!ip.isEmpty()) {
                detail["curve_image_path"] = QString("\\\\%1\\CurveImages\\%2.png").arg(ip).arg(sampleId);
            } else {
                detail["curve_image_path"] = localPath; // 降级为本地路径
            }
        }
    } else {
        detail["curve_image_url"] = QJsonValue::Null;
        detail["curve_image_path"] = QJsonValue::Null;
    }

    QJsonObject result;
    result["code"] = 200;
    result["data"] = detail;

    HttpResponse resp;
    resp.body = QJsonDocument(result).toJson();
    return resp;
}

LisApiService::HttpResponse LisApiService::handleCurves(const QString &sampleId)
{
    if (!isValidSampleId(sampleId)) {
        HttpResponse resp;
        resp.statusCode = 400;
        QJsonObject obj; obj["error"] = "Invalid sample ID format";
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    QJsonObject curves = querySampleCurves(sampleId);
    if (curves.isEmpty()) {
        HttpResponse resp;
        resp.statusCode = 404;
        QJsonObject obj; obj["error"] = "Curve data not found for sample: " + sampleId;
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    QJsonObject result;
    result["code"] = 200;
    result["data"] = curves;
    HttpResponse resp;
    resp.body = QJsonDocument(result).toJson();
    return resp;
}

LisApiService::HttpResponse LisApiService::handleCurvesImage(const QString &sampleId)
{
    if (!isValidSampleId(sampleId)) {
        HttpResponse resp;
        resp.statusCode = 400;
        resp.contentType = "application/json";
        QJsonObject obj; obj["error"] = "Invalid sample ID format";
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    QByteArray imageData = generateCurvesImage(sampleId);
    if (imageData.isEmpty()) {
        HttpResponse resp;
        resp.statusCode = 404;
        resp.contentType = "application/json";
        QJsonObject obj; obj["error"] = "Curve data not found or image generation failed";
        resp.body = QJsonDocument(obj).toJson();
        return resp;
    }

    HttpResponse resp;
    resp.statusCode = 200;
    resp.contentType = "image/png";
    resp.body = imageData;
    return resp;
}

// 以下是数据库查询函数（基本保持不变）
QString LisApiService::formatDateForSql(const QString &apiDate)
{
    QString result = apiDate;
    return result.replace("-", "/");
}

bool LisApiService::isValidSampleId(const QString &sampleId)
{
    if (sampleId.length() < 12) return false;
    for (int i = 0; i < 8; ++i) if (!sampleId[i].isDigit()) return false;
    for (int i = 8; i < 12; ++i) if (!sampleId[i].isDigit()) return false;
    if (sampleId.length() == 13 && sampleId[12] != 'F') return false;
    return true;
}

QJsonArray LisApiService::querySamplesByDate(const QString &startDate, const QString &endDate)
{
    QJsonArray samples;
    QString sqlStart = formatDateForSql(startDate);
    QString sqlEnd = formatDateForSql(endDate);

    QString connName = QString("lis_conn_%1").arg(quintptr(QThread::currentThread()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        QLOG_WARN() << "DB open failed:" << db.lastError().text();
        return samples;
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT 样本号, 姓名, 性别, 年龄, 添加时间, AA, ADP, EPI, COL, RIS "
        "FROM ResultData "
        "WHERE 样本日期 BETWEEN :startDate AND :endDate "
        "ORDER BY 样本号 DESC"
    );
    query.bindValue(":startDate", sqlStart);
    query.bindValue(":endDate", sqlEnd);

    if (!query.exec()) {
        QLOG_WARN() << "Query failed:" << query.lastError().text();
        db.close();
        QSqlDatabase::removeDatabase(connName);
        return samples;
    }

    while (query.next()) {
        QJsonObject item;
        item["sample_id"] = query.value("样本号").toString();
        item["patient_name"] = query.value("姓名").toString();
        item["gender"] = query.value("性别").toString();
        item["age"] = query.value("年龄").toInt();
        item["test_time"] = query.value("添加时间").toString();
        item["aa"] = query.value("AA").toString();
        item["adp"] = query.value("ADP").toString();
        item["epi"] = query.value("EPI").toString();
        item["col"] = query.value("COL").toString();
        item["ris"] = query.value("RIS").toString();
        samples.append(item);
    }

    db.close();
    QSqlDatabase::removeDatabase(connName);
    return samples;
}

QJsonObject LisApiService::querySampleDetail(const QString &sampleId)
{
    QJsonObject detail;
    QString connName = QString("lis_conn_%1").arg(quintptr(QThread::currentThread()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        QLOG_WARN() << "DB open failed:" << db.lastError().text();
        return detail;
    }

    QSqlQuery query(db);
    query.prepare("SELECT * FROM ResultData WHERE 样本号 = :sampleId");
    query.bindValue(":sampleId", sampleId);
    if (!query.exec() || !query.next()) {
        db.close();
        QSqlDatabase::removeDatabase(connName);
        return detail;
    }

    QSqlRecord rec = query.record();
    detail["sample_id"] = query.value(rec.indexOf("样本号")).toString();
    detail["sample_date"] = query.value(rec.indexOf("样本日期")).toString();
    detail["sample_num"] = query.value(rec.indexOf("样本编号")).toInt();
    detail["add_time"] = query.value(rec.indexOf("添加时间")).toString();
    detail["barcode"] = query.value(rec.indexOf("条形编码号")).toString();
    detail["patient_name"] = query.value(rec.indexOf("姓名")).toString();
    detail["gender"] = query.value(rec.indexOf("性别")).toString();
    detail["age"] = query.value(rec.indexOf("年龄")).toInt();
    detail["department"] = query.value(rec.indexOf("科别")).toString();
    detail["bed_number"] = query.value(rec.indexOf("床号")).toString();
    detail["diagnosis"] = query.value(rec.indexOf("临床诊断")).toString();
    detail["referring_doctor"] = query.value(rec.indexOf("送检医生")).toString();
    detail["examining_doctor"] = query.value(rec.indexOf("检验医生")).toString();
    detail["review_doctor"] = query.value(rec.indexOf("审核医生")).toString();
    detail["remark"] = query.value(rec.indexOf("备注")).toString();
    detail["submission_time"] = query.value(rec.indexOf("送检时间")).toString();
    detail["inspection_time"] = query.value(rec.indexOf("检验时间")).toString();

    QJsonObject results;
    results["AA"] = query.value(rec.indexOf("AA")).toString();
    results["ADP"] = query.value(rec.indexOf("ADP")).toString();
    results["EPI"] = query.value(rec.indexOf("EPI")).toString();
    results["COL"] = query.value(rec.indexOf("COL")).toString();
    results["RIS"] = query.value(rec.indexOf("RIS")).toString();
    detail["results"] = results;

    auto getLastAggregationValue = [](const QString &resultStr) -> double {
        if (resultStr.isEmpty()) return 0.0;
        QStringList parts = resultStr.split(',', QString::SkipEmptyParts);
        if (parts.isEmpty()) return 0.0;
        QString lastPart = parts.last().trimmed();
        if (lastPart.endsWith('%')) lastPart.chop(1);
        bool ok;
        double val = lastPart.toDouble(&ok);
        return ok ? val : 0.0;
    };

    QJsonObject maxAgg;
    maxAgg["AA"] = getLastAggregationValue(query.value(rec.indexOf("AA")).toString());
    maxAgg["ADP"] = getLastAggregationValue(query.value(rec.indexOf("ADP")).toString());
    maxAgg["EPI"] = getLastAggregationValue(query.value(rec.indexOf("EPI")).toString());
    maxAgg["COL"] = getLastAggregationValue(query.value(rec.indexOf("COL")).toString());
    maxAgg["RIS"] = getLastAggregationValue(query.value(rec.indexOf("RIS")).toString());
    detail["max_aggregation_rate"] = maxAgg;

    QJsonObject analysis;
    QStringList reagents = {"AA", "ADP", "EPI", "COL", "RIS"};
    for (const QString &reagent : reagents) {
        QJsonObject param;
        param["AUC"] = query.value(rec.indexOf(reagent + "_AUC")).toDouble();
        param["MaxSlope"] = query.value(rec.indexOf(reagent + "_MaxSlope")).toDouble();
        param["TMA"] = query.value(rec.indexOf(reagent + "_TMA")).toDouble();
        param["Lag"] = query.value(rec.indexOf(reagent + "_Lag")).toDouble();
        param["Adhesion"] = query.value(rec.indexOf(reagent + "_Adhesion")).toDouble();
        analysis[reagent] = param;
    }
    detail["analysis_parameters"] = analysis;

    db.close();
    QSqlDatabase::removeDatabase(connName);
    return detail;
}

QJsonObject LisApiService::querySampleCurves(const QString &sampleId)
{
    QJsonObject curves;
    QString connName = QString("lis_conn_%1").arg(quintptr(QThread::currentThread()));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        QLOG_WARN() << "DB open failed:" << db.lastError().text();
        return curves;
    }

    QSqlQuery query(db);
    query.prepare("SELECT AA, ADP, EPI, COL, RIS FROM CurveData WHERE 样本号 = :sampleId");
    query.bindValue(":sampleId", sampleId);
    if (!query.exec() || !query.next()) {
        db.close();
        QSqlDatabase::removeDatabase(connName);
        return curves;
    }

    auto parseCurve = [](const QString &str) -> QJsonArray {
        QJsonArray arr;
        if (str.isEmpty()) return arr;
        QStringList parts = str.split(",", QString::SkipEmptyParts);
        for (const QString &part : parts) {
            bool ok;
            double val = part.toDouble(&ok);
            arr.append(ok ? val * 100.0 : 0.0);
        }
        return arr;
    };

    curves["AA"] = parseCurve(query.value("AA").toString());
    curves["ADP"] = parseCurve(query.value("ADP").toString());
    curves["EPI"] = parseCurve(query.value("EPI").toString());
    curves["COL"] = parseCurve(query.value("COL").toString());
    curves["RIS"] = parseCurve(query.value("RIS").toString());

    db.close();
    QSqlDatabase::removeDatabase(connName);
    return curves;
}

QByteArray LisApiService::generateCurvesImage(const QString &sampleId)
{
    QJsonObject curves = querySampleCurves(sampleId);
    if (curves.isEmpty()) return QByteArray();

    QCustomPlot *customPlot = new QCustomPlot();
    customPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    customPlot->setGeometry(0, 0, 800, 400);
    customPlot->setBackground(QBrush(Qt::white));

    struct CurveInfo { QString name; QColor color; };
    QList<CurveInfo> infos = {
        {"AA", Qt::red}, {"ADP", Qt::green}, {"EPI", Qt::blue},
        {"COL", QColor(255, 165, 0)}, {"RIS", Qt::magenta}
    };

    double maxY = 0.0;
    int maxPoints = 0;
    QVector<QVector<double>> allData;

    for (const auto &info : infos) {
        QJsonArray dataPoints = curves[info.name].toArray();
        if (dataPoints.isEmpty()) {
            allData.append(QVector<double>());
            continue;
        }
        QVector<double> yVals;
        for (int i = 0; i < dataPoints.size(); ++i) {
            double y = dataPoints[i].toDouble();
            yVals.append(y);
            if (y > maxY) maxY = y;
        }
        if (dataPoints.size() > maxPoints) maxPoints = dataPoints.size();
        allData.append(yVals);
    }

    if (maxPoints == 0) {
        delete customPlot;
        return QByteArray();
    }

    customPlot->xAxis->setRange(0, maxPoints);
    double yMax = 100.0;
    if (maxY > 100.0) yMax = maxY * 1.05;
    customPlot->yAxis->setRange(0, yMax);
    customPlot->xAxis->setLabel("Time (points)");
    customPlot->yAxis->setLabel("Aggregation Rate (%)");

    for (int i = 0; i < infos.size(); ++i) {
        const auto &info = infos[i];
        const QVector<double> &yVals = allData[i];
        if (yVals.isEmpty()) continue;
        QVector<double> xVals;
        for (int j = 0; j < yVals.size(); ++j) xVals.append(j);
        customPlot->addGraph();
        customPlot->graph(i)->setData(xVals, yVals);
        customPlot->graph(i)->setPen(QPen(info.color, 2));
        customPlot->graph(i)->setName(info.name);
    }

    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(Qt::white));
    customPlot->legend->setBorderPen(Qt::NoPen);
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    customPlot->replot();
    QPixmap pixmap = customPlot->toPixmap();
    delete customPlot;

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    pixmap.save(&buffer, "PNG");

    // 保存到本地目录
    QDir dir;
    if (!dir.exists(m_imageSaveDir)) {
        dir.mkpath(m_imageSaveDir);
    }
    QString filePath = QDir(m_imageSaveDir).filePath(sampleId + ".png");
    pixmap.save(filePath, "PNG");

    return byteArray;
}

void LisApiService::cleanOldImages()
{
    QDir dir(m_imageSaveDir);
    if (!dir.exists()) return;

    QDateTime cutoff = QDateTime::currentDateTime().addDays(-m_retentionDays);
    QStringList filters;
    filters << "*.png";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);

    int removedCount = 0;
    for (const QFileInfo &info : files) {
        if (info.lastModified() < cutoff) {
            if (dir.remove(info.fileName())) {
                removedCount++;
                QLOG_DEBUG() << "Cleaned old curve image:" << info.filePath();
            } else {
                QLOG_WARN() << "Failed to delete old image:" << info.filePath();
            }
        }
    }
    if (removedCount > 0) {
        QLOG_DEBUG() << "Image cleanup finished, removed" << removedCount << "files older than"
                     << m_retentionDays << "days";
    }
}
