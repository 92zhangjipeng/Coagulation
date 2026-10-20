#ifndef CONTROLDIMMING_H
#define CONTROLDIMMING_H

#include <QMap>
#include <QObject>
#include <QVector>
#include <QMetaType>
#include <QVariant>
#include <QSet>

#define INIT_STATE          0
#define CONTROLTEMPFINISH   1
#define TOCOMPARED          2
#define FINISHEDDIMMING     4
#define INITCHANNELDATA     -1
#define CHN_ABS_END         100
#define CHN_DIMMINNED       200

struct ModuleChannelData
{
    bool ControlTemp = false;
    bool bneedcontrastive = false;
    bool Dimmingsuccessful = false;
    quint8 DimmingTimes = 0;
    quint8 ModuleNum = 0;
    quint8 indexChannel = 0;   // 1-based
    int channdelData = 0;      // 通道 ADC，空通道应对齐 3300，满量程 4095
    int ModuleDimmingVal = 0;  // 空通道 ADC 对齐目标（默认 3300）
    int ledDriveVal = 0;       // 仅记录，初始化检测不对 LED 做闭环
};

class controldimming : public QObject
{
    Q_OBJECT
public:
    explicit controldimming(QObject *parent = 0);
    ~controldimming();

    void _obtainChndata(const quint8 indexChannel, const int indexChannelData);
    void recvmoduletemp(int module);
    void sycnequipment(quint8 index);
    void getdimming_status(bool &finished);
    quint8 _chnParentModule(const quint8 index_chn);
    void SecondDimmingModule(quint8 notifyModule, int times_);
    void theEndDimmingActive();

private:
    void Disableitdirectly(QVector<quint8> FailedDimmingChn);
    void ProactiveComparisons(int differenceThreshold);
    void handleFailedChannels(const QSet<quint8>& failedChannels);
    void Collectchannelvalues(QVector<ModuleChannelData *> &ChannelVet,
                              quint8 indexChannel, int indexChannelData);
    void sendoutDimmingCode();
    void markDimmingFinished(const QString &reason);
    bool moduleAlreadyRegistered(int module) const;
    void startEmptyChannelRecheck();

signals:
    void reminderText(quint8 index, const QString outtext);
    void writemodulLedData(const QByteArrayList &dataList, const QString &info);
    void showDimmingFailedChn(const QList<quint8>& failedChn);
    void requestDimmingProgress(bool inProgress);
    void requestSmartCupClear(const QList<quint8>& failedChn1Based);

private:
    QVector<ModuleChannelData *> m_testChnDimmingStu;
    QMap<quint8, int> dimmingModule; // 兼容旧LED下发路径，初始化检测不再写入
    bool m_startCollectChannelVal;
    bool m_discardNextCompleteFrame;
    int m_frequency;
    int m_smartCupClearAttempts;
    quint8 m_totalchn;
};

#endif // CONTROLDIMMING_H
