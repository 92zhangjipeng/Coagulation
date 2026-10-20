#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif
#include "customqctestwidget.h"
#include "Custommovements/performanceverification.h"
#include "custom_style/custommessagebox.h"
#include "operclass/fullyautomatedplatelets.h"
#include "cglobal.h"
#include "QsLog/include/QsLog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QHeaderView>
#include <QDateTime>
#include <QSettings>
#include <QCoreApplication>
#include <QButtonGroup>
#include <QSplitter>
#include <QScrollArea>
#include <QFrame>
#include <QGridLayout>
#include <QFile>
#include <cmath>

namespace {

QString qcUi(const char *utf8Text)
{
    return QString::fromUtf8(utf8Text);
}

QTableWidgetItem *centeredItem(const QString &text)
{
    auto *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    return item;
}

QString timestampKey(const QString &channelText)
{
    const QString ts = QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMddHHmm"));
    const QString ch = QStringLiteral("%1").arg(channelText.toInt(), 2, 10, QLatin1Char('0'));
    return ts + ch;
}

int channelCountForEquipment(quint8 kind)
{
    switch (kind) {
    case KS600: return 4;
    case KS800: return 8;
    case KS1200: return 12;
    default: return 12;
    }
}

} // namespace

CustomQcTestWidget::CustomQcTestWidget(quint8 equipmentKind, QWidget *parent)
    : QWidget(parent)
    , m_equipmentKind(equipmentKind)
    , m_chartMonth(QDate::currentDate())
{
    setStyleSheet(QStringLiteral("CustomQcTestWidget { background-color: #F5F7FA; }"));
    buildUi();
    loadSettings();
    refreshTrendChart();

    connect(Performanceverification::instance(), &Performanceverification::customQcResultReady,
            this, &CustomQcTestWidget::applyResult);
}

