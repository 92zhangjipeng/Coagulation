#ifndef INQUIRE_SQL_INFO_H
#define INQUIRE_SQL_INFO_H

#include <QButtonGroup>
#include "InquireDataClass/querydatathread.h"
#include <QTime>
#include <QWidget>
#include <QMultiMap>
#include <QMutex>
#include "customcreatsql.h" /*数据库*/
#include "QCustomPlot.h"
#include "cglobal.h"
#include "height_data.h"
#include "calibrate.h"
#include "custombars.h"

#include <memory>


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
    void initTableWidgetStyle();
    void on_toolButton_OK_clicked();
    void onSelectRowItem(QTableWidgetItem *);
    void OnPlotClick(QCPAbstractPlottable *plottable, int dataIndex, QMouseEvent *event);
    void on_toolButtonFindexatc_clicked();
    void on_toolButton_stats_clicked();
    void on_toolButton_outPrint_clicked();


signals:
    void FindModuleStyle(const quint8);
    void FindspecifiedData(QDateEdit *pdata,const int days);

    //精确定位查找
    void Locatethelookup(QString inquierId,QString senddoctor,QString PatientsName,QString PatientsAge,
                          QString sectionkind,QString bbednum);
    //查询曲线
    void InquierCurveView(const QString FindId);


    void clickOutPdfFile(const QString PdfID); //选中信息输出Pdf

private:

    void firstrunthread();
    void creatCPGraph(QCustomPlot* pshowcurvedata);
    void cleanPlogtandUpdate(); /*清空曲线数据并率刷新*/
    void addInquireCurvedata(QStringList dataList,quint8 indexReag);

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
    void adjustYAxisRange(); // 自动调整Y轴范围

private:
    Ui::Inquire_Sql_Info *ui;
    QMutex m_dataMutex;
    QList<InqueryDatastu_t *> m_del;
    QString m_clickId;

    //Printthereport* m_printreport = nullptr; //打印报告

    QThread m_threadInqure;
    std::unique_ptr<QueryDataThread> mquiredataclass;


    QMap<QString, QColor> m_curveinfomap;
    QCPGraph* m_showAACpgraph = nullptr;
    QCPGraph* m_showADPCpgraph = nullptr;
    QCPGraph* m_showEPICpgraph = nullptr;
    QCPGraph* m_showCOLCpgraph = nullptr;
    QCPGraph* m_showRISCpgraph = nullptr;
    std::unique_ptr<QButtonGroup> m_group;
	QCPItemText *m_TextTip = nullptr;
	QList<QCPItemText*> m_curveLabels; // 存储曲线标签
	
    QList<QString> mChildren;

private:
    static const QString& getScrollBarStyle();
    static const QString& getTableWidgetStyle();
    void updateCurveLabelsPosition(); // 更新曲线标注位置
};

#endif // INQUIRE_SQL_INFO_H
