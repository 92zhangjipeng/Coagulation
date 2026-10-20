#include "graphplot.h"
#include "cglobal.h"
#include "structinstance.h"
#include "sampledataprocess.h"
#include <operclass/fullyautomatedplatelets.h>
#include <QSharedPointer>
#include <PrintReport/AggregationAnalyzer.h>
#include "globaldata.h"
#include <QtConcurrentRun>
#include <numeric>
#include <QFont>
#include <QObject>
#include <QVBoxLayout>


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif


GraphPlot::GraphPlot(QWidget *parent) : QWidget(parent),
    m_widgetCurveInner(nullptr),
    m_mainLayout(nullptr),
    mcuteNumData(1),
    mbaverage(false)
{
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setStyleSheet("border: 2px solid rgb(220, 220, 220); background-color:rgba(250,250,250);");

    bool ok_ = GlobalData::CreadFolder("suowei_testblood");
    QLOG_DEBUG() << "创建索唯测试数据文件夹状态" << ok_ << endl;

    createWidgetsDynamically();
    InitChart();
}

GraphPlot::~GraphPlot()
{
}

void GraphPlot::createWidgetsDynamically()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    m_widgetCurveInner = new QWidget(this);
    m_widgetCurveInner->setStyleSheet("border: 1px solid rgb(120, 120, 120); border-radius: 1px; background: transparent;");
    m_mainLayout->addWidget(m_widgetCurveInner);

    for (int i = 0; i < MAX_CHANNELS; ++i) {
        QWidget* aisleWidget = new QWidget(m_widgetCurveInner);
        aisleWidget->setObjectName(QString("aisle_widget_%1").arg(i + 1));
        aisleWidget->setStyleSheet("border: 1px solid rgb(225, 225, 225); border-radius: 8px; background: transparent;");

        QVBoxLayout* aisleLayout = new QVBoxLayout(aisleWidget);
        aisleLayout->setSpacing(5);
        aisleLayout->setContentsMargins(5, 5, 5, 5);

        QCustomPlot* customPlot = new QCustomPlot(aisleWidget);
        customPlot->setObjectName(QString("Plot_aisle_%1_result").arg(i + 1));
        customPlot->setStyleSheet("border: 1px solid rgb(200, 200, 200); background-color:rgba(245, 247, 249);");

        aisleLayout->addWidget(customPlot);

        m_pchnWidgetList.append(aisleWidget);
        m_customPlotList.append(customPlot);
    }
}

void GraphPlot::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

    const int availableWidth = m_widgetCurveInner->width() - HORIZONTAL_SPACING * (DEFAULT_COLUMNS + 1);
    const int availableHeight = m_widgetCurveInner->height() - VERTICAL_SPACING * 4;

    LayoutConfig config = calculateLayout(equipmentKind, availableWidth, availableHeight);
    adjustWidgetSizes(config, equipmentKind);
    positionWidgetsInGrid(config, equipmentKind);
}

void GraphPlot::innitKindequipment()
{
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

    int activeChannels = 0;
    switch (equipmentKind) {
        case KS600: activeChannels = 4; break;
        case KS800: activeChannels = 8; break;
        case KS1200: activeChannels = 12; break;
        default: activeChannels = 12;
    }

    for (int i = 0; i < m_pchnWidgetList.size(); ++i) {
        if (i < activeChannels) {
            m_pchnWidgetList[i]->show();
            initializeCurveWidget(m_customPlotList[i], i + 1);
        } else {
            m_pchnWidgetList[i]->hide();
        }
    }
}

void GraphPlot::backallCurveClear()
{
    for (int i = 0; i < MAX_CHANNELS; ++i) {
        m_lastTitleText[i].clear();
        setChannelPppAbnormal(static_cast<quint8>(i), false);
    }

    QMapIterator<quint8, QCPGraph*> it(mcurveWidgetList);
    while (it.hasNext()) {
        it.next();

        if (QCPGraph* graph = it.value()) {
            graph->data().data()->clear();
            graph->data().data()->squeeze();

            if (QCustomPlot* pPlot = GetCurvepWidget(it.key())) {
                updatePlotTitle(it.key(), pPlot, "");
                pPlot->setBackground(QBrush(QColor("#FFFFFF")));
                pPlot->replot(QCustomPlot::rpQueuedReplot);
            }
        }
    }
}

