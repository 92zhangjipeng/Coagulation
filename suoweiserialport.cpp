#pragma execution_character_set("utf-8")

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
        QMutexLocker locker(&m_writeMutex);
        m_writeTimer->setInterval(5);
        m_writeTimer->setSingleShot(true);
        startSerialthread();
    });

    connect(&m_thread, &QThread::finished, []{ QLOG_DEBUG() << "Thread exited"; });
}

SuoweiSerialPort::~SuoweiSerialPort()
{
    m_threadPool->waitForDone();
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
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }

    QMutexLocker locker(&m_serialMutex);
    if (mserialPort) {
        if (mserialPort->isOpen()) {
            mserialPort->clear();
            mserialPort->close();
        }

        delete mserialPort;
        mserialPort = nullptr;
    }

    if (m_writeTimer) {
        m_writeTimer->stop();
        delete m_writeTimer;
        m_writeTimer = nullptr;
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
        case QSerialPort::ResourceError:
            m_portOpen = false;
            break;
        default:
            QLOG_DEBUG() << "串口次要错误:" << errorMsg;
            break;
    }
}

void SuoweiSerialPort::prepareReconnect()
{
    recvdisConnectCloseSerial();

    QEventLoop loop;
    QTimer::singleShot(150, &loop, &QEventLoop::quit);
    loop.exec();

    InstancingSerialandOpen(false);
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
        clearWriteQueue();
        emit connectionStateChanged(false);
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
    bool convertOk;
    quint8 value = data[index].toUInt(&convertOk, 16);
    if (convertOk) {
        consumables->updateReagentLimit(WRITE_OPERAT, reagentType, value);
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

void SuoweiSerialPort::handleRecvdata()
{
    if (!mserialPort || !m_portOpen) return;

    const qint64 bytesAvailable = mserialPort->bytesAvailable();
    if (bytesAvailable <= 0) return;

    const qint64 readSize = qMin<qint64>(bytesAvailable, 1024);
    QByteArray newData = mserialPort->read(readSize);

    {
        QWriteLocker locker(&m_bufferLock);
        if (m_buffer.size() + newData.size() > MAX_BUFFER_SIZE) {
            int overflow = m_buffer.size() + newData.size() - MAX_BUFFER_SIZE;
            m_buffer.remove(0, overflow);
            QLOG_WARN() << "Buffer overflow! Removed" << overflow << "bytes";
        }
        m_buffer.append(newData);
    }

    processCompleteFrames();
}

void SuoweiSerialPort::processCompleteFrames()
{
    QWriteLocker locker(&m_bufferLock);

    while (m_buffer.size() >= PROTOCOL_LENGTH) {
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

void SuoweiSerialPort::InstancingSerialandOpen(const bool initconnect)
{
    QMutexLocker locker(&m_serialMutex);
    cleanupSerialPort();

    mserialPort = new QSerialPort(this);

    connect(mserialPort, &QSerialPort::readyRead,
            this, &SuoweiSerialPort::slotReadData,
            Qt::DirectConnection);

    connect(mserialPort, &QSerialPort::bytesWritten,
            this, &SuoweiSerialPort::handleBytesWritten,
            Qt::DirectConnection);

    try {
        if (!findAndValidateSerialPort()) {
            QLOG_WARN() << "未找到有效的仪器端口号!";
            throw SerialPortException("未找到仪器端口号");
        }

        if (!openSerialPort(cglobal::gserialPortName)) {
            QString errorMsg = getSerialPortErrorString(mserialPort);
            QLOG_WARN() << "串口打开失败: " << errorMsg;
            throw SerialPortException(errorMsg.toStdString());
        }

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
    } catch (const SerialPortException& e) {
        handleInitializationFailure(e.what());
    }
}

int SuoweiSerialPort::Recve_Motor_motion_state(const QStringList recv_data, const int Index)
{
    if (recv_data.size() < 5) {
        QLOG_WARN() << "Recve_Motor_motion_state: 数据长度不足，期望至少5项，实际" << recv_data.size();
        return 0;
    }

    bool convertOk;
    int Motor_motion_state = recv_data.at(4).toUInt(&convertOk, 16);
    if (!convertOk) {
        QLOG_WARN() << "Recve_Motor_motion_state: 状态码格式无效" << recv_data.at(4);
        return 0;
    }

    const int Binary = 2;
    QString hexMessage = QString("%1").arg(Motor_motion_state, 0, 10);
    QString Statestr = QString("%1").arg(QString::number(hexMessage.toInt(), Binary), 8, QChar('0'));
    int leng = Statestr.length();

    switch (Index) {
        case 0:
            if (leng >= 3) {
                Motor_motion_state = Statestr.mid(leng - 3, Binary).toUInt(nullptr, Binary);
            }
            break;
        case 1:
            if (leng >= 5) {
                Motor_motion_state = Statestr.mid(leng - 5, Binary).toUInt(nullptr, Binary);
            }
            break;
        case 2:
            if (leng >= 3) {
                Motor_motion_state = Statestr.mid(0, 3).toUInt(nullptr, Binary);
            }
            break;
        default:
            break;
    }
    return Motor_motion_state;
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
    QByteArrayList zbackorig;
    QUIUtils::allZAxisBackOrigin(zbackorig, 100);
    slotwrite_instructions_group(zbackorig, "重连先复位所有Z轴");
    QLOG_ERROR() << "测试中断重连,复位所有Z轴!" << endl;
    emit connectEquipmentagin();
}

void SuoweiSerialPort::cleanupSerialPort()
{
    if (mserialPort) {
        disconnect(mserialPort, nullptr, this, nullptr);
        if (mserialPort->isOpen()) {
            mserialPort->close();
        }
        mserialPort->deleteLater();
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
    const bool isPerfTest = Performanceverification::instance()->returnPerformanceTestFlag();

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
