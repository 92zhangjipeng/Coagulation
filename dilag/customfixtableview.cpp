#include "customfixtableview.h"
#include "ui_customfixtableview.h"
#include <QVBoxLayout>
#include <globaldata.h>
#include <qcustomplot.h>
#include <QsLog/include/QsLog.h>
#include <QMessageBox>
#include <QTimer>
#include <cglobal.h>
#include <operclass/fullyautomatedplatelets.h>


CustomFixTableView::CustomFixTableView(QWidget *parent) :
    QWidget(parent), m_viewIDstr(""), m_viewIDNum(-1),m_sampleSex(""),
    m_showAACpgraph(nullptr),
    m_showADPCpgraph(nullptr),
    m_showEPICpgraph(nullptr),
    m_showCOLCpgraph(nullptr),
    m_showRISCpgraph(nullptr),
    m_maxAggregationTracer(nullptr),
    m_maxAggregationLabel(nullptr),
    m_aucCurve(nullptr),
    m_slopeLine(nullptr),
    m_slopeLabel(nullptr),
    m_slopeStartTracer(nullptr),
    m_slopeEndTracer(nullptr),
    m_lagTimeTracer(nullptr),
    m_lagTimeLine(nullptr),
    m_lagTimeLabel(nullptr),
    ui(new Ui::CustomFixTableView)
{
    ui->setupUi(this);
    setWindowTitle("预览测试结果");
    setWindowFlags(windowFlags() | Qt::Dialog | Qt::WindowStaysOnTopHint);


    //初始化动画对象
    m_showAnim = new QPropertyAnimation(this, "geometry");
    m_hideAnim = new QPropertyAnimation(this, "geometry");

    //设置动画曲线 (平滑加速减速)
    m_showAnim->setEasingCurve(QEasingCurve::OutBack);    // 弹跳效果增强视觉反馈
    m_hideAnim->setEasingCurve(QEasingCurve::InBack);

    //设置动画时长
    m_showAnim->setDuration(500);  // 500ms
    m_hideAnim->setDuration(400);  // 稍快隐藏

    //绑定隐藏动画结束信号
    connect(m_hideAnim, &QPropertyAnimation::finished, this, &CustomFixTableView::onHideFinished);

    //业务
    m_SampleidList.clear();

    initCreateCurveWidget(ui->widgetCurveShow);
    initShowResultWidget(ui->tableWidget);
    GlobalData::QCheckboxSheet(ui->checkBoxSmooth,tr("校平曲线"));
}

CustomFixTableView::~CustomFixTableView()
{
    delete ui;
}


// 滑入显示动画
void CustomFixTableView::showWithAnimation() {
    // 获取当前屏幕尺寸
    m_currentScreen = QApplication::screenAt(QCursor::pos());
    QRect screenRect = m_currentScreen->availableGeometry();

    // 计算位置
    const int width = 1450;  // 窗口宽度
    const int height = 550; // 窗口高度
    const int x = screenRect.x() + (screenRect.width() - width) / 2;


    //初始位置：屏幕底部外
    QRect startRect(x, screenRect.bottom(), width, height);
    // 目标位置：屏幕中央
    QRect endRect(x, screenRect.top() + (screenRect.height() - height) / 2, width, height);

    //设置动画参数
    m_showAnim->setStartValue(startRect);
    m_showAnim->setEndValue(endRect);

    //显示窗口并启动动画
    show();
    raise();
    activateWindow();
    m_showAnim->start();
}

// 滑出隐藏动画
void CustomFixTableView::hideWithAnimation() {
    // 动态获取当前屏幕，避免缓存错误
    QScreen* currentScreen = QApplication::screenAt(this->pos());
    if (!currentScreen) return; // 安全处理
    QRect screenRect = currentScreen->geometry();

    QRect currentRect = geometry();
    QRect endRect = currentRect;
    // 确保完全移出屏幕：顶部对齐屏幕底部，高度归零
    endRect.moveTop(screenRect.bottom() + 1);
    endRect.setHeight(0);

    m_hideAnim->setStartValue(currentRect);
    m_hideAnim->setEndValue(endRect);

    // 连接动画完成信号到隐藏槽
    connect(m_hideAnim, &QPropertyAnimation::finished, this, &CustomFixTableView::hide);
    m_hideAnim->start();
}

// 隐藏动画结束时关闭窗口
void CustomFixTableView::onHideFinished() {
    close();  // 彻底销毁窗口
}
void CustomFixTableView::showEvent(QShowEvent *e) {
    setAttribute(Qt::WA_Mapped); // 强制刷新界面
     QWidget::showEvent(e);     // 调用基类事件
}

// 重写关闭事件：直接关闭时触发隐藏动画
void CustomFixTableView::closeEvent(QCloseEvent *event) {
    if (m_hideAnim->state() != QAbstractAnimation::Running) {
        hideWithAnimation();
        event->ignore();  // 阻止立即关闭，等待动画完成
    } else {
        event->accept();
    }
}


//业务流程
void CustomFixTableView::setClickViewId(const QString& idstr,const int& idindex){
    m_viewIDstr = idstr;
    m_viewIDNum = idindex;
    ui->labelname->setText(QString("查看样本编号:%1测试结果与曲线").arg(m_viewIDstr));
}

void CustomFixTableView::setCheckBoxState(const bool& isstate,const QString& sampleSex,
                                           const QList<QString> &TableSampleidList){
    ui->checkBoxSmooth->blockSignals(true);
    ui->checkBoxSmooth->setChecked(isstate);
    ui->checkBoxSmooth->blockSignals(false);
    m_sampleSex = sampleSex;
    m_SampleidList.clear();
    m_SampleidList = TableSampleidList;
}


void CustomFixTableView::initCreateCurveWidget(QCustomPlot *customPlot){
    if (!customPlot) {
        QLOG_WARN() << "CustomPlot pointer is null!";
        return;
    }

    // 启用抗锯齿
    customPlot->setAntialiasedElements(QCP::aeAll);
    //customPlot->setNotAntialiasedElements(QCP::aeNone);

    // 2. 基本交互设置
    setupSmoothInteractions(customPlot);

    // 3. 坐标轴美化设置
    setupBeautifulAxes(customPlot);

    // 4. 网格样式美化
    setupElegantGrid(customPlot);

    // 5. 创建图例层
    initCreatCPGraph(customPlot);

    // 6. 添加阴影效果
    //addShadowEffect(customPlot);

    customPlot->replot();
    return;
}



void CustomFixTableView::setupSmoothInteractions(QCustomPlot* customPlot)
{
    customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    customPlot->setInteraction(QCP::iRangeDrag, true);
    customPlot->setInteraction(QCP::iRangeZoom, true);

    // 平滑滚动和拖拽
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // 设置现代化字体
    QFont modernFont("Segoe UI", 10);
    customPlot->setFont(modernFont);
}

void CustomFixTableView::setupBeautifulAxes(QCustomPlot* customPlot)
{
    // X轴美化设置
    setupModernAxis(customPlot->xAxis, "时间 (S)", 0, 300, 30, 10);

    // Y轴美化设置
    setupModernAxis(customPlot->yAxis, "百分比 (%)", -20, 100, 10, 10);

    //网格和零线设置
    QPen zeroLinePen;
    zeroLinePen.setColor(QColor(Qt::darkGreen));  // 修正：使用Qt::darkGreen
    zeroLinePen.setWidth(2);
    customPlot->xAxis->grid()->setZeroLinePen(zeroLinePen);
    customPlot->yAxis->grid()->setZeroLinePen(zeroLinePen);


}

