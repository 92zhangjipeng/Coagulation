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

    const double STEP_SECONDS = 1.0;  // 每秒一个点

    //计算稳定的基线（前15个点，0-14秒）
    int baselinePoints = 15;
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    //计算基线标准差
    double stdDev = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        double diff = data[i] - result.baseline;
        stdDev += diff * diff;
    }
    stdDev = std::sqrt(stdDev / baselinePoints);

    //设置阈值（基线 + 3倍标准差 或 基线 + 2%，取较大值）
    //文献建议2-3%，取2.5%比较合适
    double threshold = result.baseline + std::max(3.0 * stdDev, 2.5);

    // 检测延迟时间（需要连续2个点超过阈值，避免噪声）
    int consecutiveCount = 0;
    int lagIndex = -1;
    double lagTimeSeconds = 0.0;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutiveCount++;
            if (consecutiveCount >= 2 && lagIndex == -1) {
                // 记录第一个超过阈值的点
                int firstAboveIndex = i - consecutiveCount + 1;

                // 线性插值提高精度（在第一个点之前插值）
                if (firstAboveIndex > 0 && data[firstAboveIndex] > data[firstAboveIndex-1]) {
                    double prevDiff = data[firstAboveIndex-1] - result.baseline;
                    double currDiff = data[firstAboveIndex] - result.baseline;
                    double thresholdDiff = threshold - result.baseline;

                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        // 插值计算精确的延迟时间（秒）
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        lagTimeSeconds = (firstAboveIndex - 1 + frac) * STEP_SECONDS;
                        lagIndex = firstAboveIndex;  // 标记已找到
                        break;
                    }
                }

                // 如果无法插值，使用第一个点的时间
                lagTimeSeconds = firstAboveIndex * STEP_SECONDS;
                lagIndex = firstAboveIndex;
                break;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    // 设置延迟时间（分钟）
    if (lagIndex >= 0 && lagTimeSeconds > 0) {
        result.lagTimeSeconds = lagTimeSeconds;
        result.lagTime = lagTimeSeconds / 60.0;  // 转换为分钟
        result.isValid = true;
    } else {
        // 未检测到延迟：可能是曲线一直低于阈值或数据异常
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        result.isValid = false;

        // 但如果有明显聚集，可能是阈值设置问题
        // 检查最大聚集率是否 > 20%
        double maxVal = *std::max_element(data.begin(), data.end());
        if (maxVal - result.baseline > 20.0) {
            // 有聚集但没检测到延迟，用最大斜率时间估算
            int maxSlopeIdx = 1;
            double maxSlope = 0;
            for (int i = 1; i < data.size(); ++i) {
                double slope = data[i] - data[i-1];  // %/秒
                if (slope > maxSlope) {
                    maxSlope = slope;
                    maxSlopeIdx = i;
                }
            }
            // 延迟时间大约在最大斜率前30-60秒
            result.lagTimeSeconds = std::max(0.0, (maxSlopeIdx - 30) * STEP_SECONDS);
            result.lagTime = result.lagTimeSeconds / 60.0;
            result.isValid = true;  // 标记为估算值
        }
    }

    //计算最大斜率（%/分钟）
    result.maxSlope = 0;
    for (int i = 1; i < data.size(); ++i) {
        double slope = (data[i] - data[i-1]) * 60.0;  // %/秒 * 60 = %/分钟
        if (slope > result.maxSlope) {
            result.maxSlope = slope;
        }
    }

    // 计算最大值和TMA时间
    result.maxValue = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxValue) {
            result.maxValue = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;  // 转换为分钟
    //result.tmaSeconds = maxIndex;      // 秒

    // 7. 计算AUC（%·分钟）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        // 减去基线，只计算净聚集面积
        double y1 = data[i-1] - result.baseline;
        double y2 = data[i] - result.baseline;
        if (y1 > 0 || y2 > 0) {
            aucPerSec += (std::max(0.0, y1) + std::max(0.0, y2)) / 2.0;
        }
    }
    result.auc = aucPerSec / 60.0;  // 秒转换为分钟

    // 8. AA特异性：粘附率 = 最大聚集率 × 80%
    //result.adhesionRate = result.maxValue * 0.8;

    // 9. 有效性验证
    // 正常AA曲线：最大聚集率应 > 30%，延迟时间 1-3分钟
    if (result.maxValue - result.baseline < 30.0) {
        result.isValid = false;  // 聚集不足
    }
    return result;
}

