#include "genericfunctions.h"
#include "testing.h"
#include "ui_testing.h"
#include <QScrollBar>
#include <QTreeWidgetItem>
#include <QCoreApplication>
#include <math.h>
#include <QTimer>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include <QDesktopWidget>
#include <string.h>
#include <QPainter>
#include <QMouseEvent>
#include <QQueue>
#include <QtConcurrent>
#include "globaldata.h"
#include "globaldata.h"
#include "loginui.h"
#include "quiutils.h"
#include <operclass/fullyautomatedplatelets.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

using namespace std;
UsbCodeDispose* Testing::m_TaskDll = nullptr;

Testing::Testing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Testing),
    mInituiBloodArea(false),
    alreadyinitchannelui(false),
    m_ProTotalTube(0),
    m_ThrowTube(0),
    m_blinkState(false)

{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    mflashingTubeList.clear();
    m_TaskDll = new UsbCodeDispose;
    mreminderPPPandPRPhole.clear();
}

Testing::~Testing()
{

    delete m_TaskDll;
    m_TaskDll = nullptr;


    if (mtimer) {
        if (mtimer->isActive())
            mtimer->stop();
        delete mtimer;
        mtimer = nullptr;
    }
    delete ui;
}

void Testing::initequipmentKind(const quint8 indextype)
{
    mtimer = new QTimer(this);
    connect(mtimer, &QTimer::timeout, this, &Testing::toggleBlinkState);
    mtimer->start(500);

    minstrumentType = indextype;

    memset(mInitEmptyArea, false, sizeof(mInitEmptyArea));

    mEmptyTubeAssigned.clear();
    mEmptyTubeAbsorb_Poorblood.clear();
    mEmptyTubeAbsorb_Richblood.clear();
    m_TestingSample.clear();
    mEmptyText.clear();
    m_Blood_Tray_Checked.clear();
    m_Test_Tray_has_hole.clear();  //已使用得空试管孔

    initializeMachineUI(minstrumentType); /*初始化测试界面测试通道*/

    //初始化添加样本对话框
    initTheTaskInterface();

}

void  Testing::toggleBlinkState(){
    m_blinkState = !m_blinkState; // 切换状态
    update();
}


void Testing::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    mInituiBloodArea = false;

    // 先获取大框的宽和高
    const int appWidth = ui->widget_main->size().width();
    const int appHeigh = ui->widget_main->size().height();

    // 间距
    const int boardSpaceX = 5;
    const int boardSpaceY = 5;

    int ChnBarHeight = 155;
    int ThrowCupWidth = 120;
    int ThrowCupHeight = boardSpaceY - 5;
    int Left_width = appWidth - boardSpaceX * 2 - 2 * 2;

    // 清除 widget_channelarea 的布局
    if (ui->widget_channelarea->layout()) {
        QLayoutItem* item;
        while ((item = ui->widget_channelarea->layout()->takeAt(0)) != nullptr) {
            delete item;
        }
        delete ui->widget_channelarea->layout();
    }

    // 测试通道区域
    ui->widget_channelarea->setGeometry(boardSpaceX, boardSpaceY, Left_width, ChnBarHeight);

    // 获取 channelarea 的实际尺寸
    int channelAreaWidth = ui->widget_channelarea->width();
    int channelAreaHeight = ui->widget_channelarea->height();

    // 弃杯孔控件尺寸
    int dropCupWidth = 80;
    int dropCupHeight = 120;
    int dropCupLeftMargin = 15;   // DroptheCup 与模块的间距

    // 计算模块可用的宽度（减去 DroptheCup 占用的空间和间距）
    int availableForModulesWidth = channelAreaWidth - dropCupWidth - dropCupLeftMargin;

    // 血样区和试管盘高度
    int BloodAndTrayTubeHeight = appHeigh - ui->widget_channelarea->height() - boardSpaceY * 3 - 4 * 2;
    int TrayTubeHeight = BloodAndTrayTubeHeight / 3 * 2;
    int taryTopy = ui->widget_channelarea->geometry().bottomLeft().y() + boardSpaceY;
    ui->widget_testcup->setGeometry(boardSpaceX, taryTopy, Left_width, TrayTubeHeight);

    // 血样管区域
    int bloodBottomLoc_topy = ui->widget_testcup->geometry().bottomLeft().y() + boardSpaceY;
    int BloodSampleHeight = appHeigh - ui->widget_channelarea->height() - ui->widget_testcup->height() - 3 * boardSpaceY;
    ui->widget_Sample_1->setFixedHeight(BloodSampleHeight);
    ui->widget_Sample_1->setFixedWidth(Left_width);
    ui->widget_Sample_1->setGeometry(boardSpaceX, bloodBottomLoc_topy, Left_width, BloodSampleHeight);

    // 动态计算试管盘大小
    constexpr int bigSpace = 29;
    int ColWidth = (Left_width - 5) / bigSpace;
    int TaryTubeWidth = ColWidth * 6;

    // 根据试管盘数量动态调整宽度，确保居中
    int visibleTraysCount = 0;
    switch (minstrumentType)
    {
        case KS600: visibleTraysCount = 2; break;
        case KS800: visibleTraysCount = 3; break;
        case KS1200: visibleTraysCount = 4; break;
        default: visibleTraysCount = 2; break;
    }

    // 重新计算试管盘宽度，使其能完美居中
    int maxTrayWidth = (Left_width - (visibleTraysCount - 1) * boardSpaceX) / visibleTraysCount;
    int finalTrayWidth = qMin(TaryTubeWidth, maxTrayWidth);

    QList<QWidget*> trayWidgets = {ui->widget_TestCup_0, ui->widget_TestCup_1,
                                    ui->widget_TestCup_2, ui->widget_TestCup_3};

    // 计算试管盘总宽度和起始位置
    int totalTraysWidth = visibleTraysCount * finalTrayWidth + (visibleTraysCount - 1) * boardSpaceX;
    int startTrayX = (Left_width - totalTraysWidth) / 2;

    for (int i = 0; i < trayWidgets.size(); ++i) {
        if (i < visibleTraysCount) {
            trayWidgets[i]->setFixedSize(finalTrayWidth, TrayTubeHeight);
            int x = startTrayX + i * (finalTrayWidth + boardSpaceX);
            trayWidgets[i]->setGeometry(x, 0, finalTrayWidth, TrayTubeHeight);
            trayWidgets[i]->show();
        } else {
            trayWidgets[i]->hide();
        }
    }

    // ========== 通道模块布局 - ProgressBar 尽可能大 ==========
    constexpr int moduleSpacing = 10;           // 模块之间的间距
    constexpr int progressBarSpacing = 10;      // ProgressBar 之间的间距
    constexpr int progressBarsPerModule = 4;    // 每个模块的 ProgressBar 数量

    // 标签相关尺寸
    constexpr int labelHeight = 28;              // 文字标签高度
    constexpr int labelTopMargin = 2;            // 标签上边距（与 ProgressBar 的间距）- 减小为2px
    constexpr int moduleTopMargin = 4;           // 模块上边距 - 减小为4px
    constexpr int moduleBottomMargin = 4;        // 模块下边距 - 减小为4px

    // 计算 ProgressBar 可用的最大尺寸
    // 可用高度 = channelarea 总高度 - 上下边距 - 标签高度 - 标签上边距
    int availableHeightForBar = channelAreaHeight - moduleTopMargin - moduleBottomMargin - labelHeight - labelTopMargin;

    // 根据型号确定模块数量
    int modulesCount = 0;
    int startTubeForUpdate = 0;
    int endTubeForUpdate = 0;

    switch (minstrumentType)
    {
        case KS600:
            modulesCount = 1;
            startTubeForUpdate = 120;
            endTubeForUpdate = 240;
            break;
        case KS800:
            modulesCount = 2;
            startTubeForUpdate = 180;
            endTubeForUpdate = 240;
            break;
        case KS1200:
            modulesCount = 3;
            startTubeForUpdate = 0;
            endTubeForUpdate = 0;
            break;
        default:
            modulesCount = 1;
            startTubeForUpdate = 120;
            endTubeForUpdate = 240;
            break;
    }

    // 计算每个模块的最大可用宽度（考虑 DroptheCup 占用的空间）
    int totalSpacingForBars = (progressBarsPerModule - 1) * progressBarSpacing;
    int maxModuleWidth = (availableForModulesWidth - (modulesCount - 1) * moduleSpacing) / modulesCount;
    int maxBarWidthByWidth = (maxModuleWidth - totalSpacingForBars) / progressBarsPerModule;

    // 取高度限制和宽度限制的最小值
    int progressBarSize = qMin(availableHeightForBar, maxBarWidthByWidth);
    // 设置最小尺寸限制
    progressBarSize = qMax(progressBarSize, 65);

    // 重新计算模块宽度
    int moduleWidth = progressBarsPerModule * progressBarSize + (progressBarsPerModule - 1) * progressBarSpacing;
    int moduleHeight = moduleTopMargin + progressBarSize + labelTopMargin + labelHeight + moduleBottomMargin;

    // 确保模块高度不超过 channelarea 高度
    moduleHeight = qMin(moduleHeight, channelAreaHeight);

    // 布局模块
    QList<QWidget*> modules = {ui->widget_Module1, ui->widget_Module2, ui->widget_Module3};

    // 设置所有模块的大小
    for (int i = 0; i < modules.size(); ++i) {
        if (i < modulesCount) {
            modules[i]->setFixedSize(moduleWidth, moduleHeight);
            modules[i]->show();
        } else {
            modules[i]->hide();
        }
    }

    // 计算模块起始位置（在可用宽度内水平居中，整体垂直居中）
    int totalModulesWidth = modulesCount * moduleWidth + (modulesCount - 1) * moduleSpacing;
    int startX = (availableForModulesWidth - totalModulesWidth) / 2;  // 在可用宽度内居中
    int startY = (channelAreaHeight - moduleHeight) / 2;  // 垂直居中

    // 设置弃杯孔位置（在 widget_channelarea 的最右侧）
    int dropCupX = channelAreaWidth - dropCupWidth;  // 最右侧
    int dropCupY = (channelAreaHeight - dropCupHeight) / 2;  // 垂直居中

    // 确保弃杯孔是 widget_channelarea 的直接子控件
    ui->DroptheCup->setParent(ui->widget_channelarea);
    ui->DroptheCup->setGeometry(dropCupX, dropCupY, dropCupWidth, dropCupHeight);
    ui->DroptheCup->show();

    // 设置模块位置（在弃杯孔左侧居中显示）
    for (int i = 0; i < modulesCount; ++i) {
        int x = startX + i * (moduleWidth + moduleSpacing);
        modules[i]->setGeometry(x, startY, moduleWidth, moduleHeight);
    }

    // 更新模块内部的 ProgressBar 和标签的位置和大小
    for (int i = 0; i < modulesCount; ++i) {
        QList<ProgressBar*> progressBars = modules[i]->findChildren<ProgressBar*>();
        QList<QLabel*> labels = modules[i]->findChildren<QLabel*>();

        // 按名称排序确保顺序正确（Channel_bock_1, Channel_bock_2, ...）
        std::sort(progressBars.begin(), progressBars.end(),
            [](ProgressBar* a, ProgressBar* b) {
                return a->objectName() < b->objectName();
            });
        std::sort(labels.begin(), labels.end(),
            [](QLabel* a, QLabel* b) {
                return a->objectName() < b->objectName();
            });

        // 设置 ProgressBar 大小和位置
        for (int j = 0; j < progressBars.size() && j < progressBarsPerModule; ++j) {
            ProgressBar* bar = progressBars[j];
            if (bar) {
                bar->setFixedSize(progressBarSize, progressBarSize);
                int barX = j * (progressBarSize + progressBarSpacing);
                bar->setGeometry(barX, moduleTopMargin, progressBarSize, progressBarSize);

                // 设置 ProgressBar 样式
                int borderRadius = qMax(progressBarSize / 10, 5);
                bar->setStyleSheet(QString(
                    "ProgressBar {"
                    "   background-color: #E8ECF0;"
                    "   border-radius: %1px;"
                    "}"
                    "ProgressBar::chunk {"
                    "   background-color: #5B9BD5;"
                    "   border-radius: %1px;"
                    "}"
                ).arg(borderRadius));
            }
        }

        // 设置标签位置和样式 - 显示在 ProgressBar 下方，减小垂直间距
        for (int j = 0; j < labels.size() && j < progressBarsPerModule; ++j) {
            QLabel* label = labels[j];
            if (label) {
                int labelX = j * (progressBarSize + progressBarSpacing);
                // 减小标签与 ProgressBar 之间的垂直间距（从 8px 减小到 2px）
                int labelY = moduleTopMargin + progressBarSize + 2;
                label->setGeometry(labelX, labelY, progressBarSize, labelHeight);

                // 设置标签样式 - 增大字体大小，使其显示更清晰
                label->setStyleSheet(
                    "QLabel {"
                    "   font-family: 'Microsoft YaHei';"
                    "   font-size: 13px;"
                    "   font-weight: 600;"
                    "   color: #2C3035;"
                    "   background: transparent;"
                    "}"
                );
                label->setAlignment(Qt::AlignCenter);
                label->setWordWrap(true);
            }
        }
    }
    // ========== 通道模块布局结束 ==========

    // 更新数据库中的试管状态（KS1200 不需要）
    if (startTubeForUpdate < endTubeForUpdate) {
        for (int i = startTubeForUpdate; i < endTubeForUpdate; ++i) {
            FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(i, TESTTUBES_CLIPPEDAWAY);
        }
    }

    return;
}


