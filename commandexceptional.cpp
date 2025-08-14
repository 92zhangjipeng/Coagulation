#include "commandexceptional.h"
#include "ui_commandexceptional.h"
#include "quiutils.h"
#include "cglobal.h"
CommandExceptional::CommandExceptional(char index,QString InfoRemid,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CommandExceptional)
{
    ui->setupUi(this);
    bgmcColor.setRgb(139, 137, 137);
    ui->label_ErrInfo->setWordWrap(true); //自动换行
    setWindowFlags(Qt::Tool |
                   Qt::FramelessWindowHint |
                   Qt::WindowStaysOnTopHint);
    //setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    mFucn = index;  // 0:未联机提示 1:配置仪器类型  2: 命令异常提示 3:耗材报警提示 4:同步本地坐标文件  5: 提示关闭软件  6:更换试管 7:更缓试剂耗材
    //QUIUtils::QLabeldisplayIcon(ui->label_titlename,":/Picture/reminderalarm.png",InfoRemid,5);

    QPixmap *pixmap = new QPixmap(":/Picture/reminderalarm.png");
    pixmap->scaled(ui->labelimage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->labelimage->setScaledContents(true);
    ui->labelimage->setPixmap(*pixmap);
    ui->label_titlename->setText(InfoRemid);
    delete pixmap;
    pixmap = nullptr;
    if(mFucn == 1)
    {
       ui->label_ErrInfo->hide();
       ui->widgetBtn->hide();
       //ui->toolButton_ok->hide();
       //ui->toolButton_Cancel->hide();
	   ui->widgetConfigEquipment->show();
    }
    else if(mFucn == 5 || mFucn == 6 || mFucn == 7)
    {
        ui->widgetConfigEquipment->hide();
    }
    else
    {
        ui->widgetConfigEquipment->hide();
        ui->toolButton_Cancel->hide();
    }
    if(parent)
    {
        QRect rect = parent->geometry();
        //计算显示原点
        int x = rect.x() + rect.width()/2 - this->width() /2;
        int y = rect.y() + rect.height()/2 - this->height()/2;
        this->move(x, y);
    }

}

CommandExceptional::~CommandExceptional()
{
    delete ui;
}
void CommandExceptional::mousePressEvent(QMouseEvent *event)
{
    this->windowsPos = this->pos();       // 获得部件当前位置
    this->mousePos = event->globalPos(); // 获得鼠标位置
    this->dPos = mousePos - windowsPos;   // 移动后部件所在的位置
}

void CommandExceptional::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

void CommandExceptional::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = bgmcColor;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}
void CommandExceptional::closeEvent(QCloseEvent *event)
{
    event->accept();
    close();
}
void CommandExceptional::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return)
    {
        on_toolButton_close_clicked();
    }
}
void CommandExceptional::setErrInfo(const quint8 ErrNum,const QString errInfo)
{
    m_indexErr = ErrNum;
    ui->label_ErrInfo->setText(errInfo);
    update();
}

void CommandExceptional::replaceIndexSupplies(const quint8 index,const QString Info)
{
    mindexSulip = index;
    ui->label_ErrInfo->setText(Info);
    update();
}

void CommandExceptional::replaceReagents(const quint8 indexReag,const QString Info)
{
    m_indexReag = indexReag;
    ui->label_ErrInfo->setText(Info);
    update();
}

void CommandExceptional::on_toolButton_ok_clicked()
{
    if(mFucn == 2)
    {
        emit ReplaceSend(m_indexErr); //命令异常提示
    }
    else if(mFucn == 5)
    {
        emit clossuoweiApp(); //有 确定取消是  确定发送的消息
    }
    else if(mFucn == 6)
    {
        emit replaceIndexSulipp(mindexSulip);
    }
    else if(mFucn == 7)
    {
         emit this->updateRagentsKinds(m_indexReag);
    }
    emit closeDel();
    //close();
}

void CommandExceptional::on_toolButton_close_clicked()
{
    if(mFucn == 2)
    {
       emit ReplaceSend(m_indexErr);
    }
    emit closeDel();
}

//配置KS600
void CommandExceptional::on_pushButton_KS600_clicked()
{
    emit this->configEquipType(KS600);
}

void CommandExceptional::on_pushButton_KS800_clicked()
{
    emit this->configEquipType(KS800);
}

void CommandExceptional::on_pushButton_KS1200_clicked()
{
    emit this->configEquipType(KS1200);
}

void CommandExceptional::on_toolButton_Cancel_clicked()
{
     emit closeDel();
}
