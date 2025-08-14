#pragma execution_character_set("utf-8")

#include "instrumentalarmprompt.h"
#include "loginui.h"
#include "ui_instrumentalarmprompt.h"
#include <QCloseEvent>
#include <QPainter>
#include "QsLog/include/QsLog.h"
#include "quiutils.h"

instrumentAlarmPrompt::instrumentAlarmPrompt(QWidget *parent, int alarmIndex, const QString alarmText) :
    QWidget(parent),
    ui(new Ui::instrumentAlarmPrompt)
{
    ui->setupUi(this);
    bgmcColor.setRgb(139, 137, 137);
    setWindowFlags(Qt::FramelessWindowHint);
    malarmIndex = alarmIndex;

    ui->label_alarminfo->setText(tr("仪器提示"));
    ui->label_image->setFixedSize(32,32);
    QPixmap *pixmap = new QPixmap(":/Picture/reminderalarm.png");
    pixmap->scaled(ui->label_image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_image->setScaledContents(true);
    ui->label_image->setPixmap(*pixmap);
    delete pixmap;
    pixmap = nullptr;

    ui->label_reminderText->setText(alarmText);

}

instrumentAlarmPrompt::~instrumentAlarmPrompt()
{
    delete ui;
}

void instrumentAlarmPrompt::_configalarmindex(int index_)
{
    malarmIndex = index_;
    return;
}
void instrumentAlarmPrompt::_configalarmText(QString outtext_)
{
    QUIUtils::QLabeldisplayIcon(ui->label_reminderText,":/Picture/reminderalarm.png",outtext_,4);
    return;
}

void instrumentAlarmPrompt::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = event->pos();
    }
}
void instrumentAlarmPrompt::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() - mouseStartPoint;
        if (event->globalY() > desktopRc.height())
        {
            curPoint.setY(desktopRc.height() - mouseStartPoint.y());
        }
        move(curPoint);
    }

    QWidget::mouseMoveEvent(event);
}
void instrumentAlarmPrompt::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}
void instrumentAlarmPrompt::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = bgmcColor;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}
void instrumentAlarmPrompt::closeEvent(QCloseEvent *event)
{
    event->accept();
    if(malarmIndex == equipmentTipInfo::LinqueCleanShortage)
    {
        QLOG_DEBUG()<< "S2清洗液 - 1"<< endl;
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