void Testing::initControlShowChannelProgress(quint8 startChannel,
                                                QWidget * progressChannel,
                                                QWidget *ptestChannel,
                                                const QPalette& pa){

    QList<ProgressBar*> channelControl = progressChannel->findChildren<ProgressBar* >();
    QList<QLabel *> chnStateTextList = ptestChannel->findChildren<QLabel* >();

    //安全检查
    if(channelControl.size() != chnStateTextList.size()){
        QLOG_WARN()<<"通道显示进度控件和提示测试Labels数量不匹配!";
        return;
    }


    // 使用局部变量避免修改外部参数
    quint8 currentChannel = startChannel;

    for(int i = 0; i < channelControl.size(); ++i){
        ProgressBar* progressBar = channelControl[i];
        QLabel* label = chnStateTextList[i];

        //初始化进度条
        progressBar->flashingReminder(true);
        progressBar->setRange(0, 100);
        progressBar->installEventFilter(this);

        //配置通道文本
        const QString chnStateKey = QString("TestTheChanne1Opening_%1").arg(currentChannel + 1);
        bool usedChannel = INI_File().rConfigPara(chnStateKey).toBool();
        progressBar->setChnTextindex(usedChannel ? currentChannel + 1 : CHN_DISABLE);

        // 存储引用（使用原始指针但明确所有权）
        m_channelShowsTheProgress.append(progressBar);
        Channelreminder.append(label);

        // 初始化标签
        if (!alreadyinitchannelui) {
            label->setPalette(pa);
            label->hide();
        }

        ++currentChannel;
    }

}

void Testing::initializeMachineUI(const quint8 equipmentIndex)
{
    QPalette pa;
    pa.setColor(QPalette::WindowText, QColor(250, 128, 114));
    QList<QWidget *> widgetFilterList = { ui->widget_Reagents ,
                                            ui->widget_Abandoned_new,
                                            ui->widget_Sample_1,
                                            ui->widget_TestCup_3,
                                            ui->widget_TestCup_2,
                                            ui->widget_TestCup_1,
                                            ui->widget_TestCup_0,
                                            ui->DroptheCup,
                                            ui->widget_cleanagent
    };
    for (auto pwidget : widgetFilterList)
        pwidget->installEventFilter(this);//设置过滤器

    initControlShowChannelProgress(0,ui->widget_Module1,ui->widget_Module1,pa);
    initControlShowChannelProgress(4,ui->widget_Module2,ui->widget_Module2,pa);
    initControlShowChannelProgress(8,ui->widget_Module3,ui->widget_Module3,pa);

    const std::unordered_map<int, ModuleVisibility> configMap = {
        {KS600,  {true,  false, false}},
        {KS800,  {true,  true,  false}},
        {KS1200, {true,  true,  true}}
    };
    auto it = configMap.find(equipmentIndex);
    if (it != configMap.end()) {
        const auto& config = it->second;
        ui->widget_Module1->setVisible(config.showModule1);
        ui->widget_Module2->setVisible(config.showModule2);
        ui->widget_Module3->setVisible(config.showModule3);
    } else {
        QLOG_WARN() << "Unknown equipment index:" << equipmentIndex;
    }
    alreadyinitchannelui = true;
    return;
}


void Testing::updateChannelProgressAndStatus(bool isWaitstate, quint8 channelIndex, const QString &status)
{
    // 防御性检查
    if (channelIndex >= m_channelShowsTheProgress.size() ||
        channelIndex >= Channelreminder.size()) {
        QLOG_WARN()<<("Invalid channel index: %d", channelIndex);
        return;
    }

    // 检查指针有效性
    if (!m_channelShowsTheProgress[channelIndex] || !Channelreminder[channelIndex]) {
        QLOG_WARN() << "Null pointer for channel index:" << channelIndex;
        return;
    }

    // 重置进度条
    m_channelShowsTheProgress[channelIndex]->setValue(0);

    QLabel* reminderLabel = Channelreminder[channelIndex];
    QString currentText = reminderLabel->text();

    // 从当前文本中提取样本号
    QRegularExpression rx("样本:(\\d+).*");
    QRegularExpressionMatch match = rx.match(currentText);
    QString sampleNumber = "0";

    if (match.hasMatch()) {
        sampleNumber = match.captured(1);
     }


    // 根据状态设置文本
    QString outText;
    if (isWaitstate) {
         outText = tr("样本:%1 [等待]").arg(sampleNumber);
    } else {
        // 正常状态显示：样本:123 [状态]
        outText = tr("样本:%1 [%2]").arg(sampleNumber).arg(status);
    }

    // 只有当文本确实改变时才更新
    if (reminderLabel->text() != outText) {
        reminderLabel->setText(outText);
    }

    reminderLabel->show();
    reminderLabel->update();

    if(!isWaitstate){
        QTimer::singleShot(3000, reminderLabel, [reminderLabel]() { reminderLabel->hide();});
    }
}






void Testing::slot_sycn_SampleTestingChangInitColor(QPoint maphole,quint8 indexChn)
{
    SampleTestingChangInitColor(maphole,indexChn);
}



/*通道禁用主界面显示*/
void  Testing::recv_NotifyChannleState(const quint8 channelIndexNum,const bool bUseing)
{
    int index = channelIndexNum - 1;
    if (bUseing) {
        m_channelShowsTheProgress.at(index)->setChnTextindex(channelIndexNum);
    } else {
        m_channelShowsTheProgress.at(index)->setChnTextindex(CHN_DISABLE);
    }
    return;
}