void CustomFixTableView::setupModernAxis(QCPAxis* axis, const QString& label,
                                       double lower, double upper,
                                       double tickStep, int tickCount)
{
    // 标签设置
   axis->setLabel(label);
   axis->setLabelColor(Qt::black);

   // 范围设置
   axis->setRange(lower, upper);

   // 刻度设置
   QSharedPointer<QCPAxisTickerFixed> ticker(new QCPAxisTickerFixed);
   ticker->setTickStep(tickStep);
   ticker->setTickCount(tickCount);
   axis->setTicker(ticker);

   // 文本颜色
   axis->setTickLabelColor(QColor("#1C1C1C"));

   // 轴线样式
   QPen axisPen(Qt::black, 1, Qt::SolidLine);
   axis->setBasePen(axisPen);

   // 刻度线样式
   QPen tickPen(QColor("#1C1C1C"), 1, Qt::SolidLine);
   axis->setTickPen(tickPen);

   // 子刻度线样式
   QPen subTickPen(Qt::black, 1, Qt::SolidLine);
   axis->setSubTickPen(subTickPen);
}

void CustomFixTableView::setupElegantGrid(QCustomPlot* customPlot)
{
    // 网格线样式
    QPen gridPen(QColor(200, 200, 200), 1, Qt::DotLine);
    gridPen.setCosmetic(true);

    customPlot->xAxis->grid()->setPen(gridPen);
    customPlot->yAxis->grid()->setPen(gridPen);

    // 移除零线的特殊设置，使用统一的网格样式
    customPlot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    customPlot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
}

void CustomFixTableView::addShadowEffect(QCustomPlot* customPlot)
{
    // 为图表区域添加阴影效果
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(customPlot);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 60));
    shadowEffect->setOffset(3, 3);
    customPlot->setGraphicsEffect(shadowEffect);


    // 设置图表区域边距，确保刻度文字可见
    customPlot->axisRect()->setAutoMargins(QCP::msAll);
    customPlot->axisRect()->setMinimumMargins(QMargins(30, 20, 20, 30)); // 确保足够的边距

    // 设置背景颜色和圆角
    customPlot->axisRect()->setBackground(QBrush(Qt::white));
    customPlot->axisRect()->setBackgroundScaled(false);

    // 增大坐标轴标签字体
    QFont axisLabelFont("微软雅黑", 11, QFont::Medium);
    customPlot->xAxis->setLabelFont(axisLabelFont);
    customPlot->yAxis->setLabelFont(axisLabelFont);

    // 增大刻度文字字体
    QFont tickLabelFont("Segoe UI", 10);
    customPlot->xAxis->setTickLabelFont(tickLabelFont);
    customPlot->yAxis->setTickLabelFont(tickLabelFont);

    // 确保刻度文字颜色对比度足够
    customPlot->xAxis->setTickLabelColor(QColor(59, 73, 89));
    customPlot->yAxis->setTickLabelColor(QColor(59, 73, 89));

    // 调整坐标轴标签位置，确保不被遮挡
    customPlot->xAxis->setLabelPadding(10);
    customPlot->yAxis->setLabelPadding(15);

    // 调整刻度文字边距
    customPlot->xAxis->setTickLabelPadding(5);
    customPlot->yAxis->setTickLabelPadding(8);
}




void CustomFixTableView::initCreatCPGraph(QCustomPlot* pshowcurvedata)
{
    // 1. 使用结构体统一管理曲线配置
    struct CurveConfig {
        QCPGraph** graphPtr;  // 指向成员变量的指针
        quint8 reagent;
        QString name;
    };

    // 2. 集中管理所有曲线配置
    const QList<CurveConfig> curveConfigs = {
        { &m_showAACpgraph,  AA_REAGENT, "AA" },
        { &m_showADPCpgraph, ADP_REAGENT, "ADP" },
        { &m_showEPICpgraph, EPI_REAGENT, "EPI" },
        { &m_showCOLCpgraph, COL_REAGENT, "COL" },
        { &m_showRISCpgraph, RIS_REAGENT, "RIS" }
    };

    // 3. 统一创建和配置曲线
    for (const auto& config : curveConfigs) {
        *config.graphPtr = pshowcurvedata->addGraph();  // 创建曲线并赋值给成员变量

        QCPGraph* graph = *config.graphPtr;
        graph->setPen(QPen(GlobalData::customCurveColor(config.reagent), 2, Qt::SolidLine));
        graph->setName(config.name);
        graph->setLineStyle(QCPGraph::lsLine);
        graph->setAntialiased(true);  // 修正为正确API
    }

    // 4. 图例设置
    pshowcurvedata->legend->setVisible(false);
    pshowcurvedata->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);
    pshowcurvedata->legend->setBrush(QColor(255, 255, 255, 125));  // 半透明白色背景

    // 5. 一次性重绘
    pshowcurvedata->replot();
}


// 1. 将曲线列表定义为类成员（避免每次重建）
void CustomFixTableView::CreatResultCruve()
{
    // 清除辅助标记
    clearAuxiliaryItems();

    for (QCPGraph* graph : calibrationGraphs()) {
        clearGraphData(graph);
    }
    ui->widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
}
void CustomFixTableView::clrarResultTable(QTableWidget *pTable)
{
    // 清空结果列（从60S积聚率到AUC）
    for (int row = 0; row < pTable->rowCount(); ++row) {
        for (int col = static_cast<int>(TableItemnum::Aggregation60s);
             col <= static_cast<int>(TableItemnum::AUC); ++col) {
            insertColumnText(pTable, row, col, "");
        }
        // 同时清空对比结果列
        insertColumnText(pTable, row, static_cast<int>(TableItemnum::OutResult), "");
    }
}

// 2. 封装清除操作
void CustomFixTableView::clearGraphData(QCPGraph* graph)
{
    if (!graph) {
        QLOG_WARN() << "Attempted to clear null graph pointer";
        return;
    }

    if (!graph->data()) {
        QLOG_WARN() << "Graph data pointer is null";
        return;
    }

    // 更高效的数据清除方式
    graph->data()->clear();
    graph->data().data()->squeeze();
}

// 3. 获取曲线列表（静态或成员函数）
QList<QCPGraph*> CustomFixTableView::calibrationGraphs() const
{
    QList<QCPGraph*> validGraphs;

    // 只添加非空指针
    if (m_showAACpgraph) validGraphs.append(m_showAACpgraph);
    if (m_showADPCpgraph) validGraphs.append(m_showADPCpgraph);
    if (m_showEPICpgraph) validGraphs.append(m_showEPICpgraph);
    if (m_showCOLCpgraph) validGraphs.append(m_showCOLCpgraph);
    if (m_showRISCpgraph) validGraphs.append(m_showRISCpgraph);

    return validGraphs;
}






