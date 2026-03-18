// dataprocessor.h（头文件需要添加新函数声明）
#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include "cglobal.h"
#include <QVector>
#include <QObject>
#include <QString>
#include <QMap>
#include <cmath>
#include <QHash>
#include <QMutex>
#include <QMutexLocker>


struct ReagentPrPInitialValues {
    int reagentAA;
    int reagentADP;
    int reagentEPI;
    int reagentCOL;
    int reagentRIS;

    // 构造函数，可以初始化所有值
    ReagentPrPInitialValues(int r1 = 0, int r2 = 0, int r3 = 0, int r4 = 0, int r5 = 0)
        : reagentAA(r1), reagentADP(r2), reagentEPI(r3), reagentCOL(r4), reagentRIS(r5) {}

    // 检查是否所有试剂都有值
    bool hasAllValues() const {
        return reagentAA != 0 && reagentADP != 0 && reagentEPI != 0 &&
               reagentCOL != 0 && reagentRIS != 0;
    }

    // 检查是否所有试剂都为零（未初始化）
    bool isAllZero() const {
        return reagentAA == 0 && reagentADP == 0 && reagentEPI == 0 &&
               reagentCOL == 0 && reagentRIS == 0;
    }

    // 获取指定试剂的值
    int getReagentValue(int index) const {
        switch(index) {
            case AA_REAGENT: return reagentAA;
            case ADP_REAGENT: return reagentADP;
            case EPI_REAGENT: return reagentEPI;
            case COL_REAGENT: return reagentCOL;
            case RIS_REAGENT: return reagentRIS;
            default: return 0;
        }
    }

    // 设置指定试剂的值
    void setReagentValue(int index, int value) {
        switch(index) {
            case AA_REAGENT: reagentAA = value; break;
            case ADP_REAGENT: reagentADP = value; break;
            case EPI_REAGENT: reagentEPI = value; break;
            case COL_REAGENT: reagentCOL = value; break;
            case RIS_REAGENT: reagentRIS = value; break;
        }
    }

    // 删除指定试剂的值
   void removeReagentValue(int index) {
       setReagentValue(index, 0);  // 设置为0表示删除
   }
};

class DataProcessor
{
public:
    DataProcessor();

    int recvTestPara(const QString& sampleNum, bool &usedTestPrp,
                     quint8 reagentIndex, int currentRichValue,
                     int baselineRich, int totalDataPoints);

    int  setReagentFirstPrpValue(const QString& sampleNum, quint8 reagentIndex, int currentRichValue);

    int  getReagentFirstPrpValue(const QString& sampleNum, quint8 reagentIndex, int baselineRich);

    bool removeReagentPrpValue(const QString& sampleNum, quint8 reagentIndex);

private:
   mutable QMutex m_mutex;
   QHash<QString, ReagentPrPInitialValues> m_mapFirstPrPVal;

};

#endif // DATAPROCESSOR_H
