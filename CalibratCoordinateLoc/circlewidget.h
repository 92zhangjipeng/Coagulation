#ifndef CIRCLEWIDGET_H
#define CIRCLEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>

class CircleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CircleWidget(int channelNumber, QWidget *parent = nullptr);

    void setActive(bool active);
    bool isActive() const { return m_active; }
    int channelNumber() const { return m_channelNumber; }
    void setExclusiveMode(bool exclusive);

signals:
    void clicked(int channelNumber);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_channelNumber;
    bool m_active;
    bool m_exclusiveMode;
    QColor m_activeColor;
    QColor m_inactiveColor;
};

#endif // CIRCLEWIDGET_H
