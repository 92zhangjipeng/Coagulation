#include "circleprogress.h"
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <complex>

CircleProgress::CircleProgress(QWidget *parent) : QWidget(parent)
{
    m_value = 0;
    m_width = 8;
    m_color = QColor(20, 135, 184);
    m_fontSize = 15;
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
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);

    const int captionH = 16;
    const int side = qMax(28, qMin(width() - 8, height() - captionH - 8));
    const qreal left = (width() - side) / 2.0;
    const qreal top = qMax(2.0, (height() - captionH - side) / 2.0);
    const qreal ring = qBound(5.5, side * 0.11, 8.0);
    const QRectF arcRect(left + ring / 2.0, top + ring / 2.0,
                         side - ring, side - ring);
    const QPointF center = arcRect.center();

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawEllipse(center, arcRect.width() / 2.0 - ring / 2.0,
                        arcRect.height() / 2.0 - ring / 2.0);

    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(197, 214, 226), ring, Qt::SolidLine, Qt::FlatCap));
    painter.drawEllipse(arcRect);

    const int shown = qBound(0, m_value, 100);
    if (shown > 0) {
        painter.setPen(QPen(m_color, ring, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(arcRect, 90 * 16, -shown * 36 * 16 / 10);
    }

    QFont font(QStringLiteral("Microsoft YaHei"));
    font.setPixelSize(qBound(12, qMin(m_fontSize, side / 5), 16));
    painter.setFont(font);
    painter.setPen(QColor(36, 59, 74));
    painter.drawText(arcRect, Qt::AlignCenter, QStringLiteral("%1%").arg(shown));

    QFont caption(QStringLiteral("Microsoft YaHei"));
    caption.setPixelSize(11);
    painter.setFont(caption);
    painter.setPen(QColor(92, 115, 128));
    const QRectF captionRect(0, top + side + 1, width(), captionH);
    painter.drawText(captionRect, Qt::AlignHCenter | Qt::AlignTop,
                     QString::fromUtf8("\xe6\x80\xbb\xe8\xbf\x9b\xe5\xba\xa6"));
}