void CustomFixTableView::showCurveTestEnd(const quint8& testEndReagent, const bool  &smooth){
    // 1. 预生成X轴数据（0-299秒）
    static const QVector<double> timePoints = []{
        QVector<double> points;
        points.reserve(300);
        for(int i = 0; i < 300; i++) points.append(i);
        return points;
    }();

    // 2. 使用映射表替代switch-case
    static const QHash<quint8, QCPGraph*> reagentGraphMap = {
        {AA_REAGENT, m_showAACpgraph},
        {ADP_REAGENT, m_showADPCpgraph},
        {EPI_REAGENT, m_showEPICpgraph},
        {COL_REAGENT, m_showCOLCpgraph},
        {RIS_REAGENT, m_showRISCpgraph}
    };

    //获取试剂数据
    auto reagentCurvedata = FullyAutomatedPlatelets::pinstancesqlData()->getCurveData(
        m_viewIDstr, testEndReagent
    );

    // 检查数据有效性和长度匹配
    if (reagentCurvedata.size() != timePoints.size()) {
        QLOG_WARN() << "Curve data size mismatch for reagent:" << testEndReagent
                   << "Expected:" << timePoints.size() << "Got:" << reagentCurvedata.size();
        return;
    }

    // 检查数据是否为空
    if (reagentCurvedata.isEmpty()) {
        QLOG_WARN() << "Empty curve data for reagent:" << testEndReagent;
        return;
    }

    // 检查数据中是否包含无效值（无穷大或NaN）
    bool hasInvalidData = false;
    for (double val : reagentCurvedata) {
        if (std::isinf(val) || std::isnan(val)) {
            hasInvalidData = true;
            QLOG_WARN() << "Invalid value (inf/nan) found in curve data for reagent:"
                       << testEndReagent << ", value:" << val;
            break;
        }
    }

    if (hasInvalidData) {
        // 可以选择清除无效数据或直接返回
        QLOG_WARN() << "Skipping curve display due to invalid data for reagent:" << testEndReagent;
        //QMessageBox::warning(this,"显示无效","测试曲线数据异常NAN/INF");
        return;
    }

    // 检查是否所有数据都是NaN（原代码中的检查）
    if (std::all_of(reagentCurvedata.begin(), reagentCurvedata.end(),
                   [](double val) { return std::isnan(val); })) {
        QLOG_WARN() << "All data is NaN for reagent:" << testEndReagent;
        return;
    }

    // 清除之前的辅助标记
    clearAuxiliaryItems();

    m_currentCurveData = reagentCurvedata; // 保存当前曲线数据

    // ============== 新增：计算四个时间段的平均值 ==============
    double avg0to60 = 0.0;
    double avg60to180 = 0.0;
    double avg180to300 = 0.0;
    double avg0to300 = 0.0;

    // 计算0-300秒总平均值
    double sumAll = 0.0;
    for (int i = 0; i < reagentCurvedata.size(); i++) {
        sumAll += reagentCurvedata[i];
    }
    avg0to300 = reagentCurvedata.size() > 0 ? sumAll / reagentCurvedata.size() : 0.0;

    // 计算0-60秒平均值
    if (reagentCurvedata.size() >= 60) {
        double sum0to60 = 0.0;
        for (int i = 0; i < 60; i++) {
            sum0to60 += reagentCurvedata[i];
        }
        avg0to60 = sum0to60 / 60;
    }

    // 计算60-180秒平均值
    if (reagentCurvedata.size() >= 180) {
        double sum60to180 = 0.0;
        for (int i = 60; i < 180; i++) {
            sum60to180 += reagentCurvedata[i];
        }
        avg60to180 = sum60to180 / 120; // 60-180秒是120秒的区间
    }

    // 计算180-300秒平均值
    if (reagentCurvedata.size() >= 300) {
        double sum180to300 = 0.0;
        for (int i = 180; i < 300; i++) {
            sum180to300 += reagentCurvedata[i];
        }
        avg180to300 = sum180to300 / 120; // 180-300秒也是120秒的区间
    }

    QLOG_INFO() << QString("时间段平均值 - 0-60: %1, 60-180: %2, 180-300: %3, 0-300: %4")
        .arg(avg0to60, 0, 'f', 1)
        .arg(avg60to180, 0, 'f', 1)
        .arg(avg180to300, 0, 'f', 1)
        .arg(avg0to300, 0, 'f', 1);
    // ============== 新增结束 =============

    //设置曲线数据
    /*DataProcessor processor;
    TransitionParams params = processor.getOrGenerateParams(m_viewIDstr);
    QVector<double> adjustedData = processor.createControlledSinTransition(reagentCurvedata,
                                                                           params.sinLength,
                                                                           params.maxNegative);
    if (testEndReagent == COL_REAGENT)
    {
        reagentCurvedata = adjustedData;
    }*/


    if (auto graph = reagentGraphMap.value(testEndReagent, nullptr)) {

        if(!smooth)
            graph->setData(timePoints, reagentCurvedata);
        else
            graph->setData(timePoints, GlobalData::smoothData(reagentCurvedata, 5));

        // 标记最大聚集率、计算AUC和斜率
        auto maxAggregationResult = markMaxAggregation(reagentCurvedata, graph);
        double maxAggregation = maxAggregationResult.first;
        double timeToMax = maxAggregationResult.second; // 获取TMA时间

        double auc = calculateAndDrawAUC(reagentCurvedata, graph); //面积

        double slope = calculateAndDrawSlope(reagentCurvedata, graph);
        double lagTime = calculateLagTime(reagentCurvedata); // 计算延迟时间

        // 在图表上标记延迟时间
        markLagTimeOnGraph(lagTime, graph);

        // 直接使用我们计算的值显示在labelbrief中
        showCustomAnalyzerResult(maxAggregation, slope, auc, timeToMax, lagTime,
                                avg0to60, avg60to180, avg180to300, avg0to300);

        // 更新表格中的详细参数
        updateTableWithCalculatedParams(testEndReagent, slope, timeToMax, lagTime, auc);
    }

    // 更新表格结果
    updatetabletestedResult(testEndReagent);

}

void CustomFixTableView::showCustomAnalyzerResult(double maxAggregation, double slope, double auc,
    double timeToMax, double lagTime,
    double avg0to60, double avg60to180,
    double avg180to300, double avg0to300) {
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString brief = QString(
        "<div style='font-family: \"微软雅黑\", Arial, sans-serif; background-color: rgba(30, 40, 70, 0.85); padding: 15px; border: 2px solid #5d9cec; border-radius: 8px; color: #ffffff;'>"
        "<table style='width: 100%; border-collapse: collapse; font-size: 16px;'>"
        "<tr><td colspan='2' style='text-align: center; font-weight: bold; padding-bottom: 15px; font-size: 20px; color: #ffffff; background-color: rgba(93, 156, 236, 0.3); border-radius: 5px;'>🩸 血小板聚集分析报告</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='width: 45%; padding: 10px 2px; font-weight: bold; color: #dce4f2;'>📊 最大聚集率:</td>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #4fc3f7; font-size: 18px;'>%1%</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #dce4f2;'>🚀 最陡斜率:</td>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #69f0ae; font-size: 18px;'>%2 %/min</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #dce4f2;'>📐 AUC面积:</td>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #b388ff; font-size: 18px;'>%3 %·min</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #dce4f2;'>⏱️ TMA时间:</td>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #ffcc80; font-size: 18px;'>%4 min</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #dce4f2;'>⏳ 延迟时间:</td>"
        "<td style='padding: 10px 2px; font-weight: bold; color: #ff8a80; font-size: 18px;'>%5 min</td></tr>"
        "<tr><td colspan='2' style='padding-top: 15px; padding-bottom: 10px; text-align: center; font-weight: bold; border-top: 2px solid #5d9cec; font-size: 18px; color: #ffffff; background-color: rgba(93, 156, 236, 0.2); border-radius: 5px;'>📈 时间段平均聚集率</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #dce4f2;'>0-60秒:</td>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #80deea; font-size: 17px;'>%6%</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #dce4f2;'>60-180秒:</td>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #80deea; font-size: 17px;'>%7%</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #dce4f2;'>180-300秒:</td>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #80deea; font-size: 17px;'>%8%</td></tr>"
        "<tr style='border-bottom: 1px solid rgba(255, 255, 255, 0.2);'>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #dce4f2;'>0-300秒:</td>"
        "<td style='padding: 8px 2px; font-weight: bold; color: #80deea; font-size: 17px;'>%9%</td></tr>"
        "<tr><td colspan='2' style='padding-top: 15px; text-align: right; border-top: 1px solid #5d9cec; font-style: italic; color: #b0bec5;'>分析时间: %10</td></tr>"
        "</table>"
        "</div>"
        )
        .arg(QString::number(maxAggregation, 'f', 2))
        .arg(QString::number(slope, 'f', 3))
        .arg(QString::number(auc, 'f', 1))
        .arg(QString::number(timeToMax, 'f', 2))
        .arg(QString::number(lagTime, 'f', 2))
        .arg(QString::number(avg0to60, 'f', 1))
        .arg(QString::number(avg60to180, 'f', 1))
        .arg(QString::number(avg180to300, 'f', 1))
        .arg(QString::number(avg0to300, 'f', 1))
        .arg(currentTime);

    ui->labelbrief->setText(brief);
    ui->labelbrief->setTextFormat(Qt::RichText);
}



