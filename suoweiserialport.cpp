#pragma execution_character_set("utf-8")

#include "suoweiserialport.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QSerialPortInfo>
#include "cglobal.h"
#include "loginui.h"
#include "genericfunctions.h"
#include "globaldata.h"
#include <QtConcurrent>
#include <QSemaphore>
#include <QQueue>
#include <operclass/fullyautomatedplatelets.h>
#include "SerialPortException.h"


SuoweiSerialPort::SuoweiSerialPort(QObject *parent) : QObject(parent)
  ,mTestMoveTrayTube(false),mTotalCommd(0), mTestMoveTrayTubeFinished(0),
  m_threadPool(new QThreadPool(this))
{
    qRegisterMetaType<QSerialPort::SerialPortError>("SerialPortError");

    m_writeTimer = new QTimer();
    this->moveToThread(&m_thread);
    m_writeTimer->moveToThread(&m_thread);
    QLOG_DEBUG()<<"串口类线程初始化构造函数";


    connect(m_writeTimer, &QTimer::timeout,
            this, &SuoweiSerialPort::processWriteQueue,
            Qt::DirectConnection);  // 直接连接


    // 线程内初始化定时器（关键修正）
    connect(&m_thread, &QThread::started, this, [this]() {
        // 在工作线程内初始化定时器
        QMutexLocker locker(&m_writeMutex);  // 线程同步
        m_writeTimer->setInterval(5);
        m_writeTimer->setSingleShot(true);

        startSerialthread();  // 执行串口初始化
    });

    // 启动线程事件循环
    connect(&m_thread, &QThread::finished, []{ QLOG_DEBUG() << "Thread exited"; });


}

SuoweiSerialPort::~SuoweiSerialPort()
{
    // 安全释放资源
    m_threadPool->waitForDone();

	if (m_thread.isRunning())
	{
		m_thread.quit();
		m_thread.wait();
	}

    slotcloseseirport();
}

void  SuoweiSerialPort::suoweiportthreadbegin()
{

    // 检查线程是否已启动或正在运行
    if (m_thread.isRunning()) {
        return;
    }
    //实时数据采集：TimeCriticalPriority + 确保代码无阻塞调用
    m_thread.setObjectName("SuoweiSerialPortThread");
    //m_thread.setPriority(QThread::HighPriority  );

    // 添加启动错误检查
    try {
        m_thread.start(QThread::TimeCriticalPriority);
    } catch (const std::exception& e) {
        QLOG_ERROR() << "Failed to start thread:" << e.what();
        // 可以考虑添加错误处理逻辑，如重试或通知上层
    }
}

void  SuoweiSerialPort::startSerialthread()
{
    // 初始化连接（在构造函数中添加）
    connect(this, &SuoweiSerialPort::writeRequested,
            this, &SuoweiSerialPort::writedataToEquipment,
            Qt::QueuedConnection);

    m_buffer.clear();
    m_threadPool->setMaxThreadCount(QThread::idealThreadCount()); // 动态设置线程数
    QLOG_DEBUG()<<"start SuoweiSerialPort threadId =:"<<QThread::currentThreadId()<<endl;

    InstancingSerialandOpen(true);
}


void SuoweiSerialPort::cleanupSerialPort()
{
    if (mserialPort) {
        disconnect(mserialPort, nullptr, this, nullptr); // 断开所有信号
        if (mserialPort->isOpen()) {
            mserialPort->close();
        }
        mserialPort->deleteLater();
        mserialPort = nullptr;
    }
    m_errorSignalConnected = false;
}

QString SuoweiSerialPort::getSerialPortErrorString(QSerialPort* port)
{
    if (!port) return "串口对象无效";

    switch (port->error()) {
    case QSerialPort::PermissionError:
        return "端口被占用或无权限";
    case QSerialPort::DeviceNotFoundError:
        return "设备未找到";
    case QSerialPort::OpenError:
        return "打开失败，设备可能已被占用";
    case QSerialPort::WriteError:
        return "写入错误";
    default:
        return port->errorString();
    }
}

void SuoweiSerialPort::handleInitializationFailure(const QString& error)
{
    QLOG_ERROR() << "串口初始化失败: " << error;
    emit connectionStateChanged(false);

    // 清理资源
    cleanupSerialPort();

    // 可以添加重试逻辑或通知用户
    emit outRminderWidget("连接错误", error);
}

