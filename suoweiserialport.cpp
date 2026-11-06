#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "suoweiserialport.h"
#include <QCoreApplication>
#include <QDataStream>
#include <QSerialPortInfo>
#include <QEventLoop>
#include <QTimer>
#include <QDateTime>
#include <QtConcurrent>
#include <QSemaphore>
#include <array>

#include "cglobal.h"
#include "loginui.h"
#include "genericfunctions.h"
#include "globaldata.h"
#include "operclass/fullyautomatedplatelets.h"
#include "SerialPortException.h"

SuoweiSerialPort::SuoweiSerialPort(QObject *parent)
    : QObject(parent)
    , mTestMoveTrayTube(false)
    , mTotalCommd(0)
    , mTestMoveTrayTubeFinished(0)
    , m_threadPool(new QThreadPool(this))
{
    qRegisterMetaType<QSerialPort::SerialPortError>("SerialPortError");

    m_writeTimer = new QTimer();
    this->moveToThread(&m_thread);
    m_writeTimer->moveToThread(&m_thread);
    QLOG_DEBUG() << "串口类线程初始化构造函数";

    connect(m_writeTimer, &QTimer::timeout,
            this, &SuoweiSerialPort::processWriteQueue,
            Qt::DirectConnection);

    connect(&m_thread, &QThread::started, this, [this]() {
        {
            QMutexLocker locker(&m_writeMutex);
            m_writeTimer->setInterval(5);
            m_writeTimer->setSingleShot(true);
        }
        startSerialthread();
    });

    connect(&m_thread, &QThread::finished, []{ QLOG_DEBUG() << "Thread exited"; });
}

SuoweiSerialPort::~SuoweiSerialPort()
{
    slotcloseseirport();
}

void SuoweiSerialPort::suoweiportthreadbegin()
{
    if (m_thread.isRunning()) {
        return;
    }
    m_thread.setObjectName("SuoweiSerialPortThread");

    try {
        m_thread.start(QThread::TimeCriticalPriority);
    } catch (const std::exception& e) {
        QLOG_ERROR() << "Failed to start thread:" << e.what();
    }
}

void SuoweiSerialPort::closePort()
{
    QMutexLocker locker(&m_serialMutex);
    if (mserialPort && mserialPort->isOpen()) {
        mserialPort->close();
    }
    m_portOpen = false;
}

void SuoweiSerialPort::slotcloseseirport()
{
    if (m_closing.exchange(true)) {
        if (QThread::currentThread() != &m_thread && m_thread.isRunning()) {
            m_thread.quit();
            m_thread.wait(5000);
        }
        return;
    }

    auto cleanupNow = [this]() {
        if (m_writeTimer) {
            m_writeTimer->stop();
        }
        QMutexLocker locker(&m_serialMutex);
        if (mserialPort) {
            disconnect(mserialPort, nullptr, this, nullptr);
            if (mserialPort->isOpen()) {
                mserialPort->clear();
                mserialPort->close();
            }
            delete mserialPort;
            mserialPort = nullptr;
        }
        if (m_writeTimer) {
            delete m_writeTimer;
            m_writeTimer = nullptr;
        }
        m_portOpen = false;
        m_errorSignalConnected = false;
    };

    const bool onSerialThread = (QThread::currentThread() == &m_thread);
    if (m_threadPool && !onSerialThread) {
        m_threadPool->waitForDone(3000);
    }

    // 退出时 closeSerial 会 Queued 到本线程；这里再 BlockingQueued 自己会永远卡住，进程退不掉
    if (onSerialThread) {
        cleanupNow();
        m_thread.quit();
        QLOG_DEBUG() << "串口线程内关闭端口，准备退出线程";
        return;
    }

    if (m_thread.isRunning()) {
        QMetaObject::invokeMethod(this, [this, cleanupNow]() {
            cleanupNow();
            m_thread.quit();
        }, Qt::BlockingQueuedConnection);
        if (!m_thread.wait(5000)) {
            QLOG_WARN() << "SuoweiSerialPort thread did not stop in time; skip terminate";
        }
    } else {
        cleanupNow();
    }

    QLOG_DEBUG() << "退出串口线程id:" << QThread::currentThreadId() << "文件[" << __FILE__ << "]";
}

void SuoweiSerialPort::slotReadData()
{
    handleRecvdata();
}

