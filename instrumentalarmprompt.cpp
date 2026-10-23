#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "instrumentalarmprompt.h"
#include "loginui.h"
#include "ui_instrumentalarmprompt.h"
#include <QCloseEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include "QsLog/include/QsLog.h"
#include "quiutils.h"
#include "cglobal.h"

namespace {

void drawWasteTank(QPainter &p, const QRectF &r)
{
    const qreal penW = qMax(1.2, r.width() / 16.0);
    const QRectF tank(r.left() + r.width() * 0.16,
                      r.top() + r.height() * 0.08,
                      r.width() * 0.68,
                      r.height() * 0.84);
    p.setPen(QPen(QColor("#7A3414"), penW));
    p.setBrush(QColor("#FFF8F2"));
    p.drawRoundedRect(tank, tank.width() * 0.16, tank.width() * 0.16);

    const QRectF fill(tank.left() + tank.width() * 0.14,
                      tank.top() + tank.height() * 0.12,
                      tank.width() * 0.72,
                      tank.height() * 0.74);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#D3542C"));
    p.drawRoundedRect(fill, penW, penW);

    p.setBrush(QColor("#F3B089"));
    p.drawRoundedRect(QRectF(fill.left(), fill.top(), fill.width(), qMax(2.0, fill.height() * 0.16)),
                      penW, penW);
}

void drawCleanBottle(QPainter &p, const QRectF &r)
{
    const qreal penW = qMax(1.2, r.width() / 16.0);
    const QRectF cap(r.left() + r.width() * 0.34,
                     r.top() + r.height() * 0.06,
                     r.width() * 0.32,
                     r.height() * 0.16);
    const QRectF body(r.left() + r.width() * 0.20,
                      r.top() + r.height() * 0.20,
                      r.width() * 0.60,
                      r.height() * 0.70);
    p.setPen(QPen(QColor("#1A5F86"), penW));
    p.setBrush(QColor("#F4FBFE"));
    p.drawRoundedRect(cap, penW, penW);
    p.drawRoundedRect(body, body.width() * 0.22, body.width() * 0.22);

    const QRectF liq(body.left() + body.width() * 0.14,
                     body.top() + body.height() * 0.66,
                     body.width() * 0.72,
                     body.height() * 0.22);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#3AA0D8"));
    p.drawRoundedRect(liq, penW, penW);

    const qreal dot = qMax(3.0, r.width() * 0.16);
    p.setBrush(QColor("#C47A12"));
    p.drawEllipse(QRectF(r.right() - dot - r.width() * 0.04,
                         r.top() + r.height() * 0.08,
                         dot, dot));
}

} // namespace

QPixmap instrumentAlarmPrompt::consumableGlyph(int alarmIndex, int side, qreal dpr, bool bright)
{
    if (dpr < 1.0)
        dpr = 1.0;
    if (side < 16)
        side = 16;

    QPixmap pm(qRound(side * dpr), qRound(side * dpr));
    pm.setDevicePixelRatio(dpr);
    pm.fill(Qt::transparent);

    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setOpacity(bright ? 1.0 : 0.72);
    const QRectF box(1.0, 1.0, side - 2.0, side - 2.0);
    if (alarmIndex == equipmentTipInfo::LinqueScrapFull)
        drawWasteTank(p, box);
    else
        drawCleanBottle(p, box);
    return pm;
}

instrumentAlarmPrompt::instrumentAlarmPrompt(QWidget *parent, int alarmIndex, const QString alarmText) :
    QWidget(parent),
    ui(new Ui::instrumentAlarmPrompt)
{
    ui->setupUi(this);
    bgmcColor = QColor("#F7FBFD");
    m_accent = QColor("#1A5F86");
    setAttribute(Qt::WA_StyledBackground, true);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    malarmIndex = alarmIndex;

    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setFixedSize(448, 206);

    ui->verticalLayout->setSpacing(0);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    ui->widget_titlebar->setMinimumHeight(46);
    ui->widget_titlebar->setMaximumHeight(46);
    ui->horizontalLayout->setContentsMargins(16, 0, 8, 0);

    ui->label_image->hide();
    ui->label_alarminfo->setMinimumWidth(0);
    ui->label_alarminfo->setMaximumWidth(QWIDGETSIZE_MAX);
    ui->label_alarminfo->setText(tr("主控板提示"));

    ui->label_reminderText->setWordWrap(true);
    ui->label_reminderText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->label_reminderText->setMinimumHeight(48);
    ui->label_reminderText->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->label_reminderText->setText(alarmText);

    QWidget *body = new QWidget(this);
    body->setObjectName(QStringLiteral("widget_alarmbody"));
    QHBoxLayout *row = new QHBoxLayout(body);
    row->setContentsMargins(18, 14, 18, 4);
    row->setSpacing(14);
    m_glyphLabel = new QLabel(body);
    m_glyphLabel->setObjectName(QStringLiteral("label_bigGlyph"));
    m_glyphLabel->setFixedSize(72, 72);
    m_glyphLabel->setAlignment(Qt::AlignCenter);
    row->addWidget(m_glyphLabel, 0, Qt::AlignVCenter);
    row->addWidget(ui->label_reminderText, 1);
    ui->verticalLayout->insertWidget(1, body);

    ui->toolButton_close->setIcon(QIcon());
    ui->toolButton_close->setText(QStringLiteral("\u00D7"));
    ui->toolButton_close->setToolButtonStyle(Qt::ToolButtonTextOnly);
    ui->toolButton_close->setFixedSize(28, 28);

    ui->horizontalLayout_2->setContentsMargins(16, 4, 16, 14);
    ui->toolButton_closeReminder->setMinimumSize(108, 34);
    ui->toolButton_closeReminder->setMaximumSize(140, 36);
    ui->toolButton_closeReminder->setText(tr("知道了"));
    applyAlarmFace();
}