void SuoweiSerialPort::InstancingSerialandOpen(const bool initconnect)
{
    //添加线程安全保护
    QMutexLocker locker(&m_serialMutex);

    // 清理现有连接（如果存在）
    cleanupSerialPort();

    // 创建新串口对象
    mserialPort = new QSerialPort(this);

    // 连接信号（使用Qt::AutoConnection让Qt决定最佳连接方式）
    connect(mserialPort, &QSerialPort::readyRead,
            this, &SuoweiSerialPort::slotReadData,
            Qt::DirectConnection);

    //保持DirectConnection用于实时写入反馈
    connect(mserialPort, &QSerialPort::bytesWritten,
            this, &SuoweiSerialPort::handleBytesWritten,
            Qt::DirectConnection);
    try {
        // 查找串口
        if (!findAndValidateSerialPort()) {
            QLOG_WARN() << "未找到有效的仪器端口号!";
            throw SerialPortException("未找到仪器端口号");
        }

        // 尝试打开串口
        if (!openSerialPort(cglobal::gserialPortName)) {
            QString errorMsg = getSerialPortErrorString(mserialPort);
            QLOG_WARN() << "串口打开失败: " << errorMsg;
            throw SerialPortException(errorMsg.toStdString());
        }

        // 成功打开后连接错误信号（确保只连接一次）
        if (!m_errorSignalConnected) {
           connect(mserialPort, &QSerialPort::errorOccurred,
                                     this, &SuoweiSerialPort::handleSerialError,
                                     Qt::QueuedConnection);
          m_errorSignalConnected = true;
        }

        QLOG_INFO() << "串口打开成功: " << cglobal::gserialPortName;

		if (!initconnect) {
			//重连状态是否遍历模组和主板信息
			emit  prepareReconnectGetData();
			if (cglobal::g_StartTesting) {
				prepareReconnectTesting();
			}
		}
		else {
			emit openequipmentCleanActive(); // 发送成功信号
			emit connectionStateChanged(true); // 明确通知连接状态变化
		}
    } catch (const SerialPortException& e) {
        handleInitializationFailure(e.what());
    }
}

//找串口号
bool SuoweiSerialPort::findAndValidateSerialPort()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        // 检查 VID 和 PID 是否有效，避免无效设备匹配
        if (info.hasVendorIdentifier() && info.hasProductIdentifier() &&
            info.vendorIdentifier() == VID_NUM &&
            info.productIdentifier() == PID_NUM)
        {
            cglobal::gserialPortName = info.portName(); // 存储串口名
            return true; // 找到匹配设备，立即返回 true
        }
    }
    return false; // 遍历结束未找到，返回 false
}


void SuoweiSerialPort::slotcloseseirport()
{
	if (m_thread.isRunning()){
		m_thread.quit();
		m_thread.wait();
	}

	QMutexLocker locker(&m_serialMutex);
    if(mserialPort)
    {
        if(mserialPort->isOpen()){
           mserialPort->clear();
           mserialPort->close();
        }

		m_writeTimer->stop();

        delete mserialPort;
        mserialPort = nullptr;
    }
	// 定时器同样处理
	if (m_writeTimer) {
		m_writeTimer->stop();
		delete m_writeTimer;
		m_writeTimer = nullptr;
	}

    QLOG_DEBUG() << "退出串口线程id:"<< QThread::currentThreadId()<<"文件["<<__FILE__<<"]";
}




// 字节写入回调（线程安全）
void SuoweiSerialPort::handleBytesWritten(qint64 bytes) {
    QMutexLocker locker(&m_writeMutex);
    if (!m_writeQueue.isEmpty() && !m_writeTimer->isActive()) {
        m_writeTimer->start();  // 直接启动定时器
    }
    //emit connectionStateChanged(true);
}




void  SuoweiSerialPort::handleRecvdata(){

    const quint64 bytesAvailable = mserialPort->bytesAvailable();
    if(bytesAvailable <= 0) return;

    // 使用局部变量读取数据，减少锁持有时间
    QByteArray newData = mserialPort->readAll();
    const int newSize = newData.size();

    // 提前检查缓冲区溢出
    {
        QMutexLocker locker(&m_bufferMutex);
        if (m_buffer.size() + newSize > MAX_BUFFER_SIZE) {
            // 计算需要保留的空间，优先保留新数据
            int overflow = (m_buffer.size() + newSize) - MAX_BUFFER_SIZE;
            int removeCount = qMin(overflow, m_buffer.size());
            m_buffer.remove(0, removeCount);
            QLOG_WARN() << "Buffer overflow! Removed" << removeCount << "bytes";
        }
        m_buffer.append(newData);
    }

    // 性能测试模式判断移到循环外
    const bool isPerfTest = Performanceverification::instance()->returnPerformanceTestFlag();


    // 分帧处理
    while (m_buffer.size() >= PROTOCOL_LENGTH) {
        QByteArray frame;
        {
            QMutexLocker locker(&m_bufferMutex);
            frame = m_buffer.left(PROTOCOL_LENGTH);
            m_buffer.remove(0, PROTOCOL_LENGTH);
        }

        if (isPerfTest) {
            processFrame(frame);
        } else {
            // 使用 lambda 捕获 frame 的拷贝，避免共享数据问题
            QtConcurrent::run(m_threadPool, [this, frame]() {
                processFrame(frame);
            });
        }
    }
}

void SuoweiSerialPort::processFrame(const QByteArray& frame) {

    // 参数校验
    if (frame.size() < PROTOCOL_LENGTH) {
        QLOG_WARN() << "Invalid frame size:" << frame.size();
        return;
    }

    // 直接访问数据，避免不必要的转换
    quint8 slaveAddr = static_cast<quint8>(frame[0]);

	// 转换为十六进制字符串列表
	QString framedata = frame.toHex(' ').toUpper();
	#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
		QStringList recvDataList = framedata.split(' ', Qt::SkipEmptyParts);
	#else
		QStringList recvDataList = framedata.split(' ', QString::SkipEmptyParts);
	#endif

    // 直接使用 QByteArray 操作可能更高效
    Parsing_received_messages(recvDataList); // 修改解析函数以直接处理 QByteArray

    if (slaveAddr == Z_AXIS) {
       
        toReadSuckAirsValBack(recvDataList);
    }
}


//串口接收消息root
void  SuoweiSerialPort::slotReadData()
{
   handleRecvdata();

}


