#include "datalprocessor.h"
#include <QDebug>
#include <algorithm>
#include <cmath>
#include <random>
#include <QRandomGenerator>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


DataProcessor::DataProcessor() {
    // 设置参数文件名，可以根据需要修改
    m_paramsFileName = "transition_params.json";
    // 程序启动时从文件加载已有的参数
    loadParamsFromFile();
}

TransitionParams DataProcessor::generateRandomParams() {
    QRandomGenerator *generator = QRandomGenerator::global();

    TransitionParams params;
    // sinLength 取 40 到 80 的随机整数
    params.sinLength = generator->bounded(40, 90);
    // maxNegative 取 -19.88 到 -1.0 的随机浮点数
    params.maxNegative = -1.0 - (generator->bounded(18.88));

    return params;
}


TransitionParams DataProcessor::generateDeterministicParams(const QString& sampleID) {
    // 使用样本号的哈希值作为随机种子，确保相同的样本号得到相同的参数
    uint seed = qHash(sampleID);
    QRandomGenerator generator(seed);

    TransitionParams params;
    // sinLength 取 40 到 80 的随机整数
    params.sinLength = generator.bounded(40, 81);
    // maxNegative 取 -19.88 到 -1.0 的随机浮点数
    params.maxNegative = -1.0 - (generator.bounded(18.88));

    qDebug() << "为样本" << sampleID << "生成参数: sinLength ="
             << params.sinLength << ", maxNegative =" << params.maxNegative;

    return params;
}

TransitionParams DataProcessor::getOrGenerateParams(const QString& sampleID) {
    // 1. 先在内存中查找
    if (m_paramsMap.contains(sampleID)) {
        qDebug() << "从内存读取样本" << sampleID << "的参数";
        return m_paramsMap[sampleID];
    }

    // 2. 内存中没有，生成新的确定性参数
    TransitionParams params = generateDeterministicParams(sampleID);

    // 3. 保存到内存映射
    m_paramsMap[sampleID] = params;

    // 4. 立即保存到文件（或者可以定期保存）
    saveParamsToFile();

    return params;
}

void DataProcessor::saveParamsToFile() {
    QJsonObject rootObject;
    QJsonArray samplesArray;

    // 遍历所有参数，保存到JSON数组
    for (auto it = m_paramsMap.constBegin(); it != m_paramsMap.constEnd(); ++it) {
        QJsonObject sampleObj;
        sampleObj["sampleID"] = it.key();
        sampleObj["sinLength"] = it.value().sinLength;
        sampleObj["maxNegative"] = it.value().maxNegative;
        samplesArray.append(sampleObj);
    }

    rootObject["samples"] = samplesArray;

    QJsonDocument doc(rootObject);
    QFile file(m_paramsFileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << doc.toJson();
        file.close();
        qDebug() << "参数已保存到文件:" << m_paramsFileName << "，样本数量:" << m_paramsMap.size();
    } else {
        qWarning() << "无法保存参数到文件:" << m_paramsFileName;
    }
}

void DataProcessor::loadParamsFromFile() {
    QFile file(m_paramsFileName);
    if (!file.exists()) {
        qDebug() << "参数文件不存在，将创建新文件";
        return;
    }

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray data = file.readAll();
        file.close();

        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isNull() || !doc.isObject()) {
            qWarning() << "参数文件格式错误";
            return;
        }

        QJsonObject rootObject = doc.object();
        QJsonArray samplesArray = rootObject["samples"].toArray();

        m_paramsMap.clear();
        for (const QJsonValue &value : samplesArray) {
            QJsonObject sampleObj = value.toObject();
            QString sampleID = sampleObj["sampleID"].toString();
            TransitionParams params;
            params.sinLength = sampleObj["sinLength"].toInt();
            params.maxNegative = sampleObj["maxNegative"].toDouble();
            m_paramsMap[sampleID] = params;
        }

        qDebug() << "从文件加载参数，样本数量:" << m_paramsMap.size();
    } else {
        qWarning() << "无法读取参数文件:" << m_paramsFileName;
    }
}

