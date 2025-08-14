#include "structinstance.h"
#include "testprojectprocess.h"
#include  "quiutils.h"
#include "loginui.h"
#include "globaldata.h"
#include <cstdint>
#include <QSemaphore>
#include <QThread>
#include <memory>
#include <array>    // 若使用 std::array
#include <cstdint>  // quint8 类型支持
#include <operclass/fullyautomatedplatelets.h>
#include <testing.h>

//#define POLLING_TIMER  800

// 定义静态成员变量
std::mutex TestProjectProcess::s_actionMutex;

const QHash<int, QString> TestProjectProcess::s_cleanActionMap = {
    {WASH_BLLODSAMPLE_NEDDLES, "清洗血样针"},
    {WASH_REAGENT_NEDDLES,     "清洗试剂针"},
    {WASH_DOUBLE_NEDDLES,      "清洗血样针和试剂针"},
    {THROW_CUPS_END,           "丟杯"}
};

const QHash<quint8, quint8> TestProjectProcess::s_reagentFocusMap = {
    {AA_REAGENT,  FOCUS_THROW_AA_TUBE},
    {ADP_REAGENT, FOCUS_THROW_ADP_TUBE},
    {EPI_REAGENT, FOCUS_THROW_EPI_TUBE},
    {COL_REAGENT, FOCUS_THROW_COL_TUBE},
    {RIS_REAGENT, FOCUS_THROW_RIS_TUBE}
};

TestProjectProcess::TestProjectProcess(QObject *parent, quint8 equipmentIndex)
    : QObject(parent),retryCount(0),mdataAArry{}, m_timer(nullptr), m_targetThread(nullptr),
      m_Errorresetsfirst(false),mTestErrSampleid(0)
{
    m_finishTestData.clear();
    m_pendingtimeSample.clear();
    mbackOriginnumList.clear();
    m_totalChannel = (equipmentIndex + 1)*4;
}

TestProjectProcess::~TestProjectProcess()
{
    QLOG_DEBUG()<<"stop"<<__FILE__<<endl;
//    if (m_timer) {
//        m_timer->stop();
//		delete m_timer;
//		m_timer = nullptr;
//    }
}
void TestProjectProcess::setTargetThread(QThread* thread) {
    m_targetThread = thread;
}

void TestProjectProcess::initTimer() {

//    Q_ASSERT(QThread::currentThread() == m_targetThread);
//    m_timer = new QTimer(nullptr);
//    m_timer->moveToThread(m_targetThread);
//    connect(m_timer, &QTimer::timeout, this, &TestProjectProcess::handleTimeout);
//    m_timer->setInterval(1000);

}

void TestProjectProcess::handleTimeout() {
//    QLOG_DEBUG() << "Timeout handled in thread:" << QThread::currentThreadId();
//    if (retryCount < 3) {
//          retryCount++;
//          // 重发指令
//          writeCommandHeader(mdataAArry ,"重发指令",DELAY_TIMENUM);
//		  QLOG_ERROR() << "重发指令:" << mdataAArry.toHex(' ').trimmed().toUpper();
//          // 重启定时器
//          m_timer->start();
//      } else {
//          promptsareDisplayed(ERRORLOG,"No response after 3 retries",true);
//		  QLOG_ERROR() << "超过3次未接收到指令";
//          m_timer->stop();
//          retryCount = 0;   // 重置计数器
//      }
}


void TestProjectProcess::promptsareDisplayed(const quint8 proIndex,const QString tips,const bool outwidgetTips){
    emit reminderTextOut(proIndex,tips);
    if(outwidgetTips){
         emit executeErrPrompt(tips);
    }
}


void TestProjectProcess::SampleReagTestFinish(QString finishid,const quint8 index_Chn,const quint8 indexReage)
{

    QString idDate;
    int completeSample = 0;
    quint8 focus_index_set = 0; //丟试剂杯动作
    GlobalData::apartSampleId(finishid,idDate,completeSample);


    Finish_Data_ kFinishChannel;
    kFinishChannel.CompleteSampleid = completeSample;
    kFinishChannel.index_Chn = index_Chn;
    kFinishChannel.index_reagent = indexReage;
    m_finishTestData.enqueue(kFinishChannel);
    QString finishLogs = QUIUtils::index_reagent_mapping_reagentName("试剂", indexReage);
    QLOG_DEBUG()<<"样本"<<completeSample <<finishLogs<<"在通道"<< index_Chn<<"测试完成入队"<<endl;

    //通道测试完成如果有样本在加样就入队
    const bool sampleFocusActive = StructInstance::getInstance()->sampleHadFocus();

    if(!sampleFocusActive) {

        StructInstance::getInstance()->config_focusSample(completeSample,true);//++焦点
        m_focus_sample_id = completeSample;

        switch(indexReage)
        {
            case AA_REAGENT:  focus_index_set = FOCUS_THROW_AA_TUBE; break;
            case ADP_REAGENT: focus_index_set = FOCUS_THROW_ADP_TUBE;break;
            case EPI_REAGENT: focus_index_set = FOCUS_THROW_EPI_TUBE;break;
            case COL_REAGENT: focus_index_set = FOCUS_THROW_COL_TUBE;break;
            case RIS_REAGENT: focus_index_set = FOCUS_THROW_RIS_TUBE;break;
        default:
            QLOG_ERROR()<<"未找到完成试剂号"<<"行号"<<__LINE__<<endl;
            break;
        }

        StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
        auto it = m_finishTestData.begin();
        while (it != m_finishTestData.end()) {
            if (it->index_Chn == index_Chn && it->index_reagent == indexReage
                    && it->CompleteSampleid == completeSample) {
                it = m_finishTestData.erase(it);
            } else {
                ++it;  // 未匹配则继续遍历
            }
        }

        emit setprogressinitboolwaittext(true,index_Chn);
        outputThrowTestChannelReagentFinish(m_focus_sample_id,indexReage);
    }
    return;
}

void TestProjectProcess::handleStartTheTestTask()
{
   StartSampleAdditionWorkflow();//开始测试任务
}

void TestProjectProcess::slottestingaddsample()
{
    if(!StructInstance::getInstance()->sampleHadFocus())
    {
       StartSampleAdditionWorkflow();
       promptsareDisplayed(TESTINGLOG,"测试中添加样本&&无焦点运动样本",false);
       QLOG_DEBUG()<<"测试中添加样本&&无焦点运动样本++样"<<endl;
    }
    else
    {
       promptsareDisplayed(TESTINGLOG,"测试中添加样本&&有焦点运动等待",false);
       QLOG_DEBUG()<<"测试中添加样本&&有焦点运动等待"<<endl;
    }
    return;
}


void TestProjectProcess::remberSendPara(const QByteArray& outdata, Commandinfo& remember_data)
{
    // 添加长度校验防止越界访问
    if (outdata.size() < PROTOCOL_LENGTH) {
        QLOG_WARN() << "Invalid outdata length:" << outdata.size();
        return;
    }

    // 闭包函数提取重复逻辑
    auto extractByte = [&outdata](int position) -> quint8 {
        return static_cast<quint8>(outdata.at(position)); // 直接取二进制值
    };

    // 一次性提取所有字段
    remember_data.addr    = extractByte(SLAVE_ADDRESS);
    remember_data.func    = extractByte(FUNCTIONALMA);
    remember_data.numindex = extractByte(COMMANDNUMBER);

    // 优化日志输出（避免刷新流）
    QLOG_DEBUG() << "send==>从机地址[" << remember_data.addr
                 << "]功能码[" << remember_data.func
                 << "]命令编号[" << remember_data.numindex << "]";

    //m_timer->start(); // 启动定时
}


bool TestProjectProcess::comparedSendPara(const Commandinfo &remember_data, quint8 addr, quint8 func,  quint8 num){
    return (remember_data.addr == addr)
            && (remember_data.func == func)
            && (remember_data.numindex == num);
}



void TestProjectProcess::writeCommandHeader(const QByteArray data,const QString commandText,int latetime) {

    if(data.isEmpty()){
        QString outerr = QString("指令%1为NULL,异常结束").arg(commandText);
        QLOG_ERROR()<<outerr<<endl;
        promptsareDisplayed(TESTINGLOG,outerr,false);
        return;
    }

    if(cglobal::g_PauseTesting && StructInstance::getInstance()->sampleHadFocus())
    {
        mpauseData = data;
        QLOG_DEBUG()<<"A2B2A循环发送指令暂停指令数"<<mpauseData.size()<<endl;
        return;
    }

    QTimer::singleShot(latetime, this, [this, data, commandText]() {
         emit WriteaSingleCommand(data,commandText);
    });

    mdataAArry = data;

    //QLOG_ERROR() << "发指令:" << mdataAArry.toHex(' ').trimmed().toUpper();
    //remberSendPara(data,mcontrastCommand);
}

