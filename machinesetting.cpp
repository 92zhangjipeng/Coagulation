#pragma execution_character_set("utf-8")

#include "machinesetting.h"
#include "ui_machinesetting.h"
#include <QFile>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QEasingCurve>
#include "loginui.h"
#include "testing.h"
#include <random>
#include <QMetaType>
#include <iostream>
#include "globaldata.h"
#include "warn_interface.h"
#include <operclass/fullyautomatedplatelets.h>


bool subDevListSort(const QCheckBox* contorl_1, const QCheckBox* contorl_2)
{
    QString num_statr = contorl_1->text();
    QString num_end  = contorl_2->text();
    int nTime_1 = QUIUtils::StringFindintnum(num_statr);
    int nTime_2 = QUIUtils::StringFindintnum(num_end);
    return nTime_1 < nTime_2; //升序
}

bool SmalltoLarge(ChannelvaluePanel *pBeginCotl,ChannelvaluePanel *pendCotl)
{
    QString objectIndex_s = pBeginCotl->objectName();
    int index_s = QUIUtils::StringFindintnum(objectIndex_s);
    QString objectIndex_e = pendCotl->objectName();
    int index_e = QUIUtils::StringFindintnum(objectIndex_e);
    return (index_s < index_e);
}




MachineSetting::MachineSetting(QWidget *parent) :
    QWidget(parent),
    mbopendallchannel(false),
    m_initAdjustcoordinates(false),
    ui(new Ui::MachineSetting)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("仪器参数设置"));
    this->activateWindow();
    this->setWindowState((this->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);


    m_preagpinLableList.clear();
    m_pbloodLableList.clear();
    m_pHandsLableList.clear();

    ui->checkBox_Recapture->hide();
    //ui->groupBox_setfile->hide();
    ui->pushButton_backsetting->hide();
}

MachineSetting::~MachineSetting()
{
    delete mResultMode;
    mResultMode = nullptr;

    if(m_showOpencvImage)
       delete m_showOpencvImage;
    m_showOpencvImage = nullptr;

	m_Performanceverification.reset();

    for(auto p : m_rootList)
    {
        QTreeWidgetItem* neddel = p;
        delete neddel;
        neddel = nullptr;
    }
    m_rootList.clear();

    auto iter = m_pbloodLableList.begin();
    while(iter != m_pbloodLableList.end())
    {
        if(iter.value() != nullptr )
        {
            delete iter.value();
            iter.value() = nullptr;
        }
        iter++;
    }
    QMap<QLabel* ,btnLable* > nullmap;
    m_pbloodLableList.swap(nullmap);


    iter = m_preagpinLableList.begin();
    while(iter != m_preagpinLableList.end())
    {
        if(iter.value() != nullptr )
        {
            delete iter.value();
            iter.value() = nullptr;
        }
        iter++;
    }
    QMap<QLabel* ,btnLable* > nullmapreagpin;
    m_preagpinLableList.swap(nullmapreagpin);

    iter = m_pHandsLableList.begin();
    while(iter != m_pHandsLableList.end())
    {
        if(iter.value() != nullptr )
        {
            delete iter.value();
            iter.value() = nullptr;
        }
        iter++;
    }
    QMap<QLabel* ,btnLable* > nullmaphands;
    m_pHandsLableList.swap(nullmaphands);


    if(m_propertyAnimation){
        delete m_propertyAnimation;
        m_propertyAnimation = nullptr;
    }

    delete ui;
}


void MachineSetting::sycnequipmentKind(quint8 indexEquipment)
{
    m_typedequipment = indexEquipment;
}

void MachineSetting::_initpara()
{
    ui->MachineSetList->setHeaderHidden(true); //隐藏表头
    ui->MachineSetList->clear();
    ui->tabWidgetSetconfigure->tabBar()->hide();

    m_AnalyticalOneData.clear();
    m_AnalyticallistData.clear();

    InitMachineui();

    initializeHospitalDepartmentInformation(); /*初始化医院科室信息*/

    ImportParameters(m_typedequipment); /*初始化导入配置参数*/

    //初始化调光表格
    initDimmingTab(ui->tableWidget_controlChn, m_typedequipment);
    ui->tableWidget_controlChn->setTabKeyNavigation(false);
    ui->widget_valuediaplay->hide();

    Instrument_configuration_channel_display(m_typedequipment); //仪器配置通道显示

    ChannelSettingStatus(); /*设置通道状态信号*/

    _initconfigmodulefucn();

    engineerLogin(false);

    //维护模式init  get
    _initBloodpinpara();

    _initReagPinpara();

    _innitHands(m_typedequipment);

    intsignalsMable();

    QObject::connect(ui->pushButton_SWITCH,&QPushButton::clicked,this,[=]()
    {
        FullyAutomatedPlatelets::mainWindow()->_displayHandoffUser();//切换用户
    });

    initSheet();//init按钮等控件样式显示

	QStringList itemCutNum{ "1" , "2" , "3" ,"5" , "10" };
    QListWidget* listWidget = new QListWidget(this);
    for(int i = 0 ; i < itemCutNum.count(); ++i){
        QListWidgetItem *item = new QListWidgetItem(itemCutNum.at(i));
        item->setTextAlignment(Qt::AlignCenter);
        listWidget->addItem(item);
    }
    ui->comboBox_CutNum->setModel(listWidget->model());
    ui->comboBox_CutNum->setView(listWidget);
    ui->checkBox_Avg_cutnum->setChecked(false);
    ui->checkBoxExperimental->setChecked(INI_File().getexperimentalMode());

    ui->checkBox_absorbance->setChecked(INI_File().rConfigPara("AbsorbanceAlgorithm").toBool());

    connect(ui->toolButton_SaveCutnum,&QToolButton::clicked,this,[=](){
        auto &ini = INI_File();
        ini.setexperimentalMode(ui->checkBoxExperimental->isChecked());
        QUIUtils::isDirExist(QUIUtils::getappload("ExperimentalMode"));
        ini.wConfigPara("AbsorbanceAlgorithm",ui->checkBox_absorbance->isChecked());
        int   curnum =  QString(ui->comboBox_CutNum->currentText()).toInt();
        bool  bcheckavge = ui->checkBox_Avg_cutnum->isChecked();
        emit  sycnViewCurvePara(curnum,bcheckavge); //测试曲线参数


        emit SetParatoInstrument({ outParaOx1aWrite() },"0x1a参数");
    });
}


void MachineSetting::initSheet()
{
    ui->pushButton_opencv->hide();
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
          m_settButtonQss = QLatin1String(styleFile.readAll());
          styleFile.close();
    }

    QHash<QPushButton*, QString> pushButtonList = {
         {ui->pushButton_ADD,    tr("添加用户")},
         {ui->pushButton_DELETE, tr("删除用户")},
         {ui->pushButton_CHANGE, tr("修改密码")},
         {ui->pushButton_SWITCH, tr("切换用户")},
         {ui->pushButton_maintenance,tr("登录维护(F4)")},
         {ui->pushButtonBloodPinParasave,tr("写入血样针参数")},
         {ui->pushButton_saved,tr("保存试剂针参数")},
         {ui->pushButton_opencv,tr("测高测试调整")},
         {ui->pushButtonopenSuck,tr("打开负压")},
         {ui->pushButtonsplitAirs,tr("关闭负压")},
         {ui->pushButton_Adjustcoordinates,tr("校准坐标")},
         {ui->pushButtonHandsParaSave,tr("保存抓手参数")},
         {ui->pushButtonsavedimming,tr("写入调光值")}
    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
         QPushButton* button = it.key();
         QString text = it.value();
         // 使用button和text进行操作
         button->setStyleSheet(m_settButtonQss);
         button->setText(text);
    }

    QFile styleFileToolBtn(":/Picture/SetPng/wholeToolButton.qss");
    if(styleFileToolBtn.open(QIODevice::ReadOnly)) {
          m_setToolButtonQss = QLatin1String(styleFileToolBtn.readAll());
          styleFileToolBtn.close();
    }
    QHash<QToolButton*, QString> toolButtonList = {
         {ui->toolButton_Import,    tr("导入坐标文件")},
         {ui->toolButton_export, tr("导出坐标文件")},
         {ui->toolButton_viewSQL, tr("同步更新")},
         {ui->toolButton_ADD, tr("添加(+)")},
         {ui->toolButton_Delete,tr("删除(-)")},
         {ui->Machine_conf_save,tr("保存")},
         {ui->toolButton_Cancel,tr("取消")},
         {ui->toolButton_oopenall,tr("测试转动")},
         {ui->toolButton_SaveCutnum,tr("确定")}

    };
    for (auto it = toolButtonList.cbegin(); it != toolButtonList.cend(); ++it) {
         QToolButton* toolbutton = it.key();
         QString text = it.value();
         toolbutton->setStyleSheet(m_setToolButtonQss);
         toolbutton->setText(text);
    }


    QFile stylespinBox(":/Picture/SetPng/wholeSpinBox.qss");
    if(stylespinBox.open(QIODevice::ReadOnly)) {
          m_setspinBoxQss = QLatin1String(stylespinBox.readAll());
          stylespinBox.close();
    } else {
        QLOG_WARN() << "QSS load error:" << stylespinBox.errorString();
    }

    QVector<QSpinBox*> spinboxList = {ui->EmptyHeigh, ui->SecurityValue_box,ui->poorBlood_changliang,
                                      ui->spinBox_CleanLinqueFailedHigh,
                                      ui->Testheighdownheigh,
                                      ui->spinBox_faliedlinque,
                                      ui->spinBox_suckairs,
                                      ui->spinBoxAbsorbX2,
                                      ui->spinBoxAbsorbX1,
                                      ui->spinBoxReagentNeedleFailehigh,
                                      ui->spinBox_TestReagentFailedhigh,
                                      ui->spinBox_deptime,
                                      ui->spinBox_AA_Suck_Vol,
                                      ui->spinBox_ADP_Suck_Vol,
                                      ui->spinBox_ADR_Suck_Vol,
                                      ui->spinBox_COL_Suck_Vol,
                                      ui->spinBox_RIS_Suck_Vol,
                                      ui->spinBox_down_AA,
                                      ui->spinBox_down_ADP,
                                      ui->spinBox_down_ADR,
                                      ui->spinBox_down_COL,
                                      ui->spinBox_down_RIS,
                                      ui->spinBox_capacity_aa,
                                      ui->spinBox_capacity_adp,
                                      ui->spinBox_capacity_epi,
                                      ui->spinBox_capacity_col,
                                      ui->spinBox_capacity_ris,
                                      ui->spinBox_capacity_cleanlinque,
                                      ui->spinBox_limit_aa,
                                      ui->spinBox_limit_adp,
                                      ui->spinBox_limit_epi,
                                      ui->spinBox_limit_col,
                                      ui->spinBox_limit_ris,
                                      ui->spinBox_limit_cleanlinque,
                                      ui->spinBox_downthrowcups,
                                      ui->spinBox_testtray_0,
                                      ui->spinBox_testtray_1,
                                      ui->spinBox_testtray_2,
                                      ui->spinBox_testtray_3,
                                      ui->spinBox_handsdownchn_0,
                                      ui->spinBox_handsdownchn_1,
                                      ui->spinBox_handsdownchn_2,
                                      ui->spinBoxSuckAirsMax,
                                      ui->spinBoxSplitAirsMin,
                                      ui->spinBox_suckLatetimer,
                                      ui->spinBox_WashesTime
                                     };
    for (auto* pspinBox : spinboxList) {
        pspinBox->setStyleSheet(m_setspinBoxQss);
    }



    QFile styleDoublespinBox(":/Picture/SetPng/wholedoubleSpinBox.qss");
    if(styleDoublespinBox.open(QIODevice::ReadOnly)) {
          m_doubleSpinboxqss = QLatin1String(styleDoublespinBox.readAll());
          styleDoublespinBox.close();
    } else {
        QLOG_WARN() << "QSS load error:" << styleDoublespinBox.errorString();
    }
    QVector<QDoubleSpinBox*> doubleSpinboxList = {
            ui->doubleSpinBox_Moduletemperature_1,
            ui->doubleSpinBox_Moduletemperature_2,
            ui->doubleSpinBox_Moduletemperature_3,
            ui->doubleSpinBox_Ratio_ben,
            ui->doubleSpinBox_PRPratio,
            ui->FixedHighvalue,
            ui->OffsetTestHeightValue,
            ui->doubleSpinBox_AA_Ratio,
            ui->doubleSpinBox_ADP_Ratio,
            ui->doubleSpinBox_ADR_Ratio,
            ui->doubleSpinBox_COL_Ratio,
            ui->doubleSpinBox_RIS_Ratio,
            ui->doubleSpinBox_Add_AA_Ratio,
            ui->doubleSpinBox_Add_ADP_Ratio,
            ui->doubleSpinBox_Add_ADR_Ratio,
            ui->doubleSpinBox_Add_COL_Ratio,
            ui->doubleSpinBox_Add_RIS_Ratio
    };
    for (auto* pdoublespinBox : doubleSpinboxList) {
        pdoublespinBox->setStyleSheet(m_doubleSpinboxqss);
    }

    GlobalData::QLineEditSheet(ui->lineEdit_Tab0_hospital_name);
    GlobalData::QCheckboxSheet(ui->checkBox_Automatic_review,tr("打印报告时自动审核未审核的记录"));
    GlobalData::QCheckboxSheet(ui->checkBox_Automatic_print,tr("测试完成后自动打印"));
    GlobalData::QCheckboxSheet(ui->checkBoxExperimental,tr("实验模式"));
    GlobalData::QLineEditSheet(ui->lineEdit_showPath);
    GlobalData::QCommboxSheet(ui->comboBox_Category);
    GlobalData::QLineEditSheet(ui->lineEdit_Abbreviation);
    GlobalData::QLineEditSheet(ui->lineEdit_Name);



    GlobalData::QCheckboxSheet(ui->UseBarCode,tr("扫描条形码"));
    GlobalData::QCheckboxSheet(ui->checkBox_UesSecondReagentHole,tr("启用预留试剂位置"));
    GlobalData::QCheckboxSheet(ui->checkBox_catchcups,tr("初始清空试杯"));

    QVector<QCheckBox*> checkboxList = {
                    ui->Channel_State_1,
                    ui->Channel_State_2,
                    ui->Channel_State_3,
                    ui->Channel_State_4
                    ,ui->Channel_State_5,
                    ui->Channel_State_6,
                    ui->Channel_State_7,
                    ui->Channel_State_8
                    ,ui->Channel_State_9,
                    ui->Channel_State_10,
                    ui->Channel_State_11,
                    ui->Channel_State_12};
    int index = 1;
    for (QCheckBox* checkbox : checkboxList) {
        GlobalData::QCheckboxSheetChnstate(checkbox, QString("通道[%1]").arg(index++));
    }

    GlobalData::QCheckboxSheet(ui->checkBox_suck_offset,tr("校准吸样"));
    GlobalData::QLineEditSheet(ui->lineEdit_maintenance);
    GlobalData::QCheckboxSheet(ui->checkBox_Recapture,tr("抓手重抓"));
    GlobalData::QCheckboxSheet(ui->checkBox_originTestData,tr("原始数据"));
    GlobalData::QCheckboxSheet(ui->checkBox_average,tr("去极值平均滤波"));
    GlobalData::QCheckboxSheet(ui->checkBox_median,tr("中位值滤波"));
    GlobalData::QCheckboxSheet(ui->checkBoxTriple,tr("三级自适应终端滤波"));
    GlobalData::QCheckboxSheet(ui->checkBox_Avg_cutnum,tr("分阶平均"));
    GlobalData::QCheckboxSheet(ui->checkBox_absorbance,tr("吸光度算法"));
    GlobalData::QCommboxSheet(ui->comboBox_CutNum);

    
	//测试曲线模式
    mResultMode = new QButtonGroup();
    mResultMode->addButton(ui->checkBox_originTestData,FILTER_NO);
    mResultMode->addButton(ui->checkBox_average, FILTER_AVERAGE_VALUE);
    mResultMode->addButton(ui->checkBox_median, FILTER_MIDVALUE);
    mResultMode->addButton(ui->checkBoxTriple,MEDIAN_EWMA_DYNAMIC);
    mResultMode->setExclusive(true);

    const int indexMode = INI_File().getFilteringMode();
    QAbstractButton* button = mResultMode->button(indexMode);
    if (button) {
        button->setChecked(true);
    }
    connect(mResultMode, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
               this, &MachineSetting::handleButtonClicked);

    return;
}