void Testing::showTestChannelInfo(const quint8& channelIndex,const QString& sampleName,
                                    const quint8&  reagentIndex){
    if (!this) {
        QLOG_FATAL() << "Parent object destroyed!";
        return;
    }

    if (Channelreminder.isEmpty()) {
        QLOG_WARN() << "通道提示文字为NULL";
        return;
    }

    const int idx = static_cast<int>(channelIndex - 1);
    if (idx >= Channelreminder.size()) return;

    //智能指针检查
   QPointer<QLabel> channelLabel = Channelreminder[idx];
   if (channelLabel.isNull()) {  // 自动检测对象是否被销毁
       QLOG_DEBUG() << "Label at index" << channelIndex-1 << "already destroyed";
       return;
   }

    // 样本ID解析（移除未使用的idDate变量）
    int sampleNumber = 0;
    QString unusedDate;
    GlobalData::apartSampleId(sampleName, unusedDate, sampleNumber);


   // 试剂名称映射（添加默认值兜底）
   const QString reagentName = GlobalData::mapIndexReagentnames(reagentIndex).isEmpty()
                              ? QStringLiteral("Unknown_Reagent")
                              : GlobalData::mapIndexReagentnames(reagentIndex);

   // 格式化输出文本（预分配内存避免多次拼接）
   const QString displayText = tr("样本:%1[%2]").arg(sampleNumber).arg(reagentName);

   QLabel* label = Channelreminder[idx];
   if (!label) {
      QLOG_WARN() << "Null label at index:" << idx;
      return;
  }
   if (label->text() != displayText) {
        label->setText(displayText);

       // 如果标签是隐藏的，才显示它
       if (!label->isVisible()) {
           label->show();
       }
   }

   // 更新样本-通道映射（使用insert直接覆盖旧值）
   m_TestingSample.insert(sampleName, channelIndex);
}


bool Testing::handlePaintEvents(QObject *watched, QEvent *event){
    if(watched == ui->widget_Reagents){
        plotReagentWellsCoordinates(REAGENT_TOTAL * 2); //参数为孔的数量
        showPaintReagents();                            //响应函数 绘制试剂
        return true;
    }

    if(watched == ui->widget_Abandoned_new){
        showCleaningbit();   /*绘制清洗位*/
        return true;
    }

    if(watched == ui->widget_Sample_1){
        if (!mInituiBloodArea){
            CreatBloodZoneAxisPos();
            mInituiBloodArea = true;
        }
        DrawBloodTopText(); //绘制血样区头部文字
        UpdateBloodHoleColors(TUBE_INIT,      m_BloodHoleInitUiAxis);
        UpdateBloodHoleColors(TUBE_CHECKED,   m_Blood_Tray_Checked);
        UpdateBloodHoleColors(TUBE_OUTRESULT, m_Blood_Tray_OutResult);
        return true;
    }

    if(watched == ui->widget_TestCup_0){
        if(!mInitEmptyArea[EMPTYTRAY_1]){
            CreatTrayTestTubeUiAxis(EMPTYTRAY_1);
            mInitEmptyArea[EMPTYTRAY_1] = true;
            init_testtube_tray(EMPTYTRAY_1);
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_0,EMPTYTRAY_1,m_BigRadius,m_SmalleRadius);
        return true;
    }

    if(watched == ui->widget_TestCup_1){
        if(!mInitEmptyArea[EMPTYTRAY_2]){
            CreatTrayTestTubeUiAxis(EMPTYTRAY_2);
            mInitEmptyArea[EMPTYTRAY_2] = true;
            init_testtube_tray(EMPTYTRAY_2);
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_1, EMPTYTRAY_2,m_BigRadius,m_SmalleRadius);
        return true;
    }

    if(watched == ui->widget_TestCup_2){
        if(!mInitEmptyArea[EMPTYTRAY_3]){
            CreatTrayTestTubeUiAxis(EMPTYTRAY_3);
            mInitEmptyArea[EMPTYTRAY_3] = true;
            init_testtube_tray(EMPTYTRAY_3);
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_2, EMPTYTRAY_3,m_BigRadius,m_SmalleRadius);
        return true;
    }

    if(watched == ui->widget_TestCup_3){
        if(!mInitEmptyArea[EMPTYTRAY_4]){
            CreatTrayTestTubeUiAxis(EMPTYTRAY_4);
            init_testtube_tray(EMPTYTRAY_4);
            mInitEmptyArea[EMPTYTRAY_4] = true;
        }
        DrawTrayTestTubeUiAxis(ui->widget_TestCup_3, EMPTYTRAY_4,m_BigRadius,m_SmalleRadius);
        return true;
    }

    if(watched == ui->DroptheCup){
        showDiscardTheCup(); /*弃杯孔*/
        return true;
    }

    if(watched == ui->widget_cleanagent)
    {
        showCleanreagent();//清洗剂
        return true;
    }

    return false;
}


bool Testing::handleResizeEvents(QObject *watched, QEvent *event){
    Q_UNUSED(event);

    // 重置初始化状态，确保下次绘制时重新计算坐标
    if (watched == ui->widget_Sample_1) {
        mInituiBloodArea = false;
    }

    // 不拦截事件，继续传递
    return false;
}

bool Testing::handleMouseEvents(QObject *watched, QEvent *event)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
    if (mouseEvent->button() == Qt::RightButton) {
        // 处理右键菜单等
        return true; // 事件已处理
    }
    return false;
}

bool Testing::eventFilter(QObject *watched, QEvent *event)
{

    //只处理绘制事件
    if(event->type() == QEvent::Paint){
        return handlePaintEvents(watched,event);
    }

    //处理其它特定事件
    else if(event->type() == QEvent::Resize){
        return handleResizeEvents(watched, event);
    }

    // 处理鼠标事件
//    else if (event->type() == QEvent::MouseButtonPress) {
//        return handleMouseEvents(watched, event);
//    }

    // 其他事件传递给基类
    return QWidget::eventFilter(watched, event);
}

void Testing::init_testtube_tray(const int index_tray)
{
    int status_tube = 0,sample_id = 0;
    QString reag_index;
    int from_tube = index_tray *ONETRAY_TOTALTUBE;
    int end_tube =  from_tube + ONETRAY_TOTALTUBE;
    for(;from_tube < end_tube; from_tube++)
    {
      FullyAutomatedPlatelets::pinstancesqlData()->FindOneEmptyTube(from_tube, status_tube, reag_index, sample_id);
      quint8 reagentindex = GlobalData::indexReagentMapReagnetNmaes(reag_index);
      if(status_tube  == TESTTUBES_SAMPLED)
      {
          InitTubeAddBlooded(from_tube, reagentindex, sample_id);      //试管已加血样
      }
      else if(status_tube  == TESTTUBES_CLIPPEDAWAY)
      {
          InitUIEmptyTubeused(from_tube);   //已使用的空试管变灰
      }
      else
      {
          FullyAutomatedPlatelets::pinstancesqlData()->UpdateTestTubeStateInfo(from_tube,
                                                                    TESTTUBES_FREETIME, "noone", 0);
      }
    }
    update();
    return;
}



//试管已被夹走用掉了
void  Testing::InitUIEmptyTubeused(int testTubeIndex)
{
    if (testTubeIndex < 0) {
        QLOG_WARN() << "无效的试管索引:" << testTubeIndex;
        return;
    }
    auto it = m_Testcups.find(testTubeIndex);
    if (it != m_Testcups.end()) {
        mEmptyTubeClipMoved.insert(it.key(), it.value());
    } else {
        QLOG_ERROR() << "试管:" << testTubeIndex << "夹取失败，未找到对应测试杯";
    }
    update();
}



//初始化试管已加血样
void  Testing::InitTubeAddBlooded(int tubeIndex, quint8 reagentType, int sampleNumber)
{
    const auto it = m_Testcups.find(tubeIndex);
    if (it != m_Testcups.end()) {
        if (reagentType == 0) {
            mEmptyTubeAbsorb_Poorblood.insert(tubeIndex, it.value());
        } else {
            mEmptyTubeAbsorb_Richblood.insert(tubeIndex, it.value());
        }
        mEmptyText.insert(tubeIndex, QString::number(sampleNumber));
    }
    update();
}


//清洗孔位置
void Testing::showCleaningbit()
{
    QPainter painter(ui->widget_Abandoned_new);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    constexpr int kMargin = 20;
    constexpr int kTextSpacing = 25;
    const QRect widgetRect = ui->widget_Abandoned_new->rect();
    m_CleanPos = {
        {0, QPointF(kMargin + Big_radius, kTextSpacing + Big_radius)},
        {1, QPointF(kMargin + Big_radius, widgetRect.height() - kTextSpacing - Big_radius)}
    };
    QMap<quint8, QPointF>::const_iterator iter;
    for (iter = m_CleanPos.begin(); iter != m_CleanPos.end(); ++iter) {
        QPalette palette;
        painter.setPen(cglobal::g_LineColor); //圆环外圈的颜色
        palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor); //内圆环背景颜色
        painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式外圆环的颜色
        painter.drawEllipse(iter.value(),Big_radius,Big_radius); //画大圆
        painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
        painter.drawEllipse(iter.value(),small_radius,small_radius); //画小圆
    }
    QFont font = this->font();
    font.setPointSize(14);
    painter.setFont(font);
    painter.setPen(cglobal::g_TextColorBlack);
    QRectF drawrect(m_CleanPos[1].x() - Big_radius*2 - 5 ,
        m_CleanPos[1].y() + kMargin,widgetRect.width(),kTextSpacing);
    painter.drawText(drawrect,Qt::AlignHCenter,"清洗位");
    painter.end();
    return;
}



/*初始化任务测高界面*/
void Testing::initTheTaskInterface()
{
    //任务对话框关闭
    FullyAutomatedPlatelets::pinstanceAddsampletest()->initcreat();
    connect(FullyAutomatedPlatelets::pinstanceAddsampletest(),
            &Height_Data::Taskconfigcloe,this,[=]()
    {
         m_opendedheight = false;
    });


    //PPP血样孔提示
    connect(FullyAutomatedPlatelets::pinstanceAddsampletest(),
            &Height_Data::reminderPPPandPRPputHole,
            this,[=](const quint8 richhole)
    {
        if(richhole == 255){
            mreminderPPPandPRPhole.clear();  // 255表示清空
        }
        else if(mreminderPPPandPRPhole.contains(richhole)){
            mreminderPPPandPRPhole.removeOne(richhole);  // 移除指定孔号
        }
        else{
            mreminderPPPandPRPhole.push_back(richhole);  // 添加新孔号
        }
    });

    //更新提示PRP孔
    connect(FullyAutomatedPlatelets::pinstanceAddsampletest(),
            &Height_Data::reminderPPPandPRPputHoleNotify,
            this,[=](const quint8 oldPRPHole,const quint8 changePRPHole)
    {
		int index = mreminderPPPandPRPhole.indexOf(oldPRPHole);
		if (index != -1) {
			mreminderPPPandPRPhole[index] = changePRPHole;
		}
    });



    connect(FullyAutomatedPlatelets::pinstanceAddsampletest(),&Height_Data::updateTestTubeSatus,
            this,&Testing::sycnChangeuiTubeStatus,
            Qt::QueuedConnection);
    return;
}


