#include "graphplot.h"
#include "ui_graphplot.h"
#include "cglobal.h"
#include "sampledataprocess.h"
#include "loginui.h"
#include <operclass/fullyautomatedplatelets.h>
#include <QSharedPointer>
#include <PrintReport/AggregationAnalyzer.h>
#include "globaldata.h"
#include <QtConcurrentRun>
#include <numeric> // for std::accumulate
#include <QFont>   // for QFont
#include <QObject> // for tr()


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif


GraphPlot::GraphPlot(QWidget *parent) :QWidget(parent),
    ui(new Ui::GraphPlot),
    mcuteNumData(1),
    mbaverage(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    bool ok_ = GlobalData::CreadFolder("suowei_testblood");
    QLOG_DEBUG()<<"创建索唯测试数据文件夹状态"<<ok_<<endl;

    InitChart();
}

GraphPlot::~GraphPlot()
{
    delete ui;


}


void GraphPlot::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    // Get equipment kind
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

    // Calculate available space
    const int availableWidth = ui->widget_CurveInner->width() - HORIZONTAL_SPACING * 5;
    const int availableHeight = ui->widget_CurveInner->height() - VERTICAL_SPACING * 4;

    // 使用辅助函数重构布局逻辑
    LayoutConfig config = calculateLayout(equipmentKind, availableWidth, availableHeight);
    adjustWidgetSizes(config, equipmentKind);
    positionWidgetsInGrid(config, equipmentKind);
}

void GraphPlot::innitKindequipment()
{
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmentKind);

    //预定义所有通道部件数组
    QWidget* allChannels[12] = {
        ui->aisle_widget_1, ui->aisle_widget_2, ui->aisle_widget_3, ui->aisle_widget_4,
        ui->aisle_widget_5, ui->aisle_widget_6, ui->aisle_widget_7, ui->aisle_widget_8,
        ui->aisle_widget_9, ui->aisle_widget_10, ui->aisle_widget_11, ui->aisle_widget_12
    };

    //根据设备类型确定通道数量
    int activeChannels = 0;
    switch (equipmentKind) {
        case KS600: activeChannels = 4; break;
        case KS800: activeChannels = 8; break;
        case KS1200: activeChannels = 12; break;
        default: activeChannels = 12; // 默认值
    }

    //统一处理通道显示/隐藏
    for (int i = 0; i < 12; ++i) {
        if (i < activeChannels) {
            m_pchnWidgetList.push_back(allChannels[i]);
            allChannels[i]->show(); // 确保显示
        } else {
            allChannels[i]->hide();
        }
    }

    for(auto pwidget : m_pchnWidgetList)
        {
            int channelIndex = QUIUtils::StringFindintnum(pwidget->objectName()); //从1开始
            if(QCustomPlot* pCurveWidget = pwidget->findChild<QCustomPlot *>()){
                initializeCurveWidget(pCurveWidget, channelIndex);
            }
        }
    return;
}

