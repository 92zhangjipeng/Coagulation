/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QWidget *MainFucnWidget;
    QGridLayout *gridLayout_2;
    QLabel *Machine_type;
    QWidget *widget_top;
    QHBoxLayout *horizontalLayout_3;
    QToolButton *toolButton_quality_start;
    QToolButton *toolButton_quality_sample;
    QToolButton *toolButton_quality_reset;
    QToolButton *toolButton_quality_cleaning;
    QToolButton *toolButton_quality_stop;
    QToolButton *toolButton_EquipmentSetting;
    QToolButton *toolButton_Exit;
    QToolButton *toolButton_about;
    QWidget *widget_btnfucn;
    QVBoxLayout *verticalLayout_2;
    QToolButton *toolButton_sampleTest_1;
    QToolButton *toolButton_resultView_2;
    QToolButton *toolButton_testingCurve_3;
    QToolButton *toolButton_TestingResult_4;
    QToolButton *toolButton_ControlSuppile_5;
    QTabWidget *tabWidget_Main;
    QWidget *TestingSample;
    QWidget *StatusBar;
    QHBoxLayout *horizontalLayout_status;
    QWidget *widget_usbstate;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_machineStaus_show;
    QLabel *label_connectstate;
    QWidget *widget_ModulI;
    QHBoxLayout *horizontalLayout;
    QLabel *label_Module1;
    QLabel *label_showModule1_value;
    QWidget *widget_ModulII;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_Module1_2;
    QLabel *label_showModule2_value_2;
    QLabel *MachineAxisValue;
    QWidget *widget_ModulIII;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_Module1_3;
    QLabel *label_showModule3_value_2;
    QWidget *widget_ReminderLiqune;
    QGridLayout *gridLayout;
    QLabel *label_Dipaly_cleanlinque;
    QLabel *label_Dipaly_ADP_2;
    QLabel *label_Dipaly_AA_1;
    QLabel *label_Dipaly_ADR_2;
    QLabel *label_Dipaly_ADR_1;
    QLabel *label_Dipaly_ADP_1;
    QLabel *label_Dipaly_COL_2;
    QLabel *label_Dipaly_COL_1;
    QLabel *label_Dipaly_AA_2;
    QLabel *label_Dipaly_RIS_1;
    QLabel *label_Dipaly_RIS_2;
    QLabel *show_wastlique;
    QLabel *showcleanliqur;
    QToolButton *sStatusInfo_text;
    QToolButton *sStatusInfo;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1954, 1117);
        MainWindow->setStyleSheet(QLatin1String(" border: 1px solid rgba(220,220,220,250);\n"
"	background-color:rgba(188,187,183,250); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px; "));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setStyleSheet(QLatin1String("QWidget{  \n"
"    border: 1px solid rgba(220,220,220,250);\n"
"	background-color:rgba(250,250,250,250); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;    	\n"
"} "));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        MainFucnWidget = new QWidget(centralWidget);
        MainFucnWidget->setObjectName(QStringLiteral("MainFucnWidget"));
        MainFucnWidget->setStyleSheet(QStringLiteral(""));
        gridLayout_2 = new QGridLayout(MainFucnWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        Machine_type = new QLabel(MainFucnWidget);
        Machine_type->setObjectName(QStringLiteral("Machine_type"));
        Machine_type->setMinimumSize(QSize(155, 141));
        Machine_type->setMaximumSize(QSize(155, 141));
        Machine_type->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout_2->addWidget(Machine_type, 0, 0, 2, 1);

        widget_top = new QWidget(MainFucnWidget);
        widget_top->setObjectName(QStringLiteral("widget_top"));
        widget_top->setMaximumSize(QSize(16777215, 150));
        widget_top->setStyleSheet(QLatin1String("QWidget{  \n"
"    border: 0px ;  \n"
"	background-color: rgb(255, 255, 255);  \n"
"    border-style: solid;  \n"
"    border-radius:18px;  \n"
"    padding:0 0px;  	\n"
"} "));
        horizontalLayout_3 = new QHBoxLayout(widget_top);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        toolButton_quality_start = new QToolButton(widget_top);
        toolButton_quality_start->setObjectName(QStringLiteral("toolButton_quality_start"));
        toolButton_quality_start->setMinimumSize(QSize(140, 150));
        toolButton_quality_start->setMaximumSize(QSize(140, 150));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        toolButton_quality_start->setFont(font);
        toolButton_quality_start->setStyleSheet(QStringLiteral(""));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Picture/Start.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_quality_start->setIcon(icon);
        toolButton_quality_start->setIconSize(QSize(150, 120));
        toolButton_quality_start->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_quality_start);

        toolButton_quality_sample = new QToolButton(widget_top);
        toolButton_quality_sample->setObjectName(QStringLiteral("toolButton_quality_sample"));
        toolButton_quality_sample->setMinimumSize(QSize(140, 150));
        toolButton_quality_sample->setMaximumSize(QSize(140, 160));
        toolButton_quality_sample->setFont(font);
        toolButton_quality_sample->setStyleSheet(QStringLiteral(""));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Picture/pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_quality_sample->setIcon(icon1);
        toolButton_quality_sample->setIconSize(QSize(150, 120));
        toolButton_quality_sample->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_quality_sample);

        toolButton_quality_reset = new QToolButton(widget_top);
        toolButton_quality_reset->setObjectName(QStringLiteral("toolButton_quality_reset"));
        toolButton_quality_reset->setMinimumSize(QSize(140, 150));
        toolButton_quality_reset->setMaximumSize(QSize(140, 160));
        toolButton_quality_reset->setFont(font);
        toolButton_quality_reset->setStyleSheet(QStringLiteral(""));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Picture/backorigin.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_quality_reset->setIcon(icon2);
        toolButton_quality_reset->setIconSize(QSize(150, 120));
        toolButton_quality_reset->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_quality_reset);

        toolButton_quality_cleaning = new QToolButton(widget_top);
        toolButton_quality_cleaning->setObjectName(QStringLiteral("toolButton_quality_cleaning"));
        toolButton_quality_cleaning->setMinimumSize(QSize(140, 150));
        toolButton_quality_cleaning->setMaximumSize(QSize(140, 150));
        toolButton_quality_cleaning->setFont(font);
        toolButton_quality_cleaning->setStyleSheet(QStringLiteral(""));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Picture/cleanning.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_quality_cleaning->setIcon(icon3);
        toolButton_quality_cleaning->setIconSize(QSize(150, 120));
        toolButton_quality_cleaning->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_quality_cleaning);

        toolButton_quality_stop = new QToolButton(widget_top);
        toolButton_quality_stop->setObjectName(QStringLiteral("toolButton_quality_stop"));
        toolButton_quality_stop->setMinimumSize(QSize(140, 150));
        toolButton_quality_stop->setMaximumSize(QSize(140, 150));
        toolButton_quality_stop->setFont(font);
        toolButton_quality_stop->setStyleSheet(QStringLiteral(""));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/Picture/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_quality_stop->setIcon(icon4);
        toolButton_quality_stop->setIconSize(QSize(150, 120));
        toolButton_quality_stop->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_quality_stop);

        toolButton_EquipmentSetting = new QToolButton(widget_top);
        toolButton_EquipmentSetting->setObjectName(QStringLiteral("toolButton_EquipmentSetting"));
        toolButton_EquipmentSetting->setMinimumSize(QSize(140, 150));
        toolButton_EquipmentSetting->setFont(font);
        toolButton_EquipmentSetting->setStyleSheet(QStringLiteral(""));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/Picture/setting.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_EquipmentSetting->setIcon(icon5);
        toolButton_EquipmentSetting->setIconSize(QSize(150, 120));
        toolButton_EquipmentSetting->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_EquipmentSetting);

        toolButton_Exit = new QToolButton(widget_top);
        toolButton_Exit->setObjectName(QStringLiteral("toolButton_Exit"));
        toolButton_Exit->setMinimumSize(QSize(140, 150));
        toolButton_Exit->setFont(font);
        toolButton_Exit->setCursor(QCursor(Qt::ArrowCursor));
        toolButton_Exit->setMouseTracking(true);
        toolButton_Exit->setTabletTracking(true);
        toolButton_Exit->setStyleSheet(QStringLiteral(""));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/Picture/exit_out.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_Exit->setIcon(icon6);
        toolButton_Exit->setIconSize(QSize(150, 120));
        toolButton_Exit->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_Exit);

        toolButton_about = new QToolButton(widget_top);
        toolButton_about->setObjectName(QStringLiteral("toolButton_about"));
        toolButton_about->setMinimumSize(QSize(140, 150));
        toolButton_about->setFont(font);
        toolButton_about->setCursor(QCursor(Qt::ArrowCursor));
        toolButton_about->setMouseTracking(true);
        toolButton_about->setTabletTracking(true);
        toolButton_about->setStyleSheet(QStringLiteral(""));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/Picture/about64main.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_about->setIcon(icon7);
        toolButton_about->setIconSize(QSize(150, 120));
        toolButton_about->setAutoRepeatDelay(300);
        toolButton_about->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout_3->addWidget(toolButton_about);


        gridLayout_2->addWidget(widget_top, 0, 1, 2, 1);

        widget_btnfucn = new QWidget(MainFucnWidget);
        widget_btnfucn->setObjectName(QStringLiteral("widget_btnfucn"));
        verticalLayout_2 = new QVBoxLayout(widget_btnfucn);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(5, 0, 0, 0);
        toolButton_sampleTest_1 = new QToolButton(widget_btnfucn);
        toolButton_sampleTest_1->setObjectName(QStringLiteral("toolButton_sampleTest_1"));
        toolButton_sampleTest_1->setMinimumSize(QSize(140, 150));
        toolButton_sampleTest_1->setFont(font);
        toolButton_sampleTest_1->setStyleSheet(QStringLiteral(""));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/Picture/test.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_sampleTest_1->setIcon(icon8);
        toolButton_sampleTest_1->setIconSize(QSize(150, 120));
        toolButton_sampleTest_1->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        verticalLayout_2->addWidget(toolButton_sampleTest_1);

        toolButton_resultView_2 = new QToolButton(widget_btnfucn);
        toolButton_resultView_2->setObjectName(QStringLiteral("toolButton_resultView_2"));
        toolButton_resultView_2->setMinimumSize(QSize(140, 150));
        toolButton_resultView_2->setFont(font);
        toolButton_resultView_2->setStyleSheet(QStringLiteral(""));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/Picture/Calibreta.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_resultView_2->setIcon(icon9);
        toolButton_resultView_2->setIconSize(QSize(150, 120));
        toolButton_resultView_2->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        verticalLayout_2->addWidget(toolButton_resultView_2);

        toolButton_testingCurve_3 = new QToolButton(widget_btnfucn);
        toolButton_testingCurve_3->setObjectName(QStringLiteral("toolButton_testingCurve_3"));
        toolButton_testingCurve_3->setMinimumSize(QSize(140, 150));
        toolButton_testingCurve_3->setFont(font);
        toolButton_testingCurve_3->setStyleSheet(QStringLiteral(""));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/Picture/Graphplot.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_testingCurve_3->setIcon(icon10);
        toolButton_testingCurve_3->setIconSize(QSize(150, 120));
        toolButton_testingCurve_3->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        verticalLayout_2->addWidget(toolButton_testingCurve_3);

        toolButton_TestingResult_4 = new QToolButton(widget_btnfucn);
        toolButton_TestingResult_4->setObjectName(QStringLiteral("toolButton_TestingResult_4"));
        toolButton_TestingResult_4->setMinimumSize(QSize(140, 150));
        toolButton_TestingResult_4->setFont(font);
        toolButton_TestingResult_4->setStyleSheet(QStringLiteral(""));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/Picture/viewresults.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_TestingResult_4->setIcon(icon11);
        toolButton_TestingResult_4->setIconSize(QSize(150, 120));
        toolButton_TestingResult_4->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        verticalLayout_2->addWidget(toolButton_TestingResult_4);

        toolButton_ControlSuppile_5 = new QToolButton(widget_btnfucn);
        toolButton_ControlSuppile_5->setObjectName(QStringLiteral("toolButton_ControlSuppile_5"));
        toolButton_ControlSuppile_5->setMinimumSize(QSize(140, 150));
        toolButton_ControlSuppile_5->setFont(font);
        toolButton_ControlSuppile_5->setStyleSheet(QStringLiteral(""));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/Picture/qualityControl.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_ControlSuppile_5->setIcon(icon12);
        toolButton_ControlSuppile_5->setIconSize(QSize(150, 120));
        toolButton_ControlSuppile_5->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        verticalLayout_2->addWidget(toolButton_ControlSuppile_5);


        gridLayout_2->addWidget(widget_btnfucn, 2, 0, 1, 1);

        tabWidget_Main = new QTabWidget(MainFucnWidget);
        tabWidget_Main->setObjectName(QStringLiteral("tabWidget_Main"));
        tabWidget_Main->setCursor(QCursor(Qt::ArrowCursor));
        tabWidget_Main->setMouseTracking(true);
        tabWidget_Main->setTabletTracking(true);
        tabWidget_Main->setContextMenuPolicy(Qt::DefaultContextMenu);
        tabWidget_Main->setAutoFillBackground(false);
        tabWidget_Main->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        tabWidget_Main->setTabPosition(QTabWidget::North);
        tabWidget_Main->setDocumentMode(false);
        tabWidget_Main->setTabsClosable(false);
        TestingSample = new QWidget();
        TestingSample->setObjectName(QStringLiteral("TestingSample"));
        tabWidget_Main->addTab(TestingSample, QString());

        gridLayout_2->addWidget(tabWidget_Main, 2, 1, 1, 1);


        verticalLayout->addWidget(MainFucnWidget);

        StatusBar = new QWidget(centralWidget);
        StatusBar->setObjectName(QStringLiteral("StatusBar"));
        StatusBar->setMinimumSize(QSize(0, 40));
        StatusBar->setMaximumSize(QSize(16777215, 60));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(12);
        StatusBar->setFont(font1);
        StatusBar->setStyleSheet(QLatin1String("QWidget#StatusBar{  \n"
"    border: 1px solid rgb(220, 220, 220); \n"
"	background-color:  rgb(255, 255, 255);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} \n"
" \n"
""));
        horizontalLayout_status = new QHBoxLayout(StatusBar);
        horizontalLayout_status->setSpacing(5);
        horizontalLayout_status->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_status->setObjectName(QStringLiteral("horizontalLayout_status"));
        horizontalLayout_status->setContentsMargins(0, 0, 10, 0);
        widget_usbstate = new QWidget(StatusBar);
        widget_usbstate->setObjectName(QStringLiteral("widget_usbstate"));
        widget_usbstate->setMinimumSize(QSize(160, 35));
        widget_usbstate->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(220, 220, 220);    \n"
