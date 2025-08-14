#pragma execution_character_set("utf-8")
#include "ini_file.h"
#include <QtCore/QtCore>
#include <QFile>
#include <QDebug>
#include "cglobal.h"
#include "QsLog/include/QsLog.h"


INI_File::INI_File()
{
    m_qstrFileName = QCoreApplication::applicationDirPath() + "/Config.ini";
    m_psetting = new QSettings(m_qstrFileName,QSettings::IniFormat);
    m_psetting->setIniCodec("UTF-8");
    m_Section_Key = "";

}

INI_File::~INI_File()
{
    delete m_psetting;
    m_psetting = nullptr;
}

//写入基础参数
bool INI_File::wConfigPara(const QString& keyValue, const QVariant& data)
{
    static const QString wBasicParaTab = "BasicPara";
    QString wKey = QString("%1/%2").arg(wBasicParaTab).arg(keyValue);
    m_psetting->setValue(wKey,data);
	return (m_psetting->status() == QSettings::NoError);
}
//批量w
bool INI_File::wBatchConfigPara(const QVariantMap& keyValues){
    static const QString wBasicParaTab = "BasicPara";
    for (auto it = keyValues.constBegin(); it != keyValues.constEnd(); ++it) {
        QString wKey = QString("%1/%2").arg(wBasicParaTab).arg(it.key());
        m_psetting->setValue(wKey, it.value());
    }
    m_psetting->sync();
    return (m_psetting->status() == QSettings::NoError);
}



//读取基本参数
QVariant INI_File::rConfigPara(const QString& keyValue)
{
    static const QString wBasicParaTab = "BasicPara";
    QString rKey = QString("%1/%2").arg(wBasicParaTab).arg(keyValue);
    return m_psetting->value(rKey);

}



void INI_File::setexperimentalMode(const bool& experimenta)
{
    QString keywrite = QString("%1/%2").arg(Instrument_parameters).arg("ExperimentalMode");
    m_psetting->setValue(keywrite,experimenta);

}
bool INI_File::getexperimentalMode(){
    QString keywrite = QString("%1/%2").arg(Instrument_parameters).arg("ExperimentalMode");
    return  m_psetting->value(keywrite).toBool();
}


int INI_File::getFilteringMode()
{
    const QString keywrite = QString("%1/%2").arg(Instrument_parameters).arg("FilteringMode");
     return  m_psetting->value(keywrite).toInt();

}
void INI_File::setFilteringMode(const int indexMode){
    const QString keywrite = QString("%1/%2").arg(Instrument_parameters).arg("FilteringMode");
    m_psetting->setValue(keywrite,indexMode);
}


quint8 INI_File::getInstrumentType()
{
    QString sectionKey = QString("%1/%2").arg(Instrument_parameters).arg("ConfigureChannelNum");
    return  m_psetting->value(sectionKey).toUInt();
}





void INI_File::setWashesTime(quint32 times)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("WashesTime");
    m_psetting->setValue(m_Section_Key,times);
}
quint32 INI_File::getWashesTime()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("WashesTime");
    return m_psetting->value(m_Section_Key).toInt();
}


void INI_File::setModuledimmingVal(quint8 indexModuled,int dimmingVal)
{
    m_Section_Key = QString("%1/Moduledimming%2Val").arg(Instrument_parameters).arg(indexModuled);
    m_psetting->setValue(m_Section_Key,dimmingVal);
}
int  INI_File::getModuledimmingVal(quint8 indexModuled)
{
     m_Section_Key = QString("%1/Moduledimming%2Val").arg(Instrument_parameters).arg(indexModuled);
     return m_psetting->value(m_Section_Key).toInt();
}






//空回值
void INI_File::SetSecurityValue(int value)
{
     m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("NullBackValue");
     m_psetting->setValue(m_Section_Key,value);
}
int INI_File::GetSecurityValue()
{
     m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("NullBackValue");
     return m_psetting->value(m_Section_Key).toInt();
}


/**
 * @brief INI_File::SetFixedHigh
 * @param high 测高物理值（蓝色基准物到针的垂直距离）
 */
void INI_File::SetFixedHigh(double high)
{
     m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("PhysicalHeight");
     m_psetting->setValue(m_Section_Key,high);
}

