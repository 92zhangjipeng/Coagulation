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
    ui(new Ui::CustomFixTableView)
{
    ui->setupUi(this);
    setWindowTitle("预览测试结果");
    setWindowFlags(windowFlags() | Qt::Dialog | Qt::WindowStaysOnTopHint);


    // 1. 初始化动画对象
    m_showAnim = new QPropertyAnimation(this, "geometry");
    m_hideAnim = new QPropertyAnimation(this, "geometry");

    // 2. 设置动画曲线 (平滑加速减速)
    m_showAnim->setEasingCurve(QEasingCurve::OutBack);    // 弹跳效果增强视觉反馈
    m_hideAnim->setEasingCurve(QEasingCurve::InBack);

    // 3. 设置动画时长
    m_showAnim->setDuration(500);  // 500ms
    m_hideAnim->setDuration(400);  // 稍快隐藏

    // 4. 绑定隐藏动画结束信号
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
    const int width = 790;  // 窗口宽度
    const int height = 930; // 窗口高度
    const int x = screenRect.x() + (screenRect.width() - width) / 2;


    // 初始位置：屏幕底部外
    QRect startRect(x, screenRect.bottom(), width, height);
    // 目标位置：屏幕中央
    QRect endRect(x, screenRect.top() + (screenRect.height() - height) / 2, width, height);

    // 设置动画参数
    m_showAnim->setStartValue(startRect);
    m_showAnim->setEndValue(endRect);


    // 显示窗口并启动动画
    show();
    raise();  // 确保置顶
    activateWindow();  // 获取焦点
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

    /*QCPTextElement *plotTitle = new QCPTextElement(customPlot);
    plotTitle->setText("数据曲线");
    plotTitle->setTextColor(Qt::black);
    plotTitle->setFont(QFont("Microsoft YaHei", 12, QFont::Bold));
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, plotTitle);*/

    //交互设置优化
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->setSelectionRectMode(QCP::srmNone);

    //字体设置优化
    QFont plotFont = customPlot->font();
    plotFont.setPointSize(10); // 适中字号
    customPlot->setFont(plotFont);

    //坐标轴标签设置
    customPlot->xAxis->setLabel("时间 (s)");
    customPlot->yAxis->setLabel("百分比 (%)");
    customPlot->xAxis->setLabelColor(Qt::black);
    customPlot->yAxis->setLabelColor(Qt::black);

    //坐标轴范围优化
    customPlot->xAxis->setRange(0, 300);
    customPlot->yAxis->setRange(-20, 100);

    //刻度设置优化 - 使用更简洁的方式
    QSharedPointer<QCPAxisTickerFixed> xTicker(new QCPAxisTickerFixed);
    xTicker->setTickStep(30.0);
    xTicker->setTickCount(11); // 0,30,60,...,300 共11个刻度
    customPlot->xAxis->setTicker(xTicker);

    QSharedPointer<QCPAxisTickerFixed> yTicker(new QCPAxisTickerFixed);
    yTicker->setTickStep(20.0); // -20,0,20,40,60,80,100
    yTicker->setTickCount(7);
    customPlot->yAxis->setTicker(yTicker);

    // 7. 刻度标签颜色
    customPlot->xAxis->setTickLabelColor(QColor("#1C1C1C"));
    customPlot->yAxis->setTickLabelColor(QColor("#1C1C1C"));

    // 8. 坐标轴线设置
    QPen axisPen(Qt::black, 1.5, Qt::SolidLine);
    customPlot->xAxis->setBasePen(axisPen);
    customPlot->yAxis->setBasePen(axisPen);

    // 9. 刻度线优化
    QPen tickPen(QColor("#666666"), 1, Qt::SolidLine);
    customPlot->xAxis->setTickPen(tickPen);
    customPlot->yAxis->setTickPen(tickPen);

    QPen subTickPen(QColor("#999999"), 0.5, Qt::SolidLine);
    customPlot->xAxis->setSubTickPen(subTickPen);
    customPlot->yAxis->setSubTickPen(subTickPen);

    // 网格线优化
    QPen gridPen(QColor(220, 220, 220), 1, Qt::DotLine);
    gridPen.setCosmetic(true);
    customPlot->xAxis->grid()->setPen(gridPen);
    customPlot->yAxis->grid()->setPen(gridPen);

    // 零线设置
    QPen zeroLinePen(Qt::blue, 1.5, Qt::DashLine);
    customPlot->xAxis->grid()->setZeroLinePen(zeroLinePen);
    customPlot->yAxis->grid()->setZeroLinePen(zeroLinePen);

    //背景和画布优化
    customPlot->setBackground(QBrush(QColor(245, 245, 245)));
    customPlot->axisRect()->setBackground(QBrush(Qt::white));

    //性能优化：延迟刷新
    customPlot->setNoAntialiasingOnDrag(true);
    customPlot->setNotAntialiasedElements(QCP::aeAll);

    //最后一次性刷新（性能优化）
    customPlot->replot();

    // 创建图例层
    initCreatCPGraph(customPlot);
    return;
}


