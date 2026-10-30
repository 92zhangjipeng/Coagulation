#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif
#include "performanceverification.h"  //性能验证
#include <loadequipmentpos.h>
#include <cglobal.h>
#include <ini_file.h>
#include <operclass/fullyautomatedplatelets.h>
#include <QGlobalStatic>
#include <globaldata.h>
#include <mainwindow.h>
#include <QString>
#include <QTimer>
#include <QPushButton>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include "QsLog/include/QsLog.h"
#include <quiutils.h>
#include <testing.h>

Q_GLOBAL_STATIC(Performanceverification,globalInstance)


Performanceverification* Performanceverification::instance(){
    return globalInstance();

}

Performanceverification::Performanceverification(QObject *parent) : QObject(parent)
  ,mstartPerformance(false),m_peChannel(0)
{
    //QLOG_DEBUG()<< "Singleton initialized";
    //qRegisterMetaType<QStringList>("QStringList");

    connect(this,&Performanceverification::sendorder,
            FullyAutomatedPlatelets::pinstanceserialusb(),
            &SuoweiSerialPort::writedataToEquipment
            ,Qt::QueuedConnection);

    connect(this,&Performanceverification::sendorderList,
            FullyAutomatedPlatelets::pinstanceserialusb(),
            &SuoweiSerialPort::slotwrite_instructions_group
            ,Qt::QueuedConnection);

    connect(FullyAutomatedPlatelets::pinstanceserialusb(),
            &SuoweiSerialPort::serialPEdata
            ,this,&Performanceverification::handleSerialData
            ,Qt::QueuedConnection);

    connect(this,&Performanceverification::obtainChannelVal,
            FullyAutomatedPlatelets::mainWindow(),
            &MainWindow::experimentReadChannel);

    connect(this,&Performanceverification::controlChannelRevolve,
            FullyAutomatedPlatelets::mainWindow(),
            &MainWindow::handleControlChannelRevolve);

    connect(this,&Performanceverification::outErrInfo,
            FullyAutomatedPlatelets::mainWindow(),
            &MainWindow::handleoutErrInfo);


}

Performanceverification::~Performanceverification()
{
    QLOG_DEBUG()<< "析构性能测试单例";
}

void Performanceverification::logMessage(const QString& msg) {
    QLOG_DEBUG() << "[Performance]" << msg;
}

void Performanceverification::allfinishClearData(){
    qDeleteAll(mdatatest.begin(),mdatatest.end());
    mdatatest.clear();
}

// 坐标配置独立函数
void configureChannelCoordinates(PerformanceData& data) {
    SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, data.channelIndex,
                                               MOTOR_HANDS_INDEX, data.handsChannelCoord);
}
// 测试台坐标配置
void configureTestTaryPositions(PerformanceData& data) {
    SingletonAxis *axis = SingletonAxis::GetInstance();
    const auto configure = [&](quint8 count, int motor, QPoint& coord) {
        axis->testTaryZoneAxisPos(READ_OPERRAT, count, motor, coord);
    };

    configure(data.waterTubeCount, MOTOR_BLOOD_INDEX, data.waterTubeCoord);
    configure(data.waterTubeCount, MOTOR_HANDS_INDEX, data.handswaterTubeCoord);

    configure(data.basicLiquidCount, MOTOR_BLOOD_INDEX, data.basicLiquidTubeCoord);
    configure(data.basicLiquidCount, MOTOR_HANDS_INDEX, data.handsBasicLinuidTubeCoord);

    configure(data.basicLiquidTubeCount, MOTOR_BLOOD_INDEX, data.basicLiquidCoord);
    configure(data.basicLiquidTubeCountM, MOTOR_BLOOD_INDEX, data.basicLiquidCoordM);
    configure(data.basicLiquidTubeCountL, MOTOR_BLOOD_INDEX, data.basicLiquidCoordL);

    configure(data.basicLiquidTubeCount, MOTOR_HANDS_INDEX,
             data.handsBasicLiquidCoord);
    configure(data.basicLiquidTubeCountM, MOTOR_HANDS_INDEX,
             data.handsBasicLiquidCoordM);
    configure(data.basicLiquidTubeCountL, MOTOR_HANDS_INDEX,
             data.handsBasicLiquidCoordL);

}



void Performanceverification::gettestChannelList(const  QMap<QString,quint8> &channelList){
    QLOG_DEBUG()<<"指控通道"<<channelList;

    const quint8 waterHole = 0;
    const quint8 basicLinque = 1;

    mdatatest.reserve(channelList.size());

    auto* axis = SingletonAxis::GetInstance();
    auto &ini = INI_File();

    QPoint suckWaterCoord(0, 0),suckBasicCoord(0,0),throwCoord(0,0);
    axis->bloodSampleZonePos(READ_OPERRAT, waterHole, suckWaterCoord);
    axis->bloodSampleZonePos(READ_OPERRAT, basicLinque, suckBasicCoord);
    axis->throwTubeHolePos(READ_OPERRAT,throwCoord);
    m_peThrowCoord = throwCoord;
    m_peThrowGripperDown = ini._gethandsdownthrowcpus();
    clearPeGrabDetection();

    auto iter = channelList.cbegin();
    while(iter != channelList.cend()){

        std::unique_ptr<PerformanceData> pdataPE(new PerformanceData);
        pdataPE->indexnumPE =  iter.key();

        pdataPE->channelIndex = iter.value() - 1;
        const quint8 gripperDown = ini._gethandsdownheiht(pdataPE->channelIndex);
        const quint8 throwGripperDown = ini._gethandsdownthrowcpus();
        m_peGripperDown = gripperDown;
        configureChannelCoordinates(*pdataPE);

        // 试管配置
        pdataPE->waterTubeCount = outfreeEmptyTube();
        pdataPE->basicLiquidCount = outfreeEmptyTube();
        pdataPE->basicLiquidTubeCount = outfreeEmptyTube();
        pdataPE->basicLiquidTubeCountM = outfreeEmptyTube();
        pdataPE->basicLiquidTubeCountL = outfreeEmptyTube();

        // 统一配置测试台坐标
        configureTestTaryPositions(*pdataPE);

        // 参数设置
        pdataPE->waterLevel = 1;
        pdataPE->basicSolutions = 1;
        pdataPE->basicLiquidLevel = 1;
        pdataPE->basicLiquidLevelM = 1;
        pdataPE->basicLiquidLevelL = 1;

        //水加样吐样操作数据
        insertWaterdata(*pdataPE,suckWaterCoord);

        //标准物质加样数据
        insertBasicLinquedata(*pdataPE, suckBasicCoord);

        insertTestWaterPEActivedata(*pdataPE,gripperDown);

        insertTestBasicLinquePEActivedata(*pdataPE,gripperDown);

        insetThrowTestingCup(*pdataPE,throwCoord,gripperDown,throwGripperDown);

        insertCleanendPE(*pdataPE);

        mdatatest.append(pdataPE.release());

        iter++;
    }

    if(!mdatatest.isEmpty()){
        setPerformanceTestFlag(true);
        runningPE();
    }


}

void Performanceverification::runningPE(){
    QByteArrayList dataout;
    clearPeGrabDetection();
    if(!outputSuckSplitWater(dataout)){
       emit controlChannelRevolve(m_peChannel,true);
       emit sendorderList(dataout,"PE蒸馏水加样");
    }
}


