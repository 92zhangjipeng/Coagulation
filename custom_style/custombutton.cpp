#include "custombutton.h"
#include "ui_custombutton.h"
#include <QPainter>
#include <QPainterPath>

customButton::customButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::customButton)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAutoFillBackground(false);
    setStyleSheet(QStringLiteral("background: transparent; border: none;"));
    installEventFilter(this);
    setMouseTracking(true);

    m_fixWidth = 90;
    m_fixHeigh = 150;

    m_minValue = 0;
    m_maxValue = 100;
    m_value = 0;
    m_alarmValue = 1;
    m_step = 5;
    m_currentValue = 0;
    m_bgRadius = 16;
    m_isForward = false;

    borderColor = QColor(70, 110, 140);
    bottlecapColor = QColor(45, 62, 80);
    bottleneckColor = QColor(160, 190, 210);
    bottleColor = QColor(210, 230, 245, 90);

    m_borderColorStart = QColor(120, 160, 190, 160);
    m_borderColorEnd = QColor(70, 110, 145, 200);

    m_normalColorStart = QColor(72, 201, 176);
    m_normalColorEnd = QColor(32, 156, 140);

    m_alarmColorStart = QColor(255, 140, 120);
    m_alarmColorEnd = QColor(220, 60, 60);

    setFixedSize(m_fixWidth, m_fixHeigh);

    m_timer = new QTimer(this);
    m_timer->setInterval(12);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateValue()));
}

customButton::~customButton()
{
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    delete ui;
}

void customButton::_setfixwidth(int _width)
{
    m_fixWidth = _width;
}

void customButton::_setfixheigh(int _heigh)
{
    m_fixHeigh = _heigh;
}

void customButton::paintEvent(QPaintEvent* /*event*/)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    drawBottlewhole(&painter);
    drawBg(&painter);
    drawBottleText(&painter);
}

void customButton::drawBottlewhole(QPainter *painter)
{
    const int lineW = 2;

    // Cap
    const int bottleCapWidth = m_fixWidth * 2 / 7;
    const int bottleCapHeight = qMax(10, bottleCapWidth * 2 / 5);
    const int spaceTopY = m_fixHeigh / 22;
    const int spaceLeftX = (m_fixWidth - bottleCapWidth) / 2;
    QRect rectCap(spaceLeftX, spaceTopY, bottleCapWidth, bottleCapHeight);
    m_rectCap = rectCap;

    QLinearGradient capGrad(rectCap.topLeft(), rectCap.bottomLeft());
    capGrad.setColorAt(0.0, QColor(90, 115, 140));
    capGrad.setColorAt(0.45, bottlecapColor);
    capGrad.setColorAt(1.0, QColor(30, 42, 55));
    painter->setPen(QPen(QColor(25, 35, 48), 1));
    painter->setBrush(capGrad);
    painter->drawRoundedRect(rectCap, 4, 4);

    // Cap highlight
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, 70));
    painter->drawRoundedRect(QRect(rectCap.x() + 3, rectCap.y() + 2,
                                   rectCap.width() - 6, qMax(3, rectCap.height() / 3)), 2, 2);

    // Neck
    const int bottleNeckWidth = bottleCapWidth * 2 / 3;
    const int bottleNeckHeight = qMax(8, bottleCapHeight * 2 / 3);
    const int bottleneckLeftX = rectCap.center().x() - bottleNeckWidth / 2;
    const int bottleneckY = rectCap.bottom() + 1;
    QRect bottleneck(bottleneckLeftX, bottleneckY, bottleNeckWidth, bottleNeckHeight);
    m_bottleneck = bottleneck;

    QLinearGradient neckGrad(bottleneck.topLeft(), bottleneck.topRight());
    neckGrad.setColorAt(0.0, QColor(190, 215, 230, 180));
    neckGrad.setColorAt(0.5, QColor(235, 245, 250, 210));
    neckGrad.setColorAt(1.0, QColor(170, 200, 220, 180));
    painter->setPen(QPen(borderColor, lineW));
    painter->setBrush(neckGrad);
    painter->drawRoundedRect(bottleneck, 3, 3);

    // Body outline + glass fill
    const int bottlesWidth = m_fixWidth * 4 / 7;
    const int bottlesHeight = (m_fixHeigh - bottleneck.height() - rectCap.height()) * 3 / 4;
    const int offsetX = rectCap.center().x() - bottlesWidth / 2;
    const int bottleTop = bottleneck.bottom() + 1;
    QRect rectBottle(offsetX, bottleTop, bottlesWidth, bottlesHeight);
    m_rectBottle = rectBottle;

    QLinearGradient glassGrad(rectBottle.topLeft(), rectBottle.topRight());
    glassGrad.setColorAt(0.0, QColor(185, 215, 235, 70));
    glassGrad.setColorAt(0.35, QColor(245, 250, 255, 40));
    glassGrad.setColorAt(0.7, QColor(200, 225, 240, 55));
    glassGrad.setColorAt(1.0, QColor(150, 185, 210, 80));
    painter->setBrush(glassGrad);
    painter->setPen(QPen(borderColor, lineW));
    painter->drawRoundedRect(rectBottle, m_bgRadius, m_bgRadius);

    // Left glass shine
    QRect shineRect(rectBottle.x() + 5, rectBottle.y() + 8,
                    qMax(4, rectBottle.width() / 6), rectBottle.height() - 16);
    QLinearGradient shineGrad(shineRect.topLeft(), shineRect.topRight());
    shineGrad.setColorAt(0.0, QColor(255, 255, 255, 90));
    shineGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter->setPen(Qt::NoPen);
    painter->setBrush(shineGrad);
    painter->drawRoundedRect(shineRect, 4, 4);

    // Smooth neck-body join (same glass tone, no gray patch)
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(220, 235, 245, 220));
    painter->drawRect(m_bottleneck.x() + 1, m_bottleneck.bottom() - 1,
                      m_bottleneck.width() - 2, 4);
}

