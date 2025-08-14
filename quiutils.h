#ifndef QUIUTILS_H
#define QUIUTILS_H

#pragma execution_character_set("utf-8")

#include <QObject>
#include <QApplication>
#include <QToolButton>
#include "cglobal.h"
#include "testing.h"
#include "suoweiserialport.h"
#include "monitor_traytest.h"
#include <algorithm>//调用sort()必须要使用这个头文件
#include <QLabel>
#include <QComboBox>


namespace QUIUtils
{
    QString bitArray2String(QBitArray array);



    /** 截取小数点后N位的方法（不进行四舍五入）
     * @brief QUIUtils::getNonRoundingNumber
     * @param number
     * @param digit
     * @return
     */
    double getNonRoundingNumber(double number, int digit);

    /**
    * @brief CalculateTheConsumptionOfCleaningFluid 计算消耗清洗液
    * @param _usedcleanlinque  消耗的量mm
    * @param LastRatio   输出剩余比 显示
    * @param updataRatio 更新比例 max512
    */
    void CalculateTheConsumptionOfCleaningFluid(quint16 _usedcleanlinque, double &LastRatio, quint16 &updataRatio);


    /**
     * @brief CalculateTheConsumptionOfReagent
     * @param indexReag         消耗试剂种类
     * @param _usedmum          使用量
     * @param LastRatio
     * @param updataRatio
     */
    void CalculateTheConsumptionOfReagent(quint8 reagentIndex,
                                          quint8 usedVolume,
                                          double &remainingRatio,
                                          quint16 &updatedRatio);

    /**
     * @brief _ObtainSuppileDataInIFile 初始化获取耗材配置文件数据到内存
     * @param mpathfile
     * @param index_reag
     * @param BottleNum
     * @param RatioReminder
     * @param BottleLimit
     * @param BottleCapacity
     */
    void _ObtainSuppileDataInIFile(QString mpathfile,
		quint8 index_reag, 
		quint8 &BottleNum, 
		quint16& RatioReminder, 
		quint8 &BottleLimit,
		quint16 &BottleCapacity);

    /**
     * @brief _sycnBottleRatioPara 设置耗材瓶内剩余比例
     * @param index_reag
     * @param RatioReminder
     */
    void sycnBottleRatioPara(quint8 index_reag, quint16 RatioReminder);


    /**
     * @brief _sycnBottleWholeNum 耗材瓶数
     * @param index_reag
     * @param BottleNum
     */
    void sycnBottleWholeNum(quint8 index_reag, quint8 BottleNum);

    /**
     * @brief _sycnBottleLimit 耗材限位
     * @param mpathfile
     * @param index_reag
     * @param BottleLimit
     */
    void _sycnBottleLimit(QString mpathfile, quint8 index_reag, quint8 BottleLimit);

    /**
     * @brief _sycnBottleCapacity 容量
     * @param mpathfile
     * @param index_reag
     * @param BottleCapacity
     */
    void _sycnBottleCapacity(QString mpathfile, quint8 index_reag, quint16 BottleCapacity);


    /**  设置按钮鼠标样式
     * @brief ConfigMousingPicture
     * @param pToolBtn
     */
    void ConfigMousingPicture(QToolButton* pToolBtn);
    void ConfigMousingPicture(QWidget* pWidget);

    /**
    * @brief deskWidth 获取桌面宽度
    * @return
    */
    int deskWidth();

    /**
    * @brief deskHeight 获取桌面高度
    * @return
    */
    int deskHeight();

    /**
    * @brief isEnglish 判断字符串是否为纯字母
    * @param str
    * @return
    */
    bool isEnglish(QString str);

    /**
    * @brief isNumber 判断字符串是否为纯数字
    * @param str
    * @return
    */
    bool isNumber(QString str);

    /**
    * @brief removeDir 删除目录及目录下文件
    * @param dirName
    * @return
    */
    bool removeDir(const QString &dirName);

    /**
    * @brief removeFile 删除文件
    * @param dirName
    * @return
    */
    bool removeFile(const QString &dirName);

    /**
    * @brief msleep 休眠msec毫秒
    * @param sec
    */
    void msleep(const int msec);

    /** 删除指定时间日志
    * @brief deleteLocalLog
    */
    void deleteLocalLog();

    /**
    * @brief 获取当前时间
    */
	QString getTodatTime();

    /**
    * @brief getappload 软件路径
    * @return
    */
    QString getappload(QString fileflodr);

    /**
    * @brief isDirExist 判断文件夹是否存在
    * @param fullPath
    * @return
    */
    bool isDirExist(const QString &fullPath);