void SuoweiSerialPort::toReadSuckAirsValBack(const QStringList& recv_data){

    quint8 commandIndex = GlobalData::analyzeCommandIndex(recv_data); //命令编号
    int commandMode, run_status,speedMode ,Action_motor; //完成状态
    GlobalData::Parse4thBytecommandstatus(recv_data,commandMode,run_status,speedMode,Action_motor);

    const bool isHandsControl = (Action_motor == MOTOR_HANDS_INDEX);
    int  airvalue = GlobalData::backHandsSuckAirValue(recv_data);
    if(isHandsControl && commandIndex == HANDS_SUCK_NUM){
        emit backHandssuck(true,airvalue);
    }else if(isHandsControl && commandIndex == HANDS_SPLIT_NUM){
        emit backHandssuck(false,airvalue);
    }
}

// 新增队列处理函数
void SuoweiSerialPort::processWriteQueue() {
    QPair<QByteArray, QString> task;

    // 获取待处理任务
    {
        QMutexLocker locker(&m_writeMutex);
        if (m_writeQueue.isEmpty()) return;
        task = m_writeQueue.dequeue();
    }

    // 设备状态检查
    if (!mserialPort || !mserialPort->isOpen()) {
        QLOG_ERROR() << "串口未初始化或已关闭" << task.second;
        clearWriteQueue();
        emit connectionStateChanged(false);
        return;
    }

    // USB流量控制（关键优化）
    const qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    const qint64 elapsed = currentTime - m_lastWriteTime;
    const qint64 minInterval = 15; // 最小写入间隔(ms)

    // 动态延时算法：基于历史间隔的加权平均
    if (elapsed < minInterval) {  // 最小写入间隔
        QThread::usleep((minInterval - elapsed) * 1000); // 微秒级休眠
        // 动态调整：繁忙时增加休眠时间
        //if (delayNeeded > 5) {
            //m_writeTimer->setInterval(qMin(m_writeTimer->interval() + 1, 20));
        //}
    }

    // 执行写入
    qint64 bytesWritten = mserialPort->write(task.first);

    // 错误处理
    if (bytesWritten == -1) {
        QLOG_ERROR() << "异步写入失败:" << task.second
                      << "错误:" << mserialPort->errorString();
        clearWriteQueue();
        emit connectionStateChanged(false);
    }
    else {
        m_lastWriteTime = currentTime;

        // 触发下一次写入
        QMutexLocker locker(&m_writeMutex);
        if (!m_writeQueue.isEmpty()) {
            m_writeTimer->start();
        }
    }
}
void SuoweiSerialPort::writedataToEquipment(const QByteArray arry, QString kindCommad) {

    // 跨线程调用保护：强制切换到对象所属线程
	if (&m_thread != QThread::currentThread()) {
		QMetaObject::invokeMethod(this, [this, arry, kindCommad]() {
			writedataToEquipment(arry, kindCommad);
		}, Qt::QueuedConnection);
		return;
	}

	// 2. 数据有效性检查
	if (arry.size() != PROTOCOL_LENGTH) {
		QLOG_WARN() << "[" << kindCommad << "] 数据长度异常加入写列队失败:"
			<< arry.size() << "(expected:" << PROTOCOL_LENGTH << ")";
		return;
	}

     //队列化管理写入请求
    {
        QMutexLocker locker(&m_writeMutex);
		m_writeQueue.enqueue(qMakePair(arry, kindCommad));
	
       // 首次写入立即触发
	   const bool isState = m_writeTimer->isActive();
       if (m_writeQueue.size() == 1 && !isState) {
           m_writeTimer->start();
       }
    }
}
void SuoweiSerialPort::slotwrite_instructions_group(const QByteArrayList& dataList,
                                                        const QString& info){
    if (dataList.isEmpty()) {
        qWarning() << "Empty instruction group received for" << info;
        return;
    }

    //使用范围循环替代迭代器
    for (const QByteArray& data : dataList) {
        // 优化5: 添加数据有效性检查
        if (data.isEmpty()) {
			QLOG_WARN() << info << "数据为空";
            continue;
        }

		if (data.size() != PROTOCOL_LENGTH) {
			QLOG_WARN() << info << "数据长度异常:" << data.size();
			continue;
		}
        emit writeRequested(data, info); // 使用信号槽自动处理线程切换
    }
}

void SuoweiSerialPort::write_data(const QByteArrayList dataList)
{
	QString info = "无定义";
    if (dataList.isEmpty()) {
        qWarning() << "Empty instruction group received for" << info;
        return;
    }

    //使用范围循环替代迭代器
    for (const QByteArray& data : dataList) {
        // 优化5: 添加数据有效性检查
        if (data.isEmpty()) {
			QLOG_WARN() << "Skipping empty instruction in group:" << info;
            continue;
        }
		if (data.size() != PROTOCOL_LENGTH) {
			QLOG_WARN() << info << "数据长度异常:" << data.size();
			continue;
		}
        emit writeRequested(data, info); // 使用信号槽自动处理线程切换
    }
}


void SuoweiSerialPort::closePort() {
	QMutexLocker locker(&m_serialMutex);
	if (mserialPort && mserialPort->isOpen()) {
		mserialPort->close();
	}
}
bool SuoweiSerialPort::_bufferfull(const int buffer_num, const int _addr)
{
    if(buffer_num >= 255 && !mreminderbufferfull)
    {
       mreminderbufferfull = true;
       emit BufferFullState(_addr);
       return false;
    }
    return true;
}

