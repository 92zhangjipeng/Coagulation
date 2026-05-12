#pragma once

#include <QWidget>
#include <QProgressBar>

class QPropertyAnimation;

class AnimationProcessBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(int alpha READ alpha WRITE setAlpha)
public:
    explicit AnimationProcessBar(QWidget *parent = nullptr);
	~AnimationProcessBar();

	int alpha() const { return m_nAlpha; }
	void setAlpha(int alpha) { m_nAlpha = alpha; }

private slots:
	void sltValueChanged(const QVariant &value);

protected:
    void paintEvent(QPaintEvent *);

private:
	//透明度
    int m_nAlpha;

	//动画类
	QPropertyAnimation *m_pAnimation = Q_NULLPTR;
};