void CustomFixTableView::showResult(const bool& smooth){
    CreatResultCruve();
    clrarResultTable(ui->tableWidget);

    static const QMap<quint8, QString> ViewCurveData = {
        {AA_REAGENT,   "AA"},
        {ADP_REAGENT,  "ADP"},
        {EPI_REAGENT,  "EPI"},
        {COL_REAGENT,  "COL"},
        {RIS_REAGENT,  "RIS"}
    };

    for (auto it = ViewCurveData.constBegin(); it != ViewCurveData.constEnd(); ++it) {
        showCurveTestEnd(it.key(),smooth);
        updatetabletestedResult(it.key());
    }

    // 统一重绘
    ui->widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);

    if(!smooth){
        //参考值
        bool  issetSex = updateSetSexReferValue();
        Analyzeresultingvalues(issetSex);
    }

}


void CustomFixTableView::insertColumnText(QTableWidget *tablewiget,
                                          const int row,
                                          const int col,
                                          const QColor &bgmC,
                                          const QString& text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    item->setBackgroundColor(bgmC);
    QFont font("楷体",12);
    tablewiget->setFont(font);
    tablewiget->setItem(row,col,item);
}

void CustomFixTableView::insertColumnText(QTableWidget *tablewiget,
                                          const int row,
                                          const int col,
                                          const QString& text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    QFont font("楷体",12);
    tablewiget->setFont(font);
    tablewiget->setItem(row,col,item);
}

void CustomFixTableView::updateParaState(QTableWidget *tablewiget,const int row,const int col,
                                         const QColor &bgmC,
                                         const QString& iconPath,const QString& text){

    QWidget *containerWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(containerWidget);
    QLabel *imageLabel = new QLabel();
    imageLabel->setPixmap(QPixmap(iconPath));  // 设置图片路径
    imageLabel->setFixedSize(QSize(32,32));
    QLabel *textLabel = new QLabel(text);
    QFont font("楷体",12);
    textLabel->setFont(font);
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    containerWidget->setLayout(layout);
    tablewiget->setCellWidget(row, col, containerWidget);
}

void  CustomFixTableView::initShowResultWidget(QTableWidget * Table)
{
    Table->setContextMenuPolicy (Qt::CustomContextMenu);
    QStringList header{
        tr("测试项目"), tr("状态"), tr("60S积聚率"), tr("180S积聚率"),
        tr("300S积聚率"), tr("Max积聚率"), tr("斜率(Slope/%/min)"),
        tr("AMT时间(TMA/min)"), tr("延迟时间(Time/min)"),
        tr("AUC面积(%*min)"), tr("对比结果"), tr("参考值")
    };
    Table->setColumnCount(header.size());
    Table->setHorizontalHeaderLabels(header);

    QFont font;
    font.setFamily("楷体");
    font.setPixelSize(14);
    font.setBold(true);

    Table->horizontalHeader()->setFont(font);
    Table->horizontalHeader()->setStretchLastSection(true); // 最后一列填充
    Table->verticalHeader()->setVisible(false);

    Table->verticalHeader()->setDefaultSectionSize(50);
    Table->verticalHeader()->setMinimumSectionSize(40); // 设置最小行高

    Table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Table->horizontalHeader()->setFixedHeight(40);
    Table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    Table->setIconSize(QSize(32, 32));

    // 设置列宽 - 根据文字内容设置合适的宽度
    Table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    Table->setColumnWidth(0, 80);   // 测试项目

    Table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    Table->setColumnWidth(1, 110);   // 状态

    Table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    Table->setColumnWidth(2, 100);   // 60S积聚率

    Table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    Table->setColumnWidth(3, 100);   // 180S积聚率

    Table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    Table->setColumnWidth(4, 100);   // 300S积聚率

    Table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    Table->setColumnWidth(5, 100);   // Max积聚率

    Table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    Table->setColumnWidth(6, 180);   // 聚集斜率

    Table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    Table->setColumnWidth(7, 160);  // AMT时间(TMA/min)

    Table->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);
    Table->setColumnWidth(8, 160);  // 延迟时间(Time/min)

    Table->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Fixed);
    Table->setColumnWidth(9, 150);  // AUC面积AUC(%*min)

    Table->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Fixed);
    Table->setColumnWidth(10, 100); // 对比结果

    // 最后一列（参考值）填充剩余空间
    Table->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Stretch);

    Table->setStyleSheet(TableWidgetCss +
        "QTableWidget::item {"
        "   padding: 5px;"  // 增加单元格内边距
        "}");

    Table->horizontalHeader()->setStyleSheet("QHeaderView::section {"
                                             "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                                             "                               stop:0 #6c6c6c, stop:1 #4a4a4a);"
                                             "   color: white;"
                                             "   padding: 8px 2px;"
                                             "   border: 1px solid #3a3a3a;"
                                             "   font-weight: bold;"
                                             "   font-size: 12pt;"
                                             "}"
                                             "QHeaderView::section:first {"
                                             "   border-left: 1px solid #3a3a3a;"
                                             "}"
                                             "QHeaderView::section:last {"
                                             "   border-right: 1px solid #3a3a3a;"
                                             "}");

    QStringList projectitems{tr("AA"), tr("ADP"), tr("EPI"), tr("COL"), tr("RIS")};
    Table->setRowCount(projectitems.size());

    // 设置每行的项目名称和背景色
    for (int row = 0; row < projectitems.size(); ++row) {
        insertColumnText(Table, row, static_cast<int>(TableItemnum::ReagentName),
                         GlobalData::customCurveColor(row + 1), projectitems.at(row));

        // 初始化其他列为空
        for (int col = 1; col < Table->columnCount(); ++col) {
            insertColumnText(Table, row, col, "");
        }
    }

    connect(Table, SIGNAL(cellClicked(int, int)), this, SLOT(viewOneReagentCurve(int, int)));
    return;
}


void CustomFixTableView::spanTableWidget(const int &fromrows ,const int& indexCols,
                                         QString pathicon,QString outResult){
    ui->tableWidget->setSpan(fromrows,  indexCols, 4,  1);
    updateParaState(ui->tableWidget, fromrows,indexCols,QColor(250,250,250),pathicon,outResult);
}


void CustomFixTableView::viewOneReagentCurve(int rows, int cols){
    if(rows < 0 || rows >= ui->tableWidget->rowCount()) return;

    // 先清除测试数据曲线再画
    CreatResultCruve();

    // 定义试剂类型映射表
    static const std::array<quint8, 5> REAGENT_MAP = {
        AA_REAGENT,   // 0
        ADP_REAGENT,  // 1
        EPI_REAGENT,  // 2
        COL_REAGENT,  // 3
        RIS_REAGENT   // 4
    };

    const quint8 indexReag = REAGENT_MAP[rows];
    showCurveTestEnd(indexReag, false);
    ui->widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
    return;
}