void CustomFixTableView::initCreatCPGraph(QCustomPlot* pshowcurvedata)
{
    if (!pshowcurvedata) return;

    //使用结构体统一管理曲线配置
    struct CurveConfig {
        QCPGraph** graphPtr;  // 指向成员变量的指针
        quint8 reagent;
        QString name;
    };

    //集中管理所有曲线配置
    const QList<CurveConfig> curveConfigs = {
        { &m_showAACpgraph,  AA_REAGENT, "AA" },
        { &m_showADPCpgraph, ADP_REAGENT, "ADP" },
        { &m_showEPICpgraph, EPI_REAGENT, "EPI" },
        { &m_showCOLCpgraph, COL_REAGENT, "COL" },
        { &m_showRISCpgraph, RIS_REAGENT, "RIS" }
    };

    // 3. 统一创建和配置曲线
    for (const auto& config : curveConfigs) {

        // 清理旧曲线
        if (*config.graphPtr) {
            pshowcurvedata->removeGraph(*config.graphPtr);
        }

        // 创建新曲线
        QCPGraph* graph = pshowcurvedata->addGraph();
        *config.graphPtr = graph;

        if (graph) {
           graph->setPen(QPen(GlobalData::customCurveColor(config.reagent), 2));
           graph->setName(config.name);
           graph->setLineStyle(QCPGraph::lsLine);
           graph->setAntialiased(true);
        }
    }
    //一次性重绘
    pshowcurvedata->replot();
}


// 1. 将曲线列表定义为类成员（避免每次重建）
void CustomFixTableView::CreatResultCruve()
{
    for (QCPGraph* graph : calibrationGraphs()) { // 成员函数返回曲线列表
        if(graph){
            clearGraphData(graph);
        }
    }
    ui->widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
}


void CustomFixTableView::clrarResultTable(QTableWidget *pTable)
{
    //清空结果列
    for (int Rows = 0; Rows < pTable->rowCount();  ++Rows) {
        insertColumnText(ui->tableWidget, Rows,static_cast<int>(TableItemnum::ReagentTimeResult),"");
    }
}

// 2. 封装清除操作
void CustomFixTableView::clearGraphData(QCPGraph* graph)
{
    if (!graph) return;

    // 更高效的数据清除方式
	graph->data()->clear();
    graph->data().data()->squeeze();
}

// 3. 获取曲线列表（静态或成员函数）
QList<QCPGraph*> CustomFixTableView::calibrationGraphs() const
{
    static const QList<QCPGraph*> graphs = {
        m_showAACpgraph,
        m_showADPCpgraph,
        m_showEPICpgraph,
        m_showCOLCpgraph,
        m_showRISCpgraph
    };
    return graphs;
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

    // 3. 获取试剂数据
    auto reagentCurvedata = FullyAutomatedPlatelets::pinstancesqlData()->getCurveData(
        m_viewIDstr, testEndReagent
    );

    // 4. 设置曲线数据
    if (auto graph = reagentGraphMap.value(testEndReagent, nullptr)) {
        if(!smooth)
            graph->setData(timePoints, reagentCurvedata);
        else
            graph->setData(timePoints, GlobalData::smoothData(reagentCurvedata, 5));
    }
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
    QStringList header{tr("测试项目"),tr("状态"),tr("结果分析"),tr("结果数据"),tr("对比结果"),tr("参考值")};
    Table->setColumnCount(header.size());
    Table->setHorizontalHeaderLabels(header);

    QFont font;
    font.setFamily("楷体");
    font.setPixelSize(14);
    font.setBold(true);

    Table->horizontalHeader()->setFont(font);
    Table->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    Table->verticalHeader()->setVisible(false); //隐藏行表头(行号)
    Table->verticalHeader()->setDefaultSectionSize(30); //设置行高
    Table->setSelectionMode(QAbstractItemView::ExtendedSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    Table->horizontalHeader()->setFixedHeight(35); //设置表头的高度
    Table->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    Table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents); // 将根据整个列或行的内容自动调整区段的大小
    Table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);    //x先自适应宽度
    Table->setIconSize(QSize(32, 32));

    Table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    Table->setColumnWidth(0, 80);
    Table->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    Table->setColumnWidth(1, 100);
    Table->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);

    Table->setColumnWidth(3, 200);
    Table->setColumnWidth(4, 120);

    Table->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(188, 187, 186); "
                                             "font:14pt '楷体'; color: black;};");

    QStringList projectitems{tr("AA"),tr("ADP"),tr("EPI"),tr("COL"),tr("RIS")};
    QStringList reagentTimeName{tr("聚集率[60s]"),tr("聚集率[180s]"),tr("聚集率[300s]"),tr("聚集率[Max]")};
    int itemHeigh = projectitems.size() * 4;
    Table->setRowCount(itemHeigh);

    for(int r = 0 ; r < 20 ; ++r)
        insertColumnText(Table, r, static_cast<int>(TableItemnum::ReagentTime),
                          GlobalData::customCurveColor(r/4 + 1),reagentTimeName.at(r%4));

    int indexRows = 0;
    for (auto Projectname : projectitems) {
        Table->setSpan(indexRows * 4,  static_cast<int>(TableItemnum::ReagentName), 4, 1);
        insertColumnText(Table, indexRows * 4,static_cast<int>(TableItemnum::ReagentName),
                         GlobalData::customCurveColor(indexRows + 1),Projectname);
        indexRows++;
    }
    connect(Table, SIGNAL(cellClicked(int, int)), this, SLOT(viewOneReagentCurve(int, int)));
    return;
}