void MachineSetting::handleButtonClicked(QAbstractButton* button){
    const int indexMode = mResultMode->id(button);
    QCheckBox* cb = qobject_cast<QCheckBox*>(button);
    // 确保是QCheckBox且被选中
    if (cb && cb->isChecked()) {
        INI_File().setFilteringMode(indexMode);
    }
}

void MachineSetting::intsignalsMable()
{
    //试剂容量
    m_capactityList = {
           ui->spinBox_capacity_aa,
           ui->spinBox_capacity_adp,
           ui->spinBox_capacity_epi,
           ui->spinBox_capacity_col,
           ui->spinBox_capacity_ris,
           ui->spinBox_capacity_cleanlinque
    };

    //试剂限位
    m_limitratioList ={ui->spinBox_limit_aa,
                      ui->spinBox_limit_adp,
                      ui->spinBox_limit_epi,
                      ui->spinBox_limit_col,
                      ui->spinBox_limit_ris,
                      ui->spinBox_limit_cleanlinque};

    connect(this, &MachineSetting::temperatureChanged,
            this, &MachineSetting::operNotifyModuleTEMP);

    ksetTempSpinboxList = {
           ui->doubleSpinBox_Moduletemperature_1,
           ui->doubleSpinBox_Moduletemperature_2,
           ui->doubleSpinBox_Moduletemperature_3
    };

    for(int i = 0; i < ksetTempSpinboxList.size(); ++i) {
        connect(ksetTempSpinboxList[i], &QDoubleSpinBox::editingFinished,
                [this, i]() {
                    emit temperatureChanged(i, ksetTempSpinboxList[i]->value());
                });
    }



    connect(ui->Machine_conf_save,&QToolButton::clicked,this,[=](){
        sendmainbordBasicPara();
    });

    //保存抓手参数到主板
    connect(ui->pushButtonHandsParaSave,&QPushButton::clicked,this,[=](){
        HandsParaWriteBoard();
    });

    //血样针参数保存到主板
    connect(ui->pushButtonBloodPinParasave,&QPushButton::clicked,this,[=](){
        BloodPinParaWriteBoadr();
    });

}

void MachineSetting::engineerLogin(bool makesure)
{
    (makesure)? ui->widget_valuediaplay->show():
            ui->widget_valuediaplay->hide();
    update();
    return;
}



void MachineSetting::showEvent(QShowEvent* event)
{
    this->setAttribute(Qt::WA_Mapped);
    QWidget::showEvent(event);
}

void MachineSetting::operNotifyModuleTEMP(int moduleIndex, double value)
{
    int indexmodule = moduleIndex + 1;
    auto &ini = INI_File();
    switch(indexmodule){
    case MODULE_1: ini.wConfigPara(MODULETEMPA,value);break;
    case MODULE_2: ini.wConfigPara(MODULETEMPB,value);break;
    case MODULE_3: ini.wConfigPara(MODULETEMPC,value);break;
    default: break;
    }
    QLOG_DEBUG()<<"设置模组:"<<indexmodule<<"温度:"<<value;
    emit config_modul_temp(indexmodule,value);
    return;
}

void MachineSetting::initSettinggeneralParameters()
{
    auto &ini = INI_File();

    bool bScanBarcode = ini.rConfigPara(SCANCODEBAR).toBool();
    ui->UseBarCode->setChecked(bScanBarcode);

    bool buseRightReagent = ini.rConfigPara(USINGASIDEREAGENTLOC).toBool();
    ui->checkBox_UesSecondReagentHole->setChecked(buseRightReagent);

    bool bbootCathCups = ini.rConfigPara(INITGRABCUPS).toBool();
    ui->checkBox_catchcups->setChecked(bbootCathCups);


    ui->checkBox_Recapture->setChecked(ini.rConfigPara(REPEATGRABCUP).toBool());//抓手重抓
    ui->checkBox_suck_offset->setChecked(ini.rConfigPara(FIRSTSUCKAIRS).toBool()); //吸空气校准

    ui->spinBox_WashesTime->setValue(INI_File().getWashesTime()); //清洗时间
    return;
}

void MachineSetting::openKeyboard()
{
    this->setFocusPolicy(Qt::StrongFocus);
    //this->releaseKeyboard(); // 键盘监听，防止输入失效
}


void MachineSetting::SlotSetChannelValueUpdate(const int IndexChannel, const int DisplayValue)
{
    m_displayChannelwidget.at(IndexChannel)->setValue(DisplayValue);
    if(cglobal::g_controldimmingfinished)
        return;
    FullyAutomatedPlatelets::pinstancedimming()->_obtainChndata(IndexChannel,DisplayValue);
    return;
}

void MachineSetting::closeEvent(QCloseEvent *event)
{
    event->accept();
    Engineerinterfacelayout(false);
    emit LoginEngineerMode(false);

    ui->widget_valuediaplay->hide();
    ui->lineEdit_maintenance->show();
    ui->label_maintenance->show();
    ui->pushButton_maintenance->setText("登入维护");
    QLOG_TRACE()<<"关闭设置界面";
    return;
}

void MachineSetting::Instrument_configuration_channel_display(const quint8 equipmentIndex)
{
    m_displayChannelwidget = ui->ChannelVale_display->findChildren<ChannelvaluePanel*>();
    m_displayChannelwidget.reserve(m_displayChannelwidget.size());
    std::sort(m_displayChannelwidget.begin(), m_displayChannelwidget.end(), SmalltoLarge);

    for (const auto &displaydchannel : m_displayChannelwidget) {
        displaydchannel->setColorHaloStart(QColor(60,60,60));
        displaydchannel->setColorOuterFrame(QColor(200,160,50));
        QString name = displaydchannel->objectName();  // 缓存对象名
        QString channelStr = name.split("_").at(1);
        displaydchannel->setChannelnum(channelStr.toInt());
    }

    // 内联隐藏模块逻辑
    switch (equipmentIndex) {
        case KS600:
            ui->widget_showModule2th->hide();
            ui->widget_showModule3th->hide();
            break;
        case KS800:
            ui->widget_showModule3th->hide();
            break;
        default: break;
    }
}

void MachineSetting::ChannelSettingStatus()
{
    QList<QCheckBox*> checkBoxList = ui->usechannel->findChildren<QCheckBox*>();
    std::sort(checkBoxList.begin(), checkBoxList.end(), subDevListSort);

    // 预定义常量字符串
    const QString enabledText = QString::fromUtf8("启用");
    const QString disabledText = QString::fromUtf8("禁用");
    const QString configKeyTemplate = "TestTheChanne1Opening_%1";
    const QString logTemplate = QString::fromUtf8("主动设置通道%1[%2]");

    for (QCheckBox* channelbox : checkBoxList)
    {
        // 在循环外计算通道索引（提高效率）
        const quint8 indexChn = QUIUtils::StringFindintnum(channelbox->objectName());

        // 使用传统信号连接方式（兼容 Qt 5.10）
        connect(channelbox, &QCheckBox::stateChanged, [=](int state) {
            // 直接使用信号传递的state参数（避免重复调用isChecked）
            const bool ischeckedState = (state == Qt::Checked);
            const QString wkey = configKeyTemplate.arg(indexChn);

            // 写入配置
            INI_File().wConfigPara(wkey, ischeckedState);

            // 更新通道状态
            const int status = ischeckedState ? CHN_STATUS_FREE : CHN_STATUS_DISABLE;
            StructInstance::getInstance()->config_testChn_State(indexChn - 1, status);

            // 通知状态变更
            FullyAutomatedPlatelets::pinstanceTesting()->recv_NotifyChannleState(indexChn, ischeckedState);

            // 记录日志
            const QString& stateText = ischeckedState ? enabledText : disabledText;
            const QString _printtext = logTemplate.arg(indexChn).arg(stateText);
            FullyAutomatedPlatelets::pinstanceinfowidget()->InsertText(USERACTIONS, _printtext);
            QLOG_DEBUG() << _printtext;
         });
    }
}


void MachineSetting::InitMachineui()
{
    initloginpassword();

    m_Modelitem = new QTreeWidgetItem();
    m_Modelitem->setText(0, tr("通用"));
    m_Modelitem->setIcon(0, QIcon(":/Picture/general.png")); // 统一图标设置
    m_rootList.append(m_Modelitem);

    m_childItem_0_1 = new QTreeWidgetItem(m_Modelitem,{tr("常规")});
    m_childItem_0_1->setIcon(0,QIcon(":/Picture/routine.png"));


    m_childItem_0_2 = new QTreeWidgetItem(m_Modelitem,{tr("信息库")});
    m_childItem_0_2->setIcon(0,QIcon(":/Picture/Database.png"));


    m_childItem_0_3 = new QTreeWidgetItem(m_Modelitem,{tr("配置参数")});
    m_childItem_0_3->setIcon(0,QIcon(":/Picture/configpara.png"));


    m_maintenance = new QTreeWidgetItem(m_Modelitem,{tr("维护参数")});
    m_maintenance->setIcon(0,QIcon(":/Picture/SetPng/savetask.png"));


    m_Modelitem_1 = new QTreeWidgetItem();
    m_Modelitem_1->setText(0 ,tr("测量"));
    m_Modelitem_1->setIcon(0,QIcon(":/Picture/SetPng/measuring.png"));
    m_rootList.append(m_Modelitem_1);

    m_childItem_1_2 = new QTreeWidgetItem(m_Modelitem_1,QStringList(QString(tr("数据检查"))));
    m_childItem_1_2->setIcon(0,QIcon(":/Picture/SetPng/inspection.png"));


    m_ReferenceValue = new  QTreeWidgetItem(m_Modelitem_1,QStringList(QString(tr("参考值"))));
    m_ReferenceValue->setIcon(0,QIcon(":/Picture/SetPng/Referencevalue.png"));


    m_Modelitem_3 = new QTreeWidgetItem;
    m_Modelitem_3->setText(0,tr("LIS"));
    m_Modelitem_3->setIcon(0,QIcon(":/Picture/list.png"));
    m_rootList.append(m_Modelitem_3);

    //性能检测
    if(!mperform)
        mperform = new QTreeWidgetItem;
    mperform->setText(0,tr("性能验证"));
    mperform->setIcon(0,QIcon(":/Picture/icon_title/PE0.png"));
    m_rootList.append(mperform);

    //m_qualitycontrol = new QTreeWidgetItem;
    //m_qualitycontrol->setText(0,tr("质控"));
   // m_qualitycontrol->setIcon(0,QIcon(":/Picture/SetPng/qualityControlActive.png"));
    //m_rootList.append(m_qualitycontrol);

    ui->MachineSetList->insertTopLevelItems(0,m_rootList);
    ui->MachineSetList->expandAll();

    connect(ui->MachineSetList,SIGNAL(itemClicked(QTreeWidgetItem*,int)), this,SLOT(CheckSelect(QTreeWidgetItem* ,int)));

    init_MchineCommon(); //常规信息
    init_TabHospitalInfo(); //医院信息参考值
    initial_ReferenceValue(); //参考值


    return;
}



//常规信息
void MachineSetting::init_MchineCommon()
{
    // 缓存数据库实例指针
    auto* pSqlData = FullyAutomatedPlatelets::pinstancesqlData();
    // 获取用户名列表并检查有效性
    QStringList allUsernames;
	pSqlData->FindAllUsername(allUsernames);
    if (allUsernames.isEmpty()) {
        QLOG_WARN() << "No users found or database error occurred";
        return;
    }

    // 加载医院名称
    const QString hospitalName = pSqlData->FindPassword("hospital_name");
    ui->lineEdit_Tab0_hospital_name->setText(hospitalName);

    // 初始化用户列表
    ui->listWidget_user->clear();
    for (const QString& username : qAsConst(allUsernames)) {
        if (username == "hospital_name") continue;

        bool isSuperUser = false;
		pSqlData->SelectUserPermissionsControl(username, isSuperUser);

        // 一次性创建带图标和文本的列表项
        QListWidgetItem* item = new QListWidgetItem(isSuperUser ? QIcon(":/Picture/SvipUser.png")
                                                                : QIcon(":/Picture/NormalUser.png"),
            username
        );

        ui->listWidget_user->addItem(item);
    }
}



//添加用户
void MachineSetting::on_pushButton_ADD_clicked()
{
    if (!hasModifyPermission(cglobal::g_UserName_str)) {
        showPermissionDeniedWarning();
        return;
    }

    QScopedPointer<AddUserName> dialog(new AddUserName(0));
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(dialog.data(),&AddUserName::AddUser,this,&MachineSetting::SetUserName);

    centerDialog(dialog.data());
    dialog->setWindowModality(Qt::ApplicationModal);

    if (dialog->isMinimized()) {
        dialog->showNormal();  // 先恢复窗口标准状态
        dialog->raise();       // 确保窗口置顶
        dialog->activateWindow(); // 激活窗口焦点
    }
    dialog->show();

    dialog.take();
}

void MachineSetting::SetUserName(const QString user, const QString Passworsstr,bool vip)
{
    int row = 0;
    QString line;
    while(row < (ui->listWidget_user->count() ))
    {
        line = ui->listWidget_user->item(row)->text();
        if(user == line)
        {
            FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加失败","用户已存在,请勿重复添加!");
            return;
        }
        row++;
    }
    QListWidgetItem *item = new QListWidgetItem(ui->listWidget_user);
    (vip)? item->setIcon(QIcon(":/Picture/SvipUser.png")):
                        item->setIcon(QIcon(":/Picture/NormalUser.png"));
    item->setText(user);
    ui->listWidget_user->addItem(item);
    FullyAutomatedPlatelets::pinstancesqlData()->AddUserName(user,Passworsstr,static_cast<int>(vip) );
    return;
}

//删除用户
void MachineSetting::on_pushButton_DELETE_clicked()
{
    // 常量定义提升可维护性
    const QString WARNING_TITLE = tr("权限控制");
    const QString NO_USER_MSG = tr("请先选择一个用户!");
    const QString SELF_DELETE_MSG = tr("请勿删除登录用户!");

    //获取选中用户
    QListWidgetItem *currentItem = ui->listWidget_user->currentItem();
    if (!currentItem) {
        QMessageBox::warning(this, tr("修改异常"), NO_USER_MSG);
        return;
    }

    // 1. 权限校验
    if (!hasModifyPermission(cglobal::g_UserName_str)) {
        showPermissionDeniedWarning();
        return;
    }


    //bool cleaeuser = false;
    if(ui->listWidget_user->currentItem() != Q_NULLPTR) //获取列表项的指针
    {
        QListWidgetItem* item = ui->listWidget_user->currentItem();
        QString del_user = item->text();
        bool cleaeuser = (del_user == cglobal::g_UserName_str) ? false : true;
        if(!cleaeuser ){
            FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(WARNING_TITLE,SELF_DELETE_MSG);
        }
        else{
            FullyAutomatedPlatelets::pinstancesqlData()->DeleteUserChar(del_user);
            delete item;
        }
    }
    return;
}

/*更改密码*/
void MachineSetting::on_pushButton_CHANGE_clicked()
{
	// 2. 获取选中用户
	QListWidgetItem *currentItem = ui->listWidget_user->currentItem();
	if (!currentItem) {
		QMessageBox::warning(this, tr("修改异常"), tr("请先选择一个用户!"));
		return;
	}

    // 1. 权限校验 應該為當前鄧麗用戶 currentItem->text()
    if (!hasModifyPermission(cglobal::g_UserName_str)) {
        showPermissionDeniedWarning();
        return;
    }

    // 3. 创建修改窗口
    createModifyDialog(currentItem->text());
    return;
}



