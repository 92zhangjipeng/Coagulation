#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <QString>
#include <QByteArray>
#include <QComboBox>
#include <QToolButton>
#include <QPushbutton>
#include <QCheckbox>
#include <QDoubleSpinbox>
#include <QTableWidget>
#include <QLabel>

//吸杯负压最小值
#define  SUCTIONCUPNEGATIVEVALUEMIN  2300     //2300
#define  SUCTIONCUPNEGATIVEVALUEMAX  2400     //2450

#define  THROWCUPNEGATIVEVALUE       2400  //丢杯负压值 2700


class GlobalData
{
public:
    GlobalData();

public:



    /** 映射试剂名
     * @brief mapReagentNames
     * @param KindConsumables
     * @return
     */
    static QString mapReagentNames(const quint8 KindConsumables);
    static QString mapIndexReagentnames(quint8 index);
    static quint8 indexReagentMapReagnetNmaes(const QString testReagName);
    static void   sycnTestingReagnetNum(QList<quint8> &testIndex, QString reagentName);


    static bool commandStatus(const int &run_status,QString &outtext);

    //输出耗材容量键
    static QString sycnBottleCapacitykey(quint8 index_reag);

    //输出耗材限位比
    static QString sycnBottleLimitskey(quint8 index_reag);


    static void splitDouble(double num, int &integer, double &fraction);


    /** 缓存区耗材写入到仪器
     * @brief writeBuffer2Equipment
     * @return
     */
    static QByteArray writeBuffer2Equipment();

    //写入试剂限位瓶数限位
    static QByteArray writeBottleLimit2Equipment();



    /**
    * @brief CreadFolder 创建文件夹
    * @return
    */
    static  bool CreadFolder(const QString Foldername);


    /**
     * @brief CreatFloadfile 在文件夹下创建文件
     * @param foldername
     * @param filename
     */
    static  void CreatFloadfileAppend(const QString &foldername, const QString &filename, const QString &data);


    /**  从协议内截取对应字节
     * @brief BytesistakenOutoftheprotocol
     * @return
     */
    static quint8 BytesistakenOutoftheprotocol(const quint8 seq, const QByteArray outcode);


    /** 解析返回抓手吸试杯的空气值
     * @brief backHandsSuckAirValue
     * @param recvdata
     * @return
     */
    static quint32 backHandsSuckAirValue(const QStringList recvdata);


    /** 解析是不是抓手吸
     * @brief analyzeWhetherGripperSuction
     * @param data 发送的命令
     * @return
     */
    static QString analyzeWhetherGripperSuction(const QStringList byteArray, bool &isSuction, quint32 &airvalue);

    /** 抓手吸杯失败重试 在通道处抓杯
     * @brief Grabthehandtotryagain
     * @param num    编号
     * @param times  重试次数
     * @return
     */
    static QByteArray Grabthehandtotryagain(const quint8 num,const int times,quint8 catchChannel);

    /** 丢被失败重试  在弃杯孔处丢杯
     * @brief ThrowCupstotryagain
     * @param num
     * @param times
     * @return
     */
    static QByteArray ThrowCupstotryagain(const quint8 num, const int times);

    /** 在试管盘处吸杯子
     * @brief GrabtheCupAttheTestTubeTray
     * @param num
     * @return
     */
    static QByteArray GrabtheCupAttheTestTubeTray(const quint8 num, const int times, quint8 testTubeHole);

    /** 在测试通道 放杯子
     * @brief putoutCupAtTestChannel
     * @param num
     * @return
     */
    static QByteArray putoutCupAtTestChannel(const quint8 num, const int times, quint8 indexChn);

    /**  抓手吸杯状态
     * @brief HandsSuckFinish
     * @param airvale
     * @return
     */
    static bool isHandsSuckFinish(quint32 airvale);

    /** 抓手吐杯状态
     * @brief HandsThrowCupFinish
     * @param airvale
     * @return
     */
    static bool isHandsThrowCupFinish(quint32 airvale);


    /** 解析收到的从机地址
     * @brief analyzeSlave_addr
     * @return
     */
    static quint8 analyzeSlave_addr(const QStringList recvData, bool outlogs);

    /**  命令编号
     * @brief analyzeCommandIndex
     * @param recvData
     * @return
     */
    static quint8 analyzeCommandIndex(const QStringList recvData);


    /** 解析命令第4个byte
     * @brief Parse4thBytecommandstatus
     * @param recvData
     * @param commandMode
     * @param sport_status
     * @param speedMode
     * @param which_motor
     */
    static void Parse4thBytecommandstatus(const QStringList recvData, int &commandMode,int &sport_status
                                             ,int &speedMode ,int &which_motor );





