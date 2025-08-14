#ifndef USBCODEDISPOSE_H
#define USBCODEDISPOSE_H

#include <QByteArray>
#include <QPoint>
#include <QStringList>

#if defined(USBCODEDISPOSE_LIBRARY)
#  define USBCODEDISPOSESHARED_EXPORT Q_DECL_EXPORT
#else
#  define USBCODEDISPOSESHARED_EXPORT Q_DECL_IMPORT
#endif

#define BIG_SAMALL_BENBACK_SPEED    2160 //大小泵复位速度
#define MACHINEBACK_SPEED           720  //开机复位
#define CLEANNIG_BACK_ORIGIN_SPEED  360  //清洗完后的复位速度
#define BIG_BEN_INHALE_ARI          720  //大泵吸空气
#define SMALL_BEN_INHALE_ARI        720  //小蹦吸空气
#define BIG_BEN_RATIO               2.88  //UL转换率
#define SMALL_BEN_RATIO             72


#define GRABCUPWITHGRIPPER         true
#define GRIPPERNORMAL              false


#define BYTE_INDEX_CONFIG_ONCE   0
#define BYTE_INDEX_CONFIG_SECOND 1
#define BYTE_INDEX_CONFIG_THIRD  2

#define CLEANING_DOWNHIGN        45    //原点清洗的时候下针高度
#define ZMOVESPEED              100     //Z探测速度
#define PUTTUBEMOVE             150     //抓手放杯子距离


#define TRAY_TESTCHANNEL        0
#define TRAY_TESTENPTY          1
#define TRAY_BLOOD              2
#define THROWCUP_AREA           3  //丟杯区

#define ADJUSTMODE              9

struct mainControl_FourByte
{
  bool type;    //命令类型
  quint8 cmd_status; //执行状态
  bool Cleanreagent; //清洗液状态
  bool Tray_1_state;   //试管盘1状态
  bool Tray_2_state;   //试管盘2状态
  bool Tray_3_state;   //试管盘3状态
  bool Tray_4_state;
  mainControl_FourByte()
  {
      type = false;
      cmd_status = 0;
      Cleanreagent = false;
      Tray_1_state = false;
      Tray_2_state = false;
      Tray_3_state = false;
      Tray_4_state = false;
  }
};

struct mainControl_FiveByte
{
    bool Waste_state;     //废液状态
    bool TestHeight;      //测高状态
    bool fengming;        //蜂鸣器状态
    bool cool_state;       //制冷
    bool pump_1_state;    //隔膜泵1
    bool pump_2_state;   //隔膜泵2
    bool Threeway_1;    //3通阀 1
    bool Threeway_2;    //3通阀 2
    mainControl_FiveByte() {
         Waste_state = false;
         TestHeight = false;
         fengming = false ;
         cool_state = false;
         pump_1_state = false;
         pump_2_state = false;
         Threeway_1 = false;
         Threeway_2 = false;
    }
};

struct ReportMotor
{
    bool    type;             //命令类型
    quint8  cmd_status;       //命令状态
    quint8  operation_mode;   //运动模式
    quint8  Axisnum;         //需要运动的电机
    quint8  sport_asix;      //相对运动的轴
    quint8  area_num;        //分区
    ReportMotor()
    {
        type = false;
        cmd_status = 0;
        operation_mode = 0;
        Axisnum = 0;
        sport_asix = 0;
        area_num = 0;
    }
};
struct Motor_Control
{
    int SpotrStep;      //微步数X
    int SportStepY;     //微步数Y
    quint8 slave_addr;  //从机地址
    quint8 func_code;   //功能码
    quint8 cmd_num;    //命令编号
    quint8 cmd_size;   //缓存命令容量
    quint8 controldata;
    quint8 controldata_2;
};

struct MainBord_Reagent{
   quint8 reagetn_type; //试剂类型
   quint8 read_reagent_state; //读卡状态
   MainBord_Reagent(){
       reagetn_type = 0;
       read_reagent_state = 0;
    }
};

struct DisposeParameter
{
    quint8 slave_addr;  //从机地址
    quint8 func_code;   //功能码
    quint8 cmd_num;    //命令编号
    quint8 cmd_size;   //缓存命令容量
    uint  Control_Para4;
    uint  Control_Para5;
    quint16 temperature_rep; //温度数据
    uint  Control_Para8;    //试剂信息
    uint  date_Reagent;     //试剂有效时间
    unsigned short  Reagent_remain;   //试剂容量
    DisposeParameter()
    {
        slave_addr = 0;
        func_code = 0;
        cmd_num = 0;
        cmd_size = 0;
        Control_Para4 = 0;
        Control_Para5 = 0;
        temperature_rep = 0;
        Control_Para8 = 0;
        date_Reagent = 0;
        Reagent_remain = 0;
    }
};

