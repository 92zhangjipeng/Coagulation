#pragma execution_character_set("utf-8")
#include "controlthemoduletemp.h"
#include "ui_controlthemoduletemp.h"
#include "quiutils.h"
#include <QDesktopWidget>

ControltheModuletemp::ControltheModuletemp(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControltheModuletemp)
{
    ui->setupUi(this);

    bgmcColor.setRgb(230, 230, 230);
    setWindowFlags(Qt::Tool |
                    Qt::FramelessWindowHint |
                    Qt::WindowStaysOnTopHint);

    ui->labelICON->setFixedSize(32,32);
    QPixmap *pixmap = new QPixmap(":/Picture/suowei.png");
    pixmap->scaled(ui->labelICON->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelICON->setScaledContents(true);
    ui->labelICON->setPixmap(*pixmap);
    delete pixmap;
    pixmap = nullptr;
    ui->label_title->setText("模组调光异常提示");

}

ControltheModuletemp::~ControltheModuletemp()
{
    delete ui;

    QLOG_DEBUG()<<"退出析构控温"<<__FUNCTION__<<__LINE__<<endl;
}

void ControltheModuletemp::mousePressEvent(QMouseEvent *event)
{
    event->accept();
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = event->pos();
    }
}
void ControltheModuletemp::mouseMoveEvent(QMouseEvent *event)
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

void ControltheModuletemp::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

void ControltheModuletemp::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = bgmcColor;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}

void ControltheModuletemp::showEvent(QShowEvent *event)
{
    this->setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(event);
}

void ControltheModuletemp::settipstext(const QString tips,const QString titleText)
{
    ui->labeltips_failed->setText(tips);
    setWindowTitle(titleText);
    m_dimmingText = titleText;
}




void ControltheModuletemp::on_toolButton_close_clicked()
{
    this->close();
}

//直接禁用通道
void ControltheModuletemp::on_toolButtonok_clicked()
{
    emit this->disableusechn();
    this->close();
}

void ControltheModuletemp::on_toolButtonexit_clicked()
{
    emit this->re_dimming();
    this->close();
}

void ControltheModuletemp::closeEvent(QCloseEvent *event)
{
    emit this->deletedimming(m_dimmingText);
    event->accept();

}
