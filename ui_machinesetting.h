/********************************************************************************
** Form generated from reading UI file 'machinesetting.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MACHINESETTING_H
#define UI_MACHINESETTING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <channelvaluepanel.h>

QT_BEGIN_NAMESPACE

class Ui_MachineSetting
{
public:
    QHBoxLayout *horizontalLayout_configureset;
    QTreeWidget *MachineSetList;
    QTabWidget *tabWidgetSetconfigure;
    QWidget *tab_1;
    QVBoxLayout *verticalLayout_8;
    QGroupBox *groupBox_usermanagement;
    QHBoxLayout *horizontalLayout_2;
    QListWidget *listWidget_user;
    QWidget *widget_Btngroup;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton_ADD;
    QPushButton *pushButton_DELETE;
    QSpacerItem *verticalSpacer;
    QPushButton *pushButton_CHANGE;
    QPushButton *pushButton_SWITCH;
    QGroupBox *groupBox_printReport;
    QGridLayout *gridLayout_printinfo;
    QCheckBox *checkBox_Automatic_print;
    QComboBox *comboBox_QC_sample_report;
    QComboBox *comboBox_Patient_sample_report;
    QLabel *label_Tab0_hospital_name;
    QLabel *label_QC_sample_report;
    QCheckBox *checkBox_Automatic_review;
    QLineEdit *lineEdit_Tab0_hospital_name;
    QLabel *label_Patient_sample_report;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_tableWidget_info_1;
    QWidget *widget_Info_0;
    QHBoxLayout *horizontalLayout_11;
    QLabel *label_Category;
    QComboBox *comboBox_Category;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_viewSQL;
    QWidget *widget_Info;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_abbreviation;
    QLabel *label_Abbreviation;
    QLineEdit *lineEdit_Abbreviation;
    QVBoxLayout *verticalLayout_name;
    QLabel *label_Name;
    QLineEdit *lineEdit_Name;
    QTabWidget *tabWidget_info;
    QWidget *tab_info;
    QVBoxLayout *verticalLayout_tableWidget_info;
    QTableWidget *tableWidget_info;
    QWidget *tab_info_1;
    QHBoxLayout *horizontalLayout_tableWidget_info_1;
    QTableWidget *tableWidget_info_1;
    QWidget *tab_info_2;
    QHBoxLayout *horizontalLayout_tableWidget_info_2;
    QTableWidget *tableWidget_info_2;
    QWidget *tab_info_3;
    QHBoxLayout *horizontalLayout_tableWidget_info_3;
    QTableWidget *tableWidget_info_3;
    QWidget *tab_info_4;
    QHBoxLayout *horizontalLayout_tableWidget_info_4;
    QTableWidget *tableWidget_info_4;
    QWidget *tab_info_5;
    QHBoxLayout *horizontalLayout_tableWidget_info_5;
    QTableWidget *tableWidget_info_5;
    QWidget *tab_info_6;
    QHBoxLayout *horizontalLayout_tableWidget_info_6;
    QTableWidget *tableWidget_info_6;
    QWidget *tab_info_7;
    QHBoxLayout *horizontalLayout_tableWidget_info_7;
    QTableWidget *tableWidget_info_7;
    QWidget *widget_info2;
    QHBoxLayout *horizontalLayout_but;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *toolButton_ADD;
    QToolButton *toolButton_Delete;
    QWidget *MachineTestConfigure;
    QGridLayout *gridLayout_12;
    QGroupBox *disposition_value;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_13;
    QCheckBox *UseBarCode;
    QCheckBox *checkBox_UesSecondReagentHole;
    QCheckBox *checkBox_suck_offset;
    QCheckBox *checkBox_catchcups;
    QWidget *okandcancelbtn;
    QHBoxLayout *horizontalLayout_5;
    QSpacerItem *horizontalSpacer_13;
    QToolButton *Machine_conf_save;
    QToolButton *toolButton_Cancel;
    QSpacerItem *horizontalSpacer_14;
    QGroupBox *usechannel;
    QGridLayout *gridLayout_11;
    QCheckBox *Channel_State_1;
    QCheckBox *Channel_State_5;
    QCheckBox *Channel_State_9;
    QCheckBox *Channel_State_2;
    QCheckBox *Channel_State_6;
    QCheckBox *Channel_State_10;
    QCheckBox *Channel_State_3;
    QCheckBox *Channel_State_7;
    QCheckBox *Channel_State_11;
    QCheckBox *Channel_State_4;
    QCheckBox *Channel_State_8;
    QCheckBox *Channel_State_12;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_18;
    QLabel *label_Moduletemperature_1;
    QDoubleSpinBox *doubleSpinBox_Moduletemperature_1;
    QLabel *label_Moduletemperature_3;
    QDoubleSpinBox *doubleSpinBox_Moduletemperature_3;
    QLabel *label_Moduletemperature_2;
    QDoubleSpinBox *doubleSpinBox_Moduletemperature_2;
    QWidget *tab_basicpara;
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidgetReferencevalue;
    QWidget *ChannelVale_display;
    QGridLayout *gridLayout_10;
    QWidget *widget_showModule1th;
    QHBoxLayout *horizontalLayout_6;
    ChannelvaluePanel *Channel_1;
    ChannelvaluePanel *Channel_2;
    ChannelvaluePanel *Channel_3;
    ChannelvaluePanel *Channel_4;
    QWidget *widget_showModule2th;
    QHBoxLayout *horizontalLayout_10;
    ChannelvaluePanel *Channel_5;
    ChannelvaluePanel *Channel_6;
    ChannelvaluePanel *Channel_7;
    ChannelvaluePanel *Channel_8;
    QWidget *widget_showModule3th;
    QGridLayout *gridLayout_9;
    ChannelvaluePanel *Channel_12;
    ChannelvaluePanel *Channel_9;
    ChannelvaluePanel *Channel_10;
    ChannelvaluePanel *Channel_11;
    QWidget *widget_valuediaplay;
    QVBoxLayout *verticalLayout_4;
    QTableWidget *tableWidget_controlChn;
    QWidget *widget_bottom;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_displayReminder;
    QPushButton *pushButtonsavedimming;
    QToolButton *toolButton_oopenall;
    QWidget *tab_6;
    QGridLayout *gridLayout_14;
    QWidget *tab_LIS;
    QVBoxLayout *verticalLayout_19;
    QTabWidget *tabWidget_3;
    QWidget *tab_TextOut;
    QHBoxLayout *horizontalLayout_37;
    QWidget *widget_12;
    QVBoxLayout *verticalLayout_20;
    QTableWidget *tableWidget_alwaysLIS;
    QCheckBox *checkBox_endAuditoutdata;
    QCheckBox *checkBox_testCompleteAutoOut;
    QWidget *widget_11;
    QVBoxLayout *verticalLayout_21;
    QPushButton *pushButton_LiS_Used;
    QPushButton *pushButton_LIS_Enable;
    QPushButton *pushButton_LIS_Config;
    QSpacerItem *verticalSpacer_4;
    QWidget *tab_Nornal;
    QVBoxLayout *verticalLayout_22;
    QGroupBox *groupBox_5;
    QHBoxLayout *horizontalLayout_38;
    QRadioButton *radioButton_tab;
    QRadioButton *radioButton_point;
    QRadioButton *radioButton_Spaceing;
    QRadioButton *radioButton_others;
    QLineEdit *lineEdit_others;
    QSpacerItem *horizontalSpacer_31;
    QLabel *label_28;
    QTableWidget *tableWidget_outTexts;
    QCheckBox *checkBox_ResultAndInfo;
    QGroupBox *groupBox_connectStyle;
    QGridLayout *gridLayout_39;
    QRadioButton *radioButton_sel_tcpip;
    QLabel *label_TextOutPath;
    QRadioButton *radioButton_selSerial;
    QLineEdit *lineEdit_TextOutPath;
    QPushButton *pushButton_SelTextOutPath;
    QRadioButton *radioButton_Sel_text;
    QWidget *widget_14;
    QGridLayout *gridLayout_38;
    QLabel *label_34;
    QLabel *label_portname;
    QLabel *label_35;
    QSpinBox *spinBox_6;
    QSpinBox *spinBox_5;
    QLabel *label_33;
    QComboBox *comboBox_6;
    QComboBox *comboBox_5;
    QLabel *label_36;
    QPushButton *pushButton_backsetting;
    QComboBox *comboBox_Serialportnum;
    QWidget *widget_13;
    QGridLayout *gridLayout_37;
    QRadioButton *radioButton_10;
    QRadioButton *radioButton_11;
    QLabel *label_setaddr;
    QSpinBox *spinBox_setPort;
    QLabel *label_setAddr;
    QLineEdit *lineEdit_setAddr;
    QWidget *tab_fucn;
    QVBoxLayout *verticalLayout_ParametersSet;
    QWidget *widget_side;
    QVBoxLayout *verticalLayout_3;
    QTabWidget *tabWidget_config;
    QWidget *tab_configbloodpin;
    QGridLayout *gridLayout_5;
    QDoubleSpinBox *doubleSpinBox_Ratio_ben;
    QPushButton *pushButton_opencv;
    QDoubleSpinBox *FixedHighvalue;
    QLabel *OffsetTestHeight;
    QLabel *Poor_blood_changliang;
    QLabel *label_failedlinque_3;
    QSpacerItem *horizontalSpacer_10;
    QSpinBox *spinBox_CleanLinqueFailedHigh;
    QSpinBox *SecurityValue_box;
    QSpinBox *Testheighdownheigh;
    QSpinBox *EmptyHeigh;
    QSpacerItem *horizontalSpacer_9;
    QLabel *label_3;
    QSpinBox *poorBlood_changliang;
    QDoubleSpinBox *OffsetTestHeightValue;
    QLabel *label_SecurityValue;
    QLabel *label_fixedhigh;
    QLabel *label_EmptyTubeHeigh;
    QLabel *label_ratio_ben;
    QPushButton *pushButtonBloodPinParasave;
    QLabel *PRPratiolable;
    QDoubleSpinBox *doubleSpinBox_PRPratio;
    QLabel *label_failedlinque;
    QSpinBox *spinBox_faliedlinque;
    QLabel *label_suckairs;
    QSpinBox *spinBox_suckairs;
    QLabel *label_AbsorbX2;
    QSpinBox *spinBoxAbsorbX2;
    QLabel *labelwashesTime;
    QSpinBox *spinBox_WashesTime;
    QWidget *tab_reagpin;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox_RetagentBox;
    QGridLayout *gridLayout_Reagent;
    QLabel *label_11;
    QLabel *label_12;
    QLabel *label_show_AA;
    QLabel *label_show_ADR;
    QDoubleSpinBox *doubleSpinBox_COL_Ratio;
    QDoubleSpinBox *doubleSpinBox_Add_COL_Ratio;
    QSpinBox *spinBox_limit_ris;
    QLabel *label_show_ADP;
    QLabel *label_show_RIS;
    QSpinBox *spinBox_AA_Suck_Vol;
    QDoubleSpinBox *doubleSpinBox_AA_Ratio;
    QLabel *label_ReagentName;
    QLabel *label_show_COL;
    QSpinBox *spinBox_limit_cleanlinque;
    QLabel *label_absorbVol;
    QSpinBox *spinBox_ADP_Suck_Vol;
    QSpinBox *spinBox_down_ADR;
    QSpinBox *spinBox_ADR_Suck_Vol;
    QDoubleSpinBox *doubleSpinBox_ADR_Ratio;
    QSpinBox *spinBox_down_RIS;
    QDoubleSpinBox *doubleSpinBox_Add_AA_Ratio;
    QSpinBox *spinBox_down_ADP;
    QSpinBox *spinBox_down_AA;
    QLabel *label_13;
    QLabel *label_limit;
    QSpinBox *spinBox_COL_Suck_Vol;
    QDoubleSpinBox *doubleSpinBox_RIS_Ratio;
    QDoubleSpinBox *doubleSpinBox_ADP_Ratio;
    QSpinBox *spinBox_capacity_epi;
    QSpinBox *spinBox_RIS_Suck_Vol;
    QLabel *label_capacity;
    QDoubleSpinBox *doubleSpinBox_Add_RIS_Ratio;
    QSpinBox *spinBox_limit_aa;
    QDoubleSpinBox *doubleSpinBox_Add_ADP_Ratio;
    QDoubleSpinBox *doubleSpinBox_Add_ADR_Ratio;
    QSpinBox *spinBox_capacity_ris;
    QSpinBox *spinBox_limit_epi;
    QSpinBox *spinBox_down_COL;
    QSpinBox *spinBox_capacity_aa;
    QLabel *label_cleanlinque;
    QSpinBox *spinBox_capacity_cleanlinque;
    QSpinBox *spinBox_capacity_adp;
    QSpinBox *spinBox_capacity_col;
    QSpinBox *spinBox_limit_col;
    QSpinBox *spinBox_limit_adp;
    QWidget *widget_makesure;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *pushButton_saved;
    QWidget *widgetFucn;
    QGridLayout *gridLayout_17;
    QSpacerItem *horizontalSpacer_11;
    QLabel *label_delpms;
    QLabel *label_failedlinque_2;
    QSpinBox *spinBoxReagentNeedleFailehigh;
    QLabel *label_AbsorbX1;
    QSpinBox *spinBoxAbsorbX1;
    QLabel *label_failedhighreagentneedle;
    QSpinBox *spinBox_TestReagentFailedhigh;
    QSpinBox *spinBox_deptime;
    QSpacerItem *horizontalSpacer_12;
    QWidget *tab_hands;
    QGridLayout *gridLayout_16;
    QGroupBox *groupBox_gripperDown;
    QGridLayout *gridLayout_2;
    QLabel *label_handthrowcups;
    QSpinBox *spinBox_downthrowcups;
    QLabel *label_testtray_0;
    QSpinBox *spinBox_testtray_0;
    QLabel *label_testtray_1;
    QSpinBox *spinBox_testtray_1;
    QLabel *label_testtray_2;
    QSpinBox *spinBox_testtray_2;
    QLabel *label_testtray_3;
    QSpinBox *spinBox_testtray_3;
    QLabel *label_handsdownchn_0;
    QSpinBox *spinBox_handsdownchn_0;
    QLabel *label_handsdownchn_1;
    QSpinBox *spinBox_handsdownchn_1;
    QLabel *label_handsdownchn_2;
    QSpinBox *spinBox_handsdownchn_2;
    QGroupBox *groupBox_Airs;
    QGridLayout *gridLayout_8;
    QLabel *label_8;
    QLabel *label_2;
    QCheckBox *checkBox_Recapture;
    QSpinBox *spinBoxSplitAirsMin;
    QLabel *labelsuckLateTimer;
    QSpinBox *spinBox_suckLatetimer;
    QSpinBox *spinBoxSuckAirsMax;
    QLabel *label_splitval;
    QLabel *label_showsuckVal;
    QPushButton *pushButtonopenSuck;
    QPushButton *pushButtonsplitAirs;
    QWidget *widget_gripperok;
    QHBoxLayout *horizontalLayout_12;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *pushButtonHandsParaSave;
    QSpacerItem *horizontalSpacer_6;
    QWidget *tab;
    QGridLayout *gridLayout_7;
    QGroupBox *groupBox_setfile;
    QGridLayout *gridLayout_13;
    QLineEdit *lineEdit_showPath;
    QToolButton *toolButton_Import;
    QLabel *label_path;
    QToolButton *toolButton_export;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_5;
    QPushButton *pushButton_Adjustcoordinates;
    QGroupBox *groupBox_curvepara;
    QVBoxLayout *verticalLayout_7;
    QGroupBox *groupBox_4;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer_7;
    QLabel *label_basecutNum;
    QComboBox *comboBox_CutNum;
    QCheckBox *checkBox_Avg_cutnum;
    QSpacerItem *horizontalSpacer_8;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_6;
    QCheckBox *checkBox_originTestData;
    QCheckBox *checkBox_average;
    QCheckBox *checkBox_median;
    QCheckBox *checkBoxTriple;
    QGroupBox *groupBox_6;
    QVBoxLayout *verticalLayout_9;
    QCheckBox *checkBox_absorbance;
    QCheckBox *checkBoxExperimental;
    QWidget *widget;
    QGridLayout *gridLayout_15;
    QToolButton *toolButton_SaveCutnum;
    QWidget *widget_loginpassword;
    QGridLayout *gridLayout_4;
    QSpacerItem *horizontalSpacer_3;
    QSpacerItem *horizontalSpacer_4;
    QGridLayout *gridLayout_3;
    QLabel *label_maintenance;
    QLineEdit *lineEdit_maintenance;
    QPushButton *pushButton_maintenance;
    QWidget *tabPE;

    void setupUi(QWidget *MachineSetting)
    {
        if (MachineSetting->objectName().isEmpty())
            MachineSetting->setObjectName(QStringLiteral("MachineSetting"));
        MachineSetting->setWindowModality(Qt::NonModal);
        MachineSetting->resize(1359, 895);
        MachineSetting->setMinimumSize(QSize(1300, 400));
        MachineSetting->setMaximumSize(QSize(1800, 16777215));
        horizontalLayout_configureset = new QHBoxLayout(MachineSetting);
        horizontalLayout_configureset->setSpacing(0);
        horizontalLayout_configureset->setObjectName(QStringLiteral("horizontalLayout_configureset"));
        horizontalLayout_configureset->setContentsMargins(5, 5, 5, 5);
        MachineSetList = new QTreeWidget(MachineSetting);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QStringLiteral("1"));
        MachineSetList->setHeaderItem(__qtreewidgetitem);
        MachineSetList->setObjectName(QStringLiteral("MachineSetList"));
        MachineSetList->setMinimumSize(QSize(170, 0));
        MachineSetList->setMaximumSize(QSize(220, 16777215));
        MachineSetList->setStyleSheet(QLatin1String("QTreeWidget\n"
"{\n"
"    background-color:rgba(188,187,183);\n"
"    font-size:22px;\n"
"    color: white;\n"
"}\n"
"QTreeWidget::item\n"
"{\n"
"    margin:5px;\n"
"    background: rgba(188,187,183);\n"
"    background-clip: margin;\n"
"}\n"
"QTreeWidget::branch\n"
"{\n"
"    background:rgba(188,187,183);\n"
"}\n"
" \n"
"QTreeView::item:hover \n"
"{\n"
"  background: rgb(69, 187, 217);\n"
"}\n"
" \n"
"QTreeView::item:selected:active{\n"
"    background: rgb(63, 147, 168);\n"
"}\n"
" \n"
"QTreeView::item:selected:!active {\n"
"    background: rgb(63, 147, 168);\n"
"}\n"
"QTreeWidget::branch:closed:has-children:!has-siblings,\n"
"QTreeWidget::branch:closed:has-children:has-siblings {\n"
"    border-image: none;\n"
"    image: url(:/Picture/plus.png);\n"
"}\n"
" \n"
" \n"
"QTreeWidget::branch:open:has-children:!has-siblings,\n"
"QTreeWidget::branch:open:has-children:has-siblings  {\n"
"    border-image: none;\n"
"    image: url(:/Picture/minus.png);\n"
"}\n"
""));

        horizontalLayout_configureset->addWidget(MachineSetList);

        tabWidgetSetconfigure = new QTabWidget(MachineSetting);
        tabWidgetSetconfigure->setObjectName(QStringLiteral("tabWidgetSetconfigure"));
        QFont font;
        font.setPointSize(12);
        tabWidgetSetconfigure->setFont(font);
        tabWidgetSetconfigure->setStyleSheet(QStringLiteral("background-color: rgb(180, 180, 180);"));
        tabWidgetSetconfigure->setDocumentMode(false);
        tabWidgetSetconfigure->setTabsClosable(false);
        tabWidgetSetconfigure->setMovable(false);
        tab_1 = new QWidget();
        tab_1->setObjectName(QStringLiteral("tab_1"));
        verticalLayout_8 = new QVBoxLayout(tab_1);
        verticalLayout_8->setSpacing(5);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(5, 5, 5, 5);
        groupBox_usermanagement = new QGroupBox(tab_1);
        groupBox_usermanagement->setObjectName(QStringLiteral("groupBox_usermanagement"));
        groupBox_usermanagement->setMaximumSize(QSize(16777215, 240));
        groupBox_usermanagement->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        horizontalLayout_2 = new QHBoxLayout(groupBox_usermanagement);
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(10, 5, 10, 5);
        listWidget_user = new QListWidget(groupBox_usermanagement);
        listWidget_user->setObjectName(QStringLiteral("listWidget_user"));
        listWidget_user->setMaximumSize(QSize(16777215, 200));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(14);
        listWidget_user->setFont(font1);

        horizontalLayout_2->addWidget(listWidget_user);

        widget_Btngroup = new QWidget(groupBox_usermanagement);
        widget_Btngroup->setObjectName(QStringLiteral("widget_Btngroup"));
        widget_Btngroup->setMaximumSize(QSize(16777215, 200));
        verticalLayout_2 = new QVBoxLayout(widget_Btngroup);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        pushButton_ADD = new QPushButton(widget_Btngroup);
        pushButton_ADD->setObjectName(QStringLiteral("pushButton_ADD"));
        pushButton_ADD->setMinimumSize(QSize(0, 30));
        pushButton_ADD->setMaximumSize(QSize(120, 16777215));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        pushButton_ADD->setFont(font2);
        pushButton_ADD->setStyleSheet(QStringLiteral(""));
        pushButton_ADD->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(pushButton_ADD);

        pushButton_DELETE = new QPushButton(widget_Btngroup);
        pushButton_DELETE->setObjectName(QStringLiteral("pushButton_DELETE"));
        pushButton_DELETE->setMinimumSize(QSize(0, 30));
        pushButton_DELETE->setMaximumSize(QSize(120, 16777215));
        pushButton_DELETE->setFont(font2);
        pushButton_DELETE->setStyleSheet(QStringLiteral(""));
        pushButton_DELETE->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(pushButton_DELETE);

        verticalSpacer = new QSpacerItem(20, 21, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        pushButton_CHANGE = new QPushButton(widget_Btngroup);
        pushButton_CHANGE->setObjectName(QStringLiteral("pushButton_CHANGE"));
        pushButton_CHANGE->setMinimumSize(QSize(0, 30));
        pushButton_CHANGE->setMaximumSize(QSize(120, 16777215));
        pushButton_CHANGE->setFont(font2);
        pushButton_CHANGE->setStyleSheet(QStringLiteral(""));
        pushButton_CHANGE->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(pushButton_CHANGE);

        pushButton_SWITCH = new QPushButton(widget_Btngroup);
        pushButton_SWITCH->setObjectName(QStringLiteral("pushButton_SWITCH"));
        pushButton_SWITCH->setMinimumSize(QSize(0, 30));
        pushButton_SWITCH->setMaximumSize(QSize(120, 16777215));
        pushButton_SWITCH->setFont(font2);
        pushButton_SWITCH->setStyleSheet(QStringLiteral(""));
        pushButton_SWITCH->setIconSize(QSize(20, 20));

        verticalLayout_2->addWidget(pushButton_SWITCH);


        horizontalLayout_2->addWidget(widget_Btngroup);


        verticalLayout_8->addWidget(groupBox_usermanagement);

        groupBox_printReport = new QGroupBox(tab_1);
        groupBox_printReport->setObjectName(QStringLiteral("groupBox_printReport"));
        groupBox_printReport->setMaximumSize(QSize(16777215, 240));
        groupBox_printReport->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        gridLayout_printinfo = new QGridLayout(groupBox_printReport);
        gridLayout_printinfo->setSpacing(5);
        gridLayout_printinfo->setObjectName(QStringLiteral("gridLayout_printinfo"));
        gridLayout_printinfo->setContentsMargins(5, 5, 5, 5);
        checkBox_Automatic_print = new QCheckBox(groupBox_printReport);
        checkBox_Automatic_print->setObjectName(QStringLiteral("checkBox_Automatic_print"));
        checkBox_Automatic_print->setMinimumSize(QSize(140, 0));
        checkBox_Automatic_print->setMaximumSize(QSize(16777215, 30));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font3.setPointSize(12);
        checkBox_Automatic_print->setFont(font3);
        checkBox_Automatic_print->setStyleSheet(QStringLiteral(""));

        gridLayout_printinfo->addWidget(checkBox_Automatic_print, 4, 2, 1, 1);

        comboBox_QC_sample_report = new QComboBox(groupBox_printReport);
        comboBox_QC_sample_report->setObjectName(QStringLiteral("comboBox_QC_sample_report"));
        comboBox_QC_sample_report->setMaximumSize(QSize(16777215, 30));
        QFont font4;
        comboBox_QC_sample_report->setFont(font4);
        comboBox_QC_sample_report->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"  color:#666666;\n"
"  font-size:24px;\n"
"  padding: 1px 15px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
"}\n"
" QComboBox::drop-down {\n"
"      subcontrol-origin: padding;\n"
"      subcontrol-position: top right;\n"
"      width: 15px;\n"
"      border:none;\n"
"}\n"
"QComboBox::down-arrow {\n"
"      image: url(:/Picture/minus.png);\n"
"  }\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font-size:24px;\n"
"    outline: 0px;  //\345\216\273\350\231\232\347\272\277\n"
"}\n"
"QComboBox QAbstractItemView::item{\n"
"	height:36px;\n"
"	color:#666666;\n"
"	padding-left:9px;\n"
"	background-color:#FFFFFF;\n"
"}\n"
"QComboBox QAbstractItemView::item:hover{ //\346\202\254\346\265\256\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#40"
                        "9CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 4px;\n"
"  }\n"
"  QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 1px;\n"
"}"));
        comboBox_QC_sample_report->setMaxVisibleItems(30);

        gridLayout_printinfo->addWidget(comboBox_QC_sample_report, 2, 2, 1, 1);

        comboBox_Patient_sample_report = new QComboBox(groupBox_printReport);
        comboBox_Patient_sample_report->setObjectName(QStringLiteral("comboBox_Patient_sample_report"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBox_Patient_sample_report->sizePolicy().hasHeightForWidth());
        comboBox_Patient_sample_report->setSizePolicy(sizePolicy);
        comboBox_Patient_sample_report->setMaximumSize(QSize(16777215, 30));
        comboBox_Patient_sample_report->setFont(font4);
        comboBox_Patient_sample_report->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"  color:#666666;\n"
"  font-size:24px;\n"
"  padding: 1px 15px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
"}\n"
" QComboBox::drop-down {\n"
"      subcontrol-origin: padding;\n"
"      subcontrol-position: top right;\n"
"      width: 15px;\n"
"      border:none;\n"
"}\n"
"QComboBox::down-arrow {\n"
"      image: url(:/Picture/minus.png);\n"
"  }\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font-size:24px;\n"
"    outline: 0px;  //\345\216\273\350\231\232\347\272\277\n"
"}\n"
"QComboBox QAbstractItemView::item{\n"
"	height:36px;\n"
"	color:#666666;\n"
"	padding-left:9px;\n"
"	background-color:#FFFFFF;\n"
"}\n"
"QComboBox QAbstractItemView::item:hover{ //\346\202\254\346\265\256\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#40"
                        "9CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 4px;\n"
"  }\n"
"  QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 1px;\n"
"}"));
        comboBox_Patient_sample_report->setMaxVisibleItems(30);

        gridLayout_printinfo->addWidget(comboBox_Patient_sample_report, 1, 2, 1, 1);

        label_Tab0_hospital_name = new QLabel(groupBox_printReport);
        label_Tab0_hospital_name->setObjectName(QStringLiteral("label_Tab0_hospital_name"));
        label_Tab0_hospital_name->setMinimumSize(QSize(100, 30));
        label_Tab0_hospital_name->setMaximumSize(QSize(16777215, 30));
        label_Tab0_hospital_name->setFont(font3);
        label_Tab0_hospital_name->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_printinfo->addWidget(label_Tab0_hospital_name, 0, 0, 1, 1);

        label_QC_sample_report = new QLabel(groupBox_printReport);
        label_QC_sample_report->setObjectName(QStringLiteral("label_QC_sample_report"));
        label_QC_sample_report->setMaximumSize(QSize(140, 30));
        label_QC_sample_report->setFont(font3);

        gridLayout_printinfo->addWidget(label_QC_sample_report, 2, 0, 1, 1);

        checkBox_Automatic_review = new QCheckBox(groupBox_printReport);
        checkBox_Automatic_review->setObjectName(QStringLiteral("checkBox_Automatic_review"));
        checkBox_Automatic_review->setMinimumSize(QSize(140, 0));
        checkBox_Automatic_review->setMaximumSize(QSize(16777215, 30));
        checkBox_Automatic_review->setFont(font3);
        checkBox_Automatic_review->setStyleSheet(QStringLiteral(""));

        gridLayout_printinfo->addWidget(checkBox_Automatic_review, 3, 2, 1, 1);

        lineEdit_Tab0_hospital_name = new QLineEdit(groupBox_printReport);
        lineEdit_Tab0_hospital_name->setObjectName(QStringLiteral("lineEdit_Tab0_hospital_name"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lineEdit_Tab0_hospital_name->sizePolicy().hasHeightForWidth());
        lineEdit_Tab0_hospital_name->setSizePolicy(sizePolicy1);
        lineEdit_Tab0_hospital_name->setMinimumSize(QSize(0, 30));
        lineEdit_Tab0_hospital_name->setMaximumSize(QSize(16777215, 30));
        QFont font5;
        font5.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font5.setPointSize(15);
        font5.setBold(false);
        font5.setItalic(false);
        font5.setWeight(50);
        lineEdit_Tab0_hospital_name->setFont(font5);
        lineEdit_Tab0_hospital_name->setStyleSheet(QStringLiteral(""));
        lineEdit_Tab0_hospital_name->setAlignment(Qt::AlignCenter);

        gridLayout_printinfo->addWidget(lineEdit_Tab0_hospital_name, 0, 2, 1, 1);

        label_Patient_sample_report = new QLabel(groupBox_printReport);
        label_Patient_sample_report->setObjectName(QStringLiteral("label_Patient_sample_report"));
        label_Patient_sample_report->setMaximumSize(QSize(140, 30));
        label_Patient_sample_report->setFont(font3);

        gridLayout_printinfo->addWidget(label_Patient_sample_report, 1, 0, 1, 1);


        verticalLayout_8->addWidget(groupBox_printReport);

        tabWidgetSetconfigure->addTab(tab_1, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_tableWidget_info_1 = new QVBoxLayout(tab_2);
        verticalLayout_tableWidget_info_1->setSpacing(0);
        verticalLayout_tableWidget_info_1->setObjectName(QStringLiteral("verticalLayout_tableWidget_info_1"));
        verticalLayout_tableWidget_info_1->setContentsMargins(0, 0, 0, 0);
        widget_Info_0 = new QWidget(tab_2);
        widget_Info_0->setObjectName(QStringLiteral("widget_Info_0"));
        widget_Info_0->setMinimumSize(QSize(0, 50));
        horizontalLayout_11 = new QHBoxLayout(widget_Info_0);
        horizontalLayout_11->setSpacing(5);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        horizontalLayout_11->setContentsMargins(5, 5, 5, 10);
        label_Category = new QLabel(widget_Info_0);
        label_Category->setObjectName(QStringLiteral("label_Category"));
        label_Category->setMinimumSize(QSize(0, 40));
        label_Category->setMaximumSize(QSize(16777215, 30));
        QFont font6;
        font6.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font6.setPointSize(12);
        label_Category->setFont(font6);

        horizontalLayout_11->addWidget(label_Category);

        comboBox_Category = new QComboBox(widget_Info_0);
        comboBox_Category->setObjectName(QStringLiteral("comboBox_Category"));
        comboBox_Category->setMinimumSize(QSize(140, 40));
        comboBox_Category->setMaximumSize(QSize(16777215, 30));
        comboBox_Category->setFont(font4);
        comboBox_Category->setStyleSheet(QStringLiteral(""));
        comboBox_Category->setMaxVisibleItems(30);

        horizontalLayout_11->addWidget(comboBox_Category);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer);

        toolButton_viewSQL = new QToolButton(widget_Info_0);
        toolButton_viewSQL->setObjectName(QStringLiteral("toolButton_viewSQL"));
        toolButton_viewSQL->setMinimumSize(QSize(80, 40));
        toolButton_viewSQL->setFont(font6);

        horizontalLayout_11->addWidget(toolButton_viewSQL);


        verticalLayout_tableWidget_info_1->addWidget(widget_Info_0);

        widget_Info = new QWidget(tab_2);
        widget_Info->setObjectName(QStringLiteral("widget_Info"));
        horizontalLayout_3 = new QHBoxLayout(widget_Info);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_abbreviation = new QVBoxLayout();
        verticalLayout_abbreviation->setSpacing(0);
        verticalLayout_abbreviation->setObjectName(QStringLiteral("verticalLayout_abbreviation"));
        label_Abbreviation = new QLabel(widget_Info);
        label_Abbreviation->setObjectName(QStringLiteral("label_Abbreviation"));
        label_Abbreviation->setMinimumSize(QSize(0, 30));
        label_Abbreviation->setMaximumSize(QSize(16777215, 30));
        label_Abbreviation->setFont(font6);

        verticalLayout_abbreviation->addWidget(label_Abbreviation);

        lineEdit_Abbreviation = new QLineEdit(widget_Info);
        lineEdit_Abbreviation->setObjectName(QStringLiteral("lineEdit_Abbreviation"));
        lineEdit_Abbreviation->setMinimumSize(QSize(0, 30));
        lineEdit_Abbreviation->setMaximumSize(QSize(16777215, 35));
        QFont font7;
        font7.setFamily(QStringLiteral("Arial"));
        font7.setPointSize(12);
        lineEdit_Abbreviation->setFont(font7);

        verticalLayout_abbreviation->addWidget(lineEdit_Abbreviation);


        horizontalLayout_3->addLayout(verticalLayout_abbreviation);

        verticalLayout_name = new QVBoxLayout();
        verticalLayout_name->setSpacing(0);
        verticalLayout_name->setObjectName(QStringLiteral("verticalLayout_name"));
        label_Name = new QLabel(widget_Info);
        label_Name->setObjectName(QStringLiteral("label_Name"));
        label_Name->setMinimumSize(QSize(0, 30));
        label_Name->setMaximumSize(QSize(16777215, 30));
        label_Name->setFont(font6);

        verticalLayout_name->addWidget(label_Name);

        lineEdit_Name = new QLineEdit(widget_Info);
        lineEdit_Name->setObjectName(QStringLiteral("lineEdit_Name"));
        lineEdit_Name->setMinimumSize(QSize(0, 30));
        lineEdit_Name->setMaximumSize(QSize(16777215, 30));
        lineEdit_Name->setFont(font7);

        verticalLayout_name->addWidget(lineEdit_Name);


        horizontalLayout_3->addLayout(verticalLayout_name);


        verticalLayout_tableWidget_info_1->addWidget(widget_Info);

        tabWidget_info = new QTabWidget(tab_2);
        tabWidget_info->setObjectName(QStringLiteral("tabWidget_info"));
        QFont font8;
        font8.setPointSize(14);
        tabWidget_info->setFont(font8);
        tabWidget_info->setStyleSheet(QString::fromUtf8("QTabWidget::pane{\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	background:rgb(246, 246, 246);\n"
"	border-top-color:transparent;\n"
"}\n"
"QTabWidget::tab-bar{\n"
"	background:rgb(0, 0, 0);\n"
"	subcontrol-position:left;\n"
"}\n"
"QTabBar::tab{\n"
"	width:173px;/*\345\256\275\345\272\246\346\240\271\346\215\256\345\256\236\351\231\205\351\234\200\350\246\201\350\277\233\350\241\214\350\260\203\346\225\264*/\n"
"	height:45px;\n"
"	background:rgb(210, 210, 210);\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	border-top-left-radius: 8px;\n"
"	border-top-right-radius: 8px;\n"
"}\n"
"QTabBar::tab:selected{	\n"
"	background:rgb(246, 246, 246);\n"
"	border-bottom-color:rgb(246, 246, 246);\n"
"}\n"
"QTabBar::tab:!selected{\n"
"	background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(240, 240, 240, 255), stop:0.5 rgba(210, 210, 210, 255), stop:1 rgba(225, 225, 225, 255));\n"
"}\n"
" "));
        tab_info = new QWidget();
        tab_info->setObjectName(QStringLiteral("tab_info"));
        verticalLayout_tableWidget_info = new QVBoxLayout(tab_info);
        verticalLayout_tableWidget_info->setSpacing(0);
        verticalLayout_tableWidget_info->setObjectName(QStringLiteral("verticalLayout_tableWidget_info"));
        verticalLayout_tableWidget_info->setContentsMargins(0, 0, 0, 0);
        tableWidget_info = new QTableWidget(tab_info);
        tableWidget_info->setObjectName(QStringLiteral("tableWidget_info"));
        tableWidget_info->setFont(font6);
        tableWidget_info->setStyleSheet(QString::fromUtf8("QTabWidget::pane{\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	background:rgb(246, 246, 246);\n"
"	border-top-color:transparent;\n"
"}\n"
"QTabWidget::tab-bar{\n"
"	background:rgb(0, 0, 0);\n"
"	subcontrol-position:left;\n"
"}\n"
"QTabBar::tab{\n"
"	width:173px;/*\345\256\275\345\272\246\346\240\271\346\215\256\345\256\236\351\231\205\351\234\200\350\246\201\350\277\233\350\241\214\350\260\203\346\225\264*/\n"
"	height:45px;\n"
"	background:rgb(210, 210, 210);\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	border-top-left-radius: 8px;\n"
"	border-top-right-radius: 8px;\n"
"}\n"
"QTabBar::tab:selected{	\n"
"	background:rgb(246, 246, 246);\n"
"	border-bottom-color:rgb(246, 246, 246);\n"
"}\n"
"QTabBar::tab:!selected{\n"
"	background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(240, 240, 240, 255), stop:0.5 rgba(210, 210, 210, 255), stop:1 rgba(225, 225, 225, 255));\n"
"}\n"
" "));

        verticalLayout_tableWidget_info->addWidget(tableWidget_info);

        tabWidget_info->addTab(tab_info, QString());
        tab_info_1 = new QWidget();
        tab_info_1->setObjectName(QStringLiteral("tab_info_1"));
        horizontalLayout_tableWidget_info_1 = new QHBoxLayout(tab_info_1);
        horizontalLayout_tableWidget_info_1->setSpacing(0);
        horizontalLayout_tableWidget_info_1->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_1"));
        horizontalLayout_tableWidget_info_1->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_1 = new QTableWidget(tab_info_1);
        tableWidget_info_1->setObjectName(QStringLiteral("tableWidget_info_1"));
        tableWidget_info_1->setFont(font7);

        horizontalLayout_tableWidget_info_1->addWidget(tableWidget_info_1);

        tabWidget_info->addTab(tab_info_1, QString());
        tab_info_2 = new QWidget();
        tab_info_2->setObjectName(QStringLiteral("tab_info_2"));
        horizontalLayout_tableWidget_info_2 = new QHBoxLayout(tab_info_2);
        horizontalLayout_tableWidget_info_2->setSpacing(0);
        horizontalLayout_tableWidget_info_2->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_2"));
        horizontalLayout_tableWidget_info_2->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_2 = new QTableWidget(tab_info_2);
        tableWidget_info_2->setObjectName(QStringLiteral("tableWidget_info_2"));
        tableWidget_info_2->setFont(font7);

        horizontalLayout_tableWidget_info_2->addWidget(tableWidget_info_2);

        tabWidget_info->addTab(tab_info_2, QString());
        tab_info_3 = new QWidget();
        tab_info_3->setObjectName(QStringLiteral("tab_info_3"));
        horizontalLayout_tableWidget_info_3 = new QHBoxLayout(tab_info_3);
        horizontalLayout_tableWidget_info_3->setSpacing(0);
        horizontalLayout_tableWidget_info_3->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_3"));
        horizontalLayout_tableWidget_info_3->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_3 = new QTableWidget(tab_info_3);
        tableWidget_info_3->setObjectName(QStringLiteral("tableWidget_info_3"));
        tableWidget_info_3->setFont(font7);

        horizontalLayout_tableWidget_info_3->addWidget(tableWidget_info_3);

        tabWidget_info->addTab(tab_info_3, QString());
        tab_info_4 = new QWidget();
        tab_info_4->setObjectName(QStringLiteral("tab_info_4"));
        horizontalLayout_tableWidget_info_4 = new QHBoxLayout(tab_info_4);
        horizontalLayout_tableWidget_info_4->setSpacing(0);
        horizontalLayout_tableWidget_info_4->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_4"));
        horizontalLayout_tableWidget_info_4->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_4 = new QTableWidget(tab_info_4);
        tableWidget_info_4->setObjectName(QStringLiteral("tableWidget_info_4"));
        tableWidget_info_4->setFont(font7);

        horizontalLayout_tableWidget_info_4->addWidget(tableWidget_info_4);

        tabWidget_info->addTab(tab_info_4, QString());
        tab_info_5 = new QWidget();
        tab_info_5->setObjectName(QStringLiteral("tab_info_5"));
        horizontalLayout_tableWidget_info_5 = new QHBoxLayout(tab_info_5);
        horizontalLayout_tableWidget_info_5->setSpacing(0);
        horizontalLayout_tableWidget_info_5->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_5"));
        horizontalLayout_tableWidget_info_5->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_5 = new QTableWidget(tab_info_5);
        tableWidget_info_5->setObjectName(QStringLiteral("tableWidget_info_5"));
        tableWidget_info_5->setFont(font7);

        horizontalLayout_tableWidget_info_5->addWidget(tableWidget_info_5);

        tabWidget_info->addTab(tab_info_5, QString());
        tab_info_6 = new QWidget();
        tab_info_6->setObjectName(QStringLiteral("tab_info_6"));
        horizontalLayout_tableWidget_info_6 = new QHBoxLayout(tab_info_6);
        horizontalLayout_tableWidget_info_6->setSpacing(0);
        horizontalLayout_tableWidget_info_6->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_6"));
        horizontalLayout_tableWidget_info_6->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_6 = new QTableWidget(tab_info_6);
        tableWidget_info_6->setObjectName(QStringLiteral("tableWidget_info_6"));
        tableWidget_info_6->setFont(font7);

        horizontalLayout_tableWidget_info_6->addWidget(tableWidget_info_6);

        tabWidget_info->addTab(tab_info_6, QString());
        tab_info_7 = new QWidget();
        tab_info_7->setObjectName(QStringLiteral("tab_info_7"));
        horizontalLayout_tableWidget_info_7 = new QHBoxLayout(tab_info_7);
        horizontalLayout_tableWidget_info_7->setSpacing(0);
        horizontalLayout_tableWidget_info_7->setObjectName(QStringLiteral("horizontalLayout_tableWidget_info_7"));
        horizontalLayout_tableWidget_info_7->setContentsMargins(0, 0, 0, 0);
        tableWidget_info_7 = new QTableWidget(tab_info_7);
        tableWidget_info_7->setObjectName(QStringLiteral("tableWidget_info_7"));
        tableWidget_info_7->setFont(font7);

        horizontalLayout_tableWidget_info_7->addWidget(tableWidget_info_7);

        tabWidget_info->addTab(tab_info_7, QString());

        verticalLayout_tableWidget_info_1->addWidget(tabWidget_info);

        widget_info2 = new QWidget(tab_2);
        widget_info2->setObjectName(QStringLiteral("widget_info2"));
        widget_info2->setMinimumSize(QSize(0, 35));
        widget_info2->setMaximumSize(QSize(16777215, 40));
        horizontalLayout_but = new QHBoxLayout(widget_info2);
        horizontalLayout_but->setSpacing(5);
        horizontalLayout_but->setObjectName(QStringLiteral("horizontalLayout_but"));
        horizontalLayout_but->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_but->addItem(horizontalSpacer_2);

        toolButton_ADD = new QToolButton(widget_info2);
        toolButton_ADD->setObjectName(QStringLiteral("toolButton_ADD"));
        toolButton_ADD->setMinimumSize(QSize(80, 30));
        toolButton_ADD->setMaximumSize(QSize(16777215, 35));
        toolButton_ADD->setFont(font7);

        horizontalLayout_but->addWidget(toolButton_ADD);

        toolButton_Delete = new QToolButton(widget_info2);
        toolButton_Delete->setObjectName(QStringLiteral("toolButton_Delete"));
        toolButton_Delete->setMinimumSize(QSize(80, 30));
        toolButton_Delete->setMaximumSize(QSize(16777215, 30));
        toolButton_Delete->setFont(font7);

        horizontalLayout_but->addWidget(toolButton_Delete);


        verticalLayout_tableWidget_info_1->addWidget(widget_info2);

        tabWidgetSetconfigure->addTab(tab_2, QString());
        MachineTestConfigure = new QWidget();
        MachineTestConfigure->setObjectName(QStringLiteral("MachineTestConfigure"));
        gridLayout_12 = new QGridLayout(MachineTestConfigure);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        disposition_value = new QGroupBox(MachineTestConfigure);
        disposition_value->setObjectName(QStringLiteral("disposition_value"));
        disposition_value->setMinimumSize(QSize(0, 140));
        disposition_value->setMaximumSize(QSize(16777215, 140));
        QFont font9;
        font9.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font9.setBold(true);
        font9.setItalic(false);
        font9.setWeight(75);
        disposition_value->setFont(font9);
        disposition_value->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
