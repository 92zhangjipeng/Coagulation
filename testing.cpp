#include "testing.h"
#include "ui_testing.h"
#include "usbcodedispose.h"
#include "canceltaskconfigure.h"
#include "structinstance.h"
#include "height_data.h"
#include "genericfunctions.h"
#include <QCoreApplication>
#include <QTimer>
#include <QThread>
#include <QDebug>
#include <QMetaType>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QLabel>
#include <QRegularExpression>
#include <unordered_map>
#include <algorithm>
#include "globaldata.h"
#include "cglobal.h"
#include "ini_file.h"
#include "quiutils.h"
#include "QsLog/include/QsLog.h"
#include <operclass/fullyautomatedplatelets.h>
#include "mainwindow.h"



#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

using namespace std;
using namespace TestingConstants;

namespace {
const QColor kHoleEmpty(255, 255, 255);
const QColor kHoleEdge(0x8A, 0xA4, 0xB6);
const QColor kRingBand(0xC5, 0xD6, 0xE2);
const QColor kHolePoor(0xF3, 0xD3, 0x6B);
const QColor kHoleRich(0xF0, 0xA1, 0x4A);
const QColor kHoleUsed(0x7E, 0x8B, 0x98);
const QColor kHoleSelected(0xE3, 0x8B, 0x84);

void drawStereoRing(QPainter &painter, const QPointF &center, qreal outerR, qreal innerR,
                    const QColor &fill, const QColor &rimTint = QColor())
{
    if (outerR < 2.0) {
        return;
    }
    Q_UNUSED(innerR);
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, true);

    const qreal band = qBound(4.5, outerR * 0.32, 8.0);
    qreal holeR = outerR - band;
    if (holeR < outerR * 0.50) {
        holeR = outerR * 0.50;
    }

    const QColor well = fill.isValid() ? fill : kHoleEmpty;
    const bool reagent = rimTint.isValid();
    const QColor edge = reagent ? rimTint : kHoleEdge;
    const QColor ring = reagent ? rimTint : kRingBand;

    painter.setPen(QPen(edge, 1.0));
    painter.setBrush(ring);
    painter.drawEllipse(center, outerR, outerR);

    painter.setPen(Qt::NoPen);
    painter.setBrush(well);
    painter.drawEllipse(center, holeR, holeR);
    painter.restore();
}
}

std::unique_ptr<UsbCodeDispose> Testing::m_TaskDll = nullptr;

// 线程安全的单例访问
UsbCodeDispose* Testing::getTaskDll()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [](){
        m_TaskDll = std::make_unique<UsbCodeDispose>();
    });
    return m_TaskDll.get();
}

Testing::Testing(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Testing),
    mInituiBloodArea(false),
    alreadyinitchannelui(false),
    m_ProTotalTube(0),
    m_ThrowTube(0),
    m_blinkState(false),
    m_channelCount(0)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    mflashingTubeList.clear();
    mreminderPPPandPRPhole.clear();
    qRegisterMetaType<QList<quint8> >("QList<quint8>");
}

Testing::~Testing()
{
    if (mtimer) {
        mtimer->stop();
        mtimer->deleteLater();
        mtimer = nullptr;
    }
    delete ui;
}

void Testing::initequipmentKind(const quint8 indextype)
{
    if (!mtimer) {
        mtimer = new QTimer(this);
        connect(mtimer, &QTimer::timeout, this, &Testing::toggleBlinkState);
    }
    if (!mtimer->isActive()) {
        mtimer->start(500);
    }

    minstrumentType = indextype;
    memset(mInitEmptyArea, false, sizeof(mInitEmptyArea));

    mEmptyTubeAssigned.clear();
    mEmptyTubeAbsorb_Poorblood.clear();
    mEmptyTubeAbsorb_Richblood.clear();
    m_TestingSample.clear();
    mEmptyText.clear();
    m_Blood_Tray_Checked.clear();

    initializeMachineUI(minstrumentType);
    initTheTaskInterface();
    markHiddenTrayTubesClippedAway();
}

void Testing::toggleBlinkState()
{
    m_blinkState = !m_blinkState;
    updateBlinkWidgets();
}

int Testing::visibleTrayCount() const
{
    switch (minstrumentType) {
        case KS600: return 2;
        case KS800: return 3;
        case KS1200: return 4;
        default: return 2;
    }
}

void Testing::markHiddenTrayTubesClippedAway()
{
    int startTube = 0;
    int endTube = 0;
    switch (minstrumentType) {
        case KS600: startTube = 120; endTube = 240; break;
        case KS800: startTube = 180; endTube = 240; break;
        default: return;
    }
    auto *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (!sql) {
        return;
    }
    for (int i = startTube; i < endTube; ++i) {
        sql->UpdateEmptyTube_State(i, TESTTUBES_CLIPPEDAWAY);
    }
}

void Testing::ensureTrayReady(int trayIndex)
{
    if (trayIndex < 0 || trayIndex > 3 || mInitEmptyArea[trayIndex]) {
        return;
    }
    CreatTrayTestTubeUiAxis(trayIndex);
    init_testtube_tray(trayIndex);
    mInitEmptyArea[trayIndex] = true;
}

void Testing::updateBlinkWidgets()
{
    bool bloodHint = false;
    {
        QReadLocker locker(&m_bloodMutex);
        bloodHint = !mreminderPPPandPRPhole.isEmpty();
    }
    if (bloodHint && ui->widget_Sample_1) {
        ui->widget_Sample_1->update();
    }

    QList<quint8> flashing;
    {
        QMutexLocker locker(&m_flashMutex);
        flashing = mflashingTubeList;
    }
    if (flashing.isEmpty()) {
        return;
    }
    bool trayNeed[4] = {false, false, false, false};
    for (quint8 hole : flashing) {
        const int tray = hole / TUBES_PER_TRAY;
        if (tray >= 0 && tray < 4) {
            trayNeed[tray] = true;
        }
    }
    QWidget *trays[4] = {
        ui->widget_TestCup_0, ui->widget_TestCup_1,
        ui->widget_TestCup_2, ui->widget_TestCup_3
    };
    for (int i = 0; i < 4; ++i) {
        if (trayNeed[i] && trays[i]) {
            trays[i]->update();
        }
    }
}

// ==================== resizeEvent 拆分为三个子函数 ====================
void Testing::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    mInituiBloodArea = false;

    setUpdatesEnabled(false);

    recalculateChannelLayout();
    recalculateTrayLayout();
    setupModuleLayout();

    const int trays = visibleTrayCount();
    for (int i = 0; i < 4; ++i) {
        mInitEmptyArea[i] = false;
        if (i < trays) {
            ensureTrayReady(i);
        }
    }

    setUpdatesEnabled(true);
    update();
}

void Testing::recalculateChannelLayout()
{
    const int appWidth = ui->widget_main->size().width();
    const int appHeigh = ui->widget_main->size().height();
    const int boardSpaceX = 5;
    const int boardSpaceY = 5;
    const int ChnBarHeight = 110;
    const int Left_width = appWidth - boardSpaceX * 2 - 2 * 2;

    // 清除 widget_channelarea 的布局
    if (ui->widget_channelarea->layout()) {
        QLayoutItem* item;
        while ((item = ui->widget_channelarea->layout()->takeAt(0)) != nullptr) {
            delete item;
        }
        delete ui->widget_channelarea->layout();
    }

    ui->widget_channelarea->setGeometry(boardSpaceX, boardSpaceY, Left_width, ChnBarHeight);

    // 计算血样区和试管盘所需最小高度（用于后续分配）
    BloodAreaLayout bloodLayout = computeBloodAreaLayout();
    int bloodDiameter = bloodLayout.bigRadius * 2;
    int bloodMinHeight = 5 * bloodDiameter + BLOOD_TOP_TEXT_HEIGHT + 20;

    int tubeDiameter = (m_BigRadius > 0) ? m_BigRadius * 2 : 40;
    int tubeMinHeight = 10 * tubeDiameter + 20;

    int totalAvailable = appHeigh - ui->widget_channelarea->height() - boardSpaceY * 3 - 4 * 2;

    int trayHeight = totalAvailable * 2 / 3;
    int bloodHeight = totalAvailable - trayHeight;

    trayHeight = qMax(trayHeight, tubeMinHeight);
    bloodHeight = totalAvailable - trayHeight;
    if (bloodHeight < bloodMinHeight) {
        bloodHeight = bloodMinHeight;
        trayHeight = totalAvailable - bloodHeight;
        if (trayHeight < tubeMinHeight) {
            qWarning() << "窗口高度不足以完整显示所有内容";
        }
    }

    ui->widget_testcup->setGeometry(boardSpaceX, ui->widget_channelarea->geometry().bottomLeft().y() + boardSpaceY,
                                   Left_width, trayHeight);
    int bloodBottomLoc_topy = ui->widget_testcup->geometry().bottomLeft().y() + boardSpaceY;
    ui->widget_Sample_1->setGeometry(boardSpaceX, bloodBottomLoc_topy, Left_width, bloodHeight);
}