// AA试剂延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateAALagTime(const QVector<double>& data)
{
    if (data.size() < 20) return 0.0;

    const double STEP_SECONDS = 1.0;

    // 计算基线（前10个点）
    double baseline = 0.0;
    for (int i = 0; i < 10; ++i) {
        baseline += data[i];
    }
    baseline /= 10.0;

    // AA阈值：基线 + 2.5%
    double threshold = baseline + 2.5;

    // 寻找连续2个点超过阈值
    int consecutive = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutive++;
            if (consecutive >= 2) {
                int startIndex = i - consecutive + 1;

                // 线性插值
                if (startIndex > 0 && data[startIndex] > data[startIndex-1]) {
                    double prevDiff = data[startIndex-1] - baseline;
                    double currDiff = data[startIndex] - baseline;
                    double thresholdDiff = threshold - baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        return ((startIndex - 1 + frac) * STEP_SECONDS) / 60.0;
                    }
                }
                return (startIndex * STEP_SECONDS) / 60.0;
            }
        } else {
            consecutive = 0;
        }
    }

    // 未找到
    return ((data.size() - 1) * STEP_SECONDS) / 60.0;
}

// ==================== ADP试剂（腺苷二磷酸）====================
ADPResult AggregationAnalyzer::analyzeADP(const QVector<double>& data)
{
    ADPResult result;
    result.isNormal = false;

    if (data.size() < 30) {
        return result;
    }

    const double STEP_SECONDS = 1.0;  // 每秒一个点

    //计算基线（前8个点，ADP基线短且稳定）
    int baselinePoints = 8;
    result.baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        result.baseline += data[i];
    }
    result.baseline /= baselinePoints;

    //计算基线标准差
    double stdDev = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        double diff = data[i] - result.baseline;
        stdDev += diff * diff;
    }
    stdDev = std::sqrt(stdDev / baselinePoints);

    //ADP阈值：基线 + max(3倍标准差, 2.0%)
    //ADP反应快，阈值略低
    double threshold = result.baseline + std::max(3.0 * stdDev, 2.0);



    // 4. 检测延迟时间（ADP使用连续2个点超过阈值）
    int consecutiveCount = 0;
    int lagIndex = -1;
    double lagTimeSeconds = 0.0;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutiveCount++;
            if (consecutiveCount >= 2 && lagIndex == -1) {
                int firstAboveIndex = i - consecutiveCount + 1;

                // 线性插值
                if (firstAboveIndex > 0 && data[firstAboveIndex] > data[firstAboveIndex-1]) {
                    double prevDiff = data[firstAboveIndex-1] - result.baseline;
                    double currDiff = data[firstAboveIndex] - result.baseline;
                    double thresholdDiff = threshold - result.baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        lagTimeSeconds = (firstAboveIndex - 1 + frac) * STEP_SECONDS;
                        lagIndex = firstAboveIndex;
                        break;
                    }
                }

                lagTimeSeconds = firstAboveIndex * STEP_SECONDS;
                lagIndex = firstAboveIndex;
                break;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    if (lagIndex >= 0 && lagTimeSeconds > 0) {
        result.lagTimeSeconds = lagTimeSeconds;
        result.lagTime = lagTimeSeconds / 60.0;
    } else {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    //计算最大斜率（%/分钟）- 使用滑动窗口
    result.aggregationRate = 0;
    int windowSize = 5;  // 5秒窗口
    for (int i = lagIndex + 1; i < data.size() - windowSize; ++i) {
        double slope = (data[i + windowSize] - data[i - 1]) / (windowSize + 1) * 60.0;
        if (slope > result.aggregationRate) {
            result.aggregationRate = slope;
        }
    }

    //计算10-60秒的平均斜率（文献常用指标）
    int startIdx = std::min(lagIndex + 10, data.size() - 1);
    int endIdx = std::min(lagIndex + 60, data.size() - 1);
    if (endIdx > startIdx + 5) {
        double slopeSum = 0;
        int count = 0;
        for (int i = startIdx; i < endIdx; ++i) {
            slopeSum += (data[i+1] - data[i]) * 60.0;
            count++;
        }
        result.slope10_60 = count > 0 ? slopeSum / count : 0;
    } else {
        result.slope10_60 = 0;
    }

    //计算最大聚集率和TMA
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    //计算AUC（减去基线）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        double y1 = data[i-1] - result.baseline;
        double y2 = data[i] - result.baseline;
        if (y1 > 0 || y2 > 0) {
            aucPerSec += (std::max(0.0, y1) + std::max(0.0, y2)) / 2.0;
        }
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

    //判断是否正常反应（参考文献值调整）
    //    ADP最大聚集率正常为35.2±13.5%，下限约22%
    result.isNormal = (result.maxAggregation >= 25.0 &&      // 降低阈值
                       result.aggregationRate >= 15.0 &&     // 降低阈值
                       result.lagTimeSeconds <= 120.0 &&     // 放宽到120秒
                       result.lagTimeSeconds > 0);

    return result;
}

