#ifndef PLATELETAGGREATIONANALYZER_H
#define PLATELETAGGREATIONANALYZER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <QDateTime>
#include <qcustomplot.h>





class PlateletAggreationAnalyzer : public QObject
{
    Q_OBJECT
public:
    struct AggregationResult {
       QString maxAggregation;    // 最大聚集率 (%)
       QString slope;             // 聚集斜率 (%/min)
       QString timeToMax;         // 达到最大振幅的时间 (min)
       QString lagTime;           // 延迟时间 (min)
       QString auc;               // 曲线下面积 (%*min)
       QString analysisTime;      // 分析时间
       QString dataStatus;        // 数据状态

       AggregationResult() {
           maxAggregation = "N/A";
           slope = "N/A";
           timeToMax = "N/A";
           lagTime = "N/A";
           auc = "N/A";
           analysisTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
           dataStatus = "待分析";
       }
    };
   
    explicit PlateletAggreationAnalyzer(QObject *parent = nullptr);

    // 分析血小板聚集数据
    AggregationResult  analyze(const QVector<double>& data, double samplingRate = 1.0);

    // 获取分析报告
    QString getAnalysisReport(const AggregationResult& result, const QVector<double>& rawData);

    // 获取简要结果（用于显示在UI上）
    QString getBriefResult(const AggregationResult& result);



private:
    // 数据预处理
    QVector<double> preprocessData(const QVector<double>& data);

    // 数据验证
    bool validateData(const QVector<double>& data);

    double calculateMaxAggregation(const QVector<double>& data);

    // 计算达到最大振幅的时间
    double calculateTimeToMax(const QVector<double>& data, double timeInterval);

    // 计算延迟时间
    double calculateLagTime(const QVector<double>& data, double maxAggregation, double timeInterval);

    // 计算聚集斜率
    double calculateSlope(const QVector<double>& data, double timeInterval);

    // 计算曲线下面积
    double calculateAUC(const QVector<double>& data, double timeInterval);

    // 获取空结果
    AggregationResult getEmptyResult(const QString& errorMsg = "无数据");






};

#endif // PLATELETAGGREATIONANALYZER_H