void Testing::recalculateTrayLayout()
{
    const int appWidth = ui->widget_main->width();
    const int Left_width = appWidth - 5 * 2 - 4;
    constexpr int bigSpace = 29;
    int ColWidth = (Left_width - 5) / bigSpace;
    int TaryTubeWidth = ColWidth * 6;
    int visibleTraysCount = visibleTrayCount();
    const int boardSpaceX = 5;
    int maxTrayWidth = (Left_width - (visibleTraysCount - 1) * boardSpaceX) / visibleTraysCount;
    int finalTrayWidth = qMin(TaryTubeWidth, maxTrayWidth);
    QList<QWidget*> trayWidgets = {ui->widget_TestCup_0, ui->widget_TestCup_1,
                                   ui->widget_TestCup_2, ui->widget_TestCup_3};
    int totalTraysWidth = visibleTraysCount * finalTrayWidth + (visibleTraysCount - 1) * boardSpaceX;
    int startTrayX = (Left_width - totalTraysWidth) / 2;
    int trayHeight = ui->widget_testcup->height();
    for (int i = 0; i < trayWidgets.size(); ++i) {
        if (i < visibleTraysCount) {
            trayWidgets[i]->setFixedSize(finalTrayWidth, trayHeight);
            trayWidgets[i]->setGeometry(startTrayX + i * (finalTrayWidth + boardSpaceX), 0,
                                        finalTrayWidth, trayHeight);
            trayWidgets[i]->show();
        } else {
            trayWidgets[i]->hide();
        }
    }
}

void Testing::setupModuleLayout()
{
    const int channelAreaWidth = ui->widget_channelarea->width();
    const int channelAreaHeight = ui->widget_channelarea->height();
    const int dropCupWidth = 80;
    const int availableForModulesWidth = channelAreaWidth - dropCupWidth - 15;

    constexpr int moduleSpacing = 10;
    constexpr int progressBarSpacing = 10;
    constexpr int progressBarsPerModule = 4;
    constexpr int labelHeight = 28;
    constexpr int labelTopMargin = 2;
    constexpr int moduleTopMargin = 4;
    constexpr int moduleBottomMargin = 4;

    int availableHeightForBar = channelAreaHeight - moduleTopMargin - moduleBottomMargin - labelHeight - labelTopMargin;

    int modulesCount = 0;
    switch (minstrumentType) {
        case KS600: modulesCount = 1; break;
        case KS800: modulesCount = 2; break;
        case KS1200: modulesCount = 3; break;
        default: modulesCount = 1; break;
    }

    int totalSpacingForBars = (progressBarsPerModule - 1) * progressBarSpacing;
    int maxModuleWidth = (availableForModulesWidth - (modulesCount - 1) * moduleSpacing) / modulesCount;
    int maxBarWidthByWidth = (maxModuleWidth - totalSpacingForBars) / progressBarsPerModule;
    int progressBarSize = qMin(availableHeightForBar, maxBarWidthByWidth);
    progressBarSize = qMax(progressBarSize, 65);

    int moduleWidth = progressBarsPerModule * progressBarSize + (progressBarsPerModule - 1) * progressBarSpacing;
    int moduleHeight = moduleTopMargin + progressBarSize + labelTopMargin + labelHeight + moduleBottomMargin;
    moduleHeight = qMin(moduleHeight, channelAreaHeight);

    QList<QWidget*> modules = {ui->widget_Module1, ui->widget_Module2, ui->widget_Module3};
    for (int i = 0; i < modules.size(); ++i) {
        if (i < modulesCount) {
            modules[i]->setFixedSize(moduleWidth, moduleHeight);
            modules[i]->show();
        } else {
            modules[i]->hide();
        }
    }

    int totalModulesWidth = modulesCount * moduleWidth + (modulesCount - 1) * moduleSpacing;
    int startX = (availableForModulesWidth - totalModulesWidth) / 2;
    int startY = (channelAreaHeight - moduleHeight) / 2;

    int dropCupX = channelAreaWidth - dropCupWidth;
    int dropCupY = (channelAreaHeight - 120) / 2;
    ui->DroptheCup->setParent(ui->widget_channelarea);
    ui->DroptheCup->setGeometry(dropCupX, dropCupY, dropCupWidth, 120);
    ui->DroptheCup->show();

    for (int i = 0; i < modulesCount; ++i) {
        int x = startX + i * (moduleWidth + moduleSpacing);
        modules[i]->setGeometry(x, startY, moduleWidth, moduleHeight);
    }

    // 更新每个模块内的 ProgressBar 和标签
    for (int i = 0; i < modulesCount; ++i) {
        QList<ProgressBar*> progressBars = modules[i]->findChildren<ProgressBar*>();
        QList<QLabel*> labels = modules[i]->findChildren<QLabel*>();
        std::sort(progressBars.begin(), progressBars.end(),
            [](ProgressBar* a, ProgressBar* b) { return a->objectName() < b->objectName(); });
        std::sort(labels.begin(), labels.end(),
            [](QLabel* a, QLabel* b) { return a->objectName() < b->objectName(); });

        for (int j = 0; j < progressBars.size() && j < progressBarsPerModule; ++j) {
            ProgressBar* bar = progressBars[j];
            if (bar) {
                bar->setFixedSize(progressBarSize, progressBarSize);
                int barX = j * (progressBarSize + progressBarSpacing);
                bar->setGeometry(barX, moduleTopMargin, progressBarSize, progressBarSize);
                int borderRadius = qMax(progressBarSize / 10, 5);
                bar->setStyleSheet(QString(
                    "ProgressBar { background-color: #E8ECF0; border-radius: %1px; }"
                    "ProgressBar::chunk { background-color: #5B9BD5; border-radius: %1px; }"
                ).arg(borderRadius));
            }
        }
        for (int j = 0; j < labels.size() && j < progressBarsPerModule; ++j) {
            QLabel* label = labels[j];
            if (label) {
                int labelX = j * (progressBarSize + progressBarSpacing);
                int labelY = moduleTopMargin + progressBarSize + 2;
                label->setGeometry(labelX, labelY, progressBarSize, labelHeight);
                label->setStyleSheet(
                    "QLabel { font-family: 'Microsoft YaHei'; font-size: 13px; font-weight: 600;"
                    "color: #2C3035; background: transparent; }");
                label->setAlignment(Qt::AlignCenter);
                label->setWordWrap(true);
            }
        }
    }
}
// ==================== resizeEvent 拆分结束 ====================

void Testing::initControlShowChannelProgress(quint8 startChannel, QWidget *progressChannel,
                                              QWidget *ptestChannel, const QPalette& pa)
{
    QList<ProgressBar*> channelControl = progressChannel->findChildren<ProgressBar*>();
    QList<QLabel*> chnStateTextList = ptestChannel->findChildren<QLabel*>();

    if (channelControl.size() != chnStateTextList.size()) {
        QLOG_WARN() << "通道显示进度控件和提示测试Labels数量不匹配!";
        return;
    }

    quint8 currentChannel = startChannel;
    for (int i = 0; i < channelControl.size(); ++i) {
        if (m_channelCount >= MAX_CHANNELS) {
            QLOG_WARN() << "超出最大通道数限制";
            break;
        }
        ProgressBar* progressBar = channelControl[i];
        QLabel* label = chnStateTextList[i];

        progressBar->flashingReminder(true);
        progressBar->setRange(0, 100);
        progressBar->installEventFilter(this);

        const QString chnStateKey = QString("TestTheChanne1Opening_%1").arg(currentChannel + 1);
        bool usedChannel = INI_File().rConfigPara(chnStateKey).toBool();
        progressBar->setChnTextindex(usedChannel ? currentChannel + 1 : CHN_DISABLE);

        m_channelShowsTheProgress[m_channelCount] = progressBar;
        Channelreminder[m_channelCount] = label;
        if (!alreadyinitchannelui) {
            label->setPalette(pa);
            label->hide();
        }
        ++currentChannel;
        ++m_channelCount;
    }
}

void Testing::initializeMachineUI(const quint8 equipmentIndex)
{
    QPalette pa;
    pa.setColor(QPalette::WindowText, QColor(250, 128, 114));
    QList<QWidget*> widgetFilterList = { ui->widget_Reagents, ui->widget_Abandoned_new,
                                         ui->widget_Sample_1, ui->widget_TestCup_3,
                                         ui->widget_TestCup_2, ui->widget_TestCup_1,
                                         ui->widget_TestCup_0, ui->DroptheCup, ui->widget_cleanagent };
    for (auto pwidget : widgetFilterList)
        pwidget->installEventFilter(this);

    m_channelCount = 0;
    initControlShowChannelProgress(0, ui->widget_Module1, ui->widget_Module1, pa);
    initControlShowChannelProgress(4, ui->widget_Module2, ui->widget_Module2, pa);
    initControlShowChannelProgress(8, ui->widget_Module3, ui->widget_Module3, pa);

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
}

