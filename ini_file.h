#ifndef INI_FILE_H
#define INI_FILE_H
#include <QSettings>
#include <QString>
#include <QCoreApplication>
#pragma execution_character_set("utf-8")


#define SCANCODEBAR            "ScanCodeBar"            //扫码
#define INITGRABCUPS           "initGrabCups"           //开机初始化抓通道内杯子
#define USINGASIDEREAGENTLOC   "UsingAsideRengentLoc"   //启用试剂预留位置
#define FIRSTSUCKAIRS          "FirstSuckAirs"          //吸血样前先吸空气
#define MODULETARGETBASICVAL   "moduleTargetBasicValue" //调光基础值
#define REPEATGRABCUP          "RepeattheGrabCup"       //抓手重抓


#define MODULETEMPA            "ModuleTemperature1"   //模组温度设置
#define MODULETEMPB            "ModuleTemperature2"
#define MODULETEMPC            "ModuleTemperature3"

//抓手吸杯判断值
#define GRIPPERLESSTHANMAX   "GripperSuctioncupMax"
#define GRIPPERBIGTHANMIM    "GripperSplitcupMin"
#define GRIPPERSUCKTIME      "TimelapseSucking"


class INI_File
{
public:
    INI_File();
    virtual ~INI_File();

    quint8  getInstrumentType();

    //实验模式
    void setexperimentalMode(const bool& experimenta);
    bool getexperimentalMode();

    //滤波模式
    int  getFilteringMode();
    void setFilteringMode(const int indexMode);



    /**  柱塞泵清洗时间
     * @brief setWashesTime
     * @param times
     */
    void    setWashesTime(quint32 times);
    quint32 getWashesTime();


    /**  设置获取模组调光值
     * @brief setModuledimmingVal
     * @param indexModuled
     * @param dimmingVal
     */
    void setModuledimmingVal(quint8 indexModuled,int dimmingVal);
    int  getModuledimmingVal(quint8 indexModuled);


    /*空回值*/
    void SetSecurityValue(int value);
    int  GetSecurityValue();

    /*测高物理值（蓝色基准物到针的垂直距离）*/
    void    SetFixedHigh(double high);
    double  GetFixedHigh();

    //测高下降的差值
    void   SetTestDifference(double UpOffsetmm);
    double GetTestDifference(void);


    //抓手在通道的下针高度
    void    _sethandsdownheiht(quint8 indexchn,quint8 downmm);
    quint8  _gethandsdownheiht(quint8 indexchn);

    //抓手下降高度 弃杯孔
    void    _sethandsdownthrowcpus(quint8 _downmm);
    quint8  _gethandsdownthrowcpus();

    //抓手在试管盘下降高度
    void    _sethandsdownheightinTesttray(quint8 indextary,quint8 downmm);
    quint8  _gethandsdownheightinTesttray(quint8 indextary);




    /*吸取试剂量测试项目时吸取测试试剂单次量   ****准备启用********************/
    void    SetLearnReagentvolume(double volume);
    double  GetLearnReagentvolume();


    /*吸各种试剂量的量*/
    void    setTypesReagentSuckVolume(const quint8 IndexReagent, quint8 VolumeNum);
    unsigned char  getTypesReagentSuckVolume(const quint8 IndexReagent);

    /*吸试剂的比例系数*/
    void    setTypesReagentSuckRatio(const quint8 IndexReagent, double VolumeRatio);
    double  getTypesReagentSuckRatio(const quint8 IndexReagent);

    /*吸多余的试剂比例*/
    void    setTypesReagentSuckAdd_Ratio(const quint8 IndexReagent, double VolumeRatio);
    double  getTypesReagentSuckAdd_Ratio(const quint8 IndexReagent);

    /*试剂加到通道下降高度*/
    void   setTypesReagentNeedleDownHigh(const quint8 IndexReagent, quint8 downHigh);
    quint8 getTypesReagentNeedleDownHigh(const quint8 IndexReagent);

    /* 吸取样本量*/
    void   SetLearnSamplevolume(double volume);
    double GetLearnSamplevolume();


    /*空试管区下针高度*/
    void   SetEmptyTubeDownHigh(double);
    double GetEmptyTubeDownHigh();

    /*探测失败高度（贫血）*/
    void SetFailedLinqueHigh(int highvalue);
    int  GetFailedLinqueHigh();

    /*探测失败高度（清洗剂）血样针*/
    void SetFailedCleanLinqueHigh(int highValue);
    int  GetFailedCleanLinqueHigh();

    /*试剂针清洗探测失败高度*/
    void setFailedCleanLinqueReagNeedle(quint8 );
    quint8 getFailedCleanLinqueReagNeedle();

    /* 探测失败高度（试剂）*/
    void SetFailedReagentsLinqueHigh(quint8 highValue);
    quint8 GetFailedReagentsLinqueHigh();

    //吸富血前吸空气量
    void _setsuckairsuckPRP(int data_);
    int  _getsuckairsuckPRP();

    //吐试剂前延时
    void _setdelayedtime(int deltime_);
    int  _getdelayedtime();

    /*吸取清洗剂 == 清洗试剂针*/
    void    SetAbsorbWashingfluidX1(quint8 ul_value);
    quint8  GetAbsorbWashingfluidX1();

    /*吸取清洗剂 ==清洗血样针*/
    void    SetAbsorbWashingfluidX2(int ul_value);
    int     GetAbsorbWashingfluidX2();



    /* 测高模式(全血模式)吸样本的模式*/
    void    SetWholeBloodModel(bool);
    bool    GetWholeBloodModel();

    /* 吸血样为血浆模式时下针的高度*/
    void    SetAbsorbTubeBottom(quint8 downmm);
    quint8  GetAbsorbTubeBottom();


    /*吸吐PPP转换比例*/
    void    SetPPPConversionScale(double ratioValue);
    double  GetPPPConversionScale();

    //吸PRP样本 转换比
    void   setPRPConvertTheratioColumn(double ratioval);
    double getPRPConvertTheratioColumn(void);


    //写入基础参数
    bool wConfigPara(const QString& keyValue, const QVariant& data);
    bool wBatchConfigPara(const QVariantMap& keyValues);


    //读取基本参数
    QVariant rConfigPara(const QString& keyValue);

private:
    QString m_qstrFileName;
    QSettings *m_psetting; 
    const QString Instrument_parameters = "InstrumentParameters";
    QString m_Section_Key;

};

#endif // INI_FILE_H