void CustomQcTestWidget::buildUi()
{
    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(10, 10, 10, 10);
    root->setSpacing(8);

    auto *topBar = new QHBoxLayout();
    m_lblRunStatus = new QLabel(qcUi("\xe5\xb0\xb1\xe7\xbb\xaa"));
    m_lblRunStatus->setStyleSheet(
        QStringLiteral("padding:6px 12px; background:#ECEFF1; border-radius:4px; font-weight:bold;"));
    topBar->addWidget(m_lblRunStatus);
    topBar->addStretch();
    m_lblMonth = new QLabel(QStringLiteral("%1-%2")
                            .arg(m_chartMonth.year())
                            .arg(m_chartMonth.month(), 2, 10, QLatin1Char('0')));
    m_lblMonth->setStyleSheet(QStringLiteral("font:bold 14px; min-width:120px;"));
    topBar->addWidget(m_lblMonth);
    auto *btnPrev = new QPushButton(QStringLiteral("<"));
    auto *btnNext = new QPushButton(QStringLiteral(">"));
    auto *btnToday = new QPushButton(qcUi("\xe4\xbb\x8a\xe6\x9c\x88"));
    btnPrev->setFixedWidth(36);
    btnNext->setFixedWidth(36);
    topBar->addWidget(btnPrev);
    topBar->addWidget(btnNext);
    topBar->addWidget(btnToday);
    topBar->addSpacing(16);
    m_btnStart = new QPushButton(qcUi("\xe5\xbc\x80\xe5\xa7\x8b\xe8\x87\xaa\xe7\xa0\x94\xe8\xb4\xa8\xe6\x8e\xa7"));
    m_btnStart->setMinimumHeight(40);
    m_btnStart->setMinimumWidth(160);
    QFile styleFile(QStringLiteral(":/Picture/SetPng/wholePushbutton.qss"));
    if (styleFile.open(QIODevice::ReadOnly)) {
        m_btnStart->setStyleSheet(QString::fromLatin1(styleFile.readAll()));
        styleFile.close();
    }
    topBar->addWidget(m_btnStart);
    root->addLayout(topBar);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setChildrenCollapsible(false);

    auto *leftScroll = new QScrollArea(splitter);
    leftScroll->setWidgetResizable(true);
    leftScroll->setFrameShape(QFrame::NoFrame);
    leftScroll->setMinimumWidth(280);
    leftScroll->setMaximumWidth(340);
    auto *left = new QWidget(leftScroll);
    auto *leftLayout = new QVBoxLayout(left);
    leftLayout->setSpacing(10);

    auto *targetBox = new QGroupBox(qcUi("\xe8\xb4\xa8\xe6\x8e\xa7\xe9\x9d\xb6\xe5\x80\xbc\xe4\xb8\x8e\xe5\x85\x81\xe5\xb7\xae"));
    auto *tg = new QGridLayout(targetBox);
    m_spinTargetH = new QDoubleSpinBox(targetBox);
    m_spinTargetM = new QDoubleSpinBox(targetBox);
    m_spinTargetL = new QDoubleSpinBox(targetBox);
    m_spinTolerance = new QDoubleSpinBox(targetBox);
    auto setupSpin = [](QDoubleSpinBox *sb) {
        sb->setRange(0.01, 200.0);
        sb->setDecimals(2);
        sb->setSuffix(QStringLiteral(" %"));
    };
    setupSpin(m_spinTargetH);
    setupSpin(m_spinTargetM);
    setupSpin(m_spinTargetL);
    m_spinTargetH->setValue(75.0);
    m_spinTargetM->setValue(50.0);
    m_spinTargetL->setValue(25.0);
    m_spinTolerance->setRange(0.1, 50.0);
    m_spinTolerance->setDecimals(2);
    m_spinTolerance->setSuffix(QStringLiteral(" %"));
    m_spinTolerance->setValue(5.0);
    tg->addWidget(new QLabel(qcUi("\xe9\xab\x98\xe5\x80\xbc\xe9\x9d\xb6\xe5\x80\xbc")), 0, 0);
    tg->addWidget(m_spinTargetH, 0, 1);
    tg->addWidget(new QLabel(qcUi("\xe4\xb8\xad\xe5\x80\xbc\xe9\x9d\xb6\xe5\x80\xbc")), 1, 0);
    tg->addWidget(m_spinTargetM, 1, 1);
    tg->addWidget(new QLabel(qcUi("\xe4\xbd\x8e\xe5\x80\xbc\xe9\x9d\xb6\xe5\x80\xbc")), 2, 0);
    tg->addWidget(m_spinTargetL, 2, 1);
    tg->addWidget(new QLabel(qcUi("\xe5\x85\x81\xe5\xb7\xae")), 3, 0);
    tg->addWidget(m_spinTolerance, 3, 1);
    leftLayout->addWidget(targetBox);

    auto *holeBox = new QGroupBox(qcUi("\xe8\xa1\x80\xe6\xa0\xb7\xe5\x8c\xba\xe5\xad\x94\xe4\xbd\x8d(\xe4\xbb\x8e0\xe8\xb5\xb7)"));
    auto *hg = new QGridLayout(holeBox);
    auto mkHole = [holeBox]() {
        auto *sb = new QSpinBox(holeBox);
        sb->setRange(0, 119);
        return sb;
    };
    m_spinHoleWater = mkHole();
    m_spinHoleStandard = mkHole();
    m_spinHoleHigh = mkHole();
    m_spinHoleMid = mkHole();
    m_spinHoleLow = mkHole();
    m_spinHoleWater->setValue(0);
    m_spinHoleStandard->setValue(1);
    m_spinHoleHigh->setValue(2);
    m_spinHoleMid->setValue(3);
    m_spinHoleLow->setValue(4);
    hg->addWidget(new QLabel(qcUi("\xe6\xb0\xb4")), 0, 0);
    hg->addWidget(m_spinHoleWater, 0, 1);
    hg->addWidget(new QLabel(qcUi("\xe6\xa0\x87\xe5\x87\x86\xe7\x89\xa9")), 0, 2);
    hg->addWidget(m_spinHoleStandard, 0, 3);
    hg->addWidget(new QLabel(qcUi("\xe9\xab\x98")), 1, 0);
    hg->addWidget(m_spinHoleHigh, 1, 1);
    hg->addWidget(new QLabel(qcUi("\xe4\xb8\xad")), 1, 2);
    hg->addWidget(m_spinHoleMid, 1, 3);
    hg->addWidget(new QLabel(qcUi("\xe4\xbd\x8e")), 2, 0);
    hg->addWidget(m_spinHoleLow, 2, 1);
    leftLayout->addWidget(holeBox);

    auto *prepHint = new QLabel(
        qcUi("\xe6\xaf\x8f\xe9\x80\x9a\xe9\x81\x93\xe8\xaf\x95\xe6\x9d\xaf\xe6\x9e\xb6 5 \xe7\xa9\xba\xe6\x9d\xaf\xef\xbc\x9b"
             "\xe8\x87\xaa\xe5\x8a\xa8\xe4\xbb\x8e\xe8\xa1\x80\xe6\xa0\xb7\xe5\x8c\xba\xe9\x85\x8d\xe6\xb6\xb2\xef\xbc\x8c"
             "\xe9\x80\x9a\xe9\x81\x93\xe6\xb5\x8b\xe8\x92\xb8\xe9\xa6\x96\xe6\xb0\xb4\xe4\xb8\x8e\xe6\xa0\x87\xe5\x87\x86\xe7\x89\xa9 AD\xef\xbc\x8c"
             "\xe5\x86\x8d\xe6\xb5\x8b\xe9\xab\x98/\xe4\xb8\xad/\xe4\xbd\x8e\xe8\xb4\xa8\xe6\x8e\xa7\xe6\xb6\xb2\xe3\x80\x82"));
    prepHint->setWordWrap(true);
    prepHint->setStyleSheet(QStringLiteral("color:#546E7A; font-size:11px;"));
    leftLayout->addWidget(prepHint);

    auto *chBox = new QGroupBox(qcUi("\xe6\xb5\x8b\xe8\xaf\x95\xe9\x80\x9a\xe9\x81\x93"));
    auto *chLayout = new QHBoxLayout(chBox);
    const int nCh = channelCountForEquipment(m_equipmentKind);
    createChannelButtons(nCh);
    chLayout->addStretch();
    for (QPushButton *btn : m_channelButtons) {
        chLayout->addWidget(btn);
    }
    chLayout->addStretch();
    chBox->setTitle(qcUi("\xe9\x80\x9a\xe9\x81\x93 1-%1").arg(nCh));
    leftLayout->addWidget(chBox);
    leftLayout->addStretch();
    leftScroll->setWidget(left);

    auto *right = new QWidget(splitter);
    auto *rightLayout = new QVBoxLayout(right);

    auto *chartBox = new QGroupBox(qcUi("\xe6\x97\xa5\xe8\xb4\xa8\xe6\x8e\xa7\xe8\xb6\x8b\xe5\x8a\xbf(\xe6\xaf\x8f\xe6\x97\xa5\xe4\xb8\x80\xe7\x82\xb9)"));
    auto *chartLayout = new QVBoxLayout(chartBox);
    auto *chartHint = new QLabel(
        qcUi("\xe5\xae\x9e\xe7\xba\xbf\xef\xbc\x9a\xe6\xb5\x8b\xe5\xbe\x97  \xe2\x94\x82  \xe8\x99\x9a\xe7\xba\xbf\xef\xbc\x9a\xe9\x9d\xb6\xe5\x80\xbc  \xe2\x94\x82  "
             "\xe6\xaf\x8f\xe5\xa4\xa9\xe4\xbb\x85\xe4\xbf\x9d\xe7\x95\x99\xe4\xb8\x80\xe6\xac\xa1\xe6\x9c\x80\xe6\x96\xb0\xe7\xbb\x93\xe6\x9e\x9c"));
    chartHint->setStyleSheet(QStringLiteral("color:#78909C; font-size:11px;"));
    chartLayout->addWidget(chartHint);
    m_trendPlot = new QCustomPlot(chartBox);
    m_trendPlot->setMinimumHeight(300);
    setupTrendPlot();
    chartLayout->addWidget(m_trendPlot, 1);
    m_lblLastCal = new QLabel(qcUi("\xe6\x9c\xac\xe6\xac\xa1\xe6\xa0\x87\xe5\xae\x9a\xef\xbc\x9a\xe2\x80\x94"));
    m_lblLastCal->setWordWrap(true);
    chartLayout->addWidget(m_lblLastCal);
    rightLayout->addWidget(chartBox, 3);

    auto *logBox = new QGroupBox(qcUi("\xe6\xb5\x8b\xe5\xae\x9a\xe8\xae\xb0\xe5\xbd\x95"));
    auto *logLayout = new QVBoxLayout(logBox);
    m_tableResult = new QTableWidget(logBox);
    initResultTable();
    logLayout->addWidget(m_tableResult);
    rightLayout->addWidget(logBox, 2);

    splitter->addWidget(leftScroll);
    splitter->addWidget(right);
    splitter->setStretchFactor(1, 1);
    root->addWidget(splitter, 1);

    m_tableTest = new QTableWidget(this);
    m_tableTest->hide();
    initTestTable();

    connect(m_btnStart, &QPushButton::clicked, this, &CustomQcTestWidget::onStartClicked);
    connect(btnPrev, &QPushButton::clicked, this, &CustomQcTestWidget::onPrevMonth);
    connect(btnNext, &QPushButton::clicked, this, &CustomQcTestWidget::onNextMonth);
    connect(btnToday, &QPushButton::clicked, this, &CustomQcTestWidget::onTodayMonth);
    connect(m_spinTargetH, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CustomQcTestWidget::onTargetChanged);
    connect(m_spinTargetM, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CustomQcTestWidget::onTargetChanged);
    connect(m_spinTargetL, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CustomQcTestWidget::onTargetChanged);
    connect(m_spinTolerance, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &CustomQcTestWidget::onTargetChanged);
}

