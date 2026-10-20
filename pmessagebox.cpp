#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "loginui.h"
#include "pmessagebox.h"
#include "ui_pmessagebox.h"
#include "QsLog/include/QsLog.h"

#include <operclass/fullyautomatedplatelets.h>

#include "cglobal.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QStyle>
#include <QVector>

namespace {

void clearOwnedLayout(QWidget *host)
{
    if (!host)
        return;
    QLayout *old = host->layout();
    if (!old)
        return;
    while (QLayoutItem *item = old->takeAt(0)) {
        if (QWidget *child = item->widget())
            delete child;
        delete item;
    }
    delete old;
}

class TrayWellMap : public QWidget
{
public:
    explicit TrayWellMap(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setAttribute(Qt::WA_StyledBackground, true);
        m_unused.fill(false, ONETRAY_TOTALTUBE);
    }

    void setUnusedHoles(const QVector<bool> &unused)
    {
        m_unused = unused;
        if (m_unused.size() < ONETRAY_TOTALTUBE)
            m_unused.resize(ONETRAY_TOTALTUBE);
        update();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);

        const QRectF plate = QRectF(rect()).adjusted(8, 8, -8, -8);
        p.setPen(QPen(QColor("#8FA8B6"), 1));
        p.setBrush(QColor("#C9D8E2"));
        p.drawRoundedRect(plate, 10, 10);

        const int cols = 6;
        const int rows = 10;
        const qreal padX = 16;
        const qreal padY = 12;
        const qreal cellW = (plate.width() - padX * 2) / cols;
        const qreal cellH = (plate.height() - padY * 2) / rows;
        const qreal bigR = qMin(cellW, cellH) * 0.38;
        const qreal smallR = bigR * 0.62;

        for (int i = 0; i < ONETRAY_TOTALTUBE; ++i) {
            const int row = i / cols;
            const int col = i % cols;
            const QPointF c(
                plate.left() + padX + cellW * (col + 0.5),
                plate.top() + padY + cellH * (row + 0.5));
            const bool unused = m_unused.value(i);

            p.setPen(QPen(cglobal::g_LineColor, 1));
            p.setBrush(cglobal::g_OuterRingColor);
            p.drawEllipse(c, bigR, bigR);

            p.setPen(Qt::NoPen);
            p.setBrush(unused ? cglobal::g_SamllBgmColor : cglobal::g_OutResult);
            p.drawEllipse(c, smallR, smallR);
        }
    }

private:
    QVector<bool> m_unused;
};

} // namespace