//----- 新增工具函数 -----//
// 权限校验逻辑封装
bool MachineSetting::hasModifyPermission(const QString clickuser) const {
    bool isSuperUser = false;
    FullyAutomatedPlatelets::pinstancesqlData()->SelectUserPermissionsControl(
        clickuser,
        isSuperUser
    );
    return isSuperUser;
}
// 权限警告统一处理
void MachineSetting::showPermissionDeniedWarning() const {
    FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(
        tr("权限控制"),
        tr("当前登录账户权限不足!")
    );
}

// 窗口创建逻辑封装
void MachineSetting::createModifyDialog(const QString& username) {

    QScopedPointer<AddUserName> dialog(new AddUserName(1));
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    connect(this, &MachineSetting::ModifyUsername,
            dialog.data(), &AddUserName::ModifyUsername_Slot);


    centerDialog(dialog.data());
    dialog->setWindowModality(Qt::ApplicationModal);

    emit ModifyUsername(username);

    if(dialog->isMinimized()){
        dialog->showNormal();  // 先恢复窗口标准状态
        dialog->raise();       // 确保窗口置顶[[12]]
        dialog->activateWindow(); // 激活窗口焦点
    }

    dialog->show();

    dialog.take();
}

// 窗口居中工具函数
void MachineSetting::centerDialog(QWidget* widget) const {
    if (!widget || !QApplication::desktop() ) return;
    QRect screenGeometry = QApplication::desktop()->availableGeometry(this);
    widget->move(
        (screenGeometry.width() - widget->width()) / 2,
        (screenGeometry.height() - widget->height()) / 2
    );
}

//医院名
void MachineSetting::on_lineEdit_Tab0_hospital_name_editingFinished()
{
    bool Alreadyhad = FullyAutomatedPlatelets::pinstancesqlData()->HostipalAlreadyHad("hospital_name");
    if (!Alreadyhad)
        FullyAutomatedPlatelets::pinstancesqlData()->AddUserName("hospital_name", ui->lineEdit_Tab0_hospital_name->text(),NORMAL_USERS);
    else
        FullyAutomatedPlatelets::pinstancesqlData()->NotifyPassword("hospital_name", ui->lineEdit_Tab0_hospital_name->text());
    return;
}


/*参考值*/
void MachineSetting::initial_ReferenceValue()
{
    QStringList itemHeandList,rowsItemList,itemProjectList,itemProjectSecList;
    itemHeandList<<tr("试剂")<<tr("项目 ")<<tr("时间[S]")<<tr("低值(男)")<<tr("高值(男)")<<tr("低值(女)")<<tr("高值(女)")<<tr("单位");
    rowsItemList <<tr("AA")<<tr("ADP")<<tr("EPI")<<tr("COL")<<tr("RIS");
    itemProjectList<< tr("聚集率-1") << tr("聚集率-2") << tr("聚集率-3") << tr("聚集率-Max");
    itemProjectSecList << tr("60") << tr("180") << tr("300") << tr("MAX");
    GlobalData::QTableWidgetinitSheet(ui->tableWidgetReferencevalue,"测试参数对比",itemHeandList,rowsItemList,false);
    ui->tableWidgetReferencevalue->setRowCount(rowsItemList.size() * 4);

    for(int i = 0 ; i < rowsItemList.size() ;i++)
    {
        ui->tableWidgetReferencevalue->setSpan(i*4,0,4,1);//合并单元格
        SetColumnText(ui->tableWidgetReferencevalue,i*4, 0, rowsItemList.at(i));
    }
    int itemrows = 0;
    for (int k = 0; k < rowsItemList.size(); k++)
    {
        for (int j = 0; j < itemProjectList.size(); j++)
        {
            SetColumnText(ui->tableWidgetReferencevalue, itemrows, 1, itemProjectList.at(j));
            SetColumnText(ui->tableWidgetReferencevalue, itemrows, 2, itemProjectSecList.at(j));
            itemrows++;
        }
    }
    ui->tableWidgetReferencevalue->blockSignals(true);
    show_Preence_sets();
    ui->tableWidgetReferencevalue->blockSignals(false);

    connect(ui->tableWidgetReferencevalue,&QTableWidget::itemChanged,this,&MachineSetting::sycnupdatesqltable);

    return;
}

void MachineSetting::sycnupdatesqltable(QTableWidgetItem *item)
{
    int row_ = item->row();
    int cols_ = item->column();
    QString key_ = ui->tableWidgetReferencevalue->item(row_/4*4, 0)->text()+ ui->tableWidgetReferencevalue->item(row_, 1)->text();
    QString update_key = "";
    switch (cols_)
    {
        case 3: update_key = "ManleLow";   break;
        case 4: update_key = "ManleHeigh"; break;
        case 5: update_key = "WomanLow";   break;
        case 6: update_key = "WomanHeigh"; break;
        case 7: update_key = "UnitBom";   break;
    default:
        break;
    }
    FullyAutomatedPlatelets::pinstancesqlData()->_updateReferenceValue(key_,update_key,item->text());
    return;
}


void MachineSetting::show_Preence_sets()
{
    QVariantList RowsData;
    QStringList KeyList;
    KeyList.clear();
    QUIUtils::itemReferenceTestValue(KeyList);
    for(int i = 0 ; i < KeyList.size() ; i++ )
    {
        RowsData.clear();
        QString keyfind = KeyList.at(i);
        FullyAutomatedPlatelets::pinstancesqlData()->FindReeferenceValue(keyfind,RowsData);
        int colstart = 3;
        for(int k = 0 ; k < RowsData.size() ; k++ )
        {
            SetColumnText(ui->tableWidgetReferencevalue,i,colstart,RowsData.at(k).toString());
            colstart++;
        }
    }

}
void MachineSetting::SetColumnText(QTableWidget * tablewiget,int row,int col,QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    item->setFont(QFont( "楷体", 14, QFont::Black ));
    if(col <= 2)
    {
        item->setBackgroundColor(QColor(245, 255, 250));
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }
    item->setTextColor(QColor(0, 0, 0));
    tablewiget->setItem(row,col,item);
    return;
}



void MachineSetting::init_TabHospitalInfo()
{
    ui->groupBox_printReport->setTitle(tr("打印报告"));
    QStringList Patient_sample_repor;
    Patient_sample_repor.clear();
    Patient_sample_repor<<tr("A3")<<tr("A4")<<tr("A5")<<tr("B3");
    ui->comboBox_QC_sample_report->insertItems(0,Patient_sample_repor);
    ui->comboBox_Patient_sample_report->insertItems(0,Patient_sample_repor);

}


void MachineSetting::initializeHospitalDepartmentInformation()
{
    ui->label_Category->setText(tr("类别:"));
    ui->label_Abbreviation->setText(tr("缩写"));
    ui->label_Name->setText(tr("名称"));

    m_CategoryList.clear();
    m_CategoryList.reserve(8);
    m_CategoryList<<tr("科别")<<tr("性别")<<tr("病区")<<tr("床号")
                 <<tr("临床诊断")<<tr("送检医生")<<tr("审核医生")<<tr("备注");

    QListWidget* plistwidget = new QListWidget(this);
    for(const QString &category : qAsConst(m_CategoryList)){ // qAsConst避免深拷贝
        QListWidgetItem *pitem = new QListWidgetItem(category);
        pitem->setTextAlignment(Qt::AlignCenter);
        plistwidget->addItem(pitem);
    }
    ui->comboBox_Category->setModel(plistwidget->model());
    ui->comboBox_Category->setView(plistwidget);
    ui->comboBox_Category->setCurrentIndex(0);



    //状态初始化
    m_Info_Index = 0;
    ui->tabWidget_info->tabBar()->hide();
    ui->toolButton_Delete->setEnabled(false);
    ui->toolButton_ADD->setEnabled(false);

    //表格列表初始化（消除硬编码）
    m_tableList = {
        ui->tableWidget_info, ui->tableWidget_info_1, ui->tableWidget_info_2,
        ui->tableWidget_info_3, ui->tableWidget_info_4, ui->tableWidget_info_5,
        ui->tableWidget_info_6, ui->tableWidget_info_7
    };

    // 6. 表格统一配置
    const QHash<QString, QVariant> tableConfig = {
        {"columnCount", 2},
        {"stretchLastSection", true},
        {"verticalSectionSize", 30}
    };

    for (QTableWidget *table : qAsConst(m_tableList)) {
            table->setColumnCount(tableConfig["columnCount"].toInt());
            table->horizontalHeader()->hide();
            table->horizontalHeader()->setStretchLastSection(tableConfig["stretchLastSection"].toBool());
            table->verticalHeader()->setDefaultSectionSize(tableConfig["verticalSectionSize"].toInt());
            table->verticalHeader()->setVisible(false);
            table->setSelectionMode(QAbstractItemView::ExtendedSelection);
            table->setSelectionBehavior(QAbstractItemView::SelectRows);
            table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    }

    //数据库数据处理优化
    QVector<QVariantList> hospitalInfo;
    FullyAutomatedPlatelets::pinstancesqlData()->SeekHospitalInfo(hospitalInfo);

    //使用映射表避免重复分割
    QMap<QString, QVector<QVariantList>> categoryMap;
    for (const QVariantList &record : qAsConst(hospitalInfo)) {
        if (record.size() < 3) continue;

        const QString key = record[0].toString().section('_', 0, 0); // 比split更高效
        categoryMap[key].append(record);
    }

    //表格填充优化
    for (int i = 0; i < m_CategoryList.size(); ++i) {
        const QString category = m_CategoryList[i];
        if (!categoryMap.contains(category)) continue;

        for (const QVariantList &record : categoryMap[category]) {
            if (record.size() < 3) continue;
            const QString data = QString("%1|%2").arg(record[1].toString(), record[2].toString());
            InitTabTwoDate(i, data);
        }
    }
}


void MachineSetting::InitTabTwoDate(int tabnum ,QString str)
{
    QStringList temp_strlist;
    temp_strlist.clear();
    int pos = str.indexOf("|");
    QString tempdata = str.left(pos);
    QString tempdata_1 =str.right(str.length()-pos-1);
    temp_strlist<<tempdata<<tempdata_1;
    int rows = temp_strlist.count()/2;
    for(int i = 0 ; i< rows ;i++)
    {
        int Total_Row = m_tableList.at(tabnum)->rowCount();
        m_tableList.at(tabnum)->insertRow(Total_Row);
        QString temp_str = temp_strlist.at(2*i);
        QString temp_str_1= temp_strlist.at(2*i+1);
        m_tableList.at(tabnum)->setItem(Total_Row,0,new QTableWidgetItem(QString(temp_str)));
        m_tableList.at(tabnum)->item(Total_Row,0)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
        m_tableList.at(tabnum)->setItem(Total_Row,1,new QTableWidgetItem(QString(temp_str_1)));
        m_tableList.at(tabnum)->item(Total_Row,1)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
    }
    return;
}

//++信息
void showWarningDialog(const QString &message) {
    QMessageBox::warning(nullptr, "添加异常", message, QMessageBox::Ok);
}
void MachineSetting::on_toolButton_ADD_clicked()
{
    QComboBox* Pcommbox = ui->comboBox_Category;

    QLineEdit *pLineEditAbbreviation =  ui->lineEdit_Abbreviation;
    QLineEdit *pLineEditName =  ui->lineEdit_Name;

    const QString abberciationText = pLineEditAbbreviation->text().trimmed();
    const QString nameText = pLineEditName->text().trimmed();
    if (abberciationText.isEmpty()) {
        showWarningDialog(tr("缩略字符为空!"));  // 封装弹窗函数
        return;
    }
    if (nameText.isEmpty()) {
        showWarningDialog(tr("名称为空!"));
        return;
    }

    const int indexSel = Pcommbox->currentIndex();
	const QString keySer = Pcommbox->currentText();

    QString Displaytext = QString("%1|%2").arg(abberciationText).arg(nameText);
    InitTabTwoDate(indexSel,Displaytext);

    QString Type = QString("%1_%2").arg(keySer).arg(abberciationText);
    FullyAutomatedPlatelets::pinstancesqlData()->InsertInfoHospital(Type,abberciationText,nameText);

    pLineEditAbbreviation->clear();
    pLineEditName->clear();
    ui->toolButton_ADD->setEnabled(false);
    ui->toolButton_Delete->setEnabled(true);
    return;
}

//--删除
void MachineSetting::on_toolButton_Delete_clicked()
{
    QComboBox* Pcommbox = ui->comboBox_Category;
    const int curindex = Pcommbox->currentIndex();

    int selectrow = m_tableList.at(curindex)->currentRow();
    if (selectrow < 0){
        showWarningDialog(tr("删除选中为空!"));
        return;
    }
    QString Kind = m_tableList.at(curindex)->item(selectrow,0)->text();
    m_tableList.at(curindex)->removeRow(selectrow);

    QString Type = QString("%1_%2").arg(curindex).arg(Kind);
    FullyAutomatedPlatelets::pinstancesqlData()->DelInfoHospital(Type);

    ui->toolButton_Delete->setEnabled(true);
    ui->toolButton_ADD->setEnabled(false);
    return;
}


void  InsertRow(QTableWidget *QualitycontrolTable , const int iRow,const int Col, const QString Value)
{
   QTableWidgetItem *item = new QTableWidgetItem(Value);
   item->setTextAlignment(Qt::AlignCenter);
   item->setTextColor(QColor(0 ,0 ,0));
   item->setFont(QFont("song", 14));
   QualitycontrolTable->setItem(iRow, Col, item); //添加到界面
   return;
}


bool CheckBoxListSort(const QCheckBox* box_1, const QCheckBox* box_2)
{
    QString num_statr = box_1->text();
    QString num_end  =  box_2->text();
    QRegExp rx("\\d+");
    QString ifinded;
    int data_d1,data_d2;
    rx.indexIn(num_statr,0);
    ifinded = rx.cap(0);
    data_d1 = ifinded.toInt();
    rx.indexIn(num_end,0);
    ifinded = rx.cap(0);
    data_d2 = ifinded.toInt();
    return data_d1 < data_d2; //升序
}




//设置提示文字字体颜色
void MachineSetting::LabelTextColors(QLabel *ptextReminder,QColor color,QString reminderStr)
{
    QFont font;
    font.setFamily("楷体");
    font.setPixelSize(30);
    QPalette pe;
    pe.setColor(QPalette::WindowText,color);
    ptextReminder->show();
    ptextReminder->setPalette(pe);
    ptextReminder->setFont(font);
    ptextReminder->setText(reminderStr);
}