" \n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled {\n"
"	border: 1px solid gray;\n"
"}\n"
""));
        gridLayout = new QGridLayout(disposition_value);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(10);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        UseBarCode = new QCheckBox(disposition_value);
        UseBarCode->setObjectName(QStringLiteral("UseBarCode"));
        UseBarCode->setMinimumSize(QSize(0, 30));
        UseBarCode->setFont(font6);
        UseBarCode->setStyleSheet(QStringLiteral(""));
        UseBarCode->setCheckable(true);

        horizontalLayout_13->addWidget(UseBarCode);

        checkBox_UesSecondReagentHole = new QCheckBox(disposition_value);
        checkBox_UesSecondReagentHole->setObjectName(QStringLiteral("checkBox_UesSecondReagentHole"));
        checkBox_UesSecondReagentHole->setMinimumSize(QSize(0, 30));
        checkBox_UesSecondReagentHole->setFont(font6);
        checkBox_UesSecondReagentHole->setStyleSheet(QStringLiteral(""));

        horizontalLayout_13->addWidget(checkBox_UesSecondReagentHole);

        checkBox_suck_offset = new QCheckBox(disposition_value);
        checkBox_suck_offset->setObjectName(QStringLiteral("checkBox_suck_offset"));
        checkBox_suck_offset->setFont(font6);
        checkBox_suck_offset->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { \n"
"    width:  16px;\n"
"    height: 32px;\n"
"	\n"
"}\n"
"/*\346\234\252\351\200\211\344\270\255*/\n"
"QCheckBox::indicator::unchecked {   \n"
"    image: url(:/Picture/check_box_unchecked.png);\n"
"}\n"
"/*\351\200\211\344\270\255*/\n"
"QCheckBox::indicator::checked { \n"
"    image: url(:/Picture/check-box-checked.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:hover {\n"
"        image: url(:/Picture/check_box_hove.png);\n"
"}"));

        horizontalLayout_13->addWidget(checkBox_suck_offset);

        checkBox_catchcups = new QCheckBox(disposition_value);
        checkBox_catchcups->setObjectName(QStringLiteral("checkBox_catchcups"));
        checkBox_catchcups->setMinimumSize(QSize(0, 30));
        checkBox_catchcups->setFont(font6);
        checkBox_catchcups->setStyleSheet(QStringLiteral(""));

        horizontalLayout_13->addWidget(checkBox_catchcups);


        gridLayout->addLayout(horizontalLayout_13, 0, 0, 1, 2);


        gridLayout_12->addWidget(disposition_value, 0, 0, 1, 2);

        okandcancelbtn = new QWidget(MachineTestConfigure);
        okandcancelbtn->setObjectName(QStringLiteral("okandcancelbtn"));
        okandcancelbtn->setMinimumSize(QSize(0, 70));
        okandcancelbtn->setMaximumSize(QSize(16777215, 70));
        horizontalLayout_5 = new QHBoxLayout(okandcancelbtn);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalSpacer_13 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_13);

        Machine_conf_save = new QToolButton(okandcancelbtn);
        Machine_conf_save->setObjectName(QStringLiteral("Machine_conf_save"));
        Machine_conf_save->setMaximumSize(QSize(16777215, 50));
        Machine_conf_save->setStyleSheet(QStringLiteral(""));
        Machine_conf_save->setIconSize(QSize(32, 32));
        Machine_conf_save->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_5->addWidget(Machine_conf_save);

        toolButton_Cancel = new QToolButton(okandcancelbtn);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMaximumSize(QSize(16777215, 50));
        toolButton_Cancel->setStyleSheet(QStringLiteral(""));
        toolButton_Cancel->setIconSize(QSize(32, 32));
        toolButton_Cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_5->addWidget(toolButton_Cancel);

        horizontalSpacer_14 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_14);


        gridLayout_12->addWidget(okandcancelbtn, 3, 0, 1, 2);

        usechannel = new QGroupBox(MachineTestConfigure);
        usechannel->setObjectName(QStringLiteral("usechannel"));
        usechannel->setMaximumSize(QSize(16777215, 122222));
        usechannel->setFont(font9);
        usechannel->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
