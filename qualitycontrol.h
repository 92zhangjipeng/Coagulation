#ifndef QUALITYCONTROL_H
#define QUALITYCONTROL_H

#include <QLabel>
#include <QPushbutton>
#include <QWidget>
#include "customplot.h"
#include "progressbar.h"
#include "ini_file.h"
#include "cglobal.h"
#include "QSimpleLed.h"
#include "loadequipmentpos.h"
#include <QTableWidget>
#include "dilag/tipcustomwidget.h"
#include <custom_style/custombutton.h>
#include <QMutexLocker>


typedef struct {
    quint8 index;
    quint8 type;
} ReagentMapping;

enum CleaningFluidDepleteType {
	REAGENT_PIN = 0, BLOOD_PIN = 1, BOTH_PINS = 2
};

namespace Ui {
class QualityControl;
}

class QualityControl : public QWidget
{
    Q_OBJECT

public:
    explicit QualityControl(QWidget *parent = 0);
    ~QualityControl();

    void initSupplies(); //更换试管按钮

    void Capacity_display_init(QTableWidget *_ptablewidget); //容量初始化

    void initializeTheTube(const quint8 instrumentType);

    void  initshowcapacity();  //显示试剂&&清洗液百分比   /*点击耗材按钮进入*/

    void _configwarmvalue(quint8 index_,quint8 _limitarm);

    void CreatReplaceRemindWidget(const QString Title,const QString ReminderText,const int ReplaceindexSuppiles);


    /**
    * @brief LosserOneReagentul  测试试剂减少
    * @param index_reag          试剂种类
    * @return
    */
    void  LosserOneReagentul(const quint8 index_reag, const quint8 kind_deplete);

    void  displayConsumablesInteger(); //显示耗材剩余整体

    void  _ShowConsumablesLimitArm(); //显示耗材报警线

    void  loc_clean_linque_display();  //本地清洗液显示比例

    void  loc_reagnet_type_display_val(quint8 index_reag); //显示本地试剂余量

    void  TotalConsumablesAlarm(); //总耗材报警


protected:
    bool    eventFilter(QObject *watched, QEvent *event);
    virtual void    resizeEvent(QResizeEvent *event) override;


signals:
    //发送信号同步主界面耗材显示
    void    SycnMainUiLosserSuppile(int , double);

    void    sendDirectives(const QByteArray ,QString);

    void    NoinitialCleaning();//开机清洗液不足未初始清洗
    void    consumablesLackPauses(const quint8 );

    //void    _writemainboardloss(const QByteArray ,quint8); //试管耗材..减少写入主板

public slots :
    void  table_itemRressed(QTableWidgetItem *ptablepressed);//显示试剂批号

    //修改耗材的整体警报限
    void  tableitemNotify(QTableWidgetItem *changedItem);

    void  slotSynchronizeEmptyTube(quint8 indextube);

    //读耗材卡确认充值
    void  sltoAddConsumables(int, unsigned short);

    //充值成功设置界面
    void  handleRechargeSuccessful(int types, quint8 setdata);

    //更换耗材试管
    void  makesureReplace(quint8 index);

    //外部清洗液报警减少一
    void handleoutSideCleanDepleteOne();


private slots:

    void slot_Timerout();

private:
    /**  填充耗材表
     * @brief FilltheConsumablesTable
     * @param ConsumablesTable
     * @param rows
     * @param cols
     * @param itemdata
     * @param enable
     */
    void FilltheConsumablesTable(QTableWidget *ConsumablesTable,int rows,int cols,QString itemdata,bool enable);
    void FilltheConsumablesTable(QTableWidget *ConsumablesTable,int rows,int cols,QString itemdata,QColor textColor,bool enable);

    void _TableLinqueLess(const quint8 index_,const quint8 total_);//液体耗材减少更新表格

    //提示试管盘有已加样任务不能更换
    bool isHadAddSampleTube(quint8 indextray);

    void TrayHadFreetube(quint8 indextray, bool &hadtube);   //试管盘试管有未用