void Testing::updateChannelProgressAndStatus(bool isWaitstate, quint8 channelIndex, const QString &status)
{
    if (channelIndex >= static_cast<quint8>(m_channelCount) ||
        channelIndex >= MAX_CHANNELS) {
        QLOG_WARN() << "Invalid channel index:" << channelIndex;
        return;
    }
    ProgressBar* progress = m_channelShowsTheProgress[channelIndex];
    QPointer<QLabel> label = Channelreminder[channelIndex];
    if (!progress || label.isNull()) {
        QLOG_WARN() << "Null pointer for channel index:" << channelIndex;
        return;
    }

    progress->setValue(0);

    QString currentText = label->text();
    QRegularExpression rx("样本:(\\d+).*");
    QRegularExpressionMatch match = rx.match(currentText);
    QString sampleNumber = match.hasMatch() ? match.captured(1) : "0";

    QString outText;
    if (isWaitstate) {
        outText = tr("样本:%1 [等待]").arg(sampleNumber);
    } else {
        outText = tr("样本:%1 [%2]").arg(sampleNumber).arg(status);
    }
    if (label->text() != outText) {
        label->setText(outText);
    }
    label->show();
    label->update();

    if (!isWaitstate) {
        QTimer::singleShot(3000, label.data(), [label]() { if (!label.isNull()) label->hide(); });
    }
}

void Testing::recv_NotifyChannleState(const quint8 channelIndexNum, const bool bUseing)
{
    int index = channelIndexNum - 1;
    if (index >= 0 && index < m_channelCount) {
        if (bUseing) {
            m_channelShowsTheProgress[index]->setChnTextindex(channelIndexNum);
        } else {
            m_channelShowsTheProgress[index]->setChnTextindex(CHN_DISABLE);
        }
    }
}

void Testing::showTestChannelInfo(const quint8& channelIndex, const QString& sampleName,
                                  const quint8& reagentIndex)
{
    if (!this) {
        QLOG_FATAL() << "Parent object destroyed!";
        return;
    }
    const int idx = static_cast<int>(channelIndex - 1);
    if (idx >= m_channelCount || idx < 0) return;

    QPointer<QLabel> channelLabel = Channelreminder[idx];
    if (channelLabel.isNull()) {
        QLOG_DEBUG() << "Label at index" << idx << "already destroyed";
        return;
    }

    int sampleNumber = 0;
    QString unusedDate;
    GlobalData::apartSampleId(sampleName, unusedDate, sampleNumber);

    const QString reagentName = GlobalData::mapIndexReagentnames(reagentIndex).isEmpty()
                                ? QStringLiteral("Unknown_Reagent")
                                : GlobalData::mapIndexReagentnames(reagentIndex);
    const QString displayText = tr("样本:%1[%2]").arg(sampleNumber).arg(reagentName);

    if (channelLabel->text() != displayText) {
        channelLabel->setText(displayText);
        if (!channelLabel->isVisible()) {
            channelLabel->show();
        }
    }
    m_TestingSample.insert(sampleName, channelIndex);
}

bool Testing::handlePaintEvents(QObject *watched, QEvent *event)
{
    Q_UNUSED(event);
    if (watched == ui->widget_Reagents) {
        if (m_Reagent_Tray.isEmpty()) {
            plotReagentWellsCoordinates(TOTAL_REAGENTS);
        }
        showPaintReagents();
        return true;
    }
    if (watched == ui->widget_Abandoned_new) {
        showCleaningbit();
        return true;
    }
    if (watched == ui->widget_Sample_1) {
        if (!mInituiBloodArea) {
            CreatBloodZoneAxisPos();
            mInituiBloodArea = true;
        }
        QPainter painter(ui->widget_Sample_1);
        if (!painter.isActive()) {
            return true;
        }
        painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        DrawBloodTopText(painter);
        UpdateBloodHoleColors(painter, TUBE_INIT, m_BloodHoleInitUiAxis);
        UpdateBloodHoleColors(painter, TUBE_CHECKED, m_Blood_Tray_Checked);
        UpdateBloodHoleColors(painter, TUBE_OUTRESULT, m_Blood_Tray_OutResult);
        DrawPutBloodReminders(painter);
        return true;
    }
    if (watched == ui->DroptheCup) {
        showDiscardTheCup();
        return true;
    }
    if (watched == ui->widget_cleanagent) {
        showCleanreagent();
        return true;
    }
    if (watched == ui->widget_TestCup_0) {
        drawTray(0, ui->widget_TestCup_0);
        return true;
    }
    if (watched == ui->widget_TestCup_1) {
        drawTray(1, ui->widget_TestCup_1);
        return true;
    }
    if (watched == ui->widget_TestCup_2) {
        drawTray(2, ui->widget_TestCup_2);
        return true;
    }
    if (watched == ui->widget_TestCup_3) {
        drawTray(3, ui->widget_TestCup_3);
        return true;
    }
    return false;
}

bool Testing::handleResizeEvents(QObject *watched, QEvent *event)
{
    Q_UNUSED(event);
    if (watched == ui->widget_Sample_1) {
        mInituiBloodArea = false;
    }
    return false;
}

bool Testing::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Paint) {
        return handlePaintEvents(watched, event);
    }
    if (event->type() == QEvent::Resize) {
        return handleResizeEvents(watched, event);
    }
    return QWidget::eventFilter(watched, event);
}

void Testing::init_testtube_tray(const int index_tray)
{
    int status_tube = 0, sample_id = 0;
    QString reag_index;
    int from_tube = index_tray * ONETRAY_TOTALTUBE;
    int end_tube = from_tube + ONETRAY_TOTALTUBE;
    for (; from_tube < end_tube; ++from_tube) {
        FullyAutomatedPlatelets::pinstancesqlData()->FindOneEmptyTube(from_tube, status_tube, reag_index, sample_id);
        quint8 reagentindex = GlobalData::indexReagentMapReagnetNmaes(reag_index);
        if (status_tube == TESTTUBES_SAMPLED) {
            InitTubeAddBlooded(from_tube, reagentindex, sample_id, false);
        } else if (status_tube == TESTTUBES_CLIPPEDAWAY) {
            InitUIEmptyTubeused(from_tube, false);
        } else {
            FullyAutomatedPlatelets::pinstancesqlData()->UpdateTestTubeStateInfo(from_tube, TESTTUBES_FREETIME, "noone", 0);
        }
    }
}

void Testing::InitUIEmptyTubeused(int testTubeIndex, bool refreshUi)
{
    if (testTubeIndex < 0) {
        QLOG_WARN() << "无效的试管索引:" << testTubeIndex;
        return;
    }
    {
        QMutexLocker locker(&m_tubeMutex);
        auto it = m_Testcups.find(testTubeIndex);
        if (it != m_Testcups.end()) {
            mEmptyTubeClipMoved.insert(it.key(), it.value());
        } else {
            QLOG_ERROR() << "试管:" << testTubeIndex << "夹取失败，未找到对应测试杯";
        }
    }
    if (refreshUi) {
        update();
    }
}

void Testing::InitTubeAddBlooded(int tubeIndex, quint8 reagentType, int sampleNumber, bool refreshUi)
{
    {
        QMutexLocker locker(&m_tubeMutex);
        auto it = m_Testcups.find(tubeIndex);
        if (it != m_Testcups.end()) {
            if (reagentType == 0) {
                mEmptyTubeAbsorb_Poorblood.insert(tubeIndex, it.value());
            } else {
                mEmptyTubeAbsorb_Richblood.insert(tubeIndex, it.value());
            }
            mEmptyText.insert(tubeIndex, QString::number(sampleNumber));
        }
    }
    if (refreshUi) {
        update();
    }
}

void Testing::showCleaningbit()
{
    QPainter painter(ui->widget_Abandoned_new);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    constexpr int kMargin = 20;
    constexpr int kTextSpacing = 25;
    const QRect widgetRect = ui->widget_Abandoned_new->rect();
    m_CleanPos = {
        {0, QPointF(kMargin + BIG_RADIUS_DEFAULT, kTextSpacing + BIG_RADIUS_DEFAULT)},
        {1, QPointF(kMargin + BIG_RADIUS_DEFAULT, widgetRect.height() - kTextSpacing - BIG_RADIUS_DEFAULT)}
    };
    for (const auto& pos : m_CleanPos) {
        drawStereoRing(painter, pos, BIG_RADIUS_DEFAULT, SMALL_RADIUS_DEFAULT, kHoleEmpty);
    }
    QFont font = this->font();
    font.setPointSize(14);
    painter.setFont(font);
    painter.setPen(cglobal::g_TextColorBlack);
    QRectF drawrect(m_CleanPos[1].x() - BIG_RADIUS_DEFAULT*2 - 5,
                    m_CleanPos[1].y() + kMargin, widgetRect.width(), kTextSpacing);
    painter.drawText(drawrect, Qt::AlignHCenter, "清洗位");
}