void CustomFixTableView::updatetabletestedResult(const quint8& indexReag)
{
    // 定义试剂类型与行索引映射
    static const QMap<quint8, int> REAGENT_ROW_MAP = {
        { AA_REAGENT,  0 },
        { ADP_REAGENT, 1 },
        { EPI_REAGENT, 2 },
        { COL_REAGENT, 3 },
        { RIS_REAGENT, 4 }
    };

    // 定义试剂类型与结果成员映射
    static const QMap<quint8, QString PatientInformationStu::*> REAGENT_MEMBER_MAP = {
        { AA_REAGENT,  &PatientInformationStu::AAResult },
        { ADP_REAGENT, &PatientInformationStu::ADPResult },
        { EPI_REAGENT, &PatientInformationStu::EPIResult },
        { COL_REAGENT, &PatientInformationStu::COLResult },
        { RIS_REAGENT, &PatientInformationStu::RISResult }
    };

    // 获取测试结果
    PatientInformationStu painterInfovec;
    FullyAutomatedPlatelets::pinstancesqlData()->getTestResultTabledata(m_viewIDstr, painterInfovec);

    auto rowIt = REAGENT_ROW_MAP.find(indexReag);
    auto memberIt = REAGENT_MEMBER_MAP.find(indexReag);
    if (rowIt == REAGENT_ROW_MAP.end() || memberIt == REAGENT_MEMBER_MAP.end()) return;

    int row = rowIt.value();
    QString PatientInformationStu::* memberPtr = memberIt.value();

    // 使用成员指针访问数据
    QString testResultVal = painterInfovec.*memberPtr;
    QStringList resultList = testResultVal.simplified().split(",");
    bool testfinish = (resultList.size() == 4);

    const int reagentStateCol = static_cast<int>(TableItemnum::ReagentState);

    if (testfinish) {
        // 填充四个时间点的聚集率
        insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::Aggregation60s), resultList.at(0));
        insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::Aggregation180s), resultList.at(1));
        insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::Aggregation300s), resultList.at(2));
        insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::AggregationMax), resultList.at(3));

        setReagentStatus(row, reagentStateCol, Status::Completed);
    }
    else if (testResultVal.isEmpty()) {
        setReagentStatus(row, reagentStateCol, Status::NotTested);
    }
    else if (testResultVal == "null") {
        setReagentStatus(row, reagentStateCol, Status::Pending);
    }
    ui->tableWidget->viewport()->update();
}


void CustomFixTableView::setReagentStatus(int row, int col, Status status) {
    QString iconPath, statusText;
    switch(status) {
        case Status::Completed:
            iconPath = ":/Picture/SetPng/status_Normal.png";
            statusText = tr("完成");
            break;
        case Status::NotTested:
            iconPath = ":/Picture/SetPng/status_Error.png";
            statusText = tr("未测试");
            break;
        case Status::Pending:
            iconPath = ":/Picture/SetPng/status_Action.png";
            statusText = tr("等待测试");
            break;
    }

    // 更新状态单元格
    updateParaState(ui->tableWidget, row, col, QColor(250,250,250), iconPath, statusText);
}


bool CustomFixTableView::updateSetSexReferValue()
{
    const QString sexMan = "男";
    bool isfindSex = !(m_sampleSex.isEmpty() || m_sampleSex.isNull());
    bool isSex = (m_sampleSex == sexMan);


    QStringList keyRefence{
        tr("AA聚集率-Max"),  // 只使用Max值作为参考
        tr("ADP聚集率-Max"),
        tr("EPI聚集率-Max"),
        tr("COL聚集率-Max"),
        tr("RIS聚集率-Max")
    };

    for (int row = 0; row < keyRefence.size(); ++row) {
        if (!isfindSex) {
            insertColumnText(ui->tableWidget, row,
                            static_cast<int>(TableItemnum::ReferenceValue), "未配置性别");
        } else {
            QString mandata, womandata;
            FullyAutomatedPlatelets::pinstancesqlData()->_obtainPersondata_(
                keyRefence.at(row), mandata, womandata);
            if (isSex) {
                insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::ReferenceValue), mandata);
            } else {
                insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::ReferenceValue), womandata);
            }
        }
    }
    return isfindSex;
}

void CustomFixTableView::Analyzeresultingvalues(const bool &alreadysetSex)
{
    QTableWidget *presultTable = ui->tableWidget;
    int totalRows = presultTable->rowCount();
    const int colsCompare = static_cast<int>(TableItemnum::OutResult);
    const int colreferne = static_cast<int>(TableItemnum::ReferenceValue);

    if(!alreadysetSex){
        for (int r = 0; r < totalRows; ++r){
            insertColumnText(presultTable, r, colsCompare, "/");
        }
        return;
    }

    for (int r = 0; r < totalRows; ++r) {
        // 使用Max聚集率进行比较（第5列）
        QTableWidgetItem* resultItem = presultTable->item(r, static_cast<int>(TableItemnum::AggregationMax));
        if (!resultItem || resultItem->text().isEmpty()) {
            insertColumnText(presultTable, r, colsCompare, "/");
            continue;
        }

        // 数值解析优化
        bool convertOk = false;
        QString datastr = resultItem->text();
        QUIUtils::parseDataratio(datastr);
        const double resultval = datastr.toDouble(&convertOk);
        if (!convertOk) {
            insertColumnText(presultTable, r, colsCompare, "?");
            continue;
        }

        // 参考值处理
        QTableWidgetItem* refItem = presultTable->item(r, colreferne);
        if (!refItem || refItem->text() == "未配置性别") continue;

        const QString refData = refItem->text();
        const QStringList parts = refData.split('(');
        if (parts.size() != 2) {
            QLOG_DEBUG() << "参考值格式错误 (括号缺失)";
            continue;
        }
        const QStringList range = parts[0].split('-');
        if (range.size() != 2) {
            QLOG_DEBUG() << "参考值范围格式错误";
            continue;
        }

        // 数值比较
        bool lowOk, highOk;
        const double lowBound = range[0].toDouble(&lowOk);
        const double highBound = range[1].toDouble(&highOk);
        if (!lowOk || !highOk) {
            QLOG_DEBUG() << "参考值非数值类型";
            continue;
        }

        // 结果判定
        if (resultval < lowBound) {
            insertColumnText(presultTable, r, colsCompare, "↓偏低");
        } else if (resultval > highBound) {
            insertColumnText(presultTable, r, colsCompare, "↑偏高");
        } else {
            insertColumnText(presultTable, r, colsCompare, "↕正常");
        }
    }
}

void CustomFixTableView::on_checkBoxSmooth_clicked()
{
    showResult(ui->checkBoxSmooth->isChecked());
}

//上一项
void CustomFixTableView::on_pushButtonBack_clicked()
{
    int rows =  m_viewIDNum - 1;
    QString  todayLast = GlobalData::ObatinCreatSampleTime();
    QString srtId = GlobalData::groupDateAndID(todayLast,rows);

    if(m_SampleidList.contains(srtId)){
        viewOneSelf(srtId,rows);
    }else{
        QMessageBox::warning(this,tr("查看失败"),tr("已是最小样本号!"));
        return;
    }
}