void customButton::drawBottleText(QPainter *painter)
{
    const QRect textArea = m_rectBottle.adjusted(4, 6, -4, -6);
    if (!textArea.isValid()) {
        return;
    }

    const bool lowLevel = (m_value <= m_alarmValue);
    const QColor nameColor = lowLevel ? QColor(90, 30, 30) : QColor(35, 55, 75);
    const QColor valueColor = lowLevel ? QColor(180, 40, 40) : QColor(25, 90, 110);

    QFont nameFont(QStringLiteral("Microsoft YaHei"), 9, QFont::DemiBold);
    QFont valueFont(QStringLiteral("Microsoft YaHei"), 9, QFont::Bold);
    if (m_reagreminder.length() >= 6) {
        nameFont.setPointSize(8);
    }

    painter->setFont(nameFont);
    painter->setPen(QColor(255, 255, 255, 180));
    painter->drawText(textArea.adjusted(1, 1, 1, 1), Qt::AlignHCenter | Qt::AlignTop, m_reagreminder);
    painter->setPen(nameColor);
    painter->drawText(textArea, Qt::AlignHCenter | Qt::AlignTop, m_reagreminder);

    const QString percentText = QString("%1%").arg(QString::number(m_value, 'f', 1));
    painter->setFont(valueFont);
    painter->setPen(QColor(255, 255, 255, 180));
    painter->drawText(textArea.adjusted(1, 1, 1, 1), Qt::AlignHCenter | Qt::AlignBottom, percentText);
    painter->setPen(valueColor);
    painter->drawText(textArea, Qt::AlignHCenter | Qt::AlignBottom, percentText);
}

void customButton::drawBg(QPainter *painter)
{
    if (qFuzzyIsNull(m_currentValue - m_minValue) || m_rectBottle.height() <= 0) {
        return;
    }

    painter->save();

    const double ratio = qBound(0.0, m_currentValue / qMax(1.0, m_maxValue - m_minValue), 1.0);
    const int inset = 2;
    QRect inner = m_rectBottle.adjusted(inset, inset, -inset, -inset);
    const int liquidH = qRound(inner.height() * ratio);
    if (liquidH <= 0) {
        painter->restore();
        return;
    }

    QRect liquidRect(inner.left(), inner.bottom() - liquidH + 1, inner.width(), liquidH);

    QPainterPath clipPath;
    clipPath.addRoundedRect(inner, m_bgRadius - 2, m_bgRadius - 2);
    painter->setClipPath(clipPath);

    QLinearGradient liquidGrad(liquidRect.topLeft(), liquidRect.bottomLeft());
    if (m_currentValue <= m_alarmValue) {
        liquidGrad.setColorAt(0.0, m_alarmColorStart);
        liquidGrad.setColorAt(0.55, m_alarmColorStart.lighter(115));
        liquidGrad.setColorAt(1.0, m_alarmColorEnd);
    } else {
        QColor top = m_normalColorStart;
        QColor mid = m_normalColorStart.lighter(118);
        QColor bottom = m_normalColorEnd;
        if (top == bottom) {
            bottom = top.darker(130);
        }
        // Keep pale colors (e.g. S1) readable
        if (top.lightness() > 220) {
            top = QColor(90, 200, 220);
            mid = QColor(130, 220, 235);
            bottom = QColor(50, 160, 185);
        }
        liquidGrad.setColorAt(0.0, top);
        liquidGrad.setColorAt(0.45, mid);
        liquidGrad.setColorAt(1.0, bottom);
    }

    painter->setPen(Qt::NoPen);
    painter->setBrush(liquidGrad);
    painter->drawRoundedRect(liquidRect, m_bgRadius - 2, m_bgRadius - 2);

    // Surface ripple highlight
    if (liquidH > 8) {
        QRect surface(liquidRect.left() + 2, liquidRect.top(), liquidRect.width() - 4, 6);
        QLinearGradient surfaceGrad(surface.topLeft(), surface.bottomLeft());
        surfaceGrad.setColorAt(0.0, QColor(255, 255, 255, 120));
        surfaceGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
        painter->setBrush(surfaceGrad);
        painter->drawRect(surface);
    }

    // Side caustic
    QRect caustic(liquidRect.left() + 3, liquidRect.top() + 4,
                  qMax(3, liquidRect.width() / 5), qMax(4, liquidRect.height() - 8));
    QLinearGradient causticGrad(caustic.topLeft(), caustic.topRight());
    causticGrad.setColorAt(0.0, QColor(255, 255, 255, 70));
    causticGrad.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter->setBrush(causticGrad);
    painter->drawRoundedRect(caustic, 3, 3);

    painter->restore();
}

