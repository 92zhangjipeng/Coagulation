// =============================================================================
// displaychanneldata.cpp - 通道数据解析、滤波与测试流程处理
// -----------------------------------------------------------------------------
// 模块职责：
//   1. 接收从串口/模组解析出的协议数据，转换为通道原始值（Recv_syncModuleChannelData）
//   2. 对每个通道这一秒内累积的原始数据执行滤波算法（filteringStyle，时间分桶模式）
//   3. 在测试流程中计算血小板聚集率（calculateAggregationRate 等）
//   4. 与 UI 层 / 业务层通信（发送信号、更新曲线、保存结果）
// 线程模型：对象整体移动到 m_workerThread 子线程，共享数据由互斥锁保护
// =============================================================================

#include "displaychanneldata.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QTextCodec>
#include <QDir>
#include <QFileInfo>
#include <QTimer>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>
#include <cmath>

#include "globaldata.h"
#include "loadequipmentpos.h"
#include "mycustomcurve.h"
#include "quiutils.h"
#include "ini_file.h"
#include <QPointer>
#include <operclass/fullyautomatedplatelets.h>
#include <suoweiAggregationRateCalculator/aggregationratecalculator.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// =============================================================================
// 匿名空间：内部使用的复合滤波辅助函数（级联中值 + EWMA）
// =============================================================================
namespace {

// -----------------------------------------------------------------------------
// 中值滤波：抗脉冲噪声
// -----------------------------------------------------------------------------
QVector<double> medianFilter(const QVector<double>& data, int windowSize)
{
    QVector<double> result;
    result.reserve(data.size());
    QVector<double> window;
    window.reserve(windowSize);

    for (int i = 0; i < data.size(); ++i) {
        // 边界处理：获取滑动窗口数据
        int start = qMax(0, i - windowSize / 2);
        int end = qMin(data.size() - 1, i + windowSize / 2);
        window.clear();
        for (int j = start; j <= end; ++j) {
            window.append(data[j]);
        }
        // 排序并取中值
        std::sort(window.begin(), window.end());
        result.append(window[window.size() / 2]);
    }
    return result;
}

// -----------------------------------------------------------------------------
// 指数加权移动平均（EWMA）：平滑随机波动
// -----------------------------------------------------------------------------
QVector<double> ewmaFilter(const QVector<double>& data, double alpha)
{
    QVector<double> result;
    if (data.isEmpty()) {
        return result;
    }
    result.reserve(data.size());
    result.append(data.first()); // 初始值
    for (int i = 1; i < data.size(); ++i) {
        result.append(alpha * data[i] + (1 - alpha) * result.last());
    }
    return result;
}

// -----------------------------------------------------------------------------
// 级联复合滤波：先中值后 EWMA
// -----------------------------------------------------------------------------
QVector<double> cascadeMedianEWMA(const QVector<double>& data)
{
    // 第一级：3 点中值滤波（建议奇数窗口）
    QVector<double> medianFiltered = medianFilter(data, 3);
    // 第二级：EWMA 平滑（alpha=0.3 适合大多数生物信号）
    return ewmaFilter(medianFiltered, 0.3);
}

// -----------------------------------------------------------------------------
// 动态权重终点优化：根据末端信噪比(SNR)动态调整终点权重
// -----------------------------------------------------------------------------
double getFinalOptimalValue(const QVector<double>& rawData)
{
    QVector<double> filtered = cascadeMedianEWMA(rawData);

    // 1. 计算末端信噪比(SNR)
    double noisePower = 0.0;
    for (int i = 1; i < filtered.size(); ++i) {
        noisePower += qPow(filtered[i] - filtered[i - 1], 2);
    }
    double snr = qAbs(filtered.last()) / (qSqrt(noisePower) + 1e-6);

    // 2. 动态权重融合：SNR 越高越信任终点值
    double alpha = qBound(0.6, 1.0 - 0.4 / (1 + snr), 0.9);
    return alpha * filtered.last()
           + (1 - alpha) * filtered[filtered.size() - 2];
}

} // namespace

// =============================================================================
// [组 1] 构造 / 析构 / 启动
// =============================================================================

displayChanneldata::displayChanneldata(QObject *parent)
    : QObject(parent)
    , m_allModulesAccepted(false)
    , m_configChannelCount(0)
    , m_experimenttestData(false)
    , m_prevPRPn(0.0f)
    , m_testColJumpEmptyData(0)
{
    // 初始化试剂偏移数组为 0
    for (int i = 0; i < 5; ++i) {
        m_aggregationRateOffsetPercent[i] = 0.0;
    }

    // 初始化 COL 跳过相关数组
    for (int i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
        m_colSkipRawCount[i] = 0;
        m_channelInColMode[i] = false;
    }

    // ============ 新增：初始化所有通道的平滑控制 ============
    for (int i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
        m_smoothControl[i].isActive = false;
        m_smoothControl[i].startValue = 0.0f;
        m_smoothControl[i].targetValue = 0.0f;
        m_smoothControl[i].currentValue = 0.0f;
        m_smoothControl[i].totalSteps = 30;  // 每秒一个点，30秒完成过渡
        m_smoothControl[i].currentStep = 0;
        m_smoothControl[i].isFirstApply = true;
        m_smoothControl[i].isCompleting = false;

        // ============ 新增：初始化动态步数参数 ============
        m_smoothControl[i].baseSteps = 20;
        m_smoothControl[i].maxSteps = 60;
        m_smoothControl[i].minJump = 3.0f;
        m_smoothControl[i].maxJump = 30.0f;
    }

    // ============ 新增：初始化试剂平滑配置 ============
    initReagentSmoothConfigs();

    sentSamples.clear();
    moveToThread(&m_workerThread);
    connect(&m_workerThread, &QThread::started, this, &displayChanneldata::startthread);
}

displayChanneldata::~displayChanneldata()
{
    shutdown();
    QLOG_TRACE() << "析构解析模组数据线程Id:" << QThread::currentThreadId();
}

void displayChanneldata::shutdown()
{
    // 幂等：已迁回主线程且工作线程已停则直接返回
    if (!m_workerThread.isRunning() && thread() != &m_workerThread) {
        if (m_timerThread) {
            m_timerThread->stopThread();
        }
        return;
    }

    // 在工作线程事件循环仍存活时：停定时器并把对象迁回主线程，避免跨线程 delete
    if (m_workerThread.isRunning() && thread() == &m_workerThread) {
        QThread *mainThread = QCoreApplication::instance()
            ? QCoreApplication::instance()->thread()
            : QThread::currentThread();
        QMetaObject::invokeMethod(this, [this, mainThread]() {
            if (m_timerThread) {
                m_timerThread->stopThread();
            }
            moveToThread(mainThread);
        }, Qt::BlockingQueuedConnection);
    }

    if (m_workerThread.isRunning()) {
        m_workerThread.quit();
        if (!m_workerThread.wait(5000)) {
            QLOG_WARN() << "displayChanneldata worker thread did not stop in time";
        }
    }

    if (m_timerThread) {
        m_timerThread->stopThread();
    }
}

// 启动数据采集：同时从配置文件加载校准参数
void displayChanneldata::startModuleData()
{
    if (!m_workerThread.isRunning()) {
        m_workerThread.start();
        setCalibrationParameters();
    }
}

// 子线程入口：根据设备类型初始化通道容量与实时数据容器
void displayChanneldata::startthread()
{
    QLOG_TRACE() << "模组数据采集线程Id:" << QThread::currentThreadId();

    // 设备类型 -> 通道数量 映射表
    const QHash<quint8, quint8> kDeviceChannelMap = {
        {KS600,  4},
        {KS800,  8},
        {KS1200, 12}
    };

    quint8 equipmentType = KS1200; // 默认值
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);
    const quint8 channelCount = kDeviceChannelMap.value(equipmentType, 12);
    QLOG_DEBUG() << "当前设备类型:" << equipmentType << "通道数量:" << channelCount;

    // 安全方式初始化测试通道标志
    std::fill_n(mOpenChnTest, kMaxChannels, false);
    std::fill_n(m_lastFilterSecondValid, kMaxChannels, false);
    std::fill_n(m_emptyFilterSeconds, kMaxChannels, 0);
    std::fill_n(m_channelHasFrame, kMaxChannels, false);

    // 预分配空间提升性能，并初始化实时数据容器
    QMutexLocker locker(&m_realtimeDataMutex);
    m_ChnRealtimeData.reserve(kMaxChannels);
    for (quint8 chn = 0; chn < kMaxChannels; ++chn) {
        m_ChnRealtimeData.insert(chn + 1, 0);
    }
}

// =============================================================================
// [组 2] 协议数据解析 & 数据分发
// =============================================================================