// 最简单的使用示例
void simplePrintExample()
{
    HospitalReportPrinter printer;

    // 1. 设置患者基本信息
    PatientBasicInfo patientInfo;
    patientInfo.patientId = "20240001";
    patientInfo.name = "张三";
    patientInfo.gender = "男";
    patientInfo.age = 45;
    patientInfo.department = "心血管内科";
    patientInfo.bedNumber = "305";
    patientInfo.testTime = QDateTime::currentDateTime();
    patientInfo.diagnosis = "冠心病，待排心肌梗死";

    printer.setPatientInfo(patientInfo);

    // 2. 设置检测结果
    QVector<TestResult> testResults;
    testResults.append({"ADP诱导聚集率", 65.5, "%", "50-80%"});
    testResults.append({"胶原诱导聚集率", 72.3, "%", "60-85%"});
    testResults.append({"肾上腺素诱导", 58.7, "%", "45-75%"});
    testResults.append({"花生四烯酸诱导", 81.2, "%", "70-90%"});
    testResults.append({"最大聚集率", 75.8, "%", "60-85%"});

    printer.setTestResults(testResults);

    // 3. 设置曲线图（如果有的话）
    QVector<CurveData> curves;
    CurveData curve1;
    //curve1.curveImage = QPixmap(":/Picture/media_playback_start.png"); // 您的曲线图
    curve1.curveTitle = "ADP诱导聚集曲线";
    curve1.analysis = "聚集功能正常，延迟时间2.5分钟，最大聚集率65.5%";
    curves.append(curve1);

    CurveData curve2;
    curve2.curveImage = QPixmap(":/Picture/calibration.png");
    curve2.curveTitle = "胶原诱导聚集曲线";
    curve2.analysis = "聚集功能良好，延迟时间1.8分钟，最大聚集率72.3%";
    curves.append(curve2);

    printer.setCurveData(curves);

    // 4. 打印报告
    if (printer.printReport("血小板聚集功能检测报告")) {
        qDebug() << "打印成功！";
    } else {
        qDebug() << "打印失败！";
    }
}


//下一项
void CustomFixTableView::on_pushButtonNext_clicked()
{
    //simplePrintExample();
    int rows =  m_viewIDNum + 1;
    QString  todayLast = GlobalData::ObatinCreatSampleTime();
    QString srtId = GlobalData::groupDateAndID(todayLast,rows);

    if(m_SampleidList.contains(srtId)){
        viewOneSelf(srtId,rows);
    }else{
        QMessageBox::warning(this,tr("查看失败"),tr("已是最大样本号!"));
        return;
    }
}


void CustomFixTableView::viewOneSelf(const QString &idstr,const int &idinter){
    ui->checkBoxSmooth->blockSignals(true);
    ui->checkBoxSmooth->setChecked(false);
    ui->checkBoxSmooth->blockSignals(false);

    setClickViewId(idstr,idinter);

    showResult(false);
}




QPair<double, double> CustomFixTableView::markMaxAggregation(const QVector<double>& data, QCPGraph* graph)
{
    if (!graph || data.isEmpty()) return qMakePair(0.0, 0.0);

    // 找到最大聚集率及其对应的时间点
    double maxAggregation = -std::numeric_limits<double>::max();
    int maxIndex = -1;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > maxAggregation && !std::isnan(data[i])) {
            maxAggregation = data[i];
            maxIndex = i;
        }
    }

    if (maxIndex == -1) return qMakePair(0.0, 0.0);

    // 计算TMA时间（秒转换为分钟）
    double timeToMax = maxIndex / 60.0;

    // 创建标记点
    m_maxAggregationTracer = new QCPItemTracer(ui->widgetCurveShow);
    m_maxAggregationTracer->setGraph(graph);
    m_maxAggregationTracer->setGraphKey(maxIndex);
    m_maxAggregationTracer->setInterpolating(false);
    m_maxAggregationTracer->setStyle(QCPItemTracer::tsCircle);
    m_maxAggregationTracer->setPen(QPen(Qt::red, 2));
    m_maxAggregationTracer->setBrush(QBrush(Qt::red));
    m_maxAggregationTracer->setSize(8);
    return qMakePair(maxAggregation, timeToMax);
}

double CustomFixTableView::calculateAndDrawAUC(const QVector<double>& data, QCPGraph* graph)
{
    if (!graph || data.isEmpty()) return 0.0;

    // 计算梯形面积 (AUC) - 单位 %·s
    double auc_per_second = 0.0;
    QVector<double> xData, yData;

    for (int i = 1; i < data.size(); ++i) {
        if (!std::isnan(data[i]) && !std::isnan(data[i-1])) {
            // 梯形面积公式: (上底 + 下底) * 高 / 2
            double trapezoidArea = (data[i] + data[i-1]) * 1.0 / 2.0; // 时间间隔为1秒
            auc_per_second += trapezoidArea;

            // 收集用于填充的数据点
            xData << (i-1) << i;
            yData << data[i-1] << data[i];
        }
    }

    // 转换为 %·min
    double auc_per_minute = auc_per_second / 60.0;

    // 创建AUC填充区域
    QCPGraph* aucFillGraph = ui->widgetCurveShow->addGraph();
    aucFillGraph->setData(xData, yData);
    aucFillGraph->setPen(Qt::NoPen);
    aucFillGraph->setBrush(QColor(255, 0, 0, 50)); // 半透明红色填充

    m_aucFillGraphs.append(aucFillGraph);
    return auc_per_minute; // 返回 %·min 的值
}

void CustomFixTableView::clearAuxiliaryItems()
{
    // 清除最大聚集率标记
    if (m_maxAggregationTracer) {
        ui->widgetCurveShow->removeItem(m_maxAggregationTracer);
        m_maxAggregationTracer = nullptr;
    }
    if (m_maxAggregationLabel) {
        ui->widgetCurveShow->removeItem(m_maxAggregationLabel);
        m_maxAggregationLabel = nullptr;
    }



    // 清除AUC填充区域
    for (QCPGraph* graph : m_aucFillGraphs) {
        ui->widgetCurveShow->removeGraph(graph);
    }
    m_aucFillGraphs.clear();

    // 清除斜率相关标记
    if (m_slopeLine) {
        ui->widgetCurveShow->removeItem(m_slopeLine);
        m_slopeLine = nullptr;
    }
    if (m_slopeLabel) {
        ui->widgetCurveShow->removeItem(m_slopeLabel);
        m_slopeLabel = nullptr;
    }
    if (m_slopeStartTracer) {
        ui->widgetCurveShow->removeItem(m_slopeStartTracer);
        m_slopeStartTracer = nullptr;
    }
    if (m_slopeEndTracer) {
        ui->widgetCurveShow->removeItem(m_slopeEndTracer);
        m_slopeEndTracer = nullptr;
    }
    // 清除延迟时间标记
    if (m_lagTimeTracer) {
        ui->widgetCurveShow->removeItem(m_lagTimeTracer);
        m_lagTimeTracer = nullptr;
    }
    if (m_lagTimeLine) {
        ui->widgetCurveShow->removeItem(m_lagTimeLine);
        m_lagTimeLine = nullptr;
    }
    if (m_lagTimeLabel) {
        ui->widgetCurveShow->removeItem(m_lagTimeLabel);
        m_lagTimeLabel = nullptr;
    }

    // 清理新增的交互式对象
    if (m_verticalLine) {
        ui->widgetCurveShow->removeItem(m_verticalLine);
        m_verticalLine = nullptr;
    }
    if (m_dataPointTracer) {
        ui->widgetCurveShow->removeItem(m_dataPointTracer);
        m_dataPointTracer = nullptr;
    }
    if (m_dataPointLabel) {
        ui->widgetCurveShow->removeItem(m_dataPointLabel);
        m_dataPointLabel = nullptr;
    }
}

QPair<int, int> CustomFixTableView::findSteepestSegment(const QVector<double>& data, int windowSize)
{
    if (data.size() < windowSize + 1) {
        return qMakePair(0, data.size() - 1);
    }

    double maxSlope = -std::numeric_limits<double>::max();
    int bestStart = 0;
    int bestEnd = windowSize;

    // 滑动窗口寻找最陡峭的线段
    for (int i = 0; i <= data.size() - windowSize - 1; ++i) {
        int start = i;
        int end = i + windowSize;

        // 检查数据有效性
        bool validSegment = true;
        for (int j = start; j <= end; ++j) {
            if (std::isnan(data[j]) || std::isinf(data[j])) {
                validSegment = false;
                break;
            }
        }

        if (!validSegment) continue;

        double slope = (data[end] - data[start]) / (end - start);

        if (slope > maxSlope) {
            maxSlope = slope;
            bestStart = start;
            bestEnd = end;
        }
    }

    return qMakePair(bestStart, bestEnd);
}

