#ifndef GENERICFUNCTIONS_H
#define GENERICFUNCTIONS_H

#include <QObject>
#include <QApplication>
#include "cglobal.h"
#include "testing.h"
#include <algorithm>//调用sort()必须要使用这个头文件

namespace GenericFunctions {

    /*
     *查找数据库获取测试盘孔坐标(血样针相对于空测试管--坐标)
     */
    QPoint GetTestTrayLoaction_Bloodneedle(int Hole);

    /*
    * 首个试剂是否用完
    */
    bool   ReagentUsingFinished(const quint8 IndexReagent );

    /*
    * 判断试剂余量&&使用预留试剂
    */
    bool Determinethereagentbalance(const bool UesdAsideLoc, int, bool EnoughReagentFirst, QPoint &SuckReagentPos);

    /*
    * 查找数据库获取测试盘孔坐标(抓手相对于空测试管)
    */
    QPoint GetTestTrayLoaction_Hands(int Hole);


    /*
    * 用户按钮减少耗材映射表
    */
    QString BiteMapingConsumablesName(const quint8 KindConsumables);



}

#endif // GENERICFUNCTIONS_H