void Performanceverification::insertWaterdata(PerformanceData &dataPE,const QPoint& suckCoord){
    QByteArrayList outputArryList;
    outputArryList.clear();
    dataPE.suckWater.enabled = false;

    QUIUtils::testPEWaterSuckSplit(outputArryList,suckCoord,dataPE.waterTubeCoord,
                                   dataPE.basicLiquidCoord,
                                   dataPE.basicLiquidCoordM,
                                   dataPE.basicLiquidCoordL);

    dataPE.suckWater.enabled = false;
	//dataPE.suckWater.commands.reserve(outputArryList.size());
    for(const QByteArray &arry : outputArryList){
        QByteArray indexField = arry.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            QLOG_WARN() << "PE指令索引字段缺失，指令数据:" << arry;
            continue;
        }
        bool ok;
        QString hexStr = indexField.toHex();
        quint8  indexcode = hexStr.toUShort(&ok, HEX_SWITCH);
        if (!ok) {
            QLOG_WARN() << "PE指令无效的指令索引值:" << indexField << "原始指令:" << arry;
            continue;
        }

        dataPE.suckWater.commands.insert(indexcode, {arry,indexcode, false,0,0});
    }

}

void Performanceverification::insertBasicLinquedata(PerformanceData &dataPE,const QPoint& suckBasicLinqueCoord){
    QByteArrayList outputArryList;
    outputArryList.clear();
    dataPE.suckBasicLiquid.enabled = false;
    QUIUtils::controlPEAddBasicLinque(outputArryList,suckBasicLinqueCoord,
                                      dataPE.basicLiquidTubeCoord,
                                      dataPE.basicLiquidCoord,
                                      dataPE.basicLiquidCoordM,
                                      dataPE.basicLiquidCoordL);




    dataPE.suckBasicLiquid.enabled = false;
	//dataPE.suckBasicLiquid.commands.reserve(outputArryList.size());

    for(const QByteArray &arry : outputArryList){
        QByteArray indexField = arry.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            QLOG_WARN() << "PE指令索引字段缺失，指令数据:" << arry;
            continue;
        }
        bool ok;
        QString hexStr = indexField.toHex();
        quint8  indexcode = hexStr.toUShort(&ok, HEX_SWITCH);
        if (!ok) {
            QLOG_WARN() << "PE指令无效的指令索引值:" << indexField << "原始指令:" << arry;
            continue;
        }
        dataPE.suckBasicLiquid.commands.insert(indexcode, {arry,indexcode, false,0,0});
    }


}

void Performanceverification::insertTestWaterPEActivedata(PerformanceData &dataPE,const quint8 gripperDown,
                                                          bool forceRepeatGrab){
    QByteArrayList outdataList;
    quint8 ksuck,ksplit;
    const quint8 trayGrabDown = QUIUtils::_hansdownheightinnertubetray(dataPE.waterTubeCount);
    QUIUtils::suckCupMoveChannelPut(dataPE.handswaterTubeCoord,
                                    dataPE.handsChannelCoord,
                                    trayGrabDown,
                                    gripperDown,
                                    outdataList,ksuck,ksplit,
                                    forceRepeatGrab);
    dataPE.testWater.enabled = false;
    dataPE.testWater.commands.clear();
    for(const QByteArray &arry : outdataList){
        QByteArray indexField = arry.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            QLOG_WARN() << "PE测试水指令索引字段缺失，指令数据:" << arry;
            continue;
        }
        bool ok;
        QString hexStr = indexField.toHex();
        quint8  indexcode = hexStr.toUShort(&ok, HEX_SWITCH);
        if (!ok) {
            QLOG_WARN() << "PE测试水指令无效的指令索引值:" << indexField << "原始指令:" << arry;
            continue;
        }
        dataPE.testWater.commands.insert(indexcode, {arry,indexcode, false,ksuck,ksplit});
    }
}

void Performanceverification::insertTestBasicLinquePEActivedata(PerformanceData &dataPE,const quint8 gripperDown,
                                                                bool forceRepeatGrab){
    QByteArrayList outdataList;
    quint8 ksuck =0;
    quint8 ksplit = 0;

    auto processCommands =[&](QByteArrayList &dataList, PerformanceData::TestOperation &liquidData, const QString &logPrefix){
        liquidData.enabled = false;
        liquidData.commands.clear();
        for (const QByteArray &arry : dataList) {
            // 直接检查索引边界（避免临时对象）
            if (arry.size() <= GET_COMMAND_INDEX) {
                QLOG_WARN() << logPrefix << "指令索引字段缺失，指令数据长度不足:" << arry.size();
                continue;
            }

            // 直接获取字节值（避免字符串转换）
            quint8 indexcode = static_cast<quint8>(arry.at(GET_COMMAND_INDEX));
            liquidData.commands.insert(indexcode, {arry, indexcode, false, ksuck, ksplit});
        }
    };

    //标准物质
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLinuidTubeCoord, dataPE.handsChannelCoord,
                                    QUIUtils::_hansdownheightinnertubetray(dataPE.basicLiquidCount),
                                    gripperDown, outdataList, ksuck, ksplit, forceRepeatGrab);
    processCommands(outdataList, dataPE.testStandardsolutions, "PE测试标准物质");

    // 高值处理
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLiquidCoord, dataPE.handsChannelCoord,
                                    QUIUtils::_hansdownheightinnertubetray(dataPE.basicLiquidTubeCount),
                                    gripperDown, outdataList, ksuck, ksplit, forceRepeatGrab);
    processCommands(outdataList, dataPE.testBasicLiquid, "PE测试标准物质高值");

    // 中值处理
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLiquidCoordM, dataPE.handsChannelCoord,
                                    QUIUtils::_hansdownheightinnertubetray(dataPE.basicLiquidTubeCountM),
                                    gripperDown, outdataList, ksuck, ksplit, forceRepeatGrab);
    processCommands(outdataList, dataPE.testBasicLiquidM, "PE测试标准物质中值");

    // 低值处理
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLiquidCoordL, dataPE.handsChannelCoord,
                                    QUIUtils::_hansdownheightinnertubetray(dataPE.basicLiquidTubeCountL),
                                    gripperDown, outdataList, ksuck, ksplit, forceRepeatGrab);
    processCommands(outdataList, dataPE.testBasicLiquidL, "PE测试标准物质低值");

}

void Performanceverification::insetThrowTestingCup(PerformanceData &dataPE,
                                                    const QPoint &throwCoord,
                                                    const quint8 gripperDown,
                                                    const quint8 throwGipperHeigh,
                                                    bool forceRepeatGrab){
    QByteArrayList throwCupsList;
    quint8 ksuck,ksplit;
    QUIUtils::ThrowChannelCup(gripperDown,throwGipperHeigh,
                              dataPE.handsChannelCoord,
                              throwCoord,
                              ksuck,
                              ksplit,
                              throwCupsList,
                              forceRepeatGrab);
    dataPE.drainWater.enabled = false;
    dataPE.drainStandardsolutions.enabled = false;
    dataPE.drainBasicLiquid.enabled = false;
    dataPE.drainBasicLiquidM.enabled = false;
    dataPE.drainBasicLiquidL.enabled = false;
    dataPE.drainWater.commands.clear();
    dataPE.drainStandardsolutions.commands.clear();
    dataPE.drainBasicLiquid.commands.clear();
    dataPE.drainBasicLiquidM.commands.clear();
    dataPE.drainBasicLiquidL.commands.clear();
    for(const QByteArray &arry : throwCupsList){
        QByteArray indexField = arry.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            QLOG_WARN() << "PE丢杯指令索引字段缺失，指令数据:" << arry;
            continue;
        }
        bool ok;
        QString hexStr = indexField.toHex();
        quint8  indexcode = hexStr.toUShort(&ok, HEX_SWITCH);
        if (!ok) {
            QLOG_WARN() << "PE丢杯指令无效的指令索引值:" << indexField << "原始指令:" << arry;
            continue;
        }
        dataPE.drainWater.commands.insert(indexcode, {arry,indexcode, false,ksuck,ksplit});
        dataPE.drainStandardsolutions.commands.insert(indexcode,{arry,indexcode, false,ksuck,ksplit});
        dataPE.drainBasicLiquid.commands.insert(indexcode,{arry,indexcode, false,ksuck,ksplit});
        dataPE.drainBasicLiquidM.commands.insert(indexcode,{arry,indexcode, false,ksuck,ksplit});
        dataPE.drainBasicLiquidL.commands.insert(indexcode,{arry,indexcode, false,ksuck,ksplit});
    }
}

