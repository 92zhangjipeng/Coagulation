#include "circleprogress.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <complex>

CircleProgress::CircleProgress(QWidget *parent) : QWidget(parent)
{
    m_value = 0;
    m_width = 20;
    m_color = QColor(0 ,191, 255);
    m_fontSize = 16;
}

void CircleProgress::setValue(int value)
{
    m_value = value;
    update();
}

void CircleProgress::setWidth(int width)
{
    m_width = width;
}

void CircleProgress::setColor(const QColor &color)
{
    m_color = color;
}

void CircleProgress::setFontPixSize(int size)
{
    m_fontSize = size;
}

void CircleProgress::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // 设置反走样
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 获取中心点
    float centerX = width() / 2;
    float centerY = height() / 2;

    // 外圆 灰色
    float outerRectW = qMin(width(), height());
    float outerRectX = centerX - outerRectW / 2;
    float outerRectY = centerY - outerRectW / 2;
    QRectF outerRect(outerRectX, outerRectY, outerRectW, outerRectW);

    QPen pen(Qt::NoPen);
    painter.setPen(pen);
    painter.setBrush(QColor("#BEBEBE"));
    painter.drawEllipse(outerRect);

    // 环状进度条
    int startAngle = 90 * 16;
    int spanAngle = -m_value * 3.6 * 16;
    painter.setBrush(m_color);
    painter.drawPie(outerRect, startAngle, spanAngle);

    // 内圆 窗口背景颜色为底色
    float innerRectX = outerRectX + m_width;
    float innerRectY = outerRectY + m_width;
    float innerRectW = outerRectW - m_width * 2;
    QRectF innerRect(innerRectX, innerRectY, innerRectW, innerRectW);
    painter.setBrush(palette().window());
    painter.drawEllipse(innerRect);

    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    pen.setColor(QColor("#FFFFFF"));
    painter.setPen(pen);
    QFont font = painter.font();
    font.setPixelSize(m_fontSize);
    painter.setFont(font);
    painter.drawText(outerRect, Qt::AlignCenter, QString("%1%").arg(m_value));
}





