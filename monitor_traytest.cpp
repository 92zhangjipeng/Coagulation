#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "monitor_traytest.h"
#include "cglobal.h"
#include "ini_file.h"
#include "loadequipmentpos.h"
#include "quiutils.h"
#include "globaldata.h"
#include <QBitArray>

#define DIMMINGLEDTIME  9

QMutex Monitor_TrayTest::mutex_mainbord;

Monitor_TrayTest::Monitor_TrayTest(QObject *parent) : QObject(parent)
{
    moveToThread(&m_thread);
    init_modul_struct();
    connect(&m_thread, &QThread::started, this, &Monitor_TrayTest::_begingrun);
}

Monitor_TrayTest::~Monitor_TrayTest()
{
    if (m_thread.isRunning()) {
        m_thread.quit();
        m_thread.wait();
    }
    free(m_config_modul_1_data);
    free(m_config_modul_2_data);
    free(m_config_modul_3_data);
    QLOG_DEBUG() << "析构模组&&调光线程";
}

void Monitor_TrayTest::_start()
{
    if (!m_thread.isRunning()) {
        m_thread.start();
    }
}

void Monitor_TrayTest::_begingrun()
{
    QLOG_DEBUG() << "模组&&调光线程id=" << QThread::currentThreadId();
}

// 初始化三个模组数据结构，分配内存并在失败时正确回滚
void Monitor_TrayTest::init_modul_struct()
{
    // 先全部置空
    m_config_modul_1_data = nullptr;
    m_config_modul_2_data = nullptr;
    m_config_modul_3_data = nullptr;


    auto allocModule = [this](quint8 idx, control_modul_stu *&ptr) -> bool {
        ptr = (control_modul_stu*)malloc(sizeof(control_modul_stu));
        if (!ptr) {
            QLOG_DEBUG() << "malloc error modul data for module" << idx;
            return false;
        }
        settingmoduledata(idx, ptr);
        return true;
    };

    bool ok1 = allocModule(MODULE_1, m_config_modul_1_data);
    bool ok2 = allocModule(MODULE_2, m_config_modul_2_data);
    bool ok3 = allocModule(MODULE_3, m_config_modul_3_data);

    // 若任一分配失败，释放已成功分配的内存，避免泄漏
    if (!ok1 || !ok2 || !ok3) {
        QLOG_DEBUG() << "Module data allocation incomplete, cleaning up";
        free(m_config_modul_1_data); m_config_modul_1_data = nullptr;
        free(m_config_modul_2_data); m_config_modul_2_data = nullptr;
        free(m_config_modul_3_data); m_config_modul_3_data = nullptr;
    }
}

void Monitor_TrayTest::settingmoduledata(quint8 index_, control_modul_stu *modulestudata)
{
    modulestudata->index_modul = index_;
    modulestudata->func_code = MOTOR_FUNCTIONCODE; // 0x17
    modulestudata->num_cmd = index_;
    modulestudata->size_cmd = 0;

    modulestudata->fucn_setting.type_ = 0;      // 默认写模式
    modulestudata->fucn_setting.cmd_status = 0;
    modulestudata->fucn_setting.cmd_fucn = 0;

    // 从配置文件读取温度
    auto &ini = INI_File();
    double moduleTemp = 0.0;
    switch (index_) {
    case MODULE_1: moduleTemp = ini.rConfigPara(MODULETEMPA).toDouble(); break;
    case MODULE_2: moduleTemp = ini.rConfigPara(MODULETEMPB).toDouble(); break;
    case MODULE_3: moduleTemp = ini.rConfigPara(MODULETEMPC).toDouble(); break;
    default: break;
    }
    modulestudata->config_temp = static_cast<quint16>(moduleTemp / TEMP_CONVERSION_RATIO);

    // 所有通道初始为停止状态
    modulestudata->modul_chn_1_data = STOP_CHN_MOTOR;
    modulestudata->modul_chn_2_data = STOP_CHN_MOTOR;
    modulestudata->modul_chn_3_data = STOP_CHN_MOTOR;
    modulestudata->modul_chn_4_data = STOP_CHN_MOTOR;
}

