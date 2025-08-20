#pragma execution_character_set("utf-8")

#include "globaldata.h"
#include "thetestmoduleprotocol.h"
#include <algorithm>  // For std::all_of
#include <unordered_map>
#include "mainwindow.h"
#include <operclass/fullyautomatedplatelets.h>

theTestModuleProtocol::theTestModuleProtocol(QObject *parent) : QObject(parent)
  ,minstrumentType(0)
  ,m_completeTemp(false)
{
    moveToThread(&m_thread);
    QObject::connect(&m_thread,&QThread::started,this,&theTestModuleProtocol::_threadrunning);
}

theTestModuleProtocol::~theTestModuleProtocol()
{
    if(m_thread.isRunning()){
        m_thread.quit();
        m_thread.wait();
    }
    QLOG_DEBUG()<<"析构模组线程";
}

void theTestModuleProtocol::_start()
{
    if(!m_thread.isRunning())
    {
        m_thread.start();
    }
}

void  theTestModuleProtocol::_threadrunning()
{
   QLOG_DEBUG()<<"模组数据采集线程ID="<<QThread::currentThreadId()<<"行号:"<<__LINE__<<endl;
}

void theTestModuleProtocol::SynchronizeInstrumentType(quint8 equipment)
{
    minstrumentType = equipment;
    m_moduleCommTemp.clear();
    m_remainingModules = 0;  // 新增计数器，跟踪未完成模块数

    switch(minstrumentType)
    {
        case KS600:
            m_moduleCommTemp.insert(MODULE_1,false);
            m_remainingModules = 1;
        break;
        case KS800:
            m_moduleCommTemp.insert(MODULE_1,false);
            m_moduleCommTemp.insert(MODULE_2,false);
            m_remainingModules = 2;
        break;
        case KS1200:
            m_moduleCommTemp.insert(MODULE_1,false);
            m_moduleCommTemp.insert(MODULE_2,false);
            m_moduleCommTemp.insert(MODULE_3,false);
            m_remainingModules = 3;
        break;
        default:break;
    }
    m_completeTemp = (m_remainingModules == 0);  // 无模块时直接标记完成
    return;
}

void theTestModuleProtocol::setArgInterface(PassparameterInterface* pInterface)
{
     m_parameterInterface = pInterface;
}



//收到模组数据
void theTestModuleProtocol::recrModuleprotocolData(const int Slave_addr, const QStringList moduleData)
{
    quint8 cmd_num = QString(moduleData.at(COMMANDNUMBER)).toInt(nullptr,HEX_SWITCH); //命令编号

    int *moduledata = new int[4]();
    moduledata[0] = QString("%1%2").arg( moduleData.at(8)).arg( moduleData.at(7)).toInt(nullptr,HEX_SWITCH);
    moduledata[1] = QString("%1%2").arg( moduleData.at(10)).arg( moduleData.at(9)).toInt(nullptr,HEX_SWITCH);
    moduledata[2] = QString("%1%2").arg( moduleData.at(12)).arg( moduleData.at(11)).toInt(nullptr,HEX_SWITCH);
    moduledata[3] = QString("%1%2").arg( moduleData.at(14)).arg( moduleData.at(13)).toInt(nullptr,HEX_SWITCH);

    //4TH模组状态
    quint8 indexModule4thBit = QString(moduleData.at(4)).toInt(nullptr,HEX_SWITCH);
    QString hex_data = QString("%1").arg(indexModule4thBit, 8, BINARY_SWITCH, QLatin1Char('0'));
    //模组的连接状态
    QString Connectstate_ = QString("%1%2").arg(hex_data.at(5)).arg(hex_data.at(6));
    quint8 iConnectstate_ = Connectstate_.toInt(nullptr,BINARY_SWITCH);
    _modulecaseconnectstate(Slave_addr,iConnectstate_,moduleData,hex_data);

    //模组的编号
    quint8 _indexModule = Slave_addr;

    switch(cmd_num)
    {
        case MODULE_1:
        case MODULE_2:
        case MODULE_3:
                    if(m_parameterInterface != nullptr)
                        m_parameterInterface->ChannelValueshow(moduleData);
                    Test_module_data(Slave_addr, moduleData);
        break;

        case W_MODULE_SPEED:
            recvWritedDimmingSpeed(Slave_addr);
        break;

        case R_MODULE_SPEED:
            recvReadDimmingSpeed(Slave_addr,moduledata,4);
        break;

        case W_MODULE_LED:
                recvWritedDimmingLed(Slave_addr);
        break;

        case R_MODULE_LED:
            recvReadDimmingLed(Slave_addr,moduledata,4);
        break;

        case W_SAVEMODULESETTING:
        {
            QTimer::singleShot(DELAY_READ_TUBE_INITVALUE, this, [this, Slave_addr]() {
                emit sendReminder(QString("保存模组%1完成!").arg(Slave_addr),W_SAVEMODULESETTING);
            });
            break;
        }
        case W_MODULE_LED_DIMMING:
                recvDimmingLed(_indexModule);
        break;

        case W_SAVEMODULESETTING_DIMMING:
        {
            QTimer::singleShot(DELAY_READ_TUBE_INITVALUE, this, [this, _indexModule]() {
                emit resetconnectModule();//重新连接模组
                emit FirstDimmingResult(_indexModule); //首次调光后再检测模组
                QLOG_DEBUG()<<"初始主动调光保存动作完成!"<<endl;
            });
            break;
        }

        default: break;
    }
    delete []moduledata;
    return;
}


void theTestModuleProtocol::recvDimmingLed(quint8 indexmodule)
{
    QTimer::singleShot(DELAY_READ_TUBE_INITVALUE *10, this, [this,indexmodule]() {
        emit FirstDimmingResult(indexmodule); //调光模组首次模组调光
    });
}





