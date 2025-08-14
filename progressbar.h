#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QMutex>
#include <QTimer>

class ProgressBar : public QWidget
{
    Q_OBJECT

public:
	enum BarStyle
	{
		//圆环状
		StyleDonut = 0,
		//圆饼状
		StylePie,
		//圆线状
		StyleLine
	};
	ProgressBar(QWidget *parent , BarStyle style = StyleDonut);
    ~ProgressBar();
     //进度条形状
    

public:
    //设置扫描弧度起始角度
    void setStartAngle(double position);

    //设置轮廓画笔的宽度
    void setOutlinePenWidth(double penWidth);

    //设置画笔宽度
    void setDataPenWidth(double penWidth);

    //设置进度条精度 88.88%,几位小数
    void setDecimals(int count);

    //设置进度条风格
    void setBarStyle(BarStyle style);

    //设置范围
    void setRange(double min, double max);

    //设置当前值
    void setValue(int val);

    //设置当前值
    void setValue(double val);

    //设置最小值
    void setMinimum(double min);

    //设置最大值
    void setMaximum(double max);

    //设置通道名字
    void setChnTextindex(int);

	void  setReagentLast(QString);

    //设置直径
    void setwidth(int);

	//设置显示试剂余量
    void SetShowLastReagent(bool );

    //闪烁
    void flashingReminder(const bool isvisible);

private slots:
    void toggleBlinkState();

protected:
    void paintEvent(QPaintEvent *event);

    //画基础图形
    void drawBase(QPainter& p, const QRectF& baseRect, const QRectF &innerRect);

    //根据值画出进度条
    void drawValue(QPainter& p, const QRectF& baseRect, double value, double arcLength, const QRectF & innerRect);

    //画中心文字
    void drawText(QPainter& p, const QRectF& innerRect, double value);
public:
    double   m_min;                                 //最小值
    double   m_max;                                 //最大值
    double   m_value;                               //当前值
    BarStyle m_barStyle;                            //进度条风格
    int      m_decimals;                            //进度条精度
    double   m_startAngel;                          //扫描弧度起始角度
    double   m_outlinePenWidth;                     //轮廓画笔的宽度
    double   m_dataPenWidth;                        //画笔宽度
    int      m_ChannelIndex;                        //通道号
	QString  m_LastReagnets;                        //试剂余量文字
    bool     m_bReagentshow;                        //显示试剂
    int      m_diameter;                            //直径
private:
	QMutex mutex;
    QTimer *mtimer;
    bool m_blinkState;

	bool m_flash;

};

#endif // PROGRESSBAR_H
