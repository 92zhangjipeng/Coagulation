#ifndef STRUCTINSTANCE_H
#define STRUCTINSTANCE_H
#pragma execution_character_set("utf-8")
#pragma once
#include <QObject>
#include <QMutex>
#include <mutex>
#include <atomic>
#include "custom_struct.h"
#include "QsLog/include/QsLog.h"


class StructInstance : public QObject
{
    Q_OBJECT
public:
    static StructInstance *getInstance()
    {
        if(m_pInstance == NULL)
        {
            QMutexLocker mlocker(&m_mutex);
            if(m_pInstance == NULL)
            {
                m_pInstance = new StructInstance();
            }
        }
        return m_pInstance;
    }
public:
    //vec_sampledata_task m_BloodsampleInfo;  //血样信息

    DataSampleList m_BloodsampleInfo;

    QVector<TESTCHNSTAUSINFO *> m_testChnStructvec_;  //测试通道

 public:
    const bool getHandsStatus(const int sampleid); //重试
    void  setHandsErrStatus(const int sampleid, bool handsState);

    const bool getSampleGiveupStatus(const int sampleid);  //放弃
    void  setSampleGiveupStatus(const int sampleid, bool isGiveup);

    const bool getSamplePendingtimeStatus(const int sampleid);  //挂起
    void  setSamplePendingtimeStatus(const int sampleid, bool ispendingtime);

    //输出挂起样本号的焦点动作
    bool continueTestPendTimeSample(quint8 &focuActive, int sampleId);

    //输出第一个未完成指令
    QByteArray outputstheFirstIncompleteInstruction(const int smapleId,const quint8 indexActive);
    QByteArrayList outputsIncompleteInstructionList(const int smapleId,const quint8 indexActive);


    //遍历加样状态 返回 true 有未完成  false:加样已完成
    static bool backSampleAddState(const DATASAMPLESTRUCT *itsample, QByteArray &data);

    //返回测试PPP的未完成指令
    static bool backSampleTestPPP(const DATASAMPLESTRUCT *itsample, QByteArray &data);

    //丟杯子未完成指令
    static bool backSampleThrowPPP(const DATASAMPLESTRUCT *itsample, QByteArray &data);

    //测试试剂动作
    static bool backTestReagentNotFinishArry(const DATASAMPLESTRUCT *itsample, QByteArray &data,const quint8 testReagent);

    //返回丢杯未完成命令
    static bool backThrowCupsNotFinishArry(const DATASAMPLESTRUCT *itsample, QByteArray &data,const quint8 testReagent);

    //输出吐试剂命令
    tSingleActive_list backSplitReagentComm(const DATASAMPLESTRUCT *itsample,const quint8 testReagent);

    //输出清洗试剂针命令
    tSingleActive_list backCleanReagentNeedleComm(const DATASAMPLESTRUCT *itsample,const quint8 testReagent);

    bool  recvPrepatchActions(const int id ,const quint8 activenum);
    void  fillRecvPrepatch(const int id, const QByteArrayList dataList); //填充
    void  clearRecvPrepatch(const int id); //清空

public:
    //根据通道测试号获取样本名
    void rootTestChnGetSampleInfo(const quint8 index_Chn,
                                    QString &infon_num,
                                    int &id);


    //获取贫血值和测试数据
    void root_getinitvaluedata(const quint8 finishChn,
                                 quint8 index_reag,
                                 quint16 &AnaemiaInitValue,
                                 QVector<double> &TestedDatamap,
                                 quint8 &suckanemia,
                                 quint8 &suckbloody);

    //防止错误,清空无效命令
    void _Clearinvalidcommands(DATASAMPLESTRUCT *psanlpeinfo);


    //设置为焦点 焦点在哪机器就处理哪个样本号 增、改
    void config_focusSample(int indexSample, const bool bfcusin);

    //取消焦点  删
    void config_dismissFocusSample(int indexSample);

    //判断有样本在焦点  查
    bool sampleHadFocus();

    //查找动作焦点 查
    quint8 read_focusSampleActive(int sample_focu);

    //样本个数
    void SampleTotal(int &num_total);


public:


    //清空内存
    void _setemptynull();

    //删除一整个测试样本包括通道数据
    void onetestend_del(int indexSample);

    void clearSampleDataByNumber(DATASAMPLESTRUCT& data, int targetSample);
    void removeSampleFromContainer(DataSampleList &container, int target);