void GraphPlot::clearCurve(quint8 channelIndex)
{
    bool hasCurve = mcurveWidgetList.contains(channelIndex);
    if (hasCurve) {
        auto graph = mcurveWidgetList.find(channelIndex);
        graph.value()->data().data()->clear();
        graph.value()->data().data()->squeeze();

        QCustomPlot* plot = GetCurvepWidget(channelIndex);
        plot->setBackground(QBrush(QColor(245, 247, 249)));
        applyChannelCurveStyle(channelIndex);
        plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

QCustomPlot* GraphPlot::GetCurvepWidget(int indexChn)
{
    if (m_plotCache.contains(indexChn)) {
        return m_plotCache[indexChn];
    }

    QCustomPlot* pbackout = nullptr;
    if (indexChn >= 0 && indexChn < m_customPlotList.size()) {
        pbackout = m_customPlotList[indexChn];
        m_plotCache[indexChn] = pbackout;
    }
    return pbackout;
}

void GraphPlot::updatePlotTitle(quint8 channelIndex, QCustomPlot* plotWidget, QString titleText)
{
    if (!plotWidget || channelIndex >= MAX_CHANNELS || !m_titemap.contains(channelIndex)) {
        return;
    }
    m_lastTitleText[channelIndex] = titleText;
    auto iter = m_titemap.find(channelIndex);
    QString outprint = tr("通道%1%2").arg(channelIndex + 1).arg(titleText);
    if (m_pppAbnormal[channelIndex]) {
        if (!outprint.contains(tr("测试异常"))) {
            outprint.append(tr(" 测试异常"));
        }
        iter.value()->setTextColor(QColor(183, 28, 28));
    } else {
        iter.value()->setTextColor(QColor(50, 50, 50));
    }
    iter.value()->setText(outprint);
    plotWidget->replot();
}

void GraphPlot::applyChannelCurveStyle(quint8 channelIndex)
{
    if (channelIndex >= MAX_CHANNELS) {
        return;
    }
    const bool abnormal = m_pppAbnormal[channelIndex];
    if (channelIndex < m_pchnWidgetList.size() && m_pchnWidgetList[channelIndex]) {
        const QString objName = m_pchnWidgetList[channelIndex]->objectName();
        if (abnormal) {
            m_pchnWidgetList[channelIndex]->setStyleSheet(
                QString::fromUtf8("#%1 { border: 3px solid #E53935; border-radius: 8px;"
                                  " background-color: #FFF8E1; }").arg(objName));
        } else {
            m_pchnWidgetList[channelIndex]->setStyleSheet(
                QString::fromUtf8("#%1 { border: 1px solid rgb(225, 225, 225); border-radius: 8px;"
                                  " background: transparent; }").arg(objName));
        }
    }
    if (channelIndex < m_customPlotList.size() && m_customPlotList[channelIndex]) {
        if (abnormal) {
            m_customPlotList[channelIndex]->setStyleSheet(
                QString::fromUtf8("border: 2px solid #FFC107; background-color: #FFF3E0;"));
            m_customPlotList[channelIndex]->setBackground(QBrush(QColor(255, 243, 224)));
        } else {
            m_customPlotList[channelIndex]->setStyleSheet(
                QString::fromUtf8("border: 1px solid rgb(200, 200, 200);"
                                  " background-color:rgba(245, 247, 249);"));
        }
    }
}

void GraphPlot::setChannelPppAbnormal(quint8 channelIndex, bool abnormal)
{
    if (channelIndex >= MAX_CHANNELS) {
        return;
    }
    m_pppAbnormal[channelIndex] = abnormal;
    applyChannelCurveStyle(channelIndex);
    if (QCustomPlot *plot = GetCurvepWidget(channelIndex)) {
        updatePlotTitle(channelIndex, plot, m_lastTitleText[channelIndex]);
    }
}

void GraphPlot::onPppInitRead(quint8 channelIndex, int pppValue)
{
    if (channelIndex >= MAX_CHANNELS) {
        return;
    }
    const bool abnormal = (pppValue > 0 && pppValue < GlobalData::kPppLowVolumeWarn);
    setChannelPppAbnormal(channelIndex, abnormal);
    QCustomPlot *plot = GetCurvepWidget(channelIndex);
    if (!plot) {
        return;
    }
    if (abnormal) {
        QLOG_WARN() << "通道" << (channelIndex + 1) << "PPP=" << pppValue
                    << "小于" << GlobalData::kPppLowVolumeWarn << "，曲线标记测试异常";
        updatePlotTitle(channelIndex, plot, tr(" PPP=%1").arg(pppValue));
    } else {
        updatePlotTitle(channelIndex, plot, QString());
    }
}

void GraphPlot::initializeCurveWidget(QCustomPlot* plotWidget, int channelIndex)
{
    int indexChannel = channelIndex - 1;
    QString showText = QString("通道%1").arg(channelIndex);

    QCPTextElement *plotTitle = new QCPTextElement(plotWidget);
    plotTitle->setText(showText);
    plotTitle->setTextColor(QColor(50, 50, 50));
    plotTitle->setFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    m_titemap.insert(indexChannel, plotTitle);

    plotWidget->plotLayout()->insertRow(0);
    plotWidget->plotLayout()->addElement(0, 0, plotTitle);

    plotWidget->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    plotWidget->setInteraction(QCP::iRangeDrag, true);
    plotWidget->setFont(QFont("Arial", 9));

    plotWidget->setBackground(QBrush(QColor("#F8FAFC")));
    plotWidget->axisRect()->setupFullAxesBox();

    plotWidget->xAxis->setLabel(QObject::tr("时间(秒)"));
    plotWidget->yAxis->setLabel(QObject::tr("聚集率(%)"));

    plotWidget->xAxis->setLabelColor(QColor(70, 130, 180));
    plotWidget->yAxis->setLabelColor(QColor(70, 130, 180));

    plotWidget->xAxis->grid()->setZeroLinePen(QPen(QColor(100, 149, 237), 1.5));
    plotWidget->yAxis->grid()->setZeroLinePen(QPen(QColor(100, 149, 237), 1.5));

    plotWidget->xAxis->setRange(0, 300);
    plotWidget->yAxis->setRange(-20, 100);

    QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    xTicker->setTickStep(30);
    xTicker->setTickCount(10);
    plotWidget->xAxis->setTicker(xTicker);

    QSharedPointer<QCPAxisTickerFixed> yTicker(new QCPAxisTickerFixed);
    yTicker->setTickStep(20);
    yTicker->setTickCount(6);
    plotWidget->yAxis->setTicker(yTicker);

    plotWidget->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
    plotWidget->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

    plotWidget->xAxis->setTickLabelColor(QColor(60, 60, 60));
    plotWidget->yAxis->setTickLabelColor(QColor(60, 60, 60));

    plotWidget->xAxis->setBasePen(QPen(QColor(80, 80, 80), 1, Qt::SolidLine));
    plotWidget->yAxis->setBasePen(QPen(QColor(80, 80, 80), 1, Qt::SolidLine));

    plotWidget->xAxis->setTickPen(QPen(QColor(70, 130, 180)));
    plotWidget->yAxis->setTickPen(QPen(QColor(70, 130, 180)));

    plotWidget->xAxis->setSubTickPen(QPen(QColor(150, 150, 150)));
    plotWidget->yAxis->setSubTickPen(QPen(QColor(150, 150, 150)));

    plotWidget->xAxis->grid()->setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    plotWidget->yAxis->grid()->setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));

    QCPGraph* m_showCpgraphChannel = plotWidget->addGraph();
    m_showCpgraphChannel->setPen(QPen(QColor(10, 10, 10), 1.5));
    m_showCpgraphChannel->setLineStyle(QCPGraph::lsLine);
    m_showCpgraphChannel->setScatterStyle(QCPScatterStyle::ssNone);   // 不显示散点
    mcurveWidgetList.insert(indexChannel, m_showCpgraphChannel);

    plotWidget->replot();
}

