#pragma execution_character_set("utf-8")
#include "plateletaggreationanalyzer.h"
#include <QsLog/include/QsLog.h>
#include <QFont>

PlateletAggreationAnalyzer::PlateletAggreationAnalyzer(QObject *parent) : QObject(parent)
{

}


PlateletAggreationAnalyzer::AggregationResult PlateletAggreationAnalyzer::analyze(const QVector<double>& data,
                                                                                    double samplingRate)
{
    AggregationResult result;
    if (data.isEmpty()) {
        result = getEmptyResult("数据为空");
        result.dataStatus = "错误: 数据为空";
        return result;
    }

    // 数据验证
    if (!validateData(data)) {
        result = getEmptyResult("数据异常");
        result.dataStatus = "错误: 数据格式异常";
        return result;
    }

    // 数据预处理 - 转换为百分比并校正基线
    QVector<double> processedData = preprocessData(data);

    double timeInterval = 1.0 / (samplingRate * 60.0); // 每个采样点的时间间隔（分钟）

    // 计算各项参数
    double ma = calculateMaxAggregation(processedData);
    double tma = calculateTimeToMax(processedData, timeInterval);
    double lag = calculateLagTime(processedData, ma, timeInterval);
    double slope = calculateSlope(processedData, timeInterval);
    double area = calculateAUC(processedData, timeInterval);

    // 转换为QString结果，并添加合理性检查
    if (ma > 100.0 || ma < 0) {
        result.dataStatus = "警告: 最大聚集率异常";
    } else if (area < 0) {
        result.dataStatus = "警告: AUC为负值";
    } else {
        result.dataStatus = "分析完成";
    }

    // 转换为QString结果
    result.maxAggregation = QString::number(ma, 'f', 2) + "%";
    result.slope = QString::number(slope, 'f', 2) + "%/min";
    result.timeToMax = QString::number(tma, 'f', 2) + "min";
    result.lagTime = QString::number(lag, 'f', 2) + "min";
    result.auc = QString::number(area, 'f', 2) + "%*min";
    result.analysisTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");



    return result;
}

bool PlateletAggreationAnalyzer::validateData(const QVector<double>& data)
{
    if (data.isEmpty()) return false;

    // 检查数据范围是否合理
    double minVal = *std::min_element(data.begin(), data.end());
    double maxVal = *std::max_element(data.begin(), data.end());

    QLOG_DEBUG() << "数据范围:" << minVal << "to" << maxVal;

    // 如果数据全为负值，可能需要特殊处理
    if (maxVal < 0) {
        QLOG_DEBUG() << "警告: 数据全为负值，可能需要反转";
    }

    return true;
}

// 获取分析报告
QString PlateletAggreationAnalyzer::getAnalysisReport(const AggregationResult& result, const QVector<double>& rawData) {
    QString report;
    report += "=== 血小板聚集分析报告 ===\n";
    report += "分析时间: " + result.analysisTime + "\n\n";

    report += "【主要参数】\n";
    report += "最大聚集率 (MA): " + result.maxAggregation + "\n";
    report += "聚集斜率: " + result.slope + "\n";
    report += "达到最大振幅的时间 (TMA): " + result.timeToMax + "\n";
    report += "延迟时间: " + result.lagTime + "\n";
    report += "曲线下面积 (AUC): " + result.auc + "\n\n";

    report += "【数据统计】\n";
    report += "数据点数: " + QString::number(rawData.size()) + "\n";

    if (!rawData.isEmpty()) {
        auto minMax = std::minmax_element(rawData.begin(), rawData.end());
        double minVal = *minMax.first * 100;
        double maxVal = *minMax.second * 100;
        report += "数据范围: " + QString::number(minVal, 'f', 2) + "% to " +
                 QString::number(maxVal, 'f', 2) + "%\n";

        double sum = std::accumulate(rawData.begin(), rawData.end(), 0.0);
        double mean = sum / rawData.size() * 100;
        report += "平均聚集率: " + QString::number(mean, 'f', 2) + "%\n";
    }

    return report;
}