void Performanceverification::insertCleanendPE(PerformanceData &dataPE){
    QByteArrayList cleanNeedleList;
    QUIUtils::CleanBloodPinActionCommd(1,cleanNeedleList);
    dataPE.cleaningSystem.enabled = false;
	//dataPE.cleaningSystem.cleaningCommands.reserve(cleanNeedleList.size());
    for(const QByteArray &arry : cleanNeedleList){
        QByteArray indexField = arry.mid(GET_COMMAND_INDEX, 1);
        if (indexField.isEmpty()) {
            QLOG_WARN() << "PE清洗结束指令索引字段缺失，指令数据:" << arry;
            continue;
        }
        bool ok;
        QString hexStr = indexField.toHex();
        quint8  indexcode = hexStr.toUShort(&ok, HEX_SWITCH);
        if (!ok) {
            QLOG_WARN() << "PE清洗结束指令无效的指令索引值:" << indexField << "原始指令:" << arry;
            continue;
        }
        dataPE.cleaningSystem.commands.insert(indexcode, {arry,indexcode, false,0,0});
    }
}



quint8 Performanceverification::outfreeEmptyTube(){
    quint8 outfreehole = 0;
    FullyAutomatedPlatelets::pinstancesqlData()->inquire_test_free_tube_status(outfreehole);
    FullyAutomatedPlatelets::pinstancesqlData()->update_test_tube_status(outfreehole,TESTTUBES_MARK,"PE",503);
    FullyAutomatedPlatelets::pinstanceTesting()->tubeMarked("PE", outfreehole);
	return outfreehole;
}

void Performanceverification::setPerformanceTestFlag(const bool &isstart){
    mstartPerformance = isstart;

}

bool Performanceverification::returnPerformanceTestFlag(){ return mstartPerformance;}

 
//业务流程
PerformanceData* Performanceverification::findChannel(quint8 targetChannel) const
{
	auto it = std::find_if(mdatatest.cbegin(), mdatatest.cend(),
		[this, targetChannel](const PerformanceData* pData) {
		return pData && pData->channelIndex == targetChannel;
	});
	return (it != mdatatest.cend()) ? *it : nullptr;
}

bool Performanceverification::outputSuckSplitWater(QByteArrayList &outSend){
    auto it = std::find_if(mdatatest.cbegin(), mdatatest.cend(),
           [](const PerformanceData *pData) {
               return  !pData->peCompleted &&
                       !pData->suckWater.enabled &&
                    std::any_of(pData->suckWater.commands.cbegin(),
                                pData->suckWater.commands.cend(),[](const auto& cmd) {
                                return !cmd.isCompleted && !cmd.cmdData.isEmpty();
                         });
    });


    if (it != mdatatest.cend()) {
		m_peChannel = (*it)->channelIndex;
        const CommandGroup& commands = (*it)->suckWater.commands;

        // 只发第一条未完成指令，后续在 ACK(MOTOR_FINISH) 后再发下一条
        for (CommandGroup::const_iterator cmdIt = commands.cbegin();
                cmdIt != commands.cend(); ++cmdIt)
        {
            const ActiveCommand& cmd = cmdIt.value();
            if (!cmd.isCompleted && !cmd.cmdData.isEmpty()) {
                outSend.push_back(cmd.cmdData);
                break;
            }
        }
        mPE = activePE::suckWater;
    }else{
        QLOG_DEBUG()<<"性能检测完成";
        setPerformanceTestFlag(false);
        allfinishClearData();
        //选取通道复原
        FullyAutomatedPlatelets::pinstanceequipmentconfig()->completedPETest();

        return true;
    }
    return false;
}


void Performanceverification::outPutSuckBasicLiquid(QByteArrayList &outSend){

    mPE = activePE::suckBasicLinque;
    clearPeGrabDetection();
    collectTestingCommands(outSend, m_peChannel,&PerformanceData::suckBasicLiquid,
                           "标准物加样PEChannel");
}

void Performanceverification::outPutTestingWater(QByteArrayList &outSend){

    mPE = activePE::testingWater;
    collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::testWater,
                            "测试水PEChannel");
    armPeGrabDetection();
}


void Performanceverification::outPutTestingBasicLiquid(QByteArrayList &outSend, const int indexVal){
    if(HEIGHVALUE == indexVal){
        mPE = activePE::testingBasicWater;
        collectTestingCommands(outSend, m_peChannel,&PerformanceData::testBasicLiquid,
                            "PE测试浊度物高值");
    }
    else if(MIDVALUE == indexVal){
        mPE = activePE::testingBasicWaterM;
        collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::testBasicLiquidM,
                            "PE测试浊度物中值");
    }
    else if(LOWVALUE == indexVal){
        mPE = activePE::testingBasicWaterL;
        collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::testBasicLiquidL,
                            "PE测试浊度物低值");
    } else if(BASICSOLUTIONS == indexVal){
        mPE = activePE::testingStandardSolutions;
        collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::testStandardsolutions,
                            "PE测试标准浊度物");
    }
    armPeGrabDetection();
    return;
}

void Performanceverification::outPutThrowWater(QByteArrayList &outSend){
    mPE = activePE::ThrowWater;
    collectTestingCommands(outSend, m_peChannel,&PerformanceData::drainWater,
                            "弃杯水PEChannel");
    armPeGrabDetection();
    return;
}
void Performanceverification::outPutThrowBasicLiquid(QByteArrayList &outSend, const int indexValue){

    if(indexValue == LiquidLevel::BasicValue){
         mPE = activePE::ThrowStandardSolutions;
         collectTestingCommands(outSend, m_peChannel,&PerformanceData::drainStandardsolutions,
                                "PE弃杯标准物");
    }
    else if(indexValue == LiquidLevel::HighValue){
        mPE = activePE::ThrowBasicLinque;
        collectTestingCommands(outSend, m_peChannel,&PerformanceData::drainBasicLiquid,
                                "PE弃杯标准物质高");
    }else if(indexValue == LiquidLevel::MidValue){
        mPE = activePE::ThrowBasicLinqueM;
        collectTestingCommands(outSend, m_peChannel,&PerformanceData::drainBasicLiquidM,
                                "PE弃杯标准物质中");
    }else if(indexValue == LiquidLevel::LowValue){
        mPE = activePE::ThrowBasicLinqueL;
        collectTestingCommands(outSend, m_peChannel,&PerformanceData::drainBasicLiquidL,
                               "PE弃杯标准物质低");
    }
    armPeGrabDetection();
    return;
}