bool SuoweiSerialPort::processModuleInfo(int moduleAddress, const QStringList& receivedData)
{
    // 定义模块地址范围常量
    static constexpr std::pair<int, int> VALID_MODULE_RANGE{MODULE_1, MODULE_3};

    // 检查地址有效性
    const bool isValidAddress = (moduleAddress >= VALID_MODULE_RANGE.first)
                                 && (moduleAddress <= VALID_MODULE_RANGE.second);

    if (!isValidAddress) {
       return false;
    }

    // 发送带校验的数据信号
    emit moduleprotocolData(
        moduleAddress, receivedData
    );
    return true;
}

bool SuoweiSerialPort::processMachineData(int deviceAddress,int functionCode,
                                           const QStringList& receivedData){

    // 第一步：快速失败检查
    const bool isTargetDevice = (deviceAddress == Z_AXIS) || (deviceAddress == MAIN_CONTROL);
    if (!isTargetDevice || functionCode != MOTOR_FUNCTIONCODE) {
        return false;
    }

    // 第二步：定义状态优先级（根据业务需求排序）
    enum ProcessPriority {
        TestingState,
        PerformanceVerification,
        NormalOperation
    };

    // 第三步：原子操作获取状态（保证线程安全）
    const bool isTesting = cglobal::g_StartTesting;
    const bool isPerfTest = Performanceverification::instance()->returnPerformanceTestFlag();

    // 第四步：按优先级处理状态
    if (isTesting) {
        emit sendTestingRecvData(receivedData);
    } else if (isPerfTest) {
        emit serialPEdata(receivedData);
    } else {
        emit serialrecvedata(receivedData);
    }
    return true;
}




bool SuoweiSerialPort::processMainBoardInfo(int deviceAddress,
                                            int functionCode,
                                            const QStringList& receivedData)
{
    // 定义协议相关常量
    constexpr int STATUS_DATA_INDEX = 2;
     constexpr int MIN_DATA_SIZE = 3;

    // 快速失败检查
    if (deviceAddress != MAIN_CONTROL) {
        return false;
    }

    // 协议处理分发
    switch (functionCode) {
        case MAINBOARD_FUNCTIONCODE: {
            // 数据有效性验证
            if (receivedData.size() < MIN_DATA_SIZE) {
                QLOG_ERROR() << "主控状态数据长度不足，预期至少"
                              << MIN_DATA_SIZE << "项，实际收到"
                              << receivedData.size();
                return false;
            }
            // 安全类型转换
            bool conversionOk = false;
            int stateCode = receivedData[STATUS_DATA_INDEX].toInt(&conversionOk, HEX_SWITCH);
            if (!conversionOk) {
                QLOG_ERROR() << "主控状态码转换失败：" << receivedData[STATUS_DATA_INDEX];
                return false;
            }
            // 状态码处理
            if (stateCode == EVERYTIME_READ_MAINSTATE) {
                emit mainControlBoardProtocolData(receivedData);
            } else {
                //emit consumablesWriteStatus(receivedData); // 启用耗材状态通知
            }
            return true;
        }
        case MAINBOARD_PARA_FUNCT: {
            StoredParameters(receivedData);
            return true;
        }

        default:
         QLOG_DEBUG() << "未支持的主板功能码：" << functionCode;
         return false;
     }
}

void SuoweiSerialPort::Parsing_received_messages(const QStringList data_list)
{
    auto safeHexToInt = [](const QString& str, bool& ok) {
            return str.toInt(&ok, HEX_SWITCH); // 直接使用16进制转换
    };

    bool ok = false;
    int slave_addr = safeHexToInt(data_list[0], ok);//从机地址
    if (!ok) {
        QLOG_ERROR() << "从机地址解析失败，非法的十六进制值:"
                    << data_list[0] << "完整数据:" << data_list;
        return;
    }

    int func_code = safeHexToInt(data_list[1], ok); //功能码
    if (!ok) {
        QLOG_ERROR() << "功能码解析失败，非法的十六进制值:"
                    << data_list[1] << "完整数据:" << data_list;
        return;
    }

    int buffer_num = safeHexToInt(data_list[3], ok);//缓存容量
    if (!ok) {
        QLOG_ERROR() << "缓存容量解析失败，非法的十六进制值:"
                    << data_list[3] << "完整数据:" << data_list;
        return;
    }
    if (slave_addr < MODULE_1 || slave_addr > MAIN_CONTROL) {
        QLOG_ERROR() << "从机地址越界，地址值: 0x" << QString::number(slave_addr, HEX_SWITCH).toUpper()
                    << "有效范围: [0x" << QString::number(MODULE_1, HEX_SWITCH).toUpper()
                    << "[0x" << QString::number(MAIN_CONTROL, HEX_SWITCH).toUpper() << "]"
                    << "原始数据:" << data_list;
        return;
    }

    //命令内存状态
    if(_bufferfull(buffer_num,slave_addr) == false)
        return;

    //解析模组数据
    if(processModuleInfo(slave_addr,data_list) == true)
        return;

    //仪器运动指令
    if(processMachineData(slave_addr,func_code,data_list) == true)
        return;

    //主控参数
    if(processMainBoardInfo(slave_addr,func_code,data_list) == true)
        return;
}