// 解析从模组接收到的协议数据（固定长度 15 字节）
//   布局：[0]模块地址 + [1..6]协议头 + [7..14]通道数据(每通道 2 字节)
void displayChanneldata::Recv_syncModuleChannelData(QStringList channelData)
{
    constexpr int HEADER_SIZE = 8;
    // baseIndex = HEADER_SIZE - 1 = 7，从该索引开始每通道占 2 字节
    // 最大索引 = 7 + 2*4 - 1 = 14，所需 size = 15
    const int minRequiredSize = HEADER_SIZE - 1 + 2 * FOUR_CHANNELS_PERMODULE;
    if (channelData.size() < minRequiredSize) {
        QLOG_ERROR() << "Recv_syncModuleChannelData: 数据长度不足，期望:" << minRequiredSize
                     << "实际:" << channelData.size();
        return;
    }

    // 使用栈数组避免内存管理开销
    std::array<int, FOUR_CHANNELS_PERMODULE> moduleData{};

    // 模块索引解析（带有效性检查）
    bool conversionOk = false;
    const int moduleIndex = channelData[0].toInt(&conversionOk, HEX_SWITCH);
    if (!conversionOk || moduleIndex < MODULE_1 || moduleIndex > MODULE_3) {
        QLOG_ERROR() << "无效模块地址：" << channelData[0]
                     << "，原始数据：" << channelData;
        return;
    }

    if (!GlobalData::isModuleChannelAdcFrame(channelData)) {
        return;
    }

    const int baseIndex = HEADER_SIZE - 1;
    std::array<bool, FOUR_CHANNELS_PERMODULE> parsed{};
    for (int channel = 0; channel < FOUR_CHANNELS_PERMODULE; ++channel) {
        const int highByteIndex = baseIndex + 2 * channel;
        const int lowByteIndex  = highByteIndex + 1;

        if (highByteIndex >= channelData.size() || lowByteIndex >= channelData.size()) {
            QLOG_ERROR() << "通道数据索引越界（模块" << moduleIndex
                         << " 通道" << channel + 1 << "）"
                         << " highByteIndex=" << highByteIndex
                         << " lowByteIndex=" << lowByteIndex
                         << " size=" << channelData.size();
            continue;
        }

        // 高低字节合并
        const QString& highByte = channelData[highByteIndex];
        const QString& lowByte  = channelData[lowByteIndex];

        bool okHigh, okLow;
        const int value = (lowByte.toInt(&okHigh, HEX_SWITCH) << 8)
                        | highByte.toInt(&okLow, HEX_SWITCH);

        if (!okHigh || !okLow) {
            QLOG_ERROR() << "通道数据解析失败（模块" << moduleIndex
                         << " 通道" << channel + 1 << "）："
                         << "高位字节：" << highByte
                         << "低位字节：" << lowByte;
            continue;
        }
        parsed[channel] = true;
        moduleData[channel] = value;
    }

    // 数据后处理：分发到通道队列
    processChannelData(moduleIndex, moduleData, parsed, channelData);
}

// 模组级数据分发：将 std::array 格式的数据写入每个通道的一秒累积桶
void displayChanneldata::processChannelData(int moduleIndex,
                                            const std::array<int, FOUR_CHANNELS_PERMODULE>& data,
                                            const std::array<bool, FOUR_CHANNELS_PERMODULE>& parsed,
                                            const QStringList& receiveOriginalData)
{
    const int baseChannel = (moduleIndex - MODULE_1) * FOUR_CHANNELS_PERMODULE;

    for (int i = 0; i < FOUR_CHANNELS_PERMODULE; ++i) {
        const int channelNumber = baseChannel + i;
        if (!parsed[i]) {
            continue;
        }
        m_channelHasFrame[channelNumber] = true;
        const int value = data[i];
        if (value <= 0) {
            QLOG_DEBUG() << "模块" << moduleIndex << "通道" << channelNumber + 1
                         << "脏0丢弃不入桶：" << value
                         << "收到原数据:" << receiveOriginalData;
            emit SetChannelValueUpdate(channelNumber, value);
            continue;
        }
        UpdateChannelValue(channelNumber, value);
    }

    // 首次完成所有通道数据接收后，初始化定时器线程
    if (!m_allModulesAccepted) {
        initializeOneTimeChannelUpdate(m_allModulesAccepted);
    }
}

// 首次接收到所有通道数据：启动定时器线程
void displayChanneldata::initializeOneTimeChannelUpdate(bool& allChannelsReceived)
{
    const QHash<quint8, quint8> kChannelConfig = {
        {KS600,  4},
        {KS800,  8},
        {KS1200, 12}
    };

    quint8 equipmentType = KS1200;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);
    const quint8 channelCount = kChannelConfig.value(equipmentType, 12);
    m_configChannelCount = channelCount;

    constexpr int MAX_CHANNELS = sizeof(m_queueChannel) / sizeof(m_queueChannel[0]);
    if (MAX_CHANNELS < channelCount) {
        QLOG_ERROR() << "通道队列容量不足，需要:" << channelCount << "，实际:" << MAX_CHANNELS;
        allChannelsReceived = false;
        return;
    }

    // 脏 0 不入桶后，不能再用「桶非空」判断通道是否到齐；解析成功即算收到过帧
    bool allDataReady = true;
    for (int i = 0; i < channelCount; ++i) {
        if (!m_channelHasFrame[i]) {
            allDataReady = false;
            break;
        }
    }

    if (!m_timerThread) {
        QLOG_DEBUG() << "当前设备类型:" << equipmentType << "，配置通道数:" << channelCount;
        try {
            m_timerThread.reset(new QTimerThread(1));
            QTimerThread* timerRawPtr = m_timerThread.get();
            timerRawPtr->createItem();
            timerRawPtr->startMultThread();

            connect(timerRawPtr, &QTimerThread::OneSecSendData,
                    this, &displayChanneldata::filteringStyle,
                    Qt::QueuedConnection);

            QLOG_INFO() << "定时器线程初始化成功";
        } catch (const std::exception& e) {
            QLOG_FATAL() << "线程创建失败:" << e.what();
            m_timerThread.reset();
            allChannelsReceived = false;
            return;
        }
    }

    allChannelsReceived = allDataReady;
}

// =============================================================================
// [组 3] 通道数据队列 & 更新接口
// =============================================================================

// 更新单个通道值：通知 UI 显示 + 入队一秒累积桶
void displayChanneldata::UpdateChannelValue(const int Channel, const int ChannelData)
{
    emit SetChannelValueUpdate(Channel, ChannelData);
    if (ChannelData <= 0) {
        return;
    }
    ChannelDataLinedup(Channel, ChannelData);
}

// 硬件原始数据入队：累积这一秒内的所有正点（≤0 视为脏数据，不入桶）
void displayChanneldata::ChannelDataLinedup(int index, int ModuleData)
{
    if (index < 0 || index >= MACHINE_SETTING_CHANNEL) {
        QLOG_ERROR() << "ChannelDataLinedup: 索引越界 index=" << index;
        return;
    }
    if (ModuleData <= 0) {
        return;
    }
    QMutexLocker locker(&queueMutex);
    auto& targetQueue = m_queueChannel[index];
    // 只在超过安全上限时丢弃最旧数据，正常情况下纯累积
    if (targetQueue.size() >= dataLengthGroup) {
        targetQueue.dequeue();
    }
    targetQueue.enqueue(ModuleData);
}

// 替换更新通道实时值（滤波后的结果写回 m_ChnRealtimeData）
void displayChanneldata::ReplaceModuleValue(quint8 Channel, int mean)
{
    QMutexLocker locker(&m_realtimeDataMutex);
    auto iter = m_ChnRealtimeData.find(Channel + 1);
    if (iter != m_ChnRealtimeData.end()) {
        iter.value() = mean;
    } else {
        m_ChnRealtimeData.insert(Channel + 1, mean);
    }
    if (mean > 0 && Channel < MACHINE_SETTING_CHANNEL) {
        ++m_positiveFilterSeq[Channel];
    }
}

// 获取某个通道的当前实时值（Channel 为 1 起始）
int displayChanneldata::GetChannelValue(quint8 Channel)
{
    QMutexLocker locker(&m_realtimeDataMutex);
    int value = 0;
    auto mi = m_ChnRealtimeData.find(Channel);
    if (mi != m_ChnRealtimeData.end()) {
        value = mi.value();
    }
    return value;
}

quint32 displayChanneldata::GetPositiveFilterSeq(quint8 Channel)
{
    if (Channel < 1 || Channel > MACHINE_SETTING_CHANNEL) {
        return 0;
    }
    QMutexLocker locker(&m_realtimeDataMutex);
    return m_positiveFilterSeq[Channel - 1];
}

bool displayChanneldata::GetStablePositiveChannelValue(quint8 Channel, quint32 minSeq,
                                                       int& value, quint32& currentSeq)
{
    value = 0;
    currentSeq = 0;
    if (Channel < 1 || Channel > MACHINE_SETTING_CHANNEL) {
        return false;
    }
    const int idx = Channel - 1;
    QMutexLocker locker(&m_realtimeDataMutex);
    currentSeq = m_positiveFilterSeq[idx];
    auto mi = m_ChnRealtimeData.find(Channel);
    if (mi != m_ChnRealtimeData.end()) {
        value = mi.value();
    }
    return m_lastFilterSecondValid[idx] && value > 0 && currentSeq > minSeq;
}

// 断线清理：清空一秒桶、滤波值、界面显示；冻结 PPP/PRP 读数，避免把清出来的 0 当初值
void displayChanneldata::handleCleanModuleBuffData()
{
    m_offlineHoldReads = true;
    m_pppZeroRetryCount = 0;
    m_prpZeroRetryCount = 0;
    std::fill_n(m_lastFilterSecondValid, MACHINE_SETTING_CHANNEL, false);
    std::fill_n(m_emptyFilterSeconds, MACHINE_SETTING_CHANNEL, 0);
    {
        QMutexLocker locker(&queueMutex);
        for (int i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
            m_queueChannel[i].clear();
        }
    }
    for (int i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
        emit SetChannelValueUpdate(i, 0);
    }
    {
        QMutexLocker locker(&m_realtimeDataMutex);
        std::fill_n(m_positiveFilterSeq, MACHINE_SETTING_CHANNEL, 0);
        for (auto it = m_ChnRealtimeData.begin(); it != m_ChnRealtimeData.end(); ++it) {
            it.value() = 0;
        }
    }
    QLOG_WARN() << "仪器掉线：已清空通道一秒桶与滤波值，暂停PPP/PRP读数";
}