void MachineSetting::CheckSelect(QTreeWidgetItem*item ,int initem ){
    if(!item )  return;

    // 定义配置项映射表
    static const QMap<QString, int> rootMap = {
        {tr("LIS"), MACHINECONFIGURE_LIS},
        {tr("质控"), MACHINE_QUALITYCONTROL},
        {tr("性能验证"), MACHINE_PE}
    };

    static const QMap<QString, QMap<int, int>> childMap = {
        {tr("通用"), {
            {MACHINECONFIGURE_COMMON, MACHINECONFIGURE_COMMON},
            {MACHINECONFIGURE_HOSPITALINFO, MACHINECONFIGURE_HOSPITALINFO},
            {MACHINECONFIGURE, MACHINECONFIGURE},
            {MACHINE_MAINTENANCE, MACHINE_MAINTENANCE_INDEX}
        }},
        {tr("测量"), {
            {0, MACHINECONFIGURE_CHANNELVALUE},
            {1, MACHINECONFIGURE_REFERENCE}
        }}
    };

    QTreeWidgetItem* parent = item->parent();
    if (!parent) {
        // 处理根节点
        const QString rootText = item->text(0);
        if (rootMap.contains(rootText)) {
            const int tabIndex = rootMap.value(rootText);
            ui->tabWidgetSetconfigure->setCurrentIndex(tabIndex);
            if (tabIndex == MACHINE_PE) {
                loadPerformanceEvaluation(ui->tabPE);
            }
        }
    }
    else {
        // 处理子节点
        const QString parentText = parent->text(initem);
        const int childIndex = parent->indexOfChild(item);

        if (childMap.contains(parentText)) {
            const QMap<int, int>& tabMapping = childMap[parentText];
            if (tabMapping.contains(childIndex)) {
                const int tabIndex = tabMapping[childIndex];
                ui->tabWidgetSetconfigure->setCurrentIndex(tabIndex);

                // 设置焦点控制
                static const QHash<int, QLineEdit*> focusMap = {
                    {MACHINECONFIGURE_HOSPITALINFO, ui->lineEdit_Abbreviation},
                    {MACHINE_MAINTENANCE, ui->lineEdit_maintenance}
                };
                if (focusMap.contains(tabIndex)) {
                    focusMap[tabIndex]->setFocus(Qt::TabFocusReason);
                }
            }
        }
    }
}






void MachineSetting::on_comboBox_Category_currentIndexChanged(int index)
{
    ui->tabWidget_info->setCurrentIndex(index);
    m_Info_Index = index;
    return;
}


void MachineSetting::on_lineEdit_Name_textChanged(const QString )
{
    QString temp_0 = ui->lineEdit_Abbreviation->text();
    QString temp =ui->lineEdit_Name->text();
    if(!temp.isEmpty() && !temp_0.isEmpty())
    {
        ui->toolButton_ADD->setEnabled(true);
    }
}
void MachineSetting::on_lineEdit_Abbreviation_textChanged(const QString)
{
    QString temp_0 = ui->lineEdit_Abbreviation->text();
    QString temp =ui->lineEdit_Name->text();
    if(!temp.isEmpty() && !temp_0.isEmpty())
    {
        ui->toolButton_ADD->setEnabled(true);
    }

}



void MachineSetting::AnalyticalJsonTwo(QStringList templist)
{
    QString temp_str;
    int nSize = templist.size();
    for(int i =0 ;i<nSize ; i++)
    {
        m_AnalyticallistData.clear();
        QString objname = templist.at(i);
        m_Analytical.AnalyticalJsonFile("通用信息",objname,m_AnalyticallistData,m_Analytical.JsonFilePath(1));
        for(int j =0; j<m_AnalyticallistData.size() ;j++)
        {
            temp_str = m_AnalyticallistData.at(j).toString();
            InitTabTwoDate(i,temp_str);
        }
    }
}


void MachineSetting::on_toolButton_Cancel_clicked()
{
    quint8 index_equipment = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,index_equipment);
    ImportParameters(index_equipment);
}

//配置添加患者科目
void MachineSetting::on_toolButton_viewSQL_clicked()
{
     emit Synchronizeupdates();
     return;
}


void MachineSetting::ImportParameters(const quint8 equipmentIndex)
{
    initSettinggeneralParameters();

    ReagentConfigParaSetOrGet(false); //初始化参数部分

    initdisplaymoduleChn(equipmentIndex); //初始化显示模组通道&&模组温度


    return;
}

void MachineSetting::updatepara(const bool isTesting){
    auto* consumables = ConsumablesOper::GetpInstance();

    // 更新试剂容量
    for (int i = 0; i < m_capactityList.size(); ++i) {
        quint16 updatevalue = 0;
        consumables->updateReagentTotal(READ_OPERRAT, i, updatevalue);
		if (m_capactityList[i]->value() != updatevalue) {
			m_capactityList[i]->setValue(updatevalue);
		}
    }

    // 更新限值比率及设备配置
    for (int i = 0; i < m_limitratioList.size(); ++i) {
        quint8 updatevalue = 0;
        consumables->updateReagentLimit(READ_OPERRAT, i, updatevalue);
		// Only update if the value has changed
		if (m_limitratioList[i]->value() != updatevalue) {
			m_limitratioList[i]->setValue(updatevalue);
            FullyAutomatedPlatelets::pinstanceinstrument()->configwarmvalue(i, updatevalue);
		}
    }

    QList<QCheckBox*> pcontrolChannelList = ui->usechannel->findChildren<QCheckBox*>();
    for(auto channelbox : pcontrolChannelList){
        bool letChangeState = (isTesting)? false : true;
        channelbox->setEnabled(letChangeState);
    }
    update();
}





//*/void  MachineSetting::configWrite2BoardReagentNeedlePara(){
//    auto &ini = INI_File();
//    const int REAGENT_COUNT = 5;
//    quint8 reagentTypes[REAGENT_COUNT] = {
//        AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
//    };

//    quint8 suckReagentVol[REAGENT_COUNT] = {
//        static_cast<quint8>(ui->spinBox_AA_Suck_Vol->value()),
//        static_cast<quint8>(ui->spinBox_ADP_Suck_Vol->value()),
//        static_cast<quint8>(ui->spinBox_ADR_Suck_Vol->value()),
//        static_cast<quint8>(ui->spinBox_COL_Suck_Vol->value()),
//        static_cast<quint8>(ui->spinBox_RIS_Suck_Vol->value())
//    };

//    double suckReagentVolRatio[REAGENT_COUNT] = {
//        static_cast<double>(ui->doubleSpinBox_AA_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_ADP_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_ADR_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_COL_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_RIS_Ratio->value())
//    };

//    double suckReagentVolRatioadd[REAGENT_COUNT] = {
//        static_cast<double>(ui->doubleSpinBox_Add_AA_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_Add_ADP_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_Add_ADR_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_Add_COL_Ratio->value()),
//        static_cast<double>(ui->doubleSpinBox_Add_RIS_Ratio->value())
//    };

//    quint8 spitReagentDownHeigh[REAGENT_COUNT] = {
//        static_cast<quint8>(ui->spinBox_down_AA->value()),
//        static_cast<quint8>(ui->spinBox_down_ADP->value()),
//        static_cast<quint8>(ui->spinBox_down_ADR->value()),
//        static_cast<quint8>(ui->spinBox_down_COL->value()),
//        static_cast<quint8>(ui->spinBox_down_RIS->value())
//    };

//    int size = qMin(m_capactityList.size(), m_limitratioList.size()); // 取最小size防止越界

//    for (int i = 0; i < REAGENT_COUNT; ++i) {
//        ini.setTypesReagentSuckVolume(reagentTypes[i],suckReagentVol[i]); //配置吸试剂的量
//        ini.setTypesReagentSuckRatio(reagentTypes[i],suckReagentVolRatio[i]); //配置吸试剂比例系数
//        ini.setTypesReagentSuckAdd_Ratio(reagentTypes[i],suckReagentVolRatioadd[i]);  //吸试剂吸多余的比例
//        ini.setTypesReagentNeedleDownHigh(reagentTypes[i],spitReagentDownHeigh[i]); //试剂加到测试通道下针高度
//    }

//    for (int i = 0; i < size; ++i) {
//        QSpinBox *capacitySpinBox = m_capactityList.at(i);
//        QSpinBox *limitSpinBox = m_limitratioList.at(i);

//        // 检查指针是否有效
//        if (!capacitySpinBox || !limitSpinBox) {
//            QLOG_WARN() << "Invalid spin box pointer at index:" << i;
//            continue; // 跳过无效项
//        }

//        // 处理容量
//        quint16 capacityValue = capacitySpinBox->value();
//        //QUIUtils::_sycnBottleCapacity(Suppilefile, i, capacityValue);
//        ConsumablesOper::GetpInstance()->updateReagentTotal(WRITE_OPERAT, i, capacityValue);

//        // 处理限位
//        quint8 limitValue = limitSpinBox->value();
//        //QUIUtils::_sycnBottleLimit(Suppilefile, i, limitValue);
//        ConsumablesOper::GetpInstance()->updateReagentLimit(WRITE_OPERAT, i, limitValue);
//        FullyAutomatedPlatelets::pinstanceinstrument()->configwarmvalue(i, limitValue);
//    }
//    if (m_capactityList.size() != m_limitratioList.size()) {
//        QLOG_WARN() << "Warning: capacity list size (" << m_capactityList.size()
//                   << ") does not match limit ratio list size (" << m_limitratioList.size() << ")";
//    }

//    //耗材 容量、限位写入仪器
//    QByteArray capacityarry,limitarry;
//    QUIUtils::_writeReagentCapacity(capacityarry);
//    QUIUtils::_writeReagentLimitOrder(limitarry);
//    emit SetParatoInstrument({capacityarry,limitarry},"配置试剂容量,限位");

//    //写入修改试剂针参数到主板
//    notifyReagentPinParaToBoard();
//    FullyAutomatedPlatelets::pinstanceinstrument()->ShowConsumablesLimitArm();
//    QMessageBox::information(this,tr("保存完成"),tr("仪器血样针参数设置完成!"));
//    return;
//}*/


void MachineSetting::configWrite2BoardReagentNeedlePara() {

    auto &ini = INI_File();
    // 1. 使用 constexpr 定义编译期常量
    static constexpr int REAGENT_COUNT = 5;

    // 2. 使用 std::array 替代原生数组，更现代更安全
    const std::array<quint8, REAGENT_COUNT> reagentTypes = {
        AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
    };

    // 3. 提取获取UI值的逻辑，消除重复代码
    const auto suckReagentVol = getUiValues<QSpinBox, quint8>({
        ui->spinBox_AA_Suck_Vol,
        ui->spinBox_ADP_Suck_Vol,
        ui->spinBox_ADR_Suck_Vol,
        ui->spinBox_COL_Suck_Vol,
        ui->spinBox_RIS_Suck_Vol
    });

    const auto suckReagentVolRatio = getUiValues<QDoubleSpinBox, double>({
        ui->doubleSpinBox_AA_Ratio,
        ui->doubleSpinBox_ADP_Ratio,
        ui->doubleSpinBox_ADR_Ratio,
        ui->doubleSpinBox_COL_Ratio,
        ui->doubleSpinBox_RIS_Ratio
    });

    const auto suckReagentVolRatioadd = getUiValues<QDoubleSpinBox, double>({
        ui->doubleSpinBox_Add_AA_Ratio,
        ui->doubleSpinBox_Add_ADP_Ratio,
        ui->doubleSpinBox_Add_ADR_Ratio,
        ui->doubleSpinBox_Add_COL_Ratio,
        ui->doubleSpinBox_Add_RIS_Ratio
    });

    const auto spitReagentDownHeigh = getUiValues<QSpinBox, quint8>({
        ui->spinBox_down_AA,
        ui->spinBox_down_ADP,
        ui->spinBox_down_ADR,
        ui->spinBox_down_COL,
        ui->spinBox_down_RIS
    });

    // 4. 配置试剂参数（逻辑更清晰）
    for (int i = 0; i < REAGENT_COUNT; ++i) {
        ini.setTypesReagentSuckVolume(reagentTypes[i], suckReagentVol[i]);
        ini.setTypesReagentSuckRatio(reagentTypes[i], suckReagentVolRatio[i]);
        ini.setTypesReagentSuckAdd_Ratio(reagentTypes[i], suckReagentVolRatioadd[i]);
        ini.setTypesReagentNeedleDownHigh(reagentTypes[i], spitReagentDownHeigh[i]);
    }

    // 5. 更新耗材容量和限位（提取成独立函数）
    updateReagentCapacityAndLimit();

    // 6. 耗材容量、限位写入仪器
    writeConsumablesToInstrument();

    // 7. 写入修改试剂针参数到主板
    notifyReagentPinParaToBoard();
    FullyAutomatedPlatelets::pinstanceinstrument()->ShowConsumablesLimitArm();

    QMessageBox::information(this, tr("保存完成"), tr("仪器血样针参数设置完成!"));
}


// 更新试剂容量和限位的独立函数
void MachineSetting::updateReagentCapacityAndLimit() {
    const int size = qMin(m_capactityList.size(), m_limitratioList.size());

    const QString Suppilefile = QCoreApplication::applicationDirPath() + "/consumables.ini";
    QFile filePara(Suppilefile);
    bool bexit = filePara.exists();
    if(!bexit){
       filePara.open(QIODevice::Append);
       QLOG_WARN()<<"耗材配置文件不存在,创建"<<__FILE__<<__LINE__<<endl;
    }

    for (int i = 0; i < size; ++i) {
        if (QSpinBox *capacitySpinBox = m_capactityList.value(i)) {
			quint16 capacityVol = capacitySpinBox->value();
            QUIUtils::sycnBottleCapacity(Suppilefile, i, capacityVol);
            ConsumablesOper::GetpInstance()->updateReagentTotal(
                WRITE_OPERAT, i, capacityVol);
        }

        if (QSpinBox *limitSpinBox = m_limitratioList.value(i)) {
            quint8 limitValue = limitSpinBox->value();
            QUIUtils::sycnBottleLimit(Suppilefile, i, limitValue);
            ConsumablesOper::GetpInstance()->updateReagentLimit(
                WRITE_OPERAT, i, limitValue);
            FullyAutomatedPlatelets::pinstanceinstrument()->configwarmvalue(i, limitValue);
        }
    }

    if (m_capactityList.size() != m_limitratioList.size()) {
        QLOG_WARN() << "List size mismatch. Capacity:" << m_capactityList.size()
                   << "Limit:" << m_limitratioList.size();
    }
}

// 写入耗材到仪器的独立函数
void MachineSetting::writeConsumablesToInstrument() {
    QByteArray capacityArray, limitArray;
    QUIUtils::_writeReagentCapacity(capacityArray);
    QUIUtils::_writeReagentLimitOrder(limitArray);
    emit SetParatoInstrument({capacityArray, limitArray}, "配置试剂容量,限位");
}



