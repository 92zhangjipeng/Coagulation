#pragma execution_character_set("utf-8")

#include "monitor_traytest.h"
#include "cglobal.h"
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
    QObject::connect(&m_thread,&QThread::started,this,&Monitor_TrayTest::_begingrun);
}

Monitor_TrayTest::~Monitor_TrayTest()
{
    if(m_thread.isRunning()){
        m_thread.quit();
        m_thread.wait();
    }
	free(m_config_modul_1_data);
	free(m_config_modul_2_data);
	free(m_config_modul_3_data);


    QLOG_DEBUG() <<"析构模组&&调光线程"<< endl;
}

void Monitor_TrayTest::_start()
{
    if(!m_thread.isRunning())
    {
        m_thread.start();
    }
}

void Monitor_TrayTest::_begingrun()
{
    QLOG_DEBUG()<<"模组&&调光线程id="<<QThread::currentThreadId();
}

//初始化模组读取命令
void Monitor_TrayTest::init_modul_struct()
{
    m_config_modul_1_data = (control_modul_stu*)malloc(sizeof(control_modul_stu));
    if(m_config_modul_1_data == NULL){
        QLOG_DEBUG()<<("malloc error modul data");
        return;
    }
    settingmoduledata(MODULE_1,m_config_modul_1_data);

    m_config_modul_2_data = (control_modul_stu*)malloc(sizeof(control_modul_stu));
    if(m_config_modul_2_data == NULL)
    {
        QLOG_DEBUG()<<("malloc error modul data");
        return;
    }
    settingmoduledata(MODULE_2,m_config_modul_2_data);


    m_config_modul_3_data = (control_modul_stu*)malloc(sizeof(control_modul_stu));
    if(m_config_modul_3_data == NULL)
    {
        QLOG_DEBUG()<<("malloc error modul data");
        return;
    }
    settingmoduledata(MODULE_3,m_config_modul_3_data);
    return;
}

void Monitor_TrayTest::controlChnMotorRotating(quint8 controlIndexChn,const bool brotating)
{
   mutex_mainbord.lock();
   quint8 indexModule = controlIndexChn / 4 + 1;
   quint8 indexModule_Channel = controlIndexChn%4;
   if(brotating == true)
   {
       config_modul_index_chn_run(indexModule,indexModule_Channel);
       QLOG_DEBUG()<<"启动模组"<<indexModule<<"的第"<<indexModule_Channel+1<<"通道转动"<<endl;
   }
   else if(brotating == false)
   {
       config_modul_index_chn_stop(indexModule,indexModule_Channel);
       QLOG_DEBUG()<<"停止模组"<<indexModule<<"的第"<<indexModule_Channel+1<<"通道转动"<<endl;
   }
   mutex_mainbord.unlock();
}

void Monitor_TrayTest::open_orclose_chn(bool bopened)
{
    mutex_mainbord.lock();
    m_config_modul_1_data->fucn_setting.cmd_fucn = 0;
    m_config_modul_1_data->fucn_setting.cmd_status = 0;
    m_config_modul_1_data->fucn_setting.type_ = 0;

    m_config_modul_2_data->fucn_setting.cmd_fucn = 0;
    m_config_modul_2_data->fucn_setting.cmd_status = 0;
    m_config_modul_2_data->fucn_setting.type_ = 0;

    m_config_modul_3_data->fucn_setting.cmd_fucn = 0;
    m_config_modul_3_data->fucn_setting.cmd_status = 0;
    m_config_modul_3_data->fucn_setting.type_ = 0;

    if(bopened == true)
    {
        m_config_modul_1_data->modul_chn_1_data = RUN_CHN_MOTOR ;
        m_config_modul_1_data->modul_chn_2_data = RUN_CHN_MOTOR ;
        m_config_modul_1_data->modul_chn_3_data = RUN_CHN_MOTOR;
        m_config_modul_1_data->modul_chn_4_data = RUN_CHN_MOTOR;

        m_config_modul_2_data->modul_chn_1_data = RUN_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_2_data = RUN_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_3_data = RUN_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_4_data = RUN_CHN_MOTOR;

        m_config_modul_3_data->modul_chn_1_data = RUN_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_2_data = RUN_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_3_data = RUN_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_4_data = RUN_CHN_MOTOR;
    }
    else
    {
        m_config_modul_1_data->modul_chn_1_data = STOP_CHN_MOTOR;
        m_config_modul_1_data->modul_chn_2_data = STOP_CHN_MOTOR;
        m_config_modul_1_data->modul_chn_3_data = STOP_CHN_MOTOR;
        m_config_modul_1_data->modul_chn_4_data = STOP_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_1_data = STOP_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_2_data = STOP_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_3_data = STOP_CHN_MOTOR;
        m_config_modul_2_data->modul_chn_4_data = STOP_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_1_data = STOP_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_2_data = STOP_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_3_data = STOP_CHN_MOTOR;
        m_config_modul_3_data->modul_chn_4_data = STOP_CHN_MOTOR;
    }
    mutex_mainbord.unlock();
}

