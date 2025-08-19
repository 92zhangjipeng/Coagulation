#pragma execution_character_set("utf-8")
#include "alarm.h"
#include "ui_alarm.h"
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QSound>
#include <QDateTime>
#include <QDir>
#include "QsLog/include/QsLog.h"
#include "quiutils.h"
#include "operclass/fullyautomatedplatelets.h"
#include <mainwindow.h>

#define TITLE_BACK_COLOR  QColor(135, 206, 250)


bool Alarm::m_SoundAlarm = true;

Alarm::Alarm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Alarm)
{

    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint| Qt::WindowDoesNotAcceptFocus);
    setWindowTitle("仪器消息");
    setWindowOpacity(0.9);
    //setAttribute(Qt::WA_TranslucentBackground);


    this->setFixedSize(OUT_UI_SIZE_WIDTH,OUT_UI_SIZE_HEIGHT);

    QPalette pe;
    pe.setColor(QPalette::WindowText,QColor(0 ,0 ,0));
    ui->titleicon->setPalette(pe);
    ui->titleicon->setText(tr("仪器警报消息"));

    ui->label_puticon->setFixedSize(32,32);
    QPixmap *pixmap = new QPixmap(":/Picture/icon_title/logshow.png");
    pixmap->scaled(ui->label_puticon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_puticon->setScaledContents(true);
    ui->label_puticon->setPixmap(*pixmap);
    delete pixmap;
    pixmap = nullptr;


    ui->makesureAlarm->setText(tr("确认报警"));
    m_play = new QMediaPlaylist(this);
    m_playSound = new QMediaPlayer(this);
    QIcon horn(":/Picture/SetPng/Horn.png");
    QIcon dishorn(":/Picture/SetPng/DisHorn.png");
    pAction =  new QAction(this);
    DisAction = new QAction(this);
    pAction->setIcon(horn);
    DisAction->setIcon(dishorn);
    ui->Horn->setDefaultAction(pAction);
    if (m_play != nullptr && m_playSound != nullptr)
    {
        m_play->addMedia(QUrl("qrc:/Picture/SetPng/warm.wav"));
        m_play->setCurrentIndex(0);
        m_play->setPlaybackMode(QMediaPlaylist::Loop);
        m_playSound->setPlaylist(m_play);
    }
    ui->textBrowser->document()->setMaximumBlockCount(500);
}

Alarm::~Alarm()
{
    delete ui;
    if(pAction)
    {
        delete pAction;
        pAction = NULL;
    }
    if(DisAction)
    {
       delete DisAction;
       DisAction = NULL;
    }
    if (m_play){
        delete	m_play;
        m_play = NULL;
    }
    if (m_playSound)
    {
        delete m_playSound;
        m_playSound = NULL;
    }
}

void Alarm::mousePressEvent(QMouseEvent *event)
{
    //读取坐鼠标点击坐标点
    m_lastPoint = event->globalPos();
}

//鼠标移动事件
void Alarm::mouseMoveEvent(QMouseEvent *event)
{
    //把移动的点记录下来
    int dx = event->globalX() - m_lastPoint.x();

    int dy = event->globalY() - m_lastPoint.y();

    m_lastPoint = event->globalPos(); //更新记录点

    move(x() + dx, y() + dy); //窗口移动到此处
}
void Alarm::mouseReleaseEvent(QMouseEvent *event)
{
    //记录移动到的坐标
    int dx = event->globalX() - m_lastPoint.x();

    int dy = event->globalY() - m_lastPoint.y();

    move(x() + dx, y() + dy);
}
void Alarm::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QColor colorBackGround = TITLE_BACK_COLOR;

    p.setRenderHint(QPainter::Antialiasing);//抗锯齿

    p.setBrush(colorBackGround);

    p.setPen(Qt::NoPen);

    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}

void Alarm::on_Close_clicked()
{
    close();
}

void Alarm::on_Horn_clicked()
{
    if(Alarm::m_SoundAlarm == true) // 警报开着的
    {
        Alarm::m_SoundAlarm = false;
        ui->Horn->setDefaultAction(DisAction);
        m_playSound->stop();
    }
    else
    {
        Alarm::m_SoundAlarm = true;
        ui->Horn->setDefaultAction(pAction);
        m_playSound->play();
    }
    return;
}

void Alarm::on_makesureAlarm_clicked()
{
    QLOG_DEBUG()<<("确认报警");
	m_playSound->stop();
    close();
}

//测试中系统提示
void Alarm::handleReminderText(quint8 reminderKind,const QString outtext) {

    QDateTime current_time = QDateTime::currentDateTime();
    if(reminderKind == TESTINGLOG) {
        QString current_date = "[" + current_time.toString("hh:mm:ss") + "测试系统]";
        appendLog(current_date, outtext, "#363636");
    }
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void Alarm::appendLog(const QString &reminderHead, const QString &detailedDescription, const QString &color) {
    QString logMessage = QString("<font color=\"%1\">%2%3</font>").arg(color).arg(reminderHead).arg(detailedDescription);
    mutex.lock();
    ui->textBrowser->append(logMessage);
    mutex.unlock();
    logToFile(logMessage);
}

void Alarm::logToFile(const QString &logMessage) {
    static QMutex fileMutex;
    fileMutex.lock();
    QFile file("alarm_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << logMessage << "\n";
        file.close();
    }
    fileMutex.unlock();
}

void Alarm::InsertText(quint8 ReminderKind,const QString detailedDescription)
{
    QDateTime current_time = QDateTime::currentDateTime();
    QString current_date = current_time.toString("hh:mm:ss");
    QString Reminder_Head;

    switch(ReminderKind)
    {
        case NORMALLOG:
            Reminder_Head = tr("[%1]\r").arg(current_date);
            appendLog(Reminder_Head, detailedDescription, "#FFFFFF"); //正常 白色
            emit AlarmIconState(NORMALLOG);
        break;
        case PROMPTLOG:
            Reminder_Head = tr("[%1提醒]\r").arg(current_date);
            appendLog(Reminder_Head, detailedDescription, "#FFFF00"); //提示 黄色
            emit AlarmIconState(NORMALLOG);
        break;
        case ERRORLOG:
            Reminder_Head = tr("[%1异常]\r").arg(current_date);
            appendLog(Reminder_Head, detailedDescription, "#FF3030"); //异常 红色
            emit AlarmIconState(ERRORLOG);
        break;
        case USERACTIONS:
            Reminder_Head = tr("[%1操作]\r").arg(current_date);
            appendLog(Reminder_Head, detailedDescription, "#4169E1"); //操作 蓝色
            emit AlarmIconState(NORMALLOG);
        break;
        case MOTHERBOARD_ERR:    break;
        case TESTINGLOG:
            Reminder_Head = tr("[%1测试系统]\r").arg(current_date);
            appendLog(Reminder_Head, detailedDescription, "#FFFFFF");  //白色
            emit AlarmIconState(NORMALLOG);
        break;
        default:
            break;
    }
    ui->textBrowser->moveCursor(QTextCursor::End);
    return;
}


void Alarm::OnOffSound(bool open)//警报的开关
{
    if(open == true)
    {
        m_playSound->play();
        ui->Horn->setDefaultAction(pAction);
    }
    else
    {
        m_playSound->stop();
        ui->Horn->setDefaultAction(DisAction);
    }
    return;
}
