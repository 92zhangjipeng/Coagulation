#pragma execution_character_set("utf-8")

#include "kettle.h"
#include <QPainter>
#include <QTimer>
#include <QsLog/include/QsLog.h>

Kettle::Kettle(QWidget *parent) : QWidget(parent)
{
    minValue = 0;
    maxValue = 100;
    value = 0;
    alarmValue = 5;
    step = 5;

    borderWidth = 3;   //边框腰粗细
    borderRadius = 16 /*8*/;
    bgRadius = 5;
    headRadius = 8;

    m_reagname = "";

    borderColorStart = QColor(8, 8, 25,140);
    borderColorEnd = QColor(6, 28, 102,226);

    alarmColorStart = QColor(250, 118, 113);
    alarmColorEnd = QColor(204, 38, 38);
    normalColorStart = QColor(238, 173 ,14 ); //正常颜色50, 205, 51
    normalColorEnd = QColor(60, 179, 133);

    isForward = false;
    currentValue = 0;

    timer = new QTimer(this);
    timer->setInterval(10);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateValue()));
    
	QPalette palette(this->palette());
    palette.setColor(QPalette::Background, QColor(188,187,183));
	this->setPalette(palette);
}

Kettle::~Kettle()
{
    if (timer->isActive()) {
            timer->stop();
        }
}
void  Kettle::_setreagname(QString reagname)
{
    m_reagname = reagname;
}
void Kettle::paintEvent(QPaintEvent *)
{
    //绘制准备工作,启用反锯齿
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    //绘制边框
    drawBorder(&painter);

    //绘制背景
    drawBg(&painter);

    //绘制头部
    drawHead(&painter);

	drawText(&painter);
}
void Kettle::drawText(QPainter *painter) {

	painter->save();

	painter->setPen(Qt::black);
    painter->setFont(QFont("楷体", 12));

    QString value1 = QString::number(value, 'f', 2) + "%";

    QTransform transform;
    transform.rotate(+90.0);
    painter->setWorldTransform(transform);
    if(m_reagname.length() <= 2)
       painter->drawText(batteryRect.topLeft().x() + 22, -(batteryRect.bottomRight().y()/2) - TEXT_POS_SHOW_Y - 15, m_reagname);
    else if(m_reagname.length() <= 5)
        painter->drawText(batteryRect.topLeft().x() + 10, -(batteryRect.bottomRight().y()/2) - TEXT_POS_SHOW_Y - 15, m_reagname);
    else if(m_reagname.length() >= 6)
        painter->drawText(batteryRect.topLeft().x() + 3, -(batteryRect.bottomRight().y()/2) - TEXT_POS_SHOW_Y - 15, m_reagname);


    painter->drawText(batteryRect.topLeft().x() + 3,  -(batteryRect.bottomRight().y()/2) - TEXT_POS_SHOW_Y, value1);
    //QLOG_DEBUG()<<"=="<<batteryRect<<"++"<<batteryRect.topLeft().x()<<"=="<<batteryRect.bottomRight().y()<<endl;
}

void Kettle::drawBorder(QPainter *painter)
{
    painter->save();

    double headWidth = width() / 15;
    double batteryWidth = width() - headWidth;

    //绘制电池边框
    QPointF topLeft(borderWidth, borderWidth);
    QPointF bottomRight(batteryWidth, height() - borderWidth);
    batteryRect = QRectF(topLeft, bottomRight);

    painter->setPen(QPen(borderColorStart, borderWidth));
    painter->setBrush(Qt::NoBrush);
    painter->drawRoundedRect(batteryRect, borderRadius, borderRadius);
	
    painter->restore();
}