void TestProjectProcess::missingReagentResetPause(){
    //先所有轴复位
    m_Errorresetsfirst = true;
    mbackOriginnumList.clear();
    QByteArrayList allAxisBackOrigin = GlobalData::letallAxisBackOrigin(mbackOriginnumList);
    sendInstructionsWithDelay(allAxisBackOrigin,DELAY_TIMENUM, "缺少试剂复位暂停");
    return;
}

void TestProjectProcess::sendInstructionsWithDelay(const QByteArrayList data_,int late_time,
                                                     const QString instructioninfo)
{
    if(cglobal::g_PauseTesting && StructInstance::getInstance()->sampleHadFocus())
    {
        m_pause_the_caching_instruction = data_;
        QLOG_DEBUG()<<"有样本焦点任务&&暂停指令数"<<m_pause_the_caching_instruction.size()<<endl;
        return;
    }

    QTimer::singleShot(late_time, this, [this, data_, instructioninfo]() {
        if(!data_.isEmpty())
        {
            emit write_instructions_group(data_,instructioninfo);
            promptsareDisplayed(TESTINGLOG,instructioninfo,false);
        }
    });
    return;
}




void TestProjectProcess::loadPedningtimeTask(){

    if(m_pendingtimeSample.size() >= m_totalChannel){
        QLOG_ERROR()<<"挂起样本数大于通道数"<<"启动失败";
        return;
    }


    auto& structInstance = *StructInstance::getInstance();
    quint8 activeIndex = 0;
    for(int sampleId : m_pendingtimeSample){
         const bool hadPendtime = structInstance.continueTestPendTimeSample(activeIndex,sampleId);
         if(!hadPendtime) {
             QLOG_ERROR()<<"挂起样本"<<sampleId<<"启动失败";
             continue;
         }
         const bool hadFocus = structInstance.sampleHadFocus();
         if(!hadFocus && m_finishTestData.isEmpty()){
             m_pendingtimeSample.removeOne(sampleId);
             structInstance.setSamplePendingtimeStatus(sampleId,false);
             structInstance.config_focusSample(sampleId ,true);
             m_focus_sample_id = sampleId;
             testSampleTryAgain(activeIndex,true);
             return;
         }
    }
}

void TestProjectProcess::handleNoPendingSamples()
{
    const QString message = tr("所有样本已完成加样");
    QLOG_INFO() << "[SampleMgr]" << message;
    promptsareDisplayed(TESTINGLOG, message,false);


    //处理挂起样本
    loadPedningtimeTask();
}

// 分解子功能为独立方法
void TestProjectProcess::executeSampleAddition(int sampleId, StructInstance& instance)
{
    // 原子化状态更新
    instance.config_focusSample(sampleId, true);
    instance.write_focusSampleActive(sampleId, FOCUS_ADDING_SAMPLE);
    instance.updateSampleState(sampleId, ADDING_SAMPLE);

    const QByteArray  outdata =  instance.addSampleHeandCommand(sampleId);
    writeCommandHeader(outdata,tr("样本%1开始加样流程").arg(sampleId),DELAY_TIMENUM);
}


void TestProjectProcess::StartSampleAdditionWorkflow()
{
    // 获取单例实例并缓存
    auto& structInstance = *StructInstance::getInstance();

    const int sampleOpt  = structInstance.OutputPrepareAspirationNum();
    m_focus_sample_id = sampleOpt;

    if(sampleOpt <= 0){
        handleNoPendingSamples();
        return;
    }

    executeSampleAddition(sampleOpt,structInstance);
}


void TestProjectProcess::testSampleTryAgain(const quint8 indexActive,const std::atomic<bool>& ispendtime)
{
    auto* structInstance = StructInstance::getInstance();



    // 统一配置表：使用结构体封装处理逻辑
    struct ActionConfig {
        std::function<QByteArray(bool)> handler; // 使用lambda封装不同方法调用
        const char* description;
    };

    // 延迟初始化配置表
    QByteArray outdata;
    QString dataexplain;

    static const std::unordered_map<quint8,ActionConfig> configMap = {
        {FOCUS_TESTING_AA,  {[=](bool is_pending){
        // 直接传递ispendtime + 类型安全检查
        return structInstance->resettheCupDropAnimation(
            m_focus_sample_id, AA_REAGENT, is_pending,true);
        },"重试抓PRP(AA)"}},

        {FOCUS_TESTING_ADP, {[=](bool is_pending){
        return structInstance->resettheCupDropAnimation(
                m_focus_sample_id, ADP_REAGENT, is_pending,true);
        },"重试抓PRP(ADP)"}},

        {FOCUS_TESTING_EPI, {[=](bool is_pending){
        return structInstance->resettheCupDropAnimation(
            m_focus_sample_id, EPI_REAGENT, is_pending,true);
        },"重试抓PRP(EPI)"}},


        {FOCUS_TESTING_COL, {[=](bool is_pending){
        return structInstance->resettheCupDropAnimation(
            m_focus_sample_id, COL_REAGENT, is_pending,true);
        },"重试抓PRP(COL)"}},

        {FOCUS_TESTING_RIS, {[=](bool is_pending){
        return structInstance->resettheCupDropAnimation(
            m_focus_sample_id, RIS_REAGENT, is_pending,true);
        },"重试抓PRP(RIS)"}},

        // 试杯丢弃逻辑
        {FOCUS_THROW_AA_TUBE,  {[=](bool /*is_pending*/){
		return structInstance->sendCommandThrowCupFirst(
            m_focus_sample_id, AA_REAGENT, GRIPPERMOVE_NORMAL); 
		},"重试丢测试AA试杯"}},

        {FOCUS_THROW_ADP_TUBE, {[=](bool /*is_pending*/){
		return structInstance->sendCommandThrowCupFirst(
            m_focus_sample_id, ADP_REAGENT, GRIPPERMOVE_NORMAL); 
		},"重试丢测试ADP试杯"}},

        {FOCUS_THROW_EPI_TUBE, {[=](bool /*is_pending*/){
		return structInstance->sendCommandThrowCupFirst(
            m_focus_sample_id, EPI_REAGENT, GRIPPERMOVE_NORMAL); 
		},"重试丢测试EPI试杯"}},

        {FOCUS_THROW_COL_TUBE, {[=](bool /*is_pending*/){
		return structInstance->sendCommandThrowCupFirst(
			m_focus_sample_id, COL_REAGENT, GRIPPERMOVE_NORMAL); 
		},"重试丢测试COL试杯"}},

        {FOCUS_THROW_RIS_TUBE, {[=](bool /*is_pending*/){
		return structInstance->sendCommandThrowCupFirst(
			m_focus_sample_id, RIS_REAGENT, GRIPPERMOVE_NORMAL); 
		},"重试丢测试RIS试杯"}},

        // 特殊逻辑
        {FOCUS_THORW_ANEMIA,	{[=](bool /*is_pending*/){
		return structInstance->outputHeadnThrowCups(
			m_focus_sample_id, GRIPPERMOVE_NORMAL); 
		},"重试丟PPP"}},

        {FOCUS_CLIP_ANEMIA_TO_CHN,  {[=](bool /*is_pending*/){
		return structInstance->sendPPP2ChannelHeanderCommand(
			m_focus_sample_id,GRIPPERMOVE_NORMAL); 
		}, "重试吸PPP到测试通道"}}
    };

    // 统一查找逻辑
    std::lock_guard<std::mutex> lock(s_actionMutex);
	// 保证后续操作不重排必须实时读取最新值，改用强内存序约束
	const bool pending_loc = ispendtime.load(std::memory_order_acquire);
    QLOG_DEBUG() << "原子量" << pending_loc;
    std::unordered_map<quint8, ActionConfig>::const_iterator it = configMap.find(indexActive);
    if (it != configMap.cend()) {
        outdata = it->second.handler(pending_loc); // 调用lambda函数
        dataexplain = it->second.description;
    }
    writeCommandHeader(outdata,dataexplain,DELAY_TIMENUM);
    return;
}



void TestProjectProcess::giveupSamplenum(const int sampleid){

     auto* instance = StructInstance::getInstance();

     const auto *itsample = instance->rfindIdSample(sampleid);

     if(itsample == nullptr) {
         QLOG_ERROR()<<"放弃样本"<<sampleid<<"失败";
         return ;
     }
    int idgiveupSample = itsample->indexTestingChn;

     //先把入队的任务踢出
     auto it = m_finishTestData.begin();
     while (it != m_finishTestData.end()) {
         if (it->index_Chn == idgiveupSample && it->CompleteSampleid == sampleid) {
             it = m_finishTestData.erase(it);
         }
         else {
             ++it;
         }
     }

     const quint8  indexFocus = instance->read_focusSampleActive(sampleid);
     quint8 kstopTestChannel = 0;
     instance->sycn_sampleneed_data(sampleid,kstopTestChannel);
     emit openChnMotor(kstopTestChannel, false);//关闭通道旋转

     if(indexFocus == FOCUS_CLIP_ANEMIA_TO_CHN){
         emit setprogressinitboolwaittext(false,kstopTestChannel);
         //通道设置为空闲,清空样本,界面进度条归0
         instance->config_testChn_State(kstopTestChannel,CHN_STATUS_FREE);
         //del样本信息 清空通道数据&&测试试剂=贫血
         instance->_deleteOneSampleTesteddata(kstopTestChannel,sampleid);
     }else{
         //放弃样本闪烁通道进度显示
         FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleChannelFlash(false,kstopTestChannel);
     }

     //失去焦点
     instance->config_dismissFocusSample(sampleid);

     //标注取消样本选用试管
     QList<quint8> giveupHole;
     instance->labelCancelTubes(sampleid,giveupHole);
     FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleShowHole(giveupHole);

     //结构体删除样本
     instance->onetestend_del(m_focus_sample_id);

     int remaining  = 0; //获取剩余样本数
     StructInstance::getInstance()->SampleTotal(remaining );

     if(remaining  == ALL_SAMPLE_TESTED){
         QLOG_DEBUG()<<"放弃样本后,测试列表为空!";
         FullyAutomatedPlatelets::pinstanceTesting()->AllSampleTested();
         instance->delalltaskinfo(false); //删除所有样本信息
         instance->_setemptynull();
     }
     else {
         TraverseChnHadFinishTesting(THROW_CUPS_END);
     }

}