// 新增配置函数（支持动态配置）
//稳定性要求高：1Mbps 对硬件和线缆质量要求极高
//时序精度敏感：微小的时序误差就会导致通信失败
//驱动程序支持：不是所有 USB 转串口芯片都支持 1Mbps
void SuoweiSerialPort::applySerialPortConfig(const QString &portname)
{
    mserialPort->setPortName(portname);

    mserialPort->setBaudRate(QSerialPort::Baud1000000);
    mserialPort->setDataBits(QSerialPort::Data8);
    mserialPort->setStopBits(QSerialPort::OneStop);
    mserialPort->setParity(QSerialPort::NoParity);
    mserialPort->setFlowControl(QSerialPort::NoFlowControl);
    // 对于 1Mbps，必须使用硬件流控制
    //mserialPort->setFlowControl(QSerialPort::HardwareControl);
    //++
    mserialPort->setDataTerminalReady(true);
    mserialPort->setRequestToSend(true);
    mserialPort->setReadBufferSize(MAX_BUFFER_SIZE);
}

// 新增错误处理函数
void SuoweiSerialPort::handleOpenError(const QString &errorMsg)
{
    mserialPort->clearError();
    QLOG_ERROR()<<"串口异常:"<<errorMsg;
}

// 统一状态更新
void SuoweiSerialPort::updateGlobalConnectionStatus(bool status)
{
    // 优化10：原子操作避免多线程冲突
    QMutexLocker locker(&mutex);
    cglobal::gserialConnecStatus = status;
}

bool  SuoweiSerialPort::openSerialPort(const QString portname)
{
	if (!mserialPort) {
		emit outRminderWidget("连接错误", "请重开软件初始化");
		return false;
	}
    //检查串口状态避免重复打开
    if (mserialPort->isOpen()) {
        mserialPort->clear();
        mserialPort->close();
        QLOG_WARN() << "串口已重新配置: " << portname;
    }

    //增强错误处理机制
    bool isOpened = false;
    mserialPort->setPortName(portname);
    if(mserialPort->open(QIODevice::ReadWrite))
    {
        //提取配置为独立函数（便于动态调整）在open()之后配置参数
        applySerialPortConfig(portname);

        //验证波特率是否生效
        if(mserialPort->baudRate() != 1000000) {
            QLOG_ERROR() << "警告: 波特率未生效，实际=" << mserialPort->baudRate();
            // 考虑回退策略，如尝试500000
        }

        isOpened = true;
        //使用信号通知代替直接修改全局变量
        emit connectionStateChanged(true);
    }
    else
    {
        const QString errorMsg = "串口打开异常: " + mserialPort->errorString();
        QLOG_ERROR() << errorMsg;  // 提升为ERROR级别日志

        //错误
        handleOpenError(errorMsg);
        isOpened = false;
    }

    // 优化7：统一状态管理
    updateGlobalConnectionStatus(isOpened);
    return isOpened;
}


//USB断开关闭串口
void SuoweiSerialPort::recvdisConnectCloseSerial() {
	
    QMutexLocker locker(&m_serialMutex); // 保持线程安全

	if (mserialPort && mserialPort->isOpen()) {
			// 1. 断开所有信号连接
			disconnect(mserialPort, nullptr, this, nullptr);

			// 2. 清空缓冲区并关闭串口
			mserialPort->clear();         // 清空待处理数据
			mserialPort->close();         // 关闭串口连接
			QLOG_INFO() << "串口已关闭: " << cglobal::gserialPortName;

			// 3. 重置错误信号标志
			m_errorSignalConnected = false;

			// 4. 通知连接状态变化
            emit connectionStateChanged(false);
	}
}


//断线重连
void SuoweiSerialPort::prepareReconnect()
{
	//QMutexLocker locker(&m_serialMutex);

	// 1. 断开现有连接
	recvdisConnectCloseSerial();

	// 2. 等待资源释放完成
	QEventLoop loop;
	QTimer::singleShot(150, &loop, &QEventLoop::quit);
	loop.exec();

	// 3. 重新初始化
	InstancingSerialandOpen(false);

	
    return;
}

//重连恢复测试
void SuoweiSerialPort::prepareReconnectTesting(){
    QByteArrayList zbackorig;
    QUIUtils::allZAxisBackOrigin(zbackorig, 100);
    slotwrite_instructions_group(zbackorig, "重连先复位所有Z轴");
    QLOG_ERROR() << "测试中断重连,复位所有Z轴!" << endl;
    emit connectEquipmentagin();
}




void SuoweiSerialPort::safeClosePort() {
    //检查端口是否已关闭或无实例
    if (!mserialPort || !mserialPort->isOpen()) {
        QLOG_DEBUG() << "端口已关闭或无实例，跳过关闭操作";
        return;
    }

    // 关闭前清除错误状态（防止连锁错误）
    mserialPort->clearError();
    mserialPort->clear();

    // 执行关闭操作
    mserialPort->close();
    QLOG_INFO() << "串口已安全关闭";


}



