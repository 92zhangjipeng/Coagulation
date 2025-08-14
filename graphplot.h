#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H


#include <QWidget>
#include "qcustomplot.h"
#include "cglobal.h"
#include "testing.h"

namespace Ui {
class GraphPlot;
}


class GraphPlot : public QWidget
{
    Q_OBJECT

public:
    explicit GraphPlot(QWidget *parent = 0);
    ~GraphPlot();

    void    innitKindequipment();


private:

    QCustomPlot* GetCurvepWidget(int indexChn);

    void    _initCreatCurveWidget(QCustomPlot* pPlotWidget, int indexchn);

    void    notifyplotname(quint8 indexchn, QCustomPlot* pPlotWidget,QString dada);

    void    DrawCurvePoint(quint8 indexChn,QVector<double> , QVector<double> );//绘制曲线

    void    InitChart();

    void    resetvect(const int& Chn,const int& reagentNum);

    void    CleanCruve(quint8 indexChannel);

    double  get_mean(QVector<double> in); //求均值


    double  calculateProgress(int channel, int reagent) const;

    void    save_test_data_to_sqllite(const quint8 indexChannel, const quint8 reagents, int finishsampleid);

protected:
    virtual void    resizeEvent(QResizeEvent *event) override;

signals:
    void    DrawProgressbar(quint8 index, double proportion);              //绘制测试通道进度



public slots:
    void    GetTestingValue(const QString& , const quint8 &, const quint8 &,
                            const float&, const int&, const int&initPRP, const  int&initPPP);

    void    backallCurveClear();

    /*单个项目测试完成 保存曲线图 进度条设置为0*/
    void  CompleteOneSample(const quint8 , const quint8);

    void  recvsycnViewCurvePara(int,bool);

private:
    Ui::GraphPlot *ui;

    QMap<quint8,QCPGraph*>   mcurveWidgetList;
    QMap<quint8,QCPTextElement* > m_titemap;

    QList<QWidget* > m_pchnWidgetList;

    QVector<double> mTestDataX[MACHINE_SETTING_CHANNEL][REAGENT_TOTAL];
    QVector<double> mTestDataY[MACHINE_SETTING_CHANNEL][REAGENT_TOTAL];

    QVector<double> mCurvePlote_x[MACHINE_SETTING_CHANNEL][REAGENT_TOTAL];
    QVector<double> mCurvePlote_y[MACHINE_SETTING_CHANNEL][REAGENT_TOTAL];




    int  mcuteNumData;
    bool mbaverage;


};

#endif // GRAPHPLOT_H