/** 所有样本测试结束测试曲线界面恢复
* @brief GraphPlot::backallCurveClear
*/
void GraphPlot::backallCurveClear()
{
    QMapIterator<quint8, QCPGraph*> it(mcurveWidgetList);
    while (it.hasNext()) {
        it.next();

        if (QCPGraph* graph = it.value()) {
            // 清空图形数据
            graph->data().data()->clear();
            graph->data().data()->squeeze();

            // 获取对应的 QCustomPlot 并设置背景
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
    if(hasCurve)
    {
        auto graph = mcurveWidgetList.find(channelIndex);
        graph.value()->data().data()->clear();
        graph.value()->data().data()->squeeze();

        QCustomPlot* plot = GetCurvepWidget(channelIndex);
        plot->setBackground(QBrush(QColor(188,187,183)));
        plot->replot(QCustomPlot::rpQueuedReplot);
    }
    return;
}

QCustomPlot* GraphPlot::GetCurvepWidget(int indexChn)
{
    // 使用缓存机制避免重复查找
    if (m_plotCache.contains(indexChn)) {
        return m_plotCache[indexChn];
    }

    QCustomPlot* pbackout = nullptr;
    for(auto pwidget : m_pchnWidgetList)
    {
        int ChnNum = QUIUtils::StringFindintnum(pwidget->objectName()) - 1; //通道从0 开始
        if(ChnNum == indexChn)
        {
            pbackout = pwidget->findChild<QCustomPlot *>();
            // 将结果存入缓存
            if (pbackout) {
                m_plotCache[indexChn] = pbackout;
            }
            break;
        }
    }
    return pbackout;
}


void GraphPlot::updatePlotTitle(quint8 channelIndex, QCustomPlot* plotWidget, QString titleText)
{
    if(m_titemap.contains(channelIndex))
    {
        auto iter = m_titemap.find(channelIndex);
        QString outprint = QString("通道%1%2").arg(channelIndex + 1).arg(titleText);
        iter.value()->setText(outprint);
        plotWidget->replot(/*QCustomPlot::rpQueuedReplot*/);
    }
}

void GraphPlot::initializeCurveWidget(QCustomPlot* plotWidget, int channelIndex)
{
	int indexChannel = channelIndex - 1;
	QString showText = QString("通道%1").arg(channelIndex);

	QCPTextElement *plotTitle = new QCPTextElement(plotWidget);
	plotTitle->setText(showText);
	plotTitle->setTextColor(QColor(0, 0, 0));

	plotTitle->setFont(QFont("宋体", 12, QFont::Normal));
	m_titemap.insert(indexChannel, plotTitle);

	plotWidget->plotLayout()->insertRow(0);
	plotWidget->plotLayout()->addElement(0, 0, plotTitle);

	// 设置曲线可拖拽 滚轮放大缩小 图像可选择
	// 禁用选择矩形
	plotWidget->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
	// 使能拖动
	plotWidget->setInteraction(QCP::iRangeDrag, true);

	plotWidget->setFont(QFont("Arial", 10));  // 设置文本的字体

											  // 设置背景色
	plotWidget->setBackground(QBrush(QColor("#f5f7f9")));
	plotWidget->axisRect()->setupFullAxesBox();  // 四边安装轴并显示

												 // 设置X/Y轴的标签
	plotWidget->xAxis->setLabel(QObject::tr("时间S"));
	plotWidget->yAxis->setLabel(QObject::tr("聚集率(%)"));

	// 设置X/Y轴标签颜色
	plotWidget->xAxis->setLabelColor(QColor(Qt::red));
	plotWidget->yAxis->setLabelColor(QColor(Qt::red));

	// 设置x=0或y=0所在直线的画笔
	plotWidget->xAxis->grid()->setZeroLinePen(QPen(QColor(Qt::darkGray)));
	plotWidget->yAxis->grid()->setZeroLinePen(QPen(QColor(Qt::darkGray)));

	// 设置X/Y轴刻度范围
	plotWidget->xAxis->setRange(0, 300);
	plotWidget->yAxis->setRange(-20, 100);

	// 刻度设置优化
	QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
	xTicker->setTickStep(30);
	xTicker->setTickCount(10);
	plotWidget->xAxis->setTicker(xTicker);

	QSharedPointer<QCPAxisTickerFixed> yTicker(new QCPAxisTickerFixed);
	yTicker->setTickStep(12);  // (-20到100共120单位，分10段)
	yTicker->setTickCount(10);
	plotWidget->yAxis->setTicker(yTicker);

	// 网格和零线设置
	QPen zeroLinePen;
	zeroLinePen.setColor(QColor(Qt::darkGreen));
	zeroLinePen.setWidth(2);
	plotWidget->xAxis->grid()->setZeroLinePen(zeroLinePen);
	plotWidget->yAxis->grid()->setZeroLinePen(zeroLinePen);

	plotWidget->xAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);
	plotWidget->yAxis->ticker()->setTickStepStrategy(QCPAxisTicker::tssReadability);

	// 设置X/Y轴刻度值文本的颜色
	plotWidget->xAxis->setTickLabelColor(QColor("#000000"));
	plotWidget->yAxis->setTickLabelColor(QColor("#000000"));

	// 设置X/Y轴轴线的画笔
	plotWidget->xAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));
	plotWidget->yAxis->setBasePen(QPen(QColor(Qt::black), 1, Qt::SolidLine));

	// 设置X/Y轴大刻度的画笔
	plotWidget->xAxis->setTickPen(QPen(QColor("#FF0000")));
	plotWidget->yAxis->setTickPen(QPen(QColor("#FF0000")));

	// 设置X/Y轴小刻度的画笔
	plotWidget->xAxis->setSubTickPen(QPen(QColor("#000000")));
	plotWidget->yAxis->setSubTickPen(QPen(QColor("#000000")));

	// 设置内部网格线的画笔
	plotWidget->xAxis->grid()->setPen(QPen(QColor(Qt::darkRed), 1, Qt::DotLine));
	plotWidget->yAxis->grid()->setPen(QPen(QColor(Qt::darkRed), 1, Qt::DotLine));

	QCPGraph* m_showCpgraphChannel = plotWidget->addGraph();
	m_showCpgraphChannel->setPen(QColor(0, 0, 0, 255));
	m_showCpgraphChannel->setSmooth(false);  // 平滑开启曲线
	mcurveWidgetList.insert(indexChannel, m_showCpgraphChannel);

	plotWidget->replot();
}