void TestProjectProcess::pendingTimeSample(const int sampleid){
    auto* instance = StructInstance::getInstance();
    auto *itsample = instance->wfindIdSample(sampleid);
    int idgiveupSample = itsample->indexTestingChn;
    if(itsample == nullptr) {
        QLOG_ERROR()<<"放弃样本"<<sampleid<<"失败";
        return ;
    }

    m_pendingtimeSample.push_back(sampleid);
    //先把入队的任务踢出 暂存
    auto it = m_finishTestData.begin();
    while (it != m_finishTestData.end()) {
        if (it->index_Chn == idgiveupSample && it->CompleteSampleid == sampleid) {
            it = m_finishTestData.erase(it);
            break;
        }
        else {
            ++it;
        }
    }

    //失去焦点
    instance->config_dismissFocusSample(sampleid);


    TraverseChnHadFinishTesting(THROW_CUPS_END);


}

void TestProjectProcess::handleGripErrGiveupSample(const bool isGiveupSample,const quint8 indexAxtive){
    auto* instance = StructInstance::getInstance();
    instance->clearRecvPrepatch(m_focus_sample_id);
    if(!isGiveupSample){
        instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_NORMAL);
        testSampleTryAgain(indexAxtive,false);
    }else{
        QByteArrayList cleanData;
        QUIUtils::cleaningDoubleNeedleAction(cleanData,2);
        instance->setSampleGiveupStatus(m_focus_sample_id,SAMPLE_GIVEUP);
        instance->fillRecvPrepatch(m_focus_sample_id,cleanData);
        sendInstructionsWithDelay(cleanData,DELAY_TIMENUM,"抓手异常取消测试任务");
    }
}

//样本任务挂起
void TestProjectProcess::handlePendingtimeoutSampleTest(const quint8 indexAxtive) {
    auto* instance = StructInstance::getInstance();
    instance->clearRecvPrepatch(m_focus_sample_id);


    //挂起后没判断在做什么就清洗双针
    QByteArrayList cleanData;
    QUIUtils::cleaningDoubleNeedleAction(cleanData,2);


    instance->setSamplePendingtimeStatus(m_focus_sample_id,true); //设置挂起状态
    instance->fillRecvPrepatch(m_focus_sample_id,cleanData);
    sendInstructionsWithDelay(cleanData,DELAY_TIMENUM,QString("抓手挂起测试任务动作编号:%1").arg(indexAxtive));
}


bool TestProjectProcess::gripperExceptionHandling(const quint8 commandIndex,const quint8 indexActive ){
    auto* instance = StructInstance::getInstance();
    auto* instrument = FullyAutomatedPlatelets::pinstanceinstrument();
    const bool gripperError = instance->getHandsStatus(m_focus_sample_id);
    if(gripperError == GRIPPERMOVE_NORMAL) return false;

    const bool activeFinished = instance->recvPrepatchActions(m_focus_sample_id,commandIndex);
    if(activeFinished){
        const bool sampleGiveUp = instance->getSampleGiveupStatus(m_focus_sample_id);
        const bool samplePendingtime = instance->getSamplePendingtimeStatus(m_focus_sample_id);
        if(!sampleGiveUp && !samplePendingtime){
            emit theGripperFailed(m_focus_sample_id, m_gripperErrTips,indexActive);//提示下一步操作
            return true;
        }
        else if(sampleGiveUp && !samplePendingtime){
            instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_NORMAL);
            instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);
            giveupSamplenum(m_focus_sample_id);//放弃样本
        }else if(!sampleGiveUp && samplePendingtime){
            //挂起处理
            instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_NORMAL);
            instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);
            pendingTimeSample(m_focus_sample_id);
        }
    }
    return true;
}


