#ifndef MYLABEL_H
#define MYLABEL_H


//自定义Label，实现双击事件
#include <QLabel>
#include <QMouseEvent>
class MyLabel : public QLabel
{
    Q_OBJECT
public:
    explicit MyLabel(QWidget *parent = Q_NULLPTR) :QLabel(parent){}
    ~MyLabel(){}
signals:
    void doubleClicked();
protected:
    void mouseDoubleClickEvent(QMouseEvent* event) {
        if (Qt::LeftButton == event->button()) {//左键单击
            emit doubleClicked();//发射信号
        }
    }
};
#endif // MYLABEL_H