" \n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled {\n"
"	border: 1px solid gray;\n"
"}\n"
""));
        gridLayout_11 = new QGridLayout(usechannel);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        gridLayout_11->setHorizontalSpacing(30);
        gridLayout_11->setVerticalSpacing(10);
        gridLayout_11->setContentsMargins(50, 10, 10, 10);
        Channel_State_1 = new QCheckBox(usechannel);
        Channel_State_1->setObjectName(QStringLiteral("Channel_State_1"));
        Channel_State_1->setMinimumSize(QSize(0, 64));
        Channel_State_1->setFont(font1);
        Channel_State_1->setStyleSheet(QStringLiteral(""));
        Channel_State_1->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_1, 0, 0, 1, 1);

        Channel_State_5 = new QCheckBox(usechannel);
        Channel_State_5->setObjectName(QStringLiteral("Channel_State_5"));
        Channel_State_5->setMinimumSize(QSize(0, 64));
        Channel_State_5->setFont(font1);
        Channel_State_5->setStyleSheet(QStringLiteral(""));
        Channel_State_5->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_5, 0, 1, 1, 1);

        Channel_State_9 = new QCheckBox(usechannel);
        Channel_State_9->setObjectName(QStringLiteral("Channel_State_9"));
        Channel_State_9->setMinimumSize(QSize(0, 64));
        Channel_State_9->setFont(font1);
        Channel_State_9->setStyleSheet(QStringLiteral(""));

        gridLayout_11->addWidget(Channel_State_9, 0, 2, 1, 1);

        Channel_State_2 = new QCheckBox(usechannel);
        Channel_State_2->setObjectName(QStringLiteral("Channel_State_2"));
        Channel_State_2->setMinimumSize(QSize(0, 64));
        Channel_State_2->setFont(font1);
        Channel_State_2->setStyleSheet(QStringLiteral(""));
        Channel_State_2->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_2, 1, 0, 1, 1);

        Channel_State_6 = new QCheckBox(usechannel);
        Channel_State_6->setObjectName(QStringLiteral("Channel_State_6"));
        Channel_State_6->setMinimumSize(QSize(0, 64));
        Channel_State_6->setFont(font1);
        Channel_State_6->setStyleSheet(QStringLiteral(""));
        Channel_State_6->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_6, 1, 1, 1, 1);

        Channel_State_10 = new QCheckBox(usechannel);
        Channel_State_10->setObjectName(QStringLiteral("Channel_State_10"));
        Channel_State_10->setMinimumSize(QSize(0, 64));
        Channel_State_10->setFont(font1);
        Channel_State_10->setStyleSheet(QStringLiteral(""));

        gridLayout_11->addWidget(Channel_State_10, 1, 2, 1, 1);

        Channel_State_3 = new QCheckBox(usechannel);
        Channel_State_3->setObjectName(QStringLiteral("Channel_State_3"));
        Channel_State_3->setMinimumSize(QSize(0, 64));
        Channel_State_3->setFont(font1);
        Channel_State_3->setStyleSheet(QStringLiteral(""));
        Channel_State_3->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_3, 2, 0, 1, 1);

        Channel_State_7 = new QCheckBox(usechannel);
        Channel_State_7->setObjectName(QStringLiteral("Channel_State_7"));
        Channel_State_7->setMinimumSize(QSize(0, 64));
        Channel_State_7->setFont(font1);
        Channel_State_7->setStyleSheet(QStringLiteral(""));
        Channel_State_7->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_7, 2, 1, 1, 1);

        Channel_State_11 = new QCheckBox(usechannel);
        Channel_State_11->setObjectName(QStringLiteral("Channel_State_11"));
        Channel_State_11->setMinimumSize(QSize(0, 64));
        Channel_State_11->setFont(font1);
        Channel_State_11->setStyleSheet(QStringLiteral(""));

        gridLayout_11->addWidget(Channel_State_11, 2, 2, 1, 1);

        Channel_State_4 = new QCheckBox(usechannel);
        Channel_State_4->setObjectName(QStringLiteral("Channel_State_4"));
        Channel_State_4->setMinimumSize(QSize(0, 64));
        Channel_State_4->setFont(font1);
        Channel_State_4->setStyleSheet(QStringLiteral(""));
        Channel_State_4->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_4, 3, 0, 1, 1);

        Channel_State_8 = new QCheckBox(usechannel);
        Channel_State_8->setObjectName(QStringLiteral("Channel_State_8"));
        Channel_State_8->setMinimumSize(QSize(0, 64));
        Channel_State_8->setFont(font1);
        Channel_State_8->setStyleSheet(QStringLiteral(""));
        Channel_State_8->setIconSize(QSize(32, 32));

        gridLayout_11->addWidget(Channel_State_8, 3, 1, 1, 1);

        Channel_State_12 = new QCheckBox(usechannel);
        Channel_State_12->setObjectName(QStringLiteral("Channel_State_12"));
        Channel_State_12->setMinimumSize(QSize(0, 64));
        Channel_State_12->setFont(font1);
        Channel_State_12->setStyleSheet(QStringLiteral(""));

        gridLayout_11->addWidget(Channel_State_12, 3, 2, 1, 1);


        gridLayout_12->addWidget(usechannel, 1, 0, 1, 2);

        groupBox = new QGroupBox(MachineTestConfigure);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 120));
        groupBox->setMaximumSize(QSize(1222222, 160));
        groupBox->setFont(font9);
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
" \n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled \n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
""));
        gridLayout_18 = new QGridLayout(groupBox);
        gridLayout_18->setObjectName(QStringLiteral("gridLayout_18"));
        label_Moduletemperature_1 = new QLabel(groupBox);
        label_Moduletemperature_1->setObjectName(QStringLiteral("label_Moduletemperature_1"));
        label_Moduletemperature_1->setMinimumSize(QSize(0, 30));
        label_Moduletemperature_1->setMaximumSize(QSize(120, 30));
        label_Moduletemperature_1->setFont(font1);
        label_Moduletemperature_1->setAlignment(Qt::AlignCenter);

        gridLayout_18->addWidget(label_Moduletemperature_1, 0, 0, 1, 1);

        doubleSpinBox_Moduletemperature_1 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_Moduletemperature_1->setObjectName(QStringLiteral("doubleSpinBox_Moduletemperature_1"));
        doubleSpinBox_Moduletemperature_1->setMinimumSize(QSize(160, 30));
        doubleSpinBox_Moduletemperature_1->setMaximumSize(QSize(180, 30));
        QFont font10;
        font10.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        doubleSpinBox_Moduletemperature_1->setFont(font10);
        doubleSpinBox_Moduletemperature_1->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Moduletemperature_1->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Moduletemperature_1->setMinimum(35);
        doubleSpinBox_Moduletemperature_1->setMaximum(40);

        gridLayout_18->addWidget(doubleSpinBox_Moduletemperature_1, 0, 1, 1, 1);

        label_Moduletemperature_3 = new QLabel(groupBox);
        label_Moduletemperature_3->setObjectName(QStringLiteral("label_Moduletemperature_3"));
        label_Moduletemperature_3->setMinimumSize(QSize(0, 30));
        label_Moduletemperature_3->setMaximumSize(QSize(120, 30));
        label_Moduletemperature_3->setFont(font1);
        label_Moduletemperature_3->setAlignment(Qt::AlignCenter);

        gridLayout_18->addWidget(label_Moduletemperature_3, 2, 0, 1, 1);

        doubleSpinBox_Moduletemperature_3 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_Moduletemperature_3->setObjectName(QStringLiteral("doubleSpinBox_Moduletemperature_3"));
        doubleSpinBox_Moduletemperature_3->setMinimumSize(QSize(160, 30));
        doubleSpinBox_Moduletemperature_3->setMaximumSize(QSize(180, 30));
        doubleSpinBox_Moduletemperature_3->setFont(font10);
        doubleSpinBox_Moduletemperature_3->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Moduletemperature_3->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Moduletemperature_3->setMinimum(35);
        doubleSpinBox_Moduletemperature_3->setMaximum(40);

        gridLayout_18->addWidget(doubleSpinBox_Moduletemperature_3, 2, 1, 1, 1);

        label_Moduletemperature_2 = new QLabel(groupBox);
        label_Moduletemperature_2->setObjectName(QStringLiteral("label_Moduletemperature_2"));
        label_Moduletemperature_2->setMinimumSize(QSize(0, 30));
        label_Moduletemperature_2->setMaximumSize(QSize(120, 30));
        label_Moduletemperature_2->setFont(font1);
        label_Moduletemperature_2->setAlignment(Qt::AlignCenter);

        gridLayout_18->addWidget(label_Moduletemperature_2, 0, 2, 1, 1);

        doubleSpinBox_Moduletemperature_2 = new QDoubleSpinBox(groupBox);
        doubleSpinBox_Moduletemperature_2->setObjectName(QStringLiteral("doubleSpinBox_Moduletemperature_2"));
        doubleSpinBox_Moduletemperature_2->setMinimumSize(QSize(160, 30));
        doubleSpinBox_Moduletemperature_2->setMaximumSize(QSize(180, 30));
        doubleSpinBox_Moduletemperature_2->setFont(font10);
        doubleSpinBox_Moduletemperature_2->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Moduletemperature_2->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Moduletemperature_2->setMinimum(35);
        doubleSpinBox_Moduletemperature_2->setMaximum(40);

        gridLayout_18->addWidget(doubleSpinBox_Moduletemperature_2, 0, 3, 1, 1);


        gridLayout_12->addWidget(groupBox, 2, 0, 1, 2);

        tabWidgetSetconfigure->addTab(MachineTestConfigure, QString());
        tab_basicpara = new QWidget();
        tab_basicpara->setObjectName(QStringLiteral("tab_basicpara"));
        verticalLayout = new QVBoxLayout(tab_basicpara);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tableWidgetReferencevalue = new QTableWidget(tab_basicpara);
        tableWidgetReferencevalue->setObjectName(QStringLiteral("tableWidgetReferencevalue"));

        verticalLayout->addWidget(tableWidgetReferencevalue);

        tabWidgetSetconfigure->addTab(tab_basicpara, QString());
        ChannelVale_display = new QWidget();
        ChannelVale_display->setObjectName(QStringLiteral("ChannelVale_display"));
        ChannelVale_display->setStyleSheet(QLatin1String("QWidget#ChannelVale_display{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(200, 200, 200);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        gridLayout_10 = new QGridLayout(ChannelVale_display);
        gridLayout_10->setSpacing(0);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        gridLayout_10->setContentsMargins(0, 0, 0, 0);
        widget_showModule1th = new QWidget(ChannelVale_display);
        widget_showModule1th->setObjectName(QStringLiteral("widget_showModule1th"));
        widget_showModule1th->setStyleSheet(QStringLiteral("background-color: rgb(180, 180, 180);"));
        horizontalLayout_6 = new QHBoxLayout(widget_showModule1th);
        horizontalLayout_6->setSpacing(5);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_6->setContentsMargins(5, 5, 5, 5);
        Channel_1 = new ChannelvaluePanel(widget_showModule1th);
        Channel_1->setObjectName(QStringLiteral("Channel_1"));

        horizontalLayout_6->addWidget(Channel_1);

        Channel_2 = new ChannelvaluePanel(widget_showModule1th);
        Channel_2->setObjectName(QStringLiteral("Channel_2"));

        horizontalLayout_6->addWidget(Channel_2);

        Channel_3 = new ChannelvaluePanel(widget_showModule1th);
        Channel_3->setObjectName(QStringLiteral("Channel_3"));

        horizontalLayout_6->addWidget(Channel_3);

        Channel_4 = new ChannelvaluePanel(widget_showModule1th);
        Channel_4->setObjectName(QStringLiteral("Channel_4"));

        horizontalLayout_6->addWidget(Channel_4);


        gridLayout_10->addWidget(widget_showModule1th, 0, 0, 1, 1);

        widget_showModule2th = new QWidget(ChannelVale_display);
        widget_showModule2th->setObjectName(QStringLiteral("widget_showModule2th"));
        widget_showModule2th->setStyleSheet(QStringLiteral("background-color: rgb(180, 180, 180);"));
        horizontalLayout_10 = new QHBoxLayout(widget_showModule2th);
        horizontalLayout_10->setSpacing(5);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_10->setContentsMargins(5, 5, 5, 5);
        Channel_5 = new ChannelvaluePanel(widget_showModule2th);
        Channel_5->setObjectName(QStringLiteral("Channel_5"));

        horizontalLayout_10->addWidget(Channel_5);

        Channel_6 = new ChannelvaluePanel(widget_showModule2th);
        Channel_6->setObjectName(QStringLiteral("Channel_6"));

        horizontalLayout_10->addWidget(Channel_6);

        Channel_7 = new ChannelvaluePanel(widget_showModule2th);
        Channel_7->setObjectName(QStringLiteral("Channel_7"));

        horizontalLayout_10->addWidget(Channel_7);

        Channel_8 = new ChannelvaluePanel(widget_showModule2th);
        Channel_8->setObjectName(QStringLiteral("Channel_8"));

        horizontalLayout_10->addWidget(Channel_8);


        gridLayout_10->addWidget(widget_showModule2th, 1, 0, 1, 1);

        widget_showModule3th = new QWidget(ChannelVale_display);
        widget_showModule3th->setObjectName(QStringLiteral("widget_showModule3th"));
        widget_showModule3th->setStyleSheet(QStringLiteral("background-color: rgb(180, 180, 180);"));
        gridLayout_9 = new QGridLayout(widget_showModule3th);
        gridLayout_9->setSpacing(5);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        gridLayout_9->setContentsMargins(5, 5, 5, 5);
        Channel_12 = new ChannelvaluePanel(widget_showModule3th);
        Channel_12->setObjectName(QStringLiteral("Channel_12"));

        gridLayout_9->addWidget(Channel_12, 0, 3, 1, 1);

        Channel_9 = new ChannelvaluePanel(widget_showModule3th);
        Channel_9->setObjectName(QStringLiteral("Channel_9"));
        Channel_9->setStyleSheet(QStringLiteral(""));

        gridLayout_9->addWidget(Channel_9, 0, 0, 1, 1);

        Channel_10 = new ChannelvaluePanel(widget_showModule3th);
        Channel_10->setObjectName(QStringLiteral("Channel_10"));

        gridLayout_9->addWidget(Channel_10, 0, 1, 1, 1);

        Channel_11 = new ChannelvaluePanel(widget_showModule3th);
        Channel_11->setObjectName(QStringLiteral("Channel_11"));

        gridLayout_9->addWidget(Channel_11, 0, 2, 1, 1);


        gridLayout_10->addWidget(widget_showModule3th, 2, 0, 1, 1);

        widget_valuediaplay = new QWidget(ChannelVale_display);
        widget_valuediaplay->setObjectName(QStringLiteral("widget_valuediaplay"));
        widget_valuediaplay->setMinimumSize(QSize(0, 60));
        widget_valuediaplay->setMaximumSize(QSize(16777215, 510));
        widget_valuediaplay->setStyleSheet(QLatin1String("QWidget#widget_valuediaplay{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(180, 180, 180);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        verticalLayout_4 = new QVBoxLayout(widget_valuediaplay);
        verticalLayout_4->setSpacing(5);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(5, 5, 5, 5);
        tableWidget_controlChn = new QTableWidget(widget_valuediaplay);
        tableWidget_controlChn->setObjectName(QStringLiteral("tableWidget_controlChn"));

        verticalLayout_4->addWidget(tableWidget_controlChn);

        widget_bottom = new QWidget(widget_valuediaplay);
        widget_bottom->setObjectName(QStringLiteral("widget_bottom"));
        widget_bottom->setMinimumSize(QSize(0, 40));
        widget_bottom->setMaximumSize(QSize(16777215, 40));
        horizontalLayout_4 = new QHBoxLayout(widget_bottom);
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(0, 0, 0, 0);
        label_displayReminder = new QLabel(widget_bottom);
        label_displayReminder->setObjectName(QStringLiteral("label_displayReminder"));
        label_displayReminder->setMaximumSize(QSize(16777215, 30));
        label_displayReminder->setFont(font1);
        label_displayReminder->setStyleSheet(QStringLiteral(""));

        horizontalLayout_4->addWidget(label_displayReminder);

        pushButtonsavedimming = new QPushButton(widget_bottom);
        pushButtonsavedimming->setObjectName(QStringLiteral("pushButtonsavedimming"));
        pushButtonsavedimming->setMinimumSize(QSize(120, 30));
        pushButtonsavedimming->setMaximumSize(QSize(120, 30));

        horizontalLayout_4->addWidget(pushButtonsavedimming);

        toolButton_oopenall = new QToolButton(widget_bottom);
        toolButton_oopenall->setObjectName(QStringLiteral("toolButton_oopenall"));
        toolButton_oopenall->setMinimumSize(QSize(120, 30));
        toolButton_oopenall->setMaximumSize(QSize(120, 30));
        toolButton_oopenall->setStyleSheet(QStringLiteral(""));
        toolButton_oopenall->setIconSize(QSize(32, 32));
        toolButton_oopenall->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_4->addWidget(toolButton_oopenall);


        verticalLayout_4->addWidget(widget_bottom);


        gridLayout_10->addWidget(widget_valuediaplay, 3, 0, 1, 1);

        tabWidgetSetconfigure->addTab(ChannelVale_display, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QStringLiteral("tab_6"));
        gridLayout_14 = new QGridLayout(tab_6);
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        tabWidgetSetconfigure->addTab(tab_6, QString());
        tab_LIS = new QWidget();
        tab_LIS->setObjectName(QStringLiteral("tab_LIS"));
        verticalLayout_19 = new QVBoxLayout(tab_LIS);
        verticalLayout_19->setObjectName(QStringLiteral("verticalLayout_19"));
        tabWidget_3 = new QTabWidget(tab_LIS);
        tabWidget_3->setObjectName(QStringLiteral("tabWidget_3"));
        tabWidget_3->setStyleSheet(QString::fromUtf8("\n"
" QTabWidget::pane{\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	background:rgb(246, 246, 246);\n"
"	border-top-color:transparent;\n"
"}\n"
"QTabWidget::tab-bar{\n"
"	background:rgb(0, 0, 0);\n"
"	alignment: center;\n"
"}\n"
"QTabBar::tab{\n"
"	width:173px;/*\345\256\275\345\272\246\346\240\271\346\215\256\345\256\236\351\231\205\351\234\200\350\246\201\350\277\233\350\241\214\350\260\203\346\225\264*/\n"
"	height:45px;\n"
"	background:rgb(210, 210, 210);\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	border-top-left-radius: 8px;\n"
"	border-top-right-radius: 8px;\n"
"}\n"
"QTabBar::tab:selected{	\n"
"	background:rgb(246, 246, 246);\n"
"	border-bottom-color:rgb(246, 246, 246);\n"
"}\n"
"QTabBar::tab:!selected{\n"
"	background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(240, 240, 240, 255), stop:0.5 rgba(210, 210, 210, 255), stop:1 rgba(225, 225, 225, 255));\n"
"}"));
        tab_TextOut = new QWidget();
        tab_TextOut->setObjectName(QStringLiteral("tab_TextOut"));
        horizontalLayout_37 = new QHBoxLayout(tab_TextOut);
        horizontalLayout_37->setObjectName(QStringLiteral("horizontalLayout_37"));
        widget_12 = new QWidget(tab_TextOut);
        widget_12->setObjectName(QStringLiteral("widget_12"));
        widget_12->setStyleSheet(QLatin1String("QWidget#widget_12\n"
"{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(205, 200, 205);  \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"}"));
        verticalLayout_20 = new QVBoxLayout(widget_12);
        verticalLayout_20->setObjectName(QStringLiteral("verticalLayout_20"));
        tableWidget_alwaysLIS = new QTableWidget(widget_12);
        tableWidget_alwaysLIS->setObjectName(QStringLiteral("tableWidget_alwaysLIS"));

        verticalLayout_20->addWidget(tableWidget_alwaysLIS);

        checkBox_endAuditoutdata = new QCheckBox(widget_12);
        checkBox_endAuditoutdata->setObjectName(QStringLiteral("checkBox_endAuditoutdata"));
        checkBox_endAuditoutdata->setStyleSheet(QString::fromUtf8("QCheckBox{\n"
" font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;\n"
"}\n"
"QCheckBox::indicator {\n"
"        width: 32px;\n"
"        height: 32px;\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked {\n"
"        image: url(:/Picture/SetPng/checkbox-blank.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox"
                        "::indicator:enabled:indeterminate {\n"
"        image: url(:/Picture/SetPng/checkbox-blank.png);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
""));

        verticalLayout_20->addWidget(checkBox_endAuditoutdata);

        checkBox_testCompleteAutoOut = new QCheckBox(widget_12);
        checkBox_testCompleteAutoOut->setObjectName(QStringLiteral("checkBox_testCompleteAutoOut"));
        checkBox_testCompleteAutoOut->setStyleSheet(QString::fromUtf8("QCheckBox{\n"
"    font: 17 15pt '\346\245\267\344\275\223';\n"
"    background-color:rgb(190, 154, 124, 255);\n"
"    border-radius:5px;padding:2px 4px;\n"
"    color: rgb(0, 0, 0);\n"
"    border-style: flat;\n"
"    background: transparent;\n"
"}\n"
"QCheckBox::indicator {\n"
"        width: 32px;\n"
"        height: 32px;\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked {\n"
"        image: url(:/Picture/SetPng/checkbox-blank.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.pn"
                        "g);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate {\n"
"        image: url(:/Picture/SetPng/checkbox-blank.png);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
""));

        verticalLayout_20->addWidget(checkBox_testCompleteAutoOut);


        horizontalLayout_37->addWidget(widget_12);

        widget_11 = new QWidget(tab_TextOut);
        widget_11->setObjectName(QStringLiteral("widget_11"));
        widget_11->setMinimumSize(QSize(100, 0));
        widget_11->setMaximumSize(QSize(100, 16777215));
        verticalLayout_21 = new QVBoxLayout(widget_11);
        verticalLayout_21->setObjectName(QStringLiteral("verticalLayout_21"));
        pushButton_LiS_Used = new QPushButton(widget_11);
        pushButton_LiS_Used->setObjectName(QStringLiteral("pushButton_LiS_Used"));
        pushButton_LiS_Used->setMinimumSize(QSize(0, 32));
        pushButton_LiS_Used->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_LiS_Used{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButton_LiS_Used:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButton_LiS_Used:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	f"
                        "ont-size:18px;\n"
"	color:white;\n"
"}"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Picture/enable.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_LiS_Used->setIcon(icon);

        verticalLayout_21->addWidget(pushButton_LiS_Used);

        pushButton_LIS_Enable = new QPushButton(widget_11);
        pushButton_LIS_Enable->setObjectName(QStringLiteral("pushButton_LIS_Enable"));
        pushButton_LIS_Enable->setMinimumSize(QSize(0, 32));
        pushButton_LIS_Enable->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_LIS_Enable{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButton_LIS_Enable:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButton_LIS_Enable:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
""
                        "	font-size:18px;\n"
"	color:white;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Picture/forbid.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_LIS_Enable->setIcon(icon1);
        pushButton_LIS_Enable->setIconSize(QSize(16, 16));

        verticalLayout_21->addWidget(pushButton_LIS_Enable);

        pushButton_LIS_Config = new QPushButton(widget_11);
        pushButton_LIS_Config->setObjectName(QStringLiteral("pushButton_LIS_Config"));
        pushButton_LIS_Config->setMinimumSize(QSize(0, 32));
        pushButton_LIS_Config->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_LIS_Config{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButton_LIS_Config:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButton_LIS_Config:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
""
                        "	font-size:18px;\n"
"	color:white;\n"
"}"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Picture/setup.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_LIS_Config->setIcon(icon2);

        verticalLayout_21->addWidget(pushButton_LIS_Config);

        verticalSpacer_4 = new QSpacerItem(20, 588, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_21->addItem(verticalSpacer_4);


        horizontalLayout_37->addWidget(widget_11);

        tabWidget_3->addTab(tab_TextOut, QString());
        tab_Nornal = new QWidget();
        tab_Nornal->setObjectName(QStringLiteral("tab_Nornal"));
        tab_Nornal->setStyleSheet(QLatin1String("QWidget#tab_Nornal\n"
"{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(205, 200, 205);  \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"}"));
        verticalLayout_22 = new QVBoxLayout(tab_Nornal);
        verticalLayout_22->setObjectName(QStringLiteral("verticalLayout_22"));
        groupBox_5 = new QGroupBox(tab_Nornal);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        groupBox_5->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        horizontalLayout_38 = new QHBoxLayout(groupBox_5);
        horizontalLayout_38->setObjectName(QStringLiteral("horizontalLayout_38"));
        radioButton_tab = new QRadioButton(groupBox_5);
        radioButton_tab->setObjectName(QStringLiteral("radioButton_tab"));
        radioButton_tab->setMinimumSize(QSize(0, 30));
        radioButton_tab->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_38->addWidget(radioButton_tab);

        radioButton_point = new QRadioButton(groupBox_5);
        radioButton_point->setObjectName(QStringLiteral("radioButton_point"));
        radioButton_point->setMinimumSize(QSize(0, 30));
        radioButton_point->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_38->addWidget(radioButton_point);

        radioButton_Spaceing = new QRadioButton(groupBox_5);
        radioButton_Spaceing->setObjectName(QStringLiteral("radioButton_Spaceing"));
        radioButton_Spaceing->setMinimumSize(QSize(0, 30));
        radioButton_Spaceing->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_38->addWidget(radioButton_Spaceing);

        radioButton_others = new QRadioButton(groupBox_5);
        radioButton_others->setObjectName(QStringLiteral("radioButton_others"));
        radioButton_others->setMinimumSize(QSize(0, 30));
        radioButton_others->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_38->addWidget(radioButton_others);

        lineEdit_others = new QLineEdit(groupBox_5);
        lineEdit_others->setObjectName(QStringLiteral("lineEdit_others"));
        lineEdit_others->setMinimumSize(QSize(0, 30));
        lineEdit_others->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"	border: 1px solid #A0A0A0; /* \350\276\271\346\241\206\345\256\275\345\272\246\344\270\2721px\357\274\214\351\242\234\350\211\262\344\270\272#A0A0A0 */\n"
"	border-radius: 3px; /* \350\276\271\346\241\206\345\234\206\350\247\222 */\n"
"	padding-left: 5px; /* \346\226\207\346\234\254\350\267\235\347\246\273\345\267\246\350\276\271\347\225\214\346\234\2115px */\n"
"	background-color: #F2F2F2; /* \350\203\214\346\231\257\351\242\234\350\211\262 */\n"
"	color: #A0A0A0; /* \346\226\207\346\234\254\351\242\234\350\211\262 */\n"
"	selection-background-color: #A0A0A0; /* \351\200\211\344\270\255\346\226\207\346\234\254\347\232\204\350\203\214\346\231\257\351\242\234\350\211\262 */\n"
"	selection-color: #F2F2F2; /* \351\200\211\344\270\255\346\226\207\346\234\254\347\232\204\351\242\234\350\211\262 */\n"
"	font-family: \"Microsoft YaHei\"; /* \346\226\207\346\234\254\345\255\227\344\275\223\346\227\217 */\n"
"	font-size: 10pt; /* \346\226\207\346\234\254\345\255\227\344\275\223\345\244\247\345\260\217 "
                        "*/\n"
"}\n"
"\n"
"QLineEdit:hover { /* \351\274\240\346\240\207\346\202\254\346\265\256\345\234\250QLineEdit\346\227\266\347\232\204\347\212\266\346\200\201 */\n"
"	border: 1px solid #298DFF;\n"
"	border-radius: 3px;\n"
"	background-color: #F2F2F2;\n"
"	color: #298DFF;\n"
"	selection-background-color: #298DFF;\n"
"	selection-color: #F2F2F2;\n"
"}\n"
"\n"
"QLineEdit[echoMode=\"2\"] { /* QLineEdit\346\234\211\350\276\223\345\205\245\346\216\251\347\240\201\346\227\266\347\232\204\347\212\266\346\200\201 */\n"
"	lineedit-password-character: 9679;\n"
"	lineedit-password-mask-delay: 2000;\n"
"}\n"
"\n"
"QLineEdit:disabled { /* QLineEdit\345\234\250\347\246\201\347\224\250\346\227\266\347\232\204\347\212\266\346\200\201 */\n"
"	border: 1px solid #CDCDCD;\n"
"	background-color: #CDCDCD;\n"
"	color: #B4B4B4;\n"
"}\n"
"\n"
"QLineEdit:read-only { /* QLineEdit\345\234\250\345\217\252\350\257\273\346\227\266\347\232\204\347\212\266\346\200\201 */\n"
"	background-color: #CDCDCD;\n"
"	color: #F2F2F2;\n"
"}\n"
""));

        horizontalLayout_38->addWidget(lineEdit_others);

        horizontalSpacer_31 = new QSpacerItem(229, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_38->addItem(horizontalSpacer_31);


        verticalLayout_22->addWidget(groupBox_5);

        label_28 = new QLabel(tab_Nornal);
        label_28->setObjectName(QStringLiteral("label_28"));
        label_28->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        verticalLayout_22->addWidget(label_28);

        tableWidget_outTexts = new QTableWidget(tab_Nornal);
        tableWidget_outTexts->setObjectName(QStringLiteral("tableWidget_outTexts"));
        tableWidget_outTexts->setStyleSheet(QLatin1String("QTableWidget#tableWidget_outTexts{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(160, 160, 160);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        verticalLayout_22->addWidget(tableWidget_outTexts);

        checkBox_ResultAndInfo = new QCheckBox(tab_Nornal);
        checkBox_ResultAndInfo->setObjectName(QStringLiteral("checkBox_ResultAndInfo"));
        checkBox_ResultAndInfo->setStyleSheet(QString::fromUtf8("QCheckBox\n"
"{\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;\n"
"}\n"
"QCheckBox::indicator {\n"
"        width: 32px;\n"
"        height: 32px;\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked {\n"
"        image: url(:/Picture/SetPng/checkbox-blank.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:unchecked:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:checked:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"QChec"
                        "kBox::indicator:enabled:indeterminate {\n"
"        image: url(:/Picture/SetPng/checkbox-blank.png);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate:hover {\n"
"        image: url(:/Picture/SetPng/checkbox-blank-fill.png);\n"
"}\n"
"QCheckBox::indicator:enabled:indeterminate:pressed {\n"
"        image: url(:/Picture/SetPng/checkbox-fill.png);\n"
"}\n"
"\n"
""));

        verticalLayout_22->addWidget(checkBox_ResultAndInfo);

        groupBox_connectStyle = new QGroupBox(tab_Nornal);
        groupBox_connectStyle->setObjectName(QStringLiteral("groupBox_connectStyle"));
        groupBox_connectStyle->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        gridLayout_39 = new QGridLayout(groupBox_connectStyle);
        gridLayout_39->setSpacing(5);
        gridLayout_39->setObjectName(QStringLiteral("gridLayout_39"));
        gridLayout_39->setContentsMargins(5, 5, 5, 5);
        radioButton_sel_tcpip = new QRadioButton(groupBox_connectStyle);
        radioButton_sel_tcpip->setObjectName(QStringLiteral("radioButton_sel_tcpip"));
        radioButton_sel_tcpip->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_39->addWidget(radioButton_sel_tcpip, 0, 1, 1, 1);

        label_TextOutPath = new QLabel(groupBox_connectStyle);
        label_TextOutPath->setObjectName(QStringLiteral("label_TextOutPath"));
        label_TextOutPath->setMaximumSize(QSize(140, 16777215));
        label_TextOutPath->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_39->addWidget(label_TextOutPath, 2, 0, 1, 1);

        radioButton_selSerial = new QRadioButton(groupBox_connectStyle);
        radioButton_selSerial->setObjectName(QStringLiteral("radioButton_selSerial"));
        radioButton_selSerial->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_39->addWidget(radioButton_selSerial, 0, 2, 1, 2);

        lineEdit_TextOutPath = new QLineEdit(groupBox_connectStyle);
        lineEdit_TextOutPath->setObjectName(QStringLiteral("lineEdit_TextOutPath"));
        lineEdit_TextOutPath->setMinimumSize(QSize(0, 30));

        gridLayout_39->addWidget(lineEdit_TextOutPath, 2, 1, 1, 2);

        pushButton_SelTextOutPath = new QPushButton(groupBox_connectStyle);
        pushButton_SelTextOutPath->setObjectName(QStringLiteral("pushButton_SelTextOutPath"));
        pushButton_SelTextOutPath->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_SelTextOutPath{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButton_SelTextOutPath:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButton_SelTextOutPath:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344"
                        "\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Picture/documents.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_SelTextOutPath->setIcon(icon3);
        pushButton_SelTextOutPath->setIconSize(QSize(32, 32));

        gridLayout_39->addWidget(pushButton_SelTextOutPath, 2, 3, 1, 1);

        radioButton_Sel_text = new QRadioButton(groupBox_connectStyle);
        radioButton_Sel_text->setObjectName(QStringLiteral("radioButton_Sel_text"));
        radioButton_Sel_text->setMaximumSize(QSize(140, 16777215));
        radioButton_Sel_text->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_39->addWidget(radioButton_Sel_text, 0, 0, 1, 1);

        widget_14 = new QWidget(groupBox_connectStyle);
        widget_14->setObjectName(QStringLiteral("widget_14"));
        widget_14->setStyleSheet(QLatin1String("QWidget#widget_14{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(160, 160, 160);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        gridLayout_38 = new QGridLayout(widget_14);
        gridLayout_38->setObjectName(QStringLiteral("gridLayout_38"));
        label_34 = new QLabel(widget_14);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_38->addWidget(label_34, 1, 5, 1, 1);

        label_portname = new QLabel(widget_14);
        label_portname->setObjectName(QStringLiteral("label_portname"));
        label_portname->setMinimumSize(QSize(45, 0));
        label_portname->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_38->addWidget(label_portname, 0, 0, 1, 1);

        label_35 = new QLabel(widget_14);
        label_35->setObjectName(QStringLiteral("label_35"));
        label_35->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_38->addWidget(label_35, 0, 3, 1, 1);

        spinBox_6 = new QSpinBox(widget_14);
        spinBox_6->setObjectName(QStringLiteral("spinBox_6"));
        spinBox_6->setStyleSheet(QString::fromUtf8("QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QSpinBox::down-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:left;\n"
" 	image: url( :/Picture/cssup.png);\n"
"    width: 12px;\n"
"    height: 20px;\n"
"}\n"
"QSpinBox \n"
"{\n"
"    padding-top: 2px;\n"
"    padding-bottom: 2px;\n"
"	padding-left: 4px;\n"
"    padding-right: 15px;\n"
"	border:1px solid rgba(150,150,150,1);\n"
"    border-radius: 3px;\n"
"	color: rgb(200,200,200);\n"
"    background-color: rgb(44,44,44);\n"
"	selection-color: rgb(235,235,235);\n"
"	selection-background-color: rgb(83,121,180);\n"
"	font-family: \"\346\226\260\345\256\213\344\275\223\";\n"
"	font-size: 20px;\n"
"}"));

        gridLayout_38->addWidget(spinBox_6, 0, 6, 1, 1);

        spinBox_5 = new QSpinBox(widget_14);
        spinBox_5->setObjectName(QStringLiteral("spinBox_5"));
        spinBox_5->setStyleSheet(QString::fromUtf8("QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QSpinBox::down-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:left;\n"
" 	image: url( :/Picture/cssup.png);\n"
"    width: 12px;\n"
"    height: 20px;\n"
"}\n"
"QSpinBox \n"
"{\n"
"    padding-top: 2px;\n"
"    padding-bottom: 2px;\n"
"	padding-left: 4px;\n"
"    padding-right: 15px;\n"
"	border:1px solid rgba(150,150,150,1);\n"
"    border-radius: 3px;\n"
"	color: rgb(200,200,200);\n"
"    background-color: rgb(44,44,44);\n"
"	selection-color: rgb(235,235,235);\n"
"	selection-background-color: rgb(83,121,180);\n"
"	font-family: \"\346\226\260\345\256\213\344\275\223\";\n"
"	font-size: 20px;\n"
"}"));

        gridLayout_38->addWidget(spinBox_5, 0, 4, 1, 1);

        label_33 = new QLabel(widget_14);
        label_33->setObjectName(QStringLiteral("label_33"));
        label_33->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_38->addWidget(label_33, 1, 3, 1, 1);

        comboBox_6 = new QComboBox(widget_14);
        comboBox_6->setObjectName(QStringLiteral("comboBox_6"));
        comboBox_6->setMinimumSize(QSize(120, 0));
        comboBox_6->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"  color:#666666;\n"
"  font-size:24px;\n"
"  padding: 1px 15px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
"}\n"
" QComboBox::drop-down {\n"
"      subcontrol-origin: padding;\n"
"      subcontrol-position: top right;\n"
"      width: 15px;\n"
"      border:none;\n"
"}\n"
"QComboBox::down-arrow {\n"
"      image: url(:/Picture/minus.png);\n"
"  }\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font-size:24px;\n"
"    outline: 0px;  //\345\216\273\350\231\232\347\272\277\n"
"}\n"
"QComboBox QAbstractItemView::item{\n"
"	height:36px;\n"
"	color:#666666;\n"
"	padding-left:9px;\n"
"	background-color:#FFFFFF;\n"
"}\n"
"QComboBox QAbstractItemView::item:hover{ //\346\202\254\346\265\256\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#40"
                        "9CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 4px;\n"
"  }\n"
"  QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 1px;\n"
"}"));

        gridLayout_38->addWidget(comboBox_6, 1, 6, 1, 1);

        comboBox_5 = new QComboBox(widget_14);
        comboBox_5->setObjectName(QStringLiteral("comboBox_5"));
        comboBox_5->setMinimumSize(QSize(120, 0));
        comboBox_5->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"  color:#666666;\n"
"  font-size:24px;\n"
"  padding: 1px 15px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
"}\n"
" QComboBox::drop-down {\n"
"      subcontrol-origin: padding;\n"
"      subcontrol-position: top right;\n"
"      width: 15px;\n"
"      border:none;\n"
"}\n"
"QComboBox::down-arrow {\n"
"      image: url(:/Picture/minus.png);\n"
"  }\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font-size:24px;\n"
"    outline: 0px;  //\345\216\273\350\231\232\347\272\277\n"
"}\n"
"QComboBox QAbstractItemView::item{\n"
"	height:36px;\n"
"	color:#666666;\n"
"	padding-left:9px;\n"
"	background-color:#FFFFFF;\n"
"}\n"
"QComboBox QAbstractItemView::item:hover{ //\346\202\254\346\265\256\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#40"
                        "9CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 4px;\n"
"  }\n"
"  QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 1px;\n"
"}"));

        gridLayout_38->addWidget(comboBox_5, 1, 4, 1, 1);

        label_36 = new QLabel(widget_14);
        label_36->setObjectName(QStringLiteral("label_36"));
        label_36->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_38->addWidget(label_36, 0, 5, 1, 1);

        pushButton_backsetting = new QPushButton(widget_14);
        pushButton_backsetting->setObjectName(QStringLiteral("pushButton_backsetting"));
        pushButton_backsetting->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_backsetting{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QPushButton#pushButton_backsetting:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}  \n"
"QPushButton#pushButton_backsetting:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\226\260\345\256\213\344\275\223"
                        "';\n"
"	font-size:18px;\n"
"	color:white;\n"
"}"));

        gridLayout_38->addWidget(pushButton_backsetting, 1, 0, 1, 3);

        comboBox_Serialportnum = new QComboBox(widget_14);
        comboBox_Serialportnum->setObjectName(QStringLiteral("comboBox_Serialportnum"));
        comboBox_Serialportnum->setMinimumSize(QSize(240, 0));
        comboBox_Serialportnum->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"  color:#666666;\n"
"  font-size:24px;\n"
"  padding: 1px 15px 1px 3px;\n"
"  border:1px solid rgba(150,150,150,1);\n"
"  border-radius:5px 5px 0px 0px;\n"
"}\n"
" QComboBox::drop-down {\n"
"      subcontrol-origin: padding;\n"
"      subcontrol-position: top right;\n"
"      width: 15px;\n"
"      border:none;\n"
"}\n"
"QComboBox::down-arrow {\n"
"      image: url(:/Picture/minus.png);\n"
"  }\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font-size:24px;\n"
"    outline: 0px;  //\345\216\273\350\231\232\347\272\277\n"
"}\n"
"QComboBox QAbstractItemView::item{\n"
"	height:36px;\n"
"	color:#666666;\n"
"	padding-left:9px;\n"
"	background-color:#FFFFFF;\n"
"}\n"
"QComboBox QAbstractItemView::item:hover{ //\346\202\254\346\265\256\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#40"
                        "9CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 4px;\n"
"  }\n"
"  QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 1px;\n"
"}"));

        gridLayout_38->addWidget(comboBox_Serialportnum, 0, 1, 1, 2);


        gridLayout_39->addWidget(widget_14, 4, 0, 1, 4);

        widget_13 = new QWidget(groupBox_connectStyle);
        widget_13->setObjectName(QStringLiteral("widget_13"));
        widget_13->setStyleSheet(QLatin1String("QWidget#widget_13{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(160, 160, 160);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        gridLayout_37 = new QGridLayout(widget_13);
        gridLayout_37->setObjectName(QStringLiteral("gridLayout_37"));
        radioButton_10 = new QRadioButton(widget_13);
        radioButton_10->setObjectName(QStringLiteral("radioButton_10"));
        radioButton_10->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_37->addWidget(radioButton_10, 0, 0, 1, 2);

        radioButton_11 = new QRadioButton(widget_13);
        radioButton_11->setObjectName(QStringLiteral("radioButton_11"));
        radioButton_11->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_37->addWidget(radioButton_11, 0, 2, 1, 2);

        label_setaddr = new QLabel(widget_13);
        label_setaddr->setObjectName(QStringLiteral("label_setaddr"));
        label_setaddr->setMinimumSize(QSize(45, 0));
        label_setaddr->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_37->addWidget(label_setaddr, 1, 0, 1, 1);

        spinBox_setPort = new QSpinBox(widget_13);
        spinBox_setPort->setObjectName(QStringLiteral("spinBox_setPort"));
        spinBox_setPort->setMinimumSize(QSize(120, 0));
        spinBox_setPort->setStyleSheet(QString::fromUtf8("QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QSpinBox::down-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:left;\n"
" 	image: url( :/Picture/cssup.png);\n"
"    width: 12px;\n"
"    height: 20px;\n"
"}\n"
"QSpinBox \n"
"{\n"
"    padding-top: 2px;\n"
"    padding-bottom: 2px;\n"
"	padding-left: 4px;\n"
"    padding-right: 15px;\n"
"	border:1px solid rgba(150,150,150,1);\n"
"    border-radius: 3px;\n"
"	color: rgb(200,200,200);\n"
"    background-color: rgb(44,44,44);\n"
"	selection-color: rgb(235,235,235);\n"
"	selection-background-color: rgb(83,121,180);\n"
"	font-family: \"\346\226\260\345\256\213\344\275\223\";\n"
"	font-size: 20px;\n"
"}"));

        gridLayout_37->addWidget(spinBox_setPort, 1, 1, 1, 1);

        label_setAddr = new QLabel(widget_13);
        label_setAddr->setObjectName(QStringLiteral("label_setAddr"));
        label_setAddr->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_37->addWidget(label_setAddr, 1, 2, 1, 1);

        lineEdit_setAddr = new QLineEdit(widget_13);
        lineEdit_setAddr->setObjectName(QStringLiteral("lineEdit_setAddr"));
        lineEdit_setAddr->setMinimumSize(QSize(0, 30));

        gridLayout_37->addWidget(lineEdit_setAddr, 1, 3, 1, 1);


        gridLayout_39->addWidget(widget_13, 3, 0, 1, 4);


        verticalLayout_22->addWidget(groupBox_connectStyle);

        tabWidget_3->addTab(tab_Nornal, QString());

        verticalLayout_19->addWidget(tabWidget_3);

        tabWidgetSetconfigure->addTab(tab_LIS, QString());
        tab_fucn = new QWidget();
        tab_fucn->setObjectName(QStringLiteral("tab_fucn"));
        tab_fucn->setFont(font10);
        verticalLayout_ParametersSet = new QVBoxLayout(tab_fucn);
        verticalLayout_ParametersSet->setSpacing(5);
        verticalLayout_ParametersSet->setObjectName(QStringLiteral("verticalLayout_ParametersSet"));
        verticalLayout_ParametersSet->setContentsMargins(5, 0, 5, 5);
        widget_side = new QWidget(tab_fucn);
        widget_side->setObjectName(QStringLiteral("widget_side"));
        widget_side->setFont(font10);
        verticalLayout_3 = new QVBoxLayout(widget_side);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        tabWidget_config = new QTabWidget(widget_side);
        tabWidget_config->setObjectName(QStringLiteral("tabWidget_config"));
        tabWidget_config->setFont(font1);
        tabWidget_config->setStyleSheet(QString::fromUtf8("QTabWidget::pane{\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	background:rgb(246, 246, 246);\n"
"	border-top-color:transparent;\n"
"}\n"
"QTabWidget::tab-bar{\n"
"	background:rgb(0, 0, 0);\n"
"	subcontrol-position:mid;\n"
"}\n"
"QTabBar::tab{\n"
"	width:173px;/*\345\256\275\345\272\246\346\240\271\346\215\256\345\256\236\351\231\205\351\234\200\350\246\201\350\277\233\350\241\214\350\260\203\346\225\264*/\n"
"	height:45px;\n"
"	background:rgb(210, 210, 210);\n"
"	border: 2px solid rgb(210, 210, 210);\n"
"	border-top-left-radius: 8px;\n"
"	border-top-right-radius: 8px;\n"
"}\n"
"QTabBar::tab:selected{	\n"
"	background:rgb(246, 246, 246);\n"
"	border-bottom-color:rgb(246, 246, 246);\n"
"}\n"
"QTabBar::tab:!selected{\n"
"	background:qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 rgba(240, 240, 240, 255), stop:0.5 rgba(210, 210, 210, 255), stop:1 rgba(225, 225, 225, 255));\n"
"}\n"
" "));
        tab_configbloodpin = new QWidget();
        tab_configbloodpin->setObjectName(QStringLiteral("tab_configbloodpin"));
        gridLayout_5 = new QGridLayout(tab_configbloodpin);
        gridLayout_5->setSpacing(5);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        gridLayout_5->setContentsMargins(5, 5, 5, 5);
        doubleSpinBox_Ratio_ben = new QDoubleSpinBox(tab_configbloodpin);
        doubleSpinBox_Ratio_ben->setObjectName(QStringLiteral("doubleSpinBox_Ratio_ben"));
        doubleSpinBox_Ratio_ben->setMinimumSize(QSize(140, 30));
        doubleSpinBox_Ratio_ben->setMaximumSize(QSize(140, 16777215));
        doubleSpinBox_Ratio_ben->setFont(font10);
        doubleSpinBox_Ratio_ben->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Ratio_ben->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Ratio_ben->setDecimals(2);
        doubleSpinBox_Ratio_ben->setMinimum(0);
        doubleSpinBox_Ratio_ben->setMaximum(5);
        doubleSpinBox_Ratio_ben->setSingleStep(0.1);
        doubleSpinBox_Ratio_ben->setValue(2.55);

        gridLayout_5->addWidget(doubleSpinBox_Ratio_ben, 1, 1, 1, 1);

        pushButton_opencv = new QPushButton(tab_configbloodpin);
        pushButton_opencv->setObjectName(QStringLiteral("pushButton_opencv"));

        gridLayout_5->addWidget(pushButton_opencv, 8, 0, 1, 1);

        FixedHighvalue = new QDoubleSpinBox(tab_configbloodpin);
        FixedHighvalue->setObjectName(QStringLiteral("FixedHighvalue"));
        FixedHighvalue->setMinimumSize(QSize(140, 30));
        FixedHighvalue->setMaximumSize(QSize(140, 16777215));
        FixedHighvalue->setFont(font10);
        FixedHighvalue->setStyleSheet(QStringLiteral(""));
        FixedHighvalue->setAlignment(Qt::AlignCenter);
        FixedHighvalue->setMinimum(40);
        FixedHighvalue->setValue(49.4);

        gridLayout_5->addWidget(FixedHighvalue, 5, 1, 1, 1);

        OffsetTestHeight = new QLabel(tab_configbloodpin);
        OffsetTestHeight->setObjectName(QStringLiteral("OffsetTestHeight"));
        OffsetTestHeight->setFont(font6);

        gridLayout_5->addWidget(OffsetTestHeight, 6, 0, 1, 1);

        Poor_blood_changliang = new QLabel(tab_configbloodpin);
        Poor_blood_changliang->setObjectName(QStringLiteral("Poor_blood_changliang"));
        Poor_blood_changliang->setMinimumSize(QSize(0, 30));
        Poor_blood_changliang->setMaximumSize(QSize(160, 30));
        Poor_blood_changliang->setFont(font6);

        gridLayout_5->addWidget(Poor_blood_changliang, 4, 0, 1, 1);

        label_failedlinque_3 = new QLabel(tab_configbloodpin);
        label_failedlinque_3->setObjectName(QStringLiteral("label_failedlinque_3"));
        label_failedlinque_3->setMinimumSize(QSize(190, 30));
        label_failedlinque_3->setMaximumSize(QSize(190, 30));
        label_failedlinque_3->setFont(font6);

        gridLayout_5->addWidget(label_failedlinque_3, 0, 3, 1, 1);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_10, 0, 2, 1, 1);

        spinBox_CleanLinqueFailedHigh = new QSpinBox(tab_configbloodpin);
        spinBox_CleanLinqueFailedHigh->setObjectName(QStringLiteral("spinBox_CleanLinqueFailedHigh"));
        spinBox_CleanLinqueFailedHigh->setMinimumSize(QSize(140, 30));
        spinBox_CleanLinqueFailedHigh->setMaximumSize(QSize(140, 30));
        spinBox_CleanLinqueFailedHigh->setFont(font10);
        spinBox_CleanLinqueFailedHigh->setStyleSheet(QStringLiteral(""));
        spinBox_CleanLinqueFailedHigh->setAlignment(Qt::AlignCenter);
        spinBox_CleanLinqueFailedHigh->setMinimum(20);
        spinBox_CleanLinqueFailedHigh->setMaximum(99);

        gridLayout_5->addWidget(spinBox_CleanLinqueFailedHigh, 0, 4, 1, 1);

        SecurityValue_box = new QSpinBox(tab_configbloodpin);
        SecurityValue_box->setObjectName(QStringLiteral("SecurityValue_box"));
        SecurityValue_box->setMinimumSize(QSize(140, 30));
        SecurityValue_box->setMaximumSize(QSize(140, 16777215));
        SecurityValue_box->setFont(font10);
        SecurityValue_box->setStyleSheet(QStringLiteral(""));
        SecurityValue_box->setAlignment(Qt::AlignCenter);
        SecurityValue_box->setMaximum(255);

        gridLayout_5->addWidget(SecurityValue_box, 3, 1, 1, 1);

        Testheighdownheigh = new QSpinBox(tab_configbloodpin);
        Testheighdownheigh->setObjectName(QStringLiteral("Testheighdownheigh"));
        Testheighdownheigh->setMinimumSize(QSize(140, 30));
        Testheighdownheigh->setMaximumSize(QSize(140, 30));
        Testheighdownheigh->setFont(font10);
        Testheighdownheigh->setStyleSheet(QStringLiteral(""));
        Testheighdownheigh->setAlignment(Qt::AlignCenter);
        Testheighdownheigh->setMinimum(20);
        Testheighdownheigh->setMaximum(99);

        gridLayout_5->addWidget(Testheighdownheigh, 1, 4, 1, 1);

        EmptyHeigh = new QSpinBox(tab_configbloodpin);
        EmptyHeigh->setObjectName(QStringLiteral("EmptyHeigh"));
        EmptyHeigh->setMinimumSize(QSize(140, 30));
        EmptyHeigh->setMaximumSize(QSize(140, 16777215));
        EmptyHeigh->setFont(font10);
        EmptyHeigh->setStyleSheet(QStringLiteral(""));
        EmptyHeigh->setAlignment(Qt::AlignCenter);
        EmptyHeigh->setMaximum(255);

        gridLayout_5->addWidget(EmptyHeigh, 0, 1, 1, 1);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_5->addItem(horizontalSpacer_9, 0, 5, 1, 1);

        label_3 = new QLabel(tab_configbloodpin);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(170, 0));
        label_3->setMaximumSize(QSize(16777215, 30));
        label_3->setFont(font6);

        gridLayout_5->addWidget(label_3, 1, 3, 1, 1);

        poorBlood_changliang = new QSpinBox(tab_configbloodpin);
        poorBlood_changliang->setObjectName(QStringLiteral("poorBlood_changliang"));
        poorBlood_changliang->setMinimumSize(QSize(140, 30));
        poorBlood_changliang->setMaximumSize(QSize(140, 16777215));
        poorBlood_changliang->setFont(font10);
        poorBlood_changliang->setStyleSheet(QStringLiteral(""));
        poorBlood_changliang->setAlignment(Qt::AlignCenter);
        poorBlood_changliang->setMinimum(100);
        poorBlood_changliang->setMaximum(500);
        poorBlood_changliang->setValue(100);

        gridLayout_5->addWidget(poorBlood_changliang, 4, 1, 1, 1);

        OffsetTestHeightValue = new QDoubleSpinBox(tab_configbloodpin);
        OffsetTestHeightValue->setObjectName(QStringLiteral("OffsetTestHeightValue"));
        OffsetTestHeightValue->setMinimumSize(QSize(140, 30));
        OffsetTestHeightValue->setMaximumSize(QSize(140, 16777215));
        OffsetTestHeightValue->setStyleSheet(QStringLiteral(""));
        OffsetTestHeightValue->setAlignment(Qt::AlignCenter);
        OffsetTestHeightValue->setDecimals(1);
        OffsetTestHeightValue->setMaximum(25.5);

        gridLayout_5->addWidget(OffsetTestHeightValue, 6, 1, 1, 1);

        label_SecurityValue = new QLabel(tab_configbloodpin);
        label_SecurityValue->setObjectName(QStringLiteral("label_SecurityValue"));
        label_SecurityValue->setMaximumSize(QSize(150, 30));
        label_SecurityValue->setFont(font6);
        label_SecurityValue->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout_5->addWidget(label_SecurityValue, 3, 0, 1, 1);

        label_fixedhigh = new QLabel(tab_configbloodpin);
        label_fixedhigh->setObjectName(QStringLiteral("label_fixedhigh"));
        label_fixedhigh->setMinimumSize(QSize(0, 30));
        label_fixedhigh->setMaximumSize(QSize(150, 30));
        label_fixedhigh->setFont(font6);

        gridLayout_5->addWidget(label_fixedhigh, 5, 0, 1, 1);

        label_EmptyTubeHeigh = new QLabel(tab_configbloodpin);
        label_EmptyTubeHeigh->setObjectName(QStringLiteral("label_EmptyTubeHeigh"));
        label_EmptyTubeHeigh->setMaximumSize(QSize(150, 30));
        label_EmptyTubeHeigh->setFont(font6);

        gridLayout_5->addWidget(label_EmptyTubeHeigh, 0, 0, 1, 1);

        label_ratio_ben = new QLabel(tab_configbloodpin);
        label_ratio_ben->setObjectName(QStringLiteral("label_ratio_ben"));
        label_ratio_ben->setFont(font6);

        gridLayout_5->addWidget(label_ratio_ben, 1, 0, 1, 1);

        pushButtonBloodPinParasave = new QPushButton(tab_configbloodpin);
        pushButtonBloodPinParasave->setObjectName(QStringLiteral("pushButtonBloodPinParasave"));
        pushButtonBloodPinParasave->setMinimumSize(QSize(0, 35));
        pushButtonBloodPinParasave->setStyleSheet(QStringLiteral(""));

        gridLayout_5->addWidget(pushButtonBloodPinParasave, 9, 2, 1, 2);

        PRPratiolable = new QLabel(tab_configbloodpin);
        PRPratiolable->setObjectName(QStringLiteral("PRPratiolable"));
        PRPratiolable->setMinimumSize(QSize(0, 30));
        PRPratiolable->setMaximumSize(QSize(16777215, 30));
        PRPratiolable->setFont(font6);

        gridLayout_5->addWidget(PRPratiolable, 2, 0, 1, 1);

        doubleSpinBox_PRPratio = new QDoubleSpinBox(tab_configbloodpin);
        doubleSpinBox_PRPratio->setObjectName(QStringLiteral("doubleSpinBox_PRPratio"));
        doubleSpinBox_PRPratio->setMinimumSize(QSize(140, 30));
        doubleSpinBox_PRPratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_PRPratio->setDecimals(2);
        doubleSpinBox_PRPratio->setMaximum(5);
        doubleSpinBox_PRPratio->setSingleStep(0.1);
        doubleSpinBox_PRPratio->setValue(2.55);

        gridLayout_5->addWidget(doubleSpinBox_PRPratio, 2, 1, 1, 1);

        label_failedlinque = new QLabel(tab_configbloodpin);
        label_failedlinque->setObjectName(QStringLiteral("label_failedlinque"));
        label_failedlinque->setMinimumSize(QSize(140, 30));
        label_failedlinque->setMaximumSize(QSize(170, 30));
        label_failedlinque->setFont(font6);

        gridLayout_5->addWidget(label_failedlinque, 2, 3, 1, 1);

        spinBox_faliedlinque = new QSpinBox(tab_configbloodpin);
        spinBox_faliedlinque->setObjectName(QStringLiteral("spinBox_faliedlinque"));
        spinBox_faliedlinque->setMinimumSize(QSize(140, 30));
        spinBox_faliedlinque->setMaximumSize(QSize(140, 30));
        spinBox_faliedlinque->setFont(font10);
        spinBox_faliedlinque->setStyleSheet(QStringLiteral(""));
        spinBox_faliedlinque->setAlignment(Qt::AlignCenter);
        spinBox_faliedlinque->setMinimum(1);
        spinBox_faliedlinque->setMaximum(99);

        gridLayout_5->addWidget(spinBox_faliedlinque, 2, 4, 1, 1);

        label_suckairs = new QLabel(tab_configbloodpin);
        label_suckairs->setObjectName(QStringLiteral("label_suckairs"));
        label_suckairs->setMinimumSize(QSize(0, 30));
        label_suckairs->setMaximumSize(QSize(16777215, 30));
        label_suckairs->setFont(font6);

        gridLayout_5->addWidget(label_suckairs, 3, 3, 1, 1);

        spinBox_suckairs = new QSpinBox(tab_configbloodpin);
        spinBox_suckairs->setObjectName(QStringLiteral("spinBox_suckairs"));
        spinBox_suckairs->setMinimumSize(QSize(140, 30));
        spinBox_suckairs->setMaximumSize(QSize(140, 16777215));
        spinBox_suckairs->setStyleSheet(QStringLiteral(""));
        spinBox_suckairs->setAlignment(Qt::AlignCenter);
        spinBox_suckairs->setMaximum(7200);

        gridLayout_5->addWidget(spinBox_suckairs, 3, 4, 1, 1);

        label_AbsorbX2 = new QLabel(tab_configbloodpin);
        label_AbsorbX2->setObjectName(QStringLiteral("label_AbsorbX2"));
        label_AbsorbX2->setMaximumSize(QSize(150, 30));
        label_AbsorbX2->setFont(font6);

        gridLayout_5->addWidget(label_AbsorbX2, 4, 3, 1, 1);

        spinBoxAbsorbX2 = new QSpinBox(tab_configbloodpin);
        spinBoxAbsorbX2->setObjectName(QStringLiteral("spinBoxAbsorbX2"));
        spinBoxAbsorbX2->setMinimumSize(QSize(140, 30));
        spinBoxAbsorbX2->setMaximumSize(QSize(140, 16777215));
        spinBoxAbsorbX2->setFont(font10);
        spinBoxAbsorbX2->setStyleSheet(QStringLiteral(""));
        spinBoxAbsorbX2->setAlignment(Qt::AlignCenter);
        spinBoxAbsorbX2->setMinimum(1);
        spinBoxAbsorbX2->setMaximum(1250);
        spinBoxAbsorbX2->setValue(100);

        gridLayout_5->addWidget(spinBoxAbsorbX2, 4, 4, 1, 1);

        labelwashesTime = new QLabel(tab_configbloodpin);
        labelwashesTime->setObjectName(QStringLiteral("labelwashesTime"));
        labelwashesTime->setMinimumSize(QSize(0, 30));
        labelwashesTime->setMaximumSize(QSize(120, 16777215));
        labelwashesTime->setFont(font6);

        gridLayout_5->addWidget(labelwashesTime, 5, 3, 1, 1);

        spinBox_WashesTime = new QSpinBox(tab_configbloodpin);
        spinBox_WashesTime->setObjectName(QStringLiteral("spinBox_WashesTime"));
        spinBox_WashesTime->setMinimumSize(QSize(100, 30));
        spinBox_WashesTime->setMaximumSize(QSize(122222, 16777215));
        spinBox_WashesTime->setFont(font6);
        spinBox_WashesTime->setAlignment(Qt::AlignCenter);
        spinBox_WashesTime->setMinimum(500);
        spinBox_WashesTime->setMaximum(65535);
        spinBox_WashesTime->setSingleStep(100);

        gridLayout_5->addWidget(spinBox_WashesTime, 5, 4, 1, 1);

        tabWidget_config->addTab(tab_configbloodpin, QString());
        tab_reagpin = new QWidget();
        tab_reagpin->setObjectName(QStringLiteral("tab_reagpin"));
        gridLayout_6 = new QGridLayout(tab_reagpin);
        gridLayout_6->setSpacing(5);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        gridLayout_6->setContentsMargins(5, 5, 5, 5);
        groupBox_RetagentBox = new QGroupBox(tab_reagpin);
        groupBox_RetagentBox->setObjectName(QStringLiteral("groupBox_RetagentBox"));
        groupBox_RetagentBox->setMaximumSize(QSize(11850, 16777215));
        groupBox_RetagentBox->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 2px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        gridLayout_Reagent = new QGridLayout(groupBox_RetagentBox);
        gridLayout_Reagent->setObjectName(QStringLiteral("gridLayout_Reagent"));
        gridLayout_Reagent->setHorizontalSpacing(10);
        gridLayout_Reagent->setVerticalSpacing(5);
        gridLayout_Reagent->setContentsMargins(0, 0, 0, 0);
        label_11 = new QLabel(groupBox_RetagentBox);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setMinimumSize(QSize(0, 20));
        label_11->setMaximumSize(QSize(160, 20));
        label_11->setFont(font1);
        label_11->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));
        label_11->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_11, 0, 2, 1, 1);

        label_12 = new QLabel(groupBox_RetagentBox);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setMinimumSize(QSize(0, 20));
        label_12->setMaximumSize(QSize(16777215, 20));
        label_12->setFont(font1);
        label_12->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));
        label_12->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_12, 0, 3, 1, 1);

        label_show_AA = new QLabel(groupBox_RetagentBox);
        label_show_AA->setObjectName(QStringLiteral("label_show_AA"));
        label_show_AA->setMaximumSize(QSize(100, 16777215));
        label_show_AA->setFont(font1);
        label_show_AA->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_show_AA, 1, 0, 1, 1);

        label_show_ADR = new QLabel(groupBox_RetagentBox);
        label_show_ADR->setObjectName(QStringLiteral("label_show_ADR"));
        label_show_ADR->setMaximumSize(QSize(100, 16777215));
        label_show_ADR->setFont(font1);
        label_show_ADR->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_show_ADR, 3, 0, 1, 1);

        doubleSpinBox_COL_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_COL_Ratio->setObjectName(QStringLiteral("doubleSpinBox_COL_Ratio"));
        doubleSpinBox_COL_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_COL_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_COL_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_COL_Ratio->setDecimals(2);
        doubleSpinBox_COL_Ratio->setMinimum(0);
        doubleSpinBox_COL_Ratio->setMaximum(2);
        doubleSpinBox_COL_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_COL_Ratio, 4, 2, 1, 1);

        doubleSpinBox_Add_COL_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_Add_COL_Ratio->setObjectName(QStringLiteral("doubleSpinBox_Add_COL_Ratio"));
        doubleSpinBox_Add_COL_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_Add_COL_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Add_COL_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Add_COL_Ratio->setDecimals(1);
        doubleSpinBox_Add_COL_Ratio->setMaximum(1);
        doubleSpinBox_Add_COL_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_Add_COL_Ratio, 4, 3, 1, 1);

        spinBox_limit_ris = new QSpinBox(groupBox_RetagentBox);
        spinBox_limit_ris->setObjectName(QStringLiteral("spinBox_limit_ris"));
        spinBox_limit_ris->setMinimumSize(QSize(0, 30));
        spinBox_limit_ris->setStyleSheet(QStringLiteral(""));
        spinBox_limit_ris->setAlignment(Qt::AlignCenter);
        spinBox_limit_ris->setMaximum(100);

        gridLayout_Reagent->addWidget(spinBox_limit_ris, 5, 6, 1, 1);

        label_show_ADP = new QLabel(groupBox_RetagentBox);
        label_show_ADP->setObjectName(QStringLiteral("label_show_ADP"));
        label_show_ADP->setMaximumSize(QSize(100, 16777215));
        label_show_ADP->setFont(font1);
        label_show_ADP->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_show_ADP, 2, 0, 1, 1);

        label_show_RIS = new QLabel(groupBox_RetagentBox);
        label_show_RIS->setObjectName(QStringLiteral("label_show_RIS"));
        label_show_RIS->setMaximumSize(QSize(100, 16777215));
        label_show_RIS->setFont(font1);
        label_show_RIS->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_show_RIS, 5, 0, 1, 1);

        spinBox_AA_Suck_Vol = new QSpinBox(groupBox_RetagentBox);
        spinBox_AA_Suck_Vol->setObjectName(QStringLiteral("spinBox_AA_Suck_Vol"));
        spinBox_AA_Suck_Vol->setMinimumSize(QSize(0, 30));
        spinBox_AA_Suck_Vol->setMaximumSize(QSize(160, 16777215));
        spinBox_AA_Suck_Vol->setStyleSheet(QStringLiteral(""));
        spinBox_AA_Suck_Vol->setAlignment(Qt::AlignCenter);
        spinBox_AA_Suck_Vol->setMaximum(2000);

        gridLayout_Reagent->addWidget(spinBox_AA_Suck_Vol, 1, 1, 1, 1);

        doubleSpinBox_AA_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_AA_Ratio->setObjectName(QStringLiteral("doubleSpinBox_AA_Ratio"));
        doubleSpinBox_AA_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_AA_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_AA_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_AA_Ratio->setDecimals(2);
        doubleSpinBox_AA_Ratio->setMinimum(0);
        doubleSpinBox_AA_Ratio->setMaximum(2);
        doubleSpinBox_AA_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_AA_Ratio, 1, 2, 1, 1);

        label_ReagentName = new QLabel(groupBox_RetagentBox);
        label_ReagentName->setObjectName(QStringLiteral("label_ReagentName"));
        label_ReagentName->setMinimumSize(QSize(0, 20));
        label_ReagentName->setMaximumSize(QSize(100, 20));
        label_ReagentName->setFont(font1);
        label_ReagentName->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));
        label_ReagentName->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_ReagentName, 0, 0, 1, 1);

        label_show_COL = new QLabel(groupBox_RetagentBox);
        label_show_COL->setObjectName(QStringLiteral("label_show_COL"));
        label_show_COL->setMaximumSize(QSize(100, 16777215));
        label_show_COL->setFont(font1);
        label_show_COL->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_show_COL, 4, 0, 1, 1);

        spinBox_limit_cleanlinque = new QSpinBox(groupBox_RetagentBox);
        spinBox_limit_cleanlinque->setObjectName(QStringLiteral("spinBox_limit_cleanlinque"));
        spinBox_limit_cleanlinque->setMinimumSize(QSize(0, 30));
        spinBox_limit_cleanlinque->setStyleSheet(QStringLiteral(""));
        spinBox_limit_cleanlinque->setAlignment(Qt::AlignCenter);
        spinBox_limit_cleanlinque->setMaximum(100);

        gridLayout_Reagent->addWidget(spinBox_limit_cleanlinque, 6, 6, 1, 1);

        label_absorbVol = new QLabel(groupBox_RetagentBox);
        label_absorbVol->setObjectName(QStringLiteral("label_absorbVol"));
        label_absorbVol->setMinimumSize(QSize(0, 20));
        label_absorbVol->setMaximumSize(QSize(160, 20));
        label_absorbVol->setFont(font1);
        label_absorbVol->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));
        label_absorbVol->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_absorbVol, 0, 1, 1, 1);

        spinBox_ADP_Suck_Vol = new QSpinBox(groupBox_RetagentBox);
        spinBox_ADP_Suck_Vol->setObjectName(QStringLiteral("spinBox_ADP_Suck_Vol"));
        spinBox_ADP_Suck_Vol->setMinimumSize(QSize(0, 30));
        spinBox_ADP_Suck_Vol->setStyleSheet(QStringLiteral(""));
        spinBox_ADP_Suck_Vol->setAlignment(Qt::AlignCenter);
        spinBox_ADP_Suck_Vol->setMaximum(2000);

        gridLayout_Reagent->addWidget(spinBox_ADP_Suck_Vol, 2, 1, 1, 1);

        spinBox_down_ADR = new QSpinBox(groupBox_RetagentBox);
        spinBox_down_ADR->setObjectName(QStringLiteral("spinBox_down_ADR"));
        spinBox_down_ADR->setMinimumSize(QSize(0, 30));
        spinBox_down_ADR->setStyleSheet(QStringLiteral(""));
        spinBox_down_ADR->setAlignment(Qt::AlignCenter);
        spinBox_down_ADR->setMaximum(70);

        gridLayout_Reagent->addWidget(spinBox_down_ADR, 3, 4, 1, 1);

        spinBox_ADR_Suck_Vol = new QSpinBox(groupBox_RetagentBox);
        spinBox_ADR_Suck_Vol->setObjectName(QStringLiteral("spinBox_ADR_Suck_Vol"));
        spinBox_ADR_Suck_Vol->setMinimumSize(QSize(0, 30));
        spinBox_ADR_Suck_Vol->setStyleSheet(QStringLiteral(""));
        spinBox_ADR_Suck_Vol->setAlignment(Qt::AlignCenter);
        spinBox_ADR_Suck_Vol->setMaximum(2000);

        gridLayout_Reagent->addWidget(spinBox_ADR_Suck_Vol, 3, 1, 1, 1);

        doubleSpinBox_ADR_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_ADR_Ratio->setObjectName(QStringLiteral("doubleSpinBox_ADR_Ratio"));
        doubleSpinBox_ADR_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_ADR_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_ADR_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_ADR_Ratio->setDecimals(2);
        doubleSpinBox_ADR_Ratio->setMinimum(0);
        doubleSpinBox_ADR_Ratio->setMaximum(2);
        doubleSpinBox_ADR_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_ADR_Ratio, 3, 2, 1, 1);

        spinBox_down_RIS = new QSpinBox(groupBox_RetagentBox);
        spinBox_down_RIS->setObjectName(QStringLiteral("spinBox_down_RIS"));
        spinBox_down_RIS->setMinimumSize(QSize(0, 30));
        spinBox_down_RIS->setStyleSheet(QStringLiteral(""));
        spinBox_down_RIS->setAlignment(Qt::AlignCenter);
        spinBox_down_RIS->setMaximum(70);

        gridLayout_Reagent->addWidget(spinBox_down_RIS, 5, 4, 1, 1);

        doubleSpinBox_Add_AA_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_Add_AA_Ratio->setObjectName(QStringLiteral("doubleSpinBox_Add_AA_Ratio"));
        doubleSpinBox_Add_AA_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_Add_AA_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Add_AA_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Add_AA_Ratio->setDecimals(1);
        doubleSpinBox_Add_AA_Ratio->setMaximum(1);
        doubleSpinBox_Add_AA_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_Add_AA_Ratio, 1, 3, 1, 1);

        spinBox_down_ADP = new QSpinBox(groupBox_RetagentBox);
        spinBox_down_ADP->setObjectName(QStringLiteral("spinBox_down_ADP"));
        spinBox_down_ADP->setMinimumSize(QSize(0, 30));
        spinBox_down_ADP->setStyleSheet(QStringLiteral(""));
        spinBox_down_ADP->setAlignment(Qt::AlignCenter);
        spinBox_down_ADP->setMaximum(70);

        gridLayout_Reagent->addWidget(spinBox_down_ADP, 2, 4, 1, 1);

        spinBox_down_AA = new QSpinBox(groupBox_RetagentBox);
        spinBox_down_AA->setObjectName(QStringLiteral("spinBox_down_AA"));
        spinBox_down_AA->setMinimumSize(QSize(0, 30));
        spinBox_down_AA->setStyleSheet(QStringLiteral(""));
        spinBox_down_AA->setAlignment(Qt::AlignCenter);
        spinBox_down_AA->setMaximum(70);

        gridLayout_Reagent->addWidget(spinBox_down_AA, 1, 4, 1, 1);

        label_13 = new QLabel(groupBox_RetagentBox);
        label_13->setObjectName(QStringLiteral("label_13"));
        label_13->setMinimumSize(QSize(0, 10));
        label_13->setMaximumSize(QSize(16777215, 20));
        label_13->setFont(font1);
        label_13->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));
        label_13->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_13, 0, 4, 1, 1);

        label_limit = new QLabel(groupBox_RetagentBox);
        label_limit->setObjectName(QStringLiteral("label_limit"));
        label_limit->setFont(font1);
        label_limit->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));

        gridLayout_Reagent->addWidget(label_limit, 0, 6, 1, 1);

        spinBox_COL_Suck_Vol = new QSpinBox(groupBox_RetagentBox);
        spinBox_COL_Suck_Vol->setObjectName(QStringLiteral("spinBox_COL_Suck_Vol"));
        spinBox_COL_Suck_Vol->setMinimumSize(QSize(0, 30));
        spinBox_COL_Suck_Vol->setStyleSheet(QStringLiteral(""));
        spinBox_COL_Suck_Vol->setAlignment(Qt::AlignCenter);
        spinBox_COL_Suck_Vol->setMaximum(2000);

        gridLayout_Reagent->addWidget(spinBox_COL_Suck_Vol, 4, 1, 1, 1);

        doubleSpinBox_RIS_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_RIS_Ratio->setObjectName(QStringLiteral("doubleSpinBox_RIS_Ratio"));
        doubleSpinBox_RIS_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_RIS_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_RIS_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_RIS_Ratio->setDecimals(2);
        doubleSpinBox_RIS_Ratio->setMinimum(0);
        doubleSpinBox_RIS_Ratio->setMaximum(2);
        doubleSpinBox_RIS_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_RIS_Ratio, 5, 2, 1, 1);

        doubleSpinBox_ADP_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_ADP_Ratio->setObjectName(QStringLiteral("doubleSpinBox_ADP_Ratio"));
        doubleSpinBox_ADP_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_ADP_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_ADP_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_ADP_Ratio->setDecimals(2);
        doubleSpinBox_ADP_Ratio->setMinimum(0);
        doubleSpinBox_ADP_Ratio->setMaximum(2);
        doubleSpinBox_ADP_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_ADP_Ratio, 2, 2, 1, 1);

        spinBox_capacity_epi = new QSpinBox(groupBox_RetagentBox);
        spinBox_capacity_epi->setObjectName(QStringLiteral("spinBox_capacity_epi"));
        spinBox_capacity_epi->setMinimumSize(QSize(0, 30));
        spinBox_capacity_epi->setStyleSheet(QStringLiteral(""));
        spinBox_capacity_epi->setAlignment(Qt::AlignCenter);
        spinBox_capacity_epi->setMinimum(100);
        spinBox_capacity_epi->setMaximum(60000);

        gridLayout_Reagent->addWidget(spinBox_capacity_epi, 3, 5, 1, 1);

        spinBox_RIS_Suck_Vol = new QSpinBox(groupBox_RetagentBox);
        spinBox_RIS_Suck_Vol->setObjectName(QStringLiteral("spinBox_RIS_Suck_Vol"));
        spinBox_RIS_Suck_Vol->setMinimumSize(QSize(0, 30));
        spinBox_RIS_Suck_Vol->setStyleSheet(QStringLiteral(""));
        spinBox_RIS_Suck_Vol->setAlignment(Qt::AlignCenter);
        spinBox_RIS_Suck_Vol->setMaximum(2000);

        gridLayout_Reagent->addWidget(spinBox_RIS_Suck_Vol, 5, 1, 1, 1);

        label_capacity = new QLabel(groupBox_RetagentBox);
        label_capacity->setObjectName(QStringLiteral("label_capacity"));
        label_capacity->setFont(font1);
        label_capacity->setStyleSheet(QStringLiteral("color: rgb(58, 23, 255);"));

        gridLayout_Reagent->addWidget(label_capacity, 0, 5, 1, 1);

        doubleSpinBox_Add_RIS_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_Add_RIS_Ratio->setObjectName(QStringLiteral("doubleSpinBox_Add_RIS_Ratio"));
        doubleSpinBox_Add_RIS_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_Add_RIS_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Add_RIS_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Add_RIS_Ratio->setDecimals(1);
        doubleSpinBox_Add_RIS_Ratio->setMaximum(1);
        doubleSpinBox_Add_RIS_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_Add_RIS_Ratio, 5, 3, 1, 1);

        spinBox_limit_aa = new QSpinBox(groupBox_RetagentBox);
        spinBox_limit_aa->setObjectName(QStringLiteral("spinBox_limit_aa"));
        spinBox_limit_aa->setMinimumSize(QSize(0, 30));
        spinBox_limit_aa->setStyleSheet(QStringLiteral(""));
        spinBox_limit_aa->setAlignment(Qt::AlignCenter);
        spinBox_limit_aa->setMaximum(100);

        gridLayout_Reagent->addWidget(spinBox_limit_aa, 1, 6, 1, 1);

        doubleSpinBox_Add_ADP_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_Add_ADP_Ratio->setObjectName(QStringLiteral("doubleSpinBox_Add_ADP_Ratio"));
        doubleSpinBox_Add_ADP_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_Add_ADP_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Add_ADP_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Add_ADP_Ratio->setDecimals(1);
        doubleSpinBox_Add_ADP_Ratio->setMaximum(1);
        doubleSpinBox_Add_ADP_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_Add_ADP_Ratio, 2, 3, 1, 1);

        doubleSpinBox_Add_ADR_Ratio = new QDoubleSpinBox(groupBox_RetagentBox);
        doubleSpinBox_Add_ADR_Ratio->setObjectName(QStringLiteral("doubleSpinBox_Add_ADR_Ratio"));
        doubleSpinBox_Add_ADR_Ratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_Add_ADR_Ratio->setStyleSheet(QStringLiteral(""));
        doubleSpinBox_Add_ADR_Ratio->setAlignment(Qt::AlignCenter);
        doubleSpinBox_Add_ADR_Ratio->setDecimals(1);
        doubleSpinBox_Add_ADR_Ratio->setMaximum(1);
        doubleSpinBox_Add_ADR_Ratio->setSingleStep(0.1);

        gridLayout_Reagent->addWidget(doubleSpinBox_Add_ADR_Ratio, 3, 3, 1, 1);

        spinBox_capacity_ris = new QSpinBox(groupBox_RetagentBox);
        spinBox_capacity_ris->setObjectName(QStringLiteral("spinBox_capacity_ris"));
        spinBox_capacity_ris->setMinimumSize(QSize(0, 30));
        spinBox_capacity_ris->setStyleSheet(QStringLiteral(""));
        spinBox_capacity_ris->setAlignment(Qt::AlignCenter);
        spinBox_capacity_ris->setMinimum(100);
        spinBox_capacity_ris->setMaximum(60000);

        gridLayout_Reagent->addWidget(spinBox_capacity_ris, 5, 5, 1, 1);

        spinBox_limit_epi = new QSpinBox(groupBox_RetagentBox);
        spinBox_limit_epi->setObjectName(QStringLiteral("spinBox_limit_epi"));
        spinBox_limit_epi->setMinimumSize(QSize(0, 30));
        spinBox_limit_epi->setStyleSheet(QStringLiteral(""));
        spinBox_limit_epi->setAlignment(Qt::AlignCenter);
        spinBox_limit_epi->setMaximum(100);

        gridLayout_Reagent->addWidget(spinBox_limit_epi, 3, 6, 1, 1);

        spinBox_down_COL = new QSpinBox(groupBox_RetagentBox);
        spinBox_down_COL->setObjectName(QStringLiteral("spinBox_down_COL"));
        spinBox_down_COL->setMinimumSize(QSize(0, 30));
        spinBox_down_COL->setStyleSheet(QStringLiteral(""));
        spinBox_down_COL->setAlignment(Qt::AlignCenter);
        spinBox_down_COL->setMaximum(70);

        gridLayout_Reagent->addWidget(spinBox_down_COL, 4, 4, 1, 1);

        spinBox_capacity_aa = new QSpinBox(groupBox_RetagentBox);
        spinBox_capacity_aa->setObjectName(QStringLiteral("spinBox_capacity_aa"));
        spinBox_capacity_aa->setMinimumSize(QSize(0, 30));
        spinBox_capacity_aa->setStyleSheet(QStringLiteral(""));
        spinBox_capacity_aa->setAlignment(Qt::AlignCenter);
        spinBox_capacity_aa->setMinimum(100);
        spinBox_capacity_aa->setMaximum(60000);

        gridLayout_Reagent->addWidget(spinBox_capacity_aa, 1, 5, 1, 1);

        label_cleanlinque = new QLabel(groupBox_RetagentBox);
        label_cleanlinque->setObjectName(QStringLiteral("label_cleanlinque"));
        label_cleanlinque->setMaximumSize(QSize(100, 16777215));
        label_cleanlinque->setFont(font1);
        label_cleanlinque->setAlignment(Qt::AlignCenter);

        gridLayout_Reagent->addWidget(label_cleanlinque, 6, 0, 1, 1);

        spinBox_capacity_cleanlinque = new QSpinBox(groupBox_RetagentBox);
        spinBox_capacity_cleanlinque->setObjectName(QStringLiteral("spinBox_capacity_cleanlinque"));
        spinBox_capacity_cleanlinque->setMinimumSize(QSize(0, 30));
        spinBox_capacity_cleanlinque->setStyleSheet(QStringLiteral(""));
        spinBox_capacity_cleanlinque->setAlignment(Qt::AlignCenter);
        spinBox_capacity_cleanlinque->setMinimum(100);
        spinBox_capacity_cleanlinque->setMaximum(60000);

        gridLayout_Reagent->addWidget(spinBox_capacity_cleanlinque, 6, 5, 1, 1);

        spinBox_capacity_adp = new QSpinBox(groupBox_RetagentBox);
        spinBox_capacity_adp->setObjectName(QStringLiteral("spinBox_capacity_adp"));
        spinBox_capacity_adp->setMinimumSize(QSize(0, 30));
        spinBox_capacity_adp->setStyleSheet(QStringLiteral(""));
        spinBox_capacity_adp->setAlignment(Qt::AlignCenter);
        spinBox_capacity_adp->setMinimum(100);
        spinBox_capacity_adp->setMaximum(60000);

        gridLayout_Reagent->addWidget(spinBox_capacity_adp, 2, 5, 1, 1);

        spinBox_capacity_col = new QSpinBox(groupBox_RetagentBox);
        spinBox_capacity_col->setObjectName(QStringLiteral("spinBox_capacity_col"));
        spinBox_capacity_col->setMinimumSize(QSize(0, 30));
        spinBox_capacity_col->setStyleSheet(QStringLiteral(""));
        spinBox_capacity_col->setAlignment(Qt::AlignCenter);
        spinBox_capacity_col->setMinimum(100);
        spinBox_capacity_col->setMaximum(60000);

        gridLayout_Reagent->addWidget(spinBox_capacity_col, 4, 5, 1, 1);

        spinBox_limit_col = new QSpinBox(groupBox_RetagentBox);
        spinBox_limit_col->setObjectName(QStringLiteral("spinBox_limit_col"));
        spinBox_limit_col->setMinimumSize(QSize(0, 30));
        spinBox_limit_col->setStyleSheet(QStringLiteral(""));
        spinBox_limit_col->setAlignment(Qt::AlignCenter);
        spinBox_limit_col->setMaximum(100);

        gridLayout_Reagent->addWidget(spinBox_limit_col, 4, 6, 1, 1);

        spinBox_limit_adp = new QSpinBox(groupBox_RetagentBox);
        spinBox_limit_adp->setObjectName(QStringLiteral("spinBox_limit_adp"));
        spinBox_limit_adp->setMinimumSize(QSize(0, 30));
        spinBox_limit_adp->setStyleSheet(QStringLiteral(""));
        spinBox_limit_adp->setAlignment(Qt::AlignCenter);
        spinBox_limit_adp->setMaximum(100);

        gridLayout_Reagent->addWidget(spinBox_limit_adp, 2, 6, 1, 1);


        gridLayout_6->addWidget(groupBox_RetagentBox, 0, 0, 1, 5);

        widget_makesure = new QWidget(tab_reagpin);
        widget_makesure->setObjectName(QStringLiteral("widget_makesure"));
        widget_makesure->setMaximumSize(QSize(16777215, 50));
        horizontalLayout_7 = new QHBoxLayout(widget_makesure);
        horizontalLayout_7->setSpacing(5);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        pushButton_saved = new QPushButton(widget_makesure);
        pushButton_saved->setObjectName(QStringLiteral("pushButton_saved"));
        pushButton_saved->setMinimumSize(QSize(180, 35));
        pushButton_saved->setMaximumSize(QSize(140, 16777215));
        pushButton_saved->setStyleSheet(QStringLiteral(""));

        horizontalLayout_7->addWidget(pushButton_saved);


        gridLayout_6->addWidget(widget_makesure, 2, 0, 1, 5);

        widgetFucn = new QWidget(tab_reagpin);
        widgetFucn->setObjectName(QStringLiteral("widgetFucn"));
        widgetFucn->setMaximumSize(QSize(16777215, 160));
        gridLayout_17 = new QGridLayout(widgetFucn);
        gridLayout_17->setSpacing(5);
        gridLayout_17->setObjectName(QStringLiteral("gridLayout_17"));
        gridLayout_17->setContentsMargins(5, 5, 5, 5);
        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_17->addItem(horizontalSpacer_11, 0, 2, 1, 1);

        label_delpms = new QLabel(widgetFucn);
        label_delpms->setObjectName(QStringLiteral("label_delpms"));
        label_delpms->setMinimumSize(QSize(0, 30));
        label_delpms->setFont(font1);

        gridLayout_17->addWidget(label_delpms, 1, 0, 1, 1);

        label_failedlinque_2 = new QLabel(widgetFucn);
        label_failedlinque_2->setObjectName(QStringLiteral("label_failedlinque_2"));
        label_failedlinque_2->setMinimumSize(QSize(140, 30));
        label_failedlinque_2->setMaximumSize(QSize(170, 30));
        label_failedlinque_2->setFont(font1);

        gridLayout_17->addWidget(label_failedlinque_2, 1, 3, 1, 1);

        spinBoxReagentNeedleFailehigh = new QSpinBox(widgetFucn);
        spinBoxReagentNeedleFailehigh->setObjectName(QStringLiteral("spinBoxReagentNeedleFailehigh"));
        spinBoxReagentNeedleFailehigh->setMinimumSize(QSize(0, 30));
        spinBoxReagentNeedleFailehigh->setMaximumSize(QSize(16777215, 30));
        spinBoxReagentNeedleFailehigh->setStyleSheet(QStringLiteral(""));
        spinBoxReagentNeedleFailehigh->setAlignment(Qt::AlignCenter);
        spinBoxReagentNeedleFailehigh->setMinimum(20);
        spinBoxReagentNeedleFailehigh->setMaximum(99);
        spinBoxReagentNeedleFailehigh->setValue(20);

        gridLayout_17->addWidget(spinBoxReagentNeedleFailehigh, 0, 4, 1, 1);

        label_AbsorbX1 = new QLabel(widgetFucn);
        label_AbsorbX1->setObjectName(QStringLiteral("label_AbsorbX1"));
        label_AbsorbX1->setMaximumSize(QSize(130, 30));
        label_AbsorbX1->setFont(font1);

        gridLayout_17->addWidget(label_AbsorbX1, 0, 0, 1, 1);

        spinBoxAbsorbX1 = new QSpinBox(widgetFucn);
        spinBoxAbsorbX1->setObjectName(QStringLiteral("spinBoxAbsorbX1"));
        spinBoxAbsorbX1->setMinimumSize(QSize(120, 30));
        spinBoxAbsorbX1->setMaximumSize(QSize(111111, 16777215));
        spinBoxAbsorbX1->setFont(font10);
        spinBoxAbsorbX1->setStyleSheet(QStringLiteral(""));
        spinBoxAbsorbX1->setAlignment(Qt::AlignCenter);
        spinBoxAbsorbX1->setMinimum(0);
        spinBoxAbsorbX1->setMaximum(90);

        gridLayout_17->addWidget(spinBoxAbsorbX1, 0, 1, 1, 1);

        label_failedhighreagentneedle = new QLabel(widgetFucn);
        label_failedhighreagentneedle->setObjectName(QStringLiteral("label_failedhighreagentneedle"));
        label_failedhighreagentneedle->setMinimumSize(QSize(0, 30));
        label_failedhighreagentneedle->setMaximumSize(QSize(11111111, 30));
        label_failedhighreagentneedle->setFont(font1);

        gridLayout_17->addWidget(label_failedhighreagentneedle, 0, 3, 1, 1);

        spinBox_TestReagentFailedhigh = new QSpinBox(widgetFucn);
        spinBox_TestReagentFailedhigh->setObjectName(QStringLiteral("spinBox_TestReagentFailedhigh"));
        spinBox_TestReagentFailedhigh->setMinimumSize(QSize(120, 30));
        spinBox_TestReagentFailedhigh->setMaximumSize(QSize(120, 30));
        spinBox_TestReagentFailedhigh->setFont(font10);
        spinBox_TestReagentFailedhigh->setStyleSheet(QStringLiteral(""));
        spinBox_TestReagentFailedhigh->setAlignment(Qt::AlignCenter);
        spinBox_TestReagentFailedhigh->setMinimum(1);
        spinBox_TestReagentFailedhigh->setMaximum(99);

        gridLayout_17->addWidget(spinBox_TestReagentFailedhigh, 1, 4, 1, 1);

        spinBox_deptime = new QSpinBox(widgetFucn);
        spinBox_deptime->setObjectName(QStringLiteral("spinBox_deptime"));
        spinBox_deptime->setMinimumSize(QSize(0, 30));
        spinBox_deptime->setStyleSheet(QStringLiteral(""));
        spinBox_deptime->setAlignment(Qt::AlignCenter);
        spinBox_deptime->setMaximum(9000000);

        gridLayout_17->addWidget(spinBox_deptime, 1, 1, 1, 1);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_17->addItem(horizontalSpacer_12, 0, 5, 1, 1);


        gridLayout_6->addWidget(widgetFucn, 1, 0, 1, 5);

        tabWidget_config->addTab(tab_reagpin, QString());
        tab_hands = new QWidget();
        tab_hands->setObjectName(QStringLiteral("tab_hands"));
        gridLayout_16 = new QGridLayout(tab_hands);
        gridLayout_16->setObjectName(QStringLiteral("gridLayout_16"));
        groupBox_gripperDown = new QGroupBox(tab_hands);
        groupBox_gripperDown->setObjectName(QStringLiteral("groupBox_gripperDown"));
        groupBox_gripperDown->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        gridLayout_2 = new QGridLayout(groupBox_gripperDown);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        label_handthrowcups = new QLabel(groupBox_gripperDown);
        label_handthrowcups->setObjectName(QStringLiteral("label_handthrowcups"));
        label_handthrowcups->setMinimumSize(QSize(0, 30));
        label_handthrowcups->setMaximumSize(QSize(180, 30));
        label_handthrowcups->setFont(font6);

        gridLayout_2->addWidget(label_handthrowcups, 0, 0, 1, 2);

        spinBox_downthrowcups = new QSpinBox(groupBox_gripperDown);
        spinBox_downthrowcups->setObjectName(QStringLiteral("spinBox_downthrowcups"));
        spinBox_downthrowcups->setMinimumSize(QSize(140, 30));
        spinBox_downthrowcups->setMaximumSize(QSize(16666, 30));
        spinBox_downthrowcups->setStyleSheet(QStringLiteral(""));
        spinBox_downthrowcups->setAlignment(Qt::AlignCenter);
        spinBox_downthrowcups->setMinimum(10);
        spinBox_downthrowcups->setMaximum(70);

        gridLayout_2->addWidget(spinBox_downthrowcups, 0, 2, 1, 1);

        label_testtray_0 = new QLabel(groupBox_gripperDown);
        label_testtray_0->setObjectName(QStringLiteral("label_testtray_0"));
        label_testtray_0->setMinimumSize(QSize(0, 30));
        label_testtray_0->setMaximumSize(QSize(180, 30));
        label_testtray_0->setFont(font6);

        gridLayout_2->addWidget(label_testtray_0, 1, 0, 1, 1);

        spinBox_testtray_0 = new QSpinBox(groupBox_gripperDown);
        spinBox_testtray_0->setObjectName(QStringLiteral("spinBox_testtray_0"));
        spinBox_testtray_0->setMinimumSize(QSize(140, 30));
        spinBox_testtray_0->setMaximumSize(QSize(16777215, 30));
        spinBox_testtray_0->setStyleSheet(QStringLiteral(""));
        spinBox_testtray_0->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_testtray_0, 1, 1, 1, 2);

        label_testtray_1 = new QLabel(groupBox_gripperDown);
        label_testtray_1->setObjectName(QStringLiteral("label_testtray_1"));
        label_testtray_1->setMinimumSize(QSize(0, 30));
        label_testtray_1->setMaximumSize(QSize(180, 30));
        label_testtray_1->setFont(font6);

        gridLayout_2->addWidget(label_testtray_1, 2, 0, 1, 1);

        spinBox_testtray_1 = new QSpinBox(groupBox_gripperDown);
        spinBox_testtray_1->setObjectName(QStringLiteral("spinBox_testtray_1"));
        spinBox_testtray_1->setMinimumSize(QSize(140, 30));
        spinBox_testtray_1->setMaximumSize(QSize(16777215, 30));
        spinBox_testtray_1->setStyleSheet(QStringLiteral(""));
        spinBox_testtray_1->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_testtray_1, 2, 1, 1, 2);

        label_testtray_2 = new QLabel(groupBox_gripperDown);
        label_testtray_2->setObjectName(QStringLiteral("label_testtray_2"));
        label_testtray_2->setMinimumSize(QSize(0, 30));
        label_testtray_2->setMaximumSize(QSize(180, 30));
        label_testtray_2->setFont(font6);

        gridLayout_2->addWidget(label_testtray_2, 3, 0, 1, 1);

        spinBox_testtray_2 = new QSpinBox(groupBox_gripperDown);
        spinBox_testtray_2->setObjectName(QStringLiteral("spinBox_testtray_2"));
        spinBox_testtray_2->setMinimumSize(QSize(140, 30));
        spinBox_testtray_2->setMaximumSize(QSize(16777215, 30));
        spinBox_testtray_2->setStyleSheet(QStringLiteral(""));
        spinBox_testtray_2->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_testtray_2, 3, 1, 1, 2);

        label_testtray_3 = new QLabel(groupBox_gripperDown);
        label_testtray_3->setObjectName(QStringLiteral("label_testtray_3"));
        label_testtray_3->setMinimumSize(QSize(0, 30));
        label_testtray_3->setMaximumSize(QSize(180, 30));
        label_testtray_3->setFont(font6);

        gridLayout_2->addWidget(label_testtray_3, 4, 0, 1, 1);

        spinBox_testtray_3 = new QSpinBox(groupBox_gripperDown);
        spinBox_testtray_3->setObjectName(QStringLiteral("spinBox_testtray_3"));
        spinBox_testtray_3->setMinimumSize(QSize(140, 30));
        spinBox_testtray_3->setMaximumSize(QSize(16777215, 30));
        spinBox_testtray_3->setStyleSheet(QStringLiteral(""));
        spinBox_testtray_3->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_testtray_3, 4, 1, 1, 2);

        label_handsdownchn_0 = new QLabel(groupBox_gripperDown);
        label_handsdownchn_0->setObjectName(QStringLiteral("label_handsdownchn_0"));
        label_handsdownchn_0->setMinimumSize(QSize(0, 30));
        label_handsdownchn_0->setMaximumSize(QSize(180, 30));
        label_handsdownchn_0->setFont(font6);

        gridLayout_2->addWidget(label_handsdownchn_0, 5, 0, 1, 1);

        spinBox_handsdownchn_0 = new QSpinBox(groupBox_gripperDown);
        spinBox_handsdownchn_0->setObjectName(QStringLiteral("spinBox_handsdownchn_0"));
        spinBox_handsdownchn_0->setMinimumSize(QSize(140, 30));
        spinBox_handsdownchn_0->setMaximumSize(QSize(16777215, 30));
        spinBox_handsdownchn_0->setStyleSheet(QStringLiteral(""));
        spinBox_handsdownchn_0->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_handsdownchn_0, 5, 1, 1, 2);

        label_handsdownchn_1 = new QLabel(groupBox_gripperDown);
        label_handsdownchn_1->setObjectName(QStringLiteral("label_handsdownchn_1"));
        label_handsdownchn_1->setMinimumSize(QSize(0, 30));
        label_handsdownchn_1->setMaximumSize(QSize(180, 30));
        label_handsdownchn_1->setFont(font6);

        gridLayout_2->addWidget(label_handsdownchn_1, 6, 0, 1, 1);

        spinBox_handsdownchn_1 = new QSpinBox(groupBox_gripperDown);
        spinBox_handsdownchn_1->setObjectName(QStringLiteral("spinBox_handsdownchn_1"));
        spinBox_handsdownchn_1->setMinimumSize(QSize(140, 30));
        spinBox_handsdownchn_1->setMaximumSize(QSize(16777215, 30));
        spinBox_handsdownchn_1->setStyleSheet(QStringLiteral(""));
        spinBox_handsdownchn_1->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_handsdownchn_1, 6, 1, 1, 2);

        label_handsdownchn_2 = new QLabel(groupBox_gripperDown);
        label_handsdownchn_2->setObjectName(QStringLiteral("label_handsdownchn_2"));
        label_handsdownchn_2->setMinimumSize(QSize(0, 30));
        label_handsdownchn_2->setMaximumSize(QSize(180, 30));
        label_handsdownchn_2->setFont(font6);

        gridLayout_2->addWidget(label_handsdownchn_2, 7, 0, 1, 1);

        spinBox_handsdownchn_2 = new QSpinBox(groupBox_gripperDown);
        spinBox_handsdownchn_2->setObjectName(QStringLiteral("spinBox_handsdownchn_2"));
        spinBox_handsdownchn_2->setMinimumSize(QSize(140, 30));
        spinBox_handsdownchn_2->setMaximumSize(QSize(16777215, 30));
        spinBox_handsdownchn_2->setStyleSheet(QStringLiteral(""));
        spinBox_handsdownchn_2->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(spinBox_handsdownchn_2, 7, 1, 1, 2);


        gridLayout_16->addWidget(groupBox_gripperDown, 0, 0, 1, 1);

        groupBox_Airs = new QGroupBox(tab_hands);
        groupBox_Airs->setObjectName(QStringLiteral("groupBox_Airs"));
        groupBox_Airs->setFont(font9);
        groupBox_Airs->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        gridLayout_8 = new QGridLayout(groupBox_Airs);
        gridLayout_8->setSpacing(5);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_8->setContentsMargins(5, 5, 5, 5);
        label_8 = new QLabel(groupBox_Airs);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setMinimumSize(QSize(0, 30));
        label_8->setFont(font6);
        label_8->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_8->addWidget(label_8, 3, 0, 1, 1);

        label_2 = new QLabel(groupBox_Airs);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(0, 30));
        label_2->setFont(font6);
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_8->addWidget(label_2, 2, 0, 1, 1);

        checkBox_Recapture = new QCheckBox(groupBox_Airs);
        checkBox_Recapture->setObjectName(QStringLiteral("checkBox_Recapture"));
        checkBox_Recapture->setMinimumSize(QSize(0, 30));
        checkBox_Recapture->setFont(font6);
        checkBox_Recapture->setStyleSheet(QStringLiteral(""));

        gridLayout_8->addWidget(checkBox_Recapture, 4, 0, 1, 1);

        spinBoxSplitAirsMin = new QSpinBox(groupBox_Airs);
        spinBoxSplitAirsMin->setObjectName(QStringLiteral("spinBoxSplitAirsMin"));
        spinBoxSplitAirsMin->setMinimumSize(QSize(180, 30));
        spinBoxSplitAirsMin->setFont(font6);
        spinBoxSplitAirsMin->setAlignment(Qt::AlignCenter);
        spinBoxSplitAirsMin->setMinimum(0);
        spinBoxSplitAirsMin->setMaximum(5000);
        spinBoxSplitAirsMin->setValue(100);

        gridLayout_8->addWidget(spinBoxSplitAirsMin, 3, 2, 1, 1);

        labelsuckLateTimer = new QLabel(groupBox_Airs);
        labelsuckLateTimer->setObjectName(QStringLiteral("labelsuckLateTimer"));
        labelsuckLateTimer->setMinimumSize(QSize(0, 30));
        labelsuckLateTimer->setFont(font6);
        labelsuckLateTimer->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_8->addWidget(labelsuckLateTimer, 2, 3, 1, 1);

        spinBox_suckLatetimer = new QSpinBox(groupBox_Airs);
        spinBox_suckLatetimer->setObjectName(QStringLiteral("spinBox_suckLatetimer"));
        spinBox_suckLatetimer->setMinimumSize(QSize(140, 30));
        spinBox_suckLatetimer->setFont(font6);
        spinBox_suckLatetimer->setAlignment(Qt::AlignCenter);
        spinBox_suckLatetimer->setMaximum(10000);
        spinBox_suckLatetimer->setSingleStep(10);

        gridLayout_8->addWidget(spinBox_suckLatetimer, 2, 4, 1, 1);

        spinBoxSuckAirsMax = new QSpinBox(groupBox_Airs);
        spinBoxSuckAirsMax->setObjectName(QStringLiteral("spinBoxSuckAirsMax"));
        spinBoxSuckAirsMax->setMinimumSize(QSize(140, 30));
        spinBoxSuckAirsMax->setMaximumSize(QSize(180, 16777215));
        spinBoxSuckAirsMax->setFont(font6);
        spinBoxSuckAirsMax->setAlignment(Qt::AlignCenter);
        spinBoxSuckAirsMax->setMinimum(0);
        spinBoxSuckAirsMax->setMaximum(5000);
        spinBoxSuckAirsMax->setValue(1000);

        gridLayout_8->addWidget(spinBoxSuckAirsMax, 2, 2, 1, 1);

        label_splitval = new QLabel(groupBox_Airs);
        label_splitval->setObjectName(QStringLiteral("label_splitval"));
        label_splitval->setMinimumSize(QSize(0, 30));
        label_splitval->setFont(font6);
        label_splitval->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(label_splitval, 1, 2, 1, 1);

        label_showsuckVal = new QLabel(groupBox_Airs);
        label_showsuckVal->setObjectName(QStringLiteral("label_showsuckVal"));
        label_showsuckVal->setMinimumSize(QSize(0, 30));
        label_showsuckVal->setFont(font6);
        label_showsuckVal->setAlignment(Qt::AlignCenter);

        gridLayout_8->addWidget(label_showsuckVal, 0, 2, 1, 1);

        pushButtonopenSuck = new QPushButton(groupBox_Airs);
        pushButtonopenSuck->setObjectName(QStringLiteral("pushButtonopenSuck"));
        pushButtonopenSuck->setMinimumSize(QSize(0, 30));
        pushButtonopenSuck->setFont(font6);

        gridLayout_8->addWidget(pushButtonopenSuck, 0, 0, 1, 1);

        pushButtonsplitAirs = new QPushButton(groupBox_Airs);
        pushButtonsplitAirs->setObjectName(QStringLiteral("pushButtonsplitAirs"));
        pushButtonsplitAirs->setMinimumSize(QSize(0, 30));
        pushButtonsplitAirs->setFont(font6);

        gridLayout_8->addWidget(pushButtonsplitAirs, 1, 0, 1, 1);


        gridLayout_16->addWidget(groupBox_Airs, 0, 1, 1, 1);

        widget_gripperok = new QWidget(tab_hands);
        widget_gripperok->setObjectName(QStringLiteral("widget_gripperok"));
        widget_gripperok->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_12 = new QHBoxLayout(widget_gripperok);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_5);

        pushButtonHandsParaSave = new QPushButton(widget_gripperok);
        pushButtonHandsParaSave->setObjectName(QStringLiteral("pushButtonHandsParaSave"));
        pushButtonHandsParaSave->setMinimumSize(QSize(0, 35));
        pushButtonHandsParaSave->setStyleSheet(QStringLiteral(""));

        horizontalLayout_12->addWidget(pushButtonHandsParaSave);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_6);


        gridLayout_16->addWidget(widget_gripperok, 1, 0, 1, 2);

        tabWidget_config->addTab(tab_hands, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_7 = new QGridLayout(tab);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridLayout_7->setHorizontalSpacing(5);
        gridLayout_7->setVerticalSpacing(20);
        gridLayout_7->setContentsMargins(5, 5, 5, 5);
        groupBox_setfile = new QGroupBox(tab);
        groupBox_setfile->setObjectName(QStringLiteral("groupBox_setfile"));
        groupBox_setfile->setMaximumSize(QSize(16777215, 120));
        groupBox_setfile->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        gridLayout_13 = new QGridLayout(groupBox_setfile);
        gridLayout_13->setSpacing(5);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        gridLayout_13->setContentsMargins(5, 5, 5, 5);
        lineEdit_showPath = new QLineEdit(groupBox_setfile);
        lineEdit_showPath->setObjectName(QStringLiteral("lineEdit_showPath"));
        lineEdit_showPath->setMinimumSize(QSize(0, 30));
        lineEdit_showPath->setFont(font5);
        lineEdit_showPath->setStyleSheet(QStringLiteral(""));
        lineEdit_showPath->setReadOnly(true);

        gridLayout_13->addWidget(lineEdit_showPath, 0, 1, 1, 1);

        toolButton_Import = new QToolButton(groupBox_setfile);
        toolButton_Import->setObjectName(QStringLiteral("toolButton_Import"));
        toolButton_Import->setFont(font10);
        toolButton_Import->setStyleSheet(QStringLiteral(""));

        gridLayout_13->addWidget(toolButton_Import, 0, 2, 1, 1);

        label_path = new QLabel(groupBox_setfile);
        label_path->setObjectName(QStringLiteral("label_path"));
        label_path->setMinimumSize(QSize(100, 0));
        label_path->setFont(font6);
        label_path->setAlignment(Qt::AlignCenter);

        gridLayout_13->addWidget(label_path, 0, 0, 1, 1);

        toolButton_export = new QToolButton(groupBox_setfile);
        toolButton_export->setObjectName(QStringLiteral("toolButton_export"));
        toolButton_export->setFont(font10);
        toolButton_export->setStyleSheet(QStringLiteral(""));

        gridLayout_13->addWidget(toolButton_export, 1, 2, 1, 1);


        gridLayout_7->addWidget(groupBox_setfile, 1, 0, 1, 2);

        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMinimumSize(QSize(200, 0));
        groupBox_2->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}"));
        verticalLayout_5 = new QVBoxLayout(groupBox_2);
        verticalLayout_5->setSpacing(5);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(5, 5, 5, 5);
        pushButton_Adjustcoordinates = new QPushButton(groupBox_2);
        pushButton_Adjustcoordinates->setObjectName(QStringLiteral("pushButton_Adjustcoordinates"));
        pushButton_Adjustcoordinates->setMinimumSize(QSize(0, 35));

        verticalLayout_5->addWidget(pushButton_Adjustcoordinates);


        gridLayout_7->addWidget(groupBox_2, 0, 1, 1, 1);

        groupBox_curvepara = new QGroupBox(tab);
        groupBox_curvepara->setObjectName(QStringLiteral("groupBox_curvepara"));
        groupBox_curvepara->setMaximumSize(QSize(600, 16777215));
        groupBox_curvepara->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 3px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}"));
        verticalLayout_7 = new QVBoxLayout(groupBox_curvepara);
        verticalLayout_7->setSpacing(5);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(5, 5, 5, 5);
        groupBox_4 = new QGroupBox(groupBox_curvepara);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setStyleSheet(QLatin1String("QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top left;\n"
"	padding:1px;\n"
"}"));
        horizontalLayout = new QHBoxLayout(groupBox_4);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_7);

        label_basecutNum = new QLabel(groupBox_4);
        label_basecutNum->setObjectName(QStringLiteral("label_basecutNum"));
        label_basecutNum->setMaximumSize(QSize(80, 30));
        label_basecutNum->setFont(font6);
        label_basecutNum->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(label_basecutNum);

        comboBox_CutNum = new QComboBox(groupBox_4);
        comboBox_CutNum->setObjectName(QStringLiteral("comboBox_CutNum"));
        comboBox_CutNum->setMinimumSize(QSize(180, 30));
        comboBox_CutNum->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(comboBox_CutNum);

        checkBox_Avg_cutnum = new QCheckBox(groupBox_4);
        checkBox_Avg_cutnum->setObjectName(QStringLiteral("checkBox_Avg_cutnum"));
        checkBox_Avg_cutnum->setMinimumSize(QSize(0, 30));
        checkBox_Avg_cutnum->setFont(font6);
        checkBox_Avg_cutnum->setStyleSheet(QStringLiteral(""));
        checkBox_Avg_cutnum->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(checkBox_Avg_cutnum);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_8);


        verticalLayout_7->addWidget(groupBox_4);

        groupBox_3 = new QGroupBox(groupBox_curvepara);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setStyleSheet(QLatin1String("QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top left;\n"
"	padding:1px;\n"
"}"));
        verticalLayout_6 = new QVBoxLayout(groupBox_3);
        verticalLayout_6->setSpacing(5);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(5, 0, 5, 5);
        checkBox_originTestData = new QCheckBox(groupBox_3);
        checkBox_originTestData->setObjectName(QStringLiteral("checkBox_originTestData"));
        checkBox_originTestData->setMinimumSize(QSize(0, 30));
        checkBox_originTestData->setFont(font6);
        checkBox_originTestData->setStyleSheet(QStringLiteral(""));
        checkBox_originTestData->setIconSize(QSize(32, 32));

        verticalLayout_6->addWidget(checkBox_originTestData);

        checkBox_average = new QCheckBox(groupBox_3);
        checkBox_average->setObjectName(QStringLiteral("checkBox_average"));
        checkBox_average->setMinimumSize(QSize(140, 30));
        checkBox_average->setFont(font6);
        checkBox_average->setStyleSheet(QStringLiteral(""));
        checkBox_average->setIconSize(QSize(32, 32));

        verticalLayout_6->addWidget(checkBox_average);

        checkBox_median = new QCheckBox(groupBox_3);
        checkBox_median->setObjectName(QStringLiteral("checkBox_median"));
        checkBox_median->setMinimumSize(QSize(0, 30));
        checkBox_median->setFont(font6);
        checkBox_median->setStyleSheet(QStringLiteral(""));
        checkBox_median->setIconSize(QSize(32, 32));

        verticalLayout_6->addWidget(checkBox_median);

        checkBoxTriple = new QCheckBox(groupBox_3);
        checkBoxTriple->setObjectName(QStringLiteral("checkBoxTriple"));
        checkBoxTriple->setFont(font6);

        verticalLayout_6->addWidget(checkBoxTriple);


        verticalLayout_7->addWidget(groupBox_3);

        groupBox_6 = new QGroupBox(groupBox_curvepara);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        groupBox_6->setStyleSheet(QLatin1String("QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top left;\n"
"	padding:1px;\n"
"}"));
        verticalLayout_9 = new QVBoxLayout(groupBox_6);
        verticalLayout_9->setSpacing(5);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(5, 5, 5, 5);
        checkBox_absorbance = new QCheckBox(groupBox_6);
        checkBox_absorbance->setObjectName(QStringLiteral("checkBox_absorbance"));
        checkBox_absorbance->setMinimumSize(QSize(0, 30));
        checkBox_absorbance->setFont(font6);

        verticalLayout_9->addWidget(checkBox_absorbance);

        checkBoxExperimental = new QCheckBox(groupBox_6);
        checkBoxExperimental->setObjectName(QStringLiteral("checkBoxExperimental"));
        checkBoxExperimental->setMinimumSize(QSize(0, 30));
        checkBoxExperimental->setFont(font6);

        verticalLayout_9->addWidget(checkBoxExperimental);


        verticalLayout_7->addWidget(groupBox_6);

        widget = new QWidget(groupBox_curvepara);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 20));
        widget->setMaximumSize(QSize(16777215, 40));
        gridLayout_15 = new QGridLayout(widget);
        gridLayout_15->setSpacing(0);
        gridLayout_15->setObjectName(QStringLiteral("gridLayout_15"));
        gridLayout_15->setContentsMargins(0, 0, 0, 0);
        toolButton_SaveCutnum = new QToolButton(widget);
        toolButton_SaveCutnum->setObjectName(QStringLiteral("toolButton_SaveCutnum"));
        toolButton_SaveCutnum->setMinimumSize(QSize(120, 30));
        toolButton_SaveCutnum->setStyleSheet(QStringLiteral(""));
        toolButton_SaveCutnum->setPopupMode(QToolButton::DelayedPopup);
        toolButton_SaveCutnum->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_15->addWidget(toolButton_SaveCutnum, 0, 0, 1, 1);


        verticalLayout_7->addWidget(widget);


        gridLayout_7->addWidget(groupBox_curvepara, 0, 0, 1, 1);

        tabWidget_config->addTab(tab, QString());

        verticalLayout_3->addWidget(tabWidget_config);

        widget_loginpassword = new QWidget(widget_side);
        widget_loginpassword->setObjectName(QStringLiteral("widget_loginpassword"));
        widget_loginpassword->setFont(font4);
        widget_loginpassword->setStyleSheet(QLatin1String("border: 0px ;\n"
"background:rgb(190, 190, 190);"));
        gridLayout_4 = new QGridLayout(widget_loginpassword);
        gridLayout_4->setSpacing(0);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 0);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_3, 0, 0, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_4->addItem(horizontalSpacer_4, 0, 2, 1, 1);

        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setHorizontalSpacing(10);
        gridLayout_3->setVerticalSpacing(20);
        gridLayout_3->setContentsMargins(20, 20, -1, 10);
        label_maintenance = new QLabel(widget_loginpassword);
        label_maintenance->setObjectName(QStringLiteral("label_maintenance"));
        label_maintenance->setMinimumSize(QSize(0, 30));
        label_maintenance->setMaximumSize(QSize(160, 16777215));
        QFont font11;
        font11.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font11.setPointSize(15);
        font11.setBold(false);
        font11.setItalic(false);
        font11.setWeight(2);
        label_maintenance->setFont(font11);
        label_maintenance->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(255, 255, 255);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_3->addWidget(label_maintenance, 0, 0, 1, 1);

        lineEdit_maintenance = new QLineEdit(widget_loginpassword);
        lineEdit_maintenance->setObjectName(QStringLiteral("lineEdit_maintenance"));
        lineEdit_maintenance->setMinimumSize(QSize(0, 30));
        lineEdit_maintenance->setMaximumSize(QSize(200, 16777215));
        lineEdit_maintenance->setFont(font5);
        lineEdit_maintenance->setStyleSheet(QStringLiteral(""));
        lineEdit_maintenance->setEchoMode(QLineEdit::Password);

        gridLayout_3->addWidget(lineEdit_maintenance, 0, 1, 1, 1);

        pushButton_maintenance = new QPushButton(widget_loginpassword);
        pushButton_maintenance->setObjectName(QStringLiteral("pushButton_maintenance"));
        pushButton_maintenance->setMinimumSize(QSize(0, 30));
        pushButton_maintenance->setMaximumSize(QSize(100000, 16777215));
        pushButton_maintenance->setFont(font10);
        pushButton_maintenance->setStyleSheet(QStringLiteral(""));

        gridLayout_3->addWidget(pushButton_maintenance, 1, 0, 1, 2);


        gridLayout_4->addLayout(gridLayout_3, 0, 1, 1, 1);


        verticalLayout_3->addWidget(widget_loginpassword);


        verticalLayout_ParametersSet->addWidget(widget_side);

        tabWidgetSetconfigure->addTab(tab_fucn, QString());
        tabPE = new QWidget();
        tabPE->setObjectName(QStringLiteral("tabPE"));
        tabWidgetSetconfigure->addTab(tabPE, QString());

        horizontalLayout_configureset->addWidget(tabWidgetSetconfigure);

        QWidget::setTabOrder(comboBox_Patient_sample_report, checkBox_Automatic_print);
        QWidget::setTabOrder(checkBox_Automatic_print, listWidget_user);
        QWidget::setTabOrder(listWidget_user, pushButton_ADD);
        QWidget::setTabOrder(pushButton_ADD, pushButton_DELETE);
        QWidget::setTabOrder(pushButton_DELETE, pushButton_CHANGE);
        QWidget::setTabOrder(pushButton_CHANGE, pushButton_SWITCH);
        QWidget::setTabOrder(pushButton_SWITCH, comboBox_Category);
        QWidget::setTabOrder(comboBox_Category, toolButton_viewSQL);
        QWidget::setTabOrder(toolButton_viewSQL, lineEdit_Abbreviation);
        QWidget::setTabOrder(lineEdit_Abbreviation, lineEdit_Name);
        QWidget::setTabOrder(lineEdit_Name, tabWidget_info);
        QWidget::setTabOrder(tabWidget_info, tableWidget_info);
        QWidget::setTabOrder(tableWidget_info, tableWidget_info_1);
        QWidget::setTabOrder(tableWidget_info_1, tableWidget_info_2);
        QWidget::setTabOrder(tableWidget_info_2, tableWidget_info_3);
        QWidget::setTabOrder(tableWidget_info_3, tableWidget_info_4);
        QWidget::setTabOrder(tableWidget_info_4, tableWidget_info_5);
        QWidget::setTabOrder(tableWidget_info_5, tableWidget_info_6);
        QWidget::setTabOrder(tableWidget_info_6, tableWidget_info_7);
        QWidget::setTabOrder(tableWidget_info_7, toolButton_ADD);
        QWidget::setTabOrder(toolButton_ADD, toolButton_Delete);
        QWidget::setTabOrder(toolButton_Delete, checkBox_Automatic_review);
        QWidget::setTabOrder(checkBox_Automatic_review, comboBox_QC_sample_report);

        retranslateUi(MachineSetting);

        tabWidgetSetconfigure->setCurrentIndex(7);
        tabWidget_info->setCurrentIndex(0);
        tabWidget_3->setCurrentIndex(1);
        tabWidget_config->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MachineSetting);
    } // setupUi

    void retranslateUi(QWidget *MachineSetting)
    {
        MachineSetting->setWindowTitle(QApplication::translate("MachineSetting", "Form", nullptr));
        groupBox_usermanagement->setTitle(QApplication::translate("MachineSetting", "\347\224\250\346\210\267\350\256\276\347\275\256", nullptr));
        pushButton_ADD->setText(QApplication::translate("MachineSetting", "\346\267\273\345\212\240", nullptr));
        pushButton_DELETE->setText(QApplication::translate("MachineSetting", "\345\210\240\351\231\244", nullptr));
        pushButton_CHANGE->setText(QApplication::translate("MachineSetting", "\344\277\256\346\224\271\345\257\206\347\240\201", nullptr));
        pushButton_SWITCH->setText(QApplication::translate("MachineSetting", "\345\210\207\346\215\242\347\224\250\346\210\267", nullptr));
        groupBox_printReport->setTitle(QString());
        checkBox_Automatic_print->setText(QApplication::translate("MachineSetting", "\346\265\213\350\257\225\345\256\214\346\210\220\345\220\216\350\207\252\345\212\250\346\211\223\345\215\260", nullptr));
        label_Tab0_hospital_name->setText(QApplication::translate("MachineSetting", "\350\256\276\347\275\256\345\214\273\351\231\242\345\220\215:", nullptr));
        label_QC_sample_report->setText(QApplication::translate("MachineSetting", "\350\264\250\346\216\247\346\212\245\350\241\250\346\250\241\346\235\277:", nullptr));
        checkBox_Automatic_review->setText(QApplication::translate("MachineSetting", "\346\211\223\345\215\260\346\212\245\345\221\212\346\227\266\350\207\252\345\212\250\345\256\241\346\240\270\346\234\252\345\256\241\346\240\270\347\232\204\350\256\260\345\275\225", nullptr));
        label_Patient_sample_report->setText(QApplication::translate("MachineSetting", "\346\202\243\350\200\205\346\240\267\346\234\254\346\212\245\345\221\212\346\250\241\346\235\277:", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tab_1), QApplication::translate("MachineSetting", "\347\224\250\346\210\267\347\256\241\347\220\206", nullptr));
        label_Category->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        toolButton_viewSQL->setText(QApplication::translate("MachineSetting", "...", nullptr));
        label_Abbreviation->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        label_Name->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info), QApplication::translate("MachineSetting", "Tab 1", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_1), QApplication::translate("MachineSetting", "Tab 2", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_2), QApplication::translate("MachineSetting", "\351\241\265", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_3), QApplication::translate("MachineSetting", "\351\241\265", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_4), QApplication::translate("MachineSetting", "\351\241\265", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_5), QApplication::translate("MachineSetting", "\351\241\265", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_6), QApplication::translate("MachineSetting", "\351\241\265", nullptr));
        tabWidget_info->setTabText(tabWidget_info->indexOf(tab_info_7), QApplication::translate("MachineSetting", "\351\241\265", nullptr));
        toolButton_ADD->setText(QApplication::translate("MachineSetting", "...", nullptr));
        toolButton_Delete->setText(QApplication::translate("MachineSetting", "...", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tab_2), QApplication::translate("MachineSetting", "\347\256\241\347\220\206\344\277\241\346\201\257", nullptr));
        disposition_value->setTitle(QApplication::translate("MachineSetting", "\344\273\252\345\231\250\351\205\215\347\275\256\345\217\202\346\225\260", nullptr));
        UseBarCode->setText(QApplication::translate("MachineSetting", "\346\211\253\346\217\217\346\235\241\345\275\242\347\240\201", nullptr));
        checkBox_UesSecondReagentHole->setText(QApplication::translate("MachineSetting", "\345\220\257\347\224\250\351\242\204\347\225\231\350\257\225\345\211\202\344\275\215", nullptr));
        checkBox_suck_offset->setText(QApplication::translate("MachineSetting", "\346\240\241\345\207\206\345\220\270\346\240\267", nullptr));
        checkBox_catchcups->setText(QApplication::translate("MachineSetting", "\345\210\235\345\247\213\346\212\223\346\235\257", nullptr));
        Machine_conf_save->setText(QApplication::translate("MachineSetting", " \344\277\235\345\255\230", nullptr));
        toolButton_Cancel->setText(QApplication::translate("MachineSetting", "\345\217\226\346\266\210", nullptr));
        usechannel->setTitle(QApplication::translate("MachineSetting", "\344\275\277\347\224\250\351\200\232\351\201\223", nullptr));
        Channel_State_1->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[1]", nullptr));
        Channel_State_5->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[5]", nullptr));
        Channel_State_9->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[9]", nullptr));
        Channel_State_2->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[2]", nullptr));
        Channel_State_6->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[6]", nullptr));
        Channel_State_10->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[10]", nullptr));
        Channel_State_3->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[3]", nullptr));
        Channel_State_7->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[7]", nullptr));
        Channel_State_11->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[11]", nullptr));
        Channel_State_4->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[4]", nullptr));
        Channel_State_8->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[8]", nullptr));
        Channel_State_12->setText(QApplication::translate("MachineSetting", "\351\200\232\351\201\223[12]", nullptr));
        groupBox->setTitle(QApplication::translate("MachineSetting", "\346\250\241\347\273\204\351\200\232\351\201\223\350\256\276\347\275\256", nullptr));
        label_Moduletemperature_1->setText(QApplication::translate("MachineSetting", "\346\250\241\347\273\204\346\270\251\345\272\2461:", nullptr));
        doubleSpinBox_Moduletemperature_1->setSuffix(QApplication::translate("MachineSetting", " \342\204\203", nullptr));
        label_Moduletemperature_3->setText(QApplication::translate("MachineSetting", "\346\250\241\347\273\204\346\270\251\345\272\2463:", nullptr));
        doubleSpinBox_Moduletemperature_3->setSuffix(QApplication::translate("MachineSetting", " \342\204\203", nullptr));
        label_Moduletemperature_2->setText(QApplication::translate("MachineSetting", "\346\250\241\347\273\204\346\270\251\345\272\2462:", nullptr));
        doubleSpinBox_Moduletemperature_2->setSuffix(QApplication::translate("MachineSetting", " \342\204\203", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(MachineTestConfigure), QApplication::translate("MachineSetting", "\351\200\232\351\201\223\347\212\266\346\200\201", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tab_basicpara), QApplication::translate("MachineSetting", "\345\257\271\346\257\224", nullptr));
        label_displayReminder->setText(QString());
        pushButtonsavedimming->setText(QApplication::translate("MachineSetting", "PushButton", nullptr));
        toolButton_oopenall->setText(QApplication::translate("MachineSetting", "\346\265\213\350\257\225\350\275\254\345\212\250", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(ChannelVale_display), QApplication::translate("MachineSetting", "\351\200\232\351\201\223\345\200\274", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tab_6), QApplication::translate("MachineSetting", "\350\264\250\346\216\247", nullptr));
        checkBox_endAuditoutdata->setText(QApplication::translate("MachineSetting", "\345\256\241\346\240\270\345\220\216\350\207\252\345\212\250\350\276\223\345\207\272\346\225\260\346\215\256", nullptr));
        checkBox_testCompleteAutoOut->setText(QApplication::translate("MachineSetting", "\346\265\213\350\257\225\345\256\214\346\210\220\345\220\216\350\207\252\345\212\250\350\276\223\345\207\272\346\225\260\346\215\256", nullptr));
        pushButton_LiS_Used->setText(QApplication::translate("MachineSetting", "\345\220\257\347\224\250", nullptr));
        pushButton_LIS_Enable->setText(QApplication::translate("MachineSetting", "\347\246\201\346\255\242 ", nullptr));
        pushButton_LIS_Config->setText(QApplication::translate("MachineSetting", "\350\256\276\347\275\256", nullptr));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_TextOut), QApplication::translate("MachineSetting", "\345\270\270\350\247\204", nullptr));
        groupBox_5->setTitle(QApplication::translate("MachineSetting", "\345\255\227\347\254\246\345\210\206\346\220\201\347\254\246", nullptr));
        radioButton_tab->setText(QApplication::translate("MachineSetting", "Tab", nullptr));
        radioButton_point->setText(QApplication::translate("MachineSetting", "\351\200\227\345\217\267 ", nullptr));
        radioButton_Spaceing->setText(QApplication::translate("MachineSetting", "\347\251\272\346\240\274", nullptr));
        radioButton_others->setText(QApplication::translate("MachineSetting", "\345\205\266\345\256\203", nullptr));
        label_28->setText(QApplication::translate("MachineSetting", "\350\276\223\345\207\272\345\255\227\346\256\265", nullptr));
        checkBox_ResultAndInfo->setText(QApplication::translate("MachineSetting", "\347\273\223\346\236\234\344\270\216\345\237\272\346\234\254\344\277\241\346\201\257\344\271\213\351\227\264\344\275\277\347\224\250\347\251\272\350\241\214\345\210\206\351\232\224", nullptr));
        groupBox_connectStyle->setTitle(QApplication::translate("MachineSetting", "\346\216\245\345\217\243\346\226\271\345\274\217", nullptr));
        radioButton_sel_tcpip->setText(QApplication::translate("MachineSetting", "TCP/IP", nullptr));
        label_TextOutPath->setText(QApplication::translate("MachineSetting", "\346\226\207\344\273\266\350\276\223\345\207\272\347\233\256\345\275\225\357\274\232", nullptr));
        radioButton_selSerial->setText(QApplication::translate("MachineSetting", "\344\270\262\345\217\243", nullptr));
        pushButton_SelTextOutPath->setText(QApplication::translate("MachineSetting", "\351\200\211\346\213\251\346\226\207\344\273\266", nullptr));
        radioButton_Sel_text->setText(QApplication::translate("MachineSetting", "\346\226\207\346\234\254\346\226\207\344\273\266", nullptr));
        label_34->setText(QApplication::translate("MachineSetting", "\346\240\241\351\252\214\344\275\215", nullptr));
        label_portname->setText(QApplication::translate("MachineSetting", "\347\253\257\345\217\243:", nullptr));
        label_35->setText(QApplication::translate("MachineSetting", "\346\263\242\347\211\271\347\216\207 ", nullptr));
        label_33->setText(QApplication::translate("MachineSetting", "\345\201\234\346\255\242\344\275\215 ", nullptr));
        label_36->setText(QApplication::translate("MachineSetting", "\346\225\260\346\215\256\344\275\215", nullptr));
        pushButton_backsetting->setText(QApplication::translate("MachineSetting", "\344\273\252\345\231\250\346\201\242\345\244\215\350\256\276\347\275\256", nullptr));
        radioButton_10->setText(QApplication::translate("MachineSetting", "\344\275\234\344\270\272\346\234\215\345\212\241\345\231\250", nullptr));
        radioButton_11->setText(QApplication::translate("MachineSetting", "\344\275\234\344\270\272\345\256\242\346\210\267\347\253\257", nullptr));
        label_setaddr->setText(QApplication::translate("MachineSetting", "\347\253\257\345\217\243:", nullptr));
        label_setAddr->setText(QApplication::translate("MachineSetting", "\345\234\260\345\235\200:", nullptr));
        tabWidget_3->setTabText(tabWidget_3->indexOf(tab_Nornal), QApplication::translate("MachineSetting", "\346\226\207\346\234\254\345\257\274\345\207\272", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tab_LIS), QApplication::translate("MachineSetting", "LIS", nullptr));
        pushButton_opencv->setText(QApplication::translate("MachineSetting", "opencv", nullptr));
        FixedHighvalue->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        OffsetTestHeight->setText(QApplication::translate("MachineSetting", "\346\265\213\351\253\230\345\201\217\347\247\273\351\253\230\345\272\246:", nullptr));
        Poor_blood_changliang->setText(QApplication::translate("MachineSetting", "PPP/PRP\346\240\267\346\234\254\351\207\217:", nullptr));
        label_failedlinque_3->setText(QApplication::translate("MachineSetting", "\346\240\267\346\234\254\351\222\210\346\270\205\346\264\227\346\216\242\346\265\213\351\253\230\345\272\246:", nullptr));
        spinBox_CleanLinqueFailedHigh->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_CleanLinqueFailedHigh->setPrefix(QString());
        SecurityValue_box->setSuffix(QApplication::translate("MachineSetting", "step", nullptr));
        SecurityValue_box->setPrefix(QString());
        Testheighdownheigh->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        Testheighdownheigh->setPrefix(QString());
        EmptyHeigh->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_3->setText(QApplication::translate("MachineSetting", "\350\241\200\346\265\206\346\250\241\345\274\217\344\270\213\351\231\215\351\253\230\345\272\246:", nullptr));
        poorBlood_changliang->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        poorBlood_changliang->setPrefix(QString());
        OffsetTestHeightValue->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_SecurityValue->setText(QApplication::translate("MachineSetting", "\347\251\272\345\233\236\345\200\274:", nullptr));
        label_fixedhigh->setText(QApplication::translate("MachineSetting", "\346\265\213\351\253\230\347\211\251\347\220\206\351\253\230\345\272\246:", nullptr));
        label_EmptyTubeHeigh->setText(QApplication::translate("MachineSetting", "\350\241\200\346\240\267\351\222\210\344\270\213\351\231\215\351\253\230\345\272\246:", nullptr));
        label_ratio_ben->setText(QApplication::translate("MachineSetting", "PPP\347\263\273\346\225\260:", nullptr));
        pushButtonBloodPinParasave->setText(QApplication::translate("MachineSetting", "\344\277\235\345\255\230", nullptr));
        PRPratiolable->setText(QApplication::translate("MachineSetting", "PRP\347\263\273\346\225\260:", nullptr));
        label_failedlinque->setText(QApplication::translate("MachineSetting", "PPP\346\216\242\346\265\213\345\272\225\351\203\250\351\253\230\345\272\246:", nullptr));
        spinBox_faliedlinque->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_faliedlinque->setPrefix(QString());
        label_suckairs->setText(QApplication::translate("MachineSetting", "\345\220\270PRP\345\211\215\345\212\240\347\251\272\346\260\224\351\207\217:", nullptr));
        label_AbsorbX2->setText(QApplication::translate("MachineSetting", "\350\241\200\346\240\267\351\222\210\346\270\205\346\264\227\351\207\217:", nullptr));
        spinBoxAbsorbX2->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        labelwashesTime->setText(QApplication::translate("MachineSetting", "\346\270\205\346\264\227\346\227\266\351\227\264:", nullptr));
        spinBox_WashesTime->setSuffix(QApplication::translate("MachineSetting", "ms", nullptr));
        spinBox_WashesTime->setPrefix(QString());
        tabWidget_config->setTabText(tabWidget_config->indexOf(tab_configbloodpin), QApplication::translate("MachineSetting", "\350\241\200\346\240\267\351\222\210\351\205\215\347\275\256\345\217\202\346\225\260", nullptr));
        groupBox_RetagentBox->setTitle(QApplication::translate("MachineSetting", "\346\265\213\350\257\225\350\257\225\345\211\202\345\217\202\346\225\260", nullptr));
        label_11->setText(QApplication::translate("MachineSetting", "\345\220\270\350\257\225\345\211\202\347\263\273\346\225\260", nullptr));
        label_12->setText(QApplication::translate("MachineSetting", "\345\237\272\347\241\200\345\220\270\351\207\217\346\267\273\345\212\240\346\257\224", nullptr));
        label_show_AA->setText(QApplication::translate("MachineSetting", "AA", nullptr));
        label_show_ADR->setText(QApplication::translate("MachineSetting", "EPI", nullptr));
        spinBox_limit_ris->setSuffix(QApplication::translate("MachineSetting", "%", nullptr));
        label_show_ADP->setText(QApplication::translate("MachineSetting", "ADP", nullptr));
        label_show_RIS->setText(QApplication::translate("MachineSetting", "RIS", nullptr));