double INI_File::GetFixedHigh()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("PhysicalHeight");
    return m_psetting->value(m_Section_Key).toDouble();
}

/**
 * @brief INI_File::SetTestDifference
 * @param UpOffsetmm PPP最底部向上偏移高度
 */
void  INI_File::SetTestDifference(double UpOffsetmm )
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("TestHeightDifference");
    m_psetting->setValue(m_Section_Key,UpOffsetmm);
}

double INI_File::GetTestDifference(void)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("TestHeightDifference");
    return m_psetting->value(m_Section_Key).toDouble();
}






//抓手在通道处的下降高度
void INI_File::_sethandsdownheiht(quint8 indexchn,quint8 downmm)
{
    quint8 ModuleNum = 0;
    if(indexchn >= 0 && indexchn < 4)
    {
        ModuleNum = MODULE_1;
    }
    if(indexchn >= 4 && indexchn < 8)
    {
        ModuleNum = MODULE_2;
    }
    if(indexchn >= 8 && indexchn < 12)
    {
        ModuleNum = MODULE_3;
    }
    QString key_ = QString("%1/HandsdownHeightinModule%2").arg(Instrument_parameters).arg(ModuleNum);
    m_psetting->setValue(key_,downmm);
    //QLOG_DEBUG()<<"模组"<<ModuleNum<<"设置下降深度="<<downmm<<endl;
}

quint8  INI_File::_gethandsdownheiht(quint8 indexchn)
{
    // 检查索引有效性：处理无效输入，避免未定义行为
    if (indexchn >= 12) {
        // 可扩展为日志记录或返回默认值，提升健壮性
        return 0; // 假设 0 为无效值，或根据需求调整
    }

    // 计算模块号：使用整数除法简化逻辑，假设 MODULE_1=1, MODULE_2=2, MODULE_3=3
    quint8 ModuleNum = (indexchn / 4) + 1; // 除法确保整数截断，匹配原始范围

    // 构建键字符串：使用常量格式提升可维护性
    const QString keyFormat = "%1/HandsdownHeightinModule%2";
    QString key_ = keyFormat.arg(Instrument_parameters).arg(ModuleNum);

    // 获取并返回值
    return m_psetting->value(key_).toUInt();
}


//抓手下降高度 弃杯孔
void  INI_File::_sethandsdownthrowcpus(quint8 _downmm)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("GripperDownThrowcups");
    m_psetting->setValue(m_Section_Key,_downmm);
}
quint8  INI_File::_gethandsdownthrowcpus()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("GripperDownThrowcups");
    return m_psetting->value(m_Section_Key).toUInt();
}


//抓手在试管盘下降高度
void INI_File::_sethandsdownheightinTesttray(quint8 indextary,quint8 downmm)
{
    QString key_ = QString("%1/HandsdownHeightinTray%2").arg(Instrument_parameters).arg(indextary);
    m_psetting->setValue(key_,downmm);
}
quint8 INI_File::_gethandsdownheightinTesttray(quint8 indextary)
{
    QString key_ = QString("%1/HandsdownHeightinTray%2").arg(Instrument_parameters).arg(indextary);
    return m_psetting->value(key_).toUInt();
}


/*
* 吸取试剂量测试项目时吸取测试试剂单次量
* 吸取试剂量小泵
*/
void INI_File::SetLearnReagentvolume(double volume)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("TaketheAmountofReagent");
    m_psetting->setValue(m_Section_Key,volume);
}

double INI_File::GetLearnReagentvolume()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("TaketheAmountofReagent");
    return m_psetting->value(m_Section_Key).toDouble();
}

void  INI_File::setTypesReagentSuckVolume(quint8 IndexReagent,quint8 VolumeNum)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_AAvolume");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_ADPvolume");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_EPIvolume");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_COLvolume");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_RISvolume");   break;
        default:
            QLOG_WARN()<<"配置吸试剂量失败";
        break;
    }
    m_psetting->setValue(configKey,VolumeNum);
}

unsigned char INI_File::getTypesReagentSuckVolume(quint8 IndexReagent)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_AAvolume");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_ADPvolume");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_EPIvolume");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_COLvolume");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_RISvolume");   break;
        default:
            QLOG_WARN()<<"获取吸试剂量失败";
        break;
    }
    return m_psetting->value(configKey).toInt();
}



