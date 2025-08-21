void on_pushButtonopenSuck_clicked();
#ifndef MACHINESETTING_H
#define MACHINESETTING_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QStringListModel>
#include <qDebug>
#include <QThread>
#include <QJsonDocument>
#include <QTableWidget>
#include <QSpinBox>
#include "json.h"
#include "delegate.h"
#include "analyticaljson.h"
#include "addusername.h"
#include "crc/JQChecksum.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QMetaType>
#include <QSignalMapper>
#include <QTextBrowser>
#include <QToolButton>
#include "ini_file.h"
#include "cglobal.h"
#include "channelvaluepanel.h"
#include "doqualitycontrol.h"
#include <iostream>
#include <cmath>
#include <memory>
#include "operregistry.h"
#include "dilag/testopcv.h"

#include    "replacethetesttubetray.h" //性能验证

#define MACHINECONFIGURE_COMMON         0 //常规设置
#define MACHINECONFIGURE_HOSPITALINFO   1 //医院用的那些信息
#define MACHINECONFIGURE                2 //机器的配置参数
#define MACHINE_MAINTENANCE             3 //维护参数
#define MACHINE_MAINTENANCE_INDEX       7 //维护index

#define MACHINECONFIGURE_CHANNELVALUE   4 //机器测试通道值
#define MACHINECONFIGURE_LIS            6 //LIS 未做
#define MACHINECONFIGURE_REFERENCE      3 //参考值对比
#define MACHINE_QUALITYCONTROL          5 //质控
#define MACHINE_PE                      8 //性能
//#define MACHINE_QUALITYTESTATER         6 //测水质控



struct btnLable
{
   QLabel* plableinfo;
   QSpinBox* pdownmovemm;
   int index_;
};


namespace Ui {
class MachineSetting;
}

class MachineSetting : public QWidget
{
    Q_OBJECT

public:
    explicit MachineSetting(QWidget *parent = 0);
    ~MachineSetting();

    void sycnequipmentKind(quint8 indexEquipment);
    void _initpara();
    void completedPETest();
    void HandleoutputResultData(const QString& id, const quint8& channel, const std::array<double, 3> &ratios);

protected:
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject *obj, QEvent *ev) override;

private:
    void initDimmingTab(QTableWidget *pdimmingTable, const quint8 indexequipment); //初始化调光表格

    void insertDataItem(QTableWidget* pdimmingTable,
                        int row, int col,
                        const QString& text,
                        bool readOnly); //插入

    void tableinsertBtn(QTableWidget *pdimmingTable, int row, int col, const QString &customQss); //表格插入按钮

    void itemChangeValue(const int row,const int cols, const int val);

    void initSheet();//初始化设置界面的控件样式

    //信号与槽连接
    void intsignalsMable();

    void initSettinggeneralParameters(); //初始化配置普通参数


    /*true set试剂针配置参数*///初始化试剂参数
    void ReagentConfigParaSetOrGet(const bool bsetConfig);
    //写
    void configWrite2BoardReagentNeedlePara();
    void updateReagentCapacityAndLimit();
    void writeConsumablesToInstrument();
    //初始化
    void initBoardReagentNeedlePara();


    void initdisplaymoduleChn(const quint8 equipmentIndex);  //初始化显示模组
    void ImportParameters(const quint8 equipmentIndex);   /*初始化仪器配置参数all*/
    void initloginpassword();
    void _initconfigmodulefucn();

    /**  工程师模式布局
     * @brief Engineerinterfacelayout
     */
    void Engineerinterfacelayout(bool enterEngineerMode);

    //维护模式
    void configBloodpinparaSignals();

	void _configReagPinparaSignals();

    void _configHandsinitChn(quint8 equipmentIndex_);

    void _initBloodpinpara();

    void _initReagPinpara();

    void _innitHands(quint8 equipmentIndex_);

    void _hidecontrol(quint8 index, QLabel* plabel, QSpinBox* pspinbox, int _ks600, int _ks800);

    void _creatstupoint(QLabel* plable, QSpinBox* pgetdownmm, int index_ , QMap<QLabel *, btnLable *> &data_);

    void loadPerformanceEvaluation(QWidget *parentWidget);

