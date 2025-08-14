#ifndef LOADEQUIPMENTPOS_H
#define LOADEQUIPMENTPOS_H

#include <QObject>
#include <QPoint>
#include <stdexcept>

typedef struct REAGENTZONEAXIS{
    quint8 index;
    QString reagname;
    QPoint Axispos;
}REAGENTZONEAXIS_;//试剂区坐标

typedef struct TESTCHNZONEAXIS {
    quint8 indexChn;
    quint8 offsetNeedle;
    QPoint axisPos;
}ChnAxis_; //通道坐标

typedef struct TRYTHECUPAXIS {
    quint8 numTube;
    quint8 offsetNeedle;
    quint8 indexTray;
    QPoint axisPos;
}TRYTHECUPAXIS_;//试杯区

typedef struct SAMPLEBLOODZONEAXISPOS {
    quint8 index;
    QPoint axisPos;
}SAMPLEBLOODZONEAXISPOS_;//血样区坐标

typedef struct EquipmentAXIS
{
    bool    bsycnFinished; //坐标同步状态
    QString equipmentType;
    QString AxixsSaveTime;
    QPoint  OriginAxis;
    quint8  euqipmentIndex;
    QPoint  cleanZoneoffsetRegNedl;
    QPoint  cleanZoneoffsetBlodNedl;
    QPoint  ThrowHoleAxis;
    QList<REAGENTZONEAXIS_* > reagentZoneAxispos;
    QList<ChnAxis_ *> pchnAxisPoint;
    QList<TRYTHECUPAXIS_ *> testTubeZoneAxisPos;
    QList<SAMPLEBLOODZONEAXISPOS_ *> bloodSampleAxisPos;

    // 构造函数
    EquipmentAXIS()
    {
        bsycnFinished = false;
        equipmentType = "";
        AxixsSaveTime = "";
        euqipmentIndex = 0;
        reagentZoneAxispos.clear();
        pchnAxisPoint.clear();
        testTubeZoneAxisPos.clear();
        bloodSampleAxisPos.clear();
    }
    // 析构函数 - 释放所有动态分配的内存
    ~EquipmentAXIS()
    {
        qDeleteAll(reagentZoneAxispos);
        qDeleteAll(pchnAxisPoint);
        qDeleteAll(testTubeZoneAxisPos);
        qDeleteAll(bloodSampleAxisPos);
    }

    // 拷贝构造函数
    EquipmentAXIS(const EquipmentAXIS& other)
    {
        bsycnFinished = other.bsycnFinished;
        equipmentType = other.equipmentType;
        AxixsSaveTime = other.AxixsSaveTime;
        OriginAxis = other.OriginAxis;
        euqipmentIndex = other.euqipmentIndex;
        cleanZoneoffsetRegNedl = other.cleanZoneoffsetRegNedl;
        cleanZoneoffsetBlodNedl = other.cleanZoneoffsetBlodNedl;
        ThrowHoleAxis = other.ThrowHoleAxis;

        // 深拷贝试剂区
        for (const auto& item : other.reagentZoneAxispos) {
            REAGENTZONEAXIS_* newItem = new REAGENTZONEAXIS_;
            *newItem = *item;
            reagentZoneAxispos.append(newItem);
        }

        // 深拷贝通道区
        for (const auto& item : other.pchnAxisPoint) {
            ChnAxis_* newItem = new ChnAxis_;
            *newItem = *item;
            pchnAxisPoint.append(newItem);
        }

        // 深拷贝试杯区
        for (const auto& item : other.testTubeZoneAxisPos) {
            TRYTHECUPAXIS_* newItem = new TRYTHECUPAXIS_;
            *newItem = *item;
            testTubeZoneAxisPos.append(newItem);
        }

        // 深拷贝血样区
        for (const auto& item : other.bloodSampleAxisPos) {
            SAMPLEBLOODZONEAXISPOS_* newItem = new SAMPLEBLOODZONEAXISPOS_;
            *newItem = *item;
            bloodSampleAxisPos.append(newItem);
        }
    }

    // 赋值运算符
    EquipmentAXIS& operator=(const EquipmentAXIS& other)
    {
        if (this != &other) {
            // 释放现有资源
            qDeleteAll(reagentZoneAxispos);
            qDeleteAll(pchnAxisPoint);
            qDeleteAll(testTubeZoneAxisPos);
            qDeleteAll(bloodSampleAxisPos);
            reagentZoneAxispos.clear();
            pchnAxisPoint.clear();
            testTubeZoneAxisPos.clear();
            bloodSampleAxisPos.clear();

            // 复制基本成员
            bsycnFinished = other.bsycnFinished;
            equipmentType = other.equipmentType;
            AxixsSaveTime = other.AxixsSaveTime;
            OriginAxis = other.OriginAxis;
            euqipmentIndex = other.euqipmentIndex;
            cleanZoneoffsetRegNedl = other.cleanZoneoffsetRegNedl;
            cleanZoneoffsetBlodNedl = other.cleanZoneoffsetBlodNedl;
            ThrowHoleAxis = other.ThrowHoleAxis;

            // 深拷贝试剂区
            for (const auto& item : other.reagentZoneAxispos) {
                REAGENTZONEAXIS_* newItem = new REAGENTZONEAXIS_;
                *newItem = *item;
                reagentZoneAxispos.append(newItem);
            }

            // 深拷贝通道区
            for (const auto& item : other.pchnAxisPoint) {
                ChnAxis_* newItem = new ChnAxis_;
                *newItem = *item;
                pchnAxisPoint.append(newItem);
            }

            // 深拷贝试杯区
            for (const auto& item : other.testTubeZoneAxisPos) {
                TRYTHECUPAXIS_* newItem = new TRYTHECUPAXIS_;
                *newItem = *item;
                testTubeZoneAxisPos.append(newItem);
            }

            // 深拷贝血样区
            for (const auto& item : other.bloodSampleAxisPos) {
                SAMPLEBLOODZONEAXISPOS_* newItem = new SAMPLEBLOODZONEAXISPOS_;
                *newItem = *item;
                bloodSampleAxisPos.append(newItem);
            }
        }
        return *this;
    }
}EquipmentAXIS_;




