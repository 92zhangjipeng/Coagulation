#ifndef TABWIDGET_H
#define TABWIDGET_H

#endif // TABWIDGET_H

#include<QTabWidget>
#include"tabbar.h"

class TabWidget : public QTabWidget
{
public:
    TabWidget(QWidget *parent=0):QTabWidget(parent){
        setTabBar(new TabBar);
        setTabPosition(QTabWidget::West);
    }
};
