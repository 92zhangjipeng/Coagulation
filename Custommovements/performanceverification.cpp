#include "performanceverification.h"  //性能验证
#include <loadequipmentpos.h>
#include <cglobal.h>
#include <ini_file.h>
#include <operclass/fullyautomatedplatelets.h>
#include <QGlobalStatic>
#include <globaldata.h>
#include <mainwindow.h>


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
void configureChannelCoordinates(PerformanceData& data, SingletonAxis* axis) {
    axis->chnZoneAxisPos(READ_OPERRAT, data.channelIndex,
                        MOTOR_HANDS_INDEX, data.handsChannelCoord);
}
// 测试台坐标配置
void configureTestTaryPositions(PerformanceData& data, SingletonAxis* axis) {
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
    QLOG_DEBUG()<<"性能验证通道"<<channelList;

    const quint8 waterHole = 0;
    const quint8 basicLinque = 1;

    mdatatest.reserve(channelList.size());

    auto* axis = SingletonAxis::GetInstance();
    auto &ini = INI_File();

    QPoint suckWaterCoord(0, 0),suckBasicCoord(0,0),throwCoord(0,0);
    axis->bloodSampleZonePos(READ_OPERRAT, waterHole, suckWaterCoord);
    axis->bloodSampleZonePos(READ_OPERRAT, basicLinque, suckBasicCoord);
    axis->throwTubeHolePos(READ_OPERRAT,throwCoord);

    auto iter = channelList.cbegin();
    while(iter != channelList.cend()){

        std::unique_ptr<PerformanceData> pdataPE(new PerformanceData);
        pdataPE->indexnumPE =  iter.key();

        pdataPE->channelIndex = iter.value() - 1;
        const quint8 gripperDown = ini._gethandsdownheiht(pdataPE->channelIndex);
        configureChannelCoordinates(*pdataPE,axis);

        // 试管配置
        pdataPE->waterTubeCount = outfreeEmptyTube();
        pdataPE->basicLiquidCount = outfreeEmptyTube();
        pdataPE->basicLiquidTubeCount = outfreeEmptyTube();
        pdataPE->basicLiquidTubeCountM = outfreeEmptyTube();
        pdataPE->basicLiquidTubeCountL = outfreeEmptyTube();

        // 统一配置测试台坐标
        configureTestTaryPositions(*pdataPE, axis);

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

        insetThrowTestingCup(*pdataPE,throwCoord,gripperDown);

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

void Performanceverification::insertTestWaterPEActivedata(PerformanceData &dataPE,const quint8 gripperDown){
    QByteArrayList outdataList;
    quint8 ksuck,ksplit;
    QUIUtils::suckCupMoveChannelPut(dataPE.handswaterTubeCoord,
                                    dataPE.handsChannelCoord,
                                    gripperDown,
                                    outdataList,ksuck,ksplit);
    dataPE.testWater.enabled = false;
	//dataPE.testWater.commands.reserve(outdataList.size());
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

void Performanceverification::insertTestBasicLinquePEActivedata(PerformanceData &dataPE,const quint8 gripperDown){
    QByteArrayList outdataList;
    quint8 ksuck =0;
    quint8 ksplit = 0;

    auto processCommands =[&](QByteArrayList &dataList, PerformanceData::TestOperation &liquidData, const QString &logPrefix){
        liquidData.enabled = false;
        // liquidData.commands.reserve(dataList.size()); // 启用预分配（如需）
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
                                    gripperDown, outdataList, ksuck, ksplit);
    processCommands(outdataList, dataPE.testStandardsolutions, "PE测试标准物质");

    // 高值处理
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLiquidCoord, dataPE.handsChannelCoord,
                                    gripperDown, outdataList, ksuck, ksplit);
    processCommands(outdataList, dataPE.testBasicLiquid, "PE测试标准物质高值");

    // 中值处理
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLiquidCoordM, dataPE.handsChannelCoord,
                                    gripperDown, outdataList, ksuck, ksplit);
    processCommands(outdataList, dataPE.testBasicLiquidM, "PE测试标准物质中值");

    // 低值处理
    QUIUtils::suckCupMoveChannelPut(dataPE.handsBasicLiquidCoordL, dataPE.handsChannelCoord,
                                    gripperDown, outdataList, ksuck, ksplit);
    processCommands(outdataList, dataPE.testBasicLiquidL, "PE测试标准物质低值");

}

void Performanceverification::insetThrowTestingCup(PerformanceData &dataPE,
                                                    const QPoint &throwCoord
                                                   ,const quint8 gripperDown){
    QByteArrayList throwCupsList;
    quint8 ksuck,ksplit;
    QUIUtils::ThrowChannelCup(gripperDown,
                              dataPE.handsChannelCoord,
                              throwCoord,
                              ksuck,
                              ksplit,
                              throwCupsList);
    dataPE.drainWater.enabled = false;
    dataPE.drainBasicLiquid.enabled = false;
    dataPE.drainBasicLiquidM.enabled = false;
    dataPE.drainBasicLiquidL.enabled = false;
	//dataPE.drainWater.commands.reserve(throwCupsList.size());
	//dataPE.drainBasicLiquid.commands.reserve(throwCupsList.size());
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

        // 预分配内存提升性能
        outSend.reserve(commands.size());

        // 使用const迭代器遍历
        for (CommandGroup::const_iterator cmdIt = commands.cbegin();
                cmdIt != commands.cend(); ++cmdIt)
        {
            const ActiveCommand& cmd = cmdIt.value();
            if (!cmd.isCompleted) {
                outSend.push_back(cmd.cmdData);

                // 如需更新完成状态（根据实际需求）
                // const_cast<ActiveCommand&>(cmd).isCompleted = true;
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
    collectTestingCommands(outSend, m_peChannel,&PerformanceData::suckBasicLiquid,
                           "标准物加样PEChannel");
}

void Performanceverification::outPutTestingWater(QByteArrayList &outSend){

    mPE = activePE::testingWater;
    collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::testWater,
                            "测试水PEChannel");

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
    return;
}

void Performanceverification::outPutThrowWater(QByteArrayList &outSend){
    mPE = activePE::ThrowWater;
    collectTestingCommands(outSend, m_peChannel,&PerformanceData::drainWater,
                            "弃杯水PEChannel");
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
    return;
}

void Performanceverification::outCleanPin(QByteArrayList &outSend){
    mPE = activePE::CleanPin;
    collectTestingCommands(outSend, m_peChannel,
                            &PerformanceData::cleaningSystem,
                            "清洗针PEChannel");
    return;
}

void Performanceverification::TrageIndexTube(const quint8& index, const int&lossKind){
    FullyAutomatedPlatelets::pinstancesqlData()->UpdateEmptyTube_State(index,lossKind);
}

void Performanceverification::suckWaterFunc(const quint8 addr,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE吸蒸馏水遍历指令为空";
        return;
    }
	
    bool isFinished = processLiquidCommands(pdata->suckWater, indexnum);
	//QLOG_WARN() << "PE NO." << indexnum<<"STATE="<< isFinished;
    if( isFinished){
        TrageIndexTube(pdata->waterTubeCount,TESTTUBES_SAMPLED);
        QByteArrayList suckBasicLiqune;
        outPutSuckBasicLiquid(suckBasicLiqune);
        emit sendorderList(suckBasicLiqune,"加样");
    }
}

void Performanceverification::testWaterFunc(const quint8 addr,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE测试蒸馏水遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->testWater, indexnum);

	if (isFinished) {
		TrageIndexTube(pdata->waterTubeCount, TESTTUBES_CLIPPEDAWAY);
		const int configDelay = INI_File()._getdelayedtime();
		const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
		QLOG_DEBUG() << "蒸馏水放到通道测试开始去读延时" << delayTime << "获取通道值";
		//实验读取数据
		emit obtainChannelVal(m_peChannel, THROWPEWATER, true);
		QTimer::singleShot(delayTime, this, [=, this]() {
			obtainChannelTestVal(pdata->waterLevel, TestType::Water);
            QLOG_DEBUG() << "PE延时" << delayTime << "结束获取蒸馏水值:" << pdata->waterLevel<<endl;
			emit obtainChannelVal(m_peChannel, THROWPEWATER, false);
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



void Performanceverification::throwWaterFunc(const quint8 addr,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE弃杯蒸馏水遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->drainWater, indexnum);
	if (isFinished) {
		QByteArrayList testBasicLiqune;
		outPutTestingBasicLiquid(testBasicLiqune, BASICSOLUTIONS);
        emit sendorderList(testBasicLiqune, "PE测试标准浊度物");

	}
}

void Performanceverification::cleanPinFunc(const quint8 addr,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE清洗遍历指令为空";
        return;
    }

	bool isFinished = processLiquidCommands(pdata->cleaningSystem, indexnum);
	pdata->peCompleted = true;
	if (isFinished) {
		outputResult();
		runningPE();
	}
}


void  Performanceverification::suckBasicLinqueFunc(const quint8 addr,const quint8 indexnum)
{
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE标准物加样遍历指令为空";
        return;
    }
	bool isFinished = processLiquidCommands(pdata->suckBasicLiquid, indexnum);
	if (isFinished) {
		TrageIndexTube(pdata->basicLiquidTubeCount, TESTTUBES_SAMPLED);
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

	bool isFinished = processLiquidCommands(pdata->testStandardsolutions, indexnum);
	if (isFinished) {
		TrageIndexTube(pdata->basicLiquidCount, TESTTUBES_CLIPPEDAWAY);
		const int configDelay = INI_File()._getdelayedtime();
		const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
        QLOG_DEBUG() << "PE标准浊度物取值开始读延时" << delayTime << "获取通道值";
		emit obtainChannelVal(m_peChannel, THROWBASIC, true);

		QTimer::singleShot(delayTime, this, [=, this]() {
			obtainChannelTestVal(pdata->basicSolutions, TestType::Basic);
            QLOG_DEBUG() << "PE标准浊度物:" << pdata->basicSolutions<<endl;
			emit obtainChannelVal(m_peChannel, THROWBASIC, false);
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
    bool isFinished = processLiquidCommands(pdata->testBasicLiquid, indexnum);
    if(isFinished){
        TrageIndexTube(pdata->basicLiquidTubeCount,TESTTUBES_CLIPPEDAWAY);

        const int configDelay  = INI_File()._getdelayedtime();
        const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;

        QLOG_DEBUG()<<"PE浊度物读取(高)值延时"<<delayTime<<"获取通道值";
        emit obtainChannelVal(m_peChannel,THROWHEIGHVAL,true);

        QTimer::singleShot(delayTime, this, [=,this]() {
            obtainChannelTestVal(pdata->basicLiquidLevel,TestType::High);
            QLOG_DEBUG()<<"PE浊度物高值:"<<pdata->basicLiquidLevel<<endl;
            emit obtainChannelVal(m_peChannel,THROWHEIGHVAL,false);
        });

    }
}

void Performanceverification::testBasicMidLinqueFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE浊度物Mid测试遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->testBasicLiquidM, indexnum);
    if(isFinished){
        TrageIndexTube(pdata->basicLiquidTubeCountM,TESTTUBES_CLIPPEDAWAY);

        const int configDelay  = INI_File()._getdelayedtime();
        const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
        QLOG_DEBUG()<<"浊度物MID到通道读取值开始去读延时"<<delayTime<<"获取通道值";

        //实验读取数据
        emit obtainChannelVal(m_peChannel,THROWMIDVAL,true);

        QTimer::singleShot(delayTime, this, [=,this]() {
            obtainChannelTestVal(pdata->basicLiquidLevelM,TestType::Mid);
            QLOG_DEBUG()<<"PE浊度物MID准物质值:"<<pdata->basicLiquidLevelM<<endl;
            emit obtainChannelVal(m_peChannel,THROWMIDVAL,false);
        });

    }
}

void Performanceverification::testBasicLowLinqueFunc(const quint8 /*addr*/, const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE标准物LOW测试遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->testBasicLiquidL, indexnum);
	if (isFinished) {
		TrageIndexTube(pdata->basicLiquidTubeCountL, TESTTUBES_CLIPPEDAWAY);

		const int configDelay = INI_File()._getdelayedtime();
		const int delayTime = DELAY_READ_TUBE_INITVALUE + configDelay;
		QLOG_DEBUG() << "浊度物LOW取值开始去读延时" << delayTime << "获取通道值";
		emit obtainChannelVal(m_peChannel, THROWLOWVAL, true);
		QTimer::singleShot(delayTime, this, [=, this]() {
			obtainChannelTestVal(pdata->basicLiquidLevelL, TestType::Low);
			QLOG_DEBUG() << "PE浊度物LOW值:" << pdata->basicLiquidLevelL << endl;
			emit obtainChannelVal(m_peChannel, THROWLOWVAL, false);
		});

	}
}

void Performanceverification::throwStandardSolutions(const quint8 /*addr*/,const quint8 indexnum){
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE浊度物丟杯遍历指令为空";
        return;
    }

    bool isFinished = processLiquidCommands(pdata->drainStandardsolutions, indexnum);
    if(isFinished){
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

    bool isFinished = processLiquidCommands(pdata->drainBasicLiquid, indexnum);
    if(isFinished){
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
    bool isFinished = processLiquidCommands(pdata->drainBasicLiquidM, indexnum);
    if( isFinished){

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
    bool isFinished = processLiquidCommands(pdata->drainBasicLiquidL, indexnum);
    if (isFinished){
        QByteArrayList cleanPinList;
        outCleanPin(cleanPinList);
        emit sendorderList(cleanPinList,"PE单通道完成清洗针");
    }
}


//接收
// 执行命令的函数
void Performanceverification::handleSerialData(const QStringList& recvdataList){

    quint8 slaveAddr =   GlobalData::analyzeSlave_addr(recvdataList,false);//从机地址
    quint8 commandIndex = GlobalData::analyzeCommandIndex(recvdataList);   //命令编号
    //完成状态
    int commandMode, run_status,speedMode ,Action_motor;
    GlobalData::Parse4thBytecommandstatus(recvdataList,commandMode,run_status,speedMode,Action_motor);

    QString showErrorlog;
    if(!GlobalData::commandStatus(run_status,showErrorlog)){
        QString error_code = recvdataList.join(" ");
        showErrorlog += error_code;
        QLOG_DEBUG()<<"异常命令:"<<error_code<<showErrorlog<<endl;
        return;
    }

	this->executeCommand(mPE, slaveAddr, commandIndex);
}


void Performanceverification::outputResult()
{
    auto *pdata = findChannel(m_peChannel);
    if(!pdata){
        QLOG_DEBUG()<<"PE输出结果为空";
        return;
    }

    // 定义常量以提高可读性和维护性
    const double epsilon = std::numeric_limits<double>::epsilon();
    const double multiplier = 100.0; // 乘法因子定义为常量

    // 检查分母是否有效：waterLevel 和 waterLevel - basicSolutions 都可能为无效值
    if (pdata->waterLevel <= epsilon) {
        QLOG_ERROR() << "Invalid waterLevel value: " << pdata->waterLevel;
        return;
    }

    // 计算实际分母：waterLevel - basicSolutions
    const double diffDenominator = static_cast<double>(pdata->waterLevel - pdata->basicSolutions);
    if (std::fabs(diffDenominator) <= epsilon) { // 添加额外检查，基于证据1的条件检查原则
        QString outText = QString("无效分母 (waterLevel - basicSolutions) value:%1").arg(diffDenominator);
        QLOG_ERROR() <<outText;
        emit outErrInfo("PE测试失败",outText);
        return; // 提前返回避免除零
    }

    emit controlChannelRevolve(m_peChannel, false);

    std::array<double, 3> outData;
    const double basicSolutionsDouble = static_cast<double>(pdata->basicSolutions); // 一次性转换，减少重复
    const double denominators[] = {
        static_cast<double>(pdata->basicLiquidLevel),
        static_cast<double>(pdata->basicLiquidLevelM),
        static_cast<double>(pdata->basicLiquidLevelL)
    };

    for (int i = 0; i < 3; ++i) {
        // 使用三元运算符模拟合并函数：如果分子无效，提供默认值（如0.0），避免未定义行为
        // 证据1建议使用coalesce处理null，这里扩展为处理无效计算[[1]]
        double numerator = static_cast<double>(denominators[i] - basicSolutionsDouble); //测量值 - 标准浊度物
        if (std::fabs(numerator) <= epsilon) { // 检查分子是否接近零
            QLOG_ERROR() <<"检查分子是否接近零,默认值0,避免无效计算:"<<numerator<<"="<<denominators[i]<<"-"<<basicSolutionsDouble;
            outData[i] = 0.0; // 默认值，避免无效计算
        } else {
            outData[i] = (numerator / diffDenominator) * multiplier  + 10.00;

        }
    }

    FullyAutomatedPlatelets::pinstanceequipmentconfig()->HandleoutputResultData(
            pdata->indexnumPE, pdata->channelIndex, outData);
    return;
}