void displayChanneldata::handleInstrumentReconnected()
{
    m_offlineHoldReads = false;
    m_pppZeroRetryCount = 0;
    m_prpZeroRetryCount = 0;
    std::fill_n(m_lastFilterSecondValid, MACHINE_SETTING_CHANNEL, false);
    std::fill_n(m_emptyFilterSeconds, MACHINE_SETTING_CHANNEL, 0);
    {
        QMutexLocker locker(&m_realtimeDataMutex);
        std::fill_n(m_positiveFilterSeq, MACHINE_SETTING_CHANNEL, 0);
    }
    QLOG_INFO() << "仪器重连：通道读数恢复，PPP/PRP零值重试计数已复位";
}

void displayChanneldata::slotStopChannelAcquisition(quint8 channelIdx)
{
    if (channelIdx >= MACHINE_SETTING_CHANNEL) {
        return;
    }
    mOpenChnTest[channelIdx] = false;
    ++m_openChnGeneration[channelIdx];
    m_channelInColMode[channelIdx] = false;
    m_colSkipRawCount[channelIdx] = 0;
    QLOG_WARN() << "通道" << channelIdx + 1 << "停止采数";
}

void displayChanneldata::slotAbortAllOpenChannelAcquisition()
{
    for (quint8 i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
        slotStopChannelAcquisition(i);
    }
    QLOG_WARN() << "掉线超时：已停止全部测试通道采数";
}

void displayChanneldata::tryOpenChannelAcquisition(quint8 openChn, quint32 openGeneration)
{
    if (openChn >= MACHINE_SETTING_CHANNEL) {
        return;
    }
    if (openGeneration != m_openChnGeneration[openChn]) {
        QLOG_DEBUG() << "通道" << openChn + 1 << "延时打开已作废";
        return;
    }
    if (!cglobal::gserialConnecStatus.load() || m_offlineHoldReads) {
        QPointer<displayChanneldata> guard(this);
        QTimer::singleShot(500, this, [guard, openChn, openGeneration]() {
            if (guard) {
                guard->tryOpenChannelAcquisition(openChn, openGeneration);
            }
        });
        return;
    }
    mOpenChnTest[openChn] = true;
    QLOG_INFO() << "[TestChn] acquisition_open chn" << (openChn + 1);
}

// =============================================================================
// [组 4] 每秒滤波主循环 & 滤波算法实现
// =============================================================================

// 每秒触发一次：对这一秒内累积的所有原始数据进行滤波，输出一个值
// 策略：时间分桶（Tumbling Window）— 每一秒的数据独立处理，处理后清空累积桶
void displayChanneldata::filteringStyle()
{
    if (!cglobal::gserialConnecStatus.load()) {
        return;
    }

    const int filterMode = INI_File().getFilteringMode();

    for (int i = 0; i < m_configChannelCount; ++i) {
        QVector<int> recvData;
        {
            QMutexLocker locker(&queueMutex);
            auto& queue = m_queueChannel[i];
            recvData.reserve(queue.size());
            while (!queue.isEmpty()) {
                recvData.append(queue.dequeue());
            }
        }

        // --- 步骤 2：对累积数据执行滤波。空秒（无正点）不算数；连续两秒无正点才记物理 0
        if (recvData.isEmpty()) {
            ++m_emptyFilterSeconds[i];
            if (m_emptyFilterSeconds[i] >= kPhysicalZeroEmptySeconds) {
                ReplaceModuleValue(i, 0);
                m_lastFilterSecondValid[i] = true;
                if (m_emptyFilterSeconds[i] == kPhysicalZeroEmptySeconds) {
                    QLOG_WARN() << "通道" << i + 1
                                << "连续" << kPhysicalZeroEmptySeconds
                                << "秒无正点，记为物理挡光0";
                }
            } else {
                m_lastFilterSecondValid[i] = false;
            }
            continue;
        }

        m_emptyFilterSeconds[i] = 0;
        int result = 0;
        switch (filterMode) {
            case FILTER_NO:
                result = recvData.last();
                break;
            case FILTER_AVERAGE_VALUE:
                result = Medianaveragefiltering(recvData, i);
                break;
            case FILTER_MIDVALUE:
                result = optimizedMedianFiltering(recvData);
                break;
            case MEDIAN_EWMA_DYNAMIC:
                result = MedianEWMADynamicAdaptiveFilter(recvData);
                break;
            default:
                QLOG_WARN() << "未知滤波模式，使用默认无滤波";
                result = recvData.last();
                break;
        }
        if (result <= 0) {
            m_lastFilterSecondValid[i] = false;
            continue;
        }
        ReplaceModuleValue(i, result);
        m_lastFilterSecondValid[i] = true;
    }

    outPutModuleTestData();
}

// 中位值平均滤波（又称防脉冲干扰平均滤波）：移除最大最小值后求平均
int displayChanneldata::Medianaveragefiltering(QVector<int> recvData, const int channel)
{
    const int size = recvData.size();

    if (size <= 2) {
        return std::accumulate(recvData.begin(), recvData.end(), 0) / std::max(1, size);
    }

    QVector<int> tempData = recvData;
    auto first = tempData.begin();
    auto last = tempData.end();

    // 使用 std::nth_element 高效放置最小值到首位置、最大值到末位置
    std::nth_element(first, first, last);
    std::nth_element(first, last - 1, last);

    // 移除首尾（最小/最大值）后求平均
    tempData.removeFirst();
    tempData.removeLast();
    const double sum = std::accumulate(tempData.begin(), tempData.end(), 0.0);
    int result = static_cast<int>(sum / tempData.size());

    // 实验模式下记录滤波原始数据到 CSV
    if (INI_File().getexperimentalMode() && m_experimenttestData.load()) {
        QString fileName = QString("%1%2.csv").arg("Medianaveragefilter").arg(channel);
        saveFilterDataToCSV(recvData, result, fileName, QCoreApplication::applicationDirPath());
    }

    return result;
}

// 中值滤波：O(n) 快速选择，偶数长度取中间两个的平均值
int displayChanneldata::optimizedMedianFiltering(QVector<int>& data)
{
    if (data.isEmpty()) {
        return 0;
    }
    const int n = data.size();
    const auto mid = data.begin() + n / 2;
    std::nth_element(data.begin(), mid, data.end());
    if (n % 2 == 1) {
        return *mid;
    }
    // 偶数长度：中间两个的平均值
    const auto leftMid = std::max_element(data.begin(), mid);
    return (*leftMid + *mid) / 2;
}

// 级联中值 + EWMA 动态自适应滤波：适用于高精度场景
int displayChanneldata::MedianEWMADynamicAdaptiveFilter(const QVector<int>& intVec)
{
    if (intVec.isEmpty()) {
        QLOG_WARN() << "MedianEWMADynamicAdaptiveFilter: 输入数据为空";
        return 0;
    }

    // 类型转换（内存连续访问提升性能）
    QVector<double> doubleData;
    doubleData.reserve(intVec.size());
    const int* src = intVec.constData();
    for (int i = 0; i < intVec.size(); ++i) {
        doubleData.append(static_cast<double>(src[i]));
    }

    // 应用复合滤波并智能舍入
    const double outResult = getFinalOptimalValue(doubleData);
    return static_cast<int>(std::round(outResult));
}

// =============================================================================
// [组 5] 测试流程主控制：启动 / 完成 / 数据处理
// =============================================================================

// 轮询所有开放中的测试通道，依次调用 processChannelData
void displayChanneldata::outPutModuleTestData()
{
    // 设备暂停只停机械臂调度，已在反应的通道继续采数、算聚集率
    constexpr auto kMaxChannels = MACHINE_SETTING_CHANNEL;
    for (int channelIdx = 0; channelIdx < kMaxChannels; ++channelIdx) {
        if (!mOpenChnTest[channelIdx]) {
            continue;
        }
        processChannelData(channelIdx);
    }
}

// 单个通道测试数据处理：读取测试参数并委派给 calculationFormula
void displayChanneldata::processChannelData(int channelIdx)
{
    const quint8 channelNumber = channelIdx + 1;
    if (channelNumber > kMaxChannels) {
        QLOG_WARN() << "无效通道号:" << channelNumber;
        return;
    }

    // 读取滤波后的当前值
    int currentValue = 0;
    {
        QMutexLocker locker(&m_realtimeDataMutex);
        const auto dataIt = m_ChnRealtimeData.constFind(channelNumber);
        if (dataIt == m_ChnRealtimeData.constEnd()) {
            QLOG_WARN() << "通道没有数据" << channelNumber;
            return;
        }
        currentValue = dataIt.value();
    }

    if (!m_lastFilterSecondValid[channelIdx] || currentValue <= 0) {
        return;
    }

    // 从业务层获取该通道的测试参数
    struct TestParams {
        int anaemiaValue = 0;
        int bloodValue = 0;
        quint8 reagentIndex = 0;
        int testSample = 0;
    } params;

    const QString sampleName = StructInstance::getInstance()->testingOutInitPPPandPRP(
            channelIdx, params.anaemiaValue, params.bloodValue,
            params.reagentIndex, params.testSample);

    if (params.reagentIndex == ANEMIA) {
        return;
    }

    calculationFormula(sampleName, params.reagentIndex, currentValue,
                       params.anaemiaValue, params.bloodValue, channelNumber);
}