void  INI_File::setTypesReagentSuckRatio(const quint8 IndexReagent,double VolumeRatio)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_AA_Ratio");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_ADP_Ratio");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_EPI_Ratio");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_COL_Ratio");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_RIS_Ratio");   break;
        default:
            QLOG_WARN()<<"配置吸试剂比例系数失败";
        break;
    }
    m_psetting->setValue(configKey,VolumeRatio);
}
double INI_File::getTypesReagentSuckRatio(const quint8 IndexReagent)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_AA_Ratio");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_ADP_Ratio");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_EPI_Ratio");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_COL_Ratio");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_RIS_Ratio");   break;
        default:
            QLOG_WARN()<<"获取吸试剂比例系数失败";
        break;
    }
    return m_psetting->value(configKey).toDouble();
}

void    INI_File::setTypesReagentSuckAdd_Ratio(const quint8 IndexReagent, double VolumeRatio)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_AA_AddRatio");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_ADP_AddRatio");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_EPI_AddRatio");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_COL_AddRatio");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_RIS_AddRatio");   break;
        default:
            QLOG_WARN()<<"配置吸试剂添加比例系数失败";
        break;
    }
    m_psetting->setValue(configKey,VolumeRatio);
}
double  INI_File::getTypesReagentSuckAdd_Ratio(const quint8 IndexReagent)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_AA_AddRatio");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_ADP_AddRatio");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_EIP_AddRatio");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_COL_AddRatio");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SuckReagent_RIS_AddRatio");   break;
        default:
            QLOG_WARN()<<"获取吸试剂添加比例系数失败";
        break;
    }
    return m_psetting->value(configKey).toDouble();
}

void INI_File::setTypesReagentNeedleDownHigh(const quint8 IndexReagent, quint8 downHigh)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_AA_downHigh");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_ADP_downHigh");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_EPI_downHigh");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_COL_downHigh");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_RIS_downHigh");   break;
        default:
            QLOG_WARN()<<"配置吐试剂到测试通道失败";
        break;
    }
    m_psetting->setValue(configKey,downHigh);
}
quint8 INI_File::getTypesReagentNeedleDownHigh(const quint8 IndexReagent)
{
    QString configKey = "";
    switch(IndexReagent)
    {
        case AA_REAGENT:    configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_AA_downHigh");    break;
        case ADP_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_ADP_downHigh");   break;
        case EPI_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_EPI_downHigh");   break;
        case COL_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_COL_downHigh");   break;
        case RIS_REAGENT:   configKey = QString("%1/%2").arg(Instrument_parameters).arg("SpitReagent_RIS_downHigh");   break;
        default:
            QLOG_WARN()<<"获取吐试剂到测试通道失败";
        break;
    }
    return m_psetting->value(configKey).toUInt();
}


/*
*吸取样本量
*吸取样本血样量大泵
*/
void INI_File::SetLearnSamplevolume(double volume)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("TaketheSampleSize");
    m_psetting->setValue(m_Section_Key,volume);
}
double INI_File::GetLearnSamplevolume()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("TaketheSampleSize");
    return m_psetting->value(m_Section_Key).toDouble();
}


/*
*
* 空试管区下针高度(血样针下针高度)
*/
void INI_File::SetEmptyTubeDownHigh(double high)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("DownHighGrabTestCups");
    m_psetting->setValue(m_Section_Key,high);
}
double INI_File::GetEmptyTubeDownHigh()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("DownHighGrabTestCups");
    return m_psetting->value(m_Section_Key).toDouble();
}


/*
* 探测失败高度（贫血）
*/
void INI_File::SetFailedLinqueHigh(int highvalue)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("AnaemiaDetectionDownHigh");
    m_psetting->setValue(m_Section_Key,highvalue);
}
int INI_File::GetFailedLinqueHigh()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("AnaemiaDetectionDownHigh");
    return m_psetting->value(m_Section_Key).toInt();
}

/*
* 探测失败高度（清洗剂=血样针）
*/
void INI_File::SetFailedCleanLinqueHigh(int highValue)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("CleanReagentDetectionDownHigh");
    m_psetting->setValue(m_Section_Key,highValue);
}
int INI_File::GetFailedCleanLinqueHigh()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("CleanReagentDetectionDownHigh");
    return  m_psetting->value(m_Section_Key).toInt();
}