void Testing::initTheTaskInterface()
{
    auto* sampleTest = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (!sampleTest) return;
    if (m_taskInterfaceInited) {
        return;
    }
    m_taskInterfaceInited = true;
    sampleTest->initcreat();
    connect(sampleTest, &Height_Data::Taskconfigcloe, this, [=]() { m_opendedheight = false; });
    connect(sampleTest, &Height_Data::reminderPPPandPRPputHole, this, [=](quint8 richhole) {
        {
            QWriteLocker locker(&m_bloodMutex);
            if (richhole == INVALID_HOLE_VALUE) {
                mreminderPPPandPRPhole.clear();
                m_bloodHoleStage.clear();
            } else if (!mreminderPPPandPRPhole.contains(richhole)) {
                mreminderPPPandPRPhole.push_back(richhole);
                if (!m_bloodHoleStage.contains(richhole)) {
                    m_bloodHoleStage.insert(richhole, BLOOD_HINT_PLACE);
                }
            }
        }
        if (richhole != INVALID_HOLE_VALUE && mtimer && !mtimer->isActive()) {
            mtimer->start(500);
        }
        if (ui->widget_Sample_1) {
            ui->widget_Sample_1->update();
        }
    });
    connect(sampleTest, &Height_Data::reminderPPPandPRPputHoleNotify, this, [=](quint8 oldPRPHole, quint8 changePRPHole) {
        {
            QWriteLocker locker(&m_bloodMutex);
            if (changePRPHole == INVALID_HOLE_VALUE) {
                mreminderPPPandPRPhole.removeOne(oldPRPHole);
                m_bloodHoleStage.remove(oldPRPHole);
            } else {
                const quint8 keepStage = m_bloodHoleStage.value(oldPRPHole, BLOOD_HINT_PLACE);
                m_bloodHoleStage.remove(oldPRPHole);
                int idx = mreminderPPPandPRPhole.indexOf(oldPRPHole);
                if (idx != -1) {
                    mreminderPPPandPRPhole[idx] = changePRPHole;
                } else if (!mreminderPPPandPRPhole.contains(changePRPHole)) {
                    mreminderPPPandPRPhole.push_back(changePRPHole);
                }
                m_bloodHoleStage.insert(changePRPHole, keepStage);
            }
        }
        if (ui->widget_Sample_1) {
            ui->widget_Sample_1->update();
        }
    });
    connect(sampleTest, &Height_Data::updateTestTubeSatus, this, &Testing::sycnChangeuiTubeStatus, Qt::QueuedConnection);
}

void Testing::DrawChannelProgress(quint8 Index, double proportion)
{
    if (Index >= static_cast<quint8>(m_channelCount)) {
        QLOG_ERROR() << "索引" << Index << "大于等于" << m_channelCount;
        return;
    }
    ProgressBar* progressBar = m_channelShowsTheProgress[Index];
    if (!progressBar) return;
    progressBar->setValue(qBound(0.0, proportion, 100.0));
    progressBar->setVisible(true);
    update();
}

Height_Data *Testing::toggleAddSampleWindow()
{
    auto *sampleTestWindow = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (!sampleTestWindow) {
        return nullptr;
    }
    if (m_opendedheight) {
        sampleTestWindow->setWindowFlags(sampleTestWindow->windowFlags() | Qt::FramelessWindowHint);
        sampleTestWindow->show();
    } else {
        sampleTestWindow->hide();
        sampleTestWindow->show();
    }
    m_opendedheight = !m_opendedheight;
    return sampleTestWindow;
}

void Testing::HandleObtainPRPImage(const QString &pathImage)
{
    ensureAddSampleWindowVisible();
    auto *sampleTestWindow = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (!sampleTestWindow) {
        return;
    }
    sampleTestWindow->opencvFindImageLine(pathImage);
}

void Testing::HandleReopencvImageTubePRP(const QString &reId, const QString &pathImage)
{
    ensureAddSampleWindowVisible();
    auto *sampleTestWindow = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (!sampleTestWindow) {
        return;
    }
    sampleTestWindow->reTestOpencvId(reId, pathImage);
}

void Testing::ensureAddSampleWindowVisible()
{
    auto *widget = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (!widget) {
        return;
    }
    widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
    if (widget->isMinimized()) {
        widget->showNormal();
    } else if (!widget->isVisible()) {
        widget->show();
    }
    widget->raise();
    widget->activateWindow();
    m_opendedheight = true;
}

void Testing::_showaddsamplewidget()
{
    auto widget = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if (!widget) return;
    if (widget->isMinimized()) {
        widget->showNormal();
        widget->raise();
        widget->activateWindow();
    }
    if (!m_opendedheight) {
        widget->setWindowFlags(widget->windowFlags() | Qt::FramelessWindowHint);
        widget->show();
        m_opendedheight = true;
    } else {
        m_opendedheight = false;
        widget->close();
        widget->show();
    }
}

void Testing::TotalTaskProgress()
{
    m_ThrowTube = 0;
    ui->widget_showtips->setValue(0);
    update();
}

void Testing::slotThrowtesttube()
{
    if (m_ProTotalTube == 0) return;
    ++m_ThrowTube;
    if (m_ThrowTube >= m_ProTotalTube)
        ui->widget_showtips->setValue(100);
    else
        ui->widget_showtips->setValue(m_ThrowTube * 100 / m_ProTotalTube);
    QLOG_DEBUG() << "丢杯个数" << m_ThrowTube << "总数" << m_ProTotalTube;
    update();
}

void Testing::allSampleTested()
{
    if (!this) {
        QLOG_ERROR() << "this pointer is null!";
        return;
    }
    QLOG_INFO() << "线程中调用的 AllSampleTested 方法:" << QThread::currentThread();
    QMetaObject::invokeMethod(this, &Testing::doAllSampleTestedUIUpdate, Qt::QueuedConnection);
}

void Testing::doAllSampleTestedUIUpdate()
{
    if (!this) return;
    cglobal::g_StartTesting.store(false);
    try {
        ui->widget_showtips->setValue(100);
        m_ProTotalTube = 0;
        m_ThrowTube = 0;
        {
            QMutexLocker locker(&m_flashMutex);
            mflashingTubeList.clear();
        }
        if (mtimer && mtimer->isActive()) {
            mtimer->stop();
        }
        for (int i = 0; i < m_channelCount; ++i) {
            if (m_channelShowsTheProgress[i]) {
                m_channelShowsTheProgress[i]->flashingReminder(true);
            }
        }
        for (int i = 0; i < m_channelCount; ++i) {
            if (!Channelreminder[i].isNull()) {
                Channelreminder[i]->hide();
            }
        }
        auto* sampleTest = FullyAutomatedPlatelets::pinstanceAddsampletest();
        if (sampleTest) {
            sampleTest->initNumAnaemiaHole();
        }
        ComplBackColorBloodArea();
        update();
        emit tsetfinishedbackorigin();
    } catch (const std::exception& e) {
        QLOG_WARN() << "Exception in AllSampleTested:" << e.what();
        setUpdatesEnabled(true);
    } catch (...) {
        QLOG_WARN() << "Unknown exception in AllSampleTested";
        setUpdatesEnabled(true);
    }
}

void Testing::sycnChangeuiTubeStatus(const QString& sample_name, quint8 anemiahole,
                                     const QList<quint8>& marktube,
                                     int index_add, int all_add_task)
{
    m_ProTotalTube += marktube.size();
    if (m_ProTotalTube != 0) {
        ui->widget_showtips->setValue(m_ThrowTube * 100 / m_ProTotalTube);
    }
    VectorSelectedBloodAreaTube(anemiahole, sample_name);
    VectorSelectedBloodAreaTube(anemiahole + 1, sample_name);

    const auto sampleNum = sample_name.split("-", QString::SkipEmptyParts).last();
    {
        QMutexLocker locker(&m_tubeMutex);
        for (quint8 tubeId : marktube) {
            auto it = m_Testcups.constFind(tubeId);
            if (it != m_Testcups.constEnd()) {
                mEmptyTubeAssigned.insert(it.key(), it.value());
                mEmptyText.insert(tubeId, sampleNum);
            }
        }
        if (cglobal::g_StartTesting && index_add == all_add_task) {
            emit testingaddsample();
        }
    }
    update();
}

void Testing::tubeMarked(const QString& outtext, const quint8& tubeIndex)
{
    {
        QMutexLocker locker(&m_tubeMutex);
        auto it = m_Testcups.find(tubeIndex);
        if (it != m_Testcups.end()) {
            mEmptyTubeAssigned.insert(tubeIndex, it.value());
            mEmptyText[tubeIndex] = outtext;
        }
        emit SynchronizeEmptyTube(tubeIndex);
    }
    update();
}