// 测试流程主控制器：COL 丢弃 / 异常检测 / 数据保存 / 完成判断
void displayChanneldata::calculationFormula(const QString& sampleNum,
                                            quint8 reagentIndex,
                                            int currentRichValue,
                                            int baselinePoor,
                                            int baselineRich,
                                            int channelIdx)
{
    // 提前计算 0-based 索引
    int chnIndex = channelIdx - 1;
    if (chnIndex < 0 || chnIndex >= MACHINE_SETTING_CHANNEL) {
        QLOG_ERROR() << "无效通道号:" << channelIdx;
        return;
    }

    // ---------- COL 试剂丢弃前 n 个数据点 ----------
    if (isCOLReagent(reagentIndex) && m_channelInColMode[chnIndex]) {
        m_colSkipRawCount[chnIndex]++;
        if (m_colSkipRawCount[chnIndex] <= m_testColJumpEmptyData) {
            QLOG_DEBUG() << "COL 模式丢弃通道" << channelIdx
                         << "第" << m_colSkipRawCount[chnIndex] << "个数据点";
            return;
        }
    }

    // 从业务层获取已采集的数据点总数
    int totalDataPoints = 0;
    StructInstance::getInstance()->fetchTestDataTotal(sampleNum, reagentIndex, totalDataPoints);
    if (totalDataPoints == 0) {
        resetSmoothControl(chnIndex);
        QLOG_DEBUG() << "【平滑】通道" << channelIdx << "新测试开始，已重置状态";
    }

    // 已达到测试总数：结束当前试剂的测试
    if (totalDataPoints == NUMBEROFTESTDATA) {
        handleTestCompletion(sampleNum, reagentIndex, channelIdx);
        onReagentTestComplete(sampleNum, reagentIndex);
        return;
    }

    // 首次采集时检测样本异常（PRP 0 值、贫血值异常）
    if (totalDataPoints == 0) {
        sampleAbnormality(baselineRich, baselinePoor, currentRichValue,
                          sampleNum, channelIdx, reagentIndex);
    }

    // 主逻辑：计算聚集率并更新 UI / 保存数据
    processTestData(sampleNum, reagentIndex, currentRichValue,
                    baselinePoor, baselineRich, channelIdx, totalDataPoints);
}

// 单个试剂测试完成处理：清状态 + 发信号 + 保存 + 清理缓存
void displayChanneldata::handleTestCompletion(const QString& sampleNum,
                                              quint8 reagentIndex,
                                              int channelIdx)
{
    m_experimenttestData.store(false);

    // 重置该通道的 COL 模式标志与跳过计数
    int chnIndex = channelIdx - 1;
    if (chnIndex >= 0 && chnIndex < MACHINE_SETTING_CHANNEL) {
        m_channelInColMode[chnIndex] = false;
        m_colSkipRawCount[chnIndex] = 0;

        // ============ 新增：测试完成时重置该通道的平滑状态 ============
        resetSmoothControl(chnIndex);
        QLOG_DEBUG() << "【平滑】通道" << channelIdx << "测试完成，已重置状态";
    }

    // 该样本是否已触发过异常告警
    bool hasTestError = false;
    {
        QMutexLocker locker(&m_sentSamplesMutex);
        hasTestError = sentSamples.contains(sampleNum);
        if (hasTestError) {
            sentSamples.remove(sampleNum);
        }
    }
    StructInstance::getInstance()->setupOneReagentsIsComplete(sampleNum, reagentIndex, hasTestError);

    if (channelIdx <= 0) {
        QLOG_WARN() << "通道号小于等于 0 有越界风险";
    } else {
        mOpenChnTest[channelIdx - 1] = false;
    }

    QLOG_INFO() << "[TestChn] reag_done sample" << sampleNum
                << "reag" << reagentIndex << "chn" << channelIdx
                << "-> testComplete";
    emit finishtestProgress(channelIdx - 1, reagentIndex);
    emit testComplete(sampleNum, channelIdx - 1, reagentIndex);

    // 耗材消耗统计
    FullyAutomatedPlatelets::pinstancesqlData()->testendAddStasReagent(sampleNum, reagentIndex);

    // 删除完成单个试剂的 PRP 值缓存
    processor.removeReagentPrpValue(sampleNum, reagentIndex);

    clearAlgorithmCache();
}

// 测试中：获取基线、计算聚集率、应用试剂偏移、保存数据并通知 UI
void displayChanneldata::processTestData(const QString& sampleNum,
                                         quint8 reagentIndex,
                                         int currentRichValue,
                                         int baselinePoor,
                                         int baselineRich,
                                         int channelIdx,
                                         int totalDataPoints)
{
    if (totalDataPoints >= NUMBEROFTESTDATA) {
        return;
    }

    // ============ 1. 计算基础聚集率 ============
    const bool isLogMode = getAbsorbanceAlgorithm();
    baselinePoor = getBaselinePoorValue(channelIdx, baselinePoor);

    bool usedTestingPrpVal = false;
    int baselineRichForCalc = processor.recvTestPara(sampleNum, usedTestingPrpVal, reagentIndex,
                                                     currentRichValue, baselineRich, totalDataPoints);
    Q_UNUSED(usedTestingPrpVal);

    const float resultValue = calculateAggregationRate(isLogMode,
            static_cast<float>(currentRichValue),
            static_cast<float>(baselineRichForCalc),
            static_cast<float>(baselinePoor));

    float finalResult = applyOffsetToAggregationRate(resultValue, reagentIndex);

    // ============ 2. 获取无效偏移配置 ============
    updateInvalidConfigCache(reagentIndex);

    const quint8 reagentIdx = reagentIndex - 1;
    const quint8 triggerInvalid = m_invalidConfigCache[reagentIdx].triggerInvalid;
    const quint8 triggerOffset = m_invalidConfigCache[reagentIdx].triggerOffset;
    const quint8 invalidMin = m_invalidConfigCache[reagentIdx].invalidMin;

    const int R = getOrCreateRandomR(sampleNum, reagentIndex);
    const float triggerVal = static_cast<float>(triggerInvalid) / 100.0f;

    // ============ 3. 判断是否需要应用偏移 ============
    const bool shouldApplyOffset = (totalDataPoints >= static_cast<int>(invalidMin) &&
                                    totalDataPoints >= R &&
                                    finalResult <= triggerVal);

    const int chnIndex = channelIdx - 1;

    // ============ 4. 处理偏移逻辑 ============
    if (shouldApplyOffset) {
        finalResult = handleOffsetApplied(sampleNum, reagentIndex, currentRichValue,
                                          baselineRichForCalc, baselinePoor,
                                          channelIdx, chnIndex, totalDataPoints,
                                          finalResult, triggerOffset, R, triggerInvalid);
    } else {
        finalResult = handleNoOffset(channelIdx, chnIndex, totalDataPoints, finalResult);
    }

    // ============ 5. 保存数据并更新UI ============
    StructInstance::getInstance()->updteSaveChnTestData(channelIdx, reagentIndex,
                                                        currentRichValue, finalResult);
    FullyAutomatedPlatelets::pinstanceTesting()->showTestChannelInfo(channelIdx, sampleNum, reagentIndex);
    emit DisplayTestingValue(sampleNum, reagentIndex, channelIdx, finalResult,
                             currentRichValue, baselineRichForCalc, baselinePoor);
}


// =============================================================================
// 辅助函数：处理无偏移情况
// =============================================================================
float displayChanneldata::handleNoOffset(int channelIdx, int chnIndex,
                                         int totalDataPoints, float finalResult)
{
    // 重置平滑状态
    if (m_smoothControl[chnIndex].isActive) {
        m_smoothControl[chnIndex].isActive = false;
        m_smoothControl[chnIndex].currentStep = 0;
        QLOG_DEBUG() << "【平滑取消】通道" << channelIdx
                     << " shouldApplyOffset=false，重置平滑状态";
    }

    QLOG_DEBUG() << "【无偏移】通道" << channelIdx
                 << " 数据点:" << totalDataPoints
                 << " 直接输出原值:" << finalResult * 100.0f << "%";

    return finalResult;
}

