#ifndef CUSTOMBUTTON_H
#define CUSTOMBUTTON_H

#include <QPaintEvent>
#include <QWidget>
#include <QTimer>
#include <QMouseEvent>

namespace Ui {
class customButton;
}

class customButton : public QWidget
{
    Q_OBJECT

public:
    explicit customButton(QWidget *parent = 0);
    ~customButton();

    void _setfixwidth(int _width);

    void _setfixheigh(int _heigh);

    void _setwidgetfixsize(int _w,int _h); //设置界面大小

	void _setBottlename(const QString );

    //设置值
	void setValue(double value);
	void setValue(int value);

	//设置范围值
	void setRange(double minValue, double maxValue);
	void setRange(int minValue, int maxValue);

	//设置最大最小值
	void setMinValue(double minValue);
	void setMaxValue(double maxValue);

    //设置警戒值
	void setAlarmValue(double alarmValue);
	void setAlarmValue(int alarmValue);

	//设置步长
	void setStep(double step);
	void setStep(int step);

	//设置正常时的渐变颜色
	void setNormalColorStart(const QColor &normalColorStart);
	void setNormalColorEnd(const QColor &normalColorEnd);

protected:
    void paintEvent(QPaintEvent*);

	void mousePressEvent(QMouseEvent* event);

	void mouseMoveEvent(QMouseEvent *e);

	void drawBottlewhole(QPainter *painter); //绘制试剂瓶整体

	void drawBottleText(QPainter *painter);

	void drawBg(QPainter *painter);

signals:
	void btninnerIndexReag();

	void valueChanged(double value);

private slots:
    void updateValue();

private:
    Ui::customButton *ui;

    int m_fixWidth; //固定宽
    int m_fixHeigh; //固定高


	QString m_reagreminder; //试剂种类

    QColor borderColor; //边框开始颜色

    QColor bottlecapColor;  //瓶盖颜色

	QColor bottleneckColor; //瓶颈颜色

	QColor bottleColor;  //瓶身颜色 


	QRect m_rectCap;     //瓶盖
	QRect m_bottleneck;  //瓶颈
	QRect m_rectBottle;  //瓶身


	double m_minValue;                //最小值
	double m_maxValue;                //最大值
	double m_value;                   //目标值
	double m_alarmValue;              //警戒值
	double m_step;                    //每次移动的步长

	int m_bgRadius;                   //瓶身进度圆角角度
	bool m_isForward;                  //是否往前移

	double m_currentValue;            //当前值


	QColor m_alarmColorStart;         //低容量时的渐变开始颜色
	QColor m_alarmColorEnd;           //低容量时的渐变结束颜色

	QColor m_normalColorStart;        //正常容量时的渐变开始颜色
	QColor m_normalColorEnd;          //正常容量时的渐变结束颜色

	QColor m_borderColorStart;        //边框渐变开始颜色
	QColor m_borderColorEnd;          //边框渐变结束颜色

	QTimer *m_timer = nullptr;                  //绘制定时器
};

#endif // CUSTOMBUTTON_H