void SuoweiSerialPort::handleSerialError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) return;

    QString errorMsg = getSerialPortErrorString(mserialPort);
    QLOG_WARN() << "串口错误:" << errorMsg << "(" << error << ")";

    switch (error) {
        case QSerialPort::DeviceNotFoundError:
        case QSerialPort::PermissionError:
        case QSerialPort::OpenError:
            m_portOpen = false;
            emit connectionStateChanged(false);
            break;
        case QSerialPort::WriteError:
        case QSerialPort::ReadError:
            break;
        case QSerialPort::ResourceError: {
            QMutexLocker locker(&m_serialMutex);
            m_portOpen = false;
            if (mserialPort && mserialPort->isOpen()) {
                mserialPort->close();
            }
            locker.unlock();
            clearReceiveBuffer();
            emit connectionStateChanged(false);
            break;
        }
        default:
            QLOG_DEBUG() << "串口次要错误:" << errorMsg;
            break;
    }
}

void SuoweiSerialPort::prepareReconnect()
{
    static bool running = false;
    if (running) {
        return;
    }
    running = true;

    abortPendingWrites();
    recvdisConnectCloseSerial();
    clearReceiveBuffer();

    bool opened = false;
    for (int attempt = 1; attempt <= 8 && !opened; ++attempt) {
        QEventLoop loop;
        QTimer::singleShot(400, &loop, &QEventLoop::quit);
        loop.exec();
        const bool lastTry = (attempt == 8);
        opened = InstancingSerialandOpen(false, lastTry);
        if (opened) {
            QLOG_INFO() << "串口重连成功，第" << attempt << "次";
        } else {
            QLOG_WARN() << "串口重连第" << attempt << "次未打开";
        }
    }
    running = false;
}

void SuoweiSerialPort::recvdisConnectCloseSerial()
{
    QMutexLocker locker(&m_serialMutex);

    if (mserialPort && mserialPort->isOpen()) {
        disconnect(mserialPort, nullptr, this, nullptr);
        mserialPort->clear();
        mserialPort->close();
        QLOG_INFO() << "串口已关闭: " << cglobal::gserialPortName;

        m_errorSignalConnected = false;
        m_portOpen = false;
        emit connectionStateChanged(false);
    }
}

void SuoweiSerialPort::slotwrite_instructions_group(const QByteArrayList& dataList, const QString& info)
{
    if (dataList.isEmpty()) {
        qWarning() << "Empty instruction group received for" << info;
        return;
    }

    for (const QByteArray& data : dataList) {
        if (data.isEmpty()) {
            QLOG_WARN() << info << "数据为空";
            continue;
        }

        if (data.size() != PROTOCOL_LENGTH) {
            QLOG_WARN() << info << "数据长度异常:" << data.size();
            continue;
        }
        emit writeRequested(data, info);
    }
}

void SuoweiSerialPort::write_data(const QByteArrayList dataList)
{
    QString info = "无定义";
    if (dataList.isEmpty()) {
        qWarning() << "Empty instruction group received for" << info;
        return;
    }

    for (const QByteArray& data : dataList) {
        if (data.isEmpty()) {
            QLOG_WARN() << "Skipping empty instruction in group:" << info;
            continue;
        }
        if (data.size() != PROTOCOL_LENGTH) {
            QLOG_WARN() << info << "数据长度异常:" << data.size();
            continue;
        }
        emit writeRequested(data, info);
    }
}

void SuoweiSerialPort::writedataToEquipment(const QByteArray arry, QString kindCommad)
{
    if (&m_thread != QThread::currentThread()) {
        QMetaObject::invokeMethod(this, [this, arry, kindCommad]() {
            writedataToEquipment(arry, kindCommad);
        }, Qt::QueuedConnection);
        return;
    }

    if (arry.size() != PROTOCOL_LENGTH) {
        QLOG_WARN() << "[" << kindCommad << "] 数据长度异常加入写列队失败:"
                    << arry.size() << "(expected:" << PROTOCOL_LENGTH << ")";
        return;
    }

    QMutexLocker locker(&m_writeMutex);
    m_writeQueue.enqueue(qMakePair(arry, kindCommad));

    const bool isState = m_writeTimer->isActive();
    if (m_writeQueue.size() == 1 && !isState) {
        m_writeTimer->start();
    }
}

