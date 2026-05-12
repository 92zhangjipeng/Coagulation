// customfixtableview.cpp
#include "customfixtableview.h"
#include "ui_customfixtableview.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <globaldata.h>
#include <qcustomplot.h>
#include <QsLog/include/QsLog.h>
#include <QMessageBox>
#include <QTimer>
#include <cglobal.h>
#include <operclass/fullyautomatedplatelets.h>
#include <PrintReport/PrintTemplate.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

using namespace TableConstants;

// 静态时间点数据缓存
static QVector<double> s_timePoints = [] {
    QVector<double> points(CURVE_DATA_SIZE);
    for (int i = 0; i < CURVE_DATA_SIZE; i++) {
        points[i] = i;
    }
    return points;
}();

// 动画时长常量
static constexpr int SHOW_ANIM_DURATION = 500;
static constexpr int HIDE_ANIM_DURATION = 400;

CustomFixTableView::CustomFixTableView(QWidget *parent) :
    QWidget(parent),
    ui(nullptr),
    m_titleBar(nullptr),
    m_contentWidget(nullptr),
    m_mainLayout(nullptr),
    m_gridLayout(nullptr),
    m_widgetShowInfo(nullptr),
    m_widgetbottom(nullptr),
    m_tableWidget(nullptr),
    m_widgetCurveShow(nullptr),
    m_analysisReport(nullptr),
    m_pushButtonBack(nullptr),
    m_pushButtonNext(nullptr),
    m_labelname(nullptr),
    m_labelAAColor(nullptr), m_labelAAcurve(nullptr),
    m_labelADPColor(nullptr), m_labelADpcurve(nullptr),
    m_labelEPIColor(nullptr), m_labelEPIcurve(nullptr),
    m_labelcolColor(nullptr), m_labelcolcurve(nullptr),
    m_labelrisColor(nullptr), m_labelriscurve(nullptr),
    m_showAnim(nullptr),
    m_hideAnim(nullptr),
    m_currentScreen(nullptr),
    m_verticalLine(nullptr),
    m_dataPointTracer(nullptr),
    m_dataPointLabel(nullptr),
    m_viewIDstr(""),
    m_viewIDNum(-1),
    m_sampleSex(""),
    m_showAACpgraph(nullptr),
    m_showADPCpgraph(nullptr),
    m_showEPICpgraph(nullptr),
    m_showCOLCpgraph(nullptr),
    m_showRISCpgraph(nullptr),
    m_checkBoxAUC(nullptr),
    m_checkBoxSmooth(nullptr),
    m_maxAggregationTracer(nullptr),    // 确保初始化为 nullptr
    m_maxAggregationLabel(nullptr),     // 确保初始化为 nullptr
    m_aucCurve(nullptr),
    m_slopeLine(nullptr),
    m_slopeLabel(nullptr),
    m_slopeStartTracer(nullptr),
    m_slopeEndTracer(nullptr),
    m_lagTimeTracer(nullptr),
    m_lagTimeLine(nullptr),
    m_lagTimeLabel(nullptr)
{
    // 设置窗口属性 - 无边框窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    // setAttribute(Qt::WA_TranslucentBackground);  // 移除透明背景，避免影响点击事件

    createDynamicUI();
    setWindowTitle("预览测试结果");

    // 初始化动画对象
    m_showAnim = new QPropertyAnimation(this, "geometry");
    m_hideAnim = new QPropertyAnimation(this, "geometry");

    m_showAnim->setEasingCurve(QEasingCurve::OutBack);
    m_hideAnim->setEasingCurve(QEasingCurve::InBack);
    m_showAnim->setDuration(SHOW_ANIM_DURATION);
    m_hideAnim->setDuration(HIDE_ANIM_DURATION);

    connect(m_hideAnim, &QPropertyAnimation::finished,
        this, &CustomFixTableView::onHideFinished);

    m_SampleidList.clear();
    initCreateCurveWidget(m_widgetCurveShow);
    initShowResultWidget(m_tableWidget);
}

CustomFixTableView::~CustomFixTableView()
{
    // 先停止所有动画
    if (m_showAnim) {
        m_showAnim->stop();
    }
    if (m_hideAnim) {
        m_hideAnim->stop();
    }

    // 清理曲线项（在 m_widgetCurveShow 被销毁之前）
    // clearAuxiliaryItems 已经包含了 AUC 填充图形的清理，避免双重删除
    clearAuxiliaryItems();

    // 注意：不要删除 m_widgetCurveShow，它由 Qt 的父子关系管理

    // 安全删除动画
    SAFE_DELETE_ANIM(m_showAnim);
    SAFE_DELETE_ANIM(m_hideAnim);
    delete ui;
}

