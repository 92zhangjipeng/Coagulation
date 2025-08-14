/********************************************************************************
** Form generated from reading UI file 'addusername.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDUSERNAME_H
#define UI_ADDUSERNAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddUserName
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_Adduser;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *Enter_Usar;
    QLineEdit *lineEdit_Enter_Usar;
    QLabel *label_setpassword;
    QLineEdit *lineEdit_Setpassword;
    QCheckBox *checkBox_addvip;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_Add;
    QPushButton *pushButton_Cancel;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget_changepassword;
    QVBoxLayout *verticalLayout_3;
    QGridLayout *gridLayout_2;
    QLabel *label_3;
    QLabel *label_4;
    QLineEdit *oldpassword;
    QLabel *label_2;
    QLineEdit *newpassword;
    QLineEdit *makesurepassword;
    QLabel *label;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *OK;
    QPushButton *Cancel;
    QSpacerItem *horizontalSpacer_4;

    void setupUi(QWidget *AddUserName)
    {
        if (AddUserName->objectName().isEmpty())
            AddUserName->setObjectName(QStringLiteral("AddUserName"));
        AddUserName->setWindowModality(Qt::ApplicationModal);
        AddUserName->resize(322, 380);
        AddUserName->setSizeIncrement(QSize(200, 130));
        AddUserName->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border-top-color: rgb(0, 0, 0);\n"
""));
        verticalLayout = new QVBoxLayout(AddUserName);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        widget_Adduser = new QWidget(AddUserName);
        widget_Adduser->setObjectName(QStringLiteral("widget_Adduser"));
        verticalLayout_2 = new QVBoxLayout(widget_Adduser);
        verticalLayout_2->setSpacing(10);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        Enter_Usar = new QLabel(widget_Adduser);
        Enter_Usar->setObjectName(QStringLiteral("Enter_Usar"));
        Enter_Usar->setMinimumSize(QSize(90, 30));
        Enter_Usar->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(15);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(2);
        Enter_Usar->setFont(font);
        Enter_Usar->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout->addWidget(Enter_Usar, 0, 0, 1, 1);

        lineEdit_Enter_Usar = new QLineEdit(widget_Adduser);
        lineEdit_Enter_Usar->setObjectName(QStringLiteral("lineEdit_Enter_Usar"));
        lineEdit_Enter_Usar->setMinimumSize(QSize(0, 30));
        lineEdit_Enter_Usar->setFont(font);
        lineEdit_Enter_Usar->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); "));

        gridLayout->addWidget(lineEdit_Enter_Usar, 0, 1, 1, 1);

        label_setpassword = new QLabel(widget_Adduser);
        label_setpassword->setObjectName(QStringLiteral("label_setpassword"));
        label_setpassword->setMinimumSize(QSize(90, 30));
        label_setpassword->setMaximumSize(QSize(16777215, 30));
        label_setpassword->setFont(font);
        label_setpassword->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout->addWidget(label_setpassword, 1, 0, 1, 1);

        lineEdit_Setpassword = new QLineEdit(widget_Adduser);
        lineEdit_Setpassword->setObjectName(QStringLiteral("lineEdit_Setpassword"));
        lineEdit_Setpassword->setMinimumSize(QSize(0, 30));
        lineEdit_Setpassword->setFont(font);
        lineEdit_Setpassword->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); "));
        lineEdit_Setpassword->setMaxLength(11);

        gridLayout->addWidget(lineEdit_Setpassword, 1, 1, 1, 1);

        checkBox_addvip = new QCheckBox(widget_Adduser);
        checkBox_addvip->setObjectName(QStringLiteral("checkBox_addvip"));
        checkBox_addvip->setMinimumSize(QSize(0, 30));
        checkBox_addvip->setFont(font);
        checkBox_addvip->setStyleSheet(QString::fromUtf8("QCheckBox{\n"
"border-style: flat;\n"
"background: transparent;\n"
"color: rgb(0, 0, 0);\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border-radius:5px;padding:2px 4px;\n"
"}\n"
"QCheckBox::indicator { \n"
"    width: 16px;\n"
"    height: 32px;	\n"
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

        gridLayout->addWidget(checkBox_addvip, 2, 0, 1, 2);


        verticalLayout_2->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_Add = new QPushButton(widget_Adduser);
        pushButton_Add->setObjectName(QStringLiteral("pushButton_Add"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton_Add->sizePolicy().hasHeightForWidth());
        pushButton_Add->setSizePolicy(sizePolicy);
        pushButton_Add->setMinimumSize(QSize(82, 30));
        pushButton_Add->setMaximumSize(QSize(82, 30));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        pushButton_Add->setFont(font1);
        pushButton_Add->setStyleSheet(QString::fromUtf8("QPushButton{  \n"
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
"QPushButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0,   y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                            "
                        "stop: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QPushButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout->addWidget(pushButton_Add);

        pushButton_Cancel = new QPushButton(widget_Adduser);
        pushButton_Cancel->setObjectName(QStringLiteral("pushButton_Cancel"));
        sizePolicy.setHeightForWidth(pushButton_Cancel->sizePolicy().hasHeightForWidth());
        pushButton_Cancel->setSizePolicy(sizePolicy);
        pushButton_Cancel->setMinimumSize(QSize(82, 30));
        pushButton_Cancel->setMaximumSize(QSize(80, 30));
        pushButton_Cancel->setFont(font1);
        pushButton_Cancel->setStyleSheet(QString::fromUtf8("QPushButton{  \n"
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
"QPushButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0,   y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                            "
                        "stop: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QPushButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  "));

        horizontalLayout->addWidget(pushButton_Cancel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout);


        verticalLayout->addWidget(widget_Adduser);

        widget_changepassword = new QWidget(AddUserName);
        widget_changepassword->setObjectName(QStringLiteral("widget_changepassword"));
        verticalLayout_3 = new QVBoxLayout(widget_changepassword);
        verticalLayout_3->setSpacing(10);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setVerticalSpacing(10);
        label_3 = new QLabel(widget_changepassword);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(0, 30));
        label_3->setFont(font);
        label_3->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_2->addWidget(label_3, 3, 0, 1, 1);

        label_4 = new QLabel(widget_changepassword);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMinimumSize(QSize(0, 30));
        label_4->setFont(font);
        label_4->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_2->addWidget(label_4, 4, 0, 1, 1);

        oldpassword = new QLineEdit(widget_changepassword);
        oldpassword->setObjectName(QStringLiteral("oldpassword"));
        oldpassword->setMinimumSize(QSize(120, 30));
        oldpassword->setFont(font);
        oldpassword->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); "));

        gridLayout_2->addWidget(oldpassword, 2, 1, 1, 1);

        label_2 = new QLabel(widget_changepassword);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(0, 30));
        label_2->setFont(font);
        label_2->setStyleSheet(QString::fromUtf8("font: 17 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_2->addWidget(label_2, 2, 0, 1, 1);

        newpassword = new QLineEdit(widget_changepassword);
        newpassword->setObjectName(QStringLiteral("newpassword"));
        newpassword->setMinimumSize(QSize(120, 30));
        newpassword->setFont(font);
        newpassword->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); "));

        gridLayout_2->addWidget(newpassword, 3, 1, 1, 1);

        makesurepassword = new QLineEdit(widget_changepassword);
        makesurepassword->setObjectName(QStringLiteral("makesurepassword"));
        makesurepassword->setMinimumSize(QSize(120, 30));
        makesurepassword->setFont(font);
        makesurepassword->setStyleSheet(QString::fromUtf8("border-radius:5px;padding:2px 4px;\n"
"font: 17 15pt '\346\245\267\344\275\223';\n"
"border: 2px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); "));

        gridLayout_2->addWidget(makesurepassword, 4, 1, 1, 1);

        label = new QLabel(widget_changepassword);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 30));
        label->setMaximumSize(QSize(16777215, 20));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font2.setPointSize(18);
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(2);
        label->setFont(font2);
        label->setStyleSheet(QString::fromUtf8("font: 17 18pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));
        label->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(label, 1, 0, 1, 2);


        verticalLayout_3->addLayout(gridLayout_2);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        OK = new QPushButton(widget_changepassword);
        OK->setObjectName(QStringLiteral("OK"));
        sizePolicy.setHeightForWidth(OK->sizePolicy().hasHeightForWidth());
        OK->setSizePolicy(sizePolicy);
        OK->setMinimumSize(QSize(82, 30));
        OK->setMaximumSize(QSize(82, 30));
        OK->setFont(font1);
        OK->setStyleSheet(QString::fromUtf8("QPushButton{  \n"
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
"QPushButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0,   y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));  \n"
"}  \n"
"QPushButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                          "
                        "  stop: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QPushButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout_2->addWidget(OK);

        Cancel = new QPushButton(widget_changepassword);
        Cancel->setObjectName(QStringLiteral("Cancel"));
        sizePolicy.setHeightForWidth(Cancel->sizePolicy().hasHeightForWidth());
        Cancel->setSizePolicy(sizePolicy);
        Cancel->setMinimumSize(QSize(82, 30));
        Cancel->setMaximumSize(QSize(80, 30));
        Cancel->setFont(font1);
        Cancel->setStyleSheet(QString::fromUtf8("QPushButton{  \n"
" border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white; \n"
"}  \n"
"QPushButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0,   y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                           "
                        " stop: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QPushButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout_2->addWidget(Cancel);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(widget_changepassword);


        retranslateUi(AddUserName);

        QMetaObject::connectSlotsByName(AddUserName);
    } // setupUi

    void retranslateUi(QWidget *AddUserName)
    {
        AddUserName->setWindowTitle(QApplication::translate("AddUserName", "Form", nullptr));
        Enter_Usar->setText(QApplication::translate("AddUserName", "TextLabel", nullptr));
        label_setpassword->setText(QApplication::translate("AddUserName", "\350\256\276\347\275\256\345\257\206\347\240\201:", nullptr));
        checkBox_addvip->setText(QApplication::translate("AddUserName", "\346\267\273\345\212\240\344\270\272\347\256\241\347\220\206\345\221\230\347\224\250\346\210\267", nullptr));
        pushButton_Add->setText(QApplication::translate("AddUserName", "PushButton", nullptr));
        pushButton_Cancel->setText(QApplication::translate("AddUserName", "PushButton", nullptr));
        label_3->setText(QApplication::translate("AddUserName", "TextLabel", nullptr));
        label_4->setText(QApplication::translate("AddUserName", "TextLabel", nullptr));
        label_2->setText(QApplication::translate("AddUserName", "TextLabel", nullptr));
        label->setText(QApplication::translate("AddUserName", "TextLabel", nullptr));
        OK->setText(QApplication::translate("AddUserName", "PushButton", nullptr));
        Cancel->setText(QApplication::translate("AddUserName", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddUserName: public Ui_AddUserName {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDUSERNAME_H
