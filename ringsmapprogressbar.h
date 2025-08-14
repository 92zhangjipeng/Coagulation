#ifndef RINGSMAPPROGRESSBAR_H
#define RINGSMAPPROGRESSBAR_H

#include <QWidget>

class RingsMapProgressbar : public QWidget
{
    Q_OBJECT

public:
    explicit RingsMapProgressbar(QWidget *parent = 0);
    void setPersent(double persent, QString passage);

protected:
    void paintEvent(QPaintEvent *);

private:
    int m_rotateAngle;//旋转角度
    int m_persent; //百分比
    QString m_passage;
};

#endif // RINGSMAPPROGRESSBAR_H
