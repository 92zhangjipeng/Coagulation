#include "QSimpleLed.h"
#include <QGradient>
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

// 定义绘制常量
constexpr qreal BASE_SIZE = 1000.0;
constexpr qreal OUTER_RADIUS = 500.0;
constexpr qreal MIDDLE_RADIUS = 450.0;
constexpr qreal INNER_RADIUS = 400.0;
constexpr qreal GRADIENT_RADIUS = 1500.0;
constexpr qreal LIGHT_SOURCE_OFFSET = 500.0;

// https://www.schemecolor.com/
QSimpleLed::ColorGroup QSimpleLed::smColorPalette[6]
{
    // CUSTOM
    { QColor(0, 0, 0), QColor(0, 0, 0), QColor(0, 0, 0), QColor(0, 0, 0) },
    // RED
    { QColor(255, 0, 0), QColor(191, 0, 0), QColor(28, 0, 0), QColor(128, 0, 0) },
    // GREEN
    { QColor(40 ,101, 205), QColor(40 ,101, 205), QColor(40 ,101, 205), QColor(20 ,50, 105) },
    // BLUE
    { QColor(0, 0, 255), QColor(0, 0, 191), QColor(0, 0, 28), QColor(0, 0, 128) },
    // YELLOW
    { QColor(255, 255, 0), QColor(191, 191, 0), QColor(28, 28, 0), QColor(128, 128, 0) },
    // ORANGE
    { QColor(255, 165, 0), QColor(255, 113, 1), QColor(20, 8, 5), QColor(99, 39, 24) },
};

QSimpleLed::QSimpleLed(QWidget *parent, QSimpleLed::LEDCOLOR color)
    : QAbstractButton(parent)
    , mColor(color)
{
    setCheckable(true);
    setMinimumSize(34, 34);
}

QSimpleLed::~QSimpleLed()
{
    // 清理定时器资源
    if (mBlinkTimer && mBlinkTimer->isActive()) {
        mBlinkTimer->stop();
    }
    delete mBlinkTimer;
    mBlinkTimer = nullptr;
}

void QSimpleLed::setColors(QSimpleLed::LEDCOLOR color)
{
    mColor = color;
}

void QSimpleLed::setStates(QSimpleLed::LEDSTATES states)
{
    switch (states) {
    case ON:
        resetStatus();
        setChecked(true);
        mStates = ON;
        break;

    case OFF:
        resetStatus();
        break;

    case BLINK:
        resetStatus();

        if (!mBlinkTimer) {
            mBlinkTimer = new QTimer(this);
            connect(mBlinkTimer, &QTimer::timeout, this, &QSimpleLed::onBlinkTimerTimeout);
        }
        mBlinkTimer->setInterval(mInterval);
        mBlinkTimer->start();
        mStates = BLINK;
        break;

    default:
        qDebug() << "LED - unknown states!!!";
    }

    update();
}

void QSimpleLed::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    qreal realSize = qMin(width(), height());

    QRadialGradient radialGent;

    // gradient - 1: 外层渐变（光源在左上角）
    radialGent = QRadialGradient(QPointF(-LIGHT_SOURCE_OFFSET, -LIGHT_SOURCE_OFFSET), 
                                 GRADIENT_RADIUS, 
                                 QPointF(-LIGHT_SOURCE_OFFSET, -LIGHT_SOURCE_OFFSET));
    radialGent.setColorAt(0, QColor(224, 224, 224));
    radialGent.setColorAt(1, QColor(28, 28, 28));

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);      // 反锯齿
    painter.translate(width()/2, height()/2);           // 绘点移到控件中心处
    painter.scale(realSize/BASE_SIZE, realSize/BASE_SIZE);
    painter.setBrush(QBrush(radialGent));
    painter.drawEllipse(QPointF(0, 0), OUTER_RADIUS, OUTER_RADIUS);

    // gradient - 2: 中层渐变（光源在右下角）
    radialGent = QRadialGradient(QPointF(LIGHT_SOURCE_OFFSET, LIGHT_SOURCE_OFFSET),
                                 GRADIENT_RADIUS,
                                 QPointF(LIGHT_SOURCE_OFFSET, LIGHT_SOURCE_OFFSET));
    radialGent.setColorAt(0, QColor(224, 224, 224));
    radialGent.setColorAt(1, QColor(28, 28, 28));

    painter.setBrush(QBrush(radialGent));
    painter.drawEllipse(QPointF(0, 0), MIDDLE_RADIUS, MIDDLE_RADIUS);

    if (isChecked()) {
        // gradient - 3: ON状态内层渐变（光源在左上角）
        radialGent = QRadialGradient(QPointF(-LIGHT_SOURCE_OFFSET, -LIGHT_SOURCE_OFFSET)
                                     , GRADIENT_RADIUS
                                     , QPointF(-LIGHT_SOURCE_OFFSET, -LIGHT_SOURCE_OFFSET));
        radialGent.setColorAt(0, smColorPalette[mColor].on0);
        radialGent.setColorAt(1, smColorPalette[mColor].on1);

    } else {
        // gradient - 4: OFF状态内层渐变（光源在右下角）
        radialGent = QRadialGradient(QPointF(LIGHT_SOURCE_OFFSET, LIGHT_SOURCE_OFFSET), 
                                     GRADIENT_RADIUS,
                                     QPointF(LIGHT_SOURCE_OFFSET, LIGHT_SOURCE_OFFSET));
        radialGent.setColorAt(0, smColorPalette[mColor].off0);
        radialGent.setColorAt(1, smColorPalette[mColor].off1);
    }

    painter.setBrush(QBrush(radialGent));

	painter.drawEllipse(QPointF(0, 0), INNER_RADIUS, INNER_RADIUS);

}

void QSimpleLed::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);

    update();
}

void QSimpleLed::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
}

void QSimpleLed::onBlinkTimerTimeout()
{
    setChecked(!isChecked());
}

void QSimpleLed::resetStatus()
{
    if (mBlinkTimer && mBlinkTimer->isActive()) {
        mBlinkTimer->stop();
    }

    setChecked(false);
    mStates = OFF;
}
