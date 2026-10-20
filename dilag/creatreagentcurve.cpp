#include "creatreagentcurve.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

CreatReagentCurve::CreatReagentCurve()
{

}


/**
 * @brief 从曲线数据计算各项指标
 * @param curve         聚集率序列，curve[i] 对应时间 i*step 秒
 * @param stepSeconds   采样步长 (秒)
 * @param type          试剂类型（用于判断是否计算粘附率）
 * @param lagThreshold  延迟阈值 (%), 超过此值认为开始聚集，默认 5%
 * @return AggregationMetrics 计算得到的指标
 */
AggregationMetrics CreatReagentCurve::ComputeMetrics(const std::vector<double>& curve,
                                    double stepSeconds, InducerType type, double lagThreshold) {
    AggregationMetrics metrics = {0.0, 0.0, 0.0, 0.0, 0.0, false, 0.0, 0.0};
    if (curve.empty()) return metrics;

	int n = static_cast<int>(curve.size());
    double tMax = (n - 1) * stepSeconds;

    // 计算基线（前10个点平均值）
    int baselinePoints = std::min(10, n);
    double baseline = 0.0;
    for (int i = 0; i < baselinePoints; ++i) {
        baseline += curve[i];
    }
    baseline /= baselinePoints;


    // 1. 最大聚集率及其时间 (TMA) - 单位分钟
    metrics.maxAggregation = curve[0];
    int tmaIdx = 0;
    for (int i = 1; i < n; ++i) {
        if (curve[i] > metrics.maxAggregation) {
            metrics.maxAggregation = curve[i];
            tmaIdx = i;
        }
    }
    metrics.tma = (tmaIdx * stepSeconds) / 60.0; // 转换为分钟

    // 2. 终点聚集率 (300秒)
    metrics.finalAggregation = curve.back();
    // 判断是否有解聚：终点比最大聚集率小且差值超过 2%
    if (metrics.maxAggregation - metrics.finalAggregation > 2.0) {
        metrics.hasDecay = true;
    }

    // 3. 最大斜率 (上升段的最大一阶差分 / 步长) - 单位 %/分钟
    double maxSlope = 0.0;
    for (int i = 1; i < n; ++i) {
        double diff = curve[i] - curve[i-1];
        if (diff > 0) {
            double slope = (diff / stepSeconds) * 60.0; // 转换为 %/分钟
            if (slope > maxSlope) maxSlope = slope;
        }
    }
    metrics.slope = maxSlope;

    // 4. 延迟时间 (首次超过阈值的时间，阈值基于基线0) - 单位分钟
    metrics.lagTime = tMax / 60.0; // 默认最大值（可能未达到阈值）
    for (int i = 0; i < n; ++i) {
        if (curve[i] - baseline >= lagThreshold) {
            // 线性插值提高精度
            if (i > 0 && curve[i] > curve[i-1]) {
                double prevDiff = curve[i-1] - baseline;
                double currDiff = curve[i] - baseline;
                double thresholdDiff = lagThreshold;
                if (currDiff > prevDiff && thresholdDiff > prevDiff) {
                    double frac = (thresholdDiff - prevDiff) / (currDiff - prevDiff);
                    metrics.lagTime = ((i - 1 + frac) * stepSeconds) / 60.0;
                    break;
                }
            }
            metrics.lagTime = (i * stepSeconds) / 60.0;
            break;
        }
    }

    // 5. ACU 面积 (梯形积分从0到终点) - 单位 %·分钟
    double area = 0.0;
        for (int i = 1; i < n; ++i) {
            double y1 = curve[i-1] - baseline;
            double y2 = curve[i] - baseline;
            if (y1 > 0 || y2 > 0) {
                area += (std::max(0.0, y1) + std::max(0.0, y2)) * (stepSeconds / 60.0) / 2.0;
            }
        }
        metrics.acuArea = area;

    // 6. 粘附率（仅AA和ADP，为最大聚集率的80%）
    if (type == InducerType::AA || type == InducerType::ADP) {
        metrics.adhesionRate = metrics.maxAggregation * 0.8;
    } else {
        metrics.adhesionRate = 0.0;
    }

    return metrics;
}





