#include "custom_text_colors.h"
#include "qDebug.h"

#define HUO_HE_HONG "e68ab8" //红鹤色
#define LIANG_LAN   "add8e6"//亮蓝
#define LIANG_TIAN_LAN   "87cefa"//亮天蓝
#define MI_CHENGE   "ffb366"//蜜橙
#define YANG_MEIGUI_HONG  "ff0da6" //洋玫瑰红

Custom_Text_Colors::Custom_Text_Colors(QObject *parent) : QObject(parent)
{

}
void Custom_Text_Colors::SetText_Colors(int Colors_index,QString enter_str,QString &out_str,QString change_text)
{
    QString Show_color;
    switch(Colors_index)
    {
        case 0:
            Show_color = QString("<font style='font-size:18px;  color:#%1;'>\\1</font>").arg(HUO_HE_HONG);
        break;
        case 1:
            Show_color = QString("<font style='font-size:18px;  color:#%1;'>\\1</font>").arg(LIANG_LAN);
        break;
        case 2:
            Show_color = QString("<font style='font-size:18px;  color:#%1;'>\\1</font>").arg(LIANG_TIAN_LAN);
        break;
        case 3:
            Show_color = QString("<font style='font-size:18px;  color:#%1;'>\\1</font>").arg(MI_CHENGE);
        case 4:
            Show_color = QString("<font style='font-size:18px;  color:#%1;'>\\1</font>").arg(YANG_MEIGUI_HONG);
        break;
        default:
            Show_color = QString("<font style='font-size:18px;  color:#%1;'>\\1</font>").arg("000000");
        break;
    }
    if(!enter_str.isEmpty())
    {
       QRegExp valueRegExp(QString("(%1)").arg(change_text));
       valueRegExp.setCaseSensitivity(Qt::CaseInsensitive);
       enter_str = enter_str.replace(valueRegExp, Show_color);
    }
    out_str = enter_str;
    //qDebug()<<out_str;
    return;
}
