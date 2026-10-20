#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "instrumentalarmprompt.h"
#include "loginui.h"
#include "ui_instrumentalarmprompt.h"
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QStyle>
#include "QsLog/include/QsLog.h"
#include "quiutils.h"
#include "cglobal.h"

instrumentAlarmPrompt::instrumentAlarmPrompt(QWidget *parent, int alarmIndex, const QString alarmText) :
    QWidget(parent),
    ui(new Ui::instrumentAlarmPrompt)
{
    ui->setupUi(this);
    bgmcColor = QColor("#F4F8FB");
    setAttribute(Qt::WA_StyledBackground, true);
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    malarmIndex = alarmIndex;

    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    setFixedSize(420, 196);

    setStyleSheet(
        "QWidget#instrumentAlarmPrompt{"
        "  background-color:#F4F8FB;"
        "  border:none;"
        "  font-family:'Microsoft YaHei';"
        "}"
        "QWidget#widget_titlebar{"
        "  background-color:#1A6B8A;"
        "  border:none;"
        "  border-top-left-radius:8px;"
        "  border-top-right-radius:8px;"
        "}"
        "QLabel#label_alarminfo{"
        "  color:#FFFFFF;"
        "  background:transparent;"
        "  border:none;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:15px;"
        "  font-weight:bold;"
        "}"
        "QLabel#label_image{"
        "  background:transparent;"
        "  border:none;"
        "}"
        "QLabel#label_reminderText{"
        "  background:transparent;"
        "  border:none;"
        "  color:#2C4A5A;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:14px;"
        "  padding:8px 16px;"
        "}"
        "QWidget#widgetputbutton{"
        "  background:transparent;"
        "}");

    ui->verticalLayout->setSpacing(0);
    ui->verticalLayout->setContentsMargins(0, 0, 0, 0);
    ui->widget_titlebar->setMinimumHeight(40);
    ui->widget_titlebar->setMaximumHeight(40);
    ui->horizontalLayout->setContentsMargins(12, 0, 8, 0);

    ui->label_alarminfo->setMinimumWidth(0);
    ui->label_alarminfo->setMaximumWidth(QWIDGETSIZE_MAX);
    ui->label_alarminfo->setText(tr("主控板提示"));

    ui->label_image->setFixedSize(22, 22);
    const QIcon warnIcon = style()->standardIcon(QStyle::SP_MessageBoxWarning);
    if (!warnIcon.isNull()) {
        ui->label_image->setPixmap(warnIcon.pixmap(22, 22));
        ui->label_image->setScaledContents(true);
    } else {
        QPixmap pixmap(":/Picture/reminderalarm.png");
        if (!pixmap.isNull()) {
            ui->label_image->setScaledContents(true);
            ui->label_image->setPixmap(
                pixmap.scaled(22, 22, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    ui->label_reminderText->setWordWrap(true);
    ui->label_reminderText->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    ui->label_reminderText->setMinimumHeight(70);
    ui->label_reminderText->setMaximumHeight(QWIDGETSIZE_MAX);
    ui->label_reminderText->setText(alarmText);

    ui->toolButton_close->setStyleSheet(
        "QToolButton{border:none;background:transparent;color:#FFFFFF;"
        "font-family:'Microsoft YaHei';font-size:16px;}"
        "QToolButton:hover{color:#FFE8C8;}");

    ui->horizontalLayout_2->setContentsMargins(16, 8, 16, 14);
    ui->toolButton_closeReminder->setMinimumSize(108, 34);
    ui->toolButton_closeReminder->setMaximumSize(140, 36);
    ui->toolButton_closeReminder->setText(tr("知道了"));
    ui->toolButton_closeReminder->setStyleSheet(
        "QToolButton{"
        "  background:#1A6B8A;"
        "  color:#FFFFFF;"
        "  border:none;"
        "  border-radius:6px;"
        "  font-family:'Microsoft YaHei';"
        "  font-size:14px;"
        "  padding:4px 16px;"
        "}"
        "QToolButton:hover{background:#22809F;}"
        "QToolButton:pressed{background:#155A75;}");
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
    p.setBrush(QColor("#F4F8FB"));
    p.setPen(QPen(QColor("#C5D8E4"), 1));
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
