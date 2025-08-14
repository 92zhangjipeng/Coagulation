#ifndef PERFORMANCEVERIFICATION_H
#define PERFORMANCEVERIFICATION_H

#pragma  once
#include <QMap>
#include <QObject>
#include <QPointer>
#include <QGlobalStatic>
#include <QPoint>
#include <QVector>
#include <QMap>
#include <QTimer>
#include <unordered_map>
#include <functional>

// 定义函数指针类型（或std::function）
using CommandFunc = std::function<void(int, int)>;


#define HEIGHVALUE      0
#define MIDVALUE        1
#define LOWVALUE        2
#define BASICSOLUTIONS  3

#define THROWPEWATER  0
#define THROWHEIGHVAL 1
#define THROWMIDVAL   2
#define THROWLOWVAL   3
#define THROWBASIC    4



struct ActiveCommand
{
    QByteArray cmdData;
    quint8 phaseNumber = 0;     // 编号
    bool isCompleted = false;   // isCompleted
    quint8 ksuck  = 0;
    quint8 ksplit = 0;
    ActiveCommand() = default;

	// 匹配插入操作的构造函数
    ActiveCommand(const QByteArray& arr, quint8 n, bool finished,quint8 sucknum,quint8 spiltnum)
        : cmdData(arr), phaseNumber(n), isCompleted(finished), 
		ksuck(sucknum), ksplit(spiltnum){}
};
using CommandGroup = QMap<quint8, ActiveCommand>;

struct PerformanceData
{
    QString indexnumPE;
    quint8 channelIndex = 0;    // 通道索引 (原indexChannel)
    QPoint handsChannelCoord;
    bool   peCompleted = false;

    // 试管配置
    quint8 waterTubeCount = 0;       // 水加样到试管号
    quint8 basicLiquidCount = 0;     //标准物质的试管号
    quint8 basicLiquidTubeCount = 0; // 基础液加样到试管号
    quint8 basicLiquidTubeCountM = 0;
    quint8 basicLiquidTubeCountL = 0;  //地值试管孔号

    //===== 坐标系统 =====//
    QPoint waterTubeCoord;       // 水管坐标
    QPoint basicLiquidTubeCoord; //标准物坐标
    QPoint basicLiquidCoord;     // 基础液路坐标
    QPoint basicLiquidCoordM;
    QPoint basicLiquidCoordL;


    QPoint handswaterTubeCoord;
    QPoint handsBasicLinuidTubeCoord;
    QPoint handsBasicLiquidCoord;
    QPoint handsBasicLiquidCoordM;
    QPoint handsBasicLiquidCoordL;

    //===== 控制参数 =====//
    // 水位控制
    int waterLevel = 1;           // 水阀值
    int basicSolutions = 1;
    int basicLiquidLevel = 1;     // 基础液路值
    int basicLiquidLevelM = 1;
    int basicLiquidLevelL = 1;

    //===== 操作指令组 =====//
    // 吸水分装
    struct SuckOperation {       // 合并相关字段
        bool enabled = false;
        CommandGroup commands;
    } suckWater, suckBasicLiquid;

    // 测试操作
    struct TestOperation {
        bool enabled = false;
        CommandGroup commands;
    } testWater,testStandardsolutions ,testBasicLiquid,testBasicLiquidM,testBasicLiquidL;

    // 排放操作
    struct DrainOperation {
        bool enabled = false;
        CommandGroup commands;
    } drainWater, drainStandardsolutions,drainBasicLiquid,drainBasicLiquidM,drainBasicLiquidL;

    // 清洁操作
    struct Cleaning {
        bool enabled = false;
        CommandGroup commands;
    } cleaningSystem;

    //===== 构造函数 =====//
    PerformanceData() = default;

    // 全参数构造
    PerformanceData(QString num,quint8 ch, bool statePE,quint8 waterTubes, quint8 liquidTubes,
                    const QPoint& waterPos, const QPoint& liquidPos,
                    int waterLv, int liquidLv)
        : indexnumPE(num),
          channelIndex(ch),
          peCompleted(statePE),
          waterTubeCount(waterTubes),
          basicLiquidTubeCount(liquidTubes),
          waterTubeCoord(waterPos),
          basicLiquidCoord(liquidPos),
          waterLevel(waterLv),
          basicLiquidLevel(liquidLv) {}
};