    /**
    * @brief delay_ms 休眠Time秒
    * @param sec
    */
    void delay_ms(int Time, bool IsThread);

    /**
    * @brief hexToQString
    * @param pHex
    * @param len
    * @return
    */
    QString hexToQString(unsigned char *pHex, int len);

    /**
    * @brief QByteArrayToQStringList
    * @param pHex
    * @param len
    * @return
    */
    QStringList QByteArrayToQStringList(QByteArray bamessage );

    /**
    * @brief label显示图标和文字
    * @param pHex
    * @param len
    * @return
    */
    void QLabeldisplayIcon(QLabel* plabTip, 
							QString pathIcon, 
							QString SetTextTip, 
							quint8 setTextColor);

    /**
    * QStringList 转 QByteArray
    */
    QByteArray StringListConversionByteArray (QStringList data);

    /**
    * 解析运动到的位置
    */
    int ParseSportLocation(const QStringList recv_data ,const int Index);

    /**
    * 解析到电机运动状态
    */
    int Recve_Motor_motion_state(const QStringList recv_data , const int Index);

    /**
    * @brief _backmotorexecutionstatus  返回电机的执行状态
    * @param hex_data   协议第4个字节 16进制数
    * @param hex_data   Z /XY
    * @return
    */
    int _backmotorexecutionstatus(int hex_data, 
									int index_addr, 
									int errorbit, 
									QString &_printLog);

    /**
    * 获得XY运动的坐标
    */
    QPoint coordinatePositionXYMotion(const QStringList recv_data);

    /*
    * 解析消息
    */
    int ObtainCommandNumber(const QStringList Recv_data,const quint8 Index);

    /**
    * @brief equipmentdoAction
    * @param indexAct  动作编号
    * @return  解释动作
    */
    QString equipmentdoAction(const quint8 indexAct);

    /**
    * @brief outputstheZ_axis_status
    * @param data_
    * @return
    */
    QString outputstheZ_axis_status(const QStringList data_);

    /**
    * 在字符串中移除字符 %
    */
    void   parseDataratio(QString &str);

    /*聚集率参考值表头*/
    void   itemReferenceTestValue(QStringList &keyListItem);

    /*翻译电机*/
    QString ReminderMotor(const int Addrs,const int Motors);

    /*
    *  电机异常映射
    */
    QString MotorStateMapStr(const quint8 IndexStatus, const int ErrorEnum);

    /*
    * 找字符串内数字
    */
    int  StringFindintnum(QString Str);

    //提示命令内存满
    void reminderfullcoder(quint8 indexaddr_, QString& fullCode_);



    /**
    * @brief index_reagent_mapping_reagentName
    * @param outText   输出信息 + 试剂
    * @param indexReag 试剂编号映射试剂名
    * @return
    */
    QString index_reagent_mapping_reagentName(QString outText, const quint8 indexReag);




    /**
    * @brief IndexPorgectMapString样本下标映射样本数据库键
    * @param IndexReagents
    * @param Result
    * @return
    */
    QString IndexPorgectMapString(quint8 IndexReagents ,bool Result);

    /* 输出获取测试结果*/
    QString OutPrintTestedResult(QVector<double> TestingDataList);


    /*读取仪器坐标&&仪器类型*/
    QByteArray ReadcoordinateArry(quint8 cmd_num);

    //uint转QbyteArray isEndianTyped: true 大端 false 小端写数据
	QByteArray qint16ToQByteArray(quint16 input, bool isEndianTyped);

    //配置仪器类型&& 写入到仪器
    void  WriteEquipmentType(quint8 equipmentKind, quint8 index_num, QByteArray& configorder);

    /**
     * @brief bufferCodeAxis 获取命令
     * @param indexNum       命令模块
     * @param stateoper      缓存/写 仪器坐标
     * @param confAxispos    坐标容器
     * @return
     */
    QByteArray bufferCodeAxis(quint8 indexNum, 
								int stateoper, 
								QMap<quint8, quint16> confAxispos);

    //试剂限位 0x03
    void _writeReagentLimitOrder(QByteArray& buffer); //写试剂限位命令

    //试剂容量0x05
    void _writeReagentCapacity(QByteArray &buffer);

    //写配置参数
    //抓手和通道基础参数
    void _writeParaNumIOrder(QByteArray &buffer,
							quint16 (&moduleI)[3],
							bool (&ChanState)[12],
							bool scanbar,
							bool rightReagent,
							bool initcatchcups,
							quint16 dimmlED);