void Kettle::drawBg(QPainter *painter)
{
    if (value == minValue) {
        return;
    }

    painter->save();

    QLinearGradient batteryGradient(QPointF(0, 0), QPointF(0, height()));
    if (currentValue <= alarmValue)
    {
        batteryGradient.setColorAt(0.0, alarmColorStart);
        batteryGradient.setColorAt(1.0, alarmColorEnd);
    }
    else
    {
        batteryGradient.setColorAt(0.0, normalColorStart);
        batteryGradient.setColorAt(1.0, normalColorEnd);
    }

    int margin = qMin(width(), height()) / 20;
    double unit = (batteryRect.width() - (margin * 2)) / 100;
    double width = currentValue * unit;

    QPointF topLeft(batteryRect.topLeft().x() + margin, batteryRect.topLeft().y() + margin);
    QPointF bottomRight(width + margin + borderWidth, batteryRect.bottomRight().y() - margin);
    QRectF rect(topLeft, bottomRight);

    painter->setPen(Qt::NoPen);
    painter->setBrush(batteryGradient);
    painter->drawRoundedRect(rect, bgRadius, bgRadius);

    painter->restore();
}

void Kettle::drawHead(QPainter *painter)
{
    painter->save();

    QPointF headRectTopLeft(batteryRect.topRight().x() , height() / 3);
    QPointF headRectBottomRight(width(), height() - height() / 3);
    QRectF headRect(headRectTopLeft, headRectBottomRight);


    QLinearGradient headRectGradient(headRect.topLeft(), headRect.bottomLeft());
    headRectGradient.setColorAt(0.0, borderColorStart);
    headRectGradient.setColorAt(1.0, borderColorEnd);

    painter->setPen(Qt::NoPen);
    painter->setBrush(headRectGradient);
    painter->drawRoundedRect(headRect, headRadius, headRadius);


    painter->restore();
}

void Kettle::updateValue()
{
    if (isForward) {
        currentValue -= step;
        if (currentValue <= value) {
            timer->stop();
        }
    } else {
        currentValue += step;
        if (currentValue >= value) {
            timer->stop();
        }
    }

    this->update();
}

double Kettle::getMinValue() const
{
    return this->minValue;
}

double Kettle::getMaxValue() const
{
    return this->maxValue;
}

double Kettle::getValue() const
{
    return this->value;
}

double Kettle::getAlarmValue() const
{
    return this->alarmValue;
}

double Kettle::getStep() const
{
    return this->step;
}

int Kettle::getBorderWidth() const
{
    return this->borderWidth;
}

int Kettle::getBorderRadius() const
{
    return this->borderRadius;
}

int Kettle::getBgRadius() const
{
    return this->bgRadius;
}

int Kettle::getHeadRadius() const
{
    return this->headRadius;
}

QColor Kettle::getBorderColorStart() const
{
    return this->borderColorStart;
}

QColor Kettle::getBorderColorEnd() const
{
    return this->borderColorEnd;
}

QColor Kettle::getAlarmColorStart() const
{
    return this->alarmColorStart;
}

QColor Kettle::getAlarmColorEnd() const
{
    return this->alarmColorEnd;
}

QColor Kettle::getNormalColorStart() const
{
    return this->normalColorStart;
}

QColor Kettle::getNormalColorEnd() const
{
    return this->normalColorEnd;
}

QSize Kettle::sizeHint() const
{
    return QSize(75, 50); //-30
}

QSize Kettle::minimumSizeHint() const
{
    return QSize(50, 25);
}

void Kettle::setRange(double minValue, double maxValue)
{
    //如果最小值大于或者等于最大值则不设置
    if (minValue >= maxValue) {
        return;
    }

    this->minValue = minValue;
    this->maxValue = maxValue;

    //如果目标值不在范围值内,则重新设置目标值
    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < minValue) {
        setValue(minValue);
    } else if (value > maxValue) {
        setValue(maxValue);
    }

    this->update();
}

void Kettle::setRange(int minValue, int maxValue)
{
    setRange((double)minValue, (double)maxValue);
}

void Kettle::setMinValue(double minValue)
{
    setRange(minValue, maxValue);
}

