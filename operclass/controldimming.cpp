#pragma execution_character_set("utf-8")
#include "controldimming.h"  //控温调光

#include "QsLog/include/QsLog.h"
#include "cglobal.h"
#include <mainwindow.h>
#include <operclass/fullyautomatedplatelets.h>
#include <QMessagebox>
#include <QThread>
#include <QTimer>
#include <ini_file.h>
#include <structinstance.h>
#include <warn_interface.h>
#include <qmainwindow.h>
#include <globaldata.h>

#define DIMMINGDIFFERENCEABSOLUTEVALUE 100 //调光差绝对值

controldimming::controldimming(QObject *parent) : QObject(parent)
  ,m_startCollectChannelVal(false),m_totalchn(0),m_frequency(0)
{
    m_testChnDimmingStu.clear();
}

controldimming::~controldimming()
{
    qDeleteAll(m_testChnDimmingStu);
    m_testChnDimmingStu.clear();
    QLOG_DEBUG()<<"退出调光实例";
}


void controldimming::getdimming_status(bool &finished)
{
    finished = cglobal::g_controldimmingfinished;
}



void controldimming::sycnequipment(quint8 index)
{
    static const QHash<quint8, int> channelMap = {
        {KS600, 4},
        {KS800, 8},
        {KS1200, 12}
    };
    m_totalchn = channelMap.value(index, m_totalchn);
}





//温度达标的完成模组号
void controldimming::recvmoduletemp(int module)
{
    // 预定义模块配置参数
    const int channels_per_module = 4;
    const int start_channel = TEST_CHANNEL_1 + ((module - 1) * channels_per_module);

    // 使用C++11智能指针管理内存
    typedef std::unique_ptr<ModuleChannelData> ModuleDataPtr;

    // 预分配内存提升性能
    m_testChnDimmingStu.reserve(m_testChnDimmingStu.size() + channels_per_module);

    for (int i = 0; i < channels_per_module; ++i) {
        ModuleDataPtr pModuleData(new ModuleChannelData);
        pModuleData->ControlTemp = true;
        pModuleData->ModuleNum = module;
        pModuleData->ModuleDimmingVal = INI_File().getModuledimmingVal(module);
        pModuleData->indexChannel = start_channel + i;
        pModuleData->channdelData = 0;
        pModuleData->DimmingTimes = 0;
        pModuleData->bneedcontrastive = false;
        pModuleData->Dimmingsuccessful = false;
		m_testChnDimmingStu.push_back(pModuleData.release());
    }

   // 进度条件判断
   if (m_testChnDimmingStu.size() >= m_totalchn) {  // 使用>=更安全
       m_startCollectChannelVal = true;
       FullyAutomatedPlatelets::mainWindow()->_dimmingprogress(true);
   }
   return;
}


void controldimming::Collectchannelvalues(QVector<ModuleChannelData *> &ChannelVet,
                                            quint8 indexChannel,int indexChannelData)
{
    for (ModuleChannelData *pmoduleState : ChannelVet) {
        if (pmoduleState->ControlTemp && pmoduleState->indexChannel == indexChannel && !pmoduleState->bneedcontrastive) {
            pmoduleState->channdelData = indexChannelData;
            pmoduleState->bneedcontrastive = true;
            break;
        }
    }
}


void controldimming::handleFailedChannels(const QSet<quint8>& failedChannels)
{
    dimmingModule.clear();
    QList<quint8> OnceDimmingChn; //已经跳过光值的通道
    OnceDimmingChn.clear();

    QVector<quint8> EndDimmingList;
    EndDimmingList.clear();

    for(auto *pModule : m_testChnDimmingStu){
		if (!failedChannels.contains(pModule->indexChannel)) continue;

		if (m_frequency == 0) {
			pModule->DimmingTimes = 1;
		}
		else if (m_frequency == 1) {
			pModule->DimmingTimes = 2;
			if(!OnceDimmingChn.contains(pModule->indexChannel))
				OnceDimmingChn.append(pModule->indexChannel);
		}
		else if (m_frequency == 2) {
			pModule->DimmingTimes = 3;
			if (!EndDimmingList.contains(pModule->indexChannel - 1)) {
				EndDimmingList.push_back(pModule->indexChannel - 1);
			}
		}
		pModule->bneedcontrastive = false;

		if (!dimmingModule.contains(pModule->ModuleNum)) {
			dimmingModule.insert(pModule->ModuleNum, pModule->ModuleDimmingVal);
		}
    }

	switch (m_frequency)
	{
		case 0: sendoutDimmingCode();
			break;
		case 1: emit showDimmingFailedChn(OnceDimmingChn);
			break;
		case 2: Disableitdirectly(EndDimmingList);
			break;
		default: break;
	}
}



