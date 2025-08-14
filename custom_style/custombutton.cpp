#include "custombutton.h"
#include "ui_custombutton.h"
#include <qpainter.h>
#include "QsLog/include/QsLog.h"

customButton::customButton(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::customButton)
{
    ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
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

    m_bgRadius = 18;

	m_isForward = false;

    borderColor = QColor(0, 0, 0);  

	bottlecapColor = QColor(0,0,0,100);

	bottleneckColor = QColor(180,180,1800); //瓶颈颜色

	bottleColor = QColor(86, 147, 198, 255);


	m_borderColorStart = QColor(8, 8, 25, 140);

	m_borderColorEnd = QColor(6, 28, 102, 226);

	m_normalColorStart = QColor(50, 205, 51);

	m_normalColorEnd = QColor(60, 179, 133);

	m_alarmColorStart = QColor(250, 118, 113);

	m_alarmColorEnd = QColor(204, 38, 38);


    this->setFixedSize(m_fixWidth, m_fixHeigh);
	

	m_timer = new QTimer(this);
	m_timer->setInterval(10);
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
	// 反走样
	painter.setRenderHint(QPainter::Antialiasing, true);

	//绘制试剂瓶整体
	drawBottlewhole(&painter);

	//绘制背景
	drawBg(&painter);

	//绘制试剂瓶文字
	drawBottleText(&painter);

}

void customButton::drawBottlewhole(QPainter *painter)
{
    int line_w = 2;
	painter->setPen(QPen(borderColor, line_w));
    //绘制瓶盖
    int bottleCapWidth =   m_fixWidth/7*2; //瓶盖宽
    int bottleCapHeight =  bottleCapWidth/5*2; //瓶盖高
    //QLOG_DEBUG()<<"瓶盖宽"<<m_fixWidth<<"瓶盖高"<<m_fixHeigh<<"=="<<bottleCapWidth<<"=="<<bottleCapHeight;

    int spaceTopY = m_fixHeigh/20;
    int spaceLeftX = (m_fixWidth - bottleCapWidth)/2;
    QRect rectCap(spaceLeftX, spaceTopY, bottleCapWidth, bottleCapHeight);
    QBrush brushCap(bottlecapColor, Qt::NoBrush);
    painter->setBrush(brushCap);
    painter->drawRoundedRect(rectCap, 2, 2);
    m_rectCap = rectCap;


    //绘制瓶颈
    int bottleNeckWidth = bottleCapWidth/3*2;
    int bottleNeckHeight = bottleCapHeight/3*2;
    int bottleneck_left_x = rectCap.center().x() - bottleNeckWidth / 2;
    int bottleneck_y = rectCap.center().y() + rectCap.height() / 2 + line_w;
    QRect  bottleneck(bottleneck_left_x, bottleneck_y, bottleNeckWidth, bottleNeckHeight);
    //QBrush bottleneck_brush(bottlecapColor, Qt::SolidPattern);
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->drawRoundedRect(bottleneck, 2, 2);
    m_bottleneck = bottleneck;


    //绘制瓶身
    int BottlesWidth = m_fixWidth/7*4;
    int BottlesHeight = (m_fixHeigh- bottleneck.height() - rectCap.height())/4*3;
    int offsetx = rectCap.center().x() - BottlesWidth/2;
    int Bottle_top = bottleneck.bottomLeft().y() + line_w;
    QRect rectBottle(offsetx, Bottle_top, BottlesWidth, BottlesHeight);
    painter->setBrush(QBrush(Qt::NoBrush));
    painter->drawRoundedRect(rectBottle, m_bgRadius, m_bgRadius);
    m_rectBottle = rectBottle;

    //擦除瓶颈位置
    QColor colorbkg(188,187,183);
    painter->setBrush(QBrush(colorbkg, Qt::Dense1Pattern   ));// Dense1Pattern NoBrush
    painter->setPen(QPen(colorbkg, 0));

    int Xtopleft = m_bottleneck.bottomLeft().x();
    int Ytopleft  = m_bottleneck.bottomLeft().y();
    int wset = m_bottleneck.width();

    painter->drawRect(Xtopleft,Ytopleft,wset,4);

}


void customButton::drawBottleText(QPainter *painter)
{
	painter->setPen(Qt::black);
    painter->setFont(QFont("楷体", 11));

    int spacing = 0;
    int spacing_ratio = 0;
    if(m_reagreminder.length() >= 6)
    {
        spacing = 5;
    }
    else if(m_reagreminder.length() == 2)
    {
        spacing = 12;
    }
    else if(m_reagreminder.length() > 2 && m_reagreminder.length() < 6)
    {
        spacing = 5;
    }
    QPoint poreag(m_rectBottle.topLeft().x() + spacing, m_rectBottle.center().y() - 5);
	painter->drawText(poreag, m_reagreminder);

	QString outshow = QString("%1%").arg(QString::number(m_value, 'f', 2));
    if(outshow.length() == 5)
        spacing_ratio = 5;
    else if(outshow.length()  == 6)
        spacing_ratio = 5;
    else if(outshow.length()  == 7)
        spacing_ratio = 2;
    QPoint posratio(m_rectBottle.topLeft().x()+ spacing_ratio , m_rectBottle.center().y() + 15);
	painter->drawText(posratio, outshow);
}