// =============================================================================
// 处理有偏移情况（包含平滑 + 动态步数）
// =============================================================================
float displayChanneldata::handleOffsetApplied(const QString& sampleNum,
                                              quint8 reagentIndex,
                                              int currentRichValue,
                                              int baselineRichForCalc,
                                              int baselinePoor,
                                              int channelIdx,
                                              int chnIndex,
                                              int totalDataPoints,
                                              float finalResult,
                                              quint8 triggerOffset,
                                              int R,
                                              quint8 triggerInvalid)
{
    // 标记未使用的参数（消除编译警告）
    Q_UNUSED(sampleNum);
    Q_UNUSED(reagentIndex);
    Q_UNUSED(currentRichValue);
    Q_UNUSED(baselineRichForCalc);
    Q_UNUSED(baselinePoor);
   // Q_UNUSED(totalDataPoints);
    //Q_UNUSED(R);

    // 计算目标值
    const float percentValue = finalResult * 100.0f;
    float targetResult = 0.0f;


    float tempPercent = percentValue;
    int offsetCount = 0;
    const int maxIterations = 100; // 防止死循环

    // 当数值小于 triggerInvalid 且未超过最大迭代次数时继续
    while (tempPercent < triggerInvalid && offsetCount < maxIterations) {
        tempPercent += static_cast<float>(triggerOffset);
        offsetCount++;
    }

    targetResult = tempPercent / 100.0f;

    QLOG_DEBUG() << "【负值偏移】通道" << channelIdx<<"数据点"<<totalDataPoints
                         << " 原始值:" << percentValue << "%"
                         << " 累加次数:" << offsetCount
                         << " 累加后值:" << tempPercent << "%"
                         << " 目标值:" << targetResult * 100.0f << "%";

    SmoothControl& control = m_smoothControl[chnIndex];

    // --- 4.1 平滑状态机 ---
    if (!control.isActive) {
        // 首次激活：根据跳跃幅度计算步数
        float targetPercent = targetResult * 100.0f;
        int calculatedSteps = calculateDynamicSteps(chnIndex, percentValue, targetPercent, reagentIndex);
        control.totalSteps = calculatedSteps;
        control.startValue = finalResult;
        control.currentValue = finalResult;
        control.targetValue = targetResult;
        control.currentStep = 0;
        control.isActive = true;

        QLOG_DEBUG() << "【平滑激活】通道" << channelIdx
                     << " 起始值:" << finalResult * 100.0f << "%"
                     << " 目标值:" << targetResult * 100.0f << "%"
                     << " 总步数:" << control.totalSteps;
    }
    else if (control.currentStep < control.totalSteps) {
        // ============ 平滑进行中：检测目标值是否变化 ============
       float currentTargetPercent = control.targetValue * 100.0f;
       float newTargetPercent = targetResult * 100.0f;
       float targetDiff = qAbs(newTargetPercent - currentTargetPercent);

       if (targetDiff > 1.0f) {
           // 目标值变化较大，重新计算剩余步数
           float currentPercent = control.currentValue * 100.0f;
           int remainingSteps = control.totalSteps - control.currentStep;
           int recalculatedSteps = calculateDynamicSteps(chnIndex, currentPercent, newTargetPercent, reagentIndex);

           // 如果重新计算的步数大于剩余步数，延长总步数
           if (recalculatedSteps > remainingSteps) {
               int extraSteps = recalculatedSteps - remainingSteps;
               control.totalSteps += extraSteps;
               QLOG_DEBUG() << "【步数延长】通道" << channelIdx
                            << " 目标变化:" << targetDiff << "%"
                            << " 增加:" << extraSteps << "步"
                            << " 新总步数:" << control.totalSteps;
           }
       }

       // 更新目标值
       control.startValue = control.currentValue;
       control.targetValue = targetResult;

       QLOG_DEBUG() << "【平滑继续】通道" << channelIdx
                    << " 步数:" << control.currentStep << "/" << control.totalSteps
                    << " 新目标值:" << targetResult * 100.0f << "%";
    }
    else {
        // 平滑已完成：直接使用目标值
        control.currentValue = targetResult;
        QLOG_DEBUG() << "【平滑已完成】通道" << channelIdx
                     << " 直接使用目标值:" << targetResult * 100.0f << "%";
        return targetResult;
    }

    // --- 4.2 执行平滑插值 ---
    float result = finalResult;

    if (control.isActive && control.currentStep < control.totalSteps) {
        result = applySmoothTransition(chnIndex, control.targetValue);
        logSmoothDebugInfo(channelIdx, sampleNum, totalDataPoints, R,
                          triggerInvalid, triggerOffset, percentValue,
                          control.targetValue, result, control);
    }
    else if (control.isActive && control.currentStep >= control.totalSteps) {
        // 平滑刚完成
        control.isActive = false;
        result = control.targetValue;
        QLOG_DEBUG() << "【平滑完成】通道" << channelIdx<<"数据点"<<totalDataPoints
                     << " 最终值:" << result * 100.0f << "%";
    }

    return result;
}

// =============================================================================
// 辅助函数：打印平滑调试信息
// =============================================================================
void displayChanneldata::logSmoothDebugInfo(int channelIdx,
                                            const QString& sampleNum,
                                            int totalDataPoints,
                                            int R,
                                            quint8 triggerInvalid,
                                            quint8 triggerOffset,
                                            float percentValue,
                                            float targetValue,
                                            float currentResult,
                                            const SmoothControl& control)
{
    QLOG_DEBUG() << "========================================";
    QLOG_DEBUG() << "【无效偏移调试信息】通道:" << channelIdx;
    QLOG_DEBUG() << "  样本编号:" << sampleNum;
    QLOG_DEBUG() << "  数据点数:" << totalDataPoints;
    QLOG_DEBUG() << "  随机数R:" << R;
    QLOG_DEBUG() << "  触发阈值:" << triggerInvalid << "%";
    QLOG_DEBUG() << "  原始值:" << percentValue << "%";
    QLOG_DEBUG() << "  偏移值:" << triggerOffset << "%";
    QLOG_DEBUG() << "  目标值:" << targetValue * 100.0f << "%";
    QLOG_DEBUG() << "  当前结果:" << currentResult * 100.0f << "%";
    QLOG_DEBUG() << "  平滑步数:" << control.currentStep << "/" << control.totalSteps;
    QLOG_DEBUG() << "========================================";
}

// ============ 新增：重置平滑状态（在测试开始时调用） ============
// 重置指定通道的平滑状态
void displayChanneldata::resetSmoothControl(int channelIdx)
{
    if (channelIdx < 0 || channelIdx >= MACHINE_SETTING_CHANNEL) {
        QLOG_WARN() << "【平滑】重置：无效通道索引:" << channelIdx;
        return;
    }

    SmoothControl& control = m_smoothControl[channelIdx];
    control.isActive = false;
    control.startValue = 0.0f;
    control.targetValue = 0.0f;
    control.currentValue = 0.0f;
    control.currentStep = 0;
    control.isFirstApply = true;
    control.isCompleting = false;

    // ============ 重置动态步数参数（保持配置不变） ============
    control.totalSteps = 30;  // 重置为默认值，下次会重新计算

    QLOG_DEBUG() << "【平滑】通道" << channelIdx + 1 << "状态已重置";
}
// 重置所有通道的平滑状态
void displayChanneldata::resetAllSmoothControls()
{
    for (int i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
        m_smoothControl[i].isActive = false;
        m_smoothControl[i].startValue = 0.0f;
        m_smoothControl[i].targetValue = 0.0f;
        m_smoothControl[i].currentValue = 0.0f;
        m_smoothControl[i].currentStep = 0;
        m_smoothControl[i].isFirstApply = true;
        m_smoothControl[i].isCompleting = false;
    }
    QLOG_DEBUG() << "【平滑】所有通道状态已重置";
}

float displayChanneldata::applySmoothTransition(int channelIdx, float newValue)
{
    if (channelIdx < 0 || channelIdx >= MACHINE_SETTING_CHANNEL) {
        QLOG_ERROR() << "【平滑】无效通道索引:" << channelIdx;
        return newValue;
    }

    SmoothControl& control = m_smoothControl[channelIdx];

    if (!control.isActive) {
        control.startValue = newValue;
        control.targetValue = newValue;
        control.currentValue = newValue;
        control.currentStep = 0;
        control.isFirstApply = true;
        control.isCompleting = false;
        return newValue;
    }

    // 如果目标值变化，更新起始值保持连续性
    if (qAbs(newValue - control.targetValue) > 0.000001f) {
        control.startValue = control.currentValue;
        control.targetValue = newValue;
        // currentStep 保持不变
    }

    if (!control.isActive) {
        control.currentValue = newValue;
        return newValue;
    }

    // 执行平滑插值（使用 control.totalSteps）
    if (control.currentStep < control.totalSteps) {
        float progress = static_cast<float>(control.currentStep) /
                        static_cast<float>(control.totalSteps);

        // 使用 ease-in-out 曲线
        float smoothProgress = progress * progress * (3.0f - 2.0f * progress);

        float interpolatedValue = control.startValue +
                                 (control.targetValue - control.startValue) * smoothProgress;

        control.currentStep++;
        control.currentValue = interpolatedValue;
        control.isCompleting = false;

        // 每5步或最后一步输出日志
        if (control.currentStep % 5 == 0 || control.currentStep == 1 ||
            control.currentStep >= control.totalSteps) {
            QLOG_DEBUG() << "【平滑进行中】通道" << channelIdx + 1
                         << " 步骤:" << control.currentStep
                         << "/" << control.totalSteps
                         << " 当前值:" << interpolatedValue * 100.0f << "%"
                         << " 目标值:" << control.targetValue * 100.0f << "%";
        }

        return interpolatedValue;
    } else {
        // 过渡完成
        control.isActive = false;
        control.currentValue = control.targetValue;
        control.isFirstApply = false;
        control.isCompleting = true;

        QLOG_DEBUG() << "【平滑完成】通道" << channelIdx + 1
                     << " 最终值:" << control.targetValue * 100.0f << "%"
                     << " 总步数:" << control.totalSteps;
        return control.targetValue;
    }
}


void displayChanneldata::updateInvalidConfigCache(quint8 reagentIndex)
{
    static INI_File config;
	const quint8 ktesttingReag = reagentIndex - 1;

    // 检查参数范围
    if (ktesttingReag >= 5) {
        QLOG_WARN() << "Invalid reagent index:" << reagentIndex;
        return;
    }

    // 只在首次调用或配置无效时读取
    if (!m_invalidConfigCache[ktesttingReag].isValid) {
        m_invalidConfigCache[ktesttingReag].triggerInvalid = config.getTriggerInvalid(reagentIndex);
        m_invalidConfigCache[ktesttingReag].triggerOffset = config.getTriggerOffset(reagentIndex);
        m_invalidConfigCache[ktesttingReag].invalidMin = config.getInvalidMin(reagentIndex);
        m_invalidConfigCache[ktesttingReag].invalidMax = config.getInvalidMax(reagentIndex);
        m_invalidConfigCache[ktesttingReag].isValid = true;

        QLOG_DEBUG() << "Loaded invalid config for reagent:" << reagentIndex
                     << "TriggerInvalid:" << m_invalidConfigCache[ktesttingReag].triggerInvalid
                     << "TriggerOffset:" << m_invalidConfigCache[ktesttingReag].triggerOffset
                     << "InvalidMin:" << m_invalidConfigCache[ktesttingReag].invalidMin
                     << "InvalidMax:" << m_invalidConfigCache[ktesttingReag].invalidMax;
    }
}
QString displayChanneldata::makeCacheKey(const QString& sampleNum, quint8 reagentIndex) const
{
    return sampleNum + "_" + QString::number(reagentIndex);
}

