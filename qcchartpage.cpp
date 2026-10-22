#include "qcchartpage.h"

#include "cglobal.h"
#include "customcreatsql.h"
#include "loadequipmentpos.h"
#include "qcstartaction.h"
#include "qcustomplot.h"
#include <operclass/fullyautomatedplatelets.h>

#include <QButtonGroup>
#include <QComboBox>
#include <QDate>
#include <QDialog>
#include <QFont>
#include <QTextEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QListView>
#include <QMouseEvent>
#include <QPushButton>
#include <QScrollBar>
#include <QVBoxLayout>

namespace {

const QColor kPageBg(217, 230, 240);
const QColor kPlotBg(231, 240, 246);
const QColor kGrid(170, 184, 196);
const QColor kAxis(90, 104, 118);

QString levelButtonStyle()
{
    return QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93';"
        "  font-size: 20px;"
        "  min-width: 72px;"
        "  min-height: 42px;"
        "  padding: 0 10px;"
        "  border: 1px solid #b0b8c0;"
        "  border-radius: 8px;"
        "  background: transparent;"
        "  color: #333333;"
        "}"
        "QPushButton:checked {"
        "  border: 2px solid #4a7ab5;"
        "  color: #1d4e89;"
        "  background: rgba(74,122,181,28);"
        "}"
        "QPushButton:hover {"
        "  border: 1px solid #4a7ab5;"
        "}");
}

QString testChannelStyle()
{
    return QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93';"
        "  font-size: 18px;"
        "  min-width: 52px;"
        "  min-height: 42px;"
        "  padding: 0;"
        "  border: 1px solid #b0b8c0;"
        "  border-radius: 8px;"
        "  background: transparent;"
        "  color: #333333;"
        "}"
        "QPushButton:checked {"
        "  border: 2px solid #4a7ab5;"
        "  color: white;"
        "  background: #4a7ab5;"
        "}"
        "QPushButton:hover { border: 1px solid #4a7ab5; }");
}

int equipmentChannelCount()
{
    quint8 kind = KS1200;
    SingletonAxis::equipmentKind(READ_OPERRAT, kind);
    if (kind == KS600)
        return 4;
    if (kind == KS800)
        return 8;
    return MACHINE_SETTING_CHANNEL;
}

QString popupScrollStyle()
{
    return QString::fromUtf8(
        "QScrollBar:vertical { width: 8px; margin: 0; background: #eef3f7; border: none; }"
        "QScrollBar::handle:vertical { background: #8aa4be; min-height: 18px; border-radius: 3px; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px; width: 0px; border: none; background: transparent; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }");
}

class PercentAxisTicker : public QCPAxisTickerFixed
{
protected:
    virtual QString getTickLabel(double tick, const QLocale &locale, QChar formatChar, int precision) Q_DECL_OVERRIDE
    {
        Q_UNUSED(locale);
        Q_UNUSED(formatChar);
        Q_UNUSED(precision);
        return QString::number(qRound(tick)) + QLatin1String("%");
    }
};

QString channelComboStyle()
{
    return QString::fromUtf8(
        "QComboBox {"
        "  border-radius: 6px;"
        "  padding: 4px 8px 4px 12px;"
        "  font: 18px '\xe6\xa5\xb7\xe4\xbd\x93';"
        "  border: 1px solid #8aa4be;"
        "  background-color: #ffffff;"
        "  color: #1a4d6b;"
        "  min-height: 36px;"
        "  min-width: 148px;"
        "}"
        "QComboBox:hover, QComboBox:focus { border: 1px solid #4a7ab5; }"
        "QComboBox::drop-down {"
        "  subcontrol-origin: padding;"
        "  subcontrol-position: top right;"
        "  width: 16px;"
        "  border: none;"
        "  border-left: 1px solid #d5e2ee;"
        "  background: #f4f8fb;"
        "  border-top-right-radius: 6px;"
        "  border-bottom-right-radius: 6px;"
        "}"
        "QComboBox::down-arrow {"
        "  image: url(:/Picture/cssdown.png);"
        "  width: 8px;"
        "  height: 8px;"
        "}"
        "QComboBox QScrollBar:vertical {"
        "  width: 8px;"
        "  margin: 0px;"
        "  background: #eef3f7;"
        "  border: none;"
        "}"
        "QComboBox QScrollBar::handle:vertical {"
        "  background: #8aa4be;"
        "  min-height: 18px;"
        "  border-radius: 3px;"
        "}"
        "QComboBox QScrollBar::add-line:vertical,"
        "QComboBox QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "  width: 0px;"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QComboBox QScrollBar::add-page:vertical,"
        "QComboBox QScrollBar::sub-page:vertical { background: transparent; }"
        "QComboBox QAbstractItemView {"
        "  background: #ffffff;"
        "  border: 1px solid #8aa4be;"
        "  border-radius: 4px;"
        "  font: 18px '\xe6\xa5\xb7\xe4\xbd\x93';"
        "  outline: 0;"
        "  padding: 4px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "  height: 34px;"
        "  color: #1a4d6b;"
        "  padding-left: 10px;"
        "  border: none;"
        "  border-radius: 0px;"
        "  background-color: #ffffff;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "  background-color: #d7ebf6;"
        "  color: #1a4d6b;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "  background-color: #4a7ab5;"
        "  color: #ffffff;"
        "}");
}

} // namespace