void Performanceverification::outCleanPin(QByteArrayList &outSend){
    mPE = activePE::CleanPin;
    clearPeGrabDetection();
    collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::cleaningSystem,
                            "清洗针PEChannel");
    return;
}

void Performanceverification::TrageIndexTube(const quint8& index, const int&lossKind){
    FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(index,lossKind);
}

void Performanceverification::suckWaterFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE吸蒸馏水遍历指令为空";
        return;
    }
	
    bool isFinished = processLiquidCommands(pdata->suckWater, indexnum,
                                            QString::fromUtf8("PE蒸馏水加样"));
    if( isFinished){
        TrageIndexTube(pdata->waterTubeCount,TESTTUBES_SAMPLED);
        QByteArrayList suckBasicLiqune;
        outPutSuckBasicLiquid(suckBasicLiqune);
        emit sendorderList(suckBasicLiqune,"加样");
    }
}

void Performanceverification::testWaterFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE测试蒸馏水遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->testWater, indexnum,
	                                        QString::fromUtf8("PE测试蒸馏水"));

	if (isFinished) {
		clearPeGrabDetection();
		TrageIndexTube(pdata->waterTubeCount, TESTTUBES_CLIPPEDAWAY);
		const int configDelay = INI_File()._getdelayedtime();
		const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
		const quint8 peCh = static_cast<quint8>(m_peChannel);
		QLOG_DEBUG() << "蒸馏水放到通道测试开始去读延时" << delayTime << "获取通道值";
		emit obtainChannelVal(m_peChannel, THROWPEWATER, true);
		QTimer::singleShot(delayTime, this, [this, peCh, delayTime]() {
			if (!returnPerformanceTestFlag()) {
				return;
			}
			PerformanceData *alive = findChannel(peCh);
			if (!alive) {
				return;
			}
			obtainChannelTestVal(alive->waterLevel, TestType::Water);
			QLOG_DEBUG() << "PE延时" << delayTime << "结束获取蒸馏水值:" << alive->waterLevel;
			emit obtainChannelVal(peCh, THROWPEWATER, false);
		});

	}
}


void Performanceverification::obtainChannelTestVal(int& val, TestType testType) {
    val = FullyAutomatedPlatelets::mainWindow()->getChannelTestVal(m_peChannel + 1);

    QByteArrayList testorders;
    switch(testType) {
        case TestType::Water:
            outPutThrowWater(testorders);
            break;
        case TestType::Basic:
            outPutThrowBasicLiquid(testorders, LiquidLevel::BasicValue);
            break;
        case TestType::High:
            outPutThrowBasicLiquid(testorders, LiquidLevel::HighValue);
            break;
        case TestType::Mid:
            outPutThrowBasicLiquid(testorders, LiquidLevel::MidValue);
            break;
        case TestType::Low:
            outPutThrowBasicLiquid(testorders, LiquidLevel::LowValue);
            break;
        default:
            qCritical("Unsupported test type: %d", static_cast<int>(testType));
            return;
    }
    emit sendorderList(std::move(testorders), getCommandName());
}



void Performanceverification::throwWaterFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE弃杯蒸馏水遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->drainWater, indexnum,
	                                        QString::fromUtf8("弃杯水PEChannel"));
	if (isFinished) {
		clearPeGrabDetection();
		QByteArrayList testBasicLiqune;
		outPutTestingBasicLiquid(testBasicLiqune, BASICSOLUTIONS);
        emit sendorderList(testBasicLiqune, "PE测试标准浊度物");

	}
}

void Performanceverification::cleanPinFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE清洗遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->cleaningSystem, indexnum,
	                                        QString::fromUtf8("清洗针PEChannel"));
	if (isFinished) {
		pdata->peCompleted = true;
		outputResult();
		runningPE();
	}
}


void Performanceverification::suckBasicLinqueFunc(const quint8 /*addr*/, const quint8 indexnum)
{
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE标准物加样遍历指令为空";
        return;
    }
	bool isFinished = processLiquidCommands(pdata->suckBasicLiquid, indexnum,
	                                        QString::fromUtf8("加样"));
	if (isFinished) {
		// 标准物 + 高/中/低 四支试管均已加样
		TrageIndexTube(pdata->basicLiquidCount, TESTTUBES_SAMPLED);
		TrageIndexTube(pdata->basicLiquidTubeCount, TESTTUBES_SAMPLED);
		TrageIndexTube(pdata->basicLiquidTubeCountM, TESTTUBES_SAMPLED);
		TrageIndexTube(pdata->basicLiquidTubeCountL, TESTTUBES_SAMPLED);
		QByteArrayList testWater;
		outPutTestingWater(testWater);
        emit sendorderList(testWater, "PE测试蒸馏水");
	}
}

//标准物质
void Performanceverification::testStandardsolutionsFunc(const quint8 /*addr*/,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE标准浊度物遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->testStandardsolutions, indexnum,
	                                        QString::fromUtf8("PE测试标准浊度物"));
	if (isFinished) {
		clearPeGrabDetection();
		TrageIndexTube(pdata->basicLiquidCount, TESTTUBES_CLIPPEDAWAY);
		const int configDelay = INI_File()._getdelayedtime();
		const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
		const quint8 peCh = static_cast<quint8>(m_peChannel);
        QLOG_DEBUG() << "PE标准浊度物取值开始读延时" << delayTime << "获取通道值";
		emit obtainChannelVal(m_peChannel, THROWBASIC, true);

		QTimer::singleShot(delayTime, this, [this, peCh, delayTime]() {
			if (!returnPerformanceTestFlag()) {
				return;
			}
			PerformanceData *alive = findChannel(peCh);
			if (!alive) {
				return;
			}
			obtainChannelTestVal(alive->basicSolutions, TestType::Basic);
            QLOG_DEBUG() << "PE标准浊度物:" << alive->basicSolutions;
			emit obtainChannelVal(peCh, THROWBASIC, false);
		});

	}
}

void  Performanceverification::testBasicLinqueFunc(const quint8 /*addr*/,const quint8 indexnum)
{
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE高浊度物测试遍历指令为空";
        return;
    }
    bool isFinished = processLiquidCommands(pdata->testBasicLiquid, indexnum,
                                            QString::fromUtf8("PE测试浊度物高值"));
    if(isFinished){
        clearPeGrabDetection();
        TrageIndexTube(pdata->basicLiquidTubeCount,TESTTUBES_CLIPPEDAWAY);

        const int configDelay  = INI_File()._getdelayedtime();
        const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
        const quint8 peCh = static_cast<quint8>(m_peChannel);

        QLOG_DEBUG()<<"PE浊度物读取(高)值延时"<<delayTime<<"获取通道值";
        emit obtainChannelVal(m_peChannel,THROWHEIGHVAL,true);

        QTimer::singleShot(delayTime, this, [this, peCh, delayTime]() {
            if (!returnPerformanceTestFlag()) {
                return;
            }
            PerformanceData *alive = findChannel(peCh);
            if (!alive) {
                return;
            }
            obtainChannelTestVal(alive->basicLiquidLevel,TestType::High);
            QLOG_DEBUG()<<"PE浊度物高值:"<<alive->basicLiquidLevel;
            emit obtainChannelVal(peCh,THROWHEIGHVAL,false);
        });

    }
}