void Testing::EmptyTubeAssigned(quint8 IndexTube)
{
    {
        QMutexLocker locker(&m_tubeMutex);
        if (m_Testcups.contains(IndexTube)) {
            auto emptytube = m_Testcups.find(IndexTube);
            int IndexHole = emptytube.key();
            mEmptyTubeAssigned.insert(IndexHole, emptytube.value());
            QLOG_DEBUG() << "试管:" << emptytube.key() << "标记选中";
            int sample_id = StructInstance::getInstance()->sycn_emptytube_get_sampleid(IndexTube);
            mEmptyText.insert(IndexTube, QString::number(sample_id));
        }
    }
    update();
}

void Testing::replacEmptyTestTary(int index_tary)
{
    int _statrHole = ONETRAY_TOTALTUBE * index_tary;
    int _endHole = _statrHole + ONETRAY_TOTALTUBE;
    for (; _statrHole < _endHole; ++_statrHole)
        VectorReplaceClear(_statrHole);
    FullyAutomatedPlatelets::pinstancesqlData()->UpadteTrayState(index_tary);
    QLOG_ERROR() << "更换盘" << QString::number(index_tary + 1) << "完成";
    update();
}

void Testing::VectorReplaceClear(quint8 Index)
{
    //QMutexLocker locker(&m_tubeMutex);
    auto eraseIfFound = [this](auto& container, quint8 idx) {
        auto it = container.find(idx);
        if (it != container.end()) container.erase(it);
    };
    eraseIfFound(mEmptyTubeAssigned, Index);
    eraseIfFound(mEmptyTubeAbsorb_Poorblood, Index);
    eraseIfFound(mEmptyTubeAbsorb_Richblood, Index);
    eraseIfFound(mEmptyTubeClipMoved, Index);
    eraseIfFound(mEmptyText, Index);
}

// ==================== 血样孔绘制相关 ====================
QFont ConfigureTextstyle(int size)
{
    QFont Textfont;
    Textfont.setFamily("楷体");
    Textfont.setPixelSize(size);
    Textfont.setLetterSpacing(QFont::AbsoluteSpacing, 1);
    return Textfont;
}

Testing::BloodAreaLayout Testing::computeBloodAreaLayout() const
{
    BloodAreaLayout layout;
    quint8 equipmentKind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentKind);
    switch (equipmentKind) {
        case KS600: layout.totalCols = 6; break;
        case KS800: layout.totalCols = 8; break;
        case KS1200: layout.totalCols = 12; break;
        default: layout.totalCols = 12; break;
    }
    QRect bloodRect = ui->widget_Sample_1->rect();
    constexpr int kSpaceTopY = 30;
    int bloodZoneHeight = bloodRect.height() - kSpaceTopY;
    constexpr int totalRows = 5;
    layout.perHoleHeight = bloodZoneHeight / totalRows;

    int bloodAreasWidth = 0;
    switch (equipmentKind) {
        case KS600:
            bloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() -
                              ui->widget_TestCup_0->geometry().bottomLeft().x();
            break;
        case KS800:
            bloodAreasWidth = ui->widget_TestCup_2->geometry().bottomRight().x() -
                              ui->widget_TestCup_0->geometry().bottomLeft().x();
            break;
        case KS1200:
            bloodAreasWidth = ui->widget_TestCup_3->geometry().bottomRight().x() -
                              ui->widget_TestCup_0->geometry().bottomLeft().x();
            break;
        default:
            bloodAreasWidth = ui->widget_TestCup_1->geometry().bottomRight().x() -
                              ui->widget_TestCup_0->geometry().bottomLeft().x();
            break;
    }
    QPoint cupGlobalPos = ui->widget_TestCup_0->mapToGlobal(QPoint(0, 0));
    int spaceLeft = ui->widget_Sample_1->mapFromGlobal(cupGlobalPos).x();
    const int columnSpacing = 5;
    layout.oneWidth = (bloodAreasWidth - (layout.totalCols - 1) * columnSpacing) / layout.totalCols;
    layout.columnStartX.clear();
    for (int n = 0; n < layout.totalCols; ++n) {
        int x = spaceLeft + n * layout.oneWidth + n * columnSpacing;
        layout.columnStartX.append(x);
    }
    int perHoleWidth = layout.oneWidth / 2;
    int diameter = static_cast<int>(std::min(perHoleWidth, layout.perHoleHeight) * 0.95);
    if (diameter < 16) diameter = 16;
    layout.bigRadius = (diameter - 2) / 2;
    layout.smallRadius = qAbs(layout.bigRadius - 2);
    return layout;
}

void Testing::CreatBloodZoneAxisPos()
{
    m_BloodHoleInitUiAxis.clear();
    m_BloodHoleNum.clear();

    BloodAreaLayout layout = computeBloodAreaLayout();
    m_BloodBigRadius = layout.bigRadius;
    m_BloodSmallRadius = layout.smallRadius;

    const int rowSpacing = 2;
    const int totalRows = 5;
    const int topMargin = 30;

    // 贫血孔（左列）
    for (int col = 0; col < layout.totalCols; ++col) {
        int colStartX = layout.columnStartX[col];
        int leftCenterX = colStartX + layout.oneWidth * 0.28;
        for (int row = 0; row < totalRows; ++row) {
            int y = topMargin + m_BloodBigRadius + row * (2 * m_BloodBigRadius + rowSpacing);
            int holeNumber = 2 * row + 10 * col;
            m_BloodHoleInitUiAxis.insert(holeNumber, QPoint(leftCenterX, y));
            m_BloodHoleNum.insert(holeNumber, QString::number(holeNumber + 1));
        }
    }
    // 富血孔（右列）
    for (int col = 0; col < layout.totalCols; ++col) {
        int colStartX = layout.columnStartX[col];
        int rightCenterX = colStartX + layout.oneWidth * 0.72;
        for (int row = 0; row < totalRows; ++row) {
            int y = topMargin + m_BloodBigRadius + row * (2 * m_BloodBigRadius + rowSpacing);
            int holeNumber = 2 * row + 1 + 10 * col;
            m_BloodHoleInitUiAxis.insert(holeNumber, QPoint(rightCenterX, y));
            m_BloodHoleNum.insert(holeNumber, QString::number(holeNumber + 1));
        }
    }
}

void Testing::DrawBloodTopText(QPainter& painter)
{
    BloodAreaLayout layout = computeBloodAreaLayout();
    QFont font = ConfigureTextstyle(18);
    painter.setFont(font);

    QStringList topTexts = {"A","B","C","D","E","F","G","H","I","J","K","L"};
    const int textHeight = 30;
    painter.setPen(cglobal::g_TextColorBlack);
    QFontMetrics fm(font);
    for (int n = 0; n < layout.totalCols && n < topTexts.size(); ++n) {
        int textCenterX = layout.columnStartX[n] + layout.oneWidth / 2;
        QString text = topTexts[n];
        QRect textRect = fm.boundingRect(text);
        QPoint textPos(textCenterX - textRect.width() / 2, textHeight - 2);
        painter.drawText(textPos, text);
    }
}

void Testing::drawBloodHole(QPainter& painter, quint8 holeId, const QPoint& center,
                            int state, const QString& showText)
{
    QColor bgColor;
    switch (state) {
        case TUBE_INIT:      bgColor = kHoleEmpty; break;
        case TUBE_CHECKED:   bgColor = (holeId % 2 == 0) ? kHolePoor : kHoleRich; break;
        case TUBE_OUTRESULT: bgColor = kHoleUsed; break;
        default:             bgColor = kHoleEmpty; break;
    }

    // 保存当前画笔（外圈线条颜色）
    QPen oldPen = painter.pen();

    painter.setBrush(Qt::NoBrush);
    drawStereoRing(painter, QPointF(center), m_BloodBigRadius, m_BloodSmallRadius, bgColor);

    if (!showText.isEmpty()) {
        painter.setPen((holeId % 2 == 0) ? cglobal::g_TextColorRoorblood : cglobal::g_TextColorRichblood);
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(showText);
        painter.drawText(center.x() - textRect.width() / 2,
                         center.y() + textRect.height() / 2 - 2,
                         showText);
        painter.setPen(oldPen);
    }
    painter.setPen(oldPen);
}

void Testing::UpdateBloodHoleColors(QPainter& painter, int State, QMap<quint8, QPoint>& MapBloodHole)
{
    if (MapBloodHole.isEmpty()) return;

    static const QFont font = ConfigureTextstyle(11);
    painter.setFont(font);
    QPen pen(cglobal::g_LineColor, 1);
    painter.setPen(pen);

    for (auto it = MapBloodHole.constBegin(); it != MapBloodHole.constEnd(); ++it) {
        quint8 holeId = it.key();
        QString showText = m_BloodHoleNum.value(holeId);
        drawBloodHole(painter, holeId, it.value(), State, showText);
    }
    DrawBloodHoleInnerText(painter, MapBloodHole);
}