private:
    using SettingAction = std::function<void()>;
    void  connectSetting(QWidget* widget, SettingAction action);

public:
    void runInThread();
    void InitMachineui();

    void init_MchineCommon();  /*初始化常规信息*/

    void initial_ReferenceValue(); //参考值
    void SetColumnText(QTableWidget * tablewiget,int row,int col,QString text);

    void init_TabHospitalInfo();
    void initializeHospitalDepartmentInformation();

    //更新参数
    void updatepara(const bool isTesting);


    //修改密码
private:
    bool hasModifyPermission(const QString clickuser) const;
    void showPermissionDeniedWarning() const;
    void createModifyDialog(const QString& username);
    void centerDialog(QWidget* widget) const;

private slots:
    void CheckSelect(QTreeWidgetItem* ,int);
    void operNotifyModuleTEMP(int moduleIndex, double value);//修改模组温度回车确定
    void on_pushButton_ADD_clicked();
    void on_pushButton_DELETE_clicked();
    void on_pushButton_CHANGE_clicked();
    void on_toolButton_Delete_clicked();
    void on_toolButton_viewSQL_clicked();
    void on_comboBox_Category_currentIndexChanged(int index);
    void on_lineEdit_Name_textChanged(const QString );
    void on_lineEdit_Abbreviation_textChanged(const QString);

    void handleButtonClick(int row,int col); //保存模组
    void handleButtonClicked(QAbstractButton* button);

    void onMaintenanceButtonClicked();


public slots:
    void SetUserName(const QString user , const QString, bool vip);

    void SlotSetChannelValueUpdate(const int, const int);

    void sycnupdatesqltable(QTableWidgetItem *item);

    void slotsendReminder(QString reminderstr, const int indexActive); //模组设置调整参数接收到信号提示

    void slotmoduleSpeedData(quint8 indexModule, int chnIData, int chnIIData, int chnIIIData, int chnIVData); //读取模组速度

    //读取模组的LED
    void slotmoduleLedData(quint8 indexModule, int chnIData, int chnIIData, int chnIIIData, int chnIVData);

    //监测返回负压值
    void handlebackHandssuck(bool issuck,int airval);

private slots:
    void on_toolButton_Cancel_clicked();
    void on_lineEdit_Tab0_hospital_name_editingFinished();
    void show_Preence_sets();
    void on_toolButton_ADD_clicked();
	void on_toolButton_Import_clicked();
    void on_toolButton_export_clicked();
	void on_pushButton_backsetting_clicked();
    void on_pushButton_SelTextOutPath_clicked();
    void on_pushButton_opencv_clicked();
    void on_pushButton_Adjustcoordinates_clicked();
    void on_pushButtonsplitAirs_clicked();
    void on_pushButtonopenSuck_clicked();
    void on_pushButtonsavedimming_clicked();

public:

    void InitTabTwoDate(int tabnum ,QString str);

    void AnalyticalJsonTwo(QStringList); //解析

    void _finishmovetestdownhigh(int index_);//测试下Z高度完成

	void openKeyboard(); //取消监听键盘

    void disablechn(QVector<quint8> indexchnList); //调整LED值失败禁用通道

signals:

    /** 登录工程师模式
     * @brief LoginEngineerMode
     */
    void LoginEngineerMode(const bool);

    void temperatureChanged(int index, double value);

    //质控通道值的获取
    void getchanneldata(const quint8,int);

    void sycnViewCurvePara(int curnum,bool bcheckavge); //测试曲线参数

    void pauseConnectModule(bool bpause);//暂停读模组

    void WriteArryCommand(const QByteArray arry, QString kindCommad);

    void testdownheight(QByteArrayList ,int ); //调试下降高度

    void controlallchn(bool bopened); //所有通道的开关

    void OpenChannelMotor(const int, const bool); //控制通道转子开关

    void SetParatoInstrument(const QByteArrayList,QString ); /*配置基础参数到仪器*/


    void Synchronizeupdates(); //同步更新患者详细信息

    void ModifyUsername(QString);//选中用户修改密码

     /*设置单个模组温度*/
    void config_modul_temp(quint8 indexmodul, double modultemp);

