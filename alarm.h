#ifndef ALARM_H
#define ALARM_H

#include "json.h"
#include "analyticaljson.h"
#include "cglobal.h"
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVariant>
#include <QWidget>
#include <QMutex>
#include <QScopedPointer>

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
    void AlarmIconState(quint8 state);

public slots:
    void OnOffSound(bool open);
    //测试流程提示信息
    void handleReminderText(quint8 reminderKind,const QString outtext);
    void InsertText(quint8 ReminderKind, const QString);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void on_Close_clicked();
    void on_Horn_clicked();
    void on_makesureAlarm_clicked();


private:
    void appendLog(const QString &reminderHead, const QString &detailedDescription, const QString &color);
    void logToFile(const QString &logMessage);
    void setupUI();
    void setupMediaPlayer();

    QMutex m_textMutex;
    QMutex m_fileMutex;

    QScopedPointer<QMediaPlayer> m_playSound;
    QScopedPointer<QMediaPlaylist> m_play;


private:
    Ui::Alarm *ui;

    QPoint m_lastPoint;
    static bool m_SoundAlarm; //报警声

    AnalyticalJson m_Analytical;
    QVariant m_AnalyticalOneData;//解析出的一个数据

    QAction *pAction = nullptr;
    QAction *DisAction = nullptr;
    QStringList m_hadReminder;
};

#endif // ALARM_H