    void clearTestingReagentContainer(tVariousReagentsvec& container);
    void clearTestingReagentData(TESTING_REAGENT* reagent);


    //取消任务--只删除样本数据
    void cancelsampletask(quint8 cancelsanpleid,
                            int &canceltube_num,
                            quint8 &anemiahole,
                            QList<quint8> &emptytubehole,
                            quint8 &_cancelbackhole);




    //获取样本所需数据(通道)
    void sycn_sampleneed_data(const int id, quint8 &tsetChn);

    //判断添加的样本是否存在
    bool outAlreadyAddSample(QString sampleId);

    //++样本血样信息
    void addSamplebloodInfo(DATASAMPLESTRUCT *&pinfo);

    //开始任务时对样本排序--有急诊的再置顶
    void SortemergencyattheTop();

    //输出准备吸样的样本号
    int  OutputPrepareAspirationNum();

    //更新样本状态== 加样中
    void updateSampleState(const int indexNum, const quint8 state);

    //仪器运动映射孔号
    bool MoveAxisMapingHole(const QPoint sportpos, quint8 &movedLochole, bool &bemptyzone);

    //运动抓走的试管
    bool EmptytubeClipAway(const QPoint sportpos, quint8 &movedLochole);

    //设置样本测试通道
    void setTestingChn(const int focuSampleID, const quint8 indexchn );

    //放弃样本输出未测试的试管号
    void labelCancelTubes(const int sampleid,QList<quint8> &outCancelTube);

    //设置焦点样本执行动作
    void  write_focusSampleActive(int sample_focu,quint8 index_active);

     //获取焦点所在&&通道已分配的
    int   ClipEndGetAnemiaValeChn(quint8 &TestingChn, quint8 indexActive,QString outText);

    void  oper_AnemiaValue(bool bwrite,quint8 indexChn,int &valuedata); //读写贫血




    //void reagentTestcommand(const int sampleiD, quint8 &firstTestReag);

    /**试剂测试指令填充到结构体
     * @brief reagentTestintotheStructure
     * @param sampleId
     * @param firstTestReagentNum
     */
    void reagentTestintotheStructure(const int sampleId, quint8 &firstTestReagentNum);


    //保存富血初值
    void setRichBloodInitValue(quint8 index_reag, int index_Chn,int setValue);

    //测试时输出贫血、富血初值
    QString testingOutInitPPPandPRP(quint8 IndexChannel,
                                     int &AnaemiaInitValue,
                                     int &bloodyinitValue,
                                     quint8 &index_reag,
                                     int &testSample);

    //单个试剂测试完
    void setupOneReagentsIsComplete(const QString SampleId, quint8 index_reag, const bool &testErr);


    //获取血样针清洗状态
    bool judge_alreadyCleanBloodpin(const int sampleId, quint8 indexActive);

    //是否有待测试剂
    bool judge_hadWaitTestReag(const int testSample, quint8 &waitTestRrag);

    //已加样完成等待通道的样本
    bool alreadyadded_wait_chn(int &wait_sampleid);

    //输出已加样未测试的吸贫血孔样本列表
    void cancel_sample_task(QList<int> &sample_tasklist, QList<int> &sampleid);

    //样本要使用的试杯
    void sycn_sample_all_test_tube(QString sample, QList<quint8> &anemiatube_bloodhole);


    //根据空试管号 返回样本id
    int  sycn_emptytube_get_sampleid(quint8 indexhole);

    //删除清洗双针命令
    void _deleteCleanDoublepinorder(tSingleActive_list* pdelorder);
    void _delTestReagentActive(tVariousReagentsvec *ptestReagActive);

    //删除完成通道数据
    void _deleteOneSampleTesteddata(const quint8 _chntested, const int indexSample);
    void _delReagentData(chntest_reagdata *pdelchndata);




public:

    void addEquipmentChn(const QList<quint8> &equipmentOperChn); //根据读取的仪器型号存在的通道值初始化

    void config_testChn_State(const quint8 indexChn,quint8 ChnState); //设置通道状态

    void config_testChn_test_reagent(const quint8 indexChn,const quint8 indexReag);//设置通道测试设试剂

    void config_testChn_test_SampleID(const quint8 indexChn,const int sampleID);//设置通道测样本号

