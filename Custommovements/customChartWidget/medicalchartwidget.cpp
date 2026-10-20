#include "medicalchartwidget.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QColor>
#include <QResizeEvent>
#include <QtMath>
#include <limits>

MedicalChartWidget::MedicalChartWidget(QWidget *parent)
    : QWidget(parent),
      m_title(""),
      m_backgroundColor("#f5f5f5"),
      m_autoUpdate(false),
      m_autoScaleY(true),
      m_fixedXMin(0), m_fixedXMax(300),
      m_fixedYMin(-20), m_fixedYMax(100)
{
    setMinimumSize(200, 150);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setBackgroundRole(QPalette::Window);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, m_backgroundColor);
    setPalette(pal);

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer, &QTimer::timeout, this, [this]() {
        if (m_autoUpdate) update();
    });
}

MedicalChartWidget::~MedicalChartWidget()
{
}

void MedicalChartWidget::addCurve(const QString &name, const QColor &color)
{
    if (!m_curves.contains(name)) {
        CurveData data;
        data.color = color;
        data.visible = true;
        m_curves.insert(name, data);
    }
}

void MedicalChartWidget::removeCurve(const QString &name)
{
    m_curves.remove(name);
}

void MedicalChartWidget::setCurveData(const QString &name, const QVector<double> &xData, const QVector<double> &yData)
{
    if (!m_curves.contains(name)) return;
    int size = qMin(xData.size(), yData.size());
    QVector<QPointF> points;
    points.reserve(size);
    for (int i = 0; i < size; ++i) {
        points.append(QPointF(xData[i], yData[i]));
    }
    m_curves[name].points = points;
    if (m_autoUpdate) update();
}

void MedicalChartWidget::addDataPoint(const QString &name, double x, double y)
{
    if (!m_curves.contains(name)) return;
    m_curves[name].points.append(QPointF(x, y));
    if (m_autoUpdate) update();
}

void MedicalChartWidget::clearCurve(const QString &name)
{
    if (m_curves.contains(name)) {
        m_curves[name].points.clear();
        if (m_autoUpdate) update();
    }
}

void MedicalChartWidget::clearAllCurves()
{
    for (auto it = m_curves.begin(); it != m_curves.end(); ++it) {
        it->points.clear();
    }
    update();
}

void MedicalChartWidget::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void MedicalChartWidget::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    QPalette pal = palette();
    pal.setColor(QPalette::Window, m_backgroundColor);
    setPalette(pal);
    update();
}

void MedicalChartWidget::setAutoUpdate(bool enable)
{
    m_autoUpdate = enable;
    if (m_autoUpdate) {
        m_updateTimer->start(100);
    } else {
        m_updateTimer->stop();
    }
}

void MedicalChartWidget::setFixedXRange(double minX, double maxX)
{
    m_fixedXMin = minX;
    m_fixedXMax = maxX;
    update();
}

void MedicalChartWidget::setFixedYRange(double minY, double maxY)
{
    m_fixedYMin = minY;
    m_fixedYMax = maxY;
    m_autoScaleY = false;
    update();
}

void MedicalChartWidget::setAutoScaleY(bool autoScale)
{
    m_autoScaleY = autoScale;
    if (autoScale) update();
}

QSize MedicalChartWidget::sizeHint() const
{
    return QSize(400, 300);
}

QSize MedicalChartWidget::minimumSizeHint() const
{
    return QSize(200, 150);
}

void MedicalChartWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

void MedicalChartWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    drawBackground(painter);
    drawGrid(painter);
    drawAxes(painter);
    drawCurves(painter);
    drawLabels(painter);
}

void MedicalChartWidget::updateGeometryData(int &marginLeft, int &marginRight,
                                            int &marginTop, int &marginBottom,
                                            int &plotWidth, int &plotHeight) const
{
    const double scale = qMin(width() / 800.0, height() / 500.0);
    marginLeft = qMax(70, static_cast<int>(120 * scale));
    marginRight = qMax(15, static_cast<int>(30 * scale));
    marginTop = qMax(25, static_cast<int>(50 * scale));
    marginBottom = qMax(35, static_cast<int>(70 * scale));
    plotWidth = width() - marginLeft - marginRight;
    plotHeight = height() - marginTop - marginBottom;
}

void MedicalChartWidget::drawBackground(QPainter &painter)
{
    painter.fillRect(rect(), m_backgroundColor);
}

