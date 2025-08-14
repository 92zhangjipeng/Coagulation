/********************************************************************************
** Form generated from reading UI file 'loginmaininterface.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINMAININTERFACE_H
#define UI_LOGINMAININTERFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "custom_style/animationprocessbar.h"

QT_BEGIN_NAMESPACE

class Ui_loginmaininterface
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout_title;
    QLabel *label_icon;
    QLabel *labelname;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_min;
    QToolButton *toolButton_max;
    QToolButton *toolButton_close;
    QFrame *frameMain;
    QGridLayout *gridLayout;
    QLabel *label_sysname;
    QLabel *label_version_number;
    QLabel *label;
    QComboBox *comboBox_user;
    QLabel *label_2;
    QLineEdit *lineEdit_password;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QToolButton *toolButton_enter;
    QToolButton *toolButton_exit;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_equipmentkind;
    QLabel *label_reminder;
	AnimationProcessBar *progressBar_readAxis;

    void setupUi(QDialog *loginmaininterface)
    {
        if (loginmaininterface->objectName().isEmpty())
            loginmaininterface->setObjectName(QStringLiteral("loginmaininterface"));
        loginmaininterface->resize(354, 336);
        verticalLayout = new QVBoxLayout(loginmaininterface);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_title = new QWidget(loginmaininterface);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMaximumSize(QSize(16777215, 36));
        widget_title->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);"));
        horizontalLayout_title = new QHBoxLayout(widget_title);
        horizontalLayout_title->setSpacing(5);
        horizontalLayout_title->setObjectName(QStringLiteral("horizontalLayout_title"));
        horizontalLayout_title->setContentsMargins(0, 0, 0, 0);
        label_icon = new QLabel(widget_title);
        label_icon->setObjectName(QStringLiteral("label_icon"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_icon->sizePolicy().hasHeightForWidth());
        label_icon->setSizePolicy(sizePolicy);
        label_icon->setMinimumSize(QSize(32, 32));
        label_icon->setMaximumSize(QSize(1199999, 199999));

        horizontalLayout_title->addWidget(label_icon);

        labelname = new QLabel(widget_title);
        labelname->setObjectName(QStringLiteral("labelname"));
        labelname->setMinimumSize(QSize(0, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(14);
        labelname->setFont(font);

        horizontalLayout_title->addWidget(labelname);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_title->addItem(horizontalSpacer);

        toolButton_min = new QToolButton(widget_title);
        toolButton_min->setObjectName(QStringLiteral("toolButton_min"));
        toolButton_min->setMinimumSize(QSize(32, 32));
        toolButton_min->setMaximumSize(QSize(32, 32));
        toolButton_min->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}\n"
""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\346\234\200\345\260\217\345\214\226.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_min->setIcon(icon);
        toolButton_min->setIconSize(QSize(32, 32));

        horizontalLayout_title->addWidget(toolButton_min);

        toolButton_max = new QToolButton(widget_title);
        toolButton_max->setObjectName(QStringLiteral("toolButton_max"));
        toolButton_max->setMinimumSize(QSize(32, 32));
        toolButton_max->setMaximumSize(QSize(32, 32));
        toolButton_max->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}\n"
""));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Picture/icon_title/\346\234\200\345\244\247\345\214\226.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_max->setIcon(icon1);
        toolButton_max->setIconSize(QSize(32, 32));

        horizontalLayout_title->addWidget(toolButton_max);

        toolButton_close = new QToolButton(widget_title);
        toolButton_close->setObjectName(QStringLiteral("toolButton_close"));
        toolButton_close->setMinimumSize(QSize(32, 32));
        toolButton_close->setMaximumSize(QSize(32, 32));
        toolButton_close->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}\n"
""));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_close->setIcon(icon2);
        toolButton_close->setIconSize(QSize(32, 32));

        horizontalLayout_title->addWidget(toolButton_close);


        verticalLayout->addWidget(widget_title);

        frameMain = new QFrame(loginmaininterface);
        frameMain->setObjectName(QStringLiteral("frameMain"));
        frameMain->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border-top-color: rgb(0, 0, 0);"));
        frameMain->setFrameShape(QFrame::StyledPanel);
        frameMain->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frameMain);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_sysname = new QLabel(frameMain);
        label_sysname->setObjectName(QStringLiteral("label_sysname"));
        label_sysname->setMinimumSize(QSize(0, 35));
        label_sysname->setMaximumSize(QSize(16777215, 35));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(18);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(9);
        label_sysname->setFont(font1);
        label_sysname->setStyleSheet(QString::fromUtf8("font: 75 18pt '\346\245\267\344\275\223';\n"
"background-color: rgba(188,187,183);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);"));
        label_sysname->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_sysname, 0, 0, 1, 2);

        label_version_number = new QLabel(frameMain);
        label_version_number->setObjectName(QStringLiteral("label_version_number"));
        sizePolicy.setHeightForWidth(label_version_number->sizePolicy().hasHeightForWidth());
        label_version_number->setSizePolicy(sizePolicy);
        label_version_number->setMinimumSize(QSize(30, 30));
        label_version_number->setMaximumSize(QSize(166666, 20));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font2.setPointSize(15);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(2);
        label_version_number->setFont(font2);
        label_version_number->setStyleSheet(QString::fromUtf8("font: 20 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));
        label_version_number->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_version_number, 1, 0, 1, 2);

        label = new QLabel(frameMain);
        label->setObjectName(QStringLiteral("label"));
        label->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout->addWidget(label, 2, 0, 1, 1);

        comboBox_user = new QComboBox(frameMain);
        comboBox_user->setObjectName(QStringLiteral("comboBox_user"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(comboBox_user->sizePolicy().hasHeightForWidth());
        comboBox_user->setSizePolicy(sizePolicy1);
        comboBox_user->setMinimumSize(QSize(240, 35));
        comboBox_user->setMaximumSize(QSize(16777215, 35));
        comboBox_user->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"border-radius:5px;\n"
"padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); \n"
"}\n"
"QComboBox::drop-down \n"
"{\n"
"   subcontrol-origin: padding;\n"
"   subcontrol-position: top right;\n"
"   width: 15px;\n"
"   border:none;\n"
"}\n"
"QComboBox::down-arrow \n"
"{\n"
"  height:30px;\n"
"  width:30px;\n"
"  image: url(:/Picture/minus.png);\n"
"}\n"
"QComboBox QAbstractItemView{\n"
"	background:rgba(255,255,255,1);\n"
"    border:1px solid rgba(150,150,150,1);\n"
"    border-radius:0px 0px 5px 5px;\n"
"	font: 24px; /*\345\255\227\344\275\223*/\n"
"    font-family: \346\245\267\344\275\223;\n"
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
"  color:"
                        "#ffffff;\n"