instrumentAlarmPrompt::~instrumentAlarmPrompt()
{
    delete ui;
}

void instrumentAlarmPrompt::_configalarmindex(int index_)
{
    malarmIndex = index_;
    QString title = tr("主控板提示");
    switch (index_) {
    case equipmentTipInfo::LinqueScrapFull:
        title = tr("废液罐已满");
        break;
    case equipmentTipInfo::LinqueCleanShortage:
        title = tr("外部清洗液不足");
        break;
    default:
        break;
    }
    ui->label_alarminfo->setText(title);
    applyAlarmFace();
}

void instrumentAlarmPrompt::applyAlarmFace()
{
    const bool waste = (malarmIndex == equipmentTipInfo::LinqueScrapFull);
    m_accent = waste ? QColor("#9A4E24") : QColor("#1A5F86");
    const QColor tint = waste ? QColor("#F8E8DC") : QColor("#E5F3FA");
    const QString font = QString::fromUtf8("\xE6\xA5\xB7\xE4\xBD\x93");
    const QString accent = m_accent.name();

    setStyleSheet(QString(
        "QWidget#instrumentAlarmPrompt{background:#F7FBFD;border:none;font-family:'%1';}"
        "QWidget#widget_alarmbody{background:transparent;}"
        "QWidget#widgetputbutton{background:transparent;}"
    ).arg(font));

    ui->widget_titlebar->setStyleSheet(QString(
        "QWidget#widget_titlebar{background:%1;border:none;"
        "border-top-left-radius:8px;border-top-right-radius:8px;}"
        "QLabel#label_alarminfo{color:#FFFFFF;background:transparent;border:none;"
        "font-family:'%2';font-size:16px;font-weight:bold;}"
    ).arg(accent, font));

    ui->label_reminderText->setStyleSheet(QString(
        "background:transparent;border:none;color:#2C4A5A;"
        "font-family:'%1';font-size:15px;"
    ).arg(font));

    ui->toolButton_close->setStyleSheet(
        "QToolButton{border:none;background:transparent;color:#FFFFFF;font-size:18px;}"
        "QToolButton:hover{color:#FFE8C8;}");

    ui->toolButton_closeReminder->setStyleSheet(QString(
        "QToolButton{background:%1;color:#FFFFFF;border:none;border-radius:6px;"
        "font-family:'%2';font-size:15px;padding:4px 16px;}"
        "QToolButton:hover{background:%3;}"
        "QToolButton:pressed{background:%4;}"
    ).arg(accent, font, m_accent.lighter(115).name(), m_accent.darker(115).name()));

    if (m_glyphLabel) {
        m_glyphLabel->setStyleSheet(QString(
            "QLabel#label_bigGlyph{background:%1;border-radius:16px;}").arg(tint.name()));
        m_glyphLabel->setPixmap(consumableGlyph(malarmIndex, 52, devicePixelRatioF(), true));
    }
}

void instrumentAlarmPrompt::_configalarmText(QString outtext_)
{
    ui->label_reminderText->setText(outtext_);
}

void instrumentAlarmPrompt::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton)
        mouseStartPoint = event->pos();
}

void instrumentAlarmPrompt::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() - mouseStartPoint;
        if (event->globalY() > desktopRc.height())
            curPoint.setY(desktopRc.height() - mouseStartPoint.y());
        move(curPoint);
    }
    QWidget::mouseMoveEvent(event);
}

void instrumentAlarmPrompt::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    QWidget::mousePressEvent(event);
}

void instrumentAlarmPrompt::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(QColor("#F7FBFD"));
    p.setPen(QPen(m_accent.isValid() ? m_accent : QColor("#C5D8E4"), 1));
    p.drawRoundedRect(rect().adjusted(1, 1, -2, -2), 8, 8);
}

void instrumentAlarmPrompt::closeEvent(QCloseEvent *event)
{
    event->accept();
    if (malarmIndex == equipmentTipInfo::LinqueCleanShortage) {
        QLOG_DEBUG() << "S2清洗液 - 1";
        emit outSideCleanDepleteOne();
    }
    close();
}

void instrumentAlarmPrompt::on_toolButton_closeReminder_clicked()
{
    close();
}

void instrumentAlarmPrompt::on_toolButton_close_clicked()
{
    close();
}
