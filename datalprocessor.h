// dataprocessor.h（头文件需要添加新函数声明）
#ifndef DATAPROCESSOR_H
#define DATAPROCESSOR_H

#include <QVector>
#include <QObject>
#include <QString>
#include <QMap>
#include <cmath>

// 定义 M_PI
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif



// 定义参数结构体
struct TransitionParams {
    int sinLength;
    double maxNegative;
    bool operator==(const TransitionParams& other) const {
        return sinLength == other.sinLength &&
               qFuzzyCompare(maxNegative, other.maxNegative);
    }
};

class DataProcessor
{
public:
    DataProcessor();

    QVector<double> createControlledSinTransition(const QVector<double>& originalData,
                                                    int sinLength = 150,
                                                    double maxNegative = -20.0);

    // 根据样本号获取或生成参数
    TransitionParams getOrGenerateParams(const QString& sampleID);

    // 保存参数到文件
    void saveParamsToFile();

    // 从文件加载参数
    void loadParamsFromFile();

    void creatRandVal(int &randLength,double &randNative);
private:
    // 生成随机参数
    TransitionParams generateRandomParams();

    // 根据样本号生成确定性随机参数（相同的样本号得到相同的参数）
    TransitionParams generateDeterministicParams(const QString& sampleID);

    // 存储参数映射：样本号 -> 参数
    QMap<QString, TransitionParams> m_paramsMap;

    // 参数文件名
    QString m_paramsFileName;

};

#endif // DATAPROCESSOR_H