void fitCurveYAxis(QCustomPlot *plot, double center);

QcChartPage::QcChartPage(QWidget *parent)
    : QWidget(parent)
    , m_highPlot(nullptr)
    , m_midPlot(nullptr)
    , m_lowPlot(nullptr)
    , m_channelBox(nullptr)
    , m_levelGroup(nullptr)
    , m_statusLabel(nullptr)
    , m_lotButton(nullptr)
    , m_startButton(nullptr)
    , m_dragCursor(false)
    , m_panX(false)
    , m_pointerMoved(false)
    , m_pressX(0)
    , m_pressLower(0)
    , m_pressUpper(0)
{
    for (int i = 0; i < 3; ++i) {
        m_curve[i].plot = nullptr;
        m_curve[i].title = nullptr;
        m_curve[i].cursor = nullptr;
        m_curve[i].valueText = nullptr;
        m_curve[i].centerPercent = 0;
        m_curve[i].levelUtf8 = "";
    }
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, kPageBg);
    setPalette(pal);

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(8, 8, 8, 8);
    root->setSpacing(8);

    QLabel *hint = new QLabel(QString::fromUtf8(
        "\xe6\xaf\x8f\xe4\xb8\xaa\xe9\x80\x9a\xe9\x81\x93\xe6\xaf\x8f\xe5\xa4\xa9"
        "\xe5\x88\x86\xe5\x88\xab\xe8\xae\xb0\xe5\xbd\x95"
        "\xe9\xab\x98\xe5\x80\xbc\xe3\x80\x81\xe4\xb8\xad\xe5\x80\xbc\xe3\x80\x81"
        "\xe4\xbd\x8e\xe5\x80\xbc\xe4\xb8\x89\xe4\xb8\xaa\xe6\x8c\x87\xe6\x8e\xa7\xe7\x82\xb9"), this);
    hint->setStyleSheet(QString::fromUtf8(
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px; color: #3d4c5c;"
        " background: transparent; border: none; }"));
    buildCharts();
    buildBottomBar();
    QWidget *bar = m_channelBox->parentWidget();
    root->addWidget(bar);
    root->addWidget(hint);
    root->addWidget(m_highPlot, 1);
    root->addWidget(m_midPlot, 1);
    root->addWidget(m_lowPlot, 1);
    reloadChannelCurves();
    connect(QcStartAction::instance(), &QcStartAction::progressText,
            this, &QcChartPage::onQcProgress);
    connect(QcStartAction::instance(), &QcStartAction::finished,
            this, &QcChartPage::onQcFinished);
}

int QcChartPage::channelNumber() const
{
    return m_channelBox ? m_channelBox->currentData().toInt() : 1;
}

QcChartPage::Level QcChartPage::level() const
{
    if (!m_levelGroup)
        return LevelAll;
    const int id = m_levelGroup->checkedId();
    if (id < LevelHigh || id > LevelAll)
        return LevelAll;
    return static_cast<Level>(id);
}

void QcChartPage::onLevelClicked(int id)
{
    applyLevel(static_cast<Level>(id));
}

void QcChartPage::onStartClicked()
{
    QString reason;
    if (!QcStartAction::instance()->start(static_cast<int>(level()), selectedTestChannels(),
                                          m_highLot, m_midLot, m_lowLot, &reason)) {
        if (m_statusLabel)
            m_statusLabel->setText(reason);
        return;
    }
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql)
        sql->saveQcLots(m_highLot, m_midLot, m_lowLot);
    if (m_startButton)
        m_startButton->setEnabled(false);
}

