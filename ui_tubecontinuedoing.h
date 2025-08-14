/********************************************************************************
** Form generated from reading UI file 'tubecontinuedoing.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TUBECONTINUEDOING_H
#define UI_TUBECONTINUEDOING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TubeContinueDoing
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_titlebar;
    QHBoxLayout *horizontalLayout_bar;
    QLabel *label_nametitle;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_close;
    QWidget *widget_control;
    QVBoxLayout *verticalLayout_2;
    QGroupBox *groupBox_absorbReagent;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QComboBox *comboBox_selReagent;
    QSpinBox *spinBox_spitheigh;
    QLabel *label;
    QComboBox *comboBox_Channel;
    QDoubleSpinBox *doubleSpinBox_addratio;
    QLabel *label_7;
    QLabel *label_9;
    QLabel *label_8;
    QComboBox *comboBox_falsesuck;
    QLabel *label_suck;
    QLabel *label_3;
    QLabel *label_absorb;
    QSpinBox *spinBox_ReagentVol;
    QLabel *label_11;
    QSpinBox *spinBox_suckAir;
    QToolButton *toolButton_runTest;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QSpinBox *spinBox_PPPVol;
    QSpinBox *spinBox_Hole;
    QLabel *label_5;
    QSpinBox *spinBox_emptyHole;
    QLabel *label_6;
    QSpacerItem *horizontalSpacer_5;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_4;
    QToolButton *toolButton_PPPok;
    QGroupBox *groupBox_2;
    QToolButton *toolButton_cruveCreat;
    QLabel *label_suck_num;
    QLabel *label_suck_times;
    QSpinBox *spinBox_suck_times;
    QLabel *label_suck_vol;
    QSpinBox *spinBox_suck_vol;
    QComboBox *comboBox_suck_hole;

    void setupUi(QWidget *TubeContinueDoing)
    {
        if (TubeContinueDoing->objectName().isEmpty())
            TubeContinueDoing->setObjectName(QStringLiteral("TubeContinueDoing"));
        TubeContinueDoing->resize(824, 621);
        TubeContinueDoing->setStyleSheet(QLatin1String("QWidget#TubeContinueDoing\n"
"{\n"
"    border: 1px solid #C0C0C0;\n"
"	background-color: rgb(160, 160, 160);\n"
"    border-style: solid;\n"
"    border-radius:10px;\n"
"    padding:0 0px;\n"
"}"));
        verticalLayout = new QVBoxLayout(TubeContinueDoing);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_titlebar = new QWidget(TubeContinueDoing);
        widget_titlebar->setObjectName(QStringLiteral("widget_titlebar"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_titlebar->sizePolicy().hasHeightForWidth());
        widget_titlebar->setSizePolicy(sizePolicy);
        widget_titlebar->setMinimumSize(QSize(0, 40));
        widget_titlebar->setMaximumSize(QSize(16777215, 40));
        widget_titlebar->setStyleSheet(QLatin1String("QWidget#widget_titlebar\n"
"{\n"
"    border: 0px solid #FFFFE0;\n"
"	background-color: rgb(160, 160, 160);\n"
"    border-style: solid;\n"
"    border-radius:10px;\n"
"    padding:0 0px;\n"
"}"));
        horizontalLayout_bar = new QHBoxLayout(widget_titlebar);
        horizontalLayout_bar->setObjectName(QStringLiteral("horizontalLayout_bar"));
        horizontalLayout_bar->setContentsMargins(1, 0, 1, 0);
        label_nametitle = new QLabel(widget_titlebar);
        label_nametitle->setObjectName(QStringLiteral("label_nametitle"));
        label_nametitle->setMinimumSize(QSize(180, 32));
        label_nametitle->setMaximumSize(QSize(200, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(12);
        label_nametitle->setFont(font);
        label_nametitle->setLayoutDirection(Qt::LeftToRight);
        label_nametitle->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_bar->addWidget(label_nametitle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_bar->addItem(horizontalSpacer);

        toolButton_close = new QToolButton(widget_titlebar);
        toolButton_close->setObjectName(QStringLiteral("toolButton_close"));
        toolButton_close->setMinimumSize(QSize(16, 16));
        toolButton_close->setMaximumSize(QSize(16777215, 32));
        toolButton_close->setStyleSheet(QLatin1String("QToolButton{\n"
"   min-width:16px;\n"
"   min-height:16px;\n"
"   border-style:outset; \n"
"}"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Picture/SetPng/error.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_close->setIcon(icon);
        toolButton_close->setIconSize(QSize(32, 32));

        horizontalLayout_bar->addWidget(toolButton_close);


        verticalLayout->addWidget(widget_titlebar);

        widget_control = new QWidget(TubeContinueDoing);
        widget_control->setObjectName(QStringLiteral("widget_control"));
        widget_control->setMinimumSize(QSize(824, 580));
        widget_control->setStyleSheet(QLatin1String("QWidget#widget_control\n"
"{\n"
"    border: 1px solid #E0FFFF;\n"
"	background-color: rgb(160, 160, 160);\n"
"    border-style: solid;\n"
"    border-radius:10px;\n"
"    padding:0 0px;\n"
"}"));
        verticalLayout_2 = new QVBoxLayout(widget_control);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        groupBox_absorbReagent = new QGroupBox(widget_control);
        groupBox_absorbReagent->setObjectName(QStringLiteral("groupBox_absorbReagent"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(12);
        font1.setUnderline(false);
        groupBox_absorbReagent->setFont(font1);
        gridLayout = new QGridLayout(groupBox_absorbReagent);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(10);
        gridLayout->setVerticalSpacing(5);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(groupBox_absorbReagent);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(88, 25));
        label_2->setMaximumSize(QSize(16777215, 30));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font2.setPointSize(12);
        label_2->setFont(font2);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        comboBox_selReagent = new QComboBox(groupBox_absorbReagent);
        comboBox_selReagent->setObjectName(QStringLiteral("comboBox_selReagent"));
        comboBox_selReagent->setMinimumSize(QSize(0, 25));
        comboBox_selReagent->setMaximumSize(QSize(16777215, 30));
        QFont font3;
        comboBox_selReagent->setFont(font3);
        comboBox_selReagent->setStyleSheet(QString::fromUtf8("QComboBox{\n"
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

        gridLayout->addWidget(comboBox_selReagent, 0, 1, 1, 1);

        spinBox_spitheigh = new QSpinBox(groupBox_absorbReagent);
        spinBox_spitheigh->setObjectName(QStringLiteral("spinBox_spitheigh"));
        spinBox_spitheigh->setMinimumSize(QSize(120, 0));
        spinBox_spitheigh->setMaximumSize(QSize(16777215, 30));
        spinBox_spitheigh->setSizeIncrement(QSize(0, 30));
        QFont font4;
        font4.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        spinBox_spitheigh->setFont(font4);
        spinBox_spitheigh->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
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
"}\n"
""));
        spinBox_spitheigh->setAlignment(Qt::AlignCenter);
        spinBox_spitheigh->setMinimum(1);
        spinBox_spitheigh->setMaximum(70);
        spinBox_spitheigh->setValue(1);

        gridLayout->addWidget(spinBox_spitheigh, 0, 6, 1, 1);

        label = new QLabel(groupBox_absorbReagent);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(136, 25));
        label->setMaximumSize(QSize(16777215, 30));
        label->setFont(font2);

        gridLayout->addWidget(label, 0, 2, 1, 1);

        comboBox_Channel = new QComboBox(groupBox_absorbReagent);
        comboBox_Channel->setObjectName(QStringLiteral("comboBox_Channel"));
        comboBox_Channel->setMinimumSize(QSize(0, 25));
        comboBox_Channel->setMaximumSize(QSize(16777215, 30));
        comboBox_Channel->setFont(font3);
        comboBox_Channel->setStyleSheet(QString::fromUtf8("QComboBox{\n"
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
        comboBox_Channel->setMaxVisibleItems(12);

        gridLayout->addWidget(comboBox_Channel, 0, 3, 1, 1);

        doubleSpinBox_addratio = new QDoubleSpinBox(groupBox_absorbReagent);
        doubleSpinBox_addratio->setObjectName(QStringLiteral("doubleSpinBox_addratio"));
        doubleSpinBox_addratio->setMinimumSize(QSize(0, 30));
        doubleSpinBox_addratio->setFont(font4);
        doubleSpinBox_addratio->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:left;\n"
" 	image: url( :/Picture/cssup.png);\n"
"    width: 12px;\n"
"    height: 20px;\n"
"}\n"
"QDoubleSpinBox \n"
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
        doubleSpinBox_addratio->setMaximum(1);
        doubleSpinBox_addratio->setSingleStep(0.1);

        gridLayout->addWidget(doubleSpinBox_addratio, 1, 3, 1, 1);

        label_7 = new QLabel(groupBox_absorbReagent);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setMinimumSize(QSize(0, 30));
        label_7->setFont(font2);

        gridLayout->addWidget(label_7, 1, 0, 1, 1);

        label_9 = new QLabel(groupBox_absorbReagent);
        label_9->setObjectName(QStringLiteral("label_9"));
        label_9->setFont(font2);

        gridLayout->addWidget(label_9, 1, 4, 1, 1);

        label_8 = new QLabel(groupBox_absorbReagent);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setMinimumSize(QSize(100, 0));
        label_8->setMaximumSize(QSize(16777215, 30));
        label_8->setFont(font2);

        gridLayout->addWidget(label_8, 1, 2, 1, 1);

        comboBox_falsesuck = new QComboBox(groupBox_absorbReagent);
        comboBox_falsesuck->setObjectName(QStringLiteral("comboBox_falsesuck"));
        comboBox_falsesuck->setMinimumSize(QSize(0, 30));
        comboBox_falsesuck->setFont(font3);
        comboBox_falsesuck->setStyleSheet(QString::fromUtf8("QComboBox{\n"
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

        gridLayout->addWidget(comboBox_falsesuck, 1, 5, 1, 2);

        label_suck = new QLabel(groupBox_absorbReagent);
        label_suck->setObjectName(QStringLiteral("label_suck"));
        label_suck->setMaximumSize(QSize(16777215, 60));
        QFont font5;
        font5.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font5.setPointSize(12);
        font5.setUnderline(false);
        label_suck->setFont(font5);
        label_suck->setFrameShape(QFrame::Box);

        gridLayout->addWidget(label_suck, 3, 0, 1, 4);

        label_3 = new QLabel(groupBox_absorbReagent);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(120, 0));
        label_3->setMaximumSize(QSize(16777215, 30));
        label_3->setFont(font2);

        gridLayout->addWidget(label_3, 0, 4, 1, 1);

        label_absorb = new QLabel(groupBox_absorbReagent);
        label_absorb->setObjectName(QStringLiteral("label_absorb"));
        label_absorb->setMaximumSize(QSize(16777215, 30));
        label_absorb->setFont(font);
        label_absorb->setFrameShape(QFrame::Box);

        gridLayout->addWidget(label_absorb, 3, 4, 1, 3);

        spinBox_ReagentVol = new QSpinBox(groupBox_absorbReagent);
        spinBox_ReagentVol->setObjectName(QStringLiteral("spinBox_ReagentVol"));
        spinBox_ReagentVol->setMinimumSize(QSize(0, 30));
        spinBox_ReagentVol->setFont(font4);
        spinBox_ReagentVol->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
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
"}\n"
""));
        spinBox_ReagentVol->setAlignment(Qt::AlignCenter);
        spinBox_ReagentVol->setMaximum(250);

        gridLayout->addWidget(spinBox_ReagentVol, 1, 1, 1, 1);

        label_11 = new QLabel(groupBox_absorbReagent);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setFont(font2);

        gridLayout->addWidget(label_11, 2, 0, 1, 1);

        spinBox_suckAir = new QSpinBox(groupBox_absorbReagent);
        spinBox_suckAir->setObjectName(QStringLiteral("spinBox_suckAir"));
        spinBox_suckAir->setMinimumSize(QSize(0, 30));
        spinBox_suckAir->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
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
"}\n"
""));
        spinBox_suckAir->setMaximum(5000);
        spinBox_suckAir->setValue(720);

        gridLayout->addWidget(spinBox_suckAir, 2, 1, 1, 1);

        toolButton_runTest = new QToolButton(groupBox_absorbReagent);
        toolButton_runTest->setObjectName(QStringLiteral("toolButton_runTest"));
        toolButton_runTest->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_runTest{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QToolButton#toolButton_runTest:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:' \346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_runTest:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:white;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Picture/media_playback_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_runTest->setIcon(icon1);
        toolButton_runTest->setIconSize(QSize(32, 32));
        toolButton_runTest->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout->addWidget(toolButton_runTest, 2, 5, 1, 2);


        verticalLayout_2->addWidget(groupBox_absorbReagent);

        groupBox = new QGroupBox(widget_control);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(0, 90));
        groupBox->setMaximumSize(QSize(16777215, 120));
        groupBox->setFont(font2);
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setHorizontalSpacing(0);
        gridLayout_2->setVerticalSpacing(10);
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        spinBox_PPPVol = new QSpinBox(groupBox);
        spinBox_PPPVol->setObjectName(QStringLiteral("spinBox_PPPVol"));
        spinBox_PPPVol->setMinimumSize(QSize(120, 30));
        spinBox_PPPVol->setFont(font4);
        spinBox_PPPVol->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
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
        spinBox_PPPVol->setAlignment(Qt::AlignCenter);
        spinBox_PPPVol->setMinimum(0);
        spinBox_PPPVol->setMaximum(1000);

        gridLayout_2->addWidget(spinBox_PPPVol, 0, 1, 1, 1);

        spinBox_Hole = new QSpinBox(groupBox);
        spinBox_Hole->setObjectName(QStringLiteral("spinBox_Hole"));
        spinBox_Hole->setMinimumSize(QSize(120, 30));
        spinBox_Hole->setFont(font4);
        spinBox_Hole->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
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
        spinBox_Hole->setAlignment(Qt::AlignCenter);
        spinBox_Hole->setMaximum(119);

        gridLayout_2->addWidget(spinBox_Hole, 0, 4, 1, 1);

        label_5 = new QLabel(groupBox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(0, 30));
        label_5->setFont(font2);

        gridLayout_2->addWidget(label_5, 0, 3, 1, 1);

        spinBox_emptyHole = new QSpinBox(groupBox);
        spinBox_emptyHole->setObjectName(QStringLiteral("spinBox_emptyHole"));
        spinBox_emptyHole->setMinimumSize(QSize(120, 30));
        spinBox_emptyHole->setFont(font4);
        spinBox_emptyHole->setStyleSheet(QString::fromUtf8("QDoubleSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QDoubleSpinBox::down-button,QSpinBox::down-button {\n"
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
        spinBox_emptyHole->setAlignment(Qt::AlignCenter);
        spinBox_emptyHole->setMaximum(59);

        gridLayout_2->addWidget(spinBox_emptyHole, 0, 7, 1, 1);

        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setMinimumSize(QSize(0, 30));
        label_6->setFont(font2);

        gridLayout_2->addWidget(label_6, 0, 6, 1, 1);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_5, 0, 5, 1, 1);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_4, 0, 2, 1, 1);

        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMinimumSize(QSize(0, 30));
        label_4->setFont(font2);

        gridLayout_2->addWidget(label_4, 0, 0, 1, 1);

        toolButton_PPPok = new QToolButton(groupBox);
        toolButton_PPPok->setObjectName(QStringLiteral("toolButton_PPPok"));
        QFont font6;
        font6.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        toolButton_PPPok->setFont(font6);
        toolButton_PPPok->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_PPPok{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QToolButton#toolButton_PPPok:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_PPPok:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:white;\n"
"}"));
        toolButton_PPPok->setIcon(icon1);
        toolButton_PPPok->setIconSize(QSize(32, 32));
        toolButton_PPPok->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_2->addWidget(toolButton_PPPok, 1, 7, 1, 1);


        verticalLayout_2->addWidget(groupBox);

        groupBox_2 = new QGroupBox(widget_control);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        toolButton_cruveCreat = new QToolButton(groupBox_2);
        toolButton_cruveCreat->setObjectName(QStringLiteral("toolButton_cruveCreat"));
        toolButton_cruveCreat->setGeometry(QRect(570, 80, 131, 35));
        toolButton_cruveCreat->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_cruveCreat{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:#F5FFFA;\n"
"}  \n"
"QToolButton#toolButton_cruveCreat:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_cruveCreat:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 100px;  \n"
"    height:30px;  \n"
"    padding:0 30px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:whit"
                        "e;\n"
"}"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Picture/test.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_cruveCreat->setIcon(icon2);
        toolButton_cruveCreat->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        label_suck_num = new QLabel(groupBox_2);
        label_suck_num->setObjectName(QStringLiteral("label_suck_num"));
        label_suck_num->setGeometry(QRect(10, 20, 72, 30));
        label_suck_num->setFont(font2);
        label_suck_times = new QLabel(groupBox_2);
        label_suck_times->setObjectName(QStringLiteral("label_suck_times"));
        label_suck_times->setGeometry(QRect(240, 20, 72, 30));
        label_suck_times->setFont(font2);
        spinBox_suck_times = new QSpinBox(groupBox_2);
        spinBox_suck_times->setObjectName(QStringLiteral("spinBox_suck_times"));
        spinBox_suck_times->setGeometry(QRect(330, 20, 131, 30));
        spinBox_suck_times->setStyleSheet(QString::fromUtf8("QSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QSpinBox::down-button,QSpinBox::down-button {\n"
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
"	font-family: \"\346\245\267\344\275\223\";\n"
"	font-size: 20px;\n"
"}\n"
""));
        spinBox_suck_times->setAlignment(Qt::AlignCenter);
        spinBox_suck_times->setMinimum(1);
        label_suck_vol = new QLabel(groupBox_2);
        label_suck_vol->setObjectName(QStringLiteral("label_suck_vol"));
        label_suck_vol->setGeometry(QRect(470, 20, 88, 30));
        label_suck_vol->setFont(font2);
        spinBox_suck_vol = new QSpinBox(groupBox_2);
        spinBox_suck_vol->setObjectName(QStringLiteral("spinBox_suck_vol"));
        spinBox_suck_vol->setGeometry(QRect(570, 20, 131, 30));
        spinBox_suck_vol->setStyleSheet(QString::fromUtf8("QSpinBox::up-button,QSpinBox::up-button\n"
"{\n"
"	subcontrol-origin:border;\n"
"    subcontrol-position:right;\n"
" 	image: url(  :/Picture/cssdown.png);\n"
"    width: 12px;\n"
"    height: 20px;       \n"
"}\n"
"QSpinBox::down-button,QSpinBox::down-button {\n"
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
"	font-family: \"\346\245\267\344\275\223\";\n"
"	font-size: 20px;\n"
"}\n"
""));
        spinBox_suck_vol->setAlignment(Qt::AlignCenter);
        spinBox_suck_vol->setMinimum(200);
        spinBox_suck_vol->setMaximum(2000);
        comboBox_suck_hole = new QComboBox(groupBox_2);
        comboBox_suck_hole->setObjectName(QStringLiteral("comboBox_suck_hole"));
        comboBox_suck_hole->setGeometry(QRect(90, 20, 131, 30));
        comboBox_suck_hole->setStyleSheet(QString::fromUtf8("QComboBox{\n"
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

        verticalLayout_2->addWidget(groupBox_2);


        verticalLayout->addWidget(widget_control);


        retranslateUi(TubeContinueDoing);

        QMetaObject::connectSlotsByName(TubeContinueDoing);
    } // setupUi

    void retranslateUi(QWidget *TubeContinueDoing)
    {
        TubeContinueDoing->setWindowTitle(QApplication::translate("TubeContinueDoing", "Form", nullptr));
        label_nametitle->setText(QApplication::translate("TubeContinueDoing", "TextLabel", nullptr));
        toolButton_close->setText(QApplication::translate("TubeContinueDoing", "...", nullptr));
        groupBox_absorbReagent->setTitle(QApplication::translate("TubeContinueDoing", "\345\220\270\345\220\220\346\265\213\350\257\225\350\257\225\345\211\202\346\265\213\350\257\225", nullptr));
        label_2->setText(QApplication::translate("TubeContinueDoing", "\345\220\270\350\257\225\345\211\202\347\261\273\345\236\213:", nullptr));
        label->setText(QApplication::translate("TubeContinueDoing", "\345\220\220\350\257\225\345\211\202\345\210\260\346\265\213\350\257\225\351\200\232\351\201\223:", nullptr));
        label_7->setText(QApplication::translate("TubeContinueDoing", "\345\220\270\350\257\225\345\211\202\351\207\217(K):", nullptr));
        label_9->setText(QApplication::translate("TubeContinueDoing", "\345\220\220\350\257\225\345\211\202\351\205\215\347\275\256:", nullptr));
        label_8->setText(QApplication::translate("TubeContinueDoing", "\346\267\273\345\212\240\346\257\224\344\276\213(R)\357\274\232", nullptr));
        label_suck->setText(QString());
        label_3->setText(QApplication::translate("TubeContinueDoing", "\351\200\232\351\201\223\345\220\220\350\257\225\345\211\202\351\253\230\345\272\246:", nullptr));
        label_absorb->setText(QString());
        spinBox_ReagentVol->setSuffix(QApplication::translate("TubeContinueDoing", "ul", nullptr));
        label_11->setText(QApplication::translate("TubeContinueDoing", "\345\220\270\347\251\272\346\260\224\351\207\217:", nullptr));
        toolButton_runTest->setText(QApplication::translate("TubeContinueDoing", "\350\277\220\350\241\214\346\265\213\350\257\225", nullptr));
        groupBox->setTitle(QApplication::translate("TubeContinueDoing", "\345\220\270\345\220\220PPP\346\265\213\350\257\225", nullptr));
        label_5->setText(QApplication::translate("TubeContinueDoing", "PPP\345\255\224\345\217\267:", nullptr));
        label_6->setText(QApplication::translate("TubeContinueDoing", "PPP\345\220\220\345\210\260\347\251\272\350\257\225\347\256\241\357\274\232", nullptr));
        label_4->setText(QApplication::translate("TubeContinueDoing", "\345\220\270PPP\351\207\217:", nullptr));
        toolButton_PPPok->setText(QApplication::translate("TubeContinueDoing", "\350\277\220\350\241\214\346\265\213\350\257\225", nullptr));
        groupBox_2->setTitle(QApplication::translate("TubeContinueDoing", "fucn", nullptr));
        toolButton_cruveCreat->setText(QApplication::translate("TubeContinueDoing", "\345\274\200\345\247\213", nullptr));
        label_suck_num->setText(QApplication::translate("TubeContinueDoing", "\345\220\270\346\240\267\345\255\224\345\217\267:", nullptr));
        label_suck_times->setText(QApplication::translate("TubeContinueDoing", "\345\220\270\346\240\267\346\254\241\346\225\260:", nullptr));
        label_suck_vol->setText(QApplication::translate("TubeContinueDoing", "\345\215\225\346\254\241\345\220\270\346\240\267\351\207\217:", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TubeContinueDoing: public Ui_TubeContinueDoing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TUBECONTINUEDOING_H