void SuoweiSerialPort::processWriteQueue()
{
    if (!m_portOpen || !mserialPort || !mserialPort->isOpen()) {
        QLOG_WARN() << "串口未打开，丢弃写队列，不当作USB掉线";
        clearWriteQueue();
        return;
    }

    QPair<QByteArray, QString> task;
    {
        QMutexLocker locker(&m_writeMutex);
        if (m_writeQueue.isEmpty()) return;
        task = m_writeQueue.dequeue();
    }

    const qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed;
    {
        QMutexLocker locker(&m_lastWriteTimeMutex);
        elapsed = currentTime - m_lastWriteTime;
    }
    const qint64 minInterval = 15;

    if (elapsed < minInterval) {
        QThread::usleep((minInterval - elapsed) * 1000);
    }

    qint64 bytesWritten = mserialPort->write(task.first);

    if (bytesWritten == -1) {
        QLOG_ERROR() << "异步写入失败:" << task.second << "错误:" << mserialPort->errorString();
        clearWriteQueue();
        emit connectionStateChanged(false);
    } else {
        {
            QMutexLocker locker(&m_lastWriteTimeMutex);
            m_lastWriteTime = currentTime;
        }

        QMutexLocker locker(&m_writeMutex);
        if (!m_writeQueue.isEmpty()) {
            m_writeTimer->start();
        }
    }
}

void SuoweiSerialPort::Recv_TrayMoveTest(const QByteArrayList arrycommd,
                                         const bool btestTray,
                                         const quint8 totalnum)
{
    write_data(arrycommd);
    mTestMoveTrayTube = btestTray;
    mTotalCommd = totalnum;
}

void SuoweiSerialPort::startSerialthread()
{
    connect(this, &SuoweiSerialPort::writeRequested,
            this, &SuoweiSerialPort::writedataToEquipment,
            Qt::QueuedConnection);

    m_buffer.clear();
    m_threadPool->setMaxThreadCount(QThread::idealThreadCount());
    QLOG_DEBUG() << "start SuoweiSerialPort threadId =:" << QThread::currentThreadId() << endl;

    InstancingSerialandOpen(true);
}

void SuoweiSerialPort::TestMoveTrayTube_recv(const QStringList backArry)
{
    if (backArry.size() < 5) {
        QLOG_WARN() << "TestMoveTrayTube_recv: 数据长度不足，期望至少5项，实际" << backArry.size();
        return;
    }

    int bFinish = Recve_Motor_motion_state(backArry, 0);
    bool convertOk;
    int MainBoardfuncode = backArry.at(1).toInt(&convertOk, 16);
    if (!convertOk) {
        QLOG_WARN() << "TestMoveTrayTube_recv: 功能码格式无效" << backArry.at(1);
        return;
    }

    if (bFinish == MOTOR_FINISH && MainBoardfuncode != 22) {
        mTestMoveTrayTubeFinished++;
        if (mTestMoveTrayTubeFinished == mTotalCommd) {
            mTestMoveTrayTubeFinished = 0;
            mTestMoveTrayTube = false;
            //this->continueSendArryTray();
        }
    }
}

void SuoweiSerialPort::handleBytesWritten(qint64 bytes)
{
    Q_UNUSED(bytes);
    QMutexLocker locker(&m_writeMutex);
    if (!m_writeQueue.isEmpty() && !m_writeTimer->isActive()) {
        m_writeTimer->start();
    }
}

bool SuoweiSerialPort::findAndValidateSerialPort()
{
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.hasVendorIdentifier() && info.hasProductIdentifier() &&
            info.vendorIdentifier() == VID_NUM &&
            info.productIdentifier() == PID_NUM) {
            cglobal::gserialPortName = info.portName();
            return true;
        }
    }
    return false;
}

bool SuoweiSerialPort::openSerialPort(const QString portname)
{
    if (!mserialPort) {
        emit outRminderWidget("连接错误", "请重开软件初始化");
        return false;
    }

    if (mserialPort->isOpen()) {
        mserialPort->clear();
        mserialPort->close();
        QLOG_WARN() << "串口已重新配置: " << portname;
    }

    bool isOpened = false;
    mserialPort->setPortName(portname);
    if (mserialPort->open(QIODevice::ReadWrite)) {
        applySerialPortConfig(portname);

        if (mserialPort->baudRate() != 1000000) {
            QLOG_ERROR() << "警告: 波特率未生效，实际=" << mserialPort->baudRate();
        }

        isOpened = true;
        m_portOpen = true;
        emit connectionStateChanged(true);
    } else {
        const QString errorMsg = "串口打开异常: " + mserialPort->errorString();
        QLOG_ERROR() << errorMsg;
        handleOpenError(errorMsg);
        isOpened = false;
        m_portOpen = false;
    }

    updateGlobalConnectionStatus(isOpened);
    return isOpened;
}

