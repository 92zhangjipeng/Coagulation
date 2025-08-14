#ifndef CIRCLEPROGRESS_H
#define CIRCLEPROGRESS_H

#include <QObject>
#include <QWidget>

class CircleProgress : public QWidget
{
    Q_OBJECT
public:
    explicit CircleProgress(QWidget *parent = nullptr);
public:
    void setValue(int value);
    void setWidth(int width);
    void setColor(const QColor &color);
    void setFontPixSize(int size);
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_value;
    int m_width;
    QColor m_color;
    int m_fontSize;

signals:

public slots:
};

#endif // CIRCLEPROGRESS_H
