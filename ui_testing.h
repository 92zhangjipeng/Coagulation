/********************************************************************************
** Form generated from reading UI file 'testing.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTING_H
#define UI_TESTING_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "circleprogress.h"
#include "progressbar.h"

QT_BEGIN_NAMESPACE

class Ui_Testing
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_Machine;
    QVBoxLayout *verticalLayout;
    QWidget *widget_Abandoned_new;
    QWidget *widget_cleanagent;
    QWidget *widget_Reagents;
    CircleProgress *widget_showtips;
    QWidget *widget_main;
    QWidget *widget_channelarea;
    QWidget *DroptheCup;
    QWidget *widget_Module1;
    QGridLayout *gridLayout_module1;
    ProgressBar *Channel_bock_1;
    ProgressBar *Channel_bock_2;
    ProgressBar *Channel_bock_3;
    ProgressBar *Channel_bock_4;
    QLabel *Channel_info_1;
    QLabel *Channel_info_2;
    QLabel *Channel_info_3;
    QLabel *Channel_info_4;
    QWidget *widget_Module2;
    QGridLayout *gridLayoutModule2;
    ProgressBar *Channel_bock_5;
    ProgressBar *Channel_bock_6;
    ProgressBar *Channel_bock_7;
    ProgressBar *Channel_bock_8;
    QLabel *Channel_info_5;
    QLabel *Channel_info_6;
    QLabel *Channel_info_7;
    QLabel *Channel_info_8;
    QWidget *widget_Module3;
    QGridLayout *gridLayoutModule3;
    ProgressBar *Channel_bock_9;
    ProgressBar *Channel_bock_10;
    ProgressBar *Channel_bock_11;
    ProgressBar *Channel_bock_12;
    QLabel *Channel_info_9;
    QLabel *Channel_info_10;
    QLabel *Channel_info_11;
    QLabel *Channel_info_12;
    QWidget *widget_testcup;
    QWidget *widget_TestCup_0;
    QWidget *widget_TestCup_1;
    QWidget *widget_TestCup_2;
    QWidget *widget_TestCup_3;
    QWidget *widget_Sample_1;
    QWidget *widget_ModulTemp;
    QVBoxLayout *verticalLayout_modul;
    QWidget *widget;

    void setupUi(QWidget *Testing)
    {
        if (Testing->objectName().isEmpty())
            Testing->setObjectName(QStringLiteral("Testing"));
        Testing->resize(1752, 877);
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(12);
        Testing->setFont(font);
        Testing->setStyleSheet(QLatin1String("QWidget#Testing{  \n"
"    border: 1px solid rgb(180, 183, 198);\n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        gridLayout = new QGridLayout(Testing);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        widget_Machine = new QWidget(Testing);
        widget_Machine->setObjectName(QStringLiteral("widget_Machine"));
        widget_Machine->setMinimumSize(QSize(90, 110));
        widget_Machine->setMaximumSize(QSize(110, 16777215));
        widget_Machine->setStyleSheet(QLatin1String("QWidget#widget_Machine{  \n"
"    border: 1px solid rgb(220, 220, 220);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        verticalLayout = new QVBoxLayout(widget_Machine);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(20, 5, 5, 5);
        widget_Abandoned_new = new QWidget(widget_Machine);
        widget_Abandoned_new->setObjectName(QStringLiteral("widget_Abandoned_new"));
        widget_Abandoned_new->setMinimumSize(QSize(80, 0));
        widget_Abandoned_new->setMaximumSize(QSize(80, 16777215));
        widget_Abandoned_new->setStyleSheet(QLatin1String("QWidget#widget_Abandoned_new{  \n"
"    border: 2px solid rgb(220, 220, 220);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        verticalLayout->addWidget(widget_Abandoned_new);

        widget_cleanagent = new QWidget(widget_Machine);
        widget_cleanagent->setObjectName(QStringLiteral("widget_cleanagent"));
        widget_cleanagent->setMinimumSize(QSize(80, 0));
        widget_cleanagent->setMaximumSize(QSize(80, 16777215));
        widget_cleanagent->setStyleSheet(QLatin1String("QWidget#widget_cleanagent{  \n"
"    border: 2px solid rgb(220, 220, 220);\n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        verticalLayout->addWidget(widget_cleanagent);

        widget_Reagents = new QWidget(widget_Machine);
        widget_Reagents->setObjectName(QStringLiteral("widget_Reagents"));
        widget_Reagents->setMinimumSize(QSize(80, 350));
        widget_Reagents->setMaximumSize(QSize(80, 16777215));
        widget_Reagents->setStyleSheet(QLatin1String("QWidget#widget_Reagents{  \n"
"    border: 2px solid rgb(220, 220, 220); \n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));

        verticalLayout->addWidget(widget_Reagents);

        widget_showtips = new CircleProgress(widget_Machine);
        widget_showtips->setObjectName(QStringLiteral("widget_showtips"));
        widget_showtips->setMaximumSize(QSize(80, 140));
        widget_showtips->setStyleSheet(QLatin1String("QWidget#widget_showtips{  \n"
"    border: 2px solid rgb(220, 220, 220);\n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        verticalLayout->addWidget(widget_showtips);


        gridLayout->addWidget(widget_Machine, 0, 0, 2, 1);

        widget_main = new QWidget(Testing);
        widget_main->setObjectName(QStringLiteral("widget_main"));
        widget_main->setStyleSheet(QLatin1String("QWidget#widget_main{  \n"
"    border: 1px solid rgb(220, 220, 220);  \n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        widget_channelarea = new QWidget(widget_main);
        widget_channelarea->setObjectName(QStringLiteral("widget_channelarea"));
        widget_channelarea->setGeometry(QRect(10, 10, 1571, 191));
        widget_channelarea->setMaximumSize(QSize(16777215, 500));
        widget_channelarea->setStyleSheet(QLatin1String("QWidget#widget_channelarea{  \n"
"    border: 2px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        DroptheCup = new QWidget(widget_channelarea);
        DroptheCup->setObjectName(QStringLiteral("DroptheCup"));
        DroptheCup->setGeometry(QRect(1480, 20, 80, 120));
        DroptheCup->setMinimumSize(QSize(80, 120));
        DroptheCup->setMaximumSize(QSize(120, 120));
        DroptheCup->setStyleSheet(QLatin1String("QWidget#DroptheCup{  \n"
"    border: 2px solid  rgb(224, 224, 223);\n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        widget_Module1 = new QWidget(widget_channelarea);
        widget_Module1->setObjectName(QStringLiteral("widget_Module1"));
        widget_Module1->setGeometry(QRect(0, 10, 476, 154));
        widget_Module1->setMinimumSize(QSize(0, 140));
        widget_Module1->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:16px;\n"
"background: transparent;"));
        gridLayout_module1 = new QGridLayout(widget_Module1);
        gridLayout_module1->setSpacing(0);
        gridLayout_module1->setObjectName(QStringLiteral("gridLayout_module1"));
        gridLayout_module1->setContentsMargins(5, 1, 0, 10);
        Channel_bock_1 = new ProgressBar(widget_Module1);
        Channel_bock_1->setObjectName(QStringLiteral("Channel_bock_1"));
        Channel_bock_1->setMinimumSize(QSize(110, 110));
        Channel_bock_1->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_1, 0, 0, 1, 1);

        Channel_bock_2 = new ProgressBar(widget_Module1);
        Channel_bock_2->setObjectName(QStringLiteral("Channel_bock_2"));
        Channel_bock_2->setMinimumSize(QSize(110, 110));
        Channel_bock_2->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_2, 0, 1, 1, 1);

        Channel_bock_3 = new ProgressBar(widget_Module1);
        Channel_bock_3->setObjectName(QStringLiteral("Channel_bock_3"));
        Channel_bock_3->setMinimumSize(QSize(110, 110));
        Channel_bock_3->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_3, 0, 2, 1, 1);

        Channel_bock_4 = new ProgressBar(widget_Module1);
        Channel_bock_4->setObjectName(QStringLiteral("Channel_bock_4"));
        Channel_bock_4->setMinimumSize(QSize(110, 110));
        Channel_bock_4->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_4, 0, 3, 1, 1);

        Channel_info_1 = new QLabel(widget_Module1);
        Channel_info_1->setObjectName(QStringLiteral("Channel_info_1"));
        Channel_info_1->setMinimumSize(QSize(0, 25));
        Channel_info_1->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        Channel_info_1->setFont(font1);
        Channel_info_1->setAlignment(Qt::AlignCenter);

        gridLayout_module1->addWidget(Channel_info_1, 1, 0, 1, 1);

        Channel_info_2 = new QLabel(widget_Module1);
        Channel_info_2->setObjectName(QStringLiteral("Channel_info_2"));
        Channel_info_2->setMinimumSize(QSize(0, 25));
        Channel_info_2->setMaximumSize(QSize(16777215, 25));
        Channel_info_2->setFont(font1);
        Channel_info_2->setAlignment(Qt::AlignCenter);

        gridLayout_module1->addWidget(Channel_info_2, 1, 1, 1, 1);

        Channel_info_3 = new QLabel(widget_Module1);
        Channel_info_3->setObjectName(QStringLiteral("Channel_info_3"));
        Channel_info_3->setMinimumSize(QSize(0, 25));
        Channel_info_3->setMaximumSize(QSize(16777215, 25));
        Channel_info_3->setFont(font1);
        Channel_info_3->setAlignment(Qt::AlignCenter);

        gridLayout_module1->addWidget(Channel_info_3, 1, 2, 1, 1);

        Channel_info_4 = new QLabel(widget_Module1);
        Channel_info_4->setObjectName(QStringLiteral("Channel_info_4"));
        Channel_info_4->setMinimumSize(QSize(0, 25));
        Channel_info_4->setMaximumSize(QSize(16777215, 25));
        Channel_info_4->setFont(font1);
        Channel_info_4->setAlignment(Qt::AlignCenter);

        gridLayout_module1->addWidget(Channel_info_4, 1, 3, 1, 1);

        widget_Module2 = new QWidget(widget_channelarea);
        widget_Module2->setObjectName(QStringLiteral("widget_Module2"));
        widget_Module2->setGeometry(QRect(490, 10, 476, 154));
        widget_Module2->setMinimumSize(QSize(0, 140));
        widget_Module2->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:16px;\n"
"background: transparent;"));
        gridLayoutModule2 = new QGridLayout(widget_Module2);
        gridLayoutModule2->setSpacing(0);
        gridLayoutModule2->setObjectName(QStringLiteral("gridLayoutModule2"));
        gridLayoutModule2->setContentsMargins(5, 1, 0, 10);
        Channel_bock_5 = new ProgressBar(widget_Module2);
        Channel_bock_5->setObjectName(QStringLiteral("Channel_bock_5"));
        Channel_bock_5->setMinimumSize(QSize(110, 110));
        Channel_bock_5->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule2->addWidget(Channel_bock_5, 0, 0, 1, 1);

        Channel_bock_6 = new ProgressBar(widget_Module2);
        Channel_bock_6->setObjectName(QStringLiteral("Channel_bock_6"));
        Channel_bock_6->setMinimumSize(QSize(110, 110));
        Channel_bock_6->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule2->addWidget(Channel_bock_6, 0, 1, 1, 1);

        Channel_bock_7 = new ProgressBar(widget_Module2);
        Channel_bock_7->setObjectName(QStringLiteral("Channel_bock_7"));
        Channel_bock_7->setMinimumSize(QSize(110, 110));
        Channel_bock_7->setMaximumSize(QSize(166666, 110));

        gridLayoutModule2->addWidget(Channel_bock_7, 0, 2, 1, 1);

        Channel_bock_8 = new ProgressBar(widget_Module2);
        Channel_bock_8->setObjectName(QStringLiteral("Channel_bock_8"));
        Channel_bock_8->setMinimumSize(QSize(110, 110));
        Channel_bock_8->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule2->addWidget(Channel_bock_8, 0, 3, 1, 1);

        Channel_info_5 = new QLabel(widget_Module2);
        Channel_info_5->setObjectName(QStringLiteral("Channel_info_5"));
        Channel_info_5->setMinimumSize(QSize(0, 20));
        Channel_info_5->setMaximumSize(QSize(16777215, 20));
        Channel_info_5->setFont(font1);
        Channel_info_5->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_5, 1, 0, 1, 1);

        Channel_info_6 = new QLabel(widget_Module2);
        Channel_info_6->setObjectName(QStringLiteral("Channel_info_6"));
        Channel_info_6->setMinimumSize(QSize(0, 20));
        Channel_info_6->setMaximumSize(QSize(16777215, 20));
        Channel_info_6->setFont(font1);
        Channel_info_6->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_6, 1, 1, 1, 1);

        Channel_info_7 = new QLabel(widget_Module2);
        Channel_info_7->setObjectName(QStringLiteral("Channel_info_7"));
        Channel_info_7->setMinimumSize(QSize(0, 20));
        Channel_info_7->setMaximumSize(QSize(16777215, 20));
        Channel_info_7->setFont(font1);
        Channel_info_7->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_7, 1, 2, 1, 1);

        Channel_info_8 = new QLabel(widget_Module2);
        Channel_info_8->setObjectName(QStringLiteral("Channel_info_8"));
        Channel_info_8->setMinimumSize(QSize(0, 20));
        Channel_info_8->setMaximumSize(QSize(16777215, 20));
        Channel_info_8->setFont(font1);
        Channel_info_8->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_8, 1, 3, 1, 1);

        widget_Module3 = new QWidget(widget_channelarea);
        widget_Module3->setObjectName(QStringLiteral("widget_Module3"));
        widget_Module3->setGeometry(QRect(975, 10, 476, 154));
        widget_Module3->setMinimumSize(QSize(0, 140));
        widget_Module3->setStyleSheet(QString::fromUtf8("border: 0px ;  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:16px;\n"
"background: transparent;"));
        gridLayoutModule3 = new QGridLayout(widget_Module3);
        gridLayoutModule3->setSpacing(0);
        gridLayoutModule3->setObjectName(QStringLiteral("gridLayoutModule3"));
        gridLayoutModule3->setContentsMargins(5, 1, 0, 10);
        Channel_bock_9 = new ProgressBar(widget_Module3);
        Channel_bock_9->setObjectName(QStringLiteral("Channel_bock_9"));
        Channel_bock_9->setMinimumSize(QSize(110, 110));
        Channel_bock_9->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_9, 0, 0, 1, 1);

        Channel_bock_10 = new ProgressBar(widget_Module3);
        Channel_bock_10->setObjectName(QStringLiteral("Channel_bock_10"));
        Channel_bock_10->setMinimumSize(QSize(110, 110));
        Channel_bock_10->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_10, 0, 1, 1, 1);

        Channel_bock_11 = new ProgressBar(widget_Module3);
        Channel_bock_11->setObjectName(QStringLiteral("Channel_bock_11"));
        Channel_bock_11->setMinimumSize(QSize(110, 110));
        Channel_bock_11->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_11, 0, 2, 1, 1);

        Channel_bock_12 = new ProgressBar(widget_Module3);
        Channel_bock_12->setObjectName(QStringLiteral("Channel_bock_12"));
        Channel_bock_12->setMinimumSize(QSize(110, 110));
        Channel_bock_12->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_12, 0, 3, 1, 1);

        Channel_info_9 = new QLabel(widget_Module3);
        Channel_info_9->setObjectName(QStringLiteral("Channel_info_9"));
        Channel_info_9->setMinimumSize(QSize(0, 20));
        Channel_info_9->setMaximumSize(QSize(16777215, 20));
        Channel_info_9->setFont(font1);
        Channel_info_9->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_9, 1, 0, 1, 1);

        Channel_info_10 = new QLabel(widget_Module3);
        Channel_info_10->setObjectName(QStringLiteral("Channel_info_10"));
        Channel_info_10->setMinimumSize(QSize(0, 20));
        Channel_info_10->setMaximumSize(QSize(16777215, 20));
        Channel_info_10->setFont(font1);
        Channel_info_10->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_10, 1, 1, 1, 1);

        Channel_info_11 = new QLabel(widget_Module3);
        Channel_info_11->setObjectName(QStringLiteral("Channel_info_11"));
        Channel_info_11->setMinimumSize(QSize(0, 20));
        Channel_info_11->setMaximumSize(QSize(16777215, 20));
        Channel_info_11->setFont(font1);
        Channel_info_11->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_11, 1, 2, 1, 1);

        Channel_info_12 = new QLabel(widget_Module3);
        Channel_info_12->setObjectName(QStringLiteral("Channel_info_12"));
        Channel_info_12->setMinimumSize(QSize(0, 20));
        Channel_info_12->setMaximumSize(QSize(16777215, 20));
        Channel_info_12->setFont(font1);
        Channel_info_12->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_12, 1, 3, 1, 1);

        widget_testcup = new QWidget(widget_main);
        widget_testcup->setObjectName(QStringLiteral("widget_testcup"));
        widget_testcup->setGeometry(QRect(10, 215, 1551, 351));
        widget_testcup->setMinimumSize(QSize(0, 0));
        widget_testcup->setStyleSheet(QLatin1String("   border: 0px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  "));
        widget_TestCup_0 = new QWidget(widget_testcup);
        widget_TestCup_0->setObjectName(QStringLiteral("widget_TestCup_0"));
        widget_TestCup_0->setGeometry(QRect(5, 0, 378, 351));
        widget_TestCup_0->setMinimumSize(QSize(0, 340));
        widget_TestCup_0->setStyleSheet(QLatin1String("QWidget#widget_TestCup_0{  \n"
"    border: 2px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        widget_TestCup_1 = new QWidget(widget_testcup);
        widget_TestCup_1->setObjectName(QStringLiteral("widget_TestCup_1"));
        widget_TestCup_1->setGeometry(QRect(393, 0, 378, 351));
        widget_TestCup_1->setMinimumSize(QSize(0, 340));
        widget_TestCup_1->setStyleSheet(QLatin1String("QWidget#widget_TestCup_1{  \n"
"    border: 2px solid  rgb(224, 224, 223);\n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        widget_TestCup_2 = new QWidget(widget_testcup);
        widget_TestCup_2->setObjectName(QStringLiteral("widget_TestCup_2"));
        widget_TestCup_2->setGeometry(QRect(781, 0, 377, 351));
        widget_TestCup_2->setMinimumSize(QSize(0, 340));
        widget_TestCup_2->setStyleSheet(QLatin1String("QWidget#widget_TestCup_2{  \n"
"    border: 2px solid  rgb(224, 224, 223); \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        widget_TestCup_3 = new QWidget(widget_testcup);
        widget_TestCup_3->setObjectName(QStringLiteral("widget_TestCup_3"));
        widget_TestCup_3->setGeometry(QRect(1168, 0, 378, 351));
        widget_TestCup_3->setMinimumSize(QSize(0, 340));
        widget_TestCup_3->setStyleSheet(QLatin1String("QWidget#widget_TestCup_3{  \n"
"    border: 2px solid  rgb(224, 224, 223);\n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"}"));
        widget_Sample_1 = new QWidget(widget_main);
        widget_Sample_1->setObjectName(QStringLiteral("widget_Sample_1"));
        widget_Sample_1->setEnabled(true);
        widget_Sample_1->setGeometry(QRect(10, 600, 1571, 270));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_Sample_1->sizePolicy().hasHeightForWidth());
        widget_Sample_1->setSizePolicy(sizePolicy);
        widget_Sample_1->setMinimumSize(QSize(800, 220));
        widget_Sample_1->setMaximumSize(QSize(16777215, 9999999));
        widget_Sample_1->setStyleSheet(QLatin1String("QWidget#widget_Sample_1{  \n"
"    border: 2px solid  rgb(224, 224, 223);\n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        gridLayout->addWidget(widget_main, 0, 1, 2, 1);

        widget_ModulTemp = new QWidget(Testing);
        widget_ModulTemp->setObjectName(QStringLiteral("widget_ModulTemp"));
        widget_ModulTemp->setMinimumSize(QSize(0, 0));
        widget_ModulTemp->setMaximumSize(QSize(0, 16777215));
        widget_ModulTemp->setStyleSheet(QLatin1String("QWidget#widget_ModulTemp{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(120, 120, 120);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        verticalLayout_modul = new QVBoxLayout(widget_ModulTemp);
        verticalLayout_modul->setSpacing(0);
        verticalLayout_modul->setObjectName(QStringLiteral("verticalLayout_modul"));
        verticalLayout_modul->setContentsMargins(5, 5, 10, 20);

        gridLayout->addWidget(widget_ModulTemp, 0, 2, 1, 1);

        widget = new QWidget(Testing);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMaximumSize(QSize(0, 0));
        widget->setStyleSheet(QLatin1String("QWidget#widget{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(60, 60, 60);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));

        gridLayout->addWidget(widget, 1, 3, 1, 1);


        retranslateUi(Testing);

        QMetaObject::connectSlotsByName(Testing);
    } // setupUi

    void retranslateUi(QWidget *Testing)
    {
        Testing->setWindowTitle(QApplication::translate("Testing", "Form", nullptr));
        Channel_info_1->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_2->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_3->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_4->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_5->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_6->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_7->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_8->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_9->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_10->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_11->setText(QApplication::translate("Testing", "TextLabel", nullptr));
        Channel_info_12->setText(QApplication::translate("Testing", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Testing: public Ui_Testing {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTING_H