/**
 * @brief 根据试剂类型随机生成合理的曲线参数
 * @param type      试剂类型
 * @param minAgg    最大聚集率的最小值 (%)
 * @param maxAgg    最大聚集率的最大值 (%)
 * @return CurveParams 填充好的参数
 */
CurveParams CreatReagentCurve::RandomCurveParams(InducerType type, double minAgg, double maxAgg) {
    static std::mt19937 rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    ));

    std::uniform_real_distribution<double> aggDist(minAgg, maxAgg);
    double maxAggregation = aggDist(rng);

    double t_mid_min = 0.0, t_mid_max = 0.0;
    double k_min = 0.0, k_max = 0.0;
    double decay_min = 0.0, decay_max = 0.0;
    double initialDrop_min = 0.0, initialDrop_max = 0.0;
    double dropDuration_min = 0.0, dropDuration_max = 0.0;

    switch (type) {
        case InducerType::AA:
            t_mid_min = 40.0; t_mid_max = 70.0;
            k_min = 0.08; k_max = 0.15;
            decay_min = 0.0; decay_max = 0.0;
            initialDrop_min = 5.0; initialDrop_max = 10.0;
            dropDuration_min = 15.0; dropDuration_max = 40.0;
            break;
        case InducerType::ADP:
            t_mid_min = 50.0; t_mid_max = 90.0;
            k_min = 0.06; k_max = 0.12;
            decay_min = 0.0; decay_max = 0.0;
            initialDrop_min = 6.0; initialDrop_max = 12.0;
            dropDuration_min = 25.0; dropDuration_max = 60.0;
            break;
        case InducerType::EPI:
            t_mid_min = 45.0; t_mid_max = 80.0;
            k_min = 0.07; k_max = 0.14;
            decay_min = 0.0; decay_max = 0.0;
            initialDrop_min = 14.0; initialDrop_max = 22.0; // 保留用于判断EPI
            dropDuration_min = 20.0; dropDuration_max = 45.0;
            break;
        case InducerType::COL:
            t_mid_min = 60.0; t_mid_max = 120.0;
            k_min = 0.04; k_max = 0.10;
            decay_min = 0.0; decay_max = 0.0;
            initialDrop_min = 0.0; initialDrop_max = 0.0;
            dropDuration_min = 0.0; dropDuration_max = 0.0;
            break;
        case InducerType::RIS:
            t_mid_min = 20.0; t_mid_max = 50.0;
            k_min = 0.05; k_max = 0.15;
            decay_min = 0.0; decay_max = 0.0;
            initialDrop_min = 0.0; initialDrop_max = 0.0;
            dropDuration_min = 0.0; dropDuration_max = 0.0;
            break;
    }

    std::uniform_real_distribution<double> tmidDist(t_mid_min, t_mid_max);
    std::uniform_real_distribution<double> kDist(k_min, k_max);
    std::uniform_real_distribution<double> decayDist(decay_min, decay_max);
    std::uniform_real_distribution<double> initialDropDist(initialDrop_min, initialDrop_max);
    std::uniform_real_distribution<double> dropDurationDist(dropDuration_min, dropDuration_max);

    CurveParams params;
    params.maxAgg = maxAggregation;
    params.t_mid = tmidDist(rng);
    params.k = kDist(rng);
    params.decay = decayDist(rng);
    params.initialDrop = initialDropDist(rng);
    params.dropDuration = dropDurationDist(rng);

    // 估算峰值时间 (达到99%归一化值的时间)
    double base = 1.0 / (1.0 + std::exp(params.k * params.t_mid));
    double scale = 1.0 - base;
    // 解 normalized = 0.99 => sigmoid = base + 0.99*scale
    double target_sigmoid = base + 0.99 * scale;
    double t_peak = params.t_mid + std::log(target_sigmoid / (1.0 - target_sigmoid)) / params.k;
    params.peakTime = (t_peak > 0) ? t_peak : params.t_mid;
    return params;
}


