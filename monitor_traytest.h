#ifndef MONITOR_TRAYTEST_H
#define MONITOR_TRAYTEST_H

#include <QObject>
#include <QTimer>
#include <QThread>

#define STOP_CHN_MOTOR   1028  //通道电机停止转动
#define RUN_CHN_MOTOR    0    //通道电机转动


#define READ_MODUL       1     //读模组
#define WRITE_MODUL      0     //写模组


typedef struct CONTROL_FUCN
{
   quint8 type_;       //命令类型
   quint8 cmd_status;  //命令状态
   quint8 cmd_fucn;    //命令功能
}control_fucn;

typedef struct TESTMODULCONTROL
{
    quint8 index_modul;
    quint8 func_code;
    quint8 num_cmd;
    quint8 size_cmd;
    control_fucn fucn_setting;
    quint16 config_temp;
    quint16 modul_chn_1_data;
    quint16 modul_chn_2_data;
    quint16 modul_chn_3_data;
    quint16 modul_chn_4_data;
}control_modul_stu;

//用来定时访问模组线程
class Monitor_TrayTest : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit Monitor_TrayTest(QObject *parent = nullptr);
    ~Monitor_TrayTest();

    void _start();

    static QMutex mutex_mainbord;

signals:

    void writemodulData(const QByteArray arrdata, QString remindererr); //发送到串口

    void sendreadModuleCommd(const QByteArrayList &dataList,
                             const QString &info); //发送到线程


private slots:
    void _begingrun();

public slots:
    /**
     * @brief Monitor_TrayTest::controlChnMotorRotating 控制模组通道旋转开关
     * @param Chn   0 -11 的通道号
     * @param brotating  true 启动旋转  false 停止
     */
    void controlChnMotorRotating(quint8 controlIndexChn, const bool brotating);

    /**
     * @brief open_orclose_chn 控制所有通道开关
     * @param bopened
     */
    void open_orclose_chn(bool bopened);

    void configModulTEMPvalue(quint8 indexmodul, double modultemp); //设置模组温度




public:

    void _sycnmoduledata(quint8 index_,QByteArray &_onemoduledata);

private:
    void init_modul_struct();//初始化模组读取命令

    void settingmoduledata(quint8 index_, control_modul_stu *&modulestudata);

    void config_modul_index_chn_run(const quint8 indexModul,const quint8 indexChn); //设置哪个模组/通道转动

    void config_modul_index_chn_stop(const quint8 indexModul,const quint8 indexChn); //停止

    void modul_1_sycn_directives(QByteArray &moduleCommand);

    void modul_2_sycn_directives(QByteArray &moduleCommand);

    void modul_3_sycn_directives(QByteArray &moduleCommand);

private:
    //QVector<ChnMotorRevolve_ *> m_ChnMotorRotating;  //通道转动状态结构体
    control_modul_stu *m_config_modul_1_data = nullptr;  //模组协议数据
    control_modul_stu *m_config_modul_2_data = nullptr;
    control_modul_stu *m_config_modul_3_data = nullptr;


};

#endif // MONITOR_TRAYTEST_H