QVector<double> DataProcessor::createControlledSinTransition(const QVector<double>& originalData,
                                                             int sinLength ,
                                                             double maxNegative ) {
    // waveCount 可以固定或也随机，这里保持原来的默认值2
    int waveCount = 3;

    QVector<double> adjusted(300, 0.0);

    qDebug() << "随机参数: sinLength =" << sinLength << ", maxNegative =" << maxNegative;

    // 时间阶段划分（基于总时长sinLength）
    int fallEndPoint = static_cast<int>(sinLength * 25.0 / 80.0); // 25秒位置
    int riseStartPoint = static_cast<int>(sinLength * 50.0 / 80.0); // 50秒位置
    int riseEndPoint = static_cast<int>(sinLength * 80.0 / 80.0); // 80秒位置

    // 确保范围有效
    fallEndPoint = qBound(10, fallEndPoint, sinLength - 30);
    riseStartPoint = qBound(fallEndPoint + 5, riseStartPoint, sinLength - 10);
    riseEndPoint = qBound(riseStartPoint + 10, riseEndPoint, sinLength);

    qDebug() << "阶段划分: 下降(0-" << fallEndPoint
             << "), 保持(" << fallEndPoint << "-" << riseStartPoint
             << "), 回升(" << riseStartPoint << "-" << riseEndPoint << ")";

    // ==================== 生成正弦过渡曲线 ====================

    // 阶段1：从0下降到maxNegative（0-25秒）
    for (int i = 0; i < fallEndPoint; i++) {
        double t = static_cast<double>(i) / fallEndPoint;
        double phase = M_PI * 0.5 * t;
        double fallProgress = sin(phase);
        double wave = sin(2.0 * M_PI * waveCount * t) * 0.1 * maxNegative;
        adjusted[i] = maxNegative * fallProgress + wave;
    }

    // 阶段2：保持在负值区域（25-50秒）
    for (int i = fallEndPoint; i < riseStartPoint; i++) {
        double t = static_cast<double>(i - fallEndPoint) / (riseStartPoint - fallEndPoint);
        double mainWave = sin(2.0 * M_PI * waveCount * t);
        double secondaryWave = sin(2.0 * M_PI * (waveCount + 1) * t + 0.5) * 0.3;
        double amplitudeReduction = 1.0 - t * 0.3;
        double wave = (mainWave * 0.7 + secondaryWave * 0.3) * 0.8 * amplitudeReduction;
        adjusted[i] = maxNegative * 0.9 + wave;
    }

    // 阶段3：从maxNegative回升到接近0（50-80秒）
    for (int i = riseStartPoint; i < riseEndPoint; i++) {
        double t = static_cast<double>(i - riseStartPoint) / (riseEndPoint - riseStartPoint);
        double baseRise = maxNegative * (1.0 - t);
        double riseWave;
        if (t < 0.7) {
            double waveAmplitude = 0.5 * (1.0 - t * 0.5);
            riseWave = sin(2.0 * M_PI * waveCount * 2.0 * t) * waveAmplitude;
        } else {
            double waveAmplitude = 0.2 * (1.0 - t);
            riseWave = sin(2.0 * M_PI * (waveCount + 1) * t) * waveAmplitude;
        }
        adjusted[i] = baseRise + riseWave;
        if (adjusted[i] > 0) adjusted[i] *= 0.5;
        if (adjusted[i] < maxNegative * 0.8) {
            adjusted[i] = maxNegative * 0.8 + (maxNegative * 0.1 * sin(t * M_PI));
        }
    }

    // 阶段4：在0附近轻微波动（80秒到sinLength结束）
    for (int i = riseEndPoint; i < sinLength; i++) {
        double t = static_cast<double>(i - riseEndPoint) / (sinLength - riseEndPoint);
        double amplitude = 0.3 * exp(-t * 2.0);
        double frequency = 1.5;
        double wave1 = sin(2.0 * M_PI * frequency * t) * amplitude * 0.7;
        double wave2 = sin(2.0 * M_PI * (frequency + 0.3) * t + 0.5) * amplitude * 0.3;
        adjusted[i] = wave1 + wave2;
        if (adjusted[i] > 0.3) adjusted[i] = 0.3;
        if (adjusted[i] < -0.3) adjusted[i] = -0.3;
    }

    // 确保最后一点为0
    adjusted[sinLength-1] = 0.0;

    // ==================== 分析原数据，找到衔接点 ====================

    struct RisePoint {
        int index;
        double value;
        double slope;
    };

    QVector<RisePoint> candidates;
    for (int i = 50; i < qMin(150, originalData.size()); i++) {
        if (originalData[i] > 0) {
            double slope = 0;
            int count = 0;
            for (int j = 1; j <= 5 && i+j < originalData.size(); j++) {
                slope += (originalData[i+j] - originalData[i]) / j;
                count++;
            }
            if (count > 0) slope /= count;

            if (slope > 0.001) {
                candidates.append({i, originalData[i], slope});
            }
        }
    }

    int bestPoint = 100;
    if (!candidates.isEmpty()) {
        std::sort(candidates.begin(), candidates.end(),
                 [](const RisePoint& a, const RisePoint& b) {
                     return a.slope > b.slope;
                 });
        bestPoint = candidates[0].index;
    }

    qDebug() << "选择衔接点:" << bestPoint << "值=" << originalData[bestPoint];

    // ==================== 过渡到原数据 ====================

    // 分析原数据在衔接点附近的波动特性
    double baseFrequency = 0.0;
    double baseAmplitude = 0.0;
    if (bestPoint + 10 < originalData.size()) {
        QVector<double> diffData;
        for (int j = bestPoint; j < qMin(bestPoint + 10, originalData.size() - 1); j++) {
            diffData.append(originalData[j+1] - originalData[j]);
        }

        int zeroCrossings = 0;
        for (int j = 1; j < diffData.size(); j++) {
            if (diffData[j-1] * diffData[j] < 0) {
                zeroCrossings++;
            }
        }
        baseFrequency = zeroCrossings * 0.1;

        double maxVal = originalData[bestPoint];
        double minVal = originalData[bestPoint];
        for (int j = bestPoint; j < qMin(bestPoint + 10, originalData.size()); j++) {
            if (originalData[j] > maxVal) maxVal = originalData[j];
            if (originalData[j] < minVal) minVal = originalData[j];
        }
        baseAmplitude = (maxVal - minVal) * 0.5;
    }

    if (baseFrequency <= 0.01) baseFrequency = 0.5;
    if (baseAmplitude <= 0.01) baseAmplitude = 0.1;

    // 第一阶段过渡
    int transition1Length = 10;
    int transition1Start = sinLength;
    int transition1End = qMin(sinLength + transition1Length, 300);

    for (int i = transition1Start; i < transition1End; i++) {
        double t = static_cast<double>(i - transition1Start) / transition1Length;
        double linearValue = originalData[bestPoint] * t;
        double currentFrequency = baseFrequency * t;
        double currentAmplitude = baseAmplitude * t * 0.5;
        double wave = sin(2.0 * M_PI * currentFrequency * (i - transition1Start)) * currentAmplitude;
        adjusted[i] = linearValue + wave;
    }

    // 第二阶段过渡
    int transition2Length = 30;
    int transition2Start = transition1End;
    int transition2End = qMin(transition2Start + transition2Length, 300);

    for (int i = transition2Start; i < transition2End; i++) {
        double t = static_cast<double>(i - transition2Start) / transition2Length;
        int targetIdx = bestPoint + (i - sinLength);
        targetIdx = qMin(targetIdx, originalData.size() - 1);
        double baseValue = (1.0 - t) * adjusted[i-1] + t * originalData[targetIdx];

        double sourceWave = 0.0;
        if (targetIdx > 0 && targetIdx < originalData.size() - 1) {
            sourceWave = (originalData[targetIdx+1] - 2*originalData[targetIdx] + originalData[targetIdx-1]) * 0.5;
        }

        double generatedWave = sin(2.0 * M_PI * baseFrequency * (i - transition1Start) + 0.3) * baseAmplitude * 0.3;
        double mixedWave = (1.0 - t) * generatedWave + t * sourceWave;
        adjusted[i] = baseValue + mixedWave * (1.0 - t * 0.8);
    }

    // 尾部跟随原数据
    int tailStart = transition2End;
    for (int i = tailStart; i < 300; i++) {
        int sourceIdx = bestPoint + (i - sinLength);
        sourceIdx = qMin(sourceIdx, originalData.size() - 1);
        double baseValue = originalData[sourceIdx];

        double continuityWave = 0.0;
        if (i > tailStart + 1) {
            double prevTrend = adjusted[i-1] - adjusted[i-2];
            continuityWave = sin(2.0 * M_PI * baseFrequency * 0.5 * (i - tailStart)) * baseAmplitude * 0.1;
        }

        if (sourceIdx > 0 && sourceIdx < originalData.size() - 1) {
            double actualTrend = originalData[sourceIdx+1] - originalData[sourceIdx-1];
            if (actualTrend * continuityWave < 0) {
                continuityWave *= 0.5;
            }
        }

        adjusted[i] = baseValue + continuityWave;
    }

    // 平滑处理
    int smoothWindow = 3;
    for (int i = sinLength; i < qMin(sinLength + 40, 300); i++) {
        double sum = 0;
        int count = 0;
        for (int j = -smoothWindow; j <= smoothWindow; j++) {
            int idx = i + j;
            if (idx >= sinLength && idx < 300) {
                sum += adjusted[idx];
                count++;
            }
        }
        if (count > 0) {
            adjusted[i] = sum / count;
        }
    }

    return adjusted;
}
