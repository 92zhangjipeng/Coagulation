#pragma execution_character_set("utf-8")

#include "alarm.h"
#include "ui_alarm.h"
#include "quiutils.h"
#include "operclass/fullyautomatedplatelets.h"
#include <mainwindow.h>
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include "QsLog/include/QsLog.h"

#define TITLE_BACK_COLOR  QColor(135, 206, 250)

bool Alarm::m_SoundAlarm = true;

Alarm::Alarm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Alarm)
{

    ui->setupUi(this);
    setupUI();
    setupMediaPlayer();
}

Alarm::~Alarm()
{
    delete ui;

    if(pAction){
        delete pAction;
        pAction = nullptr;
    }

    if(DisAction){
       delete DisAction;
       DisAction = nullptr;
    }

}

void Alarm::setupUI()
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    setWindowTitle(tr("仪器消息"));
    setWindowOpacity(0.9);
    setFixedSize(OUT_UI_SIZE_WIDTH, OUT_UI_SIZE_HEIGHT);

    QPalette pe;
    pe.setColor(QPalette::WindowText, QColor(0, 0, 0));
    ui->titleicon->setPalette(pe);
    ui->titleicon->setText(tr("仪器通用信息"));

    ui->label_puticon->setFixedSize(32, 32);
    QPixmap pixmap(":/Picture/icon_title/logshow.png");
    pixmap = pixmap.scaled(ui->label_puticon->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->label_puticon->setScaledContents(true);
    ui->label_puticon->setPixmap(pixmap);

    ui->makesureAlarm->setText(tr("确认信息"));

    QIcon horn(":/Picture/SetPng/Horn.png");
    QIcon dishorn(":/Picture/SetPng/DisHorn.png");
    pAction = new QAction(horn, "", this);
	DisAction = new QAction(dishorn, "", this);
    ui->Horn->setDefaultAction(pAction);

    ui->textBrowser->document()->setMaximumBlockCount(500);
}

void Alarm::setupMediaPlayer()
{
    m_play.reset(new QMediaPlaylist);
    m_playSound.reset(new QMediaPlayer);

    if (m_play && m_playSound) {
        m_play->addMedia(QUrl("qrc:/Picture/SetPng/warm.wav"));
        m_play->setCurrentIndex(0);
        m_play->setPlaybackMode(QMediaPlaylist::Loop);
        m_playSound->setPlaylist(m_play.data());

        if (m_SoundAlarm) {
            m_playSound->play();
        }
    }
}



void Alarm::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_lastPoint = event->globalPos();
    }
    QWidget::mousePressEvent(event);
}

//鼠标移动事件
void Alarm::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QPoint delta = event->globalPos() - m_lastPoint;
        move(pos() + delta);
        m_lastPoint = event->globalPos();
    }
    QWidget::mouseMoveEvent(event);
}

void Alarm::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPoint delta = event->globalPos() - m_lastPoint;
        move(pos() + delta);
    }
    QWidget::mouseReleaseEvent(event);
}

void Alarm::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(TITLE_BACK_COLOR);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 2, 2);
}

void Alarm::on_Close_clicked()
{
    close();
}

void Alarm::on_Horn_clicked()
{
    m_SoundAlarm = !m_SoundAlarm;

    if (m_SoundAlarm) {
        ui->Horn->setDefaultAction(pAction);
        if (m_playSound) {
            m_playSound->play();
        }
    } else {
        ui->Horn->setDefaultAction(DisAction);
        if (m_playSound) {
            m_playSound->stop();
        }
    }
}

void Alarm::on_makesureAlarm_clicked()
{
    QLOG_DEBUG() << "确认报警";
    if (m_playSound) {
        m_playSound->stop();
    }
    close();
}

//测试中系统提示
void Alarm::handleReminderText(quint8 reminderKind,const QString outtext) {

    if (reminderKind == TESTINGLOG) {
        QString currentDate = "[" + QDateTime::currentDateTime().toString("hh:mm:ss") + tr(" Test System]");
        appendLog(currentDate, outtext, "#363636");
    }
    ui->textBrowser->moveCursor(QTextCursor::End);
}

void Alarm::appendLog(const QString &reminderHead, const QString &detailedDescription, const QString &color) {
    QString logMessage = QString("<font color=\"%1\">%2 %3</font>")
                         .arg(color, reminderHead, detailedDescription);

    {
        QMutexLocker locker(&m_textMutex);
        ui->textBrowser->append(logMessage);
    }

    logToFile(logMessage);
}

void Alarm::logToFile(const QString &logMessage) {
    QMutexLocker locker(&m_fileMutex);

    QFile file("alarm_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " " << logMessage << "\n";
        file.close();
    }
}

void Alarm::InsertText(quint8 ReminderKind,const QString detailedDescription)
{
    QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString reminderHead;
    QString color;

    switch (ReminderKind) {
    case NORMALLOG:
        reminderHead = tr("[%1]").arg(currentTime);
        color = "#FFFFFF";
        emit AlarmIconState(NORMALLOG);
        break;
    case PROMPTLOG:
        reminderHead = tr("[%1 提示]").arg(currentTime);
        color = "#FFFF00";
        emit AlarmIconState(NORMALLOG);
        break;
    case ERRORLOG:
        reminderHead = tr("[%1 异常]").arg(currentTime);
        color = "#FF3030";
        emit AlarmIconState(ERRORLOG);
        break;
    case USERACTIONS:
        reminderHead = tr("[%1 操作]").arg(currentTime);
        color = "#4169E1";
        emit AlarmIconState(NORMALLOG);
        break;
    case TESTINGLOG:
        reminderHead = tr("[%1 测试流程]").arg(currentTime);
        color = "#FFFFFF";
        emit AlarmIconState(NORMALLOG);
        break;
    default:
        return;
    }

    appendLog(reminderHead, detailedDescription, color);
    ui->textBrowser->moveCursor(QTextCursor::End);
}


void Alarm::OnOffSound(bool open)//警报的开关
{
    m_SoundAlarm = open;

    if (m_playSound) {
        if (open) {
            m_playSound->play();
            ui->Horn->setDefaultAction(pAction);
        } else {
            m_playSound->stop();
            ui->Horn->setDefaultAction(DisAction);
        }
    }
}
