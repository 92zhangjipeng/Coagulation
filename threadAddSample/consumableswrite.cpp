#pragma execution_character_set("utf-8")

#include "consumableswrite.h"
#include <customcreatsql.h>
#include <custom_struct.h>
#include <QUIUtils.h>
#include <loadequipmentpos.h>
#include <QCoreApplication>
#include <globaldata.h>
#include <operclass/fullyautomatedplatelets.h>

ConsumablesWrite::ConsumablesWrite(QObject *parent) : QObject(parent)
{
    moveToThread(&m_thread);

    QObject::connect(&m_thread,&QThread::started,this,&ConsumablesWrite::_pthreadoutstart);

    mdataarry_.clear();

    m_boot = false;
}

ConsumablesWrite::~ConsumablesWrite()
{
    qDeleteAll(mdataarry_);
    mdataarry_.clear();

    m_thread.quit();
    m_thread.wait();
}

void ConsumablesWrite::Start()
{
    if(!m_thread.isRunning())
        m_thread.start();
}


void ConsumablesWrite::_pthreadoutstart()
{
    QLOG_DEBUG()<<"ConsumablesWrite"<<QThread::currentThreadId();
}

void ConsumablesWrite::_slotsendcodeList(const QByteArrayList _data, QString outtext)
{
    emit _sendDirectivesList(_data,outtext);
    QLOG_DEBUG()<<"设置界面 主板参数写入";
}

void ConsumablesWrite::handlcloseEquipmentconsumables()
{
    if(cglobal::gserialConnecStatus){
        QByteArray buffer =  GlobalData::writeBuffer2Equipment();
        emit _sendDirectives(buffer,"写入缓存耗材数据");
    }

    QThread::msleep(100);
    _shutdownWithError();
    return;




    // 定义静态常量索引列表，避免重复初始化
    static const QList<quint8> kConsumableIndices = {
        // 左侧试剂索引
        INDEX_AA_CONSUMABLE,
        INDEX_ADP_CONSUMABLE,
        INDEX_EPI_CONSUMABLE,
        INDEX_COL_CONSUMABLE,
        INDEX_RIS_CONSUMABLE,
        // 右侧试剂索引
        INDEX_AA_1_CONSUMABLE,
        INDEX_ADP_1_CONSUMABLE,
        INDEX_EPI_1_CONSUMABLE,
        INDEX_COL_1_CONSUMABLE,
        INDEX_RIS_1_CONSUMABLE,
        // 其他消耗品
        INDEX_CLEANLINQUE_CONSUMABLE,
        INDEX_TESTTUBE_CONSUMABLE,
        OUTSIDE_CLEANLINQUE_S2
    };

    // 遍历所有索引生成数据
    for (const auto& index : kConsumableIndices) {
        const QByteArray configData = _PoweronandoffconfigSupplies(index);
        if (!configData.isEmpty()) {
            _insterSTUtudata(configData, index, index);
        }
    }

    // 发送数据（假设只需发送第一个条目）
    if (!mdataarry_.isEmpty()) {
        emit _sendDirectives(mdataarry_.first()->data_, "发送配置试剂");
    }
    return;
}