void Testing::DrawChannelProgress(quint8 Index ,double proportion)
{
    // 基本安全检查
    if (Index >= static_cast<quint8>(m_channelShowsTheProgress.size())) {
        QLOG_ERROR()<<"索引"<<Index <<"大于等于"<<m_channelShowsTheProgress.size();
        return;
    }

    ProgressBar* progressBar = m_channelShowsTheProgress.value(Index, nullptr);
    if (!progressBar) {
        return;
    }

    // 设置值并确保显示
    progressBar->setValue(qBound(0.0, proportion, 100.0));
    progressBar->setVisible(true);

    // 请求更新（Qt会自动合并多次更新）
    update();

//    ProgressBar *PupdateTestProgress =  m_channelShowsTheProgress.at(Index);
//    PupdateTestProgress->setValue(proportion);
//    PupdateTestProgress->show();
//    // 限制界面刷新频率（避免频繁重绘）
//    QTimer::singleShot(0, this, [this]() { update(); });
    return;
}


/*********** 测高函数 *********************************************************/
//测高拍照完成
void Testing::HandleObtainPRPImage(const QString &pathImage)
{
    // 获取单例窗口实例
    auto* sampleTestWindow = FullyAutomatedPlatelets::pinstanceAddsampletest();

    if (m_opendedheight) {
        // 窗口首次打开时的初始化逻辑
        sampleTestWindow->setWindowFlags(sampleTestWindow->windowFlags() | Qt::FramelessWindowHint);
        sampleTestWindow->show();

    } else {
        // 关闭窗口时改用 hide() 避免资源释放问题
        sampleTestWindow->hide();
        sampleTestWindow->show();
    }

    // 统一管理窗口状态
    m_opendedheight = !m_opendedheight;  // 直接取反状态

    // 显式传递参数（必要时进行类型转换）
    sampleTestWindow->opencvFindImageLine(pathImage);

    return;
}

//从测样本
void Testing::HandleReopencvImageTubePRP(const QString &reId,const QString &pathImage){
    // 获取单例窗口实例
    auto* sampleTestWindow = FullyAutomatedPlatelets::pinstanceAddsampletest();

    if (m_opendedheight) {
        // 窗口首次打开时的初始化逻辑
        sampleTestWindow->setWindowFlags(sampleTestWindow->windowFlags() | Qt::FramelessWindowHint);
        sampleTestWindow->show();

    } else {
        // 关闭窗口时改用 hide() 避免资源释放问题
        sampleTestWindow->hide();
        sampleTestWindow->show();
    }

    // 统一管理窗口状态
    m_opendedheight = !m_opendedheight;  // 直接取反状态

    // 显式传递参数（必要时进行类型转换）
    sampleTestWindow->reTestOpencvId(reId,pathImage);
    return;
}


/*点击打开任务框*/
void Testing::_showaddsamplewidget()
{
    auto widget = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (widget->isMinimized()) {
        widget->showNormal();  // 先恢复窗口标准状态
        widget->raise();       // 确保窗口置顶
        widget->activateWindow(); // 激活窗口焦点
    }

    if(m_opendedheight == false){
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
        widget->show();
        m_opendedheight = true;
    }
    else{
        m_opendedheight = false;
        widget->close();  // 隐藏窗口
        widget->show();   // 显示窗口
    }
    return;
}


void Testing::TotalTaskProgress()
{
    m_ThrowTube = 0;
    ui->widget_showtips->setValue(0);
    update();
    return;
}




void Testing::slotThrowtesttube()
{
    if(m_ProTotalTube == 0) return;
    m_ThrowTube++;
    if(m_ThrowTube >= m_ProTotalTube)
        ui->widget_showtips->setValue(100);
    else
        ui->widget_showtips->setValue(m_ThrowTube*100/m_ProTotalTube);
    QLOG_DEBUG()<<"丢杯个数"<<m_ThrowTube << "总数"<<m_ProTotalTube;
    update();
}


/*所有样本测试完成*/
void Testing::allSampleTested()
{

    // 检查对象是否有效
    if (!this) {
        QLOG_ERROR() << "this pointer is null!";
        return;
    }

    // 添加调试输出，确认方法确实被调用
    QLOG_INFO() << "线程中调用的 AllSampleTested 方法:" << QThread::currentThread();

    try{
        // 批量UI更新开始禁用更新，进行多次修改
        //setUpdatesEnabled(false);
        cglobal::g_StartTesting = false;

        ui->widget_showtips->setValue(100);
        m_ProTotalTube = 0; //需要测试的样本数
        m_ThrowTube = 0;    //弃杯数

        //放弃样本血样孔闪烁状态回归
        mflashingTubeList.clear();

        // 安全停止timer
        if(mtimer) {
            mtimer->stop();
        }

        // 安全遍历progress列表
        for(auto *pProgress : m_channelShowsTheProgress) {
            if(pProgress) {  // 空指针检查
                pProgress->flashingReminder(true);
            }
        }

        //通道标签
        for (const QPointer<QLabel> &label : Channelreminder) {
            if (label) {
                label->hide();
            }
        }

        //试管选中状态复位 重新初始化血样下拉框血样孔
        if(FullyAutomatedPlatelets::pinstanceAddsampletest()) {
            FullyAutomatedPlatelets::pinstanceAddsampletest()->initNumAnaemiaHole();
        }

        //完成测试 血样区复原
        ComplBackColorBloodArea();

        // 批量UI更新结束
        //setUpdatesEnabled(true);
        update();

        // 最后发射信号
        emit tsetfinishedbackorigin();

    }catch (const std::exception& e) {
        QLOG_WARN() << "Exception in AllSampleTested:" << e.what();
        // 确保UI更新恢复
        setUpdatesEnabled(true);
    } catch (...) {
        QLOG_WARN() << "Unknown exception in AllSampleTested";
        setUpdatesEnabled(true);
    }
    return;
}


void Testing::sycnChangeuiTubeStatus(const QString& sample_name,quint8 anemiahole,
                                       const QList<quint8>& marktube,
                                       int index_add,int all_add_task)
{

    m_ProTotalTube += marktube.size();
    if (m_ProTotalTube != 0) {
        ui->widget_showtips->setValue(m_ThrowTube * 100 / m_ProTotalTube);
    }

    //更新选中状态
    VectorSelectedBloodAreaTube(anemiahole, sample_name);    // 选中贫血
    VectorSelectedBloodAreaTube(anemiahole + 1, sample_name);// 选中富血

    //处理试管分配
    const auto sampleNum = sample_name.split("-", QString::SkipEmptyParts).last();

    for (const quint8 tubeId : marktube) {
        const auto it = m_Testcups.constFind(tubeId);
        if (it != m_Testcups.constEnd()) {
            mEmptyTubeAssigned.insert(it.key(), it.value()); // 试管分配血样
            mEmptyText.insert(tubeId, sampleNum);
        }
    }

    //触发完成信号
    if (cglobal::g_StartTesting && index_add == all_add_task) {
        emit testingaddsample();
    }

    //更新界面
    update();
}

void Testing::ChaneColorEmptyAreaTray(QString SampleName)
{
    QList<quint8> EmptytubeTray;
    EmptytubeTray.clear();
    StructInstance::getInstance()->sycn_sample_all_test_tube(SampleName,EmptytubeTray);//找出单个样本的贫富血试管孔
    for (auto EmptyTube : EmptytubeTray)
    {
        EmptyTubeAssigned(EmptyTube);
    }
    update();
    return;
}


void Testing::tubeMarked(const QString& outtext,const quint8& tubeIndex){
    auto it = m_Testcups.find(tubeIndex);
    if (it != m_Testcups.end()) {
        // 移动试管到已使用容器（避免内存重复分配）
        mEmptyTubeAssigned.insert(tubeIndex, it.value());
        mEmptyText[tubeIndex] = outtext;  // 使用[]运算符直接写入
    }
    //同步耗材界面试管用掉
    emit SynchronizeEmptyTube(tubeIndex);
    update();
}

/*任务试管标记选中*/
void Testing::EmptyTubeAssigned(quint8 IndexTube)
{
    if(m_Testcups.contains(IndexTube))
    {
        auto emptytube = m_Testcups.find(IndexTube);
        int IndexHole = emptytube.key();
        mEmptyTubeAssigned.insert(IndexHole, emptytube.value()); //试管分配血样
        QLOG_DEBUG() << "试管:" << emptytube.key() << "标记选中";
        int sample_id = StructInstance::getInstance()->sycn_emptytube_get_sampleid(IndexTube);
        mEmptyText.insert(IndexTube, QString::number(sample_id));
    }
    update();
    return;
}

