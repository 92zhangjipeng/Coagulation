#ifndef CUSTOM_STRUCT_H
#define CUSTOM_STRUCT_H

#endif // CUSTOM_STRUCT_H

#include <QDataStream>
#include <QIODevice>
#include <QIODevice>
#include <QMetaType>
#include <QQueue>       // 必须包含QQueue定义
#include <QPoint>
#include <qvector.h>


#define  GRIPPERMOVE_ABNORMALLY     true   //抓手动作异常
#define  GRIPPERMOVE_NORMAL         false  //抓手动作正常

#define  SAMPLE_GIVEUP               true   //样本放弃或挂起
#define  SAMPLE_NORMAL              false

#define  SEND_ORIGIN_DATA            0      //发送数据起点
#define  GET_COMMAND_INDEX           2      //获取命令编号
#define  DELAY_TIMENUM               1      //延时发送时间
#define  DELAY_READ_TUBE_INITVALUE   1000   //延时读贫富血测试底值


#define  NOT_ADD_SAMPLE              0  //未加样状态
#define  ADDING_SAMPLE               1  //加样中
#define  ADDED_COMPLETE_SAMPLE       2  //加样完成

#define  CHN_STATUS_FREE             0 //通道状态- 空闲
#define  CHN_STATUS_TESTING          1 //测试中
#define  CHN_STATUS_DISABLE          2 //通道禁用
#define  CHN_STATUS_FINISHED         3 //测试完成



#define FOCUS_ADDING_SAMPLE          1  //聚焦样本执行 吸样动作
#define FOCUS_CLEAN_BLOODPIN         17 //聚焦样本执行 清洗血样针
#define FOCUS_CLIP_ANEMIA_TO_CHN     3  //夹贫血到测试通道
#define FOCUS_THORW_ANEMIA           4  //丟在测试通道中的贫血

#define FOCUS_TESTING_AA              5     //吸AA试剂夹富血到放到测试通道
#define FOCUS_TESTING_ADP             6     //吸ADP试剂夹富血到放到测试通道
#define FOCUS_TESTING_EPI             7     //吸EPI试剂夹富血到放到测试通道
#define FOCUS_TESTING_COL             8     //吸COL试剂夹富血到放到测试通道
#define FOCUS_TESTING_RIS             9     //吸RIS试剂夹富血到放到测试通道


#define FOCUS_SPIT_AA_TESTING           10  //吐AA试剂到测试通道
#define FOCUS_SPIT_ADP_TESTING          11
#define FOCUS_SPIT_EPI_TESTING          12
#define FOCUS_SPIT_COL_TESTING          13
#define FOCUS_SPIT_RIS_TESTING          14

#define FOCUS_CLEAN_DOUBLEPIN           15

//清洗试剂针
#define FOCUS_CLEAN_EREAGENTPIN_AA         16
#define FOCUS_CLEAN_EREAGENTPIN_ADP        23
#define FOCUS_CLEAN_EREAGENTPIN_EPI        24
#define FOCUS_CLEAN_EREAGENTPIN_COL        25
#define FOCUS_CLEAN_EREAGENTPIN_RIS        26

//丢杯
#define FOCUS_THROW_AA_TUBE             18
#define FOCUS_THROW_ADP_TUBE            19
#define FOCUS_THROW_EPI_TUBE            20
#define FOCUS_THROW_COL_TUBE            21
#define FOCUS_THROW_RIS_TUBE            22

#define FOCUS_ERR_BACKORIGIN            200  //仪器异常XY 先复位


#define MAX_RESIZE_VEC                 50


#pragma once

typedef struct Finishdata
{
    quint8 index_Chn;
    quint8 index_reagent;
    int CompleteSampleid;
    Finishdata() {}
}Finish_Data_;



//富血信息
typedef struct bloodyInfo
{
	QPoint bloody_axis;
    quint8 bloody_hole;
    quint8 index_reagent;
}BLOODY_INFO;
Q_DECLARE_METATYPE(BLOODY_INFO)



class single_action_commands
{
public:
    single_action_commands(quint8 index_,bool bcompleted,QByteArray arrcode)
    {
        this->arrcode = arrcode;
        this->bcompleted = bcompleted;
        this->index_ = index_;
    }
    bool operator ==(const single_action_commands &another) const
    {
        return (index_ == another.index_);
    }
    quint8 index_;
    bool bcompleted;
    QByteArray arrcode;
};
typedef QVector<single_action_commands *> tSingleActive_list;