void INI_File::setFailedCleanLinqueReagNeedle(quint8 failedhigh)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("CleanReagentDetectionDownHigh_reagneedle");
    m_psetting->setValue(m_Section_Key,failedhigh);
}
quint8 INI_File::getFailedCleanLinqueReagNeedle()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("CleanReagentDetectionDownHigh_reagneedle");
    return  m_psetting->value(m_Section_Key).toInt();
}

/*
* 探测失败高度（试剂）
*/
void INI_File::SetFailedReagentsLinqueHigh(quint8 highValue)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("ReagentDetectionDownHigh");
    m_psetting->setValue(m_Section_Key,highValue);
}
quint8 INI_File::GetFailedReagentsLinqueHigh()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("ReagentDetectionDownHigh");
    return  m_psetting->value(m_Section_Key).toInt();
}

void INI_File::_setsuckairsuckPRP(int data_)
{
    m_Section_Key ="";
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("suckPRPsuckair");
    m_psetting->setValue(m_Section_Key,data_);
}
int  INI_File::_getsuckairsuckPRP()
{
    m_Section_Key ="";
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("suckPRPsuckair");
    return  m_psetting->value(m_Section_Key).toInt();
}

void INI_File::_setdelayedtime(int deltime_)
{
    m_Section_Key ="";
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("delayedtime");
    m_psetting->setValue(m_Section_Key,deltime_);
}

int INI_File::_getdelayedtime()
{
    m_Section_Key ="";
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("delayedtime");
    return  m_psetting->value(m_Section_Key).toInt();
}

/*
*吸取清洗剂X1 的量(洗试剂针)
*吸取清洗剂 == 清洗试剂针
*/
void  INI_File::SetAbsorbWashingfluidX1(quint8 ul_value)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("WashReagentNeedleAbsorbCleanning");
    m_psetting->setValue(m_Section_Key,ul_value);
}
quint8  INI_File::GetAbsorbWashingfluidX1()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("WashReagentNeedleAbsorbCleanning");
    return m_psetting->value(m_Section_Key).toInt();
}

/*
*吸取清洗剂 ==清洗血样针
* 吸取清洗剂X2 的量(洗血样针)
*/
void  INI_File::SetAbsorbWashingfluidX2(int ul_value)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("WashBloodSampleNeedleAbsorbCleanning");
    m_psetting->setValue(m_Section_Key,ul_value);
}
int  INI_File::GetAbsorbWashingfluidX2()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("WashBloodSampleNeedleAbsorbCleanning");
    return m_psetting->value(m_Section_Key).toInt();
}

/*
* 测高模式(全血模式)
*  吸样本的模式
*/
void INI_File::SetWholeBloodModel(bool wholemode)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("SuctionMode");
    m_psetting->setValue(m_Section_Key,wholemode);
}

bool INI_File::GetWholeBloodModel()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("SuctionMode");
    return m_psetting->value(m_Section_Key).toBool();
}


/* 吸血样为血僵模式时下针的高度*/
void INI_File::SetAbsorbTubeBottom(quint8 downmm)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("NeedleInPlasmaMode");
    m_psetting->setValue(m_Section_Key,downmm);
}
quint8 INI_File::GetAbsorbTubeBottom()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("NeedleInPlasmaMode");
    return m_psetting->value(m_Section_Key).toUInt();
}


void  INI_File::SetPPPConversionScale(double ratioValue)
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("PPPConversionScale");
    m_psetting->setValue(m_Section_Key,ratioValue);
}

double INI_File::GetPPPConversionScale()
{
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("PPPConversionScale");
    return m_psetting->value(m_Section_Key).toDouble();
}


void   INI_File::setPRPConvertTheratioColumn(double ratioval){
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("PRPConvertTheratioColumn");
    m_psetting->setValue(m_Section_Key,ratioval);
}

double INI_File::getPRPConvertTheratioColumn(void){
    m_Section_Key = QString("%1/%2").arg(Instrument_parameters).arg("PRPConvertTheratioColumn");
    return m_psetting->value(m_Section_Key).toDouble();
}

