void CustomFixTableView::spanTableWidget(const int &fromrows ,
                                         const int& indexCols,
                                         QString pathicon,QString outResult){
    ui->tableWidget->setSpan(fromrows,  indexCols, 4,  1);
    updateParaState(ui->tableWidget, fromrows,indexCols,QColor(250,250,250),pathicon,outResult);
}


void CustomFixTableView::viewOneReagentCurve(int rows, int cols){
    if(rows < 0)  return;

    //先清除测试数据曲线再画
    CreatResultCruve();

    // 定义试剂类型映射表 (C++11起支持统一初始化)
    static const std::array<quint8, 5> REAGENT_MAP = {
        AA_REAGENT,   // 0-3
        ADP_REAGENT,  // 4-7
        EPI_REAGENT,  // 8-11
        COL_REAGENT,  // 12-15
        RIS_REAGENT   // 16-19
    };

    // 边界检查与索引计算
    const int index = (rows >= 0 && rows < 20) ? (rows / 4) : -1;
    if(index == -1) {
        QLOG_WARN() << "Invalid rows value:" << rows;
        return;  // 或抛出异常
    }
    const quint8 indexReag = REAGENT_MAP[index];
    showCurveTestEnd(indexReag,false);
    ui->widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
    return;
}

void CustomFixTableView::updatetabletestedResult(const quint8& indexReag)
{
    //定义试剂类型与索引映射
    static const QMap<quint8, std::pair<int, QString PatientInformationStu::*>> REAGENT_MAP = {
        {AA_REAGENT,  {0,  &PatientInformationStu::AAResult}},
        {ADP_REAGENT, {4,  &PatientInformationStu::ADPResult}},
        {EPI_REAGENT, {8,  &PatientInformationStu::EPIResult}},
        {COL_REAGENT, {12, &PatientInformationStu::COLResult}},
        {RIS_REAGENT, {16, &PatientInformationStu::RISResult}}
    };



    //获取测试结果
    PatientInformationStu painterInfovec;
    FullyAutomatedPlatelets::pinstancesqlData()->getTestResultTabledata(m_viewIDstr, painterInfovec);

    auto it = REAGENT_MAP.find(indexReag);
    if (it == REAGENT_MAP.end()) return; // 无效试剂类型

    QString testResultVal = painterInfovec.*(it->second);
    QStringList resultList = testResultVal.simplified().split(",");
    bool testfinish = (resultList.size() == 4); // 简化状态判断

    int startIdx = it->first;
    const int reagentStateCol = static_cast<int>(TableItemnum::ReagentState);

    if (testfinish) {
        // 批量填充结果列
        for (int i = 0; i < 4; ++i) {
            insertColumnText(ui->tableWidget, startIdx + i,
                             static_cast<int>(TableItemnum::ReagentTimeResult),
                             resultList.at(i));
        }
        setReagentStatus(startIdx, reagentStateCol, Status::Completed);
    } else if (testResultVal.isEmpty()) {
        setReagentStatus(startIdx, reagentStateCol, Status::NotTested);
    } else if (testResultVal == "null") {
        setReagentStatus(startIdx, reagentStateCol, Status::Pending);
    }
    ui->tableWidget->viewport()->update();
}


