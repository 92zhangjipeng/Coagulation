#pragma execution_character_set("utf-8")

#include "structinstance.h"
#include "quiutils.h"
#include "globaldata.h"
#include "globaldata.h"
#include "globaldata.h"

#include <operclass/fullyautomatedplatelets.h>
#include <mainwindow.h>


StructInstance* StructInstance::m_pInstance = nullptr;
StructInstance::Garbo m_Garbo;
QMutex StructInstance::m_mutex;



bool sample_sort(DATASAMPLESTRUCT *pfirst, DATASAMPLESTRUCT *pend)
{
    return pfirst->sample_num < pend->sample_num; //样本号从小到大排序
}

bool task_sort(QByteArray first, QByteArray end) {
    // 将QByteArray转为16进制字符串（假设存储的是二进制数据）
    QString hexFirst = first.toHex().toUpper();  // 转为"001A2B"
    QString hexEnd = end.toHex().toUpper();

    // 分割字符串，每两个字符为一组（如["00", "1A", "2B"]）
    QStringList listFirst;
    for (int i = 0; i < hexFirst.length(); i += 2) {
        listFirst.append(hexFirst.mid(i, 2));
    }

    QStringList listEnd;
    for (int i = 0; i < hexEnd.length(); i += 2) {
        listEnd.append(hexEnd.mid(i, 2));
    }

    // 检查索引有效性
    if (listFirst.size() < 3 || listEnd.size() < 3) {
        return false;  // 或抛出异常
    }

    // 转换为整数并校验
    bool ok1, ok2;
    int begin_ = listFirst.at(2).toInt(&ok1, 16);
    int end_ = listEnd.at(2).toInt(&ok2, 16);

    if (!ok1 || !ok2) {
        return false;  // 转换失败处理
    }

    return begin_ < end_;
}


bool sort_ascending(single_action_commands *first,single_action_commands *end)
{
    return first->index_ < end->index_;
}

StructInstance::StructInstance(QObject *parent) : QObject(parent)
{
    //清空数据后重新预分配（适用于已知后续数据量）
    m_BloodsampleInfo.clear();
    m_BloodsampleInfo.reserve(120);
    QLOG_DEBUG()<<"任务能够容纳的元素数量="<<m_BloodsampleInfo.capacity()
                <<"已存在容量大小="<<m_BloodsampleInfo.size() ;

    m_testChnStructvec_.clear();
    m_testChnStructvec_.reserve(MACHINE_SETTING_CHANNEL);
    QLOG_DEBUG()<<"模组结构能够容纳的元素数量="<<m_testChnStructvec_.capacity()
                <<"已存在容量大小="<<m_testChnStructvec_.size()<<endl;

}

//清空内存
void StructInstance::_setemptynull()
{
    m_BloodsampleInfo.clear();
    m_BloodsampleInfo.squeeze();
    m_BloodsampleInfo.shrink_to_fit();
    QLOG_DEBUG()<<"全部完成释放样本任务内存capacity:"<<m_BloodsampleInfo.capacity();
    return;
}


const bool StructInstance::getHandsStatus(const int sampleid){
    const auto *itSample = rfindIdSample(sampleid);
    if(!itSample) {
        QLOG_ERROR()<<"获取抓手状态失败样本号"<<sampleid<<"为空";
        return false;
    }
    return itSample->bhandsErr;
}

void  StructInstance::setHandsErrStatus(const int sampleid,bool handsState){
    auto *itSample = wfindIdSample(sampleid);
    if(!itSample) {
        QLOG_ERROR()<<"设置抓手状态失败样本号"<<sampleid<<"为空";
        return;
    }
    itSample->bhandsErr = handsState;
}


const bool StructInstance::getSampleGiveupStatus(const int sampleid){
    const auto *itSample = rfindIdSample(sampleid);
    if(!itSample) {
        QLOG_ERROR()<<"获取放弃样本号样本"<<sampleid<<"为空";
        return false;
    }
    return itSample->bgivesample;
}


void  StructInstance::setSampleGiveupStatus(const int sampleid, bool isGiveup){
    auto *itSample = wfindIdSample(sampleid);
    if(!itSample) {
        QLOG_ERROR()<<"设置放弃样本号样本"<<sampleid<<"为空";
        return;
    }
    itSample->bgivesample = isGiveup;
}


const bool StructInstance::getSamplePendingtimeStatus(const int sampleid){
    const auto *itSample = rfindIdSample(sampleid);
    if(!itSample) {
        QLOG_ERROR()<<"获取挂起样本号样本"<<sampleid<<"为空";
        return false;
    }
    return itSample->bPendingtimeSample;
}

void  StructInstance::setSamplePendingtimeStatus(const int sampleid, bool ispendingtime)
{
    auto *itSample = wfindIdSample(sampleid);
    if(!itSample) {
        QLOG_ERROR()<<"设置挂起样本号样本"<<sampleid<<"为空";
        return;
    }
    itSample->bPendingtimeSample = ispendingtime;
}


bool StructInstance::continueTestPendTimeSample(quint8 &focuActive,int sampleId){
    const auto *itSample = rfindIdSample(sampleId);
    if(!itSample) {
        QLOG_ERROR()<<"获取挂起样本号样本"<< sampleId <<"为空";
        return false;
    }
    if(itSample->bPendingtimeSample){
		focuActive = itSample->index_fcus_Active;
        return true;
    }
    return false;
}




bool  StructInstance::recvPrepatchActions(const int id ,const quint8 activenum){
    auto *itSample = wfindIdSample(id);
    if(!itSample) {
        QLOG_ERROR()<<"收样本抓手异常,失败样本号"<<id<<"为空";
        return false;
    }
    auto itactive = find_if(itSample->prePatchActions.begin(),itSample->prePatchActions.end(),
                     find_recvIndex(activenum));
    if(itactive == itSample->prePatchActions.end() || itSample == nullptr){
        QLOG_ERROR()<<"收样本抓手异常,失败样本号"<<id<<"动作编号"<<activenum<<"未查询到";
        return false;
    }
    (*itactive)->bcompleted = true;
    auto itFinish = find_if(itSample->prePatchActions.begin(),itSample->prePatchActions.end(),
                            find_allactivefinish(false));
    if(itFinish != itSample->prePatchActions.end()){
        return false;
    }else{
        return true;
    }
    return true;
}

void  StructInstance::fillRecvPrepatch(const int id,const QByteArrayList dataList) {
    auto *itSample = wfindIdSample(id);
    if(!itSample) {
        QLOG_ERROR()<<"收样本抓手异常,失败样本号"<<id<<"为空";
        return ;
    }

    for(const QByteArray &data : dataList){
        if (data.isEmpty()) {
            QLOG_WARN() << "重试指令数据为空，样本ID:" << id << endl;
            continue;
        }
        QByteArray indexField = data.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            QLOG_WARN() << "指令索引字段缺失，指令数据:" << data;
            continue;
        }
        bool ok;
        QString hexStr = indexField.toHex();
        quint8 indexcode = hexStr.toUShort(&ok, HEX_SWITCH);
        if (!ok) {
            QLOG_WARN() << "重试指令无效的指令索引值:" << indexField << "原始指令:" << data;
            continue;
        }
        itSample->prePatchActions.push_back(new single_action_commands(indexcode,false,data));
    }
}

void  StructInstance::clearRecvPrepatch(const int id){
    auto *itSample = wfindIdSample(id);
    if(!itSample) {
        QLOG_ERROR()<<"收样本抓手异常,失败样本号"<<id<<"为空";
        return ;
    }
    for(auto *command : itSample->prePatchActions){
        delete command;
    }
    itSample->prePatchActions.clear();
}


void StructInstance::rootTestChnGetSampleInfo(const quint8 index_Chn, QString &infon_num, int &id)
{
    auto it = find_if(m_BloodsampleInfo.cbegin(),
                      m_BloodsampleInfo.cend(),
                      find_sampleinfo(index_Chn));
    if(it != m_BloodsampleInfo.cend())
    {
        infon_num = (*it)->samplestr;
        id = (*it)->sample_num;
        QLOG_DEBUG() << "[校验]单个任务完成通道" << index_Chn << "获取样本号和ID" << infon_num << endl;
    }
    else
    {
        QLOG_DEBUG()<<"err未找到通道号"<<index_Chn<<"的样本号";
    }
    return;
}


void sycn_testdat_index_reag(QVector<double> &sycndata, chntest_reagdata testchndata_)
{
     auto iter = testchndata_.begin();
     while(iter != testchndata_.end())
     {
         sycndata.push_back((*iter)->test_data);
         iter++;
     }
}

void StructInstance::root_getinitvaluedata(const quint8 finishChn,quint8 index_reag,quint16 &AnaemiaInitValue,
                                               QVector<double> &TestedDatamap,quint8 &suckanemia,quint8 &suckbloody)
{
    auto it = find_if(m_testChnStructvec_.begin(),m_testChnStructvec_.end(),
                      finder_Chn(finishChn));
    if(it != m_testChnStructvec_.end())
    {
        if ((*it) != nullptr)
        {
            switch (index_reag)
            {
                case AA_REAGENT:
                    sycn_testdat_index_reag(TestedDatamap, (*it)->AA_testchndata_);
                    _delReagentData(&(*it)->AA_testchndata_);
                break;
                case ADP_REAGENT:
                    sycn_testdat_index_reag(TestedDatamap, (*it)->ADP_testchndata_);
                    _delReagentData(&(*it)->ADP_testchndata_);
                break;
                case EPI_REAGENT:
                    sycn_testdat_index_reag(TestedDatamap, (*it)->EPI_testchndata_);
                    _delReagentData(&(*it)->EPI_testchndata_);
                break;
                case COL_REAGENT:
                    sycn_testdat_index_reag(TestedDatamap, (*it)->COL_testchndata_);
                    _delReagentData(&(*it)->COL_testchndata_);
                break;
                case RIS_REAGENT:
                    sycn_testdat_index_reag(TestedDatamap, (*it)->RIS_testchndata_);
                    _delReagentData(&(*it)->RIS_testchndata_);
                break;
                default:
                    break;
            }
        }
    }
    else
    {
        QLOG_DEBUG()<<"err未找完成测试通道"<<finishChn;
        return;
    }
    auto it_ = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_sampleinfo(finishChn));
    if(it_ != m_BloodsampleInfo.end())
    {
        AnaemiaInitValue = (*it_)->initAnemiaValue;
        suckanemia = (*it_)->suckanaemia_hole;
        suckbloody = (*it_)->suckbloody_hole;
        QLOG_DEBUG()<<"通道"<< finishChn <<"的贫血值"<<AnaemiaInitValue<<endl;
    }
    else
    {
        QLOG_DEBUG()<<"完成通道"<< finishChn <<"未找到LINE="<<__LINE__<<endl;
    }
    return;
}

void StructInstance::_Clearinvalidcommands(DATASAMPLESTRUCT *psanlpeinfo) {

    // 使用模板函数统一处理容器清理和日志记录
    auto CleanContainer = [&](auto &container, const QString &logMsg) {
        int index = 0;
        for (auto it = container.begin(); it != container.end();) {
            if (*it == nullptr) {
                QLOG_DEBUG() << logMsg << "第" << index << "条命令数据为空";
                it = container.erase(it);  // 正确更新迭代器
            } else {
                ++it;
                ++index;  // 仅当未删除时递增序号
            }
        }
    };

    // 清洗双针命令校验
    CleanContainer(psanlpeinfo->clean_reag_blood_pin, "清洗双针命令数据");
    // 洗血样针命令校验
    CleanContainer(psanlpeinfo->clean_blood_pin, "清洗血样针命令数据");
    // 加样动作校验
    CleanContainer(psanlpeinfo->tadd_sample_active, "加样动作命令数据");
    // 抓贫血动作校验
    CleanContainer(psanlpeinfo->t_catchAnemiaTest, "抓贫血动作命令数据");
    // 丢杯动作校验
    CleanContainer(psanlpeinfo->t_throw_anemiacups_active, "丢杯动作命令数据");



    // 处理测试试剂相关容器
    auto _iter_ = psanlpeinfo->ptestReagent_active.begin();
    while (_iter_ != psanlpeinfo->ptestReagent_active.end()) {
        Testing_reagents *pdata = *_iter_;
        if (!pdata) {  // 检查Testing_reagents指针是否为空
            _iter_ = psanlpeinfo->ptestReagent_active.erase(_iter_);
            QLOG_DEBUG() << "测试试剂命令数据中存在空指针";
            continue;
        }

        CleanContainer(pdata->pThrowCupsActive, "测试试剂丢杯动作命令数据");
        CleanContainer(pdata->pSpitReagentsActive, "吐试剂命令数据");
        CleanContainer(pdata->pCleanReagentActive, "清洗试剂针命令数据");
        CleanContainer(pdata->pTestingReagentActive, "测试试剂动作命令数据");

        ++_iter_;
    }
}





void StructInstance::SortemergencyattheTop()
{
    std::sort(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),sample_sort);
    QList<DATASAMPLESTRUCT *> bemergency_List;
    bemergency_List.clear();
    auto iter = m_BloodsampleInfo.begin();
    while(iter != m_BloodsampleInfo.end())
    {
       DATASAMPLESTRUCT *paddblood = *iter;
       if(paddblood->bemergency == true)
       {
           bemergency_List.push_back(paddblood);
       }
       iter++;
    }
    int loc = 0;
    foreach(auto it , bemergency_List)
    {
       m_BloodsampleInfo.replace(loc,it);
       QString outtext = QString("急诊样本置顶成功:%1").arg(it->sample_num);
       QLOG_DEBUG()<<"["<<__FILE__<<"]"<<__LINE__<<__FUNCTION__<<outtext<<endl;
       loc++;
    }
    return;
}

void StructInstance::addSamplebloodInfo(DATASAMPLESTRUCT *&pinfo)
{
    _Clearinvalidcommands(pinfo);

    pinfo->_bthisSampleaddData = true;

    m_BloodsampleInfo.push_back(pinfo);

    QString outprint = QString("样本[%1]添加成功!待测样本总数:[%2]").arg(pinfo->sample_num).arg(m_BloodsampleInfo.size());

    QLOG_DEBUG()<<"["<<__FILE__<<"]"<<__LINE__<<__FUNCTION__<<outprint<<endl;

    return;
}

bool StructInstance::outAlreadyAddSample(QString sampleId)
{
    return std::any_of(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
            [&sampleId](const DATASAMPLESTRUCT* entry) {
                return entry && (entry->samplestr == sampleId);
            });
}


//设置为焦点
void StructInstance::config_focusSample(int indexSample,const bool bfcusin)
{
    auto  it = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(indexSample));

    if(it == m_BloodsampleInfo.end()) {
        QLOG_ERROR() << "[焦点设置] 无效样本ID：" << indexSample;
        return;
    }

    DATASAMPLESTRUCT *psampledatainfo  = *it;

    // 防御性编程：二次验证指针有效性
    if(psampledatainfo != nullptr) {
        psampledatainfo->fcus_in = bfcusin;
        QLOG_DEBUG() << "[焦点设置] 样本：" << indexSample
                    << " 焦点状态：" << bfcusin;
        return;
    }

    // 异常情况处理（理论上不可达）
    QLOG_FATAL() << "[焦点设置] 空指针异常 ID：" << indexSample;

    return;
}

//取消焦点
void StructInstance::config_dismissFocusSample(int indexSample)
{
    auto it = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(indexSample));
    if(it == m_BloodsampleInfo.end() || *it == nullptr) {
        QLOG_ERROR() << "[取消焦点样本] 未找到样本ID" << indexSample
                            << "或指针无效，取消焦点样本失败"<<endl;
        return;
    }
    DATASAMPLESTRUCT *ptmp = (*it);
    ptmp->fcus_in = false;
    QLOG_DEBUG()<<"样本:"<<indexSample<<"取消焦点"<<endl;
    return;
}