void GraphPlot::drawCurvePoint(quint8 channelIndex, QVector<double> xData, QVector<double> yData)
{
    // 参数验证
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
    if(hasCurve)
    {
        auto graph = mcurveWidgetList.find(channelIndex);
        graph.value()->setData(xData, yData);
        QCustomPlot* plot = GetCurvepWidget(channelIndex);
        plot->setBackground(QColor("#E8E8E8"));
        plot->replot(/*QCustomPlot::rpQueuedReplot*/);
    }
    return;
}

void GraphPlot::InitChart()
{
    for (int f = 0; f < MACHINE_SETTING_CHANNEL; f++){
        for (int k = 0; k < REAGENT_TOTAL; k++){
            mTestDataX[f][k].clear();    // 直接清空
            mTestDataY[f][k].clear();
            mTestDataX[f][k].reserve(NUMBEROFTESTDATA);
            mTestDataY[f][k].reserve(NUMBEROFTESTDATA);
        }
    }
    return;
}

/*单个项目测试完成 保存曲线图 进度条设置为0*/
void  GraphPlot::CompleteOneSample(const quint8 indexChannel, const quint8 reagents)
{
    //形参 通道数从0开始
    QString CompleteSample = "";
    int sampleid = 0;
    const quint8 testIndexReagent = reagents - 1;
    StructInstance::getInstance()->rootTestChnGetSampleInfo(indexChannel,CompleteSample,sampleid);

    resetVectorData(indexChannel,testIndexReagent);

    QCustomPlot* pPlot = GetCurvepWidget(indexChannel);
    QString outdata = "测试完成";
    updatePlotTitle(indexChannel, pPlot, outdata);

    //恢复异常通道显示状态
    FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleChannelFlash(true,indexChannel);

    QLOG_DEBUG()<<"试剂"<<GlobalData::mapIndexReagentnames(reagents)<<"测试结束通道"<<
                  indexChannel + 1<<"清空进度状态"<<endl;

    //保存测试的数据到数据库
    saveTestDataToSqllite(indexChannel,reagents,sampleid);
    return;
}







/*
 * 单个试剂测试完成后数据存入SQL
 * @brief saveTestDataToSqllite
 * @param indexChannel 通道索引
 * @param reagents 试剂类型
 * @param finishsampleid 样本ID
 */