    void  _initreagbottle(int w_,int h_); //初始化试剂瓶

    void setReagentPara(quint8 index_,bool bsetAlarm,double valuedata); //设置值

    QString mapoutreagent(quint8 index_reag, double lastratio); //输出映射耗材名&&剩余%

    void _outputtips(int pressedcol,QPoint itempos_);

    bool  RemindersuppliesEnouth(const int Typesupplies); //提示耗材不足

    bool  ReaminderUsbNotConnect();

    void  ReminderReplceTestTube(quint8 indexTray); //更换试管盘提示框

    void  updateTestTubeTrat(quint8 replaceIndexTray);  //更换试管盘更新界面

    void  updateotherSupplies(const quint8 indexSupplies); //确定更换除试管外其它耗材

    void  gridLayoutTubeTray(QVector<QSimpleLed *> tubeVector , QWidget *container, int trayId); //初始化试管布局

    //整个试管盘更换  true: OFF false:ON
    void  updatetrayAllTube(const QVector<QSimpleLed *>& ptubeTrayVec, const bool bused);


    void  ReagentIndexMapReagentStr(quint8 indexReagent,QString& outPutText);

    void updateCleaningFluidStatus(ConsumablesOper* consumables, double lastRatioPercent,
                                            quint16 updateratio);

public:
    void recv_updateTrayUsed(int IndexTray); //试管盘被拿起

    //清洗液消耗
    //void consumeCleaningFluid(const quint8 kind_deplete);
    void consumeCleaningFluid(const CleaningFluidDepleteType depleteType);

    //消耗试剂量
    void consumeReagent(const quint8 reag_kind, const quint8 index_);

private:
    Ui::QualityControl *ui;
    QMutex m_mutex;  // 互斥锁成员

    static const QMap<quint8, QString> COLUMN_CONFIG_MAP;
    static const QMap<int,quint8> ITEM_REAGENT_MAP; //试剂映射表格列
    static constexpr quint8 MAX_CONSUMABLE_VALUE = 255;  // 最大存储值

    QString mpathfile; //耗材配置文件

    QList<QSimpleLed* > m_testtubeHole;
    QSimpleLed* m_pdelegatesTube = nullptr;

    QVector <QSimpleLed *> m_tubeStateVec_1;   //试管状态1
    QVector <QSimpleLed *> m_tubeStateVec_2;   //试管状态2
    QVector <QSimpleLed *> m_tubeStateVec_3;   //试管状态3
    QVector <QSimpleLed *> m_tubeStateVec_4;   //试管状态4

    quint8 m_replaceindexTray = 0;

    QMap<quint8,customButton*> m_pReagentLinque;

    QList<QString> m_reminderTitleStr;

    enum TableIndex{PROJECT_ = 0,AA_ ,ADP_,EPI_,COL_,RIS_,
                    S1_,S2_,TESTCUPS_,TOTALITEM};

    tipcustomwidget *m_tipswidget = nullptr;

    enum TableRow{preunit_= 0, Allowance_,LimitArm,LimitAllSuppile};

    QString InitTalbe_css = "QTableWidget::item:hover{background-color:rgb(70 ,130 ,180)}"
                            "QTableWidget::item:selected{background-color:rgb(139, 139, 122)}"

                            "QTableView QTableCornerButton::section{color: white; background-color: rgb(188, 187, 186); "
                            "border: 1px solid rgb(188, 187, 186);border-radius:1px; border-color: rgb(188, 187, 186);"
                            "font: bold 1pt;padding:12px 0 0 10px}"

                            "QHeaderView::section,QTableCornerButton:section{ \
                            padding:3px; margin:0px; color:rgba(188, 187, 186, 255);  border:1px solid rgba(188, 187, 186, 255); \
                            border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                            background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                            "QTableWidget{background-color:white;border: 1px;}"
                            "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                            stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}"
                            "QTableView QTableCornerButton::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                            stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}";


};

#endif // QUALITYCONTROL_H
