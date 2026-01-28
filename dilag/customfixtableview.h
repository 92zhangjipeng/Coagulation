#ifndef CUSTOMFIXTABLEVIEW_H
#define CUSTOMFIXTABLEVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QScreen>
#include <QApplication>
#include <QCloseEvent>
#include <qcustomplot.h>
#include <suoweiFileManager/hospitalreportprinter.h>
#include "datalprocessor.h"


namespace Ui {
class CustomFixTableView;
}

class CustomFixTableView : public QWidget
{
    Q_OBJECT

public:
    explicit CustomFixTableView(QWidget *parent = nullptr);
    ~CustomFixTableView();


    void showWithAnimation();  // 滑入显示
    void hideWithAnimation();  // 滑出隐藏

    void setClickViewId(const QString& idstr,const int& idindex);
    void showResult(const bool &smooth);
    void setCheckBoxState(const bool& isstate, const QString &sampleSex,
                           const QList<QString> &TableSampleidList);

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent  *event) override;


private slots:
    void onHideFinished();           // 隐藏动画结束槽函数
    void on_checkBoxSmooth_clicked();
    void on_pushButtonBack_clicked();
    void on_pushButtonNext_clicked();

    void onCurveMouseMove(QMouseEvent* event);

public slots:
    void viewOneReagentCurve(int rows, int cols);
private:
    Ui::CustomFixTableView *ui;

    QPropertyAnimation *m_showAnim;  // 显示动画
    QPropertyAnimation *m_hideAnim;  // 隐藏动画
    QScreen *m_currentScreen;        // 当前屏幕对象


    QCPItemStraightLine* m_verticalLine = nullptr;      // 垂直线
    QCPItemTracer* m_dataPointTracer = nullptr;         // 数据点追踪器
    QCPItemText* m_dataPointLabel = nullptr;            // 数据标签
    QVector<double> m_currentCurveData;                 // 当前显示的曲线数据

    //业务
    QString m_viewIDstr;
    int m_viewIDNum;
    QString m_sampleSex;
    QList<QString> m_SampleidList;

    enum class TableItemnum {
        ReagentName,
        ReagentState,
        Aggregation60s,
        Aggregation180s,
        Aggregation300s,
        AggregationMax,
        Slope,
        TMAtime,
        LagTime,
        AUC,
        OutResult,
        ReferenceValue
    };

    enum class Status {
        Completed,
        NotTested,
        Pending
    };

    void showCustomAnalyzerResult(double maxAggregation, double slope, double auc,
                                  double timeToMax, double lagTime,
                                  double avg0to60, double avg60to180,
                                  double avg180to300, double avg0to300);

    void initCreatCPGraph(QCustomPlot* pshowcurvedata);
    void initCreateCurveWidget(QCustomPlot *customPlot);

    void setupSmoothInteractions(QCustomPlot* customPlot);
    void setupBeautifulAxes(QCustomPlot* customPlot);
    void setupModernAxis(QCPAxis* axis, const QString& label,
                         double lower, double upper,
                         double tickStep, int tickCount);
    void setupElegantGrid(QCustomPlot* customPlot);
    void addShadowEffect(QCustomPlot* customPlot);



    void showCurveTestEnd(const quint8& testEndReagent, const bool  &smooth);


    //清除
    QList<QCPGraph*> calibrationGraphs() const;
    void clearGraphData(QCPGraph* graph);
    void CreatResultCruve();
    void updatetabletestedResult(const quint8& indexReag);

    void clrarResultTable(QTableWidget *pTable);//表结果

    QCPGraph *m_showAACpgraph;
    QCPGraph *m_showADPCpgraph;
    QCPGraph *m_showEPICpgraph;
    QCPGraph *m_showCOLCpgraph;
    QCPGraph *m_showRISCpgraph;


    void initShowResultWidget(QTableWidget * Table);
    void insertColumnText(QTableWidget *tablewiget,
                            const int row,
                            const int col, const QColor &bgmC,
                            const QString& text);

    void insertColumnText(QTableWidget *tablewiget,
                              const int row,
                              const int col,
                              const QString& text);

    void updateParaState(QTableWidget *tablewiget,
                            const int row,
                            const int col,
                            const QColor &bgmC,
                            const QString& iconPath,
                            const QString& text);

    void setReagentStatus(int row, int col, Status status);

    //合并
    void spanTableWidget(const int &fromrows , const int& indexCols, QString pathicon, QString outResult);

    //参考值
    bool updateSetSexReferValue();
    void Analyzeresultingvalues(const bool &alreadysetSex);

    void viewOneSelf(const QString &idstr,const int &idinter);


private:
    // 添加以下成员变量
    QCPItemTracer *m_maxAggregationTracer;  // 最大聚集率标记点
    QCPItemText *m_maxAggregationLabel;     // 最大聚集率标签
    QCPItemCurve *m_aucCurve;               // AUC曲线

    QVector<QCPGraph*> m_aucFillGraphs;     // AUC填充区域
    QCPItemStraightLine *m_slopeLine;       // 斜率线
    QCPItemText *m_slopeLabel;              // 斜率标签
    QCPItemTracer *m_slopeStartTracer;      // 斜率起点标记
    QCPItemTracer *m_slopeEndTracer;        // 斜率终点标记

    QCPItemTracer *m_lagTimeTracer;        // 延迟时间标记点
    QCPItemStraightLine *m_lagTimeLine;    // 延迟时间垂直线
    QCPItemText *m_lagTimeLabel;           // 延迟时间标签


    // 添加新方法
    QPair<double, double> markMaxAggregation(const QVector<double>& data, QCPGraph* graph);
    double calculateAndDrawAUC(const QVector<double>& data, QCPGraph* graph);
    double calculateAndDrawSlope(const QVector<double>& data, QCPGraph* graph);
    void clearAuxiliaryItems();


    // 斜率计算辅助方法
    QPair<int, int> findSteepestSegment(const QVector<double>& data, int windowSize = 10);
    double calculateSlope(const QVector<double>& data, int start, int end);

    double calculateLagTime(const QVector<double>& data);
    void markLagTimeOnGraph(double lagTime, QCPGraph* graph);

    void updateTableWithCalculatedParams(const quint8& reagent, double slope, double timeToMax, double lagTime, double auc);


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
};

#endif // CUSTOMFIXTABLEVIEW_H
