#include "CurveImageGenerator.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>
#include <QBuffer>
#include <QPixmap>
#include <QPainter>
#include <QThread>
#include <QFile>
#include <QMetaObject>
#include <QtConcurrent>
#include <QDebug>
#include "QsLog/include/QsLog.h"


CurveImageGenerator* CurveImageGenerator::instance(/*const QString &dbPath, const QString &saveDir*/)
{
    // 静态局部对象，线程安全（C++11 保证）
    static CurveImageGenerator s_instance("", "./CurveImages");
    return &s_instance;
}

void CurveImageGenerator::init(const QString &dbPath, const QString &saveDir)
{
    m_dbPath = dbPath;
    setSaveDirectory(saveDir);

    // 默认配置：启用数据保存，目录与图片相同，保留天数与图片相同
    m_dataSaveEnabled = true;
    m_dataSaveDir.clear();
    m_dataRetentionDays = -1;

    if (!m_cleanupTimer) {
        m_cleanupTimer = new QTimer(this);
        connect(m_cleanupTimer, &QTimer::timeout, this, &CurveImageGenerator::onCleanupTimer);
        m_cleanupTimer->start(24 * 60 * 60 * 1000); // 24小时
        // 延后清理，避免启动瞬间扫盘卡顿
        QTimer::singleShot(60 * 1000, this, &CurveImageGenerator::onCleanupTimer);
    }
}

CurveImageGenerator::CurveImageGenerator(const QString &dbPath,
                                         const QString &saveDir,
                                         QObject *parent)
    : QObject(parent), m_dbPath(dbPath), m_saveDir(saveDir), m_retentionDays(90)
    , m_dataSaveEnabled(true)
    , m_dataRetentionDays(-1)
    , m_cleanupTimer(nullptr)
{
    QDir dir;
    if (!dir.exists(m_saveDir)) {
        dir.mkpath(m_saveDir);
    } 
}

CurveImageGenerator::~CurveImageGenerator()
{
    if (m_cleanupTimer) {
        m_cleanupTimer->stop();
    }
}

// ---------- 图片配置 ---------
void CurveImageGenerator::setSaveDirectory(const QString &dir)
{
    QMutexLocker locker(&m_mutex);
    m_saveDir = dir;
    QDir().mkpath(m_saveDir);
}

QString CurveImageGenerator::saveDirectory() const
{
    QMutexLocker locker(&m_mutex);
    return m_saveDir;
}

void CurveImageGenerator::setRetentionDays(int days)
{
    if (days >= 1) {
        QMutexLocker locker(&m_mutex);
        m_retentionDays = days;
    }
}

int CurveImageGenerator::retentionDays() const
{
    QMutexLocker locker(&m_mutex);
    return m_retentionDays;
}

// ---------- 数据保存配置 ----------
void CurveImageGenerator::setDataSaveEnabled(bool enabled)
{
    QMutexLocker locker(&m_mutex);
    m_dataSaveEnabled = enabled;
}

bool CurveImageGenerator::isDataSaveEnabled() const
{
    QMutexLocker locker(&m_mutex);
    return m_dataSaveEnabled;
}

void CurveImageGenerator::setDataSaveDirectory(const QString &dir)
{
    QMutexLocker locker(&m_mutex);
    m_dataSaveDir = dir;
    if (!m_dataSaveDir.isEmpty()) {
        QDir().mkpath(m_dataSaveDir);
    }
}

QString CurveImageGenerator::dataSaveDirectory() const
{
    QMutexLocker locker(&m_mutex);
    return dataSaveDirectory_unlocked();
}

void CurveImageGenerator::setDataRetentionDays(int days)
{
    if (days >= 1) {
        QMutexLocker locker(&m_mutex);
        m_dataRetentionDays = days;
    }
}

int CurveImageGenerator::dataRetentionDays() const
{
    QMutexLocker locker(&m_mutex);
    return dataRetentionDays_unlocked();
}



// ---------- 核心功能：同步生成曲线图片（自动保存数据） ----------
bool CurveImageGenerator::generateCurveImageSync(const QString &sampleId)
{
    QString localPath = QDir(m_saveDir).filePath(sampleId + ".png");
    if (QFile::exists(localPath)) {
        // 图片已存在，但数据文件可能缺失，尝试补存数据文件
        if (isDataSaveEnabled()) {
            saveSampleDataToJson(sampleId);
        }
        return true;
    }

    QJsonObject curves = querySampleCurves(sampleId);
    if (curves.isEmpty()) {
        QLOG_WARN() << "No curve data for sample:" << sampleId;
        return false;
    }

    {
       QMutexLocker locker(&m_mutex);
       if (!renderAndSaveImage(sampleId, curves)) {
           return false;
       }
   }

    // 保存数据文件（如果启用）
    if (isDataSaveEnabled()) {
        saveSampleDataToJson(sampleId);
    }
    return true;
}