//设置模组温度 设置界面设置好的数值
void Monitor_TrayTest::configModulTEMPvalue(quint8 indexmodul, double modultemp)
{
    mutex_mainbord.lock();
    switch (indexmodul)
    {
        case MODULE_1:  m_config_modul_1_data->config_temp = modultemp/TEMP_CONVERSION_RATIO;   break;
        case MODULE_2:  m_config_modul_2_data->config_temp = modultemp/TEMP_CONVERSION_RATIO;   break;
        case MODULE_3:  m_config_modul_3_data->config_temp = modultemp/TEMP_CONVERSION_RATIO;   break;
        default: break;
    }
    mutex_mainbord.unlock();
    QLOG_DEBUG()<<"设置温度成功!"<<__FUNCTION__;
    return;
}

void Monitor_TrayTest::_sycnmoduledata(quint8 index_,QByteArray &_onemoduledata)
{
    switch(index_)
    {
        case MODULE_1:   modul_1_sycn_directives(_onemoduledata);   break;
        case MODULE_2:   modul_2_sycn_directives(_onemoduledata);   break;
        case MODULE_3:   modul_3_sycn_directives(_onemoduledata);   break;
        default:break;
    }
}




void Monitor_TrayTest::settingmoduledata(quint8 index_,control_modul_stu *&modulestudata)
{
    modulestudata->index_modul = index_;
    modulestudata->func_code = MOTOR_FUNCTIONCODE; //0x17
    modulestudata->num_cmd = index_;
    modulestudata->size_cmd = 0;

    control_fucn fucn_;
    fucn_.cmd_fucn = 0;
    fucn_.cmd_status = 0;
    fucn_.type_ = READ_MODUL; //0 写 1 :读
    modulestudata->fucn_setting = fucn_;


    auto &ini = INI_File();
    double moduleTemp = 0.00;
    switch(index_){
    case MODULE_1: moduleTemp = ini.rConfigPara(MODULETEMPA).toDouble();break;
    case MODULE_2: moduleTemp = ini.rConfigPara(MODULETEMPB).toDouble();break;
    case MODULE_3: moduleTemp = ini.rConfigPara(MODULETEMPC).toDouble();break;
    default: break;
    }
    modulestudata->config_temp = moduleTemp/TEMP_CONVERSION_RATIO;
    modulestudata->modul_chn_1_data = STOP_CHN_MOTOR;
    modulestudata->modul_chn_2_data = STOP_CHN_MOTOR;
    modulestudata->modul_chn_3_data = STOP_CHN_MOTOR;
    modulestudata->modul_chn_4_data = STOP_CHN_MOTOR;
    return;
}





void Monitor_TrayTest::config_modul_index_chn_run(const quint8 indexModul,const quint8 indexChn)
{
    switch(indexModul)
    {
        case MODULE_1:
            m_config_modul_1_data->fucn_setting.type_ = WRITE_MODUL;
            switch(indexChn)
            {
                case 0: m_config_modul_1_data->modul_chn_1_data = RUN_CHN_MOTOR; break;
                case 1: m_config_modul_1_data->modul_chn_2_data = RUN_CHN_MOTOR; break;
                case 2: m_config_modul_1_data->modul_chn_3_data = RUN_CHN_MOTOR; break;
                case 3: m_config_modul_1_data->modul_chn_4_data = RUN_CHN_MOTOR; break;
            default: break;
            }
        break;
        case MODULE_2:
            m_config_modul_2_data->fucn_setting.type_ = WRITE_MODUL;
            switch(indexChn)
            {
                case 0: m_config_modul_2_data->modul_chn_1_data = RUN_CHN_MOTOR; break;
                case 1: m_config_modul_2_data->modul_chn_2_data = RUN_CHN_MOTOR; break;
                case 2: m_config_modul_2_data->modul_chn_3_data = RUN_CHN_MOTOR; break;
                case 3: m_config_modul_2_data->modul_chn_4_data = RUN_CHN_MOTOR; break;
            default: break;
            }
        break;
        case MODULE_3:
            m_config_modul_3_data->fucn_setting.type_ = WRITE_MODUL;
            switch(indexChn)
            {
                case 0: m_config_modul_3_data->modul_chn_1_data = RUN_CHN_MOTOR; break;
                case 1: m_config_modul_3_data->modul_chn_2_data = RUN_CHN_MOTOR; break;
                case 2: m_config_modul_3_data->modul_chn_3_data = RUN_CHN_MOTOR; break;
                case 3: m_config_modul_3_data->modul_chn_4_data = RUN_CHN_MOTOR; break;
            default: break;
            }
        break;
        default:break;
    }
}

