#include "aggregationratecalculator.h"

double AggregationRateCalculator::calculateAggregationRate(double prpn, double prp0, double ppp)
{
    // 参数验证
    if (!validateParameters(prpn, prp0, ppp)) {
        throw std::invalid_argument(getValidationError(prpn, prp0, ppp).toStdString());
    }

    // 检查分母是否为0（对数参数不能为0或负数）
    if (prp0 <= 0 || ppp <= 0 || prpn <= 0) {
        throw std::invalid_argument("参数必须大于0，因为log10函数要求参数为正数");
    }

    // 检查PPP和PRP0的关系（理论上PPP应该大于PRP0）
    if (ppp <= prp0) {
        throw std::invalid_argument("PPP值应该大于PRP0值");
    }

    // 计算聚集率：log10(PRPn/PRP0)/log10(PPP/PRP0)
    double numerator = log10(prpn / prp0);      // log10(PRPn/PRP0)
    double denominator = log10(ppp / prp0);     // log10(PPP/PRP0)

    // 检查分母是否为0
    if (denominator == 0) {
        throw std::invalid_argument("分母为0，无法计算聚集率");
    }

    return numerator / denominator;
}

double AggregationRateCalculator::calculateAggregationRateSafe(double prpn, double prp0, double ppp,
                                                              bool* success, QString* errorMsg)
{
    if (success) *success = false;
    if (errorMsg) errorMsg->clear();

    try {
        double result = calculateAggregationRate(prpn, prp0, ppp);
        if (success) *success = true;
        return result;
    }
    catch (const std::exception& e) {
        if (errorMsg) *errorMsg = QString::fromLocal8Bit(e.what());
        return 0.0;
    }
}

bool AggregationRateCalculator::validateParameters(double prpn, double prp0, double ppp)
{
    // 检查是否为有限数字
    if (!std::isfinite(prpn) || !std::isfinite(prp0) || !std::isfinite(ppp)) {
        return false;
    }

    // 检查是否为负数
    if (prpn < 0 || prp0 < 0 || ppp < 0) {
        return false;
    }

    return true;
}

QString AggregationRateCalculator::getValidationError(double prpn, double prp0, double ppp)
{
    if (!std::isfinite(prpn) || !std::isfinite(prp0) || !std::isfinite(ppp)) {
        return "参数必须是有效的数值";
    }

    if (prpn < 0 || prp0 < 0 || ppp < 0) {
        return "参数不能为负数";
    }

    return ""; // 参数合法
}