void CustomFixTableView::createDynamicUI()
{
    // 设置主窗口样式
    //setMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建自定义标题栏
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle("血小板聚集分析系统");

    // 连接标题栏信号
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &CustomFixTableView::onMaximizeRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &CustomFixTableView::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &CustomFixTableView::onCloseRequested);

    // 创建内容区域容器（带背景和圆角）
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("ContentWidget");
    m_contentWidget->setStyleSheet(R"(
        QWidget#ContentWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #e6f2ff, stop:1 #cce5ff);
            border: 2px solid #b3d9ff;
            border-radius: 0 0 8px 8px;
        }
    )");

    // 创建内容区域的主布局
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(15, 15, 15, 15);
    contentLayout->setSpacing(12);

    // 创建顶部控制栏（上一条/下一条按钮）
    QWidget *topControlBar = new QWidget();
    topControlBar->setMinimumHeight(50);
    topControlBar->setMaximumHeight(50);

    QHBoxLayout *topControlLayout = new QHBoxLayout(topControlBar);
    topControlLayout->setContentsMargins(0, 0, 0, 0);
    topControlLayout->setSpacing(12);

    m_pushButtonBack = new QPushButton("◀ 上一条");
    m_pushButtonBack->setMinimumSize(100, 34);
    m_pushButtonBack->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #4da6ff, stop:1 #3399ff);
            border: 1px solid #1e90ff;
            border-radius: 6px;
            color: white;
            font-weight: bold;
            font-size: 12px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #66b3ff, stop:1 #4da6ff);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #0077cc, stop:1 #005c99);
        }
    )");

    m_labelname = new QLabel();
    m_labelname->setAlignment(Qt::AlignCenter);
    m_labelname->setStyleSheet(R"(
        QLabel {
            color: #005c99;
            font-family: "微软雅黑";
            font-size: 14px;
            font-weight: bold;
            background: transparent;
        }
    )");

    m_pushButtonNext = new QPushButton("下一条 ▶");
    m_pushButtonNext->setMinimumSize(100, 34);
    m_pushButtonNext->setStyleSheet(m_pushButtonBack->styleSheet());

    topControlLayout->addWidget(m_pushButtonBack);
    topControlLayout->addStretch();
    topControlLayout->addWidget(m_labelname);
    topControlLayout->addStretch();
    topControlLayout->addWidget(m_pushButtonNext);

    contentLayout->addWidget(topControlBar);

    // 创建主要区域：曲线和结果（左右布局）
    QWidget *mainArea = new QWidget();
    QHBoxLayout *mainAreaLayout = new QHBoxLayout(mainArea);
    mainAreaLayout->setContentsMargins(0, 0, 0, 0);
    mainAreaLayout->setSpacing(12);

    // 左侧：曲线显示区域
    QWidget *curveContainer = new QWidget();
    curveContainer->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #f0f8ff, stop:1 #e6f2ff);
            border: 2px solid #b3d9ff;
            border-radius: 8px;
        }
    )");
    QVBoxLayout *curveContainerLayout = new QVBoxLayout(curveContainer);
    curveContainerLayout->setContentsMargins(12, 12, 12, 12);
    curveContainerLayout->setSpacing(10);




    m_widgetCurveShow = new QCustomPlot();
    m_widgetCurveShow->setMinimumHeight(300);
    m_widgetCurveShow->setMaximumHeight(350);
    m_widgetCurveShow->setStyleSheet(R"(
        QCustomPlot {
            background: white;
            border: 1px solid #99ccff;
            border-radius: 6px;
        }
    )");


    curveContainerLayout->addWidget(m_widgetCurveShow);

    // 右侧：结果显示区域
    QWidget *resultContainer = new QWidget();
    resultContainer->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #f0f8ff, stop:1 #e6f2ff);
            border: 2px solid #b3d9ff;
            border-radius: 8px;
        }
    )");
    QVBoxLayout *resultContainerLayout = new QVBoxLayout(resultContainer);
    resultContainerLayout->setContentsMargins(5, 5, 5, 5);
    resultContainerLayout->setSpacing(5);

    // 创建分析报告显示区域
    m_analysisReport = new QLabel();
    m_analysisReport->setContentsMargins(0, 0, 0, 0);
    m_analysisReport->setText("待测试");
    m_analysisReport->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_analysisReport->setWordWrap(true);
    m_analysisReport->setMinimumHeight(350);
    m_analysisReport->setMaximumHeight(400);
    m_analysisReport->setStyleSheet(R"(
        QLabel {
            background: white;
            border: 1px solid #99ccff;
            border-radius: 6px;
            color: #005c99;
            padding: 1px;
            font-family: "微软雅黑";
            font-size: 12px;
            line-height: 1.6;
        }
    )");


    resultContainerLayout->addWidget(m_analysisReport);

    // 增加分析报告的高度
    resultContainer->setMinimumHeight(400);
    resultContainer->setMaximumHeight(450);

    // 设置左右区域比例
    mainAreaLayout->addWidget(curveContainer, 3);
    mainAreaLayout->addWidget(resultContainer, 2);

    contentLayout->addWidget(mainArea, 2);  // 曲线和结果区域占2份（减少）

                                            // 创建表格区域
    QWidget *tableContainer = new QWidget();
    tableContainer->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #f0f8ff, stop:1 #e6f2ff);
            border: 2px solid #b3d9ff;
            border-radius: 8px;
        }
    )");
    QVBoxLayout *tableContainerLayout = new QVBoxLayout(tableContainer);
    tableContainerLayout->setContentsMargins(12, 12, 12, 12);
    tableContainerLayout->setSpacing(10);


    m_tableWidget = new QTableWidget();
    m_tableWidget->setMinimumHeight(360);
    m_tableWidget->setColumnCount(6);
    m_tableWidget->setRowCount(1);
    m_tableWidget->setHorizontalHeaderLabels(QStringList() << "AA" << "ADP" << "EPI" << "COL" << "RIS" << "状态");
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    m_tableWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->horizontalHeader()->setMinimumHeight(35);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 列宽自动拉伸填充
    m_tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_tableWidget->verticalHeader()->setDefaultSectionSize(40);  // 设置默认行高
    m_tableWidget->setStyleSheet(R"(
        QTableWidget {
            background: white;
            border: 1px solid #99ccff;
            border-radius: 6px;
            gridline-color: #e6f2ff;
            font-family: "微软雅黑";
            font-size: 13px;
            selection-background-color: #0078d7;
            selection-color: #ffffff;
        }
        QTableWidget::item {
            padding: 10px;
            border-bottom: 1px solid #f0f8ff;
        }
        QTableWidget::item:selected {
            background: #0078d7;
            color: #ffffff;
            font-weight: bold;
        }
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #1e90ff, stop:1 #0077cc);
            color: white;
            padding: 10px;
            border: 1px solid #005c99;
            font-weight: bold;
            font-size: 13px;
        }
    )");

    tableContainerLayout->addWidget(m_tableWidget);
    contentLayout->addWidget(tableContainer, 3);  // 表格区域占3份（增加）

    // 创建底部控制栏（曲线颜色图例和选项）
    QWidget *bottomControlBar = new QWidget();
    bottomControlBar->setMinimumHeight(40);
    bottomControlBar->setMaximumHeight(40);
    bottomControlBar->setStyleSheet(R"(
        QWidget {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #e6f2ff, stop:1 #d9edff);
            border: 0px solid #b3d9ff;
            border-radius: 4px;
        }
    )");

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomControlBar);
    bottomLayout->setContentsMargins(12, 5, 12, 5);
    bottomLayout->setSpacing(15);

    // 创建曲线颜色标签容器
    QWidget *colorWidget = new QWidget();
    colorWidget->setStyleSheet("border: none; background-color: transparent;");
    QHBoxLayout *colorLayout = new QHBoxLayout(colorWidget);
    colorLayout->setContentsMargins(0, 0, 0, 0);
    colorLayout->setSpacing(5);

    createColorLabelPair(colorLayout, "AA", "rgb(86, 147, 198)", m_labelAAColor, m_labelAAcurve);
    createColorLabelPair(colorLayout, "ADP", "rgb(114, 106, 176)", m_labelADPColor, m_labelADpcurve);
    createColorLabelPair(colorLayout, "EPI", "rgb(234, 42, 42)", m_labelEPIColor, m_labelEPIcurve);
    createColorLabelPair(colorLayout, "COL", "rgb(147, 153, 156)", m_labelcolColor, m_labelcolcurve);
    createColorLabelPair(colorLayout, "RIS", "rgb(188, 143, 143)", m_labelrisColor, m_labelriscurve);

    m_checkBoxSmooth = new QCheckBox("校平曲线");
    m_checkBoxSmooth->hide();
    m_checkBoxSmooth->setStyleSheet(R"(
        QCheckBox {
            spacing: 6px;
            color: #005c99;
            font-weight: bold;
            font-family: "微软雅黑";
            font-size: 12px;
            border: none;
            background-color: transparent;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border: 2px solid #66b3ff;
            border-radius: 3px;
            background: white;
        }
        QCheckBox::indicator:checked {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #4da6ff, stop:1 #3399ff);
        }
    )");

    m_checkBoxAUC = new QCheckBox("显示AUC面积");
    m_checkBoxAUC->setStyleSheet(m_checkBoxSmooth->styleSheet());

    // 添加打印按钮
    m_printButton = new QPushButton("🖨️ 打印报告");
    m_printButton->setMinimumSize(120, 34);
    m_printButton->setStyleSheet(R"(
        QPushButton {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #4da6ff, stop:1 #3399ff);
            border: 1px solid #1e90ff;
            border-radius: 6px;
            color: white;
            font-weight: bold;
            font-size: 12px;
            padding: 6px 16px;
        }
        QPushButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #66b3ff, stop:1 #4da6ff);
        }
        QPushButton:pressed {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                        stop:0 #0077cc, stop:1 #005c99);
        }
    )");

    bottomLayout->addWidget(colorWidget);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_checkBoxSmooth);
    bottomLayout->addWidget(m_checkBoxAUC);
    bottomLayout->addWidget(m_printButton);

    contentLayout->addWidget(bottomControlBar);

    // 将内容区域添加到主布局
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addWidget(m_contentWidget);

    // 连接信号槽
    connect(m_checkBoxSmooth, &QCheckBox::clicked, this, &CustomFixTableView::on_checkBoxSmooth_clicked);
    connect(m_checkBoxAUC, &QCheckBox::clicked, this, &CustomFixTableView::on_checkBoxAUC_clicked);
    connect(m_pushButtonBack, &QPushButton::clicked, this, &CustomFixTableView::on_pushButtonBack_clicked);
    connect(m_pushButtonNext, &QPushButton::clicked, this, &CustomFixTableView::on_pushButtonNext_clicked);
    connect(m_printButton, &QPushButton::clicked, this, &CustomFixTableView::on_printButton_clicked);
    connect(m_tableWidget, &QTableWidget::cellClicked, this, &CustomFixTableView::viewOneReagentCurve);
}

void CustomFixTableView::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(false);
        }
    }
    else {
        showMaximized();
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(true);
        }
    }

    // 窗口状态变化时强制更新显示
    if (m_widgetCurveShow && m_tableWidget) {
        showResult(false);
    }
}

void CustomFixTableView::onMinimizeRequested()
{
    showMinimized();
}

void CustomFixTableView::onCloseRequested()
{
    close();
}

void CustomFixTableView::createColorLabelPair(QHBoxLayout *layout, const QString &text, const QString &color,
    QLabel *&colorLabel, QLabel *&textLabel)
{
    colorLabel = new QLabel();
    colorLabel->setMinimumSize(14, 14);
    colorLabel->setMaximumSize(14, 14);
    colorLabel->setStyleSheet(QString("QLabel { background-color: %1; border-radius: 7px; }").arg(color));

    textLabel = new QLabel();
    textLabel->setText(text);
    textLabel->setStyleSheet("font-family: \"微软雅黑\"; font-size: 11px; font-weight: bold; color: #2C5F8A;");

    layout->addWidget(colorLabel);
    layout->addWidget(textLabel);
}