/*更换试管盘*/
void Testing::replacEmptyTestTary(int index_tary)
{
    //耗材界面点更换试管盘
    //QtConcurrent::run(this, &Testing::replacementTray, index); //更换试管盘、线程
    int _statrHole,_endHole = 0;
    _statrHole = ONETRAY_TOTALTUBE * index_tary;
    _endHole = _statrHole + ONETRAY_TOTALTUBE;
    for(; _statrHole < _endHole; _statrHole++)
        VectorReplaceClear(_statrHole);
    FullyAutomatedPlatelets::pinstancesqlData()->UpadteTrayState(index_tary);  //替换数据库试管状态
    //FullyAutomatedPlatelets::pinstanceinfowidget()->InsertText(PROMPTLOG,"更换盘"+ QString::number(index_tary + 1)+"完成");
    QLOG_ERROR()<<"更换盘"<<QString::number(index_tary + 1)<<"完成"<<endl;
    //MachineLayoutTask->ReplaceTrayEmptyTube(index);
    update();
    return;
}

void Testing::VectorReplaceClear(quint8 Index)
{
    // 耗材更换试管盘--处理测试界面显示
    auto eraseIfFound = [this](auto& container, quint8 index) {
       auto it = container.find(index);
       if (it != container.end()) {
           container.erase(it);
       }
    };

    eraseIfFound(mEmptyTubeAssigned, Index);
    eraseIfFound(mEmptyTubeAbsorb_Poorblood, Index);
    eraseIfFound(mEmptyTubeAbsorb_Richblood, Index);
    eraseIfFound(mEmptyTubeClipMoved, Index);
    eraseIfFound(mEmptyText, Index);
    return;
}



/****************-血样孔绘制 ******************/
QFont ConfigureTextstyle(int size)
{
    QFont Textfont;
    Textfont.setFamily("楷体");
    Textfont.setPixelSize(size);
    Textfont.setLetterSpacing(QFont::AbsoluteSpacing, 1);// 设置字符间距
    return Textfont;
}



void Testing::CreatBloodZoneAxisPos()
{
    m_BloodHoleInitUiAxis.clear();
    m_BloodHoleNum.clear();

    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

    QRect bloodRect = ui->widget_Sample_1->rect();
    constexpr  int kSpaceTopY  = 30;
    int BloodZoneHeigh = bloodRect.height()  - kSpaceTopY ;

    int BloodAreasWidth = 0; //血样区界面总宽度
    int SpaceLeft = ui->widget_TestCup_0->geometry().bottomLeft().x(); //距离左边边界
    int TotalRows = 5, TotalCols = 0;

    switch(equipmentKind)
    {
        case KS600:
            TotalCols = 6 ;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
        case KS800:
            TotalCols = 8 ;
            BloodAreasWidth = ui->widget_TestCup_2->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
        case KS1200:
            TotalCols = 12 ;
            BloodAreasWidth = ui->widget_TestCup_3->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
        default:
            TotalCols = 6 ;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();

        break;
    }
    int oneWidth =0;
    int oneHeight = 0;
    QList<QRect> BloodOnceZoneList_Left;
    QList<QRect> BloodOnceZoneList_Right;
    int drawWidth = (BloodAreasWidth - TotalCols * 5)/TotalCols; //均分血样区宽度 间隔是 5
    for(int n = 0; n < TotalCols; n++)
    {
        int x_ = SpaceLeft+ n*drawWidth + n * 5;
        int y_ = 30;
        QRect rectone(QPoint(x_,y_),QSize(drawWidth,BloodZoneHeigh));
        //QLOG_DEBUG()<<"单个="<<rectone.width()<<"hhh"<<rectone.height();
        oneWidth = rectone.width()/2;
        oneHeight = rectone.height()/5;

        int right_w = drawWidth/2;
        QRect Leftrect(QPoint(x_,30),QSize(right_w,BloodZoneHeigh));
        int right_one = Leftrect.bottomRight().x() ;
        QRect Rightrect(QPoint(right_one,30),QSize(right_w,BloodZoneHeigh));
        BloodOnceZoneList_Left.push_back(Leftrect);
        BloodOnceZoneList_Right.push_back(Rightrect);
    }

    int diameter = 0; //直径
    (oneWidth > oneHeight)? diameter = oneHeight : diameter = oneWidth;
    m_BloodBigRadius = (diameter - 2)/2;
    m_BloodSmallRadius = qAbs(m_BloodBigRadius - 2);
    //QLOG_DEBUG()<<"直径===="<<diameter<<"半"<<m_BloodBigRadius<< BloodOnceZoneList_Left;


    int num =  0;
    for(int n = 0 ; n < BloodOnceZoneList_Left.size(); n++)
    {
        QRect rect_ = BloodOnceZoneList_Left.at(n);
        for(int c = 0 ; c < TotalRows; c++)
        {
           int xpos = rect_.topLeft().x();
           int heightBlood = rect_.height()/TotalRows;
           int ypos = rect_.topLeft().y() + heightBlood * c;
           QRect BloodHole(QPoint(xpos,ypos),QSize(rect_.width(),heightBlood));
           num = 2*c + 10 * n;
           QPoint CenterPos = BloodHole.center();
           m_BloodHoleInitUiAxis.insert(num,CenterPos);
           m_BloodHoleNum.insert(num,QString("%1").arg(num + 1));
        }
    }
    for(int n = 0 ; n < BloodOnceZoneList_Right.size(); n++)
    {
        QRect rect_ = BloodOnceZoneList_Right.at(n);
        for(int c = 0 ; c < TotalRows; c++)
        {
           int xpos = rect_.topLeft().x() + m_BloodBigRadius/3*2;  //右边位置
           int heightBlood = rect_.height()/TotalRows;
           int ypos = rect_.topLeft().y() + heightBlood * c;
           QRect BloodHole(QPoint(xpos,ypos),QSize(rect_.width(),heightBlood));
           num = 2*c+1 + 10 * n;
           QPoint CenterPos(BloodHole.topLeft().x(),BloodHole.center().y());
           m_BloodHoleInitUiAxis.insert(num,CenterPos);
           m_BloodHoleNum.insert(num,QString("%1").arg(num + 1));
        }
    }
    return;
}

void Testing::DrawBloodTopText()
{
    QPainter painter(ui->widget_Sample_1);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::Qt4CompatiblePainting);
    QFont font = ConfigureTextstyle(18);
    painter.setFont(font);
    QList<QString> ReminderToptext;
    ReminderToptext<<"A"<<"B"<<"C"<<"D"<<"E"<<"F"<<"G"<<"H"<<"I"<<"J"<<"K"<<"L";

    int TextHeight = 30;
    int BloodAreasWidth = 0;
    QPoint cupGlobalPos = ui->widget_TestCup_0->mapToGlobal(QPoint(0, 0));
    int SpaceLeft = ui->widget_Sample_1->mapFromGlobal(cupGlobalPos).x();
    int TotalCols = 0;
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);

    switch(equipmentKind)
    {
        case KS600:
            TotalCols = 6;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() -
                    ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
        case KS800:
            TotalCols = 8;
            BloodAreasWidth = ui->widget_TestCup_2->geometry().bottomRight().x() -
                    ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
        case KS1200:
            TotalCols = 12;
            BloodAreasWidth = ui->widget_TestCup_3->geometry().bottomRight().x() -
                    ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
        default:
            TotalCols = 6;
            BloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() - ui->widget_TestCup_0->geometry().bottomLeft().x();
        break;
    }

    painter.setPen(cglobal::g_TextColorBlack);
    int oneWidth = (BloodAreasWidth - TotalCols * 5) / TotalCols;

    // ========== 动态计算半径（不依赖成员变量）==========
    // 获取血样区的高度
    //QRect bloodRect = ui->widget_Sample_1->rect();
    //constexpr int kSpaceTopY = 30;
    //int BloodZoneHeigh = bloodRect.height() - kSpaceTopY;

//    int perHoleWidth = oneWidth / 2;
//    int oneHeight = BloodZoneHeigh / 5;  // TotalRows = 5
//    int diameter = qMin(perHoleWidth, oneHeight);
//    int bloodRadius = (diameter - 2) / 2;
    int visualOffset = -11;  // 根据实际效果调整
    // ================================================
    for(int n = 0; n < TotalCols; n++)
       {
           // 计算每列的X起始位置
           int x_ = SpaceLeft + n * oneWidth + n * 5;

           // 左孔的中心位置
           int leftHoleCenterX = x_ + oneWidth / 4;
           // 右孔的中心位置
           int rightHoleCenterX = x_ + oneWidth * 3 / 4;

           // 文字应该在左右两个孔的中间位置（整列的中心）
           int textCenterX = (leftHoleCenterX + rightHoleCenterX) / 2;

           QFontMetrics fm(font);
           QString text = ReminderToptext.at(n);
           QRect textRect = fm.boundingRect(text);

           // 以整列的中心为基准绘制文字
           QPoint textPos(
               textCenterX - textRect.width() / 2 + visualOffset,
               TextHeight - 2
           );

           painter.drawText(textPos, text);

    }
    return;
}

void Testing::DrawBloodHoleInnerText(const QMap<quint8, QPoint>& BloodHoleMap)
{
    if (BloodHoleMap.isEmpty()) {
        return;
    }


    QPainter painter(ui->widget_Sample_1);
    if (!painter.isActive()) {
        return;
    }

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform
                           | QPainter::Qt4CompatiblePainting);
    QFont font = ConfigureTextstyle(14);
    painter.setFont(font);
    QFontMetrics fm(font);

    static const QMap<quint8, QString> numberMap = {
        { 0, "1" },
        { 2, "2" },
        { 4, "3" },
        { 1, "4" },
        { 3, "5" }
    };

    for (auto iter = BloodHoleMap.constBegin(); iter != BloodHoleMap.constEnd(); ++iter) {
        if (iter.key() % 2 == 0) {
            const QPoint& pos = iter.value();

            auto it = numberMap.find(iter.key() % 5);
            if (it != numberMap.end()) {
                QString text = it.value();
                QRect textRect = fm.boundingRect(text);

                // 计算文字位置，使其在左侧居中显示并与圆环保持 2px 边距
                QPoint textPos(
                    pos.x() - m_BloodBigRadius * 2 - 2,
                    pos.y() + textRect.height() / 2 - 2
                );
                painter.drawText(textPos, text);
            }
        }
    }
}

