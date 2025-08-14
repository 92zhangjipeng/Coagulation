#ifndef HEIGHT_DATA_H
#define HEIGHT_DATA_H

#include <QWidget>
#include <iostream>
#include <map>
#include <QMap>
#include <QMetaType>
#include <QVector>
#include "cglobal.h"
#include <QFileInfo>
#include <stdio.h>
#include <QDebug>
#include <QCloseEvent>
#include <QTableWidget>
#include <QSqlDatabase>
#include <QButtonGroup>
#include <QPointer>
#include <QComboBox>
#include "correct_data.h"
#include "custom_struct.h"
#include "notifybarcode.h"

#include "threadAddSample/mythreadaddsample.h"

#include "dilag/batchaddsample.h" //批量添加

#define  CHECK_ROW          0
#define  SAMLPE_NAME        1
#define  HEIGHT_DATA        2
#define  RICHBLOOD_HOLE     3
#define  PROJECT_ITEM       4
#define  BARCODE            5


namespace Ui {
class Height_Data;
}

class Height_Data : public QWidget
{
    Q_OBJECT

public:
    explicit Height_Data(QWidget *parent = 0);
    ~Height_Data();

     void   _initcreat();
     //补回用掉的血样孔
     void   Makeuptubenum(quint8);
     //初始化下拉血样孔
     void   _initNumAnaemiaHole();
     //测高完成
     void   slotShowTestImageTube(QString , double value, const bool &isreplace, const QString &idnum);
     //取消任务返回试管孔号
     void   _backcancelhole(const quint8 _index);


private:
    bool loadAndDisplayImage(const QString& path);
    void updateTableItem(const QString& id, double heightValue);
    void handleSampleAddition(double heightValue);
    void showHoleWarning(const QStringList& holes);
    void addNewSample(double value, const QStringList& holes);

protected:
    void    closeEvent(QCloseEvent *event);
    void    keyPressEvent(QKeyEvent *scanData);
    void    resizeEvent(QResizeEvent* event);
    bool    eventFilter(QObject *obj, QEvent *event);

signals:
    void    _sycnwaittestsampledata(QString samplename,
                                    QString savedtime,
                                    QString barcode_str,
                                    double done_pin,
                                    QString project_,
                                    quint8 CurrRichHole,
                                    int totalnum,
                                    bool insertWholeBloodMode);

    void    ReminderTextOut(quint8 ReminderKind,const QString dataStr); //提示文字界面信号
    void    Taskconfigcloe();
    void    ReminderHole(int); //提示放置的孔
    void    myTextChangedSignal(const QString &oldText, const QString &newText);
public slots:

    void    Slot_ConfigureData(unsigned int ,int ,QString);
    void    _updateaddprogress(int index_, int _total);
    void    _slotupdatetestui(QList<quint8>marktube,QString sample_name,quint8 anemiahole,int index_add,int all_add_task);
    void    SlotNotifyTestHeight(int,int,QString);
    void    ReminderPutBloodHole(int RichHolenum); //提醒放置的血样孔
    void    slotsavebarcode(unsigned int row,quint8 cols,QString noityBarcode);
    //修改PPP孔号
    void   selectPPPholeChange(const QString& index_);

private slots:
    void    tableItemClicked(int row,int col);
    void    sortByColumn(int); //点击表头
    void    on_toolButton_Cancel_clicked();
    bool    TheSameBloodHole(); //添加样本时有相同贫血孔号？
    void    _clickBloodmode(int);
    void    on_Sample_Data_tablewidget_customContextMenuRequested(const QPoint &);
    void    AF_DATA_REQUEST(QVariant sampleiddata);

private:

    /** 添加样本单个
     * @brief AddOneTestSample
     * @param addSampleMode
     * @param TestHeight
     * @param Canselecthole
     * @param barcodestr
     * @param viewImage
     */
    int AddOneTestSample(const bool addSampleMode,
                            double TestHeight,
                            QStringList Canselecthole,
                            QString barcodestr,
                            bool viewImage);