class SingletonAxis
{
public:
    static SingletonAxis *GetInstance();
    static EquipmentAXIS_ *GetpStruct();
    //static std::mutex mutex_;
    static void deleteInstance();
    static void sycnAxisState(bool bWrite, bool &sycnState);   //同步坐标标志
    static void equipmentKind(bool bWrite,QString &kindType);
    static void equipmentKind(bool bWrite,quint8 &kindType);
    static void paraAxisSaveTime(bool bWrite, QString &timesavestr);

    //保存、读取坐标
    static void originPos(const bool &bWrite, QPoint &pos);
    static void throwTubeHolePos(bool bWrite,QPoint &pos);
    static void cleanZoneAxisPos(bool bWrite,int indexNedl,QPoint &pos);
    static void reagetZoneAxisPos(bool bWrite,quint8 indexReag,QPoint &pos);
    static void chnZoneAxisPos(bool bWrite,quint8 numChn,quint8 OffsetNedl ,QPoint &pos);
    static void bloodSampleZonePos(bool bWrite,quint8 numhole,QPoint &pos);


    //空试杯盘坐标
    static quint8 testTaryZoneAxisPos(bool bWrite,quint8 numhole,quint8 indexNeedle,QPoint &pos);
    static void   WriteEmptyTube_Coordinate(quint8 numhole,quint8 indexNeedle,QPoint emptypos);
    static quint8 witchoneindexTary(const quint8 indextube);

    //修改坐标
    static void oper_OriginAxis(bool bNotif_x,int posValue);
    static void oper_ThrowTubeHolePos(bool bNotif_x,int posValue);
    static void oper_CleanZonePos(bool bNotif_x,int indexNedl,int posValue);
    static void oper_ReagentZonePos(bool bNotif_x, quint8 indexReag, quint16 posValue);
    static void oper_TestChnZoneAxispos(bool isXAxis, quint8 numChn, quint8 offsetNedl , quint16 posValue);
    static void oper_bloodSampleZonePos(bool bNotif_x, quint8 numhole, quint16 posValue);
    static void oper_TestTrayZonaPos(bool bNotif_x, quint8 numhole, quint8 indexNeedle, quint16 posValue);
    //坐标映射空号
    static quint8 TeatTayr_findHole(int indexZ,QPoint moved);

