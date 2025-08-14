#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <QTableWidget>
#include <QWidget>
#include "analyticaljson.h"
#include "height_data.h"
#include "qcommboxdelegate.h"
#include "qcustomplot.h"
#include "qsimpleled.h"
#include <PrintReport/printthereport.h>
#include <QAbstractItemView>
#include <QVariant>
#include <custom_style/widgetdelegate.h>
#include <dilag/customfixtableview.h>



#define   RESULTITEM_TOTAL      5
#define   EXPERIMENT            0
#define   ITEMS                 1
#define   RESULT_DATA           2
#define   ABNORMAL              3
#define   REFERNECEVALUE        4
#define   TOTALROWS             20


namespace Ui {
class Calibrate;
}

class Calibrate : public QWidget
{
    Q_OBJECT

public:
    explicit Calibrate(QWidget *parent = 0);
    ~Calibrate();

    void initstyle();

    //添加任务同步添加患者数据库
    void _addpatientsqltable(QString id_, QString addtime, QString barcode_, QString testProject);

    //取消任务Id
    void cancelSampleResultItem(const QString &cancelId);

    void ImportTodayInfo();

private slots:
    void dgvDellClick(QTableWidgetItem* item);


private:
    void _setNoteditableCol(QList<int> cols); //设置不可编辑列

    void _setCommboxDelegate(QList<int> cols); //设置下拉框代理

    void InitSaveTableList(QTableWidget *pTable);

    void SetColumnText(QTableWidget * tablewiget,int row,int col,QString text);

    //行背景颜色
    void MarktableWidgetColor(QTableWidget * table, int row, int cols, const QString &text, const QColor &color);

public slots:
    void    updatecommboxInfo();//同步患者下拉信息
    void    showContextpup(const QPoint &pos);
    void    recvSampleTestingErr(const QString &ErrSampleid, const quint8 &channelIdx); //样本测试异常

signals:
    void hideCurveUi();

private:
    Ui::Calibrate *ui;

    //初始化当天的样本背景色
    QColor m_loadTableColor;

    QScopedPointer<CustomFixTableView> mpdatawidget;

    //点击查看样本ID 号
    int m_clickViewSampleId;

    QList<WidgetDelegate* > m_pdelegate;
    QList<TableItemDelegate*> m_eneditdelegate;
    QList<TableSelTimeDelegate* >m_pconfigtimedelegate;



    QString mCSS = "QMenu {color:black;background-color:white; border-radius:3px;padding:5px;margin:6px;}"
                   "QMenu::item:text { padding-left:10px;padding-right:10px;}"
                   "QMenu::item:selected{ color:#1aa3ff;background-color: #e5f5ff;border-radius:3px;}";

    QString VScroBarCss =
            "QScrollBar:vertical {width: 18px; background: transparent; margin: 0px,0px,0px,0px; padding-top: 18px;padding-bottom: 18px;} "
            "QScrollBar::handle:vertical {width: 18px; background: rgba(0,0,0,25%);border-radius: 4px;min-height: 20;}"
            "QScrollBar::handle:vertical:hover {width: 8px;background: rgba(0,0,0,50%);border-radius: 4px; min-height: 20;}"
            "QScrollBar::add-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
            "QScrollBar::sub-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
            "QScrollBar::add-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
            "QScrollBar::sub-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
            "QScrollBar::sub-page:vertical {background: rgb(178,180,180); border-radius: 0px;}"
            "QScrollBar::add-page:vertical {background: rgb(178,180,180); border-radius: 0px;}";

    QString TableWidgetCss = "QTableWidget::item:hover{background-color:rgb(70 ,130 ,180)}"
                             "QTableWidget::item:selected{background-color:rgb(135, 206, 250)}"
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

     enum verifierinfo{
         SAMPLEID,                              //样本号
         ADDSAMPLETIME,                         //添加时间
         BARCODESAMPLE,                         //条形码
         NAMESAMPLE,                            //姓名
         SEXSAMPLE,                             //性别
         AGESAMPLE,                             //年龄
         DEPARTMENT,                            //科室
         HOSPITALIZATIONNUMBER,                 //住院号
         BEDNUMBER,                             //床号
         WARDCODE,                              //病区号
         SUBMISSIONTIME,                        //送检时间
         REFERTOTHEDOCTOR,                      //送检医生
         INSPECTIONTIME,                        //检验时间
         MEDICALEXAMINER,                       //检验医生
         DIAGNOSIS,                             //诊断
         REMARK,                                //备注
         REVIEWDOCTORS  ,                       //审核医生
         ITEM_TOTAL
     };
};

#endif // CALIBRATE_H