using PerformanceDataset = QVector<PerformanceData* >;  // 性能数据容器

enum class activePE{
     suckWater = 0,
     suckBasicLinque ,
     testingWater,
     ThrowWater,
     testingStandardSolutions,
     testingBasicWater,
     testingBasicWaterM,
     testingBasicWaterL,
     ThrowStandardSolutions,
     ThrowBasicLinque,
     ThrowBasicLinqueM,
     ThrowBasicLinqueL,
     CleanPin
};

class Performanceverification : public QObject
{
    Q_OBJECT

public:
	explicit Performanceverification(QObject *parent = nullptr);
	~Performanceverification();

    static Performanceverification* instance();

	static const QString& getCommandName() {
		static const QString name = tr("GetTestValueCommand");
		return name;
	}

    void allfinishClearData();

    void logMessage(const QString& msg);
    void gettestChannelList(const QMap<QString, quint8> &channelList);

	void setPerformanceTestFlag(const bool &isstart);
	bool returnPerformanceTestFlag();

    /** 找第一个未完成吸吐水样的命令
     * @brief outputSuckSplitWater
     * @param outSend
     * @return  true 有未性能测试的通道 : false 性能检测已完成
     */
    bool outputSuckSplitWater(QByteArrayList &outSend);

	PerformanceData* findChannel(quint8 ch) const;

	template<typename OperationType>
	void collectTestingCommands(QByteArrayList& outSend,
								quint8 channel,
								OperationType PerformanceData::* operationPtr,
								const QString& logTag)
	{
		outSend.clear();
		if (PerformanceData* pData = this->findChannel(channel)) {
			const OperationType& operation = pData->*operationPtr;

			if (operation.enabled) {
				//QLOG_WARN() << logTag << channel << "operation disabled";
				return;
			}

			const CommandGroup& commands = operation.commands;
			outSend.reserve(commands.size());

			// C++11 兼容遍历方式
			for (auto it = commands.constBegin(); it != commands.constEnd(); ++it) {
				const ActiveCommand& cmd = it.value();
				if (!cmd.isCompleted && !cmd.cmdData.isEmpty()) {
					outSend.push_back(cmd.cmdData);
				}
			}

			//QLOG_DEBUG() << logTag << channel
				//<< "sent" << outSend.size() << "commands";
		}
		else {
			//QLOG_ERROR() << logTag << channel << "channel data not found";
		}
	}

    template<typename LiquidContainer>
    bool processLiquidCommands(LiquidContainer& container, const quint8 indexnum) {
        auto& commands = container.commands;
        auto target = std::find_if(commands.begin(), commands.end(),
                                    [indexnum](const auto& cmd) {
                                        return cmd.phaseNumber == indexnum && !cmd.isCompleted;
                                      });
        if (target != commands.end()) {
            target->isCompleted = true;
        }
        bool allFinish = std::all_of(commands.begin(), commands.end(),
                                        [](const auto& cmd) { return cmd.isCompleted; });
        container.enabled = allFinish;
        return allFinish;
    }

    void outPutSuckBasicLiquid(QByteArrayList &outSend);

    void outPutTestingWater(QByteArrayList &outSend);
    void outPutTestingBasicLiquid(QByteArrayList &outSend,const int indexVal);

    void outPutThrowWater(QByteArrayList &outSend);
    void outPutThrowBasicLiquid(QByteArrayList &outSend,const int indexValue);
    void outCleanPin(QByteArrayList &outSend);


