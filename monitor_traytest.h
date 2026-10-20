#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#ifndef MONITOR_TRAYTEST_H
#define MONITOR_TRAYTEST_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QByteArray>

#define STOP_CHN_MOTOR   1028
#define RUN_CHN_MOTOR    0
#define READ_MODUL       1
#define WRITE_MODUL      0

typedef struct CONTROL_FUCN
{
    quint8 type_;
    quint8 cmd_status;
    quint8 cmd_fucn;
} control_fucn;

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
} control_modul_stu;

class Monitor_TrayTest : public QObject
{
    Q_OBJECT

public:
    explicit Monitor_TrayTest(QObject *parent = nullptr);
    ~Monitor_TrayTest();

    void _start();

    static QMutex mutex_mainbord;

signals:
    void writemodulData(const QByteArray arrdata, QString remindererr);


public slots:
    void controlChnMotorRotating(quint8 controlIndexChn, bool brotating);
    void open_orclose_chn(bool bopened);
    void configModulTEMPvalue(quint8 indexmodul, double modultemp);

public:
    void sycnmoduledata(quint8 index_, QByteArray &_onemoduledata);

private slots:
    void _begingrun();

private:
    void init_modul_struct();
    void settingmoduledata(quint8 index_, control_modul_stu *modulestudata);

    void config_modul_index_chn_run(quint8 indexModul, quint8 indexChn);
    void config_modul_index_chn_stop(quint8 indexModul, quint8 indexChn);

    void modul_1_sycn_directives(QByteArray &moduleCommand);
    void modul_2_sycn_directives(QByteArray &moduleCommand);
    void modul_3_sycn_directives(QByteArray &moduleCommand);

    // 辅助函数：构建模组命令（不改变原有转换逻辑）
    void buildModuleCommand(control_modul_stu* data, QByteArray& moduleCommand);
    // 辅助函数：设置指定模组、通道的电机状态
    void setChannelState(quint8 moduleIdx, quint8 channelIdx, bool run);

private:
    QThread m_thread;
    control_modul_stu *m_config_modul_1_data = nullptr;
    control_modul_stu *m_config_modul_2_data = nullptr;
    control_modul_stu *m_config_modul_3_data = nullptr;
};

#endif // MONITOR_TRAYTEST_H