void controldimming::ProactiveComparisons(int differenceThreshold)
{
	QSet<quint8> failedChannels;
    failedChannels.clear();
	for (auto *pmoduleinfo : qAsConst(m_testChnDimmingStu)) {
		const int delta = std::abs(pmoduleinfo->channdelData - pmoduleinfo->ModuleDimmingVal);
		pmoduleinfo->Dimmingsuccessful = (delta <= differenceThreshold);

		if (!pmoduleinfo->Dimmingsuccessful) {
			failedChannels.insert(pmoduleinfo->indexChannel);
		}
	}

	if (failedChannels.isEmpty()) {
		FullyAutomatedPlatelets::mainWindow()->_dimmingprogress(false); //调光进度条
		cglobal::g_controldimmingfinished = true;
		emit reminderText(PROMPTLOG, "所有模组调光完成!");
		QLOG_DEBUG() << "所有模组调光完成" << endl;
        return;
	}

    handleFailedChannels(failedChannels);
    return;
}

void controldimming::theEndDimmingActive()
{
    sendoutDimmingCode();
}

void controldimming::Disableitdirectly(QVector<quint8> FailedDimmingChn)
{
    //直接禁用
    QString outstr = "调光失败通道";
    FullyAutomatedPlatelets::pinstanceequipmentconfig()->disablechn(FailedDimmingChn); //同步设置界面通道状态
    for(int n = 0; n < FailedDimmingChn.size(); n++)
    {
       quint8 indexChannel = FailedDimmingChn.at(n);
       outstr+= QString::number(indexChannel);
       QString wkey = QString("TestTheChanne1Opening_%1").arg(indexChannel + 1);
       INI_File().wConfigPara(wkey,false);
       //INI_File().SetChannelState(indexChannel + 1,false);
       StructInstance::getInstance()->config_testChn_State(indexChannel,CHN_STATUS_DISABLE);
       FullyAutomatedPlatelets::pinstanceTesting()->recv_NotifyChannleState(indexChannel + 1, false);
    }
    FullyAutomatedPlatelets::mainWindow()->_dimmingprogress(false); //调光进度条
    cglobal::g_controldimmingfinished = true;
    emit reminderText(PROMPTLOG,"所有模组调光完成!");
    QLOG_DEBUG()<<"所有模组调光完成"<<endl;
    return;
}


void controldimming::sendoutDimmingCode()
{
    QByteArrayList dimmingLedList;
    dimmingLedList.clear();
    auto itmap = dimmingModule.begin();
    while(itmap != dimmingModule.end())
    {
       QByteArray dimmingLed = GlobalData::WriteModuleLedData(itmap.key(),W_MODULE_LED_DIMMING,itmap.value());
       dimmingLedList.append(dimmingLed);//调光
       itmap++;
    }
    emit writemodulLedData(dimmingLedList, "自动调光");
}

//收到第一次主动调光
void controldimming::SecondDimmingModule(quint8 notifyModule, int times_)
{
    if(dimmingModule.contains(notifyModule))
    {
        dimmingModule.remove(notifyModule);
    }
    if(dimmingModule.size() == 0)
    {
        m_startCollectChannelVal = true; //采集通道值
        m_frequency = m_frequency + times_;
    }
    return;
}

//获取到的通道数据
void controldimming::_obtainChndata(const quint8 indexChannel, const int indexChannelData) {
    if (!m_startCollectChannelVal) return;

    Collectchannelvalues(m_testChnDimmingStu, indexChannel + 1, indexChannelData);
    bool AllChnContrastState = true;

    // 使用智能指针避免裸指针
    for (auto& pChnVal : m_testChnDimmingStu) { // 范围for循环更安全
        if (pChnVal == nullptr) { // 显式判空
            // 日志或处理：例如跳过或标记错误
            AllChnContrastState = false;
            continue;
        }
        if (!pChnVal->bneedcontrastive) {
            AllChnContrastState = false;
            break;
        }
    }

    if (AllChnContrastState) {
        m_startCollectChannelVal = false;
        ProactiveComparisons(DIMMINGDIFFERENCEABSOLUTEVALUE);//主动对比
    }
}


quint8 controldimming::_chnParentModule(const quint8 index_chn)
{
    quint8 index_module = index_chn/4 + 1;
    return index_module;
}










//QString datastr = QString("模组%1LED光值主动对比完成!").arg(indexModule);
// this->reminderText(PROMPTLOG,datastr);


