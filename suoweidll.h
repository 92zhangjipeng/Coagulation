#ifndef SUOWEIDLL_H
#define SUOWEIDLL_H

//#include "suoweidll_global.h"
#include <QtGui/QtGui>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QSettings>
#include <QObject>


#if defined(SUOWEIDLL_LIBRARY)
#  define SUOWEIDLLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define SUOWEIDLLSHARED_EXPORT Q_DECL_IMPORT
#endif

#define HEX_DATA    16
#define HEX_LEN_F   4
#define HEX_LEN_T   2
#define OUTOF_BOUNDS  0
#define READ_PARAMETERS   true
#define WRITE_PARAMETERS  false

typedef struct Instrument_paraInfo
{
    int   config_modutmp[3];
    int   Aspirate_theReagent_ul; //吸试剂量
    int   BloodSampleNeddleDown_mm; //血样针下降高度
    int   Gripper_drops_mm;        //抓手下降高度
    int   Emptyvalue_back;         //空回值
    int   VampireSamples_ul;       //吸单份血样样本量
    double TestHight_physics_mm;   //测高物理高度
    int    AspirateCleaningSolution_ReagentNeedle_ul; //试剂针吸清洗液量
    int    AspirateCleaningSolution_BloodNeedle_ul;   //血样针吸清洗液量
    int    ProbeFailed_cleanSolution_mm;     //清洗液探测失败下降高度
    int    ProbeFailed_ReagentSolution_mm;   //试剂液探测失败下降高度
    int    ProbeFailed_AnaemiaSolution_mm;   //贫血探测失败下降高度
    int    PlasmaMode_down_mm;               //血浆模式下针高度
    bool   Channel_State[12];
    bool   bScanthebarcode;   //扫描条形码
    bool   bcloseClean; //关机清洗
    bool   busedobligateReagent; //启用预留试剂位
    bool   bWholebloodmode;  //全血模式
}Instrument_para;

class SUOWEIDLLSHARED_EXPORT Suoweidll
{
public:
    Suoweidll();
    ~Suoweidll();
public:
    /**
    *  @brief    读/写 仪器参数  命令编号命令头部分
    *  @param    协议命令编号
    *  @param    bool  true == 读  false == 写
    *  return    协议前4个字节位
    */
    QString ScanParameters(const quint8 CodeNum ,const bool bReadParamete,const bool Reflushall);
    /**
    *  @brief    读/写 仪器参数 0X00
    *  @param    bool  true == 读  false == 写
    *  return    协议第4个字节位
    */
    uint ByteIndexFourth(bool bReadPara);

