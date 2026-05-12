#include "AggregationAnalyzer.h"
#include <algorithm>
#include <cmath>

// ==================== AA试剂（氨基酸）====================

AAResult AggregationAnalyzer::analyzeAA(const QVector<double>& data)
{
    AAResult result;
    result.isValid = false;

    if (data.size() < 30) {
        return result;
    }

    // 1. 计算稳定的基线（前15个点）
    int baselinePoints = 15;
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    // 2. 计算基线标准差
    double stdDev = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        double diff = data[i] - result.baseline;
        stdDev += diff * diff;
    }
    stdDev = std::sqrt(stdDev / baselinePoints);

    // 3. 设置阈值（基线 + 3倍标准差 或 基线 + 2%，取较大值）
    double threshold = result.baseline + std::max(3.0 * stdDev, 2.0);

    // 4. 检测延时时间（需要连续3个点超过阈值）
    int consecutiveCount = 0;
    int lagIndex = -1;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutiveCount++;
            if (consecutiveCount >= 3 && lagIndex == -1) {
                lagIndex = i - 2;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    if (lagIndex >= 0) {
        result.lagTimeSeconds = lagIndex;
        result.lagTime = lagIndex / 60.0;
        result.isValid = true;
    } else {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
    }

    // 5. 计算最大斜率（%/分钟）
    result.maxSlope = 0;
    for (int i = 1; i < data.size(); ++i) {
        double slope = (data[i] - data[i-1]) * 60.0;
        if (slope > result.maxSlope) {
            result.maxSlope = slope;
        }
    }

    // 6. 计算最大值和TMA时间
    result.maxValue = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxValue) {
            result.maxValue = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 7. 计算AUC（%·分钟）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        aucPerSec += (data[i-1] + data[i]) / 2.0;
    }
    result.auc = aucPerSec / 60.0;

    return result;
}

// AA试剂延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateAALagTime(const QVector<double>& data)
{
    if (data.size() < 20) return 0.0;

    // 计算基线（前10个点的平均值）
    double baseline = 0.0;
    for (int i = 0; i < 10; ++i) {
        baseline += data[i];
    }
    baseline /= 10.0;

    // AA试剂通常延时后上升明显，使用基线+5%作为阈值
    double threshold = baseline + 5.0;

    // 寻找第一个超过阈值的点（需要连续2个点）
    int consecutive = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutive++;
            if (consecutive >= 2) {
                return (i - 1) / 60.0; // 返回分钟
            }
        } else {
            consecutive = 0;
        }
    }

    return (data.size() - 1) / 60.0;
}

// ==================== ADP试剂（腺苷二磷酸）====================

ADPResult AggregationAnalyzer::analyzeADP(const QVector<double>& data)
{
    ADPResult result;
    result.isNormal = false;

    if (data.size() < 30) {
        return result;
    }

    // 1. 计算基线（前8个点，ADP基线短且稳定）
    int baselinePoints = 8;
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    // 2. ADP阈值（基线+2%）
    double threshold = result.baseline + 2.0;

    // 3. 检测延迟时间（ADP反应快，第一个超过阈值即为延迟点）
    int lagIndex = -1;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold && data[i] > data[i-1]) {
            // 插值计算精确时间
            if (data[i-1] < threshold) {
                double t1 = i - 1;
                double t2 = i;
                double v1 = data[i-1];
                double v2 = data[i];
                result.lagTimeSeconds = t1 + (threshold - v1) * (t2 - t1) / (v2 - v1);
                lagIndex = i;
            } else {
                result.lagTimeSeconds = i;
                lagIndex = i;
            }
            result.lagTime = result.lagTimeSeconds / 60.0;
            break;
        }
    }

    if (lagIndex == -1) {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    // 4. 计算聚集速率（反应最陡的部分，通常在前60秒）
    result.aggregationRate = 0;
    int rateStart = std::max(0, lagIndex);
    int rateEnd = std::min(lagIndex + 60, data.size());

    if (rateEnd > rateStart + 5) {
        int n = rateEnd - rateStart;
        double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

        for (int i = rateStart; i < rateEnd; ++i) {
            double x = i - rateStart;
            double y = data[i];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }

        double denominator = n * sumX2 - sumX * sumX;
        if (denominator != 0) {
            double slopePerSec = (n * sumXY - sumX * sumY) / denominator;
            result.aggregationRate = slopePerSec * 60.0;
        }
    }

    // 5. 计算最大聚集率
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 6. 计算AUC
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        aucPerSec += (data[i-1] + data[i]) / 2.0;
    }
    result.auc = aucPerSec / 60.0;

    // 7. 计算10-60秒的平均斜率
    int start10 = std::min(10, (int)data.size() - 1);
    int end60 = std::min(60, (int)data.size() - 1);

    if (end60 > start10) {
        double slopeSum = 0;
        for (int i = start10 + 1; i <= end60; ++i) {
            slopeSum += (data[i] - data[i-1]);
        }
        result.slope10_60 = (slopeSum / (end60 - start10)) * 60.0;
    } else {
        result.slope10_60 = 0;
    }

    // 8. 判断是否正常反应
    result.isNormal = (result.maxAggregation >= 50.0 &&
                       result.aggregationRate >= 20.0 &&
                       result.lagTimeSeconds <= 90.0);

    return result;
}