void Testing::UpdateBloodHoleColors(int State, QMap<quint8, QPoint>& MapBloodHole)
{
    // 1. 前置安全检查
    if (!ui || !ui->widget_Sample_1) {
        QLOG_ERROR() << "UI components are not properly initialized!";
        return;
    }

    if (!ui->widget_Sample_1->isVisible()) {
        QLOG_DEBUG() << "widget_Sample_1 is not visible, skipping update.";
        return;
    }

    // 2. 检查输入参数的有效性
    if (&MapBloodHole == nullptr) {
        QLOG_ERROR() << "MapBloodHole reference is invalid!";
        return;
    }

    if (MapBloodHole.isEmpty()) {
        //QLOG_DEBUG() << "MapBloodHole is empty, nothing to draw.";
        return;
    }


    QMutexLocker locker(&m_dataMutex);

    // 3. 创建数据的安全副本
    QMap<quint8, QPoint> safeBloodHoleCopy;
    QMap<quint8, QString> safeBloodHoleNumCopy;

    try {
        safeBloodHoleCopy = MapBloodHole;
        safeBloodHoleNumCopy = m_BloodHoleNum;
    }
    catch (const std::exception& e) {
        QLOG_ERROR() << "Failed to create safe copies:" << e.what();
        return;
    }
    catch (...) {
        QLOG_ERROR() << "Unknown exception occurred while creating safe copies";
        return;
    }

    // 4. 初始化 QPainter
    QPainter painter(ui->widget_Sample_1);
        if (!painter.isActive()) {
        QLOG_ERROR() << "QPainter failed to initialize on widget_Sample_1";
        return;
    }

    // 5. 配置绘制参数
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QFont font = ConfigureTextstyle(11);
    painter.setFont(font);

    QPen pen;
    pen.setColor(cglobal::g_LineColor);
    pen.setWidthF(1);
    painter.setPen(pen);

    // 6. 预先计算字体度量
    QFontMetrics fm(font);

    // 7. 安全的迭代绘制
    for (auto iter = safeBloodHoleCopy.constBegin(); iter != safeBloodHoleCopy.constEnd(); ++iter) {
        try {
            // 双重检查迭代器有效性
            if (iter == safeBloodHoleCopy.constEnd()) {
                break;
            }

            const quint8 holeId = iter.key();
            const QPoint& center = iter.value();

            // 验证坐标有效性
            if (center.isNull() && center != QPoint(0, 0)) {
                QLOG_WARN() << "Invalid center point for holeId:" << holeId;
                continue;
            }

            // 查找对应的文本
            auto itText = safeBloodHoleNumCopy.constFind(holeId);
            if (itText == safeBloodHoleNumCopy.constEnd()) {
                QLOG_DEBUG() << "No text found for holeId:" << holeId;
                continue;
            }

            const QString& showText = itText.value();

            // 8. 确定颜色方案
            QPalette palette;
            painter.setPen(pen);

            switch (State) {
                case TUBE_INIT:
                    palette.setColor(QPalette::Background, cglobal::g_SamllBgmColor);
                    break;
                case TUBE_CHECKED:
                    (holeId % 2 != 0) ? palette.setColor(QPalette::Background, cglobal::g_RichBloodColor)
                                                            : palette.setColor(QPalette::Background, cglobal::g_PoorBloodColor);
                    break;
                case TUBE_OUTRESULT:
                     palette.setColor(QPalette::Background, cglobal::g_OutResult);
                    break;
                default:
                    QLOG_WARN() << "Unknown state:" << State << ", using default color";
                    palette.setColor(QPalette::Background, cglobal::g_SamllBgmColor); // 默认颜色
                    break;
            }

            // 9.绘制外圈
            painter.setBrush(QBrush(cglobal::g_OuterRingColor, Qt::SolidPattern));
            painter.drawEllipse(center, m_BloodBigRadius, m_BloodBigRadius); //画大圆

             // 如果是提醒孔，绘制一个矩形框来突出显示
            if(mreminderPPPandPRPhole.contains(holeId)|| mreminderPPPandPRPhole.contains(holeId + 1)){
                // 框的大小比大圆稍大一些
                int frameSize = m_BloodBigRadius + 1;
                QRect frameRect(center.x() - frameSize, center.y() - frameSize,
                               frameSize * 2, frameSize * 2);

                // 设置框的颜色（使用提醒色）
                painter.setPen(QPen(cglobal::g_ReminderBloodHoleColor, 2, Qt::SolidLine));
                painter.setBrush(Qt::NoBrush);
                painter.drawRect(frameRect);
            }

            // 10. 绘制内圈
            painter.setBrush(QBrush(palette.brush(QPalette::Background)));
            painter.drawEllipse(center, m_BloodSmallRadius, m_BloodSmallRadius);

            // 11. 绘制文本
            if (!showText.isEmpty()) {
                QColor textColor = (holeId % 2 == 0) ? cglobal::g_TextColorRoorblood : cglobal::g_TextColorRichblood;
                painter.setPen(textColor);

                // 使用 QFontMetrics 实现完美的文字居中
                QRect textRect = fm.boundingRect(showText);
                QPoint textPos(
                    center.x() - textRect.width() / 2,
                    center.y() + textRect.height() / 2 - 2
                );

                painter.drawText(textPos, showText);
            }

        }
        catch (const std::exception& e) {
            QLOG_ERROR() << "Exception while drawing hole:" << e.what();
            continue; // 继续绘制其他孔
        }
        catch (...) {
            QLOG_ERROR() << "Unknown exception while drawing hole";
            continue;
        }
    }

    // 12. 绘制内部文本
    try {
        DrawBloodHoleInnerText(safeBloodHoleCopy);
    }
    catch (const std::exception& e) {
        QLOG_ERROR() << "Exception in DrawBloodHoleInnerText:" << e.what();
    }
    catch (...) {
        QLOG_ERROR() << "Unknown exception in DrawBloodHoleInnerText";
    }

    //QLOG_DEBUG() << "UpdateBloodHoleColors completed successfully for" << safeBloodHoleCopy.size() << "holes";
}









void Testing::mousePressEvent(QMouseEvent *event)
{
    int mouse_posx = event->x() ;
    int mouse_posy = event->y() ;
    if (event->button() == Qt::RightButton /*&& !cglobal::g_StartTesting*/)
    {
       ClickCanelTask(mouse_posx, mouse_posy);
    }
    return;
}

void Testing::ToggletheReagentPosition(int mouse_posx , int mouse_posy)
{
    auto iter = m_Reagent_Tray.constBegin();
    while(iter != m_Reagent_Tray.constEnd()){
        QPoint posReagent = ui->widget_Reagents->mapToParent(QPoint(5 + iter.value().x(),iter.value().y()));
        int Leftpos =  posReagent.x() - Big_radius;
        int Rightpos = posReagent.x() + Big_radius;
        int Toppos =  posReagent.y() - Big_radius;
        int bottompos = posReagent.y() + Big_radius;
        if((mouse_posx >= Leftpos && mouse_posx <= Rightpos) && (mouse_posy >= Toppos && mouse_posy <= bottompos))
        {
            if (cglobal::g_StartTesting) {
                FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("切换试剂失败", "样本测试中请勿切换试剂位置!");
                return;
            }
            break;
        }
        iter++;
    }
    update();
    return;
}

/**
 * @brief 处理血样孔右键点击取消任务
 * @param mouse_posx 鼠标点击的X坐标（相对窗口）
 * @param mouse_posy 鼠标点击的Y坐标（相对窗口）
 * @note 仅对偶数孔号（血样孔）生效，奇数孔号（富血样孔）不响应取消操作
 * @note 使用预计算的转换矩阵优化坐标转换性能
 * @warning 样本测试过程中禁止取消任务
 * @return 无返回值，通过信号触发取消任务对话框
 */
void Testing::ClickCanelTask(int mouse_posx , int mouse_posy)
{
    // 安全检查
    if (!ui || !ui->widget_Sample_1 || !this) {
        QLOG_ERROR() << "UI components not properly initialized";
        return;
    }

    QPoint bloodpos;
    int clickedHole = -1;
    QList<int> emptytube;
    emptytube.clear();
    int leftPos, rightPos, topPos, bottomPos;

    auto iter = m_Blood_Tray_Checked.constBegin();
    while(iter != m_Blood_Tray_Checked.constEnd())
    {
        // 计算血样孔在this中的实际位置
        // 使用Qt的坐标转换方法
        QPoint holeRelativePos = iter.value();
        QPoint actualPos = this->mapFromGlobal(ui->widget_Sample_1->mapToGlobal(holeRelativePos));
        int actualX = actualPos.x();
        int actualY = actualPos.y();

        // 计算孔的边界区域
        leftPos = actualX - m_BloodBigRadius;
        rightPos = actualX + m_BloodBigRadius;
        topPos = actualY - m_BloodBigRadius;
        bottomPos = actualY + m_BloodBigRadius;

        // 边界检查
        if (actualX < 0 || actualY < 0 || leftPos < 0 || topPos < 0) {
            QLOG_WARN() << "Invalid hole position detected for hole" << iter.key();
            ++iter;
            continue;
        }

        // 调试输出
            QLOG_DEBUG() << "孔" << iter.key()
        << "点击位置:" << mouse_posx << mouse_posy
        << "孔区域:[" << leftPos << "," << rightPos
        << "][" << topPos << "," << bottomPos << "]"
        << "孔中心:(" << actualX << "," << actualY << ")"
        << "半径:" << m_BloodBigRadius
        << "控件位置:(" << ui->widget_Sample_1->pos().x() << "," << ui->widget_Sample_1->pos().y() << ")"
        << "孔相对位置:(" << iter.value().x() << "," << iter.value().y() << ")";

        if((mouse_posx >= leftPos && mouse_posx <= rightPos) &&
                (mouse_posy >= topPos && mouse_posy <= bottomPos)){

            if (cglobal::g_StartTesting)
            {
                FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("取消失败", "样本测试中请勿取消测试样本!");
                return;
            }
            clickedHole = iter.key();
            if(clickedHole % 2 == BLOOD_HOLE_TYPE_EVEN)
            {
                // ... 取消任务逻辑
                QList<int> sample_tasklist; //获取贫血的血样区孔号
                QList<int> sampleidList;
                StructInstance::getInstance()->cancel_sample_task(sample_tasklist,sampleidList);
                clickAnaemiaHoleCancelTestTask(sampleidList); //取消任务点击贫血孔号
            }
            break;
        }
        ++iter;

    }
    update();
    return;
}