    //导入文件把坐标写入到机器
    static bool importtCoordinate(const QString filePath);

    //把坐标写到坐标配置文件
    static bool exportToCoordinateText(const EquipmentAXIS_& equipment, const QString& filePath);

    static bool writeCoordinate(const QString &filePath);

    // 从文本文件导入设备坐标
    static EquipmentAXIS_ importFromCoordinateText(const QString& filePath);

    //根据试剂针移动的位置输出试剂索引
    static quint8 outPutLossReagentIndex(const QPoint &locpos);
    static bool   ismoveXYsuckReagent(const QPoint &locpos);

private:
    SingletonAxis();
    ~SingletonAxis();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    SingletonAxis(const SingletonAxis &signalAxis);
    const SingletonAxis &operator =(const SingletonAxis &signalAxis);
private:
    static SingletonAxis  *g_pSingletonAxis;
    static EquipmentAXIS_ *g_pEquipAxiaspos;
    //static EquipmentAXIS_*g_pEquipAxiaspos;
};


///////////////////////耗材信息//////////////////////////////////
//试剂耗材信息
#define INDEX_AA_CONSUMABLE           0
#define INDEX_ADP_CONSUMABLE          1
#define INDEX_EPI_CONSUMABLE          2
#define INDEX_COL_CONSUMABLE          3
#define INDEX_RIS_CONSUMABLE          4
#define INDEX_CLEANLINQUE_CONSUMABLE  5
#define INDEX_TESTTUBE_CONSUMABLE     6
#define OUTSIDE_CLEANLINQUE_S2        7

#define INDEX_AA_1_CONSUMABLE         8
#define INDEX_ADP_1_CONSUMABLE        9
#define INDEX_EPI_1_CONSUMABLE        10
#define INDEX_COL_1_CONSUMABLE        11
#define INDEX_RIS_1_CONSUMABLE        12

#define MAX_RATIO_SWITCT              512
#define DEFAULT_BOTTLE_COUNT           0 //默认瓶数


typedef struct REAGENT_CONSUMABLES
{
    quint8   indexReag;               //试剂类型 0 - 255
    quint8   remainingNum;            //剩余总量 0 - 255 瓶
    quint16  remain_Ratio;            //剩余百分比单个  512为 100
    quint8   LimitAlarm;              //限位报警
    quint16  SingleBottleCapacity;    //单瓶容量
    QString  Expirationdate;          //有效日期
}REAGENT_CONSUMABLES_;
typedef QVector<REAGENT_CONSUMABLES_* > equipmentConsumablesVec;





class ConsumablesOper
{
public:
    static ConsumablesOper *GetpInstance();
    static void del_Instance();
    static void creatReagentIndex(const quint8 index); //创建耗材结构
    static void iterateOverEquipmentConsumables();   //遍历仪器耗材

    static void getSuppileAllowance(QMap<quint8,quint16> &AllowanceRatio);  //获取机器耗材剩余量(%)
    static void getCleanLinqueAllowance(quint16 &RatioLast);                //获取清洗剂剩余百分比
    static void getSuppileInteger(QMap<quint8, quint8> &AllowanceInteger);  //获的耗材整体余量

    static void sycnTubeTotalTray(bool bwrite, quint8 &num_Bottle);     //读写试杯盘总数

    //试剂的总瓶数(包括清洗剂)
    static void syacnReagentTotalBottle(bool isWrite, quint8 reagentIndex, quint8 &bottleCount);

    //更换整体体耗材比例设置为512
    static void updateReplaceLocRatio(bool isWrite, quint8 indexReag, quint16 &bottleRatio);


    static  void TotalConsumablesAlarm(quint8 indexReag,bool &alarms); //对比总耗材报警

    static void updateReagentTotal(bool bwrite, quint8 indexReag, quint16 &ReagentTol); //更新试剂总容量