//解析到电机运动状态
int SuoweiSerialPort::Recve_Motor_motion_state(const QStringList recv_data , const int Index)
{
    int Motor_motion_state = recv_data.at(4).toUInt(nullptr, 16);
    const int Binary = 2; //二进制
    QString hexMessage = QString("%1").arg(Motor_motion_state, 0, 10);
    QString Statestr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));
    int leng = Statestr.length();
    switch (Index)
    {
        case 0:
            Motor_motion_state = Statestr.mid(leng - 3, Binary).toUInt(nullptr, Binary);  //电机状态
        break;
        case 1:
            Motor_motion_state = Statestr.mid(leng - 5, Binary).toUInt(nullptr, Binary);  //运动模式
        break;
        case 2:
            Motor_motion_state = Statestr.mid(0, 3).toUInt(nullptr, Binary);  //运动的电机
        break;
    default:
        break;
    }
    return Motor_motion_state;
}




//解析运动到的位置
int SuoweiSerialPort::ParseSportLocation(const QStringList recv_data ,const int Index)
{
    int Sport_Location = 0;
    const int Binary = 2; //二进制
    quint8 area_nums = recv_data.at(5).toUInt(nullptr, 16);
    QString hexMessage = QString("%1").arg(area_nums, 0, 10);
    QString areanumstr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));

    switch (Index)
    {
        case 0:
            Sport_Location = areanumstr.mid(4, Binary).toUInt(nullptr, Binary);  //运动到哪个区域
        break;
        case 1:
            Sport_Location = areanumstr.mid(6, Binary).toInt(nullptr, Binary); //运动的时候相对与哪根针
        break;
        case 2:
            Sport_Location = areanumstr.mid(0, 1).toInt(nullptr, Binary);     //血样针或者试剂针液面探测状态 0：未探测到
        break;
        case 3:
            Sport_Location = areanumstr.mid(0, 1).toInt(nullptr, Binary);     //抓手夹试管是否夹到试管 0：有夹到
        break;
    default:
        break;
    }
    return Sport_Location;
}





void SuoweiSerialPort::ResolutionCalibrationOperation(const QStringList SportBuffer)
{
    if(mpInstrumentCalibration->size() == 0)
    {
        mbCalibration = false;
        return ;
    }
    int bFinish = Recve_Motor_motion_state(SportBuffer, THESTATEOFMOTOR);      //电机的状态
    int condenum = QString(SportBuffer.at(COMMANDNUMBER)).toInt(nullptr,16);   //命令编号
    int MainBoardfuncode = QString(SportBuffer.at(FUNCTIONALMA)).toInt(nullptr, 16); //功能玛
    int ErrorBit = QString(SportBuffer.last()).toInt(nullptr,16);
    if(bFinish == MOTOR_FINISH && MainBoardfuncode != MAINBOARD_FUNCTIONCODE)
    {
        auto iter = mpInstrumentCalibration->begin();
        while(iter != mpInstrumentCalibration->end())
        {
            InstrumentCalibration *temporaryValue = *iter;
            int cmd_num = QString(temporaryValue->ActionCode.at(COMMANDNUMBER)).toInt(nullptr,16); //保存命令的编号
            if(condenum == cmd_num && temporaryValue->Finished == false)
            {
              temporaryValue->Finished = true;
              break;
            }
            iter++;
        }
     }
    else if(bFinish != MOTOR_FINISH && MainBoardfuncode != MAINBOARD_FUNCTIONCODE)
    {
        QString ErrorStr = QString("仪器校准异常 [命令编号:%1] [提示电机:%2] ==>(剩余仪器校准命令重发)").arg(condenum)
                .arg(QUIUtils::MotorStateMapStr(bFinish,ErrorBit));
        emit Execute_exception_prompt(ERRORLOG,ErrorStr,NotActiveRunning); //执行异常提示
        return ;
    }
    bool allcomplete = true;
    auto it = mpInstrumentCalibration->begin();
    while (it != mpInstrumentCalibration->end())
    {
        InstrumentCalibration *temporaryValue = *it;
        if (temporaryValue->Finished == false) {
            allcomplete = false;
            break;
        }
        it++;
    }
    if(allcomplete)
    {
        mbCalibration = false;
        DeleteInstrumentCalibrationCode();
    }
    return;
}

void SuoweiSerialPort::DeleteInstrumentCalibrationCode()
{
    if(mpInstrumentCalibration != nullptr)
    {
        qDeleteAll(mpInstrumentCalibration->begin(),mpInstrumentCalibration->end());
        mpInstrumentCalibration->clear();
        delete mpInstrumentCalibration;
        mpInstrumentCalibration = nullptr;
    }
}