void GraphPlot::drawCurvePoint(quint8 channelIndex, QVector<double> xData, QVector<double> yData)
{
    if (channelIndex >= MAX_CHANNELS) {
        qWarning() << "Invalid channel index:" << channelIndex << ", maximum allowed:" << MAX_CHANNELS - 1;
        return;
    }

    if (xData.size() != yData.size()) {
        qWarning() << "X and Y data size mismatch. X size:" << xData.size() << ", Y size:" << yData.size();
        return;
    }

    if (xData.isEmpty()) {
        qWarning() << "Empty data provided for curve drawing";
        return;
    }

    bool hasCurve = mcurveWidgetList.contains(channelIndex);
    if (hasCurve) {
        auto graph = mcurveWidgetList.find(channelIndex);
        graph.value()->setData(xData, yData);
        QCustomPlot* plot = GetCurvepWidget(channelIndex);
        plot->setBackground(QColor("#F0F4F8"));
        plot->replot();
    }
}

void GraphPlot::InitChart()
{
    for (int f = 0; f < MAX_CHANNELS; f++) {
        for (int k = 0; k < MAX_REAGENTS; k++) {
            mTestDataX[f][k].clear();
            mTestDataY[f][k].clear();
            mTestDataX[f][k].reserve(NUMBEROFTESTDATA);
            mTestDataY[f][k].reserve(NUMBEROFTESTDATA);
        }
    }
}