void Performanceverification::testBasicMidLinqueFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE浊度物Mid测试遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->testBasicLiquidM, indexnum,
                                            QString::fromUtf8("测试浊度物中值PE"));
    if(isFinished){
        clearPeGrabDetection();
        TrageIndexTube(pdata->basicLiquidTubeCountM,TESTTUBES_CLIPPEDAWAY);

        const int configDelay  = INI_File()._getdelayedtime();
        const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
        const quint8 peCh = static_cast<quint8>(m_peChannel);
        QLOG_DEBUG()<<"浊度物MID到通道读取值开始去读延时"<<delayTime<<"获取通道值";

        emit obtainChannelVal(m_peChannel,THROWMIDVAL,true);

        QTimer::singleShot(delayTime, this, [this, peCh, delayTime]() {
            if (!returnPerformanceTestFlag()) {
                return;
            }
            PerformanceData *alive = findChannel(peCh);
            if (!alive) {
                return;
            }
            obtainChannelTestVal(alive->basicLiquidLevelM,TestType::Mid);
            QLOG_DEBUG()<<"PE浊度物MID准物质值:"<<alive->basicLiquidLevelM;
            emit obtainChannelVal(peCh,THROWMIDVAL,false);
        });

    }
}

void Performanceverification::testBasicLowLinqueFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE标准物LOW测试遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->testBasicLiquidL, indexnum,
                                            QString::fromUtf8("测试低值标准物质PE"));
	if (isFinished) {
		clearPeGrabDetection();
		TrageIndexTube(pdata->basicLiquidTubeCountL, TESTTUBES_CLIPPEDAWAY);

		const int configDelay = INI_File()._getdelayedtime();
		const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
		const quint8 peCh = static_cast<quint8>(m_peChannel);
		QLOG_DEBUG() << "浊度物LOW取值开始去读延时" << delayTime << "获取通道值";
		emit obtainChannelVal(m_peChannel, THROWLOWVAL, true);
		QTimer::singleShot(delayTime, this, [this, peCh, delayTime]() {
			if (!returnPerformanceTestFlag()) {
				return;
			}
			PerformanceData *alive = findChannel(peCh);
			if (!alive) {
				return;
			}
			obtainChannelTestVal(alive->basicLiquidLevelL, TestType::Low);
			QLOG_DEBUG() << "PE浊度物LOW值:" << alive->basicLiquidLevelL;
			emit obtainChannelVal(peCh, THROWLOWVAL, false);
		});

	}
}

void Performanceverification::throwStandardSolutions(const quint8 /*addr*/,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE浊度物丟杯遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->drainStandardsolutions, indexnum,
                                            QString::fromUtf8("PE弃杯标准物"));
    if(isFinished){
        clearPeGrabDetection();
        QByteArrayList testBasicLiqune;
        outPutTestingBasicLiquid(testBasicLiqune,HEIGHVALUE);
        emit sendorderList(testBasicLiqune,"PE测试浊度物高值");
    }
}


void Performanceverification::throwBasicLinqueFunc(const quint8 /*addr*/,const quint8 indexnum)
{
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE标准物高丟杯遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->drainBasicLiquid, indexnum,
                                            QString::fromUtf8("PE弃杯标准物质高"));
    if(isFinished){
        clearPeGrabDetection();
        QByteArrayList testBasicLiqune;
        outPutTestingBasicLiquid(testBasicLiqune,MIDVALUE);
        emit sendorderList(testBasicLiqune,"测试浊度物中值PE");
    }
}
void Performanceverification::throwBasicMidLinqueFunc(const quint8 ,const quint8 indexnum)
{
	auto *pdata = findChannel(m_peChannel);
	if(!pdata){
	    QLOG_DEBUG()<<"PE标准物中值丟杯遍历指令为空";
	    return;
	}
    bool isFinished = processLiquidCommands(pdata->drainBasicLiquidM, indexnum,
                                            QString::fromUtf8("PE弃杯标准物质中"));
    if( isFinished){
        clearPeGrabDetection();
        QByteArrayList testBasicLiqune;
        outPutTestingBasicLiquid(testBasicLiqune,LOWVALUE);
        emit sendorderList(testBasicLiqune,"测试低值标准物质PE");
    }
}
void Performanceverification::throwBasicLowLinqueFunc(const quint8 /*addr*/,const quint8 indexnum)
{
	auto *pdata = findChannel(m_peChannel);
	if(!pdata){
	    QLOG_DEBUG()<<"PE标准物丟杯遍历指令为空";
	    return;
	}
    bool isFinished = processLiquidCommands(pdata->drainBasicLiquidL, indexnum,
                                            QString::fromUtf8("PE弃杯标准物质低"));
    if (isFinished){
        clearPeGrabDetection();
        QByteArrayList cleanPinList;
        outCleanPin(cleanPinList);
        emit sendorderList(cleanPinList,"PE单通道完成清洗针");
    }
}


bool Performanceverification::isPeGripperPickPhase() const
{
    switch (mPE) {
    case activePE::testingWater:
    case activePE::testingStandardSolutions:
    case activePE::testingBasicWater:
    case activePE::testingBasicWaterM:
    case activePE::testingBasicWaterL:
    case activePE::ThrowWater:
    case activePE::ThrowStandardSolutions:
    case activePE::ThrowBasicLinque:
    case activePE::ThrowBasicLinqueM:
    case activePE::ThrowBasicLinqueL:
        return true;
    default:
        return false;
    }
}

void Performanceverification::clearPeGrabDetection()
{
    m_peGrabArmed = false;
    m_peGrabOk = false;
    m_peGrabJudgeIndex = 255;
    m_peGrabLiftIndex = 255;
    m_peGrabDeepenTimes = 0;
}

void Performanceverification::armPeGrabDetection()
{
    clearPeGrabDetection();
    CommandGroup *group = currentGripperCommandGroup();
    if (!group || group->isEmpty()) {
        return;
    }
    const ActiveCommand &cmd = group->constBegin().value();
    m_peGrabJudgeIndex = cmd.ksuck;
    m_peGrabLiftIndex = cmd.ksplit;
    m_peGrabArmed = (m_peGrabJudgeIndex != 255 || m_peGrabLiftIndex != 255);
    m_peGrabOk = false;
    QLOG_INFO() << "[PE抓杯] 武装气密判定 阶段" << static_cast<int>(mPE)
                << "判定号" << m_peGrabJudgeIndex
                << "抬Z号" << m_peGrabLiftIndex
                << "重抓" << m_peRegrab;
}

CommandGroup *Performanceverification::currentGripperCommandGroup()
{
    PerformanceData *pdata = findChannel(static_cast<quint8>(m_peChannel));
    if (!pdata) {
        return nullptr;
    }
    switch (mPE) {
    case activePE::testingWater: return &pdata->testWater.commands;
    case activePE::testingStandardSolutions: return &pdata->testStandardsolutions.commands;
    case activePE::testingBasicWater: return &pdata->testBasicLiquid.commands;
    case activePE::testingBasicWaterM: return &pdata->testBasicLiquidM.commands;
    case activePE::testingBasicWaterL: return &pdata->testBasicLiquidL.commands;
    case activePE::ThrowWater: return &pdata->drainWater.commands;
    case activePE::ThrowStandardSolutions: return &pdata->drainStandardsolutions.commands;
    case activePE::ThrowBasicLinque: return &pdata->drainBasicLiquid.commands;
    case activePE::ThrowBasicLinqueM: return &pdata->drainBasicLiquidM.commands;
    case activePE::ThrowBasicLinqueL: return &pdata->drainBasicLiquidL.commands;
    default: return nullptr;
    }
}