// ADP延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateADPLagTime(const QVector<double>& data)
{
    if (data.size() < 15) return 0.0;

    // ADP基线计算（前8个点）
    double baseline = 0.0;
    for (int i = 0; i < 8; ++i) {
        baseline += data[i];
    }
    baseline /= 8.0;

    // ADP阈值低，反应快（基线+2%）
    double threshold = baseline + 2.0;

    // 寻找第一个明显上升的点
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold && data[i] > data[i-1]) {
            // 确认后面还有上升趋势
            if (i + 2 < data.size() && data[i+1] > data[i]) {
                return i / 60.0; // 返回分钟
            }
        }
    }

    return (data.size() - 1) / 60.0;
}

// ==================== COL试剂（胶原蛋白）====================

COLResult AggregationAnalyzer::analyzeCOL(const QVector<double>& data)
{
    COLResult result;
    result.isNormal = false;

    if (data.size() < 30) {
        return result;
    }

    // 1. 计算基线（前10个点）
    int baselinePoints = 10;
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    // 2. COL阈值（基线+3%）
    double threshold = result.baseline + 3.0;

    // 3. 检测延迟时间（需要连续2个点超过阈值）
    int consecutiveCount = 0;
    int lagIndex = -1;

    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold && data[i] > data[i-1]) {
            consecutiveCount++;
            if (consecutiveCount >= 2 && lagIndex == -1) {
                lagIndex = i - 1;
                // 插值计算精确时间
                if (data[lagIndex-1] < threshold) {
                    double t1 = lagIndex - 1;
                    double t2 = lagIndex;
                    double v1 = data[lagIndex-1];
                    double v2 = data[lagIndex];
                    result.lagTimeSeconds = t1 + (threshold - v1) * (t2 - t1) / (v2 - v1);
                } else {
                    result.lagTimeSeconds = lagIndex;
                }
                result.lagTime = result.lagTimeSeconds / 60.0;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    if (lagIndex == -1) {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    // 4. 计算最大斜率
    result.maxSlope = 0;
    for (int i = 1; i < data.size(); ++i) {
        double slope = (data[i] - data[i-1]) * 60.0;
        if (slope > result.maxSlope) {
            result.maxSlope = slope;
        }
    }

    // 5. 计算10-30秒斜率
    int start10 = std::min(10, (int)data.size() - 1);
    int end30 = std::min(30, (int)data.size() - 1);

    if (end30 > start10) {
        double slopeSum = 0;
        for (int i = start10 + 1; i <= end30; ++i) {
            slopeSum += (data[i] - data[i-1]);
        }
        result.slope10_30 = (slopeSum / (end30 - start10)) * 60.0;
    } else {
        result.slope10_30 = 0;
    }

    // 6. 计算最大聚集率
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 7. 计算AUC
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        aucPerSec += (data[i-1] + data[i]) / 2.0;
    }
    result.auc = aucPerSec / 60.0;

    // 8. 判断是否正常
    result.isNormal = (result.maxAggregation >= 60.0 &&
                       result.maxSlope >= 30.0 &&
                       result.lagTimeSeconds <= 60.0);

    return result;
}

// COL延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateCOLLagTime(const QVector<double>& data)
{
    if (data.size() < 15) return 0.0;

    double baseline = 0.0;
    for (int i = 0; i < 10; ++i) baseline += data[i];
    baseline /= 10.0;

    double threshold = baseline + 3.0; // COL阈值3%

    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold && data[i] > data[i-1]) {
            return i / 60.0;
        }
    }

    return (data.size() - 1) / 60.0;
}