// 辅助函数实现
QLabel* CustomFixTableView::createImageLabel(const QString& iconPath)
{
    QLabel* label = new QLabel();
    QPixmap pixmap(iconPath);
    if (!pixmap.isNull()) {
        QPixmap scaledPixmap = pixmap.scaled(ICON_SIZE, ICON_SIZE,
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
        label->setPixmap(scaledPixmap);
    }
    label->setFixedSize(ICON_SIZE, ICON_SIZE);
    label->setScaledContents(true);
    return label;
}

QLabel* CustomFixTableView::createTextLabel(const QString& text)
{
    QLabel* label = new QLabel(text);
    static const QFont font("微软雅黑", 11);
    label->setFont(font);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setMinimumHeight(24);
    label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    return label;
}

void CustomFixTableView::configureLayout(QHBoxLayout* layout, QLabel* imageLabel, QLabel* textLabel)
{
    layout->addWidget(imageLabel);
    layout->addWidget(textLabel, 1);  // 文本标签占据更多空间
    layout->setSpacing(8);
    layout->setContentsMargins(6, 4, 6, 4);
    layout->setAlignment(Qt::AlignVCenter);
}

void CustomFixTableView::configureContainer(QWidget* container, QHBoxLayout* layout, const QColor& bgmC)
{
    container->setLayout(layout);
    container->setAutoFillBackground(true);

    QPalette pal = container->palette();
    pal.setColor(QPalette::Window, bgmC);
    container->setPalette(pal);

    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void CustomFixTableView::updateParaState(QTableWidget *tablewiget, const int row, const int col,
    const QColor &bgmC, const QString& iconPath,
    const QString& text)
{
    if (!tablewiget) {
        QLOG_WARN() << "updateParaState: tablewiget is null";
        return;
    }

    if (row < 0 || row >= tablewiget->rowCount() ||
        col < 0 || col >= tablewiget->columnCount()) {
        QLOG_WARN() << "updateParaState: invalid row/col" << row << col;
        return;
    }

    QWidget *containerWidget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(containerWidget);


    QLabel *imageLabel = createImageLabel(iconPath);
    QLabel *textLabel = createTextLabel(text);

    configureLayout(layout, imageLabel, textLabel);
    configureContainer(containerWidget, layout, bgmC);
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 设置容器最小高度，让它能够自适应表格行高
    containerWidget->setMinimumHeight(35);
    // 不设置最大高度，让表格能够自适应调整

    tablewiget->setCellWidget(row, col, containerWidget);
}

void CustomFixTableView::showWithAnimation()
{
    m_currentScreen = QApplication::screenAt(QCursor::pos());
    if (!m_currentScreen) {
        m_currentScreen = QApplication::primaryScreen();
    }
    QRect screenRect = m_currentScreen->availableGeometry();

    // 根据屏幕大小自适应计算窗口尺寸
    int width = qMin(WINDOW_MIN_WIDTH, screenRect.width() - 100);  // 保留边距
    int maxHeight = qMin(850, (screenRect.height() * 5) / 7);  // 高度不超过850px或屏幕的5/7
    int height = qMin(WINDOW_MIN_HEIGHT, maxHeight);
    height = qMin(height, screenRect.height() - 100);  // 同时保留边距
    int x = screenRect.x() + (screenRect.width() - width) / 2;
    int y = 10;//screenRect.y() + (screenRect.height() - height) / 2;

               // 确保最小尺寸
    width = qMax(width, 800);
    height = qMax(height, 700);

    // 再次确保高度不超过850px或屏幕的5/7
    height = qMin(height, maxHeight);

    QRect startRect(x, screenRect.bottom(), width, height);
    QRect endRect(x, y, width, height);

    m_showAnim->setStartValue(startRect);
    m_showAnim->setEndValue(endRect);

    show();
    raise();
    activateWindow();
    if (m_hideAnim) {
        m_hideAnim->disconnect();
    }

    // 强制刷新显示，确保数据更新
    if (m_widgetCurveShow && m_tableWidget) {
        showResult(false);
    }

    m_showAnim->start();
}

void CustomFixTableView::hideWithAnimation()
{
    QScreen* currentScreen = QApplication::screenAt(this->pos());
    if (!currentScreen) return;
    QRect screenRect = currentScreen->geometry();

    QRect currentRect = geometry();
    QRect endRect = currentRect;
    endRect.moveTop(screenRect.bottom() + 1);
    endRect.setHeight(0);

    m_hideAnim->setStartValue(currentRect);
    m_hideAnim->setEndValue(endRect);

    connect(m_hideAnim, &QPropertyAnimation::finished,
        this, &CustomFixTableView::hide,
        Qt::UniqueConnection);
    m_hideAnim->start();
}

void CustomFixTableView::onHideFinished()
{
    close();
}

void CustomFixTableView::showEvent(QShowEvent *e)
{
    setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(e);
}

void CustomFixTableView::closeEvent(QCloseEvent *event)
{
    if (m_hideAnim->state() != QAbstractAnimation::Running) {
        hideWithAnimation();
        event->ignore();
    }
    else {
        event->accept();
    }
}

// 数据验证函数
bool CustomFixTableView::validateCurveData(const QVector<double>& data, quint8 reagent) const
{
    if (data.isEmpty()) {
        QLOG_WARN() << "Empty curve data for reagent:" << reagent;
        return false;
    }

    if (data.size() != CURVE_DATA_SIZE) {
        QLOG_WARN() << "Curve data size mismatch for reagent:" << reagent
            << "Expected:" << CURVE_DATA_SIZE << "Got:" << data.size();
        return false;
    }

    bool hasInvalidData = false;
    for (double val : data) {
        if (std::isinf(val) || std::isnan(val)) {
            hasInvalidData = true;
            QLOG_WARN() << "Invalid value (inf/nan) found in curve data for reagent:"
                << reagent << ", value:" << val;
            break;
        }
    }

    if (hasInvalidData) {
        return false;
    }

    if (std::all_of(data.begin(), data.end(),
        [](double val) { return std::isnan(val); })) {
        QLOG_WARN() << "All data is NaN for reagent:" << reagent;
        return false;
    }

    return true;
}

// 计算时间段平均值
TimeSegmentAverages CustomFixTableView::calculateTimeAverages(const QVector<double>& data) const
{
    TimeSegmentAverages result;

    if (data.isEmpty()) {
        return result;
    }

    double sumAll = 0.0;
    for (double val : data) {
        sumAll += val;
    }
    result.avg0to300 = sumAll / data.size();

    if (data.size() >= 60) {
        double sum0to60 = 0.0;
        for (int i = 0; i < 60; ++i) {
            sum0to60 += data[i];
        }
        result.avg0to60 = sum0to60 / 60;
    }

    if (data.size() >= 180) {
        double sum60to180 = 0.0;
        for (int i = 60; i < 180; ++i) {
            sum60to180 += data[i];
        }
        result.avg60to180 = sum60to180 / 120;
    }

    if (data.size() >= 300) {
        double sum180to300 = 0.0;
        for (int i = 180; i < 300; ++i) {
            sum180to300 += data[i];
        }
        result.avg180to300 = sum180to300 / 120;
    }

    QLOG_INFO() << QString("时间段平均值 - 0-60: %1, 60-180: %2, 180-300: %3, 0-300: %4")
        .arg(result.avg0to60, 0, 'f', 1)
        .arg(result.avg60to180, 0, 'f', 1)
        .arg(result.avg180to300, 0, 'f', 1)
        .arg(result.avg0to300, 0, 'f', 1);

    return result;
}

void CustomFixTableView::showCurveTestEnd(const quint8& testEndReagent, const bool &smooth)
{
    if (!m_widgetCurveShow) {
        QLOG_WARN() << "showCurveTestEnd: m_widgetCurveShow is null";
        return;
    }

    QMutexLocker locker(&m_curveDataMutex);

    auto reagentCurvedata = FullyAutomatedPlatelets::pinstancesqlData()->getCurveData(
        m_viewIDstr, testEndReagent);

    if (!validateCurveData(reagentCurvedata, testEndReagent)) {
        return;
    }

    clearAuxiliaryItems();
    m_currentCurveData = reagentCurvedata;
    TimeSegmentAverages averages = calculateTimeAverages(reagentCurvedata);

    QCPGraph* graph = nullptr;

    double auc = 0,  slope = 0 , lagTime = 0 ,timeToMax = 0;
    switch (testEndReagent) {
    case AA_REAGENT: {
        graph = m_showAACpgraph;
        AAResult analyzerResult = AggregationAnalyzer::analyzeAA(reagentCurvedata);
        auc = analyzerResult.auc;
        slope = analyzerResult.maxSlope;
        lagTime = analyzerResult.lagTime;
        timeToMax = analyzerResult.tmaTime;
        break;
    }
    case ADP_REAGENT: {
        graph = m_showADPCpgraph;
        ADPResult analyzerResult = AggregationAnalyzer::analyzeADP(reagentCurvedata);
        auc = analyzerResult.auc;
        slope = analyzerResult.aggregationRate;
        lagTime = analyzerResult.lagTime;
        timeToMax = analyzerResult.tmaTime;
        break;
    }
    case EPI_REAGENT: {
        graph = m_showEPICpgraph;
        EPIResult analyzerResult = AggregationAnalyzer::analyzeEPI(reagentCurvedata);
        auc = analyzerResult.auc;
        slope = analyzerResult.secondPhaseSlope;
        lagTime = analyzerResult.lagTime;
        timeToMax = analyzerResult.tmaTime;
        break;
    }
    case COL_REAGENT: {
        graph = m_showCOLCpgraph;
        COLResult analyzerResult = AggregationAnalyzer::analyzeCOL(reagentCurvedata);
        auc = analyzerResult.auc;
        slope = analyzerResult.maxSlope;
        lagTime = analyzerResult.lagTime;
        timeToMax = analyzerResult.tmaTime;
        break;
    }
    case RIS_REAGENT: {
        graph = m_showRISCpgraph;
        RISResult analyzerResult = AggregationAnalyzer::analyzeRIS(reagentCurvedata);
        auc = analyzerResult.auc;
        slope = analyzerResult.aggregationRate;
        lagTime = analyzerResult.lagTime;
        timeToMax = analyzerResult.tmaTime;
        break;
    }
    default:
        return;
    }

    if (!graph) {
        QLOG_WARN() << "Graph is null for reagent:" << testEndReagent;
        return;
    }

    if (!smooth) {
        graph->setData(s_timePoints, reagentCurvedata);
    }
    else {
        graph->setData(s_timePoints, GlobalData::smoothData(reagentCurvedata, 5));
    }

    // 标记最大聚集率、计算AUC和斜率
    auto maxAggregationResult = markMaxAggregation(reagentCurvedata, graph);
    double maxAggregation = maxAggregationResult.first;


    showCustomAnalyzerResult(maxAggregation, slope, auc, timeToMax, lagTime,
        averages.avg0to60, averages.avg60to180,
        averages.avg180to300, averages.avg0to300);

    updateTableWithCalculatedParams(testEndReagent, slope, timeToMax, lagTime, auc);
    updatetabletestedResult(testEndReagent);
}

void CustomFixTableView::showCustomAnalyzerResult(double maxAggregation, double slope, double auc,
    double timeToMax, double lagTime,
    double avg0to60, double avg60to180,
    double avg180to300, double avg0to300)
{
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    QString brief = QString(
        "<div style='font-family: \"微软雅黑\", Arial, sans-serif; background-color: #ffffff; padding: 15px; border: 1px solid #d0d0d0;'>"
        "<style>"
        ".result-table { width: 100%; border-collapse: collapse; font-size: 12px; text-align: center; }"
        ".result-table td { padding: 8px 12px; }"
        ".result-table .label { font-weight: normal; color: #333333; }"
        ".result-table .value { font-weight: normal; color: #000000; }"
        ".result-table .section-title { font-weight: bold; color: #333333; background-color: #f5f5f5; text-align: center; padding: 8px; }"
        ".result-table .title { text-align: center; font-weight: bold; padding: 10px; font-size: 16px; color: #333333; background-color: #f0f0f0; }"
        ".result-table .time-info { text-align: right; border-top: 1px solid #e0e0e0; color: #888888; font-size: 10px; padding-top: 8px; }"
        "</style>"

        "<table class='result-table'>"
        "<tr><td colspan='4' class='title'>血小板聚集分析报告</td></tr>"

        "<tr>"
        "<td class='label'>最大聚集率:</td>"
        "<td class='value'>%1%</td>"
        "<td class='label'>最陡斜率:</td>"
        "<td class='value'>%2 %/min</td>"
        "</tr>"

        "<tr>"
        "<td class='label'>AUC面积:</td>"
        "<td class='value'>%3 %·min</td>"
        "<td class='label'>TMA时间:</td>"
        "<td class='value'>%4 min</td>"
        "</tr>"

        "<tr>"
        "<td class='label'>延迟时间:</td>"
        "<td class='value'>%5 min</td>"
        "<td class='label'></td>"
        "<td class='value'></td>"
        "</tr>"

        "<tr><td colspan='4' class='section-title'>时间段平均聚集率</td></tr>"

        "<tr>"
        "<td class='label'>0-60秒:</td>"
        "<td class='value'>%6%</td>"
        "<td class='label'>60-180秒:</td>"
        "<td class='value'>%7%</td>"
        "</tr>"

        "<tr>"
        "<td class='label'>180-300秒:</td>"
        "<td class='value'>%8%</td>"
        "<td class='label'>0-300秒:</td>"
        "<td class='value'>%9%</td>"
        "</tr>"

        "<tr><td colspan='4' class='time-info'>报告时间: %10</td></tr>"
        "</table>"
        "</div>"
        )
        .arg(QString::number(maxAggregation, 'f', 2))
        .arg(QString::number(slope, 'f', 2))
        .arg(QString::number(auc, 'f', 1))
        .arg(QString::number(timeToMax, 'f', 2))
        .arg(QString::number(lagTime, 'f', 2))
        .arg(QString::number(avg0to60, 'f', 2))
        .arg(QString::number(avg60to180, 'f', 2))
        .arg(QString::number(avg180to300, 'f', 2))
        .arg(QString::number(avg0to300, 'f', 2))
        .arg(currentTime);

    if (m_analysisReport) {
        m_analysisReport->setText(brief);
        m_analysisReport->setTextFormat(Qt::RichText);
        m_analysisReport->setMinimumWidth(550);
        m_analysisReport->setAlignment(Qt::AlignCenter);
        m_analysisReport->setWordWrap(false);

        // 如果 m_analysisReport 在布局中，调整其大小策略
        m_analysisReport->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    }
}

void CustomFixTableView::showResult(const bool& smooth)
{
    if (!m_widgetCurveShow || !m_tableWidget) {
        QLOG_WARN() << "showResult: widgets are null";
        return;
    }

    // 如果窗口不可见，先显示窗口
    if (!isVisible()) {
        showWithAnimation();
        return;
    }

    // 窗口已经显示，强制更新数据
    CreateResultCurve();
    clearResultTable(m_tableWidget);

    m_widgetCurveShow->setUpdatesEnabled(false);

    static const QMap<quint8, QString> ViewCurveData = {
        { AA_REAGENT,   "AA" },
        { ADP_REAGENT,  "ADP" },
        { EPI_REAGENT,  "EPI" },
        { COL_REAGENT,  "COL" },
        { RIS_REAGENT,  "RIS" }
    };

    for (auto it = ViewCurveData.constBegin(); it != ViewCurveData.constEnd(); ++it) {
        showCurveTestEnd(it.key(), smooth);
        updatetabletestedResult(it.key());
    }

    m_widgetCurveShow->setUpdatesEnabled(true);
    m_widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);

    if (!smooth) {
        bool issetSex = updateSetSexReferValue();
        Analyzeresultingvalues(issetSex);
    }

    // 强制重绘窗口，确保数据正确显示
    update();
    repaint();
}

void CustomFixTableView::insertColumnText(QTableWidget *tablewiget,
    const int row, const int col,
    const QColor &bgmC,
    const QString& text)
{
    if (!tablewiget || row < 0 || row >= tablewiget->rowCount() ||
        col < 0 || col >= tablewiget->columnCount()) {
        return;
    }

    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if (bgmC.isValid()) {
        item->setBackgroundColor(bgmC);
    }
    static const QFont font("楷体", 12);
    tablewiget->setFont(font);
    tablewiget->setItem(row, col, item);
}

void CustomFixTableView::insertColumnText(QTableWidget *tablewiget,
    const int row, const int col,
    const QString& text)
{
    if (!tablewiget || row < 0 || row >= tablewiget->rowCount() ||
        col < 0 || col >= tablewiget->columnCount()) {
        return;
    }

    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    static const QFont font("楷体", 12);
    tablewiget->setFont(font);
    tablewiget->setItem(row, col, item);
}

void CustomFixTableView::initShowResultWidget(QTableWidget * Table)
{
    Table->setContextMenuPolicy(Qt::CustomContextMenu);
    QStringList header{
        tr("测试项目"), tr("状态"), tr("60S/%"), tr("180S/%"),
        tr("300S/%"), tr("Max%"), tr("斜率(%/min)"),
        tr("AMT(min)"), tr("延迟时间(min)"),
        tr("AUC(%*min)"), tr("对比结果"), tr("参考值")
    };
    Table->setColumnCount(header.size());
    Table->setHorizontalHeaderLabels(header);

    QFont font;
    font.setFamily("楷体");
    font.setPixelSize(14);
    font.setBold(true);

    Table->horizontalHeader()->setFont(font);
    Table->horizontalHeader()->setStretchLastSection(true);
    Table->verticalHeader()->setVisible(false);
    Table->verticalHeader()->setDefaultSectionSize(50);
    Table->verticalHeader()->setMinimumSectionSize(40);
    Table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Table->horizontalHeader()->setFixedHeight(40);
    Table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    Table->setIconSize(QSize(32, 32));

    Table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    Table->setColumnWidth(0, 80);
    Table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    Table->setColumnWidth(1, 130);
    Table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
    Table->setColumnWidth(2, 100);
    Table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    Table->setColumnWidth(3, 100);
    Table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
    Table->setColumnWidth(4, 100);
    Table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Fixed);
    Table->setColumnWidth(5, 120);
    Table->horizontalHeader()->setSectionResizeMode(6, QHeaderView::Fixed);
    Table->setColumnWidth(6, 120);
    Table->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);
    Table->setColumnWidth(7, 120);
    Table->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Fixed);
    Table->setColumnWidth(8, 120);
    Table->horizontalHeader()->setSectionResizeMode(9, QHeaderView::Fixed);
    Table->setColumnWidth(9, 120);
    Table->horizontalHeader()->setSectionResizeMode(10, QHeaderView::Fixed);
    Table->setColumnWidth(10, 120);
    Table->horizontalHeader()->setSectionResizeMode(11, QHeaderView::Stretch);

    QStringList projectitems{ tr("AA"), tr("ADP"), tr("EPI"), tr("COL"), tr("RIS") };
    Table->setRowCount(projectitems.size());

    for (int row = 0; row < projectitems.size(); ++row) {
        insertColumnText(Table, row, static_cast<int>(TableItemnum::ReagentName),
            GlobalData::customCurveColor(row + 1), projectitems.at(row));
        for (int col = 1; col < Table->columnCount(); ++col) {
            insertColumnText(Table, row, col, "");
        }
    }
}

