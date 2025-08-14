#include "maincontrolboardprotocol.h"
#include "mainwindow.h"
#include "qualitycontrol.h"
#include "QsLog/include/QsLog.h"
#include "genericfunctions.h"
#include "globaldata.h"

mainControlBoardProtocol::mainControlBoardProtocol(QObject *parent) : QObject(parent)
  ,mreadCardEvent(false)
{
    moveToThread(&m_thread);
    QObject::connect(&m_thread,&QThread::started,this,&mainControlBoardProtocol::_threadstart);
}

mainControlBoardProtocol::~mainControlBoardProtocol()
{
    if(m_thread.isRunning()){
        m_thread.quit();
        m_thread.wait();
    }
}

void mainControlBoardProtocol::_start(quint8 Equipment)
{
    mEquipment = Equipment;
    QLOG_DEBUG()<<"主板线程同步仪器型号"<<mEquipment;
    if(!m_thread.isRunning()){
        m_thread.start();
    }
}

void mainControlBoardProtocol::_threadstart()
{
    binitCreatSignals = false;
    m_BuzzerState = false;
    QLOG_DEBUG()<<"mainControlBoardProtocol  Thread ID:"<<QThread::currentThreadId();
}



/* 蜂鸣器开关启用*/
void mainControlBoardProtocol::RecvUsedBuzzerMark(const bool BuzzerStae)
{
    m_BuzzerState = BuzzerStae;
}


/* 测高触发状态优化版 */
void mainControlBoardProtocol::TriggerAltimetryState(const int bitAltimetryHex)
{
	quint8 bitydata = static_cast<quint8>(bitAltimetryHex);
	const bool isHeightTriggered = (bitydata >> 1) & 0x01;

    if (isHeightTriggered && !m_TriggerTestHeight) {
        triggerHeightEvent(); // 提取触发动作
        m_TriggerTestHeight = true;

        // 首次触发时启动蜂鸣器 [[2]]
        if (!m_BuzzerState) {
            m_BuzzerState = true;
            // 此处添加蜂鸣器控制代码
        }
    } else if (!isHeightTriggered) {
        m_TriggerTestHeight = false;
        // 仅当状态变化时关闭蜂鸣器
        if (m_BuzzerState) {
            m_BuzzerState = false;
        }
    }
}

// 独立触发动作函数
void mainControlBoardProtocol::triggerHeightEvent()
{
    emit MainBoardHeightTrigger();
    if (m_parameterInterface != nullptr) {
        m_parameterInterface->PromptInfo(USERACTIONS, tr("触发测高检测"), NotActiveRunning);
    }
}







//void  mainControlBoardProtocol::TriggerAltimemetryState(const int BitAltimemetryHex)
//{
//    //8代表宽度，2表示2进制，空位补零
//    QString byte5ToString = QString("%1").arg(BitAltimemetryHex, SwitchLength, SwitchBinary, QLatin1Char('0'));
//    QByteArray mainboard_5byteToChar =  byte5ToString.toLatin1();
//    int ibitStae = mainboard_5byteToChar[6] - '0';
//    bool bnormal = (ibitStae != 0) ? true : false;
//    if(true == bnormal && false == m_TriggerTestHeight)
//    {
//        if(m_parameterInterface != nullptr)
//        {
//            emit MainBoardHeightTrigger();
//            m_parameterInterface->PromptInfo(USERACTIONS,tr("触发测高检测"),NotActiveRunning);
//        }
//        m_TriggerTestHeight = true;
//    }
//    else if(bnormal == false)
//    {
//        m_TriggerTestHeight = false;
//        m_BuzzerState = false; //蜂鸣器标志
//    }
//    return;
//}