void StructInstance::sycn_sampleneed_data(const int id, quint8 &tsetChn)
{
    const auto it = find_if(m_BloodsampleInfo.cbegin(),m_BloodsampleInfo.cend(),find_id(id));

    if(it == m_BloodsampleInfo.cend()) {
        QLOG_DEBUG() << "[样本同步] 未找到ID:" << id;
        return ;
    }

    const DATASAMPLESTRUCT* ptmp = *it;


    if(ptmp != nullptr) {
        // 使用安全类型转换
        tsetChn = static_cast<quint8>(ptmp->indexTestingChn);
        QLOG_DEBUG() << "[样本同步] ID:" << id
                    << "测试通道:" << tsetChn + 1;
        return;
    }

    // 异常情况处理
    QLOG_ERROR() << "[样本同步] 空指针异常 ID:" << id;
    return;
}




void StructInstance::sycn_sample_all_test_tube(QString sample,QList<quint8> &anemiatube_bloodhole)
{
    auto iter = m_BloodsampleInfo.begin();
    while(iter != m_BloodsampleInfo.end())
    {
        DATASAMPLESTRUCT *pfinished = (*iter);
        if(pfinished->samplestr == sample)
        {
            anemiatube_bloodhole.push_back(pfinished->spitanaemia_hole);
            for(int i = 0 ; i < pfinished->bloody_tube.size();i++)
            {
                anemiatube_bloodhole.push_back(pfinished->bloody_tube.at(i).bloody_hole);
            }
            break;
        }
        iter++;
    }
    return;
}

void StructInstance::cancelsampletask(quint8 cancelsanpleid,int &canceltube_num,quint8 &anemiahole,
                                      QList<quint8> &emptytubehole, quint8 &_cancelbackhole)
{

    const auto *psampleInfo = rfindIdSample(cancelsanpleid);
    if(!psampleInfo){
       QLOG_DEBUG()<<"未找到取消样本ID"<< cancelsanpleid <<"为空"<<endl;
       return;
    }

    _cancelbackhole = psampleInfo->suckbloody_hole;
    canceltube_num = psampleInfo->bloody_tube.size() + 1;
    anemiahole = psampleInfo->suckanaemia_hole;
    const quint8 totolnum = psampleInfo->bloody_tube.size();
    emptytubehole.reserve(canceltube_num);
	emptytubehole.push_back(psampleInfo->spitanaemia_hole);
    for(int i = 0 ; i < totolnum; i++){
       emptytubehole.push_back(psampleInfo->bloody_tube.at(i).bloody_hole);
    }
    onetestend_del(cancelsanpleid);
    return;
}

void StructInstance::_delReagentData(chntest_reagdata* pdelchndata)
{
    auto it = pdelchndata->begin();
    while (it != pdelchndata->end())
    {
        if (*it != nullptr) {
            delete *it;
            *it = nullptr;
        }
        it++;
    }
    pdelchndata->erase(std::remove(pdelchndata->begin(), pdelchndata->end(), nullptr), pdelchndata->end());
    return;
}

void StructInstance::_deleteOneSampleTesteddata(const quint8 _chntested ,const int indexSample)
{
    auto it = find_if(m_testChnStructvec_.begin(), m_testChnStructvec_.end(),finder_Chn(_chntested));
    if(it != m_testChnStructvec_.end())
    {

        if((*it) != nullptr)
        {
            _delReagentData(&(*it)->AA_testchndata_);
            QVector<TESTCHNDATA *>().swap((*it)->AA_testchndata_);
            QLOG_DEBUG() << "清除通道" << _chntested + 1 << "AA删除通道数据leng=" << (*it)->AA_testchndata_.capacity();

            _delReagentData(&(*it)->ADP_testchndata_);
            QVector<TESTCHNDATA *>().swap((*it)->ADP_testchndata_);
            QLOG_DEBUG() << "清除通道" << _chntested + 1 << "ADP删除通道数据leng=" << (*it)->ADP_testchndata_.capacity();

            _delReagentData(&(*it)->EPI_testchndata_);
            QVector<TESTCHNDATA *>().swap((*it)->EPI_testchndata_);
            QLOG_DEBUG() << "清除通道" << _chntested + 1 << "EPI删除通道数据leng=" << (*it)->EPI_testchndata_.capacity();

            _delReagentData(&(*it)->COL_testchndata_);
            QVector<TESTCHNDATA *>().swap((*it)->COL_testchndata_);
            QLOG_DEBUG() << "清除通道" << _chntested + 1 << "COL删除通道数据leng=" << (*it)->COL_testchndata_.capacity();

            _delReagentData(&(*it)->RIS_testchndata_);
            QVector<TESTCHNDATA *>().swap((*it)->RIS_testchndata_);
            QLOG_DEBUG() << "清除通道" << _chntested + 1 << "RIS删除通道数据leng=" << (*it)->RIS_testchndata_.capacity();

            (*it)->Reagent = ANEMIA;
            QString err = QString("样本:%1 通道%2 删除通道数据成功!").arg(indexSample).arg(_chntested + 1);
            QLOG_DEBUG() << err << __LINE__ << endl;
        }
        else
        {
            (*it)->Reagent = ANEMIA;
            QString err = QString("样本:%1 通道%2 删除通道数据失败数据为NULL!").arg(indexSample).arg(_chntested + 1);
            QLOG_DEBUG() << err << __LINE__ << endl;
        }
    }
    else
    {
        QString err = QString("样本:%1 通道%2 删除数据失败!").arg(indexSample).arg(_chntested + 1);
        QLOG_DEBUG()<<err<<__LINE__<<endl;
    }
    return;
}


//删命令
void StructInstance::_deleteCleanDoublepinorder(tSingleActive_list* pdelorder)
{
    for(auto& ptr : *pdelorder){
        delete ptr;
        ptr = nullptr;
    }
    pdelorder->clear();
    return;
}

void  StructInstance::_delTestReagentActive(tVariousReagentsvec* ptestReagActive)
{
    // 定义lambda处理重复逻辑
    auto cleanMember = [this](QVector<single_action_commands*>& memberVec, const QString& actionName) {
            _deleteCleanDoublepinorder(&memberVec);
            QVector<single_action_commands*>().swap(memberVec);
            QLOG_DEBUG() << QString("测试试剂完成删除[%1]内存=%2 size=%3")
                            .arg(actionName).arg(memberVec.capacity()).arg(memberVec.size());
    };

    // 遍历容器并清理元素
    for (auto& item : *ptestReagActive) {
        if (item) {
            // 批量清理成员向量
            cleanMember(item->pThrowCupsActive, "丢废液杯");
            cleanMember(item->pSpitReagentsActive, "吐试剂");
            cleanMember(item->pCleanReagentActive, "吸试剂针");
            cleanMember(item->pTestingReagentActive, "吸试剂抓杯");

            delete item;
            item = nullptr;
        }
    }

    // 清理容器中的空指针
    ptestReagActive->erase(
        std::remove(ptestReagActive->begin(), ptestReagActive->end(), nullptr),
        ptestReagActive->end()
    );
}

//测试完成删除样本
void StructInstance::clearSampleDataByNumber(DATASAMPLESTRUCT& data, int targetSample) {
    // 仅处理匹配的样本号
    if (data.sample_num != targetSample) return;

    // 清空所有指针容器成员（需手动释放内存）
    auto clearPtrList = [](auto& list) {
        qDeleteAll(list);    // Qt内置函数等效于遍历+delete
        list.clear();
    };

    clearPtrList(data.prePatchActions);
    clearPtrList(data.clean_reag_blood_pin);
    clearPtrList(data.clean_blood_pin);
    clearPtrList(data.tadd_sample_active);
    clearPtrList(data.t_catchAnemiaTest);
    clearPtrList(data.t_throw_anemiacups_active);

    clearTestingReagentContainer(data.ptestReagent_active);

    // 清空普通容器成员
    data.bloody_tube.clear();

    // 重置其他成员为初始值
    data.samplestr.clear();
    data.sample_num = 0;
    data.bemergency = false;
    data._bthisSampleaddData = false;
    data.bstartTesting = false;
    data.indexTestingChn = -1;
    data.initAnemiaValue = 0;
    data.fcus_in = false;
    data.index_fcus_Active = 0;
    data.suckanaemia_hole = 0;
    data.suckbloody_hole = 0;
    data.suckanaemia_axis = {0, 0};
    data.suckbloody_axis = {0, 0};
    data.CompleteAspirationState = 0;
    data.spitanaemia_hole = 0;
    data.spitanaemia_axis = {0, 0};
    data.anemiaoffHandsAxis = {0, 0};
    data.bcleanbloody_state = false;
    data.bcleanDoublePin_state = false;
    data.bhandsErr = false;
    data.bgivesample = false;
    data.bPendingtimeSample = false;
}
void StructInstance::clearTestingReagentData(TESTING_REAGENT* reagent) {
    if (!reagent) return;

    // 定义通用指针容器清理lambda
    auto clearPtrList = [](auto& list) {
        qDeleteAll(list); // 释放所有指针内存
        list.clear();     // 清空容器
    };

    // 清空所有 tSingleActive_list 成员
    clearPtrList(reagent->pThrowCupsActive);
    clearPtrList(reagent->pSpitReagentsActive);
    clearPtrList(reagent->pCleanReagentActive);
    clearPtrList(reagent->pTestingReagentActive);

    // 重置其他成员为默认值
    reagent->suckCupIndexCommand = 0;
    reagent->index_Reagent = ANEMIA;
    reagent->indexHole = 0;
    reagent->emptyHoleAxisHands = QPoint(0, 0);
    reagent->inintBloodyValue = 0;
    reagent->testfinished = false;
}
void StructInstance::clearTestingReagentContainer(tVariousReagentsvec& container) {
    for (auto* reagent : container) {
        clearTestingReagentData(reagent); // 清空内部数据
        delete reagent;                   // 释放结构体内存
    }
    container.clear(); // 清空容器
}
void StructInstance::removeSampleFromContainer(DataSampleList& container, int target) {
    for (auto it = container.begin(); it != container.end();) {
        if ((*it)->sample_num == target) {
            clearSampleDataByNumber(**it, target); // 清空数据
            delete *it;        // 释放结构体内存
            it = container.erase(it); // 移除指针
        } else {
            ++it;
        }
    }
}
void StructInstance::onetestend_del(int indexSample)
{
    removeSampleFromContainer(m_BloodsampleInfo,indexSample);
}

void StructInstance::SampleTotal(int &num_total)
{
    num_total = m_BloodsampleInfo.size();
}


bool StructInstance::sampleHadFocus()
{
   const bool hadFocus = std::any_of(m_BloodsampleInfo.cbegin(),
                                     m_BloodsampleInfo.cend(),
                                     find_bfocus(true)
                                     );
   QLOG_DEBUG() << "试剂测试完成后"
                << (hadFocus ? "有" : "[无]")
                << "焦点任务 行号" << __LINE__;
   return hadFocus;

}

QByteArray StructInstance::addSampleHeandCommand(const int taskNumid){

    const auto it = std::find_if(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
        [taskNumid](const auto& item) { return item->sample_num == taskNumid; });

    if(it == m_BloodsampleInfo.cend() || (*it)== nullptr){
        QLOG_ERROR()<<"加样失败未找到样本"<<taskNumid<<"或者样本为null";
        return {};
    }

    const bool test_state = (*it)->bstartTesting;
    const quint8 aspirationState = (*it)->CompleteAspirationState;
    QLOG_DEBUG() << "找到样本ID" << taskNumid
        << "测试状态" << test_state
        << "加样状态" << aspirationState<<endl;
    if (test_state && aspirationState == ADDING_SAMPLE) {
        return (*it)->tadd_sample_active.front()->arrcode;
    }
    return {};
}



void  StructInstance::OutputCommands(const int taskNum, QByteArrayList &sendData)
{
    sendData.clear();

    const auto it = std::find_if(m_BloodsampleInfo.begin(), m_BloodsampleInfo.end(),
        [taskNum](const auto& item) { return item->sample_num == taskNum; });

    if (it != m_BloodsampleInfo.end()) {
        auto* pdata_info = *it;
        const bool test_state = pdata_info->bstartTesting;
        const quint8 aspirationState = pdata_info->CompleteAspirationState;

        QLOG_DEBUG() << "找到样本ID" << taskNum
            << "测试状态" << test_state
            << "加样状态" << aspirationState;

        if (!test_state && aspirationState == NOT_ADD_SAMPLE) {
            sendData.reserve(pdata_info->tadd_sample_active.size());
            for (const auto* pdirectives : pdata_info->tadd_sample_active) {
                sendData.push_back(pdirectives->arrcode);
            }
        }
    }
    else {
        QLOG_DEBUG() << "未找到样本ID" << taskNum << "加样信息指令集合失败err";
    }
}

void StructInstance::updateSampleState(const int indexNum, const quint8 state)
{
    auto list_it = find_if(m_BloodsampleInfo.cbegin(),m_BloodsampleInfo.cend(),find_id(indexNum));
    if(list_it != m_BloodsampleInfo.cend())
    {
        DATASAMPLESTRUCT *psampledata = (*list_it);
        if(psampledata->fcus_in == true)
        {
            psampledata->CompleteAspirationState = state;
            psampledata->bstartTesting = true;
            QString outprint = "";
            if(state == ADDING_SAMPLE)  outprint = "进行加样";
            else if(state == ADDED_COMPLETE_SAMPLE) outprint = "加样完成";
            QLOG_DEBUG()<<"样本"<<indexNum<<"设置加样状态"<<outprint<<"测试标志true";
        }
    } else{
        QLOG_DEBUG()<<"样本ID"<< indexNum <<"未找到设置测试状态信息"<<endl;
    }
    return;
}


int StructInstance::OutputPrepareAspirationNum()
{
    const auto iter = std::find_if(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
           [](const DATASAMPLESTRUCT* sample) {
               return sample && !sample->bstartTesting;  // 显式空指针检查
           });
    if (iter == m_BloodsampleInfo.cend()) {
            QLOG_DEBUG() << "[样本管理] 未找到待处理样品,样本总数:"
                        << m_BloodsampleInfo.size();
            return 0;
    }

    const int targetId = (*iter)->sample_num;
    QLOG_DEBUG() << "[样本管理] 找到待处理样本号 #" << targetId
                              << " | 结构编号:" << std::distance(m_BloodsampleInfo.cbegin(), iter);
    return targetId;

}



bool StructInstance::MoveAxisMapingHole(const QPoint sportpos,quint8 &movedLochole,bool &bemptyzone)
{
    bool b_anemina = false;
    auto iter = m_BloodsampleInfo.begin();
    while(iter != m_BloodsampleInfo.end())
    {
        DATASAMPLESTRUCT *pcommand = *iter;
        if(pcommand->CompleteAspirationState == ADDING_SAMPLE)
        {
            if(pcommand->suckanaemia_axis == sportpos)
            {
                movedLochole = pcommand->suckanaemia_hole;
                b_anemina = true;
                bemptyzone = false;
                break;
            }
            else if(pcommand->spitanaemia_axis == sportpos)
            {
                movedLochole = pcommand->spitanaemia_hole;
                b_anemina = true;
                bemptyzone = true;
                break;
            }
            else if(pcommand->suckbloody_axis == sportpos)
            {
                movedLochole = pcommand->suckbloody_hole;
                b_anemina = false;
                bemptyzone = false;
                break;
            }
            else
            {
               for( BLOODY_INFO bloodySpit : pcommand->bloody_tube)
               {
                   if(bloodySpit.bloody_axis == sportpos)
                   {
                       movedLochole = bloodySpit.bloody_hole;
                       b_anemina = false; //吸样的是富血
                       bemptyzone = true;
                       break;
                   }
               }
            }
            break;
        }
        iter++;
    }
    return b_anemina;
}