// ---------- 异步生成 ----------
void CurveImageGenerator::generateCurveImageAsync(const QString &sampleId)
{
    QtConcurrent::run([this, sampleId]() {
        bool ok = generateCurveImageSync(sampleId);
        emit imageGenerated(sampleId, ok, ok ? QDir(m_saveDir).filePath(sampleId + ".png") : QString());
    });
}

// ---------- 数据库查询：曲线数据 ----------
QJsonObject CurveImageGenerator::querySampleCurves(const QString &sampleId)
{
    QJsonObject curves;
    QString connName = QString("curve_gen_%1").arg(quintptr(QThread::currentThread()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(m_dbPath);
        if (!db.open()) {
            QLOG_WARN() << "Failed to open DB for curve query:" << db.lastError().text();
            QSqlDatabase::removeDatabase(connName);
            return curves;
        }

        QSqlQuery query(db);
        query.prepare("SELECT AA, ADP, EPI, COL, RIS FROM CurveData WHERE 样本号 = :sampleId");
        query.bindValue(":sampleId", sampleId);
		if (!query.exec()) {
            QLOG_WARN() << "Curve query execution failed:" << query.lastError().text()
				<< "| SampleId:" << sampleId;
			db.close();
			QSqlDatabase::removeDatabase(connName);
			return curves;
		}

		if (!query.next()) {
            QLOG_WARN() << "No curve data found for sampleId:" << sampleId;
			db.close();
			QSqlDatabase::removeDatabase(connName);
			return curves;
		}

        // 兼容 Qt 5.14 以下版本
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        auto splitBehavior = Qt::SkipEmptyParts;
#else
        auto splitBehavior = QString::SkipEmptyParts;
#endif

        auto parseCurve = [splitBehavior](const QString &str) -> QJsonArray {
            QJsonArray arr;
            if (str.isEmpty()) return arr;
            QStringList parts = str.split(",", splitBehavior);
            for (const QString &part : parts) {
                bool ok;
                double val = part.toDouble(&ok);
                arr.append(ok ? val * 100.0 : 0.0);
            }
            return arr;
        };

        curves["AA"]  = parseCurve(query.value("AA").toString());
        curves["ADP"] = parseCurve(query.value("ADP").toString());
        curves["EPI"] = parseCurve(query.value("EPI").toString());
        curves["COL"] = parseCurve(query.value("COL").toString());
        curves["RIS"] = parseCurve(query.value("RIS").toString());

        db.close();
    }
    QSqlDatabase::removeDatabase(connName);
    return curves;
}

// ---------- 数据库查询：样本完整信息（用于 JSON 保存） ----------
QJsonObject CurveImageGenerator::querySampleDetail(const QString &sampleId)
{
    QJsonObject detail;
    QString connName = QString("curve_gen_detail_%1").arg(quintptr(QThread::currentThread()));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connName);
        db.setDatabaseName(m_dbPath);
        if (!db.open()) {
            QLOG_WARN() << "Failed to open DB for detail query:" << db.lastError().text();
            QSqlDatabase::removeDatabase(connName);
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
        // 基本字段
        detail["sample_id"]   = query.value(rec.indexOf("样本号")).toString();
        detail["sample_date"] = query.value(rec.indexOf("样本日期")).toString();
        detail["sample_num"]  = query.value(rec.indexOf("样本编号")).toInt();
        detail["add_time"]    = query.value(rec.indexOf("添加时间")).toString();
        detail["barcode"]     = query.value(rec.indexOf("条形编码号")).toString();
        detail["patient_name"]= query.value(rec.indexOf("姓名")).toString();
        detail["gender"]      = query.value(rec.indexOf("性别")).toString();
        detail["age"]         = query.value(rec.indexOf("年龄")).toInt();
        detail["department"]  = query.value(rec.indexOf("科别")).toString();
        detail["bed_number"]  = query.value(rec.indexOf("床号")).toString();
        detail["diagnosis"]   = query.value(rec.indexOf("临床诊断")).toString();
        detail["referring_doctor"] = query.value(rec.indexOf("送检医生")).toString();
        detail["examining_doctor"] = query.value(rec.indexOf("检验医生")).toString();
        detail["review_doctor"]    = query.value(rec.indexOf("审核医生")).toString();
        detail["remark"]      = query.value(rec.indexOf("备注")).toString();
        detail["submission_time"]  = query.value(rec.indexOf("送检时间")).toString();
        detail["inspection_time"]  = query.value(rec.indexOf("检验时间")).toString();

        // 结果数据
        QJsonObject results;
        results["AA"]  = query.value(rec.indexOf("AA")).toString();
        results["ADP"] = query.value(rec.indexOf("ADP")).toString();
        results["EPI"] = query.value(rec.indexOf("EPI")).toString();
        results["COL"] = query.value(rec.indexOf("COL")).toString();
        results["RIS"] = query.value(rec.indexOf("RIS")).toString();
        detail["results"] = results;

        // 最大聚集率
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
        maxAgg["AA"]  = getLastAggregationValue(query.value(rec.indexOf("AA")).toString());
        maxAgg["ADP"] = getLastAggregationValue(query.value(rec.indexOf("ADP")).toString());
        maxAgg["EPI"] = getLastAggregationValue(query.value(rec.indexOf("EPI")).toString());
        maxAgg["COL"] = getLastAggregationValue(query.value(rec.indexOf("COL")).toString());
        maxAgg["RIS"] = getLastAggregationValue(query.value(rec.indexOf("RIS")).toString());
        detail["max_aggregation_rate"] = maxAgg;

        // 分析参数（AUC, MaxSlope, TMA, Lag, Adhesion）
        QJsonObject analysis;
        QStringList reagents = {"AA", "ADP", "EPI", "COL", "RIS"};
        for (const QString &reagent : reagents) {
            QJsonObject param;
            param["AUC"]       = query.value(rec.indexOf(reagent + "_AUC")).toDouble();
            param["MaxSlope"]  = query.value(rec.indexOf(reagent + "_MaxSlope")).toDouble();
            param["TMA"]       = query.value(rec.indexOf(reagent + "_TMA")).toDouble();
            param["Lag"]       = query.value(rec.indexOf(reagent + "_Lag")).toDouble();
            param["Adhesion"]  = query.value(rec.indexOf(reagent + "_Adhesion")).toDouble();
            analysis[reagent] = param;
        }
        detail["analysis_parameters"] = analysis;

        db.close();
    }
    QSqlDatabase::removeDatabase(connName);
    return detail;
}