void GraphPlot::saveTestDataToSqllite(const quint8 indexChannel, const quint8 reagents, int finishsampleid)
{
    // 参数验证
    if (finishsampleid <= 0) {
        qWarning() << "Invalid finishsampleid:" << finishsampleid;
        return;
    }

    // 获取贫血值和测试数据 && 清空测试完成试剂数据
    quint16 anaemiaInitValue = 0;
    quint8 anaemiaHole = 0, richBloodHole = 0;
    QVector<double> testedDataMap;

    StructInstance::getInstance()->root_getinitvaluedata(
        indexChannel, reagents, anaemiaInitValue,
        testedDataMap, anaemiaHole, richBloodHole);

    // 检查数据有效性
    if (testedDataMap.isEmpty()) {
        qWarning() << "No test data for channel:" << indexChannel << "reagent:" << reagents;
        return;
    }

    // 生成样本ID
    const QString sampleID = GlobalData::groupDateAndID(
    GlobalData::ObatinCreatSampleTime(), finishsampleid);

    // 转换曲线数据为字符串
    QString curvePoints;
    GlobalData::vectorconversionString(testedDataMap, curvePoints);

    // 获取数据库实例（避免重复调用）
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }

    // 保存曲线数据
    dbInstance->updateTestCurveDataTale(sampleID, reagents, curvePoints);
    dbInstance->updateTestCurveDataTale(sampleID, ANEMIA, QString::number(indexChannel));

    // 试剂类型映射表（更简洁）
    static const QMap<quint8, QString> reagentKeyMap = {
        {AA_REAGENT, "AA"},
        {ADP_REAGENT, "ADP"},
        {EPI_REAGENT, "EPI"},
        {COL_REAGENT, "COL"},
        {RIS_REAGENT, "RIS"}
    };

    const QString keyVal = reagentKeyMap.value(reagents, "UNKNOWN");

    // 保存测试结果 60S,180,300,MAXS
    const QString outResultData = QUIUtils::OutPrintTestedResult(testedDataMap);
    dbInstance->updateTestResultTable(sampleID, keyVal, outResultData);

    // 更新分析结果
    static const QMap<quint8, AnalyzerFunc> analyzerMap = {
        {AA_REAGENT, [this](const QString id, const QVector<double>& data) { analyzeAAAndSave(id,data); }},
        {ADP_REAGENT, [this](const QString id,const QVector<double>& data) { analyzeADPAndSave(id,data); }},
        {EPI_REAGENT, [this](const QString id,const QVector<double>& data) { analyzeEPIAndSave(id,data); }},
        {COL_REAGENT, [this](const QString id,const QVector<double>& data) { analyzeCOLAndSave(id,data); }},
        {RIS_REAGENT, [this](const QString id,const QVector<double>& data) { analyzeRISAndSave(id,data); }}
    };

    auto it = analyzerMap.find(reagents);
    if (it != analyzerMap.end()) {
        it.value()(sampleID,testedDataMap);
    } else {
        QLOG_WARN() << "Unknown reagent type:" << reagents;
    }


}

void GraphPlot::analyzeAAAndSave(const QString id,const QVector<double>& data)
{
    AAResult result = AggregationAnalyzer::analyzeAA(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }

    dbInstance->updateAnalysisResults(id,"AA",result.auc,result.maxSlope,
                                      result.tmaTime,result.lagTime,result.maxValue*0.8);

}

void GraphPlot::analyzeADPAndSave(const QString id,const QVector<double>& data)
{
    ADPResult result = AggregationAnalyzer::analyzeADP(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id,"ADP",result.auc,result.aggregationRate,
                                      result.tmaTime,result.lagTime,result.maxAggregation*0.8);
}
void GraphPlot::analyzeEPIAndSave(const QString id,const QVector<double>& data)
{
    EPIResult result = AggregationAnalyzer::analyzeEPI(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id,"EPI",result.auc,result.secondPhaseSlope,
                                      result.tmaTime,result.lagTime,0.00f);
}
void GraphPlot::analyzeCOLAndSave(const QString id,const QVector<double>& data)
{
    COLResult result = AggregationAnalyzer::analyzeCOL(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id,"COL",result.auc,result.maxSlope,
                                      result.tmaTime,result.lagTime,0.00f);
}
void GraphPlot::analyzeRISAndSave(const QString id,const QVector<double>& data)
{
    RISResult result = AggregationAnalyzer::analyzeRIS(data);
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }
    dbInstance->updateAnalysisResults(id,"RIS",result.auc,result.aggregationRate,
                                      result.tmaTime,result.lagTime,0.00f);
}