// ==================== EPI试剂（肾上腺素）====================

EPIResult AggregationAnalyzer::analyzeEPI(const QVector<double>& data)
{
    EPIResult result;
    result.isNormal = false;
    result.hasBiphasic = false;

    if (data.size() < 40) {
        return result;
    }

    // 1. 计算基线（前15个点）
    int baselinePoints = 15;
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    // 2. 检测第一相（小幅度上升）
    double firstPhaseThreshold = result.baseline + 2.0;
    int firstPhaseIndex = -1;

    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > firstPhaseThreshold && data[i] > data[i-1]) {
            firstPhaseIndex = i;
            result.lagTimeFirstPhase = i / 60.0;
            break;
        }
    }

    // 3. 检测第二相（大幅度上升，EPI真正的反应）
    double secondPhaseThreshold = result.baseline + 4.0;
    int consecutiveCount = 0;
    int secondPhaseIndex = -1;

    for (int i = std::max(10, firstPhaseIndex + 10); i < data.size(); ++i) {
        if (data[i] > secondPhaseThreshold && data[i] > data[i-1]) {
            consecutiveCount++;
            if (consecutiveCount >= 3 && secondPhaseIndex == -1) {
                secondPhaseIndex = i - 2;
                result.lagTimeSeconds = secondPhaseIndex;
                result.lagTime = secondPhaseIndex / 60.0;

                // 检查是否双相
                if (firstPhaseIndex > 0 && secondPhaseIndex > firstPhaseIndex + 10) {
                    double minBetween = 999;
                    for (int j = firstPhaseIndex; j < secondPhaseIndex; ++j) {
                        if (data[j] < minBetween) {
                            minBetween = data[j];
                        }
                    }
                    if (data[firstPhaseIndex] - minBetween > 2.0) {
                        result.hasBiphasic = true;
                    }
                }
                break;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    // 4. 计算第一相最大值
    if (firstPhaseIndex > 0) {
        result.firstPhaseValue = data[firstPhaseIndex];
        for (int i = firstPhaseIndex; i < std::min(firstPhaseIndex + 15, data.size()); ++i) {
            if (data[i] > result.firstPhaseValue) {
                result.firstPhaseValue = data[i];
            }
        }
    } else {
        result.firstPhaseValue = result.baseline;
    }

    // 5. 计算第二相斜率
    if (secondPhaseIndex > 0) {
        int slopeStart = secondPhaseIndex;
        int slopeEnd = std::min(secondPhaseIndex + 60, data.size());

        if (slopeEnd > slopeStart + 5) {
            int n = slopeEnd - slopeStart;
            double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

            for (int i = slopeStart; i < slopeEnd; ++i) {
                double x = i - slopeStart;
                double y = data[i];
                sumX += x;
                sumY += y;
                sumXY += x * y;
                sumX2 += x * x;
            }

            double denominator = n * sumX2 - sumX * sumX;
            if (denominator != 0) {
                double slopePerSec = (n * sumXY - sumX * sumY) / denominator;
                result.secondPhaseSlope = slopePerSec * 60.0;
            }
        }
    }

    // 6. 计算最大聚集率
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 7. 计算AUC
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        aucPerSec += (data[i-1] + data[i]) / 2.0;
    }
    result.auc = aucPerSec / 60.0;

    // 8. 判断是否正常
    result.isNormal = (result.maxAggregation >= 60.0 &&
                       result.secondPhaseSlope >= 15.0 &&
                       result.lagTimeSeconds <= 120.0);

    return result;
}

// EPI延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateEPILagTime(const QVector<double>& data)
{
    if (data.size() < 20) return 0.0;

    double baseline = 0.0;
    for (int i = 0; i < 15; ++i) baseline += data[i];
    baseline /= 15.0;

    double threshold = baseline + 4.0; // EPI阈值4%
    int consecutive = 0;

    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold && data[i] > data[i-1]) {
            consecutive++;
            if (consecutive >= 3) {
                return (i - 2) / 60.0;
            }
        } else {
            consecutive = 0;
        }
    }

    return (data.size() - 1) / 60.0;
}
// RIS延迟时间计算（简化版）
// RIS特点：反应极快，20秒内开始，阈值低至基线+1.5%
double AggregationAnalyzer::calculateRISLagTime(const QVector<double>& data)
{
    if (data.size() < 10) return 0.0;

    // RIS基线计算（前5个点，RIS基线极短）
    double baseline = 0.0;
    int baselinePoints = std::min(5, data.size() / 3);
    for (int i = 0; i < baselinePoints; ++i) {
        baseline += data[i];
    }
    baseline /= baselinePoints;

    // RIS阈值极低（基线+1.5%），因为RIS反应非常迅速
    double threshold = baseline + 1.5;

    // RIS反应快，第一个超过阈值的点即为延迟点（无需连续确认）
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold) {
            // 线性插值得到更精确的时间
            if (data[i-1] < threshold) {
                double t1 = i - 1;
                double t2 = i;
                double v1 = data[i-1];
                double v2 = data[i];
                double preciseTime = t1 + (threshold - v1) * (t2 - t1) / (v2 - v1);
                return preciseTime / 60.0; // 返回分钟
            }
            return i / 60.0;
        }
    }

    return (data.size() - 1) / 60.0;
}