    /**   下抓手抓杯
     * @brief GrabTheCupUnderTheGripper
     * @param downHeight
     * @param indexNum
     * @return
     */
    static QByteArray intToQByteArray(int input,bool isEndianTyped);
    static QByteArray GrabTheCupUnderTheGripper(double downHeight, int &indexNum, bool backOriginLoc, bool bGrab);

    /**
     * @brief ConfigEachChannelSpeed  设置模组通道转速
     * @param indexModule             模组号
     * @param indexChn                通道
     * @param setspeed                速度
     * @return
     */
    static QByteArray ConfigEachChannelSpeed(quint8 indexModule, quint8 indexChn, quint16 setspeed);

    /**
     * @brief ReadEachChannelSpeed  读取模组转速
     * @param indexModule
     * @return
     */
    static QByteArray ReadEachChannelSpeed(quint8 indexModule);

    /**
     * @brief WriteModuleLedData  写LED
     * @param indexModule
     * @param Ledval
     * @return
     */
    static QByteArray WriteModuleLedData(quint8 indexModule, quint8 FcunNum, quint16 Ledval);

    /**
     * @brief ReadModuleLedData 读LED
     * @param indexModule
     * @return
     */
    static QByteArray ReadModuleLedData(quint8 indexModule);

    /**
     * @brief SaveConfigModuleData 保存模组调整
     * @param indexModule
     * @return
     */
    static QByteArray SaveConfigModuleData(quint8 indexModule, quint8 fucnNum);


    /**
     * @brief ObatinCreatSampleTime 获取样本创建时间
     * @return
     */
    static QString ObatinCreatSampleTime();

    /**   分离样本号
     * @brief apartSampleId
     * @param sampleId
     * @param date
     * @param id
     */
    static void apartSampleId(const QString sampleId, QString &date, int &id);

    /**  组合样本号
     * @brief groupDateAndID
     * @param date
     * @param idnum
     * @return
     */
    static  QString groupDateAndID(const QString date, const int idnum);


    static  void vectorconversionString(QVector<double> curvepoint,QString &curvepos);


    /**
     * @brief QCommboxSheet 样式
     * @return
     */
    static void QCommboxSheet(QComboBox *pcommbox);

    static void QLableSheet(QLabel *plable);

    static void QLineEditSheet(QLineEdit *PLineEdit);

    static void QCheckboxSheet(QCheckBox *pbox,const QString name);

    static void QCheckboxSheetChnstate(QCheckBox *pbox,const QString name);

    static void mainseledview(QToolButton *ptoolbtn,bool checked);

    static QColor customCurveColor(quint8 indexReag); //自定义曲线的颜色

    static void  QTableWidgetinitSheet(QTableWidget *CustomTableWidget,
                                       const QString TableName,
                                       const QStringList itemHeand,
                                       const QStringList itemRows,
                                       bool binsertRows);



    // 移动平均滤波器（处理边界）
    static QVector<double> smoothData(const QVector<double>& input, int windowSize);


    /** 输出吸试剂索引
      * @brief outputtheAbsorbentIndex
      * @param focuinActive  吸试剂动作索引
      * @param index_reagent 输出试剂编号
      * @param consumable_index 输出耗材消耗的索引
      */
     static void outputtheAbsorbentIndex(const quint8 focuinActive,
                                         quint8 &index_reagent,
                                         quint8 &consumable_index);

     /** 左边的耗材是否足够
      * @brief determineReagentontheleftIsSufficient
      * @param consumableIndex
      * @return
      */
     static bool determineReagentontheleftIsSufficient(quint8 consumableIndex, quint8 reagentIndex);

     /**  左边试剂不足 更换吸试剂位置
      * @brief replactSuckReagentLoc
      * @param num
      * @param indexReagnet
      * @return
      */
     static QByteArray replactSuckReagentLoc(quint8 num, quint8 indexReagnet);



     /** 仪器异常XY 先复位
      * @brief errMachineBackOrigin
      * @return
      */
     static QByteArrayList errMachineBackOrigin(const quint8 num, const int indexZAxis);

     /** 输出吸试剂指令
      * @brief outCommandSuckReagent
      * @param indexReag
      * @return
      */
     static QByteArrayList outCommandSuckReagent(const quint8 indexReag);


     static QByteArrayList outSuckPRPtestReagentCup(int testChannel,
													 quint8 suckHole,
													 QPoint &PRPHoleAxis);

     //让xyz复位
     static QByteArrayList letallAxisBackOrigin(QList<quint8> &backnumList);

};

#endif // GLOBALDATA_H