double GraphPlot::calculateMean(const QVector<double>& inputData)
{
    double sum = std::accumulate(inputData.begin(), inputData.end(), 0.0);
    double mean =  sum / inputData.size(); //均值
    return mean;
}

void  GraphPlot::recvsycnViewCurvePara(int cutnum , bool bavge)
{
    mcuteNumData = cutnum;
    mbaverage = bavge;
    QLOG_DEBUG()<<"分段数"<<mcuteNumData<<"平均"<<mbaverage;
}

void GraphPlot::resetVectorData(const int& channel, const int& reagentNumber){
    mTestDataX[channel][reagentNumber].clear();
    mTestDataY[channel][reagentNumber].clear();
    mTestDataX[channel][reagentNumber].reserve(NUMBEROFTESTDATA);
    mTestDataY[channel][reagentNumber].reserve(NUMBEROFTESTDATA);

    mCurvePlote_x[channel][reagentNumber].clear();
    mCurvePlote_y[channel][reagentNumber].clear();
    mCurvePlote_x[channel][reagentNumber].reserve(NUMBEROFTESTDATA);
    mCurvePlote_y[channel][reagentNumber].reserve(NUMBEROFTESTDATA);
}

/*接收测试数据实时绘制曲线*/
void GraphPlot::GetTestingValue(const QString &Sample, const quint8 &project, const quint8 &test_channel,
                                const float &testingdata,
                                const int &nPRP,
                                const int &initPRP,
                                const int &initPPP){

    const quint8 testingChannel =  test_channel - 1;
    const quint8 testingReagents = project - 1;

    QString datestr;
    int idnum;
    QString ReagentName =  GlobalData::mapIndexReagentnames(project);
    GlobalData::apartSampleId(Sample, datestr, idnum);

    QString data_ = QString::number(testingdata*100,'f',2)+"%";
    QCustomPlot* pPlot = GetCurvepWidget(testingChannel);
    QString outdata = QObject::tr("%1:PPP=%2 PRP=%3 %5(%4)").arg(ReagentName).arg(initPPP).arg(initPRP).arg(nPRP).arg(data_);
    updatePlotTitle(testingChannel, pPlot,outdata);


    //绘制曲线
    mTestDataX[testingChannel][testingReagents].push_back(mTestDataY[testingChannel][testingReagents].size());
    if (!std::isfinite(testingdata)) {
        // 处理 NaN 或 Inf 的情况
         mTestDataY[testingChannel][testingReagents].push_back( 0 * 100.00);
    }else{
         mTestDataY[testingChannel][testingReagents].push_back(testingdata*100.00);
    }



    //绘制测试界面模组的进度条
    emit DrawProgressbar(testingChannel, calculateProgress(testingChannel, testingReagents));


    const QString filename = QString("Id%1通道%2%3").arg(idnum).arg(test_channel).arg(ReagentName);
    const bool isopenexperimentalMode = INI_File().getexperimentalMode();
    if(mTestDataY[testingChannel][testingReagents].size() == 1){
         clearCurve(testingChannel);
         if(isopenexperimentalMode){
             QString datastr = QString("initPPP:%1 initPRP:%2 PRP(1):%3").arg(initPPP).arg(initPRP).arg(nPRP);
             GlobalData::CreatFloadfileAppend("ExperimentalMode",filename,datastr);
         }
    }
    else if(isopenexperimentalMode){
         GlobalData::CreatFloadfileAppend("ExperimentalMode",filename, QString::number(nPRP)+":"+ data_);
    }

    if(mcuteNumData == 1 && mbaverage == false )
    {
        drawCurvePoint(testingChannel,
                       mTestDataX[testingChannel][testingReagents],
                       mTestDataY[testingChannel][testingReagents]);
    }
    else if(mcuteNumData != 1)
    {
        int numTotal = mTestDataX[testingChannel][testingReagents].size();
        if(numTotal == 1)
        {
            mCurvePlote_x[testingChannel][testingReagents].push_back(0);
            mCurvePlote_y[testingChannel][testingReagents].push_back(testingdata*100.00);
            drawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagents],
                           mCurvePlote_y[testingChannel][testingReagents]);
        }
        else if(numTotal % mcuteNumData == 0 && numTotal/mcuteNumData != 0 && mbaverage == false)
        {
           mCurvePlote_x[testingChannel][testingReagents].push_back(mTestDataY[testingChannel][testingReagents].size());
           mCurvePlote_y[testingChannel][testingReagents].push_back(*(mTestDataY[testingChannel][testingReagents].end() - 1)*100.00);
           drawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagents], mCurvePlote_y[testingChannel][testingReagents]);
        }
        else if(numTotal % mcuteNumData == 0 && numTotal/mcuteNumData != 0 && mbaverage == true)
        {
            QVector<double> prpvalue;
            prpvalue.clear();
            for(int i = 1 ; i <= mcuteNumData ; i++)
            {
               prpvalue.push_back(*(mTestDataY[testingChannel][testingReagents].end()- i));
            }
            double  outprp = calculateMean(prpvalue);
            mCurvePlote_x[testingChannel][testingReagents].push_back(mTestDataY[testingChannel][testingReagents].size());
            mCurvePlote_y[testingChannel][testingReagents].push_back(outprp);
            drawCurvePoint(testingChannel, mCurvePlote_x[testingChannel][testingReagents],
                           mCurvePlote_y[testingChannel][testingReagents]);
        }
    }


    return;
}

