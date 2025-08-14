#ifndef CGLOBAL_H
#define CGLOBAL_H
#include <QColor>
#include <QString>
#include <QMetaType>

#define PID_NUM                         22336
#define VID_NUM                         1155

#define HEX_PID                         0x5740
#define HEX_VID                         0x0483
#define DEVICESERIALNUMBER              "6D74156A5689"

#define KS600                           0
#define KS800                           1
#define KS1200                          2

#define KS600STR                        "全自动血小板聚集分析系统KS-600"
#define KS800STR                        "全自动血小板聚集分析系统KS-800"
#define KS1200STR                       "全自动血小板聚集分析系统KS-1200"

#define ENGINEERPASSWORD                "1"
#define WRITE_OPERAT                    true
#define READ_OPERRAT                    false
#define NOTIFY_XPOINT                   true
#define NOTIFY_YPOINT                   false
#define NOESETQUIPMENT                  255   //未配置仪器类型
#define HADESETQUIPMENT                 1     //已配置
#define AXISPOS_WRITE                   0  //坐标写
#define AXISPOS_READ                    1  //坐标读
#define AXISPOS_BUFFER                  2  //缓存
#define DELETE_ALL                      4  //清除仪器参数

#define CHN_DISABLE                 (-1) //通道禁用

#define SQL_SAMPLE_DATA              0 //SQL样本数据点


#define MODULE_FAILED                 3 //模组命令错误从机掉线



//机器参数配置
#define SQL_MACHINE_SETTING       3
//质控数据表
#define SQL_QUALITY_CONTROL       4
//机器任务表留下未做完的
#define SQL_TASKREMAINDER         5
//打印信息报道单
#define SQL_REPORTSHEETREPORT     6

//测试试剂统计
#define STATS                     8


#define TEMP_CONVERSION_RATIO     0.0625  //温度转换率
#define INIT_CLEAN_RATIO          0.6     //灌注血样蹦拉满比例

#define VERSION_RELEASE           "V1.0.0.0"
//机器动作的标识
#define BLOODAREA_ABSORB_BOLLD_MOVE_EMPTYTUBE  0            //从血样区吸血样到空试管加样
#define EMPTYTUBEAREA_MOVE_CHANNEL             1            //从空试管区夹试管到测试通道位置
#define PUTITDOWN_TUBE_IN_CHANNE               2            //把试管放下到测试通道内
#define CLAMPING_TUBE_IN_CHANNEL               3            //把测试通道中的试管夹起来移动到下一通道位置 不下抓手
#define CLAMPING_TUBE_THROW                    4            //夹起试管丢杯
#define CLAMPING_EMPTYTUBE_IN_CHANNEL_PUTDOWN  5            //把试管区的管子夹到测试通道放下
#define TRROW_POORBLOOD                        2            //丟贫血
#define MACHINE_BACKORIGIN                     10            //机器回原点
#define EMPTYAREA_SCRATCHPOORBLOOD             11           //在试管区抓贫血试管
#define SPITREAGENT_CLEANNINGBLOOD             12           //吐试剂&&清洗血样针
#define EMPTYAREA_SCPATCHTUBE                  13           //在试管区夹试管
#define PUTDOWNTUBE_INCHANNEL                  14           //在测试通道放下试管
#define TESTCHANNEL_THROWTUBE                  15           //丟测试通道中的贫血
#define SPIT_REAGENT_TO_CHANNEL                16           //吐试剂到测试杯中
#define SUCK_REAGENT_TO_CHANNEAL               17           //吸试剂
#define ACTION_CLEAN_DOUBLENEDDLE              18           //清洗双针
#define ACTION_CLEAN_REAGENTNEDDLE             19           //清洗试剂针