bool Performanceverification::markPeCommandCompleteOnly(quint8 indexnum)
{
    CommandGroup *group = currentGripperCommandGroup();
    if (!group) {
        return false;
    }
    for (auto it = group->begin(); it != group->end(); ++it) {
        if (it.value().phaseNumber == indexnum && !it.value().isCompleted) {
            it.value().isCompleted = true;
            return true;
        }
    }
    return false;
}

void Performanceverification::peUpdateGrabOkFromAck(const QStringList &recv, quint8 commandIndex)
{
    if (!m_peGrabArmed) {
        return;
    }
    if (commandIndex != m_peGrabJudgeIndex && commandIndex != m_peGrabLiftIndex) {
        return;
    }
    bool suction = false;
    quint32 air = 0;
    GlobalData::analyzeWhetherGripperSuction(recv, suction, air);
    const bool held = (commandIndex == m_peGrabLiftIndex)
            ? GlobalData::isHandsLiftStillHolding(air, suction)
            : GlobalData::isHandsVacuumStillSealed(air);
    if (commandIndex == m_peGrabLiftIndex || held) {
        m_peGrabOk = held;
    }
    QLOG_INFO() << "[PE抓杯] 判定 命令号" << commandIndex
                << "air" << air << "suction" << int(suction)
                << "held" << held << "grabOk" << m_peGrabOk;
}

bool Performanceverification::peIsGrabOpenDescend(quint8 commandIndex) const
{
    if (!m_peGrabArmed || m_peGrabJudgeIndex == 0 || m_peGrabJudgeIndex == 255) {
        return false;
    }
    return commandIndex == static_cast<quint8>(m_peGrabJudgeIndex - 1);
}

QByteArray Performanceverification::peBuildGrabDeepenCmd(quint8 commandIndex) const
{
    if (!Testing::getTaskDll()) {
        return QByteArray();
    }
    PerformanceData *pdata = findChannel(static_cast<quint8>(m_peChannel));
    if (!pdata) {
        return QByteArray();
    }
    const int times = m_peGrabDeepenTimes;
    switch (mPE) {
    case activePE::testingWater:
        return GlobalData::GrabtheCupAttheTestTubeTray(commandIndex, times, pdata->waterTubeCount);
    case activePE::testingStandardSolutions:
        return GlobalData::GrabtheCupAttheTestTubeTray(commandIndex, times, pdata->basicLiquidCount);
    case activePE::testingBasicWater:
        return GlobalData::GrabtheCupAttheTestTubeTray(commandIndex, times, pdata->basicLiquidTubeCount);
    case activePE::testingBasicWaterM:
        return GlobalData::GrabtheCupAttheTestTubeTray(commandIndex, times, pdata->basicLiquidTubeCountM);
    case activePE::testingBasicWaterL:
        return GlobalData::GrabtheCupAttheTestTubeTray(commandIndex, times, pdata->basicLiquidTubeCountL);
    case activePE::ThrowWater:
    case activePE::ThrowStandardSolutions:
    case activePE::ThrowBasicLinque:
    case activePE::ThrowBasicLinqueM:
    case activePE::ThrowBasicLinqueL:
        return GlobalData::Grabthehandtotryagain(commandIndex, times, pdata->channelIndex);
    default:
        return QByteArray();
    }
}

void Performanceverification::peSendFirstPending(const QString &info)
{
    CommandGroup *group = currentGripperCommandGroup();
    if (!group) {
        return;
    }
    for (auto it = group->constBegin(); it != group->constEnd(); ++it) {
        const ActiveCommand &cmd = it.value();
        if (!cmd.isCompleted && !cmd.cmdData.isEmpty()) {
            QByteArrayList one;
            one.append(cmd.cmdData);
            emit sendorderList(one, info);
            return;
        }
    }
}

bool Performanceverification::peRebuildCurrentGripperPhase()
{
    PerformanceData *pdata = findChannel(static_cast<quint8>(m_peChannel));
    if (!pdata) {
        return false;
    }
    auto &ini = INI_File();
    const quint8 gripperDown = ini._gethandsdownheiht(pdata->channelIndex);
    const quint8 throwDown = ini._gethandsdownthrowcpus();
    m_peGripperDown = gripperDown;
    m_peThrowGripperDown = throwDown;

    switch (mPE) {
    case activePE::testingWater:
        insertTestWaterPEActivedata(*pdata, gripperDown, m_peRegrab);
        break;
    case activePE::testingStandardSolutions:
    case activePE::testingBasicWater:
    case activePE::testingBasicWaterM:
    case activePE::testingBasicWaterL: {
        // 四个抓杯组一起重建，保留与初始一致
        insertTestBasicLinquePEActivedata(*pdata, gripperDown, m_peRegrab);
        break;
    }
    case activePE::ThrowWater:
    case activePE::ThrowStandardSolutions:
    case activePE::ThrowBasicLinque:
    case activePE::ThrowBasicLinqueM:
    case activePE::ThrowBasicLinqueL:
        insetThrowTestingCup(*pdata, m_peThrowCoord, gripperDown, throwDown, m_peRegrab);
        break;
    default:
        return false;
    }

    // 重建后该阶段全部未完成
    if (CommandGroup *group = currentGripperCommandGroup()) {
        for (auto it = group->begin(); it != group->end(); ++it) {
            it.value().isCompleted = false;
        }
        if (mPE == activePE::testingWater) {
            pdata->testWater.enabled = false;
        } else if (mPE == activePE::testingStandardSolutions) {
            pdata->testStandardsolutions.enabled = false;
        } else if (mPE == activePE::testingBasicWater) {
            pdata->testBasicLiquid.enabled = false;
        } else if (mPE == activePE::testingBasicWaterM) {
            pdata->testBasicLiquidM.enabled = false;
        } else if (mPE == activePE::testingBasicWaterL) {
            pdata->testBasicLiquidL.enabled = false;
        } else if (mPE == activePE::ThrowWater) {
            pdata->drainWater.enabled = false;
        } else if (mPE == activePE::ThrowStandardSolutions) {
            pdata->drainStandardsolutions.enabled = false;
        } else if (mPE == activePE::ThrowBasicLinque) {
            pdata->drainBasicLiquid.enabled = false;
        } else if (mPE == activePE::ThrowBasicLinqueM) {
            pdata->drainBasicLiquidM.enabled = false;
        } else if (mPE == activePE::ThrowBasicLinqueL) {
            pdata->drainBasicLiquidL.enabled = false;
        }
    }
    armPeGrabDetection();
    return true;
}

void Performanceverification::peFinishAbortHome()
{
    m_peAbortHoming = false;
    m_peAbortHome.clear();
    m_peAbortExpect = 255;
    setPerformanceTestFlag(false);
    allfinishClearData();
    if (FullyAutomatedPlatelets::pinstanceequipmentconfig()) {
        FullyAutomatedPlatelets::pinstanceequipmentconfig()->completedPETest();
    }
    emit outErrInfo(QString::fromUtf8("PE抓杯失败"),
                    QString::fromUtf8("未抓到试管，已放弃本次性能验证"));
}

void Performanceverification::peSendNextAbortHome()
{
    if (m_peAbortHome.isEmpty()) {
        peFinishAbortHome();
        return;
    }
    const QByteArray cmd = m_peAbortHome.takeFirst();
    if (cmd.size() <= GET_COMMAND_INDEX) {
        peSendNextAbortHome();
        return;
    }
    m_peAbortExpect = static_cast<quint8>(cmd.at(GET_COMMAND_INDEX));
    QByteArrayList one;
    one.append(cmd);
    emit sendorderList(one, QString::fromUtf8("PE放弃回原点"));
}

