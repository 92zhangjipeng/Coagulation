#ifndef CREATREAGENTCURVE_H
#define CREATREAGENTCURVE_H


#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <algorithm>
#include <limits>


// 试剂类型枚举
enum class InducerType {
    AA,   // 花生四烯酸
    ADP,  // 二磷酸腺苷
    EPI,  // 肾上腺素
    COL,  // 胶原
    RIS   // 瑞斯托霉素
};

// 曲线生成参数
struct CurveParams {
    double maxAgg;       // 最大聚集率 (%)
    double t_mid;        // 半最大聚集时间 (秒)
    double k;            // 斜率参数
    double decay;        // 解聚系数 [0,1]
    double peakTime;     // 到达峰值的时间 (秒)
    double initialDrop;  // 初始下降幅度 (%)
    double dropDuration; // 初始下降持续时间 (秒)
};

// 计算得到的指标
struct AggregationMetrics {
    double maxAggregation;   // 最大聚集率 (%)
    double slope;            // 最大斜率 (%/分钟)
    double acuArea;          // 曲线下面积 (%·分钟)
    double lagTime;          // 延迟时间 (分钟)
    double tma;              // 达峰时间 (分钟)
    bool hasDecay;           // 是否存在解聚
    double finalAggregation; // 终点聚集率 (300秒)
    double adhesionRate;     // 粘附率 (%) - 仅AA/ADP有，为最大聚集率的80%
};



class CreatReagentCurve
{
public:
    CreatReagentCurve();

    /**
     * @brief 从曲线数据计算各项指标
     * @param curve         聚集率序列，curve[i] 对应时间 i*step 秒
     * @param stepSeconds   采样步长 (秒)
     * @param type          试剂类型（用于判断是否计算粘附率）
     * @param lagThreshold  延迟阈值 (%), 超过此值认为开始聚集，默认 5%
     * @return AggregationMetrics 计算得到的指标
     */
    AggregationMetrics ComputeMetrics(const std::vector<double>& curve, double stepSeconds,
                                      InducerType type, double lagThreshold = 5.0);


    /**
     * @brief 根据试剂类型随机生成合理的曲线参数
     * @param type      试剂类型
     * @param minAgg    最大聚集率的最小值 (%)
     * @param maxAgg    最大聚集率的最大值 (%)
     * @return CurveParams 填充好的参数
     */
    CurveParams RandomCurveParams(InducerType type, double minAgg, double maxAgg);



    /**
     * @brief 根据给定的曲线参数生成聚集曲线 (支持解聚)
     * @param params    曲线参数
     * @param duration  总时长 (秒)
     * @param step      采样步长 (秒)
     * @return std::vector<double> 聚集率 (%)
     */
    std::vector<double> GenerateCurve(const CurveParams& params, int duration = 300, int step = 1);


    /**
     * @brief 简便接口：生成随机曲线并计算指标
     * @param type      试剂类型
     * @param minAgg    最大聚集率最小值 (%)
     * @param maxAgg    最大聚集率最大值 (%)
     * @param duration  总时长 (秒)
     * @param step      采样步长 (秒)
     * @return std::pair< std::vector<double>, AggregationMetrics > 曲线和指标
     */
    std::pair<std::vector<double>, AggregationMetrics> GenerateRandomCurveWithMetrics(
        InducerType type, double minAgg, double maxAgg, int duration = 300, int step = 1);

};

#endif // CREATREAGENTCURVE_H