#define EQUIPMENT_FREETIME                     0           //仪器空闲
#define EQUIPMENT_BOOT_CLEAN                   100         //仪器开机清洗
#define EQUIPMENT_BTN_REAET                    101         //按钮复位
#define EQUIPMENT_BTN_CLEAN                    102         //按钮清洗
#define COORDINATE_FINE_TUNING_TEST            103         //坐标微调测试动作
//点击QLable
#define BLOODPINDOWNHEIGH                     30            //血样针下降高度
#define BLOODPINDOWNHEIGH_CLEANLINQUEFAILED   31            //血样针液面探测失败下降高度(清洗液)
#define BLOODPINDOWNHEIGH_SERUMMODEL          32            //血样针血浆模式下针高度
#define BLOODPINDOWNHEIGH_ANEMIALINQUEFAILED  33            //血样针液面探测失败下降高度(贫血)

#define REAGPIN_CLEANLINQUE_DOWN               40            //血样针在清洗液探测失败高度
#define REAGPIN_REAGLINQUE_FAILEDDOWN          41            //试剂针试剂位液面探测失败高度
#define REAGPIN_DOWNHEIGH_IN_AA                42            //试剂针在测试AA时下针高度
#define REAGPIN_DOWNHEIGH_IN_ADP               43            //试剂针在测试ADP时下针高度
#define REAGPIN_DOWNHEIGH_IN_EPI               44            //试剂针在测试EPI时下针高度
#define REAGPIN_DOWNHEIGH_IN_COL               45            //试剂针在测试COL时下针高度
#define REAGPIN_DOWNHEIGH_IN_RIS               46            //试剂针在测试RIS时下针高度

#define HANDSDOWN_THROWCUP                     50            //抓手到弃杯孔
#define HANDSDOWN_TRAYTUBE_1                   51            //抓手到试管盘
#define HANDSDOWN_TRAYTUBE_2                   52
#define HANDSDOWN_TRAYTUBE_3                   53
#define HANDSDOWN_TRAYTUBE_4                   54

#define HANDSDOWN_CHN_1                        55            //抓手到通道下降高度
#define HANDSDOWN_CHN_2                        56
#define HANDSDOWN_CHN_3                        57
#define HANDSDOWN_CHN_4                        58
#define HANDSDOWN_CHN_5                        59
#define HANDSDOWN_CHN_6                        60
#define HANDSDOWN_CHN_7                        61
#define HANDSDOWN_CHN_8                        62
#define HANDSDOWN_CHN_9                        63
#define HANDSDOWN_CHN_10                       64
#define HANDSDOWN_CHN_11                       65
#define HANDSDOWN_CHN_12                       66


#define WRITE_REAGENTLIMITS                    0x02     //试剂限位


#define  TEST_THE_TUBEREEL_1                   1  //测试管盘 1
#define  TEST_THE_TUBEREEL_2                   2  //测试管盘 2
#define  TEST_THE_TUBEREEL_3                   3  //测试管盘 3
#define  TEST_THE_TUBEREEL_4                   4  //测试管盘 4
#define  ONETRAY_TOTALTUBE                     60 //一个试管盘 60个试管

//动作做质控
#define SUCK_WATER    1  //吸的是水
#define SUCK_PRP      2
#define SUCK_PPP      3

#define WATER_PUT_HOLE   0 //水放置的位置
#define PPP_PUT_HOLE     2
#define PRP_PUT_HOLE     3



#define BINARY_SWITCH            2          //2进制
#define HEX_SWITCH               16         //16进制转换
#define NUMBEROFTESTDATA         300        //数据测试个数
#define PROTOCOL_LENGTH          15         //协议长度
#define MACHINE_SETTING_CHANNEL  12         //机器配置的通道数
#define REAGENT_TOTAL             5         //试剂种类


#define FOUR_CHANNELS_PERMODULE   4       //每个模组4通道

#define MODULE_1                 1          //模组 1
#define MODULE_2                 2
#define MODULE_3                 3
#define Z_AXIS                   4          //Z轴电机
#define MAIN_CONTROL			 5          //主板控制
#define MAINBOARD_PARA_FUNCT     21         //主板保存参数功能码
#define MAINBOARD_FUNCTIONCODE   22         //主板功能码0x16
#define MOTOR_FUNCTIONCODE       23         //电机功能码


#define SAVE_MODUL_NOTIFY           3  //保存模组调整
#define READ_MODUL_NOTIFY           0  //读取模组数据
#define NOTIFY_MODUL_LED_VALUE      1  //调整模组LED值
#define NOTIFY_MODUL_SPEED_VALUE    2  //调整速度