//////////测试模组//////////
struct TestModeBityStruct
{
    bool type;
    quint8 cmd_status;
    bool heap_status;
    bool motor1_status;
    bool motor2_status;
    bool motor3_status;
    bool motor4_status;
    bool motor5_status;
    bool motor6_status;
    bool motor7_status;
    bool motor8_status;
    bool motor9_status;
    bool motor10_status;
    bool motor11_status;
    bool motor12_status;

    TestModeBityStruct()
    {
        type = false;
        cmd_status = 0;
        heap_status = true;
        motor1_status = false;
        motor2_status= false;
        motor3_status= false;
        motor4_status= false;
        motor5_status = false;
        motor6_status= false;
        motor7_status= false;
        motor8_status= false;
        motor9_status = false;
        motor10_status= false;
        motor11_status= false;
        motor12_status= false;
    }
};

struct TestModeStruct
{
    int adcvalue_rep ;  //通道数据
    quint8 temperature_rep; //温度

    quint8 slave_addr;
    quint8 func_code;
    quint8 cmd_num;
    quint8 cmd_size;
    uint DataBity;
};






//液面探测用正速度模式

enum SpeedMode{LoactionMode,Correctspeed,Negativespeed,Keepspeed}; //速度模式

enum MoveComparative{BloodNddle,ReagentNeddle,Hand}; //xy运动轴相对于Z

enum MotorZNameDown{ZMotortongs,ZMotorReagent,ZMotorBlood}; //下针的时候用得下标

enum Endian{LittileEndian,BigEndian};

enum ByteSize{SizeofMainBord = 15, SizeofXorYMotorBord = 15,SizeofTestMode = 15}; //数据长度

enum TestModeMotor{TestModeMotorGroup1 ,TestModeMotorGroup2,TestModeMotorGroup3 };

enum IndexModule{Module_one = 1,Module_two,Module_three}; //模组号

enum BitArry{Bit_0 ,Bit_1,Bit_2,Bit_3,Bit_4,Bit_5,Bit_6,Bit_7};


class USBCODEDISPOSESHARED_EXPORT UsbCodeDispose
{

public:
    UsbCodeDispose();
    ~UsbCodeDispose();


public:

    //到空试管 -- 把空试管夹到测试通道
    QByteArrayList QualityControl2Channel(quint8 Indexcode, QPoint TestTubePos, QPoint TestChannelPos, int downHandsmm);

public:

    /** 读取模组信息状态命令
     * @brief _SetTestModeInfo
     * @param indexTestMode  模组号
     * @param temperature    温度
     * @param indexfan       风扇
     * @param open           开光状态
     * @return
     */
    QByteArray _SetTestModeInfo(quint8 indexTestMode, quint8 temperature, quint8 indexfan, bool open);


     /** 主板状态命令 （未用）
     * @brief _ReadorWriteMainBoard
     * @param Write      读和写
     * @param Type_Reagent 试剂号
     * @param Reagent_remain
     * @return
     */
    QByteArray _ReadorWriteMainBoard(bool Write, quint8 Type_Reagent, unsigned short Reagent_remain);

    /** 控制蜂鸣器
     * @brief _buzzerControl
     * @param open
     * @return
     */
    QByteArray _buzzerControl(bool open);

    /** 获取试剂余量(未用)
     * @brief _GetReagentTypecapacity
     * @param type
     * @return
     */
    QByteArray _GetReagentTypecapacity(quint8 type);


    /** 设置到板子上余量(未用)
     * @brief SetConsumableRemaining
     * @param State
     * @param Types
     * @param Caption
     * @return
     */
    QByteArray SetConsumableRemaining(int State, quint8 Types, unsigned short Caption);

    /** 测试模式下XYZ定位
     * @brief XYLocation
     * @param XYPos
     * @param RelativeZ
     * @param Areas
     * @param high
     * @param Codenum
     * @param FaileLinuuehigh
     * @return
     */
    QByteArrayList  XYLocation(QPoint XYPos, uint RelativeZ, uint Areas, int high , quint8 &Codenum, int FaileLinuuehigh);


    /** 调整坐标模式下丢杯
     * @brief AdjustModeThrowTube
     * @param XYPos
     * @param RelativeZ
     * @param Areas
     * @param high
     * @param Codenum
     * @return
     */
    QByteArrayList  AdjustModeThrowTube(QPoint XYPos, uint RelativeZ, uint Areas, int high, quint8 &Codenum);



    /** XY运动到坐标
     * @brief _DLLXYMovePoint_
     * @param XPoint
     * @param YPoint
     * @param indexcode
     * @param offsetZ
     * @param Area
     * @return
     */
    QByteArray _DLLXYMovePoint_(int XPoint,int YPoint,quint8 &indexcode, quint8 offsetZ,quint8 Area);


    /** XY负速度模式复位
     * @brief _DLLXYMoveReposition_
     * @param indexcode
     * @param Speed  复位速度
     * @return
     */
    QByteArray _DLLXYMoveReposition_(quint8 &indexcode,int Speed);