void CustomFixTableView::setReagentStatus(int row, int col, Status status) {
    QPair<QString, QString> resources;
    switch(status) {
        case Status::Completed:
            resources = {":/Picture/SetPng/status_Normal.png", tr("完成")};
            break;
        case Status::NotTested:
            resources = {":/Picture/SetPng/status_Error.png", tr("未测试")};
            break;
        case Status::Pending:
            resources = {":/Picture/SetPng/status_Action.png", tr("等待测试")};
            break;
    }
    spanTableWidget(row, col, resources.first, resources.second);
}


bool CustomFixTableView::updateSetSexReferValue()
{
    const QString sexMan = "男";
    bool  isfindSex = (m_sampleSex.isEmpty()|| m_sampleSex.isNull())? false: true;
    bool isSex = (m_sampleSex == sexMan)? true: false;


    QStringList keyRefence{tr("AA聚集率-1"),tr("AA聚集率-2"),tr("AA聚集率-3"),tr("AA聚集率-Max"),
                          tr("ADP聚集率-1"),tr("ADP聚集率-2"),tr("ADP聚集率-3"),tr("ADP聚集率-Max"),
                          tr("EPI聚集率-1"),tr("EPI聚集率-2"),tr("EPI聚集率-3"),tr("EPI聚集率-Max"),
                          tr("COL聚集率-1"),tr("COL聚集率-2"),tr("COL聚集率-3"),tr("COL聚集率-Max"),
                          tr("RIS聚集率-1"),tr("RIS聚集率-2"),tr("RIS聚集率-3"),tr("RIS聚集率-Max")};


    int rows = 0;
    for(QList<QString>::iterator it = keyRefence.begin(); it != keyRefence.end(); it++)
    {
        if(!isfindSex){
           insertColumnText(ui->tableWidget, rows,
                            static_cast<int>(TableItemnum::ReferenceValue),"未配置性别");
        }
        else{
            QString mandata,womandata;
            FullyAutomatedPlatelets::pinstancesqlData()->_obtainPersondata_((*it),mandata,womandata);
            if(isSex){
               insertColumnText(ui->tableWidget, rows , static_cast<int>(TableItemnum::ReferenceValue), mandata);
            }else{
               insertColumnText(ui->tableWidget, rows , static_cast<int>(TableItemnum::ReferenceValue), womandata);
            }
        }
        rows++;
     }
     return isfindSex;
}

void CustomFixTableView::Analyzeresultingvalues(const bool &alreadysetSex)
{

    QTableWidget *presultTable = ui->tableWidget;
    int totalRows = presultTable->rowCount();
    const int colsResult = static_cast<int>(TableItemnum::ReagentTimeResult);
    const int colsCompare = static_cast<int>(TableItemnum::OutResult);
    const int colreferne =  static_cast<int>(TableItemnum::ReferenceValue);
    if(!alreadysetSex){
         for (int r = 0; r < totalRows; ++r){
             insertColumnText(presultTable,r,colsCompare, "/");
         }
         return;
    }

    for (int r = 0; r < totalRows; ++r) {  // 修复循环变量名 (r vs n)
        // 1. 结果值处理
        QTableWidgetItem* resultItem = presultTable->item(r, colsResult);
        if (!resultItem || resultItem->text().isEmpty()) {
            insertColumnText(presultTable, r, colsCompare, "/");
            continue;
        }

        // 2. 数值解析优化
        bool convertOk = false;
        QString datastr = resultItem->text();
        QUIUtils::parseDataratio(datastr);
        const double resultval = datastr.toDouble(&convertOk);
        if (!convertOk) {  // 解析失败处理
            insertColumnText(presultTable, r, colsCompare, "?");
            continue;
        }

        // 3. 参考值处理
        QTableWidgetItem* refItem = presultTable->item(r, colreferne);
        if (!refItem || refItem->text() == "未配置性别") continue;

        const QString refData = refItem->text();
        // 4. 范围解析优化
        const QStringList parts = refData.split('(');
        if (parts.size() != 2) {  // 简化嵌套判断
            QLOG_DEBUG() << "参考值格式错误 (括号缺失)";
            continue;
        }
        const QStringList range = parts[0].split('-');
        if (range.size() != 2) {  // 统一错误处理
            QLOG_DEBUG() << "参考值范围格式错误";
            continue;
        }

        // 5. 数值比较
        bool lowOk, highOk;
        const double lowBound = range[0].toDouble(&lowOk);
        const double highBound = range[1].toDouble(&highOk);
        if (!lowOk || !highOk) {  // 边界值转换校验
            QLOG_DEBUG() << "参考值非数值类型";
            continue;
        }

        // 6. 结果判定 (优化逻辑分支)
        if (resultval < lowBound) {
            insertColumnText(presultTable, r, colsCompare, "↓偏低");  // 修正列为colsCompare
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

//下一项
void CustomFixTableView::on_pushButtonNext_clicked()
{
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