void Testing::DrawPutBloodReminders(QPainter& painter)
{
    QList<quint8> reminderHoles;
    QMap<quint8, quint8> stages;
    {
        QReadLocker locker(&m_bloodMutex);
        reminderHoles = mreminderPPPandPRPhole;
        stages = m_bloodHoleStage;
    }
    if (reminderHoles.isEmpty()) {
        return;
    }

    auto findCenter = [this](quint8 hole, QPoint &out) -> bool {
        auto itInit = m_BloodHoleInitUiAxis.constFind(hole);
        if (itInit != m_BloodHoleInitUiAxis.cend()) {
            out = itInit.value();
            return true;
        }
        auto itChecked = m_Blood_Tray_Checked.constFind(hole);
        if (itChecked != m_Blood_Tray_Checked.cend()) {
            out = itChecked.value();
            return true;
        }
        auto itDone = m_Blood_Tray_OutResult.constFind(hole);
        if (itDone != m_Blood_Tray_OutResult.cend()) {
            out = itDone.value();
            return true;
        }
        return false;
    };

    auto stageOf = [&stages](quint8 stored, quint8 prp, quint8 ppp) -> quint8 {
        if (stages.contains(stored)) {
            return stages.value(stored);
        }
        if (stages.contains(prp)) {
            return stages.value(prp);
        }
        if (stages.contains(ppp)) {
            return stages.value(ppp);
        }
        return BLOOD_HINT_PLACE;
    };

    // 行距只有 2px，高度必须收在孔半径内，描边也不挤到邻行
    const qreal padX = 2.0;

    for (quint8 stored : reminderHoles) {
        const quint8 prp = (stored % 2 == 0) ? static_cast<quint8>(stored + 1) : stored;
        const quint8 ppp = (prp > 0) ? static_cast<quint8>(prp - 1) : 0;
        QPoint pppCenter;
        QPoint prpCenter;
        const bool hasPpp = findCenter(ppp, pppCenter);
        const bool hasPrp = findCenter(prp, prpCenter);
        if (!hasPpp && !hasPrp) {
            continue;
        }

        QPoint left = hasPpp ? pppCenter : prpCenter;
        QPoint right = hasPrp ? prpCenter : pppCenter;
        if (left.x() > right.x()) {
            qSwap(left, right);
        }

        const quint8 stage = stageOf(stored, prp, ppp);
        QColor hint = cglobal::g_ReminderBloodHoleColor;
        QString tag = QString::fromUtf8("\xE6\x94\xBE\xE7\xBD\xAE"); // 放置
        bool pulse = true;
        if (stage == BLOOD_HINT_ADDING) {
            hint = QColor(255, 145, 0, 230);
            tag = QString::fromUtf8("\xE5\x8A\xA0\xE6\xA0\xB7\xE4\xB8\xAD"); // 加样中
        } else if (stage == BLOOD_HINT_TESTING) {
            hint = QColor(70, 130, 255, 230);
            tag = QString::fromUtf8("\xE6\xB5\x8B\xE8\xAF\x95\xE4\xB8\xAD"); // 测试中
            pulse = false;
        } else if (stage == BLOOD_HINT_DONE) {
            hint = QColor(46, 170, 100, 230);
            tag = QString::fromUtf8("\xE5\xAE\x8C\xE6\x88\x90"); // 完成
            pulse = false;
        }

        const int haloAlpha = pulse ? (m_blinkState ? 70 : 36) : 52;
        const qreal ringW = pulse ? (m_blinkState ? 2.0 : 1.5) : 1.6;
        const qreal halfH = qMax(1.0, m_BloodBigRadius - ringW * 0.5 - 0.5);

        QRectF capsule(left.x() - m_BloodBigRadius - padX,
                       left.y() - halfH,
                       (right.x() - left.x()) + 2.0 * m_BloodBigRadius + 2.0 * padX,
                       2.0 * halfH);

        QPainterPath path;
        path.addRoundedRect(capsule, halfH, halfH);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(hint.red(), hint.green(), hint.blue(), haloAlpha));
        painter.drawPath(path);

        QPen ring(hint, ringW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(ring);
        painter.setBrush(Qt::NoBrush);
        painter.drawPath(path);

        const int fontPx = qBound(7, static_cast<int>(capsule.height() * 0.48), 10);
        QFont tagFont = ConfigureTextstyle(fontPx);
        QFontMetrics fm(tagFont);
        if (fm.height() + 2 <= capsule.height() && fm.width(tag) + 2 <= capsule.width()) {
            painter.setFont(tagFont);
            painter.setPen(QColor(12, 42, 70));
            painter.drawText(capsule, Qt::AlignCenter, tag);
        }
    }
}

void Testing::DrawBloodHoleInnerText(QPainter& painter, const QMap<quint8, QPoint>& BloodHoleMap)
{
    if (BloodHoleMap.isEmpty()) return;
    QFont font = ConfigureTextstyle(14);
    painter.setFont(font);
    QFontMetrics fm(font);

    static const QMap<quint8, QString> numberMap = {
        {0, "1"}, {2, "2"}, {4, "3"}, {1, "4"}, {3, "5"}
    };
    constexpr int margin = 2;
    for (auto it = BloodHoleMap.constBegin(); it != BloodHoleMap.constEnd(); ++it) {
        if (it.key() % 2 == 0) {
            auto numIt = numberMap.find(it.key() % 5);
            if (numIt == numberMap.end()) continue;
            QString text = numIt.value();
            QRect textRect = fm.boundingRect(text);
            int leftEdgeOfHole = it.value().x() - m_BloodBigRadius;
            int textX = leftEdgeOfHole - margin - textRect.width();
            QPoint textPos(textX, it.value().y() + textRect.height() / 2 - 2);
            painter.drawText(textPos, text);
        }
    }
}

void Testing::mousePressEvent(QMouseEvent *event)
{
    int mouse_posx = event->x();
    int mouse_posy = event->y();
    if (event->button() == Qt::RightButton) {
        ClickCanelTask(mouse_posx, mouse_posy);
    }
}

void Testing::ClickCanelTask(int mouse_posx, int mouse_posy)
{
    if (!ui || !ui->widget_Sample_1 || !this) {
        QLOG_ERROR() << "UI components not properly initialized";
        return;
    }
    int leftPos, rightPos, topPos, bottomPos;
    for (auto it = m_Blood_Tray_Checked.constBegin(); it != m_Blood_Tray_Checked.constEnd(); ++it) {
        QPoint actualPos = this->mapFromGlobal(ui->widget_Sample_1->mapToGlobal(it.value()));
        int actualX = actualPos.x();
        int actualY = actualPos.y();
        leftPos = actualX - m_BloodBigRadius;
        rightPos = actualX + m_BloodBigRadius;
        topPos = actualY - m_BloodBigRadius;
        bottomPos = actualY + m_BloodBigRadius;

        if ((mouse_posx >= leftPos && mouse_posx <= rightPos) &&
            (mouse_posy >= topPos && mouse_posy <= bottomPos)) {
            if (cglobal::g_StartTesting) {
                FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("取消失败", "样本测试中请勿取消测试样本!");
                return;
            }
            int clickedHole = it.key();
            if (clickedHole % 2 == BLOOD_HOLE_TYPE_EVEN) {
                QList<int> sample_tasklist, sampleidList;
                StructInstance::getInstance()->cancel_sample_task(sample_tasklist, sampleidList);
                clickAnaemiaHoleCancelTestTask(sampleidList);
            }
            break;
        }
    }
    update();
}

void Testing::clickAnaemiaHoleCancelTestTask(QList<int> waitTestSampleId)
{
    CancelTaskConfigure *pCancelwaitTestTask = new CancelTaskConfigure(this, waitTestSampleId);
    connect(pCancelwaitTestTask, &CancelTaskConfigure::makesureCacelIdList, this, [=](QList<int> cancelsampleid) {
        QString date = GlobalData::ObatinCreatSampleTime();
        for (int cancelwaittestsampleid : cancelsampleid) {
            int cancelTubeTotalnum = 0;
            quint8 cancelAnemiaHole = 0;
            QList<quint8> CancelEmptyTubeList;
            quint8 cancelBackHole = 0;
            QString sampleid = GlobalData::groupDateAndID(date, cancelwaittestsampleid);
            FullyAutomatedPlatelets::pinstancesqlData()->deleteTestResultTable(sampleid);
            FullyAutomatedPlatelets::pinstancesqlData()->deleteTestCurveDataTale(sampleid);
            StructInstance::getInstance()->cancelsampletask(cancelwaittestsampleid, cancelTubeTotalnum,
                                                            cancelAnemiaHole, CancelEmptyTubeList, cancelBackHole);
            m_ProTotalTube -= cancelTubeTotalnum;
            auto* sampleTest = FullyAutomatedPlatelets::pinstanceAddsampletest();
            if (sampleTest) sampleTest->backcancelhole(cancelBackHole);
            CanceltaskbackTubecolor(cancelAnemiaHole, cancelBackHole, CancelEmptyTubeList);
            FullyAutomatedPlatelets::pinstancepatientdata()->cancelSampleResultItem(sampleid);
        }
    });
    pCancelwaitTestTask->setAttribute(Qt::WA_DeleteOnClose);
    pCancelwaitTestTask->exec();
}