#define W_MODULE_SPEED             105 //写模组速度 命令编号
#define R_MODULE_SPEED             106 //读模组速度
#define W_MODULE_LED               107  //写模组LED 设置界面
#define R_MODULE_LED               108  //读模组LED
#define W_SAVEMODULESETTING        109  //保存模组调整 设置界面

#define W_MODULE_LED_DIMMING       110    //写模组LED 调光
#define W_SAVEMODULESETTING_DIMMING  111  //保存模组调整 调光



#define WR_MODULE_DIMMING          101 //读写模组电流
#define WR_MODULE_TEMP_DATA        200 //读写模组数据温度





#define	 REPLACEMENT_AA_I       "AA(1)"
#define	 REPLACEMENT_AA_II	    "AA(2)"
#define	 REPLACEMENT_ADP_I		"ADP(1)"
#define	 REPLACEMENT_ADP_II		"ADP(2)"
#define	 REPLACEMENT_ADR_I		"ADR(1)"
#define	 REPLACEMENT_ADR_II		"ADR(2)"
#define	 REPLACEMENT_COL_I		"COL(1)"
#define	 REPLACEMENT_COL_II		"COL(2)"
#define	 REPLACEMENT_RIS_I		"RIS(1)"
#define	 REPLACEMENT_RIS_II     "RIS(2)"
#define  CLEANLINQUE_SI         "Cleanfluid_S1"
#define  CLEANLINQUE_SII        "Cleanfluid_S2"




#define  FIND_CHANNEL_EMPTY      0 //无空闲的测试通道
#define  TEST_CHANNEL_1          1
#define  TEST_CHANNEL_2          2
#define  TEST_CHANNEL_3          3
#define  TEST_CHANNEL_4          4
#define  TEST_CHANNEL_5          5
#define  TEST_CHANNEL_6          6
#define  TEST_CHANNEL_7          7
#define  TEST_CHANNEL_8          8
#define  TEST_CHANNEL_9          9
#define  TEST_CHANNEL_10        10
#define  TEST_CHANNEL_11        11
#define  TEST_CHANNEL_12        12

//滤波模式
#define  FILTER_NO              0
#define  FILTER_AVERAGE_VALUE   1   //中位均值
#define  FILTER_MIDVALUE        2  //中值


//测高模块的参照物高度 mm
#define  REFERENCE_HEIGHT		 20 
#define  REFERENCE_TO_BOTTOM     26.8   //51.8    //26.8

//读取耗材卡状态
#define CONSUMABLES_READ_NORMAL       0x00  //正常(读取)
#define CONSUMABLES_REDUCE            0x01  //正常损耗(缓存)
#define CONSUMABLES_READ_SUCESSFULLY  0x02  //刷卡成功
#define CONSUMABLES_READ_FAIL         0x03  //刷卡失败
#define CONSUMABLES_READ_ABNORMAL     0x04  //刷卡异常
#define RECHARGE_SUCESSFULLY          0x05  //充值成功(充值写入)
#define RECHARGE_FAIL				  0x06  //充值失败(刷卡写入失败)
#define BUFFER_WRITE                  0x07  //缓存区写入







#define CHANNEL_TESTING               true   //通道测试中
#define CHANNEL_IDLE                  false  //通道空闲

#define WASHES_PUMPS                  true  //清洗泵
#define DIS_WASHES_PUMPS              false //不清洗

#define ANEMIA                        0  //贫血
#define AA_REAGENT                    1
#define ADP_REAGENT                   2
#define EPI_REAGENT                   3
#define COL_REAGENT                   4
#define RIS_REAGENT                   5


#define REAGENTS_TOTAL             1000 //试剂总量 1000ul
#define CLEANREAGENT_TOTAL         60000 //内部清洗剂量 60000ul


//测试结果单个值
#define RESULT_SIXTYSEC               0  //  60S的值
#define RESULT_ONEHUNDEREIGHTYSEC     1  // 180S值
#define RESULT_THREEHUNDERSEC         2
#define RESULT_MAXSEC                 3