void CustomQcTestWidget::setupTrendPlot()
{
    if (!m_trendPlot) {
        return;
    }
    m_trendPlot->setBackground(QBrush(Qt::white));
    m_trendPlot->yAxis->setLabel(qcUi("\xe8\x81\x9a\xe9\x9b\x86\xe7\x8e\x87 (%)"));
    m_trendPlot->yAxis->setRange(0, 100);
    m_trendPlot->yAxis->grid()->setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    m_trendPlot->xAxis->grid()->setPen(QPen(QColor(235, 235, 235), 1, Qt::DotLine));
    m_trendPlot->legend->setVisible(true);
    m_trendPlot->legend->setFont(QFont(QStringLiteral("Microsoft YaHei"), 9));
    m_trendPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
}

std::array<double, 3> CustomQcTestWidget::currentTargets() const
{
    return {m_spinTargetH->value(), m_spinTargetM->value(), m_spinTargetL->value()};
}

void CustomQcTestWidget::drawTargetReferenceLines()
{
    if (!m_trendPlot) {
        return;
    }
    const auto targets = currentTargets();
    const double tol = m_spinTolerance->value();
    const int days = m_chartMonth.daysInMonth();
    const double x0 = 1.0;
    const double x1 = static_cast<double>(days);

    const QColor bandColor(200, 230, 201, 80);
    const struct LevelStyle { double y; QColor line; } levels[] = {
        {targets[0], QColor(230, 81, 0)},
        {targets[1], QColor(25, 118, 210)},
        {targets[2], QColor(56, 142, 60)}
    };
    for (const LevelStyle &lv : levels) {
        auto *band = new QCPItemRect(m_trendPlot);
        band->topLeft->setCoords(x0, lv.y + tol);
        band->bottomRight->setCoords(x1, qMax(0.0, lv.y - tol));
        band->setPen(Qt::NoPen);
        band->setBrush(QBrush(bandColor));

        auto *line = new QCPItemLine(m_trendPlot);
        line->start->setCoords(x0, lv.y);
        line->end->setCoords(x1, lv.y);
        line->setPen(QPen(lv.line, 1, Qt::DashLine));
    }
}