int displayChanneldata::getOrCreateRandomR(const QString& sampleNum, quint8 reagentIndex)
{
    QString key = makeCacheKey(sampleNum, reagentIndex);

    // 检查是否已存在该样本+试剂的随机数
    auto it = m_sampleReagentRandomCache.find(key);
    if (it != m_sampleReagentRandomCache.end()) {
        return it.value();
    }

    // 确保配置已加载
    updateInvalidConfigCache(reagentIndex);

    // 获取配置值
    quint8 ktesttingReag = reagentIndex - 1;
    quint8 invalidMin = m_invalidConfigCache[ktesttingReag].invalidMin;
    quint8 invalidMax = m_invalidConfigCache[ktesttingReag].invalidMax;

    int R = QRandomGenerator::global()->bounded(static_cast<int>(invalidMin),
                                                static_cast<int>(invalidMax) + 1);//因为不包含上限，所以要+1

    // 缓存起来
    m_sampleReagentRandomCache.insert(key, R);

    return R;
}

void displayChanneldata::onReagentTestComplete(const QString& sampleNum, quint8 reagentIndex)
{
    QString key = makeCacheKey(sampleNum, reagentIndex);
    m_sampleReagentRandomCache.remove(key);
}







// 样本异常检测：PRP 0 值提示 + PPP 0 值取消任务
bool displayChanneldata::sampleAbnormality(const int& initprp,
                                           const int anaemiaValue,
                                           const int& curprp,
                                           const QString& sampleid,
                                           const quint8& channelIdx,
                                           const quint8 indexReagent)
{
    // PRP 可能为 0：样本量较少，提示但不取消任务（每个样本只提示一次）
    if (initprp <= 0 || curprp <= 0) {
        bool needInsert = false;
        {
            QMutexLocker locker(&m_sentSamplesMutex);
            if (!sentSamples.contains(sampleid)) {
                needInsert = true;
                sentSamples.insert(sampleid);
            }
        }
        if (needInsert) {
            QString reagentName = QUIUtils::index_reagent_mapping_reagentName(indexReagent);
            QString errorMsg = QString("样本ID %1[%2] 通道 %3 可能PRP样本量较少")
                    .arg(sampleid).arg(reagentName).arg(channelIdx);
            QLOG_WARN() << errorMsg;
            emit channelDataError(errorMsg);
        }
    }

    // 贫血值为 0：取消任务
    if (anaemiaValue == 0) {
        bool needEmit = false;
        {
            QMutexLocker locker(&m_sentSamplesMutex);
            if (!sentSamples.contains(sampleid)) {
                needEmit = true;
                sentSamples.insert(sampleid);
            }
        }
        if (needEmit) {
            emit sampleTestingErr(sampleid, channelIdx - 1);
            return true;
        }
    }
    return false;
}

// =============================================================================
// [组 6] 聚集率计算相关辅助
// =============================================================================

// 获取 PPP 基线值：通道覆盖优先，否则使用默认基线
int displayChanneldata::getBaselinePoorValue(int channelIdx, int currentBaseline)
{
    const quint16 channelPPPValue = customPPPValue::getChannelPPPValues(channelIdx);
    return (channelPPPValue != 0) ? channelPPPValue : currentBaseline;
}

// 对数算法：分段校准聚集率
double displayChanneldata::calculateTestAggregationRate(double prpn, double ppp, double prp0)
{
    if (prp0 == 0) {
        QLOG_WARN() << "PRP0为0，避免除零错误";
        return 0.0;
    }

    double k = ppp / prp0;
    double prpRatio = prpn / prp0;

    if (prpRatio <= 0) {
        QLOG_WARN() << "PRP比率非正数，无法计算对数" << prpRatio;
        return 0.0;
    }

    double denominator = 1.0;
    bool conditionMatched = false;

    if (k < mk1 && k > 0) {
        denominator = std::log10(mcalibrationFactor1);
        conditionMatched = true;
    } else if (k >= mk2min && k < mk2max) {
        if (prpn > ppp) {
            float randVal = getRandomFactor(0.95f, 1.00f);
            prpn = ppp * randVal;
            prpRatio = prpn / prp0;
            if (prpRatio <= 0) {
                return 0.0;
            }
        }
        denominator = std::log10(k);
        conditionMatched = true;
    } else if (k >= mk3min && k < mk3max) {
        double adjustedK = (ppp * mcalibrationFactor3) / prp0;
        if (adjustedK <= 0) {
            return 0.0;
        }
        denominator = std::log10(adjustedK);
        conditionMatched = true;
    } else if (k <= mk4) {
        float adjustedK = (ppp * mcalibrationFactor4) / prp0;
        if (adjustedK <= 0) {
            return 0.0;
        }
        denominator = std::log10(adjustedK);
        conditionMatched = true;
    }

    // 未命中任何分段：默认公式 log10(PRPn/PRP0) / log10(PPP/PRP0)
    if (!conditionMatched) {
        if (k <= 0) {
            return 0.0;
        }
        denominator = std::log10(k);
    }

    // 检查分母合法性
    if (denominator == 0 || std::isnan(denominator) || std::isinf(denominator)) {
        return 0.0;
    }

    double result = std::log10(prpRatio) / denominator;
    if (std::isnan(result) || std::isinf(result)) {
        return 0.0;
    }
    return result;
}

// 对外接口：根据算法模式分派到对数 / 线性算法
float displayChanneldata::calculateAggregationRate(const bool isLogMode,
                                                   float PRPn,
                                                   float PRP0,
                                                   float PPP)
{
    float result = 0.0f;

    // PRPn 微调：避免 PRPn >= PPP 导致无意义结果
    if (PRPn >= PPP) {
        float randVal = getRandomFactor(0.95f, 1.00f);
        PRPn = PPP * randVal;
    }

    if (isLogMode) {
        double outResult = calculateTestAggregationRate(PRPn, PPP, PRP0);
        result = static_cast<double>(outResult);
    } else {
        try {
            bool success;
            QString errorMsg;
            double safeRate = AggregationRateCalculator::calculateAggregationRateSafe(
                    PRPn, PRP0, PPP, &success, &errorMsg);
            result = static_cast<float>(safeRate);
            if (!success) {
                QLOG_DEBUG() << "计算失败:" << errorMsg;
            }
        } catch (const std::exception& e) {
            QLOG_WARN() << "计算错误:" << e.what();
        }
    }
    return result;
}

