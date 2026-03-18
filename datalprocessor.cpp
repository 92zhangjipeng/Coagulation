#include "datalprocessor.h"
#include "QsLog/include/QsLog.h"
#include <algorithm>
#pragma execution_character_set("utf-8")
#include <cmath>
#include <random>
#include <QRandomGenerator>
#include <QFile>


DataProcessor::DataProcessor() {
   m_mapFirstPrPVal.clear();
}


int DataProcessor::recvTestPara(const QString& sampleNum,bool &usedTestPrp,
                  quint8 reagentIndex,
                  int currentRichValue,
                  int baselineRich,
                  int totalDataPoints)
{
    if(totalDataPoints == 0){
        // 第一个数据点，设置PRP值
        usedTestPrp = false;
        return setReagentFirstPrpValue(sampleNum, reagentIndex, currentRichValue);
    }
    else { // totalDataPoints > 0
        // 后续数据点，获取之前设置的PRP值
        usedTestPrp = true;
        return getReagentFirstPrpValue(sampleNum, reagentIndex, baselineRich);
    }

}

int DataProcessor::setReagentFirstPrpValue(const QString &sampleNum, quint8 reagentIndex, int currentRichValue)
{
    QMutexLocker locker(&m_mutex);  // 加锁

    if (sampleNum.isEmpty()) {
        QLOG_ERROR() << "设置PRP值失败：样本号为空";
        return 0;
    }

    ReagentPrPInitialValues initialValues;

    // 如果已经存在，获取现有值
    if (m_mapFirstPrPVal.contains(sampleNum)) {
        initialValues = m_mapFirstPrPVal[sampleNum];
    }

    // 设置当前试剂的初始值
    initialValues.setReagentValue(reagentIndex, currentRichValue);

    // 更新Map
    m_mapFirstPrPVal[sampleNum] = initialValues;

    QLOG_DEBUG() << QString("样本号: %1, 试剂: %2, PRP初始值: %3")
                            .arg(sampleNum)
                            .arg(reagentIndex)
                            .arg(currentRichValue)<<endl;

    return currentRichValue;
}

int DataProcessor::getReagentFirstPrpValue(const QString& sampleNum,quint8 reagentIndex,int baselineRich)
{
    QMutexLocker locker(&m_mutex);  // 加锁

    if (sampleNum.isEmpty()) {
        QLOG_WARN() << "获取PRP值失败：样本号为空，使用基线值:" << baselineRich;
        return baselineRich;
    }

    if (!m_mapFirstPrPVal.contains(sampleNum)) {
        QLOG_WARN() << QString("样本 %1 不存在于初始值映射表中，使用提供的基线值: %2")
                           .arg(sampleNum)
                           .arg(baselineRich);
        return baselineRich;
    }

    int storedInitialValue = m_mapFirstPrPVal[sampleNum].getReagentValue(reagentIndex);

    if (storedInitialValue == 0) {
        QLOG_WARN() << QString("样本 %1 试剂 %2 没有存储的初始值，使用提供的值: %3")
                           .arg(sampleNum)
                           .arg(reagentIndex)
                           .arg(baselineRich);
        return baselineRich;
    }

    return storedInitialValue;
}

//删除单个试剂的PRP值
bool DataProcessor::removeReagentPrpValue(const QString& sampleNum, quint8 reagentIndex)
{
    QMutexLocker locker(&m_mutex);

    if (!m_mapFirstPrPVal.contains(sampleNum)) {
        QLOG_WARN() << QString("删除失败：样本 %1 不存在").arg(sampleNum);
        return false;
    }

    ReagentPrPInitialValues values = m_mapFirstPrPVal[sampleNum];
    values.removeReagentValue(reagentIndex);

    // 如果删除后所有值都为零，则删除整个样本
    if (values.isAllZero()) {
        m_mapFirstPrPVal.remove(sampleNum);
        QLOG_DEBUG() << QString("样本 %1 的所有PRP值已删除").arg(sampleNum);
    } else {
        m_mapFirstPrPVal[sampleNum] = values;
        QLOG_DEBUG() << QString("样本 %1 试剂 %2 的PRP值已删除").arg(sampleNum).arg(reagentIndex);
    }

    return true;
}


