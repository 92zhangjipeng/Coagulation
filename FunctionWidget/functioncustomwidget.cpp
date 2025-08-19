#pragma execution_character_set("utf-8")

#include "functioncustomwidget.h"
#include "ui_functioncustomwidget.h"
#include <QDesktopWidget>

FunctionCustomWidget::FunctionCustomWidget(QString titlestr, QString warmtext,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionCustomWidget)
{
    ui->setupUi(this);
    setMouseTracking(true);
    setWindowTitle("提示");
    QImage image(":/Picture/icon_title/提示.png");
    ui->label_icon->setPixmap(QPixmap::fromImage(image));
    ui->label_name->setText(titlestr);
    ui->label_warmtext->setText(warmtext);
    setWindowFlags(Qt::FramelessWindowHint);
    this->setConnectBtn();
    this->InitStyle();
//    ui->toolButton_func_1->setText("取消退出");
//    ui->toolButton_func_2->setText("关机清洗");
//    ui->toolButton_func_3->setText("退出软件");
    m_myborder = new MyBorderContainer(this);
}

FunctionCustomWidget::~FunctionCustomWidget()
{
    delete ui;
}

void FunctionCustomWidget::setremimdertext(QString reminder_)
{
    ui->label_warmtext->setText(reminder_);
}
void FunctionCustomWidget::setTextbtnfunction(const int index_,const QString text_)
{
    switch(index_)
    {
        case 0:
        {
            ui->toolButton_func_1->setText(text_);
            connect(ui->toolButton_func_1,&QToolButton::clicked,this,[=](){
                emit this->sender_1function_();
                close();
            });
            break;
        }
        case 1:
        {
            ui->toolButton_func_2->setText(text_);
            connect(ui->toolButton_func_2,&QToolButton::clicked,this,[=](){
                emit this->sender_2function_();
                close();
            });
            break;
        }
        case 2:
        {
            ui->toolButton_func_3->setText(text_);
            connect(ui->toolButton_func_3,&QToolButton::clicked,this,[=](){
                emit this->sender_3function_();
                close();
            });
            break;
        }
        default: break;
    }
}

void FunctionCustomWidget::setConnectBtn(){
    connect(ui->toolButton_min,&QToolButton::clicked,this,[=](){
        this->showMinimized();
    });

    connect(ui->toolButton_max, &QToolButton::clicked, this, [=]() {
        onBtnMenuMaxClicked();
    });

    connect(ui->toolButton_close,&QToolButton::clicked,this,[=](){
        close();
    });
}
void FunctionCustomWidget::onBtnMenuMaxClicked( )
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
            this->setGeometry(desktop->screenGeometry(1));
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

void FunctionCustomWidget::InitStyle()
{
    max = false;
    this->location = this->geometry();
    mousePressed = false;

}

bool FunctionCustomWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->onBtnMenuMaxClicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void FunctionCustomWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void FunctionCustomWidget::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void FunctionCustomWidget::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