void Testing::clickAnaemiaHoleCancelTestTask(QList<int> waitTestSampleId)
{
    CancelTaskConfigure *pCancelwaitTestTask = new CancelTaskConfigure(this, waitTestSampleId);
    connect(pCancelwaitTestTask,&CancelTaskConfigure::makesureCacelIdList,this,[=](QList<int> cancelsampleid)
    {
        QString date = GlobalData::ObatinCreatSampleTime();
        foreach (int cancelwaittestsampleid, cancelsampleid)
        {
            int cancelTubeTotalnum = 0;
            quint8 cancelAnemiaHole = 0; //贫血样孔号
            QList<quint8> CancelEmptyTubeList;
            CancelEmptyTubeList.clear();
            quint8 cancelBackHole = 0; //富血样孔号

            QString sampleid =  GlobalData::groupDateAndID(date,cancelwaittestsampleid);
            FullyAutomatedPlatelets::pinstancesqlData()->deleteTestResultTable(sampleid);
            FullyAutomatedPlatelets::pinstancesqlData()->deleteTestCurveDataTale(sampleid);

            StructInstance::getInstance()->cancelsampletask(cancelwaittestsampleid,cancelTubeTotalnum,cancelAnemiaHole,
                                                            CancelEmptyTubeList,cancelBackHole);

            m_ProTotalTube = m_ProTotalTube - cancelTubeTotalnum;
            FullyAutomatedPlatelets::pinstanceAddsampletest()->backcancelhole(cancelBackHole);
            CanceltaskbackTubecolor(cancelAnemiaHole, cancelBackHole, CancelEmptyTubeList);

            FullyAutomatedPlatelets::pinstancepatientdata()->cancelSampleResultItem(sampleid);
        }

    });
    pCancelwaitTestTask->setAttribute(Qt::WA_DeleteOnClose);
    pCancelwaitTestTask->exec();
    return;
}

QList<quint8> Testing::getAllocatePPPandPRPHole()
{
    return  mreminderPPPandPRPhole;
}

//取消任务孔号
void Testing::CanceltaskbackTubecolor(int PPPHole, int PRPHoles, QList<quint8> emptytubehole)
{

    auto processHole = [this](int hole){
        auto it = m_Blood_Tray_Checked.find(hole);
        if(it != m_Blood_Tray_Checked.end()){
            m_BloodHoleInitUiAxis.insert(it.key(),it.value());
            m_Blood_Tray_Checked.erase(it);

            m_BloodHoleNum[hole] = QString("%1").arg(hole + 1);
        }
    };

    // 处理PPPHole和PRPHoles
    processHole(PPPHole);
    processHole(PRPHoles);

    // 从提示列表中移除取消的孔号
    //mreminderPPPandPRPhole.removeOne(PPPHole);
    //mreminderPPPandPRPhole.removeOne(PRPHoles);

    for(int tube : emptytubehole){
        FullyAutomatedPlatelets::pinstancesqlData()->update_test_tube_status(tube,
                                                                             TESTTUBES_FREETIME,
                                                                             "noone",                                                                      0);
        // 删除相关映射条目：直接使用find检查
        auto emptyIt = mEmptyTubeAssigned.find(tube);
        if (emptyIt != mEmptyTubeAssigned.end()) {
            mEmptyTubeAssigned.erase(emptyIt);
        }

        auto textIt = mEmptyText.find(tube);
        if (textIt != mEmptyText.end()) {
            mEmptyText.erase(textIt);
        }

        // 从提示列表中移除空试管孔号
        //mreminderPPPandPRPhole.removeOne(tube);
    }
    return;
}


void Testing::VectorSelectedBloodAreaTube(quint8 IndexTube,QString Samplenum)
{
   auto itInit = m_BloodHoleInitUiAxis.find(IndexTube);  //直接find
   if (itInit != m_BloodHoleInitUiAxis.end())
   {
        m_Blood_Tray_Checked.insert(itInit.key(), itInit.value());
        m_BloodHoleInitUiAxis.erase(itInit);


        QString numStr = Samplenum.split("-", QString::SkipEmptyParts).last();
        auto TextNum = m_BloodHoleNum.find(IndexTube);
        TextNum.value().swap(numStr);
   }
   return;
}



void Testing::SampleTestingChangInitColor(QPoint BloodPoint ,quint8 FinishChannel)
{
    const quint8 pppHole = BloodPoint.x();
    const quint8 prpHole = BloodPoint.y();
    QLOG_DEBUG()<<"PPP血样孔号:"<<pppHole<<"PRP血样孔号:"<<prpHole;

    // 使用lambda函数处理重复的map操作
    auto processHole = [this](quint8 hole) {
        auto it = m_Blood_Tray_Checked.find(hole);
        if (it != m_Blood_Tray_Checked.end()) {
            m_Blood_Tray_OutResult.insert(it.key(), it.value());
            m_Blood_Tray_Checked.erase(it);
        }
    };

    processHole(pppHole);
    processHole(prpHole);

    // 处理通道显示
    const quint8 completedChannel = FinishChannel - 1;
    DrawChannelProgress(completedChannel, 0);      // 通道显示变0
    Channelreminder.at(completedChannel)->hide();  // 隐藏通道显示信息

    for (auto it = m_TestingSample.begin(); it != m_TestingSample.end(); ) {
        if (it.value() == FinishChannel) {
            it = m_TestingSample.erase(it);
        }
        else {
            ++it;
        }
    }
    return;
}

void Testing::ComplBackColorBloodArea()
{
    m_Blood_Tray_OutResult.clear();
    m_Blood_Tray_Checked.clear();
    CreatBloodZoneAxisPos();
    return;
}






/*******************画空试管区***********************/
void  Testing::DrawTrayTestTubeUiAxis(QWidget* pTrayWidget,quint8 IndexTray,quint8 BigRadius,
                                        quint8 SmalleRadius)
{
    QPainter painter(pTrayWidget);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = ConfigureTextstyle(11);
    painter.setFont(font);

    quint8 FirstHole = 0;
    quint8 EndHoleNum = 0;
    QMap<quint8, QPointF>::iterator  TrayTubeFirst;

    switch(IndexTray)
    {
        case EMPTYTRAY_1:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_oneHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_oneHole;
                EndHoleNum =  TestTrayHole::Tray_twoHole;
            }
        break;

        case EMPTYTRAY_2:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_twoHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_twoHole;
                EndHoleNum =  TestTrayHole::Tray_threeHole;
            }
        break;
        case EMPTYTRAY_3:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_threeHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_threeHole;
                EndHoleNum =  TestTrayHole::Tray_fourHole;
            }
        break;
        case EMPTYTRAY_4:
            TrayTubeFirst = m_Testcups.find(TestTrayHole::Tray_fourHole);
            if(TrayTubeFirst != m_Testcups.end()){
                FirstHole = TestTrayHole::Tray_fourHole;
                EndHoleNum =  TestTrayHole::Tray_totalHole;
            }
        break;
        default:break;
    }

    if(TrayTubeFirst == m_Testcups.end())
        return;


    while(TrayTubeFirst != m_Testcups.end() && TrayTubeFirst.key() < EndHoleNum)
    {
        QPalette palette;
        painter.setPen(cglobal::g_LineColor); //圆环外圈的颜色
        const quint8 holeNum = TrayTubeFirst.key();

        if (mEmptyTubeAssigned.contains(holeNum)){
            palette.setColor(QPalette::Background, cglobal::g_InnerRing_Checked); //分配任务
        }
        else if (mEmptyTubeAbsorb_Poorblood.contains(holeNum)){
            palette.setColor(QPalette::Background, cglobal::g_PoorBloodColor);
        }
        else if (mEmptyTubeAbsorb_Richblood.contains(holeNum)){
            palette.setColor(QPalette::Background, cglobal::g_RichBloodColor);   //PRP加完血样内圈背景色
        }
        else if (mEmptyTubeClipMoved.contains(holeNum)){
             palette.setColor(QPalette::Background, cglobal::g_OutResult);       //夹走
        }
        else{
            palette.setColor(QPalette::Background, cglobal::g_SamllBgmColor);
        }

        if(mflashingTubeList.contains(holeNum)){
            if (m_blinkState)
                palette.setColor(QPalette::Background, QColor(255 ,255 ,255));
            else
                palette.setColor(QPalette::Background, QColor(255,0,0));
        }
        painter.setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿
        painter.setBrush(QBrush(cglobal::g_OuterRingColor, Qt::SolidPattern)); //设置画刷形式 -- 外圆环的颜色
        painter.drawEllipse(TrayTubeFirst.value(), BigRadius, BigRadius); //画大圆
        painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
        painter.drawEllipse(TrayTubeFirst.value(), SmalleRadius, SmalleRadius); //画小圆
        painter.setPen(cglobal::g_TextColorWhite);
        QString SampleText = mEmptyText[TrayTubeFirst.key()];

        // 使用智能文本对齐方式
        if (!SampleText.isEmpty()) {
            QFontMetrics fm(font);
            QRect textRect = fm.boundingRect(SampleText);
            QPoint textPos(
                TrayTubeFirst.value().x() - textRect.width() / 2,
                TrayTubeFirst.value().y() + textRect.height() / 2 - 2
            );
            painter.drawText(textPos, SampleText);
        }

        if(EndHoleNum == (FirstHole + 1))
            break;
        TrayTubeFirst++;
        FirstHole++;
    }
    return;
}



