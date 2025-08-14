#ifndef CONSUMABLESWRITE_H
#define CONSUMABLESWRITE_H

#include <QObject>
#include <QThread>

struct ConsumablesStu
{
    QByteArray data_; //数据
    quint8 index_num; //命令编号
    quint8 index_reag; //试剂编号
};

struct CardAnalysisResult {
    QString workStatus;    // 工作状态描述
    QString dataType;      // 数据类型描述
    quint8 statusCode;     // 原始状态码(0-15)
    quint8 typeCode;       // 原始类型码(0-15)
};

class ConsumablesWrite : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit ConsumablesWrite(QObject *parent = nullptr);
    ~ConsumablesWrite();

    void Start();

private slots:
    void _pthreadoutstart();

signals:
    void _sendDirectives(const QByteArray _data,QString outtext);

    void _sendDirectivesList(const QByteArrayList &dataList,
                             const QString &info);

    void _ShutdownApp();

    void closeSerial();



public slots:

    void handlcloseEquipmentconsumables(); //关机配置耗材

    void _slotsendcodeList(const QByteArrayList _data, QString outtext);

    void _recvConsumableswriteState(QStringList recvdata_);

    void initdelfile(); //线程清文件防止界面卡顿

private:
    void _recvdatacancelmark(quint8 index_);

    QByteArray _PoweronandoffconfigSupplies(quint8 reagentIndex); //开关机配置耗材命令

    void  _insterSTUtudata(const QByteArray lossCleanarry, quint8 indexcode_, quint8 indexreag);

    void _shutdownWithError();

    //状态解析函数
    CardAnalysisResult analyzeCardData(quint8 byteData);

    // 专用充值状态判断函数
    bool isRechargeSuccessful(const CardAnalysisResult& result);

private:
    QList<ConsumablesStu* > mdataarry_;
    bool m_boot;
};

#endif // CONSUMABLESWRITE_H
