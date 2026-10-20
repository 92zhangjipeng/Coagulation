#ifndef LOGINMAININTERFACE_H
#define LOGINMAININTERFACE_H

#include <QDialog>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QProgressBar>
#include <QRegularExpressionValidator>
#include <QAtomicInt>
#include <memory>

#include <commandexceptional.h>
#include <loadequipmentpos.h>
#include "mybordercontainer.h"
#include "opencvFindRBC/customtitlebar.h"  // 新增：引入自定义标题栏

namespace Ui {
class loginmaininterface;
}

class loginmaininterface : public QDialog
{
    Q_OBJECT

public:
    explicit loginmaininterface(QWidget *parent = 0);
    ~loginmaininterface();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);
    void timerEvent(QTimerEvent *event);
    void keyPressEvent(QKeyEvent *event) override;

private:
    void InitStyle();
    void initConnections();      // 新增：连接标题栏信号
    void initPasswordField();
    void asyncInitDatabase();
    void initHardware();
    void loaduser();

    void setupConfigEquipmentReminder(const QString& reminderStr);
    void setupToolReminder(const QString& reminderStr);
    void setupDefaultReminder(const QString& reminderStr);

    void closeTimerSerial();
    void wirteMachineParaProgress();
    void readMachineParaFinished();

    // UI创建函数
    QFrame* createMainFrame();
    QToolButton* createActionButton(const QString& iconPath, const QString& text, const QString& objectName);

private slots:
    void on_toolButton_enter_clicked();
    void on_toolButton_exit_clicked();

    // 标题栏槽函数
    void onTitleBarCloseRequested();
    void onTitleBarMinimizeRequested();
    void onTitleBarMaximizeRequested();

signals:
    void signalStart();
    void configuredModel(const quint8 &, bool, QString);
    void sycnParaConfigFileSatte(bool, QString);

public slots:
    void ToReadtEquipmentTypePos(quint8 kindType, QString saveTimes);
    void slotProgressshow(bool bWrite);
    void slotsetEquipmentIndex();
    void CreatReminderWidget(char index, const QString& titleStr, const QString& reminderStr);
    void slotclosetimercon(bool _conned);
    void closeReminder();

private:
    Ui::loginmaininterface *ui;
    MyBorderContainer *m_myborder;
    loadEquipmentPos *mLoadcoordinates = nullptr;

    // 自定义标题栏
    CustomTitleBar *m_titleBar = nullptr;

    int mtotalcommed;
    QAtomicInt m_gotcompleted;
    CommandExceptional *m_pReminderExceptional = nullptr;
    int mtimerconnect;
    QString _parasettingPath;
    bool m_bparaexit;
    bool m_TimerRunning;

    QPoint testTubeZoneoffsetHands[4];
    QPoint testTubeZoneoffsetBloodPin[4];
    bool mAxisFinished;

    bool max;
    bool mousePressed;
    QPoint mousePoint;
    QRect location;

    // UI控件指针
    QLabel *m_label_sysname = nullptr;
    QLabel *m_label_version_number = nullptr;
    QComboBox *m_comboBox_user = nullptr;
    QLineEdit *m_lineEdit_password = nullptr;
    QToolButton *m_toolButton_enter = nullptr;
    QToolButton *m_toolButton_exit = nullptr;
    QLabel *m_label_equipmentkind = nullptr;
    QLabel *m_label_reminder = nullptr;
    QProgressBar *m_progressBar_readAxis = nullptr;
};

#endif // LOGINMAININTERFACE_H