// ==================== 核心：统一的协议打包函数（完全复现原有低效转换） ====================
void Monitor_TrayTest::buildModuleCommand(control_modul_stu* data, QByteArray& moduleCommand)
{
    if (!data) {
        moduleCommand.clear();
        return;
    }

    moduleCommand.clear();
    // 以下代码与原 modul_x_sycn_directives 100% 一致，仅将 m_config_modul_X_data 替换为 data
    moduleCommand = QByteArray::fromHex(QString::number(data->index_modul, 16).toUtf8());
    moduleCommand.push_back(QByteArray::fromHex(QString::number(data->func_code, 16).toUtf8()));
    moduleCommand.push_back(QByteArray::fromHex(QString::number(data->num_cmd, 16).toUtf8()));
    moduleCommand.push_back(QByteArray::fromHex(QString::number(data->size_cmd, 16).toUtf8()));

    QBitArray fucn_oper(8);
    fucn_oper.fill(0);
    QString typed_ = QString("%1").arg(data->fucn_setting.type_, 1, 2, QLatin1Char('0'));
    fucn_oper.setBit(7, typed_.toInt());
    typed_ = QString("%1").arg(data->fucn_setting.cmd_status, 2, 2, QLatin1Char('0'));
    fucn_oper.setBit(6, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(5, QString(typed_.at(1)).toInt());
    typed_ = QString("%1").arg(data->fucn_setting.cmd_fucn, 5, 2, QLatin1Char('0'));
    fucn_oper.setBit(0, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(1, QString(typed_.at(1)).toInt());
    fucn_oper.setBit(2, QString(typed_.at(2)).toInt());
    fucn_oper.setBit(3, QString(typed_.at(3)).toInt());
    fucn_oper.setBit(4, QString(typed_.at(4)).toInt());

    QString sSendHex = QUIUtils::bitArray2String(fucn_oper);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    moduleCommand.push_back(byte_);

    moduleCommand.push_back(QUIUtils::qint16ToQByteArray(data->config_temp, false));
    moduleCommand.push_back(QUIUtils::qint16ToQByteArray(data->modul_chn_1_data, false));
    moduleCommand.push_back(QUIUtils::qint16ToQByteArray(data->modul_chn_2_data, false));
    moduleCommand.push_back(QUIUtils::qint16ToQByteArray(data->modul_chn_3_data, false));
    moduleCommand.push_back(QUIUtils::qint16ToQByteArray(data->modul_chn_4_data, false));
}

// ==================== 统一的通道状态设置（消除重复代码） ====================
void Monitor_TrayTest::setChannelState(quint8 moduleIdx, quint8 channelIdx, bool run)
{
    control_modul_stu* target = nullptr;
    switch (moduleIdx) {
        case MODULE_1: target = m_config_modul_1_data; break;
        case MODULE_2: target = m_config_modul_2_data; break;
        case MODULE_3: target = m_config_modul_3_data; break;
        default: return;
    }
    if (!target) return;

    target->fucn_setting.type_ = WRITE_MODUL;
    quint16 value = run ? RUN_CHN_MOTOR : STOP_CHN_MOTOR;
    switch (channelIdx) {
        case 0: target->modul_chn_1_data = value; break;
        case 1: target->modul_chn_2_data = value; break;
        case 2: target->modul_chn_3_data = value; break;
        case 3: target->modul_chn_4_data = value; break;
        default: break;
    }
}


// 以下三个函数保持原始实现（未改动，仅内部依赖）
void Monitor_TrayTest::config_modul_index_chn_run(quint8 indexModul, quint8 indexChn)
{
    setChannelState(indexModul, indexChn, true);



}

void Monitor_TrayTest::config_modul_index_chn_stop(quint8 indexModul, quint8 indexChn)
{
   setChannelState(indexModul, indexChn, false);
}

// 构建模组命令（原始逻辑完全保留）
void Monitor_TrayTest::modul_1_sycn_directives(QByteArray &moduleCommand)
{
    buildModuleCommand(m_config_modul_1_data, moduleCommand);
}

void Monitor_TrayTest::modul_2_sycn_directives(QByteArray &moduleCommand)
{
    buildModuleCommand(m_config_modul_2_data, moduleCommand);
}

void Monitor_TrayTest::modul_3_sycn_directives(QByteArray &moduleCommand)
{
    buildModuleCommand(m_config_modul_3_data, moduleCommand);
}

void Monitor_TrayTest::sycnmoduledata(quint8 index_, QByteArray &_onemoduledata)
{
    // 修复数据竞争：加锁保护
    QMutexLocker locker(&mutex_mainbord);

    // 空指针保护：若任一模组数据未成功分配，则直接返回空命令
    if (!m_config_modul_1_data || !m_config_modul_2_data || !m_config_modul_3_data) {
        _onemoduledata.clear();
        QLOG_DEBUG() << "Module data not fully initialized, returning empty command";
        return;
    }

    switch (index_) {
    case MODULE_1: modul_1_sycn_directives(_onemoduledata); break;
    case MODULE_2: modul_2_sycn_directives(_onemoduledata); break;
    case MODULE_3: modul_3_sycn_directives(_onemoduledata); break;
    default: break;
    }
}

void Monitor_TrayTest::controlChnMotorRotating(quint8 controlIndexChn, bool brotating)
{
    // 输入验证：通道号必须在 0~11 范围内
    if (controlIndexChn >= 12) {
        QLOG_WARN() << "Invalid channel index:" << controlIndexChn << ", ignored";
        return;
    }

    QMutexLocker locker(&mutex_mainbord);

    // 空指针保护
    if (!m_config_modul_1_data || !m_config_modul_2_data || !m_config_modul_3_data) {
        QLOG_ERROR() << "Module data not initialized, cannot control motor";
        return;
    }

    quint8 indexModule = controlIndexChn / 4 + 1;
    quint8 indexModule_Channel = controlIndexChn % 4;

    if (brotating) {
        config_modul_index_chn_run(indexModule, indexModule_Channel);
        QLOG_DEBUG() << "启动模组" << indexModule << "的第" << indexModule_Channel + 1 << "通道转动";
    } else {
        config_modul_index_chn_stop(indexModule, indexModule_Channel);
        QLOG_DEBUG() << "停止模组" << indexModule << "的第" << indexModule_Channel + 1 << "通道转动";
    }
}

void Monitor_TrayTest::open_orclose_chn(bool bopened)
{
    QMutexLocker locker(&mutex_mainbord);

    if (!m_config_modul_1_data || !m_config_modul_2_data || !m_config_modul_3_data) {
        QLOG_ERROR() << "Module data not initialized, cannot open/close channels";
        return;
    }

    quint16 state = bopened ? RUN_CHN_MOTOR : STOP_CHN_MOTOR;

    m_config_modul_1_data->fucn_setting.cmd_fucn = 0;
    m_config_modul_1_data->fucn_setting.cmd_status = 0;
    m_config_modul_1_data->fucn_setting.type_ = 0;
    m_config_modul_1_data->modul_chn_1_data = state;
    m_config_modul_1_data->modul_chn_2_data = state;
    m_config_modul_1_data->modul_chn_3_data = state;
    m_config_modul_1_data->modul_chn_4_data = state;

    m_config_modul_2_data->fucn_setting.cmd_fucn = 0;
    m_config_modul_2_data->fucn_setting.cmd_status = 0;
    m_config_modul_2_data->fucn_setting.type_ = 0;
    m_config_modul_2_data->modul_chn_1_data = state;
    m_config_modul_2_data->modul_chn_2_data = state;
    m_config_modul_2_data->modul_chn_3_data = state;
    m_config_modul_2_data->modul_chn_4_data = state;

    m_config_modul_3_data->fucn_setting.cmd_fucn = 0;
    m_config_modul_3_data->fucn_setting.cmd_status = 0;
    m_config_modul_3_data->fucn_setting.type_ = 0;
    m_config_modul_3_data->modul_chn_1_data = state;
    m_config_modul_3_data->modul_chn_2_data = state;
    m_config_modul_3_data->modul_chn_3_data = state;
    m_config_modul_3_data->modul_chn_4_data = state;
}

void Monitor_TrayTest::configModulTEMPvalue(quint8 indexmodul, double modultemp)
{
    QMutexLocker locker(&mutex_mainbord);

    if (!m_config_modul_1_data || !m_config_modul_2_data || !m_config_modul_3_data) {
        QLOG_ERROR() << "Module data not initialized, cannot set temperature";
        return;
    }

    quint16 tempValue = static_cast<quint16>(qBound(0, qRound(modultemp / TEMP_CONVERSION_RATIO), 65535));
    switch (indexmodul) {
    case MODULE_1: m_config_modul_1_data->config_temp = tempValue; break;
    case MODULE_2: m_config_modul_2_data->config_temp = tempValue; break;
    case MODULE_3: m_config_modul_3_data->config_temp = tempValue; break;
    default: break;
    }
    QLOG_INFO() << "模组" << indexmodul << "目标温度写入轮询缓存"
                << modultemp << "raw" << tempValue;
}