    void _writeParaNumIIHandsOrder(QByteArray &buffer,
									quint8 throwdownmm,
									quint8 (&handsinierTray)[4],
									quint8 (&HandsinnerModule)[3],
									bool brebackCatch);

    //血样针参数
    void _writeParaNumBloodOrder(QByteArray &buffer,
                                  quint16 cleanLinqueblood,
                                  quint16 suckppporprp,
                                  quint16 suckairs, quint32 cleanTime,
                                  bool iswrite);



    void _writeParaBloodOtherOrder(QByteArray &buffer, quint8 (&bloodpara)[10]);

    //写试剂针参数 0x17-0x18
    void _writeReagPinParaDataOrder(QByteArray &bufferI,QByteArray &bufferII, quint8 (&ReagentSuck)[5], 
									quint8 (&ReagentDown)[5], quint8 (&otherdata)[8]);

    /**  写入板子参数 0x19
     * @brief WriteBottleLimitArry
     * @param ParaData
     * @return
     */
    QByteArray WriteBottleLimitArry(QMap<int,quint8> ParaData, const bool Writedirectly);

    //0x1a写入主板
    QByteArray writeGripperParaDataArry(const quint16 lessMax,
                                          const quint16 bigthanMin,
                                          const quint16 suckTime,const bool Writedirectly);



    //group buffer 坐标
    void  get6_7NumcodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer,
                             quint8 bufferState_y);

    void  get89NumberCodeArry(QByteArray &x_Buffer,quint8 bufferState_x,QByteArray &y_Buffer,
                               quint8 bufferState_y);

    void  get0a0bNumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer,
								quint8 bufferState_y);

    void  get0c0dNumberCodeArry(QByteArray &x_Buffer,quint8 bufferState_x,QByteArray &y_Buffer,
								quint8 bufferState_y);

    void  get0e0fNumberCodeArry(QByteArray &x_Buffer,quint8 bufferState_x,QByteArray &y_Buffer,
								quint8 bufferState_y);

    void  get1011NumberCodeArry(QByteArray &x_Buffer,quint8 bufferState_x, QByteArray &y_Buffer,
								quint8 bufferState_y);

    void  get1213NumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer,
								quint8 bufferState_y);

    void  get1415NumberCodeArry(QByteArray &x_Buffer, quint8 bufferState_x, QByteArray &y_Buffer,
								quint8 bufferState_y);

    void  mapEquipmentAxisCommmd(QMap<quint8,QByteArray> &sendArry);

    void  clearequipmentpara(QByteArray &clear_data); //清空仪器内数据


    /**耗材的读操作
     * @brief readSuppliesControlOrder
     * @param outarry
     * @param indexnumcode
     * @param indexReag
     */
    void readSuppliesControlOrder(QByteArray& outarry, quint8 indexnumcode, quint8 indexReag);



    /**  耗材的写入 瓶数和百分比操作
     * @brief writeSuppliesBottleControlOrder
     * @param outarry
     * @param indexnumcode
     * @param indexReag
     * @param configBottle
     * @param bottleRatio  百分比
     * @param isBuffer  缓存、写入
     */
    void writeSuppliesBottleControlOrder(QByteArray& outarry, quint8 indexnumcode, quint8 indexReag,
                                            quint8 configBottle, quint16 bottleRatio, const bool isBuffer);



    /** 确认读卡充值
     * @brief ConfirmReadthecardRecharge
     * @param suppilesKind
     * @param indexnum_
     * @param totalSuppiles
     * @param sendData
     */
    void  ConfirmReadthecardRecharge(quint8 suppilesKind,
                                     quint8 indexnum_,
                                     quint8 totalSuppiles,
                                     QByteArray &sendData);



    /** 遍历主板命令(读)
     * @brief Traverse_the_motherboard
     * @param MotherboardCommd
     */
    void Traverse_the_motherboard(QByteArray &MotherboardCommd);


    /** 遍历读取模组数据
     * @brief Traverse_the_Module
     * @param IndexModul
     * @param moduleCommand
     */
    void Traverse_the_Module(const int IndexModul,QByteArray &moduleCommand);



    //生成试剂区其它坐标 以0号试剂坐标为基准
    void  CreatReagArsOtherAxis(QPoint indexFirstPos,
								QMap<quint8,QPoint> &ReagentZoneOffsetKitsPin);

    void  creatBloodSampleAxis(int equipType, 
								QPoint firstPos, 
								QMap<quint8,QPoint> &bloodSampleZone);

    void  creatTeatTubeAxiis(int indexZ, 
							int equipType,
							QMap<quint8,QPoint> &TsetTubeAxispos, 
							QPoint arrStartpos[],
							int length);


    //试剂针清洗转转换称 微升量
    double reagent_pin_clean_deplete(int turn_);

    //血样针清洗消耗清洗液
    double bloody_pin_clean_deplete(int turn_);



    //测试抓手抓取测试杯区试管准确度
    void test_hands_catchtube_precise(QPoint form_axis, 
									QPoint end_axis, 
									int down_mm, 
									QByteArrayList &directives_list);

    /// \brief 开关机灌注动作
    QByteArrayList perfusionAction(uint &back_leng,
                                    const bool enoughReagent, const int cleantime);

    //Z轴全部复位
    void allZAxisBackOrigin(QByteArrayList &ZbackOrigin,
											quint8 hadsize);

    //仪器复位==先速度模式复位原点==再位置模式到位置原点
    void _equipmentbackoriginloc(QByteArrayList &_backOrigin);

    //灌注后清洗针
    void cleaningDoubleNeedleAction(QByteArrayList &CleanActionarry,int reagent_total);

    void _Locatetheorigin(QByteArrayList &ZbackOrigin, QPoint loc_, int down_);//定位原点下双针

    //清洗血样针
    void CleanBloodPinActionCommd(int HadPorjects, QByteArrayList &CleanActionarry);

    //清洗试剂针
    void CleanReagentsPinActionCommd(QVector<QByteArray> &CleanActionarry);




    /**
    * @brief sampleSuckAnaemia  吸贫血-吐贫血
    * @param indexSample        待测样本号
    * @param suckAnaemia        吸富血命令
    * @param SpitAnaemia        吐富血命令
    * @param AnaemiaHole_num    贫血血样孔号
    * @param AnaemiaAxis        贫血坐标
    * @param AnaemiaoffTubeHole_num 贫血在空测试管孔号
    * @param AnaemiainEmptyTubeAxis 贫血在空测试管坐标
    */
