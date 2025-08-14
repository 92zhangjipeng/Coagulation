/********************************************************************************
** Form generated from reading UI file 'configureprojectitem.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGUREPROJECTITEM_H
#define UI_CONFIGUREPROJECTITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigureProjectItem
{
public:
    QVBoxLayout *verticalLayout_3;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout_title;
    QLabel *label_Image;
    QLabel *label_title;
    QPushButton *pushButton_Close;
    QWidget *widgetFucn;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_group;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton_AA;
    QRadioButton *radioButton_ADP;
    QRadioButton *radioButton_EPI;
    QRadioButton *radioButton_COL;
    QRadioButton *radioButton_RIS;
    QTableWidget *tableWidget_group;
    QWidget *widget_btn;
    QGridLayout *gridLayout_2;
    QToolButton *toolButtonCancel;
    QToolButton *toolButtonSaved;
    QToolButton *toolButton_addtable;
    QToolButton *toolButton_LossGroup;

    void setupUi(QWidget *ConfigureProjectItem)
    {
        if (ConfigureProjectItem->objectName().isEmpty())
            ConfigureProjectItem->setObjectName(QStringLiteral("ConfigureProjectItem"));
        ConfigureProjectItem->setWindowModality(Qt::ApplicationModal);
        ConfigureProjectItem->resize(600, 362);
        ConfigureProjectItem->setMinimumSize(QSize(310, 320));
        ConfigureProjectItem->setMaximumSize(QSize(600, 999999));
        verticalLayout_3 = new QVBoxLayout(ConfigureProjectItem);
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(1, 0, 1, 10);
        widget_title = new QWidget(ConfigureProjectItem);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMinimumSize(QSize(0, 35));
        widget_title->setMaximumSize(QSize(16777215, 35));
        widget_title->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border: 1px solid rgba(220,220,220);"));
        horizontalLayout_title = new QHBoxLayout(widget_title);
        horizontalLayout_title->setSpacing(0);
        horizontalLayout_title->setObjectName(QStringLiteral("horizontalLayout_title"));
        horizontalLayout_title->setContentsMargins(1, 0, 0, 0);
        label_Image = new QLabel(widget_title);
        label_Image->setObjectName(QStringLiteral("label_Image"));
        label_Image->setMaximumSize(QSize(32, 32));
        label_Image->setStyleSheet(QLatin1String("border-style: flat;\n"
""));
        label_Image->setPixmap(QPixmap(QString::fromUtf8(":/Picture/suowei.png")));
        label_Image->setScaledContents(false);

        horizontalLayout_title->addWidget(label_Image);

        label_title = new QLabel(widget_title);
        label_title->setObjectName(QStringLiteral("label_title"));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(14);
        label_title->setFont(font);
        label_title->setStyleSheet(QLatin1String("border-style: flat;\n"
"background: transparent;\n"
""));

        horizontalLayout_title->addWidget(label_title);

        pushButton_Close = new QPushButton(widget_title);
        pushButton_Close->setObjectName(QStringLiteral("pushButton_Close"));
        pushButton_Close->setMaximumSize(QSize(32, 32));
        pushButton_Close->setStyleSheet(QLatin1String("\n"
"	border-style: flat;\n"
"	background: transparent;\n"
""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButton_Close->setIcon(icon);
        pushButton_Close->setIconSize(QSize(32, 32));

        horizontalLayout_title->addWidget(pushButton_Close);


        verticalLayout_3->addWidget(widget_title);

        widgetFucn = new QWidget(ConfigureProjectItem);
        widgetFucn->setObjectName(QStringLiteral("widgetFucn"));
        horizontalLayout = new QHBoxLayout(widgetFucn);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(1, 0, 1, 0);
        groupBox_group = new QGroupBox(widgetFucn);
        groupBox_group->setObjectName(QStringLiteral("groupBox_group"));
        groupBox_group->setMinimumSize(QSize(120, 0));
        groupBox_group->setMaximumSize(QSize(130, 400));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setBold(true);
        font1.setItalic(false);
        font1.setWeight(75);
        groupBox_group->setFont(font1);
        groupBox_group->setStyleSheet(QString::fromUtf8("QGroupBox{\n"
"	border: 1px solid gray;\n"
"	border-radius:15px;\n"
"	margin-top:2ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font: bold 15px;\n"
"} \n"
"QGroupBox::title{\n"
"	subcontrol-origin: margin;\n"
"	subcontrol-position:top center;\n"
"	padding: 2px;\n"
"}\n"
"QGroupBox::enabled{\n"
"	border: 1px solid gray;\n"
"}\n"
"QGroupBox::!enabled{\n"
"	border: 1px solid gray;\n"
"}\n"
"\n"
""));
        groupBox_group->setCheckable(false);
        groupBox_group->setChecked(false);
        verticalLayout = new QVBoxLayout(groupBox_group);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(20, 5, 10, 5);
        radioButton_AA = new QRadioButton(groupBox_group);
        radioButton_AA->setObjectName(QStringLiteral("radioButton_AA"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        radioButton_AA->setFont(font2);
        radioButton_AA->setStyleSheet(QString::fromUtf8("/*\345\215\225\351\200\211\346\241\206\346\234\252\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::unchecked { \n"
"    image: url(:/Picture/icon_radio_button_uncheck.png);\n"
"}\n"
"/*\345\215\225\351\200\211\346\241\206\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::checked { \n"
"    image: url(:/Picture/icon_radio_button_checked.png);\n"
"}\n"
"/*RadioButton\345\222\214checkbox\345\255\227\344\275\223\345\222\214\351\227\264\350\267\235\350\256\276\347\275\256*/\n"
"QRadioButton ,QCheckBox{\n"
"    spacing: 5px;\n"
"    font-size: 16px;\n"
"	font-family:'\346\245\267\344\275\223';\n"
"}"));

        verticalLayout->addWidget(radioButton_AA);

        radioButton_ADP = new QRadioButton(groupBox_group);
        radioButton_ADP->setObjectName(QStringLiteral("radioButton_ADP"));
        radioButton_ADP->setFont(font2);
        radioButton_ADP->setStyleSheet(QString::fromUtf8("/*\345\215\225\351\200\211\346\241\206\346\234\252\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::unchecked {\n"
"     \n"
"    image: url(:/Picture/icon_radio_button_uncheck.png);\n"
"}\n"
"/*\345\215\225\351\200\211\346\241\206\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::checked { \n"
"    image: url(:/Picture/icon_radio_button_checked.png);\n"
"}\n"
"/*RadioButton\345\222\214checkbox\345\255\227\344\275\223\345\222\214\351\227\264\350\267\235\350\256\276\347\275\256*/\n"
"QRadioButton ,QCheckBox{\n"
"    spacing: 5px;\n"
"    font-size: 16px;\n"
"	font-family:'\346\245\267\344\275\223';\n"
"}"));

        verticalLayout->addWidget(radioButton_ADP);

        radioButton_EPI = new QRadioButton(groupBox_group);
        radioButton_EPI->setObjectName(QStringLiteral("radioButton_EPI"));
        radioButton_EPI->setFont(font2);
        radioButton_EPI->setStyleSheet(QString::fromUtf8("/*\345\215\225\351\200\211\346\241\206\346\234\252\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::unchecked {\n"
"     \n"
"    image: url(:/Picture/icon_radio_button_uncheck.png);\n"
"}\n"
"/*\345\215\225\351\200\211\346\241\206\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::checked { \n"
"    image: url(:/Picture/icon_radio_button_checked.png);\n"
"}\n"
"/*RadioButton\345\222\214checkbox\345\255\227\344\275\223\345\222\214\351\227\264\350\267\235\350\256\276\347\275\256*/\n"
"QRadioButton ,QCheckBox{\n"
"    spacing: 5px;\n"
"    font-size: 16px;\n"
"	font-family:'\346\245\267\344\275\223';\n"
"}"));

        verticalLayout->addWidget(radioButton_EPI);

        radioButton_COL = new QRadioButton(groupBox_group);
        radioButton_COL->setObjectName(QStringLiteral("radioButton_COL"));
        radioButton_COL->setFont(font2);
        radioButton_COL->setStyleSheet(QString::fromUtf8("/*\345\215\225\351\200\211\346\241\206\346\234\252\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::unchecked {\n"
"     \n"
"    image: url(:/Picture/icon_radio_button_uncheck.png);\n"
"}\n"
"/*\345\215\225\351\200\211\346\241\206\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::checked { \n"
"    image: url(:/Picture/icon_radio_button_checked.png);\n"
"}\n"
"/*RadioButton\345\222\214checkbox\345\255\227\344\275\223\345\222\214\351\227\264\350\267\235\350\256\276\347\275\256*/\n"
"QRadioButton ,QCheckBox{\n"
"    spacing: 5px;\n"
"    font-size: 16px;\n"
"	font-family:'\346\245\267\344\275\223';\n"
"}"));

        verticalLayout->addWidget(radioButton_COL);

        radioButton_RIS = new QRadioButton(groupBox_group);
        radioButton_RIS->setObjectName(QStringLiteral("radioButton_RIS"));
        radioButton_RIS->setFont(font2);
        radioButton_RIS->setStyleSheet(QString::fromUtf8("/*\345\215\225\351\200\211\346\241\206\346\234\252\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::unchecked {\n"
"     \n"
"    image: url(:/Picture/icon_radio_button_uncheck.png);\n"
"}\n"
"/*\345\215\225\351\200\211\346\241\206\351\200\211\344\270\255\346\240\267\345\274\217*/\n"
"QRadioButton::indicator::checked { \n"
"    image: url(:/Picture/icon_radio_button_checked.png);\n"
"}\n"
"/*RadioButton\345\222\214checkbox\345\255\227\344\275\223\345\222\214\351\227\264\350\267\235\350\256\276\347\275\256*/\n"
"QRadioButton ,QCheckBox{\n"
"    spacing: 5px;\n"
"    font-size: 16px;\n"
"	font-family:'\346\245\267\344\275\223';\n"
"}"));

        verticalLayout->addWidget(radioButton_RIS);


        horizontalLayout->addWidget(groupBox_group);

        tableWidget_group = new QTableWidget(widgetFucn);
        tableWidget_group->setObjectName(QStringLiteral("tableWidget_group"));
        tableWidget_group->setMinimumSize(QSize(300, 220));
        tableWidget_group->setMaximumSize(QSize(9999, 400));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font3.setPointSize(12);
        tableWidget_group->setFont(font3);
        tableWidget_group->setStyleSheet(QLatin1String("QTableWidget#tableWidget_group{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(230, 230, 230);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        horizontalLayout->addWidget(tableWidget_group);


        verticalLayout_3->addWidget(widgetFucn);

        widget_btn = new QWidget(ConfigureProjectItem);
        widget_btn->setObjectName(QStringLiteral("widget_btn"));
        widget_btn->setMinimumSize(QSize(0, 70));
        widget_btn->setMaximumSize(QSize(16777215, 90));
        gridLayout_2 = new QGridLayout(widget_btn);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(0, 0, 0, 0);
        toolButtonCancel = new QToolButton(widget_btn);
        toolButtonCancel->setObjectName(QStringLiteral("toolButtonCancel"));
        toolButtonCancel->setMinimumSize(QSize(130, 35));
        toolButtonCancel->setMaximumSize(QSize(130, 35));
        toolButtonCancel->setFont(font2);
        toolButtonCancel->setStyleSheet(QString::fromUtf8(" QToolButton#toolButtonCancel{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButtonCancel:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButtonCancel:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));
        toolButtonCancel->setIconSize(QSize(16, 16));
        toolButtonCancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_2->addWidget(toolButtonCancel, 0, 1, 1, 1);

        toolButtonSaved = new QToolButton(widget_btn);
        toolButtonSaved->setObjectName(QStringLiteral("toolButtonSaved"));
        toolButtonSaved->setMinimumSize(QSize(130, 35));
        toolButtonSaved->setMaximumSize(QSize(130, 35));
        toolButtonSaved->setFont(font2);
        toolButtonSaved->setStyleSheet(QString::fromUtf8("QToolButton#toolButtonSaved{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButtonSaved:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButtonSaved:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));
        toolButtonSaved->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_2->addWidget(toolButtonSaved, 0, 0, 1, 1);

        toolButton_addtable = new QToolButton(widget_btn);
        toolButton_addtable->setObjectName(QStringLiteral("toolButton_addtable"));
        toolButton_addtable->setMinimumSize(QSize(130, 35));
        toolButton_addtable->setMaximumSize(QSize(130, 35));
        toolButton_addtable->setFont(font2);
        toolButton_addtable->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_addtable{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_addtable:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_addtable:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));
        toolButton_addtable->setIconSize(QSize(32, 32));
        toolButton_addtable->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_2->addWidget(toolButton_addtable, 0, 2, 1, 1);

        toolButton_LossGroup = new QToolButton(widget_btn);
        toolButton_LossGroup->setObjectName(QStringLiteral("toolButton_LossGroup"));
        toolButton_LossGroup->setMinimumSize(QSize(130, 35));
        toolButton_LossGroup->setMaximumSize(QSize(130, 35));
        toolButton_LossGroup->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_LossGroup{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_LossGroup:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_LossGroup:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));

        gridLayout_2->addWidget(toolButton_LossGroup, 0, 3, 1, 1);


        verticalLayout_3->addWidget(widget_btn);


        retranslateUi(ConfigureProjectItem);

        QMetaObject::connectSlotsByName(ConfigureProjectItem);
    } // setupUi

    void retranslateUi(QWidget *ConfigureProjectItem)
    {
        ConfigureProjectItem->setWindowTitle(QApplication::translate("ConfigureProjectItem", "\351\205\215\347\275\256\351\241\271\347\233\256", nullptr));
        label_Image->setText(QString());
        label_title->setText(QString());
        pushButton_Close->setText(QString());
        groupBox_group->setTitle(QString());
        radioButton_AA->setText(QString());
        radioButton_ADP->setText(QString());
        radioButton_EPI->setText(QString());
        radioButton_COL->setText(QString());
        radioButton_RIS->setText(QString());
        toolButtonCancel->setText(QApplication::translate("ConfigureProjectItem", "\351\200\200\345\207\272", nullptr));
        toolButtonSaved->setText(QApplication::translate("ConfigureProjectItem", "\347\241\256\345\256\232", nullptr));
        toolButton_addtable->setText(QApplication::translate("ConfigureProjectItem", "\346\267\273\345\212\240\347\273\204\345\220\210", nullptr));
        toolButton_LossGroup->setText(QApplication::translate("ConfigureProjectItem", "\345\210\240\351\231\244\347\273\204\345\220\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigureProjectItem: public Ui_ConfigureProjectItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGUREPROJECTITEM_H
