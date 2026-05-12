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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include "opencvFindRBC/customtitlebar.h"

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
    void handleReminderText(quint8 reminderKind, const QString outtext);
    void InsertText(quint8 ReminderKind, const QString);

protected:
    void paintEvent(QPaintEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();
    void onHornClicked();
    void onConfirmClicked();

private:
    void initUI();
    void initStyle();
    void setupMediaPlayer();
    void appendLog(const QString &reminderHead, const QString &detailedDescription, const QString &color);
    void logToFile(const QString &logMessage);

    Ui::Alarm *ui;
    CustomTitleBar *m_titleBar;

    // 主布局
    QVBoxLayout *m_mainLayout;
    QWidget *m_centerWidget;
    QVBoxLayout *m_centerLayout;

    // 控件
    QLabel *m_iconLabel;
    QTextBrowser *m_textBrowser;
    QPushButton *m_hornBtn;
    QPushButton *m_confirmBtn;

    QMutex m_textMutex;
    QMutex m_fileMutex;

    QScopedPointer<QMediaPlayer> m_playSound;
    QScopedPointer<QMediaPlaylist> m_play;

    static bool m_SoundAlarm;
    bool m_isMaximized;
    QRect m_normalGeometry;

    AnalyticalJson m_Analytical;
    QVariant m_AnalyticalOneData;

    QAction *pAction = nullptr;
    QAction *DisAction = nullptr;
    QStringList m_hadReminder;
};

#endif // ALARM_H
