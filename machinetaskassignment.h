#ifndef MACHINETASKASSIGNMENT_H
#define MACHINETASKASSIGNMENT_H

#include <QMap>
#include <QPoint>
#include <QVector>

using namespace std;

#if defined(MACHINETASKASSIGNMENT_LIBRARY)
#  define MACHINETASKASSIGNMENTSHARED_EXPORT Q_DECL_EXPORT
#else
#  define MACHINETASKASSIGNMENTSHARED_EXPORT Q_DECL_IMPORT
#endif


#define EMPTYTOTALTUBE 240
#define WAITINGDONE    280



#define TUBE_EMPTY     0   //空试管未使用
#define TUBE_MARK      1   //试管被标记
#define TUBE_ADDBLOOD  2   //空试管加了血样
#define TUBE_USEDED    3   //试管被使用

#define TRAY_REPLACE_ALL  -1 //更换全部试管
#define TRAY_REPLACE_1    0
#define TRAY_REPLACE_2    1
#define TRAY_REPLACE_3    2
#define TRAY_REPLACE_4    3


#pragma pack (1)
typedef struct EmptyTubeTray_
{
   quint8 HoleNum;
   bool Used;

}EmptyTubeTray;

typedef QVector<EmptyTubeTray*> EmptyTubeTrayVec;


typedef struct TaskHloeRich_
{
    quint8 reagent;        //试剂号
    quint8 bloodHole;     //可删

    quint8 EmptyHole;       //富血在空试管区的位置孔号
    int  InitializeRichValue;

    bool MovedEmptytrayRich;        //富血吸到空测试管
    bool bRichEmptyTubeCatch;       //富血在试管区被夹起

    bool MoveTestChannelRich;       //夹到测试通道

    bool addReagnet;                //吐试剂
    bool AbsorbReagent;             //吸试剂


    bool Clean;

    bool ThrowtheCup;
    bool Completed;

    bool bCleanReagentNeddle;                 //清洗试剂针
    QByteArrayList CleanReagentNeddleCode;    //清洗试剂针命令
    QByteArrayList EmptyTube2TestChannelCode; //富血移动到测试通道命令
    QByteArrayList ThrowChannelTubeCode;      //把测试通道试管丟掉
    QByteArrayList AbsorbReagentCode;         //先吸测试试剂命令
    QByteArrayList AddReagentCode;            //吐加试剂命令
    QList<float> TestValuelist;
    TaskHloeRich_()
    {
        reagent = 1;
        InitializeRichValue = -1;
        Clean = false;
        MovedEmptytrayRich = false;
        MoveTestChannelRich = false;
        Completed = false;
        ThrowtheCup = false;
        bCleanReagentNeddle = false;
        addReagnet = false;
        AbsorbReagent = false;

        TestValuelist.clear();
        CleanReagentNeddleCode.clear();
        EmptyTube2TestChannelCode.clear();
        ThrowChannelTubeCode.clear();
        AbsorbReagentCode.clear();

        bRichEmptyTubeCatch = false;
    }
}TaskHloeRich;
typedef QVector<TaskHloeRich*> TaskHloeRichVec;

typedef struct SingleSample_
{
    QString SampleName;                 //样本号
    int SampleIndex;                    //样本号int
    quint8 TestChannel;                 //分配的测试通道
    quint32 TestHigh;                   //富血的测高值
    quint8 PoorReagent;                 //贫血试剂号 0

    quint8 PoorbloodHole;               //贫血在血样区孔号
    bool   bSuck_PoorBlood;             //贫血被吸走

    quint8 RichbloodHole;               //富血在血样区孔号
    bool   bSuck_RichBlood;             //吸走富血血样


    quint8 PoorEmptyHole;               //贫血在试管区分配孔号

    bool MovedEmptytrayPoor;            //贫血是否滴加到试管标志
    bool bPoorEmptyTubeCatch;           //贫血在试管区被夹起


    int  InitializePoorValue;            //测试到贫血的初始值
    bool MoveTestChannelPoor;           //贫血是否被夹到测试位


    bool ComlpPoorBlood;                //贫血已完成
    bool PoorbloodThrowCup;             //贫血丢
    bool endTestall;                      //此样本所有项目测试结束
    bool CleanBloodNeddle;                //清洗贫血针
    QByteArrayList  Cleanbloodneddlecode; //清洗贫血针命令
    QByteArrayList  PoorMove2TestChannel;  //贫血夹到测试通道
    QByteArrayList  ThrowTubeChannel;       //丟在测试通道的杯
    TaskHloeRichVec TaskRichVector_;        //样本的富血容器

    SingleSample_()
    {
        InitializePoorValue = -1;
        TestChannel = 0;
        MovedEmptytrayPoor = false;
        MoveTestChannelPoor = false;
        endTestall = false;
        PoorbloodThrowCup = false;
        CleanBloodNeddle = false;
        PoorReagent = 0;
        TestHigh = 0;
        Cleanbloodneddlecode.clear();
        PoorMove2TestChannel.clear();
        ThrowTubeChannel.clear();


        bSuck_PoorBlood = false;
        bPoorEmptyTubeCatch = false;
    }
}SingleSample;
typedef QVector<SingleSample*> SingleSampleVec;