void customButton::_setBottlename(const QString info)
{
    m_reagreminder = info;
}

void customButton::setValue(double value)
{
    if (qFuzzyCompare(value, this->m_value)) {
        return;
    }

    if (value < m_minValue) {
        value = m_minValue;
    } else if (value > m_maxValue) {
        value = m_maxValue;
    }

    if (value > m_currentValue) {
        m_isForward = false;
    } else if (value < m_currentValue) {
        m_isForward = true;
    } else {
        this->m_value = value;
        this->update();
        return;
    }

    this->m_value = value;
    if (value < 100) {
        m_currentValue = value;
    }
    this->update();

    emit valueChanged(value);
    m_timer->stop();
    m_timer->start();
}

void customButton::setValue(int value)
{
    setValue(static_cast<double>(value));
    this->update();
}

void customButton::updateValue()
{
    if (m_isForward) {
        m_currentValue -= m_step;
        if (m_currentValue <= m_value) {
            m_currentValue = m_value;
            m_timer->stop();
        }
    } else {
        m_currentValue += m_step;
        if (m_currentValue >= m_value) {
            m_currentValue = m_value;
            m_timer->stop();
        }
    }

    this->update();
}

void customButton::_setwidgetfixsize(int _w, int _h)
{
    m_fixWidth = _w;
    m_fixHeigh = _h;
    this->setFixedWidth(_w);
    this->setFixedHeight(_h);
}

void customButton::mousePressEvent(QMouseEvent* event)
{
    const int press_x = event->x();
    const int press_y = event->y();
    if (event->button() == Qt::LeftButton) {
        const QPoint click_(press_x, press_y);
        if (m_rectBottle.contains(click_)) {
            emit btninnerIndexReag();
        }
        update();
    }
}

void customButton::mouseMoveEvent(QMouseEvent * /*e*/)
{
}

void customButton::setRange(double minValue, double maxValue)
{
    if (minValue >= maxValue) {
        return;
    }

    this->m_minValue = minValue;
    this->m_maxValue = maxValue;

    if (m_value < minValue) {
        setValue(minValue);
    } else if (m_value > maxValue) {
        setValue(maxValue);
    }

    this->update();
}

void customButton::setRange(int minValue, int maxValue)
{
    setRange(static_cast<double>(minValue), static_cast<double>(maxValue));
}

void customButton::setMinValue(double minValue)
{
    setRange(minValue, m_maxValue);
}

void customButton::setMaxValue(double maxValue)
{
    setRange(m_minValue, maxValue);
}

void customButton::setAlarmValue(double alarmValue)
{
    if (!qFuzzyCompare(this->m_alarmValue, alarmValue)) {
        this->m_alarmValue = alarmValue;
        this->update();
    }
}

void customButton::setAlarmValue(int alarmValue)
{
    setAlarmValue(static_cast<double>(alarmValue));
    this->update();
}

void customButton::setStep(double step)
{
    if (!qFuzzyCompare(this->m_step, step)) {
        this->m_step = step;
        this->update();
    }
}

void customButton::setStep(int step)
{
    setStep(static_cast<double>(step));
}

void customButton::setNormalColorStart(const QColor &normalColorStart)
{
    if (this->m_normalColorStart != normalColorStart) {
        this->m_normalColorStart = normalColorStart;
        this->update();
    }
}

void customButton::setNormalColorEnd(const QColor &normalColorEnd)
{
    if (this->m_normalColorEnd != normalColorEnd) {
        this->m_normalColorEnd = normalColorEnd;
        this->update();
    }
}