void GraphPlot::CompleteOneSample(const quint8 indexChannel, const quint8 reagents)
{
    QString CompleteSample = "";
    int sampleid = 0;
    const quint8 testIndexReagent = reagents - 1;
    StructInstance::getInstance()->rootTestChnGetSampleInfo(indexChannel, CompleteSample, sampleid);

    resetVectorData(indexChannel, testIndexReagent);

    QCustomPlot* pPlot = GetCurvepWidget(indexChannel);
    QString outdata = "测试完成";
    updatePlotTitle(indexChannel, pPlot, outdata);

    FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleChannelFlash(true, indexChannel);

    QLOG_DEBUG() << "试剂" << GlobalData::mapIndexReagentnames(reagents) << "测试结束通道"
                 << indexChannel + 1 << "清空进度状态" << endl;


    // --- 获取一次数据，避免重复调用 rootGetinitvaluedata ---
    quint16 anaemiaInitValue = 0;
    quint8 anaemiaHole = 0, richBloodHole = 0;
    QVector<double> testedDataMap;
    StructInstance::getInstance()->rootGetinitvaluedata(indexChannel, reagents,
                                                         anaemiaInitValue,
                                                         testedDataMap,
                                                         anaemiaHole,
                                                         richBloodHole);

    // 有效：使用已获取的真实数据保存
    saveTestDataToSqllite(indexChannel, reagents, sampleid, testedDataMap);
}
// ==================== 辅助函数实现 ====================

QString GraphPlot::getFormattedSampleId(int finishsampleid, bool &isFollowUp) const
{
    const QString sampleID = GlobalData::groupDateAndID(GlobalData::ObatinCreatSampleTime(), finishsampleid);
    auto *paddTaskCol = FullyAutomatedPlatelets::pinstancepatientdata();
    isFollowUp = paddTaskCol ? paddTaskCol->hasfollowUpSample(sampleID) : false;
    return isFollowUp ? QString("%1F").arg(sampleID) : sampleID;
}

void GraphPlot::saveCurveDataToDatabase(const QString &sampleId, quint8 reagent,
                                        const QVector<double> &curveData, quint8 indexChannel)
{
    auto *db = FullyAutomatedPlatelets::pinstancesqlData();
    if (!db) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }

    QString curvePoints;
    GlobalData::vectorconversionString(curveData, curvePoints);
    db->updateTestCurveDataTale(sampleId, reagent, curvePoints);
    db->setTestCurveDataChannel(sampleId,indexChannel);
    //db->updateTestCurveDataTale(sampleId, ANEMIA, QString::number(indexChannel));

    static const QMap<quint8, QString> reagentKeyMap = {
        {AA_REAGENT, "AA"}, {ADP_REAGENT, "ADP"}, {EPI_REAGENT, "EPI"},
        {COL_REAGENT, "COL"}, {RIS_REAGENT, "RIS"}
    };
    const QString keyVal = reagentKeyMap.value(reagent, "UNKNOWN");
    const QString outResultData = QUIUtils::OutPrintTestedResult(curveData);
    db->updateTestResultTable(sampleId, keyVal, outResultData);
}