void SuoweiSerialPort::applySerialPortConfig(const QString &portname)
{
    mserialPort->setPortName(portname);
    mserialPort->setBaudRate(QSerialPort::Baud1000000);
    mserialPort->setDataBits(QSerialPort::Data8);
    mserialPort->setStopBits(QSerialPort::OneStop);
    mserialPort->setParity(QSerialPort::NoParity);
    mserialPort->setFlowControl(QSerialPort::NoFlowControl);
    mserialPort->setDataTerminalReady(true);
    mserialPort->setRequestToSend(true);
    mserialPort->setReadBufferSize(MAX_BUFFER_SIZE);
}

void SuoweiSerialPort::handleOpenError(const QString &errorMsg)
{
    mserialPort->clearError();
    QLOG_ERROR() << "串口异常:" << errorMsg;
}

void SuoweiSerialPort::updateGlobalConnectionStatus(bool status)
{
    QMutexLocker locker(&mutex);
    cglobal::gserialConnecStatus = status;
}

void SuoweiSerialPort::safeClosePort()
{
    if (!mserialPort || !mserialPort->isOpen()) {
        QLOG_DEBUG() << "端口已关闭或无实例，跳过关闭操作";
        return;
    }

    mserialPort->clearError();
    mserialPort->clear();
    mserialPort->close();
    m_portOpen = false;
    QLOG_INFO() << "串口已安全关闭";
}

void SuoweiSerialPort::StoredParameters(const QStringList& recvdata)
{
    if (recvdata.size() < 13) {
        QLOG_ERROR() << "协议数据长度不足：" << recvdata.size();
        return;
    }

    ConsumablesOper* consumables = ConsumablesOper::GetpInstance();

    bool convertOk = false;
    quint8 cmd_num = recvdata[ProtocolIndex::CMD_NUM].toUInt(&convertOk, 16);
    if (!convertOk) {
        QLOG_ERROR() << "无效命令号格式：" << recvdata[ProtocolIndex::CMD_NUM];
        return;
    }

    switch (cmd_num) {
        case 3: handleCmd3(recvdata, consumables); break;
        case 5: handleCmd5(recvdata, consumables); break;
        default: QLOG_WARN() << "未知命令号：" << cmd_num << "接收的数据" << recvdata; break;
    }
}

void SuoweiSerialPort::updateReagentLimit(const QStringList& data, int index,
                                        int reagentType, ConsumablesOper* consumables)
{
    Q_UNUSED(consumables);
    bool convertOk;
    quint8 value = data[index].toUInt(&convertOk, 16);
    if (convertOk) {
        ConsumablesOper::updateReagentLimit(WRITE_OPERAT, reagentType, value);
    } else {
        QLOG_ERROR() << "无效限制值[" << index << "]:" << data[index];
    }
}

quint16 SuoweiSerialPort::parseHexPair(const QStringList& data, int highIndex,
                                     int lowIndex, bool& convertOk)
{
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

void SuoweiSerialPort::handleCmd3(const QStringList& recvdata, ConsumablesOper* consumables)
{
    for (int k = 0; k < ProtocolIndex::REAGENT_LIMIT_COUNT; k++) {
        updateReagentLimit(recvdata, ProtocolIndex::REAGENT_LIMIT_START + k, k, consumables);
    }

    updateReagentLimit(recvdata, ProtocolIndex::TUBE_LIMIT,
                      INDEX_TESTTUBE_CONSUMABLE, consumables);

    updateReagentLimit(recvdata, ProtocolIndex::CLEAN_LIMIT,
                      INDEX_CLEANLINQUE_CONSUMABLE, consumables);

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

void SuoweiSerialPort::handleCmd5(const QStringList& recvdata, ConsumablesOper* consumables)
{
    Q_UNUSED(consumables);
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

        ConsumablesOper::updateReagentTotal(WRITE_OPERAT, k, datavalue);
        QLOG_DEBUG() << QString("解析到主板%1耗材容量：%2")
                        .arg(REAGENT_MAP[k].first)
                        .arg(datavalue);
    }
}

namespace {

bool frameLooksAligned(const QByteArray &buffer)
{
    if (buffer.size() < 2) {
        return false;
    }
    const quint8 addr = static_cast<quint8>(buffer.at(0));
    const quint8 func = static_cast<quint8>(buffer.at(1));
    if (addr < MODULE_1 || addr > MAIN_CONTROL) {
        return false;
    }
    return func == MAINBOARD_PARA_FUNCT
            || func == MAINBOARD_FUNCTIONCODE
            || func == MOTOR_FUNCTIONCODE;
}

}

void SuoweiSerialPort::clearReceiveBuffer()
{
    QWriteLocker locker(&m_bufferLock);
    m_buffer.clear();
    mreminderbufferfull = false;
}

QStringList SuoweiSerialPort::instrumentPortNames() const
{
    QStringList names;
    const auto ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        if (info.hasVendorIdentifier() && info.hasProductIdentifier()
                && info.vendorIdentifier() == VID_NUM
                && info.productIdentifier() == PID_NUM) {
            names.append(info.portName());
        }
    }
    return names;
}

