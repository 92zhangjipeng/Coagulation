#ifndef CUSTOMFIXTABLEVIEW_H
#define CUSTOMFIXTABLEVIEW_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QScreen>
#include <QApplication>
#include <QCloseEvent>
#include <qcustomplot.h>


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

public slots:
    void viewOneReagentCurve(int rows, int cols);
private:
    Ui::CustomFixTableView *ui;

    QPropertyAnimation *m_showAnim;  // 显示动画
    QPropertyAnimation *m_hideAnim;  // 隐藏动画
    QScreen *m_currentScreen;        // 当前屏幕对象

    //业务
    QString m_viewIDstr;
    int m_viewIDNum;
    QString m_sampleSex;
    QList<QString> m_SampleidList;

    enum class TableItemnum {
        ReagentName,
        ReagentState,
        ReagentTime,
        ReagentTimeResult,
        OutResult,
        ReferenceValue
    };

    enum class Status {
        Completed,
        NotTested,
        Pending
    };


    void initCreatCPGraph(QCustomPlot* pshowcurvedata);
    void initCreateCurveWidget(QCustomPlot *customPlot);
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
};

#endif // CUSTOMFIXTABLEVIEW_H