bool StructInstance::EmptytubeClipAway(const QPoint sportpos,quint8 &movedLochole)
{
    bool findedhole = false;
    auto iter = m_BloodsampleInfo.begin();
    while(iter != m_BloodsampleInfo.end())
    {
        DATASAMPLESTRUCT *pcommand = *iter;
        if(pcommand->fcus_in)
        {
            if(pcommand->anemiaoffHandsAxis == sportpos)
            {
                movedLochole = pcommand->spitanaemia_hole;
                findedhole = true;
                break;
            }
            else
            {
                auto it_ = pcommand->ptestReagent_active.begin();
                while(it_ != pcommand->ptestReagent_active.end())
                {
                    Testing_reagents *preaghole = (*it_);
                    if(preaghole->emptyHoleAxisHands == sportpos)
                    {
                        findedhole = true;
                        movedLochole = preaghole->indexHole;
                        break;
                    }
                    it_++;
                }
            }
          break;
        }
        iter++;
    }
    return  findedhole ;
}
//void StructInstance::execution_after_interruption(QByteArray &outputArry){return;}

void StructInstance::setTestingChn(const int focuSampleID,const quint8 indexchn)
{
    DATASAMPLESTRUCT *psampleInfo = NULL;
    auto list_it = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(focuSampleID));
    if(list_it != m_BloodsampleInfo.end())
    {
        psampleInfo = (*list_it);
        if(psampleInfo->fcus_in == true)
        {
            psampleInfo->indexTestingChn = indexchn;

            config_testChn_State(indexchn,CHN_STATUS_TESTING);

            config_testChn_test_reagent(indexchn,ANEMIA);

            config_testChn_test_SampleID(indexchn,focuSampleID);

            QPoint anemiaEmptyHandAxis(0,0);

            //创建夹PPP试管到测试通道指令
            QVector<QByteArray> catchAnemia2testChn;
            quint8 suckReset = 0,splitReset = 0;
            QUIUtils::ClipAnemiatoTestChn(indexchn,psampleInfo->spitanaemia_hole,catchAnemia2testChn,
                                          anemiaEmptyHandAxis,suckReset,splitReset);

            psampleInfo->anemiaoffHandsAxis = anemiaEmptyHandAxis;
            creatSuckPPP2theTestChannel(focuSampleID,catchAnemia2testChn,suckReset,splitReset);

            //创建样本丢 PPP 命令
            creatThrowCupsCommands(focuSampleID,indexchn);
        }
    }
    else
    {
        QLOG_DEBUG()<<"未找到样本ID"<< focuSampleID <<"继续夹贫血加载信息失败"<<endl;
    }
    return;
}

void StructInstance::write_focusSampleActive(int sample_focu,quint8 index_active)
{
    auto itFocus = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sample_focu));
    if(itFocus == m_BloodsampleInfo.end() || *itFocus == nullptr){
        QLOG_ERROR() << "[设置样本焦点] 样本ID" << sample_focu<<"未找到"
                    << "或指针无效，设置样本焦点失败"<<endl;
        return;
    }
    DATASAMPLESTRUCT *psampledata = *itFocus;
    psampledata->index_fcus_Active = index_active;
    QString stractive = QUIUtils::equipmentdoAction(index_active);
    QString outtext = QString("样本ID:%1准备开始执行%2动作").arg(sample_focu).arg(stractive);
    QLOG_DEBUG()<<outtext<<endl;
}



quint8 StructInstance::read_focusSampleActive(int sample_focu)
{
    quint8 index_active = 0;

    auto it = find_if(m_BloodsampleInfo.cbegin(),m_BloodsampleInfo.cend(),find_id(sample_focu));

    if(it == m_BloodsampleInfo.cend() || *it == nullptr){
        QLOG_ERROR() << "[读取焦点样本] 未找到样本ID" << sample_focu
                    << "或指针无效，读取焦点样本失败"<<endl;
        return index_active;
    }

    DATASAMPLESTRUCT *ptmp = (*it);
    if(ptmp->fcus_in == true){
        index_active = ptmp->index_fcus_Active;
    }
    return index_active;
}





int StructInstance::ClipEndGetAnemiaValeChn(quint8 &TestingChn, quint8 indexActive, QString outText)
{
    int sampleid = 0;
    auto iter = m_BloodsampleInfo.begin();
    while(iter != m_BloodsampleInfo.end())
    {
        DATASAMPLESTRUCT *pcommand = *iter;
        if(pcommand->fcus_in == true)
        {
            if(pcommand->index_fcus_Active == indexActive)
            {
                TestingChn = pcommand->indexTestingChn;
                sampleid = pcommand->sample_num;
                QLOG_DEBUG()<<"通道:"<<TestingChn+1<<outText<<"["<<__FILE__<<"]"<<__LINE__<<__FUNCTION__<<endl;
            }
            break;
        }
        iter++;
    }
    return sampleid;
}

void StructInstance::oper_AnemiaValue(bool bwrite,quint8 indexChn,int &valuedata)
{
    auto iter = std::find_if(m_BloodsampleInfo.begin(), m_BloodsampleInfo.end(),
           [indexChn](DATASAMPLESTRUCT* p) { return p->indexTestingChn == indexChn; });

    if (iter != m_BloodsampleInfo.end()) {
        DATASAMPLESTRUCT* pcommand = *iter;
        if (bwrite == WRITE_OPERAT) {
            pcommand->initAnemiaValue = valuedata;
        } else {
            valuedata = pcommand->initAnemiaValue;
        }
    }
}


void StructInstance::reagentTestintotheStructure(const int sampleId, quint8 &firstTestReagentNum){

    auto *itSample = wfindIdSample(sampleId);
    if(!itSample) return;

    if(!itSample->fcus_in || itSample->index_fcus_Active != FOCUS_THORW_ANEMIA){
        QLOG_WARN()<<"样本"<<sampleId<<"丢PPP完成，焦点未设置丢PPP继续执行失败退出";
        return;
    }

    const quint8 testingChannel = itSample->indexTestingChn;
    const quint8 testingReagnetNum = itSample->bloody_tube.size();
    QLOG_DEBUG()<<"样本"<<sampleId<<"丢完PPP要测试的试剂个数:"<<testingReagnetNum;

    quint8 indexnum = 0;
    for(int n = 0; n < testingReagnetNum; ++n){
        Testing_reagents *pReagentInfo = new Testing_reagents;
        QPoint BloodyoffHandsAxis(0,0);
        pReagentInfo->testfinished = false;
        pReagentInfo->index_Reagent  = itSample->bloody_tube[n].index_reagent;
        pReagentInfo->indexHole = itSample->bloody_tube[n].bloody_hole;
        pReagentInfo->inintBloodyValue = 0;

        //丢杯指令
        QVector<QByteArray> kthrowCupvec;
        quint8 sucknum,splitnum;
        QUIUtils::ThrowTestChnCup(testingChannel,kthrowCupvec,sucknum,splitnum);
        pReagentInfo->pThrowCupsActive.reserve(kthrowCupvec.size());
        pReagentInfo->throwPRRsucknum = sucknum;
        pReagentInfo->throePRPsplitnum = splitnum;
        for(const QByteArray &throwcups : kthrowCupvec){
            char indexChar = throwcups.at(GET_COMMAND_INDEX);
            indexnum = QByteArray::fromRawData(&indexChar, 1).toHex().toUShort(nullptr, HEX_SWITCH);
            pReagentInfo->pThrowCupsActive.push_back(new single_action_commands(indexnum ,false,throwcups));
        }
        std::sort(pReagentInfo->pThrowCupsActive.begin(),pReagentInfo->pThrowCupsActive.end(),
                  sort_ascending);

        //吐试剂
        QVector<QByteArray> addReagentToTestChannel;
        addReagentToTestChannel.clear();
        QUIUtils::spitReanentToTestChn(testingChannel,pReagentInfo->index_Reagent,addReagentToTestChannel);
        pReagentInfo->pSpitReagentsActive.reserve(addReagentToTestChannel.size());
        for(const QByteArray &splitReagent : addReagentToTestChannel){
            char indexChar = splitReagent.at(GET_COMMAND_INDEX);
            indexnum = QByteArray::fromRawData(&indexChar, 1).toHex().toUShort(nullptr, HEX_SWITCH);
            pReagentInfo->pSpitReagentsActive.push_back(new single_action_commands(indexnum,false,splitReagent));

        }
        std::sort(pReagentInfo->pSpitReagentsActive.begin(),
                  pReagentInfo->pSpitReagentsActive.end(),
                  sort_ascending);

        //清洗试剂针
        QVector<QByteArray> cleanReagpinVec;
        QUIUtils::CleanReagentsPinActionCommd(cleanReagpinVec);
        pReagentInfo->pCleanReagentActive.reserve(cleanReagpinVec.size());
        for(const QByteArray &cleanReagentNeedle : cleanReagpinVec){
            char indexChar = cleanReagentNeedle.at(GET_COMMAND_INDEX);
            indexnum = QByteArray::fromRawData(&indexChar, 1).toHex().toUShort(nullptr, HEX_SWITCH);
            pReagentInfo->pCleanReagentActive.push_back(new single_action_commands(indexnum,false,cleanReagentNeedle));
        }
        std::sort(pReagentInfo->pCleanReagentActive.begin(),pReagentInfo->pCleanReagentActive.end(),
                  sort_ascending);

        //抓PRP血样杯到测试通道
       QVector<QByteArray> suckCup2TestChannel;
       quint8 suckPRPindex = 0,ktestPRPsuckBacknum,ktestPRPsplitBacknum; //吸杯起始指令号
       QUIUtils::suckReagentClipTubetoChnPut(pReagentInfo->index_Reagent,
                                             testingChannel,
                                             pReagentInfo->indexHole,
                                             suckCup2TestChannel,
                                             BloodyoffHandsAxis,
                                             suckPRPindex,
                                             ktestPRPsuckBacknum,
                                             ktestPRPsplitBacknum);

       pReagentInfo->pTestingReagentActive.reserve(suckCup2TestChannel.size());
       for(const QByteArray &suckPRP2Channel : suckCup2TestChannel){
           char indexChar = suckPRP2Channel.at(GET_COMMAND_INDEX);
           indexnum = QByteArray::fromRawData(&indexChar, 1).toHex().toUShort(nullptr, HEX_SWITCH);
           pReagentInfo->pTestingReagentActive.push_back(new single_action_commands(indexnum,false,suckPRP2Channel));
       }
       pReagentInfo->testPRPsucknum = ktestPRPsuckBacknum;
       pReagentInfo->testPRPsplitnum = ktestPRPsplitBacknum;
       pReagentInfo->suckCupIndexCommand = suckPRPindex;
       std::sort(pReagentInfo->pTestingReagentActive.begin(),
                 pReagentInfo->pTestingReagentActive.end(),
                 sort_ascending);

       pReagentInfo->emptyHoleAxisHands = BloodyoffHandsAxis;
       itSample->ptestReagent_active.push_back(pReagentInfo);
    }

    //找第一个测试的试剂
    for(int i = 0; i < itSample->ptestReagent_active.size(); i++)
    {
        auto it_ =  itSample->ptestReagent_active.begin();
        Testing_reagents * preag = (*it_);
        if(preag->inintBloodyValue == 0){
              firstTestReagentNum = preag->index_Reagent; //第一个测试的试剂
              QLOG_DEBUG()<<QUIUtils::index_reagent_mapping_reagentName("首测试的试剂:",firstTestReagentNum);
              break;
          }
    }

}


void StructInstance::testPrpGripperErr(const int sampleId,quint8 indexReag,bool &outErr){
    backorigintimes();
    outErr = GRIPPERMOVE_ABNORMALLY;

	//重置丢杯动作
    resettheCupDropAnimation(sampleId,indexReag,false,false);
    return;
}
bool StructInstance::recv_suckReagentClipTube(const int sampleId,int index_code,
                                                quint8 indexReag,bool HandsControl,
                                                bool isSuction,quint32 airvale,QByteArray &sendData,
                                                bool &outErr,bool &laterTimer)
{
    bool iscomplete = false;
    const auto iter = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sampleId));
    if(iter == m_BloodsampleInfo.cend()|| *iter == nullptr){
        QLOG_ERROR() << "[Send吸试剂] 未找到样本ID" << sampleId
                    << "或指针无效，收到Send吸试剂到通道指令失败"<<endl;
        return iscomplete;
    }

    DATASAMPLESTRUCT *psampleinfo = *iter;
    //quint8 putChannel = psampleinfo->indexTestingChn;
    quint8 prphole = 0;
    auto itprpole = std::find_if(psampleinfo->bloody_tube.constBegin(), psampleinfo->bloody_tube.constEnd(),
                          [indexReag](const BLOODY_INFO& datahole) {
                              return datahole.index_reagent == indexReag;
                          });
    if (itprpole != psampleinfo->bloody_tube.constEnd()) {
        prphole = itprpole->bloody_hole;
    }


    auto itreagent = find_if(psampleinfo->ptestReagent_active.begin(),psampleinfo->ptestReagent_active.end(),
                             findtest_testreagent(indexReag));

    if(itreagent == psampleinfo->ptestReagent_active.cend() || *itreagent == nullptr) {
        QLOG_ERROR() << "[Send吸试剂] 样本ID" << sampleId<<"未找到试剂编号"<<indexReag
                    << "或指针无效，收到Send吸试剂到通道指令失败"<<endl;
        return iscomplete;
    }

    Testing_reagents *kptestingReagent = *itreagent;

    auto itend = find_if(kptestingReagent->pTestingReagentActive.begin(),kptestingReagent->pTestingReagentActive.end(),
                         recv_index(index_code));

    if(itend == kptestingReagent->pTestingReagentActive.cend() || *itend == nullptr) {
        QLOG_ERROR() << "[Send吸试剂] 样本ID" << sampleId<<"试剂编号"<<indexReag<<"未找到命令号"<<index_code
                    << "或指针无效，收到Send吸试剂到通道指令失败"<<endl;
        return iscomplete;
    }

    single_action_commands *endReagentCommand = *itend;


    if(!HandsControl){
        endReagentCommand->bcompleted = true;
        allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
        return iscomplete;
    }

    if(isSuction){
        bool isscuess = GlobalData::isHandsSuckFinish(airvale);
        const quint8 suckBackRoiginnum = kptestingReagent->testPRPsucknum;
        if(!isscuess && suckBackRoiginnum == index_code){
            testPrpGripperErr(sampleId,indexReag,outErr);
            QLOG_ERROR()<<"测试试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"[复位]吸取失败";
            return false;
        }else if(isscuess && suckBackRoiginnum == index_code){
             QLOG_DEBUG()<<"吸杯复位测试试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"负压值"<<airvale;
             endReagentCommand->bcompleted = true;
             allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
             return iscomplete;
        }

        const quint8 firstSuckReagCup = suckBackRoiginnum - 2;
        if(firstSuckReagCup == index_code){
            endReagentCommand->bcompleted = true;
            laterTimer = true;
            QLOG_DEBUG()<<"测试试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"舍弃初始[吸杯]负压值"<<airvale;
            allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
            return iscomplete;
        }

        //吸杯
        const quint8 suckreagentnum = suckBackRoiginnum - 1;
        if(suckreagentnum == index_code){
            bool success =  GlobalData::isHandsSuckFinish(airvale);
            int& retries = m_handsFailedtimes;
            const int maxRetries = 3;
            if(success){
                endReagentCommand->bcompleted = true;
                allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
                return iscomplete;
            } else if(retries <= maxRetries){
                quint8 numindex = endReagentCommand->index_;
                retries++;
                sendData =  GlobalData::GrabtheCupAttheTestTubeTray(numindex, retries, prphole);
                return  false;
            }else {
                testPrpGripperErr(sampleId,indexReag,outErr);
                QLOG_DEBUG() <<  "[吸]PRP试杯3次失败" << endl;
                return false;
            }
        }
    }
    else{

        //放杯
        const quint8 splitreagentnum = kptestingReagent->testPRPsplitnum - 1;
        if(splitreagentnum == index_code){
            if(GlobalData::isHandsThrowCupFinish(airvale)){
                endReagentCommand->bcompleted = true;
            }
            QLOG_DEBUG()<<"测试试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"放杯等待负压值为:"<<airvale;
            allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
            return iscomplete;
        }

        const quint8 firstSplitReagentCup = kptestingReagent->testPRPsplitnum - 2;
        if(firstSplitReagentCup == index_code){
            endReagentCommand->bcompleted = true;
            QLOG_DEBUG()<<"测试试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"舍弃初始[丢杯]负压值"<<airvale;
            allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
            return iscomplete;
        }

        if(kptestingReagent->testPRPsplitnum == index_code){
            QLOG_DEBUG()<<"放杯复位测试试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"负压值"<<airvale;
            endReagentCommand->bcompleted = true;
            allTseaReagentActiveFinish(kptestingReagent,iscomplete,sendData);
            return iscomplete;
        }
    }
	return iscomplete;
}
void StructInstance::allTseaReagentActiveFinish(const Testing_reagents *ptestReagent,bool &iscomplete,QByteArray &outdata){
    auto itFinish = find_if(ptestReagent->pTestingReagentActive.cbegin(),
                            ptestReagent->pTestingReagentActive.cend(),
                            find_allactivefinish(false));
    if(itFinish != ptestReagent->pTestingReagentActive.cend())
    {
         iscomplete = false;
         outdata = (*itFinish)->arrcode;
    }
    else
    {
        iscomplete = true;
        backorigintimes();
    }
}