/*********单个试管的状态**********/
typedef struct _TubeState_
{
    quint8 Tube_num;   //试管编号
    quint8 tube_state; //试管状态   0: 未使用  1: 被标记  2: 已使用
    quint8 Reagent;   //试剂号
    int smaplenum; //样本号
    _TubeState_(){
        Tube_num = 0;
        tube_state = 0;
        Reagent = 0;
        smaplenum = 0;
    }
}_TubeState_;
typedef QVector<_TubeState_*> SingleTube;  //单个试管的状态



//typedef struct _SampleRichTube
//{

//}_SampleRichTube;
//typedef QVector<_SampleRichTube*> _SampleRichTube_Vec;

//new struct 样本结构
//typedef struct _OneSampleStruct
//{
//    QString SampleName;                     //样本号
//    int     SampleIndex;                    //样本号int
//    quint8  RunnintTestChannel;             //测试通道要测试的通道
//    int     InitRoorbloodValue;             //贫血初始值


//    _SampleRichTube_Vec RichBloodVectorStu; //样本的富血容器




//    quint32 TestHigh;                   //富血的测高值
//    quint8  PoorReagent;                 //贫血试剂号 0
//    quint8  PoorbloodHole;               //贫血在血样区孔号
//    quint8  PoorEmptyHole;               //贫血在试管区分配孔号
//    int     InitializePoorValue;            //测试到贫血的初始值
//    bool    MovedEmptytrayPoor;            //贫血是否滴加到试管标志
//    bool    MoveTestChannelPoor;           //贫血是否被夹到测试位




//    SingleSample_()
//    {
//        InitializePoorValue = -1;
//        TestChannel = 0;
//        MovedEmptytrayPoor = false;
//        MoveTestChannelPoor = false;
//        endTestall = false;
//        PoorbloodThrowCup = false;
//        CleanBloodNeddle = false;
//        PoorReagent = 0;
//        TestHigh = 0;

//    }
//}_OneSampleStruct;
//typedef QVector<_OneSampleStruct*> _Vec_SampleStu; //单个样本结构体数据



#pragma pack ()










class MACHINETASKASSIGNMENTSHARED_EXPORT MachineTaskAssignment
{
public:
    MachineTaskAssignment();
    ~MachineTaskAssignment();
    enum EmptyTray{EmptyTray_1 = 60,EmptyTray_2 = 120,EmptyTray_3 = 180,EmptyTray_4 = 240};

public:
    void    CloseDllFile();

    void    CleanEmptyDllStu(); //清空结构体样本数据

    int     TaskTotalNum(); //配置样本数量

    int     EmptyTubeLast();     //空试管区剩余多少试管

    bool    ChangeChannelTubeMoveFlag(int HoleNum);            //改变测试通道完成夹到位置

    bool    FindPoorBloodMovedTsetChannel(quint8 &Channel, quint8 &Reagents);   //贫血移动到测试通道

    bool    FindRichBlood(quint8 &Channel, quint8 &Reagents, quint8 &richemptyhole); //富血夹到测试通道中准备测试

    bool    FindRichBlood2TestChannel(quint8 Channel, quint8 &Reagents);            //富血被抓到测试模组

    void    TestChannelAddReagents(int hole,int state);     //通道添加试剂

    void    SampleareTesting(QVector<int> &);         //有样本在测试中

    uchar   ReagentMapping(QString ReagentStr);

    bool    DrawBloodSample2EmptyTube(QString,quint8,quint32,QString); //样本号、富血、测高、项目

    QString ChannelMapSampleName(quint8 Channel); //通道的样本名

    int     TotalsampleNum(); //样本总数

    void    ChangeEmptySign(quint8 );  //改变空试管区标志

    quint8  FindChannel(quint8 emptyhole);  //找空试管孔对应通道

    bool    GetAllBloodAndFirstrich(int Channel);  //清洗针动作要清洗几根针

