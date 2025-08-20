#include "channelfactory.h"
#include "displaychanneldata.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QTextCodec>
#include "globaldata.h"
#include "loadequipmentpos.h"
#include "mycustomcurve.h"
#include "quiutils.h"
#include <random>
#include <iostream>
#include <creatcurve_data/customcurveadp.h>
#include <operclass/fullyautomatedplatelets.h>


QMutex m_mutex;

displayChanneldata::displayChanneldata(QObject *parent) : QObject(parent)
    ,m_AllModulAccept(false)
    ,m_configChannelCount(0)
{
    moveToThread(&m_thread);
    sentSamples.clear();

    connect(&m_thread,&QThread::started,
            this,&displayChanneldata::_startthread);
}

displayChanneldata::~displayChanneldata()
{
    if(m_thread.isRunning()){
        m_thread.quit();
        m_thread.wait();
    }
    if(m_timerThread){
        m_timerThread->stopThread();
    }
    QLOG_TRACE()<<"析构解析模组数据线程Id:"<<QThread::currentThreadId();
}

void displayChanneldata::_start()
{
    if(!m_thread.isRunning()){
        m_thread.start();
    }
}

void displayChanneldata::_startthread()
{
    QLOG_TRACE()<<"模组数据采集线程Id:"<<QThread::currentThreadId();

    // 设备类型映射表（C++11特性，需确认编译器支持）
    const QHash<quint8, quint8> kDeviceChannelMap = {
        {KS600,  4},
        {KS800,  8},
        {KS1200, 12}
    };
    // 获取设备类型
    quint8 equipmentType = KS1200; // 默认值
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);

    // 确定通道数量
    const quint8 channelCount = kDeviceChannelMap.value(equipmentType, 12);
    QLOG_DEBUG() << "当前设备类型：" << equipmentType
                 << "，通道数量：" << channelCount<<endl;

    // 初始化测试通道标志（安全方式）
    constexpr size_t kMaxChannels = MACHINE_SETTING_CHANNEL;
    std::fill_n(mOpneChnTest, kMaxChannels, false);
    //memset(mOpneChnTest, false, sizeof(mOpneChnTest));

    // 清空数据容器
    m_channelRawData.clear();       // 建议重命名为有意义的变量名
    m_processedChannelData.clear(); // 同上

    // 预分配空间提升性能
    m_ChnRealtimeData.reserve(kMaxChannels);

    // 初始化实时数据容器
    for(quint8 chn = 0; chn < kMaxChannels; ++chn) {
        m_ChnRealtimeData.insert(chn + 1, 0); // 保持原有逻辑
    }

}




// 新增数据处理函数
void displayChanneldata::processChannelData(int moduleIndex,
                                            const std::array<int, FOUR_CHANNELS_PERMODULE>& data,
                                            const  QStringList &receiveOriginalData)
{
    const int baseChannel = (moduleIndex - MODULE_1) * FOUR_CHANNELS_PERMODULE;

    for (int i = 0; i < FOUR_CHANNELS_PERMODULE; ++i) {
        const int channelNumber = baseChannel + i; // 通道号从1开始
        const int value = data[i];
        if (value <= 0) {
            QLOG_ERROR() << "模块" << moduleIndex << "通道" << channelNumber + 1
                         << "无效数据：" << value <<"收到原数据:"<<receiveOriginalData;
        }
        UpdateChannelValue(channelNumber, value);
    }

    // 初始化逻辑优化
    if (!m_AllModulAccept) {
        initializeOneTimeChannelUpdate(m_AllModulAccept);
    }
}

