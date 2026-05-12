#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H

#include <QWidget>
#include <array>
#include <functional>
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

    void    initializeCurveWidget(QCustomPlot* plotWidget, int channelIndex);

    void    updatePlotTitle(quint8 channelIndex, QCustomPlot* plotWidget, QString title);

    void    drawCurvePoint(quint8 channelIndex, QVector<double> xData, QVector<double> yData); //绘制曲线

    void    InitChart();

    void    resetVectorData(const int& channel, const int& reagentNumber);

    void    clearCurve(quint8 channelIndex);

    double  calculateMean(const QVector<double>& inputData); //求均值


    double  calculateProgress(int channel, int reagent) const;

    /*
     * 单个试剂测试完成后数据存入SQL
     * @brief saveTestDataToSqllite
     * @param indexChannel
     * @param reagents
     * @param finishsampleid
     */
    void saveTestDataToSqllite(const quint8 indexChannel, const quint8 reagents, int finishsampleid);

private:
    using AnalyzerFunc = std::function<void(const QString id,const QVector<double>&)>;

    void analyzeAAAndSave(const QString id, const QVector<double>& data);
    void analyzeADPAndSave(const QString id,const QVector<double>& data);
    void analyzeEPIAndSave(const QString id,const QVector<double>& data);
    void analyzeCOLAndSave(const QString id,const QVector<double>& data);
    void analyzeRISAndSave(const QString id,const QVector<double>& data);

protected:
    virtual void    resizeEvent(QResizeEvent *event) override;

private:
    // 布局计算辅助函数
    struct LayoutConfig {
        int columns;
        int rows;
        int curveWidth;
        int curveHeight;
    };

    LayoutConfig calculateLayout(quint8 equipmentKind, int availableWidth, int availableHeight) const;
    void positionWidgetsInGrid(const LayoutConfig& config, quint8 equipmentKind);
    void adjustWidgetSizes(const LayoutConfig& config, quint8 equipmentKind);

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

    // 缓存机制：存储通道索引到QCustomPlot的映射，避免重复查找
    mutable QHash<int, QCustomPlot*> m_plotCache;

    // 预定义常量以提高可读性和维护性
    static constexpr int MAX_CHANNELS = 12;
    static constexpr int MAX_REAGENTS = 5;

    // 布局常量
    static constexpr int HORIZONTAL_SPACING = 5;
    static constexpr int VERTICAL_SPACING = 5;
    static constexpr int DEFAULT_COLUMNS = 4;

    // 使用std::array替代原始数组，提供更好的类型安全性和内存管理
    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mTestDataX;
    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mTestDataY;

    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mCurvePlote_x;
    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mCurvePlote_y;

    int  mcuteNumData;
    bool mbaverage;


};

#endif // GRAPHPLOT_H
