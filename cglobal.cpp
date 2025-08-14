#include "cglobal.h"
#include <QCoreApplication>
cglobal::cglobal()
{

}
cglobal::~cglobal()
{

}


bool cglobal::g_StartTesting = false;
bool cglobal::g_PauseTesting = false;
bool cglobal::g_QualityControlhighValue = false;

bool cglobal::g_quality_start = false;

bool cglobal::gserialConnecStatus = false;

bool cglobal::g_controldimmingfinished = false;

QString cglobal::g_UserName_str ="";



QColor cglobal::g_LineColor(13, 23, 114);           //最外圈线的颜色
//QColor cglobal::g_OuterRingColor(6, 28, 102,226);    //最外外环颜色
QColor cglobal::g_OuterRingColor(6, 28, 102,140);
//QColor  cglobal::g_SamllBgmColor(125,125,128);	 //内圆颜色--初始状态底色

QColor cglobal::g_ReminderBloodHoleColor(238,10,10,200);

QColor  cglobal::g_SamllBgmColor(188,187,183);       //内圆颜色--初始状态底色未使用

QColor  cglobal::g_InnerRing_Checked(250, 128 ,114);  //空试管选中颜色 250,128,114


QColor  cglobal::g_PoorBloodColor(255 ,255, 0);        //贫血颜色
QColor  cglobal::g_RichBloodColor(255, 165, 0);       //富血颜色



QColor  cglobal::g_OutResult(89 ,89 ,89);         //出结果或者试管被夹走 --试管用掉颜色



QColor cglobal::g_TextColorBlack(0 ,0, 0);    //(159 ,20 ,20);
QColor cglobal::g_TextColorRoorblood(0, 0, 0);    //贫血文字颜色   255, 165, 0    250, 229, 211
QColor cglobal::g_TextColorRichblood(0 ,0, 0);    //255 ,255, 211


QColor cglobal::g_TextColorWhite(0, 0, 0);     //空试管区文字颜色


QColor  cglobal::g_InnerRing_absorbed(54 ,54 ,54);		   
QColor  cglobal::g_SelectColor(238, 59, 59);	  //试管加完血样 --红
//QColor cglobal::g_OuterRingColor(230, 230, 250);   //外环颜色
QColor cglobal::g_MoveedColor(72 ,118 ,255);   //待测内环颜色

quint16 cglobal::g_cmd_num = 1;
QString cglobal::gserialPortName = "";           //连接串口号

 bool cglobal::g_moduleIdisconnected = false;   //模组I - III 掉线状态
 bool cglobal::g_moduleIIdisconnected = false;
 bool cglobal::g_moduleIIIdisconnected = false;