int SuoweiSerialPort::pendingWriteCount()
{
    QMutexLocker locker(&m_writeMutex);
    return m_writeQueue.size();
}

void SuoweiSerialPort::handleRecvdata()
{
    if (!mserialPort || !m_portOpen) return;

    QByteArray newData;
    while (mserialPort->bytesAvailable() > 0 && newData.size() < MAX_BUFFER_SIZE) {
        const qint64 chunk = qMin<qint64>(mserialPort->bytesAvailable(), 1024);
        newData.append(mserialPort->read(chunk));
    }
    if (newData.isEmpty()) {
        return;
    }

    {
        QWriteLocker locker(&m_bufferLock);
        if (m_buffer.size() + newData.size() > MAX_BUFFER_SIZE) {
            QLOG_WARN() << "接收缓冲溢出，整段丢弃后重新对齐"
                        << (m_buffer.size() + newData.size());
            m_buffer.clear();
        }
        m_buffer.append(newData);
    }

    processCompleteFrames();
}

void SuoweiSerialPort::processCompleteFrames()
{
    QWriteLocker locker(&m_bufferLock);

    int dropped = 0;
    while (m_buffer.size() >= PROTOCOL_LENGTH && !frameLooksAligned(m_buffer)) {
        m_buffer.remove(0, 1);
        ++dropped;
    }
    if (dropped > 0) {
        QLOG_WARN() << "串口帧错位，丢弃" << dropped << "字节后重新对齐";
    }

    while (m_buffer.size() >= PROTOCOL_LENGTH) {
        if (!frameLooksAligned(m_buffer)) {
            break;
        }
        QByteArray frame = m_buffer.left(PROTOCOL_LENGTH);
        m_buffer.remove(0, PROTOCOL_LENGTH);

        locker.unlock();

        if (Performanceverification::instance()->returnPerformanceTestFlag()) {
            processFrame(frame);
        } else {
            if (m_threadPool->activeThreadCount() < m_threadPool->maxThreadCount()) {
                QtConcurrent::run(m_threadPool, [this, frame]() {
                    processFrame(frame);
                });
            } else {
                processFrame(frame);
            }
        }

        locker.relock();
    }
}

void SuoweiSerialPort::processFrame(const QByteArray& frame)
{
    if (frame.size() < PROTOCOL_LENGTH) {
        QLOG_WARN() << "Invalid frame size:" << frame.size();
        return;
    }

    quint8 slaveAddr = static_cast<quint8>(frame[0]);

    QString framedata = frame.toHex(' ').toUpper();
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QStringList recvDataList = framedata.split(' ', Qt::SkipEmptyParts);
#else
    QStringList recvDataList = framedata.split(' ', QString::SkipEmptyParts);
#endif

    Parsing_received_messages(recvDataList);

    if (slaveAddr == Z_AXIS) {
        toReadSuckAirsValBack(recvDataList);
    }
}

void SuoweiSerialPort::abortPendingWrites()
{
    clearWriteQueue();
}

void SuoweiSerialPort::clearWriteQueue()
{
    QMutexLocker locker(&m_writeMutex);

    if (!m_writeQueue.isEmpty()) {
        QLOG_INFO() << "清空写入队列，丢弃" << m_writeQueue.size() << "个待发送数据包";
        m_writeQueue.clear();
    }

    if (m_writeTimer && m_writeTimer->isActive()) {
        m_writeTimer->stop();
    }
}