void CustomQcTestWidget::refreshTrendChart()
{
    if (!m_trendPlot) {
        return;
    }

    QVector<double> dayx, yH, yM, yL;
    const QString monthKey = m_chartMonth.toString(QStringLiteral("yyyyMM"));
    if (FullyAutomatedPlatelets::pinstancesqlData()) {
        FullyAutomatedPlatelets::pinstancesqlData()->selectCustomQcDayResultThisMonth(
            monthKey, dayx, yH, yM, yL);
    }

    const int days = m_chartMonth.daysInMonth();
    QSharedPointer<QCPAxisTickerFixed> ticker(new QCPAxisTickerFixed);
    ticker->setTickStep(1.0);
    ticker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    m_trendPlot->xAxis->setTicker(ticker);
    m_trendPlot->xAxis->setRange(0.5, days + 0.5);
    m_trendPlot->xAxis->setLabel(qcUi("\xe6\x97\xa5(\xe5\x8f\xb7)"));
    m_lblMonth->setText(QStringLiteral("%1-%2")
                         .arg(m_chartMonth.year())
                         .arg(m_chartMonth.month(), 2, 10, QLatin1Char('0')));

    m_trendPlot->clearGraphs();
    m_trendPlot->clearItems();

    auto addSeries = [&](const QVector<double> &x, const QVector<double> &y,
                         const QColor &color, const QString &name) {
        m_trendPlot->addGraph();
        const int idx = m_trendPlot->graphCount() - 1;
        m_trendPlot->graph(idx)->setData(x, y);
        m_trendPlot->graph(idx)->setName(name);
        QPen pen(color, 2);
        m_trendPlot->graph(idx)->setPen(pen);
        m_trendPlot->graph(idx)->setLineStyle(QCPGraph::lsLine);
        m_trendPlot->graph(idx)->setScatterStyle(
            QCPScatterStyle(QCPScatterStyle::ssCircle, color, Qt::white, 7));
    };

    addSeries(dayx, yH, QColor(230, 81, 0),
              qcUi("\xe9\xab\x98\xe5\x80\xbc\xe6\xb5\x8b\xe5\xbe\x97"));
    addSeries(dayx, yM, QColor(25, 118, 210),
              qcUi("\xe4\xb8\xad\xe5\x80\xbc\xe6\xb5\x8b\xe5\xbe\x97"));
    addSeries(dayx, yL, QColor(56, 142, 60),
              qcUi("\xe4\xbd\x8e\xe5\x80\xbc\xe6\xb5\x8b\xe5\xbe\x97"));

    drawTargetReferenceLines();
    m_trendPlot->replot(QCustomPlot::rpQueuedReplot);
}