void  StructInstance::changesuckReagentFirstCommandLocation(const int sampleid,quint8 reagentType)
{
    const auto iter = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sampleid));

    //严格空指针检查 + 返回默认空数据
    if(iter == m_BloodsampleInfo.end() || *iter == nullptr) {
        QLOG_ERROR() << "[吸试剂] 未找到样本ID" << sampleid
                    << "或指针无效，发送吸试剂指令头失败"<<endl;
        return ; // 返回空QByteArray
    }

    DATASAMPLESTRUCT *psampleinfo = *iter;

    const auto itreagent = find_if(psampleinfo->ptestReagent_active.begin(),
                                   psampleinfo->ptestReagent_active.end(),
                                   findtest_testreagent(reagentType));
    if(itreagent == psampleinfo->ptestReagent_active.end() || *itreagent == nullptr) {
        QLOG_ERROR() << "[吸试剂] 未找到样本ID" << sampleid <<"试剂编号"<<reagentType
                    << "或指针无效，发送吸试剂指令头失败"<<endl;
        return ;
    }
    if ((*itreagent)->pTestingReagentActive.isEmpty()) {
        QLOG_WARN() << "[吸试剂] 样本ID" << sampleid
            << "吸试剂指令列表为空" << endl;
        return ;
    }
    auto* pCommand = (*itreagent)->pTestingReagentActive.first();
    quint8 num = pCommand->index_;
    pCommand->arrcode = GlobalData::replactSuckReagentLoc(num,reagentType);
    return;
}

QByteArray StructInstance::sendsuckReagentCatchCuptoTestChannelHead(const int sampleid,quint8 reagentType)
{
    const auto iter = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sampleid));

    //严格空指针检查 + 返回默认空数据
    if(iter == m_BloodsampleInfo.end() || *iter == nullptr) {
        QLOG_ERROR() << "[吸试剂抓测试杯到通道] 未找到样本ID" << sampleid
                    << "或指针无效，发送吸试剂抓测试杯到通道指令头失败"<<endl;
        return {}; // 返回空QByteArray
    }

    DATASAMPLESTRUCT *psampleinfo = *iter;

    const auto itreagent = find_if(psampleinfo->ptestReagent_active.begin(),
                                   psampleinfo->ptestReagent_active.end(),
                                   findtest_testreagent(reagentType));

    if(itreagent == psampleinfo->ptestReagent_active.end() || *itreagent == nullptr) {
        QLOG_ERROR() << "[吸试剂抓测试杯到通道] 未找到样本ID" << sampleid <<"试剂编号"<<reagentType
                    << "或指针无效，发送吸试剂抓测试杯到通道指令头失败"<<endl;
        return {};
    }

    if ((*itreagent)->pTestingReagentActive.isEmpty()) {
        QLOG_WARN() << "[吸试剂抓测试杯到通道] 样本ID" << sampleid
            << "的吸试剂抓测试杯到通道指令列表为空" << endl;
        return{};
    }

    const auto* pCommand = (*itreagent)->pTestingReagentActive.constFirst();
    if(!pCommand || pCommand->arrcode.isEmpty()) {
        QLOG_WARN() << "[吸试剂抓测试杯到通道] 样本ID" << sampleid
                      << "的首条指令无效或arrcode为空"<<endl;
        return {};
    }
    return pCommand->arrcode;

}


void StructInstance::setRichBloodInitValue(quint8 index_reag, int index_Chn,int setValue)
{
    auto iter = m_BloodsampleInfo.begin();
    while(iter != m_BloodsampleInfo.end())
    {
         DATASAMPLESTRUCT *pcommand = *iter;
         if(pcommand->indexTestingChn == index_Chn && pcommand->fcus_in)
         {
             auto it = pcommand->ptestReagent_active.begin();
             while(it != pcommand->ptestReagent_active.end())
             {
                 Testing_reagents *ptrag = (*it);
                 if(ptrag->index_Reagent == index_reag)
                 {
                     ptrag->inintBloodyValue = setValue;
                     QLOG_DEBUG()<<"通道"<<index_Chn
                                <<"试剂"<<GlobalData::mapIndexReagentnames(index_reag)
                                <<"intPRP值:"<<ptrag->inintBloodyValue;
                     break;
                 }
                 it++;
             }
             break;
         }
         iter++;
    }
}


void StructInstance::send_spitReagentData(const int SampleId, quint8 index_reagent,QByteArrayList &senddata)
{
    senddata.clear();
    DATASAMPLESTRUCT *pspatreag = NULL;
    Testing_reagents *ptestReag = NULL;
    single_action_commands *psingleactv = NULL;
    QVector<DATASAMPLESTRUCT *>::iterator it = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(SampleId));
    if(it != m_BloodsampleInfo.end())
    {
        pspatreag = (*it);
        auto it_ = find_if(pspatreag->ptestReagent_active.begin(),pspatreag->ptestReagent_active.end(),findtest_testreagent(index_reagent));
        if(it_ != pspatreag->ptestReagent_active.end())
        {
            ptestReag = (*it_);
            auto iter = ptestReag->pSpitReagentsActive.begin();
            while(iter != ptestReag->pSpitReagentsActive.end())
            {
                psingleactv = (*iter);
                QByteArray arrydata = psingleactv->arrcode;
                senddata.push_back(arrydata);
                iter++;
            }
        }
        else
        {
           QLOG_DEBUG()<<"未找到样本ID"<< SampleId <<"未找到试剂"<<index_reagent<<"LINE="<<__LINE__<<endl;
        }
    }
    else
    {
       QLOG_DEBUG()<<"未找到样本ID"<< SampleId <<"向通道吐试剂信息 LINE="<<__LINE__<<endl;
       return;
    }
    return;
}


bool StructInstance::recv_spitReagentData(const int SampleId, quint8 index_reagent,const int index_num)
{
    bool bfinishactive = false;
    auto *psampleInfo = wfindIdSample(SampleId);
    if(!psampleInfo){
        QLOG_DEBUG() << "未找到样本ID" << SampleId << "吐试剂信息为空LINE"<< __LINE__ << endl;
        return false;
    }

    auto splitReagent = find_if(psampleInfo->ptestReagent_active.cbegin(),
                                psampleInfo->ptestReagent_active.cend(),
                                findtest_testreagent(index_reagent));
    if(splitReagent == psampleInfo->ptestReagent_active.cend() || *splitReagent == nullptr){

        QLOG_DEBUG() << "样本ID" << SampleId << "吐试剂" <<GlobalData::mapIndexReagentnames(index_reagent)
                     << "命令编号"<< index_num<<"命令为NULL" << __LINE__ << endl;
        return false;
    }

    auto splitReagentIndexnum = find_if((*splitReagent)->pSpitReagentsActive.begin(),
										(*splitReagent)->pSpitReagentsActive.end(), recv_index(index_num));
	if (splitReagentIndexnum == (*splitReagent)->pSpitReagentsActive.end() || (*splitReagent) == nullptr) {
		QLOG_DEBUG() << "样本ID" << SampleId << "吐试剂" << GlobalData::mapIndexReagentnames(index_reagent)
			<< "命令编号" << index_num << "命令为NULL" << __LINE__ << endl;
		return false;
	}

	(*splitReagentIndexnum)->bcompleted = true;

	auto itfinishedAll = find_if((*splitReagent)->pSpitReagentsActive.cbegin(),
								(*splitReagent)->pSpitReagentsActive.cend(),
								find_allactivefinish(false));
	if (itfinishedAll == (*splitReagent)->pSpitReagentsActive.cend()) {
		bfinishactive = true;
		QLOG_DEBUG() << "样本ID" << SampleId <<"吐"<< GlobalData::mapIndexReagentnames(index_reagent) 
			         << "试剂完成" << "LINE" << __LINE__;
		return  bfinishactive;
	}
	else {
		bfinishactive = false;
		return bfinishactive;
	}
	return bfinishactive;
}



QString StructInstance::testingOutInitPPPandPRP(quint8 IndexChannel,
                                                int &AnaemiaInitValue,
                                                int &bloodyinitValue,
                                                quint8 &index_reag,
                                                int &testSample){

    const auto it = std::find_if(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
                     [IndexChannel](const DATASAMPLESTRUCT* sample) {
                        return sample->indexTestingChn == IndexChannel;
                      });

    if (it == m_BloodsampleInfo.cend()) return "";

    const DATASAMPLESTRUCT* pcommand = *it;
    AnaemiaInitValue = pcommand->initAnemiaValue;
    testSample = pcommand->sample_num;


    const auto reagentIt = std::find_if(pcommand->ptestReagent_active.cbegin(),
                                        pcommand->ptestReagent_active.cend(),
            [](const auto& reagent) {
                return /*reagent->inintBloodyValue != 0 &&*/ !reagent->testfinished;
            });

    if (reagentIt != pcommand->ptestReagent_active.cend()) {
            bloodyinitValue = (*reagentIt)->inintBloodyValue;
            index_reag = (*reagentIt)->index_Reagent;
        }
   return pcommand->samplestr;
}


void StructInstance::setupOneReagentsIsComplete(const QString SampleId,quint8 index_reag,const bool& testErr)
{
	QString idDate;
	int completeSample = 0;
	GlobalData::apartSampleId(SampleId, idDate, completeSample);
    auto * itSample = wfindIdSample(completeSample);
    if(!itSample){
        QLOG_ERROR()<<"样本"<<SampleId<<"试剂"
                   <<GlobalData::mapIndexReagentnames(index_reag)
                   <<"测试完成,但是样本指针为空"<<endl;
        return;
    }
    auto itFinishTest = find_if(itSample->ptestReagent_active.begin(),
                        itSample->ptestReagent_active.end(),
                        findtest_testreagent(index_reag));
    if(itFinishTest == itSample->ptestReagent_active.end() || (*itFinishTest) == nullptr ){
        QLOG_DEBUG()<<"未找到试剂"<<GlobalData::mapIndexReagentnames(index_reag)
                    <<"或试剂结构为空"<<endl;
        return;
    }
    (*itFinishTest)->testfinished = true;

    if(testErr){
       //删除除了  (*itFinishTest)->testfinished == true 的其它试剂数据信息
        Testing_reagents* finishedReagent  = *itFinishTest;
        int deleteCount = 0;

        // 创建新容器只保留完成项
        tVariousReagentsvec tempVec;
        tempVec.push_back(finishedReagent);

        // 释放其他试剂项内存
        for(auto ptr : itSample->ptestReagent_active) {
            if(ptr != finishedReagent) {
                delete ptr;  // 显式释放内存
                deleteCount++;
            }
        }

        itSample->ptestReagent_active = tempVec;
        QLOG_DEBUG() << "已删除" << deleteCount << "个未完成试剂项，保留1个完成项";
    }
    return;
}


bool StructInstance::judge_alreadyCleanBloodpin(const int sampleId,quint8 indexActive)
{
    bool hadcleanBloodpin = false;
    const auto *itSample = rfindIdSample(sampleId);
    if(itSample == nullptr) return false;

    if(itSample->fcus_in && itSample->index_fcus_Active == indexActive){
        hadcleanBloodpin = itSample->bcleanbloody_state;
    }
    return hadcleanBloodpin;
}


void StructInstance::creat_cleanBloodyPin(bool bcleanDoublePin, QByteArrayList allcommand,
                                            DATASAMPLESTRUCT *&psampleinfo)
{
    if(bcleanDoublePin)
        psampleinfo->clean_reag_blood_pin.reserve(allcommand.size());
    else
        psampleinfo->clean_blood_pin.reserve(allcommand.size());

    for(const QByteArray &cleanNeedle : allcommand){
        char  indexChar = cleanNeedle.at(GET_COMMAND_INDEX);
        quint8 indexnum = QByteArray::fromRawData(&indexChar, 1).toHex().toUShort(nullptr, HEX_SWITCH);
        if(bcleanDoublePin){
            psampleinfo->bcleanDoublePin_state = false;
            psampleinfo->clean_reag_blood_pin.push_back(new single_action_commands(indexnum,false,cleanNeedle));
        }else{
             psampleinfo->bcleanbloody_state = false;
             psampleinfo->clean_blood_pin.push_back(new single_action_commands(indexnum,false,cleanNeedle));
        }
    }
    return;
}



void StructInstance::send_CleanbloodyPinActive(const int sampleID, QByteArrayList &senddata)
{
    senddata.clear();

    const auto *itSample = rfindIdSample(sampleID);
    if(itSample == nullptr){
        QLOG_DEBUG()<<"样本"<< sampleID<<"清洗血样针失败,查找样本结构失败";
    }
    senddata.reserve(itSample->clean_blood_pin.size());
    for(auto& pcleanblood : itSample->clean_blood_pin){
        if(!pcleanblood->bcompleted)
            senddata.push_back(pcleanblood->arrcode);
    }
    return;
}

bool StructInstance::isDoublePinCleaningFinished(DATASAMPLESTRUCT *pclean_double,int loseFocusSample)
{
    bool cleanfinished_ = false;
    auto iter_ = find_if(pclean_double->clean_reag_blood_pin.cbegin(),
                         pclean_double->clean_reag_blood_pin.cend(),
                         find_allactivefinish(false));
    if(iter_ != pclean_double->clean_reag_blood_pin.cend())
    {
        cleanfinished_ = false;
    }
    else
    {
        cleanfinished_ = true;
        QString _print = QString::fromUtf8("样本ID:%1清洗双针完成结束!").arg(loseFocusSample);
        pclean_double->bcleanDoublePin_state = true;
        pclean_double->bcleanbloody_state = true;
        QLOG_DEBUG()<<_print<<endl;
    }
    return cleanfinished_;
}