//找接收到的指令编号
typedef struct find_recvIndex
{
    find_recvIndex(quint8 index) : m_index(index){ }
    bool operator()(single_action_commands *another ) const
    {
        return (m_index == another->index_);
    }
private:
    quint8 m_index;
}recv_index;


//找是否有未完成的指令
typedef struct find_finishall
{
    find_finishall(bool bstate) : m_bcomplete(bstate){}
    bool operator()(single_action_commands *another ) const
    {
        return (m_bcomplete == another->bcompleted);
    }
private:
    bool m_bcomplete;
}find_allactivefinish;





typedef struct Testing_reagents
{
    tSingleActive_list  pThrowCupsActive;       //丢杯
    tSingleActive_list  pSpitReagentsActive;    //吐试剂
    tSingleActive_list  pCleanReagentActive;    //清洗试剂针
    tSingleActive_list  pTestingReagentActive;  //吸试剂试管盘抓杯到测试通道

    quint8 throwPRRsucknum = 0;
    quint8 throePRPsplitnum = 0;

    quint8 testPRPsucknum = 0;
    quint8 testPRPsplitnum = 0;

    quint8 suckCupIndexCommand = 0;  //吸杯开始指令号
    quint8 index_Reagent = 0;       //测试测试剂
    quint8 indexHole = 0;           //血样的孔
    QPoint emptyHoleAxisHands{0,0}; //富血空相对抓手坐标
    int    inintBloodyValue = 0;    //富血初值
    bool   testfinished = false;
    Testing_reagents() = default;

}TESTING_REAGENT;
typedef QVector<Testing_reagents *> tVariousReagentsvec;

//根据测试试剂找
typedef struct findtest_testreagent
{
    findtest_testreagent(quint8 num) : index_reag(num) {}
    bool operator()(Testing_reagents *another) const
    {
        return (index_reag == another->index_Reagent);
    }
private:
    quint8 index_reag;
}findtest_testreagent;

//找未完成测试的试剂
typedef struct find_nottest
{
    find_nottest(bool wait_test) : m_notTest(wait_test) {}
    bool operator()(Testing_reagents *another) const
    {
        return (m_notTest == another->testfinished);
    }
private:
    bool m_notTest;
}find_nottest;


#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    #define DATA_CONTAINER QVector
#else
    #define DATA_CONTAINER QList
#endif


typedef struct DataSampleStruct
{
    QString samplestr;
    int     sample_num = 0;
    bool    bemergency = false;            //急诊
    bool    _bthisSampleaddData = false;  //样本添加数据状态
    bool    bstartTesting = false;        //运行样本开始测试后变为true
    int     indexTestingChn = -1;        //测试通道
    int     initAnemiaValue = 0 ;        //贫血初值
    bool    fcus_in = false;             //聚焦样本
    quint8  index_fcus_Active = 0;       //聚焦执行在哪个步骤
    quint8  suckanaemia_hole = 0;        //吸PPP孔号
    quint8  suckbloody_hole = 0;         //吸PRP孔号
    QPoint  suckanaemia_axis{0,0};
    QPoint  suckbloody_axis{0,0};
    quint8  CompleteAspirationState = 0;  //吸样状态
    quint8  spitanaemia_hole = 0;         //在试管区吐PPP孔号
    QPoint  spitanaemia_axis{0,0};
    QPoint  anemiaoffHandsAxis{0,0};     //贫血抓手坐标
    bool    bcleanbloody_state = false;     //清洗血样针状态
    bool    bcleanDoublePin_state = false;  //清洗双针状态

    bool    bhandsErr = false;    //抓手异常
    bool    bgivesample = false;  //放弃样本
    bool    bPendingtimeSample = false; //挂起样本
    tSingleActive_list prePatchActions;   //补救抓手前的动作


    // 容器成员（直接默认构造，避免冗余操作）
    QVector<BLOODY_INFO> bloody_tube;
    tSingleActive_list   clean_reag_blood_pin;       //清洗双针
    tSingleActive_list   clean_blood_pin;            //清洗血样针
    tSingleActive_list   tadd_sample_active;         //加样动作


    tSingleActive_list   t_catchAnemiaTest;          //抓PPP到测试通道
    quint8  suctionPPPreset = 0;                     //吸PPP复位编号
    quint8  spitPPPreset = 0;                        //吐PPP复位编号


    tSingleActive_list  t_throw_anemiacups_active;  //从测试通道丢PPP
    quint8 throwPPPsuckBacknum = 0;
    quint8 throwPPPsplitBacknum = 0;


    tVariousReagentsvec  ptestReagent_active;        //测试试剂动作

    DataSampleStruct() = default;
}DATASAMPLESTRUCT;