void CustomFixTableView::spanTableWidget(const int &fromrows, const int& indexCols,
    QString pathicon, QString outResult)
{
    m_tableWidget->setSpan(fromrows, indexCols, 4, 1);
    updateParaState(m_tableWidget, fromrows, indexCols, QColor(250, 250, 250), pathicon, outResult);
}

void CustomFixTableView::viewOneReagentCurve(int rows, int cols)
{
    Q_UNUSED(cols)

    if (!m_tableWidget) {
        QLOG_WARN() << "viewOneReagentCurve: tableWidget is null";
        return;
    }

    // 获取所有选中的行
    QList<QTableWidgetItem*> selectedItems = m_tableWidget->selectedItems();
    QSet<int> selectedRows;

    for (QTableWidgetItem* item : selectedItems) {
        selectedRows.insert(item->row());
    }

    // 如果没有选中任何行，或者只选中了一行，显示单条曲线
    if (selectedRows.size() <= 1) {
        if (rows < 0 || rows >= m_tableWidget->rowCount()) {
            QLOG_WARN() << "viewOneReagentCurve: invalid row index:" << rows
                << "Max rows:" << m_tableWidget->rowCount();
            return;
        }

        CreateResultCurve();

        static const std::array<quint8, 5> REAGENT_MAP = {
            AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
        };

        if (rows >= static_cast<int>(REAGENT_MAP.size())) {
            QLOG_WARN() << "viewOneReagentCurve: invalid row index for reagent map:" << rows;
            return;
        }

        const quint8 indexReag = REAGENT_MAP[rows];
        showCurveTestEnd(indexReag, false);
        m_widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
    }
    else {
        // 多选模式：显示所有选中行的曲线
        CreateResultCurve();

        static const std::array<quint8, 5> REAGENT_MAP = {
            AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
        };

        for (int row : selectedRows) {
            if (row >= 0 && row < static_cast<int>(REAGENT_MAP.size())) {
                const quint8 indexReag = REAGENT_MAP[row];
                showCurveTestEnd(indexReag, false);
            }
        }

        m_widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
    }
}