void  MachineSetting::initBoardReagentNeedlePara(){
    auto &ini = INI_File();
    const int REAGENT_COUNT = 5;
    quint8 reagentTypes[REAGENT_COUNT] = {
        AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
    };

    const std::array<QSpinBox*, REAGENT_COUNT> suckVolSpinBoxes = {
            ui->spinBox_AA_Suck_Vol,
            ui->spinBox_ADP_Suck_Vol,
            ui->spinBox_ADR_Suck_Vol,
            ui->spinBox_COL_Suck_Vol,
            ui->spinBox_RIS_Suck_Vol
    };

    const std::array<QDoubleSpinBox*, REAGENT_COUNT> ratioSpinBoxes = {
        ui->doubleSpinBox_AA_Ratio,
        ui->doubleSpinBox_ADP_Ratio,
        ui->doubleSpinBox_ADR_Ratio,
        ui->doubleSpinBox_COL_Ratio,
        ui->doubleSpinBox_RIS_Ratio
    };

    const std::array<QDoubleSpinBox*, REAGENT_COUNT> addRatioSpinBoxes = {
        ui->doubleSpinBox_Add_AA_Ratio,
        ui->doubleSpinBox_Add_ADP_Ratio,
        ui->doubleSpinBox_Add_ADR_Ratio,
        ui->doubleSpinBox_Add_COL_Ratio,
        ui->doubleSpinBox_Add_RIS_Ratio
    };

    const std::array<QSpinBox*, REAGENT_COUNT> downHeightSpinBoxes = {
        ui->spinBox_down_AA,
        ui->spinBox_down_ADP,
        ui->spinBox_down_ADR,
        ui->spinBox_down_COL,
        ui->spinBox_down_RIS
    };

    const std::array<QLabel*, REAGENT_COUNT> showLabels = {
        ui->label_show_AA,
        ui->label_show_ADP,
        ui->label_show_ADR,
        ui->label_show_COL,
        ui->label_show_RIS
    };

    const std::array<int, REAGENT_COUNT> downHeightParams = {
        REAGPIN_DOWNHEIGH_IN_AA,
        REAGPIN_DOWNHEIGH_IN_ADP,
        REAGPIN_DOWNHEIGH_IN_EPI,
        REAGPIN_DOWNHEIGH_IN_COL,
        REAGPIN_DOWNHEIGH_IN_RIS
    };


    int size = qMin(m_capactityList.size(), m_limitratioList.size()); // 取最小size防止越界

    // 配置吸试剂的量
    for (int i = 0; i < REAGENT_COUNT; ++i) {
        if (suckVolSpinBoxes[i]) {
            suckVolSpinBoxes[i]->setValue(ini.getTypesReagentSuckVolume(reagentTypes[i]));
        }
    }

    // 配置吸试剂比例系数
    for (int i = 0; i < REAGENT_COUNT; ++i) {
        if (ratioSpinBoxes[i]) {
            ratioSpinBoxes[i]->setValue(ini.getTypesReagentSuckRatio(reagentTypes[i]));
        }
    }

    // 吸试剂吸多余的增益比例
    for (int i = 0; i < REAGENT_COUNT; ++i) {
        if (addRatioSpinBoxes[i]) {
            addRatioSpinBoxes[i]->setValue(ini.getTypesReagentSuckAdd_Ratio(reagentTypes[i]));
        }
    }

    // 试剂加到测试通道下针高度
    for (int i = 0; i < REAGENT_COUNT; ++i) {
        if (downHeightSpinBoxes[i]) {
            downHeightSpinBoxes[i]->setValue(ini.getTypesReagentNeedleDownHigh(reagentTypes[i]));
        }
        if (showLabels[i] && downHeightSpinBoxes[i]) {
            _creatstupoint(showLabels[i], downHeightSpinBoxes[i], downHeightParams[i], m_preagpinLableList);
        }
    }

    if (m_capactityList.size() != m_limitratioList.size()) {
        QLOG_WARN() << "List size mismatch. Capacity:" << m_capactityList.size()
                   << "Limit:" << m_limitratioList.size();
    }

    // 试剂容量 && 试剂限位
    for (int k = 0; k < size; ++k) {
        quint16 value_capacity = 0;
        quint8 value_limit = 0;

        ConsumablesOper::GetpInstance()->updateReagentTotal(READ_OPERRAT, k, value_capacity);
        if (QSpinBox* spinBox = m_capactityList.value(k)) {
            spinBox->setValue(value_capacity);
        }

        ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT, k, value_limit);
        if (QSpinBox* spinBox = m_limitratioList.value(k)) {
            spinBox->setValue(value_limit);
        }
    }

}


void  MachineSetting::ReagentConfigParaSetOrGet(const bool bsetConfig)
{
    if(bsetConfig)
    {
        configWrite2BoardReagentNeedlePara();
    }
    else
    {
        initBoardReagentNeedlePara();
    }
    return;
}

void MachineSetting::initdisplaymoduleChn( const quint8 equipmentIndex)
{
    bool binitactiv = false;
    auto &ini = INI_File();

	QDoubleSpinBox *pmodulTempA = ui->doubleSpinBox_Moduletemperature_1;
	QDoubleSpinBox *pmodulTempB = ui->doubleSpinBox_Moduletemperature_2;
	QDoubleSpinBox *pmodulTempC = ui->doubleSpinBox_Moduletemperature_3;

    //模组通道
    QList<QCheckBox*> QCheckBoxList = ui->usechannel->findChildren<QCheckBox*>();
    std::sort(QCheckBoxList.begin(), QCheckBoxList.end(), subDevListSort);
    for(int index = 0; index < QCheckBoxList.size(); index++)
    {
        int channelIndex = index + 1;
        QString ChnKeyData = QString("TestTheChanne1Opening_%1").arg(channelIndex);
        const bool bCheckUsed = ini.rConfigPara(ChnKeyData).toBool();
        QCheckBoxList.at(index)->setChecked(bCheckUsed);

        if(equipmentIndex == KS600)
        {
            if(index >= 4)
            {
                ini.wConfigPara(ChnKeyData,false);
                QCheckBoxList.at(index)->setChecked(false);
                QCheckBoxList.at(index)->hide();
            }
            if(binitactiv == false)
            {
				pmodulTempA->setValue(ini.rConfigPara(QString("ModuleTemperature%1").arg(MODULE_1)).toDouble());
				pmodulTempB->hide();
                pmodulTempC->hide();
                ui->label_Moduletemperature_2->hide();
                ui->label_Moduletemperature_3->hide();
                binitactiv = true;
            }
        }
        else if(equipmentIndex == KS800)
        {
            if(index >= 8){
                ini.wConfigPara(ChnKeyData,false);
                QCheckBoxList.at(index)->setChecked(false);
                QCheckBoxList.at(index)->hide();
            }
            if(binitactiv == false)
            {
				pmodulTempA->setValue(ini.rConfigPara(QString("ModuleTemperature%1").arg(MODULE_1)).toDouble());
				pmodulTempB->setValue(ini.rConfigPara(QString("ModuleTemperature%1").arg(MODULE_2)).toDouble());
				pmodulTempC->hide();
                ui->label_Moduletemperature_3->hide();
                binitactiv = true;
            }
        }
        else if(equipmentIndex == KS1200)
        {
            if(binitactiv == false)
            {
				pmodulTempA->setValue(ini.rConfigPara(QString("ModuleTemperature%1").arg(MODULE_1)).toDouble());
				pmodulTempB->setValue(ini.rConfigPara(QString("ModuleTemperature%1").arg(MODULE_2)).toDouble());
				pmodulTempC->setValue(ini.rConfigPara(QString("ModuleTemperature%1").arg(MODULE_3)).toDouble());
                binitactiv = true;
            }
        }
    }
    return;
}


/*机器配置基础参数写到板子里*/
void MachineSetting::sendmainbordBasicPara()
{
    QString loadUser = cglobal::g_UserName_str;
    bool supVip = false;
    FullyAutomatedPlatelets::pinstancesqlData()->SelectUserPermissionsControl(loadUser,supVip); /*用户权限控制*/
    if(!supVip)
    {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("权限控制","用户权限不足,保存失败!");
        ImportParameters(INI_File().getInstrumentType());
        return;
    }


	bool scanbar = ui->UseBarCode->isChecked();
	bool rightReagent = ui->checkBox_UesSecondReagentHole->isChecked();
	bool initcatchcups = ui->checkBox_catchcups->isChecked();
    QVariantMap configs = {
            {FIRSTSUCKAIRS, ui->checkBox_suck_offset->isChecked()}, //吸样校准
            {SCANCODEBAR, scanbar }, //使用条形码
            {USINGASIDEREAGENTLOC, rightReagent },//启用预留试剂位
            {INITGRABCUPS, initcatchcups } //开机初始化抓通道内杯子

    };
    INI_File().wBatchConfigPara(configs);


    QByteArray sycnmainboardArry;
    quint16 ModuleData[3] = {0};
    ModuleData[0] = ui->doubleSpinBox_Moduletemperature_1->value()/TEMP_CONVERSION_RATIO;
    ModuleData[1] = ui->doubleSpinBox_Moduletemperature_2->value()/TEMP_CONVERSION_RATIO;
    ModuleData[2] = ui->doubleSpinBox_Moduletemperature_3->value()/TEMP_CONVERSION_RATIO;

    bool ChnState[12] = {false};
    QList<QCheckBox*> checkbox_chn_list = ui->usechannel->findChildren<QCheckBox*>();
    std::sort(checkbox_chn_list.begin(), checkbox_chn_list.end(), subDevListSort);
    for(auto channelbox : checkbox_chn_list)
    {
        int index_chn = QUIUtils::StringFindintnum(channelbox->objectName());
        ChnState[index_chn - 1] = channelbox->isChecked();
    }

    quint16 disused = 0;
    QUIUtils::_writeParaNumIOrder(sycnmainboardArry,ModuleData,ChnState, scanbar, rightReagent, initcatchcups, disused);

    QByteArrayList sendList;
    sendList.push_back(sycnmainboardArry);
    emit SetParatoInstrument(sendList,"基础参数写入主板");
    QMessageBox::information(this,tr("保存完成"),tr("仪器基础参数设置完成!"));
    return;
}

void MachineSetting::HandsParaWriteBoard()
{
    bool brebackCatch = ui->checkBox_Recapture->isChecked();
    INI_File().wConfigPara(REPEATGRABCUP,brebackCatch);


    quint8 throwdownmm = ui->spinBox_downthrowcups->value();

    //小于等于MAX 大余等于MIn
    QVariantMap configs = {
            {GRIPPERLESSTHANMAX, ui->spinBoxSuckAirsMax->value()},
            {GRIPPERBIGTHANMIM, ui->spinBoxSplitAirsMin->value()},
            {GRIPPERSUCKTIME, ui->spinBox_suckLatetimer->value()}
    };
    INI_File().wBatchConfigPara(configs);

    quint8 handsinierTray[4] ={0};
    handsinierTray[0] =  ui->spinBox_testtray_0->value();
    handsinierTray[1] =  ui->spinBox_testtray_1->value();
    handsinierTray[2] =  ui->spinBox_testtray_2->value();
    handsinierTray[3] =  ui->spinBox_testtray_3->value();

    quint8 HandsinnerModule[3] ={0};
    HandsinnerModule[0] = ui->spinBox_handsdownchn_0->value();
    HandsinnerModule[1] = ui->spinBox_handsdownchn_1->value();
    HandsinnerModule[2] = ui->spinBox_handsdownchn_2->value();

    QByteArray buffer;
    QUIUtils::_writeParaNumIIHandsOrder(buffer,throwdownmm,handsinierTray,HandsinnerModule, brebackCatch);

    QByteArray GripperContl = outParaOx1aWrite();
    emit SetParatoInstrument({ buffer, GripperContl },"抓手参数写入主板");
    return;
}



QByteArray MachineSetting::outParaOx1aWrite(){
    quint8 FilteringMode = static_cast<quint8>(INI_File().getFilteringMode());
    return QUIUtils::writeGripperParaDataArry(ui->spinBoxSuckAirsMax->value(),
                                        ui->spinBoxSplitAirsMin->value(),
                                        ui->spinBox_suckLatetimer->value(),
                                        FilteringMode,
                                        ui->checkBox_absorbance->isChecked(),
                                        ui->checkBoxExperimental->isChecked(),false);
}


void MachineSetting::configWriteDimming2Board(){
    auto& ini = INI_File();
    const QByteArray arryData = QUIUtils::writeModuleDimmingVal0x1b(
           static_cast<quint16>(ini.getModuledimmingVal(MODULE_1)),
           static_cast<quint16>(ini.getModuledimmingVal(MODULE_2)),
           static_cast<quint16>(ini.getModuledimmingVal(MODULE_3)),
           false
    );
    emit SetParatoInstrument({ arryData },"模组调光值写入主板");
}




void MachineSetting::BloodPinParaWriteBoadr()
{
    BloodPinParams params;

    params.paramIndex4 = {
		static_cast<quint16>(ui->spinBoxAbsorbX2->value()) ,
		static_cast<quint16>(ui->poorBlood_changliang->value()),
		static_cast<quint16>(ui->spinBox_suckairs->value()),
        static_cast<quint32>(ui->spinBox_WashesTime->value())

    };

    double max_value = 400;
    //应用四舍五入（避免浮点误差）
    const double dataheighOffset = ui->OffsetTestHeightValue->value() * 10.0;
    const quint8 safeValue = static_cast<quint8>(std::round(dataheighOffset));

    const double pppratio = ui->doubleSpinBox_Ratio_ben->value()*100.0 ;
    const quint8 pppValue = static_cast<quint8>((pppratio/max_value) * 255);

    const double prpratio = ui->doubleSpinBox_PRPratio->value() * 100.00;
    const quint8 prpValue = static_cast<quint8>((prpratio/max_value) * 255);

    quint8 prep = 0;

    params.paramIndex16 ={
        static_cast<quint8>(ui->EmptyHeigh->value()),
        static_cast<quint8>(pppValue),
        static_cast<quint8>(ui->SecurityValue_box->value()),
        static_cast<quint8>(ui->FixedHighvalue->value()),
        static_cast<quint8>(ui->spinBox_CleanLinqueFailedHigh->value()),
        static_cast<quint8>(ui->Testheighdownheigh->value()),
        static_cast<quint8>(ui->spinBox_faliedlinque->value()),
        static_cast<quint8>(prpValue),
        static_cast<quint8>(safeValue),
        static_cast<quint8>(prep)
    };

    // 数据打包
    QByteArray bufferq16;
    QUIUtils::_writeParaNumBloodOrder(bufferq16,
            params.paramIndex4.cleanLinqueblood,
            params.paramIndex4.suckppporprp,
            params.paramIndex4.suckairs,
            params.paramIndex4.washesTime,
            false);

    QByteArray bufferq8;
    quint8 bloodpara[10] = {
            params.paramIndex16.emptyHeight,
            params.paramIndex16.ratioBen,
            params.paramIndex16.securityValue,
            params.paramIndex16.fixedHighValue,
            params.paramIndex16.cleanFailedHigh,
            params.paramIndex16.testHeightDown,
            params.paramIndex16.failedLinque,
            params.paramIndex16.prpRatio,
            params.paramIndex16.testHeightOffset,
            params.paramIndex16.prep
    };
    QUIUtils::_writeParaBloodOtherOrder(bufferq8, bloodpara);

    // 统一发送
    emit SetParatoInstrument({bufferq16, bufferq8}, "血样针参数写入主板");

    return;
}






void MachineSetting::notifyReagentPinParaToBoard()
{
    auto &ini = INI_File();

    // 使用原生数组替代std::array
    const int REAGENT_COUNT = 5;
    quint8 reagentTypes[REAGENT_COUNT] = {
        AA_REAGENT, ADP_REAGENT, EPI_REAGENT, COL_REAGENT, RIS_REAGENT
    };

    // 初始化数组
    quint8 ReagentPinSuckum[REAGENT_COUNT] = {0};
    quint8 ReagentDownPinmm[REAGENT_COUNT] = {0};
    quint8 otherReagentData[8] = {0};

    for (int i = 0; i < REAGENT_COUNT; ++i) {
        ReagentPinSuckum[i] = ini.getTypesReagentSuckVolume(reagentTypes[i]);
        ReagentDownPinmm[i] = ini.getTypesReagentNeedleDownHigh(reagentTypes[i]);
    }

    otherReagentData[0] = ini.GetAbsorbWashingfluidX1();
    otherReagentData[1] = ini.getFailedCleanLinqueReagNeedle();
    otherReagentData[2] = ini.GetFailedCleanLinqueHigh();

    for (int i = 0; i < REAGENT_COUNT; ++i) {
        otherReagentData[3 + i] = static_cast<quint8>(ini.getTypesReagentSuckRatio(reagentTypes[i]) * 100.0);
    }

    // 生成命令数据
    QByteArray sendOrder, sendOtherOrder;
    QUIUtils::_writeReagPinParaDataOrder(sendOrder, sendOtherOrder,
                                         ReagentPinSuckum,
                                         ReagentDownPinmm,
                                        otherReagentData);
    emit SetParatoInstrument({sendOrder,sendOtherOrder},"配置试剂针参数");
	return;
}





void MachineSetting::_initconfigmodulefucn()
{

    //控制搅拌电机启停
    connect(ui->toolButton_oopenall,&QToolButton::clicked,this,[=](){
        if(mbopendallchannel == false)
        {
            emit controlallchn(true);
            mbopendallchannel = true;
            ui->toolButton_oopenall->setText("关闭所有转子");
        }
        else
        {
           emit controlallchn(false);
           mbopendallchannel = false;
           ui->toolButton_oopenall->setText("打开所有转子");
        }
    });

}