    // 命令执行入口函数
    void executeCommand(activePE mPE, int slaveAddr, int commandIndex) {
        // 初始化命令映射表（使用[this]捕获当前对象）
        static const std::unordered_map<activePE, CommandFunc> commandMap = {
            { activePE::suckWater,             [this](int addr, int idx) { this->suckWaterFunc(addr, idx); } },
            { activePE::suckBasicLinque,       [this](int addr, int idx) { this->suckBasicLinqueFunc(addr, idx); } },
            { activePE::testingWater,          [this](int addr, int idx) { this->testWaterFunc(addr, idx); } },
            { activePE::ThrowWater,            [this](int addr, int idx) { this->throwWaterFunc(addr, idx); } },
            { activePE::testingStandardSolutions, [this](int addr, int idx) { this->testStandardsolutionsFunc(addr, idx); } },
            { activePE::ThrowStandardSolutions, [this](int addr, int idx) { this->throwStandardSolutions(addr, idx); } },
            { activePE::testingBasicWater,     [this](int addr, int idx) { this->testBasicLinqueFunc(addr, idx); } },
            { activePE::ThrowBasicLinque,      [this](int addr, int idx) { this->throwBasicLinqueFunc(addr, idx); } },
            { activePE::testingBasicWaterM,    [this](int addr, int idx) { this->testBasicMidLinqueFunc(addr, idx); } },
            { activePE::ThrowBasicLinqueM,     [this](int addr, int idx) { this->throwBasicMidLinqueFunc(addr, idx); } },
            { activePE::testingBasicWaterL,    [this](int addr, int idx) { this->testBasicLowLinqueFunc(addr, idx); } },
            { activePE::ThrowBasicLinqueL,     [this](int addr, int idx) { this->throwBasicLowLinqueFunc(addr, idx); } },
            { activePE::CleanPin,              [this](int addr, int idx) { this->cleanPinFunc(addr, idx); } }

            };

        auto it = commandMap.find(mPE);
        if (it != commandMap.end()) {
            it->second(slaveAddr, commandIndex);  // 执行对应的函数
        }
        else {
            // 处理未定义的状态（例如记录错误日志）
            // handleUnexpectedState(mPE);
        }
}


signals:
    void sendorder(const QByteArray arry, QString kindCommad);
    void sendorderList(const QByteArrayList &dataList,
                        const QString &info);

    void obtainChannelVal(const quint8& peChannel,const int &index,bool isstart);
    void controlChannelRevolve(const quint8&channelNum,const bool& isRevolve);

    void outErrInfo(const QString, const QString);


public slots:
    void handleSerialData(const QStringList& );

//构建性能测试
private:
    quint8 outfreeEmptyTube();

    void   runningPE();

    void   insertWaterdata(PerformanceData &dataPE,const QPoint& suckCoord);

    void   insertBasicLinquedata(PerformanceData &dataPE,const QPoint& suckBasicLinqueCoord);

    void   insertTestWaterPEActivedata(PerformanceData &dataPE, const quint8 gripperDown);

    void   insertTestBasicLinquePEActivedata(PerformanceData &dataPE,const quint8 gripperDown);

    void   insetThrowTestingCup(PerformanceData &dataPE,
                                 const QPoint &throwCoord
                                ,const quint8 gripperDown);

    void  insertCleanendPE(PerformanceData &dataPE);

private:
    void suckWaterFunc(const quint8 addr,const quint8 indexnum);
    void testWaterFunc(const quint8 addr,const quint8 indexnum);
    void throwWaterFunc(const quint8 addr,const quint8 indexnum);
    void cleanPinFunc (const quint8 addr,const quint8 indexnum);

    void suckBasicLinqueFunc(const quint8 addr,const quint8 indexnum);

    //标准物质
    void testStandardsolutionsFunc(const quint8 /*addr*/,const quint8 indexnum);
    void throwStandardSolutions(const quint8 /*addr*/,const quint8 indexnum);


    void testBasicLinqueFunc(const quint8, const quint8 indexnum);
    void testBasicMidLinqueFunc(const quint8, const quint8 indexnum);
    void testBasicLowLinqueFunc(const quint8 ,const quint8 indexnum);

    void throwBasicLinqueFunc(const quint8, const quint8 indexnum);
    void throwBasicMidLinqueFunc(const quint8 ,const quint8 indexnum);
    void throwBasicLowLinqueFunc(const quint8 ,const quint8 indexnum);


    void outputResult();

    void TrageIndexTube(const quint8& index, const int&lossKind);

    enum class TestType { Water, Basic,High, Mid, Low };
    enum LiquidLevel { BasicValue,HighValue, MidValue, LowValue };
    void obtainChannelTestVal(int& val, TestType testType);

private:
	
	Q_DISABLE_COPY(Performanceverification) // 禁止拷贝和赋值
	Performanceverification(Performanceverification&&) = delete;
	Performanceverification& operator=(Performanceverification&&) = delete;

    bool  mstartPerformance;
    //std::vector<std::unique_ptr<PerformanceDataset>> mdatatest;
   
    PerformanceDataset mdatatest;
    int m_peChannel;
    activePE mPE;

};

#endif // PERFORMANCEVERIFICATION_H
