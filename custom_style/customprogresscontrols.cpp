#pragma execution_character_set("utf-8")

#include "customprogresscontrols.h"
#include "ui_customprogresscontrols.h"
#include "cglobal.h"
#include  "QsLog/include/QsLog.h"
#include <QPainter>
#include <QGuiApplication>
#include <QMouseEvent>
#include <globaldata.h>

customProgresscontrols::customProgresscontrols(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::customProgresscontrols)
{
    ui->setupUi(this);

    // 基础设置
    setAttribute(Qt::WA_TransparentForMouseEvents, true); // 鼠标事件穿透
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint
                             | Qt::WindowDoesNotAcceptFocus | Qt::NoDropShadowWindowHint);
    setWindowOpacity(0.9);

    setAttribute(Qt::WA_StyledBackground);
    setAutoFillBackground(true);

    m_totalnum = 1;
    setWindowIcon(QIcon(":/Picture/suowei.png"));
    ui->label_ratio->setStyleSheet("border: none;");
    ui->progressBar_ratio->setStyleSheet("QProgressBar{text-align:center;background-color:#DDDDDD;border: 1px solid #DDDDDD;border-radius:5px;}"
                                          "QProgressBar::chunk{background-color:#05B8CC;border-radius:5px; width:6px;margin:0.5px;}");
    ui->progressBar_ratio->setValue(0);
    ui->progressBar_ratio->setRange(0,100);
    ui->progressBar_ratio->setOrientation(Qt::Horizontal);

    // 初始化动画对象
    if(m_showAnimation.isNull()){
        m_showAnimation.reset(new QPropertyAnimation(this, "geometry"));
    }

    //m_showAnimation = new QPropertyAnimation(this, "geometry");
    m_showAnimation->setDuration(500); // 500ms动画时长
    m_showAnimation->setEasingCurve(QEasingCurve::OutCubic); // 平滑动画曲线

    //m_closeAnimation = new QPropertyAnimation(this, "geometry");
    if(m_closeAnimation.isNull()){
        m_closeAnimation.reset(new QPropertyAnimation(this, "geometry"));
    }
    m_closeAnimation->setDuration(500);
    m_closeAnimation->setEasingCurve(QEasingCurve::InCubic);// 带加速效果

    // 连接动画结束信号
    connect(m_closeAnimation.data(), &QPropertyAnimation::finished, this, [=](){
        QWidget::close(); // 动画结束后真正关闭窗口
    });

    // 设置初始位置为屏幕左侧外
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();
    setGeometry(-width(),
                (screenGeometry.height() - height()) / 2,
                width(),
                height());

}

customProgresscontrols::~customProgresscontrols()
{
    m_showAnimation->stop();
    m_closeAnimation->stop();
    QLOG_DEBUG()<<"析构进度框类["<<__FILE__<<"]";
    delete ui;
}

void customProgresscontrols::paintEvent(QPaintEvent *event) {
    //QPainter painter(this);
    //painter.fillRect(event->rect(), Qt::transparent); // 填充透明背景
    // 添加自定义绘制代码（如进度条）
    QWidget::paintEvent(event);
}

void customProgresscontrols::showEvent(QShowEvent *event) {
    if (!m_isAnimating) {
        m_isAnimating = true;

        // 获取屏幕尺寸
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();

        // 初始位置：左侧屏幕外
        QRect startRect = geometry();

        // 目标位置：屏幕中央
        QRect endRect = QRect(
            (screenGeometry.width() - width()) / 2,
            (screenGeometry.height() - height()) / 2,
            width(),
            height()
        );

        // 设置动画参数
        m_showAnimation->setStartValue(startRect);
        m_showAnimation->setEndValue(endRect);
        m_showAnimation->start();
    }

    QWidget::showEvent(event);
}

void customProgresscontrols::closeEvent(QCloseEvent *event) {
    if (!m_isAnimating) {
        event->ignore(); // 阻止立即关闭
        m_isAnimating = true;

        // 获取屏幕尺寸
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->availableGeometry();

        // 当前位置
        QRect startRect = geometry();

        // 目标位置：右侧屏幕外
        QRect endRect = QRect(
            screenGeometry.width(),
            startRect.y(),
            width(),
            height()
        );

        // 设置关闭动画
        m_closeAnimation->setStartValue(startRect);
        m_closeAnimation->setEndValue(endRect);
        m_closeAnimation->start();
    }
}

// 添加鼠标拖动支持
void customProgresscontrols::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void customProgresscontrols::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - m_dragPosition);
        event->accept();
    }
}


void customProgresscontrols::_settitle(const QString title)
{
    this->setWindowTitle(title);
}

void customProgresscontrols::_setprogressvalue(const QString info,int numstep)
{
    ui->progressBar_ratio->setValue(numstep*100/m_totalnum);
    ui->label_ratio->setText(QString("%1%2%").arg(info).arg(numstep*100/m_totalnum));
    ui->progressBar_ratio->setFormat("");
    ui->progressBar_ratio->update();
}

void customProgresscontrols::_setprogressRand(int min,int max)
{
    ui->progressBar_ratio->setRange(min,max);
}


void customProgresscontrols::_setprogresstotalnum(int numtotal)
{
    if(numtotal != 0)
        m_totalnum = numtotal;
}

void customProgresscontrols::_setlabletext(const QString reminder)
{
    ui->label_ratio->setText(reminder);
}

void customProgresscontrols::_setMinvalue(int val){ ui->progressBar_ratio->setMinimum(val); }

void  customProgresscontrols::_setMaxvalue(int val){ ui->progressBar_ratio->setMaximum(val); }

int  customProgresscontrols::_getprogressdata(){ return ui->progressBar_ratio->value(); }