    /**
    * @brief DLL_XYMoveSpecifiedPosition  XY 运动到指定位置
    * @param Position  移动坐标
    * @param offset_Z  相对Z轴
    * @param Area      区域
    * @param indexcode 命令编号
    * @return
    */
    QByteArray  DLL_XYMoveSpecifiedPosition(const QPoint Position , const quint8 offset_Z, const quint8 Area, quint8 &indexcode);

    /**
     * @brief DLL_ZMoveSpecifiedPosition   Z下降到位置
     * @param ZaxisType                    Z轴编号
     * @param DescendingHeight             下降高度
     * @param Zones                        区域
     * @param indexcode                    命令编号
     * @param LinqueTest                   液面探测
     * @param LinqueFailedhigh             探测失败
     * @param bOnceaginScratch             抓取失败重抓
     * @param GripperGrab                  抓手抓杯状态
     * @return
     */
    QByteArray  DLL_ZMoveSpecifiedPosition(const int ZaxisType, const int DescendingHeight,
                                             const quint8 Zones, quint8 &indexcode,
                                             bool LinqueTest, int LinqueFailedhigh,
                                             const bool bOnceaginScratch, const bool GripperGrab);



    //Z轴复位
    QByteArray  DLL_ZAxis_Reset(const quint8 IndexZ, const int FailedHigh, const quint8 ZbackArea, quint8 &indexcode,
                                  const bool bRepeattheGrabCup);

    /**  血样泵吸吐清洗
     * @brief BigBenActive
     * @param suck        吸、吐标志
     * @param milliliter  运动的数值
     * @param CodeNum     命令编号
     * @param cleanuse    是否清洗
     * @param washesTime  清洗时间
     * @return
     */
    QByteArray  BigBenActive(bool suck, int milliliter, quint8 &CodeNum, bool cleanuse, int washesTime);    //大泵

    /**   试剂泵吸吐清洗
     * @brief SmallBenActive
     * @param suck
     * @param milliliter
     * @param clranning
     * @param indexcode
     * @param washesTime
     * @return
     */
    QByteArray  SmallBenActive(bool suck, int milliliter, bool clranning, quint8 &indexcode, int washesTime); //小泵


    QByteArray  BenNegativespeedBackOrigin(int Index, int speed, quint8 &CodeNum, bool cleanuse); //泵速度模式复位
private:

    //修改单个bit 液面探测
    uint       NotifySignalBit(int Index, bool NotifyValue);


    /**
     * @brief _DLLZDownward   Z轴下降指令
     * @param IndexZ          Z轴下标
     * @param IndexArea
     * @param downmm
     * @param indexcode
     * @param LinqueTest
     * @param FailedLinque
     * @param GripperGrab    抓手抓杯
     * @return
     */
    QByteArray _DLLZDownward(int IndexZ, quint8 IndexArea, quint8 downmm,
                             quint8 &indexcode, bool LinqueTest, uint FailedLinque, bool GripperGrab);


    /*Z轴复位 -1 全部复位*/
    QByteArrayList _DLLZBackorigin(int IndexZ, int ZbackArea, quint8 &indexcode);


    QByteArray  Mindfunction(bool clean,bool Learn, bool XYtogether); //电机命令转换函数

    QByteArray  Mindfunction_Axis_Z(int Gripre_gripping);

    void        GroupIndex_4_byte(quint8 Spotr_axis, quint8 Sport_mode,quint8 cmd_state,quint8 cmd_type);

    /**
    * @brief configIndexFiveBytePara  配置泵命令第5字节
    * @param bitindexnotused          未用
    * @param configDiaphragmPump      设置隔膜泵
    * @param onoffvalue               开关阀
    * @param suctionwaits             吸样等待
    * @param TestAreas                区域
    * @param offsetZ                  相对针偏移
    */
    void  configIndexFiveBytePara(const bool bitindexnotused, const bool configDiaphragmPump, const bool onoffvalue,const bool suctionwaits,
                                                   quint8 TestAreas,quint8 offsetZ);


    void  GroupIndex_5_byte_Zaxis(bool Action_state , bool Optocoupler, bool Liquid_level_detection, bool GripperGrab, quint8 Coordinate_partitioning , quint8 Relative_position);

    QByteArray  Group_MainboardOrder(uint Byet_mode4, uint Byet_mode5, uint Byet_mode8, unsigned short Value); //主板编码
private:
    const quint8 Max_Bit = 8;
    const quint8 binary = 2;
    //主控板信息
    struct mainControl_FourByte * StuConfig;
    struct mainControl_FiveByte * StuConfigFive;
    struct MainBord_Reagent * StuReagentInfo;
    struct DisposeParameter *Strut_MainBoard;

    //电机信息
    struct ReportMotor * Struct_bitMotor;

    struct Motor_Control *Struct_Motor;

    //测试模组
    struct TestModeBityStruct *StructTestmodeBit;

    struct TestModeStruct *StructTestmode;


public:
    enum MotorIndex
    {
        XMotor = 0,
        YMotor,
        PlungerBig,
        PlungerSamll,
        XYjoint
    };

};

#endif // USBCODEDISPOSE_H