void SuoweiSerialPort::toReadSuckAirsValBack(const QStringList& recv_data)
{
    quint8 commandIndex = GlobalData::analyzeCommandIndex(recv_data);
    int commandMode, run_status, speedMode, Action_motor;
    GlobalData::Parse4thBytecommandstatus(recv_data, commandMode, run_status, speedMode, Action_motor);

    const bool isHandsControl = (Action_motor == MOTOR_HANDS_INDEX);
    int airvalue = GlobalData::backHandsSuckAirValue(recv_data);
    if (isHandsControl && commandIndex == HANDS_SUCK_NUM) {
        emit backHandssuck(true, airvalue);
    } else if (isHandsControl && commandIndex == HANDS_SPLIT_NUM) {
        emit backHandssuck(false, airvalue);
    }
}

bool SuoweiSerialPort::InstancingSerialandOpen(const bool initconnect, bool notifyFailure)
{
    QMutexLocker locker(&m_serialMutex);
    clearReceiveBuffer();
    cleanupSerialPort();

    try {
        const QStringList ports = instrumentPortNames();
        if (ports.isEmpty()) {
            QLOG_WARN() << "未找到有效的仪器端口号!";
            throw SerialPortException("未找到仪器端口号");
        }

        QString lastError = QString::fromUtf8("串口打开失败");
        for (const QString &name : ports) {
            mserialPort = new QSerialPort(this);
            connect(mserialPort, &QSerialPort::readyRead,
                    this, &SuoweiSerialPort::slotReadData,
                    Qt::DirectConnection);
            connect(mserialPort, &QSerialPort::bytesWritten,
                    this, &SuoweiSerialPort::handleBytesWritten,
                    Qt::DirectConnection);
            m_errorSignalConnected = false;

            cglobal::gserialPortName = name;
            if (openSerialPort(name)) {
                if (!m_errorSignalConnected) {
                    connect(mserialPort, &QSerialPort::errorOccurred,
                            this, &SuoweiSerialPort::handleSerialError,
                            Qt::QueuedConnection);
                    m_errorSignalConnected = true;
                }
                QLOG_INFO() << "串口打开成功: " << cglobal::gserialPortName;
                if (!initconnect) {
                    emit prepareReconnectGetData();
                    if (cglobal::g_StartTesting) {
                        prepareReconnectTesting();
                    }
                } else {
                    emit openequipmentCleanActive();
                    emit connectionStateChanged(true);
                }
                return true;
            }

            lastError = getSerialPortErrorString(mserialPort);
            QLOG_WARN() << "串口打开失败: " << name << lastError;
            cleanupSerialPort();
        }
        throw SerialPortException(lastError.toStdString());
    } catch (const SerialPortException& e) {
        if (notifyFailure) {
            handleInitializationFailure(e.what());
        } else {
            QLOG_WARN() << "串口打开未成功:" << e.what();
            cleanupSerialPort();
        }
        return false;
    }
}