void MedicalChartWidget::drawGrid(QPainter &painter)
{
    int marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight;
    updateGeometryData(marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight);
    if (plotWidth < 10 || plotHeight < 10) return;

    const double scale = qMin(width() / 800.0, height() / 500.0);
    painter.setPen(QPen(QColor("#e0e0e0"), qMax(1.0, scale), Qt::SolidLine));
    for (int i = 0; i <= 6; ++i) {
        int x = marginLeft + (i * plotWidth / 6);
        painter.drawLine(x, marginTop, x, height() - marginBottom);
        int y = marginTop + (i * plotHeight / 6);
        painter.drawLine(marginLeft, y, width() - marginRight, y);
    }
}

void MedicalChartWidget::drawAxes(QPainter &painter)
{
    int marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight;
    updateGeometryData(marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight);
    if (plotWidth < 10 || plotHeight < 10) return;

    const double scale = qMin(width() / 800.0, height() / 500.0);
    painter.setPen(QPen(QColor("#333333"), qMax(2.0, scale * 2), Qt::SolidLine));
    painter.drawLine(marginLeft, marginTop, marginLeft, height() - marginBottom);
    painter.drawLine(marginLeft, height() - marginBottom, width() - marginRight, height() - marginBottom);

    painter.setPen(QPen(QColor("#333333"), qMax(1.0, scale), Qt::SolidLine));
    const int arrowSize = qMax(4, static_cast<int>(8 * scale));
    const int tickSize = qMax(3, static_cast<int>(6 * scale));
    painter.drawLine(width() - marginRight, height() - marginBottom,
                     width() - marginRight - arrowSize, height() - marginBottom - arrowSize/2);
    painter.drawLine(width() - marginRight, height() - marginBottom,
                     width() - marginRight - arrowSize, height() - marginBottom + arrowSize/2);
    painter.drawLine(marginLeft, marginTop, marginLeft - arrowSize/2, marginTop + arrowSize);
    painter.drawLine(marginLeft, marginTop, marginLeft + arrowSize/2, marginTop + arrowSize);

    for (int i = 0; i <= 6; ++i) {
        int x = marginLeft + (i * plotWidth / 6);
        painter.drawLine(x, height() - marginBottom, x, height() - marginBottom + tickSize);
        int y = marginTop + (i * plotHeight / 6);
        painter.drawLine(marginLeft - tickSize, y, marginLeft, y);
    }
}