bool StructInstance::recv_cleanDobulePin(const int index_num,int loseFocusSample)
{
    const auto sampleIt = std::find_if(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
            [loseFocusSample](const DATASAMPLESTRUCT* sample) {
                return sample->sample_num == loseFocusSample;
            });

    if (sampleIt == m_BloodsampleInfo.cend()) {
        QLOG_DEBUG() << "[Sample] ID:" << loseFocusSample << "not found in cleaning process" << __LINE__;
        return false;
    }

    auto& commands = (*sampleIt)->clean_reag_blood_pin;

    const auto commandIt = std::find_if(commands.cbegin(), commands.cend(),
        [index_num](const single_action_commands* cmd) {
        return cmd->index_ == index_num;
    });

    if (commandIt == commands.cend()) {
        QLOG_ERROR()<< QString("[Clean] Sample %1: Command %2 not found")
            .arg(loseFocusSample)
            .arg(index_num);
        return false;
    }
    (*commandIt)->bcompleted = true;

    return isDoublePinCleaningFinished(*sampleIt, loseFocusSample);
}

void StructInstance::sendCleanDoubleNeedle(const int sampleid, QByteArrayList &senddata)
{
    senddata.clear();
    auto it = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sampleid));
    if(it == m_BloodsampleInfo.end() || (*it)== nullptr){
        QLOG_ERROR()<<"清洗双针样本为空"<<sampleid<<"或者样本任务为空";
        return;
    }

    auto* pcleanDoublePin = *it;
    for (auto* psingleactv : pcleanDoublePin->clean_reag_blood_pin) {
       if (!psingleactv->bcompleted) {
           senddata.push_back(psingleactv->arrcode);
       }
    }
    return;
}


void StructInstance::send_cleanReagPin(const int sampleid, const quint8 index_reagent, QByteArrayList &senddata)
{
    senddata.clear();
    const auto *psampleinfo = rfindIdSample(sampleid);
    if(!psampleinfo){
       QLOG_DEBUG()<<"未找到样本ID"<< sampleid <<QUIUtils::index_reagent_mapping_reagentName("洗试剂针",index_reagent)
                   <<"信息"<<__LINE__<<endl;
       return;
    }

    auto cleanReagentNeedle = find_if(psampleinfo->ptestReagent_active.cbegin(),
                                      psampleinfo->ptestReagent_active.cend(),
                                      findtest_testreagent(index_reagent));
    if(cleanReagentNeedle ==  psampleinfo->ptestReagent_active.cend() || *cleanReagentNeedle == nullptr){
        QLOG_DEBUG()<<"样本ID"<< sampleid<<QUIUtils::index_reagent_mapping_reagentName("清洗试剂:",index_reagent)
                    <<"指令未找到或者为空"<<endl;
        return;
    }

	auto& crn = **cleanReagentNeedle;
	senddata.reserve(crn.pCleanReagentActive.size());
	for (const auto& elem : crn.pCleanReagentActive) {
		if (!elem->bcompleted) {
			senddata.push_back(elem->arrcode);
		}
	}
    return;
}



bool StructInstance::recv_cleanReagentPin(const int index_num, quint8 index_reag, int loseFocusSample)
{
    bool finished = false;
	auto *psampleInfo = wfindIdSample(loseFocusSample);
	if (!psampleInfo) {
		QLOG_DEBUG() << "未找到样本ID" << loseFocusSample << QUIUtils::index_reagent_mapping_reagentName("接收洗试剂针", index_reag)
			  << "信息" << __LINE__ << endl;
		return finished;
	}

	auto recvCleanReagent = find_if(psampleInfo->ptestReagent_active.begin(),
							psampleInfo->ptestReagent_active.end(),
							findtest_testreagent(index_reag));
	if (recvCleanReagent == psampleInfo->ptestReagent_active.end() || (*recvCleanReagent) == nullptr) {
		QLOG_DEBUG() << "未找到洗试剂针样本ID" << loseFocusSample<<QUIUtils::index_reagent_mapping_reagentName("未找到试剂:", index_reag);
		return finished;
	}

	auto recvIndex = find_if((*recvCleanReagent)->pCleanReagentActive.begin(),
							 (*recvCleanReagent)->pCleanReagentActive.end(),
							 recv_index(index_num));
	if (recvIndex == (*recvCleanReagent)->pCleanReagentActive.end() || *recvIndex == nullptr) {
		QLOG_DEBUG() << "样本ID" << loseFocusSample << QUIUtils::index_reagent_mapping_reagentName("清洗试剂:", index_reag) 
			<< "未找到编号" << index_num << endl;
		return finished;
	}

	(*recvIndex)->bcompleted = true;
	auto finishCleanAll = find_if((*recvCleanReagent)->pCleanReagentActive.cbegin(),
									(*recvCleanReagent)->pCleanReagentActive.cend(),
									find_allactivefinish(false));
	if (finishCleanAll == (*recvCleanReagent)->pCleanReagentActive.cend()) {
        finished = true;
		//QLOG_DEBUG() << "样本ID" << loseFocusSample <<QUIUtils::index_reagent_mapping_reagentName("清洗试剂针:", indexreag) << endl;
	}
	else {
        finished = false;
	}
	return finished;
}




bool StructInstance::recv_cleanbloodPin(const int index_num, int loseFocusSample)
{
    bool bfinished = false;
    auto *sampleinfo = wfindIdSample(loseFocusSample);
    if(!sampleinfo){
        QLOG_DEBUG()<<"样本ID"<< loseFocusSample <<"未找到洗血样针信息"<<__LINE__<<endl;
        return bfinished;
    }

    auto cleanBloodNeedle = find_if(sampleinfo->clean_blood_pin.begin(),
                                    sampleinfo->clean_blood_pin.end(),recv_index(index_num));

    if(cleanBloodNeedle == sampleinfo->clean_blood_pin.end() || *cleanBloodNeedle == nullptr){
        QLOG_DEBUG()<<"样本"<<loseFocusSample<<"未找到洗血样针命令编号:"<<index_num<<endl;
        return bfinished;
    }

	(*cleanBloodNeedle)->bcompleted = true;

	auto cleanFinishAll = find_if(sampleinfo->clean_blood_pin.cbegin(),
								sampleinfo->clean_blood_pin.cend(),
								find_allactivefinish(false));
	if (cleanFinishAll == sampleinfo->clean_blood_pin.cend()) {
		bfinished = true;
        sampleinfo->bcleanDoublePin_state = true;
        sampleinfo->bcleanbloody_state = true;
        QLOG_DEBUG()<<"样本"<<loseFocusSample<<"洗血样针指令完成标志设为true"<<endl;
	}
	else {
		bfinished = false;
	}
	return bfinished;
}


QByteArray StructInstance::sendCommandThrowCupFirst(const int SampleID, quint8 finishReag, bool isreplaceFlag){

    const auto itsample = find_if(m_BloodsampleInfo.cbegin(),m_BloodsampleInfo.cend(),find_id(SampleID));
    if(itsample == m_BloodsampleInfo.cend() || *itsample == nullptr){
        QLOG_ERROR() << "[发送首弃测试完成试杯] 未找到样本ID" << SampleID
                    << "或指针无效，弃测试完成试杯指令头失败"<<endl;
        return {};
    }

    DATASAMPLESTRUCT *kpthrowCupid = *itsample;

    const auto it = find_if(kpthrowCupid->ptestReagent_active.cbegin(),
                            kpthrowCupid->ptestReagent_active.cend(),
                            findtest_testreagent(finishReag));

    if(it == kpthrowCupid->ptestReagent_active.cend() || *it == nullptr){
        QLOG_ERROR() << "[发送首弃测试完成试杯]样本ID" << SampleID <<"[未找到试剂]"<<finishReag
                    << "或指针无效,弃测试完成试杯试剂动作指令失败"<<endl;
        return {}; // 返回空QByteArray
    }

    Testing_reagents *kpfinishReagent = *it;

    if(isreplaceFlag){
        for(auto &pCommand : kpfinishReagent->pThrowCupsActive){
			pCommand->bcompleted = false;
        }
    }

    const auto* pCommand = kpfinishReagent->pThrowCupsActive.constFirst();
    if(!pCommand || pCommand->arrcode.isEmpty()) {
        QLOG_WARN() << "[发送首弃测试完成试杯] 样本ID" << SampleID
                      << "的首条指令无效或arrcode为空"<<endl;
        return {};
    }
    return pCommand->arrcode;
}

bool StructInstance::_boolThrowCupFinished(const int _sampleid,quint8 index_reag, Testing_reagents *ptestReag)
{
    bool bFinished = false;
    auto iter_ = find_if(ptestReag->pThrowCupsActive.begin(),ptestReag->pThrowCupsActive.end(),find_allactivefinish(false));
    if(iter_ != ptestReag->pThrowCupsActive.end())
    {
        bFinished = false;
        QString remind_ = QString("样本:%1 未完成%2").arg(_sampleid)
                .arg(QUIUtils::index_reagent_mapping_reagentName("丟试杯",index_reag));
        QLOG_DEBUG()<<remind_;
    }
    else
    {
        bFinished = true;
        QLOG_DEBUG()<<"样本"<< _sampleid <<QUIUtils::index_reagent_mapping_reagentName("丟试杯",index_reag)<<endl;
    }
    return bFinished;
}


void StructInstance::throwPRPFailed(const int sampleID,quint8 indexReag,bool &outErr){
    outErr = GRIPPERMOVE_ABNORMALLY;
    sendCommandThrowCupFirst(sampleID,indexReag,GRIPPERMOVE_ABNORMALLY);
    backorigintimes();
}
bool StructInstance::recvThrowCupPara(const int sampleID,const int commandIndex,quint8 indexReag,
                                      bool HandsControl,bool isSuction,quint32 airvale,
                                      QByteArray &sendData,bool &outErr,bool &laterTimer){

    bool isthrocupfinish = false;
    auto itsample = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sampleID));
    if(itsample == m_BloodsampleInfo.cend() || *itsample == nullptr){
        QLOG_ERROR() << "[收弃测试完成试杯] 未找到样本ID" << sampleID
                    << "或指针无效，收弃测试完成试杯指令头失败"<<endl;
        return false;
    }

    DATASAMPLESTRUCT *p_recv_throwcups = *itsample;
    quint8 kthrowChannel = p_recv_throwcups->indexTestingChn;

    auto iter = find_if(p_recv_throwcups->ptestReagent_active.begin(),
                         p_recv_throwcups->ptestReagent_active.end(),
                         findtest_testreagent(indexReag));
    if(iter == p_recv_throwcups->ptestReagent_active.end() || *iter == nullptr){
        QLOG_ERROR() << "[收弃测试完成试杯] 未找到样本ID" << sampleID<<"未找到试剂"<<indexReag
                     << "或指针无效，收弃测试完成试杯指令头失败"<<endl;
        return false;
    }
    Testing_reagents *preaginfo = *iter;

    auto it = find_if(preaginfo->pThrowCupsActive.begin(),preaginfo->pThrowCupsActive.end(),
                      recv_index(commandIndex));

    if(it == preaginfo->pThrowCupsActive.end() || *it == nullptr){
        QLOG_ERROR() << "[收弃测试完成试杯] 未找到样本ID" << sampleID<<"试剂"<<indexReag
                     <<"未找到命令编号"<<commandIndex
                     << "或指针无效，收弃测试完成试杯指令头失败"<<endl;
        return false;
    }

    single_action_commands *kpthrowCup = *it;

    if(!HandsControl){
        kpthrowCup->bcompleted = true;
        allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
        return isthrocupfinish;
    }


    if(isSuction){
        const quint8 throwCupsuckBackNum = preaginfo->throwPRRsucknum;
        bool  issucess = GlobalData::isHandsSuckFinish(airvale);
        if(!issucess && throwCupsuckBackNum == commandIndex){
            throwPRPFailed(sampleID,indexReag,outErr);
            return false;
        }else if(issucess && throwCupsuckBackNum == commandIndex){
            QLOG_DEBUG()<<"吸杯复位丢试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"负压值"<<airvale;
            kpthrowCup->bcompleted = true;
            allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
            return isthrocupfinish;
        }

        quint8 firstGrippersuck = preaginfo->throwPRRsucknum - 2;
        if(firstGrippersuck == commandIndex){
            kpthrowCup->bcompleted = true;
            laterTimer = true;
            QLOG_DEBUG()<<"丢试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"舍弃初次[吸杯]负压值"<<airvale;
            allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
            return isthrocupfinish;
        }

        const quint8 throwsuckPRPnum = preaginfo->throwPRRsucknum - 1;
        if(throwsuckPRPnum == commandIndex){
            bool success =  GlobalData::isHandsSuckFinish(airvale);
            int& retries =  m_handsFailedtimes ;
            const int maxRetries =  3;
            if (success) {
                kpthrowCup->bcompleted = true;
                allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
                return isthrocupfinish;
            }
            else if (retries <= maxRetries)
            {
                quint8 numindex = kpthrowCup->index_;
                retries++;
                sendData =  GlobalData::Grabthehandtotryagain(numindex, retries, kthrowChannel);
                return false;
            }
            else
            {
                throwPRPFailed(sampleID, indexReag,outErr);
                QLOG_DEBUG() << "通道吸试管丢杯3次失败"<<endl;
                return isSuction ? false : false;
            }
        }

    }
    else{

        const quint8 firstSplitCupVal = preaginfo->throePRPsplitnum - 2;
        if(firstSplitCupVal == commandIndex){
            kpthrowCup->bcompleted = true;
            QLOG_DEBUG()<<"丢试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"舍弃初次[放杯]负压值"<<airvale;
            allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
            return isthrocupfinish;
        }


        const quint8 throwPRPSplitnum =  preaginfo->throePRPsplitnum - 1;
        if(throwPRPSplitnum == commandIndex){
            if(GlobalData::isHandsThrowCupFinish(airvale))
                 kpthrowCup->bcompleted = true;
            QLOG_DEBUG()<<"等待丢试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"放杯负压值"<<airvale;
            allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
            return isthrocupfinish;
        }

        if(preaginfo->throePRPsplitnum == commandIndex){
            QLOG_DEBUG()<<"放杯复位丢试剂"<<GlobalData::mapIndexReagentnames(indexReag)<<"负压值"<<airvale;
            kpthrowCup->bcompleted = true;
            allThrowPRPFinish(preaginfo,isthrocupfinish,sendData);
            return isthrocupfinish;
        }

    }
    return isthrocupfinish;
}


void StructInstance::allThrowPRPFinish(const Testing_reagents *preagdata,bool &isthrocupfinish,QByteArray &outdata){
    auto itFinish = find_if(preagdata->pThrowCupsActive.cbegin(),preagdata->pThrowCupsActive.cend(),
                            find_allactivefinish(false));
    if(itFinish != preagdata->pThrowCupsActive.cend())
    {
         isthrocupfinish = false;
         outdata = (*itFinish)->arrcode;
    }
    else
    {
        isthrocupfinish = true;
        backorigintimes();
    }
}