    void    PoorBloodMoveTestChannelsign(quint8 channel, quint8 &testReagent); //贫血移动到测试通道改变标志

    bool    HandsRichBlood2Testmode(quint8 Channel, quint8 &Emptyhole); //抓取样本得富血到测试通道测试

    void    RichBloodInitValue(quint8 Reagent, int RichValue, quint8 Channel); //保存单个项目富血初始值

    int     GetInitRichValue(quint8 channel, quint8 &Reagent);  //获取初始富血值

    int     GetInitPoorValue(quint8 channel,QString &namestr);   //获取初始贫血值

    int     GetSingleResultNum(quint8 channel, quint8 Reagent); //单个项目得结果个数

    void    SetSingleResultNum(quint8 channel, quint8 Reagent,float TestResult); //保存单个测试结果

    void    SetSingleState(quint8 channel, quint8 Reagent, bool comple); //单个项目的状态

    void    SetAllComplete(quint8 Channel,bool state); //设置样本项目全部做完了

    bool    LastRichSample(quint8 Channel, quint8 regent); //剩余富血待测样本

    //测试通道满的时候任务未分配
    void    TestModelFullTesting(QPoint &PoorBlood, QMultiMap<quint8, quint8> &richBlood, uint &high);

    //"样本号" << "测试通道" << "贫血孔" << "富血孔" << "贫血值" <<"APD结果"<<"AA结果"<<"COL结果"<<"R结果"
    void    SampleProjectFinishSaveSQL(quint8, quint8 indexreagent, QString &, quint8 &, quint8 &, int &poorvalue, QList<float>&Listdata);

    bool    GetPoorBloodMovedChannel(quint8 Channel);  //Get贫血移动到测试通道标志

    void    DeleteThisComple(int sampleID ); //删除完成测试的样本

    int     NumUnassignedChannel(); //未分配任务通道的个数

    bool    NewTaskSample(quint8 Channel);

    QString GetSampleName(quint8 Channel, quint8 reagents, QString &Resultout);

    bool    EmptyTubeHasAdded();   //空试管区有没贫血

    bool    MovePoorBlood2EmptyTube(QPoint &poorTube);    //单个样本的贫血孔

    //单个样本的富血孔
    void    GenerateUsbCommandParameters(QMultiMap<quint8, quint8> &PresonalRich, uint &high);

    bool    ChangeEmptyTubeMoveFlag(int HoleNum);               //改变空试管区加血样完成

    bool    ALLBloodAddEmptyTube(); //判断一个样本全部移动到空试管区

    bool    SetSampleTestChannel(quint8 Channel);  //样本数据绑定到要测试的通道

    //丢杯后的动作
    bool    ThrowTubeNextTask(quint8 Channel ,bool ThrowPoorblood ,quint8 &EmptyTube,quint8 &Reagents);

    //////////////////////////////////////////////////////////////////////////////////////////
    void     SetBloodAddSampleEmptyTube(quint16 );  //血样加到空试管

    void     ChangeThrowCupSign(quint8 Channelnum, bool, quint8 doneProject);  //改变丢杯标志

    void     AllProjectDone(); //删除

    void     DeleteOneTask(quint8 Channel,quint8 Project);

    bool     HadSamplePoorbloodWaitMoveChannel(quint8 &Channel); //有任务下一步动作是移动到测试通道

    quint8   BackdoingTestBloodnum(quint8 ); //返回正在测试的血样号

    void     EmptyTraySelTube(QString Samplename, QList<quint8> &PersonBlood); //任务选中的要用的空试管孔

    void     ColorSampleNumText(quint8, QString &manySample); //根据空试管分配的孔号返回样本号

    void     ColorEmptyAddBlood(quint8 EmptyHole, bool &poorblood, quint8 &BloodStyleHole); //任务完成血样孔变灰的孔

    bool     AllTestChanneltesting();//所有分配了测试通道的样本正在测试

    void     LastSampletesting(quint8 &); //有样本即将完成

    bool     WaitNumSample(); //添加任务要等几个

    bool     ChannelState();

    bool     WaitTestEmptygsample(quint8 FreeChannel); //有待测样本在空试管区

    bool     FindTestChannelNum(quint8 ); //找通道号
 /////////////////////////////////////////////////////////////////////////////////////////////////
    void    GetSampleReagentNums(quint8 TestChannel, QList<quint8> &); //获得样本的试剂号

    void    InsertAbsorbReagentCode(quint8 IndexChannel, quint8 IndexReagent , QByteArrayList Arraycode);  //插入吸试剂命令