void Performanceverification::peAbortOnGrabGiveUp()
{
    clearPeGrabDetection();
    m_peRegrab = false;
    m_pePromptingRegrab = false;
    emit controlChannelRevolve(static_cast<quint8>(m_peChannel), false);

    m_peAbortHome = GlobalData::errMachineBackOrigin(200, MOTOR_HANDS_INDEX);
    m_peAbortHoming = true;
    if (m_peAbortHome.isEmpty()) {
        peFinishAbortHome();
        return;
    }
    peSendNextAbortHome();
}

void Performanceverification::peRequestRegrabPrompt()
{
    if (!returnPerformanceTestFlag() || m_pePromptingRegrab) {
        return;
    }
    m_pePromptingRegrab = true;
    QWidget *host = FullyAutomatedPlatelets::mainWindow();
    QObject *target = host ? static_cast<QObject *>(host) : static_cast<QObject *>(this);
    QTimer::singleShot(0, target, [this]() { pePromptRegrab(); });
}

void Performanceverification::pePromptRegrab()
{
    if (!returnPerformanceTestFlag() || !m_pePromptingRegrab) {
        return;
    }
    QWidget *parent = QApplication::activeWindow();
    if (!parent) {
        parent = FullyAutomatedPlatelets::mainWindow();
    }
    QDialog dlg(parent);
    dlg.setWindowTitle(QString::fromUtf8("抓杯失败"));
    dlg.setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    dlg.setWindowModality(Qt::ApplicationModal);
    dlg.setStyleSheet(QString::fromUtf8(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel#messageLabel { color: #1F2A3E; font: 14pt \"Microsoft YaHei\"; }"
        "QPushButton { background-color: #F2F4F7; color: #1F2A3E; border: 1px solid #CBD0D8;"
        " border-radius: 3px; padding: 4px 20px; min-width: 90px; min-height: 32px;"
        " font: 12pt \"Microsoft YaHei\"; }"
        "QPushButton:hover { background-color: #E6E9F0; }"
        "QPushButton#againButton { background-color: #0078D7; color: white; border-color: #005A9E; font-weight: bold; }"
        "QPushButton#againButton:hover { background-color: #106EBE; }"));

    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
    mainLayout->setContentsMargins(24, 20, 24, 20);
    mainLayout->setSpacing(16);
    const QString tip = (m_peGrabDeepenTimes >= 3)
            ? QString::fromUtf8("负压没到，已经再往下吸了3次，还是没抓到试管。\n重抓会从头再抓一次。放弃后先抬起抓手和双针，再回到原点。")
            : QString::fromUtf8("没有抓到试管。\n重抓会再抓一次。放弃后先抬起抓手和双针，再回到原点。");
    QLabel *message = new QLabel(tip, &dlg);
    message->setObjectName("messageLabel");
    message->setWordWrap(true);
    message->setMinimumWidth(360);
    mainLayout->addWidget(message);

    QHBoxLayout *buttons = new QHBoxLayout();
    buttons->addStretch();
    QPushButton *again = new QPushButton(QString::fromUtf8("重抓"), &dlg);
    again->setObjectName("againButton");
    again->setDefault(true);
    QPushButton *giveUp = new QPushButton(QString::fromUtf8("放弃"), &dlg);
    buttons->addWidget(again);
    buttons->addWidget(giveUp);
    mainLayout->addLayout(buttons);
    QObject::connect(again, &QPushButton::clicked, &dlg, &QDialog::accept);
    QObject::connect(giveUp, &QPushButton::clicked, &dlg, &QDialog::reject);
    dlg.adjustSize();
    dlg.setMinimumWidth(420);

    const bool retry = dlg.exec() == QDialog::Accepted;
    if (!returnPerformanceTestFlag()) {
        m_pePromptingRegrab = false;
        return;
    }
    m_pePromptingRegrab = false;
    if (retry) {
        m_peRegrab = true;
        if (!peRebuildCurrentGripperPhase()) {
            peAbortOnGrabGiveUp();
            return;
        }
        peSendFirstPending(QString::fromUtf8("PE抓杯重试"));
        return;
    }
    peAbortOnGrabGiveUp();
}

//接收
// 执行命令的函数
void Performanceverification::handleSerialData(const QStringList& recvdataList){

    if (m_pePromptingRegrab) {
        return;
    }

    if (m_peAbortHoming) {
        quint8 commandIndex = GlobalData::analyzeCommandIndex(recvdataList);
        int commandMode = 0;
        int run_status = 0;
        int speedMode = 0;
        int actionMotor = 0;
        GlobalData::Parse4thBytecommandstatus(recvdataList, commandMode, run_status, speedMode, actionMotor);
        if (run_status == MOTOR_WAITING || run_status == MOTOR_RUNNING) {
            return;
        }
        if (commandIndex != m_peAbortExpect) {
            return;
        }
        QString homeErr;
        if (!GlobalData::commandStatus(run_status, homeErr)) {
            QLOG_WARN() << "[PE放弃] 回原点失败" << homeErr << recvdataList.join(" ");
            peFinishAbortHome();
            return;
        }
        peSendNextAbortHome();
        return;
    }

    quint8 slaveAddr =   GlobalData::analyzeSlave_addr(recvdataList,false);//从机地址
    quint8 commandIndex = GlobalData::analyzeCommandIndex(recvdataList);   //命令编号
    //完成状态
    int commandMode, run_status,speedMode ,Action_motor;
    GlobalData::Parse4thBytecommandstatus(recvdataList,commandMode,run_status,speedMode,Action_motor);

    if (run_status == MOTOR_WAITING || run_status == MOTOR_RUNNING) {
        return;
    }

    const bool picking = m_peGrabArmed && isPeGripperPickPhase();

    QString showErrorlog;
    if(!GlobalData::commandStatus(run_status,showErrorlog)){
        QString error_code = recvdataList.join(" ");
        showErrorlog += error_code;
        QLOG_DEBUG()<<"异常命令:"<<error_code<<showErrorlog<<endl;
        if (picking) {
            peRequestRegrabPrompt();
        }
        return;
    }

    if (picking && peIsGrabOpenDescend(commandIndex)) {
        markPeCommandCompleteOnly(commandIndex);
        const int waitMs = GlobalData::gripperGrabDelayMs();
        QLOG_INFO() << "[PE抓杯] 开负压下扎完成，等" << waitMs << "ms再读负压";
        QTimer::singleShot(waitMs, this, [this]() {
            if (!returnPerformanceTestFlag() || m_peAbortHoming || m_pePromptingRegrab) {
                return;
            }
            peSendFirstPending(QString::fromUtf8("PE抓杯等负压后读"));
        });
        return;
    }

    if (picking && commandIndex == m_peGrabJudgeIndex) {
        bool suction = false;
        quint32 air = 0;
        GlobalData::analyzeWhetherGripperSuction(recvdataList, suction, air);
        const bool held = GlobalData::isHandsVacuumStillSealed(air);
        QLOG_INFO() << "[PE抓杯] 读负压 命令号" << commandIndex
                    << "air" << air << "held" << held
                    << "加深次数" << m_peGrabDeepenTimes;
        if (!held) {
            if (m_peGrabDeepenTimes >= 3) {
                QLOG_WARN() << "[PE抓杯] 加深3次仍未吸住，先不抬";
                peRequestRegrabPrompt();
                return;
            }
            ++m_peGrabDeepenTimes;
            const QByteArray deepen = peBuildGrabDeepenCmd(commandIndex);
            if (deepen.isEmpty()) {
                peRequestRegrabPrompt();
                return;
            }
            QByteArrayList one;
            one.append(deepen);
            QLOG_WARN() << "[PE抓杯] 负压未到，再下" << m_peGrabDeepenTimes << "mm，泵保持开";
            emit sendorderList(one, QString::fromUtf8("PE抓杯负压未到再下"));
            return;
        }
        m_peGrabDeepenTimes = 0;
        m_peGrabOk = true;
    }

    if (picking && commandIndex == m_peGrabLiftIndex) {
        peUpdateGrabOkFromAck(recvdataList, commandIndex);
        if (!m_peGrabOk) {
            markPeCommandCompleteOnly(commandIndex);
            QLOG_WARN() << "[PE抓杯] 抬Z后气密未吸住，进入重抓";
            peRequestRegrabPrompt();
            return;
        }
        m_peRegrab = false;
        // 抓取段已通过，后续放杯/丢杯不再做气密拦截
        m_peGrabArmed = false;
    }

	this->executeCommand(mPE, slaveAddr, commandIndex);
}




