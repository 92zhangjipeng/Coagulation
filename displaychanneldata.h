#ifndef DISPLAYCHANNELDATA_H
#define DISPLAYCHANNELDATA_H
#pragma once


// =============================================================================
// 第三方 / Qt 标准库头文件
// =============================================================================
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QThread>
#include <QHash>
#include <QString>
#include <atomic>
#include <array>
#include <map>
#include <memory>

// =============================================================================
// 项目内部头文件
// =============================================================================
#include "channelvaluepanel.h"
#include "cglobal.h"
#include "ini_file.h"
#include <QCoreApplication>
#include "qtimerthread.h"
#include "testing.h"
#include "mydatabase.h"
#include "mycustomcurve.h"
#include "dilag/custompppvalue.h"
#include "datalprocessor.h"

class QTextBrowser;
class QTextStream;
class QFile;

/**
 * @brief displayChanneldata 类
 *
 * 负责解析从硬件模块接收到的通道数据，并执行以下处理：
 *   1. 接收并解析串口/协议数据 → 转换为通道原始值
 *   2. 对通道数据进行滤波处理（中位值平均 / 中值 / 级联EWMA）
 *   3. 测试阶段计算血小板聚集率（支持对数/线性两种算法）
 *   4. COL 试剂特殊处理（丢弃前 n 个数据点）
 *   5. 与 UI 层通信：发送信号更新显示、进度条、曲线等
 *
 * 工作线程：对象整体移动到 m_workerThread 子线程运行，
 * 所有共享状态访问均通过互斥锁保护以保证线程安全。
 */
class displayChanneldata : public QObject
{
    Q_OBJECT

// =============================================================================
// 构造 / 析构 & 对外公共接口
// =============================================================================
public:
    explicit displayChanneldata(QObject *parent = nullptr);
    ~displayChanneldata();

    void startModuleData();                              ///< 启动数据采集线程
    void shutdown();                                     ///< 安全停线程并将对象迁回调用线程，再允许 delete
    void UpdateChannelValue(const int Channel, const int ChannelData); ///< 更新单个通道值
    int  GetChannelValue(quint8 Channel);                ///< 获取通道当前滤波值（1 起始通道号）
    quint32 GetPositiveFilterSeq(quint8 Channel);        ///< 正点滤波代次（1 起始通道号）
    bool GetStablePositiveChannelValue(quint8 Channel, quint32 minSeq,
                                       int& value, quint32& currentSeq); ///< 新的正点滤波秒且值>0
    void setCalibrationParameters();                     ///< 从配置文件加载校准参数

// =============================================================================
// 信号
// =============================================================================
signals:
    void SetChannelValueUpdate(const int, const int);                                  ///< 通知 UI 更新通道数值
    void DisplayTestingValue(const QString&, const quint8&, const quint8&,
                             const float&, const int&, const int&, const int&);        ///< 发送测试值（曲线+进度条）
    void addSampmeTestCleanPin(const int, const quint8 index_reagent);                 ///< 通知清洗针
    void CompleteGetAnemiaValue(quint8);                                               ///< 贫血值读取完成
    void spitReagentTesting();                                                          ///< 吐试剂
    void testComplete(QString, const quint8, const quint8);                             ///< 单个测试完成
    void finishtestProgress(const quint8 Channel, const quint8 Reagents);              ///< 试剂完成（清曲线+保存）
    void sampleTestingErr(const QString& sampleid, const quint8& channelIdx);           ///< 样本测试异常
    void sendChannelQualitydata(int, int);                                              ///< 质控数据发送
    void signalShowPPPError(int value);                                                 ///< PPP 值异常（仅实验模式弹窗）
    void pppInitRead(quint8 channelIndex, int pppValue);                                ///< PPP 初值已读（0 基通道号）
    void signalPrpReadFailed();                                                         ///< PRP 初值读取失败（实验模式暂停后重读）
    void signalPppZeroGiveUp(int sampleId, quint8 channel);                             ///< 正常模式 PPP 连续为 0：丢杯并放弃样本
    void signalPrpZeroSkipReagent(int sampleId, quint8 reagent, quint8 focusActive);     ///< 正常模式 PRP 连续为 0：丢当前杯后换试剂/放弃
    void testingLogHint(const QString& text);                                           ///< 写到查看日志（测试流程）
    void channelDataError(const QString& errorMsg);                                     ///< 通道数据错误

// =============================================================================
// 公共槽函数（外部可调用）
// =============================================================================
public slots:
    void getqualityControlValue(const quint8 Channelnum, int active);   ///< 读取质控值
    void startthread();                                                  ///< 子线程入口（初始化通道/映射）
    void Recv_syncModuleChannelData(const QStringList channelData);      ///< 解析模组数据（主入口）
    void slotgetAnemiaValue();                                           ///< 读取通道贫血值(PPP)
    void slotreadbloodyInitValue(quint8 indexReagent, quint8 indexActive); ///< 读取通道富血初始值(PRP)
    void slotopenTestChnTest(const int sampleId, const quint8 indexActive, const quint8 index_reagent); ///< 打开通道开始测试
    void handleCleanModuleBuffData();                                    ///< 断线时清空模组缓冲区
    void handleInstrumentReconnected();                                  ///< 重连后恢复读数（不把清零后的 0 当初值）
    void slotStopChannelAcquisition(quint8 channelIdx);                  ///< 放弃/结束样本时停止该通道采数
    void slotAbortAllOpenChannelAcquisition();                           ///< 掉线超时：停所有开测通道采数

// =============================================================================
// 内部工具：数据接收 & 队列
// =============================================================================
private:
    void ChannelDataLinedup(int index, int ModuleData);                 ///< 通道数据入队（滑动窗口）
    void ReplaceModuleValue(quint8 Channel, int mean);                   ///< 替换更新通道实时值（滤波后写回）
    void filteringStyle();                                               ///< 每秒滤波：遍历通道调用滤波算法
    void tryOpenChannelAcquisition(quint8 openChn, quint32 openGeneration); ///< 延时开测；掉线则等到重连

