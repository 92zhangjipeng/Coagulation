#ifndef MEDICALCHARTWIDGET_H
#define MEDICALCHARTWIDGET_H

#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QMap>
#include <QColor>
#include <QTimer>

class MedicalChartWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)

public:
    explicit MedicalChartWidget(QWidget *parent = nullptr);
    ~MedicalChartWidget();

    // 多曲线接口
    void addCurve(const QString &name, const QColor &color);
    void removeCurve(const QString &name);
    void setCurveData(const QString &name, const QVector<double> &xData, const QVector<double> &yData);
    void addDataPoint(const QString &name, double x, double y);
    void clearCurve(const QString &name);
    void clearAllCurves();

    // 全局设置
    void setTitle(const QString &title);
    QString title() const { return m_title; }
    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const { return m_backgroundColor; }
    void setAutoUpdate(bool enable);
    bool autoUpdate() const { return m_autoUpdate; }

    // 数据范围设置（可选）
    void setFixedXRange(double minX, double maxX);
    void setFixedYRange(double minY, double maxY);
    void setAutoScaleY(bool autoScale);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct CurveData {
        QVector<QPointF> points;
        QColor color;
        bool visible;
    };
    QMap<QString, CurveData> m_curves;

    QString m_title;
    QColor m_backgroundColor;
    bool m_autoUpdate;
    QTimer *m_updateTimer;

    bool m_autoScaleY;
    double m_fixedXMin, m_fixedXMax;
    double m_fixedYMin, m_fixedYMax;

    void drawBackground(QPainter &painter);
    void drawGrid(QPainter &painter);
    void drawAxes(QPainter &painter);
    void drawCurves(QPainter &painter);
    void drawLabels(QPainter &painter);
    void updateGeometryData(int &marginLeft, int &marginRight, int &marginTop, int &marginBottom,
                            int &plotWidth, int &plotHeight) const;
};

#endif // MEDICALCHARTWIDGET_H