int SuoweiSerialPort::Recve_Motor_motion_state(const QStringList recv_data, const int Index)
{
    if (recv_data.size() < 5) {
       QLOG_WARN() << "Recve_Motor_motion_state: 数据长度不足，期望至少5项，实际" << recv_data.size();
       return 0;
   }

   bool convertOk;
   int stateValue = recv_data.at(4).toUInt(&convertOk, 16);
   if (!convertOk) {
       QLOG_WARN() << "Recve_Motor_motion_state: 状态码格式无效" << recv_data.at(4);
       return 0;
   }

   switch (Index) {
       case 0:
           // 原始：取二进制字符串的最后 2 个字符（倒数第3位起的2个字符），即位 b2 b1
           // 等价于 ((stateValue & 0b00000110) >> 1)
           stateValue = (stateValue >> 1) & 0x03;   // 实际上 (stateValue & 0x06) >> 1
           break;
       case 1:
           // 原始：取二进制字符串的第 4、5 个字符（从高位算起），即位 b4 b3
           // 等价于 ((stateValue & 0b00110000) >> 3)
           stateValue = (stateValue >> 3) & 0x03;
           break;
       case 2:
           // 原始：取二进制字符串的前 3 个字符，即位 b7 b6 b5
           // 等价于 (stateValue >> 5) & 0x07
           stateValue = (stateValue >> 5) & 0x07;
           break;
       default:
           // 其他 Index 保持不变（原始函数未修改值）
           break;
   }
   return stateValue;


//    if (recv_data.size() < 5) {
//        QLOG_WARN() << "Recve_Motor_motion_state: 数据长度不足，期望至少5项，实际" << recv_data.size();
//        return 0;
//    }

//    bool convertOk;
//    int stateValue = recv_data.at(4).toUInt(&convertOk, 16);
//    if (!convertOk) {
//        QLOG_WARN() << "Recve_Motor_motion_state: 状态码格式无效" << recv_data.at(4);
//        return 0;
//    }

//    const int Binary = 2;
//    QString hexMessage = QString("%1").arg(Motor_motion_state, 0, 10);
//    QString Statestr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));
//    int leng = Statestr.length();

//    switch (Index) {
//        case 0:
//            if (leng >= 3) {
//                Motor_motion_state = Statestr.mid(leng - 3, Binary).toUInt(nullptr, Binary);
//            }
//            break;
//        case 1:
//            if (leng >= 5) {
//                Motor_motion_state = Statestr.mid(leng - 5, Binary).toUInt(nullptr, Binary);
//            }
//            break;
//        case 2:
//            if (leng >= 3) {
//                Motor_motion_state = Statestr.mid(0, 3).toUInt(nullptr, Binary);
//            }
//            break;
//        default:
//            break;
//    }
   // return Motor_motion_state;
}

int SuoweiSerialPort::ParseSportLocation(const QStringList recv_data, const int Index)
{
    if (recv_data.size() < 6) {
        QLOG_WARN() << "ParseSportLocation: 数据长度不足，期望至少6项，实际" << recv_data.size();
        return 0;
    }

    int Sport_Location = 0;
    const int Binary = 2;
    bool convertOk;
    quint8 area_nums = recv_data.at(5).toUInt(&convertOk, 16);
    if (!convertOk) {
        QLOG_WARN() << "ParseSportLocation: 区域编号格式无效" << recv_data.at(5);
        return 0;
    }

    QString hexMessage = QString("%1").arg(area_nums, 0, 10);
    QString areanumstr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));
    int leng = areanumstr.length();

    switch (Index) {
        case 0:
            if (leng >= 6) {
                Sport_Location = areanumstr.mid(4, Binary).toUInt(nullptr, Binary);
            }
            break;
        case 1:
            if (leng >= 8) {
                Sport_Location = areanumstr.mid(6, Binary).toInt(nullptr, Binary);
            }
            break;
        case 2:
            if (leng >= 1) {
                Sport_Location = areanumstr.mid(0, 1).toInt(nullptr, Binary);
            }
            break;
        case 3:
            if (leng >= 1) {
                Sport_Location = areanumstr.mid(0, 1).toInt(nullptr, Binary);
            }
            break;
        default:
            break;
    }
    return Sport_Location;
}

void SuoweiSerialPort::prepareReconnectTesting()
{
    // 测试中的复位只交给 TestProjectProcess 跟踪。这里再发一组 Z 复位没有应答名单，
    // 会和后面的全轴复位抢指令，名单对不齐时任务就停在重连里。
    QLOG_ERROR() << "测试中断重连，交给测试流程全轴复位后续跑";
    emit connectEquipmentagin();
}

void SuoweiSerialPort::cleanupSerialPort()
{
    // 调用方必须已持有 m_serialMutex。这里不能再锁：
    // InstancingSerialandOpen / handleInitializationFailure 已加过同一把非递归锁。
    if (mserialPort) {
        disconnect(mserialPort, nullptr, this, nullptr);
        if (mserialPort->isOpen()) {
            mserialPort->close();
        }
        // 与 slotcloseseirport 一致：同步释放，避免 deleteLater 与后续裸 delete 竞态
        delete mserialPort;
        mserialPort = nullptr;
    }
    m_errorSignalConnected = false;
    m_portOpen = false;
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
    cleanupSerialPort();
    emit outRminderWidget("连接错误", error);
}

bool SuoweiSerialPort::isPortValid() const
{
    return mserialPort != nullptr && m_portOpen;
}