//解析 按钮复位  按钮清洗 校准复位 完成状态
bool SuoweiSerialPort::FinishedActive(InstrumentReset_Vec *pStu)
{
    bool allcomplete = true;
    auto iter = pStu->begin();
    while(iter != pStu->end())
    {
        InstrumentReset *temporaryValue = *iter;
        if(temporaryValue->Finished == false)
        {
            allcomplete = false;
            break;
        }
        iter++;
    }
    if(allcomplete)
    {
        mbInstrumentreset = false;
        //int index = mpInstrumentReset->first()->Activeindex;
        DeleteInstrumentResetCode();

    }
    return allcomplete;
}
void SuoweiSerialPort::DeleteInstrumentResetCode()
{
    if(mpInstrumentReset != nullptr)
    {
        qDeleteAll(mpInstrumentReset->begin(),mpInstrumentReset->end());
        mpInstrumentReset->clear();
        delete mpInstrumentReset;
        mpInstrumentReset= nullptr;
    }
}
void SuoweiSerialPort::ResolutionResetOperation(const QStringList SportBuffer)
{
    if(mpInstrumentReset->size() == 0)
    {
        mbInstrumentreset = false;
        return ;
    }
    int bFinish = Recve_Motor_motion_state(SportBuffer, THESTATEOFMOTOR);      //电机的状态
    int condenum = QString(SportBuffer.at(COMMANDNUMBER)).toInt(nullptr,16);   //命令编号
    int MainBoardfuncode = QString(SportBuffer.at(FUNCTIONALMA)).toInt(nullptr, 16); //功能玛
    int ErrorBit = QString(SportBuffer.last()).toInt(nullptr,16);
    if(bFinish == MOTOR_FINISH && MainBoardfuncode != MAINBOARD_FUNCTIONCODE)
    {
        auto iter = mpInstrumentReset->begin();
        while(iter != mpInstrumentReset->end())
        {
            InstrumentReset *temporaryValue = *iter;
            int cmd_num = QString(temporaryValue->ActionCode.at(COMMANDNUMBER)).toInt(nullptr,16); //保存命令的编号
            if(condenum == cmd_num && temporaryValue->Finished == false)
            {
              temporaryValue->Finished = true;
              break;
            }
            iter++;
        }
     }
    else if(bFinish != MOTOR_FINISH && MainBoardfuncode != MAINBOARD_FUNCTIONCODE)
    {
        QString ErrorStr = QString("仪器复位异常:[命令编号:%1] [提示电机:%2 %3] ==>(剩余仪器复位命令重发)").arg(condenum)
                .arg(QUIUtils::MotorStateMapStr(bFinish,ErrorBit));
        emit Execute_exception_prompt(ERRORLOG,ErrorStr,btndoingActive); //执行异常提示
        return;
    }
    FinishedActive(mpInstrumentReset);
    return;
}


void SuoweiSerialPort::Recv_TrayMoveTest(const QByteArrayList arrycommd,
                                         const bool btestTray,
                                         const quint8 totalnum)
{
    write_data(arrycommd);
    mTestMoveTrayTube = btestTray;
    mTotalCommd = totalnum;
}


void SuoweiSerialPort::TestMoveTrayTube_recv(const QStringList backArry)
{
    int bFinish = Recve_Motor_motion_state(backArry, 0);//电机的状态
    int MainBoardfuncode = QString(backArry.at(1)).toInt(nullptr, 16); //功能玛
    if(bFinish == MOTOR_FINISH && MainBoardfuncode != 22)
    {
        mTestMoveTrayTubeFinished++;
        if(mTestMoveTrayTubeFinished == mTotalCommd)
        {
            mTestMoveTrayTubeFinished = 0;
            mTestMoveTrayTube = false;
            this->continueSendArryTray();
        }
    }
    return;
}



void SuoweiSerialPort::StoredParameters(const QStringList& recvdata) {
    // 边界检查（防止越界崩溃）
    if (recvdata.size() < 13) { // 确保能访问最大索引13
        QLOG_ERROR() << "协议数据长度不足：" << recvdata.size();
        return;
    }

    // 单次获取单例实例（减少调用开销）
    ConsumablesOper* consumables = ConsumablesOper::GetpInstance();

    // 安全转换命令号
    bool convertOk = false;
    quint8 cmd_num = recvdata[ProtocolIndex::CMD_NUM].toUInt(&convertOk, 16);
    if (!convertOk) {
        QLOG_ERROR() << "无效命令号格式：" << recvdata[ProtocolIndex::CMD_NUM];
        return;
    }

    // 命令分发处理
    switch (cmd_num) {
        case 3: handleCmd3(recvdata, consumables); break;
        case 5: handleCmd5(recvdata, consumables); break;
        default: QLOG_WARN() << "未知命令号：" << cmd_num<<"接收的数据"<<recvdata;
        break;
    }
}

// 公用函数：安全更新试剂限制
void SuoweiSerialPort::updateReagentLimit(const QStringList& data, int index,
                                        int reagentType, ConsumablesOper* consumables) {
    bool convertOk;
    quint8 value = data[index].toUInt(&convertOk, 16);
    if (convertOk) {
        consumables->updateReagentLimit(WRITE_OPERAT, reagentType, value);
    } else {
        QLOG_ERROR() << "无效限制值[" << index << "]:" << data[index];
    }
}
// 公用函数：解析高低字节十六进制对
quint16 SuoweiSerialPort::parseHexPair(const QStringList& data, int highIndex,
                                     int lowIndex, bool& convertOk) {
    convertOk = false;
    if (highIndex >= data.size() || lowIndex >= data.size()) return 0;

    bool highOk, lowOk;
    quint16 high = data[highIndex].toUShort(&highOk, 16);
    quint16 low = data[lowIndex].toUShort(&lowOk, 16);

    if (highOk && lowOk) {
        convertOk = true;
        return (high << 8) | low;
    }
    return 0;
}