    bool hadFreeTestChn(quint8 &indexChn_);  //遍历是否有空闲通道

    bool hadTestChnTestFinish(quint8 &indexChn_); //有测试完成通道

    void fetchTestDataTotal(QString samplename, quint8 index_reag, int &totalNum); //获取测试个数

    void updte_saveChnTestData(const quint8 &IndexChannel,
                                 const  quint8 &index_reag,
                                 const int &testValue, const float &testcalculatingData);//++测试数据

    void delalltaskinfo(const bool exitapp);

public:
    const DATASAMPLESTRUCT *rfindIdSample(const int idSample);

    DATASAMPLESTRUCT *wfindIdSample(const int idSample);


    //丢PPP指令动作
    void creatThrowCupsCommands(const int sampleId, const quint8 indexChn);
    QByteArray outputHeadnThrowCups(const int sampleId,bool isSetAllFalse);
    void throwPPPFailed(const int sampleId,bool &kGripperFailed);
    void allGripperThrowFinish(const DATASAMPLESTRUCT *psampledata,bool &isthrowFinish,QByteArray &outdata);
    /**
     * @brief recv_throwCupcommd
     * @param sampleId
     * @param index_code
     * @param HandsControl
     * @param isSuction
     * @param airvale
     * @param sendData
     * @param kGripperFailed
     * @param laterTimer  吸PPP舍弃第一次吸的值延时读第2次吸杯值
     * @return
     */
    bool recv_throwCupcommd(const int sampleId, const quint8 index_code, bool HandsControl,
                              bool isSuction, quint32 airvale,
                              QByteArray &sendData, bool &kGripperFailed, bool &laterTimer);






    /** 构造加样处理函数
     * @brief creataddSampleCommands
     * @param addSamplecommand
     * @param psampleinfo
     */
    void creataddSampleCommands(const QByteArrayList addSamplecommand,
                                   DATASAMPLESTRUCT *&psampleinfo);



    //输出吸样命令(开始)根据样本号
    void OutputCommands(const int taskNum, QByteArrayList &sendData);  ///////////////测试后可删除

    bool Recv_addsampleData(const int sampleId, const quint8 index); //收到样本加样命令  ///////////////测试后可删除

    /**  样本加样首指令
     * @brief addSampleHeandCommand
     * @param taskNumid
     * @return
     */
    QByteArray addSampleHeandCommand(const int taskNumid);

    /**  接收加样处理
     * @brief recvAddSampleIndexCommand
     * @param sampleId
     * @param index
     * @param completedAddSample
     * @return
     */
    QByteArray recvAddSampleIndexCommand(const int sampleId, const quint8 index,bool &completedAddSample);



    /**抓贫血到测试通道**/
    void creatSuckPPP2theTestChannel(const int sampleId,const QVector<QByteArray> CatchAnemiacommand,
                                       const quint8 isSuctionPPPreset,const quint8 isSpitPPPreset);

    QByteArray sendPPP2ChannelHeanderCommand(const int sampleId,bool isSetAllFalse);


    void outsuckPPPOtherOrder(const DATASAMPLESTRUCT * psampledata,bool &allFinished,QByteArray &outdata);

    bool processPPPCupToChannel(const int sampleId,
                                 const quint8 index,
                                 bool HandsControl,
                                 bool isSuction,
                                 quint32 airvale,
                                 QByteArray &sendData,
                                 bool &kGripperFailed, bool &lateTimer);


    void testPrpGripperErr(const int sampleId,quint8 indexReag,bool &outErr);
    void allTseaReagentActiveFinish(const Testing_reagents *ptestReagent,bool &iscomplete,QByteArray &outdata);
    /** 吸试剂抓PRP到测试通道指令解析并继续发送
     * @brief recv_suckReagentClipTube
     * @param sampleId     样本号
     * @param index_code   命令编号
     * @param indexReag    试剂号
     * @param HandsControl 抓手标志
     * @param isSuction    吸杯/放杯
     * @param airvale      负压值
     * @param sendData     继续发送的数据
     * @param outErr       吸杯或者吐杯失败3次处理提示
     * @return
     */
    bool recv_suckReagentClipTube(const int sampleId,
                                    int index_code,
                                    quint8 indexReag,
                                    bool HandsControl,
                                    bool isSuction,
                                    quint32 airvale,
                                    QByteArray &sendData,
                                    bool &outErr, bool &laterTimer);


