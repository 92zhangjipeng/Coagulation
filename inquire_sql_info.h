#ifndef INQUIRE_SQL_INFO_H
#define INQUIRE_SQL_INFO_H

#include <QButtonGroup>
#include "InquireDataClass/querydatathread.h"
#include <QTime>
#include <QWidget>
#include <QDialog>
#include <QFrame>
#include <QLabel>
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
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:

    void ViewLoadInquierdata(int numtotal, int n_ing,InqueryDatastu_t *pdata);
    void ViewLoadInquierdataList(InquireRowList rows);
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

    void onTableContextMenu(const QPoint &pos);   // 右键菜单响应
    void deleteSelectedItem();                    // 删除选中项
signals:
    void FindModuleStyle(const quint8);
    void FindspecifiedData(QDateEdit *pdata,const int days);

    //删除患者表一样的Id
    void sycnDelOthertableWidgetRow(const QString DelSameId);
    void sycnDelTestSample(const QString DelSameId);

    //精确定位查找
    void Locatethelookup(QString inquierId,QString senddoctor,QString PatientsName,QString PatientsAge,
                          QString sectionkind,QString bbednum);
    //查询曲线
    void InquierCurveView(const QString FindId);
    void clickOutPdfFile(const QString PdfID); //选中信息输出Pdf

private:
    bool deleteSampleFromDatabase(const QString &sampleId);  // 删除数据库中的样本
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
    void cancelInquireFill();
    void fillInquireChunk();
    void setInquireBusy(bool busy);

private:
    Ui::Inquire_Sql_Info *ui;
    QMutex m_dataMutex;
    QList<InqueryDatastu_t *> m_del;
    QString m_clickId;
    InquireRowList m_pendingInquireRows;
    int m_inquireFillIndex = 0;
    bool m_inquireFilling = false;

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

    // 鼠标交互：竖直线、圆点、数值提示框
    QCPItemLine *m_vLineCursor = nullptr;    // 跟随鼠标的竖直线
    QList<QCPItemEllipse*> m_curveDotList;   // 每条曲线与竖直线交点的圆点
    QCPItemText *m_cursorValueTip = nullptr; // 显示当前 X/Y 值的提示框

    QList<QString> mChildren;

    // 查询条件区动态控件
    QCheckBox* checkBox_today = nullptr;
    QCheckBox* checkBox_this_month = nullptr;
    QCheckBox* checkBox_all = nullptr;
    QCheckBox* checkBox_select_time = nullptr;
    QLabel* label_fromdate = nullptr;
    QDateEdit* dateEdit_begin = nullptr;
    QLabel* label = nullptr;
    QDateEdit* dateEdit_end = nullptr;
    QToolButton* toolButton_OK = nullptr;
    QToolButton* toolButton_stats = nullptr;
    QToolButton* toolButton_outPrint = nullptr;
    QLabel* label_Sample = nullptr;
    QSpinBox* spinBox_label_Sample = nullptr;
    QLabel* label_Name = nullptr;
    QLineEdit* lineEdit_Name = nullptr;
    QLabel* label_age = nullptr;
    QComboBox* comboBox_age = nullptr;
    QLabel* label_Section = nullptr;
    QComboBox* comboBox_Section = nullptr;
    QLabel* label_sendingdoctor = nullptr;
    QComboBox* comboBox_sendingdoctor = nullptr;
    QLabel* label_bednum = nullptr;
    QLineEdit* lineEdit_bednum = nullptr;
    QToolButton* toolButtonFindexatc = nullptr;

    QFrame* m_sampleFrame = nullptr;
    QFrame* m_curveFollow = nullptr;
    QLabel* m_curveFollowTitle = nullptr;
    QWidget* m_curveResizeGrip = nullptr;
    QDialog* m_exactDialog = nullptr;
    int m_followSampleRow = -1;
    QSize m_curveFollowSize;
    bool m_curveResizing = false;
    QPoint m_curveResizeAnchor;
    QSize m_curveResizeStartSize;

private:
    static const QString& getScrollBarStyle();
    static const QString& getTableWidgetStyle();
    void updateCurveLabelsPosition(); // 更新曲线标注位置
    void placeCurveFollow(int sampleStartRow);
    void hideCurveFollow();
    void placeCurveResizeGrip();
    bool handleCurveResize(QObject *watched, QEvent *event);

    // 安全相关辅助函数
    static QString sanitizeInput(const QString& input);  // 输入过滤防注入
    void initInputValidators();                           // 初始化输入验证器
    void cleanupMemory();                                  // 清理未释放内存
    bool m_threadInitialized = false;                      // 线程初始化标记


    QString medicalMenuStyle = R"(
        QMenu {
            background-color: #F5F7FA;
            border: 1px solid #CBD5E1;
            border-radius: 4px;
            padding: 4px 0px;
            font-family: "Microsoft YaHei";
            font-size: 12pt;
        }
        QMenu::item {
            padding: 8px 30px 8px 20px;
            color: #1E293B;
            background-color: transparent;
            border-bottom: 1px solid transparent;
        }
        QMenu::item:selected {
            background-color: #E2E8F0;
            color: #0F172A;
            border-left: 3px solid #0891B2;
            background-color: #E2E8F0;
        }
        QMenu::item:pressed {
            background-color: #CBD5E1;
        }
        QMenu::separator {
            height: 1px;
            background-color: #E2E8F0;
            margin: 4px 8px;
        }
        QMenu::right-arrow {
            image: none;
        }
    )";
};

#endif // INQUIRE_SQL_INFO_H
