#pragma execution_character_set("utf-8")

#include "connecttoinstrument.h"
#include "ui_connecttoinstrument.h"
#include "QsLog/include/QsLog.h"
#include "cglobal.h"
#include "QsLog/include/QsLog.h"
#include <QDesktopWidget>

ConnectToInstrument::ConnectToInstrument(QString titlestr, QString warmtext, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnectToInstrument)
{
    ui->setupUi(this);
    setMouseTracking(true);



    this->setWindowTitle("提示");
    this->setWindowIcon(QIcon(":/Picture/suowei.png"));

    ui->label_icon->setFixedSize(32,32);

    QPixmap pixmapmouse(":/Picture/test_hover.png");
    QCursor cursor(pixmapmouse);
    this->setCursor(cursor);

    ui->label_name->setText(titlestr);
    ui->label_warmtext->setText(warmtext);
    setWindowFlags(Qt::FramelessWindowHint);

    setConnectBtn();
    InitStyle();

    m_myborder = new MyBorderContainer(this);

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &ConnectToInstrument::onTimeout);
    m_timer->setInterval(1000);
    m_timer->start();
}

ConnectToInstrument::~ConnectToInstrument()
{
    delete ui;
}

void ConnectToInstrument::on_pushButton_ok_clicked()
{
    emit giveupTestSample();
    close();
}

void ConnectToInstrument::notifybuttonDisplayName(QString surename,QString cancelname ){
    ui->pushButton_ok->setText(surename);
    ui->pushButton_cancel->setText(cancelname);
}

void ConnectToInstrument::settitle(QString title_)
{
    ui->label_name->setText(title_);
}

void ConnectToInstrument::setremtext(QString outputText)
{
     ui->label_warmtext->setText(outputText);
}

void ConnectToInstrument::setConnectBtn(){
    connect(ui->toolButton_min,&QToolButton::clicked,this,[=](){
        this->showMinimized();
    });

    connect(ui->toolButton_max, &QToolButton::clicked, this, [=]() {
        onBtnMenuMaxClicked();
    });


    connect(ui->toolButton_close,&QToolButton::clicked,this,[=](){
          emit pendingTimeout(); //默认挂起
          close();


    });
}
void ConnectToInstrument::onBtnMenuMaxClicked( )
{
    if (max) {
        this->setGeometry(location);
        ui->toolButton_max->setToolTip("最大化");
        ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/最大化.png"));
    }
    else {
        QDesktopWidget* desktop = QApplication::desktop();
        int N = desktop->screenCount();
        if (N == 2)
        {
            location = this->geometry();
            setGeometry(desktop->screenGeometry(1));
            ui->toolButton_max->setToolTip("还原");
            ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));

        }
        else {
            location = this->geometry();
            this->setGeometry(qApp->desktop()->availableGeometry());
            ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));
            ui->toolButton_max->setToolTip("还原");
        }
    }
    max = !max;
}

void ConnectToInstrument::InitStyle()
{
    max = false;
    this->location = this->geometry();
    mousePressed = false;

    connect(ui->pushButton_cancel,&QPushButton::clicked,this,[this](){
        emit tryAgainTestSample();
        close();
    });

}

bool ConnectToInstrument::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->onBtnMenuMaxClicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void ConnectToInstrument::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void ConnectToInstrument::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void ConnectToInstrument::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}



void ConnectToInstrument::closeEvent(QCloseEvent *event)
{
    event->accept();
    delete this;
}

void ConnectToInstrument::onTimeout()
{
    remainingSeconds--;
    ui->pushButton_hendingtime->setText(QString("挂起: %1 秒").arg(remainingSeconds));

    if (remainingSeconds <= 0) {
        m_timer->stop();  // 停止定时器
        emit pendingTimeout();
        QLOG_DEBUG() << "倒计时结束";
    }
}
