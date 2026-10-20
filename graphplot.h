#ifndef GRAPHPLOT_H
#define GRAPHPLOT_H

#include <QWidget>
#include <array>
#include <QSet>
#include <functional>
#include <QVBoxLayout>
#include "qcustomplot.h"
#include "cglobal.h"
#include "testing.h"


class GraphPlot : public QWidget
{
    Q_OBJECT

public:
    explicit GraphPlot(QWidget *parent = 0);
    ~GraphPlot();

    void innitKindequipment();
private:

    QCustomPlot* GetCurvepWidget(int indexChn);

    void initializeCurveWidget(QCustomPlot* plotWidget, int channelIndex);

    void updatePlotTitle(quint8 channelIndex, QCustomPlot* plotWidget, QString title);

    void setChannelPppAbnormal(quint8 channelIndex, bool abnormal);

    void applyChannelCurveStyle(quint8 channelIndex);

    void drawCurvePoint(quint8 channelIndex, QVector<double> xData, QVector<double> yData);

    void InitChart();

    void resetVectorData(const int& channel, const int& reagentNumber);

    void clearCurve(quint8 channelIndex);

    double  calculateMean(const QVector<double>& inputData);

    double  calculateProgress(int channel, int reagent) const;

    void saveTestDataToSqllite(const quint8 indexChannel, const quint8 reagents, int finishsampleid);


    // 辅助函数：获取格式化的样本ID（处理复查标记）
    QString getFormattedSampleId(int finishsampleid, bool &isFollowUp) const;

    // 辅助函数：保存曲线数据到数据库（曲线点 + 通道贫血信息 + 结果摘要）
    void saveCurveDataToDatabase(const QString &sampleId, quint8 reagent,
                                const QVector<double> &curveData, quint8 indexChannel);

    // 辅助函数：根据试剂类型调用对应的分析函数并保存结果（数据已预处理）
    void saveAnalysisResultForReagent(const QString &sampleId, quint8 reagent,
                                         const QVector<double> &processedData);

    // 重载：接受已获取的数据，避免重复获取
    void saveTestDataToSqllite(quint8 indexChannel, quint8 reagents,
                                int finishsampleid, const QVector<double> &testedDataMap);

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
    struct LayoutConfig {
        int columns;
        int rows;
        int curveWidth;
        int curveHeight;
    };

    LayoutConfig calculateLayout(quint8 equipmentKind, int availableWidth, int availableHeight) const;
    void positionWidgetsInGrid(const LayoutConfig& config, quint8 equipmentKind);
    void adjustWidgetSizes(const LayoutConfig& config, quint8 equipmentKind);
    void createWidgetsDynamically();

signals:
    void    DrawProgressbar(quint8 index, double proportion);


public slots:
    void    GetTestingValue(const QString& , const quint8 &, const quint8 &,
                            const float&, const int&, const int&initPRP, const  int&initPPP);

    void    backallCurveClear();

    void  CompleteOneSample(const quint8 , const quint8);

    void  recvsycnViewCurvePara(int,bool);

    void  onPppInitRead(quint8 channelIndex, int pppValue);

private:
    static constexpr int MAX_CHANNELS = 12;
    static constexpr int MAX_REAGENTS = 5;
    static constexpr int HORIZONTAL_SPACING = 10;
    static constexpr int VERTICAL_SPACING = 10;
    static constexpr int DEFAULT_COLUMNS = 4;

    QWidget* m_widgetCurveInner;
    QVBoxLayout* m_mainLayout;

    QMap<quint8,QCPGraph*>   mcurveWidgetList;
    QMap<quint8,QCPTextElement* > m_titemap;

    QList<QWidget* > m_pchnWidgetList;
    QList<QCustomPlot* > m_customPlotList;
    std::array<bool, MAX_CHANNELS> m_pppAbnormal{};
    std::array<QString, MAX_CHANNELS> m_lastTitleText;

    mutable QHash<int, QCustomPlot*> m_plotCache;

    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mTestDataX;
    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mTestDataY;

    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mCurvePlote_x;
    std::array<std::array<QVector<double>, MAX_REAGENTS>, MAX_CHANNELS> mCurvePlote_y;

    int  mcuteNumData;
    bool mbaverage;

};

#endif // GRAPHPLOT_H