//导入==将坐标配置文件导入到仪器配置
void MachineSetting::on_toolButton_Import_clicked()
{
    QString file_path = "";
    QString defaultpath = QApplication::applicationDirPath() + "/coordinateFile.txt";//默认路径
    QDir dir(defaultpath);
    if(dir.exists())
    {
        file_path = QFileDialog::getOpenFileName(this,tr("导入坐标文件"),defaultpath,"txt files(*.txt)");
    }
    else
    {
        file_path = QFileDialog::getOpenFileName(this,tr("导入坐标文件"),".../","txt files(*.txt)");
    }
    ui->lineEdit_showPath->setText(file_path);
    if(!file_path.isEmpty())
    {
        SingletonAxis::GetInstance()->importtCoordinate(file_path); //先读文件坐标 再写入到仪器
    }
}


//导出 ==将仪器配置文件导出到自定义路径文本
void MachineSetting::on_toolButton_export_clicked()
{
    QString strPath = QApplication::applicationDirPath() + "/coordinateFile.txt";
    const bool isExport = SingletonAxis::GetInstance()->writeCoordinate(strPath);
    QString stateInfo = (isExport)? "导出坐标文件成功!":"导出坐标文件失败!";
    QMessageBox::about(this,"导出坐标文件",stateInfo);
    return;
}

//选择文件
void MachineSetting::on_pushButton_SelTextOutPath_clicked()
{

}

void MachineSetting::on_pushButton_backsetting_clicked()
{
    QByteArray senddata;
    QUIUtils::clearequipmentpara(senddata);
    QByteArrayList tmp;
    tmp.push_back(senddata);
    emit SetParatoInstrument(tmp, "清空仪器参数");
    QLOG_DEBUG() << "清空仪器参数" << endl;
}

void MachineSetting::disablechn(QVector<quint8> indexchnList)
{
    QVector<quint8> _disableChn;
    foreach (quint8 indexchn_, indexchnList) {
       _disableChn.push_back(indexchn_ + 1);
    }
    QList<QCheckBox*> checkbox_chn_list = ui->usechannel->findChildren<QCheckBox*>();
    std::sort(checkbox_chn_list.begin(), checkbox_chn_list.end(), subDevListSort);
    for(auto channelbox : checkbox_chn_list)
    {
        channelbox->blockSignals(true);
        int index_chn = QUIUtils::StringFindintnum(channelbox->objectName());
        if(_disableChn.contains(index_chn))
        {
            channelbox->setChecked(false);
            QString _printtext = QString::fromUtf8("调光同步设置界面禁用通道%1").arg(index_chn);
            FullyAutomatedPlatelets::pinstanceinfowidget()->InsertText(USERACTIONS,_printtext);
            _disableChn.removeOne(index_chn);
        }
        channelbox->blockSignals(false);
        update();
    }
    return;
}



void MachineSetting::initloginpassword()
{
    //快捷方式
    if (!menterShortcut) {
        menterShortcut =  new QShortcut(QKeySequence(Qt::Key_F4), this);
        connectEnterReturnShortcuts();
    }
	ui->lineEdit_maintenance->setFocus();
    ui->lineEdit_maintenance->setEchoMode(QLineEdit::Password);
    ui->lineEdit_maintenance->setPlaceholderText("请输入维护密码");// 添加输入提示
    ui->tabWidget_config->hide();
    ui->widget_loginpassword->show();

    // 确保按钮信号只连接一次
   static bool isConnected = false;
   if (!isConnected) {
       connect(ui->pushButton_maintenance, &QPushButton::clicked,
               this, &MachineSetting::onMaintenanceButtonClicked);
       isConnected = true;
   }
}

void MachineSetting::connectEnterReturnShortcuts() {
    if (shortcutsConnected) return;

    // 连接F4快捷键
    connect(menterShortcut, &QShortcut::activated,
            ui->pushButton_maintenance, &QPushButton::click);
    shortcutsConnected = true;
}

void MachineSetting::disconnectEnterReturnShortcuts() {
    if (!shortcutsConnected) return;
    disconnect(menterShortcut,  nullptr, nullptr, nullptr);
    shortcutsConnected = false;
}

void MachineSetting::onMaintenanceButtonClicked(){
    const QString inputPassword = ui->lineEdit_maintenance->text();
    const QString storedPassword = ENGINEERPASSWORD;

    if (inputPassword == storedPassword) {
        ui->lineEdit_maintenance->clear();
        ui->lineEdit_maintenance->hide();
        ui->label_maintenance->hide();
        ui->widget_valuediaplay->show();
        ui->widget_valuediaplay->show();
        ui->pushButton_maintenance->setText("退出维护");
        disconnectEnterReturnShortcuts();
        Engineerinterfacelayout(true);  // 统一由该函数管理界面状态
        emit LoginEngineerMode(true);



    } else {
        Engineerinterfacelayout(false);
        emit LoginEngineerMode(false);
		ui->lineEdit_maintenance->setFocus();
        ui->label_maintenance->show();
        ui->lineEdit_maintenance->show();
        ui->lineEdit_maintenance->clear();
        ui->lineEdit_maintenance->setFocus();
        ui->widget_valuediaplay->hide();
        ui->pushButton_maintenance->setText("登入维护(F4)");
        connectEnterReturnShortcuts();
        if (!inputPassword.isEmpty()) {
			QMessageBox::warning(this, "错误", "密码错误");  // 增加错误反馈
			return;
		}    
    }
}
void MachineSetting::Engineerinterfacelayout(bool enterEngineerMode)
{
    ui->tabWidget_config->setVisible(enterEngineerMode);
    if (enterEngineerMode) {
        // 延迟创建动画对象
        if (!m_propertyAnimation) {
            m_propertyAnimation = new QPropertyAnimation(ui->widget_loginpassword, "geometry");
            m_propertyAnimation->setDuration(500);
            m_propertyAnimation->setEasingCurve(QEasingCurve::OutBounce); // 弹跳效果 [[15]][[16]]
        }

        // 动态计算布局尺寸
        const int totalWidth = ui->widget_side->width();
        const int totalHeight = ui->widget_side->height();
        const int loginHeight = 120;
        const int tableHeight = totalHeight - loginHeight;

        // 设置组件尺寸
        ui->widget_loginpassword->setFixedSize(totalWidth, loginHeight);
        ui->tabWidget_config->setFixedSize(totalWidth, tableHeight);

        // 动画起始位置：右下角居中
        m_propertyAnimation->setStartValue(QRect(
            width() - totalWidth / 2,
            height() - loginHeight / 2,
            totalWidth,
            loginHeight
        ));

        // 动画结束位置：底部贴边
        m_propertyAnimation->setEndValue(QRect(
            0,
            tableHeight, // 紧贴参数界面的底部
            totalWidth,
            loginHeight
        ));

        m_propertyAnimation->start();
    }
}




void MachineSetting::connectSetting(QWidget* widget, SettingAction action)
{
    if (auto spinBox = qobject_cast<QSpinBox*>(widget)) {
        connect(spinBox, &QSpinBox::editingFinished, this, action);
    }
    else if (auto doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
        connect(doubleSpinBox, &QDoubleSpinBox::editingFinished, this, action);
    }
}

void MachineSetting::configBloodpinparaSignals()
{
    // 统一配置表：控件指针 + INI设置函数 + 日志信息
    const std::vector<std::tuple<QWidget*, std::function<void()>, QString>> configTable = {
        // 格式: {控件指针, 设置函数, 日志文本}
        std::make_tuple(ui->EmptyHeigh,[=]{
            INI_File().SetEmptyTubeDownHigh(ui->EmptyHeigh->value());
        }, "空试管区下血样针降高度"),

		std::make_tuple(ui->doubleSpinBox_Ratio_ben, [=]{
            INI_File().SetPPPConversionScale(ui->doubleSpinBox_Ratio_ben->value());
        }, ""),

		std::make_tuple(ui->doubleSpinBox_PRPratio, [=]{
            INI_File().setPRPConvertTheratioColumn(ui->doubleSpinBox_PRPratio->value());
        }, ""),

		std::make_tuple(ui->SecurityValue_box, [=]{
            INI_File().SetSecurityValue(ui->SecurityValue_box->value());
        }, "空回值"),

		std::make_tuple(ui->poorBlood_changliang, [=]{
            INI_File().SetLearnSamplevolume(ui->poorBlood_changliang->value());
        }, "样本用量"),

		std::make_tuple(ui->FixedHighvalue, [=]{
            INI_File().SetFixedHigh(ui->FixedHighvalue->value());
        }, "测高物理高度"),

		std::make_tuple(ui->OffsetTestHeightValue, [=]{
            INI_File().SetTestDifference(ui->OffsetTestHeightValue->value());
        }, ""),

		std::make_tuple(ui->spinBoxAbsorbX2, [=]{
            INI_File().SetAbsorbWashingfluidX2(ui->spinBoxAbsorbX2->value());
        }, "洗血样针吸清洗液的量"),

		std::make_tuple(ui->spinBox_WashesTime, [=]{
            INI_File().setWashesTime(ui->spinBox_WashesTime->value());
        }, "清洗时间"),

		std::make_tuple(ui->spinBox_CleanLinqueFailedHigh, [=]{
            INI_File().SetFailedCleanLinqueHigh(ui->spinBox_CleanLinqueFailedHigh->value());
        }, "液面探测失败下降高度(清洗剂)血样针"),

		std::make_tuple(ui->Testheighdownheigh, [=]{
            INI_File().SetAbsorbTubeBottom(ui->Testheighdownheigh->value());
        }, "血浆模式下针高度"),

		std::make_tuple(ui->spinBox_faliedlinque, [=]{
            INI_File().SetFailedLinqueHigh(ui->spinBox_faliedlinque->value());
        }, "液面探测失败高度(贫血)"),

		std::make_tuple(ui->spinBox_suckairs, [=]{
            INI_File()._setsuckairsuckPRP(ui->spinBox_suckairs->value());
        }, "吸富血前吸空气量:")
    };

	// 使用std::get替代结构化绑定
    for (const auto& item : configTable) {
        QWidget* widget = std::get<0>(item);
        auto action = std::get<1>(item);
        QString logText = std::get<2>(item);

        connectSetting(widget, [=]{
            action();
            if (!logText.isEmpty()) {
                // 类型安全的值获取
                QVariant value;
                if (auto spinBox = qobject_cast<QSpinBox*>(widget)) {
                    value = spinBox->value();
                }
                else if (auto doubleSpinBox = qobject_cast<QDoubleSpinBox*>(widget)) {
                    value = doubleSpinBox->value();
                }

                QLOG_DEBUG() << logText << value.toString();
            }
        });
    }

}



void MachineSetting::_creatstupoint(QLabel* plable,QSpinBox* pgetdownmm,int index_, QMap<QLabel* ,btnLable* > &data_)
{
    btnLable* pstuinfo = new btnLable;
    pstuinfo->plableinfo = plable;
    pstuinfo->pdownmovemm = pgetdownmm;
    pstuinfo->index_ = index_;
    data_.insert(plable,pstuinfo);
}

void MachineSetting::_initBloodpinpara()
{
    //血样针下降高度
    quint8 EmptyTubeDownBloodNeedlehigh = INI_File().GetEmptyTubeDownHigh();
    ui->EmptyHeigh->setValue(EmptyTubeDownBloodNeedlehigh);
    _creatstupoint(ui->label_EmptyTubeHeigh,ui->EmptyHeigh,BLOODPINDOWNHEIGH,m_pbloodLableList);

    /*吸吐样转换比例样本系数*/
    ui->doubleSpinBox_Ratio_ben->setValue(INI_File().GetPPPConversionScale()); //PPP
    ui->doubleSpinBox_PRPratio->setValue(INI_File().getPRPConvertTheratioColumn());//PRP

    //空回值
    quint8 EmptybackValue = INI_File().GetSecurityValue();
    ui->SecurityValue_box->setValue(EmptybackValue);


    //吸血样本的量PPP/PRP
    quint8  bloodSampleAspirated = INI_File().GetLearnSamplevolume();
    ui->poorBlood_changliang->setValue(bloodSampleAspirated);

    double Differencemm =  INI_File().GetTestDifference();
    ui->OffsetTestHeightValue->setValue(Differencemm);

    //物理测高固定高度
    double Physicalheight = INI_File().GetFixedHigh();
    ui->FixedHighvalue->setValue(Physicalheight);


    //清洗  血样针  吸清洗液
    int CleanBloodNeedleLinque = INI_File().GetAbsorbWashingfluidX2();
    ui->spinBoxAbsorbX2->setValue(CleanBloodNeedleLinque);


    //液面探测失败高度 血样针(清洗液)
    quint8 CleanLinqueDetectionFailed = INI_File().GetFailedCleanLinqueHigh();
    ui->spinBox_CleanLinqueFailedHigh->setValue(CleanLinqueDetectionFailed);
    _creatstupoint(ui->label_failedlinque_3,ui->spinBox_CleanLinqueFailedHigh,BLOODPINDOWNHEIGH_CLEANLINQUEFAILED,m_pbloodLableList);


    //血浆模式下针高度
    quint8  Plasmapattern = INI_File().GetAbsorbTubeBottom();
    ui->Testheighdownheigh->setValue(Plasmapattern);
    _creatstupoint(ui->label_3, ui->Testheighdownheigh, BLOODPINDOWNHEIGH_SERUMMODEL, m_pbloodLableList);


    //液面探测失败高度==贫血
    quint8 anemiaDetectionFailed = INI_File().GetFailedLinqueHigh();
    ui->spinBox_faliedlinque->setValue(anemiaDetectionFailed);
    _creatstupoint(ui->label_failedlinque,ui->spinBox_faliedlinque,BLOODPINDOWNHEIGH_ANEMIALINQUEFAILED,m_pbloodLableList);


    //吸贫血前吸空气的量
    int suckairvalue = INI_File()._getsuckairsuckPRP();
    ui->spinBox_suckairs->setValue(suckairvalue);


    update();

    configBloodpinparaSignals();

    auto iter = m_pbloodLableList.begin();
    while(iter != m_pbloodLableList.end())
    {
        iter.key()->installEventFilter(this);
        iter++;
    }

    return;
}


void MachineSetting::_configReagPinparaSignals()
{
    connect(ui->spinBoxAbsorbX1,&QSpinBox::editingFinished,this,[=](){
        INI_File().SetAbsorbWashingfluidX1(ui->spinBoxAbsorbX1->value());
        QLOG_DEBUG()<<"洗试剂针吸清洗液的量"<<ui->spinBoxAbsorbX1->value();
    });

    connect(ui->spinBoxReagentNeedleFailehigh,&QSpinBox::editingFinished,this,[=](){
        INI_File().setFailedCleanLinqueReagNeedle(ui->spinBoxReagentNeedleFailehigh->value());
        QLOG_DEBUG()<<"清洗试剂针失败高度"<<ui->spinBoxReagentNeedleFailehigh->value();
    });

    connect(ui->spinBox_TestReagentFailedhigh,&QSpinBox::editingFinished,this,[=](){
        INI_File().SetFailedReagentsLinqueHigh(ui->spinBox_TestReagentFailedhigh->value());
        QLOG_DEBUG()<<"液面探测失败下降高度(试剂)"<<ui->spinBox_TestReagentFailedhigh->value();
    });

    connect(ui->pushButton_saved,&QPushButton::clicked,this,[=](){
        ReagentConfigParaSetOrGet(true);
    });

    connect(ui->spinBox_deptime,&QSpinBox::editingFinished,this,[=](){
        INI_File()._setdelayedtime(ui->spinBox_deptime->value());
    });
}
void MachineSetting::_initReagPinpara()
{
    //清洗 试剂针   吸取清洗液的量
    int CleanReagentNeedleLinque = INI_File().GetAbsorbWashingfluidX1();
    ui->spinBoxAbsorbX1->setValue(CleanReagentNeedleLinque);

    //液面探测失败高度 试剂针(清洗液)
    quint8  reagneedlefailedhigh = INI_File().getFailedCleanLinqueReagNeedle();
    ui->spinBoxReagentNeedleFailehigh->setValue(reagneedlefailedhigh);
    _creatstupoint(ui->label_failedlinque_2,ui->spinBoxReagentNeedleFailehigh,REAGPIN_CLEANLINQUE_DOWN,m_preagpinLableList);

    //液面探测失败高度 吸(试剂)
    quint8 ReagentLinqueDetectionFailed = INI_File().GetFailedReagentsLinqueHigh();
    ui->spinBox_TestReagentFailedhigh->setValue(ReagentLinqueDetectionFailed);
    _creatstupoint(ui->label_failedhighreagentneedle,ui->spinBox_TestReagentFailedhigh,REAGPIN_REAGLINQUE_FAILEDDOWN,m_preagpinLableList);

    //吐试剂到通道延时
    ui->spinBox_deptime->setValue(INI_File()._getdelayedtime());

    _configReagPinparaSignals();

    auto it = m_preagpinLableList.begin();
    while(it != m_preagpinLableList.end())
    {
        it.key()->installEventFilter(this);
        it++;
    }

}

