//void on_saveoffset_clicked();
//void on_Samples_Begin_editingFinished();
#ifndef TESTING_H
#define TESTING_H

#include <QWidget>
#include <QPaintEvent>
#include <QRadioButton>
#include <QVariantList>
#include <QtWidgets/QWidget>
#include "json.h"
#include "analyticaljson.h"
#include <QMenu>
#include <QAction>
#include <QTreeWidget>
#include "customcreatsql.h" /*数据库*/
#include "crc/vld.h"
#include "replacethetesttubetray.h"
#include "usbcodedispose.h"
#include "progressbar.h"
#include "tubemappintpostable.h"  //机器坐标映射
#include "machinetaskassignment.h"
//#include "floorplanofinterface.h"
#include "canceltaskconfigure.h"  //取消任务配置
#include "structinstance.h"

enum class CleanIndexNeedle{
    CleanDoubeNeedle = 0,
    CleanBloodNeedle,
    CleanReagentNeedle
};

struct ModuleVisibility {
    bool showModule1;
    bool showModule2;
    bool showModule3;
};

#define  AREA_REAGENT   0
#define  AREA_EMPTTUBE  1
#define  AREA_BLOOD     2


#define  TUBE_INIT		0   //初始状态
#define  TUBE_CHECKED	1   //分配任务
#define  TUBE_OUTRESULT	2   //测试结果已出

#define  EMPTYTRAY_1    0
#define  EMPTYTRAY_2    1
#define  EMPTYTRAY_3    2
#define  EMPTYTRAY_4    3

enum TestTrayHole{Tray_oneHole = 0,Tray_twoHole = 60 ,Tray_threeHole = 120 ,Tray_fourHole = 180 ,Tray_totalHole = 240};
namespace Ui
{
    class Testing;
}

class Testing : public QWidget
{
    Q_OBJECT
public:
    explicit Testing(QWidget *parent = 0);
    ~Testing();

    //USB任务数据库
    static UsbCodeDispose *m_TaskDll;

    void initequipmentKind(const quint8 indextype);

    //所有样本测试完成
    void AllSampleTested();

    //通道禁用主界面显示
    void recv_NotifyChannleState(const quint8 channelIndexNum,const bool bUseing);

    /**
    * @brief _RecvBloodSuck2EmptyTube 测试中..血样加到试管
    * @param banemia                  是否是贫血
    * @param IndexTube                试管孔号
    */
    void recvBloodSuck2EmptyTube(bool banemia,quint8 IndexTube);

    //重测样本号prp高度
    void repPrpheight(const QString idnum,bool );

    void giveupSampleShowHole(QList<quint8> holeList);
    void giveupSampleChannelFlash(const bool &isChannelNormal,const quint8 indexChannel);

    void showTestChannelInfo(const quint8& channelIndex,
                    const   QString& sampleName,
                    const  quint8&reagentIndex);

protected:
    void            mousePressEvent(QMouseEvent *event); //鼠标键被按下的事件
    bool            eventFilter(QObject *obj, QEvent *e);
    virtual void    resizeEvent(QResizeEvent *event) override;

signals:
     void   SynchronizeEmptyTube(quint8);
     void   testingaddsample();  //测试中添加样本
     void   tsetfinishedbackorigin();
     void   sendRepPrpheight(const QString& idnum,bool);

private slots:
     void toggleBlinkState();

public slots:
    void sycnChangeuiTubeStatus(const QString& sample_name,
                                  quint8 anemiahole,
                                  const QList<quint8> &marktube,
                                  int index_add, int all_add_task);

    //丟了一个试管杯
    void slotThrowtesttube();

    //测高完成改变标志
    void HandleObtainPRPImage(const QString &pathImage);
    void HandleReopencvImageTubePRP(const QString &reId,const QString &pathImage);

    //绘制进度条
    void DrawChannelProgress(quint8 index ,double proportion);

    void SlotRemderbloodhole(int richhole);

    /*初始化任务界面*/
    void initTheTaskInterface();

    //空试管被夹走
    void EmptyTubeClipMoved(quint8 IndexTube);

private:

    bool handlePaintEvents(QObject *watched, QEvent *event);
    bool handleResizeEvents(QObject *watched, QEvent *event);
    bool handleMouseEvents(QObject *watched, QEvent *event);

    //取消待测样本
    void clickAnaemiaHoleCancelTestTask(QList<int> waitTestSampleId);

    void initializeMachineUI(const quint8 equipmentIndex);

    //选中改变空试管颜色
    void ChaneColorEmptyAreaTray(QString);

    //显示通道测试控件初始化
    void initControlShowChannelProgress(quint8 startChannel, QWidget * progressChannel,
                                           QWidget *ptestChannel, const QPalette &pa);

    //取消任务
    void ClickCanelTask(int posx,int posy);

    //切换试剂位置
    void ToggletheReagentPosition(int posx,int posy);

    void VectorReplaceClear(quint8 Index);

    void ComplBackColorBloodArea();

    void VectorSelectedBloodAreaTube(quint8 IndexTube ,QString);
    void CanceltaskbackTubecolor(int PPPHole, int PRPHoles, QList<quint8>);

    //更新血样孔状态颜色
    void UpdateBloodHoleColors(int State, QMap<quint8, QPoint> &MapBloodHole);

    //创建血样孔坐标
    void CreatBloodZoneAxisPos();
    void DrawBloodTopText();
    void DrawBloodHoleInnerText(const QMap<quint8, QPoint>& BloodHoleMap);

    /*绘制试剂坐标 */
    void plotReagentWellsCoordinates(quint8 totalReagents);
    void showPaintReagents();

    //初始化试管已加血样
    void InitTubeAddBlooded(int tubeIndex, quint8 reagentType, int sampleNumber);
    void CreatTrayTestTubeUiAxis(int indexTray);

    /*画测试试管显示UI位置坐标*/
    void DrawTrayTestTubeUiAxis(QWidget* pTrayWidget,
                                   quint8 IndexTray,
                                   quint8 BigRadius,
                                   quint8 SmalleRadius);

    void showCleaningbit();
    void showDiscardTheCup();/*弃杯孔*/
    void showCleanreagent(); /*清洗剂*/

public:
    //初始化试杯盘
    void init_testtube_tray(const int);
     //bool WanttocloseSoftware(Q);
    //开始测试显示进度
    void TotalTaskProgress();
    void _showaddsamplewidget();

    //初始化试管已被使用
    void InitUIEmptyTubeused(int testTubeIndex);

    //样本在测试颜色变为init
    void SampleTestingChangInitColor(QPoint,quint8);

    //试管被分配任务 标记
    void EmptyTubeAssigned(quint8 IndexTube);

    void replacEmptyTestTary(int index_tary);

    //性能检测&&质控  标记试管
    void tubeMarked(const QString& outtext,const quint8& tubeIndex);

public slots:
    //设置通道进度条0显示等待
    void updateChannelProgressAndStatus(bool isWaitstate, quint8 index_Chn, const QString &status);

    void slot_sycn_SampleTestingChangInitColor(QPoint maphole,quint8 indexChn);

private:
    Ui::Testing *ui;
    mutable QMutex m_dataMutex;
    QMap<quint8, QPoint> m_BloodHoleInitUiAxis;       //血样孔试管容器 init
    QMap<quint8, QPoint> m_Blood_Tray_Checked;
    QMap<quint8, QPoint> m_Blood_Tray_OutResult;

    QMap<quint8, QPointF> m_Testcups;                   //试管数量总数
    QMap<quint8, QString> mEmptyText;                   //空试管样本号文字
    QMap<quint8, QPointF> mEmptyTubeAssigned;          //空试管区试管分配血样
    QMap<quint8, QPointF> mEmptyTubeAbsorb_Poorblood;  //空试管区加了血样
    QMap<quint8, QPointF> mEmptyTubeAbsorb_Richblood;  //空试管区加了血样
    QMap<quint8, QPointF> mEmptyTubeClipMoved;         //空试管区试管被夹走 -- 开机初始化

    QList<quint8> mflashingTubeList; //闪烁的试管号

    bool m_opendedheight = false;

    QMap<quint8,QPointF> m_CleanPos; //清洗位

    QMap<quint8,QPoint>  m_Reagent_Tray;//试剂位
    
    QPointF m_CleanningReagent;             //清洗试剂
    QPointF m_DiscardCupHole;               //弃杯孔
    const int Top_Bottom_space = 30;        //与上下间距--试剂
    const int Top_Bottom_blood = 16;        //血样盘
    const int Big_radius = 16;              //大圆半径(血样孔，试管孔，试剂孔，清洗位)18
    const int small_radius = 13;            //小圆半径15
    const int TopDraw_Bigradius = 36;       //大半径 （清洗剂，测试位，弃杯孔）
    const int TopDraw_Smallradius = 32;



    //QList<quint16> Select_tubehole;
    QMap<quint8,QPointF> m_Test_Tray_has_hole;		//任务已分配的试管孔
    QMap<quint8,QPointF> m_Test_tray_useded ;		//试管盘已经用的

     /*仪器类型*/
    quint8  minstrumentType;
   
	bool mInituiBloodArea;
	bool mInitEmptyArea[4];


    //空试管区绘制圆环大小
    quint8 m_BigRadius;
    quint8 m_SmalleRadius;

    //血样绘制圆大小
    QMap<quint8, QString> m_BloodHoleNum; //血样孔编号数字
    quint8 m_BloodBigRadius ;
    quint8 m_BloodSmallRadius;


	bool alreadyinitchannelui;

    //通道显示进度
    QList<ProgressBar *>     m_channelShowsTheProgress;
    QList<QPointer<QLabel>>  Channelreminder;

	QMap<int, QString> m_notassignment;
	int mReminderTube;  //提示要放的孔号

    int m_ProTotalTube;
    int m_ThrowTube; //丟掉试管

    QMap<QString,quint8> m_TestingSample; //在测试的样本

    QTimer *mtimer = nullptr;
    bool m_blinkState;
};

#endif // TESTING_H
