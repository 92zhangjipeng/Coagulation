#include "circlewidget.h"
#include <QPainter>
#include <QMouseEvent>

CircleWidget::CircleWidget(int channelNumber, QWidget *parent)
    : QWidget(parent), m_channelNumber(channelNumber), m_active(false), m_exclusiveMode(false)
{
    setFixedSize(60, 60);
    m_activeColor = QColor("#5BB8E8");
    m_inactiveColor = QColor("#D4EAF5");
}

void CircleWidget::setExclusiveMode(bool exclusive)
{
    m_exclusiveMode = exclusive;
}

void CircleWidget::setActive(bool active)
{
    if (m_active != active) {
        m_active = active;
        update();
    }
}

void CircleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制外圆环
    QPen pen;
    pen.setWidth(3);
    pen.setColor(m_active ? m_activeColor : m_inactiveColor);
    painter.setPen(pen);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(m_active ? m_activeColor : m_inactiveColor);
    painter.setBrush(brush);

    // 绘制环形圆（外圆+内圆透明）
    int outerRadius = width() / 2 - 5;
    int innerRadius = outerRadius - 8;

    QPoint center(width() / 2, height() / 2);

    // 绘制外圆
    painter.drawEllipse(center, outerRadius, outerRadius);

    // 绘制内圆（透明）
    painter.setBrush(Qt::NoBrush);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(255, 255, 255, 200)));
    painter.drawEllipse(center, innerRadius, innerRadius);

    // 绘制通道编号
    painter.setPen(QPen(QColor("#2C5F8A")));
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    painter.drawText(rect(), Qt::AlignCenter, QString::number(m_channelNumber));
}

void CircleWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // 无论是否互斥模式，都发出点击信号
        // 互斥逻辑由外部处理
        emit clicked(m_channelNumber);
    }
    QWidget::mousePressEvent(event);
}