void Kettle::setMaxValue(double maxValue)
{
    setRange(minValue, maxValue);
}

void Kettle::setValue(double value)
{
    //值和当前值一致则无需处理
    if (value == this->value) {
        return;
    }

    //值小于最小值则取最小值,大于最大值则取最大值
    if (value < minValue) {
        value = minValue;
    } else if (value > maxValue) {
        value = maxValue;
    }

    if (value > currentValue) {
        isForward = false;
    } else if (value < currentValue) {
        isForward = true;
    } else {
        this->value = value;
        this->update();
        return;
    }

    this->value = value;
    this->update();
    emit valueChanged(value);
    timer->stop();
    timer->start();
}

void Kettle::setValue(int value)
{
    setValue((double)value);
    this->update();
}

void Kettle::setAlarmValue(double alarmValue)
{
    if (this->alarmValue != alarmValue) {
        this->alarmValue = alarmValue;
        //QLOG_DEBUG()<<"set alarm="<<alarmValue;
        this->update();
    }
}

void Kettle::setAlarmValue(int alarmValue)
{
    setAlarmValue((double)alarmValue);
    this->update();
}

void Kettle::setStep(double step)
{
    if (this->step != step) {
        this->step = step;
        this->update();
    }
}

void Kettle::setStep(int step)
{
    setStep((double)step);

}

void Kettle::setBorderWidth(int borderWidth)
{
    if (this->borderWidth != borderWidth) {
        this->borderWidth = borderWidth;
        this->update();
    }
}

void Kettle::setBorderRadius(int borderRadius)
{
    if (this->borderRadius != borderRadius) {
        this->borderRadius = borderRadius;
        this->update();
    }
}

void Kettle::setBgRadius(int bgRadius)
{
    if (this->bgRadius != bgRadius) {
        this->bgRadius = bgRadius;
        this->update();
    }
}

void Kettle::setHeadRadius(int headRadius)
{
    if (this->headRadius != headRadius) {
        this->headRadius = headRadius;
        this->update();
    }
}

void Kettle::setBorderColorStart(const QColor &borderColorStart)
{
    if (this->borderColorStart != borderColorStart) {
        this->borderColorStart = borderColorStart;
        this->update();
    }
}

void Kettle::setBorderColorEnd(const QColor &borderColorEnd)
{
    if (this->borderColorEnd != borderColorEnd) {
        this->borderColorEnd = borderColorEnd;
        this->update();
    }
}

void Kettle::setAlarmColorStart(const QColor &alarmColorStart)
{
    if (this->alarmColorStart != alarmColorStart) {
        this->alarmColorStart = alarmColorStart;
        this->update();
    }
}

void Kettle::setAlarmColorEnd(const QColor &alarmColorEnd)
{
    if (this->alarmColorEnd != alarmColorEnd) {
        this->alarmColorEnd = alarmColorEnd;
        this->update();
    }
}

void Kettle::setNormalColorStart(const QColor &normalColorStart)
{
    if (this->normalColorStart != normalColorStart) {
        this->normalColorStart = normalColorStart;
        this->update();
    }
}

void Kettle::setNormalColorEnd(const QColor &normalColorEnd)
{
    if (this->normalColorEnd != normalColorEnd) {
        this->normalColorEnd = normalColorEnd;
        this->update();
    }
}

void Kettle::mousePressEvent(QMouseEvent* event)
{
    int press_x = event->x() ;
    int press_y = event->y() ;
    if (event->button() == Qt::LeftButton)
    {
       _replacereag(press_x, press_y);
       update();
    }
}

void Kettle::_replacereag(int x_,int y_)
{
    //QLOG_DEBUG()<<"x==="<<x_<<"y==="<<y_;
    QPoint click_(x_,y_);
    if(batteryRect.contains(click_))
    {
       //QLOG_DEBUG()<<"inner!!!!";
       emit _clickinnerReag();
    }
}
