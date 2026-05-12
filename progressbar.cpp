#include "progressbar.h"
#include <QPainter>
#include <QPointF>
#include <QtMath>
#include <qDebug>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

ProgressBar::ProgressBar(QWidget *parent, BarStyle style) : QWidget(parent)
        , m_min(0), m_max(100), m_value(0), m_startAngel(90), m_barStyle(style)
        , m_outlinePenWidth(0), m_dataPenWidth(0), m_decimals(0),m_ChannelIndex(0)
        ,m_bReagentshow(false),mtimer(nullptr),m_blinkState(false), m_flash(false)
{
    // 移除默认样式，完全自定义绘制
    this->setStyleSheet("background: transparent;");

    mtimer = new QTimer(this);
    connect(mtimer, &QTimer::timeout, this, &ProgressBar::toggleBlinkState);
    mtimer->setInterval(500);
    mtimer->start();
}

ProgressBar::~ProgressBar()
{
    if(mtimer->isActive()){
        mtimer->stop();
        delete mtimer;
        mtimer = nullptr;
    }
}

void  ProgressBar::toggleBlinkState(){
    if (m_flash) {
        m_blinkState = !m_blinkState;
        update();
    }
}

void ProgressBar::setStartAngle(double angle)
{
    if (angle != m_startAngel)
    {
        m_startAngel = angle;
        update();
    }
}

void ProgressBar::setOutlinePenWidth(double penWidth)
{
    if (penWidth != m_outlinePenWidth)
    {
        m_outlinePenWidth = penWidth;
        update();
    }
}

void ProgressBar::setDataPenWidth(double penWidth)
{
    if (penWidth != m_dataPenWidth)
    {
        m_dataPenWidth = penWidth;
        update();
    }
}

void ProgressBar::setDecimals(int count)
{
    if (count >= 0 && count != m_decimals)
    {
        m_decimals = count;
        update();
    }
}

void ProgressBar::setBarStyle(BarStyle style)
{
    if (style != m_barStyle)
    {
        m_barStyle = style;
        update();
    }
}

void ProgressBar::setRange(double min, double max)
{
    m_min = min;
    m_max = max;

    if (m_max < m_min)
    {
        qSwap(m_max, m_min);
    }

    if (m_value < m_min)
    {
        m_value = m_min;
    }
    else if (m_value > m_max)
    {
        m_value = m_max;
    }

    update();
}

void ProgressBar::setValue(double val)
{
    if (m_value != val)
    {
        if (val < m_min)
        {
            m_value = m_min;
        }
        else if (val > m_max)
        {
            m_value = m_max;
        }
        else
        {
            m_value = val;
        }
        update();
    }
}

void ProgressBar::setValue(int val)
{
    QMutexLocker locker(&mutex);
    setValue(double(val));
}

void ProgressBar::setMinimum(double min)
{
    setRange(min, m_max);
}

void ProgressBar::setMaximum(double max)
{
    setRange(m_min, max);
}

void ProgressBar::setChnTextindex(int index)
{
    m_ChannelIndex = index;
}

void ProgressBar::setReagentLast(QString text)
{
    m_LastReagnets = text;
}

void ProgressBar::setwidth(int width)
{
    m_diameter = width;
}