/*解析到模组数据*/
void displayChanneldata::Recv_syncModuleChannelData(QStringList channelData)
{
    constexpr int HEADER_SIZE = 8;
    // 使用栈数组避免内存管理
    std::array<int, FOUR_CHANNELS_PERMODULE> moduleData{};

    // 模块索引处理（带错误处理）
    bool conversionOk = false;
    const int moduleIndex = channelData[0].toInt(&conversionOk, HEX_SWITCH);
    if (!conversionOk || moduleIndex < MODULE_1 || moduleIndex > MODULE_3) {
            QLOG_ERROR() << "无效模块地址：" << channelData[0]
                        << "，原始数据：" << channelData;
            return;
    }

    const int baseIndex = HEADER_SIZE - 1; // 对应原始代码的7
    for (int channel = 0; channel < FOUR_CHANNELS_PERMODULE; ++channel){
            const int highByteIndex = baseIndex + 2 * channel;
            const int lowByteIndex =  highByteIndex + 1;

            // 合并字节处理
            const QString& highByte = channelData[highByteIndex];
            const QString& lowByte = channelData[lowByteIndex];

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
            moduleData[channel] = value;
    }

    // 数据后处理
    processChannelData(moduleIndex, moduleData,channelData);
}


void displayChanneldata::initializeOneTimeChannelUpdate(bool& allChannelsReceived)
{
    // 使用查找表替代switch-case
    const QHash<quint8, quint8> kChannelConfig {
        {KS600,  4},
        {KS800,  8},
        {KS1200, 12}
    };

    // 获取设备类型并验证
    quint8 equipmentType = KS1200; // 默认值
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);


    // 安全获取通道数量
    const quint8 channelCount = kChannelConfig.value(equipmentType, 12);
    m_configChannelCount = channelCount;
    QLOG_DEBUG() << "当前设备类型:" << equipmentType<< "，配置通道数:" << channelCount;

    // 验证通道队列容量
	// 使用安全的方式获取数组元素数量
    constexpr int MAX_CHANNELS = sizeof(m_queueChannel) / sizeof(m_queueChannel[0]);
    if (MAX_CHANNELS < channelCount) {
        QLOG_ERROR() << "通道队列容量不足，需要:" << channelCount << "，实际:" << MAX_CHANNELS<<endl;
        allChannelsReceived = false;
        return;
    }

    // 检查所有通道数据
    bool allDataReady = true;
    for (int i = 0; i < channelCount; ++i) {
        if (m_queueChannel[i].isEmpty()) {
            allDataReady = false;
            break;
        }
    }


    // 定时器线程安全初始化
    static QBasicMutex timerMutex;
    QMutexLocker locker(&timerMutex);

    if (!m_timerThread) {
        try {
            m_timerThread.reset(new QTimerThread(1));

            // 获取原始指针用于Qt信号连接
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
	if (!allDataReady) {
		allChannelsReceived = false;
	}else
		allChannelsReceived = true;
}

void displayChanneldata::UpdateChannelValue(const int Channel ,const int ChannelData)
{
    emit SetChannelValueUpdate(Channel, ChannelData); //发送信号到设置界面显示 通道数值
    ChannelDataLinedup(Channel,ChannelData);
    return;
}



/*通道数据收集*/
void displayChanneldata::ChannelDataLinedup(int index ,int ModuleData)
{
    // 获取目标队列引用（避免重复查找）
    auto& targetQueue = m_queueChannel[index];
    if(targetQueue.size() < dataLengthGroup){
         targetQueue.enqueue(ModuleData); //直接入队
    }
    else{
        targetQueue.dequeue();   //先出队
        targetQueue.enqueue(ModuleData); //在入队
    }
    return;
}



void displayChanneldata::filteringStyle()
{
    /*输出模组测试结果*/
    if(!cglobal::gserialConnecStatus)
        return;

    // 预分配内存，减少循环中可能的动态分配开销
    QVector<int> filteredValues;
    filteredValues.reserve(m_configChannelCount);
	const int filterMode = INI_File().getFilteringMode();

    //避免重复获取队列大小，提前缓存队列引用
    for (int i = 0; i < m_configChannelCount; ++i) {
        auto& queue = m_queueChannel[i]; // 引用减少访问开销

        //直接使用QVector构造避免出队/入队操作
        QVector<int> RecvData;
        RecvData.reserve(queue.size());
        for (auto it = queue.constBegin(); it != queue.constEnd(); ++it) {
            RecvData.append(*it);
        }
 
        if (RecvData.isEmpty()) {
            QLOG_ERROR() << "通道" << i << "数据为空，跳过处理";
            filteredValues.append(0); // 默认值
            continue;
        }


        // 优化3：使用策略模式抽象滤波逻辑
        int result = 0;
        switch (filterMode) {
            case FILTER_NO:             result =  RecvData.last();                   break;
            case FILTER_AVERAGE_VALUE:  result =  Medianaveragefiltering(RecvData);  break;
            case FILTER_MIDVALUE:       result =  optimizedMedianFiltering(RecvData);break;
            case MEDIAN_EWMA_DYNAMIC:   result =  MedianEWMADynamicAdaptiveFilter(RecvData); break;
            default:
                QLOG_WARN() << "未知滤波模式，使用默认无滤波";
                result = RecvData.last();
            break;
        }
        ReplaceModuleValue(i, result);
    }
    OutputModulTestResult();
}



/*输出测试的结果*/
void displayChanneldata::OutputModulTestResult()
{
    constexpr auto kMaxChannels = static_cast<int>(MACHINE_SETTING_CHANNEL);

    for(int channelIdx = 0; channelIdx < kMaxChannels; ++channelIdx) {

        if(!mOpneChnTest[channelIdx]) continue;

        const quint8 channelNumber = channelIdx + 1;
        if(channelNumber > kMaxChannels) {
            QLOG_WARN() << "Invalid channel number:" << channelNumber;
            continue;
        }

        // 单次查找操作替代contains+find
        const auto dataIt = m_ChnRealtimeData.constFind(channelNumber);
        if(dataIt == m_ChnRealtimeData.constEnd()) {
            QLOG_WARN() << "No data for channel" << channelNumber;
            continue;
        }

        // 明确初始化参数语义
        struct TestParams {
            int anaemiaValue = 0;
            int bloodValue = 0;
            quint8 reagentIndex = 0;
            int testSample = 0;
        } params;

        // 提取数据操作
        const QString sampleName = StructInstance::getInstance()->testingOutInitPPPandPRP(
                                                                                        channelIdx,
                                                                                        params.anaemiaValue,
                                                                                        params.bloodValue,
                                                                                        params.reagentIndex,
                                                                                        params.testSample
                                                                                    );
        // 过滤无效测试类型
        if(params.reagentIndex == ANEMIA) continue;

        //判断样本prp prp 是否异常 0
        sampleAbnormality(params.bloodValue,
                          params.anaemiaValue,
                          dataIt.value(),
                          sampleName,channelIdx);

        // 执行核心计算
        Calculation_formula(sampleName,
                            params.reagentIndex,
                            dataIt.value(),
                            params.anaemiaValue,
                            params.bloodValue,
                            channelNumber);

    }
    return;
}


/*公式部分*/
void displayChanneldata::Calculation_formula(const QString& sampleNum,
                                                quint8 reagentIndex,
                                                int currentRichValue,
                                                int baselinePoor, int baselineRich,
                                                int channelIdx){

    int totalDataPoints = 0;

    // 获取测试数据总数（改为引用传参）
    StructInstance::getInstance()->fetchTestDataTotal(sampleNum, reagentIndex, totalDataPoints);

    // 3. 根据算法模式计算
    const bool isLogMode = INI_File().rConfigPara("AbsorbanceAlgorithm").toBool();
    const float resultValue = calculateAggregationRate(isLogMode,static_cast<float>(currentRichValue),
                                                       static_cast<float>(baselineRich),
                                                       static_cast<float>(baselinePoor));
    //获取测试数据的个数
    if(totalDataPoints < NUMBEROFTESTDATA)
    {
        StructInstance::getInstance()->updte_saveChnTestData(channelIdx,reagentIndex,
                                                             currentRichValue, resultValue);
        //提示测试通道在测样本信息
        FullyAutomatedPlatelets::pinstanceTesting()->showTestChannelInfo(channelIdx,sampleNum,reagentIndex);

        emit DisplayTestingValue(sampleNum,reagentIndex,channelIdx,resultValue,
                                    currentRichValue,
                                    baselineRich,
                                    baselinePoor);
    }
    else if(totalDataPoints == NUMBEROFTESTDATA)
    {
        //试剂测试标志 true
        bool ishadTestErr = sentSamples.contains(sampleNum);
        StructInstance::getInstance()->setupOneReagentsIsComplete(sampleNum,reagentIndex,ishadTestErr);
        (ishadTestErr)? sentSamples.remove(sampleNum): sentSamples.size();

        mOpneChnTest[channelIdx - 1] = false; //通道测试数据==300 设标志为true
        //处理测试数据
        emit finishtestProgress(channelIdx - 1,reagentIndex);
        //有通道完成测试
        emit testComplete(sampleNum,channelIdx - 1,reagentIndex);
        //耗材消耗统计
        FullyAutomatedPlatelets::pinstancesqlData()->testendAddStasReagent(sampleNum,reagentIndex);
    }
}

/**
 * 计算血小板聚集率
 * @param isLogMode 是否使用对数计算模式
 * @param PRPn 当前血小板计数
 * @param PRP0 初始血小板计数
 * @param PPP 血小板贫乏血浆计数
 * @return 计算得到的聚集率，无效输入返回NaN(对数模式)或0(线性模式)
 */
float displayChanneldata::calculateAggregationRate(const bool isLogMode, float PRPn, float PRP0, float PPP)
{
    // 1. 基础参数有效性检查
    if (PRP0 <= 0.0f || PPP <= 0.0f) {
        QLOG_WARN() << "Invalid parameters - PRP0 or PPP <= 0:"
                   << "PRPn=" << PRPn
                   << "PRP0=" << PRP0
                   << "PPP=" << PPP;
        return isLogMode ? NAN : 0.0f;
    }

    // 2. 根据计算模式处理
    if (isLogMode) {
        // 对数模式: log10(PRPn/PRP0) / log10(PPP/PRP0)
        const float ratio = PRPn / PRP0;
        const float denominatorRatio = PPP / PRP0;

        if (ratio <= 0.0f || denominatorRatio <= 0.0f) {
            QLOG_WARN() << "Log mode invalid - ratio <= 0:"
                       << "ratio=" << ratio
                       << "denominatorRatio=" << denominatorRatio;
            return NAN;
        }

        const float result = log10f(ratio) / log10f(denominatorRatio);
        //QLOG_DEBUG() << "Log mode result:" << result;
        return result;
    }
    else {
        // 线性模式: (PRPn - PRP0) / (PPP - PRP0)
        const float numerator = PRPn - PRP0;
        const float denominator = PPP - PRP0;

        if (denominator == 0.0f) {
            QLOG_WARN() << "Linear mode invalid - denominator == 0";
            return 0.0f;
        }

        const float result = numerator / denominator;
        //QLOG_DEBUG() << "Linear mode result:" << result;
        return result;
    }
}



/*替换更新接收模组数据*/
void displayChanneldata::ReplaceModuleValue(quint8 Channel, int mean)
{
    if(m_ChnRealtimeData.contains(Channel+1))
    {
        auto iter = m_ChnRealtimeData.find(Channel + 1);
        if(iter !=  m_ChnRealtimeData.end()){
            iter.value() = mean;
        }
    }else{
        m_ChnRealtimeData.insert(Channel + 1, mean);
    }
    return;
}




//获得通道某一时刻 值 初始贫富 血
int displayChanneldata::GetChannelValue(quint8 Channel)
{
    int value = 0;
    if(m_ChnRealtimeData.contains(Channel))
    {
        auto mi = m_ChnRealtimeData.find(Channel);
        value = mi.value();
    }
    return value;
}

void displayChanneldata::getqualityControlValue(const quint8 Channelnum,int active){
    int value = 0;
    if(m_ChnRealtimeData.contains(Channelnum))
    {
        auto mi = m_ChnRealtimeData.find(Channelnum);
        value = mi.value();
        QLOG_DEBUG() << "通道"<<Channelnum<<"值:"<< value;
    }
    emit sendChannelQualitydata(active,value);
}





void displayChanneldata::slotgetAnemiaValue()
{
    int sampleid = 0;
    quint8 testChnindex = 0;
    sampleid = StructInstance::getInstance()->ClipEndGetAnemiaValeChn(testChnindex,
                                                       FOCUS_CLIP_ANEMIA_TO_CHN,
                                                      "准备读取贫血值");
    const int testingChannel = testChnindex + 1;
    const auto it = m_ChnRealtimeData.find(testingChannel);
    if(it == m_ChnRealtimeData.constEnd()) {
        QLOG_WARN() << "[" << __FILE__ << "]" << __LINE__
                    << "未找到通道数据,通道号:" << testingChannel;
        return;
    }

    int aneminaValue = it.value();
    StructInstance::getInstance()->oper_AnemiaValue(WRITE_OPERAT,testChnindex,aneminaValue);
    QLOG_DEBUG()<<"[样本"<<sampleid<<"]"<<"通道"<<testChnindex<<"PPP"<<aneminaValue<<"读取成功"<<endl;

    //获取贫血成功
    emit CompleteGetAnemiaValue(testChnindex);
}

void displayChanneldata::slotreadbloodyInitValue(quint8 indexReagent,quint8 indexActive)
{
    quint8 indexChn  = 0;
    StructInstance::getInstance()->ClipEndGetAnemiaValeChn(indexChn,indexActive,"富血init值");
    auto it = m_ChnRealtimeData.find(indexChn + 1);
    int bloodyValue = it.value();
    StructInstance::getInstance()->setRichBloodInitValue(indexReagent,indexChn,bloodyValue); //设置富血初值

    emit spitReagentTesting(); //吐试剂
}


void displayChanneldata::slotopenTestChnTest(const int sampleId,const quint8 indexActive,const quint8 index_reagent)
{
    quint8 openChn  = 0;
    StructInstance::getInstance()->sycn_sampleneed_data(sampleId,openChn);
    StructInstance::getInstance()->config_testChn_test_reagent(openChn,index_reagent); //设置测试试剂
    mOpneChnTest[openChn] = true;
    QLOG_DEBUG()<<"通道"<<openChn+1<<"打开阀门接收数据";

    //如果清洗血样针的命令不为空==清洗双针
    bool alreadyCleanBloodpin = StructInstance::getInstance()->judge_alreadyCleanBloodpin(sampleId,indexActive);
    if(!alreadyCleanBloodpin)
    {
        emit  addSampmeTestCleanPin(WASH_DOUBLE_NEDDLES,ANEMIA);   //清洗双针
        QLOG_DEBUG()<<"准备清洗双针";
    }
    else
    {
        emit  addSampmeTestCleanPin(WASH_REAGENT_NEDDLES,index_reagent);  //清洗试剂针
        QLOG_DEBUG()<<"准备清试剂针"<<index_reagent;
    }
    return;
}


//中位值平均滤波法
int displayChanneldata::Medianaveragefiltering(QVector<int> RecvData)
{
    if (RecvData.size() <= 2) {
        return std::accumulate(RecvData.begin(), RecvData.end(), 0) / std::max(1, RecvData.size());
    }

    int maxValue = *std::max_element(RecvData.begin(), RecvData.end());
    int minValue = *std::min_element(RecvData.begin(), RecvData.end());
    RecvData.removeOne(maxValue);
    RecvData.removeOne(minValue);
    const double sum = std::accumulate(RecvData.begin(), RecvData.end(), 0.0);
    return static_cast<int>(sum / RecvData.size());
}


//中值滤波
int displayChanneldata::optimizedMedianFiltering(QVector<int>& data)
{
    if (data.isEmpty()) return 0;
    const int n = data.size();
    const auto mid = data.begin() + n / 2;
    std::nth_element(data.begin(), mid, data.end());
    if (n % 2 == 1) {
        return *mid;
    } else {
        // 偶数长度时取中间两个的平均值
        const auto left_mid = std::max_element(data.begin(), mid);
        return (*left_mid + *mid) / 2;
    }
}



bool displayChanneldata::sampleAbnormality(const int& initprp,
                                            const int anaemiaValue,
                                            const int&curprp,
                                            const QString& sampleid,
                                            const quint8& channelIdx){
    if (initprp == 0 || curprp == 0 && anaemiaValue == 0) {
       // 检查该样本的信号是否已发送过
       if (!sentSamples.contains(sampleid)) {
           emit sampleTestingErr(sampleid,channelIdx); // 发送信号
           sentSamples.insert(sampleid);    // 标记该样本的信号已发送
           return true;
       }
    }
    return false;
}


void displayChanneldata::handleCleanModuleBuffData(){
    QMutexLocker locker(&queueMutex); // 加锁（离开作用域自动解锁）

    //m_ChnRealtimeData.clear();
    for (int i = 0; i < MACHINE_SETTING_CHANNEL; ++i) {
        m_queueChannel[i].clear();
        emit SetChannelValueUpdate(i, 0);
    }

}



//级联中值+EWMA复合滤波

// 中值滤波（抗脉冲噪声）
QVector<double> medianFilter(const QVector<double>& data, int windowSize) {
    QVector<double> result;
    QVector<double> window(windowSize);

    for (int i = 0; i < data.size(); ++i) {
        // 获取滑动窗口数据（边界处理）
        int start = qMax(0, i - windowSize/2);
        int end = qMin(data.size()-1, i + windowSize/2);
        window.clear();
        for (int j = start; j <= end; ++j) {
            window.append(data[j]);
        }

        // 排序并取中值
        std::sort(window.begin(), window.end());
        result.append(window[window.size()/2]);
    }
    return result;
}

// 指数加权移动平均（平滑随机波动）
QVector<double> ewmaFilter(const QVector<double>& data, double alpha) {
    QVector<double> result;
    if (data.isEmpty()) return result;

    result.append(data.first()); // 初始值
    for (int i = 1; i < data.size(); ++i) {
        result.append(alpha * data[i] + (1-alpha) * result.last());
    }
    return result;
}

// 级联复合滤波器
QVector<double> cascadeMedianEWMA(const QVector<double>& data) {
    // 第一级：3点中值滤波（建议奇数窗口）
    QVector<double> medianFiltered = medianFilter(data, 3);

    // 第二级：EWMA平滑（α=0.3适合大多数生物信号）
    return ewmaFilter(medianFiltered, 0.3);
}


//动态权重终点优化（高级）
//适用场景：对精度要求极高的场景（如医疗诊断）
//原理：根据信噪比动态调整终点权重 结合趋势预测补偿滞后
double getFinalOptimalValue(const QVector<double>& rawData) {
    QVector<double> filtered = cascadeMedianEWMA(rawData);

    // 1. 计算末端信噪比(SNR)
    double noisePower = 0.0;
    for(int i = 1; i < filtered.size(); ++i)
        noisePower += qPow(filtered[i]-filtered[i-1], 2);
    double snr = qAbs(filtered.last()) / (qSqrt(noisePower)+1e-6);

    // 2. 动态权重融合
    double alpha = qBound(0.6, 1.0 - 0.4/(1+snr), 0.9); // SNR越高越信任终点值
    return alpha * filtered.last() +
           (1 - alpha) * filtered[filtered.size()-2];
}

int displayChanneldata::MedianEWMADynamicAdaptiveFilter(const QVector<int>& intVec) {
    //空数据安全处理
    if (intVec.isEmpty()) {
        QLOG_WARN() << "MedianEWMADynamicAdaptiveFilter: 输入数据为空";
        return 0;
    }

    //高效类型转换（内存连续访问）
    QVector<double> doubleData;
    doubleData.reserve(intVec.size());

    const int* src = intVec.constData();
    for (int i = 0; i < intVec.size(); ++i) {
        doubleData.append(static_cast<double>(src[i]));
    }

    //应用复合滤波算法
    const double outResult = getFinalOptimalValue(doubleData);

    //智能舍入策略（避免截断误差）
    return static_cast<int>(std::round(outResult));
}