    /** 吸试剂抓PRP到测试通道指令发送首个
     * @brief sendsuckReagentCatchCuptoTestChannelHead
     * @param sampleid
     * @param reagentType
     * @return
     */
    QByteArray sendsuckReagentCatchCuptoTestChannelHead(const int sampleid,quint8 reagentType);

    void changesuckReagentFirstCommandLocation(const int sampleid,quint8 reagentType);




    /********* 向通道吐试剂**************/
    void send_spitReagentData(const int SampleId, quint8 index_reagent, QByteArrayList &senddata); //根--吐试剂到测试通道
    bool recv_spitReagentData(const int SampleId, quint8 index_reagent, const int index_num); //收到吐试剂



    /*****************丢测试试剂杯子动作*******************/
    QByteArray sendCommandThrowCupFirst(const int SampleID,quint8 finishReag,  bool isreplaceFlag);

    void throwPRPFailed(const int sampleID,quint8 indexReag,bool &outErr);
    void allThrowPRPFinish(const Testing_reagents *preagdata,bool &isthrocupfinish,QByteArray &outdata);
    bool recvThrowCupPara(const int sampleID,
                            const int commandIndex,
                            quint8 indexReag,
                            bool HandsControl,
                            bool isSuction,
                            quint32 airvale,
                            QByteArray &sendData,
                            bool &outErr, bool &laterTimer);




    bool _boolThrowCupFinished(const int _sampleid,quint8 index_reag, Testing_reagents *ptestReag);


    /********************清洗双针********************************/
     void sendCleanDoubleNeedle(const int sampleid, QByteArrayList &senddata);
     bool recv_cleanDobulePin(const int index_num, int loseFocusSample); //清洗双针收
     bool isDoublePinCleaningFinished(DATASAMPLESTRUCT *pclean_double,int loseFocusSample);


    /******************** 清洗血样针 *********************************/
    void creat_cleanBloodyPin(bool bcleanDoublePin, QByteArrayList allcommand, DATASAMPLESTRUCT *&psampleinfo);
    void send_CleanbloodyPinActive(const int sampleID, QByteArrayList &senddata);  //清洗开始
    bool recv_cleanbloodPin(const int index_num, int loseFocusSample); //清洗血样


    /****清洗试剂针********/
    void send_cleanReagPin(const int sampleid, const quint8 index_reagent, QByteArrayList &senddata); //洗试剂针
    bool recv_cleanReagentPin(const int index_num, quint8 index_reag, int loseFocusSample); //清洗试剂针


public:

    /** 抓PRP到测试通道失败重置抓杯命令
     * @brief resettheCupDropAnimation
     * @param sampleId
     * @param indexReag
     * @param isSendSuckCup false:重置false true:发首个抓杯
     * @return
     */
    QByteArray resettheCupDropAnimation(
                        const int sampleId, const quint8 indexReag,
                        const std::atomic<bool>& ispendingtime, const bool &isSendSuckCup);





private:
    explicit StructInstance(QObject *parent = 0);//构造函数
    StructInstance(const StructInstance &,QObject *parent = 0): QObject(parent) {}//拷贝构造函数
    StructInstance& operator =(const StructInstance&){return *this;}//赋值操作符重写
    static StructInstance* m_pInstance;//定义单例指针
    static QMutex m_mutex;//互斥锁

	mutable std::mutex m_reagentMutex;

    void backorigintimes();
    int m_handsFailedtimes = 0; //抓手吸杯失败次数
    int m_handsThrowFailedtimes = 0; //抓手丢被失败次数


    inline void logDebug(const QString& msg) { QLOG_DEBUG() << "[PPP]" << msg; }
    inline void logError(const QString& msg) { QLOG_ERROR() << "[PPP]" << msg; }

public:
    class Garbo     //专门用来析构m_pInstance指针的类
    {
        public:
            ~Garbo()
            {
                if(m_pInstance != NULL)
                {
					m_pInstance->delalltaskinfo(true);
                    delete m_pInstance;
                    m_pInstance = NULL;
                    QLOG_DEBUG()<<"["<<__FILE__<<"]"<<__LINE__<<__FUNCTION__<<"m_pInstance 被析构";
                }
            }
    };
    static Garbo m_garbo;
};
#endif // STRUCTINSTANCE_H