    // =========================================================================
    // 内部工具：滤波算法
    // =========================================================================
    int Medianaveragefiltering(QVector<int> recvData, const int channel);  ///< 中位值平均滤波（去极值后求平均）
    int optimizedMedianFiltering(QVector<int>& data);                       ///< 中值滤波（O(n) 快速选择）
    int MedianEWMADynamicAdaptiveFilter(const QVector<int>& intVec);        ///< 级联中值+EWMA 动态自适应滤波

    // =========================================================================
    // 内部工具：测试流程
    // =========================================================================
    void outPutModuleTestData();                                          ///< 轮询通道：对开放通道调用 processChannelData
    void processChannelData(int channelIdx);                              ///< 单通道测试数据处理（读取参数+调用公式）
    void processChannelData(int moduleIndex,
                            const std::array<int, FOUR_CHANNELS_PERMODULE>& data,
                            const std::array<bool, FOUR_CHANNELS_PERMODULE>& parsed,
                            const QStringList& receiveOriginalData);      ///< 模组数据分发到通道（解析失败/脏0不入桶）
    void calculationFormula(const QString& sampleNum,
                            quint8 reagentIndex,
                            int currentRichValue,
                            int baselinePoor,
                            int baselineRich,
                            int channelIdx);                              ///< 测试流程主控制器（COL跳过/异常检测/数据点判断）
    void handleTestCompletion(const QString& sampleNum,
                              quint8 reagentIndex,
                              int channelIdx);                            ///< 测试完成：清状态+发信号+保存
    void processTestData(const QString& sampleNum,
                         quint8 reagentIndex,
                         int currentRichValue,
                         int baselinePoor,
                         int baselineRich,
                         int channelIdx,
                         int totalDataPoints);                            ///< 测试中：计算聚集率+更新UI
    bool sampleAbnormality(const int& initprp,
                           const int anaemiaValue,
                           const int& curprp,
                           const QString& sampleid,
                           const quint8& channelIdx,
                           const quint8 indexReagent);                    ///< 样本异常检测（PRP 0值/贫血值异常）

    // =========================================================================
    // 内部工具：聚集率计算
    // =========================================================================
    int getBaselinePoorValue(int channelIdx, int currentBaseline);        ///< 获取 PPP 基线值（支持通道覆盖）
    double calculateTestAggregationRate(double prpn, double ppp, double prp0); ///< 对数算法：分段校准
    float calculateAggregationRate(const bool isLogMode, float PRPn, float PRP0, float PPP); ///< 对外接口：按算法模式分派
    float getRandomFactor(float min, float max);                           ///< 安全随机数：参数修正时微调 PRPn
    float applyOffsetToAggregationRate(float originalRate, quint8 reagentIndex); ///< 按试剂类型应用偏移百分比

    // =========================================================================
    // 内部工具：辅助（实验数据保存 / 初始化）
    // =========================================================================
    void initializeOneTimeChannelUpdate(bool& allChannelsReceived);       ///< 首次收到所有通道数据：启动定时器线程
    void saveFilterDataToCSV(const QVector<int>& data, int result,
                             const QString& filename = "filter_data.csv",
                             const QString& path = QCoreApplication::applicationDirPath());