// ADP延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateADPLagTime(const QVector<double>& data)
{
    if (data.size() < 15) return 0.0;

    const double STEP_SECONDS = 1.0;

    // 基线（前8个点）
    double baseline = 0.0;
    for (int i = 0; i < 8; ++i) {
        baseline += data[i];
    }
    baseline /= 8.0;

    // ADP阈值：基线+2%
    double threshold = baseline + 2.0;

    // 连续2个点超过阈值
    int consecutive = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutive++;
            if (consecutive >= 2) {
                int startIndex = i - consecutive + 1;
                if (startIndex > 0 && data[startIndex] > data[startIndex-1]) {
                    double prevDiff = data[startIndex-1] - baseline;
                    double currDiff = data[startIndex] - baseline;
                    double thresholdDiff = threshold - baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        return ((startIndex - 1 + frac) * STEP_SECONDS) / 60.0;
                    }
                }
                return (startIndex * STEP_SECONDS) / 60.0;
            }
        } else {
            consecutive = 0;
        }
    }

    return ((data.size() - 1) * STEP_SECONDS) / 60.0;
}

// ==================== COL试剂（胶原蛋白）====================

COLResult AggregationAnalyzer::analyzeCOL(const QVector<double>& data)
{
    COLResult result;
    result.isNormal = false;

    if (data.size() < 30) {
        return result;
    }

    const double STEP_SECONDS = 1.0;

    // 1. 计算基线（前10个点）
    int baselinePoints = 10;
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

    // 3. COL阈值：基线 + max(3倍标准差, 3.0%)
    //    COL正常聚集率高，但需要比AA/ADP更高阈值来滤除噪声
    double threshold = result.baseline + std::max(3.0 * stdDev, 3.0);

    // 4. 检测延迟时间（COL使用连续3个点超过阈值，因延迟期较长）
    //    文献显示COL延迟约60秒[citation:9]
    int consecutiveCount = 0;
    int lagIndex = -1;
    double lagTimeSeconds = 0.0;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutiveCount++;
            // COL使用3个连续点，因为延迟后上升可能较缓
            if (consecutiveCount >= 3 && lagIndex == -1) {
                int firstAboveIndex = i - consecutiveCount + 1;

                // 线性插值提高精度
                if (firstAboveIndex > 0 && data[firstAboveIndex] > data[firstAboveIndex-1]) {
                    double prevDiff = data[firstAboveIndex-1] - result.baseline;
                    double currDiff = data[firstAboveIndex] - result.baseline;
                    double thresholdDiff = threshold - result.baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        lagTimeSeconds = (firstAboveIndex - 1 + frac) * STEP_SECONDS;
                        lagIndex = firstAboveIndex;
                        break;
                    }
                }

                lagTimeSeconds = firstAboveIndex * STEP_SECONDS;
                lagIndex = firstAboveIndex;
                break;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    if (lagIndex >= 0 && lagTimeSeconds > 0) {
        result.lagTimeSeconds = lagTimeSeconds;
        result.lagTime = lagTimeSeconds / 60.0;
    } else {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    // 5. 计算最大斜率（%/分钟）- 使用滑动窗口
    result.maxSlope = 0;
    int windowSize = 5;
    for (int i = lagIndex + 1; i < data.size() - windowSize; ++i) {
        double slope = (data[i + windowSize] - data[i - 1]) / (windowSize + 1) * 60.0;
        if (slope > result.maxSlope) {
            result.maxSlope = slope;
        }
    }

    // 6. 计算10-30秒斜率（COL早期上升速率）
    int startIdx = std::min(lagIndex + 10, data.size() - 1);
    int endIdx = std::min(lagIndex + 30, data.size() - 1);
    if (endIdx > startIdx + 3) {
        double slopeSum = 0;
        int count = 0;
        for (int i = startIdx; i < endIdx; ++i) {
            slopeSum += (data[i+1] - data[i]) * 60.0;
            count++;
        }
        result.slope10_30 = count > 0 ? slopeSum / count : 0;
    } else {
        result.slope10_30 = 0;
    }

    // 7. 计算最大聚集率
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 8. 计算AUC（减去基线）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        double y1 = data[i-1] - result.baseline;
        double y2 = data[i] - result.baseline;
        if (y1 > 0 || y2 > 0) {
            aucPerSec += (std::max(0.0, y1) + std::max(0.0, y2)) / 2.0;
        }
    }
    result.auc = aucPerSec / 60.0;

    // 9. 判断是否正常（参考文献值调整）
    //    COL正常最大聚集率 68-89%[citation:7]，取下限约50%
    //    延迟时间文献约60秒[citation:9]，放宽到120秒
    result.isNormal = (result.maxAggregation >= 50.0 &&
                       result.maxSlope >= 20.0 &&      // COL上升陡峭，但阈值放宽
                       result.lagTimeSeconds <= 120.0 && // 放宽到120秒
                       result.lagTimeSeconds > 0);

    return result;
}