private:
    void    engineerLogin(bool makesure); //工程师登录

    void    ChannelSettingStatus(); /*设置通道状态*/

    void    Instrument_configuration_channel_display(const quint8 equipmentIndex); //显示通道值根据仪器配置显示通道数

    void    handleButtonAction(const int moduleIndex, int col);




    //写试剂针参数
    void    notifyReagentPinParaToBoard();

    void    LabelTextColors(QLabel *ptextReminder,QColor color,QString reminderStr);//设置提示文字字体颜色

    void    sendmainbordBasicPara();  /*机器要用得参数保存*/
    void    HandsParaWriteBoard();
    void    BloodPinParaWriteBoadr();

    QByteArray outParaOx1aWrite();

    //调光值写入到主板
    void configWriteDimming2Board();




private:
    Ui::MachineSetting *ui;
    QButtonGroup *mResultMode = nullptr;  //测试结果数据模式



    QList<quint8> QualityChannel; //质控通道
    //functionclass *m_pQualityTest;

    QList<QTreeWidgetItem*> m_rootList ;
    QTreeWidgetItem * m_Modelitem;
    QTreeWidgetItem * m_childItem_0_1;
    QTreeWidgetItem * m_childItem_0_2;
    QTreeWidgetItem * m_childItem_0_3; //参数配置
    QTreeWidgetItem*  m_maintenance;   //维护设置

    QTreeWidgetItem * m_Modelitem_1;

    QTreeWidgetItem * m_childItem_1_2;
    QTreeWidgetItem  *m_ReferenceValue;

    //LIS
    QTreeWidgetItem * m_Modelitem_3;
    //质控
    //QTreeWidgetItem *m_qualitycontrol;

    //质控2
    QTreeWidgetItem *m_qualitycontrol_2;

    //性能
    QTreeWidgetItem *mperform = nullptr;

    int m_Info_Index;
    QList<QTableWidget *> m_tableList;
    QStringList m_CategoryList;
    QJsonObject m_obj_Needle[4]; //试剂针
    QJsonObject m_obj_Debug_Needle[3]; //调试--试剂针


    AnalyticalJson m_Analytical;
    QVariant m_AnalyticalOneData;//解析出的一个数据
    QVariantList m_AnalyticallistData;

    QList<ChannelvaluePanel* > m_displayChannelwidget;


    //通道转子
    bool mbopendallchannel;

    QVector<QDoubleSpinBox *> ksetTempSpinboxList;
    QList<QSpinBox *> m_capactityList;   //试剂容量
    QList<QSpinBox *> m_limitratioList;  //试剂限位


    bool m_initedDimmingTable = false; //插入通道调整参数到表格完成状态

    quint8 m_typedequipment;
    QPropertyAnimation* m_propertyAnimation = nullptr;

    QMap<QLabel* , btnLable*>   m_pbloodLableList;   //血样针lable要下针命令
    QMap<QLabel* , btnLable* >  m_preagpinLableList; //试剂针lable要下针命令
    QMap<QLabel* , btnLable*>   m_pHandsLableList;   //抓手针lable要下针命令

    //测试opencv
    TestOpcv *m_showOpencvImage = nullptr;

    //性能验证
    std::unique_ptr<ReplaceTheTestTubeTray> m_Performanceverification;

    bool m_initAdjustcoordinates; //点击调整坐标标记

    //按钮样式
    QString m_settButtonQss;
    QString m_setToolButtonQss;
    QString m_setspinBoxQss;
    QString m_doubleSpinboxqss;

    QShortcut *menterShortcut = nullptr;        // f4 快捷键
    bool shortcutsConnected = false;           // 连接状态标志

    void connectEnterReturnShortcuts();
    void disconnectEnterReturnShortcuts();


    // 私有辅助模板函数
    template<typename T, typename ValueType>
    std::array<ValueType, 5> getUiValues(
        const std::array<T*, 5>& spinBoxes)
    {
        std::array<ValueType, 5> values;
        for (int i = 0; i < 5; ++i) {
            if (spinBoxes[i]) {
                values[i] = static_cast<ValueType>(spinBoxes[i]->value());
            } else {
                values[i] = ValueType{};
                QLOG_WARN() << "Invalid spinbox pointer at index:" << i;
            }
        }
        return values;
    }

};

#endif // MACHINESETTING_H
