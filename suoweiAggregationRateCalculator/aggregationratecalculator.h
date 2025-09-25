#ifndef AGGREGATIONRATECALCULATOR_H
#define AGGREGATIONRATECALCULATOR_H

#include <cmath>
#include <stdexcept>
#include <QString>

class AggregationRateCalculator
{
public:
    /**
     * @brief 计算血小板聚集率
     * @param prpn 当前PRP样本的光密度值
     * @param prp0 初始PRP样本的光密度值
     * @param ppp PPP样本的光密度值
     * @return 聚集率计算结果
     * @throws std::invalid_argument 当参数不合法时抛出异常
     */
    static double calculateAggregationRate(double prpn, double prp0, double ppp);

    /**
     * @brief 安全计算聚集率，包含错误检查
     * @param prpn 当前PRP样本的光密度值
     * @param prp0 初始PRP样本的光密度值
     * @param ppp  PPP样本的光密度值
     * @param success 输出参数，指示计算是否成功
     * @param errorMsg 输出参数，错误信息
     * @return 聚集率计算结果，如果失败返回0.0
     */
    static double calculateAggregationRateSafe(double prpn, double prp0, double ppp,
                                              bool* success = nullptr, QString* errorMsg = nullptr);

    /**
     * @brief 验证输入参数是否合法
     * @param prpn 当前PRP样本的光密度值
     * @param prp0 初始PRP样本的光密度值
     * @param ppp PPP样本的光密度值
     * @return 参数是否合法
     */
    static bool validateParameters(double prpn, double prp0, double ppp);

    /**
     * @brief 获取参数验证的错误信息
     * @param prpn 当前PRP样本的光密度值
     * @param prp0 初始PRP样本的光密度值
     * @param ppp PPP样本的光密度值
     * @return 错误信息字符串，如果参数合法返回空字符串
     */
    static QString getValidationError(double prpn, double prp0, double ppp);

private:
    // 防止实例化
    AggregationRateCalculator() = delete;
    ~AggregationRateCalculator() = delete;
};

#endif // AGGREGATIONRATECALCULATOR_H