    static void updateReagentLimit(bool bwrite,quint8 indexReag,quint8 &limitArlm);     //更新试剂限位

    static bool _ObtainConsumablesBalance(quint8 index_Supplies, quint8 &remainingQuantity); //获得耗材余额


private:
    ConsumablesOper();
    ~ConsumablesOper();

    // 将其拷贝构造和赋值构造成为私有函数, 禁止外部拷贝和赋值
    ConsumablesOper(const ConsumablesOper &consumable);
    const ConsumablesOper &operator =(const ConsumablesOper &consumable);
private:
    static ConsumablesOper  *g_pConsumable;
    static equipmentConsumablesVec *g_pVecReagentInfo;
};










//////////////////////////坐标信息////////////////////////
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QSqlError>
#include <QMap>
#include <QByteArray>


#define EQUIPMENT_TYPED    0  //仪器的类型

#define EQUIPMENTPARA_I    1 //仪器模组 通道开关 条形码
#define HANDSPARADATA_II   2 //仪器抓手参数

#define REAGENT_LIMIT      3 //试剂报警线和清洗液总量

#define BlOODPINPARADATA        4 //血样针参数
#define BLOODPINPARAOTHERDATA   22 //血样针参数all

#define REAGENT_CAPACITY   5 //试剂的容量


#define AXIS_ORIGIN_X      6  //原点 清洗液 试剂区 弃杯孔 x
#define AXIS_ORIGIN_Y      7  //原点 清洗液 试剂区 弃杯孔 Y

#define AXIS_CHN1_5_X      8  //通道1-5offset 试剂针 x
#define AXIS_CHN1_5_Y      9  //通道1-5offset 试剂针 Y

#define AXIS_CHN6_10_X     10  //通道6-10offset 试剂针 x
#define AXIS_CHN6_10_Y     11  //通道6-10offset 试剂针 Y

#define AXIS_CHN11_12_X    12  //通道11-12offset 试剂针 x 1-3抓手x
#define AXIS_CHN11_12_Y    13  //通道11-12offset 试剂针 y 1-3抓手y


#define AXIS_CHN4_8_HANDSX  14 //通道 4-8抓手x
#define AXIS_CHN4_8_HANDSY  15 //通道 4-8抓手Y

#define AXIS_CHN9_12_HANDSX  16 //通道 9-12抓手x 和样本区血样针 x
#define AXIS_CHN9_12_HANDSY  17 //通道 9-12抓手Y 和样本区血样针 y

#define AXIS_TRAY_OFFSET_BLOODPINX   18//试管盘 血样针 1-4 x 和抓手盘 1

#define AXIS_TRAY_OFFSET_BLOODPINY   19 //试管盘 血样针 1-4 y 和抓手盘 1y

#define AXIS_TRAY_OFFSET_HANDSX      20 //试管盘抓手 x

#define AXIS_TRAY_OFFSET_HANDSY      21 //试管盘抓手 y

#define PARAREAGENTPINDATA_I         23
#define PARAREAGENTPINDATA_II        24
#define PARALIMINTBOTTLE             25  //0X19

#define CONTROLGRIPPERPARA           26  //0X1A




#define MAINBORD_REAGENT_AA_INFO            100 //主板 AA试剂信息
#define MAINBORD_REAGENT_ADP_INFO           101
#define MAINBORD_REAGENT_EPI_INFO           102
#define MAINBORD_REAGENT_COL_INFO           103
#define MAINBORD_REAGENT_RIS_INFO           104
#define MAINBORD_REAGENT_CLEAN_S1_INFO      105

#define MAINBORD_REAGENT_TESTTUBE_INFO      106
#define MAINBORD_REAGENT_CLEAN_S2_INFO      107

#define MAINBORD_REAGENT_AA_1_INFO           108
#define MAINBORD_REAGENT_ADP_1_INFO          109
#define MAINBORD_REAGENT_EPI_1_INFO          110
#define MAINBORD_REAGENT_COL_1_INFO          111
#define MAINBORD_REAGENT_RIS_1_INFO          112


struct READPARAMENTER{
    QByteArray _readparaorder;
    bool readfinish;
};