void SuoweiSerialPort::Parsing_received_messages(const QStringList data_list)
{
    if (data_list.size() < 4) {
        QLOG_ERROR() << "Parsing_received_messages: 数据长度不足，期望至少4项，实际" << data_list.size();
        return;
    }

    auto safeHexToInt = [](const QString& str, bool& ok) {
        return str.toInt(&ok, HEX_SWITCH);
    };

    bool ok = false;
    int slave_addr = safeHexToInt(data_list[0], ok);
    if (!ok) {
        QLOG_ERROR() << "从机地址解析失败，非法的十六进制值:"
                    << data_list[0] << "完整数据:" << data_list;
        return;
    }

    int func_code = safeHexToInt(data_list[1], ok);
    if (!ok) {
        QLOG_ERROR() << "功能码解析失败，非法的十六进制值:"
                    << data_list[1] << "完整数据:" << data_list;
        return;
    }

    int buffer_num = safeHexToInt(data_list[3], ok);
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

    if (_bufferfull(buffer_num, slave_addr) == false)
        return;

    if (processModuleInfo(slave_addr, data_list) == true)
        return;

    if (processMachineData(slave_addr, func_code, data_list) == true)
        return;

    if (processMainBoardInfo(slave_addr, func_code, data_list) == true)
        return;
}

bool SuoweiSerialPort::_bufferfull(const int buffer_num, const int _addr)
{
    if (buffer_num >= 255 && !mreminderbufferfull) {
        mreminderbufferfull = true;
        emit BufferFullState(_addr);
        return false;
    }
    return true;
}

bool SuoweiSerialPort::processModuleInfo(int moduleAddress, const QStringList& receivedData)
{
    static constexpr std::pair<int, int> VALID_MODULE_RANGE{MODULE_1, MODULE_3};

    const bool isValidAddress = (moduleAddress >= VALID_MODULE_RANGE.first)
                                 && (moduleAddress <= VALID_MODULE_RANGE.second);

    if (!isValidAddress) {
        return false;
    }

    emit moduleprotocolData(moduleAddress, receivedData);
    return true;
}

bool SuoweiSerialPort::_machinesportinfo(const int _addr, const int func_code, const QStringList _recvdata)
{
    Q_UNUSED(_addr);
    Q_UNUSED(func_code);
    Q_UNUSED(_recvdata);
    return false;
}

bool SuoweiSerialPort::processMachineData(int deviceAddress, int functionCode,
                                           const QStringList& receivedData)
{
    const bool isTargetDevice = (deviceAddress == Z_AXIS) || (deviceAddress == MAIN_CONTROL);
    if (!isTargetDevice || functionCode != MOTOR_FUNCTIONCODE) {
        return false;
    }

    const bool isTesting = cglobal::g_StartTesting;
    const bool isHitRun = cglobal::g_hitTesting.load();
    const bool isPerfTest = Performanceverification::instance()->returnPerformanceTestFlag();
    const bool isQcRun = cglobal::g_quality_start.load();

    if (isTesting) {
        emit sendTestingRecvData(receivedData);
    } else if (isHitRun) {
        emit serialHitData(receivedData);
    } else if (isPerfTest) {
        emit serialPEdata(receivedData);
    } else if (isQcRun) {
        emit serialQcData(receivedData);
    } else {
        emit serialrecvedata(receivedData);
    }
    return true;
}

bool SuoweiSerialPort::processMainBoardInfo(int deviceAddress,
                                            int functionCode,
                                            const QStringList& receivedData)
{
    constexpr int STATUS_DATA_INDEX = 2;
    constexpr int MIN_DATA_SIZE = 3;

    if (deviceAddress != MAIN_CONTROL) {
        return false;
    }

    switch (functionCode) {
        case MAINBOARD_FUNCTIONCODE: {
            if (receivedData.size() < MIN_DATA_SIZE) {
                QLOG_ERROR() << "主控状态数据长度不足，预期至少"
                              << MIN_DATA_SIZE << "项，实际收到"
                              << receivedData.size();
                return false;
            }

            bool conversionOk = false;
            int stateCode = receivedData[STATUS_DATA_INDEX].toInt(&conversionOk, HEX_SWITCH);
            if (!conversionOk) {
                QLOG_ERROR() << "主控状态码转换失败：" << receivedData[STATUS_DATA_INDEX];
                return false;
            }

            if (stateCode == EVERYTIME_READ_MAINSTATE) {
                emit mainControlBoardProtocolData(receivedData);
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