Q_DECLARE_METATYPE(DATASAMPLESTRUCT)

// 跨版本容器定义
using DataSampleList = DATA_CONTAINER<DATASAMPLESTRUCT* >;

// 清理宏定义
#undef DATA_CONTAINER
//typedef QVector<DATASAMPLESTRUCT *> vec_sampledata_task;  //所有样本信息

//找样本ID是否存在
typedef struct find_id
{
	find_id(int sample_id) : m_id(sample_id) {}
    bool operator()(DATASAMPLESTRUCT* rhs) const
	{
		return (m_id == rhs->sample_num);
	}
private:
	int m_id;
}find_id;

//找未开始测试的样本是否存在
typedef struct find_not_testing
{
    find_not_testing(bool bfinished) : bsampleState(bfinished){}
    bool operator()(DATASAMPLESTRUCT* data) const
    {
        return (bsampleState == data->bstartTesting);
    }
private:
    bool bsampleState;
}FIND_UN_TEST;



//根据测试通道找信息
typedef struct find_sampleinfo
{
    find_sampleinfo(int testChn) : m_Chn(testChn) {}
    bool operator()(DATASAMPLESTRUCT* rhs)const
    {
        return (m_Chn == rhs->indexTestingChn);
    }
private:
    int m_Chn;
}find_sampleinfo;
//找样本是否存在有焦点任务
typedef struct find_bfocus
{
    find_bfocus(bool hanfocuon) : m_hanfocus(hanfocuon) {}
    bool operator()(DATASAMPLESTRUCT* rhs)const
    {
        return (m_hanfocus == rhs->fcus_in);
    }
private:
    bool m_hanfocus;
}find_bfocus;



typedef struct find_id_str
{
    find_id_str(QString sample_idstr) : m_idstr(sample_idstr) {}
    bool operator()(DATASAMPLESTRUCT* other)const
    {
        return (m_idstr == other->samplestr);
    }
private:
	QString m_idstr;
}find_id_str;







//测试通道
typedef struct TestChnData
{
    int index_;
    int raw_data;
    float test_data;
}TESTCHNDATA;
typedef QVector<TESTCHNDATA *> chntest_reagdata;

typedef struct TestChnStatus
{
    quint8  index_Chn;
    quint8  Chn_Status;
    QString samplename; //ID
    quint8  Reagent;
    chntest_reagdata  AA_testchndata_;
    chntest_reagdata  ADP_testchndata_;
    chntest_reagdata  EPI_testchndata_;
    chntest_reagdata  COL_testchndata_;
    chntest_reagdata  RIS_testchndata_;
}TESTCHNSTAUSINFO;

//找执找通道的命令
typedef struct finder_Chn
{
    finder_Chn(quint8 num) :index_testChn(num){}
    bool operator()(TESTCHNSTAUSINFO *another) const
    {
        return (index_testChn == another->index_Chn);
    }
private:
    quint8 index_testChn;
}finder_Chn;
//根据样本找数据
typedef struct finder_sampleId
{
    finder_sampleId(QString num) :idstr(num){}
    bool operator()(TESTCHNSTAUSINFO *another) const
    {
        return (idstr == another->samplename);
    }
private:
    QString idstr;
}finder_sampleId;


//找测试状态
typedef struct finder_ChnSatte
{
	finder_ChnSatte(quint8 num) :index_testState(num){}
    bool operator()(TESTCHNSTAUSINFO *another) const
    {
        return (index_testState == another->Chn_Status);
    }
private:
    quint8 index_testState;
}finder_ChnSatte;







struct BloodPinParams {
    // 16位参数组
    struct {
        quint16 cleanLinqueblood;    // 吸收X2清洗量 [[原始代码]]
        quint16 suckppporprp;        // 常量吸废血量 [[原始代码]]
        quint16 suckairs;            // 吸空气量 [[原始代码]]
        quint32 washesTime;          // 清洗次数 [[原始代码]]
    } paramIndex4;

    // 8位参数组
    struct {
        quint8 emptyHeight;           // 空吸高度
        quint8 ratioBen;              // 比例因子（需*10存储）
        quint8 securityValue;         // 安全值
        quint8 fixedHighValue;        // 固定高度
        quint8 cleanFailedHigh;       // 清洗失败高度
        quint8 testHeightDown;        // 测高下降高度
        quint8 failedLinque;          // 失败清洗次数
        quint8 prpRatio;              // PRP比例（需*10存储）
        quint8 testHeightOffset;      // 测高偏移量
        quint8 prep;                  //预备

    } paramIndex16;
};






