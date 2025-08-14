/********************************************************************************
** Form generated from reading UI file 'inquire_sql_info.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INQUIRE_SQL_INFO_H
#define UI_INQUIRE_SQL_INFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_Inquire_Sql_Info
{
public:
    QGridLayout *gridLayout_2;
    QWidget *widget_date_;
    QHBoxLayout *horizontalLayout;
    QCheckBox *checkBox_today;
    QCheckBox *checkBox_this_month;
    QCheckBox *checkBox_all;
    QCheckBox *checkBox_select_time;
    QLabel *label_fromdate;
    QDateEdit *dateEdit_begin;
    QLabel *label;
    QDateEdit *dateEdit_end;
    QToolButton *toolButton_OK;
    QToolButton *toolButton_stats;
    QTableWidget *tableWidget_SQL_Inquire;
    QProgressBar *progressBarLoad;
    QGroupBox *groupBox_Findexatc;
    QGridLayout *gridLayout;
    QComboBox *comboBox_sendingdoctor;
    QLabel *label_bednum;
    QLineEdit *lineEdit_bednum;
    QLabel *label_sendingdoctor;
    QLabel *label_age;
    QSpinBox *spinBox_label_Sample;
    QComboBox *comboBox_Section;
    QComboBox *comboBox_age;
    QLabel *label_Sample;
    QLineEdit *lineEdit_Name;
    QLabel *label_Name;
    QLabel *label_Section;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QToolButton *toolButton_creatPdf;
    QToolButton *toolButton_outPrint;
    QToolButton *toolButtonFindexatc;
    QWidget *widget_curve;
    QHBoxLayout *horizontalLayout_3;
    QCustomPlot *Inquire_curve_1;
    QCustomPlot *widget_sats_curve;

    void setupUi(QWidget *Inquire_Sql_Info)
    {
        if (Inquire_Sql_Info->objectName().isEmpty())
            Inquire_Sql_Info->setObjectName(QStringLiteral("Inquire_Sql_Info"));
        Inquire_Sql_Info->resize(1658, 1126);
        Inquire_Sql_Info->setStyleSheet(QLatin1String("QWidget#Inquire_Sql_Info{  \n"
"    border: 2px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;   	\n"
"} "));
        gridLayout_2 = new QGridLayout(Inquire_Sql_Info);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        widget_date_ = new QWidget(Inquire_Sql_Info);
        widget_date_->setObjectName(QStringLiteral("widget_date_"));
        widget_date_->setMaximumSize(QSize(16777215, 60));
        widget_date_->setStyleSheet(QString::fromUtf8("QWidget#widget_date_{  \n"
"    border: 1px solid rgb(220, 220, 220);  \n"
"	background-color:  rgb(255, 255, 255);  \n"
"    border-style: solid;  \n"
"    border-radius:2px;  \n"
"    padding:0 0px;\n"
"    font-family: \"\346\245\267\344\275\223\";\n"
"    font-size: 20px  	\n"
"} "));
        horizontalLayout = new QHBoxLayout(widget_date_);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 10);
        checkBox_today = new QCheckBox(widget_date_);
        checkBox_today->setObjectName(QStringLiteral("checkBox_today"));
        checkBox_today->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(16);
        checkBox_today->setFont(font);
        checkBox_today->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { \n"
"    width: 32px;\n"
"    height: 32px;\n"
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

        horizontalLayout->addWidget(checkBox_today);

        checkBox_this_month = new QCheckBox(widget_date_);
        checkBox_this_month->setObjectName(QStringLiteral("checkBox_this_month"));
        checkBox_this_month->setMinimumSize(QSize(0, 30));
        checkBox_this_month->setMaximumSize(QSize(200, 16777215));
        checkBox_this_month->setFont(font);
        checkBox_this_month->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { \n"
"    width: 32px;\n"
"    height: 32px;\n"
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

        horizontalLayout->addWidget(checkBox_this_month);

        checkBox_all = new QCheckBox(widget_date_);
        checkBox_all->setObjectName(QStringLiteral("checkBox_all"));
        checkBox_all->setMinimumSize(QSize(0, 30));
        checkBox_all->setMaximumSize(QSize(200, 16777215));
        checkBox_all->setFont(font);
        checkBox_all->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { \n"
"    width: 32px;\n"
"    height: 32px;\n"
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
"    image: url(:/Picture/check_box_hove.png);\n"
"}"));
        checkBox_all->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(checkBox_all);

        checkBox_select_time = new QCheckBox(widget_date_);
        checkBox_select_time->setObjectName(QStringLiteral("checkBox_select_time"));
        checkBox_select_time->setMinimumSize(QSize(0, 30));
        checkBox_select_time->setFont(font);
        checkBox_select_time->setStyleSheet(QString::fromUtf8("QCheckBox::indicator { \n"
"    width: 32px;\n"
"    height: 32px;\n"
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

        horizontalLayout->addWidget(checkBox_select_time);

        label_fromdate = new QLabel(widget_date_);
        label_fromdate->setObjectName(QStringLiteral("label_fromdate"));
        label_fromdate->setMaximumSize(QSize(100, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(14);
        label_fromdate->setFont(font1);

        horizontalLayout->addWidget(label_fromdate);

        dateEdit_begin = new QDateEdit(widget_date_);
        dateEdit_begin->setObjectName(QStringLiteral("dateEdit_begin"));
        dateEdit_begin->setMinimumSize(QSize(100, 35));
        dateEdit_begin->setSizeIncrement(QSize(0, 0));
        dateEdit_begin->setFont(font);
        dateEdit_begin->setStyleSheet(QStringLiteral(""));
        dateEdit_begin->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(dateEdit_begin);

        label = new QLabel(widget_date_);
        label->setObjectName(QStringLiteral("label"));
        label->setMaximumSize(QSize(100, 16777215));
        label->setFont(font1);
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(label);

        dateEdit_end = new QDateEdit(widget_date_);
        dateEdit_end->setObjectName(QStringLiteral("dateEdit_end"));
        dateEdit_end->setMinimumSize(QSize(100, 35));
        dateEdit_end->setFont(font);
        dateEdit_end->setStyleSheet(QStringLiteral(""));
        dateEdit_end->setAlignment(Qt::AlignCenter);

        horizontalLayout->addWidget(dateEdit_end);

        toolButton_OK = new QToolButton(widget_date_);
        toolButton_OK->setObjectName(QStringLiteral("toolButton_OK"));
        toolButton_OK->setMinimumSize(QSize(200, 35));
        toolButton_OK->setMaximumSize(QSize(120, 30));
        toolButton_OK->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_OK{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_OK:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_OK:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        toolButton_OK->setIconSize(QSize(32, 32));
        toolButton_OK->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_OK);

        toolButton_stats = new QToolButton(widget_date_);
        toolButton_stats->setObjectName(QStringLiteral("toolButton_stats"));
        toolButton_stats->setMinimumSize(QSize(80, 0));
        toolButton_stats->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_stats{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_stats:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_stats:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout->addWidget(toolButton_stats);


        gridLayout_2->addWidget(widget_date_, 0, 0, 1, 2);

        tableWidget_SQL_Inquire = new QTableWidget(Inquire_Sql_Info);
        tableWidget_SQL_Inquire->setObjectName(QStringLiteral("tableWidget_SQL_Inquire"));
        tableWidget_SQL_Inquire->setMinimumSize(QSize(1200, 350));
        tableWidget_SQL_Inquire->setMaximumSize(QSize(16777215, 199999));
        tableWidget_SQL_Inquire->setStyleSheet(QStringLiteral(""));

        gridLayout_2->addWidget(tableWidget_SQL_Inquire, 1, 0, 1, 2);

        progressBarLoad = new QProgressBar(Inquire_Sql_Info);
        progressBarLoad->setObjectName(QStringLiteral("progressBarLoad"));
        progressBarLoad->setMinimumSize(QSize(0, 20));
        progressBarLoad->setMaximumSize(QSize(16777215, 20));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font2.setPointSize(12);
        progressBarLoad->setFont(font2);
        progressBarLoad->setValue(0);

        gridLayout_2->addWidget(progressBarLoad, 2, 0, 1, 2);

        groupBox_Findexatc = new QGroupBox(Inquire_Sql_Info);
        groupBox_Findexatc->setObjectName(QStringLiteral("groupBox_Findexatc"));
        groupBox_Findexatc->setMinimumSize(QSize(0, 350));
        groupBox_Findexatc->setMaximumSize(QSize(350, 350));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font3.setBold(true);
        font3.setItalic(false);
        font3.setWeight(75);
        groupBox_Findexatc->setFont(font3);
        groupBox_Findexatc->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 1px solid gray;\n"
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
        groupBox_Findexatc->setAlignment(Qt::AlignCenter);
        gridLayout = new QGridLayout(groupBox_Findexatc);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        comboBox_sendingdoctor = new QComboBox(groupBox_Findexatc);
        comboBox_sendingdoctor->setObjectName(QStringLiteral("comboBox_sendingdoctor"));
        comboBox_sendingdoctor->setMinimumSize(QSize(0, 30));
        comboBox_sendingdoctor->setMaximumSize(QSize(16777215, 30));
        QFont font4;
        comboBox_sendingdoctor->setFont(font4);
        comboBox_sendingdoctor->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(comboBox_sendingdoctor, 1, 1, 1, 1);

        label_bednum = new QLabel(groupBox_Findexatc);
        label_bednum->setObjectName(QStringLiteral("label_bednum"));
        label_bednum->setMinimumSize(QSize(0, 20));
        label_bednum->setMaximumSize(QSize(120, 30));
        label_bednum->setFont(font2);

        gridLayout->addWidget(label_bednum, 6, 0, 1, 1);

        lineEdit_bednum = new QLineEdit(groupBox_Findexatc);
        lineEdit_bednum->setObjectName(QStringLiteral("lineEdit_bednum"));
        lineEdit_bednum->setMinimumSize(QSize(0, 30));
        lineEdit_bednum->setMaximumSize(QSize(16777215, 30));
        QFont font5;
        font5.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font5.setPointSize(15);
        font5.setBold(false);
        font5.setItalic(false);
        font5.setWeight(50);
        lineEdit_bednum->setFont(font5);
        lineEdit_bednum->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(lineEdit_bednum, 6, 1, 1, 1);

        label_sendingdoctor = new QLabel(groupBox_Findexatc);
        label_sendingdoctor->setObjectName(QStringLiteral("label_sendingdoctor"));
        label_sendingdoctor->setMinimumSize(QSize(0, 20));
        label_sendingdoctor->setMaximumSize(QSize(120, 30));
        label_sendingdoctor->setFont(font2);

        gridLayout->addWidget(label_sendingdoctor, 1, 0, 1, 1);

        label_age = new QLabel(groupBox_Findexatc);
        label_age->setObjectName(QStringLiteral("label_age"));
        label_age->setMinimumSize(QSize(0, 20));
        label_age->setMaximumSize(QSize(120, 30));
        label_age->setFont(font2);

        gridLayout->addWidget(label_age, 3, 0, 1, 1);

        spinBox_label_Sample = new QSpinBox(groupBox_Findexatc);
        spinBox_label_Sample->setObjectName(QStringLiteral("spinBox_label_Sample"));
        spinBox_label_Sample->setMinimumSize(QSize(0, 30));
        spinBox_label_Sample->setMaximumSize(QSize(16777215, 30));
        QFont font6;
        font6.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        spinBox_label_Sample->setFont(font6);
        spinBox_label_Sample->setStyleSheet(QStringLiteral(""));
        spinBox_label_Sample->setAlignment(Qt::AlignCenter);
        spinBox_label_Sample->setMinimum(1);
        spinBox_label_Sample->setMaximum(999999);

        gridLayout->addWidget(spinBox_label_Sample, 0, 1, 1, 1);

        comboBox_Section = new QComboBox(groupBox_Findexatc);
        comboBox_Section->setObjectName(QStringLiteral("comboBox_Section"));
        comboBox_Section->setMinimumSize(QSize(0, 30));
        comboBox_Section->setMaximumSize(QSize(16777215, 30));
        comboBox_Section->setFont(font4);
        comboBox_Section->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(comboBox_Section, 4, 1, 1, 1);

        comboBox_age = new QComboBox(groupBox_Findexatc);
        comboBox_age->setObjectName(QStringLiteral("comboBox_age"));
        comboBox_age->setMinimumSize(QSize(0, 30));
        comboBox_age->setMaximumSize(QSize(16777215, 30));
        comboBox_age->setFont(font4);
        comboBox_age->setStyleSheet(QStringLiteral(""));
        comboBox_age->setIconSize(QSize(16, 16));

        gridLayout->addWidget(comboBox_age, 3, 1, 1, 1);

        label_Sample = new QLabel(groupBox_Findexatc);
        label_Sample->setObjectName(QStringLiteral("label_Sample"));
        label_Sample->setMinimumSize(QSize(0, 20));
        label_Sample->setMaximumSize(QSize(120, 30));
        label_Sample->setFont(font2);

        gridLayout->addWidget(label_Sample, 0, 0, 1, 1);

        lineEdit_Name = new QLineEdit(groupBox_Findexatc);
        lineEdit_Name->setObjectName(QStringLiteral("lineEdit_Name"));
        lineEdit_Name->setMinimumSize(QSize(0, 30));
        lineEdit_Name->setMaximumSize(QSize(16777215, 30));
        lineEdit_Name->setFont(font5);
        lineEdit_Name->setStyleSheet(QStringLiteral(""));
        lineEdit_Name->setFrame(true);

        gridLayout->addWidget(lineEdit_Name, 2, 1, 1, 1);

        label_Name = new QLabel(groupBox_Findexatc);
        label_Name->setObjectName(QStringLiteral("label_Name"));
        label_Name->setMinimumSize(QSize(0, 20));
        label_Name->setMaximumSize(QSize(120, 30));
        label_Name->setFont(font2);

        gridLayout->addWidget(label_Name, 2, 0, 1, 1);

        label_Section = new QLabel(groupBox_Findexatc);
        label_Section->setObjectName(QStringLiteral("label_Section"));
        label_Section->setMinimumSize(QSize(0, 20));
        label_Section->setMaximumSize(QSize(120, 30));
        label_Section->setFont(font2);

        gridLayout->addWidget(label_Section, 4, 0, 1, 1);

        widget = new QWidget(groupBox_Findexatc);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMaximumSize(QSize(16777215, 160));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        toolButton_creatPdf = new QToolButton(widget);
        toolButton_creatPdf->setObjectName(QStringLiteral("toolButton_creatPdf"));
        toolButton_creatPdf->setMinimumSize(QSize(80, 30));
        toolButton_creatPdf->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_creatPdf{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_creatPdf:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_creatPdf:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        gridLayout_3->addWidget(toolButton_creatPdf, 2, 0, 1, 1);

        toolButton_outPrint = new QToolButton(widget);
        toolButton_outPrint->setObjectName(QStringLiteral("toolButton_outPrint"));
        toolButton_outPrint->setMinimumSize(QSize(80, 30));
        toolButton_outPrint->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_outPrint{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_outPrint:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_outPrint:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        gridLayout_3->addWidget(toolButton_outPrint, 0, 1, 1, 1);

        toolButtonFindexatc = new QToolButton(widget);
        toolButtonFindexatc->setObjectName(QStringLiteral("toolButtonFindexatc"));
        toolButtonFindexatc->setMinimumSize(QSize(80, 30));
        toolButtonFindexatc->setStyleSheet(QString::fromUtf8("QToolButton#toolButtonFindexatc{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButtonFindexatc:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButtonFindexatc:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        toolButtonFindexatc->setIconSize(QSize(32, 32));
        toolButtonFindexatc->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_3->addWidget(toolButtonFindexatc, 0, 0, 1, 1);


        gridLayout->addWidget(widget, 8, 0, 1, 2);


        gridLayout_2->addWidget(groupBox_Findexatc, 3, 0, 1, 1);

        widget_curve = new QWidget(Inquire_Sql_Info);
        widget_curve->setObjectName(QStringLiteral("widget_curve"));
        widget_curve->setMinimumSize(QSize(800, 350));
        widget_curve->setMaximumSize(QSize(16777215, 350));
        widget_curve->setStyleSheet(QLatin1String("QWidget#widget_curve{  \n"
"    border: 1px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;   	\n"
"}"));
        horizontalLayout_3 = new QHBoxLayout(widget_curve);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        Inquire_curve_1 = new QCustomPlot(widget_curve);
        Inquire_curve_1->setObjectName(QStringLiteral("Inquire_curve_1"));
        Inquire_curve_1->setMinimumSize(QSize(0, 0));
        Inquire_curve_1->setStyleSheet(QLatin1String("border: 1px solid  rgb(224, 224, 223);  \n"
"background-color:rgba(188,187,183);\n"
"border-style: solid;  \n"
"border-radius:10px;  \n"
"padding:0 0px; "));

        horizontalLayout_3->addWidget(Inquire_curve_1);

        widget_sats_curve = new QCustomPlot(widget_curve);
        widget_sats_curve->setObjectName(QStringLiteral("widget_sats_curve"));
        widget_sats_curve->setMinimumSize(QSize(450, 0));
        widget_sats_curve->setMaximumSize(QSize(450, 16777215));
        widget_sats_curve->setStyleSheet(QLatin1String("border: 1px solid  rgb(224, 224, 223);  \n"
"background-color:rgba(188,187,183);\n"
"border-style: solid;  \n"
"border-radius:10px;  \n"
"padding:0 0px; "));

        horizontalLayout_3->addWidget(widget_sats_curve);


        gridLayout_2->addWidget(widget_curve, 3, 1, 1, 1);


        retranslateUi(Inquire_Sql_Info);

        QMetaObject::connectSlotsByName(Inquire_Sql_Info);
    } // setupUi

    void retranslateUi(QWidget *Inquire_Sql_Info)
    {
        Inquire_Sql_Info->setWindowTitle(QApplication::translate("Inquire_Sql_Info", "Form", nullptr));
        checkBox_today->setText(QApplication::translate("Inquire_Sql_Info", "\344\273\212\345\244\251", nullptr));
        checkBox_this_month->setText(QApplication::translate("Inquire_Sql_Info", "\346\234\254\346\234\210", nullptr));
        checkBox_all->setText(QApplication::translate("Inquire_Sql_Info", "\345\205\250\351\203\250", nullptr));
        checkBox_select_time->setText(QApplication::translate("Inquire_Sql_Info", "\346\214\207\345\256\232", nullptr));
        label_fromdate->setText(QApplication::translate("Inquire_Sql_Info", "\350\265\267\345\247\213\346\227\245\346\234\237:", nullptr));
        label->setText(QApplication::translate("Inquire_Sql_Info", "\347\273\223\346\235\237\346\227\245\346\234\237:", nullptr));
        toolButton_OK->setText(QApplication::translate("Inquire_Sql_Info", "\346\237\245\346\211\276", nullptr));
        toolButton_stats->setText(QApplication::translate("Inquire_Sql_Info", "\347\273\237\350\256\241", nullptr));
        groupBox_Findexatc->setTitle(QApplication::translate("Inquire_Sql_Info", "\347\262\276\347\241\256\346\237\245\346\211\276", nullptr));
        label_bednum->setText(QApplication::translate("Inquire_Sql_Info", "\345\272\212\345\217\267:", nullptr));
        label_sendingdoctor->setText(QApplication::translate("Inquire_Sql_Info", "\351\200\201\346\243\200\345\214\273\347\224\237:", nullptr));
        label_age->setText(QApplication::translate("Inquire_Sql_Info", "\346\202\243\350\200\205\345\271\264\351\276\204:", nullptr));
        label_Sample->setText(QApplication::translate("Inquire_Sql_Info", "\346\240\267\346\234\254\345\217\267:", nullptr));
        label_Name->setText(QApplication::translate("Inquire_Sql_Info", "\346\202\243\350\200\205\345\247\223\345\220\215:", nullptr));
        label_Section->setText(QApplication::translate("Inquire_Sql_Info", "\347\247\221\345\210\253:", nullptr));
        toolButton_creatPdf->setText(QApplication::translate("Inquire_Sql_Info", "\350\276\223\345\207\272PDF", nullptr));
        toolButton_outPrint->setText(QApplication::translate("Inquire_Sql_Info", "\346\211\223\345\215\260", nullptr));
        toolButtonFindexatc->setText(QApplication::translate("Inquire_Sql_Info", "\347\262\276\347\241\256\346\237\245\346\211\276", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Inquire_Sql_Info: public Ui_Inquire_Sql_Info {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INQUIRE_SQL_INFO_H