QByteArray ConsumablesWrite::_PoweronandoffconfigSupplies(quint8 reagentIndex)
{
    QByteArray outputData;
    QString consumableName;

    // 获取试剂名称（日志用）
    QUIUtils::_mapConsumablesName(reagentIndex, consumableName);

    // 定义索引映射表（右侧试剂 -> 左侧试剂）
    static const QHash<quint8, quint8> kRightToLeftIndexMap = {
        {INDEX_AA_1_CONSUMABLE,  INDEX_AA_CONSUMABLE},
        {INDEX_ADP_1_CONSUMABLE, INDEX_ADP_CONSUMABLE},
        {INDEX_EPI_1_CONSUMABLE, INDEX_EPI_CONSUMABLE},
        {INDEX_COL_1_CONSUMABLE, INDEX_COL_CONSUMABLE},
        {INDEX_RIS_1_CONSUMABLE, INDEX_RIS_CONSUMABLE}
    };

    // 确定实际操作的索引
    const quint8 targetIndex = kRightToLeftIndexMap.value(reagentIndex, reagentIndex);

    // 获取操作器实例
    ConsumablesOper* pOper = ConsumablesOper::GetpInstance();

    // 读取瓶数
    quint8 bottleCount = 0;
    pOper->syacnReagentTotalBottle(READ_OPERRAT, targetIndex, bottleCount);

    // 读取余量比
    quint16 remainingRatio = 0;
    pOper->updateReplaceLocRatio(READ_OPERRAT, reagentIndex, remainingRatio);

    // 生成控制指令
   QUIUtils::writeSuppliesBottleControlOrder(outputData, reagentIndex, reagentIndex,
                                             bottleCount, remainingRatio,false
   );

   // 记录日志
    QLOG_DEBUG() << consumableName << "写入余量比:" << remainingRatio
                   << "瓶数:" << bottleCount;

   return outputData;
}



void ConsumablesWrite::_insterSTUtudata(const QByteArray lossCleanarry,quint8 indexcode_,quint8 indexreag)
{
    ConsumablesStu* pdata = new ConsumablesStu;
    pdata->data_ = lossCleanarry;
    pdata->index_num = indexcode_;
    pdata->index_reag = indexreag;
    bool _hadpara = false;
    auto iter = mdataarry_.begin();
    while(iter != mdataarry_.end())
    {
        if((*iter)->index_num == indexcode_)
        {
            _hadpara = true;
            break;
        }
        iter++;
    }
    if(!_hadpara)
    {
        mdataarry_.push_back(pdata);
    }
    else
    {
        delete pdata;
        pdata = nullptr;
        QLOG_WARN()<<"开关机要配置的耗材数已存在待发送列表中(命令号):"<<indexcode_<<endl;
    }

    return;
}

//统一关闭逻辑
void ConsumablesWrite::_shutdownWithError()
{
    emit closeSerial();
    emit _ShutdownApp();
}
// 专用充值状态判断函数
bool ConsumablesWrite::isRechargeSuccessful(const CardAnalysisResult& result)
{
    // 有效状态码范围检查
    if(result.statusCode > 0x06) {
        QLOG_WARN() << "非法充值状态码:" << result.statusCode;
        return false;
    }

    // 状态码定义（与之前映射表一致）
    const quint8 RECHARGE_SUCCESS = 0x05; // 0101
    const quint8 RECHARGE_FAILURE = 0x06;  // 0110

    switch(result.statusCode) {
    case RECHARGE_SUCCESS:
        QLOG_INFO() << "充值成功 | 类型:" << result.dataType;
        return true;
    case RECHARGE_FAILURE:
        QLOG_ERROR() << "充值失败 | 错误码:" << result.statusCode;
        return false;
    default:
        QLOG_DEBUG() << "非充值操作状态:" << result.workStatus;
        return false;
    }
}

// 状态解析函数
CardAnalysisResult ConsumablesWrite::analyzeCardData(quint8 byteData)
{
    // 位域分离
    const quint8 statusCode = byteData & 0x0F;      // 低4位：工作状态
    const quint8 typeCode = (byteData >> 4) & 0x0F; // 高4位：数据类型

    // 工作状态映射表
    static const QMap<quint8, QString> statusMap = {
        {0x00, "正常"},        // 0000
        {0x01, "更新耗材总量"}, // 0001
        {0x02, "读卡成功"},     // 0010
        {0x03, "读卡失败"},     // 0011
        {0x04, "刷卡异常"},     // 0100
        {0x05, "充值成功"},     // 0101
        {0x06, "充值失败"},     // 0110
        // 0x07-0x0F 保留
    };

    // 数据类型映射表
    static const QMap<quint8, QString> typeMap = {
        {0x00, "AA试剂"},      // 0000
        {0x01, "ADP试剂"},     // 0001
        {0x02, "EPI试剂"},     // 0010
        {0x03, "COL试剂"},     // 0011
        {0x04, "RIS试剂"},     // 0100
        {0x05, "内部清洗剂"},   // 0101
        {0x06, "试管"},        // 0110（注意：外部清洗液需要确认编码）
        {0x07, "外部清洗液"},   // 0111（修正编码）
        // 0x08-0x0F 保留
    };

    return {
        statusMap.contains(statusCode) ?
            statusMap[statusCode] : "未知状态",
        typeMap.contains(typeCode) ?
            typeMap[typeCode] : "未知类型",
        statusCode,
        typeCode
    };
}