    void    InsertPoorBloodCode(quint8 IndexChannel, QByteArrayList Arraycode);  //插入贫血移动到测试通道

    void    InsertThrowTubeCode(quint8 IndexChannel,QByteArrayList Arraycode);   //丟杯

    void    GetRichBloodinEmptyTube(quint8 IndexChannel,QList<quint8> &RichinbloodEmptyHolelist); //获取富血在空试管中的孔号

    void    InsertRichblood2ChannelCode(quint8 IndexChannel, quint8 EmptyTube ,QByteArrayList Arraycode);//富血夹的测试通道

    void    InsertAddReagentCode(quint8 IndexChannel, quint8 IndexReagent , QByteArrayList Arraycode);

    //void    InsertCleanReagentNeddlecode(quint8 IndexChannel,QByteArrayList Arraycode);

    QByteArrayList OutClipPoorblood2TestChannel(quint8 IndexChannel);  //输出夹贫血到测试通道命令

    bool    GetPoorBloodClipTestChannel(quint8 IndexChannel);  //贫血移动到测试通道状态

    void    SetPoorBloodClipTestChannel(quint8 IndexChannel, bool cliped);

    bool    GetInitPoorValueSaved(quint8 IndexChannel);  //贫血值是否保存成功

    QByteArrayList OutThrowTube(quint8 IndexChannel); //丟杯命令

    QByteArrayList OutAbsorbEmptyTube2TestChannel(quint8 IndexChannel); //夹空试管到测试通道

    void    AllSingletonProjectState(quint8 IndexChannel,QMap<quint8,bool> &); //单个样本所以项目的状态

    QByteArrayList OutAbsorbReagent(quint8 IndexChannel); //吸第试剂

    bool     GetEmpty2TestChannel(quint8 IndexChannel,quint8 Reagents);  //获取试管夹到通道状态

    void     SetEmpty2TestChannel(quint8 IndexChannel,quint8 Reagents,bool cliped);

    QByteArrayList    outAddReagentCode(quint8 Channel, quint8 reagent);

    void    SetAddReagentState(quint8 Channel,quint8 Reagent,bool );   // 设置添加试剂标志

    bool    GetCleanReagentsNeddle(quint8 Channel, quint8 Reagent);   //试剂针的清洗状态

    bool    GetCleanBloodNeddle(quint8 Channel);     //血样针的清洗状态

    void    SetCleanReagentsNeddle(quint8 Channel, quint8 Reagent ,bool);   //设置试剂针的清洗状态

    void    SetCleanBloodNeddle(quint8 Channel , bool Cleaned);
    /*通过样本号设置血样针清洗状态*/
    void    SetBloodNeedleCleanState(const QString SampleName,const bool CleanState );

    //通过样本号获取血样针清洗状态
    bool    GetWashBloodNeddleState(QString SampleNum);

    bool    HandProjectWillDone();   //有样本快测完成

    void    SetThrowRichtube(quint8 IndexChannel,quint8 reagent,bool cliped);  //改变丢杯状态

    QByteArrayList  outAbsorbReagentothers(quint8 IndexChannel,quint8 reagent); //吸第非第一个项目试剂

    QByteArrayList  OutEmptyTube2TestChannelother(quint8 IndexChannel,quint8 Reagent); //夹其它空试管

    quint8   SetEmptyTube2TestChannelFlag(quint8 IndexChannel, quint8 HoleNum); //设置空试管夹到测试通道

    bool     EndTaskSampleTesting(quint8); //有任务完成最后一个样本要在测试中

    bool     FindPoorBloodMoveChannel(quint8 &EmptyTube, quint8 channel); //找到贫血是否移动到测试通道测试

    bool     ClipRichTube2ChannelState(quint8 IndexChannel,quint8 Reagents); //富血移动到测试通道状态

    QString     CancelTask(int ,QList<int> &);  //取消任务

    void      Tasknottesting(QQueue<QString> TodayItem,QList<int> &); //样本未开始测试的样本

    int       SamplenummapPoorblood(int ); //样本号映射贫血号







    /******************试管设置*************************/
    void     ReplaceTrayEmptyTube(int IndexTray);  //更换试管盘
    int      GetEmptyTubeindex();                   //获取空试管的位置
    quint8   GetRemainingtubestotal();              //获取台面剩余空试管总数
    void     SetTubeMark(quint8 );                   //设置试管被标记
    void     SetTubeAddBlood(quint8 tubenum);        // 滴加血样
    void     SetTubeMovedTest(quint8);               //设置血样试管被夹走测试
    void     InitAllTubeInfo(quint8 tubenum , quint8 stated, quint8 ReagentType, int Number); //配置表中保存的试管信息
    void     SetTubeBindInfo(quint8,quint8,int);        //试管绑定样本号试剂
    void     SynchTubeInfo(quint8, quint8 & , int & Number);         //同步试管信息






