#ifndef THETESTMODULEPROTOCOL_H
#define THETESTMODULEPROTOCOL_H

#include <QMap>
#include <QObject>
#include <QThread>
#include "passparameterinterface.h"

class theTestModuleProtocol : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit theTestModuleProtocol(QObject *parent = nullptr);
    ~theTestModuleProtocol();

    void _start();

    void  SynchronizeInstrumentType(quint8 equipment); //同步仪器类型

    void  setArgInterface(PassparameterInterface* pInterface);

private slots:
    void _threadrunning();

signals:
    void errmodule(QString,QString); //模组报错

    void resetconnectModule(); //重新连接模组

    void pauseConnectModule(); //保存模组配置时暂停通讯

    void sendReminder(QString ,const int);

    void readModuleSpeed(quint8,int,int,int,int);

    void readModuleLed(quint8,int,int,int,int);

    void _writeDataToSerial(const QByteArray,QString);

    void FirstDimmingResult(quint8 moduleIndex);



public slots:
	
    void recrModuleprotocolData(const int Slave_addr,const QStringList moduleData);
private:
    /** 解析收到的模组数据 温度&&数据
     * @brief Test_module_data
     * @param slave_address
     * @param Machinedata
     */
    void Test_module_data(const int slave_address,const QStringList Machinedata);

    void _modulecaseconnectstate(int slaveaddr, quint8 bit_, QStringList ordrerr, QString disbit_); //模组连接状态

    void recvReadDimmingSpeed(quint8 ModuleIndex, int arr[], int size);

    void recvWritedDimmingSpeed(quint8 ModuleIndex);

    void recvWritedDimmingLed(quint8 ModuleIndex);

    void recvReadDimmingLed(quint8 ModuleIndex, int arr[], int size);

    void recvDimmingLed(quint8 indexmodule); //收到调光发送保存

    void getTemperatureDimming(const double moduleTEMP,const quint8 indexModule); //初始调光需要控温

private:
    quint8 minstrumentType;

    QMap<quint8,bool> m_moduleCommTemp;
    int m_remainingModules = 0;  // 新增计数器，跟踪未完成模块数

    const float mbasictemp = 37.00f;
    bool m_completeTemp;

    PassparameterInterface *m_parameterInterface = nullptr;
};

#endif // THETESTMODULEPROTOCOL_H
