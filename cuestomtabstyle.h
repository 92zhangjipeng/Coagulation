#ifndef CUESTOMTABSTYLE_H
#define CUESTOMTABSTYLE_H


#include <QPainter>
#include <QStyleOptionTab>
#include <QRect>
#include <QSize>
#include <QProxystyle>
#include <QtGui>

class CuestomTabStyle:public QProxyStyle
{
public:
    CuestomTabStyle(QString load);

    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
            const QSize &size, const QWidget *widget) const;

    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;


    QString m_loadstr;
};

#endif // CUESTOMTABSTYLE_H