void QcChartPage::onLotButtonClicked()
{
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("\xe8\xb4\xa8\xe6\x8e\xa7\xe6\x89\xb9\xe5\x8f\xb7"));
    dialog.setModal(true);
    dialog.setStyleSheet(QString::fromUtf8(
        "QDialog { background: #e7f0f6; }"
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px; color: #2c3a48; }"));

    QGridLayout *grid = new QGridLayout(&dialog);
    grid->setContentsMargins(22, 18, 22, 18);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(14);

    const char *labels[3] = {
        "\xe9\xab\x98\xe5\x80\xbc\xe6\x89\xb9\xe5\x8f\xb7",
        "\xe4\xb8\xad\xe5\x80\xbc\xe6\x89\xb9\xe5\x8f\xb7",
        "\xe4\xbd\x8e\xe5\x80\xbc\xe6\x89\xb9\xe5\x8f\xb7"
    };
    const QString current[3] = { m_highLot, m_midLot, m_lowLot };
    QComboBox *boxes[3] = { nullptr, nullptr, nullptr };
    for (int i = 0; i < 3; ++i) {
        grid->addWidget(new QLabel(QString::fromUtf8(labels[i]), &dialog), i, 0);
        QComboBox *box = new QComboBox(&dialog);
        box->setEditable(true);
        box->setInsertPolicy(QComboBox::NoInsert);
        box->setStyleSheet(channelComboStyle());
        box->setMinimumWidth(220);
        QListView *view = new QListView(box);
        view->setFrameShape(QFrame::NoFrame);
        view->setSpacing(0);
        QScrollBar *popupBar = view->verticalScrollBar();
        if (popupBar)
            popupBar->setStyleSheet(popupScrollStyle());
        box->setView(view);
        box->setMaxVisibleItems(8);
        if (box->lineEdit())
            box->lineEdit()->setMaxLength(32);
        if (sql) {
            const QStringList choices = sql->loadQcLotChoices(i);
            for (int n = 0; n < choices.size(); ++n) {
                if (box->findText(choices.at(n)) < 0)
                    box->addItem(choices.at(n));
            }
        }
        if (!current[i].isEmpty() && box->findText(current[i]) < 0)
            box->addItem(current[i]);
        box->setCurrentText(current[i]);
        boxes[i] = box;
        grid->addWidget(box, i, 1);
    }

    QLabel *hint = new QLabel(&dialog);
    hint->setStyleSheet(QString::fromUtf8(
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 14px; color: #b23b3b; }"));
    grid->addWidget(hint, 3, 0, 1, 2);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch(1);
    QPushButton *saveBtn = new QPushButton(QString::fromUtf8("\xe4\xbf\x9d\xe5\xad\x98"), &dialog);
    QPushButton *cancelBtn = new QPushButton(QString::fromUtf8("\xe5\x8f\x96\xe6\xb6\x88"), &dialog);
    const QString btnStyle = QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px;"
        "  min-width: 88px; min-height: 36px;"
        "  border: 1px solid #4a7ab5; border-radius: 8px;"
        "  background: #4a7ab5; color: white;"
        "}"
        "QPushButton:hover { background: #3d6aa0; }");
    saveBtn->setStyleSheet(btnStyle);
    cancelBtn->setStyleSheet(QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px;"
        "  min-width: 88px; min-height: 36px;"
        "  border: 1px solid #b0b8c0; border-radius: 8px;"
        "  background: transparent; color: #2c3a48;"
        "}"));
    buttons->addWidget(saveBtn);
    buttons->addWidget(cancelBtn);
    grid->addLayout(buttons, 4, 0, 1, 2);

    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    connect(saveBtn, &QPushButton::clicked, &dialog, [&]() {
        const QString highLot = boxes[0]->currentText().trimmed();
        const QString midLot = boxes[1]->currentText().trimmed();
        const QString lowLot = boxes[2]->currentText().trimmed();
        if (!sql || !sql->saveQcLots(highLot, midLot, lowLot)) {
            hint->setText(QString::fromUtf8("\xe6\x89\xb9\xe5\x8f\xb7\xe4\xbf\x9d\xe5\xad\x98\xe5\xa4\xb1\xe8\xb4\xa5"));
            return;
        }
        dialog.accept();
    });
    if (dialog.exec() != QDialog::Accepted)
        return;
    m_highLot = boxes[0]->currentText().trimmed();
    m_midLot = boxes[1]->currentText().trimmed();
    m_lowLot = boxes[2]->currentText().trimmed();
    updateChartTitles();
    if (m_statusLabel)
        m_statusLabel->setText(QString::fromUtf8("\xe6\x89\xb9\xe5\x8f\xb7\xe5\xb7\xb2\xe4\xbf\x9d\xe5\xad\x98"));
}

void QcChartPage::onQcProgress(const QString &text)
{
    if (m_statusLabel)
        m_statusLabel->setText(text);
}

void QcChartPage::showResultDialog(const QString &text)
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromUtf8("\xe8\xb4\xa8\xe6\x8e\xa7\xe7\xbb\x93\xe6\x9e\x9c"));
    dialog.setModal(true);
    dialog.setStyleSheet(QString::fromUtf8("QDialog { background: #e7f0f6; }"));
    QVBoxLayout *lay = new QVBoxLayout(&dialog);
    lay->setContentsMargins(18, 16, 18, 16);
    lay->setSpacing(12);

    QTextEdit *edit = new QTextEdit(&dialog);
    edit->setReadOnly(true);
    edit->setPlainText(text);
    edit->setMinimumSize(480, 360);
    edit->setFont(QFont(QString::fromUtf8("\xe6\xa5\xb7\xe4\xbd\x93"), 16));
    edit->setStyleSheet(QString::fromUtf8(
        "QTextEdit { background: white; color: #1a4d6b; border: 1px solid #b0b8c0; border-radius: 8px; }"));
    lay->addWidget(edit);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch(1);
    QPushButton *ok = new QPushButton(QString::fromUtf8("\xe7\xa1\xae\xe5\xae\x9a"), &dialog);
    ok->setCursor(Qt::PointingHandCursor);
    ok->setStyleSheet(QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 18px;"
        "  min-width: 96px; min-height: 36px;"
        "  border: 1px solid #4a7ab5; border-radius: 8px;"
        "  background: #4a7ab5; color: white;"
        "}"
        "QPushButton:hover { background: #3d6aa0; }"));
    connect(ok, &QPushButton::clicked, &dialog, &QDialog::accept);
    buttons->addWidget(ok);
    lay->addLayout(buttons);
    dialog.exec();
}

