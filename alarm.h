#ifndef ALARM_H
#define ALARM_H
#include "json.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVariant>
#include <QWidget>
#include "analyticaljson.h"
#include "cglobal.h"
#include <QMutex>


#define OUT_UI_SIZE_WIDTH  650
#define OUT_UI_SIZE_HEIGHT 400


namespace Ui {
class Alarm;
}

class Alarm : public QWidget
{
    Q_OBJECT

public:
    explicit Alarm(QWidget *parent = 0);
    ~Alarm();

signals:
    void AlarmIconState(quint8 );

protected:
    //鼠标点击事件
    void mousePressEvent(QMouseEvent *event);
    //鼠标移动事件
    void mouseMoveEvent(QMouseEvent *event);
    //鼠标释放事件
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *);

private slots:
    void on_Close_clicked();

    void on_Horn_clicked();

    void on_makesureAlarm_clicked();

public slots:

    void OnOffSound(bool open);

    //测试流程提示信息
    void handleReminderText(quint8 reminderKind,const QString outtext);


    void InsertText(quint8 ReminderKind, const QString);


private:
    void appendLog(const QString &reminderHead, const QString &detailedDescription, const QString &color);
    void logToFile(const QString &logMessage);
    QMutex mutex;
private:
    Ui::Alarm *ui;

    QPoint m_lastPoint;

    static bool m_SoundAlarm; //报警声

    QMediaPlayer  *m_playSound = nullptr;

    QMediaPlaylist *m_play = nullptr;

    AnalyticalJson m_Analytical;

    QVariant m_AnalyticalOneData;//解析出的一个数据

    QAction *pAction = nullptr;

    QAction *DisAction = nullptr;

    QStringList m_hadReminder;
};

#endif // ALARM_H