double CustomFixTableView::calculateSlope(const QVector<double>& data, int start, int end)
{
    if (start >= end || start < 0 || end >= data.size()) {
        return 0.0;
    }

    // 使用线性回归计算更精确的斜率
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    int n = end - start + 1;

    for (int i = start; i <= end; ++i) {
        double x = i - start;  // 相对时间
        double y = data[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }

    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0) return 0.0;

    return (n * sumXY - sumX * sumY) / denominator;
}

double CustomFixTableView::calculateAndDrawSlope(const QVector<double>& data, QCPGraph* graph)
{
    if (!graph || data.size() < 20) return 0;  // 需要足够的数据点

    // 找到最陡峭的线段
    auto segment = findSteepestSegment(data, 15);  // 15个点的窗口
    int start = segment.first;
    int end = segment.second;

    if (start >= end) return 0;

    // 计算斜率（每秒的斜率，单位：%/s）
    double slopePerSecond = calculateSlope(data, start, end);

    // 转换为每分钟的斜率（单位：%/min）
    double slopePerMinute = slopePerSecond * 60.0;

    double intercept = data[start] - slopePerSecond * start;  // 计算截距

    // 创建斜率线 (延长显示)
    int extendedStart = qMax(0, start - 5);
    int extendedEnd = qMin(data.size() - 1, end + 5);

    double y1 = slopePerSecond * extendedStart + intercept;
    double y2 = slopePerSecond * extendedEnd + intercept;

    // 确保斜率线在合理范围内
    y1 = qMax(-20.0, qMin(100.0, y1));
    y2 = qMax(-20.0, qMin(100.0, y2));

    m_slopeLine = new QCPItemStraightLine(ui->widgetCurveShow);
    m_slopeLine->point1->setCoords(extendedStart, y1);
    m_slopeLine->point2->setCoords(extendedEnd, y2);
    m_slopeLine->setPen(QPen(QColor(0, 150, 0), 2, Qt::DashLine));  // 绿色虚线

    // 创建起点标记
    m_slopeStartTracer = new QCPItemTracer(ui->widgetCurveShow);
    m_slopeStartTracer->position->setCoords(start, data[start]);
    m_slopeStartTracer->setStyle(QCPItemTracer::tsSquare);
    m_slopeStartTracer->setPen(QPen(Qt::darkGreen, 2));
    m_slopeStartTracer->setBrush(QBrush(Qt::green));
    m_slopeStartTracer->setSize(6);

    // 创建终点标记
    m_slopeEndTracer = new QCPItemTracer(ui->widgetCurveShow);
    m_slopeEndTracer->position->setCoords(end, data[end]);
    m_slopeEndTracer->setStyle(QCPItemTracer::tsSquare);
    m_slopeEndTracer->setPen(QPen(Qt::darkGreen, 2));
    m_slopeEndTracer->setBrush(QBrush(Qt::green));
    m_slopeEndTracer->setSize(6);

    // 创建斜率标签 - 显示每分钟的斜率
    m_slopeLabel = new QCPItemText(ui->widgetCurveShow);
    m_slopeLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    m_slopeLabel->position->setCoords(0.85, 0.15);
    m_slopeLabel->setText(QString("斜率: %1 %/min\n时间段: %2-%3s")
                         .arg(slopePerMinute, 0, 'f', 3)
                         .arg(start)
                         .arg(end));
    m_slopeLabel->setFont(QFont("微软雅黑", 9, QFont::Normal));
    m_slopeLabel->setPen(QPen(Qt::darkGreen));
    m_slopeLabel->setBrush(QBrush(QColor(255, 255, 255, 200)));
    m_slopeLabel->setPadding(QMargins(8, 4, 8, 4));
    m_slopeLabel->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);

    // ============== 新增：创建交互式垂直线 ==============
    // 创建垂直线
    m_verticalLine = new QCPItemStraightLine(ui->widgetCurveShow);
    m_verticalLine->setPen(QPen(QColor(0, 0, 255), 1, Qt::DashLine)); // 蓝色虚线

    // 创建数据点追踪器（显示聚集率）
    m_dataPointTracer = new QCPItemTracer(ui->widgetCurveShow);
    m_dataPointTracer->setStyle(QCPItemTracer::tsCircle);
    m_dataPointTracer->setPen(QPen(Qt::red, 2));
    m_dataPointTracer->setBrush(QBrush(Qt::yellow));
    m_dataPointTracer->setSize(6);

    // 创建数据标签
    m_dataPointLabel = new QCPItemText(ui->widgetCurveShow);
    m_dataPointLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_dataPointLabel->setFont(QFont("微软雅黑", 8));
    m_dataPointLabel->setPen(QPen(Qt::blue));
    m_dataPointLabel->setBrush(QBrush(QColor(255, 255, 255, 200)));
    m_dataPointLabel->setPadding(QMargins(5, 3, 5, 3));

    // 设置标签位置为追踪器上方
    m_dataPointLabel->position->setParentAnchor(m_dataPointTracer->position);
    m_dataPointLabel->position->setCoords(5, -15); // 在追踪器上方15像素

    // 初始隐藏，等待鼠标移动
    m_verticalLine->setVisible(false);
    m_dataPointTracer->setVisible(false);
    m_dataPointLabel->setVisible(false);

    // 启用鼠标追踪
    ui->widgetCurveShow->setMouseTracking(true);
    //ui->widgetCurveShow->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 连接鼠标移动信号
    connect(ui->widgetCurveShow, &QCustomPlot::mouseMove, this, &CustomFixTableView::onCurveMouseMove);
    // ============== 新增结束 ==============


    // 在分析结果中也显示斜率信息
    QLOG_INFO() << "最陡峭斜率:" << slopePerMinute << "%/min"
                << "(每秒斜率:" << slopePerSecond << "%/s)"
                << "时间段:" << start << "-" << end << "秒";

    return slopePerMinute;  // 返回每分钟的斜率
}