// COL延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateCOLLagTime(const QVector<double>& data)
{
    if (data.size() < 15) return 0.0;

   const double STEP_SECONDS = 1.0;

   // 基线（前10个点）
   double baseline = 0.0;
   for (int i = 0; i < 10; ++i) {
       baseline += data[i];
   }
   baseline /= 10.0;

   // COL阈值：基线+3%
   double threshold = baseline + 3.0;

   // 连续3个点超过阈值（COL延迟期较长）
   int consecutive = 0;
   for (int i = 0; i < data.size(); ++i) {
       if (data[i] > threshold) {
           consecutive++;
           if (consecutive >= 3) {
               int startIndex = i - consecutive + 1;

               // 线性插值
               if (startIndex > 0 && data[startIndex] > data[startIndex-1]) {
                   double prevDiff = data[startIndex-1] - baseline;
                   double currDiff = data[startIndex] - baseline;
                   double thresholdDiff = threshold - baseline;
                   if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                       double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                       return ((startIndex - 1 + frac) * STEP_SECONDS) / 60.0;
                   }
               }
               return (startIndex * STEP_SECONDS) / 60.0;
           }
       } else {
           consecutive = 0;
       }
   }

   return ((data.size() - 1) * STEP_SECONDS) / 60.0;
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

    const double STEP_SECONDS = 1.0;

    // 1. 计算基线（前15个点）
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

    // 3. 检测第一相（小幅度上升）- EPI特有
    double firstPhaseThreshold = result.baseline + std::max(2.0 * stdDev, 2.0);
    int firstPhaseIndex = -1;
    double firstPhaseTime = 0.0;

    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > firstPhaseThreshold && data[i] > data[i-1]) {
            firstPhaseIndex = i;
            // 插值计算
            if (i > 0 && data[i] > data[i-1]) {
                double prevDiff = data[i-1] - result.baseline;
                double currDiff = data[i] - result.baseline;
                double thresholdDiff = firstPhaseThreshold - result.baseline;
                if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                    double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                    firstPhaseTime = (i - 1 + frac) * STEP_SECONDS;
                } else {
                    firstPhaseTime = i * STEP_SECONDS;
                }
            }
            result.lagTimeFirstPhase = firstPhaseTime / 60.0;
            break;
        }
    }

    // 4. 检测第二相（主反应）- EPI真正的聚集
    //    EPI第二相阈值应略高（文献显示个体差异大）
    double secondPhaseThreshold = result.baseline + std::max(3.0 * stdDev, 3.0);
    int consecutiveCount = 0;
    int secondPhaseIndex = -1;
    double secondPhaseTime = 0.0;

    int searchStart = std::max(10, firstPhaseIndex + 5); // 从第一相后开始搜索
    for (int i = searchStart; i < data.size(); ++i) {
        if (data[i] > secondPhaseThreshold) {
            consecutiveCount++;
            if (consecutiveCount >= 3 && secondPhaseIndex == -1) {
                int firstAboveIndex = i - consecutiveCount + 1;

                // 线性插值
                if (firstAboveIndex > 0 && data[firstAboveIndex] > data[firstAboveIndex-1]) {
                    double prevDiff = data[firstAboveIndex-1] - result.baseline;
                    double currDiff = data[firstAboveIndex] - result.baseline;
                    double thresholdDiff = secondPhaseThreshold - result.baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        secondPhaseTime = (firstAboveIndex - 1 + frac) * STEP_SECONDS;
                        secondPhaseIndex = firstAboveIndex;
                        break;
                    }
                }

                secondPhaseTime = firstAboveIndex * STEP_SECONDS;
                secondPhaseIndex = firstAboveIndex;
                break;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    if (secondPhaseIndex >= 0 && secondPhaseTime > 0) {
        result.lagTimeSeconds = secondPhaseTime;
        result.lagTime = secondPhaseTime / 60.0;
    } else {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    // 5. 检测是否双相（EPI特征）
    //    双相判断：第一相和第二相之间存在明显的"谷"
    if (firstPhaseIndex > 0 && secondPhaseIndex > firstPhaseIndex + 5) {
        // 找到第一相峰值（第一相后15秒内的最大值）
        double firstPeak = data[firstPhaseIndex];
        int firstPeakIdx = firstPhaseIndex;
        int peakSearchEnd = std::min(firstPhaseIndex + 15, data.size());
        for (int i = firstPhaseIndex; i < peakSearchEnd; ++i) {
            if (data[i] > firstPeak) {
                firstPeak = data[i];
                firstPeakIdx = i;
            }
        }

        // 找到第一相和第二相之间的最小值
        double valley = 999;
        int valleyIdx = firstPeakIdx;
        for (int i = firstPeakIdx + 1; i < secondPhaseIndex; ++i) {
            if (data[i] < valley) {
                valley = data[i];
                valleyIdx = i;
            }
        }

        // 双相条件：峰值与谷值差值 > 2%，且谷值明显低于第一相峰值
        if (firstPeak - valley > 2.0 && valley < firstPeak - 1.0) {
            result.hasBiphasic = true;
            result.firstPhaseValue = firstPeak;
        } else {
            result.firstPhaseValue = firstPeak;
        }
    } else {
        result.firstPhaseValue = result.baseline;
    }

    // 6. 计算第二相斜率（使用滑动窗口）
    result.secondPhaseSlope = 0;
    int windowSize = 5;
    for (int i = secondPhaseIndex + 1; i < data.size() - windowSize; ++i) {
        double slope = (data[i + windowSize] - data[i - 1]) / (windowSize + 1) * 60.0;
        if (slope > result.secondPhaseSlope) {
            result.secondPhaseSlope = slope;
        }
    }

    // 7. 计算最大聚集率
    result.maxAggregation = data[0];
    int maxIndex = 0;
    for (int i = 1; i < data.size(); ++i) {
        if (data[i] > result.maxAggregation) {
            result.maxAggregation = data[i];
            maxIndex = i;
        }
    }
    result.tmaTime = maxIndex / 60.0;

    // 8. 计算AUC（减去基线）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        double y1 = data[i-1] - result.baseline;
        double y2 = data[i] - result.baseline;
        if (y1 > 0 || y2 > 0) {
            aucPerSec += (std::max(0.0, y1) + std::max(0.0, y2)) / 2.0;
        }
    }
    result.auc = aucPerSec / 60.0;

    // 9. 判断是否正常（参考EPI文献调整）
    //    EPI个体差异大，正常范围39-80%，亦有20.5%健康人反应<20%
    //    因此采用更宽松的判断标准
    result.isNormal = (result.maxAggregation >= 20.0 &&      // 放宽到20%
                       result.lagTimeSeconds > 0 &&
                       result.lagTimeSeconds <= 180.0);      // 放宽到180秒

    return result;
}

// EPI延迟时间计算（简化版，推荐日常使用）
double AggregationAnalyzer::calculateEPILagTime(const QVector<double>& data)
{
    if (data.size() < 20) return 0.0;

    const double STEP_SECONDS = 1.0;

    // 基线（前15个点）
    double baseline = 0.0;
    for (int i = 0; i < 15; ++i) {
        baseline += data[i];
    }
    baseline /= 15.0;

    // EPI阈值：基线+3%（个体差异大，不宜过高）
    double threshold = baseline + 3.0;

    // 连续3个点超过阈值
    int consecutive = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutive++;
            if (consecutive >= 3) {
                int startIndex = i - consecutive + 1;
                if (startIndex > 0 && data[startIndex] > data[startIndex-1]) {
                    double prevDiff = data[startIndex-1] - baseline;
                    double currDiff = data[startIndex] - baseline;
                    double thresholdDiff = threshold - baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        return ((startIndex - 1 + frac) * STEP_SECONDS) / 60.0;
                    }
                }
                return (startIndex * STEP_SECONDS) / 60.0;
            }
        } else {
            consecutive = 0;
        }
    }

    return ((data.size() - 1) * STEP_SECONDS) / 60.0;
}



