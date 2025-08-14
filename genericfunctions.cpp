#ifdef Q_OS_UNIX
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#else
#include <qt_windows.h>
#endif

#include "genericfunctions.h"
#include "loadequipmentpos.h"
//#include "loginui.h"



/*查找数据库获取测试盘孔坐标(血样针相对于空测试管--坐标)*/
QPoint GenericFunctions::GetTestTrayLoaction_Bloodneedle(int Hole)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QPoint EmptyTubePos(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,Hole,MOTOR_BLOOD_INDEX,EmptyTubePos);
    return EmptyTubePos;
}

/*首个试剂是否用完*/
bool GenericFunctions::ReagentUsingFinished(const quint8 IndexReagent)
{
    //ConsumablesOper *pReagentTotal = ConsumablesOper::GetpInstance();
    quint16 ReagentRatio = 0;
    quint8 LimintLine = 0;
    float fratiovalue = 0.0f;
    bool buseLeftReag = false;
    switch(IndexReagent)
    {
        case AA_REAGENT:
                    ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT,INDEX_AA_CONSUMABLE,ReagentRatio);
                    fratiovalue = float(ReagentRatio*100.00/MAX_RATIO_SWITCT);
                    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,INDEX_AA_CONSUMABLE,LimintLine);
                    if(fratiovalue <= LimintLine )
                    {
                        buseLeftReag = true;
                        QLOG_DEBUG()<<"左AA试剂剩余百分比"<<ReagentRatio<<"AA限位"<<LimintLine;
                    }
        break;
        case ADP_REAGENT:
                    ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT,INDEX_ADP_CONSUMABLE,ReagentRatio);
                    fratiovalue = float(ReagentRatio*100.00/MAX_RATIO_SWITCT);
                    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,INDEX_ADP_CONSUMABLE,LimintLine);
                    if(fratiovalue <= LimintLine )
                    {
                        buseLeftReag = true;
                        QLOG_DEBUG()<<"左ADP试剂剩余百分比"<<ReagentRatio<<"ADP限位"<<LimintLine;
                    }
        break;
        case EPI_REAGENT:
                    ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT,INDEX_EPI_CONSUMABLE,ReagentRatio);
                    fratiovalue = float(ReagentRatio*100.00/MAX_RATIO_SWITCT);
                    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,INDEX_EPI_CONSUMABLE,LimintLine);
                    if(fratiovalue <= LimintLine )
                    {
                        buseLeftReag = true;
                        QLOG_DEBUG()<<"左EPI试剂剩余百分比"<<ReagentRatio<<"EPI限位"<<LimintLine;
                    }
        break;
        case COL_REAGENT:
                    ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT,INDEX_COL_CONSUMABLE,ReagentRatio);
                    fratiovalue = float(ReagentRatio*100.00/MAX_RATIO_SWITCT);
                    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,INDEX_COL_CONSUMABLE,LimintLine);
                    if(fratiovalue <= LimintLine )
                    {
                        buseLeftReag = true;
                        QLOG_DEBUG()<<"左COL试剂剩余百分比"<<ReagentRatio<<"COL限位"<<LimintLine;
                    }
        break;
        case RIS_REAGENT:
                    ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT,INDEX_RIS_CONSUMABLE,ReagentRatio);
                    fratiovalue = float(ReagentRatio*100.00/MAX_RATIO_SWITCT);
                    ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT,INDEX_RIS_CONSUMABLE,LimintLine);
                    if(fratiovalue <= LimintLine )
                    {
                        buseLeftReag = true;
                        QLOG_DEBUG()<<"左RIS试剂剩余百分比"<<ReagentRatio<<"RIS限位"<<LimintLine;
                    }
        break;
		default:break;
    }
    return buseLeftReag;
}

/*判断试剂余量&&使用预留试剂*/
bool GenericFunctions::Determinethereagentbalance(const bool UesdAsideLoc,int KindReagents, bool EnoughReagentFirst,QPoint &SuckReagentPos)
{
    QPoint ReagentMovePos(0,0);
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    if(UesdAsideLoc == false && EnoughReagentFirst == false)
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,KindReagents,ReagentMovePos);
        QLOG_DEBUG()<<"****不使用限位&&第一个试剂够"<<KindReagents<<"坐标"<<ReagentMovePos<<endl;
    }
    else if(UesdAsideLoc == false && EnoughReagentFirst == true)
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,KindReagents,ReagentMovePos);
        QLOG_DEBUG()<<"****不使用限位&&第一个试剂不够"<<KindReagents<<"坐标"<<ReagentMovePos<<endl;
        return false;
    }
    else if(UesdAsideLoc == true && EnoughReagentFirst == false)
    {
         SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,KindReagents ,ReagentMovePos);
         QLOG_DEBUG()<<"****使用限位&&第一个试剂够"<<KindReagents<<"坐标"<<ReagentMovePos<<endl;
    }
    else if(UesdAsideLoc == true && EnoughReagentFirst == true)
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,KindReagents+5,ReagentMovePos);
        QLOG_DEBUG()<<"****使用限位&&第一个试剂 不够"<<KindReagents+5<<"坐标"<<ReagentMovePos<<endl;
    }
    SuckReagentPos = ReagentMovePos;
    return true;
}


/*
* 查找数据库获取测试盘孔坐标(抓手相对于空测试管)
*/
QPoint GenericFunctions::GetTestTrayLoaction_Hands(int Hole)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QPoint EmptyTubePos(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,Hole,MOTOR_HANDS_INDEX,EmptyTubePos);
    return EmptyTubePos;
}



/*
* 用户按钮减少耗材映射表
*/
QString GenericFunctions::BiteMapingConsumablesName(const quint8 KindConsumables)
{
    static const QHash<quint8, QString> consumableNames = {
            {INDEX_AA_CONSUMABLE,          "AA"},
            {INDEX_ADP_CONSUMABLE,         "ADP"},
            {INDEX_EPI_CONSUMABLE,         "EPI"},
            {INDEX_COL_CONSUMABLE,         "COL"},
            {INDEX_RIS_CONSUMABLE,         "RIS"},
            {INDEX_CLEANLINQUE_CONSUMABLE, "清洗剂S1(内)"},
            {INDEX_TESTTUBE_CONSUMABLE,    "测试试管"},
            {OUTSIDE_CLEANLINQUE_S2,       "清洗剂S2(外)"}
        };
    return consumableNames.value(KindConsumables, "");
}