//血样区的血样被加到空试管区
void Testing::recvBloodSuck2EmptyTube(bool banemia, quint8 IndexTube)
{
    auto it = mEmptyTubeAssigned.find(IndexTube);

    if (it != mEmptyTubeAssigned.end()){
        // 根据贫血状态选择目标容器
        auto& targetMap = banemia ? mEmptyTubeAbsorb_Poorblood : mEmptyTubeAbsorb_Richblood;

        targetMap.insert(IndexTube, it.value());

        // 安全地从原容器中移除
        mEmptyTubeAssigned.erase(it);

        // 更新数据库状态
        FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(
                IndexTube, TESTTUBES_SAMPLED);
    }

    update();
}

void Testing::giveupSampleShowHole(QList<quint8> holeList){

    for(quint8 hole : holeList){
        if(!mflashingTubeList.contains(hole)){
            mflashingTubeList.append(hole);
        }
    }
}

void Testing::giveupSampleChannelFlash(const bool &isChannelNormal, const quint8 indexChannel){
    Q_ASSERT(indexChannel < m_channelShowsTheProgress.size());
    m_channelShowsTheProgress[indexChannel]->flashingReminder(isChannelNormal);
    QColor colors;
    (isChannelNormal)?  colors.setRgb(0,0,0): colors.setRgb(255,0,0);
    QLOG_WARN()<<"通道"<<indexChannel + 1<<"测试状态:"<<isChannelNormal;
    QPalette palette =  Channelreminder[indexChannel]->palette();
    palette.setColor(QPalette::WindowText, colors);
    Channelreminder[indexChannel]->setPalette(palette);
}


//试管被夹走 再同步耗材界面
void Testing::EmptyTubeClipMoved(quint8 IndexTube)
{
    if (mEmptyTubeAbsorb_Poorblood.contains(IndexTube))
    {
        auto emptytube = mEmptyTubeAbsorb_Poorblood.find(IndexTube);
        mEmptyTubeClipMoved.insert(emptytube.key(), emptytube.value());
        emit SynchronizeEmptyTube(emptytube.key());
        mEmptyTubeAbsorb_Poorblood.erase(emptytube);
    }
    if (mEmptyTubeAbsorb_Richblood.contains(IndexTube))
    {
        auto emptytube = mEmptyTubeAbsorb_Richblood.find(IndexTube);
        mEmptyTubeClipMoved.insert(emptytube.key(), emptytube.value());
        emit SynchronizeEmptyTube(emptytube.key());  //同步质控界面试管用掉
        mEmptyTubeAbsorb_Richblood.erase(emptytube);
    }
    FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(IndexTube,TESTTUBES_CLIPPEDAWAY);

    repaint();
    return;
}



//绘制试剂&&坐标
void Testing::plotReagentWellsCoordinates(quint8 totalReagents)
{
    m_Reagent_Tray.clear();
    QPoint ReagentWellpos(0,0);
    for(int i = 0 ; i < totalReagents ; i++)
    {
        (i%2 == 0) ? ReagentWellpos.setX(Big_radius+3) : ReagentWellpos.setX(Big_radius + (2 * Big_radius + 10));
        ReagentWellpos.setY(Top_Bottom_space + Big_radius + (i/2)*(Top_Bottom_space+2*Big_radius));
        m_Reagent_Tray.insert(i,ReagentWellpos);
    }
}

void Testing::showPaintReagents()
{
    QPainter painter(ui->widget_Reagents);
    painter.setRenderHint(QPainter::Antialiasing,true);
    QStringList NameText{"AA","ADP","EPI","COL","RIS"};
    QFont font = ConfigureTextstyle(13);
    painter.setFont(font);
    QMap<quint8,QPoint>::iterator iter = m_Reagent_Tray.begin();
    quint8 index = 0;
    QColor WaitUseReagent(0,0,0);
    while(iter != m_Reagent_Tray.end())
    {
        QPalette palette;
        painter.setPen(cglobal::g_LineColor); // -- 圆环外圈的颜色
        //内圆环背景颜色
         palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor);
        //设置渲染,启动反锯齿
        painter.setRenderHint(QPainter::Antialiasing, true);
        //设置画刷形式 -- 外圆环的颜色
        painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern));
        //画大圆
        painter.drawEllipse(iter.value(),Big_radius,Big_radius);
        //设置画刷为背景色
        painter.setBrush(QBrush(palette.brush(QPalette::Background)));
        //画小圆
        painter.drawEllipse(iter.value(),small_radius,small_radius);

        quint8 indexreag = iter.key()/2 + 1;
        painter.setPen(GlobalData::customCurveColor(indexreag));

        QString name =  NameText.at(index/2);
        if(name.length() >= 3)
            painter.drawText(QPoint(iter.value().x()-11 ,iter.value().y() + 5),name);
        else
            painter.drawText(QPoint(iter.value().x()-8,iter.value().y() + 5),name);
        iter++;
        index++;
    }
    update();
    return;
}

/**清洗剂*/
void Testing::showCleanreagent()
{
    QPainter painter(ui->widget_cleanagent);
    QFont font = ConfigureTextstyle(12);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing,true);
    m_CleanningReagent.setX((ui->widget_cleanagent->width() >> 1));
    m_CleanningReagent.setY((ui->widget_cleanagent->height() >> 1));
    QPalette palette;
    painter.setPen(cglobal::g_LineColor); // -- 圆环外圈的颜色
    palette.setColor(QPalette::Background,cglobal::g_SamllBgmColor); //-----内圆环背景颜色
    painter.setRenderHint(QPainter::Antialiasing, true);//设置渲染,启动反锯齿
    painter.setBrush(QBrush(cglobal::g_OuterRingColor,Qt::SolidPattern)); //设置画刷形式 -- 外圆环的颜色
    painter.drawEllipse(m_CleanningReagent,TopDraw_Bigradius,TopDraw_Bigradius); //画大圆
    painter.setBrush(QBrush(palette.brush(QPalette::Background))); //设置画刷为背景色
    painter.drawEllipse(m_CleanningReagent,TopDraw_Smallradius,TopDraw_Smallradius); //画小圆
    painter.setPen(cglobal::g_TextColorBlack);
    painter.drawText(QPoint(m_CleanningReagent.x() - 22,m_CleanningReagent.y() + 5),tr("清洗剂S1"));
}


/*显示弃杯孔*/
void Testing::showDiscardTheCup()
{
   QPainter painter(ui->DroptheCup);
   QFont font = ConfigureTextstyle(14);
   painter.setFont(font);
   painter.setRenderHint(QPainter::Antialiasing, true);

   // 计算中心点（圆心）
   int centerX = ui->DroptheCup->width() / 2;
   int centerY = ui->DroptheCup->height() / 2;
   m_DiscardCupHole.setX(centerX);
   m_DiscardCupHole.setY(centerY);

   // 绘制外圆环
   painter.setPen(cglobal::g_LineColor);
   painter.setBrush(QBrush(cglobal::g_OuterRingColor, Qt::SolidPattern));
   painter.drawEllipse(m_DiscardCupHole, TopDraw_Bigradius, TopDraw_Bigradius);

   // 绘制内圆（背景色）
   painter.setBrush(QBrush(cglobal::g_SamllBgmColor));
   painter.drawEllipse(m_DiscardCupHole, TopDraw_Smallradius, TopDraw_Smallradius);

   // 绘制文字（居中于圆心）
   painter.setPen(QColor(0, 0, 0));
   QString text = tr("弃杯孔");
   QRect textRect = painter.fontMetrics().boundingRect(text);
   QPoint textPos(centerX - textRect.width() / 2,
                  centerY + textRect.height() / 3); // 微调垂直居中
   painter.drawText(textPos, text);
}

/*测试试管显示初始坐标*/
void Testing::CreatTrayTestTubeUiAxis(int indexTray) {
    constexpr quint8 Total_Cols = 6;
    constexpr quint8 Total_Rows = 10;
    constexpr int spacing = 5;

    // 预定义控件和起始孔映射
    QWidget* trayWidgets[] = {ui->widget_TestCup_0, ui->widget_TestCup_1,
                             ui->widget_TestCup_2, ui->widget_TestCup_3};
    quint8 trayHoles[] = {Tray_oneHole, Tray_twoHole, Tray_threeHole, Tray_fourHole};

    if (indexTray < EMPTYTRAY_1 || indexTray > EMPTYTRAY_4) return;
    int idx = indexTray - EMPTYTRAY_1;
    quint8 FirstIndexHole = trayHoles[idx];
    int AloneWidth = trayWidgets[idx]->width();
    int AloneHeight = trayWidgets[idx]->height();

    const int Width_OnceWidth = AloneWidth / Total_Cols;
    const int Width_OnceHeight = AloneHeight / Total_Rows;
    const int setdiameter = std::min(Width_OnceWidth, Width_OnceHeight);

    // 与 widget_Sample_1 样式一致的半径计算
    m_BigRadius = (setdiameter - spacing) / 2;
    m_SmalleRadius = qAbs(m_BigRadius - 2);

    for (int r = 0; r < Total_Rows; ++r) {
        const int y_base = r * Width_OnceHeight;
        for (int c = 0; c < Total_Cols; ++c) {
            const QPoint center(
                c * Width_OnceWidth + Width_OnceWidth / 2,
                y_base + Width_OnceHeight / 2
            );
            m_Testcups.insert(FirstIndexHole++, center);
        }
    }
}

//重测高度
void Testing::repPrpheight(const QString idnum,bool falgrep){
    emit sendRepPrpheight(idnum,falgrep);
}