// 线程安全的均匀分布随机数：用于 PRPn 微调
float displayChanneldata::getRandomFactor(float min, float max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    if (min > max) {
        QLOG_WARN() << "getRandomFactor: min > max，交换值 min=" << min << " max=" << max;
        std::swap(min, max);
    }
    if (min == max) {
        return min;
    }
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

// 按试剂类型应用整体偏移（+n%），用于不同试剂的一致性校准
float displayChanneldata::applyOffsetToAggregationRate(float originalRate, quint8 reagentIndex)
{
    if (reagentIndex > 0 && reagentIndex <= 5) {
        float offsetRate = originalRate * (1.0f + static_cast<float>(
                m_aggregationRateOffsetPercent[reagentIndex - 1]));
        return offsetRate;
    }
    return originalRate;
}

// 从配置文件加载校准参数：包括分段边界、校准系数、试剂偏移、COL 跳过点数
void displayChanneldata::setCalibrationParameters()
{
    static INI_File config;

    mk1 = config.rConfigPara(MAXK1).toDouble();
    mk2min = config.rConfigPara(MINK2).toDouble();
    mk2max = config.rConfigPara(MAXK2).toDouble();
    mk3min = config.rConfigPara(MINK3).toDouble();
    mk3max = config.rConfigPara(MAXK3).toDouble();
    mk4 = config.rConfigPara(MINK4).toDouble();
    mcalibrationFactor1 = config.rConfigPara(RATIOK1).toDouble();
    mcalibrationFactor3 = config.rConfigPara(RATIOK3).toDouble();
    mcalibrationFactor4 = config.rConfigPara(RATIOK4).toDouble();

    quint8 jumpEmptyCol = config.getColTestGiveupNum();
    m_testColJumpEmptyData = jumpEmptyCol;

    // 读取每种试剂的偏移百分比
    for (int reagent = AA_REAGENT; reagent <= RIS_REAGENT; ++reagent) {
        float offsetPercent = config.getoffsetTestRatio(reagent);
        m_aggregationRateOffsetPercent[reagent - 1] = offsetPercent / 100.0;
        QLOG_DEBUG() << "试剂类型" << reagent << "偏移设置为：" << offsetPercent << "%";
    }
}

// =============================================================================
// [组 7] 对外槽函数：贫血值读取 / 富血初始值 / 打开通道测试 / 质控值
// =============================================================================

// 读取贫血值(PPP)：实验模式校验 2000-3000；正常模式不判范围，读到 0 则重试最多 8 次
void displayChanneldata::slotgetAnemiaValue()
{
    int sampleid = 0;
    quint8 testChnindex = 0;
    m_experimenttestData.store(true);
    sampleid = StructInstance::getInstance()->ClipEndGetAnemiaValeChn(
                testChnindex, FOCUS_CLIP_ANEMIA_TO_CHN, "准备读取贫血值");
    if (sampleid <= 0) {
        m_pppZeroRetryCount = 0;
        QLOG_WARN() << "[TestChn] ppp_read_no_focus 未定位夹PPP焦点样本";
        emit testingLogHint(tr("未定位到夹PPP焦点样本，无法读取PPP"));
        if (INI_File().getexperimentalMode()) {
            emit signalShowPPPError(0);
        }
        return;
    }

    if (!cglobal::gserialConnecStatus.load() || m_offlineHoldReads) {
        QLOG_WARN() << "slotgetAnemiaValue: 掉线中，不把清零后的0当作PPP初值，样本" << sampleid;
        emit testingLogHint(tr("仪器掉线，暂不读取PPP，待重连后重读"));
        return;
    }

    const int testingChannel = testChnindex + 1;
    int anemiaValue = 0;
    bool missingChannelData = false;
    {
        QMutexLocker locker(&m_realtimeDataMutex);
        const auto it = m_ChnRealtimeData.find(testingChannel);
        if (it == m_ChnRealtimeData.constEnd()) {
            missingChannelData = true;
        } else {
            anemiaValue = it.value();
        }
    }
    if (missingChannelData) {
        QLOG_WARN() << "[" << __FILE__ << "]" << __LINE__
                    << "未找到通道数据,通道号:" << testingChannel;
        anemiaValue = 0;
    }

    const bool lastSecondValid = (testChnindex < MACHINE_SETTING_CHANNEL)
            && m_lastFilterSecondValid[testChnindex];
    if (!lastSecondValid) {
        emit testingLogHint(tr("样本%1 通道%2 本秒无有效正点，PPP重读（不计入失败）")
                            .arg(sampleid).arg(testingChannel));
        QPointer<displayChanneldata> guard(this);
        QTimer::singleShot(kInvalidSecondRetryMs, this, [guard]() {
            if (guard) {
                guard->slotgetAnemiaValue();
            }
        });
        return;
    }

    const bool experimental = INI_File().getexperimentalMode();
    if (!experimental && anemiaValue <= 0) {
        if (m_pppZeroRetryCount < kPrpZeroRetryMax) {
            ++m_pppZeroRetryCount;
            emit testingLogHint(tr("样本%1 PPP物理真0，第%2/%3次重读（通道%4）")
                                .arg(sampleid)
                                .arg(m_pppZeroRetryCount)
                                .arg(kPrpZeroRetryMax)
                                .arg(testingChannel));
            QPointer<displayChanneldata> guard(this);
            QTimer::singleShot(kPrpZeroRetryIntervalMs, this, [guard]() {
                if (guard) {
                    guard->slotgetAnemiaValue();
                }
            });
            return;
        }
        m_pppZeroRetryCount = 0;
        anemiaValue = 0;
        StructInstance::getInstance()->oper_AnemiaValue(WRITE_OPERAT, testChnindex, anemiaValue);
        emit testingLogHint(tr("样本%1 PPP读初值异常（连续%2次物理真0），丢弃通道PPP并放弃该样本")
                            .arg(sampleid).arg(kPrpZeroRetryMax));
        emit signalPppZeroGiveUp(sampleid, testChnindex);
        return;
    }

    m_pppZeroRetryCount = 0;
    StructInstance::getInstance()->oper_AnemiaValue(WRITE_OPERAT, testChnindex, anemiaValue);
    QLOG_INFO() << "[TestChn] ppp_read_ok sample" << sampleid
                << "chn" << (testChnindex + 1) << "value" << anemiaValue;
    emit pppInitRead(testChnindex, anemiaValue);
    if (anemiaValue > 0 && anemiaValue < GlobalData::kPppLowVolumeWarn) {
        emit testingLogHint(tr("样本%1 通道%2 PPP=%3（小于%4），可能加样少或有气泡")
                            .arg(sampleid)
                            .arg(testingChannel)
                            .arg(anemiaValue)
                            .arg(GlobalData::kPppLowVolumeWarn));
    }

    if (auto *sql = FullyAutomatedPlatelets::pinstancesqlData()) {
        sql->saveSamplePppInit(sampleid, testingChannel, anemiaValue);
    }

    if (experimental && !GlobalData::isPppBaselineValid(anemiaValue)) {
        QLOG_ERROR() << "PPP异常值" << anemiaValue << "禁止继续弃杯/开测";
        emit signalShowPPPError(anemiaValue);
        return;
    }

    QLOG_INFO() << "[TestChn] ppp_read_complete -> throwAnemiaCup chn" << (testChnindex + 1);
    emit CompleteGetAnemiaValue(testChnindex);
}

// 读取富血初始值(PRP)：正常模式只要求非0，为0则自动重读几次；实验模式额外校验与PPP关系
void displayChanneldata::slotreadbloodyInitValue(quint8 indexReagent, quint8 indexActive)
{
    quint8 indexChn = 0;
    const int sampleid = StructInstance::getInstance()->ClipEndGetAnemiaValeChn(indexChn, indexActive, "富血init值");
    if (sampleid <= 0) {
        m_prpZeroRetryCount = 0;
        QLOG_WARN() << "slotreadbloodyInitValue: 未定位到抓PRP焦点样本";
        emit testingLogHint(tr("未定位到抓PRP焦点样本，无法读取PRP"));
        emit signalPrpReadFailed();
        return;
    }

    if (!cglobal::gserialConnecStatus.load() || m_offlineHoldReads) {
        QLOG_WARN() << "slotreadbloodyInitValue: 掉线中，不把清零后的0当作PRP初值，样本" << sampleid;
        emit testingLogHint(tr("仪器掉线，暂不读取PRP，待重连后重读"));
        return;
    }

    int bloodyValue = 0;
    bool missingChannelData = false;
    {
        QMutexLocker locker(&m_realtimeDataMutex);
        auto it = m_ChnRealtimeData.find(indexChn + 1);
        if (it == m_ChnRealtimeData.end()) {
            missingChannelData = true;
        } else {
            bloodyValue = it.value();
        }
    }
    const bool lastSecondValid = (indexChn < MACHINE_SETTING_CHANNEL)
            && m_lastFilterSecondValid[indexChn];
    if (!lastSecondValid) {
        emit testingLogHint(tr("样本%1 通道%2 本秒无有效正点，PRP重读（不计入失败）")
                            .arg(sampleid).arg(indexChn + 1));
        QPointer<displayChanneldata> guard(this);
        const quint8 reagent = indexReagent;
        const quint8 focus = indexActive;
        QTimer::singleShot(kInvalidSecondRetryMs, this, [guard, reagent, focus]() {
            if (guard) {
                guard->slotreadbloodyInitValue(reagent, focus);
            }
        });
        return;
    }
    if (missingChannelData || bloodyValue <= 0) {
        if (m_prpZeroRetryCount < kPrpZeroRetryMax) {
            ++m_prpZeroRetryCount;
            emit testingLogHint(tr("PRP物理真0，第%1/%2次重读（通道%3）")
                                .arg(m_prpZeroRetryCount)
                                .arg(kPrpZeroRetryMax)
                                .arg(indexChn + 1));
            QPointer<displayChanneldata> guard(this);
            const quint8 reagent = indexReagent;
            const quint8 focus = indexActive;
            QTimer::singleShot(kPrpZeroRetryIntervalMs, this, [guard, reagent, focus]() {
                if (guard) {
                    guard->slotreadbloodyInitValue(reagent, focus);
                }
            });
            return;
        }
        m_prpZeroRetryCount = 0;
        if (INI_File().getexperimentalMode()) {
            emit testingLogHint(tr("PRP连续%1次为0，已暂停，点「继续」后再读").arg(kPrpZeroRetryMax));
            emit signalPrpReadFailed();
            return;
        }
        emit testingLogHint(tr("样本%1 %2 PRP连续%3次为0，丢弃当前试剂杯")
                            .arg(sampleid)
                            .arg(GlobalData::mapIndexReagentnames(indexReagent))
                            .arg(kPrpZeroRetryMax));
        emit signalPrpZeroSkipReagent(sampleid, indexReagent, indexActive);
        return;
    }

    m_prpZeroRetryCount = 0;

    if (INI_File().getexperimentalMode()) {
        int pppValue = 0;
        StructInstance::getInstance()->oper_AnemiaValue(READ_OPERRAT, indexChn, pppValue);
        if (!GlobalData::isPppBaselineValid(pppValue) || pppValue <= bloodyValue) {
            QLOG_WARN() << "slotreadbloodyInitValue: PRP与PPP关系异常,通道号:" << indexChn + 1
                        << "PPP:" << pppValue << "PRP:" << bloodyValue;
            emit testingLogHint(tr("实验模式：PRP=%1 与 PPP=%2 关系异常")
                                .arg(bloodyValue).arg(pppValue));
            emit signalPrpReadFailed();
            return;
        }
    }

    StructInstance::getInstance()->setRichBloodInitValue(indexReagent, indexChn, bloodyValue);
    if (auto *sql = FullyAutomatedPlatelets::pinstancesqlData()) {
        sql->saveSamplePrpInit(sampleid, indexReagent, indexChn + 1, bloodyValue);
    }
    setCalibrationParameters();
    QLOG_INFO() << "[TestChn] prp_read_ok sample" << sampleid
                << "reag" << indexReagent << "chn" << (indexChn + 1)
                << "value" << bloodyValue << "-> spitReagent";
    emit spitReagentTesting();
}

// 打开通道开始测试：清洗针 + 设置 COL 模式 + 延时 2 秒后打开通道
void displayChanneldata::slotopenTestChnTest(const int sampleId,
                                             const quint8 indexActive,
                                             const quint8 index_reagent)
{
    const auto *psample = StructInstance::getInstance()->rfindIdSample(sampleId);
    if (!psample || psample->indexTestingChn < 0) {
        QLOG_WARN() << "[TestChn] open_test_reject sample" << sampleId << "无有效测试通道";
        return;
    }
    quint8 openChn = static_cast<quint8>(psample->indexTestingChn);
    QLOG_INFO() << "[TestChn] open_test_start sample" << sampleId
                << "chn" << (openChn + 1) << "reag" << index_reagent
                << "focusActive" << indexActive;
    StructInstance::getInstance()->config_testChn_test_reagent(openChn, index_reagent);

    // 清洗针策略：已清洗双针 → 清洗试剂针，否则 → 清洗双针
    bool alreadyCleanBloodpin = StructInstance::getInstance()->judge_alreadyCleanBloodpin(
                sampleId, indexActive);
    if (!alreadyCleanBloodpin) {
        emit addSampmeTestCleanPin(WASH_DOUBLE_NEDDLES, ANEMIA);
        QLOG_DEBUG() << "准备清洗双针";
    } else {
        emit addSampmeTestCleanPin(WASH_REAGENT_NEDDLES, index_reagent);
        QLOG_DEBUG() << "准备清试剂针" << index_reagent;
    }

    if (openChn >= MACHINE_SETTING_CHANNEL) {
        QLOG_WARN() << "接收数据越界!";
        return;
    }

    // 根据试剂类型设置 COL 模式及重置计数器
    if (isCOLReagent(index_reagent)) {
        m_channelInColMode[openChn] = true;
        m_colSkipRawCount[openChn] = 0;
        QLOG_DEBUG() << "通道" << openChn + 1 << "启用 COL 模式，将跳空前n个数据点";
    } else {
        m_channelInColMode[openChn] = false;
    }

    // 延时 2 秒后打开通道（保证物理准备完成）；放弃/停采会增加代次，迟到的定时器直接作废
    const quint32 openGeneration = ++m_openChnGeneration[openChn];
    QPointer<displayChanneldata> guard(this);
    QTimer::singleShot(2000, this, [guard, openChn, openGeneration]() {
        if (guard) {
            guard->tryOpenChannelAcquisition(openChn, openGeneration);
        }
    });
}

// 读取质控值：保存到日志并发送信号
void displayChanneldata::getqualityControlValue(const quint8 Channelnum, int active)
{
    QMutexLocker locker(&m_realtimeDataMutex);
    int value = 0;
    if (m_ChnRealtimeData.contains(Channelnum)) {
        auto mi = m_ChnRealtimeData.find(Channelnum);
        value = mi.value();
        QLOG_DEBUG() << "通道" << Channelnum << "值:" << value;
    }
    emit sendChannelQualitydata(active, value);
}

// =============================================================================
// [组 8] 辅助：实验数据保存（CSV）
// =============================================================================

// 注意：savedrandData / creatvalue 为头文件声明的扩展接口，当前版本尚未启用，
// 如需启用请在此处补充实现。

// 将滤波原始数据保存为 CSV 文件用于实验分析
void displayChanneldata::saveFilterDataToCSV(const QVector<int>& data, int result,
                                             const QString& filename,
                                             const QString& path)
{
    // 每个文件的首次运行状态（线程安全）
    static QMap<QString, bool> firstTimeMap;
    static QMutex firstTimeMutex;

    // 规范化路径，防止路径遍历攻击
    QDir dir(path);
    QString filePath = dir.cleanPath(dir.absoluteFilePath(filename));

    // 安全检查：确保路径在预期范围内
    QString canonicalBase = QFileInfo(path).canonicalFilePath();
    QString canonicalTarget = QFileInfo(filePath).canonicalFilePath();
    if (!canonicalBase.isEmpty() && !canonicalTarget.isEmpty()
            && !canonicalTarget.startsWith(canonicalBase)) {
        QLOG_ERROR() << "saveFilterDataToCSV: 路径访问被拒绝 path=" << filePath;
        return;
    }

    bool isFirstTime = false;
    {
        QMutexLocker locker(&firstTimeMutex);
        isFirstTime = !firstTimeMap.contains(filePath) || firstTimeMap[filePath];
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }
    QTextStream out(&file);

    // 首次写入表头
    if (isFirstTime) {
        out << "Timestamp,";
        for (int i = 0; i < data.size(); ++i) {
            out << "RawData_" << i;
            if (i < data.size() - 1) {
                out << ",";
            }
        }
        out << ",FilteredResult\n";
        QMutexLocker locker(&firstTimeMutex);
        firstTimeMap[filePath] = false;
    }

    // 写入时间戳
    out << QDateTime::currentDateTime().toString("hh:mm:ss.zzz") << ",";

    // 写入所有原始数据
    for (int i = 0; i < data.size(); ++i) {
        out << data[i];
        if (i < data.size() - 1) {
            out << ",";
        }
    }

    // 写入滤波结果
    out << "," << result << "\n";
    file.close();
}



// =============================================================================
// 新增：初始化各试剂的平滑配置
// =============================================================================
void displayChanneldata::initReagentSmoothConfigs()
{
	// AA试剂：聚集反应较温和，步数可以少一些
	ReagentSmoothConfig configAA;
	configAA.minSteps = 15;
	configAA.maxSteps = 45;
	configAA.stepChangeRate = 2.5f;
	configAA.minJump = 3.0f;
	configAA.maxJump = 25.0f;
	configAA.enableDynamic = true;
	m_reagentSmoothConfig[AA_REAGENT] = configAA;

	// ADP试剂：聚集反应较快，需要更多步数平滑
	ReagentSmoothConfig configADP;
	configADP.minSteps = 20;
	configADP.maxSteps = 70;
	configADP.stepChangeRate = 1.8f;
	configADP.minJump = 3.0f;
	configADP.maxJump = 35.0f;
	configADP.enableDynamic = true;
	m_reagentSmoothConfig[ADP_REAGENT] = configADP;

	// EPI试剂：反应速度中等
	ReagentSmoothConfig configEPI;
	configEPI.minSteps = 18;
	configEPI.maxSteps = 55;
	configEPI.stepChangeRate = 2.0f;
	configEPI.minJump = 3.0f;
	configEPI.maxJump = 30.0f;
	configEPI.enableDynamic = true;
	m_reagentSmoothConfig[EPI_REAGENT] = configEPI;

	// COL试剂：反应较慢，步数可以少
	ReagentSmoothConfig configCOL;
	configCOL.minSteps = 10;
	configCOL.maxSteps = 35;
	configCOL.stepChangeRate = 3.0f;
	configCOL.minJump = 3.0f;
	configCOL.maxJump = 25.0f;
	configCOL.enableDynamic = true;
	m_reagentSmoothConfig[COL_REAGENT] = configCOL;

	// RIS试剂：反应速度中等
	ReagentSmoothConfig configRIS;
	configRIS.minSteps = 18;
	configRIS.maxSteps = 55;
	configRIS.stepChangeRate = 2.0f;
	configRIS.minJump = 3.0f;
	configRIS.maxJump = 30.0f;
	configRIS.enableDynamic = true;
	m_reagentSmoothConfig[RIS_REAGENT] = configRIS;

	QLOG_DEBUG() << "【平滑配置】试剂平滑配置已初始化";
}

// =============================================================================
// 新增：动态计算平滑步数
// =============================================================================
int displayChanneldata::calculateDynamicSteps(int chnIndex,
                                              float startPercent,
                                              float targetPercent,
                                              quint8 reagentIndex)
{
    // 检查通道索引有效性
    if (chnIndex < 0 || chnIndex >= MACHINE_SETTING_CHANNEL) {
        QLOG_WARN() << "【动态步数】无效通道索引:" << chnIndex;
        return 30;  // 返回默认值
    }

    SmoothControl& control = m_smoothControl[chnIndex];

    // 计算跳跃幅度
    float jumpPercent = qAbs(targetPercent - startPercent);

    // 获取试剂配置
    ReagentSmoothConfig config;
    auto it = m_reagentSmoothConfig.find(reagentIndex);
    if (it != m_reagentSmoothConfig.end()) {
        config = it->second;
    } else {
        // 默认配置
        config.minSteps = 20;
        config.maxSteps = 50;
        config.stepChangeRate = 2.0f;
        config.minJump = 3.0f;
        config.maxJump = 30.0f;
        config.enableDynamic = true;
        QLOG_WARN() << "【动态步数】未找到试剂" << reagentIndex << "的配置，使用默认值";
    }

    int steps = 0;

    if (!config.enableDynamic) {
        // 不使用动态步数，返回固定值
        steps = 30;
    } else if (jumpPercent <= config.minJump) {
        // 跳跃很小，使用基础步数
        steps = config.minSteps;
    } else if (jumpPercent >= config.maxJump) {
        // 跳跃很大，使用最大步数
        steps = config.maxSteps;
    } else {
        // 线性插值计算步数
        float ratio = (jumpPercent - config.minJump) / (config.maxJump - config.minJump);
        steps = config.minSteps + static_cast<int>(ratio * (config.maxSteps - config.minSteps));

        // 根据每步变化率微调
        int stepsByRate = static_cast<int>(jumpPercent / config.stepChangeRate);
        steps = qMax(steps, stepsByRate);
    }

    // 确保在合理范围内
    steps = qBound(config.minSteps, steps, config.maxSteps);

    // 如果平滑已经在进行中，确保不会减少总步数
    if (control.isActive && control.currentStep > 0) {
        // 如果当前步数已经超过计算值，保留当前进度
        if (control.currentStep > steps) {
            steps = control.currentStep + 5;  // 至少保留5步余量
            steps = qMin(steps, config.maxSteps);
            QLOG_DEBUG() << "【步数保护】当前步数" << control.currentStep
                         << "已超过计算值" << steps << "，调整为" << steps;
        }
    }

    QLOG_DEBUG() << "【动态步数】通道" << chnIndex + 1
                 << " 试剂:" << reagentIndex
                 << " 跳跃:" << jumpPercent << "%"
                 << " 步数:" << steps
                 << " (范围:" << config.minSteps << "-" << config.maxSteps << ")"
                 << " 每步变化:" << jumpPercent / steps << "%";

    return steps;
}