void MedicalChartWidget::drawCurves(QPainter &painter)
{
    int marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight;
    updateGeometryData(marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight);
    if (plotWidth < 10 || plotHeight < 10) return;

    // 确定 X 范围
    double minX = m_fixedXMin, maxX = m_fixedXMax;
    // 确定 Y 范围（如需自动缩放）
    double minY = m_fixedYMin, maxY = m_fixedYMax;
    if (m_autoScaleY) {
        bool hasData = false;
        double globalMinY = std::numeric_limits<double>::max();
        double globalMaxY = std::numeric_limits<double>::lowest();
        for (const auto &curve : m_curves) {
            for (const auto &p : curve.points) {
                double y = p.y();
                if (!std::isnan(y) && !std::isinf(y)) {
                    globalMinY = qMin(globalMinY, y);
                    globalMaxY = qMax(globalMaxY, y);
                    hasData = true;
                }
            }
        }
        if (hasData) {
            minY = globalMinY;
            maxY = globalMaxY;
            double margin = (maxY - minY) * 0.1;
            if (margin < 1.0) margin = 5.0;
            minY -= margin;
            maxY += margin;
            if (maxY - minY < 1.0) {
                minY -= 5.0;
                maxY += 5.0;
            }
        }
    }

    painter.setPen(QPen(QColor("#000000"), qMax(2.0, qMin(width() / 800.0, height() / 500.0) * 2.5),
                        Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for (auto it = m_curves.begin(); it != m_curves.end(); ++it) {
        const QVector<QPointF> &points = it->points;
        if (points.size() < 2) continue;
        painter.setPen(QPen(it->color, qMax(2.0, qMin(width() / 800.0, height() / 500.0) * 2.5),
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPointF prevScreen(-1, -1);
        for (const QPointF &p : points) {
            double x = marginLeft + ((p.x() - minX) / (maxX - minX)) * plotWidth;
            double y = height() - marginBottom - ((p.y() - minY) / (maxY - minY)) * plotHeight;
            QPointF screen(x, y);
            if (prevScreen.x() >= 0)
                painter.drawLine(prevScreen, screen);
            prevScreen = screen;
        }
    }
}

void MedicalChartWidget::drawLabels(QPainter &painter)
{
    int marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight;
    updateGeometryData(marginLeft, marginRight, marginTop, marginBottom, plotWidth, plotHeight);
    if (plotWidth < 10 || plotHeight < 10) return;

    const double scale = qMin(width() / 800.0, height() / 500.0);
    const int yLabelWidth = qMax(30, static_cast<int>(45 * scale));
    const int tickLabelWidth = marginLeft - yLabelWidth - 10;

    QFont font = painter.font();
    font.setFamily("Microsoft YaHei");
    font.setPointSize(qMax(8, static_cast<int>(13 * scale)));
    painter.setFont(font);
    painter.setPen(QColor("#333333"));

    // X 轴刻度
    double minX = m_fixedXMin, maxX = m_fixedXMax;
    for (int i = 0; i <= 6; ++i) {
        int x = marginLeft + (i * plotWidth / 6);
        double value = minX + (i * (maxX - minX) / 6);
        QString label = QString::number(value, 'f', 0);
        int labelWidth = qMax(20, static_cast<int>(50 * scale));
        QRect textRect(x - labelWidth/2, height() - marginBottom + static_cast<int>(6 * scale),
                       labelWidth, static_cast<int>(25 * scale));
        painter.drawText(textRect, Qt::AlignCenter, label);
    }

    // Y 轴刻度
    double minY = m_fixedYMin, maxY = m_fixedYMax;
    if (m_autoScaleY) {
        // 动态计算用于标签的最小/最大值（基于实际数据范围）
        bool hasData = false;
        double globalMinY = std::numeric_limits<double>::max();
        double globalMaxY = std::numeric_limits<double>::lowest();
        for (const auto &curve : m_curves) {
            for (const auto &p : curve.points) {
                double y = p.y();
                if (!std::isnan(y) && !std::isinf(y)) {
                    globalMinY = qMin(globalMinY, y);
                    globalMaxY = qMax(globalMaxY, y);
                    hasData = true;
                }
            }
        }
        if (hasData) {
            minY = globalMinY;
            maxY = globalMaxY;
            double margin = (maxY - minY) * 0.1;
            if (margin < 1.0) margin = 5.0;
            minY -= margin;
            maxY += margin;
            if (maxY - minY < 1.0) {
                minY -= 5.0;
                maxY += 5.0;
            }
        }
    }
    for (int i = 0; i <= 6; ++i) {
        int y = marginTop + (i * plotHeight / 6);
        double value = maxY - (i * (maxY - minY) / 6);
        QString label = QString::number(value, 'f', 0);
        int labelWidth = qMax(20, tickLabelWidth);
        QRect textRect(yLabelWidth, y - static_cast<int>(12 * scale), labelWidth, static_cast<int>(24 * scale));
        painter.drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, label);
    }

    // 轴标签
    font.setPointSize(qMax(8, static_cast<int>(13 * scale)));
    font.setBold(true);
    painter.setFont(font);
    if (plotWidth > 50) {
        QRect xLabelRect(marginLeft, height() - static_cast<int>(35 * scale), plotWidth, static_cast<int>(25 * scale));
        painter.drawText(xLabelRect, Qt::AlignCenter, "时间 (秒)");
    }
    if (plotHeight > 50) {
        QRect yLabelRect(0, marginTop - static_cast<int>(20 * scale), yLabelWidth, plotHeight + static_cast<int>(40 * scale));
        painter.save();
        painter.translate(yLabelRect.center().x(), yLabelRect.center().y());
        painter.rotate(-90);
        painter.drawText(-static_cast<int>(60 * scale), 0, static_cast<int>(120 * scale), static_cast<int>(25 * scale), Qt::AlignCenter, "聚集率 (%)");
        painter.restore();
    }

    // 标题
    if (!m_title.isEmpty() && plotWidth > 50) {
        font.setPointSize(qMax(9, static_cast<int>(16 * scale)));
        font.setBold(true);
        painter.setFont(font);
        painter.setPen(QColor("#111111"));
        QRect titleRect(marginLeft, static_cast<int>(6 * scale), plotWidth, static_cast<int>(35 * scale));
        painter.drawText(titleRect, Qt::AlignCenter, m_title);
    }
}