void CustomFixTableView::updatetabletestedResult(const quint8& indexReag)
{
    static const QMap<quint8, int> REAGENT_ROW_MAP = {
        { AA_REAGENT,  0 },
        { ADP_REAGENT, 1 },
        { EPI_REAGENT, 2 },
        { COL_REAGENT, 3 },
        { RIS_REAGENT, 4 }
    };

    static const QMap<quint8, QString PatientInformationStu::*> REAGENT_MEMBER_MAP = {
        { AA_REAGENT,  &PatientInformationStu::AAResult },
        { ADP_REAGENT, &PatientInformationStu::ADPResult },
        { EPI_REAGENT, &PatientInformationStu::EPIResult },
        { COL_REAGENT, &PatientInformationStu::COLResult },
        { RIS_REAGENT, &PatientInformationStu::RISResult }
    };

    PatientInformationStu painterInfovec;
    FullyAutomatedPlatelets::pinstancesqlData()->getTestResultTabledata(m_viewIDstr, painterInfovec);

    auto rowIt = REAGENT_ROW_MAP.find(indexReag);
    auto memberIt = REAGENT_MEMBER_MAP.find(indexReag);
    if (rowIt == REAGENT_ROW_MAP.end() || memberIt == REAGENT_MEMBER_MAP.end()) return;

    int row = rowIt.value();
    QString PatientInformationStu::* memberPtr = memberIt.value();

    QString testResultVal = painterInfovec.*memberPtr;
    QStringList resultList = testResultVal.simplified().split(",");
    bool testfinish = (resultList.size() == 4);

    const int reagentStateCol = static_cast<int>(TableItemnum::ReagentState);

    if (testfinish) {
        insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::Aggregation60s), resultList.at(0));
        insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::Aggregation180s), resultList.at(1));
        insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::Aggregation300s), resultList.at(2));
        insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::AggregationMax), resultList.at(3));
        setReagentStatus(row, reagentStateCol, Status::Completed);
    }
    else if (testResultVal.isEmpty()) {
        setReagentStatus(row, reagentStateCol, Status::NotTested);
    }
    else if (testResultVal == "null") {
        setReagentStatus(row, reagentStateCol, Status::Pending);
    }
    m_tableWidget->viewport()->update();
}

void CustomFixTableView::setReagentStatus(int row, int col, Status status)
{
    QString iconPath, statusText;
    switch (status) {
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
    updateParaState(m_tableWidget, row, col, QColor(250, 250, 250), iconPath, statusText);
}

bool CustomFixTableView::updateSetSexReferValue()
{
    const QString sexMan = "男";
    bool isfindSex = !(m_sampleSex.isEmpty() || m_sampleSex.isNull());
    bool isSex = (m_sampleSex == sexMan);

    QStringList keyRefence{
        tr("AA聚集率-Max"),
        tr("ADP聚集率-Max"),
        tr("EPI聚集率-Max"),
        tr("COL聚集率-Max"),
        tr("RIS聚集率-Max")
    };

    for (int row = 0; row < keyRefence.size(); ++row) {
        if (!isfindSex) {
            insertColumnText(m_tableWidget, row,
                static_cast<int>(TableItemnum::ReferenceValue), "未配置性别");
        }
        else {
            QString mandata, womandata;
            FullyAutomatedPlatelets::pinstancesqlData()->obtainPersondata(
                keyRefence.at(row), mandata, womandata);
            if (isSex) {
                insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::ReferenceValue), mandata);
            }
            else {
                insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::ReferenceValue), womandata);
            }
        }
    }
    return isfindSex;
}

void CustomFixTableView::Analyzeresultingvalues(const bool &alreadysetSex)
{
    QTableWidget *presultTable = m_tableWidget;
    int totalRows = presultTable->rowCount();
    const int colsCompare = static_cast<int>(TableItemnum::OutResult);
    const int colreferne = static_cast<int>(TableItemnum::ReferenceValue);

    if (!alreadysetSex) {
        for (int r = 0; r < totalRows; ++r) {
            insertColumnText(presultTable, r, colsCompare, "/");
        }
        return;
    }

    for (int r = 0; r < totalRows; ++r) {
        QTableWidgetItem* resultItem = presultTable->item(r, static_cast<int>(TableItemnum::AggregationMax));
        if (!resultItem || resultItem->text().isEmpty()) {
            insertColumnText(presultTable, r, colsCompare, "/");
            continue;
        }

        bool convertOk = false;
        QString datastr = resultItem->text();
        QUIUtils::parseDataratio(datastr);
        const double resultval = datastr.toDouble(&convertOk);
        if (!convertOk) {
            insertColumnText(presultTable, r, colsCompare, "?");
            continue;
        }

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

        bool lowOk, highOk;
        const double lowBound = range[0].toDouble(&lowOk);
        const double highBound = range[1].toDouble(&highOk);
        if (!lowOk || !highOk) {
            QLOG_DEBUG() << "参考值非数值类型";
            continue;
        }

        if (resultval < lowBound) {
            insertColumnText(presultTable, r, colsCompare, "↓偏低");
        }
        else if (resultval > highBound) {
            insertColumnText(presultTable, r, colsCompare, "↑偏高");
        }
        else {
            insertColumnText(presultTable, r, colsCompare, "↕正常");
        }
    }
}

void CustomFixTableView::on_checkBoxSmooth_clicked()
{
    showResult(m_checkBoxSmooth->isChecked());
}

void CustomFixTableView::on_checkBoxAUC_clicked()
{
    if (!m_widgetCurveShow || !m_checkBoxAUC) {
       QLOG_WARN() << "on_checkBoxAUC_clicked: m_widgetCurveShow is null";
       return;
    }

    bool isChecked = m_checkBoxAUC->isChecked();

    // 如果没有填充曲线，尝试找到当前显示的曲线并创建填充
    if (m_aucFillGraphs.isEmpty()) {
       // 获取当前可见的曲线
       for (int i = 0; i < m_widgetCurveShow->graphCount(); ++i) {
           QCPGraph* graph = m_widgetCurveShow->graph(i);
           // 排除颜色图例中的曲线，只处理数据曲线
           if (graph && graph->visible() &&
               graph != m_showAACpgraph && graph != m_showADPCpgraph &&
               graph != m_showEPICpgraph && graph != m_showCOLCpgraph &&
               graph != m_showRISCpgraph) {
               continue;
           }

           if (graph && graph->visible() && graph->data() && graph->data()->size() > 0) {
               // 从现有曲线提取数据
               QVector<double> xData, yData;
               for (auto it = graph->data()->constBegin(); it != graph->data()->constEnd(); ++it) {
                   xData.append(it->key);
                   yData.append(it->value);
               }

               QCPGraph* fillGraph = m_widgetCurveShow->addGraph();
               fillGraph->setPen(QPen(Qt::NoPen));
               fillGraph->setBrush(QBrush(QColor(10, 150, 136, 60)));

               QVector<double> fillX, fillY;
               fillX.reserve(xData.size() * 2);
               fillY.reserve(yData.size() * 2);

               for (int j = 0; j < xData.size(); ++j) {
                   fillX.append(xData[j]);
                   fillY.append(yData[j]);
               }
               for (int j = xData.size() - 1; j >= 0; --j) {
                   fillX.append(xData[j]);
                   fillY.append(0);
               }

               fillGraph->setData(fillX, fillY);
               m_aucFillGraphs.append(fillGraph);
               break;
           }
       }
    }

    // 控制可见性
    for (QCPGraph* graph : m_aucFillGraphs) {
       if (graph) {
           graph->setVisible(isChecked);
       }
    }

    updateAUCDisplay();
    m_widgetCurveShow->replot();
}