bool StructInstance::judge_hadWaitTestReag(const int testSample,quint8 &waitTestRrag)
{
    bool bhand_waittest_reag = false;
    const auto *psampleInfo = rfindIdSample(testSample);
    if(!psampleInfo){
        bhand_waittest_reag = false;
        QLOG_DEBUG()<<"未找到样本ID"<< testSample <<"无待测试剂"<<__LINE__<<endl;
    }

    auto waittesTing = find_if(psampleInfo->ptestReagent_active.cbegin(),
                               psampleInfo->ptestReagent_active.cend(),
                                find_nottest(false));
    if(waittesTing == psampleInfo->ptestReagent_active.cend()){
         bhand_waittest_reag = false;
         QLOG_DEBUG()<<"未找到样本ID"<< testSample <<"无待测试剂"<<__LINE__<<endl;
    }else{
         waitTestRrag = (*waittesTing)->index_Reagent;
         bhand_waittest_reag = true;
         QString outText = QUIUtils::index_reagent_mapping_reagentName("还有待测试剂:",(*waittesTing)->index_Reagent);
         QLOG_WARN()<<"样本"<<testSample<<outText<<"状态"<<(*waittesTing)->testfinished<<__LINE__<<endl;
    }
    return bhand_waittest_reag;
}



bool StructInstance::alreadyadded_wait_chn(int &wait_sampleid)
{
    DATASAMPLESTRUCT *psampleId = NULL;
    auto it = m_BloodsampleInfo.begin();
    while(it != m_BloodsampleInfo.end())
    {
       psampleId = (*it);
       if(psampleId->bcleanbloody_state && psampleId->bcleanDoublePin_state && psampleId->indexTestingChn == -1)
       {
          wait_sampleid = psampleId->sample_num;
          QLOG_DEBUG()<<"已加样等待通道样本号:"<<wait_sampleid<<endl;
          return true;
       }
       it++;
    }
    return false;
}


bool StructInstance::backSampleAddState(const DATASAMPLESTRUCT *itsample,QByteArray &data){
   
    auto itaddsample = find_if(
            itsample->tadd_sample_active.cbegin(),
            itsample->tadd_sample_active.cend(), find_allactivefinish(false));
    if(itaddsample != itsample->tadd_sample_active.cend())
    {
        data = (*itaddsample)->arrcode;
        return true;
    }
    else {
        QLOG_DEBUG() << "样本" << itsample->sample_num << "加样已完成!";
        return false;
    }
    //return false;
}

bool StructInstance::backSampleTestPPP(const DATASAMPLESTRUCT *itsample, QByteArray &data){
    auto itaddsample = find_if(
            itsample->t_catchAnemiaTest.cbegin(),
            itsample->t_catchAnemiaTest.cend(),find_allactivefinish(false));
    if(itaddsample != itsample->t_catchAnemiaTest.cend())
    {
        data = (*itaddsample)->arrcode;
        return true;
    }
    else {
        QLOG_DEBUG() << "样本" << itsample->sample_num << "测试PPP已完成!";
        return false;
    }
}

bool StructInstance::backSampleThrowPPP(const DATASAMPLESTRUCT *itsample, QByteArray &data){
    auto itaddsample = find_if(itsample->t_throw_anemiacups_active.cbegin(),
                            itsample->t_throw_anemiacups_active.cend(),find_allactivefinish(false));
    if(itaddsample != itsample->t_throw_anemiacups_active.cend())
    {
        data = (*itaddsample)->arrcode;
        return true;
    }
    else {
        QLOG_DEBUG() << "样本" << itsample->sample_num << "丢PPP已完成!";
        return false;
    }
}

bool StructInstance::backTestReagentNotFinishArry(const DATASAMPLESTRUCT *itsample,
                                                  QByteArray &data,const quint8 testReagent){

    auto itindexReagent = find_if(itsample->ptestReagent_active.begin(),itsample->ptestReagent_active.end(),
                          findtest_testreagent(testReagent));

    if(itindexReagent == itsample->ptestReagent_active.end()){
        QLOG_ERROR()<<"样本"<<itsample->sample_num<<"测试试剂"<<testReagent<<"未找到信息";
        return false;
    }

    auto itTestActive = find_if((*itindexReagent)->pTestingReagentActive.cbegin(),
								(*itindexReagent)->pTestingReagentActive.cend(),find_allactivefinish(false));

	if (itTestActive == (*itindexReagent)->pTestingReagentActive.cend()) {
		QLOG_ERROR() << "样本" << itsample->sample_num << "测试试剂" << testReagent << "未找到吸试剂夹杯信息"
			<< (*itindexReagent)->index_Reagent;
		return false;
	}
	else {
		data = (*itTestActive)->arrcode;
		return true;
	}
}

bool StructInstance::backThrowCupsNotFinishArry(const DATASAMPLESTRUCT *itsample, QByteArray &data,const quint8 testReagent)
{
    auto itindexReagent = find_if(itsample->ptestReagent_active.begin(),itsample->ptestReagent_active.end(),
                          findtest_testreagent(testReagent));

    if(itindexReagent == itsample->ptestReagent_active.end()){
        QLOG_ERROR()<<"样本"<<itsample->sample_num<<"测试试剂"<<testReagent<<"未找到信息";
        return false;
    }

    auto itThrowActive = find_if((*itindexReagent)->pThrowCupsActive.cbegin(),
                                (*itindexReagent)->pThrowCupsActive.cend(),find_allactivefinish(false));

    if (itThrowActive == (*itindexReagent)->pThrowCupsActive.cend()) {
        QLOG_ERROR() << "样本" << itsample->sample_num << "测试试剂" << testReagent << "未找到丢杯信息"
            << (*itindexReagent)->index_Reagent;
        return false;
    }
    else {
        data = (*itThrowActive)->arrcode;
        return true;
    }
}
tSingleActive_list StructInstance::backSplitReagentComm(const DATASAMPLESTRUCT *itsample,
                                                    const quint8 testReagent){

    auto itindexReagent = find_if(itsample->ptestReagent_active.begin(),itsample->ptestReagent_active.end(),
                          findtest_testreagent(testReagent));

    if(itindexReagent == itsample->ptestReagent_active.end()){
        QLOG_ERROR()<<"样本"<<itsample->sample_num<<"测试试剂"<<testReagent<<"未找到信息";
		return {};
    }
	return (*itindexReagent)->pSpitReagentsActive;
}

tSingleActive_list StructInstance::backCleanReagentNeedleComm(const DATASAMPLESTRUCT *itsample,
                                                          const quint8 testReagent){

    auto itindexReagent = find_if(itsample->ptestReagent_active.begin(),itsample->ptestReagent_active.end(),
                          findtest_testreagent(testReagent));

    if(itindexReagent == itsample->ptestReagent_active.end()){
        QLOG_ERROR()<<"样本"<<itsample->sample_num<<"测试试剂"<<testReagent<<"未找到信息";
        return {};
    }
    return (*itindexReagent)->pCleanReagentActive;
}


QByteArrayList StructInstance::outputsIncompleteInstructionList(const int smapleId,const quint8 indexActive){
    const auto *itsample = rfindIdSample(smapleId);
    if(!itsample){
        QLOG_ERROR()<<"查找样本未完成指令样本号为空"<<"样本号"<<smapleId;
        return {};
    }
   
    tSingleActive_list continuttestact;

    switch(indexActive){
        case FOCUS_SPIT_AA_TESTING:
            continuttestact = backSplitReagentComm(itsample,AA_REAGENT);
        break;
        case FOCUS_SPIT_ADP_TESTING:
            continuttestact = backSplitReagentComm(itsample,ADP_REAGENT);
        break;
        case FOCUS_SPIT_EPI_TESTING:
            continuttestact = backSplitReagentComm(itsample,EPI_REAGENT);
        break;
        case FOCUS_SPIT_COL_TESTING:
            continuttestact = backSplitReagentComm(itsample,COL_REAGENT);
        break;
        case FOCUS_SPIT_RIS_TESTING:
            continuttestact = backSplitReagentComm(itsample,RIS_REAGENT);
        break;
        case FOCUS_CLEAN_DOUBLEPIN:
            continuttestact = itsample->clean_reag_blood_pin;
        break;
        case FOCUS_CLEAN_BLOODPIN:
            continuttestact = itsample->clean_blood_pin;
        break;
        case FOCUS_CLEAN_EREAGENTPIN_AA:
            continuttestact = backCleanReagentNeedleComm(itsample,AA_REAGENT);
        break;
        case FOCUS_CLEAN_EREAGENTPIN_ADP:
            continuttestact = backCleanReagentNeedleComm(itsample,ADP_REAGENT);
        break;
        case FOCUS_CLEAN_EREAGENTPIN_EPI:
            continuttestact = backCleanReagentNeedleComm(itsample,EPI_REAGENT);
        break;
        case FOCUS_CLEAN_EREAGENTPIN_COL:
            continuttestact = backCleanReagentNeedleComm(itsample,COL_REAGENT);
        break;
        case FOCUS_CLEAN_EREAGENTPIN_RIS:
            continuttestact = backCleanReagentNeedleComm(itsample,RIS_REAGENT);
        break;
        default:
                QLOG_ERROR() << "无效的indexActive值：" << indexActive;
            return {};
    }
    QByteArrayList outputDataList;
    for (const auto &item : continuttestact) {
        if (!item->bcompleted) {
            outputDataList.append(item->arrcode);
        }
    }
    return outputDataList;
}

QByteArray StructInstance::outputstheFirstIncompleteInstruction(const int smapleId,const quint8 indexActive){
   const auto *itsample = rfindIdSample(smapleId);
   if(!itsample){
       QLOG_ERROR()<<"查找样本未完成指令样本号为空"<<"样本号"<<smapleId;
       return {};
   }

   QByteArray outputData;
   bool hasPendingData = false;

   switch(indexActive){
        case FOCUS_ADDING_SAMPLE:
			hasPendingData =  backSampleAddState(itsample, outputData);
        break;
        case FOCUS_CLIP_ANEMIA_TO_CHN:
			hasPendingData = backSampleTestPPP(itsample, outputData);
        break;
        case FOCUS_THORW_ANEMIA:
			hasPendingData = backSampleThrowPPP(itsample, outputData);
        break;
        case FOCUS_TESTING_AA:
			hasPendingData = backTestReagentNotFinishArry(itsample, outputData,AA_REAGENT);
        break;
        case FOCUS_TESTING_ADP:
			hasPendingData = backTestReagentNotFinishArry(itsample, outputData,ADP_REAGENT);
        break;
       case FOCUS_TESTING_EPI:
		    hasPendingData = backTestReagentNotFinishArry(itsample, outputData,EPI_REAGENT);
       break;
       case FOCUS_TESTING_COL:
		   hasPendingData = backTestReagentNotFinishArry(itsample, outputData,COL_REAGENT);
       break;
       case FOCUS_TESTING_RIS:
		   hasPendingData = backTestReagentNotFinishArry(itsample, outputData,RIS_REAGENT);
       break;
       case FOCUS_THROW_AA_TUBE:
		   hasPendingData = backThrowCupsNotFinishArry(itsample, outputData,AA_REAGENT);
       break;
       case FOCUS_THROW_ADP_TUBE:
		   hasPendingData = backThrowCupsNotFinishArry(itsample, outputData,ADP_REAGENT);
       break;
       case FOCUS_THROW_EPI_TUBE:
		   hasPendingData = backThrowCupsNotFinishArry(itsample, outputData,EPI_REAGENT);
       break;
       case FOCUS_THROW_COL_TUBE:
		   hasPendingData = backThrowCupsNotFinishArry(itsample, outputData,COL_REAGENT);
       break;
       case FOCUS_THROW_RIS_TUBE:
		   hasPendingData = backThrowCupsNotFinishArry(itsample, outputData,RIS_REAGENT);
       break;
   }
   return hasPendingData ? outputData : QByteArray();
}






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void StructInstance::addEquipmentChn(const QList<quint8> &equipmentOperChn)
{
    for(quint8 chn = 0; chn < MACHINE_SETTING_CHANNEL; chn++)
    {
        TESTCHNSTAUSINFO *pChninfo = new TESTCHNSTAUSINFO;
        pChninfo->index_Chn = chn;
        pChninfo->Reagent = ANEMIA;

        if(equipmentOperChn.contains(chn + 1))
			pChninfo->Chn_Status = CHN_STATUS_FREE;
        else
            pChninfo->Chn_Status = CHN_STATUS_DISABLE;

        pChninfo->AA_testchndata_.reserve(300);
        pChninfo->AA_testchndata_.clear();
        pChninfo->ADP_testchndata_.reserve(300);
        pChninfo->ADP_testchndata_.clear();
        pChninfo->EPI_testchndata_.reserve(300);
        pChninfo->EPI_testchndata_.clear();
        pChninfo->COL_testchndata_.reserve(300);
        pChninfo->COL_testchndata_.clear();
        pChninfo->RIS_testchndata_.reserve(300);
        pChninfo->RIS_testchndata_.clear();
        m_testChnStructvec_.push_back(pChninfo);
    }
    return;
}
void StructInstance::config_testChn_State(const quint8 indexChn,quint8 ChnState)
{
    QVector<TESTCHNSTAUSINFO *>::iterator _it = find_if(m_testChnStructvec_.begin(),
                                                        m_testChnStructvec_.end(),
                                                        finder_Chn(indexChn));
    if(_it != m_testChnStructvec_.end())
    {
        TESTCHNSTAUSINFO *pChnInfo = (*_it);
        pChnInfo->Chn_Status = ChnState;
        QString outinfo = "";
        if(ChnState == CHN_STATUS_DISABLE)  outinfo = "禁用";
        else if(ChnState == CHN_STATUS_TESTING) outinfo ="测试中";
        else if(ChnState == CHN_STATUS_FINISHED) outinfo = "测试完成";
        else if(ChnState == CHN_STATUS_FREE) outinfo = "空闲中";
        //QLOG_DEBUG()<<"设置通道"<<indexChn + 1 <<"状态:"<<outinfo<<__FUNCTION__<<__LINE__;
    }
    else
    {
        QLOG_DEBUG()<<"未找到通道"<<indexChn<<"信息"<<endl;
    }
}

void StructInstance::config_testChn_test_reagent(const quint8 indexChn,const quint8 indexReag)
{
    QVector<TESTCHNSTAUSINFO *>::iterator _it = find_if(m_testChnStructvec_.begin(),m_testChnStructvec_.end(),finder_Chn(indexChn));
    if(_it != m_testChnStructvec_.end())
    {
        TESTCHNSTAUSINFO *pChnInfo = (*_it);
        pChnInfo->Reagent = indexReag;
        QLOG_DEBUG()<<"设置通道"<<indexChn<<QUIUtils::index_reagent_mapping_reagentName("测试试剂:",indexReag)<<endl;
    }
    else
    {
        QLOG_DEBUG()<<"未找到通道"<<indexChn<<"信息"<<endl;
    }
}

void StructInstance::config_testChn_test_SampleID(const quint8 indexChn,const int sampleID)
{
    QVector<TESTCHNSTAUSINFO *>::iterator _it = find_if(m_testChnStructvec_.begin(),m_testChnStructvec_.end(),finder_Chn(indexChn));
    if(_it != m_testChnStructvec_.end())
    {
        TESTCHNSTAUSINFO *pChnInfo = (*_it);
        pChnInfo->samplename = QString::number(sampleID);
        QLOG_DEBUG()<<"设置通道"<<indexChn<<"测试样本ID"<<sampleID<<__FILE__<<__LINE__<<endl;
    }
    else
    {
        QLOG_DEBUG()<<"未找到通道"<<indexChn<<"信息"<<endl;
    }
}


bool StructInstance::hadFreeTestChn(quint8 &indexChn_)
{
    auto it = std::find_if(m_testChnStructvec_.begin(), m_testChnStructvec_.end(),
            [](const TESTCHNSTAUSINFO* p) { return p->Chn_Status == CHN_STATUS_FREE; });

    if (it != m_testChnStructvec_.end()) {
            indexChn_ = (*it)->index_Chn;
            return true;
    }
    return false;
}