PMessageBox::PMessageBox(QWidget *parent, int indexTray) :QDialog(parent),
ui(new Ui::PMessageBox)
{
    ui->setupUi(this);
    bgmcColor = QColor("#F4F8FB");
    setAttribute(Qt::WA_StyledBackground, true);
    setWindowFlags(Qt::Tool |
                   Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);

    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setFixedSize(440, 528);

    setStyleSheet(
        "QDialog#PMessageBox{"
        "  background-color:#F4F8FB;"
        "  border:none;"
        "  font-family:'Microsoft YaHei';"
        "}"
        "QWidget#widget_title{"
        "  background-color:#1A6B8A;"
        "  border:none;"
        "  border-radius:8px;"
        "}"
        "QLabel#label_titlename{"
        "  color:#FFFFFF;"
        "  background:transparent;"
        "  border:none;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:16px;"
        "  font-weight:bold;"
        "  padding:0px;"
        "}"
        "QLabel#label_showicon{"
        "  background:transparent;"
        "  border:none;"
        "}"
        "QWidget#widget_TrayShow{"
        "  background:#FFFFFF;"
        "  border:1px solid #E2EEF4;"
        "  border-radius:8px;"
        "}"
        "QLabel#label_remindertext{"
        "  background:#FFF8E8;"
        "  border:1px solid #E8D39A;"
        "  border-radius:6px;"
        "  color:#5C4A1F;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:13px;"
        "  font-weight:normal;"
        "  padding:8px 10px;"
        "}"
        "QWidget#widget{"
        "  background:transparent;"
        "}");

    ui->verticalLayout_traylayout->setSpacing(10);
    ui->verticalLayout_traylayout->setContentsMargins(12, 12, 12, 12);

    ui->widget_title->setMinimumHeight(40);
    ui->widget_title->setMaximumHeight(40);

    ui->widget_TrayShow->setMinimumSize(0, 0);
    ui->widget_TrayShow->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    ui->widget_TrayShow->setFixedHeight(292);

    ui->label_remindertext->setMinimumHeight(0);
    ui->label_remindertext->setMaximumHeight(88);
    ui->label_remindertext->setWordWrap(true);
    ui->label_remindertext->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->label_titlename->setText(tr("试管盘%1 脱离位置").arg(indexTray));
    ui->label_titlename->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    ui->label_showicon->setFixedSize(22, 22);
    const QIcon warnIcon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
    if (!warnIcon.isNull()) {
        ui->label_showicon->setPixmap(warnIcon.pixmap(22, 22));
        ui->label_showicon->setScaledContents(true);
    } else {
        QPixmap pixmap(":/Picture/reminderalarm.png");
        if (!pixmap.isNull()) {
            ui->label_showicon->setScaledContents(true);
            ui->label_showicon->setPixmap(
                pixmap.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    ui->horizontalLayout->setContentsMargins(0, 4, 0, 4);
    ui->horizontalLayout->setSpacing(12);
    ui->horizontalLayout->removeWidget(ui->toolButton_OK);
    ui->horizontalLayout->removeWidget(ui->toolButton_Cancel);
    ui->horizontalLayout->addStretch();
    ui->horizontalLayout->addWidget(ui->toolButton_Cancel);
    ui->horizontalLayout->addWidget(ui->toolButton_OK);

    ui->toolButton_Cancel->setMinimumSize(132, 36);
    ui->toolButton_Cancel->setMaximumSize(180, 36);
    ui->toolButton_OK->setMinimumSize(132, 36);
    ui->toolButton_OK->setMaximumSize(180, 36);
    ui->toolButton_Cancel->setText(tr("忽略，杯子还要用"));
    ui->toolButton_OK->setText(tr("弃用剩余杯"));
    ui->toolButton_Cancel->setDefault(true);
    ui->toolButton_Cancel->setAutoDefault(true);
    ui->toolButton_OK->setDefault(false);
    ui->toolButton_OK->setAutoDefault(false);

    ui->toolButton_Cancel->setStyleSheet(
        "QPushButton{"
        "  background:#FFFFFF;"
        "  color:#2C4A5A;"
        "  border:1px solid #C5D8E4;"
        "  border-radius:6px;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:13px;"
        "  font-weight:normal;"
        "  padding:4px 10px;"
        "}"
        "QPushButton:hover{background:#EAF4F8;border-color:#9BB8C8;}"
        "QPushButton:pressed{background:#D7E8F0;}");
    ui->toolButton_OK->setStyleSheet(
        "QPushButton{"
        "  background:#E85D4C;"
        "  color:#FFFFFF;"
        "  border:1px solid #C94A3C;"
        "  border-radius:6px;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:13px;"
        "  font-weight:bold;"
        "  padding:4px 10px;"
        "}"
        "QPushButton:hover{background:#F07162;}"
        "QPushButton:pressed{background:#C94A3C;}"
        "QPushButton:disabled{background:#E2B8B2;border-color:#D7A8A2;color:#F7EDEA;}");

    mIndextubeTray = indexTray - 1;
    testTubeTrayLastNum(mIndextubeTray);
}

PMessageBox::~PMessageBox()
{
    if (m_pdelegatesTube) {
        delete[] m_pdelegatesTube;
        m_pdelegatesTube = nullptr;
    }
    delete ui;
}

void PMessageBox::setindexTray(int index_)
{
    ui->label_titlename->setText(tr("试管盘%1 脱离位置").arg(index_));
    mIndextubeTray = index_ - 1;
    testTubeTrayLastNum(mIndextubeTray);
}

bool PMessageBox::isPartiallyVisible(QDialog* pwidget)
{
    if (!pwidget)
        return false;
    QRect widgetRect = pwidget->rect();
    QPoint globalPos = pwidget->mapToGlobal(QPoint(0, 0));
    QRect screenrect = QApplication::desktop()->screenGeometry(globalPos);
    return screenrect.intersects(widgetRect);
}

void PMessageBox::testTubeTrayLastNum(const int alarmIndex)
{
    QVariantList TubeNumInfo;
    const int starttube = alarmIndex * ONETRAY_TOTALTUBE;
    const int endtube = (alarmIndex + 1) * ONETRAY_TOTALTUBE;

    QVector<int> Tubenumber;
    mnotuseTube.clear();
    Tubenumber.reserve(ONETRAY_TOTALTUBE);
    for (int i = starttube; i < endtube; ++i)
        Tubenumber.push_back(i);

    if (!FullyAutomatedPlatelets::pinstancesqlData()) {
        QLOG_ERROR() << "SQL data instance is null";
        return;
    }

    FullyAutomatedPlatelets::pinstancesqlData()->FindAllEmptyTube(TubeNumInfo);
    for (int i = 0; i < TubeNumInfo.size(); ++i) {
        const AllTubeInfo tempinfo = TubeNumInfo.at(i).value<AllTubeInfo>();
        const int numbertube = tempinfo.TubeNumbers;
        if (Tubenumber.contains(numbertube)
                && tempinfo.TubeStatus == TESTTUBES_FREETIME) {
            mnotuseTube.push_back(numbertube);
        }
    }
    creatTrayPointLoc(ui->widget_TrayShow, mnotuseTube, starttube);
}

void PMessageBox::creatTrayPointLoc(QWidget *widgetRect, QList<int> &notUsedTubeList, const int indexhole)
{
    if (!widgetRect)
        return;

    if (m_pdelegatesTube) {
        delete[] m_pdelegatesTube;
        m_pdelegatesTube = nullptr;
    }
    clearOwnedLayout(widgetRect);

    QVector<bool> unused(ONETRAY_TOTALTUBE, false);
    for (int i = 0; i < ONETRAY_TOTALTUBE; ++i) {
        if (notUsedTubeList.contains(indexhole + i))
            unused[i] = true;
    }

    auto *root = new QVBoxLayout(widgetRect);
    root->setContentsMargins(6, 4, 6, 4);
    root->setSpacing(4);

    auto *map = new TrayWellMap(widgetRect);
    map->setUnusedHoles(unused);

    auto *legend = new QLabel(widgetRect);
    legend->setAlignment(Qt::AlignCenter);
    legend->setStyleSheet(
        "QLabel{background:transparent;border:none;color:#5A7384;"
        "font-family:'Microsoft YaHei';font-size:12px;}");
    legend->setText(tr("浅色杯：未用　　深色孔：已取走"));

    root->addWidget(map, 1);
    root->addWidget(legend);

    const int left = notUsedTubeList.size();
    if (left > 0) {
        ui->label_remindertext->setText(
            tr("试管盘已离开到位，本盘还有 %1 个未用试杯。\n"
               "请先把盘子放回。若已换新盘，再点「弃用剩余杯」，把这些杯子标为已用。")
                .arg(left));
        ui->toolButton_OK->setEnabled(true);
    } else {
        ui->label_remindertext->setText(
            tr("试管盘已离开到位。本盘试杯已全部用完，放回或更换新盘即可。"));
        ui->toolButton_OK->setEnabled(false);
    }
}

void PMessageBox::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton)
        mouseStartPoint = event->pos();
}

void PMessageBox::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        const QPoint delta = event->pos() - mouseStartPoint;
        move(pos() + delta);
    }
}

void PMessageBox::mouseReleaseEvent(QMouseEvent *event)
{
    event->accept();
}

void PMessageBox::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor("#F4F8FB"));
    painter.setPen(QPen(QColor("#C5D8E4"), 1));
    painter.drawRoundedRect(rect().adjusted(1, 1, -2, -2), 8, 8);
}

void PMessageBox::on_toolButton_OK_clicked()
{
    emit _makesureupdateTraytube(mIndextubeTray);
    accept();
}

void PMessageBox::on_toolButton_Cancel_clicked()
{
    emit _cancelupdateTraytube(mIndextubeTray);
    reject();
}