void Testing::onsycnDelTestSample(const QString delSampleId)
{
    int cancelTubeTotalnum = 0;
    quint8 cancelAnemiaHole = 0;
    QList<quint8> CancelEmptyTubeList;
    quint8 cancelBackHole = 0;
    QString dataDel;
    int delTodayId = 0;
    GlobalData::apartSampleId(delSampleId,dataDel, delTodayId);
    const QString  todayDate = GlobalData::ObatinCreatSampleTime();
    auto *pinstance = StructInstance::getInstance();
    auto* sampleTest = FullyAutomatedPlatelets::pinstanceAddsampletest();
    if(todayDate == dataDel && pinstance && sampleTest)
    {
        pinstance->cancelsampletask(delTodayId, cancelTubeTotalnum,
                                    cancelAnemiaHole, CancelEmptyTubeList, cancelBackHole);

        m_ProTotalTube -= cancelTubeTotalnum;
        sampleTest->backcancelhole(cancelBackHole);
        CanceltaskbackTubecolor(cancelAnemiaHole, cancelBackHole, CancelEmptyTubeList);
    }
    return;
}

void Testing::CanceltaskbackTubecolor(int PPPHole, int PRPHoles, QList<quint8> emptytubehole)
{
    auto processHole = [this](int hole) {
        QWriteLocker locker(&m_bloodMutex);
        auto it = m_Blood_Tray_Checked.find(hole);
        if (it != m_Blood_Tray_Checked.end()) {
            m_BloodHoleInitUiAxis.insert(it.key(), it.value());
            m_Blood_Tray_Checked.erase(it);
            m_BloodHoleNum[hole] = QString("%1").arg(hole + 1);
        }
    };
    processHole(PPPHole);
    processHole(PRPHoles);
    {
        QWriteLocker locker(&m_bloodMutex);
        mreminderPPPandPRPhole.removeOne(static_cast<quint8>(PRPHoles));
        mreminderPPPandPRPhole.removeOne(static_cast<quint8>(PPPHole));
        m_bloodHoleStage.remove(static_cast<quint8>(PRPHoles));
        m_bloodHoleStage.remove(static_cast<quint8>(PPPHole));
        if (PPPHole > 0) {
            mreminderPPPandPRPhole.removeOne(static_cast<quint8>(PPPHole + 1));
            m_bloodHoleStage.remove(static_cast<quint8>(PPPHole + 1));
        }
    }

    //QMutexLocker locker(&m_tubeMutex);
    for (int tube : emptytubehole) {
        FullyAutomatedPlatelets::pinstancesqlData()->update_test_tube_status(tube, TESTTUBES_FREETIME, "noone", 0);
        auto emptyIt = mEmptyTubeAssigned.find(tube);
        if (emptyIt != mEmptyTubeAssigned.end()) mEmptyTubeAssigned.erase(emptyIt);
        auto textIt = mEmptyText.find(tube);
        if (textIt != mEmptyText.end()) mEmptyText.erase(textIt);
    }
}

void Testing::VectorSelectedBloodAreaTube(quint8 IndexTube, QString Samplenum)
{
    QWriteLocker locker(&m_bloodMutex);
    auto itInit = m_BloodHoleInitUiAxis.find(IndexTube);
    if (itInit != m_BloodHoleInitUiAxis.end()) {
        m_Blood_Tray_Checked.insert(itInit.key(), itInit.value());
        m_BloodHoleInitUiAxis.erase(itInit);
        QString numStr = Samplenum.split("-", QString::SkipEmptyParts).last();
        auto TextNum = m_BloodHoleNum.find(IndexTube);
        if (TextNum != m_BloodHoleNum.end())
            TextNum.value().swap(numStr);
    }
}

void Testing::updateBloodHoleHintStage(quint8 anemiaHole, quint8 richHole, quint8 stage)
{
    {
        QWriteLocker locker(&m_bloodMutex);
        auto apply = [this, stage](quint8 hole) {
            if (hole == 0 || hole == INVALID_HOLE_VALUE) {
                return;
            }
            if (!mreminderPPPandPRPhole.contains(hole)) {
                mreminderPPPandPRPhole.push_back(hole);
            }
            m_bloodHoleStage[hole] = stage;
        };
        apply(richHole);
        apply(anemiaHole);
    }
    if (mtimer && !mtimer->isActive()) {
        mtimer->start(500);
    }
    if (ui->widget_Sample_1) {
        ui->widget_Sample_1->update();
    }
}

void Testing::SampleTestingChangInitColor(QPoint BloodPoint, quint8 FinishChannel)
{
    quint8 pppHole = BloodPoint.x();
    quint8 prpHole = BloodPoint.y();
    QLOG_DEBUG() << "PPP血样孔号:" << pppHole << "PRP血样孔号:" << prpHole;

    auto processHole = [this](quint8 hole) {
        auto it = m_Blood_Tray_Checked.find(hole);
        if (it != m_Blood_Tray_Checked.end()) {
            m_Blood_Tray_OutResult.insert(it.key(), it.value());
            m_Blood_Tray_Checked.erase(it);
        }
    };
    processHole(pppHole);
    processHole(prpHole);
    updateBloodHoleHintStage(pppHole, prpHole, BLOOD_HINT_DONE);

    quint8 completedChannel = FinishChannel - 1;
    if (completedChannel < static_cast<quint8>(m_channelCount)) {
        DrawChannelProgress(completedChannel, 0);
        if (!Channelreminder[completedChannel].isNull())
            Channelreminder[completedChannel]->hide();
    }
    for (auto it = m_TestingSample.begin(); it != m_TestingSample.end(); ) {
        if (it.value() == FinishChannel) it = m_TestingSample.erase(it);
        else ++it;
    }
}

void Testing::ComplBackColorBloodArea()
{
    QWriteLocker locker(&m_bloodMutex);
    m_Blood_Tray_OutResult.clear();
    m_Blood_Tray_Checked.clear();
    mreminderPPPandPRPhole.clear();
    m_bloodHoleStage.clear();
    CreatBloodZoneAxisPos();
}

// ==================== 空试管区绘制 ====================
void Testing::drawTray(int trayIndex, QWidget* trayWidget)
{
    if (!trayWidget) return;
    ensureTrayReady(trayIndex);
    DrawTrayTestTubeUiAxis(trayWidget, static_cast<quint8>(trayIndex), m_BigRadius, m_SmalleRadius);
}

void Testing::CreatTrayTestTubeUiAxis(int trayIndex)
{
    constexpr quint8 Total_Cols = 6;
    constexpr quint8 Total_Rows = 10;
    constexpr int spacing = 3;

    QWidget* trayWidgets[] = {ui->widget_TestCup_0, ui->widget_TestCup_1,
                              ui->widget_TestCup_2, ui->widget_TestCup_3};
    if (trayIndex < 0 || trayIndex > 3) return;
    QWidget* trayWidget = trayWidgets[trayIndex];
    if (!trayWidget) return;

    quint8 startHole = trayIndex * TUBES_PER_TRAY;
    int width = trayWidget->width();
    int height = trayWidget->height();

    const int colWidth = width / Total_Cols;
    const int rowHeight = height / Total_Rows;
    int diameter = static_cast<int>(std::min(colWidth, rowHeight) * 0.95);
    if (diameter < MIN_TUBE_DIAMETER) diameter = MIN_TUBE_DIAMETER;
    m_BigRadius = (diameter - spacing) / 2;
    m_SmalleRadius = qAbs(m_BigRadius - 2);

    for (int i = startHole; i < startHole + TUBES_PER_TRAY; ++i) {
        m_Testcups.remove(i);
    }

    for (int r = 0; r < Total_Rows; ++r) {
        int y = r * rowHeight + rowHeight / 2;
        for (int c = 0; c < Total_Cols; ++c) {
            int x = c * colWidth + colWidth / 2;
            m_Testcups.insert(startHole++, QPointF(x, y));
        }
    }
}