/**
 * @brief 根据给定的曲线参数生成聚集曲线 (支持解聚)
 * @param params    曲线参数
 * @param duration  总时长 (秒)
 * @param step      采样步长 (秒)
 * @return std::vector<double> 聚集率 (%)
 */
std::vector<double> CreatReagentCurve::GenerateCurve(const CurveParams& params, int duration, int step) {
    const double t_mid = params.t_mid;
    const double k = params.k;
    const double maxAgg = params.maxAgg;
    const double decay = params.decay;
    const double peakTime = params.peakTime;
    const double initialDrop = params.initialDrop;
    const double dropDuration = params.dropDuration;

    static std::mt19937 rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    std::normal_distribution<double> noiseDist(0, 1.5);

    int numPoints = duration / step;
    std::vector<double> curve(numPoints, 0.0);

    // 先生成不带噪声的曲线，计算实际最大值
    std::vector<double> baseCurve(numPoints, 0.0);
    
    // 判断是否是EPI（initialDrop≥14；AA/ADP 的初始下降都低于这个值）
    bool isEPI = (initialDrop >= 14.0);
    
    if (isEPI) {
        // EPI双相聚集曲线（和ADP类似，但是更弱）
        // 第一相：快速上升到约20-30%，然后可能轻微下降
        // 第二相：再次上升到最大聚集率
        std::uniform_real_distribution<double> firstPhaseMaxDist(0.20, 0.35);
        std::uniform_real_distribution<double> firstPhaseEndDist(60.0, 100.0);
        std::uniform_real_distribution<double> secondPhaseStartDist(100.0, 130.0);
        std::uniform_real_distribution<double> secondPhaseEndDist(200.0, 260.0);
        std::uniform_real_distribution<double> dropFactorDist(0.05, 0.15);
        
        double firstPhaseMaxRatio = firstPhaseMaxDist(rng);
        double firstPhaseMax = maxAgg * firstPhaseMaxRatio;
        double firstPhaseEnd = firstPhaseEndDist(rng);
        double secondPhaseStart = secondPhaseStartDist(rng);
        double secondPhaseEnd = secondPhaseEndDist(rng);
        double dropFactor = dropFactorDist(rng);
        
        for (int i = 0; i < numPoints; ++i) {
            double t = static_cast<double>(i * step);
            double agg = 0.0;
            
            if (t <= firstPhaseEnd) {
                // 第一相：快速上升到约20-35%
                double phase = t / firstPhaseEnd;
                agg = firstPhaseMax * (1.0 - std::cos(phase * M_PI / 2.0));
            } else if (t <= secondPhaseStart) {
                // 平台期/轻微下降期：在第一相和第二相之间
                double phase = (t - firstPhaseEnd) / (secondPhaseStart - firstPhaseEnd);
                double dropFromFirstPhase = firstPhaseMax * dropFactor;
                agg = firstPhaseMax - dropFromFirstPhase * phase;
            } else {
                // 第二相：再次上升到最大聚集率（在secondPhaseEnd时达到最大值）
                double phase = (t <= secondPhaseEnd) ? 
                                (t - secondPhaseStart) / (secondPhaseEnd - secondPhaseStart) : 1.0;
                agg = (firstPhaseMax - firstPhaseMax * dropFactor) + 
                      (maxAgg - (firstPhaseMax - firstPhaseMax * dropFactor)) * 
                      (1.0 - std::cos(phase * M_PI / 2.0));
            }
            
            // EPI可能有轻微的初始下降（根据用户图中的红色曲线）
            if (t <= 50.0 && initialDrop > 0.0) {
                double dropPhase = t / 50.0;
                double initDropFactor = 0.15 * initialDrop;
                agg -= initDropFactor * (1.0 - std::cos(dropPhase * M_PI / 2.0));
            }
            
            if (i == 0) agg = 0.0;
            baseCurve[i] = agg;
        }
    } else {
        // 其他试剂：用原来的Sigmoid曲线
        // 计算t=0时的sigmoid值，用于归一化确保从0开始
        double sigmoidAt0 = 1.0 / (1.0 + std::exp(k * t_mid));
        double normFactor = 1.0 / (1.0 - sigmoidAt0);
        
        for (int i = 0; i < numPoints; ++i) {
            double t = static_cast<double>(i * step);
            double sigmoid = 1.0 / (1.0 + std::exp(-k * (t - t_mid)));
            // 归一化确保从0开始
            double normalized = (sigmoid - sigmoidAt0) * normFactor;
            double agg = normalized * maxAgg;
            
            // 添加初始下降再回升（AA、ADP）
            if (t <= dropDuration && initialDrop > 0.0) {
                double dropPhase = t / dropDuration;
                double dropFactor;
                if (dropPhase < 0.3) {
                    dropFactor = 1.0 - std::cos(dropPhase / 0.3 * M_PI / 2.0);
                } else {
                    double recoverPhase = (dropPhase - 0.3) / 0.7;
                    dropFactor = std::cos(recoverPhase * M_PI / 2.0);
                }
                agg -= initialDrop * dropFactor;
            }
            
            if (decay > 0.0 && t > peakTime) {
                const double tau = 60.0;
                double decayFactor = 1.0 - decay * (1.0 - std::exp(-(t - peakTime) / tau));
                if (decayFactor < (1.0 - decay)) decayFactor = 1.0 - decay;
                agg = maxAgg * decayFactor;
            }
            
            if (i == 0) agg = 0.0;
            
            baseCurve[i] = agg;
        }
    }
    
    // 先生成带噪声的曲线，找到真实的最大值，然后再缩放
    std::vector<double> tempCurve(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double agg = baseCurve[i];
        agg += noiseDist(rng);
        if (agg < -20.0) agg = -20.0;
        if (i == 0) agg = 0.0;
        tempCurve[i] = agg;
    }
    
    // 计算带噪声曲线的最大值
    double tempMax = tempCurve[0];
    for (int i = 1; i < numPoints; ++i) {
        if (tempCurve[i] > tempMax) {
            tempMax = tempCurve[i];
        }
    }
    
    // 计算缩放因子，让带噪声曲线的最大值正好达到目标值
    double scaleFactor = (tempMax > 0.1) ? maxAgg / tempMax : 1.0;
    
    // 应用缩放因子
    for (int i = 0; i < numPoints; ++i) {
        double agg = tempCurve[i] * scaleFactor;
        
        // 再次限制范围，防止缩放后超出
        if (agg < -20.0) agg = -20.0;
        if (agg > maxAgg + 0.2) agg = maxAgg + 0.2;
        
        if (i == 0) agg = 0.0;
        
        curve[i] = agg;
    }
    
    return curve;
}


/**
 * @brief 简便接口：生成随机曲线并计算指标
 * @param type      试剂类型
 * @param minAgg    最大聚集率最小值 (%)
 * @param maxAgg    最大聚集率最大值 (%)
 * @param duration  总时长 (秒)
 * @param step      采样步长 (秒)
 * @return std::pair< std::vector<double>, AggregationMetrics > 曲线和指标
 */
std::pair<std::vector<double>, AggregationMetrics> CreatReagentCurve::GenerateRandomCurveWithMetrics(
    InducerType type, double minAgg, double maxAgg, int duration, int step) {
    auto params = RandomCurveParams(type, minAgg, maxAgg);
    auto curve = GenerateCurve(params, duration, step);
    auto metrics = ComputeMetrics(curve, step, type);
    // 用生成的真实最大聚集率覆盖（因为解聚可能导致计算的最大值小于原始参数）
    metrics.maxAggregation = params.maxAgg;
    // 重新计算粘附率
    if (type == InducerType::AA || type == InducerType::ADP) {
        metrics.adhesionRate = metrics.maxAggregation * 0.8;
    }
    return {curve, metrics};
}