// 4th协议解析函数
DeviceStatus mainControlBoardProtocol::parseDeviceStatus(quint8 statusByte) {

    DeviceStatus status;

    // 试杯盘检测（bit3-bit0）
    status.cup4Present = (statusByte >> 7) & 0x01;  // bit0 (原代码错位为cup4)
    status.cup3Present = (statusByte >> 6) & 0x01; // bit1
    status.cup2Present = (statusByte >> 5) & 0x01; // bit2
    status.cup1Present = (statusByte >> 4) & 0x01; // bit3

    // 清洗检测（bit4，0=正常）
    status.isCleanNormal = !((statusByte >> 3) & 0x01);

    // 命令状态（bit6-5）
    status.cmdStatus = static_cast<CommandStatus>((statusByte >> 1) & 0x03);

    // 命令类型（bit7）
    status.type = static_cast<CommandType>((statusByte & 0x01));

    return status;
}
void mainControlBoardProtocol::th4mainControlBoardData(const DeviceStatus &status)
{
    //QLOG_DEBUG() << "命令类型:" << (status.type == CMD_WRITE ? "写命令" : "读报告");
    QString cmdStatusStr;
    switch(status.cmdStatus) {
       case STATUS_PENDING_B: cmdStatusStr = "等待"; break;
       case STATUS_RUNNING: cmdStatusStr = "运行中"; break;
       case STATUS_COMPLETED: cmdStatusStr = "执行完成"; break;
       case STATUS_ERROR: cmdStatusStr = "命令错误"; break;
    }
    //QLOG_DEBUG() << "命令状态:" << cmdStatusStr;
    if(status.cmdStatus == STATUS_COMPLETED && status.type == CMD_READ)
    {
        triggeredOutCleanInfo(status.isCleanNormal); //S2清洗液状态

        triggeredTary_1_info(status.cup1Present);

        triggeredTary_2_info(status.cup2Present);

        if(mEquipment == KS800){
            triggeredTary_3_info(status.cup3Present);
        }
        else if(mEquipment != KS600 && mEquipment != KS800){
            triggeredTary_3_info(status.cup3Present);
            triggeredTary_4_info(status.cup4Present);
        }

    }
    //QLOG_DEBUG() << "试杯盘1:" << (status.cup1Present ? "存在" : "缺失");
    //QLOG_DEBUG() << "试杯盘2:" << (status.cup2Present ? "存在" : "缺失");
    //QLOG_DEBUG() << "试杯盘3:" << (status.cup3Present ? "存在" : "缺失");
    //QLOG_DEBUG() << "试杯盘4:" << (status.cup4Present ? "存在" : "缺失")<<endl;
    return;
}

void  mainControlBoardProtocol::triggeredOutCleanInfo(const bool s2Cleanstatus)
{
    bool outsidecleannlinque = s2Cleanstatus;

    if(outsidecleannlinque == false && m_boutsideCleanLinqueNull == false)
    {
        QLOG_DEBUG()<<"外部清洗液报警bit="<< s2Cleanstatus;
        m_boutsideCleanLinqueNull = true;
        emit equipmentReminder(static_cast<int>(equipmentTipInfo::LinqueCleanShortage));
    }
    else if(outsidecleannlinque == true)
    {
        m_boutsideCleanLinqueNull = false;
        emit _normaloper(static_cast<quint8>(equipmentTipInfo::LinqueCleanShortage));
    }
    else if(outsidecleannlinque == false && m_boutsideCleanLinqueNull == true){
        quint8 indexS2 = static_cast<quint8>(equipmentTipInfo::LinqueCleanShortage);
        emit _reminderErrorInfo(indexS2,"外部清洗液报警,请处理!");
    }
    return;
}



/**
 * @brief 检测抓板耗材刷卡状态
 * @param mainControlBoardData_suppileBit 刷卡状态字节
 * @param mainControlBoardData_suppileTotal 瓶数总量
 * @param suppileDate   有效期
 */
void mainControlBoardProtocol::DetectCardStatusofScratchingBoardConsumables(quint8 mainControlBoardData_suppileBit,
                                                                                quint8  mainControlBoardData_suppileTotal,
                                                                                quint16 suppileDate)
{

    // 参数校验
    if (mainControlBoardData_suppileTotal == 0) return;

    // 二进制解析优化
    const quint8  readCardState = mainControlBoardData_suppileBit & 0x0F; //读卡状态
    const quint8  kindsuppilesbity = (mainControlBoardData_suppileBit & 0xF0) >> 4;
    const QString kindsuppiles = GenericFunctions::BiteMapingConsumablesName(kindsuppilesbity);//耗材类型

    QString outTips;
    const QMap<int,QString> statusPrefix{
        {CONSUMABLES_READ_FAIL, "刷卡失败"},
        {CONSUMABLES_READ_ABNORMAL, "刷卡设备异常"},
        {RECHARGE_FAIL, "刷卡充值失败"}
    };
    if(statusPrefix.contains(readCardState)){
        configreadCardflag();
        outTips = QString("%1: %2卡,请重试!").arg(statusPrefix.value(readCardState)).arg(kindsuppiles);
        emit Abnormalcardswipingprompt(outTips);
    }

    const QMap<int,QString> statusSucess{
        {CONSUMABLES_READ_SUCESSFULLY, "刷卡成功"},
        {RECHARGE_SUCESSFULLY, "刷卡充值成功"}
    };
    if(statusSucess.contains(readCardState)){

        outTips = QString("%1%2: 充值量:%3? 批号:%4")
                .arg(kindsuppiles)
                .arg(readCardState == 2 ? "刷卡成功" : "刷卡充值成功")
                .arg(mainControlBoardData_suppileTotal)
                .arg(suppileDate);
        int indexState = statusSucess.find(readCardState).key();
        if(indexState == CONSUMABLES_READ_SUCESSFULLY && !mreadCardEvent){
            mreadCardEvent = true; //刷卡
            emit cardSwipeSuccessful(outTips,kindsuppilesbity,mainControlBoardData_suppileTotal,suppileDate);
        }else if(indexState == RECHARGE_SUCESSFULLY){
            emit swipeCardSuccessfullyWritten(outTips, kindsuppilesbity,mainControlBoardData_suppileTotal);
        }
    }
    const QMap<int,QString> statusNormal{
        {CONSUMABLES_READ_NORMAL, "正常读状态"},
        {CONSUMABLES_REDUCE, "缓存成功"},
        {BUFFER_WRITE, "缓存写入"}
    };
    if(statusNormal.contains(readCardState)){
       //QLOG_DEBUG()<<statusNormal.find(readCardState).value();
       configreadCardflag();
    }
    return;
}
void mainControlBoardProtocol::configreadCardflag(){
    mreadCardEvent = false;
}