    int  _Addtasksmanually(); //手动添加任务

    void _updateotherinserthole(int _rows, QString index_); //更新其它选项富血孔


    void _savewaitTestSample(); //保存

    void _sycnstudata(QList<int>,QTableWidget *TaskWidget ); //同步到数据结构中

    void _deleteSelectedSample(); //删除选中待测样本

    void batchAddTask(); //批量添加任务

    void _obtainSelectedSample(QList<int> &itemList); //获取选中的样本

    bool SQLTheSameItem(QString textchange);//重复样本名 sql

    bool repeatData(QString); //重复样本名

    void InitTablewidget();

    void SetColumnText(int row, int col, QString text);//设置列数据

    bool testhightValueNotZero(QList<int> checklist);

    bool WidetItemNUll(QTableWidget *TaskWidget, QList<int> selRows); //item 是否为空

    void DeleteAllItems(QTableWidget * table);
    void initshowimg();

    bool SameSampleandtube(QList<int> TaskList, QTableWidget *TaskWidget);
    //反选
    void selectInverseItem();
    void selectAllItem(const bool bselAll);               //全选/全不选中

    bool NeedTubeEnouthTesting(const int hadtube, int &needtube, QList<int> selItems);
    //＋血样孔是否足够
    bool _enoughAddSampleHole(QStringList &canselholeList_);

    bool shouldIgnoreClick(int col) const;
    void saveOriginalValue(int row, int col);
    void handleSampleOrHeightClick(int row, int col);
    void handleProjectItemClick(int row, int col);
    void handleBarcodeClick(int row, int col);


private:
    Ui::Height_Data *ui;

    QString m_OriginallyValue;//双击item的值

    Correct_Data *m_correctdata = nullptr; //修改测高值、样本号

    QPointer<NotifyBarCode> m_Notifycode; //手动修正条形码

    QMap<QString,bool> m_pressedhole;  //添加任务选中的孔号<血样孔号,状态>

    QMap<QString,quint8> m_selbloodholetemp; //保留临时孔号<样本号,PPP孔号>

    QMap<QString,QString> mCreatTime;

	QButtonGroup *mtestmodebox;

    QString m_Barcodestr;   //扫码枪数据


    QString blood_combox_css = "QComboBox{font-family:'楷体';font-size:20px;color: white;\
                            background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:0,\
                            stop:0 rgba(188,187,186,255), stop:1 rgba(188, 187, 186, 255));border:1px solid black;\
                            color:black;margin:0px;border: 1px solid #f0ebeb;padding-left:10px;}"
                            "QComboBox::drop-down{border:transparent;width:20px;height:40px}"
                            "QComboBox::down-arrow{image:url(:/Picture/SetPng/commbox.png);}"
                            "QComboBox QAbstractItemView::item{height:25px;}";


    QString InitTalbe_css = "QTableWidget::item:hover{background-color:rgb(70 ,130 ,180)}"
                            "QTableWidget::item:selected{background-color:rgb(139, 139, 122)}"

                            "QTableView QTableCornerButton::section{color: white; background-color: rgb(188, 187, 186); "
                            "border: 1px solid rgb(188, 187, 186);border-radius:0px; border-color: rgb(188, 187, 186);"
                            "font: bold 1pt;padding:12px 0 0 10px}"

                            "QHeaderView::section,QTableCornerButton:section{ \
                            padding:3px; margin:0px; color:rgba(188, 187, 186, 255);  border:1px solid rgba(188, 187, 186, 255); \
                            border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                            background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                            "QTableWidget{background-color:white;border:none;}"
                            "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                            stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}"
                            "QTableView QTableCornerButton::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                            stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}";

    mythreadaddsample* m_threadaddsample = nullptr;

    QPointer<batchAddSample> m_batchaddTestnumSample;
};

#endif // HEIGHT_DATA_H