void Monitor_TrayTest::config_modul_index_chn_stop(const quint8 indexModul,const quint8 indexChn)
{
    switch(indexModul)
    {
        case MODULE_1:
            m_config_modul_1_data->fucn_setting.type_ = WRITE_MODUL;
            switch(indexChn)
            {
                case 0: m_config_modul_1_data->modul_chn_1_data = STOP_CHN_MOTOR; break;
                case 1: m_config_modul_1_data->modul_chn_2_data = STOP_CHN_MOTOR; break;
                case 2: m_config_modul_1_data->modul_chn_3_data = STOP_CHN_MOTOR; break;
                case 3: m_config_modul_1_data->modul_chn_4_data = STOP_CHN_MOTOR; break;
            default: break;
            }
        break;
        case MODULE_2:
            m_config_modul_2_data->fucn_setting.type_ = WRITE_MODUL;
            switch(indexChn)
            {
                case 0: m_config_modul_2_data->modul_chn_1_data = STOP_CHN_MOTOR; break;
                case 1: m_config_modul_2_data->modul_chn_2_data = STOP_CHN_MOTOR; break;
                case 2: m_config_modul_2_data->modul_chn_3_data = STOP_CHN_MOTOR; break;
                case 3: m_config_modul_2_data->modul_chn_4_data = STOP_CHN_MOTOR; break;
            default: break;
            }
        break;
        case MODULE_3:
            m_config_modul_3_data->fucn_setting.type_ = WRITE_MODUL;
            switch(indexChn)
            {
                case 0: m_config_modul_3_data->modul_chn_1_data = STOP_CHN_MOTOR; break;
                case 1: m_config_modul_3_data->modul_chn_2_data = STOP_CHN_MOTOR; break;
                case 2: m_config_modul_3_data->modul_chn_3_data = STOP_CHN_MOTOR; break;
                case 3: m_config_modul_3_data->modul_chn_4_data = STOP_CHN_MOTOR; break;
            default: break;
            }
        break;
        default:break;
    }
}






void Monitor_TrayTest::modul_1_sycn_directives(QByteArray &moduleCommand)
{
    moduleCommand.clear();
    moduleCommand = QByteArray::fromHex(QString::number(m_config_modul_1_data->index_modul,HEX_SWITCH).toUtf8());
    moduleCommand.push_back(QByteArray::fromHex(QString::number( m_config_modul_1_data->func_code,HEX_SWITCH).toUtf8()));//功能码
    moduleCommand.push_back(QByteArray::fromHex(QString::number(m_config_modul_1_data->num_cmd,HEX_SWITCH).toUtf8())); //命令编号
    moduleCommand.push_back(QByteArray::fromHex(QString::number(m_config_modul_1_data->size_cmd,HEX_SWITCH).toUtf8())); //缓存命令


    QBitArray fucn_oper(8);
    fucn_oper.fill(0);
    QString typed_ = QString("%1").arg(m_config_modul_1_data->fucn_setting.type_, 1, 2, QLatin1Char('0'));
    fucn_oper.setBit(7, typed_.toInt());
    typed_ = QString("%1").arg(m_config_modul_1_data->fucn_setting.cmd_status,2, 2, QLatin1Char('0'));
    fucn_oper.setBit(6, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(5, QString(typed_.at(1)).toInt());
    typed_ = QString("%1").arg(m_config_modul_1_data->fucn_setting.cmd_fucn,5, 2, QLatin1Char('0'));
    fucn_oper.setBit(0, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(1, QString(typed_.at(1)).toInt());
    fucn_oper.setBit(2, QString(typed_.at(2)).toInt());
    fucn_oper.setBit(3, QString(typed_.at(3)).toInt());
    fucn_oper.setBit(4, QString(typed_.at(4)).toInt());

    //QLOG_DEBUG()<<"+++++"<<fucn_oper;
    QString sSendHex = QUIUtils::bitArray2String(fucn_oper);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    moduleCommand.push_back(byte_);


    QByteArray EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_1_data->config_temp, false);
    moduleCommand.push_back(EndianArry); //温度

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_1_data->modul_chn_1_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_1_data->modul_chn_2_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_1_data->modul_chn_3_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_1_data->modul_chn_4_data, false);
    moduleCommand.push_back(EndianArry);
    //QLOG_DEBUG()<<"模组1命令:"<< moduleCommand.toHex(' ').trimmed().toUpper();
    return;
}

