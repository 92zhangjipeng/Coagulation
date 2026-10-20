#ifndef CALIBRATE_H
#define CALIBRATE_H

#include <QTableWidget>
#include <QWidget>
#include <QShowEvent>
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
    void addpatientsqltable(QString sampleId, QString addtime, QString barcode,
                            QString testProject, bool isEmergency = false);

    void addfollowUpSample(const QString sampleidF);
    void removefollowUpSample(const QString sampleidF){m_followUpSample.remove(sampleidF);}
    bool hasfollowUpSample(const QString& name) { return m_followUpSample.contains(name); }
    void clearfollowUpSample(){m_followUpSample.clear();}

    //取消任务Id
    int cancelSampleResultItem(const QString &cancelId);

    void ImportTodayInfo();
    void onPreviewPageShown();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void dgvDellClick(QTableWidgetItem* item);

    void onTableAboutToEdit(QTableWidgetItem *item);


private:
    void drawSamplePairFrame();   // 绘制配对样本框
    void refreshPairFrame();
    void rebuildPairRowsCache();  // 数据变化时重建配对行号缓存,绘制时直接取用

    void setNoteditableCol(QList<int> cols); //设置不可编辑列

    void _setCommboxDelegate(QList<int> cols); //设置下拉框代理

    void InitSaveTableList(QTableWidget *pTable);

    void SetColumnText(QTableWidget * tablewiget,int row,int col,QString text);

    void syncTableLayout();
    void applyDefaultRowHeight(QTableWidget *table, int row);

    //行背景颜色
    void MarktableWidgetColor(QTableWidget * table, int row, int cols, const QString &text, const QColor &color);

public slots:
    void updatecommboxInfo();//同步患者下拉信息
    void showContextpup(const QPoint &pos);
    void recvSampleTestingErr(const QString &ErrSampleid, const quint8 &channelIdx); //样本测试异常
    //查询界面删除相同ID 记录
    void onsycnDelOthertableWidgetRow(const QString delSameId);

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

    QSet<QString> m_followUpSample;

    //配对样本行号缓存:数据变化时由rebuildPairRowsCache重建,
    //避免每次绘制都O(n²)重算配对关系(导致切到结果预览后界面卡死、内存持续上涨)
    QSet<int> m_pairRowsCache;

    static const QString VScroBarCss;
    static const QString TableWidgetCss;

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
