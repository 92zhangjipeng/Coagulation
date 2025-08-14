#include "cuestomtabstyle.h"
#include <QStyleOptionTab>
#include <qDebug>

CuestomTabStyle::CuestomTabStyle(QString load)
{
    m_loadstr = load;
}
QSize CuestomTabStyle::sizeFromContents(QStyle::ContentsType type, const QStyleOption *option, const QSize &size, const QWidget *widget) const
{
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);
    if (type == QStyle::CT_TabBarTab) {
        s.transpose();
        s.rwidth() = 120; // 设置每个tabBar中item的大小
        s.rheight() = 80;
    }
    return s;
}

void CuestomTabStyle::drawControl(QStyle::ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const
{
    if (element == CE_TabBarTabLabel) {
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
            QRect allRect = tab->rect;
            QStyleOptionTab opt(*tab);
            if (tab->state & QStyle::State_Selected) {  //选中状态：tab的Qlabel为矩形，底色为白色，边框淡灰色，文字为淡蓝色且加粗居中（具体颜色值由拾色器提取）
                painter->save();
                painter->setPen(0xdadbdc);  //设置画笔颜色为淡灰色
                painter->setBrush(QBrush(0xffffff));  //设置画刷为白色
                painter->drawRect(allRect.adjusted(0,0,0,0));  //重绘tab的矩形边框
                painter->restore();  //还原为默认
                painter->save();
                painter->setPen(0x006ab1);  //重新设置画笔颜色为淡蓝色
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);  //设置文字居中
                painter->setFont(QFont("", 14, QFont::Bold));  //设置文字样式，大小为9并加粗，颜色由画笔决定
                painter->drawText(allRect, tab->text, option);  //重绘文字

//                opt.shape = QTabBar::RoundedNorth;
//                qDebug()<<"222"<<m_loadstr;
//                QIcon icon(m_loadstr);
//                opt.icon = icon;
//                opt.palette.setCurrentColorGroup(QPalette::Disabled);
//                 opt.state |= QStyle::State_Sunken;
//                QProxyStyle::drawControl(element, &opt, painter, widget);
                painter->restore();
            }
            else if(tab->state & QStyle::State_MouseOver) { //hover状态：tab的Qlabel为矩形，底色为灰色，边框仍未淡灰色，文字加粗居中
                painter->save();
                painter->setPen(0xdadbdc);  //设置画笔颜色为淡灰色
                painter->setBrush(QBrush(0xf2f2f2));  //设置画刷为灰色
                painter->drawRect(allRect.adjusted(0,0,0,0));  //重绘tab的矩形边框
                painter->restore();  //还原

                painter->save();
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);  //设置文字居中
                painter->setFont(QFont("",14, QFont::Bold));  //设置文字样式，大小为9并加粗，颜色由画笔决定
                painter->drawText(allRect, tab->text, option);  //重绘文字
                painter->restore();
            }
            else //其它的：tab的Qlabel为矩形，底色为灰色，边框为淡灰色不变，文字不加粗但居中
            {
                painter->save();
                painter->setPen(0xdadbdc);
                painter->setBrush(QBrush(0xf2f2f2));
                painter->drawRect(allRect.adjusted(0,0,0,0));
                painter->restore();

                painter->save();
                QTextOption option;
                option.setAlignment(Qt::AlignCenter);
                painter->drawText(allRect, tab->text, option);
                painter->restore();
            }

            return;
        }
    }

    if (element == CE_TabBarTab) {
        QProxyStyle::drawControl(element, option, painter, widget);
    }
}