    /**
     * @brief QByteArrayToQStringList
     * @param pHex
     * @param len
     * @return
     */
    QStringList  QByteArrayToQStringList(QByteArray bamessage );



public:
    //只读取到仪器参数写入到INI文件（仪器内已存在参数）
    bool ParseandSetIni_Index(const int Index,const QStringList Recvdata);
    bool ParseandSetIni_0X01(const QStringList Recvdata);
    /**
     *@brief    解析仪器参数功能码 0x02
     *@param    血样针下针高度     5
     *@param    抓手下降高度       6
     *@param    空回值             7
     *@param    吸血样的量         8
     *@param    测高物理高度       9
     *@param    清洗试剂针试剂量   10
     *@param    清洗血样针吸清洗液量   11-12
     *@param    清洗液探测失败高度  13
     *@param    试剂探测失败下针高度  14
    */
    bool ParseandSetIni_0X02(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码 0x03
     *@param    血浆模式下针高度    5
     *@param    贫血探测失败高度    6
     *@param    AA限位报警          7
     *@param    ADP限位报警         8
     *@param    ADR限位报警         9
     *@param    COL限位报警         10
     *@param    RIS限位报警         11
     *@param    测试杯              12
     *@param    清洗剂              13
     **@param   未用                14
    */
    bool ParseandSetIni_0X03(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码 0x04
     *@param    AA 0试剂剩余    5 -6
     *@param    ADP 0试剂剩余   7 -8
     *@param    ADR 0试剂剩余   9 -10
     *@param    COL 0试剂剩余   11 -12
     *@param    RIS 0试剂剩余   13 -14
    */
    bool ParseandSetIni_0X04(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码 0x05
     *@param    AA 1试剂剩余    5 -6
     *@param    ADP 1试剂剩余   7 -8
     *@param    ADR 1试剂剩余   9 -10
     *@param    COL 1试剂剩余   11 -12
     *@param    RIS 1试剂剩余   13 -14
    */
    bool ParseandSetIni_0X05(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码 0x05
     *@param    清洗剂  剩余    5 -6
     *@param    弃杯孔offset抓手X   7 -8
     *@param    弃杯孔offset抓手y   9 -10
     *@param    原点X         11 -12
     *@param    原点Y         13 -14
    */
    bool ParseandSetIni_0X06(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码     0x07
     *@param    清洗剂offset血样针x    5 -6
     *@param    清洗剂offset血样针y    7 -8
     *@param    清洗剂offset试剂针x    9 -10
     *@param    清洗剂offset试剂针y    11 -12
     *@param    未用                   13 -14
    */
    bool ParseandSetIni_0X07(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码      0x08
     *@param    试剂区offset试剂针_x    5 -6
     *@param    试剂区offset试剂针_y    7 -8
     *@param    血样区offset血样针_x    9 -10
     *@param    血样区offset血样针_y    11 -12
     *@param    未用                    13 -14
    */
    bool ParseandSetIni_0X08(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码         0x09
     *@param    空试管区1offset血样针_x    5 -6
     *@param    空试管区1offset血样针_y   7 -8
     *@param    空试管区2offset血样针_x   9 -10
     *@param    空试管区2offset血样针_y   11 -12
     *@param    未用                  13 -14
    */
    bool ParseandSetIni_0X09(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x0a
     *@param    空试管区3offset血样针_x    5 -6
     *@param    空试管区3offset血样针_y   7 -8
     *@param    空试管区4offset血样针_x   9 -10
     *@param    空试管区4offset血样针_y   11 -12
     *@param    未用                  13 -14
    */
    bool ParseandSetIni_0X0a(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x0b
     *@param    空试管区1offset抓手_x    5 -6
     *@param    空试管区1offset抓手_y   7 -8
     *@param    空试管区2offset抓手_x   9 -10
     *@param    空试管区2offset抓手_y   11 -12
     *@param    未用                  13 -14
    */
    bool ParseandSetIni_0X0b(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x0c
     *@param    空试管区3offset抓手_x    5 -6
     *@param    空试管区3offset抓手_y    7 -8
     *@param    空试管区4offset抓手_x    9 -10
     *@param    空试管区4offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X0c(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x0d
     *@param    测试通道1offset抓手_x    5 -6
     *@param    测试通道1offset抓手_y    7 -8
     *@param    测试通道2offset抓手_x    9 -10
     *@param    测试通道2offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X0d(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x0e
     *@param    测试通道3offset抓手_x    5 -6
     *@param    测试通道3offset抓手_y    7 -8
     *@param    测试通道4offset抓手_x    9 -10
     *@param    测试通道4offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X0e(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x0f
     *@param    测试通道5offset抓手_x    5 -6
     *@param    测试通道5offset抓手_y    7 -8
     *@param    测试通道6offset抓手_x    9 -10
     *@param    测试通道6offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X0f(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x10
     *@param    测试通道7offset抓手_x    5 -6
     *@param    测试通道7offset抓手_y    7 -8
     *@param    测试通道8offset抓手_x    9 -10
     *@param    测试通道8offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X10(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x11
     *@param    测试通道9offset抓手_x    5 -6
     *@param    测试通道9offset抓手_y    7 -8
     *@param    测试通道10offset抓手_x    9 -10
     *@param    测试通道10offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X11(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x12
     *@param    测试通道11offset抓手_x    5 -6
     *@param    测试通道11offset抓手_y    7 -8
     *@param    测试通道12offset抓手_x    9 -10
     *@param    测试通道12offset抓手_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X12(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x13
     *@param    测试通道1offset试剂针_x    5 -6
     *@param    测试通道1offset试剂针_y    7 -8
     *@param    测试通道2offset试剂针_x    9 -10
     *@param    测试通道2offset试剂针_y    11 -12
     *@param    未用                     13 -14
    */
    bool ParseandSetIni_0X13(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x14
     *@param    测试通道3offset试剂针_x    5 -6
     *@param    测试通道3offset试剂针_y    7 -8
     *@param    测试通道4offset试剂针_x    9 -10
     *@param    测试通道4offset试剂针_y    11 -12
     *@param    未用                     13 -14
     */
    bool ParseandSetIni_0X14(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x15
     *@param    测试通道5offset试剂针_x    5 -6
     *@param    测试通道5offset试剂针_y    7 -8
     *@param    测试通道6offset试剂针_x    9 -10
     *@param    测试通道6offset试剂针_y    11 -12
     *@param    未用                     13 -14
     */
    bool ParseandSetIni_0X15(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x16
     *@param    测试通道7offset试剂针_x    5 -6
     *@param    测试通道7offset试剂针_y    7 -8
     *@param    测试通道8offset试剂针_x    9 -10
     *@param    测试通道8offset试剂针_y    11 -12
     *@param    未用                     13 -14
     */
    bool ParseandSetIni_0X16(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x17
     *@param    测试通道9offset试剂针_x    5 -6
     *@param    测试通道9offset试剂针_y    7 -8
     *@param    测试通道10offset试剂针_x    9 -10
     *@param    测试通道10offset试剂针_y    11 -12
     *@param    未用                     13 -14
     */
    bool ParseandSetIni_0X17(const QStringList Recvdata);

    /**
     *@brief    解析仪器参数功能码        0x18
     *@param    测试通道11offset试剂针_x    5 -6
     *@param    测试通道11offset试剂针_y    7 -8
     *@param    测试通道12offset试剂针_x    9 -10
     *@param    测试通道12offset试剂针_y    11 -12
     *@param    未用                     13 -14
     */
    bool ParseandSetIni_0X18(const QStringList Recvdata);

public:
    //仪器未空 向仪器内写INI文件参数/修改
    bool  ConfigureInIParaToInstrument_0x01();

    bool  ConfigureInIParaToInstrument_0x02();

    bool  ConfigureInIParaToInstrument_0x03();

    bool ConfigureInIParaToInstrument_0x04();

    bool  ConfigureInIParaToInstrument_0x05();

    bool  ConfigureInIParaToInstrument_0x06();

    bool  ConfigureInIParaToInstrument_0x07();

    bool  ConfigureInIParaToInstrument_0x08();

    bool  ConfigureInIParaToInstrument_0x09();

    bool  ConfigureInIParaToInstrument_0x0a();

    bool  ConfigureInIParaToInstrument_0x0b();

    bool  ConfigureInIParaToInstrument_0x0c();

    /** 设置通道相对抓手的 1 - 2 通道坐标
    * @brief ConfigureInIParaToInstrument_0x12
    * @return
    */
    bool  ConfigureInIParaToInstrument_0x0d();

    bool  ConfigureInIParaToInstrument_0x0e();

    bool  ConfigureInIParaToInstrument_0x0f();

    bool  ConfigureInIParaToInstrument_0x10();

    bool  ConfigureInIParaToInstrument_0x11();

    /** 设置通道相对抓手的 11 - 12 通道坐标
    * @brief ConfigureInIParaToInstrument_0x12
    * @return
    */
    bool  ConfigureInIParaToInstrument_0x12();

    /** 设置通道相对试剂针的 1 - 2 通道坐标
    * @brief ConfigureInIParaToInstrument_0x12
    * @return
    */
    bool  ConfigureInIParaToInstrument_0x13();

    bool  ConfigureInIParaToInstrument_0x14();

    bool  ConfigureInIParaToInstrument_0x15();

    bool  ConfigureInIParaToInstrument_0x16();

    bool  ConfigureInIParaToInstrument_0x17();

    /** 设置通道相对试剂针的 11 - 12 通道坐标
    * @brief ConfigureInIParaToInstrument_0x12
    * @return
    */
    bool  ConfigureInIParaToInstrument_0x18();

public:
    /**获取仪器串口
    * @brief RecveInstrumentPort
    */
    void RecveInstrumentPort(QSerialPort *);
    int  Paraconfigsendserialport(QByteArray );

    /** ini文件
    * @brief RecveiniSetting
    */
    void RecveiniSetting(QSettings *);

public:
    /** 初始化仪器参数数据表
    * @brief InitInstrumentParaSql
    */
    void InitInstrumentParaSql();

    /**判断表是否存在
     * @brief isTableExist
     * @param tableName
     * @return
     */
    bool isTableExist(QString tableName);

    void NotifyPoint();

private:
    QSerialPort *mInstrumentPort = nullptr;
    QSettings *mpInifile = nullptr;
    Instrument_para *mpInstrument_parastu = nullptr;

    const int MaxValue_Onebyte = 254;
    const int MaxValue_Twobyte = 65534;

    //INI文件 组
    const QString Instrument_parameters = "InstrumentParameters";
    const QString Instrument_consumables = "InstrumentConsumables";
    const QString Instrument_coordinates = "InstrumentCoordinates";
    enum ProtocolSubscript{
        parameter_0_IndexByte = 0,
        parameter_1_IndexByte,
        parameter_2_IndexByte,
        parameter_3_IndexByte,
        parameter_4_IndexByte,
        parameter_5_IndexByte,
        parameter_6_IndexByte,
        parameter_7_IndexByte,
        parameter_8_IndexByte,
        parameter_9_IndexByte,
        parameter_10_IndexByte,
        parameter_11_IndexByte,
        parameter_12_IndexByte,
        parameter_13_IndexByte,
        parameter_14_IndexByte
    };
};

#endif // SUOWEIDLL_H