void CustomFixTableView::updateAUCDisplay()
{
    if (!m_checkBoxAUC) {
        QLOG_WARN() << "updateAUCDisplay: m_checkBoxAUC is null";
        return;
    }

    bool showAUC = m_checkBoxAUC->isChecked();

    if (m_aucFillGraphs.isEmpty()) {
        QLOG_INFO() << "updateAUCDisplay: No AUC graphs to show/hide";
        return;
    }

    for (QCPGraph* graph : m_aucFillGraphs) {
        if (graph) {
            graph->setVisible(showAUC);
        }
    }
}

void CustomFixTableView::on_pushButtonBack_clicked()
{
    // 基于m_SampleidList中的实际顺序导航，而不是连续的ID
    int currentIndex = -1;
    QString currentId = GlobalData::groupDateAndID(GlobalData::ObatinCreatSampleTime(), m_viewIDNum);

    // 查找当前样本在列表中的位置
    for (int i = 0; i < m_SampleidList.size(); ++i) {
        if (m_SampleidList[i] == currentId) {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex == -1) {
        QMessageBox::warning(this, tr("查看失败"), tr("当前样本不在列表中!"));
        return;
    }

    // 上一个样本
    if (currentIndex > 0) {
        QString prevId = m_SampleidList[currentIndex - 1];
        QString date;
        int idNum;
        GlobalData::apartSampleId(prevId, date, idNum);
        viewOneSelf(prevId, idNum);
    }
    else {
        QMessageBox::warning(this, tr("查看失败"), tr("已是最小样本号!"));
        return;
    }

}

void CustomFixTableView::on_pushButtonNext_clicked()
{
    // 基于m_SampleidList中的实际顺序导航，而不是连续的ID
    int currentIndex = -1;
    QString currentId = GlobalData::groupDateAndID(GlobalData::ObatinCreatSampleTime(), m_viewIDNum);

    // 查找当前样本在列表中的位置
    for (int i = 0; i < m_SampleidList.size(); ++i) {
        if (m_SampleidList[i] == currentId) {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex == -1) {
        QMessageBox::warning(this, tr("查看失败"), tr("当前样本不在列表中!"));
        return;
    }

    // 下一个样本
    if (currentIndex < m_SampleidList.size() - 1) {
        QString nextId = m_SampleidList[currentIndex + 1];
        QString date;
        int idNum;
        GlobalData::apartSampleId(nextId, date, idNum);
        viewOneSelf(nextId, idNum);
    }
    else {
        QMessageBox::warning(this, tr("查看失败"), tr("已是最大样本号!"));
        return;
    }
}

void CustomFixTableView::on_printButton_clicked()
{
    auto* dbInstance = FullyAutomatedPlatelets::pinstancesqlData();
    if (!dbInstance) {
        QLOG_ERROR() << "Database instance is null";
        return;
    }

    // 创建实例并设置自动删除，防止内存泄漏
    PrintTemplate *printWidget = new PrintTemplate(nullptr, dbInstance);
    printWidget->setAttribute(Qt::WA_DeleteOnClose);

    // 设置样本号并显示
    printWidget->setSampleId(m_viewIDstr);

    // 获取粘附率显示设置
    INI_File iniConfig;
    bool showAdhesion = iniConfig.getPrintAdhesion();
    printWidget->setShowAdhesionRate(showAdhesion);

    // 显示窗口
    printWidget->show();
}



void CustomFixTableView::viewOneSelf(const QString &idstr, const int &idinter)
{
    m_checkBoxSmooth->blockSignals(true);
    m_checkBoxSmooth->setChecked(false);
    m_checkBoxSmooth->blockSignals(false);

    setClickViewId(idstr, idinter);
    showResult(false);
}

void CustomFixTableView::setClickViewId(QString idstr, const int& idindex)
{
    m_viewIDstr = std::move(idstr);
    m_viewIDNum = idindex;
    if (m_labelname) {
        m_labelname->setText(QString("样本编号: %1").arg(m_viewIDstr));
    }
    if (m_titleBar) {
        m_titleBar->setTitle(QString("血小板聚集分析系统 - 样本: %1").arg(m_viewIDstr));
    }
}

void CustomFixTableView::setCheckBoxState(const bool& isstate, const QString &sampleSex,
    const QList<QString> &TableSampleidList)
{
    if (m_checkBoxSmooth) {
        m_checkBoxSmooth->blockSignals(true);
        m_checkBoxSmooth->setChecked(isstate);
        m_checkBoxSmooth->blockSignals(false);
    }
    m_sampleSex = sampleSex;
    m_SampleidList.clear();
    m_SampleidList = TableSampleidList;
}

void CustomFixTableView::initCreateCurveWidget(QCustomPlot *customPlot)
{
    if (!customPlot) {
        QLOG_WARN() << "CustomPlot pointer is null!";
        return;
    }

    customPlot->setAntialiasedElements(QCP::aeAll);
    setupSmoothInteractions(customPlot);
    setupBeautifulAxes(customPlot);
    setupElegantGrid(customPlot);
    initCreateCPGraph(customPlot);
    customPlot->replot();
}

void CustomFixTableView::setupSmoothInteractions(QCustomPlot* customPlot)
{
    customPlot->setSelectionRectMode(QCP::SelectionRectMode::srmNone);
    customPlot->setInteraction(QCP::iRangeDrag, false);
    customPlot->setInteraction(QCP::iRangeZoom, false);
    customPlot->setInteraction(QCP::iSelectPlottables, false);
    customPlot->axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    customPlot->axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);

    // 禁用拖拽和缩放
    // 禁用拖拽 - 传空的方向
    customPlot->axisRect()->setRangeDrag(Qt::Orientations());

    // 禁用缩放
    customPlot->axisRect()->setRangeZoom(Qt::Orientations());

    QFont modernFont("Segoe UI", 10);
    customPlot->setFont(modernFont);
}

void CustomFixTableView::setupBeautifulAxes(QCustomPlot* customPlot)
{
    setupMedicalAxis(customPlot->xAxis, "时间 (S)", 0, 300, 30, 10);
    setupMedicalAxis(customPlot->yAxis, "百分比 (%)", -20, 100, 10, 10);

    QPen zeroLinePen;
    zeroLinePen.setColor(QColor(0, 150, 136));
    zeroLinePen.setWidth(2);
    zeroLinePen.setStyle(Qt::DashLine);
    customPlot->xAxis->grid()->setZeroLinePen(zeroLinePen);
    customPlot->yAxis->grid()->setZeroLinePen(zeroLinePen);
}

void CustomFixTableView::setupMedicalAxis(QCPAxis* axis, const QString& label,
    double lower, double upper,
    double tickStep, int tickCount)
{
    axis->setLabel(label);
    axis->setLabelColor(QColor(0, 92, 153));
    QFont labelFont("微软雅黑", 12, QFont::Bold);
    axis->setLabelFont(labelFont);
    axis->setRange(lower, upper);

    QSharedPointer<QCPAxisTickerFixed> ticker(new QCPAxisTickerFixed);
    ticker->setTickStep(tickStep);
    ticker->setTickCount(tickCount);
    axis->setTicker(ticker);

    axis->setTickLabelColor(QColor(0, 119, 204));
    QFont tickLabelFont("微软雅黑", 10);
    axis->setTickLabelFont(tickLabelFont);

    QPen axisPen(QColor(0, 92, 153), 2, Qt::SolidLine);
    axis->setBasePen(axisPen);

    QPen tickPen(QColor(0, 150, 136), 2, Qt::SolidLine);
    axis->setTickPen(tickPen);

    QPen subTickPen(QColor(0, 119, 204), 1, Qt::SolidLine);
    axis->setSubTickPen(subTickPen);
}

void CustomFixTableView::setupElegantGrid(QCustomPlot* customPlot)
{
    QPen gridPen(QColor(179, 229, 252), 1, Qt::DotLine);
    gridPen.setCosmetic(true);
    customPlot->xAxis->grid()->setPen(gridPen);
    customPlot->yAxis->grid()->setPen(gridPen);

    QPen subGridPen(QColor(225, 245, 254), 0.5, Qt::DotLine);
    subGridPen.setCosmetic(true);
    customPlot->xAxis->grid()->setSubGridPen(subGridPen);
    customPlot->yAxis->grid()->setSubGridPen(subGridPen);
}

void CustomFixTableView::addShadowEffect(QCustomPlot* customPlot)
{
    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(customPlot);
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 60));
    shadowEffect->setOffset(3, 3);
    customPlot->setGraphicsEffect(shadowEffect);

    customPlot->axisRect()->setAutoMargins(QCP::msAll);
    customPlot->axisRect()->setMinimumMargins(QMargins(30, 20, 20, 30));
    customPlot->axisRect()->setBackground(QBrush(Qt::white));
    customPlot->axisRect()->setBackgroundScaled(false);

    QFont axisLabelFont("微软雅黑", 11, QFont::Medium);
    customPlot->xAxis->setLabelFont(axisLabelFont);
    customPlot->yAxis->setLabelFont(axisLabelFont);

    QFont tickLabelFont("Segoe UI", 10);
    customPlot->xAxis->setTickLabelFont(tickLabelFont);
    customPlot->yAxis->setTickLabelFont(tickLabelFont);

    customPlot->xAxis->setTickLabelColor(QColor(59, 73, 89));
    customPlot->yAxis->setTickLabelColor(QColor(59, 73, 89));
    customPlot->xAxis->setLabelPadding(10);
    customPlot->yAxis->setLabelPadding(15);
    customPlot->xAxis->setTickLabelPadding(5);
    customPlot->yAxis->setTickLabelPadding(8);
}