//    void sampleSuckAnaemia(const QString indexSample, QByteArrayList &suckAnaemia, QByteArrayList &SpitAnaemia,
//                             quint8 &AnaemiaHole_num,QPoint &AnaemiaAxis,quint8 &AnaemiaoffTubeHole_num,
//                             QPoint &AnaemiainEmptyTubeAxis);

    /**
    * @brief suckPPPEndSplitPPP     吸贫血-吐贫血 &&吸富血吐富血
    * @param out_directives          输出指令
    * @param suckPPPLoc              吸ppp坐标
    * @param anemia_spit_pos         吐PPP坐标
    */
    int suckPPPEndSplitPPP(QByteArrayList &out_directives,
                            const QPoint suckPPPLoc,
                            const QPoint splitPPPloc);

    int SuckPRPandSpitoutPRP(QByteArrayList &out_directives,
							int Testheigt,
							QPoint suck_bloodypos ,
							QList<QPoint> spit_bloody_axisList);

    /**

    */
    /**
    * @brief sampleSuckBloody  吸富血-吐富血
    * @brief numarry           吸吐贫血命令条数
    * @param indexSample       待测样本号
    * @param suckBloody        吸富血命令
    * @param SpitBloody        吐富血命令
    * @param bloodyHole         吸富血孔号
    * @param bloodyOffBloodHole 吸富血坐标
    * @param bloodyProject      吐富血对应的试剂号
    * @param bloodyinfo         吐富血孔号对应坐标
    * @return
    */