//0 三通阀2 状态 ==> 1 三通阀1 状态 ==> 2 隔膜泵 2状态 ==> 3  隔膜泵 1状态 ==> 4 制冷 ==> 5 蜂鸣器 ==> 6 测高检测 ==> 7 废液检测
void mainControlBoardProtocol::Waste_detection(const int WastLinqueHex)
{
    QString byte5ToString = QString("%1").arg(WastLinqueHex, SwitchLength, SwitchBinary, QLatin1Char('0'));
    QByteArray mainboard_5byteToChar =  byte5ToString.toLatin1();
    int ibitStae = mainboard_5byteToChar[7] - '0';
    bool bnormal = (ibitStae == 0) ? true : false;

    //quint8 bitydata = static_cast<quint8>(WastLinqueHex);
    //const bool isWastLinqueTriggered = (bitydata >> 0x01);


    if(bnormal == false && m_btheWasteTankIsFull == false)
    {
        m_btheWasteTankIsFull = true;
        emit equipmentReminder((int)equipmentTipInfo::LinqueScrapFull);
        QLOG_DEBUG()<<tr("废液满bit=")<<ibitStae;
    }
    else if(bnormal == false && m_btheWasteTankIsFull == true){
        emit _reminderErrorInfo((quint8)equipmentTipInfo::LinqueScrapFull,"废液灌报警,请处理!");
    }
    else if(bnormal == true)
    {
        m_btheWasteTankIsFull = false;
        emit _normaloper((quint8)equipmentTipInfo::LinqueScrapFull);
    }
    return;
}


void  mainControlBoardProtocol::triggeredTary_1_info(const bool isTriggered)
{
    const bool isTrayFault = !isTriggered;

    // 状态1: 托盘异常且之前状态正常
    if (isTrayFault && !m_isTray1Normal) {
        m_isTray1Normal = true;
        emit equipmentReminder(static_cast<int>(equipmentTipInfo::TestTubeTrayI));
        return;
    }

    // 状态2: 托盘异常且之前状态已异常
    if (isTrayFault) {
        emit _reminderErrorInfo(static_cast<quint8>(equipmentTipInfo::TestTubeTrayI),
                        tr("试管盘1位置异常未放置,请重新放置！"));
        return;
    }

    // 状态3: 托盘状态恢复正常
    m_isTray1Normal = false;

    return;
}

void  mainControlBoardProtocol::triggeredTary_2_info(const bool ctrigger)
{
    bool bnormal_tray_2 = ctrigger;
    if(bnormal_tray_2 == false && m_bEmptyCupTrayIINormal == false)
    {
        m_bEmptyCupTrayIINormal = true;
        emit equipmentReminder((int)equipmentTipInfo::TestTubeTrayII);
    }
    else if(bnormal_tray_2 == true){
        m_bEmptyCupTrayIINormal = false;
    }
    else if(bnormal_tray_2 == false && m_bEmptyCupTrayIINormal == true){
        emit _reminderErrorInfo((quint8)equipmentTipInfo::TestTubeTrayII,"试管盘 2 脱离位置,请处理!");
    }
}