struct WRITEPARAMENTER
{
    QByteArray _writeparaorder;
    bool _writefinish;
};
struct AXISPOSGROUPER{
    quint16 oringin_x;
    quint16 oringin_y;

    quint16 cleanoffBloodPin_x;
    quint16 cleanoffBloodPin_y;

    quint16 cleanoffReagPin_x;
    quint16 cleanoffReagPin_y;

    quint16 ReagZone_x;
    quint16 ReagZone_y;

    quint16 ThrowCups_x;
    quint16 ThrowCups_y;



};
class loadEquipmentPos : public QObject
{
    Q_OBJECT
public:
    explicit loadEquipmentPos(QObject *parent = nullptr);
    ~loadEquipmentPos();

signals:
    void closetimercon(bool);

private slots:
    void receiveInfo();
	
public slots:
    void StatrLoad();

    //先设置仪器型号再写坐标
    void writeEquipmenttyped(const quint8 &index_, bool _ExitParaFile, QString _ParaPath);

    void slotwritedataToEquip(const QByteArray arry);

    void _sycnobtainEquipmenttyped(bool Parafilestate,QString ParaFilePath);
public:
	void CloseSerial();



private:
    void handleReadDevicePara(const QStringList hexArray);

    void openLoadSerialPort(const QString Portname);

    //翻译转换2个字节一个坐标的参数
    void Translation_conversion(const QStringList origindata, QMap<quint8, quint16> &conversiondata);

    void recvEquipmentKind(const QStringList hexArry); //0x00  收仪器型号？

    //0X01
    void    recvParaIData(const QStringList hexArry);

    //0x02
    void    recvParaIIdData(const QStringList hexArry);

    //0x03
    void    recvReagentLimit(const QStringList hexArry);

    //0x04
    void    recvBloodPinq16data(const QStringList hexArry);

    //0x16
    void _recvBloodOtherdataAll(const QStringList hexArry);

     //0x17
    void _recvReagentData(const QStringList hexArry);

    //0x18
    void _recvReagentDataOther(const QStringList hexArry);

    //0x19
    void recveBottleLimit(const QStringList hexArry);

    //0x1a 吸杯负压判断值
    void recveNegativePressure(const QStringList hexArry);

    void  _recvReagentCapacity(const QStringList hexArry);

    void _recvOrininAxis(const QStringList hexArry); //0x06 收到原点坐标x...

    void _recvOrininAxisY(const QStringList hexArry); //0x07 收到原点坐标y...

    //通道相对试剂针1-5
    void _recvChnoffsetReagpinXI_V(const QStringList hexArry);
    void _recvChnoffsetReagpinYI_V(const QStringList hexArry);

    //通道相对试剂针6-10
    void _recvChnoffsetReagpinXIV_X(const QStringList ArryRecvdata);
    void _recvChnoffsetReagpinYIV_X(const QStringList ArryRecvdata);

    void _recvChnoffsetReagpinXXI_XII(const QStringList ArryRecvdata); //11 12试剂针 1-3抓手
    void _recvChnoffsetReagpinYXI_XII(const QStringList ArryRecvdata);

    void _recvHandChnVI_IIIV_X(const QStringList ArryRecvdata); //抓手通道 4- 8
    void _recvHandChnVI_IIIV_Y(const QStringList ArryRecvdata);

    void _recvHandsChnX_XIII_X(const QStringList ArryRecvdata); //抓手9 - 12
    void _recvHandsChnX_XIII_Y(const QStringList ArryRecvdata);

    void _recvTraytubeoffsetbloodpin_x(const QStringList ArryRecvdata);
    void _recvTraytubeoffsetbloodpin_y(const QStringList ArryRecvdata);

    void  _recvTraytubeoffsetHands_x(const QStringList ArryRecvdata);
    void  _recvTraytubeoffsetHands_y(const QStringList ArryRecvdata);


    void groupReagentinfo(bool bread);

    void _Parsing_received_messages(const QStringList ArryRecvdata); //解析接收数据

    void _equipmentParaParsing(quint8 index_, const QStringList ArryRecvdata);