QString  PlateletAggreationAnalyzer::getBriefResult(const AggregationResult& result) {
   return QString("MA: %1 | Slope: %2 | TMA: %3 | Lag: %4 | AUC: %5")
           .arg(result.maxAggregation)
           .arg(result.slope)
           .arg(result.timeToMax)
           .arg(result.lagTime)
           .arg(result.auc);
}


// 数据预处理
QVector<double> PlateletAggreationAnalyzer::preprocessData(const QVector<double>& data) {
    QVector<double> processed;

    // 转换为百分比形式
    for (double value : data) {
        processed.append(value);
    }

    // 方法3: 基线校正（可选）
    // double baseline = processed.first();
    // for (double& value : processed) {
    //     value -= baseline;
    // }

    return processed;
}

// 计算最大聚集率
double PlateletAggreationAnalyzer::calculateMaxAggregation(const QVector<double>& data) {
    auto maxElement = std::max_element(data.begin(), data.end());
    double maxVal = *maxElement;

    // 合理性检查
    if (maxVal > 1000) {
        QLOG_DEBUG() << "警告: 最大聚集率异常高:" << maxVal;
    }

    return maxVal;
}

// 计算达到最大振幅的时间
double PlateletAggreationAnalyzer::calculateTimeToMax(const QVector<double>& data, double timeInterval) {
    auto maxElement = std::max_element(data.begin(), data.end());
    int maxIndex = std::distance(data.begin(), maxElement);
    return maxIndex * timeInterval;
}

// 计算延迟时间
double PlateletAggreationAnalyzer::calculateLagTime(const QVector<double>& data, double maxAggregation, double timeInterval) {
    double baseline = data.first();
    double activationThreshold = baseline + (maxAggregation - baseline) * 0.1;

    const int consecutiveRise = 3;
    int lagIndex = -1;

    for (int i = 0; i < data.size() - consecutiveRise; ++i) {
        bool isConsecutiveRise = true;
        for (int j = 0; j < consecutiveRise - 1; ++j) {
            if (data[i + j + 1] <= data[i + j]) {
                isConsecutiveRise = false;
                break;
            }
        }

        if (isConsecutiveRise && data[i] >= activationThreshold) {
            lagIndex = i;
            break;
        }
    }

    // 备选方案：阈值法
    if (lagIndex == -1) {
        for (int i = 0; i < data.size(); ++i) {
            if (data[i] >= activationThreshold) {
                lagIndex = i;
                break;
            }
        }
    }

    return (lagIndex != -1) ? (lagIndex * timeInterval) : 0.0;
}

// 计算聚集斜率
double PlateletAggreationAnalyzer::calculateSlope(const QVector<double>& data, double timeInterval) {
    const int slopeWindow = 5;
    double maxSlope = 0.0;

    for (int i = 0; i < data.size() - slopeWindow; ++i) {
        // 线性回归计算斜率
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        for (int j = 0; j < slopeWindow; ++j) {
            double x = j * timeInterval;
            double y = data[i + j];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }

        double n = slopeWindow;
        double denominator = n * sumX2 - sumX * sumX;
        if (denominator != 0) {
            double currentSlope = (n * sumXY - sumX * sumY) / denominator;
            if (currentSlope > maxSlope) {
                maxSlope = currentSlope;
            }
        }
    }

    return maxSlope;
}

// 计算曲线下面积
double PlateletAggreationAnalyzer::calculateAUC(const QVector<double>& data, double timeInterval) {
    double area = 0.0;
    for (int i = 1; i < data.size(); ++i) {
        area += (data[i] + data[i-1]) * 0.5 * timeInterval;
    }
    return area;
}

// 获取空结果
PlateletAggreationAnalyzer::AggregationResult PlateletAggreationAnalyzer::getEmptyResult(const QString& errorMsg) {
    AggregationResult result;
    result.maxAggregation = errorMsg;
    result.slope = errorMsg;
    result.timeToMax = errorMsg;
    result.lagTime = errorMsg;
    result.auc = errorMsg;
    result.analysisTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    return result;
}