// RIS完整分析
RISResult AggregationAnalyzer::analyzeRIS(const QVector<double>& data)
{
    RISResult result;
    result.isNormal = false;

    if (data.size() < 20) {
        return result;
    }

    // 1. 计算基线（前5-8个点，RIS基线极短）
    int baselinePoints = std::min(8, data.size() / 4);
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    // 2. RIS阈值（基线+1.5%，反应快阈值低）
    double threshold = result.baseline + 1.5;

    // 3. 检测延迟时间（RIS反应快，第一个超过阈值即为延迟点）
    int lagIndex = -1;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > threshold) {
            // 插值计算精确时间
            if (data[i-1] < threshold) {
                double t1 = i - 1;
                double t2 = i;
                double v1 = data[i-1];
                double v2 = data[i];
                result.lagTimeSeconds = t1 + (threshold - v1) * (t2 - t1) / (v2 - v1);
                lagIndex = i;
            } else {
                result.lagTimeSeconds = i;
                lagIndex = i;
            }
            result.lagTime = result.lagTimeSeconds / 60.0;
            break;
        }
    }

    if (lagIndex == -1) {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    // 4. 计算聚集速率（RIS上升非常快，通常前30秒内完成主要上升）
    int rateStart = std::max(0, lagIndex);
    int rateEnd = std::min(lagIndex + 30, data.size()); // RIS在30秒内快速上升

    if (rateEnd > rateStart + 3) {
        int n = rateEnd - rateStart;
        double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;

        for (int i = rateStart; i < rateEnd; ++i) {
            double x = i - rateStart;
            double y = data[i];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }

        double denominator = n * sumX2 - sumX * sumX;
        if (denominator != 0) {
            double slopePerSec = (n * sumXY - sumX * sumY) / denominator;
            result.aggregationRate = slopePerSec * 60.0;
        }
    }

    // 5. 计算最大聚集率（通常在5分钟内达到平台）
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 6. 计算AUC（%·分钟）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        aucPerSec += (data[i-1] + data[i]) / 2.0;
    }
    result.auc = aucPerSec / 60.0;

    // 7. 判断是否正常（RIS正常反应标准）
    // 正常RIS诱导聚集率通常>50-60%
    result.isNormal = (result.maxAggregation >= 50.0 &&      // 最大聚集率≥50%
                       result.aggregationRate >= 40.0 &&    // 聚集速率≥40%/分钟（RIS上升快）
                       result.lagTimeSeconds <= 30.0);      // 延迟时间≤30秒

    return result;
}
