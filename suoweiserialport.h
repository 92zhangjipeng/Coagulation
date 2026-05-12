#ifndef SUOWEISERIALPORT_H
#define SUOWEISERIALPORT_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QByteArray>
#include <QDebug>
#include <QThread>
#include <QMutex>
#include <QQueue>
#include <QFuture>
#include <QReadWriteLock>
#include <QTimer>
#include <QMetaObject>
#include <atomic>

#include "testing.h"
#include "quiutils.h"
#include "thetestmoduleprotocol.h"
#include "mainControlBoardProtocol.h"
#include "loadequipmentpos.h"

namespace ProtocolIndex {
    constexpr int CMD_NUM = 2;
    constexpr int REAGENT_LIMIT_START = 5;
    constexpr int REAGENT_LIMIT_COUNT = 5;
    constexpr int TUBE_LIMIT = 10;
    constexpr int CLEAN_LIMIT = 11;
    constexpr int CLEAN_TOTAL_LOW = 12;
    constexpr int CLEAN_TOTAL_HIGH = 13;
    constexpr int REAGENT_TOTAL_START = 5;
}

class SuoweiSerialPort : public QObject
{
    Q_OBJECT

public:
    explicit SuoweiSerialPort(QObject *parent = nullptr);
    ~SuoweiSerialPort();

    void suoweiportthreadbegin();
    void closePort();

public slots:
    void slotcloseseirport();
    void slotReadData();
    void handleSerialError(QSerialPort::SerialPortError error);
    void prepareReconnect();
    void recvdisConnectCloseSerial();
    void slotwrite_instructions_group(const QByteArrayList &dataList, const QString &info);
    void write_data(const QByteArrayList);
    void writedataToEquipment(const QByteArray arry, QString kindCommad);
    void processWriteQueue();
    void Recv_TrayMoveTest(const QByteArrayList, const bool, const quint8);

signals:
    void openequipmentCleanActive();
    void writeRequested(const QByteArray &data, const QString &kindCommand);
    void moduleprotocolData(const int Slave_addr, const QStringList moduleData);
    void mainControlBoardProtocolData(const QStringList moduleData);
    void ConsumableswriteState(QStringList moduleData);
    void outRminderWidget(QString, QString);
    void connectionStateChanged(bool);
    void prepareReconnectGetData();
    void serialrecvedata(const QStringList);
    void serialPEdata(const QStringList&);
    void Execute_exception_prompt(const quint8 Index, const QString, const quint8);
    void backHandssuck(bool, int);
    void outArmText(quint8 kinds, const QString detailedStr);
    void continueSendArryTray();
    void Disconnectandreconnect();
    void sendTestingRecvData(const QStringList);
    void connectEquipmentagin();
    void BufferFullState(int _addr);

private slots:
    void startSerialthread();
    void TestMoveTrayTube_recv(const QStringList backArry);
    void handleBytesWritten(qint64 bytes);

private:
    bool findAndValidateSerialPort();
    bool openSerialPort(const QString portname);
    void applySerialPortConfig(const QString &portname);
    void handleOpenError(const QString &errorMsg);
    void updateGlobalConnectionStatus(bool status);
    void safeClosePort();

    void StoredParameters(const QStringList& recvdata);
    void updateReagentLimit(const QStringList& data, int index, int reagentType, ConsumablesOper* consumables);
    quint16 parseHexPair(const QStringList& data, int highIndex, int lowIndex, bool& convertOk);
    void handleCmd3(const QStringList& recvdata, ConsumablesOper* consumables);
    void handleCmd5(const QStringList& recvdata, ConsumablesOper* consumables);

    void handleRecvdata();
    void processCompleteFrames();
    void processFrame(const QByteArray& frame);
    void clearWriteQueue();

    void toReadSuckAirsValBack(const QStringList &recv_data);
    void InstancingSerialandOpen(const bool initconnect);
    int Recve_Motor_motion_state(const QStringList, const int);
    int ParseSportLocation(const QStringList recv_data, const int Index);
    void prepareReconnectTesting();
    void cleanupSerialPort();
    QString getSerialPortErrorString(QSerialPort* port);
    void handleInitializationFailure(const QString& error);
    bool isPortValid() const;
    void Parsing_received_messages(const QStringList);
    bool _bufferfull(const int buffer_num, const int _addr);
    bool processModuleInfo(int moduleAddress, const QStringList &receivedData);
    bool _machinesportinfo(const int _addr, const int func_code, const QStringList _recvdata);
    bool processMachineData(int deviceAddress, int functionCode, const QStringList& receivedData);
    bool processMainBoardInfo(int deviceAddress, int functionCode, const QStringList &receivedData);

private:
    QMutex mutex;
    QThread m_thread;
    QMutex m_serialMutex;
    QReadWriteLock m_bufferLock;
    std::atomic<bool> m_portOpen{ false };
    QSerialPort *mserialPort = nullptr;
    QByteArray m_buffer;
    QThreadPool *m_threadPool;
    const int MAX_BUFFER_SIZE = 15 * 256;
    const quint16 mprotocolLength = 15;
    bool mreminderbufferfull = false;
    bool mTestMoveTrayTube;
    quint8 mTotalCommd;
    quint8 mTestMoveTrayTubeFinished;
    bool m_errorSignalConnected = false;
    qint64 m_lastWriteTime = 0;
    QMutex m_writeMutex;
    QMutex m_lastWriteTimeMutex;
    QQueue<QPair<QByteArray, QString>> m_writeQueue;
    QTimer *m_writeTimer;
    QMetaObject::Connection m_bytesWrittenConnection;
};

#endif // SUOWEISERIALPORT_H