void QcChartPage::onQcFinished(bool ok, const QString &text)
{
    if (m_statusLabel) {
        m_statusLabel->setText(ok
            ? QString::fromUtf8("\xe8\xb4\xa8\xe6\x8e\xa7\xe5\xae\x8c\xe6\x88\x90")
            : text);
    }
    if (ok)
        reloadChannelCurves();
    if (m_startButton)
        m_startButton->setEnabled(true);
    showResultDialog(text);
}

QList<int> QcChartPage::selectedTestChannels() const
{
    QList<int> channels;
    for (int i = 0; i < m_testChannelButtons.size(); ++i) {
        QPushButton *btn = m_testChannelButtons.at(i);
        if (btn && btn->isChecked())
            channels.append(btn->property("channel").toInt());
    }
    return channels;
}

void QcChartPage::reloadChannelCurves()
{
    if (!m_highPlot || !m_midPlot || !m_lowPlot)
        return;
    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    QVector<double> x;
    QVector<double> high;
    QVector<double> mid;
    QVector<double> low;
    if (sql) {
        const QList<QcChannelDayValue> rows = sql->queryQcChannelDaily(channelNumber());
        x.reserve(rows.size());
        high.reserve(rows.size());
        mid.reserve(rows.size());
        low.reserve(rows.size());
        for (int i = 0; i < rows.size(); ++i) {
            const QDate day = QDate::fromString(rows.at(i).qcDate, QStringLiteral("yyyy-MM-dd"));
            if (!day.isValid())
                continue;
            x.append(QCPAxisTickerDateTime::dateTimeToKey(QDateTime(day)));
            high.append(rows.at(i).highValue);
            mid.append(rows.at(i).midValue);
            low.append(rows.at(i).lowValue);
        }
    }
    if (m_highPlot->graphCount() > 0)
        m_highPlot->graph(0)->setData(x, high);
    if (m_midPlot->graphCount() > 0)
        m_midPlot->graph(0)->setData(x, mid);
    if (m_lowPlot->graphCount() > 0)
        m_lowPlot->graph(0)->setData(x, low);
    for (int i = 0; i < 3; ++i)
        fitCurveYAxis(m_curve[i].plot, m_curve[i].centerPercent);
    updateChartTitles();
    showRecentDays();
    if (!x.isEmpty())
        placeCursor(x.last());
    m_highPlot->replot();
    m_midPlot->replot();
    m_lowPlot->replot();
}

void QcChartPage::buildCharts()
{
    m_curve[0] = createChart(QColor(30, 90, 160), 75, "\xe9\xab\x98\xe5\x80\xbc");
    m_curve[1] = createChart(QColor(36, 130, 90), 50, "\xe4\xb8\xad\xe5\x80\xbc");
    m_curve[2] = createChart(QColor(176, 96, 40), 25, "\xe4\xbd\x8e\xe5\x80\xbc");
    m_highPlot = m_curve[0].plot;
    m_midPlot = m_curve[1].plot;
    m_lowPlot = m_curve[2].plot;
}