void customButton::drawBg(QPainter *painter)
{
	if (m_value == m_minValue) {
		return;
	}

	painter->save();

	QLinearGradient batteryGradient(QPointF(0, 0), QPointF(0, height()));
	if (m_currentValue <= m_alarmValue)
	{
		batteryGradient.setColorAt(0.0, m_alarmColorStart);
		batteryGradient.setColorAt(1.0, m_alarmColorEnd);
	}
	else
	{
		batteryGradient.setColorAt(0.0, m_normalColorStart);
		batteryGradient.setColorAt(1.0, m_normalColorEnd);
	}

	double unit = m_rectBottle.height()  / 100;
	double unit_ = (double(m_rectBottle.height() % 100)) /100;
	double unit_rel = unit + unit_;
	double heightdarw = m_currentValue * unit_rel;
    //QLOG_DEBUG()<<"当前值"<<heightdarw<<"====="<<m_currentValue<<"*"<<unit_rel;
    //if (m_currentValue == 100)
        //heightdarw = m_rectBottle.topRight().y();//- m_bgRadius;
    //QLOG_DEBUG() << "tt" << m_rectBottle.height()<<"fff"<< m_rectBottle.bottomRight().y() <<"top"<< m_rectBottle.topRight().y()<<"tf"<< m_rectBottle.topLeft().y();
	QPoint topLeft(m_rectBottle.bottomLeft().x() , m_rectBottle.bottomRight().y() - heightdarw);

	QPoint bottomRight (m_rectBottle.bottomRight().x(), m_rectBottle.bottomRight().y());

	QRect  rect(topLeft, bottomRight);

	painter->setPen(Qt::NoPen);
	painter->setBrush(batteryGradient);
	painter->drawRoundedRect(rect, m_bgRadius, m_bgRadius);

	painter->restore();
}

void customButton::_setBottlename(const QString info)
{
	m_reagreminder = info;
}

//设置值
void  customButton::setValue(double value)
{
	//值和当前值一致则无需处理
	if (value == this->m_value) {
		return;
	}

	//值小于最小值则取最小值,大于最大值则取最大值
	if (value < m_minValue) {
		value = m_minValue;
	}
	else if (value > m_maxValue) {
		value = m_maxValue;
	}

	if (value > m_currentValue)
	{
		m_isForward = false;
	}
	else if (value < m_currentValue) 
	{
		m_isForward = true;
	}
	else 
	{
		this->m_value = value;
		this->update();
		return;
	}

	this->m_value = value;
	if (value < 100)
        m_currentValue = value;
	this->update();

	emit valueChanged(value);
	m_timer->stop();
	m_timer->start();
}

void  customButton::setValue(int value)
{
	setValue((double)value);
	this->update();
}

void customButton::updateValue()
{
	if (m_isForward)
	{
		m_currentValue -= m_step;
		if (m_currentValue <= m_value) {
			m_timer->stop();
		}
	}
	else 
	{
		m_currentValue += m_step;
		if (m_currentValue >= m_value) {
			m_timer->stop();
		}
	}

	this->update();
}

void customButton::_setwidgetfixsize(int _w,int _h)
{
    m_fixWidth = _w;
    m_fixHeigh = _h;

    this->setFixedWidth(_w);
    this->setFixedHeight(_h);
}


void customButton::mousePressEvent(QMouseEvent* event)
{
	int press_x = event->x();
	int press_y = event->y();
	if (event->button() == Qt::LeftButton)
	{
		QPoint click_(press_x, press_y);
		if (m_rectBottle.contains(click_))
		{
			emit btninnerIndexReag();
		}
		update();
	}
}
void customButton::mouseMoveEvent(QMouseEvent *e)
{
	//this->setValue(e->pos().x());
}


void customButton::setRange(double minValue, double maxValue)
{
	//如果最小值大于或者等于最大值则不设置
	if (minValue >= maxValue) {
		return;
	}

	this->m_minValue = minValue;
	this->m_maxValue = maxValue;

	//如果目标值不在范围值内,则重新设置目标值
	//值小于最小值则取最小值,大于最大值则取最大值
	if (m_value < minValue) {
		setValue(minValue);
	}
	else if (m_value > maxValue) {
		setValue(maxValue);
	}

	this->update();
}

void customButton::setRange(int minValue, int maxValue)
{
	setRange((double)minValue, (double)maxValue);
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
	if (this->m_alarmValue != alarmValue) {
		this->m_alarmValue = alarmValue;
		//QLOG_DEBUG()<<"set alarm="<<alarmValue;
		this->update();
	}
}

void customButton::setAlarmValue(int alarmValue)
{
	setAlarmValue((double)alarmValue);
	this->update();
}

void customButton::setStep(double step)
{
	if (this->m_step != step) {
		this->m_step = step;
		this->update();
	}
}

void customButton::setStep(int step)
{
	setStep((double)step);

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
