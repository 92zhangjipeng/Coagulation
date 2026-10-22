#ifndef QCCHARTPAGE_H
#define QCCHARTPAGE_H

#include <QWidget>

#include <QList>
#include <QVector>

class QButtonGroup;
class QComboBox;
class QCPItemStraightLine;
class QCPItemText;
class QCPTextElement;
class QCustomPlot;
class QLabel;
class QMouseEvent;
class QPushButton;

// QC page shell. Each channel keeps one high, mid and low point per day.
class QcChartPage : public QWidget
{
    Q_OBJECT

public:
    enum Level {
        LevelHigh = 0,
        LevelMid = 1,
        LevelLow = 2,
        LevelAll = 3
    };

    explicit QcChartPage(QWidget *parent = nullptr);

    int channelNumber() const;
    Level level() const;

private slots:
    void onLevelClicked(int id);
    void onStartClicked();
    void onQcProgress(const QString &text);
    void onQcFinished(bool ok, const QString &text);
    void onLotButtonClicked();
    void showResultDialog(const QString &text);
    void reloadChannelCurves();
    void onPlotMousePress(QMouseEvent *event);
    void onPlotMouseMove(QMouseEvent *event);
    void onPlotMouseRelease(QMouseEvent *event);

private:
    struct CurveView {
        QCustomPlot *plot;
        QCPTextElement *title;
        QCPItemStraightLine *cursor;
        QCPItemText *valueText;
        int centerPercent;
        const char *levelUtf8;
    };

    void buildCharts();
    void buildBottomBar();
    CurveView createChart(const QColor &pointColor, int centerPercent, const char *levelUtf8);
    void applyLevel(Level level);
    void updateChartTitles();
    void showRecentDays();
    void placeCursor(double key);
    bool nearCursor(QCustomPlot *plot, int pixelX) const;
    void panPlots(double lower);
    QList<int> selectedTestChannels() const;

    CurveView m_curve[3];
    QCustomPlot *m_highPlot;
    QCustomPlot *m_midPlot;
    QCustomPlot *m_lowPlot;
    QComboBox *m_channelBox;
    QButtonGroup *m_levelGroup;
    QVector<QPushButton *> m_testChannelButtons;
    QLabel *m_statusLabel;
    QPushButton *m_lotButton;
    QPushButton *m_startButton;
    QString m_highLot;
    QString m_midLot;
    QString m_lowLot;
    bool m_dragCursor;
    bool m_panX;
    bool m_pointerMoved;
    int m_pressX;
    double m_pressLower;
    double m_pressUpper;
};

#endif