void QcChartPage::buildBottomBar()
{
    QWidget *bar = new QWidget(this);
    bar->setStyleSheet(QString::fromUtf8(
        "QWidget { background: transparent; border: none; }"
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 20px; color: #2c3a48; background: transparent; border: none; }"));

    QVBoxLayout *lay = new QVBoxLayout(bar);
    lay->setContentsMargins(8, 4, 8, 4);
    lay->setSpacing(10);

    QHBoxLayout *top = new QHBoxLayout();
    top->setSpacing(16);
    lay->addLayout(top);

    top->addWidget(new QLabel(QString::fromUtf8("\xe9\xa2\x84\xe8\xa7\x88\xe9\x80\x9a\xe9\x81\x93"), bar));
    m_channelBox = new QComboBox(bar);
    QListView *channelView = new QListView(m_channelBox);
    channelView->setFrameShape(QFrame::NoFrame);
    channelView->setSpacing(0);
    QScrollBar *popupBar = channelView->verticalScrollBar();
    if (popupBar)
        popupBar->setStyleSheet(popupScrollStyle());
    m_channelBox->setView(channelView);
    m_channelBox->setStyleSheet(channelComboStyle());
    m_channelBox->setMaxVisibleItems(8);
    const int channelCount = equipmentChannelCount();
    for (int ch = 1; ch <= channelCount; ++ch)
        m_channelBox->addItem(QString::fromUtf8("\xe9\x80\x9a\xe9\x81\x93%1").arg(ch), ch);
    connect(m_channelBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &QcChartPage::reloadChannelCurves);
    top->addWidget(m_channelBox);

    top->addSpacing(28);
    top->addWidget(new QLabel(QString::fromUtf8("\xe8\xb4\xa8\xe6\x8e\xa7\xe5\x80\xbc"), bar));

    m_levelGroup = new QButtonGroup(bar);
    m_levelGroup->setExclusive(true);
    struct LevelButton {
        const char *text;
        int id;
    };
    const LevelButton levels[] = {
        {"\xe9\xab\x98", LevelHigh},
        {"\xe4\xb8\xad", LevelMid},
        {"\xe4\xbd\x8e", LevelLow},
        {"\xe5\x85\xa8\xe5\x80\xbc", LevelAll}
    };
    for (int i = 0; i < 4; ++i) {
        QPushButton *btn = new QPushButton(QString::fromUtf8(levels[i].text), bar);
        btn->setCheckable(true);
        btn->setStyleSheet(levelButtonStyle());
        m_levelGroup->addButton(btn, levels[i].id);
        top->addWidget(btn);
        if (levels[i].id == LevelAll)
            btn->setChecked(true);
    }
    connect(m_levelGroup,
            static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &QcChartPage::onLevelClicked);

    top->addStretch(1);

    QHBoxLayout *channels = new QHBoxLayout();
    channels->setSpacing(12);
    lay->addLayout(channels);
    channels->addWidget(new QLabel(QString::fromUtf8("\xe6\xb5\x8b\xe8\xaf\x95\xe9\x80\x9a\xe9\x81\x93"), bar));
    for (int ch = 1; ch <= channelCount; ++ch) {
        QPushButton *btn = new QPushButton(QString::number(ch), bar);
        btn->setCheckable(true);
        btn->setChecked(false);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setProperty("channel", ch);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        btn->setStyleSheet(testChannelStyle());
        m_testChannelButtons.append(btn);
        channels->addWidget(btn);
    }

    m_statusLabel = new QLabel(bar);
    m_statusLabel->setMinimumWidth(220);
    m_statusLabel->setStyleSheet(QString::fromUtf8(
        "QLabel { font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 16px; color: #1a4d6b;"
        " background: transparent; border: none; }"));
    top->addWidget(m_statusLabel);

    m_lotButton = new QPushButton(QString::fromUtf8("\xe6\x89\xb9\xe5\x8f\xb7"), bar);
    m_lotButton->setCursor(Qt::PointingHandCursor);
    m_lotButton->setStyleSheet(QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 22px;"
        "  min-width: 110px; min-height: 46px;"
        "  border: 1px solid #4a7ab5; border-radius: 8px;"
        "  background: transparent; color: #1d4e89;"
        "}"
        "QPushButton:hover { background: rgba(74,122,181,28); }"
        "QPushButton:pressed { background: rgba(74,122,181,48); }"));
    connect(m_lotButton, &QPushButton::clicked, this, &QcChartPage::onLotButtonClicked);
    top->addWidget(m_lotButton);

    m_startButton = new QPushButton(QString::fromUtf8("\xe5\x90\xaf\xe5\x8a\xa8"), bar);
    m_startButton->setCursor(Qt::PointingHandCursor);
    m_startButton->setStyleSheet(QString::fromUtf8(
        "QPushButton {"
        "  font-family: '\xe6\xa5\xb7\xe4\xbd\x93'; font-size: 22px;"
        "  min-width: 110px; min-height: 46px;"
        "  border: 1px solid #4a7ab5; border-radius: 8px;"
        "  background: #4a7ab5; color: white;"
        "}"
        "QPushButton:hover { background: #3d6aa0; }"
        "QPushButton:pressed { background: #325884; }"
        "QPushButton:disabled { background: #b7c3ce; border-color: #b7c3ce; color: white; }"));
    connect(m_startButton, &QPushButton::clicked, this, &QcChartPage::onStartClicked);
    top->addWidget(m_startButton);

    CustomCreatSql *sql = FullyAutomatedPlatelets::pinstancesqlData();
    if (sql) {
        const QcLotRecord saved = sql->loadQcLots();
        m_highLot = saved.highLot;
        m_midLot = saved.midLot;
        m_lowLot = saved.lowLot;
    }
}

QcChartPage::CurveView QcChartPage::createChart(const QColor &pointColor, int centerPercent, const char *levelUtf8)
{
    CurveView view;
    view.plot = nullptr;
    view.title = nullptr;
    view.cursor = nullptr;
    view.valueText = nullptr;
    view.centerPercent = centerPercent;
    view.levelUtf8 = levelUtf8;

    QCustomPlot *plot = new QCustomPlot(this);
    plot->setStyleSheet(QString::fromUtf8("QCustomPlot { border: 1px solid rgb(190,205,216); background: transparent; }"));
    plot->setBackground(QBrush(kPlotBg));
    plot->setInteractions(QCP::Interactions());
    plot->setMouseTracking(true);

    QFont titleFont(QString::fromUtf8("\xe6\xa5\xb7\xe4\xbd\x93"), 16);
    QFont axisFont(QString::fromUtf8("\xe6\xa5\xb7\xe4\xbd\x93"), 11);
    plot->plotLayout()->insertRow(0);
    QCPTextElement *titleEl = new QCPTextElement(plot, QString(), titleFont);
    titleEl->setTextColor(QColor(40, 56, 72));
    plot->plotLayout()->addElement(0, 0, titleEl);

    const QDate today = QDate::currentDate();
    const double xMin = QCPAxisTickerDateTime::dateTimeToKey(today.addDays(-40));
    const double xMax = QCPAxisTickerDateTime::dateTimeToKey(today.addDays(12));
    const double todayKey = QCPAxisTickerDateTime::dateTimeToKey(today);

    QSharedPointer<QCPAxisTickerDateTime> xTicker(new QCPAxisTickerDateTime);
    xTicker->setDateTimeFormat(QString::fromUtf8("M\xe6\x9c\x88""d"));
    xTicker->setTickCount(6);
    plot->xAxis->setTicker(xTicker);
    plot->xAxis->setRange(xMin, xMax);

    QSharedPointer<PercentAxisTicker> yTicker(new PercentAxisTicker);
    yTicker->setTickStep(1.0);
    yTicker->setScaleStrategy(QCPAxisTickerFixed::ssNone);
    plot->yAxis->setTicker(yTicker);
    plot->yAxis->setRange(centerPercent - 4.2, centerPercent + 4.2);

    plot->xAxis->setTickLabelFont(axisFont);
    plot->yAxis->setTickLabelFont(axisFont);
    plot->xAxis->setBasePen(QPen(kAxis));
    plot->yAxis->setBasePen(QPen(kAxis));
    plot->xAxis->setTickPen(QPen(kAxis));
    plot->yAxis->setTickPen(QPen(kAxis));
    plot->xAxis->setSubTickPen(QPen(kAxis));
    plot->yAxis->setSubTickPen(QPen(kAxis));

    plot->xAxis->grid()->setVisible(false);
    plot->yAxis->grid()->setPen(QPen(kGrid, 1, Qt::DotLine));
    plot->yAxis->grid()->setSubGridVisible(false);

    QCPItemStraightLine *baseLine = new QCPItemStraightLine(plot);
    baseLine->point1->setCoords(xMin, centerPercent);
    baseLine->point2->setCoords(xMax, centerPercent);
    baseLine->setPen(QPen(QColor(70, 110, 150), 1.4, Qt::SolidLine));

    QCPItemStraightLine *cursor = new QCPItemStraightLine(plot);
    cursor->point1->setCoords(todayKey, centerPercent - 4);
    cursor->point2->setCoords(todayKey, centerPercent + 4);
    cursor->setPen(QPen(QColor(176, 64, 64), 1.6, Qt::SolidLine));

    QCPItemText *valueText = new QCPItemText(plot);
    valueText->position->setCoords(todayKey, centerPercent);
    valueText->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    valueText->setFont(QFont(QString::fromUtf8("\xe6\xa5\xb7\xe4\xbd\x93"), 12));
    valueText->setColor(QColor(40, 56, 72));
    valueText->setBrush(QColor(255, 255, 255, 230));
    valueText->setPen(QPen(pointColor));
    valueText->setPadding(QMargins(4, 2, 4, 2));
    valueText->setText(QString());

    QCPGraph *points = plot->addGraph();
    points->setLineStyle(QCPGraph::lsLine);
    points->setPen(QPen(pointColor, 1.6));
    points->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, pointColor, 7));

    connect(plot, &QCustomPlot::mousePress, this, &QcChartPage::onPlotMousePress);
    connect(plot, &QCustomPlot::mouseMove, this, &QcChartPage::onPlotMouseMove);
    connect(plot, &QCustomPlot::mouseRelease, this, &QcChartPage::onPlotMouseRelease);

    plot->replot();
    view.plot = plot;
    view.title = titleEl;
    view.cursor = cursor;
    view.valueText = valueText;
    return view;
}