    /*new*/
    //血样吸完成等待分配通道测试
    bool    DLL_SampleWaitintTest(QString &SampleNum);
    void    DLL_SetSuckRoorOrRichBlood(int Hole);       //吸贫血、富血
    void    DLL_SetRoorOrRichBloodMovedEmptyTube(int Hole);   //贫血/富血 吸到空试管区
    bool    DLL_HadSampleSuckFinished(QString &SampleNum);   //样本吸样完成？
    void    DLL_SampleConfigureChannel(const QString assignchannel_Samplenum, const quint8 Channel); //为样本分配测试通道
    int     DLL_NotAssignchannelNum(); //未分配测试通道的样本 个数
    quint8  DLL_PoorEmptyTube(const QString assignchannel_Samplenum);  //贫血在空试管盘的孔号
    void    DLL_SetCatchEmptyTube(const int HoleFinish, bool &PoorBlood);//空试管区试管被抓起true
    quint8  DLL_MatchDistributionChannel(int emptytubenum);    //匹配分配通道
    bool    SetSampleanaemiaValue(quint8 Channel, int poorValue); //设置样本贫血值到结构
    void    DLL_PoorBloodPutDownChannel(quint8 Channel);  //贫血放到测试通道
    void    DLL_RichBloodPutDownChannel(const quint8 Channel);  //富血放到测试通道
    bool    GetThrowPoorBloodTube(quint8 IndexChannel);  //丟贫血的状态
    void    SetThrowPoorBloodTube(quint8 IndexChannel ,bool bthrow);  //贫血丢杯完成
    quint8  DLL_AgenttobetestedIndex(const quint8, quint8 &EmptyTube);  //待测试剂
    void    DLL_AbsorbReagentFlag(const quint8); //标记样本项目试剂已吸
    void    DLL_SaveRichBloodInitValue(const quint8 Testchannel, const int InitRichValue);//保存富血初始值
    void    DLL_SpitReagentFlag(const int Channel);     //吐试剂

    int     DLL_GetInitPoorValue(const quint8 );
    int     DLL_GetInitRichValue(const quint8 TestChannel);
    int     DLL_GetTestChannelValueSize(const quint8 TestChannel, const quint8 ReagentTypy);
    void    DLL_SetTestingValue(const quint8, const quint8 Reagent, const double);
    bool    DLL_SetRunningTestFinish(const quint8, quint8 Reagents);  //单个项目测试完成

    QString DLL_GetSampleName(const quint8 channel);
    quint8  DLL_GetTypeReagent(const quint8 channel);
    void    DLL_SetThrowRichbloodTube(const quint8 TestChannel); //丢富血标志
    bool    DLL_ChannelHadFinishen(quint8 &);  //有通道完成了试剂测试

    bool    DLL_NeedCleanMachine(const quint8 ); //通道吐完试剂后清洗状态
    //获取样本有项目数
    void     DLL_GetHaveNumberTypeReagent(const QString SampleNameIndex , int &KindReagents);

    //存在任务样本号
    void     DLL_GetSampleNumber(QStringList &testingSample);

    //血样吸血到空试管区的动作
    void  DLL_SampleAbsorbBlood_2_EmptyAreatube_Init(QString SampleIndex);


public:
    void DLL_SampleNotassignmentChannel(QVector<QString> &SampleNumVec);  //样本未分配测试通道

    void DLL_GetSuckPoorbloodAndSlipHole(QString SampleIndex,QPoint &PoorBloodPos);//根据样本号获取贫血 吸血样、吐血样的孔号

    quint32 DLL_GetSuckRichBloodAndSlip(QString SampleIndex ,QMultiMap<quint8,quint8> &PresonalRich);//根据样本号获取要吸富血孔号吐试管号

    void   DLL_GetSampleEmptyTube(const QString SampleIndex,QMap<quint8,quint8> &Reagent_Emptyhole); //获取试剂映射试管孔号

    void  DLL_SetTestInitValue(const quint8 Reagent, QString SampleName, quint8 TestChannel, const int setinitValue);
private:

    SingleTube* m_SignleTubeState = nullptr;  //单个试管的状态

    SingleSampleVec * m_SingleSampleVec = nullptr; //样本
};
#endif // MACHINETASKASSIGNMENT_H