void Monitor_TrayTest::modul_2_sycn_directives(QByteArray &moduleCommand)
{
    moduleCommand.clear();
    moduleCommand = QByteArray::fromHex(QString::number(m_config_modul_2_data->index_modul,HEX_SWITCH).toUtf8());
    moduleCommand.push_back(QByteArray::fromHex(QString::number( m_config_modul_2_data->func_code,HEX_SWITCH).toUtf8()));//功能码
    moduleCommand.push_back(QByteArray::fromHex(QString::number(m_config_modul_2_data->num_cmd,HEX_SWITCH).toUtf8())); //命令编号
    moduleCommand.push_back(QByteArray::fromHex(QString::number(m_config_modul_2_data->size_cmd,HEX_SWITCH).toUtf8())); //缓存命令
    //新加功能
    QBitArray fucn_oper(8);
    fucn_oper.fill(0);
    QString typed_ = QString("%1").arg(m_config_modul_2_data->fucn_setting.type_, 1, 2, QLatin1Char('0'));
    fucn_oper.setBit(7, typed_.toInt());
    typed_ = QString("%1").arg(m_config_modul_2_data->fucn_setting.cmd_status,2, 2, QLatin1Char('0'));
    fucn_oper.setBit(6, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(5, QString(typed_.at(1)).toInt());
    typed_ = QString("%1").arg(m_config_modul_2_data->fucn_setting.cmd_fucn,5, 2, QLatin1Char('0'));
    fucn_oper.setBit(0, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(1, QString(typed_.at(1)).toInt());
    fucn_oper.setBit(2, QString(typed_.at(2)).toInt());
    fucn_oper.setBit(3, QString(typed_.at(3)).toInt());
    fucn_oper.setBit(4, QString(typed_.at(4)).toInt());
    //QLOG_DEBUG()<<"+++++"<<fucn_oper;

    QString sSendHex = QUIUtils::bitArray2String(fucn_oper);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    moduleCommand.push_back(byte_);


    QByteArray EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_2_data->config_temp, false);
    moduleCommand.push_back(EndianArry); //温度

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_2_data->modul_chn_1_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_2_data->modul_chn_2_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_2_data->modul_chn_3_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_2_data->modul_chn_4_data, false);
    moduleCommand.push_back(EndianArry);
    //QLOG_DEBUG()<<"模组2命令:"<< moduleCommand.toHex(' ').trimmed().toUpper();
    return;
}

void Monitor_TrayTest::modul_3_sycn_directives(QByteArray &moduleCommand)
{
    moduleCommand.clear();
    moduleCommand = QByteArray::fromHex(QString::number(m_config_modul_3_data->index_modul,HEX_SWITCH).toUtf8());
    moduleCommand.push_back(QByteArray::fromHex(QString::number( m_config_modul_3_data->func_code,HEX_SWITCH).toUtf8()));//功能码
    moduleCommand.push_back(QByteArray::fromHex(QString::number(m_config_modul_3_data->num_cmd,HEX_SWITCH).toUtf8())); //命令编号
    moduleCommand.push_back(QByteArray::fromHex(QString::number(m_config_modul_3_data->size_cmd,HEX_SWITCH).toUtf8())); //缓存命令
    //新加功能
    QBitArray fucn_oper(8);
    fucn_oper.fill(0);
    QString typed_ = QString("%1").arg(m_config_modul_3_data->fucn_setting.type_, 1, 2, QLatin1Char('0'));
    fucn_oper.setBit(7, typed_.toInt());
    typed_ = QString("%1").arg(m_config_modul_3_data->fucn_setting.cmd_status,2, 2, QLatin1Char('0'));
    fucn_oper.setBit(6, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(5, QString(typed_.at(1)).toInt());
    typed_ = QString("%1").arg(m_config_modul_3_data->fucn_setting.cmd_fucn,5, 2, QLatin1Char('0'));
    fucn_oper.setBit(0, QString(typed_.at(0)).toInt());
    fucn_oper.setBit(1, QString(typed_.at(1)).toInt());
    fucn_oper.setBit(2, QString(typed_.at(2)).toInt());
    fucn_oper.setBit(3, QString(typed_.at(3)).toInt());
    fucn_oper.setBit(4, QString(typed_.at(4)).toInt());
    //QLOG_DEBUG()<<"+++++"<<fucn_oper;

    QString sSendHex = QUIUtils::bitArray2String(fucn_oper);
    QByteArray byte_ = QByteArray::fromHex(sSendHex.toLatin1());
    moduleCommand.push_back(byte_);

    QByteArray EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_3_data->config_temp, false);
    moduleCommand.push_back(EndianArry); //温度

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_3_data->modul_chn_1_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_3_data->modul_chn_2_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_3_data->modul_chn_3_data, false);
    moduleCommand.push_back(EndianArry);

    EndianArry = QUIUtils::qint16ToQByteArray(m_config_modul_3_data->modul_chn_4_data, false);
    moduleCommand.push_back(EndianArry);

    //QLOG_DEBUG()<<"模组3命令"<< moduleCommand.toHex(' ').trimmed().toUpper();
    return;
}



