#ifndef LOGINUI_H
#define LOGINUI_H
#include <QColor>
#include <qDebug>
#include <QMessageBox>
#include <QMainWindow>
#include <mainwindow.h>
#include "loadequipmentpos.h"
#include "cglobal.h"


class QPainter;
class QTimerEvent;

namespace Ui
{
    class LoginUi;
}

class LoginUi : public QWidget
{
    Q_OBJECT
public:
    explicit LoginUi(QWidget *parent = 0);
    ~LoginUi();
protected:
    //这是一个虚函数，从QEvent继承而来.
    void timerEvent(QTimerEvent*event);
    void paintEvent(QPaintEvent *);
signals:
    void sig_start();
    void writedataToEquip(const QByteArray arry);
    void axisInitComplete(const int ); //初始化坐标完成发送信号缓存
    void start_ReadAxispos();   //读取到仪器类型开始读取坐标
private slots:
    void on_toolClose_clicked();
    void on_pushButtonCancel_clicked();
    void on_pushButtonLogin_clicked();
    void ShowIt();
	void Deleetmain();
    /*可视化密码*/
    void VisualizationPassword();
    void on_comboBoxUser_currentTextChanged(const QString &);
    void on_pushButton_show_clicked();
public slots:
   void slotEquipmentType(bool bhadTyped,quint8 kindType, QString saveTimes);

   void slotBackAxisProg(bool bwritr,quint8 Num);

   void slotsetEquipmentIndex();



   void slotShowProg(bool bshow);

   void CreatReminderWidget(char index, QString titleStr, QString reminderStr); //创建提示框

   void closeReminder();        //析构对话框


protected:
    void mousePressEvent(QMouseEvent *event);//鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event); //鼠标移动事件
    bool eventFilter(QObject *obj, QEvent *event);
    void keyPressEvent(QKeyEvent *event);
private:
    void initEquipmentAxix(const quint8 indexType);
	void enterMainWidget();
    void creatTestChnAixisPos(int indexZ, int equipType, QMap<quint8, QPoint> &ChnAxispos);

    //把读取到的仪器坐标写到本地内存
    void configLocPosdata();

private:
    Ui::LoginUi *ui;

    CommandExceptional *m_pReminderExceptional = nullptr;
    loadEquipmentPos *mpLoadPos = nullptr;



    int mtimerconnect;

	QPoint testTubeZoneoffsetHands[4];
	QPoint testTubeZoneoffsetBloodPin[4];
	quint8 mtotalcommed;
    bool mAxisFinished;


    MainWindow *m_pmainControlApp = nullptr;
    QPoint mousePos;
    QPoint windowsPos;
    QPoint dPos;
    bool mViewPassword;
public:
    void Init_login(void );//初始化登录界面
};

#endif // LOGINUI_H