void CustomQcTestWidget::onTargetChanged()
{
    refreshTrendChart();
}

void CustomQcTestWidget::onPrevMonth()
{
    m_chartMonth = m_chartMonth.addMonths(-1);
    refreshTrendChart();
}

void CustomQcTestWidget::onNextMonth()
{
    m_chartMonth = m_chartMonth.addMonths(1);
    refreshTrendChart();
}

void CustomQcTestWidget::onTodayMonth()
{
    m_chartMonth = QDate::currentDate();
    refreshTrendChart();
}

void CustomQcTestWidget::createChannelButtons(int channelCount)
{
    const QString buttonStyle = QStringLiteral(
        "QPushButton { background:#1976D2; border-radius:24px; min-width:44px; min-height:44px;"
        " font:bold 12px; color:white; }"
        "QPushButton:checked { background:#D84315; }");

    auto *group = new QButtonGroup(this);
    group->setExclusive(false);
    for (int i = 0; i < channelCount; ++i) {
        auto *btn = new QPushButton(QString::number(i + 1), this);
        btn->setCheckable(true);
        btn->setStyleSheet(buttonStyle);
        m_channelButtons.append(btn);
        group->addButton(btn, i);
        connect(btn, &QPushButton::clicked, this, [this, btn](bool checked) {
            syncTestRow(btn->text(), checked);
        });
    }
}

void CustomQcTestWidget::initTestTable()
{
    m_tableTest->setColumnCount(ColCount);
}