void ProgressBar::paintEvent(QPaintEvent* /*event*/)
{
    if(!m_blinkState){
        return;
    }

    // 外圈直径 - 使用整个控件宽度
    double outerDiameter = this->width();
    double centerX = this->width() / 2.0;
    double centerY = this->height() / 2.0;

    // 外圈矩形（居中绘制）
    QRectF baseRect(centerX - outerDiameter/2, centerY - outerDiameter/2,
                    outerDiameter, outerDiameter);

    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform |
                     QPainter::TextAntialiasing);

    // 内圆直径
    double innerDiameter = 0;

    // 计算内圈矩形
    if (m_barStyle == StyleLine)
    {
        innerDiameter = outerDiameter - m_outlinePenWidth;
    }
    else if(m_barStyle == StyleDonut)
    {
        innerDiameter = outerDiameter * 0.7;  // 稍微增大内圈，使圆环更宽
    }
    double delta = (outerDiameter - innerDiameter) / 2;
    QRectF innerRect(baseRect.x() + delta, baseRect.y() + delta,
                     innerDiameter, innerDiameter);

    // 绘制阴影背景
    drawShadowBackground(p, baseRect);

    // 绘制基础图形
    drawBase(p, baseRect, innerRect);

    // 计算当前步长比例
    double arcStep = 360.0 / (m_max - m_min) * m_value;

    // 根据值画出进度条
    drawValue(p, baseRect, m_value, arcStep, innerRect);

    // 绘制文字（在内圈区域绘制，显示在圆环中间）
    drawText(p, this->rect(), m_value);

    p.end();
}

void ProgressBar::drawShadowBackground(QPainter &p, const QRectF &baseRect)
{
    // 绘制柔和的阴影效果
    QPainterPath shadowPath;
    shadowPath.addEllipse(baseRect.adjusted(2, 2, 2, 2));

    QColor shadowColor(0, 0, 0, 20);
    p.setPen(Qt::NoPen);
    p.setBrush(shadowColor);
    p.drawPath(shadowPath);
}

void ProgressBar::drawBase(QPainter &p, const QRectF &baseRect, const QRectF &innerRect)
{
    switch (m_barStyle)
    {
        case StyleDonut:
        {
            // 绘制环形背景（空心圆环）
            QPainterPath ringPath;
            ringPath.setFillRule(Qt::OddEvenFill);
            ringPath.addEllipse(baseRect);
            ringPath.addEllipse(innerRect);

            // 环形背景渐变
            QLinearGradient ringGradient(baseRect.topLeft(), baseRect.bottomRight());
            ringGradient.setColorAt(0, QColor("#E5E7EB"));
            ringGradient.setColorAt(1, QColor("#D1D5DB"));

            p.setPen(Qt::NoPen);
            p.setBrush(ringGradient);
            p.drawPath(ringPath);

            // 绘制内圈和外圈边框
            QPen borderPen(QColor("#9CA3AF"), 1);
            p.setPen(borderPen);
            p.setBrush(Qt::NoBrush);
            p.drawEllipse(baseRect);
            p.drawEllipse(innerRect);

            // 根据通道状态设置中心区域颜色
            QColor centerColor;
            if (m_ChannelIndex != -1) {
                // 正常状态：白色背景
                centerColor = QColor("#FFFFFF");
            } else {
                // 禁用状态：医疗红色背景
                centerColor = QColor("#DC2626");
            }

            p.setPen(Qt::NoPen);
            p.setBrush(centerColor);
            p.drawEllipse(innerRect);

            break;
        }

        case StylePie:
        {
            if (m_ChannelIndex != -1) {
                // 正常状态：浅色背景
                QLinearGradient gradient(baseRect.topLeft(), baseRect.bottomRight());
                gradient.setColorAt(0, QColor("#F5F7FA"));
                gradient.setColorAt(1, QColor("#E4E8EC"));
                p.setPen(QPen(QColor("#D0D5D9"), 1));
                p.setBrush(gradient);
            } else {
                // 禁用状态：深色背景
                p.setPen(QPen(QColor("#555555"), 1));
                p.setBrush(QBrush(QColor("#2C2C2C")));
            }
            p.drawEllipse(baseRect);
            break;
        }

        case StyleLine:
        {
            if (m_ChannelIndex != -1) {
                QLinearGradient gradient(baseRect.topLeft(), baseRect.bottomRight());
                gradient.setColorAt(0, QColor("#F5F7FA"));
                gradient.setColorAt(1, QColor("#E4E8EC"));
                p.setPen(QPen(QColor("#D0D5D9"), m_outlinePenWidth));
                p.setBrush(gradient);
            } else {
                p.setPen(QPen(QColor("#555555"), m_outlinePenWidth));
                p.setBrush(QBrush(QColor("#2C2C2C")));
            }
            p.drawEllipse(baseRect.adjusted(m_outlinePenWidth / 2, m_outlinePenWidth / 2,
                                            -m_outlinePenWidth / 2, -m_outlinePenWidth / 2));
            break;
        }

        default:
        {
            break;
        }
    }
}

