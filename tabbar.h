#ifndef TABBAR_H
#define TABBAR_H

#include <QApplication>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>



class TabBar : public QTabBar
{
public:
    TabBar();
    QSize tabSizeHint(int index) const{
        QSize s = QTabBar::tabSizeHint(index);
        s.transpose();
        return s;
    }
protected:
    void paintEvent(QPaintEvent * event);

};



#endif // TABBAR_H