double chartTickStep(double span)
{
    if (span <= 12.0)
        return 1.0;
    const double rough = span / 8.0;
    double base = 1.0;
    while (base * 10.0 <= rough)
        base *= 10.0;
    if (rough <= base * 2.0)
        return base * 2.0;
    if (rough <= base * 5.0)
        return base * 5.0;
    return base * 10.0;
}

void fitCurveYAxis(QCustomPlot *plot, double center)
{
    if (!plot || plot->graphCount() < 1)
        return;
    QCPGraph *graph = plot->graph(0);
    const int count = graph->dataCount();
    double lowest = center;
    double highest = center;
    for (int i = 0; i < count; ++i) {
        const double value = graph->dataMainValue(i);
        if (value < lowest)
            lowest = value;
        if (value > highest)
            highest = value;
    }
    double lower = center - 4.2;
    double upper = center + 4.2;
    if (lowest < lower || highest > upper) {
        const double span = highest - lowest;
        double pad = span * 0.08;
        if (pad < 4.2)
            pad = 4.2;
        lower = lowest - pad;
        upper = highest + pad;
    }
    plot->yAxis->setRange(lower, upper);
    QSharedPointer<PercentAxisTicker> ticker = qSharedPointerDynamicCast<PercentAxisTicker>(plot->yAxis->ticker());
    if (ticker) {
        ticker->setTickStep(chartTickStep(upper - lower));
        ticker->setTickOrigin(center);
    }
}

