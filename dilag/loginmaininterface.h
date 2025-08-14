#ifndef LOGINMAININTERFACE_H
#define LOGINMAININTERFACE_H

#include <QDialog>
#include <commandexceptional.h>
#include <loadequipmentpos.h>
#include <QAtomicInt>
#include <memory>
#include "mybordercontainer.h"
//#include  <dilag/equipmentmainwidget.h>


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
    void keyPressEvent(QKeyEvent *event) override ;

private:
    void setConnectBtn();
    void InitStyle();


    void initTitleBar();
    void initPasswordField();
    void asyncInitDatabase();
    void initHardware();


    void _loaduser();

private slots:
    void onBtnMenuCloseClicked();
    void onBtnMenuMaxClicked();
    void onBtnMenuMinClicked();
    void on_toolButton_enter_clicked();
    void on_toolButton_exit_clicked();

signals:
    void sig_start();

    //配置机型坐标 写坐标
    void makesureequipment(const quint8 & ,bool,QString);

    void sycnParaConfigFileSatte(bool,QString);

public slots:

   void ToReadtEquipmentTypePos(quint8 kindType, QString saveTimes); //读仪器坐标

   void slotProgressshow(bool bWrite); //返回读写进度

   void slotsetEquipmentIndex();

   void CreatReminderWidget(char index, QString titleStr, QString reminderStr); //创建提示框

   void slotclosetimercon(bool _conned);

   void closeReminder();        //析构对话框


private:
    Ui::loginmaininterface *ui;

    MyBorderContainer *m_myborder;

    loadEquipmentPos *mLoadcoordinates = nullptr; //加载仪器坐标

    int mtotalcommed;
    QAtomicInt  m_gotcompleted;

    CommandExceptional *m_pReminderExceptional = nullptr;  //提示框

    int mtimerconnect; //定时连接




    QString _parasettingPath;
    bool m_bparaexit;

	bool m_TimerRunning; //定时器运行标志

    QPoint testTubeZoneoffsetHands[4];
    QPoint testTubeZoneoffsetBloodPin[4];
    bool mAxisFinished;

    bool max;
    bool mousePressed;
    QPoint mousePoint;
    QRect location;


    //EquipmentMainWidget* ptest = nullptr;

};

#endif // LOGINMAININTERFACE_H