// ---------- 保存样本数据为 JSON 文件 ----------
bool CurveImageGenerator::saveSampleDataToJson(const QString &sampleId)
{
    QMutexLocker locker(&m_mutex);   // 这里加锁

    QString dataDir = dataSaveDirectory_unlocked();   // 使用不加锁版本
    QDir dir;
    if (!dir.exists(dataDir) && !dir.mkpath(dataDir)) {
        QLOG_WARN() << "Failed to create data directory:" << dataDir;
        return false;
    }

    QString jsonPath = QDir(dataDir).filePath(sampleId + ".json");

    QJsonObject detail = querySampleDetail(sampleId);
    if (detail.isEmpty()) {
        QLOG_WARN() << "No detail data for sample:" << sampleId << ", cannot save JSON";
        return false;
    }

    // 添加保存时间戳
    detail["saved_at"] = QDateTime::currentDateTime().toString(Qt::ISODate);

    QFile file(jsonPath);
    if (!file.open(QIODevice::WriteOnly)) {
        QLOG_WARN() << "Cannot open JSON file for write:" << jsonPath;
        return false;
    }
    QByteArray jsonData = QJsonDocument(detail).toJson(QJsonDocument::Indented);
    qint64 written = file.write(jsonData);
    file.close();

    bool ok = (written == jsonData.size());
    if (ok) {
        QLOG_DEBUG() << "Sample data saved:" << jsonPath;
        emit dataSaved(sampleId, true, jsonPath);
    } else {
        QLOG_WARN() << "Failed to write JSON file:" << jsonPath;
    }
    return ok;
}