//接收解消息
void TestProjectProcess::recvEquipmentdata(const QStringList recvData)
{
    quint8 slaveAddr =   GlobalData::analyzeSlave_addr(recvData,false);//从机地址
    quint8 commandIndex = GlobalData::analyzeCommandIndex(recvData); //命令编号

    if (m_Errorresetsfirst) {
        if (mbackOriginnumList.removeOne(commandIndex) && mbackOriginnumList.isEmpty()) {
            m_Errorresetsfirst = false;
            exceptionRetest("异常断开任务连接测试");
        }
        return;
    }


    //完成状态
    int commandMode, run_status,speedMode ,Action_motor;
    GlobalData::Parse4thBytecommandstatus(recvData,commandMode,run_status,speedMode,Action_motor);

    QString showErrorlog;
    if(!GlobalData::commandStatus(run_status,showErrorlog)){
        QString error_code = recvData.join(" ");
        showErrorlog += error_code;
        QLOG_DEBUG()<<"异常命令:"<<error_code<<showErrorlog<<"暂停"<<endl;
        promptsareDisplayed(TESTINGLOG,showErrorlog,true);
        return;
    }

    const quint8 indexActive = StructInstance::getInstance()->read_focusSampleActive(m_focus_sample_id); 

    if(gripperExceptionHandling(commandIndex,indexActive)){
        return;
    }


    switch(indexActive){
        case FOCUS_ADDING_SAMPLE:{
            //收到加样动作指令
            receiveInstructionsSampleactive(commandIndex,Action_motor,recvData);
            break;
        }
        case FOCUS_CLIP_ANEMIA_TO_CHN:{
            receiveInstructionsPPPCuptoChannel(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
        case FOCUS_THORW_ANEMIA:{
            recv_throwAnemia(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }

       case FOCUS_TESTING_AA:{
            recvNextTestAA(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_ADP:{
            recvNextTestADP(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_EPI:{
            recvNextTestEPI(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_COL:{
            recvNextTestCOL(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
       case FOCUS_TESTING_RIS:{
            recvNextTestRIS(commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }


        case FOCUS_SPIT_AA_TESTING: {
            recv_spitReagent(indexActive,commandIndex,AA_REAGENT);
            break;
        }
        case FOCUS_SPIT_ADP_TESTING: {
            recv_spitReagent(indexActive,commandIndex,ADP_REAGENT);
            break;
        }

        case FOCUS_SPIT_EPI_TESTING: {
            recv_spitReagent(indexActive,commandIndex,EPI_REAGENT);
            break;
        }
        case FOCUS_SPIT_COL_TESTING: {
            recv_spitReagent(indexActive,commandIndex,COL_REAGENT);
            break;
        }
        case FOCUS_SPIT_RIS_TESTING:{
            recv_spitReagent(indexActive,commandIndex,RIS_REAGENT);
            break;
        }
        case FOCUS_CLEAN_DOUBLEPIN:{
            recv_cleanDoublePin(commandIndex);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_AA:{
            recv_cleanReagentPin(commandIndex,AA_REAGENT);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_ADP:{
            recv_cleanReagentPin(commandIndex,ADP_REAGENT);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_EPI:{
            recv_cleanReagentPin(commandIndex,EPI_REAGENT);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_COL:{
            recv_cleanReagentPin(commandIndex,COL_REAGENT);
            break;
        }
        case FOCUS_CLEAN_EREAGENTPIN_RIS:{
            recv_cleanReagentPin(commandIndex,RIS_REAGENT);
            break;
        }
        case FOCUS_CLEAN_BLOODPIN:{
            recv_cleanBloodPin(commandIndex);
            break;
        }

        case FOCUS_THROW_AA_TUBE:
        case FOCUS_THROW_ADP_TUBE:
        case FOCUS_THROW_EPI_TUBE:
        case FOCUS_THROW_COL_TUBE:
        case FOCUS_THROW_RIS_TUBE:
        {
            recv_throwCup(indexActive,commandIndex,slaveAddr,Action_motor,recvData);
            break;
        }
            default:
                break;
        }


}




void TestProjectProcess::getFreeTimeTestChn()
{
    auto* instance = StructInstance::getInstance(); // 单例实例复用[[6]]
    quint8 freeChannel = 0;
    const bool hasFreeChannel = instance->hadFreeTestChn(freeChannel);

    if (hasFreeChannel) {
        // 通道操作模块
        instance->setTestingChn(m_focus_sample_id, freeChannel);
        emit openChnMotor(freeChannel, true); // 信号发射 打开通道
        instance->write_focusSampleActive(m_focus_sample_id, FOCUS_CLIP_ANEMIA_TO_CHN);
        writeCommandHeader(instance->sendPPP2ChannelHeanderCommand(m_focus_sample_id,false),
                    tr("吸PPP试管到测试通道首命令"),DELAY_TIMENUM);
    } else {
        QLOG_DEBUG()<<"["<<__FILE__<<"]"<<__LINE__<<__FUNCTION__<<"加样后无测试通道==>清洗血样针";
        instance->write_focusSampleActive(m_focus_sample_id,FOCUS_CLEAN_BLOODPIN);
        QByteArrayList write_data_;
        instance->send_CleanbloodyPinActive(m_focus_sample_id,write_data_);
        sendInstructionsWithDelay(write_data_,DELAY_TIMENUM,"洗血样针");
        FullyAutomatedPlatelets::pinstanceinstrument()->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,1);
    }
    return;
}






void TestProjectProcess::throwAnemiaCup(quint8 testChn)
{
    StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, FOCUS_THORW_ANEMIA);
    QLOG_DEBUG()<<"通道"<<testChn + 1<<"丟PPP试杯"<<endl;


    QByteArray sendHeandArry = StructInstance::getInstance()->outputHeadnThrowCups(m_focus_sample_id,false);
    writeCommandHeader(sendHeandArry,tr("弃杯动作发送首命令"),DELAY_TIMENUM);
    return;
}


void TestProjectProcess::FindSuckReagentAxis(QPoint moveingloc, const int &indexSuckReagent)
{
	static const int kReagentIndices[] = {
        INDEX_AA_CONSUMABLE,  INDEX_ADP_CONSUMABLE, INDEX_EPI_CONSUMABLE,
        INDEX_COL_CONSUMABLE, INDEX_RIS_CONSUMABLE, INDEX_AA_1_CONSUMABLE,
        INDEX_ADP_1_CONSUMABLE, INDEX_EPI_1_CONSUMABLE,
        INDEX_COL_1_CONSUMABLE, INDEX_RIS_1_CONSUMABLE
    };
	constexpr int MAX_REAGENT_INDEX = sizeof(kReagentIndices) / sizeof(kReagentIndices[0]) - 1;

	// 获取试剂索引
	quint8 testingReag = SingletonAxis::GetInstance()->outPutLossReagentIndex(moveingloc);

	// 增强日志输出
	const QString logPrefix = QStringLiteral("[FindSuckReagentAxis]");
	if (testingReag > MAX_REAGENT_INDEX) {
		QLOG_ERROR() << logPrefix << "无效试剂索引:" << testingReag
            << "，最大允许值:" << MAX_REAGENT_INDEX<<"赋值"<<indexSuckReagent;
        testingReag = indexSuckReagent - 1;
        //return;
	}
	// 单例安全访问
	if (auto* instrument = FullyAutomatedPlatelets::pinstanceinstrument()) {
		instrument->LosserOneReagentul(kReagentIndices[testingReag], 0);
		QLOG_DEBUG() << logPrefix << "成功发送试剂指令，索引:" << testingReag;
	}
	else {
		QLOG_ERROR() << logPrefix << "仪器单例实例未初始化!";
	}
}


bool TestProjectProcess::controlsuckReagentLoc(quint8 primaryReagentIndex,quint8 testingReagent)
{
    // 配置缓存优化（类初始化时加载）
    const bool useAlternateReagent = INI_File().rConfigPara(USINGASIDEREAGENTLOC).toBool();

    // 试剂仓映射表（可扩展设计）
    static const QMap<quint8, quint8> REAGENT_MAPPING {
        {INDEX_AA_CONSUMABLE,  INDEX_AA_1_CONSUMABLE},
        {INDEX_ADP_CONSUMABLE, INDEX_ADP_1_CONSUMABLE},
        {INDEX_EPI_CONSUMABLE, INDEX_EPI_1_CONSUMABLE},
        {INDEX_COL_CONSUMABLE, INDEX_COL_1_CONSUMABLE},
        {INDEX_RIS_CONSUMABLE, INDEX_RIS_1_CONSUMABLE}
    };

    // 安全获取备用仓索引
    quint8 alternateReagentIndex = REAGENT_MAPPING.value(primaryReagentIndex, INDEX_AA_1_CONSUMABLE);
    Q_ASSERT_X(alternateReagentIndex != INDEX_AA_1_CONSUMABLE, "Reagent Control",
                  QString("Invalid primary reagent index: %1").arg(primaryReagentIndex).toUtf8());

    // 统一余量检测接口
    auto checkReagentSufficiency = [](quint8 reagentIndex, quint8 required) {
        return GlobalData::determineReagentontheleftIsSufficient(reagentIndex, required);
    };

    const bool isPrimarySufficient = checkReagentSufficiency(primaryReagentIndex, testingReagent);
    const bool isAlternateSufficient = checkReagentSufficiency(alternateReagentIndex, testingReagent);

    // 决策矩阵处理
    if(useAlternateReagent) {
       bool  EnoughsSuppile = handleAlternateReagentStrategy(primaryReagentIndex, testingReagent,
                                      isPrimarySufficient, isAlternateSufficient);
       return EnoughsSuppile;
    } else {
        //默认左边试剂位置 -先复位-再提示
        bool isEnoughSuppile =  handlePrimaryOnlyStrategy(isPrimarySufficient, primaryReagentIndex);
        return isEnoughSuppile;
    }
    return true;
}

bool  TestProjectProcess::handlePrimaryOnlyStrategy(bool isPrimarySufficient,const quint8& index){
    if(!isPrimarySufficient){
        missingReagentResetPause();
        emit pauseSycnMainuiUpdate(index);
        return false;
    }
    return true;
}

// 分离策略处理函数
bool TestProjectProcess::handleAlternateReagentStrategy(quint8 primary,
                                                        quint8 alternate,
                                                        bool isPrimarySufficient,
                                                        bool isAlternateSufficient)
{
    if(!isPrimarySufficient && !isAlternateSufficient) {
        triggerConsumablesPause(primary, alternate);
        QLOG_WARN()<<"试剂和预留位置剩余试剂不足"<<primary;
        return false;
    } else if(!isPrimarySufficient) {//左边试剂不足 启用右边
       StructInstance::getInstance()->changesuckReagentFirstCommandLocation(m_focus_sample_id,alternate);
    }
    return true;
}

// 统一暂停触发机制
void TestProjectProcess::triggerConsumablesPause(quint8 primary, quint8 alternate)
{
    missingReagentResetPause();
    emit pauseSycnMainuiUpdate(primary);
}

bool TestProjectProcess::isHandsControl(int slaveAddr, int Action_motor) const {
    return (slaveAddr == Z_AXIS && Action_motor == MOTOR_HANDS_INDEX);
}
bool TestProjectProcess::isMainControlMotorXY(int slaveAddr, int Action_motor) const {
    return (slaveAddr == MAIN_CONTROL && Action_motor == MOTOR_XY);
}

const QMap<quint8, TestProjectProcess::FocusReagentPair> TestProjectProcess::FOCUS_MAPPING = []{
    QMap<quint8, FocusReagentPair> map;  // C++11 Lambda 初始化静态常量
    map.insert(FOCUS_TESTING_AA,  qMakePair(FOCUS_SPIT_AA_TESTING,  AA_REAGENT));
    map.insert(FOCUS_TESTING_ADP, qMakePair(FOCUS_SPIT_ADP_TESTING, ADP_REAGENT));
    map.insert(FOCUS_TESTING_EPI, qMakePair(FOCUS_SPIT_EPI_TESTING, EPI_REAGENT));
    map.insert(FOCUS_TESTING_COL, qMakePair(FOCUS_SPIT_COL_TESTING, COL_REAGENT));
    map.insert(FOCUS_TESTING_RIS, qMakePair(FOCUS_SPIT_RIS_TESTING, RIS_REAGENT));
    return map;
}();

void TestProjectProcess::slotSpitReagentTesting()
{
    StructInstance* instance = StructInstance::getInstance();  // 获取单例
    const quint8 currentFocus = instance->read_focusSampleActive(m_focus_sample_id);

    // 查找状态映射
    auto it = FOCUS_MAPPING.find(currentFocus);
    if (it == FOCUS_MAPPING.constEnd()) {
        QLOG_DEBUG() << "[吐试剂到测试通道] Invalid focus index:" << currentFocus
                  << "Sample ID:" << m_focus_sample_id;
        return;
    }

    const quint8 newFocus = it.value().first;
    const quint8 reagentType = it.value().second;

    instance->write_focusSampleActive(m_focus_sample_id, newFocus);


    // 生成指令数据
    QByteArrayList writeData;
    instance->send_spitReagentData(m_focus_sample_id, reagentType, writeData);

    if (writeData.isEmpty()) {
        qCritical() << "[吐试剂到测试通道] Empty command for reagent:" << reagentType;
        return;
    }

    sendInstructionsWithDelay(writeData,
                      DELAY_TIMENUM,
                      tr("吐试剂到测试通道"));
    return;
}

void TestProjectProcess::recv_spitReagent(quint8 indexActive,const int index_num,quint8 Dischargethetypeofreagent)
{
    bool finished_spitreagent = StructInstance::getInstance()->recv_spitReagentData(m_focus_sample_id,
                                                                                    Dischargethetypeofreagent,
                                                                                    index_num);
    if(finished_spitreagent == true)
    {
        //吐试剂完成==>打开通道开始测试-->清洗针
        quint8 indexreagent,Consumablesindex;
        GlobalData::outputtheAbsorbentIndex(indexActive,indexreagent,Consumablesindex);
        QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"吐试剂完成"<<indexreagent<<endl;
        emit openTestChnTest(m_focus_sample_id,indexActive,indexreagent);
    }
    return;
}


void TestProjectProcess::slotaddSampmeTestCleanPin(const int cleanPin ,const quint8 index_reagent)
{
    quint8 focus_index_set = 0;
    QString outtext = "";
    auto* instrument = FullyAutomatedPlatelets::pinstanceinstrument();
    QByteArrayList send_data_;
    if(cleanPin == WASH_DOUBLE_NEDDLES)
    {
        focus_index_set = FOCUS_CLEAN_DOUBLEPIN;
        StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
        StructInstance::getInstance()->sendCleanDoubleNeedle(m_focus_sample_id,send_data_);
        outtext ="清洗双针";
        instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,2);
    }
    else if(cleanPin == WASH_REAGENT_NEDDLES)
    {
        switch(index_reagent)
        {
            case AA_REAGENT:  focus_index_set = FOCUS_CLEAN_EREAGENTPIN_AA; break;
            case ADP_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_ADP; break;
            case EPI_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_EPI; break;
            case COL_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_COL; break;
            case RIS_REAGENT: focus_index_set = FOCUS_CLEAN_EREAGENTPIN_RIS; break;
            default:break;
        }
        QLOG_DEBUG()<<"样本(清洗试剂针):"<<m_focus_sample_id<<GlobalData::mapIndexReagentnames(index_reagent);
        StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
        StructInstance::getInstance()->send_cleanReagPin(m_focus_sample_id,index_reagent, send_data_);
        outtext = "清洗试剂针";
        instrument->LosserOneReagentul(INDEX_CLEANLINQUE_CONSUMABLE,0);
    }
    sendInstructionsWithDelay(send_data_,DELAY_TIMENUM,outtext);
    return;
}


void TestProjectProcess::recv_cleanDoublePin(const int index_num)
{
    bool finished_clean = StructInstance::getInstance()->recv_cleanDobulePin(index_num,
                                                                             m_focus_sample_id);
    if(finished_clean)
    {
        StructInstance::getInstance()->config_dismissFocusSample(m_focus_sample_id); //失去焦点
        //判断是否有样本测试完成
        TraverseChnHadFinishTesting(WASH_DOUBLE_NEDDLES);
    }
    return;
}

void TestProjectProcess::recv_cleanReagentPin(const int index_num,const quint8 index_reag)
{
    bool finished_clean = StructInstance::getInstance()->recv_cleanReagentPin(index_num,index_reag,m_focus_sample_id);
    if(finished_clean)
    {
        StructInstance::getInstance()->config_dismissFocusSample(m_focus_sample_id); //失去焦点
        TraverseChnHadFinishTesting(WASH_REAGENT_NEDDLES);
    }
    return;
}

void TestProjectProcess::recv_cleanBloodPin(const int index_num)
{
    bool finished_clean = StructInstance::getInstance()->recv_cleanbloodPin(index_num,m_focus_sample_id);
    if(finished_clean)
    {
        StructInstance::getInstance()->config_dismissFocusSample(m_focus_sample_id); //失去焦点
        TraverseChnHadFinishTesting(WASH_BLLODSAMPLE_NEDDLES);
    }
    return;
}


// 处理完成通道任务
void TestProjectProcess::processFinishedChannelTask()
{
    const Finish_Data_ task = m_finishTestData.dequeue();

    // 调试日志输出
    QLOG_DEBUG() << "出队任务通道 " << task.index_Chn
                          << " | " << QUIUtils::index_reagent_mapping_reagentName("试剂", task.index_reagent);

    // 获取样本信息
    QString sampleName;
    int sampleId = 0;
    StructInstance::getInstance()->rootTestChnGetSampleInfo(task.index_Chn, sampleName, sampleId);

    // 设置焦点样本
    setupFocusSample(sampleId, task.index_reagent);

    // 发送控制命令
    sendControlCommands(task.index_Chn, sampleId, task.index_reagent);
}

// 设置焦点样本
void TestProjectProcess::setupFocusSample(int sampleId, quint8 reagentType)
{
    StructInstance::getInstance()->config_focusSample(sampleId, true);
    m_focus_sample_id = sampleId;

    // 获取焦点类型
    const quint8 focusType = s_reagentFocusMap.value(reagentType, 0);
    //Q_ASSERT_X(focusType != FOCUS_UNKNOWN, Q_FUNC_INFO, "无效试剂类型");

    // 写入焦点状态
    StructInstance::getInstance()->write_focusSampleActive(sampleId, focusType);
}

// 发送控制命令
void TestProjectProcess::sendControlCommands(quint8 chn, int sampleId, quint8 reagent)
{
    emit setprogressinitboolwaittext(true, chn);

    outputThrowTestChannelReagentFinish(sampleId,reagent);
}

// 处理空闲通道
void TestProjectProcess::handleIdleChannelTasks()
{
    int waitSampleId = -1;
    if (!StructInstance::getInstance()->alreadyadded_wait_chn(waitSampleId)){
        StartSampleAdditionWorkflow();
        return;
    }


    quint8 freeChn = 0;
    if (!StructInstance::getInstance()->hadFreeTestChn(freeChn)) {
        StartSampleAdditionWorkflow();
        return;
    }

    m_focus_sample_id = waitSampleId;
    StructInstance::getInstance()->config_focusSample(waitSampleId, true);
    StructInstance::getInstance()->write_focusSampleActive(
        waitSampleId, FOCUS_CLIP_ANEMIA_TO_CHN);
    StructInstance::getInstance()->setTestingChn(waitSampleId, freeChn);

    emit openChnMotor(freeChn, true);

    const QByteArray cmd = StructInstance::getInstance()->sendPPP2ChannelHeanderCommand(waitSampleId,
                                                                                        GRIPPERMOVE_NORMAL);
    writeCommandHeader(cmd, tr("吸待测PPP试管到测试通道首命令"), DELAY_TIMENUM);
}


//清洗完针 和 样本全部测试完成后调用
void TestProjectProcess::TraverseChnHadFinishTesting(int clean_indexPin)
{
    // 1. 清洗类型描述生成
     const QString& out_text = s_cleanActionMap.value(clean_indexPin, "");
     Q_ASSERT_X(!out_text.isEmpty(), Q_FUNC_INFO, "Invalid clean_indexPin value");
     promptsareDisplayed(TESTINGLOG,out_text,false);

    // 2. 队列任务处理 丢杯完成获取清洗完成 已有样本测试完成入队且无焦点任务存在 执行出队丢杯
    if (!m_finishTestData.isEmpty()) {
        processFinishedChannelTask();
        return; // 处理完成后直接返回
    }

    // 3. 空闲通道处理逻辑
    handleIdleChannelTasks();
}




void TestProjectProcess::recv_throwCup(quint8 indexActive,const int commandIndex,const int slaveAddr,
                                        const int Action_motor,const QStringList &recvdata) {

    static const QMap<quint8, quint8> reagentMapping = {
        {FOCUS_THROW_AA_TUBE,  AA_REAGENT},
        {FOCUS_THROW_ADP_TUBE, ADP_REAGENT},
        {FOCUS_THROW_EPI_TUBE, EPI_REAGENT},
        {FOCUS_THROW_COL_TUBE, COL_REAGENT},
        {FOCUS_THROW_RIS_TUBE, RIS_REAGENT}
    };
    quint8 index_reagent = reagentMapping.value(indexActive, AA_REAGENT);

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
        GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
    }

    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    QByteArray outdata;
    auto* instance = StructInstance::getInstance();
    bool finished_throwcup = instance->recvThrowCupPara(m_focus_sample_id,commandIndex,index_reagent, handsControlActive,
                                                        isSuction, airValue,outdata,kGripperFailed,klaterTimer);

    if(finished_throwcup)
    {
        emit throwtesttube();
        throwcup_finish();
        return;
    }else{
        QString illustrateText  = (isSuction ? "吸(丢测试通道测试杯)3次失败" : "吐(丢测试通道测试杯)杯3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"继续发送丢测试通道测试杯指令";
        commandNotCompleteOrErr(kGripperFailed,outdata,tips,klaterTimer);
   }

}

void TestProjectProcess::throwcup_finish()
{
     //首先看该样本是否还有待测试剂
     quint8 waittest_reag = 0;
     auto* instance = StructInstance::getInstance();
     bool bhand_waittest_reag = instance->judge_hadWaitTestReag(m_focus_sample_id,waittest_reag);
     if(bhand_waittest_reag)
     {
        // 定义试剂类型到参数的映射表
        typedef QPair<quint8, quint8> ReagentParams;
        static const QHash<int, ReagentParams> reagentMapping = []() {
            QHash<int, ReagentParams> map;
            map.insert(AA_REAGENT,  ReagentParams(FOCUS_TESTING_AA,  INDEX_AA_CONSUMABLE));
            map.insert(ADP_REAGENT, ReagentParams(FOCUS_TESTING_ADP, INDEX_ADP_CONSUMABLE));
            map.insert(EPI_REAGENT, ReagentParams(FOCUS_TESTING_EPI, INDEX_EPI_CONSUMABLE));
            map.insert(COL_REAGENT, ReagentParams(FOCUS_TESTING_COL, INDEX_COL_CONSUMABLE));
            map.insert(RIS_REAGENT, ReagentParams(FOCUS_TESTING_RIS, INDEX_RIS_CONSUMABLE));
            return map;
        }();

        // 获取参数
        ReagentParams params = reagentMapping.value(waittest_reag, ReagentParams(0, 255));
        quint8 focus_index = params.first; //动作下标
        quint8 indexreag_wastage = params.second;  //处理试剂--判断是否足够耗材下标
        StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index);
        static const QString  outLog = GlobalData::mapIndexReagentnames(waittest_reag);

        //控制吸试剂位置
        if(!controlsuckReagentLoc(indexreag_wastage,waittest_reag)){
            QLOG_DEBUG()<<"待测试剂"<<outLog<<"不足仪器复位暂停";
            return;
        }

        QString output = QString("样本%1继续吸试剂:%2").arg(m_focus_sample_id).arg(outLog);
        promptsareDisplayed(TESTINGLOG,output,false);
        QLOG_DEBUG()<<output;

        writeCommandHeader(instance->sendsuckReagentCatchCuptoTestChannelHead(
                                m_focus_sample_id,
                                waittest_reag),
                                tr("吸试剂抓测试杯到测试通道通道首命令"),
                                DELAY_TIMENUM);
     }
     else
     {
         const bool HandErr = false;
         theTestChannelStopTesting(HandErr);
     }
     return;
}



/**
 * @brief TestProjectProcess::
 * 1.清洗完针该样本失去 焦点 遍历通道内 测试样本状态
 * 2.再测试中添加了样本 遍历样本库有无样本有焦点
 * 3.样本测试完成，且无测试试剂继续测试的  需要
 */

void TestProjectProcess::equipment_err_continue()
{
    //exceptionRetest("动作异常");
    //先所有轴复位
    m_Errorresetsfirst = true;
    mbackOriginnumList.clear();
    QByteArrayList allAxisBackOrigin = GlobalData::letallAxisBackOrigin(mbackOriginnumList);
    sendInstructionsWithDelay(allAxisBackOrigin,DELAY_TIMENUM, "动作异常");
    return;
}


void TestProjectProcess::updatetubeColorState(const QStringList data)
{
    QPoint movedAxis = QUIUtils::coordinatePositionXYMotion(data);
    quint8 backHole = 250;
    bool bemptyzone = false;
    bool  oper_anemia = StructInstance::getInstance()->MoveAxisMapingHole(movedAxis,backHole,bemptyzone);
    if(bemptyzone == true)
    {
        QLOG_DEBUG()<<"改变空试管颜色标识:"<<backHole<<"吐样"<<__FILE__<<__LINE__<<endl;
        FullyAutomatedPlatelets::pinstanceTesting()->_RecvBloodSuck2EmptyTube(oper_anemia,backHole);
    }
    else
    {
        QLOG_DEBUG()<<"血样针在血样区吸样孔号"<<backHole<<endl;
    }
    return;
}


QPoint TestProjectProcess::updateEmptyClipAway(const QStringList data)
{
    QPoint movedAxis = QUIUtils::coordinatePositionXYMotion(data);
    quint8 backHole = 0;
    if(StructInstance::getInstance()->EmptytubeClipAway(movedAxis,backHole))
    {
        QString outtext = QString::fromUtf8("抓手将试管%1夹走测试").arg(backHole);
        promptsareDisplayed(TESTINGLOG,outtext,false);
        emit EmptyTubeCatched(backHole);
        QLOG_DEBUG() << "试管区夹走试管编号:"<< backHole<<endl;
    }
    return movedAxis;
}


/** 断线重连
* @brief TestProjectProcess::slot_connectEquipmentagin
* 判断有无焦点样本
* 有就先继续做完
* 无就看完成的容器
*/
void TestProjectProcess::slot_connectEquipmentagin()
{
    promptsareDisplayed(TESTINGLOG,"重连继续遍历测试指令",false);

    if(!cglobal::g_StartTesting) return;

    //先所有轴复位
    m_Errorresetsfirst = true;
    mbackOriginnumList.clear();
    QByteArrayList allAxisBackOrigin = GlobalData::letallAxisBackOrigin(mbackOriginnumList);
    sendInstructionsWithDelay(allAxisBackOrigin,DELAY_TIMENUM, "重连继续遍历测试指令");

}





//测试通道停止测试
void TestProjectProcess::theTestChannelStopTesting(const bool handError)
{
    // 获取单例实例一次
    auto* instance = StructInstance::getInstance();

    quint8 kstopTestChannel = 0;
    instance->sycn_sampleneed_data(m_focus_sample_id,kstopTestChannel);

    //关闭通道旋转
    emit openChnMotor(kstopTestChannel, false);

    //设置通道文字提示
    emit setprogressinitboolwaittext(false,kstopTestChannel);

    //失去焦点
    instance->config_dismissFocusSample(m_focus_sample_id);

    //通道设置为空闲--清空样本--界面进度条归0
    instance->config_testChn_State(kstopTestChannel,CHN_STATUS_FREE);

    //del样本信息 清空通道数据&&测试试剂=贫血
    instance->_deleteOneSampleTesteddata(kstopTestChannel,m_focus_sample_id);

    //结构体删除样本
    instance->onetestend_del(m_focus_sample_id);


    QString outtext;
    outtext = (handError)? tr("样本:%1抓手异常取消测试样本,关闭通道%2搅拌电机").arg(m_focus_sample_id)
                         .arg(kstopTestChannel + 1) : tr("样本:%1完成所有试剂测试,关闭通道%2搅拌电机")
                         .arg(m_focus_sample_id).arg(kstopTestChannel + 1);
    promptsareDisplayed(TESTINGLOG,outtext,false);

    // 生成状态报告
    const QString actionDesc = handError ? "抓手异常取消测试样本" : "完成所有试剂测试";
    const QString message = QString("%1%2，关闭通道%3搅拌电机")
                               .arg(handError ? "样本:" : "样本%1")
                               .arg(actionDesc)
                               .arg(kstopTestChannel + 1)
                               .arg(m_focus_sample_id);
    promptsareDisplayed(TESTINGLOG, message,false);



    int remaining  = 0; //获取剩余样本数
    StructInstance::getInstance()->SampleTotal(remaining );

    if(remaining  == ALL_SAMPLE_TESTED){
        FullyAutomatedPlatelets::pinstanceTesting()->AllSampleTested();
        instance->delalltaskinfo(false); //删除所有样本信息
        instance->_setemptynull();
    }
    else {
        TraverseChnHadFinishTesting(THROW_CUPS_END);
    }
    return;
}


/**
 * @brief 恢复测试任务
 */
void TestProjectProcess::handleResumeaTestTask()
{
    // 提前缓存单例指针和焦点状态，减少重复调用
    StructInstance* instance = StructInstance::getInstance();
    const bool hasFocus = instance->sampleHadFocus();

    // 使用别名提高代码可读性
    auto& pauseData = mpauseData;
    auto& cachedInstructions = m_pause_the_caching_instruction;

    // 优先处理有暂停数据的情况
    if (!pauseData.isEmpty() && hasFocus) {
        emit WriteaSingleCommand(pauseData, tr("继续发送指令"));
        return;
    }


    // 定义日志标签常量
    constexpr const char* TAG = "ResumeTask";
    const QString fileInfo = QString("%1:%2").arg(__FILE__).arg(__LINE__);

    // 处理缓存指令逻辑
    if (!cachedInstructions.isEmpty()) {
        if (hasFocus) {
            emit write_instructions_group(cachedInstructions, tr("继续任务指令群"));
            QLOG_DEBUG() << QString("[%1] 恢复缓存指令 | %2").arg(TAG, fileInfo);
        } else {
            QLOG_DEBUG() << QString("[%1] 有缓存指令但无任务焦点 | %2").arg(TAG, fileInfo);
        }
    } else {
        QLOG_DEBUG() << QString("[%1] 无缓存指令 | %2").arg(TAG, fileInfo);
    }
    return;
}


void TestProjectProcess::outputThrowTestChannelReagentFinish(const int finishId,const quint8 finishReagent)
{
    const QByteArray outHeadnArry = StructInstance::getInstance()->sendCommandThrowCupFirst(finishId,
                                                                                      finishReagent,
                                                                                      false);
    writeCommandHeader(outHeadnArry,"丢测试通道内完成试剂杯首索引指令",DELAY_TIMENUM);
}




//收到加样动作指令
void TestProjectProcess::receiveInstructionsSampleactive(int index_num, int axismovement,const QStringList recvdata)
{
    if(axismovement == MOTOR_XY)
        updatetubeColorState(recvdata);

    bool iscompleteAddSample = false;
    QByteArray outdata = StructInstance::getInstance()->recvAddSampleIndexCommand(m_focus_sample_id,
                                                                                  index_num,
                                                                                  iscompleteAddSample);
    if(!iscompleteAddSample){

        writeCommandHeader(outdata,"加样指令",DELAY_TIMENUM);

    }else{
        //设置加样完成状态
        StructInstance::getInstance()->updateSampleState(m_focus_sample_id,
                                                         ADDED_COMPLETE_SAMPLE);
        //加样完成找空闲通道
        getFreeTimeTestChn();
    }
    return;
}



void TestProjectProcess::receiveInstructionsPPPCuptoChannel(const int indexcode,const int slaveAddr,
                                                               const int Action_motor,const QStringList& recvdata)
{
   if(Action_motor == MOTOR_XY)
       updateEmptyClipAway(recvdata);

   quint32 airValue  = 0;
   bool isSuction = false;
   const bool isHandsControl = (slaveAddr == Z_AXIS && Action_motor == MOTOR_HANDS_INDEX);

   if(isHandsControl) {
      QString outputTips = GlobalData::analyzeWhetherGripperSuction(recvdata,isSuction,airValue);
      promptsareDisplayed(TESTINGLOG,outputTips + "(加PPP动作)",false);
   }

   QByteArray sendData;
   sendData.clear();
   bool errorHands = GRIPPERMOVE_NORMAL;
   bool klateTimer = false;
   const bool operationSuccess  = StructInstance::getInstance()->processPPPCupToChannel(m_focus_sample_id,indexcode,
                                                                                       isHandsControl,isSuction,
                                                                                       airValue,sendData,
                                                                                       errorHands,klateTimer);
   if(operationSuccess) {
       QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"PPP吸到测试通道成功,开始延时1S准备读取PPP初值"<<endl;
       const int configDelay  = INI_File()._getdelayedtime();
       QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + configDelay , this, [this]() {
            emit getAnemiaValue();
       });
       return;
   }else{

       QString illustrateText  = (isSuction ? "吸PPP杯3次失败" : "PPP放到测试通道3次失败");
       QString tips = errorHands ?  illustrateText + "复位等待处理" :"移动PPP试管到测试通道";
       commandNotCompleteOrErr(errorHands,sendData,tips,klateTimer);
   }
   return;
}



void TestProjectProcess::commandNotCompleteOrErr(const bool isGripperError,
                                                    const QByteArray senddata,
                                                    const QString tips,
                                                    const bool klatetimer)
{
    if(isGripperError == GRIPPERMOVE_NORMAL && klatetimer){
        int Timelapsesucking = INI_File().rConfigPara(GRIPPERSUCKTIME).toInt();
        QTimer::singleShot(Timelapsesucking , this, [this, senddata, tips]() {
            writeCommandHeader(senddata, tips,DELAY_TIMENUM);
        });
    }else if(isGripperError == GRIPPERMOVE_NORMAL && !klatetimer){
        writeCommandHeader(senddata,tips,DELAY_TIMENUM);
    }
    else {
        auto* instance = StructInstance::getInstance();
        m_gripperErrTips = tips;
        instance->setHandsErrStatus(m_focus_sample_id,GRIPPERMOVE_ABNORMALLY);
        instance->clearRecvPrepatch(m_focus_sample_id);
        const QByteArrayList outBackOriginCommand  = GlobalData::errMachineBackOrigin(FOCUS_ERR_BACKORIGIN,MOTOR_HANDS_INDEX);
        instance->fillRecvPrepatch(m_focus_sample_id,outBackOriginCommand);
        sendInstructionsWithDelay(outBackOriginCommand,DELAY_TIMENUM,tips);
    }
}




//接收丟PPP命令 命令号，从机地址，运动电机，数据
void TestProjectProcess::recv_throwAnemia(const int indexconde,int slaveAddr,int Action_motor,
                                           const QStringList recvdata)
{
    QByteArray sendData;
    sendData.clear();
    quint32 airvale = 0;
    bool isSuction = false;

    const bool isHandsControl = (slaveAddr == Z_AXIS && Action_motor == MOTOR_HANDS_INDEX);
    if(isHandsControl) {
       QString outputTips = GlobalData::analyzeWhetherGripperSuction(recvdata,isSuction,airvale);
       promptsareDisplayed(TESTINGLOG,outputTips + "(丟PPP动作)",false);
    }

    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    auto* instance = StructInstance::getInstance();
    const bool kthrowCupFinish = instance->recv_throwCupcommd(m_focus_sample_id,indexconde,isHandsControl,isSuction,airvale,
                                                              sendData,kGripperFailed,klaterTimer);
    if(kthrowCupFinish){

        //计数杯子
        emit throwtesttube();

        quint8 testingReag = 0;
        instance->reagentTestintotheStructure(m_focus_sample_id,testingReag);

        // 定义试剂类型到参数的映射表
        typedef QPair<quint8, quint8> ReagentParams;
        static const QHash<int, ReagentParams> reagentMapping = []() {
            QHash<int, ReagentParams> map;
            map.insert(AA_REAGENT,  ReagentParams(FOCUS_TESTING_AA,  INDEX_AA_CONSUMABLE));
            map.insert(ADP_REAGENT, ReagentParams(FOCUS_TESTING_ADP, INDEX_ADP_CONSUMABLE));
            map.insert(EPI_REAGENT, ReagentParams(FOCUS_TESTING_EPI, INDEX_EPI_CONSUMABLE));
            map.insert(COL_REAGENT, ReagentParams(FOCUS_TESTING_COL, INDEX_COL_CONSUMABLE));
            map.insert(RIS_REAGENT, ReagentParams(FOCUS_TESTING_RIS, INDEX_RIS_CONSUMABLE));
            return map;
        }();

        // 获取参数
        ReagentParams params = reagentMapping.value(testingReag, ReagentParams(0, 255));
        quint8 focus_index = params.first;  //吸试剂
        quint8 indexreag_wastage = params.second;  //处理试剂--判断是否足够
        instance->write_focusSampleActive(m_focus_sample_id,focus_index);

        controlsuckReagentLoc(indexreag_wastage,testingReag);

        const QByteArray outdata = instance->sendsuckReagentCatchCuptoTestChannelHead(m_focus_sample_id,testingReag);
        writeCommandHeader(outdata,tr("吸试剂抓测试杯到测试通道通道首命令"),DELAY_TIMENUM);
        return;

    }else{
        QString illustrateText  = (isSuction ? "丢PPP杯3次失败" : "PPP丢到弃杯孔3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"丢PPP试管到弃杯孔";
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer);
    }
    return;
}


void TestProjectProcess::recvNextTestAA(const int commandIndex,const int slaveAddr,const int Action_motor,
                                           const QStringList &recvdata){

    const int latencyTime = INI_File()._getdelayedtime();

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
        GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
    }
    else if (isMainControlMotorXY(slaveAddr, Action_motor)) {
        QPoint moveingAxis = updateEmptyClipAway(recvdata);
        if(SingletonAxis::GetInstance()->ismoveXYsuckReagent(moveingAxis)){
            //减少试剂 吸试剂和抓杯到通道的XY 全部处理 要过滤掉不是吸试剂的XY
            FindSuckReagentAxis(moveingAxis,AA_REAGENT);
        }

    }

    QByteArray sendData;
    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    const bool actionCompleted = StructInstance::getInstance()->recv_suckReagentClipTube(
                m_focus_sample_id,commandIndex,AA_REAGENT,handsControlActive,isSuction,
                airValue,sendData,kGripperFailed,klaterTimer
    );

    if (actionCompleted) {

        QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"AA开始延时1S准备读取PrP初值"<<endl;
        QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + latencyTime, this, [this]() {
            emit readbloodyInitValue(AA_REAGENT,FOCUS_TESTING_AA);
        });
        return;

    }else{
        QString illustrateText  = (isSuction ? "吸AA(PRP)杯3次失败" : "吐AA(PRP)杯3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"继续发送吸AA(PRP)到通道测试指令";
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer);
    }
    return;
}


void TestProjectProcess::recvNextTestADP(const int commandIndex,const int slaveAddr,const int Action_motor,
                                          const QStringList &recvdata)
{
    const int latencyTime = INI_File()._getdelayedtime();

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
            GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
    } else if (isMainControlMotorXY(slaveAddr, Action_motor)) {
        QPoint moveingAxis = updateEmptyClipAway(recvdata);

        if(SingletonAxis::GetInstance()->ismoveXYsuckReagent(moveingAxis)){
            //减少试剂
            FindSuckReagentAxis(moveingAxis,ADP_REAGENT);
        }
    }

    QByteArray sendData; // 延迟到实际需要时声明
    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    const bool actionCompleted = StructInstance::getInstance()->recv_suckReagentClipTube(
                m_focus_sample_id,commandIndex,ADP_REAGENT,handsControlActive,
                isSuction,airValue,sendData,kGripperFailed,klaterTimer);

    if (actionCompleted) {
        QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"ADP开始延时1S准备读取PrP初值"<<endl;
        QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + latencyTime, this, [this]() {
            emit readbloodyInitValue(ADP_REAGENT,FOCUS_TESTING_ADP);
        });
        return;
    }else{
        QString illustrateText  = (isSuction ? "吸ADP(PRP)杯3次失败" : "吐ADP(PRP)杯3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"继续发送吸ADP(PRP)到通道测试指令";
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer);
    }
    return;
}


void TestProjectProcess::recvNextTestEPI(const int commandIndex,const int slaveAddr,const int Action_motor,
                                          const QStringList &recvdata)
{

    const int latencyTime = INI_File()._getdelayedtime();

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
            GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
    } else if (isMainControlMotorXY(slaveAddr, Action_motor)) {
        QPoint moveingAxis = updateEmptyClipAway(recvdata);
        if(SingletonAxis::GetInstance()->ismoveXYsuckReagent(moveingAxis)){
            //减少试剂
            FindSuckReagentAxis(moveingAxis,EPI_REAGENT);
        }
    }

    QByteArray sendData; // 延迟到实际需要时声明
    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;

    const bool actionCompleted = StructInstance::getInstance()->recv_suckReagentClipTube(
        m_focus_sample_id,commandIndex,EPI_REAGENT,
        handsControlActive,isSuction,airValue,sendData,kGripperFailed,klaterTimer
    );

    if (actionCompleted) {
        QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"EPI开始延时1S准备读取PrP初值"<<endl;
        QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + latencyTime, this, [this]() {
            emit readbloodyInitValue(EPI_REAGENT,FOCUS_TESTING_EPI);
        });
        return;
    }else{
        QString illustrateText  = (isSuction ? "吸EPI(PRP)杯3次失败" : "吐EPI(PRP)杯3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"继续发送吸EPI(PRP)到通道测试指令";
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer);
    }
    return;
}


void TestProjectProcess::recvNextTestCOL(const int commandIndex,const int slaveAddr,const int Action_motor,
                                         const QStringList &recvdata)
{

    const int latencyTime = INI_File()._getdelayedtime();

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
            GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
    } else if (isMainControlMotorXY(slaveAddr, Action_motor)) {
        QPoint moveingAxis = updateEmptyClipAway(recvdata);
        if(SingletonAxis::GetInstance()->ismoveXYsuckReagent(moveingAxis)){
            //减少试剂
            FindSuckReagentAxis(moveingAxis,COL_REAGENT);
        }
    }

    QByteArray sendData; // 延迟到实际需要时声明
    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    bool klaterTimer = false;
    const bool actionCompleted = StructInstance::getInstance()->recv_suckReagentClipTube(
                m_focus_sample_id,commandIndex, COL_REAGENT,handsControlActive,isSuction,
                airValue,sendData,kGripperFailed,klaterTimer
    );

    if (actionCompleted) {
        QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"COL开始延时1S准备读取PrP初值"<<endl;
        QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + latencyTime, this, [this]() {
            emit readbloodyInitValue(COL_REAGENT,FOCUS_TESTING_COL);
        });
        return;
    }else{
        QString illustrateText  = (isSuction ? "吸COL(PRP)杯3次失败" : "吐COL(PRP)杯3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"继续发送吸COL(PRP)到通道测试指令";
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer);
   }
   return;
}


void TestProjectProcess::recvNextTestRIS(const int commandIndex,const int slaveAddr,const int Action_motor,
                                           const QStringList &recvdata){

    const int latencyTime = INI_File()._getdelayedtime();

    bool isSuction = false;
    quint32 airValue = 0;
    const bool handsControlActive = isHandsControl(slaveAddr, Action_motor);

    if (handsControlActive) {
            GlobalData::analyzeWhetherGripperSuction(recvdata, isSuction, airValue);
    } else if (isMainControlMotorXY(slaveAddr, Action_motor)) {
        QPoint moveingAxis = updateEmptyClipAway(recvdata);
        if(SingletonAxis::GetInstance()->ismoveXYsuckReagent(moveingAxis)){
            //减少试剂
            FindSuckReagentAxis(moveingAxis,RIS_REAGENT);
        }
    }

    QByteArray sendData; // 延迟到实际需要时声明
    bool kGripperFailed = GRIPPERMOVE_NORMAL;
    auto* instance = StructInstance::getInstance();
    bool klaterTimer = false;
    const bool actionCompleted = instance->recv_suckReagentClipTube(
                m_focus_sample_id,commandIndex,RIS_REAGENT,handsControlActive,isSuction,
                airValue,sendData,kGripperFailed,klaterTimer
    );

    if (actionCompleted) {
        QLOG_DEBUG()<<"样本"<<m_focus_sample_id<<"RIS开始延时1S准备读取PrP初值"<<endl;
        QTimer::singleShot(DELAY_READ_TUBE_INITVALUE + latencyTime, this, [this]() {
             emit readbloodyInitValue(RIS_REAGENT,FOCUS_TESTING_RIS);
        });
        return;
    }else{
        QString illustrateText  = (isSuction ? "吸RIS(PRP)杯3次失败" : "吐RIS(PRP)杯3次失败");
        QString tips = kGripperFailed?  illustrateText + "复位等待处理":"继续发送吸RIS(PRP)到通道测试指令";
        commandNotCompleteOrErr(kGripperFailed,sendData,tips,klaterTimer);
   }
   return;
}






void TestProjectProcess::exceptionRetest(QString tips){

    if(!cglobal::g_StartTesting) return;

    auto* instance = StructInstance::getInstance();
    const bool hadTestingFocus = instance->sampleHadFocus();
    if(hadTestingFocus){
        quint8  indexActive = instance->read_focusSampleActive(m_focus_sample_id);
        QByteArray outputData = instance->outputstheFirstIncompleteInstruction(m_focus_sample_id,indexActive);
        if(!outputData.isEmpty()){
            writeCommandHeader(outputData,tips,DELAY_TIMENUM);
        }else if(outputData.isEmpty()){
          QByteArrayList outputDatalist = instance->outputsIncompleteInstructionList(m_focus_sample_id,indexActive);
          if(!outputDatalist.isEmpty()){
             sendInstructionsWithDelay(outputDatalist,DELAY_TIMENUM,tips);
          }
        }else{
            QLOG_DEBUG()<<tips<<"发送指令未空"<<endl;
            promptsareDisplayed(TESTINGLOG,QString("%1无未发送指令").arg(tips),false);
        }

    }else
    {
        if(!m_finishTestData.isEmpty())
        {
            Finish_Data_ complete_ = m_finishTestData.dequeue();
            quint8 test_finish_chn = complete_.index_Chn;
            quint8 finish_reag =  complete_.index_reagent;

            QString samplename = "";
            int sampleId = 0;
            quint8 focus_index_set = 0;
            StructInstance::getInstance()->rootTestChnGetSampleInfo(test_finish_chn,samplename,sampleId);
            StructInstance::getInstance()->config_focusSample(sampleId,true);//++焦点
            m_focus_sample_id = sampleId;
            switch(finish_reag)
            {
                case AA_REAGENT:  focus_index_set = FOCUS_THROW_AA_TUBE; break;
                case ADP_REAGENT: focus_index_set = FOCUS_THROW_ADP_TUBE;break;
                case EPI_REAGENT: focus_index_set = FOCUS_THROW_EPI_TUBE;break;
                case COL_REAGENT: focus_index_set = FOCUS_THROW_COL_TUBE;break;
                case RIS_REAGENT: focus_index_set = FOCUS_THROW_RIS_TUBE;break;
            }
            StructInstance::getInstance()->write_focusSampleActive(m_focus_sample_id, focus_index_set);
            emit setprogressinitboolwaittext(true,test_finish_chn);

            outputThrowTestChannelReagentFinish(m_focus_sample_id,finish_reag);
        }
        else
        {
            QString outtext = tips + QString::fromUtf8("测试通道内无完成测试样本,继续遍历加样样本");
            promptsareDisplayed(TESTINGLOG,outtext,false);
            StartSampleAdditionWorkflow();
        }
    }
}