bool StructInstance::hadTestChnTestFinish(quint8 &indexChn_)
{
    // 使用 std::find_if 替代手动迭代器遍历
    const auto it = std::find_if(
        m_testChnStructvec_.cbegin(),
        m_testChnStructvec_.cend(),
        [](const TESTCHNSTAUSINFO* pChninfo) {
            return pChninfo->Chn_Status == CHN_STATUS_FINISHED;
        }
    );

    // 直接返回是否找到，并设置索引
    if (it != m_testChnStructvec_.cend()) {
        indexChn_ = (*it)->index_Chn;
        return true;
    }
    return false;
}




void  StructInstance::fetchTestDataTotal(QString samplename,quint8 index_reag,int &totalNum)
{
    totalNum = 0;

    QString sampleDate,idsampleNum;
    int sampleId;
    GlobalData::apartSampleId(samplename,sampleDate,sampleId);
    idsampleNum = QString::number(sampleId);


    // 优化后的代码实现
    auto findMatch = std::find_if(m_testChnStructvec_.cbegin(),
                                 m_testChnStructvec_.cend(),
                                 [&](const TESTCHNSTAUSINFO* pdata) {
        return pdata->samplename == idsampleNum &&
               index_reag == pdata->Reagent;
    });

	
    if (findMatch != m_testChnStructvec_.cend()) {
		switch (index_reag)
		{
			case AA_REAGENT:  totalNum = (*findMatch)->AA_testchndata_.size(); break;
			case ADP_REAGENT: totalNum = (*findMatch)->ADP_testchndata_.size(); break;
			case EPI_REAGENT: totalNum = (*findMatch)->EPI_testchndata_.size(); break;
			case COL_REAGENT: totalNum = (*findMatch)->COL_testchndata_.size();break;
			case RIS_REAGENT: totalNum = (*findMatch)->RIS_testchndata_.size();break;
			default:break;
		}
    }
}



void StructInstance::updte_saveChnTestData(const quint8 &IndexChannel,const quint8 &index_reag,
                                           const int &testValue,const float &testcalculatingData){

    const quint8 relChannel = IndexChannel - 1; //从0开始

    auto findMatch = std::find_if(m_testChnStructvec_.cbegin(),
                                 m_testChnStructvec_.cend(),
                                 [&](const TESTCHNSTAUSINFO* pdata) {
        return pdata->index_Chn == relChannel &&
               index_reag == pdata->Reagent;
    });

    if (findMatch != m_testChnStructvec_.cend()) {
        TESTCHNDATA *ptestingRelData = new TESTCHNDATA;
        ptestingRelData->raw_data = testValue;
        ptestingRelData->test_data = testcalculatingData;

        switch (index_reag)
        {
            case AA_REAGENT:
                ptestingRelData->index_ = (*findMatch)->AA_testchndata_.size() + 1;
				(*findMatch)->AA_testchndata_.push_back(ptestingRelData);
            break;
            case ADP_REAGENT:  
				ptestingRelData->index_ = (*findMatch)->ADP_testchndata_.size();
				(*findMatch)->ADP_testchndata_.push_back(ptestingRelData);
			break;
            case EPI_REAGENT:   
				ptestingRelData->index_ = (*findMatch)->EPI_testchndata_.size();
				(*findMatch)->EPI_testchndata_.push_back(ptestingRelData);
			break;
            case COL_REAGENT:   
				ptestingRelData->index_ = (*findMatch)->COL_testchndata_.size();
				(*findMatch)->COL_testchndata_.push_back(ptestingRelData);
			break;
            case RIS_REAGENT:   
				ptestingRelData->index_ = (*findMatch)->RIS_testchndata_.size();
				(*findMatch)->RIS_testchndata_.push_back(ptestingRelData);
			break;
            default:break;
        }
    }
}


void StructInstance::delalltaskinfo(const bool exitapp)
{
    for (auto iter = m_testChnStructvec_.begin(); iter != m_testChnStructvec_.end(); ++iter)
    {
        if ((*iter) != nullptr)
        {
            _delReagentData(&(*iter)->AA_testchndata_);
            QVector<TESTCHNDATA *>().swap((*iter)->AA_testchndata_);

            _delReagentData(&(*iter)->ADP_testchndata_);
            QVector<TESTCHNDATA *>().swap((*iter)->ADP_testchndata_);

            _delReagentData(&(*iter)->EPI_testchndata_);
            QVector<TESTCHNDATA *>().swap((*iter)->EPI_testchndata_);

            _delReagentData(&(*iter)->COL_testchndata_);
            QVector<TESTCHNDATA *>().swap((*iter)->COL_testchndata_);

            _delReagentData(&(*iter)->RIS_testchndata_);
            QVector<TESTCHNDATA *>().swap((*iter)->RIS_testchndata_);

            if (exitapp == true){
                delete (*iter);
                (*iter) = nullptr;
            }
            else{
                (*iter)->Reagent = ANEMIA;
                (*iter)->samplename.clear();
            }
        }
    }
    if (exitapp == true) {
        m_testChnStructvec_.erase(std::remove(m_testChnStructvec_.begin(),
                                              m_testChnStructvec_.end(), nullptr), m_testChnStructvec_.end());
        m_testChnStructvec_.clear();
        m_testChnStructvec_.shrink_to_fit();
    }


    //释放样本运动数据
	QList<int> delSampleid;
	delSampleid.clear();
    for (auto iter = m_BloodsampleInfo.begin(); iter != m_BloodsampleInfo.end(); ++iter){
		delSampleid.push_back((*iter)->sample_num);
    }
	for (int delId : delSampleid) {
		removeSampleFromContainer(m_BloodsampleInfo,delId);
	}
    return;
}


void StructInstance::creataddSampleCommands(const QByteArrayList addSamplecommand,DATASAMPLESTRUCT *&psampleinfo)
{
    for(const QByteArray& command : addSamplecommand) {
        if(command.isEmpty()) {
            QLOG_WARN() << "加样空指令数据，样本ID:" << psampleinfo->sample_num<<endl;
            continue;
         }
         QByteArray indexField = command.mid(GET_COMMAND_INDEX, 1);
         if(indexField.isEmpty()) {
             QLOG_WARN() << "加样指令索引字段缺失，指令数据:" << command;
             continue;
         }

         bool ok;
         QString hexStr = indexField.toHex();
         quint8 index_code = hexStr.toUShort(&ok, HEX_SWITCH); // 直接转换QByteArray
         if(!ok) {
             QLOG_WARN() << "加样无效的指令索引值:" << indexField << "原始指令:" << command;
             continue;
         }
         psampleinfo->tadd_sample_active.push_back(
             new single_action_commands(index_code, false, command));
         //QLOG_DEBUG()<<"加样指令编号"<<index_code;
    }
    //QLOG_DEBUG()<<"样本"<<psampleinfo->sample_num<<"加样动作指令长度"<< psampleinfo->tadd_sample_active.size()<<endl;
}

QByteArray StructInstance::recvAddSampleIndexCommand(const int sampleId, const quint8 index,bool &completedAddSample)
{
    auto *pIdsample = wfindIdSample(sampleId);
    if(pIdsample == nullptr) return {};

    auto actionIt = find_if(pIdsample->tadd_sample_active.begin(),
                            pIdsample->tadd_sample_active.end(),
                            find_recvIndex(index));
    if (actionIt == pIdsample->tadd_sample_active.end() || (*actionIt) == nullptr) {
        QLOG_DEBUG() << QString("样本id[%1]加样未找到动作编号[%2]").arg(sampleId).arg(index);
        completedAddSample = false;
        return {};
    }

    (*actionIt)->bcompleted = true;

    auto finishit = find_if(pIdsample->tadd_sample_active.cbegin(),
                            pIdsample->tadd_sample_active.cend(), find_finishall(false));
    if (finishit == pIdsample->tadd_sample_active.cend()) {
        QLOG_DEBUG() << "样本" << sampleId << "收加样指令" << index << "加样完毕状态完成;" << __LINE__ << endl;
        completedAddSample = true;
        return {};
    }
    else {
        completedAddSample = false;
        return (*finishit)->arrcode;
    }
    return {};
}


bool  StructInstance::Recv_addsampleData(const int sampleId,const quint8 index)
{
    auto sampleIt = find_if(m_BloodsampleInfo.begin(),m_BloodsampleInfo.end(),find_id(sampleId));
    if (sampleIt == m_BloodsampleInfo.end()) {
        QLOG_DEBUG() << QString("样本id[%1]未找到(加样)").arg(sampleId);
        return false;
    }

    DATASAMPLESTRUCT*  psampleinfo = *sampleIt;

    auto actionIt = find_if(psampleinfo->tadd_sample_active.begin(),
        psampleinfo->tadd_sample_active.end(),
        find_recvIndex(index));

    if (actionIt == psampleinfo->tadd_sample_active.end()) {
        QLOG_DEBUG() << QString("样本id[%1]加样未找到动作编号[%2]").arg(sampleId).arg(index);
        return false;
    }

    single_action_commands* psingleactive = *actionIt;
    psingleactive->bcompleted = true;
    //QLOG_DEBUG()<<"收加样指令编号"<<index;

    auto itFinish = find_if(psampleinfo->tadd_sample_active.begin(),
                    psampleinfo->tadd_sample_active.end(), find_finishall(false));
    if (itFinish == psampleinfo->tadd_sample_active.end()) {
        QLOG_DEBUG() << "样本" << sampleId << "收加样指令" << index << "加样完毕状态完成;" <<__LINE__<< endl;
        return true;
    }
    return  false;
}




void StructInstance::creatSuckPPP2theTestChannel(const int sampleId, const QVector<QByteArray> CatchAnemiacommand,
                                                   const quint8 isSuctionPPPreset,const quint8 isSpitPPPreset)
{
    auto *pitSample = wfindIdSample(sampleId);
    if(!pitSample){
        QLOG_ERROR() << "未找到样本ID" << sampleId << "创建夹贫血到通道指令失败"<<endl;
        return;
    }
    pitSample->suctionPPPreset = isSuctionPPPreset;
    pitSample->spitPPPreset = isSpitPPPreset;

   for(const QByteArray& command : CatchAnemiacommand) {
       if(command.isEmpty()) {
           QLOG_WARN() << "接收到空指令数据，样本ID:" << sampleId<<endl;
           continue;
        }
        QByteArray indexField = command.mid(GET_COMMAND_INDEX, 1);
        if(indexField.isEmpty()) {
            QLOG_WARN() << "指令索引字段缺失，指令数据:" << command;
            continue;
        }

        bool ok;
        QString hexStr = indexField.toHex();
        quint8 index_code = hexStr.toUShort(&ok, HEX_SWITCH); // 直接转换QByteArray
        if(!ok) { // 优化点4：增加转换校验
            QLOG_WARN() << "无效的指令索引值:" << indexField << "原始指令:" << command;
            continue;
        }
		pitSample->t_catchAnemiaTest.push_back(new single_action_commands(index_code, false, command));
   }
}

QByteArray StructInstance::sendPPP2ChannelHeanderCommand(const int sampleId, bool isSetAllFalse)
{
    // 一级指针有效性检查
    auto* itsample = wfindIdSample(sampleId);
    if (!itsample) return QByteArray();


    // 二级引用简化访问
    auto& sampleRef = *itsample;
    auto& cupsContainer = sampleRef.t_catchAnemiaTest;

    // 条件修改逻辑
   if (isSetAllFalse) {
       for (auto it = cupsContainer.begin(); it != cupsContainer.end(); ++it) {
           if (*it) {
               (*it)->bcompleted = false;
           }
       }
   }
   // 前置空容器检查
   if (cupsContainer.empty()) {
       return QByteArray();
   }
   return cupsContainer.front() ? cupsContainer.front()->arrcode : QByteArray();
}






///丟测试通道内的杯子根据试剂号知道
void StructInstance::creatThrowCupsCommands(const int sampleId,const quint8 indexChn)
{
    QVector<QByteArray> throwCups;
    quint8 sucknum,splitnum;
    QUIUtils::ThrowTestChnCup(indexChn, throwCups,sucknum,splitnum);

    // 通过谓词查找样本信息
    const auto itSample = std::find_if(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
        [sampleId](const DATASAMPLESTRUCT* pSample) {
        return pSample->sample_num == sampleId;
    });

    if (itSample == m_BloodsampleInfo.cend()) {
        QLOG_ERROR() << "样本ID" << sampleId << "未找到，无法创建丢杯指令";
        return; // 提前返回避免空指针风险
    }

    DATASAMPLESTRUCT* const pSampleInfo = *itSample;
    pSampleInfo->throwPPPsuckBacknum = sucknum;
    pSampleInfo->throwPPPsplitBacknum = splitnum;

    // 预分配内存避免多次扩容
    pSampleInfo->t_throw_anemiacups_active.reserve(throwCups.size());

    for (const QByteArray& cmd : throwCups) {
        if (cmd.isEmpty()) {
            QLOG_WARN() << "丟PPP空指令数据，样本ID:" << sampleId << endl;
            continue;
        }

         QByteArray indexField = cmd.mid(GET_COMMAND_INDEX, 1);
         if (indexField.isEmpty()) {
             QLOG_WARN() << "指令索引字段缺失，指令数据:" << cmd;
             continue;
         }

         bool ok;
         QString hexStr = indexField.toHex();
         quint8 index_code = hexStr.toUShort(&ok, HEX_SWITCH); // 直接转换QByteArray
         if (!ok) {
             QLOG_WARN() << "丟PPP无效的指令索引值:" << indexField << "原始指令:" << cmd;
             continue;
         }

         pSampleInfo->t_throw_anemiacups_active.push_back(
             new single_action_commands(index_code, false, cmd)
        );
    }
}


void StructInstance::throwPPPFailed(const int sampleId,bool &kGripperFailed){
    backorigintimes();
    kGripperFailed = GRIPPERMOVE_ABNORMALLY;
    outputHeadnThrowCups(sampleId,GRIPPERMOVE_ABNORMALLY);
    return;
}