void ProgressBar::drawValue(QPainter &p, const QRectF &baseRect , double value, double arcLength
                          , const QRectF & innerRect)
{
    if (value == m_min)
    {
        return;
    }

    // 根据进度值选择颜色（医疗设备常用配色 - 使用更现代的配色）
    QColor progressColor;
    if (value < 33) {
        progressColor = QColor("#3B82F6");    // 现代蓝色 - 初期
    } else if (value < 66) {
        progressColor = QColor("#22C55E");    // 现代绿色 - 中期
    } else {
        progressColor = QColor("#F97316");    // 现代橙色 - 接近完成
    }

    if (m_barStyle == StyleLine)
    {
        // 绘制多层发光效果
        p.save();
        p.setPen(QPen(progressColor, 6));
        p.setBrush(Qt::NoBrush);
        p.setOpacity(0.15);
        p.drawArc(baseRect, m_startAngel * 16, -arcLength * 16);

        p.setPen(QPen(progressColor, 4));
        p.setOpacity(0.3);
        p.drawArc(baseRect, m_startAngel * 16, -arcLength * 16);

        p.setPen(QPen(progressColor, 2));
        p.setOpacity(1.0);
        p.drawArc(baseRect, m_startAngel * 16, -arcLength * 16);
        p.restore();
    }
    else if (m_barStyle == StyleDonut)
    {
        // 创建进度条扇形区域（填充整个灰色圆环）
        QPainterPath progressPath;
        progressPath.setFillRule(Qt::WindingFill);

        // 获取中心点
        QPointF center = baseRect.center();

        // 计算起始角度和结束角度对应的点（从顶部开始逆时针）
        double startAngleRad = qDegreesToRadians(m_startAngel);
        double endAngleRad = qDegreesToRadians(m_startAngel - arcLength);

        // 外圈半径
        double outerRadius = baseRect.width() / 2;
        // 内圈半径
        double innerRadius = innerRect.width() / 2;

        // 起始点（外圈）
        QPointF startOuter(center.x() + outerRadius * qCos(startAngleRad),
                          center.y() - outerRadius * qSin(startAngleRad));
        // 结束点（外圈）
        QPointF endOuter(center.x() + outerRadius * qCos(endAngleRad),
                        center.y() - outerRadius * qSin(endAngleRad));
        // 起始点（内圈）
        QPointF startInner(center.x() + innerRadius * qCos(startAngleRad),
                          center.y() - innerRadius * qSin(startAngleRad));
        // 结束点（内圈）
        QPointF endInner(center.x() + innerRadius * qCos(endAngleRad),
                        center.y() - innerRadius * qSin(endAngleRad));

        // 构建路径：外圈圆弧 -> 连接到内圈 -> 内圈反向圆弧 -> 连接回起点
        progressPath.moveTo(startOuter);
        progressPath.arcTo(baseRect, m_startAngel, -arcLength);
        progressPath.lineTo(endInner);
        progressPath.arcTo(innerRect, m_startAngel - arcLength, arcLength);
        progressPath.lineTo(startOuter);

        // 创建渐变填充
        QLinearGradient progressGradient(startOuter, endOuter);
        progressGradient.setColorAt(0, progressColor.lighter(120));
        progressGradient.setColorAt(1, progressColor.darker(110));

        // 绘制进度条
        p.setPen(Qt::NoPen);
        p.setBrush(progressGradient);
        p.drawPath(progressPath);

        // 添加边框
        p.setPen(QPen(progressColor.darker(130), 1));
        p.setBrush(Qt::NoBrush);
        p.drawPath(progressPath);
    }
    else
    {
        // 获取中心点坐标
        QPointF centerPoint = baseRect.center();
        QPainterPath dataPath;
        dataPath.setFillRule(Qt::WindingFill);
        dataPath.moveTo(centerPoint);
        // 逆时针画弧长
        dataPath.arcTo(baseRect, m_startAngel, -arcLength);
        if (m_barStyle == StylePie)
        {
            dataPath.lineTo(centerPoint);
            p.setPen(QPen(progressColor, m_dataPenWidth));
        }

        // 使用更平滑的径向渐变
        QRadialGradient pieGradient(centerPoint, baseRect.width()/2);
        pieGradient.setColorAt(0, progressColor.lighter(130));
        pieGradient.setColorAt(0.5, progressColor);
        pieGradient.setColorAt(1, progressColor.darker(120));
        p.setBrush(pieGradient);
        p.drawPath(dataPath);
    }
}