void QcChartPage::updateChartTitles()
{
    const int channel = channelNumber();
    const QString lots[3] = { m_highLot, m_midLot, m_lowLot };
    for (int i = 0; i < 3; ++i) {
        if (!m_curve[i].title)
            continue;
        QString text = QString::fromUtf8("\xe9\x80\x9a\xe9\x81\x93%1 ").arg(channel)
                + QString::fromUtf8(m_curve[i].levelUtf8);
        if (!lots[i].isEmpty())
            text += QString::fromUtf8("  \xe6\x89\xb9\xe5\x8f\xb7 ") + lots[i];
        m_curve[i].title->setText(text);
        if (m_curve[i].plot)
            m_curve[i].plot->replot();
    }
}

void QcChartPage::showRecentDays()
{
    QCPGraph *graph = m_highPlot ? m_highPlot->graph(0) : nullptr;
    if (!graph || graph->dataCount() < 1)
        return;
    const double lastKey = graph->dataMainKey(graph->dataCount() - 1);
    const double firstKey = graph->dataMainKey(0);
    const double daySec = 86400.0;
    const double span = 14.0 * daySec;
    double upper = lastKey + daySec;
    double lower = upper - span;
    if (lower < firstKey - daySec)
        lower = firstKey - daySec;
    upper = lower + span;
    for (int i = 0; i < 3; ++i) {
        if (m_curve[i].plot)
            m_curve[i].plot->xAxis->setRange(lower, upper);
    }
}