//完成状态
#define MOTOR_WAITING                 0
#define MOTOR_RUNNING                 1
#define MOTOR_FINISH                  2
#define MOTOR_ERROR                   3

//Z轴(运动)电机下标
#define MOTOR_HANDS_INDEX             0
#define MOTOR_REAGNET_INDEX           1
#define MOTOR_BLOOD_INDEX             2

#define HANDS_SUCK_NUM                200
#define HANDS_SPLIT_NUM               201

//XY运动(相对)Z轴
#define OFFSET_ZBLOOD                 0
#define OFFSET_ZREAGNET               1
#define OFFSET_ZHANDS                 2

//机器分的区域
#define OFFSET_BLOODAREA              2
#define OFFSET_THROWTUBEAREA          3
#define OFFSET_EMPTYTUBEAREA          1
#define OFFSET_TESTINGAREA            0

//液面探测状态
#define LIAUID_LEVEL_DETECTION_STATUS   -1

//抓手抓试管状态
#define MOTOR_XY                         4



//空试管得使用状态
#define TESTTUBES_FREETIME             0   //测试管为空
#define TESTTUBES_MARK                 1   //测试管被标记
#define TESTTUBES_SAMPLED              2   //测试管已吸样
#define TESTTUBES_CLIPPEDAWAY          3   //测试管被夹走

//用户权限管理
#define NORMAL_USERS                   0 //普通用户
#define INTENDANT_CONTROL_VIP          1 //管理者用户



#define WASH_DOUBLE_NEDDLES           0  //清洗双针
#define WASH_BLLODSAMPLE_NEDDLES      1  //清洗血样针
#define WASH_REAGENT_NEDDLES          2  //试剂针
#define THROW_CUPS_END                3 //丟杯后


#define THESTATEOFMOTOR              0 //电机的状态
#define MOTOR_SPORTMODE              1 //电机运动模式
#define RUNNING_MOTOR                2 //运动的电机

#define SLAVE_ADDRESS                0 //从机地址
#define FUNCTIONALMA                 1 //功能玛
#define COMMANDNUMBER                2 //命令编号

//线程池下标
#define   ALTIMETRY_MODULE          4 //测高模块


//打开摄像头位置
#define   OPEN_DEFAULT              0 //仪器检测到连接只有一个摄像头为打开默认
#define   OPEN_EXTERNAL             1 //仪器有外界摄像头打开外部摄像头

//日志提示等级
#define   NORMALLOG             0  //正常
#define   PROMPTLOG             1  //提示
#define   ERRORLOG              2  //错误
#define   USERACTIONS           3  //用户操作
#define   MOTHERBOARD_ERR       4 //清洗液S2 废液提示 主板信号异常
#define   TESTINGLOG            5 //测试过程中的提示

#define   ALL_SAMPLE_TESTED     0 //所有样本测试完成

/////////////////////主板耗材命令编号//////////////////////////////
#define EVERYTIME_READ_MAINSTATE   254   //定时读取主板状态命令编号

#define REPLACE_WHOLE_BOTTLE_AA    100
#define REPLACE_WHOLE_BOTTLE_AA_II  101

#define REPLACE_WHOLE_BOTTLE_ADP    102
#define REPLACE_WHOLE_BOTTLE_ADP_II  103

#define REPLACE_WHOLE_BOTTLE_EPI    104
#define REPLACE_WHOLE_BOTTLE_EPI_II  105

#define REPLACE_WHOLE_BOTTLE_COL    106
#define REPLACE_WHOLE_BOTTLE_COL_II  107

#define REPLACE_WHOLE_BOTTLE_RIS    108
#define REPLACE_WHOLE_BOTTLE_RIS_II  109

#define REPLACE_WHOLE_PLATE_TUBE_I      110
#define REPLACE_WHOLE_PLATE_TUBE_II     111
#define REPLACE_WHOLE_PLATE_TUBE_III    112
#define REPLACE_WHOLE_PLATE_TUBE_IV     113
#define REPLACE_WHOLE_BOTTLE_CLEANLINQUE  114


#define REDUCE_AA                       200
#define REDUCE_ADP                      201
#define REDUCE_EPI                      202
#define REDUCE_COL                      203
#define REDUCE_RIS                      204