"}\n"
"QComboBox QAbstractItemView::item:selected{//\351\200\211\344\270\255\n"
"  background-color:#409CE1;\n"
"  color:#ffffff;\n"
"}\n"
"\n"
"QComboBox:on { \n"
"      padding-top: 3px;\n"
"      padding-left: 8px;\n"
"}\n"
"QComboBox::down-arrow:on { \n"
"      top: 1px;\n"
"      left: 8px;\n"
"}"));
        comboBox_user->setEditable(true);
        comboBox_user->setIconSize(QSize(32, 32));

        gridLayout->addWidget(comboBox_user, 2, 1, 1, 1);

        label_2 = new QLabel(frameMain);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout->addWidget(label_2, 3, 0, 1, 1);

        lineEdit_password = new QLineEdit(frameMain);
        lineEdit_password->setObjectName(QStringLiteral("lineEdit_password"));
        lineEdit_password->setMinimumSize(QSize(240, 35));
        lineEdit_password->setMaximumSize(QSize(16777215, 35));
        lineEdit_password->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); "));

        gridLayout->addWidget(lineEdit_password, 3, 1, 1, 1);

        widget = new QWidget(frameMain);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setStyleSheet(QLatin1String("border-style: flat;\n"
"	background: transparent;"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        toolButton_enter = new QToolButton(widget);
        toolButton_enter->setObjectName(QStringLiteral("toolButton_enter"));
        sizePolicy1.setHeightForWidth(toolButton_enter->sizePolicy().hasHeightForWidth());
        toolButton_enter->setSizePolicy(sizePolicy1);
        toolButton_enter->setMinimumSize(QSize(100, 40));
        toolButton_enter->setMaximumSize(QSize(16777215, 40));
        toolButton_enter->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_enter{  \n"
"     border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:24px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_enter:hover{ \n"
"	border: 1px solid #C0C0C0;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 90px;  \n"
"    height:30px;  \n"
"    padding:0 20px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;    \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_enter:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgb"
                        "a(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));
        toolButton_enter->setIconSize(QSize(32, 32));
        toolButton_enter->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_enter);

        toolButton_exit = new QToolButton(widget);
        toolButton_exit->setObjectName(QStringLiteral("toolButton_exit"));
        sizePolicy1.setHeightForWidth(toolButton_exit->sizePolicy().hasHeightForWidth());
        toolButton_exit->setSizePolicy(sizePolicy1);
        toolButton_exit->setMinimumSize(QSize(0, 40));
        toolButton_exit->setMaximumSize(QSize(16777215, 40));
        toolButton_exit->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_exit{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:24px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_exit:hover{ \n"
"    border: 1px solid #C0C0C0;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 90px;  \n"
"    height:30px;  \n"
"    padding:0 20px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:18px;\n"
"	color:white;        \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_exit:pressed{  \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 r"
                        "gba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        toolButton_exit->setIconSize(QSize(32, 32));
        toolButton_exit->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_exit);


        gridLayout->addWidget(widget, 4, 0, 1, 2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_equipmentkind = new QLabel(frameMain);
        label_equipmentkind->setObjectName(QStringLiteral("label_equipmentkind"));
        label_equipmentkind->setMinimumSize(QSize(0, 30));
        label_equipmentkind->setFont(font2);
        label_equipmentkind->setStyleSheet(QString::fromUtf8("font: 20 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));
        label_equipmentkind->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_equipmentkind);

        label_reminder = new QLabel(frameMain);
        label_reminder->setObjectName(QStringLiteral("label_reminder"));
        label_reminder->setFont(font2);
        label_reminder->setStyleSheet(QString::fromUtf8("font: 20 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_2->addWidget(label_reminder);


        gridLayout->addLayout(horizontalLayout_2, 5, 0, 1, 2);

        progressBar_readAxis = new AnimationProcessBar(frameMain);
        progressBar_readAxis->setObjectName(QStringLiteral("progressBar_readAxis"));
        progressBar_readAxis->setMinimumSize(QSize(0, 30));
        progressBar_readAxis->setValue(0);
        progressBar_readAxis->setTextVisible(true);

        gridLayout->addWidget(progressBar_readAxis, 6, 0, 1, 2);


        verticalLayout->addWidget(frameMain);


        retranslateUi(loginmaininterface);

        QMetaObject::connectSlotsByName(loginmaininterface);
    } // setupUi

    void retranslateUi(QDialog *loginmaininterface)
    {
        loginmaininterface->setWindowTitle(QApplication::translate("loginmaininterface", "Dialog", nullptr));
        label_icon->setText(QString());
        labelname->setText(QApplication::translate("loginmaininterface", "\347\224\250\346\210\267\347\231\273\345\275\225", nullptr));
        toolButton_min->setText(QString());
        toolButton_max->setText(QString());
        toolButton_close->setText(QString());
        label_sysname->setText(QApplication::translate("loginmaininterface", "\345\205\250\350\207\252\345\212\250\350\241\200\345\260\217\346\235\277\350\201\232\351\233\206\344\273\252\345\210\206\346\236\220\347\263\273\347\273\237", nullptr));
        label_version_number->setText(QApplication::translate("loginmaininterface", "\347\211\210\346\234\254\344\277\241\346\201\257:", nullptr));
        label->setText(QApplication::translate("loginmaininterface", "\347\224\250\346\210\267\345\220\215:", nullptr));
        label_2->setText(QApplication::translate("loginmaininterface", "\345\257\206\347\240\201:", nullptr));
        toolButton_enter->setText(QApplication::translate("loginmaininterface", "  \347\231\273  \345\205\245", nullptr));
        toolButton_exit->setText(QApplication::translate("loginmaininterface", "  \351\200\200  \345\207\272", nullptr));
        label_equipmentkind->setText(QApplication::translate("loginmaininterface", "\344\273\252\345\231\250\347\261\273\345\236\213:", nullptr));
        label_reminder->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class loginmaininterface: public Ui_loginmaininterface {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINMAININTERFACE_H