double GraphPlot::calculateProgress(int channel, int reagent) const {
    return qBound(0.0,
           static_cast<double>(mTestDataX[channel][reagent].size())*100/NUMBEROFTESTDATA,
           100.0);
}

// 布局计算辅助函数实现
GraphPlot::LayoutConfig GraphPlot::calculateLayout(quint8 equipmentKind, int availableWidth, int availableHeight) const
{
    LayoutConfig config;
    config.columns = DEFAULT_COLUMNS;

    // Determine grid layout based on equipment kind
    switch (equipmentKind) {
    case KS600:
        config.rows = 1;
        config.curveHeight = availableHeight / 3 + availableHeight / 6; // same as /3 + /3/2
        break;
    case KS800:
        config.rows = 2;
        config.curveHeight = availableHeight / config.rows;
        break;
    case KS1200:
        config.rows = 3;
        config.curveHeight = availableHeight / config.rows;
        break;
    default:
        config.rows = 1;
        config.curveHeight = availableHeight / 3 + availableHeight / 6;
        break;
    }

    config.curveWidth = availableWidth / config.columns;
    return config;
}

void GraphPlot::positionWidgetsInGrid(const LayoutConfig& config, quint8 equipmentKind)
{
    // Position all widgets in the grid
    for (int i = 0; i < m_pchnWidgetList.size(); i++) {
        int col = i % config.columns;
        int row = i / config.columns;

        // Skip if we exceed the expected widget count for this equipment
        if ((equipmentKind == KS600 && i >= 4) ||
            (equipmentKind == KS800 && i >= 8) ||
            (equipmentKind == KS1200 && i >= 12)) {
            break;
        }

        int x = HORIZONTAL_SPACING * (col + 1) + col * config.curveWidth;
        int y = VERTICAL_SPACING * (row + 1) + row * config.curveHeight;

        if (equipmentKind == KS600) {
            y = VERTICAL_SPACING + config.curveHeight / 2; // Special vertical position for KS600
        }

        m_pchnWidgetList[i]->setGeometry(x, y, config.curveWidth, config.curveHeight);
    }
}

void GraphPlot::adjustWidgetSizes(const LayoutConfig& config, quint8 equipmentKind)
{
    for (int i = 0; i < m_pchnWidgetList.size(); i++) {
        // Skip if we exceed the expected widget count for this equipment
        if ((equipmentKind == KS600 && i >= 4) ||
            (equipmentKind == KS800 && i >= 8) ||
            (equipmentKind == KS1200 && i >= 12)) {
            break;
        }

        if (equipmentKind == KS600) {
            m_pchnWidgetList[i]->setFixedSize(config.curveWidth, config.curveHeight);
        } else {
            m_pchnWidgetList[i]->setMaximumSize(config.curveWidth, config.curveHeight);
        }
    }
}