void MachineSetting::_hidecontrol(quint8 index,QLabel *plabel, QSpinBox* pspinbox,int _ks600 ,int _ks800)
{

    switch(m_typedequipment)
    {
        case KS600:
        {
            if(index >= _ks600){
                plabel->hide();
                pspinbox->hide();
            }
            break;
        }
        case KS800:
        {
            if(index >= _ks800)
            {
                plabel->hide();
                pspinbox->hide();
            }
            break;
        }
        case KS1200:
        {
            break;
        }
        default:break;
    }
}
void MachineSetting::_configHandsinitChn(quint8 equipmentIndex_)
{
    QMap<QLabel*, QSpinBox*>  modulecontrol_;
    modulecontrol_.insert(ui->label_handsdownchn_0,ui->spinBox_handsdownchn_0);
    modulecontrol_.insert(ui->label_handsdownchn_1,ui->spinBox_handsdownchn_1);
    modulecontrol_.insert(ui->label_handsdownchn_2,ui->spinBox_handsdownchn_2);

    QList<int> indexactiveList;
    indexactiveList<<MODULE_1<<MODULE_2<<MODULE_3;
    int k = 0;
    auto it = modulecontrol_.begin();
    while(it != modulecontrol_.end())
    {
        _creatstupoint(it.key(),it.value(),indexactiveList.at(k),m_pHandsLableList);
        int key_chn = QUIUtils::StringFindintnum(it.key()->objectName());
        if(equipmentIndex_ == KS600 && key_chn >= MODULE_1 )
        {
            it.key()->hide();
            it.value()->hide();
        }
        else if(equipmentIndex_ == KS800 && key_chn >= MODULE_2)
        {
            it.key()->hide();
            it.value()->hide();
        }
        it.key()->setText(QString("抓手下降深度(模组%1):").arg(key_chn+1));
        it.value()->setValue(INI_File()._gethandsdownheiht(key_chn*4));
        _hidecontrol(key_chn,it.key(),it.value(),MODULE_1,MODULE_2);

        connect(it.value(),&QSpinBox::editingFinished,this,[=](){
            QSpinBox* pspinbox = (QSpinBox* )sender();
            int index = QUIUtils::StringFindintnum(pspinbox->objectName());
            INI_File()._sethandsdownheiht(index*4,pspinbox->value());

        });
        it++;
        k++;
    }
}
void MachineSetting::_innitHands(quint8 equipmentIndex_)
{
    _configHandsinitChn(equipmentIndex_); //通道
    ui->spinBoxSuckAirsMax->setValue(INI_File().rConfigPara(GRIPPERLESSTHANMAX).toInt());
    ui->spinBoxSplitAirsMin->setValue(INI_File().rConfigPara(GRIPPERBIGTHANMIM).toInt());
    ui->spinBox_suckLatetimer->setValue(INI_File().rConfigPara(GRIPPERSUCKTIME).toInt());


    QMap<QLabel*, QSpinBox*>  Traytestcontrol_;
    Traytestcontrol_.insert(ui->label_testtray_0,ui->spinBox_testtray_0);
    Traytestcontrol_.insert(ui->label_testtray_1, ui->spinBox_testtray_1);
    Traytestcontrol_.insert(ui->label_testtray_2, ui->spinBox_testtray_2);
    Traytestcontrol_.insert(ui->label_testtray_3, ui->spinBox_testtray_3);
    QList<int> indexactiveList;
    indexactiveList<<HANDSDOWN_TRAYTUBE_1<<HANDSDOWN_TRAYTUBE_2<<HANDSDOWN_TRAYTUBE_3<<HANDSDOWN_TRAYTUBE_4;
    int k = 0;
    auto iter = Traytestcontrol_.begin();
    while(iter != Traytestcontrol_.end())
    {
        _creatstupoint(iter.key(),iter.value(),indexactiveList.at(k),m_pHandsLableList);
        int key_chn = QUIUtils::StringFindintnum(iter.key()->objectName());
        iter.key()->setText(QString("试管盘%1抓手下降深度:").arg(key_chn+1));
        iter.value()->setValue(INI_File()._gethandsdownheightinTesttray(key_chn));
        _hidecontrol(key_chn,iter.key(),iter.value(),2,3);
        if(equipmentIndex_ == KS600 && key_chn >= 4 )
        {
            iter.key()->hide();
            iter.value()->hide();
        }
        else if(equipmentIndex_ == KS800 && key_chn >= 8)
        {
            iter.key()->hide();
            iter.value()->hide();
        }
        connect(iter.value(),&QSpinBox::editingFinished,this,[=](){
            QSpinBox* pspinbox = (QSpinBox* )sender();
            int index = QUIUtils::StringFindintnum(pspinbox->objectName());
            INI_File()._sethandsdownheightinTesttray(index,pspinbox->value());
            QLOG_DEBUG()<<"试管盘"<<index+1<<"设置下降深度="<<pspinbox->value();
        });
        iter++;
        k++;
    }

    //弃杯
    ui->spinBox_downthrowcups->blockSignals(true);
    ui->spinBox_downthrowcups->setValue(INI_File()._gethandsdownthrowcpus());
    ui->spinBox_downthrowcups->blockSignals(false);
    _creatstupoint(ui->label_handthrowcups,ui->spinBox_downthrowcups,HANDSDOWN_THROWCUP,m_pHandsLableList);
    connect(ui->spinBox_downthrowcups,&QSpinBox::editingFinished,this,[=](){
         INI_File()._sethandsdownthrowcpus(ui->spinBox_downthrowcups->value());
         QLOG_DEBUG()<<"抓手在弃杯处下降高度"<<ui->spinBox_downthrowcups->value();
    });
}

bool MachineSetting::eventFilter(QObject *obj, QEvent *ev)
{
    QLabel *plabel = qobject_cast<QLabel *>(obj);
    if(m_pbloodLableList.contains(plabel))
    {
       if(ev->type() == QEvent::MouseButtonPress)
       {
           auto iter = m_pbloodLableList.find(plabel);
           btnLable* pstuinfo = iter.value();
           if(cglobal::g_StartTesting || !cglobal::gserialConnecStatus)
               return false;
           plabel->setStyleSheet("color: rgba(118 ,238, 0, 240);");
           QByteArrayList senddata_ = QUIUtils::_controltestbloodpindownheigh(pstuinfo->index_,
                                                            pstuinfo->pdownmovemm->value()); //控制血样针下针命令
           emit testdownheight(senddata_,pstuinfo->index_);
           return true;
       }
       return false;
    }
    else if (m_preagpinLableList.contains(plabel))
    {
        if(ev->type() == QEvent::MouseButtonPress)
        {
            auto it_ = m_preagpinLableList.find(plabel);
            btnLable* pstuinfo = it_.value();
            if(cglobal::g_StartTesting || !cglobal::gserialConnecStatus)
                return false;
            plabel->setStyleSheet("color: rgba(118 ,238, 0, 240);");
            QByteArrayList senddata_ = QUIUtils::_controlReagpindownheih(pstuinfo->index_,pstuinfo->pdownmovemm->value());
            emit testdownheight(senddata_,pstuinfo->index_);
            return true;
        }
        return false;
    }
    else if (m_pHandsLableList.contains(plabel))
    {
        if(ev->type() == QEvent::MouseButtonPress)
        {
            auto it_ = m_pHandsLableList.find(plabel);
            btnLable* pstuinfo = it_.value();
            if(cglobal::g_StartTesting || !cglobal::gserialConnecStatus)
                return false;
            plabel->setStyleSheet("color: rgba(118 ,238, 0, 240);");
            QByteArrayList senddata_ = QUIUtils::_controlHandsdowntest(pstuinfo->index_,pstuinfo->pdownmovemm->value());
            emit testdownheight(senddata_,pstuinfo->index_);
            return true;
        }
        return false;
    }

    return MachineSetting::eventFilter(obj, ev);
}

void MachineSetting::_finishmovetestdownhigh(int index_)
{
	auto findAndResetStyle = [index_](auto& container) -> bool {
		for (auto it = container.begin(); it != container.end(); ++it) {
			// 使用Qt迭代器API 
			btnLable* pstuinfo = it.value();  // 关键修正：value()方法
			if (pstuinfo->index_ == index_) {
				pstuinfo->plableinfo->setStyleSheet("");
				return true;
			}
		}
		return false;
	};

    // 按顺序搜索三个列表
    if (findAndResetStyle(m_pbloodLableList)) return;
    if (findAndResetStyle(m_preagpinLableList)) return;
    findAndResetStyle(m_pHandsLableList);
}



/////// 调整模组参数或者查看模组参数 数据   ///////////////////
void MachineSetting::slotsendReminder(QString reminderstr,const int indexActive)
{
    ui->label_displayReminder->setText(reminderstr);
    if(W_SAVEMODULESETTING ==  indexActive)
    {
        QLOG_DEBUG()<<"收到模组保存成功命令,并继续遍历模组"<<endl;
        emit pauseConnectModule(false);//继续读模组和主板消息
    }
    update();
}

void MachineSetting::slotmoduleSpeedData(quint8 indexModule,int chnIData,int chnIIData,int chnIIIData,int chnIVData)
{

    // 1. 定义模块通道映射常量 - 提高可读性
    static const QMap<quint8, QPair<quint8, quint8>> MODULE_CHANNEL_MAP = {
        {MODULE_1, {TEST_CHANNEL_1 - 1, TEST_CHANNEL_4 - 1}},
        {MODULE_2, {TEST_CHANNEL_5 - 1, TEST_CHANNEL_8 - 1}},
        {MODULE_3, {TEST_CHANNEL_9 - 1, TEST_CHANNEL_12 - 1}}
    };
    // 2. 使用安全数据结构
    QVector<int> speedData = { chnIData, chnIIData, chnIIIData, chnIVData };
    const int CHANNELS_PER_MODULE = 4; // 显式定义通道数

    // 3. 信号阻塞使用RAII模式 - 更安全
    QSignalBlocker blocker(ui->tableWidget_controlChn);

    // 4. 边界检查与错误处理
    if (!MODULE_CHANNEL_MAP.contains(indexModule)) {
        QLOG_WARN() << "无效的模组索引:" << indexModule;
        ui->label_displayReminder->setText("错误：未知模组");
        return;
    }

    // 5. 获取通道范围
   const auto &channelRange = MODULE_CHANNEL_MAP[indexModule];
   quint8 startRow = channelRange.first;
   quint8 endRow = channelRange.second;

   // 6. 通道数量验证
   const int expectedChannels = endRow - startRow + 1;
   if (expectedChannels != CHANNELS_PER_MODULE) {
       QLOG_WARN() << "模组" << indexModule << "通道数不匹配:"
                  << expectedChannels << "!= 4";
       return;
   }

   //批量更新表格数据 - 减少函数调用
   static const QColor textColor = QApplication::palette().text().color();  // 适配主题
   static const QFont tableFont("楷体", 14, QFont::Black);
   for (int i = 0; i < CHANNELS_PER_MODULE; ++i) {
       const int row = startRow + i;

       // 8. 创建表格项并设置数据
       QTableWidgetItem *speedItem = new QTableWidgetItem(QString::number(speedData[i]));
       speedItem->setForeground(textColor);
       speedItem->setFont(tableFont);
       speedItem->setTextAlignment(Qt::AlignCenter);

       // 9. 使用setItem替代自定义函数 - 标准化操作
       ui->tableWidget_controlChn->setItem(row, 1, speedItem);
   }

   // 10. 使用tr()实现国际化支持
   ui->label_displayReminder->setText(
       tr("读取模组%1转速成功!").arg(indexModule)
   );
}


void MachineSetting::slotmoduleLedData(quint8 indexModule, int chnIData, int chnIIData,
                                       int chnIIIData, int chnIVData)
{
    quint8 ifrom, iend;
    switch (indexModule) {
        case MODULE_1:
            ifrom = TEST_CHANNEL_1 - 1;
            iend = TEST_CHANNEL_4 - 1;
            break;
        case MODULE_2:
            ifrom = TEST_CHANNEL_5 - 1;
            iend = TEST_CHANNEL_8 - 1;
            break;
        case MODULE_3:
            ifrom = TEST_CHANNEL_9 - 1;
            iend = TEST_CHANNEL_12 - 1;
            break;
        default:
            QLOG_WARN()<<("Invalid module index: %d", indexModule); // 错误日志
            return; // 提前返回，避免无效操作
    }

    // 阻塞QTableWidget信号并保存原始状态
    bool oldState = ui->tableWidget_controlChn->blockSignals(true);
    for (int i = ifrom; i <= iend; i++) {
        // 直接计算数据索引，避免额外数组和变量
        int dataIndex = i - ifrom;
        int data = (dataIndex == 0) ? chnIData :
                  (dataIndex == 1) ? chnIIData :
                  (dataIndex == 2) ? chnIIIData : chnIVData;
        insertDataItem(ui->tableWidget_controlChn, i, 2 /* LED数据显示列 */, QString::number(data), false);
    }
    ui->tableWidget_controlChn->blockSignals(oldState); // 恢复信号状态

    // 更新标签（不涉及QTableWidget信号，无需阻塞）
    ui->label_displayReminder->setText(QString("读取模组%1LED成功!").arg(indexModule));
}