bool CurveImageGenerator::renderAndSaveImage(const QString &sampleId, const QJsonObject &curves)
{
    // 图像尺寸与边距
    const int width = 800;
    const int height = 400;
    const int leftMargin = 60;      // Y轴标签
    const int rightMargin = 60;     // 为曲线末端标签留空间
    const int topMargin = 30;
    const int bottomMargin = 55;    // 增大底部边距

    QRect plotRect(leftMargin, topMargin,
                   width - leftMargin - rightMargin,
                   height - topMargin - bottomMargin);
    if (plotRect.width() <= 0 || plotRect.height() <= 0) {
        QLOG_WARN() << "Invalid plot rectangle size";
        return false;
    }

    // 曲线信息
    struct CurveInfo { QString name; QColor color; };
    QList<CurveInfo> infos = {
        {"AA",  Qt::red},
        {"ADP", Qt::green},
        {"EPI", Qt::blue},
        {"COL", QColor(255, 165, 0)},
        {"RIS", Qt::magenta}
    };

    // 解析数据（最多 301 个点，对应 X=0~300）
    struct CurveData {
        QVector<double> yVals;
        bool valid;
    };
    QVector<CurveData> allData;
    double maxY = 0.0;
    double minY = 0.0;

    for (const auto &info : infos) {
        QJsonArray dataPoints = curves[info.name].toArray();
        QVector<double> yVals;
        int pointsToTake = qMin(dataPoints.size(), 301);
        yVals.reserve(pointsToTake);
        for (int i = 0; i < pointsToTake; ++i) {
            double y = dataPoints[i].toDouble();
            yVals.append(y);
            if (y > maxY) maxY = y;
            if (y < minY) minY = y;
        }
        allData.append({yVals, !yVals.isEmpty()});
    }

    bool anyValid = false;
    for (const auto &d : allData) if (d.valid) { anyValid = true; break; }
    if (!anyValid) {
        QLOG_WARN() << "No valid curve data for sample" << sampleId;
        return false;
    }

    // Y轴范围
    double yMaxRange = (maxY > 100.0) ? maxY * 1.05 : 100.0;
    double yMinRange = (minY < 0) ? minY * 1.05 : -20.0;
    if (yMinRange > 0) yMinRange = -20.0;

    // X轴范围 0 ~ 300
    const double xMin = 0.0;
    const double xMax = 300.0;

    // 创建白色背景图像
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 坐标转换
    auto toPixel = [&](double x, double y) -> QPoint {
        int px = plotRect.left() + int((x - xMin) / (xMax - xMin) * plotRect.width());
        int py = plotRect.bottom() - int((y - yMinRange) / (yMaxRange - yMinRange) * plotRect.height());
        return QPoint(px, py);
    };

    // ---------- 1. 坐标轴 ----------
    painter.setPen(QPen(Qt::black, 1));
    painter.drawLine(plotRect.bottomLeft(), plotRect.bottomRight()); // X轴
    painter.drawLine(plotRect.topLeft(), plotRect.bottomLeft());     // Y轴

    // X轴刻度与标签（0 到 300，步长 50），同时绘制垂直虚线网格线
    QFont axisFont("Arial", 10);
    painter.setFont(axisFont);
    const double xTickStep = 50.0;
    const int tickLength = 6;
    const int labelOffsetY = 8;
    for (double x = xMin; x <= xMax + 0.1; x += xTickStep) {
        int px = toPixel(x, 0).x();
        // 垂直虚线网格线（从绘图区顶部到底部）
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
        painter.drawLine(px, plotRect.top(), px, plotRect.bottom());
        // 刻度线（黑色实线）
        painter.setPen(QPen(Qt::black, 1));
        painter.drawLine(px, plotRect.bottom(), px, plotRect.bottom() + tickLength);
        // 刻度数字
        QString label = QString::number(int(x));
        QRect textRect(px - 15, plotRect.bottom() + labelOffsetY, 30, 20);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }

    // Y轴刻度与水平网格线
    double yRange = yMaxRange - yMinRange;
    double yTickStep = std::pow(10.0, std::floor(std::log10(yRange / 5.0)));
    if (yRange / yTickStep < 3) yTickStep /= 2.0;
    double yStart = std::ceil(yMinRange / yTickStep) * yTickStep;
    for (double y = yStart; y <= yMaxRange + 1e-9; y += yTickStep) {
        int py = toPixel(xMin, y).y();
        // 水平虚线网格线
        painter.setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
        painter.drawLine(plotRect.left(), py, plotRect.right(), py);
        // 刻度线
        painter.setPen(QPen(Qt::black, 1));
        painter.drawLine(plotRect.left() - 5, py, plotRect.left(), py);
        // 标签
        QString label = QString::number(y, 'f', (yTickStep < 1 ? 1 : 0));
        QRect textRect(plotRect.left() - 50, py - 8, 45, 16);
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // 轴标题
    QFont titleFont("Arial", 12, QFont::Bold);
    painter.setFont(titleFont);
    int titleOffsetY = 30;
    painter.drawText(QRect(plotRect.left(), plotRect.bottom() + titleOffsetY,
                           plotRect.width(), 25),
                     Qt::AlignCenter, "Time (s)");
    painter.save();
    painter.translate(20, plotRect.top() + plotRect.height() / 2);
    painter.rotate(-90);
    painter.drawText(QRect(-100, -10, 200, 20), Qt::AlignCenter, "Aggregation Rate (%)");
    painter.restore();

    // ---------- 2. 绘制曲线 及 末端标签 ----------
    painter.setFont(axisFont);
    for (int i = 0; i < infos.size(); ++i) {
        const CurveData &data = allData[i];
        if (!data.valid) continue;

        const QVector<double> &yVals = data.yVals;
        if (yVals.size() == 1) {
            QPoint pt = toPixel(0.0, yVals[0]);
            painter.setPen(QPen(infos[i].color, 2));
            painter.drawEllipse(pt, 3, 3);
            painter.setPen(QPen(infos[i].color, 1));
            painter.drawText(pt.x() + 5, pt.y() - 5, infos[i].name);
            continue;
        }

        QVector<QPointF> points;
        points.reserve(yVals.size());
        for (int j = 0; j < yVals.size(); ++j) {
            double x = xMin + j;
            double y = yVals[j];
            points.append(toPixel(x, y));
        }

        painter.setPen(QPen(infos[i].color, 2));
        for (int j = 0; j < points.size() - 1; ++j) {
            painter.drawLine(points[j], points[j+1]);
        }

        // 末端标签
        QPointF lastPoint = points.last();
        QPoint labelPos(lastPoint.x() + 5, lastPoint.y() - 5);
        painter.setPen(QPen(infos[i].color, 1));
        painter.drawText(labelPos, infos[i].name);
    }

    painter.end();

    // 保存文件
    QString filePath = QDir(m_saveDir).filePath(sampleId + ".png");
    bool saved = image.save(filePath, "PNG");
    if (saved) {
        QLOG_WARN() << "Curve image saved:" << filePath;
    } else {
        QLOG_WARN() << "Failed to save curve image:" << filePath;
    }
    return saved;
}

// ---------- 清理过期文件 ----------
void CurveImageGenerator::cleanupOldFiles(const QString &dir, const QStringList &filters, int retentionDays)
{
    QDir directory(dir);
    if (!directory.exists()) return;

    QFileInfoList files = directory.entryInfoList(filters, QDir::Files);
    QDateTime now = QDateTime::currentDateTime();
    qint64 retentionMs = static_cast<qint64>(retentionDays) * 24 * 60 * 60 * 1000;

    int removed = 0;
    for (const QFileInfo &fi : files) {
        if (fi.lastModified().msecsTo(now) > retentionMs) {
            if (QFile::remove(fi.absoluteFilePath())) {
                ++removed;
                QLOG_DEBUG() << "Removed old file:" << fi.filePath();
            } else {
                QLOG_WARN() << "Failed to remove:" << fi.filePath();
            }
        }
    }
    if (removed) {
        QLOG_DEBUG() << "Cleaned up" << removed << "files from" << dir;
    }
}

void CurveImageGenerator::onCleanupTimer()
{
    QMutexLocker locker(&m_mutex);
    // 清理图片
    cleanupOldFiles(m_saveDir, {"*.png"}, m_retentionDays);

    // 清理数据文件：直接计算目录和保留天数，不调用 dataSaveDirectory() 等
    if (m_dataSaveEnabled) {
        QString dataDir = m_dataSaveDir.isEmpty() ? m_saveDir : m_dataSaveDir;
        int dataDays = (m_dataRetentionDays > 0) ? m_dataRetentionDays : m_retentionDays;
        cleanupOldFiles(dataDir, {"*.json"}, dataDays);
    }
}

QString CurveImageGenerator::dataSaveDirectory_unlocked() const
{
    return m_dataSaveDir.isEmpty() ? m_saveDir : m_dataSaveDir;
}

int CurveImageGenerator::dataRetentionDays_unlocked() const
{
    return (m_dataRetentionDays > 0) ? m_dataRetentionDays : m_retentionDays;
}
