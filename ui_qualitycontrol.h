/********************************************************************************
** Form generated from reading UI file 'qualitycontrol.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUALITYCONTROL_H
#define UI_QUALITYCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <custom_style/custombutton.h>

QT_BEGIN_NAMESPACE

class Ui_QualityControl
{
public:
    QGridLayout *gridLayout_controlSuppile;
    QWidget *CleanReagent_area;
    customButton *s1CleanLinque;
    QWidget *Tray_testtube;
    QTableWidget *tableWidget_Reagent_status;
    QFrame *frame;
    QLabel *Testtube_tray_1;
    QWidget *TestCupe_1;
    QToolButton *Chane_tray_1;
    QFrame *frame_2;
    QLabel *Testtube_tray_2;
    QWidget *TestCupe_2;
    QToolButton *Chane_tray_2;
    QFrame *frame_3;
    QLabel *Testtube_tray_3;
    QWidget *TestCupe_3;
    QToolButton *Chane_tray_3;
    QFrame *frame_4;
    QLabel *Testtube_tray_4;
    QWidget *TestCupe_4;
    QToolButton *Chane_tray_4;
    QWidget *Group_Reagentarea;
    customButton *AAReagentLeft;
    customButton *AAReagentRight;
    customButton *ADPReagentLeft;
    customButton *ADPReagentRight;
    customButton *EPIReagentLeft;
    customButton *EPIReagentRight;
    customButton *COLReagentLeft;
    customButton *COLReagentRight;
    customButton *RISReagentLeft;
    customButton *RISReagentRight;

    void setupUi(QWidget *QualityControl)
    {
        if (QualityControl->objectName().isEmpty())
            QualityControl->setObjectName(QStringLiteral("QualityControl"));
        QualityControl->resize(1857, 1190);
        QualityControl->setMinimumSize(QSize(240, 0));
        QualityControl->setMaximumSize(QSize(999999, 16777215));
        QualityControl->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        gridLayout_controlSuppile = new QGridLayout(QualityControl);
        gridLayout_controlSuppile->setSpacing(5);
        gridLayout_controlSuppile->setObjectName(QStringLiteral("gridLayout_controlSuppile"));
        gridLayout_controlSuppile->setContentsMargins(5, 0, 5, 0);
        CleanReagent_area = new QWidget(QualityControl);
        CleanReagent_area->setObjectName(QStringLiteral("CleanReagent_area"));
        CleanReagent_area->setMinimumSize(QSize(240, 150));
        CleanReagent_area->setMaximumSize(QSize(240, 150));
        CleanReagent_area->setStyleSheet(QLatin1String("QWidget#CleanReagent_area\n"
"{  \n"
"    border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        s1CleanLinque = new customButton(CleanReagent_area);
        s1CleanLinque->setObjectName(QStringLiteral("s1CleanLinque"));
        s1CleanLinque->setGeometry(QRect(150, 20, 81, 101));
        s1CleanLinque->setMouseTracking(true);
        s1CleanLinque->setTabletTracking(true);
        s1CleanLinque->setFocusPolicy(Qt::ClickFocus);

        gridLayout_controlSuppile->addWidget(CleanReagent_area, 0, 0, 1, 1);

        Tray_testtube = new QWidget(QualityControl);
        Tray_testtube->setObjectName(QStringLiteral("Tray_testtube"));
        Tray_testtube->setStyleSheet(QLatin1String(" border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183);   \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	"));
        tableWidget_Reagent_status = new QTableWidget(Tray_testtube);
        tableWidget_Reagent_status->setObjectName(QStringLiteral("tableWidget_Reagent_status"));
        tableWidget_Reagent_status->setGeometry(QRect(30, 510, 1451, 301));
        tableWidget_Reagent_status->setMinimumSize(QSize(280, 300));
        tableWidget_Reagent_status->setMaximumSize(QSize(99999, 16777215));
        tableWidget_Reagent_status->setStyleSheet(QStringLiteral(""));
        frame = new QFrame(Tray_testtube);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setGeometry(QRect(30, 20, 271, 481));
        frame->setMinimumSize(QSize(0, 400));
        frame->setMaximumSize(QSize(9999, 99999));
        frame->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        Testtube_tray_1 = new QLabel(frame);
        Testtube_tray_1->setObjectName(QStringLiteral("Testtube_tray_1"));
        Testtube_tray_1->setGeometry(QRect(5, 5, 261, 20));
        Testtube_tray_1->setMaximumSize(QSize(16777215, 20));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(16);
        Testtube_tray_1->setFont(font);
        Testtube_tray_1->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        Testtube_tray_1->setAlignment(Qt::AlignCenter);
        TestCupe_1 = new QWidget(frame);
        TestCupe_1->setObjectName(QStringLiteral("TestCupe_1"));
        TestCupe_1->setGeometry(QRect(5, 30, 261, 406));
        TestCupe_1->setMinimumSize(QSize(200, 0));
        TestCupe_1->setMaximumSize(QSize(9999, 16777215));
        TestCupe_1->setStyleSheet(QLatin1String("QWidget#TestCupe_1\n"
"{  \n"
"	border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        Chane_tray_1 = new QToolButton(frame);
        Chane_tray_1->setObjectName(QStringLiteral("Chane_tray_1"));
        Chane_tray_1->setGeometry(QRect(58, 441, 155, 35));
        Chane_tray_1->setMinimumSize(QSize(145, 35));
        Chane_tray_1->setMaximumSize(QSize(16777215, 35));
        Chane_tray_1->setStyleSheet(QString::fromUtf8("QToolButton#Chane_tray_1\n"
"{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#Chane_tray_1:hover\n"
"{     \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#Chane_tray_1:pressed\n"
"{  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        Chane_tray_1->setIconSize(QSize(16, 16));
        Chane_tray_1->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        frame_2 = new QFrame(Tray_testtube);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setGeometry(QRect(370, 20, 271, 481));
        frame_2->setMinimumSize(QSize(0, 400));
        frame_2->setMaximumSize(QSize(9999, 9999));
        frame_2->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        Testtube_tray_2 = new QLabel(frame_2);
        Testtube_tray_2->setObjectName(QStringLiteral("Testtube_tray_2"));
        Testtube_tray_2->setGeometry(QRect(5, 5, 208, 20));
        Testtube_tray_2->setMaximumSize(QSize(16777215, 20));
        Testtube_tray_2->setFont(font);
        Testtube_tray_2->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        Testtube_tray_2->setAlignment(Qt::AlignCenter);
        TestCupe_2 = new QWidget(frame_2);
        TestCupe_2->setObjectName(QStringLiteral("TestCupe_2"));
        TestCupe_2->setGeometry(QRect(5, 30, 261, 406));
        TestCupe_2->setMinimumSize(QSize(200, 0));
        TestCupe_2->setMaximumSize(QSize(9999, 16777215));
        TestCupe_2->setSizeIncrement(QSize(300, 0));
        TestCupe_2->setStyleSheet(QLatin1String("QWidget#TestCupe_2{  \n"
"    border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183);   \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        Chane_tray_2 = new QToolButton(frame_2);
        Chane_tray_2->setObjectName(QStringLiteral("Chane_tray_2"));
        Chane_tray_2->setGeometry(QRect(58, 441, 155, 35));
        Chane_tray_2->setMinimumSize(QSize(145, 35));
        Chane_tray_2->setStyleSheet(QString::fromUtf8("QToolButton#Chane_tray_2\n"
"{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#Chane_tray_2:hover\n"
"{     \n"
"    \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#Chane_tray_2:pressed\n"
"{  \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        Chane_tray_2->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        frame_3 = new QFrame(Tray_testtube);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setGeometry(QRect(744, 20, 281, 431));
        frame_3->setMaximumSize(QSize(9999, 9999));
        frame_3->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        Testtube_tray_3 = new QLabel(frame_3);
        Testtube_tray_3->setObjectName(QStringLiteral("Testtube_tray_3"));
        Testtube_tray_3->setGeometry(QRect(5, 5, 271, 20));
        Testtube_tray_3->setMaximumSize(QSize(16777215, 20));
        Testtube_tray_3->setFont(font);
        Testtube_tray_3->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        Testtube_tray_3->setAlignment(Qt::AlignCenter);
        TestCupe_3 = new QWidget(frame_3);
        TestCupe_3->setObjectName(QStringLiteral("TestCupe_3"));
        TestCupe_3->setGeometry(QRect(5, 30, 271, 356));
        TestCupe_3->setMinimumSize(QSize(200, 0));
        TestCupe_3->setMaximumSize(QSize(9999, 16777215));
        TestCupe_3->setSizeIncrement(QSize(300, 0));
        TestCupe_3->setStyleSheet(QLatin1String("QWidget#TestCupe_3{  \n"
"    border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183);    \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        Chane_tray_3 = new QToolButton(frame_3);
        Chane_tray_3->setObjectName(QStringLiteral("Chane_tray_3"));
        Chane_tray_3->setGeometry(QRect(63, 391, 155, 35));
        Chane_tray_3->setMinimumSize(QSize(145, 35));
        Chane_tray_3->setStyleSheet(QString::fromUtf8("QToolButton#Chane_tray_3\n"
"{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#Chane_tray_3:hover\n"
"{     \n"
"   \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#Chane_tray_3:pressed\n"
"{  \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        Chane_tray_3->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        frame_4 = new QFrame(Tray_testtube);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setGeometry(QRect(1100, 40, 291, 411));
        frame_4->setMaximumSize(QSize(9999, 9999));
        frame_4->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        Testtube_tray_4 = new QLabel(frame_4);
        Testtube_tray_4->setObjectName(QStringLiteral("Testtube_tray_4"));
        Testtube_tray_4->setGeometry(QRect(5, 5, 281, 20));
        Testtube_tray_4->setMaximumSize(QSize(16777215, 20));
        Testtube_tray_4->setFont(font);
        Testtube_tray_4->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));
        Testtube_tray_4->setAlignment(Qt::AlignCenter);
        TestCupe_4 = new QWidget(frame_4);
        TestCupe_4->setObjectName(QStringLiteral("TestCupe_4"));
        TestCupe_4->setGeometry(QRect(5, 30, 281, 336));
        TestCupe_4->setMinimumSize(QSize(200, 0));
        TestCupe_4->setMaximumSize(QSize(9999, 16777215));
        TestCupe_4->setSizeIncrement(QSize(300, 0));
        TestCupe_4->setStyleSheet(QLatin1String("QWidget#TestCupe_4{  \n"
"    border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183);   \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        Chane_tray_4 = new QToolButton(frame_4);
        Chane_tray_4->setObjectName(QStringLiteral("Chane_tray_4"));
        Chane_tray_4->setGeometry(QRect(68, 371, 155, 35));
        Chane_tray_4->setMinimumSize(QSize(145, 35));
        Chane_tray_4->setStyleSheet(QString::fromUtf8("QToolButton#Chane_tray_4\n"
"{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#Chane_tray_4:hover\n"
"{     \n"
"    \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#Chane_tray_4:pressed\n"
"{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        Chane_tray_4->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_controlSuppile->addWidget(Tray_testtube, 0, 1, 2, 1);

        Group_Reagentarea = new QWidget(QualityControl);
        Group_Reagentarea->setObjectName(QStringLiteral("Group_Reagentarea"));
        Group_Reagentarea->setMinimumSize(QSize(240, 730));
        Group_Reagentarea->setMaximumSize(QSize(240, 16777215));
        Group_Reagentarea->setMouseTracking(true);
        Group_Reagentarea->setStyleSheet(QLatin1String("QWidget#Group_Reagentarea{  \n"
"    border: 2px solid rgba(220,220,220);\n"
"	background-color:rgba(188,187,183); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        AAReagentLeft = new customButton(Group_Reagentarea);
        AAReagentLeft->setObjectName(QStringLiteral("AAReagentLeft"));
        AAReagentLeft->setGeometry(QRect(5, 5, 113, 201));
        AAReagentRight = new customButton(Group_Reagentarea);
        AAReagentRight->setObjectName(QStringLiteral("AAReagentRight"));
        AAReagentRight->setGeometry(QRect(123, 5, 112, 201));
        ADPReagentLeft = new customButton(Group_Reagentarea);
        ADPReagentLeft->setObjectName(QStringLiteral("ADPReagentLeft"));
        ADPReagentLeft->setGeometry(QRect(5, 211, 113, 201));
        ADPReagentRight = new customButton(Group_Reagentarea);
        ADPReagentRight->setObjectName(QStringLiteral("ADPReagentRight"));
        ADPReagentRight->setGeometry(QRect(123, 211, 112, 201));
        EPIReagentLeft = new customButton(Group_Reagentarea);
        EPIReagentLeft->setObjectName(QStringLiteral("EPIReagentLeft"));
        EPIReagentLeft->setGeometry(QRect(5, 417, 113, 201));
        EPIReagentRight = new customButton(Group_Reagentarea);
        EPIReagentRight->setObjectName(QStringLiteral("EPIReagentRight"));
        EPIReagentRight->setGeometry(QRect(123, 417, 112, 201));
        COLReagentLeft = new customButton(Group_Reagentarea);
        COLReagentLeft->setObjectName(QStringLiteral("COLReagentLeft"));
        COLReagentLeft->setGeometry(QRect(5, 623, 113, 201));
        COLReagentRight = new customButton(Group_Reagentarea);
        COLReagentRight->setObjectName(QStringLiteral("COLReagentRight"));
        COLReagentRight->setGeometry(QRect(123, 623, 112, 201));
        RISReagentLeft = new customButton(Group_Reagentarea);
        RISReagentLeft->setObjectName(QStringLiteral("RISReagentLeft"));
        RISReagentLeft->setGeometry(QRect(5, 829, 113, 201));
        RISReagentRight = new customButton(Group_Reagentarea);
        RISReagentRight->setObjectName(QStringLiteral("RISReagentRight"));
        RISReagentRight->setGeometry(QRect(123, 829, 112, 201));

        gridLayout_controlSuppile->addWidget(Group_Reagentarea, 1, 0, 1, 1);


        retranslateUi(QualityControl);

        QMetaObject::connectSlotsByName(QualityControl);
    } // setupUi

    void retranslateUi(QWidget *QualityControl)
    {
        QualityControl->setWindowTitle(QApplication::translate("QualityControl", "Form", nullptr));
        Testtube_tray_1->setText(QApplication::translate("QualityControl", "\346\265\213\350\257\225\346\235\257\345\214\272(1)", nullptr));
        Chane_tray_1->setText(QApplication::translate("QualityControl", "\346\233\264\346\215\242\350\257\225\346\235\257I", nullptr));
        Testtube_tray_2->setText(QApplication::translate("QualityControl", "\346\265\213\350\257\225\346\235\257\345\214\272(2)", nullptr));
        Chane_tray_2->setText(QApplication::translate("QualityControl", "\346\233\264\346\215\242\350\257\225\346\235\257II", nullptr));
        Testtube_tray_3->setText(QApplication::translate("QualityControl", "\346\265\213\350\257\225\346\235\257\345\214\272(3)", nullptr));
        Chane_tray_3->setText(QApplication::translate("QualityControl", "\346\233\264\346\215\242\350\257\225\346\235\257III", nullptr));
        Testtube_tray_4->setText(QApplication::translate("QualityControl", "\346\265\213\350\257\225\346\235\257\345\214\272(4)", nullptr));
        Chane_tray_4->setText(QApplication::translate("QualityControl", "\346\233\264\346\215\242\350\257\225\346\235\257IV", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QualityControl: public Ui_QualityControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUALITYCONTROL_H