// RIS延迟时间计算（简化版）
// RIS特点：反应极快，20秒内开始，阈值低至基线+1.5%
// RIS完整分析
RISResult AggregationAnalyzer::analyzeRIS(const QVector<double>& data)
{
    RISResult result;
    result.isNormal = false;

    if (data.size() < 20) {
        return result;
    }

    const double STEP_SECONDS = 1.0;

    // 1. 计算基线（固定前8个点，RIS基线短）
    int baselinePoints = std::min(8, data.size());
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

    // 3. RIS阈值（基线 + max(3倍标准差, 1.5%)）
    //    RIS反应极快，阈值低
    double threshold = result.baseline + std::max(3.0 * stdDev, 1.5);

    // 4. 检测延迟时间（RIS反应快，连续2个点超过阈值）
    int consecutiveCount = 0;
    int lagIndex = -1;
    double lagTimeSeconds = 0.0;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutiveCount++;
            if (consecutiveCount >= 2 && lagIndex == -1) {
                int firstAboveIndex = i - consecutiveCount + 1;

                // 线性插值提高精度
                if (firstAboveIndex > 0 && data[firstAboveIndex] > data[firstAboveIndex-1]) {
                    double prevDiff = data[firstAboveIndex-1] - result.baseline;
                    double currDiff = data[firstAboveIndex] - result.baseline;
                    double thresholdDiff = threshold - result.baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        lagTimeSeconds = (firstAboveIndex - 1 + frac) * STEP_SECONDS;
                        lagIndex = firstAboveIndex;
                        break;
                    }
                }

                lagTimeSeconds = firstAboveIndex * STEP_SECONDS;
                lagIndex = firstAboveIndex;
                break;
            }
        } else {
            consecutiveCount = 0;
        }
    }

    if (lagIndex >= 0 && lagTimeSeconds > 0) {
        result.lagTimeSeconds = lagTimeSeconds;
        result.lagTime = lagTimeSeconds / 60.0;
    } else {
        result.lagTimeSeconds = 0;
        result.lagTime = 0;
        return result;
    }

    // 5. 计算聚集速率（RIS上升非常快，使用前20秒内最大斜率）
    result.aggregationRate = 0;
    int windowSize = 3;
    int rateEnd = std::min(lagIndex + 20, data.size());
    for (int i = lagIndex + 1; i < rateEnd - windowSize; ++i) {
        double slope = (data[i + windowSize] - data[i - 1]) / (windowSize + 1) * 60.0;
        if (slope > result.aggregationRate) {
            result.aggregationRate = slope;
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

    // 7. 计算AUC（减去基线）
    double aucPerSec = 0;
    for (int i = 1; i < data.size(); ++i) {
        double y1 = data[i-1] - result.baseline;
        double y2 = data[i] - result.baseline;
        if (y1 > 0 || y2 > 0) {
            aucPerSec += (std::max(0.0, y1) + std::max(0.0, y2)) / 2.0;
        }
    }
    result.auc = aucPerSec / 60.0;

    // 8. 判断是否正常（基于文献参考值）
    //    高浓度RIS正常应 >50%，通常可达70-90%
    //    延迟时间应在30秒内
    result.isNormal = (result.maxAggregation >= 50.0 &&
                       result.lagTimeSeconds > 0 &&
                       result.lagTimeSeconds <= 30.0);

    return result;
}

// RIS简化版延迟时间计算
double AggregationAnalyzer::calculateRISLagTime(const QVector<double>& data)
{
    if (data.size() < 10) return 0.0;

    const double STEP_SECONDS = 1.0;

    // RIS基线计算（前8个点固定）
    int baselinePoints = std::min(8, data.size());
    double baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        baseline += data[i];
    }
    baseline /= baselinePoints;

    // RIS阈值（基线+1.5%）
    double threshold = baseline + 1.5;

    // 连续2个点超过阈值
    int consecutive = 0;
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > threshold) {
            consecutive++;
            if (consecutive >= 2) {
                int startIndex = i - consecutive + 1;
                if (startIndex > 0 && data[startIndex] > data[startIndex-1]) {
                    double prevDiff = data[startIndex-1] - baseline;
                    double currDiff = data[startIndex] - baseline;
                    double thresholdDiff = threshold - baseline;
                    if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                        double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                        return ((startIndex - 1 + frac) * STEP_SECONDS) / 60.0;
                    }
                }
                return (startIndex * STEP_SECONDS) / 60.0;
            }
        } else {
            consecutive = 0;
        }
    }

    return ((data.size() - 1) * STEP_SECONDS) / 60.0;
}