void CustomQcTestWidget::initResultTable()
{
    m_tableResult->setColumnCount(9);
    m_tableResult->setHorizontalHeaderLabels(QStringList()
        << qcUi("\xe6\x97\xb6\xe9\x97\xb4") << qcUi("\xe8\xae\xb0\xe5\xbd\x95ID") << qcUi("\xe9\x80\x9a\xe9\x81\x93")
        << qcUi("\xe6\xb0\xb4AD") << qcUi("\xe6\xa0\x87\xe5\x87\x86AD")
        << qcUi("\xe9\xab\x98(%)") << qcUi("\xe4\xb8\xad(%)") << qcUi("\xe4\xbd\x8e(%)")
        << qcUi("\xe5\x88\xa4\xe5\xae\x9a"));
    m_tableResult->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableResult->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void CustomQcTestWidget::syncTestRow(const QString &channelText, bool add)
{
    if (add) {
        for (int row = 0; row < m_tableTest->rowCount(); ++row) {
            if (m_tableTest->item(row, ColChannel)
                && m_tableTest->item(row, ColChannel)->text() == channelText) {
                return;
            }
        }
        const int row = m_tableTest->rowCount();
        m_tableTest->insertRow(row);
        m_tableTest->setItem(row, ColId, centeredItem(timestampKey(channelText)));
        m_tableTest->setItem(row, ColChannel, centeredItem(channelText));
    } else {
        for (int row = m_tableTest->rowCount() - 1; row >= 0; --row) {
            if (m_tableTest->item(row, ColChannel)
                && m_tableTest->item(row, ColChannel)->text() == channelText) {
                m_tableTest->removeRow(row);
            }
        }
    }
}

QMap<QString, quint8> CustomQcTestWidget::collectTestChannels() const
{
    QMap<QString, quint8> map;
    for (int row = 0; row < m_tableTest->rowCount(); ++row) {
        const auto *idItem = m_tableTest->item(row, ColId);
        const auto *chItem = m_tableTest->item(row, ColChannel);
        if (!idItem || !chItem) {
            continue;
        }
        bool ok = false;
        const quint8 ch = static_cast<quint8>(chItem->text().toUShort(&ok));
        if (!ok || ch < 1) {
            continue;
        }
        map.insert(idItem->text().trimmed(), ch);
    }
    return map;
}

void CustomQcTestWidget::onStartClicked()
{
    saveSettings();

    if (!cglobal::gserialConnecStatus.load()) {
        CustomMessageBox::warning(this, qcUi("\xe5\x90\xaf\xe5\x8a\xa8\xe5\xa4\xb1\xe8\xb4\xa5"), qcUi("\xe4\xbb\xaa\xe5\x99\xa8\xe7\xa6\xbb\xe7\xba\xbf"));
        return;
    }
    if (cglobal::g_StartTesting.load()) {
        CustomMessageBox::warning(this, qcUi("\xe5\x90\xaf\xe5\x8a\xa8\xe5\xa4\xb1\xe8\xb4\xa5"), qcUi("\xe4\xbb\xaa\xe5\x99\xa8\xe6\xad\xa3\xe5\x9c\xa8\xe6\x99\xae\xe9\x80\x9a\xe6\xb5\x8b\xe8\xaf\x95"));
        return;
    }
    if (Performanceverification::instance()->returnPerformanceTestFlag()) {
        CustomMessageBox::warning(this, qcUi("\xe5\x90\xaf\xe5\x8a\xa8\xe5\xa4\xb1\xe8\xb4\xa5"), qcUi("\xe8\xb4\xa8\xe6\x8e\xa7\xe6\xb5\x8b\xe8\xaf\x95\xe8\xbf\x9b\xe8\xa1\x8c\xe4\xb8\xad"));
        return;
    }

    const QMap<QString, quint8> channels = collectTestChannels();
    if (channels.isEmpty()) {
        CustomMessageBox::warning(this, qcUi("\xe5\x90\xaf\xe5\x8a\xa8\xe5\xa4\xb1\xe8\xb4\xa5"), qcUi("\xe8\xaf\xb7\xe9\x80\x89\xe6\x8b\xa9\xe9\x80\x9a\xe9\x81\x93"));
        return;
    }

    const int needTubes = channels.size() * 5;
    const int freeTubes = FullyAutomatedPlatelets::pinstancesqlData()
            ? FullyAutomatedPlatelets::pinstancesqlData()->BackEmptyTubeNum()
            : 0;
    if (freeTubes < needTubes) {
        CustomMessageBox::warning(this, qcUi("\xe5\x90\xaf\xe5\x8a\xa8\xe5\xa4\xb1\xe8\xb4\xa5"),
                                  qcUi("\xe7\xa9\xba\xe8\xaf\x95\xe7\xae\xa1\xe4\xb8\x8d\xe8\xb6\xb3\xef\xbc\x9a\xe9\x9c\x80 %1 \xe4\xb8\xaa\xef\xbc\x8c\xe7\xa9\xba\xe9\x97\xb2 %2 \xe4\xb8\xaa")
                                  .arg(needTubes).arg(freeTubes));
        return;
    }

    Performanceverification::CustomQcCalibration cal;
    cal.targetPercent = currentTargets();
    cal.tolerancePercent = m_spinTolerance->value();

    Performanceverification::PeBloodSampleHoles holes;
    holes.water = static_cast<quint8>(m_spinHoleWater->value());
    holes.standard = static_cast<quint8>(m_spinHoleStandard->value());
    holes.high = static_cast<quint8>(m_spinHoleHigh->value());
    holes.mid = static_cast<quint8>(m_spinHoleMid->value());
    holes.low = static_cast<quint8>(m_spinHoleLow->value());
    holes.useFiveZonePrep = true;
    Performanceverification::saveBloodSampleHolesToIni(holes);

    Performanceverification::instance()->setCustomQcCalibration(cal);
    Performanceverification::instance()->setBloodSampleHoles(holes);

    m_lblRunStatus->setText(qcUi("\xe6\xb5\x8b\xe8\xaf\x95\xe4\xb8\xad\xe2\x80\xa6"));
    m_lblRunStatus->setStyleSheet(
        QStringLiteral("padding:6px 12px; background:#FFF9C4; color:#F57F17; font-weight:bold;"));
    setUiBusy(true);
    Performanceverification::instance()->getCustomQcChannelList(channels);
}

void CustomQcTestWidget::setUiBusy(bool busy)
{
    m_btnStart->setEnabled(!busy);
    for (QPushButton *btn : m_channelButtons) {
        btn->setEnabled(!busy);
    }
    for (QSpinBox *sb : {m_spinHoleWater, m_spinHoleStandard, m_spinHoleHigh, m_spinHoleMid, m_spinHoleLow}) {
        sb->setEnabled(!busy);
    }
    for (QDoubleSpinBox *sb : {m_spinTargetH, m_spinTargetM, m_spinTargetL, m_spinTolerance}) {
        sb->setEnabled(!busy);
    }
}

void CustomQcTestWidget::resetTestSelection()
{
    for (QPushButton *btn : m_channelButtons) {
        btn->blockSignals(true);
        btn->setChecked(false);
        btn->blockSignals(false);
    }
    m_tableTest->setRowCount(0);
}

void CustomQcTestWidget::onTestSessionFinished()
{
    setUiBusy(false);
    resetTestSelection();
    m_lblRunStatus->setText(qcUi("\xe5\xb0\xb1\xe7\xbb\xaa"));
    m_lblRunStatus->setStyleSheet(
        QStringLiteral("padding:6px 12px; background:#ECEFF1; border-radius:4px; font-weight:bold;"));
}

void CustomQcTestWidget::applyResult(const QString &id, quint8 channel,
                                     const std::array<double, 3> &measured,
                                     const std::array<double, 3> &target,
                                     const std::array<double, 3> &bias,
                                     bool pass,
                                     int waterChannelAd,
                                     int standardChannelAd)
{
    Q_UNUSED(target);
    Q_UNUSED(bias);

    const int row = m_tableResult->rowCount();
    m_tableResult->insertRow(row);
    auto fmt = [](double v) { return QString::number(v, 'f', 2); };

    m_tableResult->setItem(row, ResTime,
                           centeredItem(QDateTime::currentDateTime().toString(QStringLiteral("HH:mm:ss"))));
    m_tableResult->setItem(row, ResId, centeredItem(id));
    m_tableResult->setItem(row, ResChannel, centeredItem(QString::number(channel)));
    m_tableResult->setItem(row, ResWaterAd, centeredItem(QString::number(waterChannelAd)));
    m_tableResult->setItem(row, ResStdAd, centeredItem(QString::number(standardChannelAd)));
    m_tableResult->setItem(row, ResMeasH, centeredItem(fmt(measured[0])));
    m_tableResult->setItem(row, ResMeasM, centeredItem(fmt(measured[1])));
    m_tableResult->setItem(row, ResMeasL, centeredItem(fmt(measured[2])));

    auto *verdict = centeredItem(pass ? qcUi("\xe5\x90\x88\xe6\xa0\xbc") : qcUi("\xe8\xb6\x85\xe5\xb7\xae"));
    verdict->setForeground(pass ? QColor(27, 94, 32) : QColor(198, 40, 40));
    m_tableResult->setItem(row, ResVerdict, verdict);

    m_lblLastCal->setText(
        qcUi("\xe6\x9c\xac\xe6\xac\xa1\xe6\xa0\x87\xe5\xae\x9a\xef\xbc\x9a\xe6\xb0\xb4 AD=%1\xef\xbc\x8c\xe6\xa0\x87\xe5\x87\x86\xe7\x89\xa9 AD=%2  \xe2\x94\x82  "
             "\xe9\xab\x98/%3% \xe4\xb8\xad/%4% \xe4\xbd\x8e/%5%")
        .arg(waterChannelAd)
        .arg(standardChannelAd)
        .arg(fmt(measured[0]))
        .arg(fmt(measured[1]))
        .arg(fmt(measured[2])));

    const QString dayKey = QDate::currentDate().toString(QStringLiteral("yyyyMMdd"));
    if (FullyAutomatedPlatelets::pinstancesqlData()) {
        FullyAutomatedPlatelets::pinstancesqlData()->insertCustomQcDayResult(
            dayKey, fmt(measured[0]), fmt(measured[1]), fmt(measured[2]));
    }
    if (m_chartMonth.year() == QDate::currentDate().year()
            && m_chartMonth.month() == QDate::currentDate().month()) {
        refreshTrendChart();
    }

    m_lblRunStatus->setText(pass ? qcUi("\xe6\x9c\x80\xe8\xbf\x91\xe5\x88\xa4\xe5\xae\x9a\xef\xbc\x9a\xe5\x90\x88\xe6\xa0\xbc")
                                 : qcUi("\xe6\x9c\x80\xe8\xbf\x91\xe5\x88\xa4\xe5\xae\x9a\xef\xbc\x9a\xe8\xb6\x85\xe5\xb7\xae"));
    m_lblRunStatus->setStyleSheet(pass
        ? QStringLiteral("padding:6px 12px; background:#C8E6C9; color:#1B5E20; font-weight:bold;")
        : QStringLiteral("padding:6px 12px; background:#FFCDD2; color:#B71C1C; font-weight:bold;"));
}

void CustomQcTestWidget::loadSettings()
{
    QSettings ini(QCoreApplication::applicationDirPath() + QStringLiteral("/CustomQcTest.ini"),
                  QSettings::IniFormat);
    m_spinTargetH->setValue(ini.value(QStringLiteral("Target/high"), 75.0).toDouble());
    m_spinTargetM->setValue(ini.value(QStringLiteral("Target/mid"), 50.0).toDouble());
    m_spinTargetL->setValue(ini.value(QStringLiteral("Target/low"), 25.0).toDouble());
    m_spinTolerance->setValue(ini.value(QStringLiteral("Target/tolerance"), 5.0).toDouble());
    m_spinHoleWater->setValue(ini.value(QStringLiteral("BloodZone/water"), 0).toInt());
    m_spinHoleStandard->setValue(ini.value(QStringLiteral("BloodZone/standard"), 1).toInt());
    m_spinHoleHigh->setValue(ini.value(QStringLiteral("BloodZone/high"), 2).toInt());
    m_spinHoleMid->setValue(ini.value(QStringLiteral("BloodZone/mid"), 3).toInt());
    m_spinHoleLow->setValue(ini.value(QStringLiteral("BloodZone/low"), 4).toInt());
}

void CustomQcTestWidget::saveSettings() const
{
    QSettings ini(QCoreApplication::applicationDirPath() + QStringLiteral("/CustomQcTest.ini"),
                  QSettings::IniFormat);
    ini.setValue(QStringLiteral("Target/high"), m_spinTargetH->value());
    ini.setValue(QStringLiteral("Target/mid"), m_spinTargetM->value());
    ini.setValue(QStringLiteral("Target/low"), m_spinTargetL->value());
    ini.setValue(QStringLiteral("Target/tolerance"), m_spinTolerance->value());
    ini.setValue(QStringLiteral("BloodZone/water"), m_spinHoleWater->value());
    ini.setValue(QStringLiteral("BloodZone/standard"), m_spinHoleStandard->value());
    ini.setValue(QStringLiteral("BloodZone/high"), m_spinHoleHigh->value());
    ini.setValue(QStringLiteral("BloodZone/mid"), m_spinHoleMid->value());
    ini.setValue(QStringLiteral("BloodZone/low"), m_spinHoleLow->value());
    ini.setValue(QStringLiteral("BloodZone/useFiveZonePrep"), true);
}
