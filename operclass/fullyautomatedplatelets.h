#ifndef FULLYAUTOMATEDPLATELETS_H
#define FULLYAUTOMATEDPLATELETS_H


#include <QApplication>
#include <QAbstractItemView>
#include <alarm.h>
#include <qualitycontrol.h>
#include <suoweiserialport.h>
#include <usb_initconnect.h>
#include <displayLogsText.h>
#include <calibrate.h>
#include <machinesetting.h>
#include <inquire_sql_info.h>
#include "operclass/controldimming.h"
#include "configureprojectitem.h"   //配置项目
#include "height_data.h"  //添加测试样本
#include "instrumentalarmprompt.h" //耗材提示
#include "threadAddSample/consumableswrite.h" //写入主板线程

#include "customcreatsql.h"



class QItemSelectionModel;
class ViewSettings;
class MainWindow;

class FullyAutomatedPlatelets : public QApplication
{
    Q_OBJECT
public:
    explicit FullyAutomatedPlatelets(int &argc, char ** argv);
    virtual ~FullyAutomatedPlatelets();

    static FullyAutomatedPlatelets *instance();

    static MainWindow *mainWindow();

    static controldimming *pinstancedimming();

    static Alarm *pinstanceinfowidget();

    static QualityControl *pinstanceinstrument();

    static Testing *pinstanceTesting();

    static ConfigureProjectItem *pinstanceTestproject();//配置项目

    static Height_Data *pinstanceAddsampletest();

    static instrumentAlarmPrompt* pinstancesuppilereminder();

    static ConsumablesWrite* pinstanceWirteBoard();

    static SuoweiSerialPort* pinstanceserialusb();

    static USB_InitConnect* pinstanceSingleactive();

    static displayLogsText* pinstanceLogfile();

    static CustomPlot* pinstanceadjustcoordinates();

    static mainControlBoardProtocol* pinstanceMainboarddata();

    static Calibrate* pinstancepatientdata();

    static Monitor_TrayTest* pinstanceobtainModuledata();

    static MachineSetting* pinstanceequipmentconfig();

	static CustomCreatSql* pinstancesqlData();

    static Inquire_Sql_Info* pinstanceInquiredata();



private:

    MainWindow *pmainWindow;

	CustomCreatSql* _msqldata;

    controldimming *_mcontroldimming; //控温调光

    Alarm *_mreminderinfowidget; //仪器信息提示

    //耗材
    QScopedPointer<QualityControl> minstrumentConsumables;

    //数据查询sql
    QScopedPointer<Inquire_Sql_Info> minquireSqldata;


    Testing *_mtestingwidget; //测试界面

    ConfigureProjectItem *_mpsetTestproject;

    Height_Data *_maddtestsamplecase;

    instrumentAlarmPrompt* _msuppilereminder; //耗材提示框

    ConsumablesWrite* _mwritecommand;

    SuoweiSerialPort* _mpsreialport;

    USB_InitConnect* _mpSingleactive;

    displayLogsText* _mLoadingLogfile;

    CustomPlot* _mAdjustthecoordinates;

    mainControlBoardProtocol* _mparsemainboard; //解析主板信息

    Calibrate* _mppatientinfo; //患者信息

    Monitor_TrayTest* _mpobtainModuledata; //模组数据调光 调速

    MachineSetting *_mequipmentconfig; //仪器设置

    bool _ready;
    void disconnectAllConnections();

};

#endif // FULLYAUTOMATEDPLATELETS_H