void GraphPlot::saveAnalysisResultForReagent(const QString &sampleId, quint8 reagent,
                                             const QVector<double> &processedData)
{
    // 直接调用原有的五个分析函数，它们内部已有数据库操作
    switch (reagent) {
        case AA_REAGENT: analyzeAAAndSave(sampleId, processedData); break;
        case ADP_REAGENT: analyzeADPAndSave(sampleId, processedData); break;
        case EPI_REAGENT: analyzeEPIAndSave(sampleId, processedData); break;
        case COL_REAGENT: analyzeCOLAndSave(sampleId, processedData); break;
        case RIS_REAGENT: analyzeRISAndSave(sampleId, processedData); break;
        default: QLOG_WARN() << "Unknown reagent type:" << reagent;
    }
}


// 新增重载版本：接受已获取的数据，避免重复调用 rootGetinitvaluedata
void GraphPlot::saveTestDataToSqllite(const quint8 indexChannel, const quint8 reagents,
                                      int finishsampleid, const QVector<double> &testedDataMap)
{
    if (finishsampleid <= 0 || testedDataMap.isEmpty()) {
        QLOG_WARN() << "Invalid finishsampleid or empty data";
        return;
    }

    bool isFollowUp = false;
    QString testFinishSampleId = getFormattedSampleId(finishsampleid, isFollowUp);

    // 保存原始曲线数据到数据库
    saveCurveDataToDatabase(testFinishSampleId, reagents, testedDataMap, indexChannel + 1);

    // 处理数据：放大100倍并四舍五入保留4位小数，然后交给分析函数
    QVector<double> processedData;
    processedData.reserve(testedDataMap.size());
    for (double val : testedDataMap) {
        val = val * 100.0;
        val = std::round(val * 10000.0) / 10000.0;
        processedData.append(val);
    }
    saveAnalysisResultForReagent(testFinishSampleId, reagents, processedData);
}



void GraphPlot::saveTestDataToSqllite(const quint8 indexChannel, const quint8 reagents, int finishsampleid)
{
    // 此处仍然需要获取数据，因为可能被其他地方单独调用
    quint16 anaemiaInitValue = 0;
    quint8 anaemiaHole = 0, richBloodHole = 0;
    QVector<double> testedDataMap;
    StructInstance::getInstance()->rootGetinitvaluedata(indexChannel, reagents,
                                                         anaemiaInitValue,
                                                         testedDataMap,
                                                         anaemiaHole,
                                                         richBloodHole);
    if (testedDataMap.isEmpty()) {
        QLOG_WARN() << "No test data for channel:" << indexChannel << "reagent:" << reagents;
        return;
    }
    // 调用新重载
    saveTestDataToSqllite(indexChannel, reagents, finishsampleid, testedDataMap);
}


void GraphPlot::analyzeAAAndSave(const QString id, const QVector<double>& data)
{
    AAResult result = AggregationAnalyzer::analyzeAA(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }

    dbInstance->updateAnalysisResults(id, "AA", result.auc, result.maxSlope, result.tmaTime, result.lagTime, result.maxValue * 0.8);
}

void GraphPlot::analyzeADPAndSave(const QString id, const QVector<double>& data)
{
    ADPResult result = AggregationAnalyzer::analyzeADP(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id, "ADP", result.auc, result.aggregationRate, result.tmaTime, result.lagTime, result.maxAggregation * 0.8);
}

void GraphPlot::analyzeEPIAndSave(const QString id, const QVector<double>& data)
{
    EPIResult result = AggregationAnalyzer::analyzeEPI(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id, "EPI", result.auc, result.secondPhaseSlope, result.tmaTime, result.lagTime, 0.00f);
}

void GraphPlot::analyzeCOLAndSave(const QString id, const QVector<double>& data)
{
    COLResult result = AggregationAnalyzer::analyzeCOL(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id, "COL", result.auc, result.maxSlope, result.tmaTime, result.lagTime, 0.00f);
}

void GraphPlot::analyzeRISAndSave(const QString id, const QVector<double>& data)
{
    RISResult result = AggregationAnalyzer::analyzeRIS(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id, "RIS", result.auc, result.aggregationRate, result.tmaTime, result.lagTime, 0.00f);
}

