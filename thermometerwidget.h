#ifndef THERMOMETERWIDGET_H
#define THERMOMETERWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPropertyAnimation>
#include <QPainter>
#include <QTimer>

class ThermometerWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ getValue WRITE setValue)  //声明属性
public:
    explicit ThermometerWidget(QWidget *parent = nullptr);
    qreal getValue();
    void setValue(qreal value);
    void changeValue(qreal value);
protected:
    void paintEvent(QPaintEvent *e);

signals:

public slots:
    void startAnimation();
private:
    qreal m_value;
    qreal curValue;
    int m_width;
    QRectF m_rect;
    int maxValue, minValue;
    qreal m_radius;
    QPropertyAnimation *m_valueAnimation;
    void updateRect();


};

#endif // THERMOMETERWIDGET_H