    void _mainbordParadata(quint8 indexReagent, const QStringList ArryRecvdata); //读取主板内试剂耗材信息


    void RecvSuippleNextStep(const bool bRead,quint8 finished,quint8 nextSend,quint8 indexReagent,
                                                 quint8 lastBottle,quint16 OneBottleRatio);
    void RecvSuippleNextStepOnlyBottle(const bool bRead,quint8 finished,quint8 nextSend,quint8 indexReagent,quint8 lastBottle);
    void RecvSuippleNextStepOnlyRatio(const bool bRead,quint8 finished,quint8 nextSend,quint8 indexReagent,
                                      quint16 OneBottleRatio);

    void _GroupReadParaCommder(quint8 index_); //发送读取仪器内坐标的命令

    void _completeddel(quint8 index_); //完成的删除

    //写坐标--begin
    void configAxisPoint(QPoint &input,uint x_,uint y_);
    void LoadCoordinateData(QString filePath, QString keydata_x, QString keydata_y,QPoint& outdata);

    void loadParaData(bool _bexit, QString filePath,  QString _key ,quint8& outdata);
    void loadParaData(bool _bexit, QString filePath,  QString _key ,quint16& outdata);
    void loadParaData(bool _bexit, QString filePath,  QString _key ,bool& outdata);
    void loadParaData(bool _bexit, QString filePath,  QString _key ,double& outdata);
    void loadParaData(bool _bexit, QString filePath,  QString _key ,quint32& outdata);
	void loadParaData(bool isexit, QString filePath,  QString key, QVariant& data);



    void _obatinwriteOrder();

    void _sendWriteAxisOrder(quint8 index);

    void _writeFinish(quint8 index);

    //填充坐标写入到仪器
    /*************/
    //原点
    QPoint initOriginAxis(const quint8 &indexEquipment);

    QPoint initCleanLinqueoffsetBloodPin(const quint8 &indexEquipment);

    QPoint initCleanLinqueoffsetReagentPin(const quint8 &indexEquipment);
    QPoint initThrowCupsAxis(const quint8 &indexEquipment);
    QPoint initReagentHoleAxis(const quint8 &indexEquipment);

    void initHandsoffsetChn(const quint8 &indexEquipment);
    void initReagentPinOffsetChn(const quint8 &indexEquipment);
    void initBloodZoneAxisPos(const quint8 &indexEquipment);

    void initTrayTubeOffsetHands(const quint8 &indexEquipment);
    void initTrayTubeOffsetBloodPin(const quint8 &indexEquipment);

    void initOrderNumI();

    void initWriteHandsParaII(bool _bexit,QString _path);

    void _initwriteBloodPinParaX(bool _bexit,QString _path);//0x04

    void _initwriteBloodpinotherPara(bool _bexit,QString _path); //0x16

    void _initwritereagentParaDataI(bool _bexit,QString _path);//0X17-0X18


    /**把参数写入板子 0x19
     * @brief initWriteParaintoEquipment
     * @param hexbuf  16进制字符
     * @param configFileExit 配置文件是否存在
     * @param pathFile  配置文件路径
     */
    QByteArray initWriteParaintoEquipment();

    //抓手参数
    QByteArray initWriteGripperPara();

    void initwriteMainReagNum();//写主板内耗材数



signals:
    void _whiletoReadEquipPosAixs(quint8, QString); //读取到仪器型号 直接读取坐标

    void progresstotal(int totalsize); //读或者写的总进度数

    void sendUpdateProgressshow(bool);

    void setEquipmentIndex();

private:
    QSerialPort *minitPort = nullptr;

    bool mcreatSetType = false;       //设置仪器类型标志


    bool mequipnotconnect; //未连接提示一次标志
    QString mserialname ;

    bool m_bParafileExit;
    QString m_ParaFilePath;


    QMap<quint8,READPARAMENTER*> mwriteAxismap; //读取仪器内参数
    QMap<quint8,WRITEPARAMENTER*> m_axiswriteequipment; //把坐标写入到仪器内
    bool m_bReadorWrite;   //true 写 false 读
};

#endif // LOADEQUIPMENTPOS_H
