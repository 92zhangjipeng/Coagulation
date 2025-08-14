/********************************************************************************
** Form generated from reading UI file 'handoff_staff.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HANDOFF_STAFF_H
#define UI_HANDOFF_STAFF_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_handoff_staff
{
public:
    QGridLayout *gridLayout_2;
    QSpacerItem *horizontalSpacer;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QComboBox *comboBox_handofstaf;
    QLineEdit *lineEdit_passwordhandof;
    QLabel *label_user;
    QToolButton *toolButton_handofstaff;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *handoff_staff)
    {
        if (handoff_staff->objectName().isEmpty())
            handoff_staff->setObjectName(QStringLiteral("handoff_staff"));
        handoff_staff->resize(322, 247);
        handoff_staff->setStyleSheet(QLatin1String("border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); \n"
"border-style: solid;  \n"
"border-radius:10px;  \n"
"padding:0 0px;  "));
        gridLayout_2 = new QGridLayout(handoff_staff);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer, 2, 2, 1, 1);

        groupBox = new QGroupBox(handoff_staff);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setStyleSheet(QString::fromUtf8("QGroupBox\n"
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
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        comboBox_handofstaf = new QComboBox(groupBox);
        comboBox_handofstaf->setObjectName(QStringLiteral("comboBox_handofstaf"));
        comboBox_handofstaf->setMinimumSize(QSize(0, 35));
        comboBox_handofstaf->setStyleSheet(QString::fromUtf8("QComboBox{\n"
"border-radius:5px;padding:2px 4px;\n"
"font: 75 11pt \"\346\245\267\344\275\223\";\n"
"color: rgb(255, 255, 255);\n"
"background-color:rgb(158, 158, 118);\n"
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
"  color:#ffffff;\n"
"}\n"
"QComboBox QAbstractIte"
                        "mView::item:selected{//\351\200\211\344\270\255\n"
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

        gridLayout->addWidget(comboBox_handofstaf, 0, 0, 1, 1);

        lineEdit_passwordhandof = new QLineEdit(groupBox);
        lineEdit_passwordhandof->setObjectName(QStringLiteral("lineEdit_passwordhandof"));
        lineEdit_passwordhandof->setMinimumSize(QSize(0, 35));
        lineEdit_passwordhandof->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 75 11pt \"\346\245\267\344\275\223\";\n"
"color: rgb(255, 255, 255);\n"
"background-color:rgb(158, 158, 118);"));

        gridLayout->addWidget(lineEdit_passwordhandof, 1, 0, 1, 1);


        gridLayout_2->addWidget(groupBox, 1, 1, 1, 1);

        label_user = new QLabel(handoff_staff);
        label_user->setObjectName(QStringLiteral("label_user"));
        label_user->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(14);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        label_user->setFont(font);
        label_user->setStyleSheet(QLatin1String("   border: 0px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  "));
        label_user->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label_user, 0, 1, 1, 1);

        toolButton_handofstaff = new QToolButton(handoff_staff);
        toolButton_handofstaff->setObjectName(QStringLiteral("toolButton_handofstaff"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(toolButton_handofstaff->sizePolicy().hasHeightForWidth());
        toolButton_handofstaff->setSizePolicy(sizePolicy);
        toolButton_handofstaff->setMinimumSize(QSize(300, 35));
        toolButton_handofstaff->setStyleSheet(QString::fromUtf8("QToolButton{  \n"
" border: 1px solid  rgb(200, 200, 200);\n"
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
"QToolButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255)); \n"
"}  \n"
"QToolButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                            s"
                        "top: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QToolButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  "));

        gridLayout_2->addWidget(toolButton_handofstaff, 2, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_2, 2, 0, 1, 1);


        retranslateUi(handoff_staff);

        QMetaObject::connectSlotsByName(handoff_staff);
    } // setupUi

    void retranslateUi(QWidget *handoff_staff)
    {
        handoff_staff->setWindowTitle(QApplication::translate("handoff_staff", "Form", nullptr));
        groupBox->setTitle(QApplication::translate("handoff_staff", "\345\210\207\346\215\242\347\224\250\346\210\267", nullptr));
        label_user->setText(QApplication::translate("handoff_staff", "TextLabel", nullptr));
        toolButton_handofstaff->setText(QApplication::translate("handoff_staff", " \345\210\207  \346\215\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class handoff_staff: public Ui_handoff_staff {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HANDOFF_STAFF_H
