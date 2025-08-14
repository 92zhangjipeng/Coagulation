#pragma execution_character_set("utf-8")

#include "progressbar.h"
#include <QPainter>
#include <QPointF>
#include <QtMath>
#include <qDebug>

ProgressBar::ProgressBar(QWidget *parent, BarStyle style) : QWidget(parent)
        , m_min(0), m_max(100), m_value(0), m_startAngel(90), m_barStyle(style)
        , m_outlinePenWidth(0), m_dataPenWidth(0), m_decimals(0),m_ChannelIndex(0)
        ,m_bReagentshow(false),mtimer(nullptr),m_blinkState(false), m_flash(false)
{
    this->setStyleSheet("border: 1px solid #C0C0C0;"
                        "background-color: rgb(230, 230, 230);"
                        "border-style: solid; "
                        "border-radius:10px;"
                        "padding:0 0px; "
                       );
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
		m_blinkState = !m_blinkState; // 切换状态
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

    //外圈直径
    double outerDiameter = this->width() - 10;

    //外圈矩形
    QRectF baseRect(0, 0, outerDiameter, outerDiameter);
    QPainter p(this);
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    //内圆直径
    double innerDiameter = 0;

    //内圈矩形
    QRectF innerRect;

    //计算内圈矩形
    if (m_barStyle == StyleLine)
    {
        innerDiameter = outerDiameter - m_outlinePenWidth;
    }
    else if(m_barStyle == StyleDonut)
    {
        innerDiameter = outerDiameter * 0.8;
    }
    double delta = (outerDiameter - innerDiameter) / 2;
    innerRect = QRectF(delta, delta, innerDiameter, innerDiameter);


    //画基础图形
    drawBase(p, baseRect, innerRect);

    //计算当前步长比例
    double arcStep = 360.0 / (m_max - m_min) * m_value;

    //根据值画出进度条
    drawValue(p, baseRect, m_value, arcStep, innerRect);

    //画文字
    drawText(p, baseRect, m_value);

    p.end();
}

void ProgressBar::drawBase(QPainter &p, const QRectF &baseRect, const QRectF &innerRect)
{
    switch (m_barStyle)
    {
        case StyleDonut:
        {
            QPainterPath dataPath;
            dataPath.setFillRule(Qt::OddEvenFill);
            dataPath.moveTo(baseRect.center());
            dataPath.addEllipse(innerRect);

            QPen pen;
            pen.setWidth(10);
            if (m_ChannelIndex != -1) {
                pen.setColor(QColor("#DEE3E7"));
                p.setPen(pen);
                p.setBrush(QBrush(QColor("#FFFFFF")));
            }
            else {
                pen.setColor(QColor("#FF0000"));
                p.setPen(pen);
                p.setBrush(QBrush(QColor("#1C1C1C")));
            }
            p.drawPath(dataPath);
            break;
        }

        case StylePie:
        {
			if (m_ChannelIndex != -1) {
				p.setPen(QPen(QColor("#FFFFFF"), m_outlinePenWidth));
				p.setBrush(Qt::NoBrush);
			}
			else {
				p.setPen(QPen(QColor("#FF0000"), m_outlinePenWidth));
				p.setBrush(QBrush(QColor("#1C1C1C")));
			}
            p.drawEllipse(baseRect);
            break;
        }

        case StyleLine:
        {
			if (m_ChannelIndex != -1) {
				p.setPen(QPen(QColor("#FFFFFF"), m_outlinePenWidth));
				p.setBrush(Qt::NoBrush);
			}
			else {
				p.setPen(QPen(QColor("#FF0000"), m_outlinePenWidth));
				p.setBrush(QBrush(QColor("#1C1C1C")));
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

    if (m_barStyle == StyleLine)
    {
        p.setPen(QColor("#2F8DED"));
        p.setBrush(Qt::NoBrush);
        p.drawArc(baseRect,m_startAngel * 16, -arcLength * 16);
    }
    else if (m_barStyle == StyleDonut)
    {
        QPen pen;
        pen.setColor(QColor("#2F8DED"));
        pen.setWidth(10);
        pen.setCapStyle(Qt::RoundCap);
        p.setPen(pen);
        p.drawArc(innerRect, m_startAngel*16 , -16*arcLength);
    }
    else
    {
        //获取中心点坐标
        QPointF centerPoint = baseRect.center();
        QPainterPath dataPath;
        dataPath.setFillRule(Qt::WindingFill);
        dataPath.moveTo(centerPoint);
        //逆时针画弧长
        dataPath.arcTo(baseRect, m_startAngel, -arcLength);
        if (m_barStyle == StylePie)
        {
            dataPath.lineTo(centerPoint);
            p.setPen(QPen(QColor("#2F8DED"), m_dataPenWidth));
        }
        p.setBrush(QBrush(QColor("#2F8DED")));
        p.drawPath(dataPath);
    }

}

void ProgressBar::drawText(QPainter &p, const QRectF &rect, double value)
{
    QFont f;
    f.setFamily("楷体");
    f.setPixelSize(20);
    p.setFont(f);
    QString textToDraw = "%";
	QString Channel;
    double percent = (value - m_min) / (m_max - m_min) * 100.0;
	if (m_bReagentshow)
	{
        Channel = QString("\n%1").arg(m_LastReagnets);
	}
    else
	{
        if(m_ChannelIndex != -1){
            p.setPen(QColor(255, 64 ,64));
            Channel = QString("\n%1%2").arg(tr("通道")).arg(m_ChannelIndex);
        }else{
            p.setPen(QColor(205, 38, 38));
            Channel = QString("\n%1").arg(tr("禁用"));
        }
	}
    textToDraw = QString::number(percent, 'f', m_decimals) + textToDraw + Channel;
    p.drawText(rect, Qt::AlignCenter, textToDraw);
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
