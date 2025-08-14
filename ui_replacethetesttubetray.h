/********************************************************************************
** Form generated from reading UI file 'replacethetesttubetray.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPLACETHETESTTUBETRAY_H
#define UI_REPLACETHETESTTUBETRAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_ReplaceTheTestTubeTray
{
public:
    QGridLayout *gridLayout;
    QGroupBox *groupBox_testChannel;
    QGroupBox *groupBox_testing;
    QVBoxLayout *verticalLayout_2;
    QTableWidget *tableWidget_test;
    QWidget *widgetbtn;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *toolButton;
    QSpacerItem *horizontalSpacer_4;
    QGroupBox *groupBox_inqure;
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget_result;
    QWidget *widgetfucn;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_day;
    QPushButton *pushButton_month;
    QPushButton *pushButton_year;
    QPushButton *pushButton_all;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widgetLable;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelHeighValColor;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_5;
    QLabel *labelMidValColor;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer_6;
    QLabel *labelLowValColor;
    QLabel *label_6;
    QDateEdit *dateEdit_resultCuve;
    QCustomPlot *widget_PECurve;

    void setupUi(QWidget *ReplaceTheTestTubeTray)
    {
        if (ReplaceTheTestTubeTray->objectName().isEmpty())
            ReplaceTheTestTubeTray->setObjectName(QStringLiteral("ReplaceTheTestTubeTray"));
        ReplaceTheTestTubeTray->resize(1186, 807);
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(16);
        ReplaceTheTestTubeTray->setFont(font);
        gridLayout = new QGridLayout(ReplaceTheTestTubeTray);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox_testChannel = new QGroupBox(ReplaceTheTestTubeTray);
        groupBox_testChannel->setObjectName(QStringLiteral("groupBox_testChannel"));
        groupBox_testChannel->setMinimumSize(QSize(0, 100));
        groupBox_testChannel->setMaximumSize(QSize(16777215, 150));
        groupBox_testChannel->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:12px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font: 19px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding: 0 px;\n"
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

        gridLayout->addWidget(groupBox_testChannel, 0, 0, 1, 2);

        groupBox_testing = new QGroupBox(ReplaceTheTestTubeTray);
        groupBox_testing->setObjectName(QStringLiteral("groupBox_testing"));
        groupBox_testing->setMinimumSize(QSize(570, 0));
        groupBox_testing->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:12px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font: 19px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding: 0 px;\n"
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
        verticalLayout_2 = new QVBoxLayout(groupBox_testing);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        tableWidget_test = new QTableWidget(groupBox_testing);
        tableWidget_test->setObjectName(QStringLiteral("tableWidget_test"));

        verticalLayout_2->addWidget(tableWidget_test);

        widgetbtn = new QWidget(groupBox_testing);
        widgetbtn->setObjectName(QStringLiteral("widgetbtn"));
        widgetbtn->setMinimumSize(QSize(0, 40));
        widgetbtn->setMaximumSize(QSize(16777215, 80));
        horizontalLayout_2 = new QHBoxLayout(widgetbtn);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        toolButton = new QToolButton(widgetbtn);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setMinimumSize(QSize(0, 35));

        horizontalLayout_2->addWidget(toolButton);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_4);


        verticalLayout_2->addWidget(widgetbtn);


        gridLayout->addWidget(groupBox_testing, 1, 0, 1, 1);

        groupBox_inqure = new QGroupBox(ReplaceTheTestTubeTray);
        groupBox_inqure->setObjectName(QStringLiteral("groupBox_inqure"));
        groupBox_inqure->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 2px solid gray;\n"
"	border-radius:12px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font: 19px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding: 0 px;\n"
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
        verticalLayout = new QVBoxLayout(groupBox_inqure);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tableWidget_result = new QTableWidget(groupBox_inqure);
        tableWidget_result->setObjectName(QStringLiteral("tableWidget_result"));

        verticalLayout->addWidget(tableWidget_result);

        widgetfucn = new QWidget(groupBox_inqure);
        widgetfucn->setObjectName(QStringLiteral("widgetfucn"));
        widgetfucn->setMinimumSize(QSize(0, 40));
        widgetfucn->setMaximumSize(QSize(16777215, 80));
        horizontalLayout = new QHBoxLayout(widgetfucn);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        horizontalSpacer = new QSpacerItem(104, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_day = new QPushButton(widgetfucn);
        pushButton_day->setObjectName(QStringLiteral("pushButton_day"));
        pushButton_day->setMinimumSize(QSize(0, 35));

        horizontalLayout->addWidget(pushButton_day);

        pushButton_month = new QPushButton(widgetfucn);
        pushButton_month->setObjectName(QStringLiteral("pushButton_month"));
        pushButton_month->setMinimumSize(QSize(0, 35));

        horizontalLayout->addWidget(pushButton_month);

        pushButton_year = new QPushButton(widgetfucn);
        pushButton_year->setObjectName(QStringLiteral("pushButton_year"));
        pushButton_year->setMinimumSize(QSize(0, 35));

        horizontalLayout->addWidget(pushButton_year);

        pushButton_all = new QPushButton(widgetfucn);
        pushButton_all->setObjectName(QStringLiteral("pushButton_all"));
        pushButton_all->setMinimumSize(QSize(0, 35));

        horizontalLayout->addWidget(pushButton_all);

        horizontalSpacer_2 = new QSpacerItem(104, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widgetfucn);


        gridLayout->addWidget(groupBox_inqure, 1, 1, 1, 1);

        widgetLable = new QWidget(ReplaceTheTestTubeTray);
        widgetLable->setObjectName(QStringLiteral("widgetLable"));
        widgetLable->setMinimumSize(QSize(0, 20));
        widgetLable->setMaximumSize(QSize(16777215, 15));
        horizontalLayout_3 = new QHBoxLayout(widgetLable);
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 0, 5, 0);
        labelHeighValColor = new QLabel(widgetLable);
        labelHeighValColor->setObjectName(QStringLiteral("labelHeighValColor"));
        labelHeighValColor->setMinimumSize(QSize(30, 0));
        labelHeighValColor->setMaximumSize(QSize(80, 16777215));
        labelHeighValColor->setStyleSheet(QStringLiteral("background-color: rgb(255, 110, 40);"));

        horizontalLayout_3->addWidget(labelHeighValColor);

        label_2 = new QLabel(widgetLable);
        label_2->setObjectName(QStringLiteral("label_2"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        label_2->setFont(font1);

        horizontalLayout_3->addWidget(label_2);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_5);

        labelMidValColor = new QLabel(widgetLable);
        labelMidValColor->setObjectName(QStringLiteral("labelMidValColor"));
        labelMidValColor->setMinimumSize(QSize(30, 0));
        labelMidValColor->setMaximumSize(QSize(80, 16777215));
        labelMidValColor->setStyleSheet(QStringLiteral("background-color: rgb(0, 0, 255);"));

        horizontalLayout_3->addWidget(labelMidValColor);

        label_4 = new QLabel(widgetLable);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setFont(font1);

        horizontalLayout_3->addWidget(label_4);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_6);

        labelLowValColor = new QLabel(widgetLable);
        labelLowValColor->setObjectName(QStringLiteral("labelLowValColor"));
        labelLowValColor->setMinimumSize(QSize(30, 0));
        labelLowValColor->setMaximumSize(QSize(80, 16777215));
        labelLowValColor->setStyleSheet(QStringLiteral("background-color: rgb(0, 255, 0);"));

        horizontalLayout_3->addWidget(labelLowValColor);

        label_6 = new QLabel(widgetLable);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setFont(font1);

        horizontalLayout_3->addWidget(label_6);


        gridLayout->addWidget(widgetLable, 3, 0, 1, 2);

        dateEdit_resultCuve = new QDateEdit(ReplaceTheTestTubeTray);
        dateEdit_resultCuve->setObjectName(QStringLiteral("dateEdit_resultCuve"));
        dateEdit_resultCuve->setMinimumSize(QSize(130, 35));
        dateEdit_resultCuve->setMaximumSize(QSize(16777215, 35));

        gridLayout->addWidget(dateEdit_resultCuve, 2, 0, 1, 2);

        widget_PECurve = new QCustomPlot(ReplaceTheTestTubeTray);
        widget_PECurve->setObjectName(QStringLiteral("widget_PECurve"));
        widget_PECurve->setMinimumSize(QSize(0, 240));

        gridLayout->addWidget(widget_PECurve, 4, 0, 1, 2);


        retranslateUi(ReplaceTheTestTubeTray);

        QMetaObject::connectSlotsByName(ReplaceTheTestTubeTray);
    } // setupUi

    void retranslateUi(QWidget *ReplaceTheTestTubeTray)
    {
        ReplaceTheTestTubeTray->setWindowTitle(QApplication::translate("ReplaceTheTestTubeTray", "\346\233\264\346\215\242\350\257\225\347\256\241\347\233\230", nullptr));
        groupBox_testChannel->setTitle(QApplication::translate("ReplaceTheTestTubeTray", "\346\265\213\350\257\225\351\200\232\351\201\223", nullptr));
        groupBox_testing->setTitle(QApplication::translate("ReplaceTheTestTubeTray", "\346\200\247\350\203\275\346\243\200\346\265\213", nullptr));
        toolButton->setText(QApplication::translate("ReplaceTheTestTubeTray", "\345\220\257\345\212\250", nullptr));
        groupBox_inqure->setTitle(QApplication::translate("ReplaceTheTestTubeTray", "\345\256\214\346\210\220\350\256\260\345\275\225", nullptr));
        pushButton_day->setText(QApplication::translate("ReplaceTheTestTubeTray", "PushButton", nullptr));
        pushButton_month->setText(QApplication::translate("ReplaceTheTestTubeTray", "PushButton", nullptr));
        pushButton_year->setText(QApplication::translate("ReplaceTheTestTubeTray", "PushButton", nullptr));
        pushButton_all->setText(QApplication::translate("ReplaceTheTestTubeTray", "PushButton", nullptr));
        labelHeighValColor->setText(QString());
        label_2->setText(QApplication::translate("ReplaceTheTestTubeTray", "\351\253\230\345\200\274", nullptr));
        labelMidValColor->setText(QString());
        label_4->setText(QApplication::translate("ReplaceTheTestTubeTray", "\344\270\255\345\200\274", nullptr));
        labelLowValColor->setText(QString());
        label_6->setText(QApplication::translate("ReplaceTheTestTubeTray", "\344\275\216\345\200\274", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ReplaceTheTestTubeTray: public Ui_ReplaceTheTestTubeTray {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPLACETHETESTTUBETRAY_H
