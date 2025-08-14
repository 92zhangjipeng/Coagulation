#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include <QButtonGroup>
#include <QTableWidget>
#include <QWidget>
#include <QScrollBar>
#include <QSpinBox>
#include <QPair>
#include "testing.h"
#include <QStyledItemDelegate>
#include "tubecontinuedoing.h"

namespace Ui {
class CustomPlot;
}



class MyQStyledItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    MyQStyledItemDelegate(int height, QObject *parent) : QStyledItemDelegate(parent), m_Height(height) {}
    ~MyQStyledItemDelegate() {}

public:
    int m_Height ;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(m_Height);    //这里设置高度
        return size;
    }
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
        QStyledItemDelegate::paint(painter, option, index);
    }
};

enum InstrumentCoordinate{
    Origin_bloodNeedle = 0,
    CleanLinque_bloodNeedle = 1,
    Bloodsample_BloodNeedle = 2,
    EmptyTube_1_BloodNeedle = 3,
    EmptyTube_2_BloodNeedle = 4,
    EmptyTube_3_BloodNeedle = 5,
    EmptyTube_4_BloodNeedle = 6,
    CleanLinque_ReagentNeedle = 7,
    ReagentLinque_ReagentNeedle = 8,
    TestChannel_ReagentNeedle = 9,
    TestChannel_Hands =10,
    ThrowCup_Hands = 11,
    EmptyTube_1_Hands =12,
    EmptyTube_2_Hands =13,
    EmptyTube_3_Hands =14,
    EmptyTube_4_Hands =15,


};

enum NotifyHolePos{
    Empty_1_topLeft = 13,
    Empty_1_topRight = 16,
    Empty_1_bottomLeft = 43,
    Empty_1_bottomRight = 46,
    Empty_2_topLeft = 73,
    Empty_2_topRight = 76,
    Empty_2_bottomLeft = 103,
    Empty_2_bottomRight = 106,
    Empty_3_topLeft = 133,
    Empty_3_topRight = 136,
    Empty_3_bottomLeft = 163,
    Empty_3_bottomRight = 166,
    Empty_4_topLeft = 193,
    Empty_4_topRight = 196,
    Empty_4_bottomLeft = 223,
    Empty_4_bottomRight = 226,
};

enum AreasCalibration{
    origin =0 ,
    detergent,
    Reagentarea,
    testchannel,
    Discardhole,
    Testtubearea1,
    Testtubearea2,
    Testtubearea3,
    Testtubearea4,
    bloodsamplearea
};
enum IndexZ{
    Hands_z = 0,
    Reagent_z,
    Blood_z
};
enum TheGripperDrops{
   GipperDown_top = 0,
   GipperDown_mid,
   GipperDown_bottom,
   GipperDown_custom
};
enum TableIndexPos{
    IndexNum = 0,
    Instrument_xpos,
    Instrument_ypos,
    Instrument_save
};

class CustomPlot : public QWidget
{
    Q_OBJECT

public:
    explicit CustomPlot(QWidget *parent = 0);
    ~CustomPlot();

    void initstyle();
protected:
    //这是一个虚函数，继承自QEvent.只要重写了这个虚函数，当你按下窗口右上角的"×"时，就会调用你所重写的此函数.
    void closeEvent(QCloseEvent*event);
private slots:

    /** 抓手下降高度
    * @brief CheckGipperDownHigh
    */
    void  CheckGipperDownHigh(QAbstractButton*);

    /** 保存/复原 修改坐标
    * @brief BtnClickSavePoint
    */
    void  BtnClickSavePoint();


    /** 基础改变其它也改变/复原
    * @brief BtnClickOthersSavePoint
    */
    void  BtnClickOthersSavePoint();

    /**修改基础值
    * @brief UpdateBaseValue_x
    */
    void  UpdateBaseValue_x(int, QSpinBox *pbox);
    void  UpdateBaseValue_y(int);

    /** 表格添加按钮
    * @brief AddBtnSave_backValue
    */
    void AddBtnSave_backValue(int rowIndex, bool NeedChaneOther);
    void on_comboBox_calibrationAarea_activated(int index);

    void  on_toolButton_Backorigin_clicked();

    void  on_toolButton_Clean_1_clicked();
    void  on_toolButton_throwTube_clicked();
    void  on_pushButton_TrayHands_clicked();

    void on_spinBox_downValue_valueChanged(int arg1);

    void on_pushButton_TrayHands_2_clicked();

    void on_toolButton_video_clicked();

private:
    /** 初始化显示坐标表
    * @brief InitdisplayPointTablewidget
    */
    void InitdisplayPointTablewidget(QTableWidget* Tablewidgetpos);

    /*初始化血样区域孔孔号*/
    void InitBloodZoneNum(quint8 indexModels);

    /*初始化下降高度commbox*/
    void initCommboxView(quint8 indexType);

    /*初始化血样区域孔孔号*/
    void initEmptyTubeHole(quint8 indexType);

    /*初始化测试通道孔号*/
    void initTestChnHole(quint8 indexType);

    void  UserClickOriginAxis(); //点击校验原点


    /** 是否显示原点坐标、是否写X坐标、修改X坐标值
    *bFindShow ：true 显示 false 修改X坐标
    * bWrite_x ：true 修改X false 修改Y
    * notifyValue: 修改值
    */
    void displayOriginAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue);

    //弃杯孔
    void displayThrowHole(bool bFindShow,bool bWrite_x,int notifyValue);

    //清洗区坐标显示 修改
    void displayCleanLinqueAxisPoint(bool bFindShow, quint8  indexZ,bool bWrite_x, int notifyValue);

    //试剂区坐标
    int displayReagentAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue);

    //测试通道
    void displayTestChnAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ, quint8 ChnNum, int notifyValue);

    int displayEmptyTrayAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ, quint8 indexTray, quint8 tubeNum,int notifyValue);

    //血样区
    int displayBloodHoleAxisPoint(bool bFindShow, bool bWrite_x,quint8 indexhole, int notifyValue);

    /** 显示仪器区域坐标
    * @brief NotifyShowInstrumentPoint
    * @param displayPoint
    */
    void NotifyShowInstrumentPoint(QMap<quint8,QPoint> displayPoint, const QString);

    /** 修改一个其它左边跟着变化
    * @brief InsertOneChangeOthersChange
    * @param displayPoint
    */
    void InsertOneChangeOthersChange(QMap<quint8, QPoint> displayPoint, const QString);

    //更新列表的偏移值
    void updateNotifyOffsetValue(bool bupdate_x, const int offsetValue);

    void writeboard(quint8 intdexZ,quint8 row, QByteArrayList &sendcommd);

    /** 查询修改坐标
    * @brief QuerytoModifyCoordinates
    * @param PointGather  坐标集合
    * @param FindIndexKey 找孔的下标
    * @param FindedCoordinates  找到的坐标
    * @return 查找状态
    */
    bool QuerytoModifyCoordinates(QMap<int,QPoint> PointGather,int FindIndexKey,QPoint &FindedCoordinates);

    /** 选中行 变背景色
    * @brief SelectHoleChangebgm
    * @param SelIndexRows
    */
    void SelectHoleChangebgm(bool bChangecolor, int SelIndexRows, int indexTray);

    /** 修改控件背景色&&还原
    * @brief ChangeControlColors
    * @param bChangecolor
    * @param SelIndexRows
    */
    void ChangeControlColors(bool bChangecolor,int SelIndexRows);

    void DelTableText();/*清空列表先*/

    void ReminderTable(int TableRow);/*显示测试区试管坐标抓手*/

    void test_catch_putdown_cup(const int form_, const int end_, QMap<int,QByteArrayList> &group_directives); //测试抓试管杯命令

    void _sendmoveActiveToEquipment(AreasCalibration indexZone, QPoint MovingAxis, quint8 indexZ); //移动测试命令发送到机器

    void ExitCoordinateSaving(); //退出调试时写入缓存坐标
signals:
    void writdAxisata(const QByteArrayList ,QString);

    void Resetmaneuver(); //复位调用按钮复位

    void SportActive(int ACtionType, const QByteArrayList GroupAction); /*仪器校准动作*/

    void TrayMoveTest(const QByteArrayList,const bool,const quint8);//试管托盘运动测试

public slots:

   void ClickBloodTube();

   void ClickEmptyTube();

   void ClickTestChannelTube();

   void ClickReagentsTube();

    /** 收到校准消息完成
    * @brief Recv_CalibrationMoved
    */
    void Recv_CalibrationMoved();

    void recv_scynTestCommand(int index, const QByteArrayList commdArry); //吸试剂到测试通道调试

    void send_test_cups_accurate(); //抓杯测试收到完成一个

    void slotsendcode(QByteArrayList data_);
private:
    Ui::CustomPlot *ui;
    QFont mfont;
    TubeContinueDoing *mpTestCaseRun = nullptr;
    QButtonGroup * m_CheckGroupBox = nullptr;

    int m_downhigh;  //抓手下降高度
    int m_indexDown = -1;
    int m_CalibrationArea;  //选取的区域
    QPoint mbtnClickPos;   //单击的坐标
    quint8   mcodeNum;       //命令编号
    QFont mcustFont;


    QMap<int,QPoint> mReagentLinqueVaue; //记录初始列表内的值==试剂液、试剂针
    QMap<int,QPoint> mEmptytestcup_offset_Hands[4];       //空测试杯、抓手
    QMap<int,QPoint> mEmptytestcup_offset_BloodNeedle[4]; //空测试杯、血样针
    QMap<int,QPoint> mSampleBloodPos; //血样区坐标
    QButtonGroup * mbloodBtnGroupbox = nullptr;
    quint8  mInstrumentType;
    int     m_selectRows = -1;   //选中行


    const  QString m_Cssbtn = "QToolButton{border-image: url(:/Picture/SetPng/btninit.png);font-family:'楷体';}"
                        "QToolButton:checked{border-image: url(:Picture/SetPng/btnpushing.png);font-family:'楷体';}"
                       "QToolButton:pressed{border-image: url(:/Picture/SetPng/btnpush.png);font-family:'楷体';}"
                       "QToolButton:hover{border-image: url(:/Picture/SetPng/btnon.png);font-family:'楷体';}";

    const QString m_SpinboxsheetX = "QSpinBox{font-size: 15pt;font-family:'楷体';}"
                              "QSpinBox::up-button{border-image:url(:/Picture/plus.png);"
                              "subcontrol-position:right;width:30px;height:30px;}"
                              "QSpinBox::down-button{border-image:url(:/Picture/min32.png);"
                              "subcontrol-position:left;width:30px;height:30px;}"
                              "QSpinBox:up-button:pressed{margin-top:2px;}"
                              "QSpinBox:down-button:pressed{margin-top:2px;}"
                              "QSpinBox{background-color: rgb(255,250,205);}";

    const QString m_SpinboxsheetY = "QSpinBox{font-size: 15pt;font-family:'楷体';}"
                              "QSpinBox::up-button{border-image:url(:/Picture/plus.png);"
                              "subcontrol-position:right;width:30px;height:30px;}"
                              "QSpinBox::down-button{border-image:url(:/Picture/min32.png);"
                              "subcontrol-position:left;width:30px;height:30px;}"
                              "QSpinBox:up-button:pressed{margin-top:2px;}"
                              "QSpinBox:down-button:pressed{margin-top:2px;}"
                              "QSpinBox{background-color: rgb(255 ,228 ,225);}";
    QMap<int,QByteArrayList> mtest_catch_put_commad;
    const quint8 total_tube_num = 60;
    QVector<int> mfromHole;
    QVector<int> mendHole;
    int mfromtray;
    int mendtray;
    bool mbstopTestadjust;




};

#endif // CUSTOMPLOT_H