"background-color: rgb(255, 255, 255); \n"
"border-style: solid;  \n"
"border-radius:10px;  \n"
"padding:0 0px;\n"
"font-family: \"\346\245\267\344\275\223\";\n"
"font-size: 22px  "));
        horizontalLayout_2 = new QHBoxLayout(widget_usbstate);
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 0, 0, 0);
        label_machineStaus_show = new QLabel(widget_usbstate);
        label_machineStaus_show->setObjectName(QStringLiteral("label_machineStaus_show"));
        label_machineStaus_show->setMinimumSize(QSize(32, 32));
        label_machineStaus_show->setMaximumSize(QSize(32, 32));
        label_machineStaus_show->setFont(font);
        label_machineStaus_show->setStyleSheet(QString::fromUtf8("QLabel#label_machineStaus_show{  \n"
"    border: 0px solid rgb(220, 220, 220);    \n"
"	background-color: rgb(255, 255, 255); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\245\267\344\275\223\";\n"
"    font-size: 22px  	\n"
"} "));
        label_machineStaus_show->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_machineStaus_show);

        label_connectstate = new QLabel(widget_usbstate);
        label_connectstate->setObjectName(QStringLiteral("label_connectstate"));
        label_connectstate->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        horizontalLayout_2->addWidget(label_connectstate);


        horizontalLayout_status->addWidget(widget_usbstate);

        widget_ModulI = new QWidget(StatusBar);
        widget_ModulI->setObjectName(QStringLiteral("widget_ModulI"));
        widget_ModulI->setMinimumSize(QSize(150, 0));
        widget_ModulI->setStyleSheet(QString::fromUtf8("QWidget#widget_ModulI{  \n"
"    border: 1px solid rgb(220, 220, 220);  \n"
"	background-color:  rgb(255, 255, 255);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\245\267\344\275\223\";\n"
"    font-size: 20px  	\n"
"} "));
        horizontalLayout = new QHBoxLayout(widget_ModulI);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        label_Module1 = new QLabel(widget_ModulI);
        label_Module1->setObjectName(QStringLiteral("label_Module1"));
        label_Module1->setMinimumSize(QSize(100, 0));
        label_Module1->setMaximumSize(QSize(100, 30));
        label_Module1->setFont(font);
        label_Module1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        horizontalLayout->addWidget(label_Module1);

        label_showModule1_value = new QLabel(widget_ModulI);
        label_showModule1_value->setObjectName(QStringLiteral("label_showModule1_value"));
        label_showModule1_value->setMinimumSize(QSize(40, 20));
        label_showModule1_value->setMaximumSize(QSize(80, 30));
        label_showModule1_value->setFont(font);
        label_showModule1_value->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        label_showModule1_value->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label_showModule1_value);


        horizontalLayout_status->addWidget(widget_ModulI);

        widget_ModulII = new QWidget(StatusBar);
        widget_ModulII->setObjectName(QStringLiteral("widget_ModulII"));
        widget_ModulII->setMinimumSize(QSize(150, 0));
        widget_ModulII->setStyleSheet(QString::fromUtf8("QWidget#widget_ModulII{  \n"
"    border: 1px solid rgb(220, 220, 220);   \n"
"	background-color:   rgb(255, 255, 255); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\245\267\344\275\223\";\n"
"    font-size: 20px  	\n"
"} "));
        horizontalLayout_13 = new QHBoxLayout(widget_ModulII);
        horizontalLayout_13->setSpacing(0);
        horizontalLayout_13->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        horizontalLayout_13->setContentsMargins(5, 5, 5, 5);
        label_Module1_2 = new QLabel(widget_ModulII);
        label_Module1_2->setObjectName(QStringLiteral("label_Module1_2"));
        label_Module1_2->setMinimumSize(QSize(100, 0));
        label_Module1_2->setMaximumSize(QSize(100, 30));
        label_Module1_2->setFont(font);
        label_Module1_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        horizontalLayout_13->addWidget(label_Module1_2);

        label_showModule2_value_2 = new QLabel(widget_ModulII);
        label_showModule2_value_2->setObjectName(QStringLiteral("label_showModule2_value_2"));
        label_showModule2_value_2->setMinimumSize(QSize(40, 20));
        label_showModule2_value_2->setMaximumSize(QSize(80, 30));
        label_showModule2_value_2->setFont(font);
        label_showModule2_value_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        label_showModule2_value_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(label_showModule2_value_2);


        horizontalLayout_status->addWidget(widget_ModulII);

        MachineAxisValue = new QLabel(StatusBar);
        MachineAxisValue->setObjectName(QStringLiteral("MachineAxisValue"));
        MachineAxisValue->setMinimumSize(QSize(0, 0));
        MachineAxisValue->setMaximumSize(QSize(1, 16777215));
        MachineAxisValue->setStyleSheet(QString::fromUtf8("QLabel#MachineAxisValue{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(120, 120, 120);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\226\260\345\256\213\344\275\223\";\n"
"    font-size: 24px  	\n"
"} "));
        MachineAxisValue->setFrameShape(QFrame::NoFrame);

        horizontalLayout_status->addWidget(MachineAxisValue);

        widget_ModulIII = new QWidget(StatusBar);
        widget_ModulIII->setObjectName(QStringLiteral("widget_ModulIII"));
        widget_ModulIII->setMinimumSize(QSize(150, 0));
        widget_ModulIII->setStyleSheet(QString::fromUtf8("QWidget#widget_ModulIII{  \n"
"    border: 1px solid rgb(220, 220, 220);   \n"
"	background-color:rgb(255, 255, 255); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\245\267\344\275\223\";\n"
"    font-size: 20px  	\n"
"} "));
        horizontalLayout_16 = new QHBoxLayout(widget_ModulIII);
        horizontalLayout_16->setSpacing(0);
        horizontalLayout_16->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        horizontalLayout_16->setContentsMargins(5, 5, 5, 5);
        label_Module1_3 = new QLabel(widget_ModulIII);
        label_Module1_3->setObjectName(QStringLiteral("label_Module1_3"));
        label_Module1_3->setMinimumSize(QSize(100, 0));
        label_Module1_3->setMaximumSize(QSize(100, 30));
        label_Module1_3->setFont(font);
        label_Module1_3->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        horizontalLayout_16->addWidget(label_Module1_3);

        label_showModule3_value_2 = new QLabel(widget_ModulIII);
        label_showModule3_value_2->setObjectName(QStringLiteral("label_showModule3_value_2"));
        label_showModule3_value_2->setMinimumSize(QSize(40, 20));
        label_showModule3_value_2->setMaximumSize(QSize(80, 30));
        label_showModule3_value_2->setFont(font);
        label_showModule3_value_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        label_showModule3_value_2->setAlignment(Qt::AlignCenter);

        horizontalLayout_16->addWidget(label_showModule3_value_2);


        horizontalLayout_status->addWidget(widget_ModulIII);

        widget_ReminderLiqune = new QWidget(StatusBar);
        widget_ReminderLiqune->setObjectName(QStringLiteral("widget_ReminderLiqune"));
        widget_ReminderLiqune->setMinimumSize(QSize(260, 0));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        widget_ReminderLiqune->setFont(font2);
        widget_ReminderLiqune->setStyleSheet(QString::fromUtf8("QWidget#widget_ReminderLiqune{  \n"
"    border: 1px solid  rgb(220, 220, 220);  \n"
"	background-color: rgb(255, 255, 255); \n"
"	color:rgb(250,250,250); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\226\260\345\256\213\344\275\223\";\n"
"    font-size: 24px  	\n"
"} "));
        gridLayout = new QGridLayout(widget_ReminderLiqune);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(15);
        gridLayout->setVerticalSpacing(5);
        gridLayout->setContentsMargins(5, 1, 5, 1);
        label_Dipaly_cleanlinque = new QLabel(widget_ReminderLiqune);
        label_Dipaly_cleanlinque->setObjectName(QStringLiteral("label_Dipaly_cleanlinque"));
        label_Dipaly_cleanlinque->setMinimumSize(QSize(40, 25));
        label_Dipaly_cleanlinque->setFont(font);
        label_Dipaly_cleanlinque->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_cleanlinque, 0, 0, 1, 1);

        label_Dipaly_ADP_2 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_ADP_2->setObjectName(QStringLiteral("label_Dipaly_ADP_2"));
        label_Dipaly_ADP_2->setFont(font);
        label_Dipaly_ADP_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_ADP_2, 1, 2, 1, 1);

        label_Dipaly_AA_1 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_AA_1->setObjectName(QStringLiteral("label_Dipaly_AA_1"));
        label_Dipaly_AA_1->setMinimumSize(QSize(60, 25));
        label_Dipaly_AA_1->setFont(font);
        label_Dipaly_AA_1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_AA_1, 0, 1, 1, 1);

        label_Dipaly_ADR_2 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_ADR_2->setObjectName(QStringLiteral("label_Dipaly_ADR_2"));
        label_Dipaly_ADR_2->setFont(font);
        label_Dipaly_ADR_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_ADR_2, 1, 3, 1, 1);

        label_Dipaly_ADR_1 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_ADR_1->setObjectName(QStringLiteral("label_Dipaly_ADR_1"));
        label_Dipaly_ADR_1->setMinimumSize(QSize(80, 25));
        label_Dipaly_ADR_1->setFont(font);
        label_Dipaly_ADR_1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_ADR_1, 0, 3, 1, 1);

        label_Dipaly_ADP_1 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_ADP_1->setObjectName(QStringLiteral("label_Dipaly_ADP_1"));
        label_Dipaly_ADP_1->setMinimumSize(QSize(80, 25));
        label_Dipaly_ADP_1->setFont(font);
        label_Dipaly_ADP_1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_ADP_1, 0, 2, 1, 1);

        label_Dipaly_COL_2 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_COL_2->setObjectName(QStringLiteral("label_Dipaly_COL_2"));
        label_Dipaly_COL_2->setFont(font);
        label_Dipaly_COL_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_COL_2, 1, 4, 1, 1);

        label_Dipaly_COL_1 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_COL_1->setObjectName(QStringLiteral("label_Dipaly_COL_1"));
        label_Dipaly_COL_1->setMinimumSize(QSize(80, 25));
        label_Dipaly_COL_1->setFont(font);
        label_Dipaly_COL_1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_COL_1, 0, 4, 1, 1);

        label_Dipaly_AA_2 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_AA_2->setObjectName(QStringLiteral("label_Dipaly_AA_2"));
        label_Dipaly_AA_2->setMinimumSize(QSize(0, 12));
        label_Dipaly_AA_2->setFont(font);
        label_Dipaly_AA_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_AA_2, 1, 1, 1, 1);

        label_Dipaly_RIS_1 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_RIS_1->setObjectName(QStringLiteral("label_Dipaly_RIS_1"));
        label_Dipaly_RIS_1->setMinimumSize(QSize(80, 25));
        label_Dipaly_RIS_1->setFont(font);
        label_Dipaly_RIS_1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_RIS_1, 0, 5, 1, 1);

        label_Dipaly_RIS_2 = new QLabel(widget_ReminderLiqune);
        label_Dipaly_RIS_2->setObjectName(QStringLiteral("label_Dipaly_RIS_2"));
        label_Dipaly_RIS_2->setFont(font);
        label_Dipaly_RIS_2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        gridLayout->addWidget(label_Dipaly_RIS_2, 1, 5, 1, 1);


        horizontalLayout_status->addWidget(widget_ReminderLiqune);

        show_wastlique = new QLabel(StatusBar);
        show_wastlique->setObjectName(QStringLiteral("show_wastlique"));
        show_wastlique->setMinimumSize(QSize(48, 32));
        show_wastlique->setMaximumSize(QSize(48, 36));
        show_wastlique->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        horizontalLayout_status->addWidget(show_wastlique);

        showcleanliqur = new QLabel(StatusBar);
        showcleanliqur->setObjectName(QStringLiteral("showcleanliqur"));
        showcleanliqur->setMinimumSize(QSize(48, 32));
        showcleanliqur->setMaximumSize(QSize(48, 36));
        showcleanliqur->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));

        horizontalLayout_status->addWidget(showcleanliqur);

        sStatusInfo_text = new QToolButton(StatusBar);
        sStatusInfo_text->setObjectName(QStringLiteral("sStatusInfo_text"));
        sStatusInfo_text->setMinimumSize(QSize(0, 0));
        sStatusInfo_text->setMaximumSize(QSize(16777215, 40));
        sStatusInfo_text->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/Picture/SetPng/text.png"), QSize(), QIcon::Normal, QIcon::Off);
        sStatusInfo_text->setIcon(icon13);
        sStatusInfo_text->setIconSize(QSize(32, 32));

        horizontalLayout_status->addWidget(sStatusInfo_text);

        sStatusInfo = new QToolButton(StatusBar);
        sStatusInfo->setObjectName(QStringLiteral("sStatusInfo"));
        sStatusInfo->setMinimumSize(QSize(0, 0));
        sStatusInfo->setMaximumSize(QSize(16777215, 40));
        sStatusInfo->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        QIcon icon14;
        icon14.addFile(QStringLiteral(":/Picture/icon_title/talk_nor.png"), QSize(), QIcon::Normal, QIcon::Off);
        sStatusInfo->setIcon(icon14);
        sStatusInfo->setIconSize(QSize(32, 32));

        horizontalLayout_status->addWidget(sStatusInfo);


        verticalLayout->addWidget(StatusBar);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        tabWidget_Main->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        Machine_type->setText(QString());
        toolButton_quality_start->setText(QApplication::translate("MainWindow", "\345\274\200\345\247\213", nullptr));
        toolButton_quality_sample->setText(QApplication::translate("MainWindow", "\346\232\202\345\201\234", nullptr));
        toolButton_quality_reset->setText(QApplication::translate("MainWindow", "\345\244\215\344\275\215", nullptr));
        toolButton_quality_cleaning->setText(QApplication::translate("MainWindow", "\346\270\205\346\264\227", nullptr));
        toolButton_quality_stop->setText(QApplication::translate("MainWindow", "\344\273\273\345\212\241", nullptr));
        toolButton_EquipmentSetting->setText(QApplication::translate("MainWindow", "\344\273\252\345\231\250\350\256\276\347\275\256", nullptr));
        toolButton_Exit->setText(QApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
        toolButton_about->setText(QApplication::translate("MainWindow", "\345\205\263\344\272\216", nullptr));
        toolButton_sampleTest_1->setText(QApplication::translate("MainWindow", "\346\240\267\346\234\254\346\265\213\350\257\225", nullptr));
        toolButton_resultView_2->setText(QApplication::translate("MainWindow", "\347\273\223\346\236\234\351\242\204\350\247\210", nullptr));
        toolButton_testingCurve_3->setText(QApplication::translate("MainWindow", "\346\265\213\350\257\225\346\233\262\347\272\277", nullptr));
        toolButton_TestingResult_4->setText(QApplication::translate("MainWindow", "\347\273\223\346\236\234\346\237\245\350\257\242", nullptr));
        toolButton_ControlSuppile_5->setText(QApplication::translate("MainWindow", "\350\200\227\346\235\220\347\256\241\347\220\206", nullptr));
        tabWidget_Main->setTabText(tabWidget_Main->indexOf(TestingSample), QApplication::translate("MainWindow", "\346\240\267\346\234\254\346\265\213\350\257\225", nullptr));
        label_machineStaus_show->setText(QString());
        label_connectstate->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_Module1->setText(QApplication::translate("MainWindow", "\346\250\241\347\273\2041(\342\204\203):", nullptr));
        label_showModule1_value->setText(QString());
        label_Module1_2->setText(QApplication::translate("MainWindow", "\346\250\241\347\273\2042(\342\204\203):", nullptr));
        label_showModule2_value_2->setText(QString());
        MachineAxisValue->setText(QString());
        label_Module1_3->setText(QApplication::translate("MainWindow", "\346\250\241\347\273\2043(\342\204\203):", nullptr));
        label_showModule3_value_2->setText(QString());
        label_Dipaly_cleanlinque->setText(QApplication::translate("MainWindow", "S:", nullptr));
        label_Dipaly_ADP_2->setText(QApplication::translate("MainWindow", "ADP(R):", nullptr));
        label_Dipaly_AA_1->setText(QApplication::translate("MainWindow", "AA(L):", nullptr));
        label_Dipaly_ADR_2->setText(QApplication::translate("MainWindow", "EPI(R):", nullptr));
        label_Dipaly_ADR_1->setText(QApplication::translate("MainWindow", "EPI(L):", nullptr));
        label_Dipaly_ADP_1->setText(QApplication::translate("MainWindow", "ADP(L):", nullptr));
        label_Dipaly_COL_2->setText(QApplication::translate("MainWindow", "COL(R):", nullptr));
        label_Dipaly_COL_1->setText(QApplication::translate("MainWindow", "COL(L):", nullptr));
        label_Dipaly_AA_2->setText(QApplication::translate("MainWindow", "AA(R):", nullptr));
        label_Dipaly_RIS_1->setText(QApplication::translate("MainWindow", "RIS(L):", nullptr));
        label_Dipaly_RIS_2->setText(QApplication::translate("MainWindow", "RIS(R):", nullptr));
        show_wastlique->setText(QString());
        showcleanliqur->setText(QString());
        sStatusInfo_text->setText(QString());
        sStatusInfo->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