void CustomFixTableView::initCreateCPGraph(QCustomPlot* pshowcurvedata)
{
    struct CurveConfig {
        QCPGraph** graphPtr;
        quint8 reagent;
        QString name;
    };

    const QList<CurveConfig> curveConfigs = {
        { &m_showAACpgraph,  AA_REAGENT, "AA" },
        { &m_showADPCpgraph, ADP_REAGENT, "ADP" },
        { &m_showEPICpgraph, EPI_REAGENT, "EPI" },
        { &m_showCOLCpgraph, COL_REAGENT, "COL" },
        { &m_showRISCpgraph, RIS_REAGENT, "RIS" }
    };

    for (const auto& config : curveConfigs) {
        *config.graphPtr = pshowcurvedata->addGraph();
        QCPGraph* graph = *config.graphPtr;
        graph->setPen(QPen(GlobalData::customCurveColor(config.reagent), 2, Qt::SolidLine));
        graph->setName(config.name);
        graph->setLineStyle(QCPGraph::lsLine);
        graph->setAntialiased(true);
    }

    pshowcurvedata->legend->setVisible(false);
    pshowcurvedata->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignLeft);
    pshowcurvedata->legend->setBrush(QColor(255, 255, 255, 125));
    pshowcurvedata->replot();
}

void CustomFixTableView::CreateResultCurve()
{
    if (!m_widgetCurveShow) {
        QLOG_WARN() << "CreateResultCurve: m_widgetCurveShow is null";
        return;
    }

    clearAuxiliaryItems();

    for (QCPGraph* graph : calibrationGraphs()) {
        clearGraphData(graph);
    }

    m_widgetCurveShow->replot(QCustomPlot::rpQueuedReplot);
}

void CustomFixTableView::clearResultTable(QTableWidget *pTable)
{
    for (int row = 0; row < pTable->rowCount(); ++row) {
        for (int col = static_cast<int>(TableItemnum::Aggregation60s);
        col <= static_cast<int>(TableItemnum::AUC); ++col) {
            insertColumnText(pTable, row, col, "");
        }
        insertColumnText(pTable, row, static_cast<int>(TableItemnum::OutResult), "");
    }
}

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

    graph->data()->clear();
    graph->data().data()->squeeze();
}

QList<QCPGraph*> CustomFixTableView::calibrationGraphs() const
{
    QList<QCPGraph*> validGraphs;
    if (m_showAACpgraph) validGraphs.append(m_showAACpgraph);
    if (m_showADPCpgraph) validGraphs.append(m_showADPCpgraph);
    if (m_showEPICpgraph) validGraphs.append(m_showEPICpgraph);
    if (m_showCOLCpgraph) validGraphs.append(m_showCOLCpgraph);
    if (m_showRISCpgraph) validGraphs.append(m_showRISCpgraph);
    return validGraphs;
}

QPair<double, double> CustomFixTableView::markMaxAggregation(const QVector<double>& data, QCPGraph* graph)
{
    if (data.isEmpty() || !graph || !m_widgetCurveShow) {
        return qMakePair(0.0, 0.0);
    }

    double maxAggregation = -std::numeric_limits<double>::max();
    int maxIndex = 0;

    for (int i = 0; i < data.size(); ++i) {
        if (data[i] > maxAggregation) {
            maxAggregation = data[i];
            maxIndex = i;
        }
    }

    // 删除旧的 tracer 如果存在 - 使用安全宏
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_maxAggregationTracer);

    m_maxAggregationTracer = new QCPItemTracer(m_widgetCurveShow);
    m_maxAggregationTracer->setGraph(graph);
    m_maxAggregationTracer->setGraphKey(maxIndex);
    m_maxAggregationTracer->setStyle(QCPItemTracer::tsCircle);
    m_maxAggregationTracer->setPen(QPen(Qt::red, 3));
    m_maxAggregationTracer->setBrush(QBrush(Qt::yellow));
    m_maxAggregationTracer->setSize(10);

    // 删除旧的 label 如果存在 - 使用安全宏
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_maxAggregationLabel);

    m_maxAggregationLabel = new QCPItemText(m_widgetCurveShow);
    m_maxAggregationLabel->position->setParentAnchor(m_maxAggregationTracer->position);
    m_maxAggregationLabel->position->setCoords(0, -15);
    m_maxAggregationLabel->setText(QString("最大: %1%").arg(maxAggregation, 0, 'f', 1));
    m_maxAggregationLabel->setFont(QFont("微软雅黑", 9, QFont::Bold));
    m_maxAggregationLabel->setPen(QPen(Qt::black));
    m_maxAggregationLabel->setBrush(QBrush(QColor(255, 255, 255, 200)));

    return qMakePair(maxAggregation, static_cast<double>(maxIndex));
}

double CustomFixTableView::calculateAndDrawAUC(const QVector<double>& data, QCPGraph* graph)
{
    if (data.isEmpty() || !graph || !m_widgetCurveShow) {
        return 0.0;
    }

    double auc = 0.0;
    for (int i = 0; i < data.size() - 1; ++i) {
        double area = (data[i] + data[i + 1]) / 2.0;
        auc += area;
    }

    m_tempXData.clear();
    m_tempYData.clear();
    m_tempXData.reserve(data.size() * 2);
    m_tempYData.reserve(data.size() * 2);

    for (int i = 0; i < data.size(); ++i) {
        m_tempXData.append(i);
        m_tempYData.append(data[i]);
    }

    for (int i = data.size() - 1; i >= 0; --i) {
        m_tempXData.append(i);
        m_tempYData.append(0);
    }

    QCPGraph* fillGraph = m_widgetCurveShow->addGraph();
    fillGraph->setPen(QPen(Qt::NoPen));
    fillGraph->setBrush(QBrush(QColor(10, 150, 136, 60)));
    fillGraph->setVisible(m_checkBoxAUC && m_checkBoxAUC->isChecked());
    fillGraph->setData(m_tempXData, m_tempYData);
    m_aucFillGraphs.append(fillGraph);

    return auc;
}

void CustomFixTableView::clearAuxiliaryItems()
{
    // 先检查 m_widgetCurveShow 是否有效
    if (!m_widgetCurveShow) {
        // 即使控件为空，也要将指针置空
        m_maxAggregationTracer = nullptr;
        m_maxAggregationLabel = nullptr;
        m_slopeLine = nullptr;
        m_slopeLabel = nullptr;
        m_slopeStartTracer = nullptr;
        m_slopeEndTracer = nullptr;
        m_lagTimeTracer = nullptr;
        m_lagTimeLine = nullptr;
        m_lagTimeLabel = nullptr;
        m_verticalLine = nullptr;
        m_dataPointTracer = nullptr;
        m_dataPointLabel = nullptr;
        return;
    }

    // 删除重复的宏定义！直接使用头文件中定义的宏
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_maxAggregationTracer);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_maxAggregationLabel);

    // 清除AUC填充区域
    for (QCPGraph* graph : m_aucFillGraphs) {
        if (graph && m_widgetCurveShow) {
            try {
                // removeGraph 默认会删除图形，不需要手动 delete
                m_widgetCurveShow->removeGraph(graph);
            }
            catch (...) {
                // 忽略删除异常
            }
        }
    }
    m_aucFillGraphs.clear();

    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_slopeLine);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_slopeLabel);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_slopeStartTracer);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_slopeEndTracer);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_lagTimeTracer);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_lagTimeLine);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_lagTimeLabel);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_verticalLine);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_dataPointTracer);
    SAFE_DELETE_CURVE_ITEM(m_widgetCurveShow, m_dataPointLabel);
}