//通到数据调光表
void MachineSetting::initDimmingTab(QTableWidget *pdimmingTable, const quint8 indexequipment)
{

    // 1. 定义列索引常量 - 提高可读性和可维护性
    enum ColumnIndex {
        COL_CHANNEL = 0,
        COL_SPEED,
        COL_LED_VALUE,
        COL_SET_MODULE_LED,
        COL_READ_SPEED,
        COL_READ_LED,
        COL_APPLY_TO_MODULE
    };

    // 2. 设备通道配置映射表 - 消除switch-case硬编码
    static const QMap<quint8, quint8> deviceChannelMap = {
        {KS800,  8},
        {KS1200, 12},
        {KS600,  4}
    };

    // 3. 获取设备通道数
    quint8 totalChn = deviceChannelMap.value(indexequipment, 12); // 默认12通道

    // 4. 准备表头数据
    QStringList headers = {
        "通道号", "转速", "LED值", "设置模组LED", "读转速", "读LED", "设置到模组"
    };

    QStringList rowLabels;
    rowLabels.reserve(totalChn);
    for (int i = 0; i < totalChn; ++i) {
        rowLabels << QString("通道%1").arg(i + 1);
    }


    // 5. 信号阻塞保护 (RAII方式)
    QSignalBlocker blocker(pdimmingTable);

    // 6. 初始化表格框架
    GlobalData::QTableWidgetinitSheet(pdimmingTable, "调整通道参数", headers, rowLabels, true);

    // 7. 设置列宽
    pdimmingTable->horizontalHeader()->resizeSection(COL_SET_MODULE_LED, 200);
    pdimmingTable->horizontalHeader()->resizeSection(COL_APPLY_TO_MODULE, 200);

    // 8. 缓存样式表 (避免重复加载)
    static QString customQss;
    if (customQss.isEmpty()) {
        QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
        if (styleFile.open(QIODevice::ReadOnly)) {
            customQss = QString::fromLatin1(styleFile.readAll());
        }
    }

    // 9. 初始化表格内容
    for (int i = 0; i < totalChn; ++i) {
        // 第一列：通道号
        insertDataItem(pdimmingTable, i, COL_CHANNEL, rowLabels[i], false);

        // 第二列：转速状态
        insertDataItem(pdimmingTable, i, COL_SPEED, "未读取", true);

        // 第三列：LED状态
        insertDataItem(pdimmingTable, i, COL_LED_VALUE, "未读取", false);
    }

    // 10. 分组设置按钮 (每4个通道一组)
    const int groupSize = 4;
    const int groupCount = qCeil(static_cast<qreal>(totalChn) / groupSize);

    for (int groupIdx = 0; groupIdx < groupCount; ++groupIdx) {
        const int firstRow = groupIdx * groupSize;
        const int rowSpan = qMin(groupSize, totalChn - firstRow);

        // 设置模组LED值
        pdimmingTable->setSpan(firstRow, COL_SET_MODULE_LED, rowSpan, 1);
        int dimmingVal = INI_File().getModuledimmingVal(groupIdx + 1);
        insertDataItem(pdimmingTable, firstRow, COL_SET_MODULE_LED,
                       QString::number(dimmingVal), true);

        // 读转速按钮
        pdimmingTable->setSpan(firstRow, COL_READ_SPEED, rowSpan, 1);
        tableinsertBtn(pdimmingTable, firstRow, COL_READ_SPEED, customQss);

        // 读LED按钮
        pdimmingTable->setSpan(firstRow, COL_READ_LED, rowSpan, 1);
        tableinsertBtn(pdimmingTable, firstRow, COL_READ_LED, customQss);

        // 应用到模组按钮
        pdimmingTable->setSpan(firstRow, COL_APPLY_TO_MODULE, rowSpan, 1);
        tableinsertBtn(pdimmingTable, firstRow, COL_APPLY_TO_MODULE, customQss);
    }

    // 11. 连接信号 (确保只连接一次)
   static bool signalConnected = false;
   if (!signalConnected) {
       connect(pdimmingTable, &QTableWidget::itemChanged, [=](QTableWidgetItem* item) {
           if (!item) return;
           itemChangeValue(item->row(), item->column(), item->text().toInt());
       });
       signalConnected = true;
   }

   m_initedDimmingTable = true;
}


void MachineSetting::itemChangeValue(const int row, const int cols, const int val) {
    // 定义列常量，避免魔术数字
    const int SPEED_COLUMN = 1;
    const int LED_COLUMN = 3;

    // 公共计算：模块索引（每个模块4个通道）
    quint8 indexmodule = row / 4 + 1;

    switch (cols) {
        case SPEED_COLUMN: {
            quint16 setspeed = static_cast<quint16>(val);
            quint8 indexchn = row + 1;  // 通道索引从1开始

            // 构建设置转速的命令
            QByteArray writeSpeedArry = GlobalData::ConfigEachChannelSpeed(indexmodule, indexchn, setspeed);
            QString commandDescription = tr("设置模组%1通道%2转速%3")
                                        .arg(indexmodule)
                                        .arg(indexchn)
                                        .arg(setspeed);
            emit WriteArryCommand(writeSpeedArry, commandDescription);
            break;
        }
        case LED_COLUMN: {
            quint16 dataled = static_cast<quint16>(val);

            // 保存LED值到INI文件
            INI_File().setModuledimmingVal(indexmodule, dataled);

            // 构建设置LED的命令
            QByteArray senddata = GlobalData::WriteModuleLedData(indexmodule, W_MODULE_LED, dataled);
            QString commandDescription = tr("写模组%1LED值").arg(indexmodule);
            emit WriteArryCommand(senddata, commandDescription);
            break;
        }
        default:
            // 可选：处理未识别的列，例如记录警告
            QLOG_WARN() << "未知的列索引:" << cols << "在行" << row;
            break;
    }
}

void MachineSetting::insertDataItem(QTableWidget* pdimmingTable,
                                   int row, int col,
                                   const QString& text,
                                   bool readOnly)
{
    // 参数校验 - 确保行列索引有效
    if (!pdimmingTable || row < 0 || col < 0 || row >= pdimmingTable->rowCount() || col >= pdimmingTable->columnCount()) {
        QLOG_WARN() << "Invalid table position: (" << row << "," << col << ")";
        return;
    }

    // 2. 创建表项并设置基本属性
    QTableWidgetItem* item = new QTableWidgetItem(text);

    // 3. 使用样式常量 - 提高可维护性
    static const QColor textColor = QApplication::palette().text().color();  // 适配主题
    static const QFont tableFont("楷体", 14, QFont::Black);

    item->setForeground(textColor);
    item->setFont(tableFont);
    item->setTextAlignment(Qt::AlignCenter);  // 合并水平和垂直居中

    // 4. 设置编辑权限
    //item->setFlags(readOnly ? item->flags() & \~Qt::ItemIsEditable : item->flags() | Qt::ItemIsEditable);
	if(!readOnly)
		item->setFlags(Qt::ItemIsEnabled);
	
    // 5. 单次设置表项
    pdimmingTable->setItem(row, col, item);
}


//void MachineSetting::insertDataItem(QTableWidget *pdimmingTable,int rows,int cols,QString itemdata,bool enable)
//{
//    QTableWidgetItem *item_ = new QTableWidgetItem(itemdata);
//    item_->setTextColor(QColor(0 ,0, 0));
//    pdimmingTable->setItem(rows,cols,item_);
//    if(!enable)
//        item_->setFlags(Qt::ItemIsEnabled);//设置改item不可修改;
//    pdimmingTable->item(rows,cols)->setFont(QFont( "楷体", 14, QFont::Black ));
//    pdimmingTable->item(rows,cols)->setTextAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
//    return;
//}

void MachineSetting::tableinsertBtn(QTableWidget *pdimmingTable,int row,int col,const QString& customQss)
{

    //减少重复计算 - 提取公共表达式
    const int moduleIndex = row / 4 + 1;

    //使用映射表替代switch-case - 提高可读性和可维护性
    static const QHash<int, QString> buttonTextMap = {
        {4, "模组%1转速"},
        {5, "模组%1LED"},
        {6, "保存模组%1"}
    };

    // 3. 创建按钮并配置
    QPushButton* btnSave = new QPushButton();
    btnSave->setMinimumHeight(35 * 4);
    btnSave->setStyleSheet(customQss);

    //设置按钮文本 - 使用映射表
    QString buttonText = buttonTextMap.value(col, "未知操作");
    btnSave->setText(buttonText.arg(moduleIndex));

    //优化信号连接 - 使用新式语法并传递上下文信息
    connect(btnSave, &QPushButton::clicked, this, [this, row, col]() {
        handleButtonClick(row, col);  // 新增处理函数
    });

    //简化布局创建 - 直接使用按钮作为单元格控件
    pdimmingTable->setCellWidget(row, col, btnSave);
}


void MachineSetting::handleButtonClick(int row,int col)
{
    // 直接计算模组索引（无需通过控件位置）
    const int moduleIndex = row / 4;
    handleButtonAction(moduleIndex, col);
}

void MachineSetting::handleButtonAction(int moduleIndex, int column)
{
    // 1. 定义操作映射表 - 消除重复switch
    struct ModuleAction {

		// 添加显式构造函数
		ModuleAction(std::function<QByteArray()> g, QString n, bool p = false)
			: getData(std::move(g)),
			actionName(std::move(n)),
			requiresPause(p) {}

        std::function<QByteArray()> getData;
        QString actionName;
        bool requiresPause = false;
    };

    // 2. 使用映射表替代多重switch
    static const std::map<std::pair<int, int>, ModuleAction> actionMap = {
        {std::make_pair(0, 4), ModuleAction{
             []{ return GlobalData::ReadEachChannelSpeed(MODULE_1); },
                  "读取模组1转速"}},
        {std::make_pair(1, 4), ModuleAction{
             []{ return GlobalData::ReadEachChannelSpeed(MODULE_2); },
                  "读取模组2转速"}},
        {std::make_pair(2, 4), ModuleAction{
             []{ return GlobalData::ReadEachChannelSpeed(MODULE_3); },
                  "读取模组3转速"}},

        {std::make_pair(0, 5), ModuleAction{
             []{ return GlobalData::ReadModuleLedData(MODULE_1); },
                  "读取模组1LED"}},
        {std::make_pair(1, 5), ModuleAction{
             []{ return GlobalData::ReadModuleLedData(MODULE_2); },
                  "读取模组2LED"}},
        {std::make_pair(2, 5), ModuleAction{
             []{ return GlobalData::ReadModuleLedData(MODULE_3); },
                  "读取模组3LED"}},

        {std::make_pair(0, 6), ModuleAction{
             []{ return GlobalData::SaveConfigModuleData(MODULE_1, W_SAVEMODULESETTING); },
                  "保存模组1", true}},
        {std::make_pair(1, 6), ModuleAction{
             []{ return GlobalData::SaveConfigModuleData(MODULE_2, W_SAVEMODULESETTING); },
                  "保存模组2", true}},
        {std::make_pair(2, 6), ModuleAction{
             []{ return GlobalData::SaveConfigModuleData(MODULE_3, W_SAVEMODULESETTING); },
                  "保存模组3", true}}
    };

    // 3. 查找操作配置
    auto it = actionMap.find({moduleIndex, column});
    if (it == actionMap.end()) {
        QLOG_ERROR() << "无效的按钮操作: 模组" << moduleIndex << "列" << column;
        return;
    }

    const ModuleAction& action = it->second;

    // 4. 非阻塞式延时处理
    auto executeAction = [this, action] {
        if (action.requiresPause) {
            emit pauseConnectModule(true);
        }

        // 5. 异常安全的数据获取
        try {
            QByteArray sendData = action.getData();
            emit WriteArryCommand(sendData, action.actionName);
        } catch (const std::exception& e) {
            QLOG_ERROR() << "操作执行失败:" << e.what();
        }
    };

    // 6. 延时执行机制（仅需延时100ms时使用）
    if (action.requiresPause) {
        QTimer::singleShot(100, this, executeAction);
    } else {
        executeAction();
    }
}



//opencv 测试识别参数阈值调整
void MachineSetting::on_pushButton_opencv_clicked()
{
    if(!m_showOpencvImage)
        m_showOpencvImage = new TestOpcv();
    m_showOpencvImage->show();
}



//载入性能验证验证
void MachineSetting::loadPerformanceEvaluation(QWidget *parentWidget){
    // 检查测试状态（线程安全方式）
    if(cglobal::g_StartTesting) {
        QMessageBox::warning(parentWidget,
                           tr("操作禁止"),
                           tr("仪器测试中，请等待当前测试完成。"),
                           QMessageBox::Ok);
        return;
    }

	// 确保UI操作在主线程执行
	Q_ASSERT(QThread::currentThread() == qApp->thread());

    // 单例模式管理窗口
    if(!m_Performanceverification) {
        m_Performanceverification.reset(new ReplaceTheTestTubeTray(m_typedequipment,parentWidget));
		
		// 清理旧布局（安全内存管理）
		QLayout* oldLayout = parentWidget->layout();
		if (oldLayout) {
			QLayoutItem* item;
			while ((item = oldLayout->takeAt(0)) != nullptr) {
				delete item->widget();
				delete item;
			}
			delete oldLayout;
		}

		// 配置自适应布局
		QHBoxLayout* mainLayout = new QHBoxLayout(parentWidget);
		mainLayout->setContentsMargins(0, 0, 0, 0);  // 消除边距
		mainLayout->addWidget(m_Performanceverification.get());
    }

	

    // 异步加载资源
	QTimer::singleShot(0, this, [this, parentWidget]() {
		if (m_Performanceverification) {
            m_Performanceverification->showNormal();
			parentWidget->updateGeometry();
		}
	});
}

void MachineSetting::HandleoutputResultData(const QString& id,const quint8& channel,const std::array<double, 3>& ratios)
{
    if (!m_Performanceverification)
        return;
    m_Performanceverification->fillData(id, channel,ratios);
}

void MachineSetting::completedPETest(){
    if(m_Performanceverification){
        m_Performanceverification.get()->handlecompletedPETest();
    }
}



//校准坐标
void MachineSetting::on_pushButton_Adjustcoordinates_clicked()
{
    CustomPlot* pkjustcoordinate = FullyAutomatedPlatelets::pinstanceadjustcoordinates();

    if (!m_initAdjustcoordinates) {
        m_initAdjustcoordinates = true;
        pkjustcoordinate->initstyle();

        // 使用新式语法连接，检查返回值
        bool conn1 = connect(pkjustcoordinate, &CustomPlot::writdAxisata,
                            this, [=](const QByteArrayList data_, QString info) {
            emit FullyAutomatedPlatelets::mainWindow()->_sendcodeList(data_, info);
        });
        Q_ASSERT(conn1); // 确保连接成功 [[15]]

        bool conn2 = connect(pkjustcoordinate, &CustomPlot::Resetmaneuver,
                            this, [=]() {
            FullyAutomatedPlatelets::mainWindow()->machineReposition();
        });
        Q_ASSERT(conn2);

        bool conn3 = connect(pkjustcoordinate, &CustomPlot::SportActive,
                            FullyAutomatedPlatelets::pinstanceSingleactive(),
                            &USB_InitConnect::slotCeratActionDate);
        Q_ASSERT(conn3);

        bool conn4 = connect(FullyAutomatedPlatelets::pinstanceSingleactive(),
                            &USB_InitConnect::CoordinatefinetuningactionFinish,
                            pkjustcoordinate, &CustomPlot::Recv_CalibrationMoved);
        Q_ASSERT(conn4);

        bool conn5 = connect(pkjustcoordinate, &CustomPlot::TrayMoveTest,
                            FullyAutomatedPlatelets::pinstanceserialusb(),
                            &SuoweiSerialPort::Recv_TrayMoveTest);
        Q_ASSERT(conn5);

        bool conn6 = connect(FullyAutomatedPlatelets::pinstanceserialusb(),
                            &SuoweiSerialPort::continueSendArryTray,
                            pkjustcoordinate, &CustomPlot::send_test_cups_accurate);
        Q_ASSERT(conn6);

        // 窗口关闭时重置初始化标志
        connect(pkjustcoordinate, &QWidget::destroyed, this, [=]() {
            m_initAdjustcoordinates = false;
        });
    }
    pkjustcoordinate->showMaximized();

}





void MachineSetting::on_pushButtonsplitAirs_clicked()
{
    quint8 downhands = 0;
    quint8 HandsSucknum =  HANDS_SPLIT_NUM;
    QByteArray poutdata = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, downhands,
                                                                          0, HandsSucknum,false,
                                                                          downhands,false,
                                                                          GRIPPERNORMAL);
    emit WriteArryCommand(poutdata,"关负压泵");
}

void MachineSetting::on_pushButtonopenSuck_clicked()
{
    quint8 downhands = 0;
    quint8 HandsSplitnum =  HANDS_SUCK_NUM;
    QByteArray poutdata = Testing::m_TaskDll->DLL_ZMoveSpecifiedPosition(MOTOR_HANDS_INDEX, downhands,
                                                                          0, HandsSplitnum,false,
                                                                          downhands,false,
                                                                          GRABCUPWITHGRIPPER);
    emit WriteArryCommand(poutdata,"打开负压泵");
}

void MachineSetting::handlebackHandssuck(bool issuck,int airval){
    if(issuck)
        ui->label_showsuckVal->setText(QString::number(airval));
    else
        ui->label_splitval->setText(QString::number(airval));
}

void MachineSetting::on_pushButtonsavedimming_clicked()
{
    configWriteDimming2Board();
}
