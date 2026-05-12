#ifndef AGGREGATIONANALYZER_H
#define AGGREGATIONANALYZER_H

#include <QVector>
#include <QString>
#include <cmath>

// AA试剂分析结果
struct AAResult {
    double lagTime;          // 延时时间（分钟）
    double lagTimeSeconds;   // 延时时间（秒）
    double baseline;         // 基线值
    double maxSlope;         // 最大斜率（%/分钟）
    double maxValue;         // 最大值（%）
    double tmaTime;          // 达到最大值的平均时间（分钟）
    double auc;              // 曲线下面积（%·分钟）
    bool isValid;            // 分析是否有效
};

// ADP试剂分析结果
struct ADPResult {
    double lagTime;          // 延时时间（分钟）
    double lagTimeSeconds;   // 延时时间（秒）
    double baseline;         // 基线值
    double aggregationRate;  // 聚集速率（%/分钟）
    double maxAggregation;   // 最大聚集率（%）
    double tmaTime;          // 达到最大聚集率的时间（分钟）
    double auc;              // 曲线下面积（%·分钟）
    double slope10_60;       // 10-60秒的平均斜率（%/分钟）
    bool isNormal;           // 是否正常反应
};

// COL试剂分析结果
struct COLResult {
    double lagTime;          // 延时时间（分钟）
    double lagTimeSeconds;   // 延时时间（秒）
    double baseline;         // 基线值
    double maxSlope;         // 最大斜率（%/分钟）
    double maxAggregation;   // 最大聚集率（%）
    double tmaTime;          // 达到最大聚集率的时间（分钟）
    double auc;              // 曲线下面积（%·分钟）
    double slope10_30;       // 10-30秒的平均斜率（%/分钟）
    bool isNormal;           // 是否正常反应
};

// EPI试剂分析结果
struct EPIResult {
    double lagTime;          // 延时时间（分钟）- 第二相开始时间
    double lagTimeSeconds;   // 延时时间（秒）
    double lagTimeFirstPhase; // 第一相延时时间（分钟）
    double baseline;         // 基线值
    double firstPhaseValue;  // 第一相最大聚集率（%）
    double secondPhaseSlope; // 第二相斜率（%/分钟）
    double maxAggregation;   // 最大聚集率（%）
    double tmaTime;          // 达到最大聚集率的时间（分钟）
    double auc;              // 曲线下面积（%·分钟）
    bool hasBiphasic;        // 是否有双相反应
    bool isNormal;           // 是否正常反应
};

struct RISResult {
    double lagTime;          // 延时时间（分钟）
    double lagTimeSeconds;   // 延时时间（秒）
    double baseline;         // 基线值
    double maxAggregation;   // 最大聚集率（%）
    double aggregationRate;  // 聚集速率（%/分钟）
    double tmaTime;          // 达到最大聚集率的时间（分钟）
    double auc;              // 曲线下面积（%·分钟）
    bool isNormal;           // 是否正常反应
};

class AggregationAnalyzer {
public:
    // AA试剂分析
    static AAResult analyzeAA(const QVector<double>& data);
    static double calculateAALagTime(const QVector<double>& data);

    // ADP试剂分析
    static ADPResult analyzeADP(const QVector<double>& data);
    static double calculateADPLagTime(const QVector<double>& data);

    // COL试剂分析
    static COLResult analyzeCOL(const QVector<double>& data);
    static double calculateCOLLagTime(const QVector<double>& data);

    // EPI试剂分析
    static EPIResult analyzeEPI(const QVector<double>& data);
    static double calculateEPILagTime(const QVector<double>& data);

    // RIS试剂分析（完整版）
    static RISResult analyzeRIS(const QVector<double>& data);
    // RIS延迟时间计算（简化版，推荐日常使用）
    static double calculateRISLagTime(const QVector<double>& data);
};

#endif // AGGREGATIONANALYZER_H