double GraphPlot::calculateMean(const QVector<double>& inputData)
{
    double sum = std::accumulate(inputData.begin(), inputData.end(), 0.0);
    double mean = sum / inputData.size();
    return mean;
}

void GraphPlot::recvsycnViewCurvePara(int cutnum, bool bavge)
{
    mcuteNumData = cutnum;
    mbaverage = bavge;
    QLOG_DEBUG() << "分段数" << mcuteNumData << "平均" << mbaverage;
}

void GraphPlot::resetVectorData(const int& channel, const int& reagentNumber)
{
    mTestDataX[channel][reagentNumber].clear();
    mTestDataY[channel][reagentNumber].clear();
    mTestDataX[channel][reagentNumber].reserve(NUMBEROFTESTDATA);
    mTestDataY[channel][reagentNumber].reserve(NUMBEROFTESTDATA);

    mCurvePlote_x[channel][reagentNumber].clear();
    mCurvePlote_y[channel][reagentNumber].clear();
    mCurvePlote_x[channel][reagentNumber].reserve(NUMBEROFTESTDATA);
    mCurvePlote_y[channel][reagentNumber].reserve(NUMBEROFTESTDATA);
}

void GraphPlot::GetTestingValue(const QString& Sample, const quint8& project, const quint8& test_channel,
                                const float& testingdata, const int& nPRP, const int& initPRP, const int& initPPP)
{
    const quint8 testingChannel = test_channel - 1;
    const quint8 testingReagent = project - 1;

    QString datestr;
    int idnum;
    QString ReagentName = GlobalData::mapIndexReagentnames(project);
    GlobalData::apartSampleId(Sample, datestr, idnum);

    QString data_;
    float percentValue = testingdata * 100.0f;

    if (percentValue < 0.01) {
        // 非常小的值，显示6位小数
        data_ = QString::number(percentValue, 'f', 6) + "%";
    } else if (percentValue < 0.1) {
        // 较小值，显示4位小数
        data_ = QString::number(percentValue, 'f', 4) + "%";
    } else if (percentValue < 1.0) {
        // 中等值，显示3位小数
        data_ = QString::number(percentValue, 'f', 3) + "%";
    } else {
        // 正常值，显示2位小数
        data_ = QString::number(percentValue, 'f', 2) + "%";
    }



    QCustomPlot* pPlot = GetCurvepWidget(testingChannel);
    if (initPPP > 0 && initPPP < GlobalData::kPppLowVolumeWarn) {
        setChannelPppAbnormal(testingChannel, true);
    }
    QString outdata = QObject::tr("%1:PPP=%2 PRP=%3 %5(%4)").arg(ReagentName).arg(initPPP).arg(initPRP).arg(nPRP).arg(data_);
    updatePlotTitle(testingChannel, pPlot, outdata);

    mTestDataX[testingChannel][testingReagent].push_back(mTestDataY[testingChannel][testingReagent].size());
    if (!std::isfinite(testingdata)) {
        mTestDataY[testingChannel][testingReagent].push_back(0 * 100.00);
    } else {
        mTestDataY[testingChannel][testingReagent].push_back(testingdata * 100.00);
    }

    emit DrawProgressbar(testingChannel, calculateProgress(testingChannel, testingReagent));

    const QString filename = QString("Id%1通道%2%3").arg(idnum).arg(test_channel).arg(ReagentName);
    const bool isopenexperimentalMode = INI_File().getexperimentalMode();
    if (mTestDataY[testingChannel][testingReagent].size() == 1) {
        clearCurve(testingChannel);
        if (isopenexperimentalMode) {
            QString datastr = QString("initPPP:%1 initPRP:%2 PRP(1):%3").arg(initPPP).arg(initPRP).arg(nPRP);
            GlobalData::CreatFloadfileAppend("ExperimentalMode", filename, datastr);
        }
    } else if (isopenexperimentalMode) {
        GlobalData::CreatFloadfileAppend("ExperimentalMode", filename, QString::number(nPRP) + ":" + data_);
    }

    if (mcuteNumData == 1 && mbaverage == false) {
        drawCurvePoint(testingChannel, mTestDataX[testingChannel][testingReagent], mTestDataY[testingChannel][testingReagent]);
    } else if (mcuteNumData != 1) {
        int numTotal = mTestDataX[testingChannel][testingReagent].size();
        if (numTotal == 1) {
            mCurvePlote_x[testingChannel][testingReagent].push_back(0);
            mCurvePlote_y[testingChannel][testingReagent].push_back(testingdata * 100.00);
            drawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagent], mCurvePlote_y[testingChannel][testingReagent]);
        } else if (numTotal % mcuteNumData == 0 && numTotal / mcuteNumData != 0 && mbaverage == false) {
            mCurvePlote_x[testingChannel][testingReagent].push_back(mTestDataY[testingChannel][testingReagent].size());
            mCurvePlote_y[testingChannel][testingReagent].push_back(*(mTestDataY[testingChannel][testingReagent].end() - 1) * 100.00);
            drawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagent], mCurvePlote_y[testingChannel][testingReagent]);
        } else if (numTotal % mcuteNumData == 0 && numTotal / mcuteNumData != 0 && mbaverage == true) {
            QVector<double> prpvalue;
            prpvalue.clear();
            for (int i = 1; i <= mcuteNumData; ++i) {
                prpvalue.push_back(*(mTestDataY[testingChannel][testingReagent].end() - i));
            }
            double outprp = calculateMean(prpvalue);
            mCurvePlote_x[testingChannel][testingReagent].push_back(mTestDataY[testingChannel][testingReagent].size());
            mCurvePlote_y[testingChannel][testingReagent].push_back(outprp);
            drawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagent], mCurvePlote_y[testingChannel][testingReagent]);
        }
    }
}