void ProgressBar::drawText(QPainter &p, const QRectF &rect, double value)
{
    // 使用更现代的字体
    QFont f;
    f.setFamily("Microsoft YaHei");
    f.setPixelSize(16);
    f.setWeight(QFont::Medium);
    p.setFont(f);

    QString textToDraw = "%";
    QString Channel;
    double percent = (value - m_min) / (m_max - m_min) * 100.0;

    if (m_bReagentshow)
    {
        Channel = QString("%1").arg(m_LastReagnets);
    }
    else
    {
        if(m_ChannelIndex != -1){
            Channel = QString("%1%2").arg(tr("通道")).arg(m_ChannelIndex);
        }else{
            Channel = QString("%1").arg(tr("禁用"));
        }
    }

    // 绘制百分比数字
    QString percentText = QString::number(percent, 'f', m_decimals) + "%";

    // 绘制百分比（合适大小，确保100%能完整显示）
    QFont percentFont = f;
    percentFont.setPixelSize(18);
    percentFont.setWeight(QFont::Bold);
    p.setFont(percentFont);

    // 根据通道状态和进度值设置百分比颜色
    QColor percentColor;
    if (m_ChannelIndex != -1) {
        // 正常状态：根据进度值调整颜色
        if (value < 33) {
            percentColor = QColor("#2E7DC8");
        } else if (value < 66) {
            percentColor = QColor("#5D9A4E");
        } else {
            percentColor = QColor("#D99228");
        }
    } else {
        // 禁用状态：白色文字（在红色背景上提高对比度）
        percentColor = QColor("#FFFFFF");
    }
    p.setPen(percentColor);

    // 计算控件中心位置
    QPointF center(rect.center());

    // 绘制百分比（居中显示在整个控件中心）
    p.drawText(rect, Qt::AlignCenter, percentText);

    // 绘制通道号（字体增大，与百分比更协调）
    QFont channelFont = f;
    channelFont.setPixelSize(12);
    channelFont.setWeight(QFont::Medium);
    p.setFont(channelFont);

    // 根据通道状态设置文字颜色
    if(m_ChannelIndex != -1){
        // 正常状态：深灰色文字
        p.setPen(QColor("#4B5563"));
    }else{
        // 禁用状态：白色文字（在红色背景上提高对比度）
        p.setPen(QColor("#FFFFFF"));
    }

    // 通道号在百分比下面显示（从中心向下偏移）
    QRectF channelRect(rect.x(), center.y() + 10, rect.width(), rect.height() * 0.55);
    p.drawText(channelRect, Qt::AlignTop | Qt::AlignHCenter, Channel);
}

void ProgressBar::SetShowLastReagent(bool showlast)
{
    m_bReagentshow = showlast;
}

void ProgressBar::flashingReminder(const bool isvisible)
{
    if(!isvisible){
        m_flash = true;
    }else{
        m_flash = false;
        m_blinkState = true;
    }
}