QPair<int, int> CustomFixTableView::findSteepestSegment(const QVector<double>& data, int windowSize)
{
    if (data.size() < windowSize + 1) {
        return qMakePair(0, data.size() - 1);
    }

    double maxSlope = -std::numeric_limits<double>::max();
    int bestStart = 0;
    int bestEnd = windowSize;

    for (int i = 0; i <= data.size() - windowSize - 1; ++i) {
        int start = i;
        int end = i + windowSize;

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

    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    int n = end - start + 1;

    for (int i = start; i <= end; ++i) {
        double x = i - start;
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
    if (!graph || !m_widgetCurveShow || data.size() < 20) return 0;

    auto segment = findSteepestSegment(data, SLOPE_WINDOW_SIZE);
    int start = segment.first;
    int end = segment.second;

    if (start >= end) return 0;

    double slopePerSecond = calculateSlope(data, start, end);
    double slopePerMinute = slopePerSecond * 60.0;
    double intercept = data[start] - slopePerSecond * start;

    int extendedStart = qMax(0, start - 5);
    int extendedEnd = qMin(data.size() - 1, end + 5);

    double y1 = slopePerSecond * extendedStart + intercept;
    double y2 = slopePerSecond * extendedEnd + intercept;
    y1 = qMax(-20.0, qMin(100.0, y1));
    y2 = qMax(-20.0, qMin(100.0, y2));

    // 修改前：m_slopeLine.reset(new QCPItemStraightLine(m_widgetCurveShow));
    // 修改后：
    if (!m_slopeLine) {
        m_slopeLine = new QCPItemStraightLine(m_widgetCurveShow);
    }
    m_slopeLine->point1->setCoords(extendedStart, y1);
    m_slopeLine->point2->setCoords(extendedEnd, y2);
    m_slopeLine->setPen(QPen(QColor(0, 150, 0), 2, Qt::DashLine));

    // 修改前：m_slopeStartTracer.reset(new QCPItemTracer(m_widgetCurveShow));
    // 修改后：
    if (!m_slopeStartTracer) {
        m_slopeStartTracer = new QCPItemTracer(m_widgetCurveShow);
    }
    m_slopeStartTracer->position->setCoords(start, data[start]);
    m_slopeStartTracer->setStyle(QCPItemTracer::tsSquare);
    m_slopeStartTracer->setPen(QPen(Qt::darkGreen, 2));
    m_slopeStartTracer->setBrush(QBrush(Qt::green));
    m_slopeStartTracer->setSize(6);

    // 修改前：m_slopeEndTracer.reset(new QCPItemTracer(m_widgetCurveShow));
    // 修改后：
    if (!m_slopeEndTracer) {
        m_slopeEndTracer = new QCPItemTracer(m_widgetCurveShow);
    }
    m_slopeEndTracer->position->setCoords(end, data[end]);
    m_slopeEndTracer->setStyle(QCPItemTracer::tsSquare);
    m_slopeEndTracer->setPen(QPen(Qt::darkGreen, 2));
    m_slopeEndTracer->setBrush(QBrush(Qt::green));
    m_slopeEndTracer->setSize(6);

    // 修改前：m_slopeLabel.reset(new QCPItemText(m_widgetCurveShow));
    // 修改后：
    if (!m_slopeLabel) {
        m_slopeLabel = new QCPItemText(m_widgetCurveShow);
    }
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

    // 修改前：if (!m_verticalLine) { m_verticalLine.reset(new QCPItemStraightLine(m_widgetCurveShow)); }
    // 修改后：
    if (!m_verticalLine) {
        m_verticalLine = new QCPItemStraightLine(m_widgetCurveShow);
        m_verticalLine->setPen(QPen(QColor(0, 0, 255), 1, Qt::DashLine));
        m_verticalLine->setVisible(false);
    }

    // 修改前：if (!m_dataPointTracer) { m_dataPointTracer.reset(new QCPItemTracer(m_widgetCurveShow)); }
    // 修改后：
    if (!m_dataPointTracer) {
        m_dataPointTracer = new QCPItemTracer(m_widgetCurveShow);
        m_dataPointTracer->setStyle(QCPItemTracer::tsCircle);
        m_dataPointTracer->setPen(QPen(Qt::red, 2));
        m_dataPointTracer->setBrush(QBrush(Qt::yellow));
        m_dataPointTracer->setSize(6);
        m_dataPointTracer->setVisible(false);
    }

    // 修改前：if (!m_dataPointLabel) { m_dataPointLabel.reset(new QCPItemText(m_widgetCurveShow)); }
    // 修改后：
    if (!m_dataPointLabel) {
        m_dataPointLabel = new QCPItemText(m_widgetCurveShow);
        m_dataPointLabel->setPositionAlignment(Qt::AlignLeft | Qt::AlignTop);
        m_dataPointLabel->setFont(QFont("微软雅黑", 8));
        m_dataPointLabel->setPen(QPen(Qt::blue));
        m_dataPointLabel->setBrush(QBrush(QColor(255, 255, 255, 200)));
        m_dataPointLabel->setPadding(QMargins(5, 3, 5, 3));
        m_dataPointLabel->position->setParentAnchor(m_dataPointTracer->position);
        m_dataPointLabel->position->setCoords(5, -15);
        m_dataPointLabel->setVisible(false);
    }

    m_widgetCurveShow->setMouseTracking(true);
    connect(m_widgetCurveShow, &QCustomPlot::mouseMove, this, &CustomFixTableView::onCurveMouseMove);

    QLOG_INFO() << "最陡峭斜率:" << slopePerMinute << "%/min"
        << "(每秒斜率:" << slopePerSecond << "%/s)"
        << "时间段:" << start << "-" << end << "秒";

    return slopePerMinute;
}

double CustomFixTableView::calculateLagTime(const QVector<double>& data)
{
    if (data.isEmpty()) return 0.0;

    auto &ini = INI_File();
    bool isWholeBlood = ini.GetWholeBloodModel();

    const double baselineThreshold = isWholeBlood ? 3.0 : 2.0;
    const int minRiseDuration = isWholeBlood ? 5 : 4;
    const double riseThreshold = isWholeBlood ? 1.0 : 0.5;
    const double maxDropReset = isWholeBlood ? 2.0 : 1.0;

    int validCount = 0;
    for (const auto& value : data) {
        if (!std::isnan(value)) validCount++;
    }
    if (validCount < MIN_DATA_POINTS) return 0.0;

    bool isRising = false;
    int riseStartIndex = -1;
    int consecutiveRiseCount = 0;
    double previousPeak = 0.0;

    for (int i = 1; i < data.size(); ++i) {
        if (std::isnan(data[i]) || std::isnan(data[i - 1])) continue;

        double currentValue = data[i];
        double previousValue = data[i - 1];
        double difference = currentValue - previousValue;

        if (!isRising && currentValue > baselineThreshold) {
            return i / 60.0;
        }

        if (!isRising) {
            if (difference > riseThreshold) {
                isRising = true;
                riseStartIndex = i - 1;
                consecutiveRiseCount = 1;
                previousPeak = currentValue;
            }
        }
        else {
            if (difference > 0) {
                consecutiveRiseCount++;
                previousPeak = currentValue;
            }
            else {
                double dropAmount = previousPeak - currentValue;
                if (dropAmount <= maxDropReset && consecutiveRiseCount > 0) {
                    consecutiveRiseCount++;
                    if (currentValue > previousPeak) {
                        previousPeak = currentValue;
                    }
                }
                else {
                    isRising = false;
                    consecutiveRiseCount = 0;
                    riseStartIndex = -1;
                }
            }
        }

        if (consecutiveRiseCount >= minRiseDuration) {
            double totalRise = data[i] - data[riseStartIndex];
            double minTotalRise = isWholeBlood ? 3.0 : 2.0;

            if (totalRise >= minTotalRise) {
                return riseStartIndex / 60.0;
            }
            else {
                isRising = false;
                consecutiveRiseCount = 0;
                riseStartIndex = -1;
            }
        }

        if (i > 120) {
            double recentMax = *std::max_element(data.begin() + i - 10, data.begin() + i);
            if (recentMax < baselineThreshold / 2) {
                return 0.0;
            }
        }
    }

    if (riseStartIndex != -1 && consecutiveRiseCount >= minRiseDuration / 2) {
        double finalRise = data.back() - data[riseStartIndex];
        if (finalRise > baselineThreshold) {
            return riseStartIndex / 60.0;
        }
    }

    return 0.0;
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

    insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::Slope),
        QString::number(slope, 'f', 2));
    insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::TMAtime),
        QString::number(timeToMax, 'f', 2));
    insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::LagTime),
        QString::number(lagTime, 'f', 2));
    insertColumnText(m_tableWidget, row, static_cast<int>(TableItemnum::AUC),
        QString::number(auc, 'f', 2));
}

void CustomFixTableView::onCurveMouseMove(QMouseEvent* event)
{
    if (!m_verticalLine || !m_dataPointTracer || !m_dataPointLabel || !m_widgetCurveShow) {
        return;
    }

    double x = m_widgetCurveShow->xAxis->pixelToCoord(event->pos().x());

    if (x < 0 || x > CURVE_DATA_SIZE - 1) {
        m_verticalLine->setVisible(false);
        m_dataPointTracer->setVisible(false);
        m_dataPointLabel->setVisible(false);
        m_widgetCurveShow->replot();
        return;
    }

    int index = qRound(x);
    index = qBound(0, index, m_currentCurveData.size() - 1);

    double aggregationValue = m_currentCurveData.value(index, 0.0);

    m_verticalLine->point1->setCoords(x, 0);
    m_verticalLine->point2->setCoords(x, 100);
    m_verticalLine->setVisible(true);

    m_dataPointTracer->position->setCoords(x, aggregationValue);
    m_dataPointTracer->setVisible(true);

    m_dataPointLabel->setText(QString("时间: %1 s\n聚集率: %2 %")
        .arg(x, 0, 'f', 1)
        .arg(aggregationValue, 0, 'f', 1));
    m_dataPointLabel->setVisible(true);

    m_widgetCurveShow->replot();
}
