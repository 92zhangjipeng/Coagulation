#pragma execution_character_set("utf-8")

#include "sampledataprocess.h"
#include "testing.h"
#include "sendusbcommand.h"
#include <QDir>
#include "loginui.h"





SampleDataProcess::SampleDataProcess(QObject *parent) : QObject(parent)
{

}
SampleDataProcess::~SampleDataProcess()
{

}

//样本下标映射样本数据库键
QString IndexPorgectMapString(quint8 IndexReagents ,bool Result)
{
    QString UPdataKey;
    if (!Result)
	{
		switch (IndexReagents)
		{
            case AA_REAGENT:  UPdataKey = "AA数据"; break;
            case ADP_REAGENT: UPdataKey = "ADP数据"; break;
            case EPI_REAGENT: UPdataKey = "EPI数据"; break;
            case COL_REAGENT: UPdataKey = "COL数据"; break;
            case RIS_REAGENT: UPdataKey = "RIS数据"; break;
            default:
                UPdataKey = "AA结果";
            break;
		}
	}
	else
	{
		switch (IndexReagents)
		{
            case AA_REAGENT:  UPdataKey = "AA";  break;
            case ADP_REAGENT: UPdataKey = "ADP"; break;
            case EPI_REAGENT: UPdataKey = "EPI"; break;
            case COL_REAGENT: UPdataKey = "COL"; break;
            case RIS_REAGENT: UPdataKey = "RIS"; break;
			default:
				UPdataKey = "AA";
            break;
		}    
    }
    return UPdataKey;
}
































































































