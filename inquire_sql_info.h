#ifndef INQUIRE_SQL_INFO_H
#define INQUIRE_SQL_INFO_H

#include <QButtonGroup>
#include <QTime>
#include <QWidget>
#include <QMultiMap>
#include "customcreatsql.h" /*数据库*/
#include "QCustomPlot.h"
#include "cglobal.h"
#include "height_data.h"
#include "calibrate.h"
#include "custombars.h"
#include "InquireDataClass/querydatathread.h"


#define INQUIRE_ALL_SQL      0
#define INQUIRE_TODAY_SQL    1
#define INQUIRE_MONTH_SQL    2
#define INQUIRE_SPECIFIC_SQL 3

namespace Ui {
class Inquire_Sql_Info;
}

class Inquire_Sql_Info : public QWidget
{
    Q_OBJECT

public:
    explicit Inquire_Sql_Info(QWidget *parent = 0);
    ~Inquire_Sql_Info();

    void LoginEngineerMode(const bool); //工程师模式

    void initDateEditUI(QDateEdit*dateEdit,QWidget*parent);
protected:
    virtual void resizeEvent(QResizeEvent *event) override;

    void closeEvent(QCloseEvent *event);
public slots:

    void ViewLoadInquierdata(int numtotal, int n_ing,InqueryDatastu_t *pdata);

    void RecvCurveData(const QVector<QString> &data); //接收曲线数据

    void myMoveEvent(QMouseEvent *event);

private slots:
    /*初始化列表样式*/
    void Init_tablewidget_style();

    void on_toolButton_OK_clicked();

    void SelectItem(QTableWidgetItem *);

    void OnPlotClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void on_toolButtonFindexatc_clicked();

    void on_toolButton_stats_clicked();

    void on_toolButton_outPrint_clicked();

    void on_toolButton_creatPdf_clicked();

signals:
    void FindModuleStyle(const quint8);

    void FindspecifiedData(QDateEdit *pdata,const int days);

    void Locatethelookup(QString inquierId,QString senddoctor,QString PatientsName,QString PatientsAge,
                          QString sectionkind,QString bbednum);//精确定位查找

    void InquierCurveView(const QString FindId); //查询曲线

    void writepdfprint(QString path_);

    void _printoutresult();

    void clickOutPdfFile(const QString PdfID); //选中信息输出Pdf

private:

    void firstrunthread();

    void _creatCPGraph(QCustomPlot* pshowcurvedata);

    void cleanPlogtandUpdate(); /*清空曲线数据并率刷新*/

    void addInquireCurvedata(QStringList dataList,quint8 indexReag);

    void insertparaTopdffile(InqueryDatastu_t *pdata);

    //清空查找到的数据
    void DeleteStuData();

    //插入数据到表格
    void SetColumnTextGroup(QTableWidget * tablewiget, int row, int col, QString text);

    void InsertInqireResult(int Rows,quint8 Cols,QString Datastr);

	void initTextTip();

    void setupRealtimeDataDemo(QCustomPlot *customPlot); //设置qcustomplot画图属性

    void init_sats_curve(QVector<double> useed_reag);

    void stats_today_reagent(); //统计当天

    void stats_all_reagent(); //统计全部使用数据

    void stats_thismonth_reagent(); //统计当月试剂使用量

    void stats_designate_reagent();//查询指定时间使用试剂统计

    void CheckWhetherTestOrNot(QString &ResultCheck);

    void InsertOneRowsData(InqueryDatastu_t *pdata);

private:
    Ui::Inquire_Sql_Info *ui;
    QList<InqueryDatastu_t *> m_del;
    QString m_clickId;

    //Printthereport* m_printreport = nullptr; //打印报告

    QThread m_threadInqure;
    QueryDataThread *mquiredataclass = nullptr;


    QMap<QString, QColor> m_curveinfomap;
    QCPGraph* m_showAACpgraph = nullptr;
    QCPGraph* m_showADPCpgraph = nullptr;
    QCPGraph* m_showEPICpgraph = nullptr;
    QCPGraph* m_showCOLCpgraph = nullptr;
    QCPGraph* m_showRISCpgraph = nullptr;
    QButtonGroup *m_group = nullptr;

    QCPItemText * m_TextTip = nullptr;
    QList<QString> mChildren;

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
                             "QTableWidget{border:1px solid #696969;}"
                             "QTableWidget::item:selected{background-color:rgb(139, 139, 122)}"
                              "QTableView QTableCornerButton::section{color: white; background-color: rgb(188, 187, 186); "
                              "border: 1px solid rgb(188, 187, 186);border-radius:0px; border-color: rgb(188, 187, 186);"
                              "font: bold 1pt;padding:12px 0 0 10px}"
                               "QHeaderView::section,QTableCornerButton:section{ \
                               padding:3px; margin:0px; color:rgba(188, 187, 186, 255);  border:1px solid rgba(188, 187, 186, 255); \
                               border-left-width:0px; border-right-width:1px; border-top-width:0px; border-bottom-width:1px; \
                               background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #646464,stop:1 #525252); }"
                               "QTableWidget{background-color:white;border:1px;}"
                               "QHeaderView::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                               stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}"
                               "QTableView QTableCornerButton::section {background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,\
                               stop:0 rgba(188, 187, 186, 255), stop: 0.5 rgba(188, 187, 186, 255),stop: 0.6 rgba(188, 187, 186, 255), stop:1 rgba(188, 187, 186, 255)); color: white;}";
};

#endif // INQUIRE_SQL_INFO_H