bool StructInstance::recv_throwCupcommd(const int sampleId,const quint8 index_code,bool HandsControl,bool isSuction,
                                          quint32 airvale,QByteArray &sendData,bool &kGripperFailed,bool &laterTimer){

    auto *pIdsample = wfindIdSample(sampleId);
    if (!pIdsample){
        logError(QString("Invalid sample ID: %1").arg(sampleId));
        return false;
    }

    quint8 kthrowChannel = pIdsample->indexTestingChn;
    auto kPPPthrow_it = find_if(pIdsample->t_throw_anemiacups_active.begin(),
                                pIdsample->t_throw_anemiacups_active.end(),
                                recv_index(index_code));
    if(kPPPthrow_it == pIdsample->t_throw_anemiacups_active.end() || (*kPPPthrow_it) == nullptr){
        QLOG_ERROR()<<"丟PPP样本"<< sampleId <<"或者丟PPP任务为空";
        return false;
    }

    single_action_commands *kpsampleThrowActive = *kPPPthrow_it;
    bool isthrowFinish = false;


    if(!HandsControl){
        kpsampleThrowActive->bcompleted = true;
        allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
        return isthrowFinish;
    }

    if(isSuction)
    {
        bool issuccess = GlobalData::isHandsSuckFinish(airvale);
        const quint8 suckBackOriginnum = pIdsample->throwPPPsuckBacknum;
        //吸动作复位负压值未到区间
        if(suckBackOriginnum == index_code && !issuccess){
            throwPPPFailed(sampleId,kGripperFailed);
            QLOG_DEBUG() << "丢PPP动作失败!";
            return false;
        }else if(suckBackOriginnum == index_code && issuccess){
            QLOG_DEBUG()<<"吸杯复位[丢PPP动作]的负压值"<<airvale;
            kpsampleThrowActive->bcompleted = true;
            allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
            return isthrowFinish;
        }
        //舍弃值
        const quint8 firstGrippersuck = suckBackOriginnum - 2;
        if(firstGrippersuck == index_code){
            kpsampleThrowActive->bcompleted = true;
            laterTimer = true;
            QLOG_DEBUG()<<"舍弃初次吸[丢PPP动作]的负压值"<<airvale;
            allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
            return isthrowFinish;
        }

        //吸杯成功？动作
        const quint8 suckPPPnum = pIdsample->throwPPPsuckBacknum - 1;
        if(suckPPPnum == index_code){
            bool success =  GlobalData::isHandsSuckFinish(airvale);
            int& retries =  m_handsFailedtimes;
            const char* logMsg =  "吸PPP试杯丟失败次数" ;
            const char* failMsg =  "吸杯3次失败";
            const int maxRetries = 3;  // 统一最大重试次数

            if (!success) {
                if (retries <= maxRetries) {
                    quint8 numindex = kpsampleThrowActive->index_;
                    sendData =  GlobalData::Grabthehandtotryagain(numindex, ++retries, kthrowChannel);
                    QLOG_WARN() << logMsg << retries << endl;
                    return false;
                } else {
                    throwPPPFailed(sampleId,kGripperFailed);
                    QLOG_ERROR()<<failMsg;
                    return false;
                }
            }else{
                 kpsampleThrowActive->bcompleted = true;
                 allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
                 return isthrowFinish;
            }
        }
    }else{
        if(pIdsample->throwPPPsplitBacknum == index_code){
            QLOG_DEBUG()<<"放杯复位[丢PPP动作]的负压值"<<airvale;
            kpsampleThrowActive->bcompleted = true;
            allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
            return isthrowFinish;
        }

        const quint8 firstGripperSplit = pIdsample->throwPPPsplitBacknum - 2;
        if(firstGripperSplit == index_code){
            kpsampleThrowActive->bcompleted = true;
            QLOG_DEBUG()<<"舍弃初次放[丢PPP动作]的负压值"<<airvale;
            allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
            return isthrowFinish;
        }

        const quint8 splitPPPnum = pIdsample->throwPPPsplitBacknum - 1;
        if(splitPPPnum == index_code){
            if(GlobalData::isHandsThrowCupFinish(airvale)){
                kpsampleThrowActive->bcompleted = true;
            }
            QLOG_DEBUG()<<"等待放[丢PPP动作]的负压值"<<airvale;
            //sendData = GlobalData::ThrowCupstotryagain(index_code, 3);
            allGripperThrowFinish(pIdsample,isthrowFinish,sendData);
            return isthrowFinish;
        }
    }

   return isthrowFinish;
}

void StructInstance::allGripperThrowFinish(const DATASAMPLESTRUCT *psampledata,bool &isthrowFinish,QByteArray &outdata){
    auto iter = find_if(psampledata->t_throw_anemiacups_active.cbegin(),psampledata->t_throw_anemiacups_active.cend(),
                        find_allactivefinish(false));
    if(iter != psampledata->t_throw_anemiacups_active.cend()){
        outdata = (*iter)->arrcode;
        isthrowFinish = false;
    }else{
        backorigintimes();
        isthrowFinish = true;
        QLOG_DEBUG()<<"完成丢贫血杯"<<endl;
    }
}


QByteArray StructInstance::outputHeadnThrowCups(const int sampleId, bool isSetAllFalse)
{
    // 一级指针有效性检查
	auto* itsample = wfindIdSample(sampleId);
	if (!itsample) return QByteArray();

   
    // 二级引用简化访问
    auto& sampleRef = *itsample;
    auto& cupsContainer = sampleRef.t_throw_anemiacups_active;

    // 条件修改逻辑
   if (isSetAllFalse) {
       for (auto it = cupsContainer.begin(); it != cupsContainer.end(); ++it) {
		   if (*it) {
			   (*it)->bcompleted = false;
		   }
       }
   }
   // 前置空容器检查
   if (cupsContainer.empty()) {
       return QByteArray();
   }
   return cupsContainer.front() ? cupsContainer.front()->arrcode : QByteArray();

}




void StructInstance::cancel_sample_task(QList<int> &sample_tasklist,QList<int> &sampleid)
{
    DATASAMPLESTRUCT *psampleinfo = NULL;
    auto iter = m_BloodsampleInfo.constBegin();
    while(iter != m_BloodsampleInfo.constEnd())
    {
        psampleinfo = (*iter);
        if(psampleinfo->bstartTesting == false && psampleinfo->indexTestingChn == -1)
        {
            sample_tasklist.push_back(psampleinfo->suckanaemia_hole);
            sampleid.push_back(psampleinfo->sample_num);
        }
        iter++;
    }
}

//根据空试管号 返回样本id
int StructInstance::sycn_emptytube_get_sampleid(quint8 indexhole)
{
    int sampleid = 0;
    DATASAMPLESTRUCT *psampleinfo = NULL;
    auto iter = m_BloodsampleInfo.begin();
    if(iter != m_BloodsampleInfo.end())
    {
        psampleinfo = (*iter);
        if(psampleinfo->spitanaemia_hole == indexhole)
        {
            sampleid = psampleinfo->sample_num;
            return sampleid;
        }
        else
        {
            for(int k = 0; k < psampleinfo->bloody_tube.size(); k++){
                if(psampleinfo->bloody_tube.at(k).bloody_hole == indexhole){
                    sampleid = psampleinfo->sample_num;
                    break;
                }
            }
        }
        iter++;
    }
    return sampleid;
}


QByteArray StructInstance::resettheCupDropAnimation(const int sampleId,
                                                    const quint8 indexReag,
                                                    const std::atomic<bool> &ispendingtime,
                                                    const bool &isSendSuckCup){

	std::lock_guard<std::mutex> lock(m_reagentMutex); // 线程安全
   
    auto *itSample = wfindIdSample(sampleId);
	if (!itSample) { 
		QLOG_DEBUG() << "样本ID" << sampleId << "查找失败";
		return{}; 
	}

    const bool PendActive = ispendingtime.load();
	QLOG_DEBUG() << "原子量使用" << PendActive;
    auto itReagent = std::find_if(itSample->ptestReagent_active.begin(),
								  itSample->ptestReagent_active.end(),
								  findtest_testreagent(indexReag));

    if(itReagent == itSample->ptestReagent_active.end() || !*itReagent){
        QLOG_DEBUG()<< "样本ID" << sampleId<<"抓PRP失败重置动作失败";
        return {};
    }
	// 分支语义常量
	constexpr uint8_t FULL_RETEST_MASK = 0b11; // PendActive=true, isSendSuckCup=true
	constexpr uint8_t PARTIAL_RESET_MASK = 0b00; // PendActive=false, isSendSuckCup=false
	constexpr uint8_t SINGLE_ACTION_MASK = 0b01; // PendActive=false, isSendSuckCup=true
	//const uint8_t branchFlag = (PendActive << 1) | isSendSuckCup;
	const uint8_t branchFlag = (PendActive << 1) | static_cast<uint8_t>(isSendSuckCup);

	switch (branchFlag) {
	case FULL_RETEST_MASK:
		for (auto &arrdatastu : (*itReagent)->pTestingReagentActive)
		{
			arrdatastu->bcompleted = false;
		}
		QLOG_DEBUG() << "试剂" << indexReag << "挂起重测";
		return (*itReagent)->pTestingReagentActive.front()->arrcode;

	case PARTIAL_RESET_MASK: {
			quint8 startIndex = (*itReagent)->suckCupIndexCommand;
			for (auto &arrdatastu : (*itReagent)->pTestingReagentActive) {
				if (arrdatastu->index_ >= startIndex) {
					arrdatastu->bcompleted = false;
				}
			}
			break;
		}
	case SINGLE_ACTION_MASK: {
			quint8 startIndex = (*itReagent)->suckCupIndexCommand;
			auto target = std::find_if((*itReagent)->pTestingReagentActive.begin(),
				(*itReagent)->pTestingReagentActive.end(),
				[startIndex](const auto& item) {
				return item->index_ == startIndex;
			});
			if (target != (*itReagent)->pTestingReagentActive.end()) {
				return (*target)->arrcode;
			}
			break;
		}
	default:
		Q_ASSERT_X(false, "resettheCupDropAnimation", "未处理分支");
	}
    return {};
}



void  StructInstance::backorigintimes(){
     m_handsFailedtimes = 0; //抓手吸杯失败次数
     m_handsThrowFailedtimes = 0; //助手丢被失败次数
}






const DATASAMPLESTRUCT *StructInstance::rfindIdSample(const int idSample){
    const auto itSample = std::find_if(m_BloodsampleInfo.cbegin(), m_BloodsampleInfo.cend(),
        [idSample](const DATASAMPLESTRUCT* pSample) {
        return   pSample != nullptr &&  pSample->sample_num == idSample;
    });

    if (itSample == m_BloodsampleInfo.cend()) {
        QLOG_ERROR() << "样本ID" << idSample << "未找到指令结构";
        return nullptr; // 提前返回避免空指针风险
    }
    return (*itSample);
}

DATASAMPLESTRUCT *StructInstance::wfindIdSample(const int idSample){
    auto itSample = std::find_if(m_BloodsampleInfo.begin(), m_BloodsampleInfo.end(),
        [idSample](const DATASAMPLESTRUCT* pSample) {
        return   pSample != nullptr &&  pSample->sample_num == idSample;
    });

    if (itSample == m_BloodsampleInfo.end()) {
        QLOG_ERROR() << "样本ID" << idSample << "未找到指令结构";
        return nullptr; // 提前返回避免空指针风险
    }
    return (*itSample);
}

void StructInstance::labelCancelTubes(const int sampleid,QList<quint8> &outCancelTube){
    outCancelTube.clear();
    const auto *itsample = rfindIdSample(sampleid);
    outCancelTube.append(itsample->spitanaemia_hole);
    for(BLOODY_INFO  holedata : itsample->bloody_tube){
        outCancelTube.append(holedata.bloody_hole);
    }
    QLOG_WARN()<<"放弃样本"<<sampleid<<"血样试管位置"<<outCancelTube;
}





bool StructInstance::processPPPCupToChannel(const int sampleId,const quint8 index,bool HandsControl,
                                              bool isSuction, quint32 airvale,
                                              QByteArray &sendData,bool &kGripperFailed,bool &lateTimer){

    bool isMovePPPComplete = false;
    auto *itsample = wfindIdSample(sampleId);
    if(!itsample){
        QLOG_ERROR() << "[SendPPP] 未找到样本ID" << sampleId
                     << "或指针无效，收到夹贫血到通道指令失败"<<endl;
        return isMovePPPComplete;
    }


    auto itIndex = find_if(itsample->t_catchAnemiaTest.cbegin(),
                           itsample->t_catchAnemiaTest.cend(),
                           recv_index(index));
    if (itIndex == itsample->t_catchAnemiaTest.cend() || *itIndex == nullptr) {
        QLOG_ERROR() << "[SendPPP] 未找到样本ID" << sampleId << "指令号"
                     <<index<< "或指针无效，收到夹贫血到通道指令失败";
        return isMovePPPComplete;
    }

    quint8 putChannel = itsample->indexTestingChn;
    quint8 ppphole = itsample->spitanaemia_hole;
    const int MAX_RETRY_COUNT = 3;

    if(!HandsControl){
        (*itIndex)->bcompleted = true;
        outsuckPPPOtherOrder(itsample,isMovePPPComplete,sendData);
        return isMovePPPComplete;
    }

    if(isSuction){
        //吸杯动作复位负压值检测
        const quint8 ksuckPPPBacknum = itsample->suctionPPPreset;
        bool  issuckBackSucess =  GlobalData::isHandsSuckFinish(airvale);
        if(ksuckPPPBacknum == index && !issuckBackSucess){
            kGripperFailed = GRIPPERMOVE_ABNORMALLY;
            backorigintimes();
            sendPPP2ChannelHeanderCommand(sampleId,GRIPPERMOVE_ABNORMALLY);
            QLOG_DEBUG() <<"吸PPP杯复位失败"<<endl;
            return false;
        }
        else if (ksuckPPPBacknum == index && issuckBackSucess){
            QLOG_DEBUG()<<"吸杯复位[测试PPP动作]的负压值"<<airvale;
            (*itIndex)->bcompleted = true;
            outsuckPPPOtherOrder(itsample,isMovePPPComplete,sendData);
            return isMovePPPComplete;
        }

        //第一次吸杯的值舍弃
        const quint8 firsesuckVal = itsample->suctionPPPreset - 2;
        if(index == firsesuckVal){
            (*itIndex)->bcompleted = true;
            lateTimer = true;
            QLOG_DEBUG()<<"吸PPP舍弃初次负压值"<<airvale;
            outsuckPPPOtherOrder(itsample,isMovePPPComplete,sendData);
            return isMovePPPComplete;
        }

        const quint8 suckCupNum = itsample->suctionPPPreset - 1;
        if(suckCupNum == index){
            bool success =  GlobalData::isHandsSuckFinish(airvale);
            int& retries = m_handsFailedtimes;
            const int maxRetries =  MAX_RETRY_COUNT ;
            if(success){
                (*itIndex)->bcompleted = true;
                outsuckPPPOtherOrder(itsample, isMovePPPComplete, sendData);
                return isMovePPPComplete;
            }else if(retries <= maxRetries){
                quint8 numindex = (*itIndex)->index_;
                retries++;
                sendData =  GlobalData::GrabtheCupAttheTestTubeTray(numindex, retries, ppphole);
                return  false;
            }else {
                kGripperFailed = GRIPPERMOVE_ABNORMALLY;
                backorigintimes();
                sendPPP2ChannelHeanderCommand(sampleId,GRIPPERMOVE_ABNORMALLY);
                QLOG_DEBUG() << "吸PPP杯3次失败" <<endl;
                return false;
            }
        }

    }else{

        if(itsample->spitPPPreset == index){
            QLOG_DEBUG()<<"放杯复位[测试PPP动作]的负压值"<<airvale;
            (*itIndex)->bcompleted = true;
            outsuckPPPOtherOrder(itsample,isMovePPPComplete,sendData);
            return isMovePPPComplete;
        }

        //等待释放负压
        const quint8 splitCupnum = itsample->spitPPPreset - 1;
        if(splitCupnum == index){
            if (GlobalData::isHandsThrowCupFinish(airvale)) {
                (*itIndex)->bcompleted = true;
            }
            outsuckPPPOtherOrder(itsample,isMovePPPComplete,sendData);
            return isMovePPPComplete;
        }

        //吐值舍弃初次
        const quint8 firstsplitnum = itsample->spitPPPreset - 2;
        if(index == firstsplitnum){
            (*itIndex)->bcompleted = true;
            QLOG_DEBUG()<<"吐PPP舍弃初次负压值"<<airvale;
            outsuckPPPOtherOrder(itsample,isMovePPPComplete,sendData);
            return isMovePPPComplete;
        }
    }
    return isMovePPPComplete;
}

void StructInstance::outsuckPPPOtherOrder(const DATASAMPLESTRUCT * psampledata,bool &allFinished,QByteArray &outdata){
    auto itfinish = find_if(psampledata->t_catchAnemiaTest.cbegin(),psampledata->t_catchAnemiaTest.cend(),
                            find_allactivefinish(false));
    if(itfinish == psampledata->t_catchAnemiaTest.cend())
    {
        allFinished = true;
        backorigintimes();
        QLOG_DEBUG()<<"样本"<<psampledata->sample_num<<"移动PPP到测试通道"
                    <<psampledata->indexTestingChn<<"完成"<<endl;
    }
    else
    {
       allFinished = false;
       outdata = (*itfinish)->arrcode;
    }
}