double GraphPlot::calculateProgress(int channel, int reagent) const
{
    return qBound(0.0, static_cast<double>(mTestDataX[channel][reagent].size()) * 100 / NUMBEROFTESTDATA, 100.0);
}

GraphPlot::LayoutConfig GraphPlot::calculateLayout(quint8 equipmentKind, int availableWidth, int availableHeight) const
{
    LayoutConfig config;

    switch (equipmentKind) {
        case KS600:
            config.columns = 2;
            config.rows = 2;
            config.curveHeight = availableHeight / config.rows;
            config.curveWidth = availableWidth / config.columns;
            break;
        case KS800:
            config.columns = 4;
            config.rows = 2;
            config.curveHeight = availableHeight / config.rows;
            config.curveWidth = availableWidth / config.columns;
            break;
        case KS1200:
            config.columns = 4;
            config.rows = 3;
            config.curveHeight = availableHeight / config.rows;
            config.curveWidth = availableWidth / config.columns;
            break;
        default:
            config.columns = 4;
            config.rows = 3;
            config.curveHeight = availableHeight / config.rows;
            config.curveWidth = availableWidth / config.columns;
            break;
    }

    return config;
}

void GraphPlot::positionWidgetsInGrid(const LayoutConfig& config, quint8 equipmentKind)
{
    int activeChannels = 0;
    switch (equipmentKind) {
        case KS600: activeChannels = 4; break;
        case KS800: activeChannels = 8; break;
        case KS1200: activeChannels = 12; break;
        default: activeChannels = 12;
    }

    for (int i = 0; i < activeChannels && i < m_pchnWidgetList.size(); ++i) {
        int col = i % config.columns;
        int row = i / config.columns;

        int x = HORIZONTAL_SPACING + col * (config.curveWidth + HORIZONTAL_SPACING);
        int y = VERTICAL_SPACING + row * (config.curveHeight + VERTICAL_SPACING);

        m_pchnWidgetList[i]->setGeometry(x, y, config.curveWidth, config.curveHeight);
    }
}

void GraphPlot::adjustWidgetSizes(const LayoutConfig& config, quint8 equipmentKind)
{
    int activeChannels = 0;
    switch (equipmentKind) {
        case KS600: activeChannels = 4; break;
        case KS800: activeChannels = 8; break;
        case KS1200: activeChannels = 12; break;
        default: activeChannels = 12;
    }

    for (int i = 0; i < activeChannels && i < m_pchnWidgetList.size(); ++i) {
        m_pchnWidgetList[i]->setFixedSize(config.curveWidth, config.curveHeight);
    }
}