    // =========================================================================
    // 配置参数缓存（避免频繁读取 INI 文件）
    // =========================================================================
    mutable bool m_absorbanceAlgorithmKnown = false;
    mutable bool m_absorbanceAlgorithmValue = false;
    bool getAbsorbanceAlgorithm() const {
        if (!m_absorbanceAlgorithmKnown) {
            m_absorbanceAlgorithmValue = INI_File().rConfigPara("AbsorbanceAlgorithm").toBool();
            m_absorbanceAlgorithmKnown = true;
        }
        return m_absorbanceAlgorithmValue;
    }
    void clearAlgorithmCache() {
        m_absorbanceAlgorithmKnown = false;
        QLOG_DEBUG() << "对数模式缓存清除";
    }

    // =========================================================================
    // 辅助：COL 试剂判断（内联避免函数调用开销）
    // =========================================================================
    inline bool isCOLReagent(quint8 reagentIndex) const {
        return reagentIndex == COL_REAGENT;
    }

// =============================================================================
// 成员变量：按功能分组
// =============================================================================
private:
    // -------------------------------------------------------------------------
    // [1] 通道数据：实时值 + 历史队列
    // -------------------------------------------------------------------------
    QHash<quint8, int> m_ChnRealtimeData;    ///< 1 起始通道号 → 当前滤波后数值
    QMutex m_realtimeDataMutex;               ///< 保护 m_ChnRealtimeData 的跨线程访问
    QQueue<int> m_queueChannel[MACHINE_SETTING_CHANNEL];  ///< 0 起始：每通道一秒累积桶（仅正点）
    bool m_channelHasFrame[MACHINE_SETTING_CHANNEL] = {}; ///< 0 起始：该通道是否已收到过解析成功的帧
    static constexpr int dataLengthGroup = 100;  ///< 单秒最大累积点数（保护内存，防止硬件高频数据溢出）
    quint8 m_configChannelCount = 0;          ///< 当前设备配置的有效通道数
    int m_pppZeroRetryCount = 0;              ///< 物理真 0 时的 PPP 重读次数
    int m_prpZeroRetryCount = 0;              ///< 物理真 0 时的 PRP 重读次数
    bool m_offlineHoldReads = false;          ///< 掉线清值后禁止把 0 当作 PPP/PRP 初值
    bool m_lastFilterSecondValid[MACHINE_SETTING_CHANNEL] = {}; ///< 上一滤波秒是否有正点/已确认物理0
    int m_emptyFilterSeconds[MACHINE_SETTING_CHANNEL] = {};     ///< 连续无正点的滤波秒数
    quint32 m_positiveFilterSeq[MACHINE_SETTING_CHANNEL] = {};  ///< 正点滤波秒代次（换液后用来丢掉旧值）
    static constexpr int kPrpZeroRetryMax = 8;
    static constexpr int kPrpZeroRetryIntervalMs = 250;
    static constexpr int kInvalidSecondRetryMs = 1000;          ///< 本秒无正点：等下一滤波秒再读
    static constexpr int kPhysicalZeroEmptySeconds = 2;         ///< 连续两秒无正点 → 记为物理挡光 0

    // -------------------------------------------------------------------------
    // [2] 定时器 / 线程
    // -------------------------------------------------------------------------
    std::unique_ptr<QTimerThread> m_timerThread;  ///< 每秒触发一次滤波的定时器线程
    QThread m_workerThread;                        ///< 数据解析+处理的工作线程
    std::atomic<bool> m_experimenttestData{false};///< 是否处于实验/数据采集模式

    // -------------------------------------------------------------------------
    // [3] 样本状态（测试阶段）
    // -------------------------------------------------------------------------
    QSet<QString> sentSamples;               ///< 已记录异常信息的样本ID（防重复告警）
    QMutex m_sentSamplesMutex;               ///< 保护 sentSamples 跨线程访问
    static constexpr size_t kMaxChannels = MACHINE_SETTING_CHANNEL;
    bool mOpenChnTest[MACHINE_SETTING_CHANNEL] = {};  ///< 通道是否处于测试中
    quint32 m_openChnGeneration[MACHINE_SETTING_CHANNEL] = {};   ///< 打开采数代次，用于作废迟到的 2s 定时器

    // -------------------------------------------------------------------------
    // [4] COL 试剂专用：跳过前 n 个数据点
    // -------------------------------------------------------------------------
    quint8 m_testColJumpEmptyData = 0;        ///< COL 试剂需丢弃的数据点数量（配置项）
    int m_colSkipRawCount[MACHINE_SETTING_CHANNEL] = {};    ///< 每个通道已丢弃点数计数器
    bool m_channelInColMode[MACHINE_SETTING_CHANNEL] = {};   ///< 每个通道是否当前为 COL 测试

