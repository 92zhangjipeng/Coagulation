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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
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
    QVBoxLayout *verticalLayout_main;
    QWidget *widget_channelarea;
    QHBoxLayout *horizontalLayout_channel;
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
    QSpacerItem *horizontalSpacer_4;
    QWidget *DroptheCup;
    QWidget *widget_testcup;
    QHBoxLayout *horizontalLayout_testcup;
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
        font.setFamily(QStringLiteral("Microsoft YaHei"));
        font.setPointSize(12);
        Testing->setFont(font);
        Testing->setStyleSheet(QString::fromUtf8("/* \344\270\273\347\252\227\345\217\243 - #BCBBB7 \350\211\262\347\263\273 */\n"
"QWidget#Testing{\n"
"    background-color: #BCBBB7;\n"
"    border: none;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"/* \346\211\200\346\234\211 QWidget \347\273\237\344\270\200\350\203\214\346\231\257\350\211\262\345\222\214\350\276\271\346\241\206 */\n"
"QWidget {\n"
"    background-color: #BCBBB7;\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"/* \344\270\273\345\206\205\345\256\271\345\214\272\345\237\237\351\200\217\346\230\216\350\276\271\346\241\206 */\n"
"QWidget#widget_main {\n"
"    background-color: #BCBBB7;\n"
"    border: none;\n"
"}\n"
"\n"
"/* \346\250\241\345\235\227\345\256\271\345\231\250\351\200\217\346\230\216 */\n"
"QWidget#widget_Module1,\n"
"QWidget#widget_Module2,\n"
"QWidget#widget_Module3 {\n"
"    background-color: transparent;\n"
"    border: none;\n"
"}\n"
"\n"
"/* \346\265\213\350\257\225\346\235\257\345\214\272\345\237\237\345\256\271\345\231\250\351\200\217\346\230\216"
                        " */\n"
"QWidget#widget_testcup {\n"
"    background-color: transparent;\n"
"    border: none;\n"
"}\n"
"\n"
"/* \346\265\213\350\257\225\346\235\257\345\255\220\345\256\271\345\231\250 - #DEE2E6 \350\211\262\347\263\273 */\n"
"QWidget#widget_TestCup_0,\n"
"QWidget#widget_TestCup_1,\n"
"QWidget#widget_TestCup_2,\n"
"QWidget#widget_TestCup_3 {\n"
"    background-color: #BCBBB7;\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"/* \346\240\207\347\255\276\346\240\267\345\274\217 - \346\267\261\347\201\260\350\211\262\346\226\207\345\255\227 */\n"
"QLabel {\n"
"    color: #2C3035;\n"
"    background: transparent;\n"
"    font-family: 'Microsoft YaHei';\n"
"    border: none;\n"
"}\n"
"\n"
"/* \346\214\211\351\222\256\351\200\232\347\224\250\346\240\267\345\274\217 */\n"
"QPushButton {\n"
"    background-color: #C8CCD0;\n"
"    border: 1px solid #B0B4B8;\n"
"    border-radius: 5px;\n"
"    color: #2C3035;\n"
"    padding: 5px 10px;\n"
"}\n"
"QPushButton:hover {\n"
"    background-color: "
                        "#C0C4C8;\n"
"}\n"
"QPushButton:pressed {\n"
"    background-color: #B0B4B8;\n"
"}\n"
"\n"
"/* \350\276\223\345\205\245\346\241\206\346\240\267\345\274\217 */\n"
"QLineEdit, QTextEdit, QComboBox {\n"
"    background-color: #BCBBB7;\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 5px;\n"
"    color: #2C3035;\n"
"    padding: 4px;\n"
"}\n"
"QLineEdit:focus, QTextEdit:focus, QComboBox:focus {\n"
"    border: 1px solid #9A9EA3;\n"
"}\n"
"\n"
"/* \346\273\232\345\212\250\346\235\241\346\240\267\345\274\217 */\n"
"QScrollBar:vertical {\n"
"    background-color: #DEE2E6;\n"
"    width: 12px;\n"
"    border-radius: 6px;\n"
"}\n"
"QScrollBar::handle:vertical {\n"
"    background-color: #C5C9CD;\n"
"    border-radius: 6px;\n"
"    min-height: 20px;\n"
"}\n"
"QScrollBar::handle:vertical:hover {\n"
"    background-color: #B0B4B8;\n"
"}\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    border: none;\n"
"    background: none;\n"
"}\n"
"\n"
"/* \350\241\250\346\240\274\346\240\267\345\274\217"
                        " */\n"
"QTableWidget {\n"
"    background-color: #BCBBB7;\n"
"    alternate-background-color: #DEE2E6;\n"
"    gridline-color: #C8CCD0;\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 5px;\n"
"}\n"
"QHeaderView::section {\n"
"    background-color: #C8CCD0;\n"
"    color: #2C3035;\n"
"    font-family: 'Microsoft YaHei';\n"
"    font-size: 12px;\n"
"    padding: 4px;\n"
"    border: none;\n"
"}\n"
"QTableWidget::item {\n"
"    color: #2C3035;\n"
"}\n"
"\n"
"/* \345\210\206\347\273\204\346\241\206\346\240\267\345\274\217 */\n"
"QGroupBox {\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 8px;\n"
"    margin-top: 1ex;\n"
"    color: #2C3035;\n"
"    font-weight: bold;\n"
"}\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top center;\n"
"    padding: 0 5px;\n"
"}\n"
"\n"
"/* \350\277\233\345\272\246\346\235\241\346\240\267\345\274\217 */\n"
"QProgressBar {\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 5px;\n"
"    background-color: #E8ECF0;\n"
" "
                        "   text-align: center;\n"
"    color: #2C3035;\n"
"}\n"
"QProgressBar::chunk {\n"
"    background-color: #9A9EA3;\n"
"    border-radius: 5px;\n"
"}\n"
"\n"
"/* \351\200\211\351\241\271\345\215\241\346\240\267\345\274\217 */\n"
"QTabWidget::pane {\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 5px;\n"
"    background-color: #E8ECF0;\n"
"}\n"
"QTabBar::tab {\n"
"    background-color: #C8CCD0;\n"
"    color: #2C3035;\n"
"    padding: 6px 12px;\n"
"    margin-right: 2px;\n"
"    border-top-left-radius: 5px;\n"
"    border-top-right-radius: 5px;\n"
"}\n"
"QTabBar::tab:selected {\n"
"    background-color: #E8ECF0;\n"
"    color: #1C2025;\n"
"}\n"
"QTabBar::tab:hover:!selected {\n"
"    background-color: #C0C4C8;\n"
"}\n"
"\n"
"/* \345\244\215\351\200\211\346\241\206\345\222\214\345\215\225\351\200\211\346\241\206\346\240\267\345\274\217 */\n"
"QCheckBox, QRadioButton {\n"
"    color: #2C3035;\n"
"    spacing: 8px;\n"
"}\n"
"QCheckBox::indicator, QRadioButton::indicator {\n"
"    width: 16px;\n"
"    heigh"
                        "t: 16px;\n"
"    border: 1px solid #C5C9CD;\n"
"    border-radius: 3px;\n"
"    background-color: #E8ECF0;\n"
"}\n"
"QCheckBox::indicator:checked, QRadioButton::indicator:checked {\n"
"    background-color: #9A9EA3;\n"
"}\n"
"QRadioButton::indicator {\n"
"    border-radius: 8px;\n"
"}\n"
"\n"
"/* \346\273\221\345\212\250\346\235\241\346\240\267\345\274\217 */\n"
"QSlider::groove:horizontal {\n"
"    border: 1px solid #C5C9CD;\n"
"    height: 6px;\n"
"    background-color: #E8ECF0;\n"
"    border-radius: 3px;\n"
"}\n"
"QSlider::handle:horizontal {\n"
"    background-color: #9A9EA3;\n"
"    width: 14px;\n"
"    margin: -4px 0;\n"
"    border-radius: 7px;\n"
"}\n"
"QSlider::handle:horizontal:hover {\n"
"    background-color: #B0B4B8;\n"
"}\n"
"\n"
"/* \350\217\234\345\215\225\346\240\217\346\240\267\345\274\217 */\n"
"QMenuBar {\n"
"    background-color: #C8CCD0;\n"
"    color: #2C3035;\n"
"}\n"
"QMenuBar::item:selected {\n"
"    background-color: #C0C4C8;\n"
"}\n"
"QMenu {\n"
"    background-color: #E8ECF0;\n"
""
                        "    border: 1px solid #C5C9CD;\n"
"    color: #2C3035;\n"
"}\n"
"QMenu::item:selected {\n"
"    background-color: #C8CCD0;\n"
"}\n"
"\n"
"/* \347\212\266\346\200\201\346\240\217\346\240\267\345\274\217 */\n"
"QStatusBar {\n"
"    background-color: #C8CCD0;\n"
"    color: #2C3035;\n"
"}\n"
"\n"
"/* \346\266\210\346\201\257\346\241\206\346\240\267\345\274\217 */\n"
"QMessageBox {\n"
"    background-color: #E8ECF0;\n"
"    color: #2C3035;\n"
"}\n"
"QMessageBox QPushButton {\n"
"    min-width: 80px;\n"
"}"));
        gridLayout = new QGridLayout(Testing);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        widget_Machine = new QWidget(Testing);
        widget_Machine->setObjectName(QStringLiteral("widget_Machine"));
        widget_Machine->setMinimumSize(QSize(90, 110));
        widget_Machine->setMaximumSize(QSize(110, 16777215));
        verticalLayout = new QVBoxLayout(widget_Machine);
        verticalLayout->setSpacing(8);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(10, 5, 5, 5);
        widget_Abandoned_new = new QWidget(widget_Machine);
        widget_Abandoned_new->setObjectName(QStringLiteral("widget_Abandoned_new"));
        widget_Abandoned_new->setMinimumSize(QSize(80, 0));
        widget_Abandoned_new->setMaximumSize(QSize(80, 16777215));

        verticalLayout->addWidget(widget_Abandoned_new);

        widget_cleanagent = new QWidget(widget_Machine);
        widget_cleanagent->setObjectName(QStringLiteral("widget_cleanagent"));
        widget_cleanagent->setMinimumSize(QSize(80, 0));
        widget_cleanagent->setMaximumSize(QSize(80, 16777215));

        verticalLayout->addWidget(widget_cleanagent);

        widget_Reagents = new QWidget(widget_Machine);
        widget_Reagents->setObjectName(QStringLiteral("widget_Reagents"));
        widget_Reagents->setMinimumSize(QSize(80, 350));
        widget_Reagents->setMaximumSize(QSize(80, 16777215));

        verticalLayout->addWidget(widget_Reagents);

        widget_showtips = new CircleProgress(widget_Machine);
        widget_showtips->setObjectName(QStringLiteral("widget_showtips"));
        widget_showtips->setMaximumSize(QSize(80, 140));

        verticalLayout->addWidget(widget_showtips);


        gridLayout->addWidget(widget_Machine, 0, 0, 2, 1);

        widget_main = new QWidget(Testing);
        widget_main->setObjectName(QStringLiteral("widget_main"));
        verticalLayout_main = new QVBoxLayout(widget_main);
        verticalLayout_main->setSpacing(5);
        verticalLayout_main->setObjectName(QStringLiteral("verticalLayout_main"));
        verticalLayout_main->setContentsMargins(0, 0, 5, 0);
        widget_channelarea = new QWidget(widget_main);
        widget_channelarea->setObjectName(QStringLiteral("widget_channelarea"));
        widget_channelarea->setMinimumSize(QSize(0, 200));
        widget_channelarea->setMaximumSize(QSize(16777215, 220));
        horizontalLayout_channel = new QHBoxLayout(widget_channelarea);
        horizontalLayout_channel->setSpacing(5);
        horizontalLayout_channel->setObjectName(QStringLiteral("horizontalLayout_channel"));
        horizontalLayout_channel->setContentsMargins(5, 1, 0, 1);
        widget_Module1 = new QWidget(widget_channelarea);
        widget_Module1->setObjectName(QStringLiteral("widget_Module1"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget_Module1->sizePolicy().hasHeightForWidth());
        widget_Module1->setSizePolicy(sizePolicy);
        widget_Module1->setMinimumSize(QSize(0, 140));
        gridLayout_module1 = new QGridLayout(widget_Module1);
        gridLayout_module1->setObjectName(QStringLiteral("gridLayout_module1"));
        gridLayout_module1->setHorizontalSpacing(5);
        gridLayout_module1->setVerticalSpacing(1);
        gridLayout_module1->setContentsMargins(5, 1, 5, 1);
        Channel_bock_1 = new ProgressBar(widget_Module1);
        Channel_bock_1->setObjectName(QStringLiteral("Channel_bock_1"));
        Channel_bock_1->setMinimumSize(QSize(100, 100));
        Channel_bock_1->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_1, 0, 0, 1, 1);

        Channel_bock_2 = new ProgressBar(widget_Module1);
        Channel_bock_2->setObjectName(QStringLiteral("Channel_bock_2"));
        Channel_bock_2->setMinimumSize(QSize(100, 100));
        Channel_bock_2->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_2, 0, 1, 1, 1);

        Channel_bock_3 = new ProgressBar(widget_Module1);
        Channel_bock_3->setObjectName(QStringLiteral("Channel_bock_3"));
        Channel_bock_3->setMinimumSize(QSize(100, 100));
        Channel_bock_3->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_3, 0, 2, 1, 1);

        Channel_bock_4 = new ProgressBar(widget_Module1);
        Channel_bock_4->setObjectName(QStringLiteral("Channel_bock_4"));
        Channel_bock_4->setMinimumSize(QSize(100, 100));
        Channel_bock_4->setMaximumSize(QSize(16777215, 110));

        gridLayout_module1->addWidget(Channel_bock_4, 0, 3, 1, 1);

        Channel_info_1 = new QLabel(widget_Module1);
        Channel_info_1->setObjectName(QStringLiteral("Channel_info_1"));
        Channel_info_1->setMinimumSize(QSize(0, 25));
        Channel_info_1->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei"));
        font1.setPointSize(10);
        Channel_info_1->setFont(font1);
        Channel_info_1->setAlignment(Qt::AlignCenter);

        gridLayout_module1->addWidget(Channel_info_1, 1, 0, 1, 1);

        Channel_info_2 = new QLabel(widget_Module1);
        Channel_info_2->setObjectName(QStringLiteral("Channel_info_2"));
        Channel_info_2->setMinimumSize(QSize(0, 30));
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


        horizontalLayout_channel->addWidget(widget_Module1);

        widget_Module2 = new QWidget(widget_channelarea);
        widget_Module2->setObjectName(QStringLiteral("widget_Module2"));
        sizePolicy.setHeightForWidth(widget_Module2->sizePolicy().hasHeightForWidth());
        widget_Module2->setSizePolicy(sizePolicy);
        widget_Module2->setMinimumSize(QSize(0, 140));
        gridLayoutModule2 = new QGridLayout(widget_Module2);
        gridLayoutModule2->setObjectName(QStringLiteral("gridLayoutModule2"));
        gridLayoutModule2->setHorizontalSpacing(5);
        gridLayoutModule2->setVerticalSpacing(1);
        gridLayoutModule2->setContentsMargins(5, 1, 5, 1);
        Channel_bock_5 = new ProgressBar(widget_Module2);
        Channel_bock_5->setObjectName(QStringLiteral("Channel_bock_5"));
        Channel_bock_5->setMinimumSize(QSize(100, 100));
        Channel_bock_5->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule2->addWidget(Channel_bock_5, 0, 0, 1, 1);

        Channel_bock_6 = new ProgressBar(widget_Module2);
        Channel_bock_6->setObjectName(QStringLiteral("Channel_bock_6"));
        Channel_bock_6->setMinimumSize(QSize(100, 100));
        Channel_bock_6->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule2->addWidget(Channel_bock_6, 0, 1, 1, 1);

        Channel_bock_7 = new ProgressBar(widget_Module2);
        Channel_bock_7->setObjectName(QStringLiteral("Channel_bock_7"));
        Channel_bock_7->setMinimumSize(QSize(100, 100));
        Channel_bock_7->setMaximumSize(QSize(166666, 110));

        gridLayoutModule2->addWidget(Channel_bock_7, 0, 2, 1, 1);

        Channel_bock_8 = new ProgressBar(widget_Module2);
        Channel_bock_8->setObjectName(QStringLiteral("Channel_bock_8"));
        Channel_bock_8->setMinimumSize(QSize(100, 100));
        Channel_bock_8->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule2->addWidget(Channel_bock_8, 0, 3, 1, 1);

        Channel_info_5 = new QLabel(widget_Module2);
        Channel_info_5->setObjectName(QStringLiteral("Channel_info_5"));
        Channel_info_5->setMinimumSize(QSize(0, 25));
        Channel_info_5->setMaximumSize(QSize(16777215, 25));
        Channel_info_5->setFont(font1);
        Channel_info_5->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_5, 1, 0, 1, 1);

        Channel_info_6 = new QLabel(widget_Module2);
        Channel_info_6->setObjectName(QStringLiteral("Channel_info_6"));
        Channel_info_6->setMinimumSize(QSize(0, 25));
        Channel_info_6->setMaximumSize(QSize(16777215, 25));
        Channel_info_6->setFont(font1);
        Channel_info_6->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_6, 1, 1, 1, 1);

        Channel_info_7 = new QLabel(widget_Module2);
        Channel_info_7->setObjectName(QStringLiteral("Channel_info_7"));
        Channel_info_7->setMinimumSize(QSize(0, 25));
        Channel_info_7->setMaximumSize(QSize(16777215, 25));
        Channel_info_7->setFont(font1);
        Channel_info_7->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_7, 1, 2, 1, 1);

        Channel_info_8 = new QLabel(widget_Module2);
        Channel_info_8->setObjectName(QStringLiteral("Channel_info_8"));
        Channel_info_8->setMinimumSize(QSize(0, 25));
        Channel_info_8->setMaximumSize(QSize(16777215, 25));
        Channel_info_8->setFont(font1);
        Channel_info_8->setAlignment(Qt::AlignCenter);

        gridLayoutModule2->addWidget(Channel_info_8, 1, 3, 1, 1);


        horizontalLayout_channel->addWidget(widget_Module2);

        widget_Module3 = new QWidget(widget_channelarea);
        widget_Module3->setObjectName(QStringLiteral("widget_Module3"));
        sizePolicy.setHeightForWidth(widget_Module3->sizePolicy().hasHeightForWidth());
        widget_Module3->setSizePolicy(sizePolicy);
        widget_Module3->setMinimumSize(QSize(0, 140));
        gridLayoutModule3 = new QGridLayout(widget_Module3);
        gridLayoutModule3->setObjectName(QStringLiteral("gridLayoutModule3"));
        gridLayoutModule3->setHorizontalSpacing(5);
        gridLayoutModule3->setVerticalSpacing(1);
        gridLayoutModule3->setContentsMargins(5, 1, 5, 1);
        Channel_bock_9 = new ProgressBar(widget_Module3);
        Channel_bock_9->setObjectName(QStringLiteral("Channel_bock_9"));
        Channel_bock_9->setMinimumSize(QSize(100, 100));
        Channel_bock_9->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_9, 0, 0, 1, 1);

        Channel_bock_10 = new ProgressBar(widget_Module3);
        Channel_bock_10->setObjectName(QStringLiteral("Channel_bock_10"));
        Channel_bock_10->setMinimumSize(QSize(100, 100));
        Channel_bock_10->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_10, 0, 1, 1, 1);

        Channel_bock_11 = new ProgressBar(widget_Module3);
        Channel_bock_11->setObjectName(QStringLiteral("Channel_bock_11"));
        Channel_bock_11->setMinimumSize(QSize(100, 100));
        Channel_bock_11->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_11, 0, 2, 1, 1);

        Channel_bock_12 = new ProgressBar(widget_Module3);
        Channel_bock_12->setObjectName(QStringLiteral("Channel_bock_12"));
        Channel_bock_12->setMinimumSize(QSize(100, 100));
        Channel_bock_12->setMaximumSize(QSize(16777215, 110));

        gridLayoutModule3->addWidget(Channel_bock_12, 0, 3, 1, 1);

        Channel_info_9 = new QLabel(widget_Module3);
        Channel_info_9->setObjectName(QStringLiteral("Channel_info_9"));
        Channel_info_9->setMinimumSize(QSize(0, 25));
        Channel_info_9->setMaximumSize(QSize(16777215, 25));
        Channel_info_9->setFont(font1);
        Channel_info_9->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_9, 1, 0, 1, 1);

        Channel_info_10 = new QLabel(widget_Module3);
        Channel_info_10->setObjectName(QStringLiteral("Channel_info_10"));
        Channel_info_10->setMinimumSize(QSize(0, 25));
        Channel_info_10->setMaximumSize(QSize(16777215, 25));
        Channel_info_10->setFont(font1);
        Channel_info_10->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_10, 1, 1, 1, 1);

        Channel_info_11 = new QLabel(widget_Module3);
        Channel_info_11->setObjectName(QStringLiteral("Channel_info_11"));
        Channel_info_11->setMinimumSize(QSize(0, 25));
        Channel_info_11->setMaximumSize(QSize(16777215, 25));
        Channel_info_11->setFont(font1);
        Channel_info_11->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_11, 1, 2, 1, 1);

        Channel_info_12 = new QLabel(widget_Module3);
        Channel_info_12->setObjectName(QStringLiteral("Channel_info_12"));
        Channel_info_12->setMinimumSize(QSize(0, 25));
        Channel_info_12->setMaximumSize(QSize(16777215, 25));
        Channel_info_12->setFont(font1);
        Channel_info_12->setAlignment(Qt::AlignCenter);

        gridLayoutModule3->addWidget(Channel_info_12, 1, 3, 1, 1);


        horizontalLayout_channel->addWidget(widget_Module3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_channel->addItem(horizontalSpacer_4);

        DroptheCup = new QWidget(widget_channelarea);
        DroptheCup->setObjectName(QStringLiteral("DroptheCup"));
        DroptheCup->setMinimumSize(QSize(80, 120));
        DroptheCup->setMaximumSize(QSize(120, 220));

        horizontalLayout_channel->addWidget(DroptheCup);


        verticalLayout_main->addWidget(widget_channelarea);

        widget_testcup = new QWidget(widget_main);
        widget_testcup->setObjectName(QStringLiteral("widget_testcup"));
        widget_testcup->setMinimumSize(QSize(0, 340));
        horizontalLayout_testcup = new QHBoxLayout(widget_testcup);
        horizontalLayout_testcup->setSpacing(5);
        horizontalLayout_testcup->setObjectName(QStringLiteral("horizontalLayout_testcup"));
        horizontalLayout_testcup->setContentsMargins(5, 5, 0, 5);
        widget_TestCup_0 = new QWidget(widget_testcup);
        widget_TestCup_0->setObjectName(QStringLiteral("widget_TestCup_0"));
        sizePolicy.setHeightForWidth(widget_TestCup_0->sizePolicy().hasHeightForWidth());
        widget_TestCup_0->setSizePolicy(sizePolicy);
        widget_TestCup_0->setMinimumSize(QSize(0, 340));

        horizontalLayout_testcup->addWidget(widget_TestCup_0);

        widget_TestCup_1 = new QWidget(widget_testcup);
        widget_TestCup_1->setObjectName(QStringLiteral("widget_TestCup_1"));
        sizePolicy.setHeightForWidth(widget_TestCup_1->sizePolicy().hasHeightForWidth());
        widget_TestCup_1->setSizePolicy(sizePolicy);
        widget_TestCup_1->setMinimumSize(QSize(0, 340));

        horizontalLayout_testcup->addWidget(widget_TestCup_1);

        widget_TestCup_2 = new QWidget(widget_testcup);
        widget_TestCup_2->setObjectName(QStringLiteral("widget_TestCup_2"));
        sizePolicy.setHeightForWidth(widget_TestCup_2->sizePolicy().hasHeightForWidth());
        widget_TestCup_2->setSizePolicy(sizePolicy);
        widget_TestCup_2->setMinimumSize(QSize(0, 340));

        horizontalLayout_testcup->addWidget(widget_TestCup_2);

        widget_TestCup_3 = new QWidget(widget_testcup);
        widget_TestCup_3->setObjectName(QStringLiteral("widget_TestCup_3"));
        sizePolicy.setHeightForWidth(widget_TestCup_3->sizePolicy().hasHeightForWidth());
        widget_TestCup_3->setSizePolicy(sizePolicy);
        widget_TestCup_3->setMinimumSize(QSize(0, 340));

        horizontalLayout_testcup->addWidget(widget_TestCup_3);


        verticalLayout_main->addWidget(widget_testcup);

        widget_Sample_1 = new QWidget(widget_main);
        widget_Sample_1->setObjectName(QStringLiteral("widget_Sample_1"));
        widget_Sample_1->setEnabled(true);
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(widget_Sample_1->sizePolicy().hasHeightForWidth());
        widget_Sample_1->setSizePolicy(sizePolicy1);
        widget_Sample_1->setMinimumSize(QSize(800, 220));
        widget_Sample_1->setMaximumSize(QSize(16777215, 9999999));

        verticalLayout_main->addWidget(widget_Sample_1);


        gridLayout->addWidget(widget_main, 0, 1, 2, 1);

        widget_ModulTemp = new QWidget(Testing);
        widget_ModulTemp->setObjectName(QStringLiteral("widget_ModulTemp"));
        widget_ModulTemp->setMinimumSize(QSize(0, 0));
        widget_ModulTemp->setMaximumSize(QSize(0, 16777215));
        verticalLayout_modul = new QVBoxLayout(widget_ModulTemp);
        verticalLayout_modul->setSpacing(0);
        verticalLayout_modul->setObjectName(QStringLiteral("verticalLayout_modul"));
        verticalLayout_modul->setContentsMargins(5, 5, 5, 5);

        gridLayout->addWidget(widget_ModulTemp, 0, 2, 1, 1);

        widget = new QWidget(Testing);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMaximumSize(QSize(0, 0));

        gridLayout->addWidget(widget, 1, 3, 1, 1);


        retranslateUi(Testing);

        QMetaObject::connectSlotsByName(Testing);
    } // setupUi

    void retranslateUi(QWidget *Testing)
    {
        Testing->setWindowTitle(QApplication::translate("Testing", "Form", nullptr));
        Channel_info_1->setText(QApplication::translate("Testing", "\351\200\232\351\201\2231\357\274\232AA", nullptr));
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
