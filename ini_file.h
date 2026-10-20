#ifndef INI_FILE_H
#define INI_FILE_H
#include <QSettings>
#include <QString>
#include <QCoreApplication>
#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif


#define SCANCODEBAR            "ScanCodeBar"            //扫码
#define INITGRABCUPS           "initGrabCups"           //开机初始化抓通道内杯子
#define USINGASIDEREAGENTLOC   "UsingAsideRengentLoc"   //启用试剂预留位置
#define FIRSTSUCKAIRS          "FirstSuckAirs"          //吸血样前先吸空气
#define REPEATGRABCUP          "RepeattheGrabCup"       //抓手重抓


#define MODULETEMPA            "ModuleTemperature1"   //模组温度设置
#define MODULETEMPB            "ModuleTemperature2"
#define MODULETEMPC            "ModuleTemperature3"

//抓手吸杯判断值
#define GRIPPERLESSTHANMAX   "GripperSuctioncupMax"
#define GRIPPERBIGTHANMIM    "GripperSplitcupMin"
#define GRIPPERSUCKTIME      "TimelapseSucking"


//测试K值
#define MAXK1    "maxk1"
#define MINK2    "mink2"
#define MAXK2    "maxk2"
#define MINK3    "mink3"
#define MAXK3    "maxk3"
#define MINK4    "mink4"
#define RATIOK1   "ratiok1"
#define RATIOK3   "ratiok3"
#define RATIOK4   "ratiok4"


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

    //试管盘管理
    quint8  getTubeManager();
    void    setTubeManager(const quint8 currentTray);




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

    void   setRefBottomDistance(const double distance);
    double getRefBottomDistance(void);


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

    /*探测失败高度（清洗剂）样本针*/
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

    // 吸完试剂后延时抬针（试剂针参数，与样本针吸样抬针延时独立）
    void SetReagentAfterSuckLiftDelayMs(int ms);
    int GetReagentAfterSuckLiftDelayMs();

    /*吸取清洗剂 == 清洗试剂针*/
    void    SetAbsorbWashingfluidX1(quint8 ul_value);
    quint8  GetAbsorbWashingfluidX1();

    /*吸取清洗剂 ==清洗样本针*/
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

    void   setPEAddSuckRatio(double ratioval);
    double getPEAddSuckRatio(void);



    //PPP 和PRP 抬针前延时毫秒
    int GetAfterSpitDelayMs();
    void SetAfterSpitDelayMs(const int Ms);


    int GetAfterSuckDelayMs();
    void SetAfterSuckDelayMs(const int Ms);

    //吸多百分比PRP
    int     GetAddSuckPRP();
    void   SetAddSuckPRP(int num);

    /*
    * 胶原启动数据舍弃
    * @brief setColTestGiveupNum
    * @param num
    */
    void setColTestGiveupNum(quint8 num);
    quint8 getColTestGiveupNum();

    /*
    * 测试偏移
    * @brief setoffsetTestRatio
    */
    void  setoffsetTestRatio(quint8 type, float Percentage);
    float getoffsetTestRatio(quint8 type);


    /** 试剂反应低于  Percentage 触发无效
     * @brief setTriggerInvalid
     * @param Percentage
     */
    void  setTriggerInvalid(int reagentIndex,quint8 Percentage);
    quint8 getTriggerInvalid(int reagentIndex);

    void setTriggerOffset(quint8 type, quint8 val); //偏移比
    quint8 getTriggerOffset(quint8 type);

    /** 触发无效试剂后 自定义生成范围
     * @brief setInvalidMaxAndMin
     * @param key
     * @param Perce
     */
    void setInvalidMax(quint8 type, quint8 maxPerce);
    quint8 getInvalidMax(quint8 type);

    void setInvalidMin(quint8 type, quint8 minPerce);
    quint8 getInvalidMin(quint8 type);


    /*
     * 粘附率
     * @brief INI_File::setPrintAdhesion
     * @param show
     */
    void setPrintAdhesion(const bool show);
    bool getPrintAdhesion();


    //写入基础参数
    bool wConfigPara(const QString& keyValue, const QVariant& data);
    bool wBatchConfigPara(const QVariantMap& keyValues);


    //读取基本参数
    QVariant rConfigPara(const QString& keyValue);




private:
    QString m_qstrFileName;
    QSettings *m_psetting; 
    QString m_Section_Key;
    static const QString Instrument_parameters;
    static const QString KEY_REF_BOTTOM_DISTANCE;
    static const QString kPrintPara;
    static const QString kTestContl;
    QString buildKey(quint8 type, const QString& paramName) const;

};

#endif // INI_FILE_H