//    int sampleSuckBloody( const quint8 numarry,
//                            const QString indexSample,
//                            QByteArrayList &suckBloody,
//                            QByteArrayList &SpitBloody,
//                            quint8 &bloodyHole,
//                            QPoint &bloodyOffBloodHole,
//                            QMap<quint8,quint8> &bloodyProject,
//                            QMap<quint8,QPoint> &bloodyinfo);

    //返回在试管盘下降高度
    quint8 _hansdownheightinnertubetray(quint8 _hole);

    /**
    * @brief ClipAnemiatoTestChn 抓贫血到测试通道
    * @param indexChn            通道号
    * @param anaemia_axis        贫血孔号
    * @param outarry             输出命令
    * @param suckBack            吸PPP复位命令号
    * @param splitBack           吐PPP复位命令号
    */
    void ClipAnemiatoTestChn(quint8 indexChn,
                              quint8 anaemia_hole,
                              QVector<QByteArray> &outarry,
                              QPoint &AnemiainEmptyHoleAxis,
                              quint8 &suckBack, quint8 &splitBack);

    /**
    * @brief ThrowTestChnCup 丢测试通道内杯子
    * @param indexChn
    * @param outarry
    * @param issuckBack  吸复位命令号
    * @param issplitBack 吐复位指令号
    */
    void ThrowTestChnCup(quint8 indexChn, QVector<QByteArray> &outarry, quint8 &issuckBack, quint8 &issplitBack);

    /**
    * @brief initequipmentgrabcups 开机初始化抓杯
    * @param index_equipment       仪器类型
    * @param _throwthecups         灌注+清洗动作+丟杯
    */
    void initequipmentgrabcups(quint8 index_equipment,
								QByteArrayList &_throwthecups);

    /** 吸试剂-夹杯-放到测试通道
    * @brief suckReagentClipTubetoChnPut
    * @param indexReag
    * @param TestChn
    * @param emptyHole
    * @param outputArry
    */
    void suckReagentClipTubetoChnPut(quint8 indexReag,int TestChn,quint8 emptyHole,QVector<QByteArray> &outputArry,
                                       QPoint &bloodyHoleAxis,quint8 &suckPRPindex, quint8 &ktestPRPsuckBacknum,
                                       quint8 &ktestPRPsplitBacknum);

    /**
    * @brief spitReanentToTestChn 向测试通道吐试剂
    * @param ChnIndex
    * @param SpitReagentActive
    */
    void spitReanentToTestChn(const int ChnIndex, 
								quint8 indexReagent,
								QVector<QByteArray> &SpitReagentActive);

    /**
    * @brief qualitysucksampleact  质控吸样
    * @param suckSampleAxis
    * @param splitSampleAxis
    * @param outdataList
    */
    void qualitysucksampleact(QPoint suckSampleAxis,
                                QPoint splitSampleAxis,
                                QByteArrayList &outdataList);

    /** 在杯盘吸杯到通道
     * @brief suckCupMoveChannelPut
     * @param GrriperSuckHoleAxis
     * @param channelPutAxis
     * @param gripperDown
     * @param grippersuckCup2Channel
     */
    void suckCupMoveChannelPut(const QPoint GrriperSuckHoleAxis, const QPoint channelPutAxis,
                                 quint8 gripperDown, QByteArrayList &grippersuckCup2Channel,
                                 quint8 &sucknum, quint8 &splitnum);

    /** 丢测试通道的试杯
     * @brief ThrowChannelCup
     * @param channelAxis
     * @param throwCupAxis
     * @param throwSucknum
     * @param throwSplitnum
     * @param throwCupsList
     */
     void ThrowChannelCup(quint8 downsuck, const QPoint channelAxis,const QPoint throwCupAxis, quint8 &throwSucknum,
                            quint8 &throwSplitnum,QByteArrayList &throwCupsList)
    ;


    void _mapConsumablesName(quint8 index_, QString& out_);  //映射耗材名字

    QByteArrayList _controltestbloodpindownheigh(int  index_,int down_mm); //控制血样针下针高度命令

    QByteArrayList _controlReagpindownheih(int  index_,int down_mm);//控制试剂针下针高度命令

    QByteArrayList _controlHandsdowntest(int index_,int down_mm);


    /**
     * @brief ReplaceSuckReagentLoc 开始试剂预留位 判断试剂与警戒线 切换吸试剂位置
     * @param SuckReagArry  原始吸试剂命令容器
     * @param indexReag     要吸的试剂
     */
    void ReplaceSuckReagentLoc(QByteArrayList& SuckReagArry, 
								quint8 indexReag, 
								double LastRatio);



    /**  转换吸试剂的步数
     * @brief QUIUtils::SuckReagentNumberofSteps
     * @param suckIndexReagent  试剂种类
     * @return
     */
    int SuckReagentNumberofSteps(const unsigned char suckIndexReagent);


    //控制PE 加标准物质 高 中 低 4个试杯
    void controlPEAddBasicLinque(QByteArrayList &directives,const QPoint &suckAxis,
                                     const QPoint &standardSolutions,
                                     const QPoint &PEHeighValAxis,
                                     const QPoint &PEMidValAxis,
                                     const QPoint &PELowValAxis);

    //控制PE 加标蒸馏水质 高 中 低 3个试杯
    void testPEWaterSuckSplit(QByteArrayList &outDirectives,const QPoint &suckWaterAxis,
                                        const QPoint &splitWaterAxis,
                                        const QPoint &PEHeighValAxis,
                                        const QPoint &PEMidValAxis,
                                        const QPoint &PELowValAxis);
}

#endif // QUIUTILS_H