    // -------------------------------------------------------------------------
    // [5] 校准参数（从 setCalibrationParameters() 加载）
    // -------------------------------------------------------------------------
    double mk1 = 2.3;                         ///< k 区间 1 上界
    double mk2min = 2.3;                      ///< k 区间 2 下界
    double mk2max = 4.0;                      ///< k 区间 2 上界
    double mk3min = 4.0;                      ///< k 区间 3 下界
    double mk3max = 6.5;                      ///< k 区间 3 上界
    double mk4 = 6.5;                         ///< k 区间 4 上界
    double mcalibrationFactor1 = 2.3;         ///< 区间 1 校准系数
    double mcalibrationFactor3 = 0.6;         ///< 区间 3 校准系数
    double mcalibrationFactor4 = 0.5;         ///< 区间 4 校准系数
    double m_aggregationRateOffsetPercent[5]; ///< 5 种试剂（AA/ADP/EPI/COL/RIS）的整体偏移

    // -------------------------------------------------------------------------
    // [6] 其他
    // -------------------------------------------------------------------------
    bool m_allModulesAccepted = false;        ///< 模组是否完成首次数据采集
    QMutex queueMutex;                        ///< 保护 m_queueChannel 在断线清理时的访问
    float m_prevPRPn = 0.0f;                  ///< 上一次 PRP 数值（供参考）
    DataProcessor processor;                   ///< 测试参数处理器（基线PRP值等）


    // 缓存无效判定参数 - 按试剂下标存储
    struct InvalidConfig {
        quint8 triggerInvalid = 0;
        quint8 triggerOffset = 0;
        quint8 invalidMin = 0;
        quint8 invalidMax = 0;
        bool isValid = false;
    };
    InvalidConfig m_invalidConfigCache[5];

    // ============ 新增：平滑过渡控制 ============
    struct SmoothControl {
        bool isActive = false;
        float startValue = 0.0f;
        float targetValue = 0.0f;
        float currentValue = 0.0f;
        int totalSteps = 30;           // 改为30步，更适合每秒一个点
        int currentStep = 0;
        bool isFirstApply = true;      // 标记是否是第一次应用
        bool isCompleting = false;     // 标记是否在完成过渡中

        // ============ 新增：动态步数相关 ============
        int baseSteps = 20;          // 基础步数
        int maxSteps = 60;           // 最大步数
        float minJump = 3.0f;        // 最小跳跃阈值（%），低于此值使用基础步数
        float maxJump = 30.0f;       // 最大跳跃阈值（%），高于此值使用最大步数
    };

    struct ReagentSmoothConfig {
        int minSteps = 15;
        int maxSteps = 60;
        float stepChangeRate = 2.0f;  // 每步最大变化率（%）
        float minJump = 3.0f;
        float maxJump = 30.0f;
        bool enableDynamic = true;
    };

    // ============ 新增：试剂平滑配置映射 ============
    std::map<quint8, ReagentSmoothConfig> m_reagentSmoothConfig;

    // ============ 新增：动态步数计算方法 ============
    int calculateDynamicSteps(int chnIndex,
                                  float startPercent,
                                  float targetPercent,
                                  quint8 reagentIndex);

    // ============ 新增：初始化试剂平滑配置 ============
    void initReagentSmoothConfigs();

    // 每个通道独立的平滑控制（最多12通道）
    SmoothControl m_smoothControl[MACHINE_SETTING_CHANNEL];

    // ============ 新增：平滑过渡方法 ============
    float applySmoothTransition(int channelIdx,float newValue);
    void resetSmoothControl(int channelIdx);
    void resetAllSmoothControls();

    // ============ 无效偏移处理辅助函数 ============
    float handleNoOffset(int channelIdx, int chnIndex, int totalDataPoints, float finalResult);
    float handleOffsetApplied(const QString& sampleNum,
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
                                 quint8 triggerInvalid);

    void logSmoothDebugInfo(int channelIdx,
                               const QString& sampleNum,
                               int totalDataPoints,
                               int R,
                               quint8 triggerInvalid,
                               quint8 triggerOffset,
                               float percentValue,
                               float targetValue,
                               float currentResult,
                               const SmoothControl& control);



    // 缓存每个样本的每个试剂的随机数 R
    // key: sampleNum + "_" + reagentIndex, value: 随机数 R
    QHash<QString, int> m_sampleReagentRandomCache;
    void updateInvalidConfigCache(quint8 reagentIndex);
    int getOrCreateRandomR(const QString& sampleNum, quint8 reagentIndex);
    QString makeCacheKey(const QString& sampleNum, quint8 reagentIndex) const;
    void onReagentTestComplete(const QString& sampleNum, quint8 reagentIndex);





};

#endif // DISPLAYCHANNELDATA_H