void Performanceverification::outputResult()
{
	auto *pdata = findChannel(m_peChannel);
	if (!pdata) {
		QLOG_DEBUG() << "PE输出结果为空";
		return;
	}

	// 创建精度格式化函数
	auto formatDouble = [](double value, int precision = 10) -> QString {
		return QString::number(value, 'f', precision);
	};

	// 更新输出使用的公式
	QLOG_DEBUG() << "PE计算公式: [log10(测量值 / 标准浊度物) / log10(waterLevel / basicSolutions)] × 100";
	QLOG_DEBUG() << "具体参数:";
	QLOG_DEBUG() << "  - waterLevel: " << formatDouble(pdata->waterLevel);
	QLOG_DEBUG() << "  - basicSolutions: " << formatDouble(pdata->basicSolutions);
	QLOG_DEBUG() << "  - basicLiquidLevel(高值): " << formatDouble(pdata->basicLiquidLevel);
	QLOG_DEBUG() << "  - basicLiquidLevelM(中值): " << formatDouble(pdata->basicLiquidLevelM);
	QLOG_DEBUG() << "  - basicLiquidLevelL(低值): " << formatDouble(pdata->basicLiquidLevelL);

	const double epsilon = std::numeric_limits<double>::epsilon();
	const double multiplier = 100.0;

	// 检查基础值有效性
	if (pdata->waterLevel <= epsilon) {
		QLOG_ERROR() << "Invalid waterLevel value: " << pdata->waterLevel;
		return;
	}

	if (pdata->basicSolutions <= epsilon) {
		QLOG_ERROR() << "Invalid basicSolutions value: " << pdata->basicSolutions;
		return;
	}

	// 使用double类型进行计算，避免精度丢失
	const double waterLevelDouble = static_cast<double>(pdata->waterLevel);
	const double basicSolutionsDouble = static_cast<double>(pdata->basicSolutions);

	// 计算分母的基础值
	const double denominatorBase = waterLevelDouble / basicSolutionsDouble;
	if (denominatorBase <= epsilon) {
		QString outText = QString("无效的分母基础值 (waterLevel / basicSolutions): %1").arg(formatDouble(denominatorBase));
		QLOG_ERROR() << outText;
		emit outErrInfo("PE测试失败", outText);
		return;
	}

	emit controlChannelRevolve(m_peChannel, false);

	std::array<double, 3> outData;
	const double measurements[] = {
		static_cast<double>(pdata->basicLiquidLevel),
		static_cast<double>(pdata->basicLiquidLevelM),
		static_cast<double>(pdata->basicLiquidLevelL)
	};

	const char* levelNames[] = { "高值", "中值", "低值" };

	// 计算分母的log10值（固定值）- 使用高精度double
	double denominatorLog = std::log10(denominatorBase);
	QLOG_DEBUG() << "固定分母计算:";
	QLOG_DEBUG() << "  waterLevel / basicSolutions: " << formatDouble(waterLevelDouble)
		<< " / " << formatDouble(basicSolutionsDouble) << " = " << formatDouble(denominatorBase);  // 修复：改为除法符号

	QLOG_DEBUG() << "  log10(分母): log10(" << formatDouble(denominatorBase, 15)
		<< ") = " << formatDouble(denominatorLog, 15);

	for (int i = 0; i < 3; ++i) {
		// 计算分子的基础值 - 使用double避免精度丢失
		double moleculeBase = measurements[i] / basicSolutionsDouble;

		QLOG_DEBUG() << levelNames[i] << "计算:";
		QLOG_DEBUG() << "  分子基础值 (" << levelNames[i] << "测量值 / 标准浊度物): "  // 修复：改为除法描述
			<< formatDouble(measurements[i], 15) << " / " << formatDouble(basicSolutionsDouble, 15)  // 修复：改为除法符号
			<< " = " << formatDouble(moleculeBase, 15);

		if (moleculeBase <= epsilon) {
			QLOG_ERROR() << "  分子基础值必须大于0才能计算log10，当前值: " << formatDouble(moleculeBase);
			outData[i] = 0.0;
		}
		else {
			// 计算分子的log10值
			double moleculeLog = std::log10(moleculeBase);
			QLOG_DEBUG() << "  log10(分子): log10(" << formatDouble(moleculeBase, 15)
				<< ") = " << formatDouble(moleculeLog, 15);
			QLOG_DEBUG() << "  log10(分母): " << formatDouble(denominatorLog, 15);

			// 计算比值：log10(分子) / log10(分母) - 使用高精度计算
			if (std::fabs(denominatorLog) <= epsilon) {
				QLOG_ERROR() << "  分母log10值接近零，避免除零错误: " << formatDouble(denominatorLog);
				outData[i] = 0.0;
			}
			else {
				double ratio = moleculeLog / denominatorLog;
				outData[i] = ratio * multiplier;

				QLOG_DEBUG() << "  精确比值计算:";
				QLOG_DEBUG() << "    分子log10: " << formatDouble(moleculeLog, 15);
				QLOG_DEBUG() << "    分母log10: " << formatDouble(denominatorLog, 15);
				QLOG_DEBUG() << "    比值: " << formatDouble(moleculeLog, 15) << " / "
					<< formatDouble(denominatorLog, 15) << " = " << formatDouble(ratio, 15);
				QLOG_DEBUG() << "    最终结果(" << levelNames[i] << "): " << formatDouble(ratio, 15)
					<< " × " << multiplier << " = " << formatDouble(outData[i], 15);
			}
		}

		// 输出结果
		QLOG_DEBUG() << "  " << levelNames[i] << "结果: " << formatDouble(outData[i]) << "%";
	}

	FullyAutomatedPlatelets::pinstanceequipmentconfig()->HandleoutputResultData(
		pdata->indexnumPE, pdata->channelIndex, outData);

	// 输出最终结果汇总
	QLOG_INFO() << "PE测试最终结果(对数比值) - 高值:" << formatDouble(outData[0])
		<< "%, 中值:" << formatDouble(outData[1])
		<< "%, 低值:" << formatDouble(outData[2]) << "%";
	return;
}  