void theTestModuleProtocol::recvReadDimmingSpeed(quint8 ModuleIndex,int arr[], int size)
{
    int moduleChndata[4] ={0,0,0,0};
    for(int i = 0; i < size ; ++i)
    {
        moduleChndata[i] = arr[i];
    }
    emit readModuleSpeed(ModuleIndex,moduleChndata[0],moduleChndata[1],moduleChndata[2],moduleChndata[3]);
}


void theTestModuleProtocol::recvWritedDimmingSpeed(quint8 ModuleIndex)
{
    QString outtext = QString("模组%1转速成功!").arg(ModuleIndex);
    emit sendReminder(outtext,W_MODULE_SPEED);
}


void theTestModuleProtocol::recvReadDimmingLed(quint8 ModuleIndex, int arr[], int size)
{
    int moduleChndata[4] ={0,0,0,0};
    for(int i = 0; i < size ; ++i)
    {
        moduleChndata[i] = arr[i];
    }
    emit readModuleLed(ModuleIndex,moduleChndata[0],moduleChndata[1],moduleChndata[2],moduleChndata[3]);
}

void theTestModuleProtocol::recvWritedDimmingLed(quint8 ModuleIndex)
{
    QString outtext = QString("模组%1LED成功!").arg(ModuleIndex);

    emit sendReminder(outtext,W_MODULE_LED);

    //emit this->resetconnectModule();//重新连接模组
}



void theTestModuleProtocol::_modulecaseconnectstate(int slaveaddr,quint8 bit_, QStringList ordrerr,QString disbit_)
{
    if(bit_ == MODULE_FAILED)
    {
        switch(slaveaddr)
        {
            case  MODULE_1:
                cglobal::g_moduleIdisconnected = true;
            break;
            case  MODULE_2:
                cglobal::g_moduleIIdisconnected = true;
            break;
            case  MODULE_3:
                cglobal::g_moduleIIIdisconnected = true;

            break;
        default: break;
        }
		QString data_ = ordrerr.join(" ");
        QString title_ = QString("模组%1接收异常指令").arg(slaveaddr);
        QString data_out = QString("模组%1[命令错误从机掉线] 收到数据:%2 异常字节:%3").arg(slaveaddr).arg(data_).arg(disbit_);
        emit this->errmodule(title_,data_out);
    }
    else
    {
        switch(slaveaddr)
        {
            case  MODULE_1:
                cglobal::g_moduleIdisconnected =  false;
            break;
            case  MODULE_2:
                cglobal::g_moduleIIdisconnected =  false;
            break;
            case  MODULE_3:
                cglobal::g_moduleIIIdisconnected = false;
            break;
        default: break;
        }
    }
    return;
}


void theTestModuleProtocol::Test_module_data(const int slave_address,const QStringList Machinedata)
{
    QString recv_data;
    quint8 Index_Data_Hbit = 6;
    quint8 Index_Data_Lowbit = 5;
    double Moduletemperature = 0.00f;
    auto &ini = INI_File();

    double ModulTemp_1 = ini.rConfigPara(MODULETEMPA).toDouble() - 37.00;
    double ModulTemp_2 = ini.rConfigPara(MODULETEMPB).toDouble() - 37.00;
    double ModulTemp_3 = ini.rConfigPara(MODULETEMPC).toDouble() - 37.00;
    quint32  temp_= 0;
    switch(slave_address)
    {
        case MODULE_1:
            recv_data = Machinedata.at(Index_Data_Hbit) + Machinedata.at(Index_Data_Lowbit);
            temp_ =  recv_data.toUInt(nullptr, HEX_SWITCH);
            Moduletemperature = temp_*TEMP_CONVERSION_RATIO - ModulTemp_1 ;
        break;
        case MODULE_2:
            recv_data = Machinedata.at(Index_Data_Hbit) + Machinedata.at(Index_Data_Lowbit);
            temp_ =  recv_data.toUInt(nullptr, HEX_SWITCH);
            Moduletemperature = temp_* TEMP_CONVERSION_RATIO - ModulTemp_2 ;
        break;
        case MODULE_3:
            recv_data = Machinedata.at(Index_Data_Hbit) + Machinedata.at(Index_Data_Lowbit);
            temp_ =  recv_data.toUInt(nullptr, HEX_SWITCH);
            Moduletemperature = temp_* TEMP_CONVERSION_RATIO - ModulTemp_3 ;
        break;
        default:
            break;
    }
    if(m_parameterInterface != nullptr)
    {
        m_parameterInterface->displayPara(slave_address,Moduletemperature);
    }


    getTemperatureDimming(Moduletemperature,slave_address);



    return ;
}

void theTestModuleProtocol::getTemperatureDimming(const double moduleTEMP, const quint8 indexModule)
{
    if (m_completeTemp)
            return;  // 提前返回，避免冗余操作

    if (moduleTEMP >= mbasictemp)
    {
        if (m_moduleCommTemp.contains(indexModule))
        {
            auto it = m_moduleCommTemp.find(indexModule);
            if (!it.value())  // 仅当模块未完成时更新
            {
                it.value() = true;  // 标记为完成
                m_remainingModules--;  // 减少未完成计数
                QLOG_DEBUG()<<"模组"<<indexModule<<"控温到达"<<moduleTEMP;
                if (m_remainingModules <= 0)
                {
                    m_completeTemp = true;  // 所有模块完成
                }
                FullyAutomatedPlatelets::pinstancedimming()->recvmoduletemp(indexModule);
            }
        }
    }




}