double CustomFixTableView::calculateLagTime(const QVector<double>& data)
{
    if (data.isEmpty()) return 0.0;

    auto &ini = INI_File();
    bool isWholeBlood = ini.GetWholeBloodModel();

    // 参数配置 - 根据样本类型调整
    const double baselineThreshold = isWholeBlood ? 3.0 : 2.0;
    const int minRiseDuration = isWholeBlood ? 5 : 4;
    const double riseThreshold = isWholeBlood ? 1.0 : 0.5;
    const int minDataPoints = 10; // 最小有效数据点数
    const double maxDropReset = isWholeBlood ? 2.0 : 1.0; // 允许的最大下降幅度

    // 数据预处理：检查数据有效性
    int validCount = 0;
    for (const auto& value : data) {
        if (!std::isnan(value)) validCount++;
    }
    if (validCount < minDataPoints) return 0.0;

    bool isRising = false;
    int riseStartIndex = -1;
    int consecutiveRiseCount = 0;
    double previousPeak = 0.0;

    for (int i = 1; i < data.size(); ++i) {
        if (std::isnan(data[i]) || std::isnan(data[i-1])) continue;

        double currentValue = data[i];
        double previousValue = data[i-1];
        double difference = currentValue - previousValue;

        // 检查基线阈值条件（仅在未开始上升时检查）
        if (!isRising && currentValue > baselineThreshold) {
            return i / 60.0;
        }

        // 上升检测逻辑
        if (!isRising) {
            if (difference > riseThreshold) {
                // 开始新的上升段
                isRising = true;
                riseStartIndex = i - 1;
                consecutiveRiseCount = 1;
                previousPeak = currentValue;
            }
        } else {
            // 已经在上升段中
            if (difference > 0) {
                // 继续上升
                consecutiveRiseCount++;
                previousPeak = currentValue;
            } else {
                // 处理下降情况
                double dropAmount = previousPeak - currentValue;

                // 如果下降幅度不大，不立即重置，允许小幅回调
                if (dropAmount <= maxDropReset && consecutiveRiseCount > 0) {
                    consecutiveRiseCount++; // 继续计数，但注意不是上升
                    // 小幅下降不重置，但重新设置peak
                    if (currentValue > previousPeak) {
                        previousPeak = currentValue;
                    }
                } else {
                    // 大幅下降，重置上升检测
                    isRising = false;
                    consecutiveRiseCount = 0;
                    riseStartIndex = -1;
                }
            }
        }

        // 检查是否满足持续上升条件
        if (consecutiveRiseCount >= minRiseDuration) {
            // 验证上升幅度是否显著
            double totalRise = data[i] - data[riseStartIndex];
            double minTotalRise = isWholeBlood ? 3.0 : 2.0; // 最小总上升幅度

            if (totalRise >= minTotalRise) {
                return riseStartIndex / 60.0;
            } else {
                // 上升幅度不足，继续监测
                isRising = false;
                consecutiveRiseCount = 0;
                riseStartIndex = -1;
            }
        }

        // 提前终止条件：如果已经检测到足够长的数据段但仍未找到lag time
        // 可以避免处理异常数据时的时间浪费
        if (i > 120) { // 2分钟后的数据
            double recentMax = *std::max_element(data.begin() + i - 10, data.begin() + i);
            if (recentMax < baselineThreshold / 2) {
                // 长时间低值，可能没有聚集发生
                return 0.0;
            }
        }
    }

    // 最终检查：如果数据末尾有明显上升趋势但未达到持续条件
    if (riseStartIndex != -1 && consecutiveRiseCount >= minRiseDuration / 2) {
        double finalRise = data.back() - data[riseStartIndex];
        if (finalRise > baselineThreshold) {
            return riseStartIndex / 60.0;
        }
    }

    return 0.0;
}



void CustomFixTableView::markLagTimeOnGraph(double lagTime, QCPGraph* graph)
{
    if (!graph || lagTime <= 0) return;

    // 将分钟转换为数据点索引（秒）
    double lagTimeSeconds = lagTime * 60;

    // 获取延迟时间点的Y坐标值
    double lagYValue = 0;
    bool foundValue = false;

    // 在曲线数据中查找最接近的Y值
    if (graph->data()->size() > 0) {
        int closestIndex = 0;
        double minDiff = std::numeric_limits<double>::max();

        for (int i = 0; i < graph->data()->size(); ++i) {
            double timeDiff = std::abs(graph->data()->at(i)->key - lagTimeSeconds);
            if (timeDiff < minDiff) {
                minDiff = timeDiff;
                closestIndex = i;
                lagYValue = graph->data()->at(i)->value;
                foundValue = true;
            }
        }
    }

    // 创建延迟时间标记点
    m_lagTimeTracer = new QCPItemTracer(ui->widgetCurveShow);
    m_lagTimeTracer->position->setCoords(lagTimeSeconds, lagYValue);
    m_lagTimeTracer->setStyle(QCPItemTracer::tsSquare);
    m_lagTimeTracer->setPen(QPen(Qt::blue, 3));
    m_lagTimeTracer->setBrush(QBrush(QColor(0, 150, 255)));
    m_lagTimeTracer->setSize(8);

    // 创建垂直线标记延迟时间点
    m_lagTimeLine = new QCPItemStraightLine(ui->widgetCurveShow);
    m_lagTimeLine->point1->setCoords(lagTimeSeconds, -20);
    m_lagTimeLine->point2->setCoords(lagTimeSeconds, foundValue ? lagYValue : 100);
    m_lagTimeLine->setPen(QPen(QColor(0, 100, 200, 150), 2, Qt::DashLine));

    // 创建延迟时间标签
    m_lagTimeLabel = new QCPItemText(ui->widgetCurveShow);
    m_lagTimeLabel->position->setType(QCPItemPosition::ptAxisRectRatio);
    m_lagTimeLabel->position->setCoords(0.85, 0.35); // 放在右上角，在斜率标签下方
    m_lagTimeLabel->setText(QString("延迟时间\n%1 min").arg(lagTime, 0, 'f', 2));
    m_lagTimeLabel->setFont(QFont("微软雅黑", 9, QFont::Normal));
    m_lagTimeLabel->setPen(QPen(Qt::darkBlue));
    m_lagTimeLabel->setBrush(QBrush(QColor(255, 255, 255, 230)));
    m_lagTimeLabel->setPadding(QMargins(8, 4, 8, 4));
    m_lagTimeLabel->setPositionAlignment(Qt::AlignRight | Qt::AlignTop);

    QLOG_INFO() << "延迟时间标记: " << lagTime << "分钟, 位置: " << lagTimeSeconds << "秒";
}




void CustomFixTableView::updateTableWithCalculatedParams(const quint8& reagent,
     double slope, double timeToMax, double lagTime, double auc)
{
    static const QMap<quint8, int> REAGENT_ROW_MAP = {
        { AA_REAGENT,  0 },
        { ADP_REAGENT, 1 },
        { EPI_REAGENT, 2 },
        { COL_REAGENT, 3 },
        { RIS_REAGENT, 4 }
    };

    auto rowIt = REAGENT_ROW_MAP.find(reagent);
    if (rowIt == REAGENT_ROW_MAP.end()) return;

    int row = rowIt.value();

    // 更新计算参数到表格
    insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::Slope),
        QString::number(slope, 'f', 3));
    insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::TMAtime),
        QString::number(timeToMax, 'f', 2));
    insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::LagTime),
        QString::number(lagTime, 'f', 2));
    insertColumnText(ui->tableWidget, row, static_cast<int>(TableItemnum::AUC),
        QString::number(auc, 'f', 1));
}


void CustomFixTableView::onCurveMouseMove(QMouseEvent* event)
{
    if (!m_verticalLine || !m_dataPointTracer || !m_dataPointLabel) return;

    double x = ui->widgetCurveShow->xAxis->pixelToCoord(event->pos().x());
    double y = ui->widgetCurveShow->yAxis->pixelToCoord(event->pos().y());

    // 确保x在有效范围内
    if (x < 0 || x > 299) {
        m_verticalLine->setVisible(false);
        m_dataPointTracer->setVisible(false);
        m_dataPointLabel->setVisible(false);
        ui->widgetCurveShow->replot();
        return;
    }

    // 计算最接近时间点的索引
    int index = qRound(x); // 四舍五入到最接近的整数秒
    if (index < 0) index = 0;
    if (index >= m_currentCurveData.size()) index = m_currentCurveData.size() - 1;

    // 获取该时间点的聚集率值
    double aggregationValue = 0.0;
    if (index < m_currentCurveData.size()) {
        aggregationValue = m_currentCurveData[index];
    }

    // 更新垂直线位置
    m_verticalLine->point1->setCoords(x, 0);
    m_verticalLine->point2->setCoords(x, 100); // 假设Y轴范围是0-100
    m_verticalLine->setVisible(true);

    // 更新数据点追踪器
    m_dataPointTracer->position->setCoords(x, aggregationValue);
    m_dataPointTracer->setVisible(true);

    // 更新数据标签
    m_dataPointLabel->setText(QString("时间: %1 s\n聚集率: %2 %")
                             .arg(x, 0, 'f', 1)
                             .arg(aggregationValue, 0, 'f', 1));
    m_dataPointLabel->setVisible(true);

    ui->widgetCurveShow->replot();
}