#ifndef QT_NO_TOOLTIP
        spinBox_AA_Suck_Vol->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        spinBox_AA_Suck_Vol->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        label_ReagentName->setText(QApplication::translate("MachineSetting", "\350\257\225\345\211\202\345\220\215", nullptr));
        label_show_COL->setText(QApplication::translate("MachineSetting", "COL", nullptr));
        spinBox_limit_cleanlinque->setSuffix(QApplication::translate("MachineSetting", "%", nullptr));
        label_absorbVol->setText(QApplication::translate("MachineSetting", "\345\220\270\350\257\225\345\211\202\351\207\217", nullptr));
        spinBox_ADP_Suck_Vol->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_down_ADR->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_ADR_Suck_Vol->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_down_RIS->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
#ifndef QT_NO_TOOLTIP
        doubleSpinBox_Add_AA_Ratio->setToolTip(QApplication::translate("MachineSetting", "\345\220\270\346\240\267\351\207\217*\350\275\254\346\215\242\346\257\224*\345\220\270\350\257\225\345\211\202\347\263\273\346\225\260*\345\220\270\350\257\225\345\211\202\346\225\264\344\275\223\346\267\273\345\212\240\351\207\217", nullptr));
#endif // QT_NO_TOOLTIP
        spinBox_down_ADP->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_down_AA->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_13->setText(QApplication::translate("MachineSetting", "\345\220\220\350\257\225\345\211\202\351\253\230\345\272\246", nullptr));
        label_limit->setText(QApplication::translate("MachineSetting", "\350\257\225\345\211\202\346\212\245\350\255\246\351\231\220", nullptr));
        spinBox_COL_Suck_Vol->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_capacity_epi->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_RIS_Suck_Vol->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        label_capacity->setText(QApplication::translate("MachineSetting", "\350\257\225\345\211\202\345\256\271\351\207\217", nullptr));
        spinBox_limit_aa->setSuffix(QApplication::translate("MachineSetting", "%", nullptr));
        spinBox_capacity_ris->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_limit_epi->setSuffix(QApplication::translate("MachineSetting", "%", nullptr));
        spinBox_down_COL->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_capacity_aa->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_capacity_aa->setPrefix(QString());
        label_cleanlinque->setText(QApplication::translate("MachineSetting", "\346\270\205\346\264\227\346\266\262", nullptr));
        spinBox_capacity_cleanlinque->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_capacity_adp->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_capacity_col->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        spinBox_limit_col->setSuffix(QApplication::translate("MachineSetting", "%", nullptr));
        spinBox_limit_adp->setSuffix(QApplication::translate("MachineSetting", "%", nullptr));
        pushButton_saved->setText(QApplication::translate("MachineSetting", "\344\277\235\345\255\230", nullptr));
        label_delpms->setText(QApplication::translate("MachineSetting", "\345\212\240\345\205\245\350\257\225\345\211\202\345\273\266\346\227\266\357\274\232", nullptr));
        label_failedlinque_2->setText(QApplication::translate("MachineSetting", "\350\257\225\345\211\202\346\216\242\346\265\213\345\272\225\351\203\250\351\253\230\345\272\246:", nullptr));
        spinBoxReagentNeedleFailehigh->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBoxReagentNeedleFailehigh->setPrefix(QString());
        label_AbsorbX1->setText(QApplication::translate("MachineSetting", "\350\257\225\345\211\202\351\222\210\346\270\205\346\264\227\351\207\217:", nullptr));
        spinBoxAbsorbX1->setSuffix(QApplication::translate("MachineSetting", "ul", nullptr));
        label_failedhighreagentneedle->setText(QApplication::translate("MachineSetting", "\350\257\225\345\211\202\351\222\210\346\270\205\346\264\227\346\216\242\346\265\213\351\253\230\345\272\246:", nullptr));
        spinBox_TestReagentFailedhigh->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_TestReagentFailedhigh->setPrefix(QString());
        spinBox_deptime->setSuffix(QApplication::translate("MachineSetting", "ms", nullptr));
        tabWidget_config->setTabText(tabWidget_config->indexOf(tab_reagpin), QApplication::translate("MachineSetting", "\350\257\225\345\211\202\351\222\210\351\205\215\347\275\256\345\217\202\346\225\260", nullptr));
        groupBox_gripperDown->setTitle(QApplication::translate("MachineSetting", "\346\260\224\346\211\213\345\220\270\346\240\267\351\253\230\345\272\246", nullptr));
        label_handthrowcups->setText(QApplication::translate("MachineSetting", "\346\212\223\346\211\213\344\270\213\351\231\215\351\253\230\345\272\246(\345\274\203\346\235\257\345\255\224):", nullptr));
        spinBox_downthrowcups->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        spinBox_downthrowcups->setPrefix(QString());
        label_testtray_0->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_testtray_0->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_testtray_1->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_testtray_1->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_testtray_2->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_testtray_2->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_testtray_3->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_testtray_3->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_handsdownchn_0->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_handsdownchn_0->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_handsdownchn_1->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_handsdownchn_1->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        label_handsdownchn_2->setText(QApplication::translate("MachineSetting", "TextLabel", nullptr));
        spinBox_handsdownchn_2->setSuffix(QApplication::translate("MachineSetting", "mm", nullptr));
        groupBox_Airs->setTitle(QApplication::translate("MachineSetting", "\346\260\224\345\216\213\345\200\274\345\210\244\345\256\232\350\214\203\345\233\264", nullptr));
        label_8->setText(QApplication::translate("MachineSetting", "\346\224\276\346\235\257\346\210\220\345\212\237Min\345\200\274:", nullptr));
        label_2->setText(QApplication::translate("MachineSetting", "\345\220\270\346\235\257\346\210\220\345\212\237Max\345\200\274:", nullptr));
        checkBox_Recapture->setText(QApplication::translate("MachineSetting", "\346\212\223\346\211\213\351\207\215\350\257\225", nullptr));
        labelsuckLateTimer->setText(QApplication::translate("MachineSetting", "\345\220\270\346\235\257\345\273\266\346\227\266:", nullptr));
        spinBox_suckLatetimer->setSuffix(QApplication::translate("MachineSetting", "ms", nullptr));
        spinBox_suckLatetimer->setPrefix(QString());
        label_splitval->setText(QString());
        label_showsuckVal->setText(QString());
        pushButtonopenSuck->setText(QApplication::translate("MachineSetting", "\346\211\223\345\274\200\350\264\237\345\216\213", nullptr));
        pushButtonsplitAirs->setText(QApplication::translate("MachineSetting", "\345\205\263\351\227\255\350\264\237\345\216\213", nullptr));
        pushButtonHandsParaSave->setText(QApplication::translate("MachineSetting", "\344\277\235\345\255\230", nullptr));
        tabWidget_config->setTabText(tabWidget_config->indexOf(tab_hands), QApplication::translate("MachineSetting", "\346\212\223\346\211\213\351\205\215\347\275\256\345\217\202\346\225\260", nullptr));
        groupBox_setfile->setTitle(QApplication::translate("MachineSetting", "\345\235\220\346\240\207\346\226\207\344\273\266\345\217\202\346\225\260\346\226\207\344\273\266\345\257\274\345\205\245\344\270\216\345\257\274\345\207\272", nullptr));
        toolButton_Import->setText(QApplication::translate("MachineSetting", "\345\257\274\345\205\245", nullptr));
        label_path->setText(QApplication::translate("MachineSetting", "\346\226\207\344\273\266\350\267\257\345\276\204\357\274\232", nullptr));
        toolButton_export->setText(QApplication::translate("MachineSetting", "\345\257\274\345\207\272", nullptr));
        groupBox_2->setTitle(QApplication::translate("MachineSetting", "\346\240\241\345\207\206", nullptr));
        pushButton_Adjustcoordinates->setText(QApplication::translate("MachineSetting", "\346\240\241\345\207\206\345\235\220\346\240\207", nullptr));
        groupBox_curvepara->setTitle(QApplication::translate("MachineSetting", "\346\265\213\350\257\225\346\233\262\347\272\277\346\250\241\345\274\217", nullptr));
        groupBox_4->setTitle(QApplication::translate("MachineSetting", "\347\273\230\347\202\271\346\250\241\345\274\217", nullptr));
        label_basecutNum->setText(QApplication::translate("MachineSetting", "\345\210\206\351\230\266\346\225\260:", nullptr));
        checkBox_Avg_cutnum->setText(QApplication::translate("MachineSetting", "\345\210\206\351\230\266\345\271\263\345\235\207", nullptr));
        groupBox_3->setTitle(QApplication::translate("MachineSetting", "\346\225\260\346\215\256\351\207\207\351\233\206\346\250\241\345\274\217", nullptr));
        checkBox_originTestData->setText(QApplication::translate("MachineSetting", "\345\216\237\345\247\213\346\225\260\346\215\256", nullptr));
        checkBox_average->setText(QApplication::translate("MachineSetting", "\345\216\273\346\236\201\345\200\274\345\271\263\345\235\207\346\273\244\346\263\242", nullptr));
        checkBox_median->setText(QApplication::translate("MachineSetting", "\344\270\255\344\275\215\345\200\274\346\273\244\346\263\242", nullptr));
        checkBoxTriple->setText(QApplication::translate("MachineSetting", "\344\270\211\347\272\247\350\207\252\351\200\202\345\272\224\347\273\210\347\253\257\346\273\244\346\263\242", nullptr));
        groupBox_6->setTitle(QApplication::translate("MachineSetting", "\350\276\223\345\207\272\346\250\241\345\274\217", nullptr));
        checkBox_absorbance->setText(QApplication::translate("MachineSetting", "\345\220\270\345\205\211\345\272\246\347\256\227\346\263\225", nullptr));
        checkBoxExperimental->setText(QApplication::translate("MachineSetting", "\345\256\236\351\252\214\346\250\241\345\274\217", nullptr));
        toolButton_SaveCutnum->setText(QApplication::translate("MachineSetting", "\347\241\256\345\256\232", nullptr));
        tabWidget_config->setTabText(tabWidget_config->indexOf(tab), QApplication::translate("MachineSetting", "\346\240\241\345\207\206\345\235\220\346\240\207", nullptr));
        label_maintenance->setText(QApplication::translate("MachineSetting", "\347\273\264\346\212\244\345\257\206\347\240\201\357\274\232", nullptr));
        pushButton_maintenance->setText(QApplication::translate("MachineSetting", "\347\231\273\345\275\225\347\273\264\346\212\244", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tab_fucn), QApplication::translate("MachineSetting", "Z\350\275\264\345\217\202\346\225\260", nullptr));
        tabWidgetSetconfigure->setTabText(tabWidgetSetconfigure->indexOf(tabPE), QApplication::translate("MachineSetting", "PE", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MachineSetting: public Ui_MachineSetting {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MACHINESETTING_H