// 命令3处理函数
void SuoweiSerialPort::handleCmd3(const QStringList& recvdata, ConsumablesOper* consumables) {
    // 更新试剂限制值
    for (int k = 0; k < ProtocolIndex::REAGENT_LIMIT_COUNT; k++) {
        updateReagentLimit(recvdata, ProtocolIndex::REAGENT_LIMIT_START + k, k, consumables);
    }

    // 更新试管限制
    updateReagentLimit(recvdata, ProtocolIndex::TUBE_LIMIT,
                      INDEX_TESTTUBE_CONSUMABLE, consumables);

    // 更新清洗剂限制
    updateReagentLimit(recvdata, ProtocolIndex::CLEAN_LIMIT,
                      INDEX_CLEANLINQUE_CONSUMABLE, consumables);

    // 处理清洗剂总量（高低字节拼接）
    bool convertOk;
    quint16 dataCleanTol = parseHexPair(recvdata, ProtocolIndex::CLEAN_TOTAL_HIGH,
                                      ProtocolIndex::CLEAN_TOTAL_LOW, convertOk);

    if (convertOk) {
        consumables->updateReagentTotal(WRITE_OPERAT,
                                        INDEX_CLEANLINQUE_CONSUMABLE,
                                      dataCleanTol);
        QLOG_DEBUG() << "获取仪器清洗剂容量：" << dataCleanTol;
    }
}


// 命令5处理函数
void SuoweiSerialPort::handleCmd5(const QStringList& recvdata, ConsumablesOper* consumables) {
    // 试剂类型映射表
    static const std::array<QPair<const char*, int>, 5> REAGENT_MAP = {{
        {"AA", AA_REAGENT}, {"ADP", ADP_REAGENT},
        {"EPI", EPI_REAGENT}, {"COL", COL_REAGENT}, {"RIS", RIS_REAGENT}
    }};

    for (int k = 0; k < REAGENT_MAP.size(); k++) {
        bool convertOk;
        quint16 datavalue = parseHexPair(recvdata,
                                        2 * k + ProtocolIndex::REAGENT_TOTAL_START + 1,
                                        2 * k + ProtocolIndex::REAGENT_TOTAL_START,
                                        convertOk);

        if (!convertOk) continue;

        consumables->updateReagentTotal(WRITE_OPERAT, k, datavalue);
        QLOG_DEBUG() << QString("解析到主板%1耗材容量：%2")
                        .arg(REAGENT_MAP[k].first)
                        .arg(datavalue);
    }
}



void SuoweiSerialPort::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError)
        return;


    QString errorMsg = getSerialPortErrorString(mserialPort);
    QLOG_WARN() << "串口异常: " << errorMsg;

    QLOG_ERROR() << "Serial port error occurred:"<< mserialPort->errorString()
                << "(" << error << ")";
}

/**
 * 线程安全地清空写入队列
 * @note 调用此方法会丢弃所有待发送的数据包
 */
void SuoweiSerialPort::clearWriteQueue()
{
    QMutexLocker locker(&m_writeMutex);

    if (!m_writeQueue.isEmpty()) {
        QLOG_INFO() << "清空写入队列，丢弃" << m_writeQueue.size() << "个待发送数据包";
        m_writeQueue.clear();
    }

    // 确保定时器停止
    if (m_writeTimer && m_writeTimer->isActive()) {
        m_writeTimer->stop();
    }
}



//解析存在板子里的参数
/*void SuoweiSerialPort::StoredParameters(const QStringList recvdata)
{
    int indexCmd_num = 2;
    quint8 cmd_num = QString(recvdata.at(indexCmd_num)).toUInt(nullptr,16);
    if(cmd_num == 3)
    {
        for(int k = 0 ; k < 5 ;k++)
        {
            quint8 datavalue = QString(recvdata.at(k+5)).toUInt(nullptr,16);
            ConsumablesOper::GetpInstance()->updateReagentLimit(WRITE_OPERAT,k ,datavalue);
        }
        quint8 dataTubeLimit = QString(recvdata.at(10)).toUInt(nullptr,16);
        ConsumablesOper::GetpInstance()->updateReagentLimit(WRITE_OPERAT,INDEX_TESTTUBE_CONSUMABLE,dataTubeLimit);

        quint8 datacleanLimit = QString(recvdata.at(11)).toUInt(nullptr,16);
        ConsumablesOper::GetpInstance()->updateReagentLimit(WRITE_OPERAT,INDEX_CLEANLINQUE_CONSUMABLE,datacleanLimit);

        QString cleanTotal = recvdata.at(13) + recvdata.at(12);
        quint16 dataCleanTol = cleanTotal.toInt(nullptr,16);
        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,INDEX_CLEANLINQUE_CONSUMABLE ,dataCleanTol);
        QLOG_DEBUG()<<"获取仪器清洗剂容量"<<dataCleanTol;
    }
    else if(cmd_num == 5)
    {
        for(int k = 0; k < 5; k++)
        {
            QString valuestr = recvdata.at(2 *k + 6) + recvdata.at(2 *k + 5);
            quint16 datavalue = valuestr.toInt(nullptr,16);
            ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT,k ,datavalue);
            int index_reag = k + 1;
            QString _output;
            switch(index_reag){
                case AA_REAGENT: _output = QString("解析到主板耗材容量(AA)%1").arg(datavalue);
                break;
                case ADP_REAGENT: _output = QString("解析到主板耗材容量(ADP)%1").arg(datavalue);
                break;
                case EPI_REAGENT:_output = QString("解析到主板耗材容量(EPI)%1").arg(datavalue);
                break;
                case COL_REAGENT:_output = QString("解析到主板耗材容量(COL)%1").arg(datavalue);
                break;
                case RIS_REAGENT:_output = QString("解析到主板耗材容量(RIS)%1").arg(datavalue);
                break;
                default:QLOG_DEBUG()<<"读取主板耗材容量异常";break;
            }
            QLOG_DEBUG()<<_output<<endl;
        }
    }
}*/