void QcChartPage::placeCursor(double key)
{
    for (int i = 0; i < 3; ++i) {
        CurveView &view = m_curve[i];
        if (!view.plot || !view.cursor || view.plot->graphCount() < 1)
            continue;
        QCPGraph *graph = view.plot->graph(0);
        const int count = graph->dataCount();
        if (count < 1) {
            view.valueText->setText(QString());
            continue;
        }
        int nearest = 0;
        double best = qAbs(graph->dataMainKey(0) - key);
        for (int n = 1; n < count; ++n) {
            const double dist = qAbs(graph->dataMainKey(n) - key);
            if (dist < best) {
                best = dist;
                nearest = n;
            }
        }
        const double snapped = graph->dataMainKey(nearest);
        const double value = graph->dataMainValue(nearest);
        const QCPRange yRange = view.plot->yAxis->range();
        view.cursor->point1->setCoords(snapped, yRange.lower);
        view.cursor->point2->setCoords(snapped, yRange.upper);
        view.valueText->position->setCoords(snapped, value);
        view.valueText->setText(QString::number(value, 'f', 1) + QLatin1String("%"));
        view.plot->replot();
    }
}

bool QcChartPage::nearCursor(QCustomPlot *plot, int pixelX) const
{
    if (!plot)
        return false;
    for (int i = 0; i < 3; ++i) {
        if (m_curve[i].plot != plot || !m_curve[i].cursor)
            continue;
        const double key = m_curve[i].cursor->point1->key();
        const double lineX = plot->xAxis->coordToPixel(key);
        return qAbs(lineX - pixelX) <= 12.0;
    }
    return false;
}

void QcChartPage::panPlots(double lower)
{
    const double span = m_pressUpper - m_pressLower;
    if (span <= 0)
        return;
    QCPGraph *graph = m_highPlot ? m_highPlot->graph(0) : nullptr;
    if (graph && graph->dataCount() > 0) {
        const double firstKey = graph->dataMainKey(0);
        const double lastKey = graph->dataMainKey(graph->dataCount() - 1);
        const double pad = 86400.0;
        if (lower < firstKey - pad)
            lower = firstKey - pad;
        if (lower + span > lastKey + pad)
            lower = lastKey + pad - span;
    }
    for (int i = 0; i < 3; ++i) {
        if (!m_curve[i].plot)
            continue;
        m_curve[i].plot->xAxis->setRange(lower, lower + span);
        m_curve[i].plot->replot();
    }
}

void QcChartPage::onPlotMousePress(QMouseEvent *event)
{
    QCustomPlot *plot = qobject_cast<QCustomPlot *>(sender());
    if (!plot || !event || event->button() != Qt::LeftButton)
        return;
    m_pointerMoved = false;
    m_pressX = event->pos().x();
    m_pressLower = plot->xAxis->range().lower;
    m_pressUpper = plot->xAxis->range().upper;
    m_dragCursor = nearCursor(plot, m_pressX);
    m_panX = !m_dragCursor;
    if (m_dragCursor)
        plot->setCursor(Qt::SizeHorCursor);
}

void QcChartPage::onPlotMouseMove(QMouseEvent *event)
{
    QCustomPlot *plot = qobject_cast<QCustomPlot *>(sender());
    if (!plot || !event)
        return;
    if (!(event->buttons() & Qt::LeftButton)) {
        plot->setCursor(nearCursor(plot, event->pos().x()) ? Qt::SizeHorCursor : Qt::ArrowCursor);
        return;
    }
    if (qAbs(event->pos().x() - m_pressX) > 3)
        m_pointerMoved = true;
    if (m_dragCursor) {
        placeCursor(plot->xAxis->pixelToCoord(event->pos().x()));
        return;
    }
    if (m_panX && m_pointerMoved) {
        const double width = plot->axisRect()->width();
        if (width > 1.0) {
            const double coordPerPixel = (m_pressUpper - m_pressLower) / width;
            panPlots(m_pressLower + (m_pressX - event->pos().x()) * coordPerPixel);
        }
    }
}

void QcChartPage::onPlotMouseRelease(QMouseEvent *event)
{
    QCustomPlot *plot = qobject_cast<QCustomPlot *>(sender());
    if (plot && event && event->button() == Qt::LeftButton && !m_pointerMoved && !m_dragCursor)
        placeCursor(plot->xAxis->pixelToCoord(event->pos().x()));
    m_dragCursor = false;
    m_panX = false;
    m_pointerMoved = false;
    if (plot)
        plot->setCursor(Qt::ArrowCursor);
}

void QcChartPage::applyLevel(Level level)
{
    const bool all = (level == LevelAll);
    if (m_highPlot)
        m_highPlot->setVisible(all || level == LevelHigh);
    if (m_midPlot)
        m_midPlot->setVisible(all || level == LevelMid);
    if (m_lowPlot)
        m_lowPlot->setVisible(all || level == LevelLow);
}