void  mainControlBoardProtocol::triggeredTary_3_info(const bool ctrigger)
{
    bool bnormal_tray_3 = ctrigger;
    //(ctrigger == '0') ? bnormal_tray_3 = false : bnormal_tray_3 = true;
     if(bnormal_tray_3 == false && m_bEmptyCupTrayIIINormal == false)
     {
         m_bEmptyCupTrayIIINormal = true;
         emit equipmentReminder(equipmentTipInfo::TestTubeTrayIII);
     }
     else if(bnormal_tray_3 == true)
     {
         m_bEmptyCupTrayIIINormal = false;
     }
     else if(bnormal_tray_3 == false && m_bEmptyCupTrayIIINormal == true){
         emit _reminderErrorInfo((quint8)equipmentTipInfo::TestTubeTrayIII,"试管盘 3 脱离位置,请处理!");
     }

}

void  mainControlBoardProtocol::triggeredTary_4_info(const bool ctrigger)
{
    bool bnormal_tray_4 = ctrigger;
    //(ctrigger == '0') ? bnormal_tray_4 = false : bnormal_tray_4 = true;
     if(bnormal_tray_4 == false && m_bEmptyCupTrayIVNormal == false)
     {
         m_bEmptyCupTrayIVNormal = true;
         emit equipmentReminder((int)equipmentTipInfo::TestTubeTrayIIII);
     }
     else if(bnormal_tray_4 == true)
     {
         m_bEmptyCupTrayIVNormal = false;
     }
     else if(bnormal_tray_4 == false && m_bEmptyCupTrayIVNormal== true){
         emit this->_reminderErrorInfo((quint8)equipmentTipInfo::TestTubeTrayIIII,"试管盘 4 脱离位置,请处理!");
     }
}

//外部清洗液X2(主板第4个字节)
void mainControlBoardProtocol::Detect_Externalcleaningfluid(const QStringList Boardmessages)
{
    //0 试管盘4 状态 ==> 1 试管盘3 状态 ==> 2 试管盘3 状态 ==> 3  试管盘1状态 ==> 4 清洗液状态
    int mainboard_index_4_byte_Hex = QString(Boardmessages.at(4)).toInt(nullptr,16);
    QString str_data = QString("%1").arg(mainboard_index_4_byte_Hex, 8, 2, QLatin1Char('0'));
    QByteArray mainboard_index_4_byte =  str_data.toLatin1();
    if(mainboard_index_4_byte.size() == 8)
    {
        triggeredTary_1_info(mainboard_index_4_byte.at(3));
        triggeredTary_2_info(mainboard_index_4_byte.at(2));
        if(mEquipment == KS800){
            triggeredTary_3_info(mainboard_index_4_byte.at(1));
        }
        else if(mEquipment != KS600 && mEquipment != KS800){
            triggeredTary_3_info(mainboard_index_4_byte.at(1));
            triggeredTary_4_info(mainboard_index_4_byte.at(0));
        }
        //triggeredOutCleanInfo(mainboard_index_4_byte.at(4));  //外部清洗液
    }
    return;

}



void mainControlBoardProtocol::recvmainControlBoardProtocol(const QStringList mainControlBoardData)
{
    if(!binitCreatSignals)
    {
        QLOG_TRACE()<<"分析主板消息线程Id:"<< QThread::currentThreadId();
        binitCreatSignals = true;
    }

    QString hexstr;
    bool ok;
    hexstr = mainControlBoardData[IndexByte_5];
    quint8 mainControlBoardData_5th = hexstr.toInt(&ok,HEX_SWITCH);

    //收到测高的命令
    TriggerAltimetryState(mainControlBoardData_5th);


    hexstr = mainControlBoardData[IndexByte_8];
    quint8 mainControlBoardData_suppileBit = hexstr.toInt(&ok,HEX_SWITCH);

    hexstr = mainControlBoardData[IndexByte_8 + 1];
    quint8 mainControlBoardData_suppileTotal = hexstr.toInt(&ok,HEX_SWITCH);


    hexstr = mainControlBoardData[IndexByte_14] + mainControlBoardData[IndexByte_13];
    quint16 suppileDate = hexstr.toInt(&ok,HEX_SWITCH);

    //检测抓板耗材刷卡状态
    DetectCardStatusofScratchingBoardConsumables(mainControlBoardData_suppileBit,
                                                 mainControlBoardData_suppileTotal,suppileDate);


    Waste_detection(mainControlBoardData_5th); //废液状态


    hexstr = mainControlBoardData[IndexByte_4];
    quint8 mainControlBoardData_4thBit = hexstr.toInt(&ok,HEX_SWITCH);
    DeviceStatus data4th = parseDeviceStatus(mainControlBoardData_4thBit);

    //试管盘和S2清洗液
    th4mainControlBoardData(data4th);

    return;


    //Detect_Externalcleaningfluid(mainControlBoardData); //外部清洗液状态

    //return;
}

void  mainControlBoardProtocol::setArgInterface(PassparameterInterface* pInterface)
{
    m_parameterInterface = pInterface;
}