#define REDUCE_AA_II                    205
#define REDUCE_ADP_II                   206
#define REDUCE_EPI_II                   207
#define REDUCE_COL_II                   208
#define REDUCE_RIS_II                         209

#define REDUCE_CLEANLINQUE                    210

#define TIME_SEND_MODULECODER                 30
#define TIME_SEND_MODULECODER_FAILED          1000



enum ReadCapacity
{
    Consumables_AA = 0,
    Consumables_ADP,
    Consumables_ADR,
    Consumables_COL,
    Consumables_RIS,
    Consumables_X1,
    Consumables_X2,
    Consumables_TestCupTray
};

//仪器的运动状态
enum  Instrumentmovement
{
    NotActiveRunning = 0,
    Suck_Anemia = 1,
    Spitting_Anemia = 2,
    Suck_RichBlood = 3,
    SPitting_RichBlood = 4,
    Grabthe_Anemiatube = 5,
    Tube_Putinthetestposition = 6,
    Throwtesttube_ofthetestchannel = 7,
    AspirationReagent_PickUpBloodCup = 8,
    SpittheReagentintoTestChannel = 9,
    WashthedoubleNeedles = 10,
    CleaningtheReagentNeedle,
    WashthebloodsampleNeedle,
    InitMachineCleanActive,
    btndoingActive
};

//仪器提示类型
enum equipmentTipInfo
{
    LinqueCleanShortage = 0,
    TestTubeTrayI,
    TestTubeTrayII,
    TestTubeTrayIII,
    TestTubeTrayIIII,
    LinqueScrapFull,
    HighDetectionFailed,
    BeepClose,
    Cooler,
    DiaphragmPumpI,
    DiaphragmPumpII,
    ThreeWayValueI,
    ThreeWayValueII
};


typedef struct PrintPatientInfo
{
    QString hospital_;
    QString sampleNumber;       //样本号
    QString barCode;            //条码
    QString patientName;
    QString patientSex;
    QString patientAge;
    QString patientDepartment;      //部门
    QString Hospitalizationnumber; //住院号
    QString patientWard;            //病区
    QString patientBedNumber;       //床号
    QString patientClinical;        //临床诊断
    QString Referredtodoctor;       //送检医生
    QString Testdoctor;             //检验医生
    QString doctorRemark;           //备注
    QString Auditthedoctor;         //审核医生
}PrintPatientInfo;


class cglobal
{
public:
    cglobal();
    ~cglobal();
public:



        static  bool gserialConnecStatus;   //串口连接状态
        static QString gserialPortName;     //连接串口号
        static QString g_UserName_str;

        static bool g_QualityControlhighValue; //质控高低值
        static bool g_quality_start;
        static bool g_StartTesting;             //开始测试
        static bool g_PauseTesting;             //暂停


        static QColor g_LineColor;              //线的颜色

        static QColor g_SamllBgmColor;          //内圆背景色 --初始状态
		static QColor g_PoorBloodColor;         //贫血颜色
		static QColor g_RichBloodColor;        //富血颜色
		static QColor g_OutResult;
        static QColor g_TextColorBlack;        //文字颜色(外部颜色字体)
        static QColor g_TextColorRoorblood;    //贫血文字颜色
        static QColor g_TextColorRichblood;    //富血文字颜色
        static QColor g_TextColorWhite;

		static QColor g_InnerRing_Checked;		//血样区 内圆环选中状态颜色
		static QColor g_InnerRing_absorbed;     //血样区 内圆血样被吸走颜色

        static QColor g_OuterRingColor;         //外圆环颜色
        static QColor g_ReminderBloodHoleColor;  //选用血样孔外环提示颜色

        static QColor g_SelectColor;            //选中试管颜色

        static QColor g_MoveedColor;            //移动试管颜色

        static quint16 g_cmd_num;               //命令编号

        static bool g_controldimmingfinished;    //调光完成标志

        static bool g_moduleIdisconnected;   //模组I - III 掉线状态
        static bool g_moduleIIdisconnected;
        static bool g_moduleIIIdisconnected;



};

#endif // CGLOBAL_H