void ConsumablesWrite::_recvConsumableswriteState(QStringList recvdata_)
{
    if (mdataarry_.isEmpty()) return;

    // 基础校验
    if (recvdata_.size() < 13) {
        QLOG_ERROR() << "Invalid data format. Expected 13 elements, got" << recvdata_.size();
        emit _shutdownWithError();
        return;
    }

    // 提取基础信息
    int num_ = recvdata_.at(2).toInt(nullptr, HEX_SWITCH);
    QLOG_DEBUG() << "耗材收主板编号消息:" << num_;


    quint8 receivedData = recvdata_.at(8).toInt(nullptr, HEX_SWITCH);
    auto result = analyzeCardData(receivedData);


    // 状态检测
    bool configFinished = isRechargeSuccessful(result);
    QLOG_DEBUG() << "(瓶)配置状态 | 类型:" << result.dataType << "| 完成:" << result.workStatus
                 <<"|配置瓶数"<<recvdata_.at(9).toInt(nullptr, HEX_SWITCH)<<endl;

    quint8 ratioofBottle = recvdata_.at(10).toInt(nullptr, HEX_SWITCH);
    CardAnalysisResult analysis = analyzeCardData(ratioofBottle);

    bool configFinishedRatio = isRechargeSuccessful(result);
    quint16 lastratio = QString(recvdata_[12] + recvdata_[11]).toInt(nullptr,HEX_SWITCH);
    QLOG_DEBUG() << "(瓶比例)配置状态 | 类型:" << analysis.dataType << "| 完成:" << analysis.workStatus
                 <<"|剩余比:"<<lastratio<<endl;


    // 处理配置结果
    if (configFinished && configFinishedRatio) {
        _recvdatacancelmark(num_);

        if (!mdataarry_.isEmpty() && cglobal::gserialConnecStatus) {
            QByteArray senddata = mdataarry_.first()->data_;
            emit _sendDirectives(senddata, "配置耗材百分比损耗");
            // 释放已处理数据
            //delete mdataarry_.takeFirst();
        } else {
            QLOG_ERROR() << "配置完成 | 串口状态:" << cglobal::gserialConnecStatus
                        << "| 剩余指令:" << mdataarry_.size() << endl;
            _shutdownWithError();
        }
    } else {
            QLOG_DEBUG() << "试剂配置未完成，执行关闭流程"<<endl;
            _shutdownWithError();
    }
    return;
}


void ConsumablesWrite::_recvdatacancelmark(quint8 index_)
{
    QString kind_reag;
    auto iter = mdataarry_.begin();
    while(iter != mdataarry_.end())
    {
       if((*iter)->index_num == index_)
       {
           kind_reag = GlobalData::mapReagentNames(index_);
           QLOG_DEBUG()<<"耗材:"<< kind_reag <<"写入耗材完成"<<endl;
           mdataarry_.removeOne(*iter);
           ConsumablesStu* pdel = (ConsumablesStu*)*iter;
           delete pdel;
           pdel = nullptr;
           break;
       }
       iter++;
    }
}


void ConsumablesWrite::initdelfile()
{
    QUIUtils::deleteLocalLog(); //删除日志指定时间
    QLOG_DEBUG()<<"清理日志文件完成"<<endl;

    //del非当天存在数据库内样本任务信息
    QString delPara = QUIUtils::getTodatTime() ;
    FullyAutomatedPlatelets::pinstancesqlData()->DeleteOtherDaysTask(delPara);
    QLOG_DEBUG()<<"清理非当天样本任务信息完成"<<endl;
    return ;
}
