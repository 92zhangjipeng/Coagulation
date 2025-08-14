#ifndef CONTROLDIMMING_H
#define CONTROLDIMMING_H

#include <QMap>
#include <QObject>
#include <QVector>
#include <QMetaType>
#include <QVariant>
#include <QSet>


#define   INIT_STATE                0  //模组初始状态
#define   CONTROLTEMPFINISH         1  //模组调温已到
#define   TOCOMPARED                2  //光亮对比完成

#define   FINISHEDDIMMING           4  //调光完成

#define   INITCHANNELDATA           -1 //通道初值

#define   CHN_ABS_END              100 //通道已调光对比
#define   CHN_DIMMINNED            200 //通道调光完成

struct ModuleChannelData
{
    bool ControlTemp;  //模组控温状态
    bool bneedcontrastive; //需要对比
    bool Dimmingsuccessful; //对比调光状态
    quint8 DimmingTimes; //调光次数
    quint8 ModuleNum;  //模组编号
    quint8 indexChannel;  //通道号
    int channdelData;    //通道值
    int ModuleDimmingVal; //模组调光值
};





class controldimming : public QObject
{
    Q_OBJECT
public:
    explicit controldimming(QObject *parent = 0);
    ~controldimming();

    void _obtainChndata(const quint8 indexChannel, const int indexChannelData);

    void recvmoduletemp(int module);  //接收模组温度

	void sycnequipment(quint8 index);

    void getdimming_status(bool &finished); //同步获取调光状态

    quint8  _chnParentModule(const quint8 index_chn); //通道属于哪个模组

    void SecondDimmingModule(quint8 notifyModule, int times_);

    void theEndDimmingActive();

private:


    /**  直接禁用通道
     * @brief Disableitdirectly
     */
    void    Disableitdirectly(QVector<quint8> FailedDimmingChn);


    /** 所有通道数据采集完成主动开始对比
     * @brief ProactiveComparisons
     * @param differenceThreshold 差值
     */
    void ProactiveComparisons(int differenceThreshold);
    void handleFailedChannels(const QSet<quint8>& failedChannels);

    /** 收集通道值
     * @brief Collectchannelvalues
     * @param ChannelVet
     */
    void    Collectchannelvalues(QVector<ModuleChannelData *> &ChannelVet,quint8 indexChannel,int indexChannelData);

    /**  发送调光命令
     * @brief sendoutDimmingCode
     */
    void    sendoutDimmingCode();





signals:

    void    reminderText(quint8 index,const QString outtext);

    void    writemodulLedData(const QByteArrayList &dataList,const QString &info);

    /**  第一次调光失败弹出提示
     * @brief showDimmingFailedChn
     */
    void    showDimmingFailedChn(const QList<quint8>& failedChn);

private:
    QVector<ModuleChannelData *> m_testChnDimmingStu;


	

    QMap<quint8,int> dimmingModule; //失败要调光的模组
    bool m_startCollectChannelVal;
    int m_frequency;  //调光频率
    quint8 m_totalchn;


};

#endif // CONTROLDIMMING_H