void Testing::drawTrayTube(QPainter& painter, const QPointF& center, quint8 holeNum,
                           quint8 bigRadius, quint8 smallRadius)
{
    QColor bgColor;
    //QMutexLocker locker(&m_tubeMutex);
    if (mEmptyTubeAssigned.contains(holeNum)) {
        bgColor = kHoleSelected;
    } else if (mEmptyTubeAbsorb_Poorblood.contains(holeNum)) {
        bgColor = kHolePoor;
    } else if (mEmptyTubeAbsorb_Richblood.contains(holeNum)) {
        bgColor = kHoleRich;
    } else if (mEmptyTubeClipMoved.contains(holeNum)) {
        bgColor = kHoleUsed;
    } else {
        bgColor = kHoleEmpty;
    }

    if (mflashingTubeList.contains(holeNum)) {
        if (m_blinkState)
            bgColor = QColor(255, 255, 255);
        else
            bgColor = QColor(255, 0, 0);
    }

    drawStereoRing(painter, center, bigRadius, smallRadius, bgColor);

    QString sampleText = mEmptyText.value(holeNum);
    if (!sampleText.isEmpty()) {
        painter.setPen(cglobal::g_TextColorWhite);
        QFontMetrics fm(painter.font());
        QRect textRect = fm.boundingRect(sampleText);
        QPoint textPos(center.x() - textRect.width() / 2, center.y() + textRect.height() / 2 - 2);
        painter.drawText(textPos, sampleText);
    }
}

void Testing::DrawTrayTestTubeUiAxis(QWidget* pTrayWidget, quint8 IndexTray,
                                     quint8 BigRadius, quint8 SmalleRadius)
{
    QPainter painter(pTrayWidget);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QFont font = ConfigureTextstyle(11);
    painter.setFont(font);

    quint8 FirstHole = 0, EndHoleNum = 0;
    QMap<quint8, QPointF>::iterator TrayTubeFirst;
    switch (IndexTray) {
        case 0: TrayTubeFirst = m_Testcups.find(0); FirstHole = 0; EndHoleNum = 60; break;
        case 1: TrayTubeFirst = m_Testcups.find(60); FirstHole = 60; EndHoleNum = 120; break;
        case 2: TrayTubeFirst = m_Testcups.find(120); FirstHole = 120; EndHoleNum = 180; break;
        case 3: TrayTubeFirst = m_Testcups.find(180); FirstHole = 180; EndHoleNum = 240; break;
        default: return;
    }
    if (TrayTubeFirst == m_Testcups.end()) return;

    while (TrayTubeFirst != m_Testcups.end() && TrayTubeFirst.key() < EndHoleNum) {
        drawTrayTube(painter, TrayTubeFirst.value(), TrayTubeFirst.key(), BigRadius, SmalleRadius);
        ++TrayTubeFirst;
    }
}

void Testing::recvBloodSuck2EmptyTube(bool banemia, quint8 IndexTube)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "recvBloodSuck2EmptyTube", Qt::QueuedConnection,
                                  Q_ARG(bool, banemia), Q_ARG(quint8, IndexTube));
        return;
    }
    {
        QMutexLocker locker(&m_tubeMutex);
        auto it = mEmptyTubeAssigned.find(IndexTube);
        if (it != mEmptyTubeAssigned.end()) {
            auto& targetMap = banemia ? mEmptyTubeAbsorb_Poorblood : mEmptyTubeAbsorb_Richblood;
            targetMap.insert(IndexTube, it.value());
            mEmptyTubeAssigned.erase(it);
            FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(IndexTube, TESTTUBES_SAMPLED);
        }
    }
    update();
}

void Testing::giveupSampleShowHole(const QList<quint8>& holeList)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "giveupSampleShowHole", Qt::QueuedConnection,
                                  Q_ARG(QList<quint8>, holeList));
        return;
    }
    {
        QMutexLocker locker(&m_flashMutex);
        for (quint8 hole : holeList) {
            if (!mflashingTubeList.contains(hole))
                mflashingTubeList.append(hole);
        }
    }
}

void Testing::giveupSampleChannelFlash(bool isChannelNormal, quint8 indexChannel)
{
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "giveupSampleChannelFlash", Qt::QueuedConnection,
                                  Q_ARG(bool, isChannelNormal), Q_ARG(quint8, indexChannel));
        return;
    }
    if (indexChannel >= static_cast<quint8>(m_channelCount)) return;
    m_channelShowsTheProgress[indexChannel]->flashingReminder(isChannelNormal);
    QColor colors;
    colors.setRgb(isChannelNormal ? 0 : 255, 0, 0);
    QLOG_WARN() << "通道" << indexChannel + 1 << "测试状态:" << isChannelNormal;
    if (!Channelreminder[indexChannel].isNull()) {
        QPalette palette = Channelreminder[indexChannel]->palette();
        palette.setColor(QPalette::WindowText, colors);
        Channelreminder[indexChannel]->setPalette(palette);
    }
}

//试管被使用掉了
void Testing::EmptyTubeClipMoved(quint8 IndexTube)
{
	// 显式捕获 this 和 IndexTube
	auto moveTube = [this, IndexTube](QMap<quint8, QPointF>& source, QMap<quint8, QPointF>& dest) {
		auto it = source.find(IndexTube);
		if (it != source.end()) {
			dest.insert(it.key(), std::move(it.value()));
			emit SynchronizeEmptyTube(it.key());   // 假设信号参数为 quint8
			source.erase(it);
		}
	};
    moveTube(mEmptyTubeAbsorb_Poorblood, mEmptyTubeClipMoved);
    moveTube(mEmptyTubeAbsorb_Richblood, mEmptyTubeClipMoved);

    FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(IndexTube, TESTTUBES_CLIPPEDAWAY);
    repaint();
}

// ==================== 试剂绘制 ====================
void Testing::plotReagentWellsCoordinates(quint8 totalReagents)
{
    m_Reagent_Tray.clear();
    QPoint ReagentWellpos;
    for (int i = 0; i < totalReagents; ++i) {
        (i % 2 == 0) ? ReagentWellpos.setX(BIG_RADIUS_DEFAULT + 3) : ReagentWellpos.setX(BIG_RADIUS_DEFAULT + (2 * BIG_RADIUS_DEFAULT + 10));
        ReagentWellpos.setY(REAGENT_TOP_BOTTOM_SPACE + BIG_RADIUS_DEFAULT + (i/2) * (REAGENT_TOP_BOTTOM_SPACE + 2 * BIG_RADIUS_DEFAULT));
        m_Reagent_Tray.insert(i, ReagentWellpos);
    }
}

void Testing::showPaintReagents()
{
    QPainter painter(ui->widget_Reagents);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QStringList NameText{"AA","ADP","EPI","COL","RIS"};
    QFont font = ConfigureTextstyle(13);
    painter.setFont(font);
    quint8 index = 0;
    for (auto it = m_Reagent_Tray.begin(); it != m_Reagent_Tray.end(); ++it) {
        quint8 indexreag = it.key()/2 + 1;
        const QColor rim = GlobalData::customCurveColor(indexreag);
        drawStereoRing(painter, it.value(), BIG_RADIUS_DEFAULT, SMALL_RADIUS_DEFAULT,
                       kHoleEmpty, rim);
        painter.setPen(rim);
        QString name = NameText.at(index/2);
        if (name.length() >= 3)
            painter.drawText(QPoint(it.value().x() - 11, it.value().y() + 5), name);
        else
            painter.drawText(QPoint(it.value().x() - 8, it.value().y() + 5), name);
        ++index;
    }
}

void Testing::showCleanreagent()
{
    QPainter painter(ui->widget_cleanagent);
    QFont font = ConfigureTextstyle(12);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing, true);
    m_CleanningReagent.setX(ui->widget_cleanagent->width() >> 1);
    m_CleanningReagent.setY(ui->widget_cleanagent->height() >> 1);
    drawStereoRing(painter, m_CleanningReagent, TOP_DRAW_BIG_RADIUS, TOP_DRAW_SMALL_RADIUS,
                   kHoleEmpty);
    painter.setPen(cglobal::g_TextColorBlack);
    painter.drawText(QPoint(m_CleanningReagent.x() - 22, m_CleanningReagent.y() + 5), tr("清洗剂S1"));
}

void Testing::showDiscardTheCup()
{
    QPainter painter(ui->DroptheCup);
    QFont font = ConfigureTextstyle(14);
    painter.setFont(font);
    painter.setRenderHint(QPainter::Antialiasing, true);
    int centerX = ui->DroptheCup->width() / 2;
    int centerY = ui->DroptheCup->height() / 2;
    m_DiscardCupHole.setX(centerX);
    m_DiscardCupHole.setY(centerY);

    drawStereoRing(painter, m_DiscardCupHole, TOP_DRAW_BIG_RADIUS, TOP_DRAW_SMALL_RADIUS,
                   kHoleEmpty);
    painter.setPen(QColor(0, 0, 0));
    QString text = tr("弃杯孔");
    QRect textRect = painter.fontMetrics().boundingRect(text);
    QPoint textPos(centerX - textRect.width() / 2, centerY + textRect.height() / 3);
    painter.drawText(textPos, text);
}

void Testing::repPrpheight(const QString& idnum, bool falgrep)
{
	emit sendRepPrpheight(idnum, falgrep);
}
