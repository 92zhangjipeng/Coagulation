/********************************************************************************
** Form generated from reading UI file 'calibrationposition.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATIONPOSITION_H
#define UI_CALIBRATIONPOSITION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CalibrationPosition
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *display_channel_1;
    QHBoxLayout *horizontalLayout_2;
    QTextBrowser *textBrowser_ChannelTubeValue;
    QTextBrowser *textBrowser_ChannelWaterValue;
    QTextBrowser *textBrowser_Addreagent_1;
    QWidget *display_channel_2;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_3;
    QTextBrowser *textBrowser_Channel2_tubevalue;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_4;
    QTextBrowser *textBrowser_Channel_2_addwatervalue;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_5;
    QTextBrowser *textBrowser_Addreagent_2;
    QWidget *display_channel_3;
    QHBoxLayout *horizontalLayout_4;
    QTextBrowser *textBrowser_Channel3_tubevalue;
    QTextBrowser *textBrowser_Channel_3_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_3;
    QWidget *display_channel_4;
    QHBoxLayout *horizontalLayout_5;
    QTextBrowser *textBrowser_Channel4_tubevalue;
    QTextBrowser *textBrowser_Channel_4_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_4;
    QWidget *display_channel_5;
    QHBoxLayout *horizontalLayout_6;
    QTextBrowser *textBrowser_Channel5_tubevalue;
    QTextBrowser *textBrowser_Channel_5_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_5;
    QWidget *display_channel_6;
    QHBoxLayout *horizontalLayout_7;
    QTextBrowser *textBrowser_Channel6_tubevalue;
    QTextBrowser *textBrowser_Channel_6_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_6;
    QWidget *display_channel_7;
    QHBoxLayout *horizontalLayout_8;
    QTextBrowser *textBrowser_Channel7_tubevalue;
    QTextBrowser *textBrowser_Channel_7_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_7;
    QWidget *display_channel_8;
    QHBoxLayout *horizontalLayout_9;
    QTextBrowser *textBrowser_Channel8_tubevalue;
    QTextBrowser *textBrowser_Channel_8_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_8;
    QWidget *display_channel_9;
    QHBoxLayout *horizontalLayout_10;
    QTextBrowser *textBrowser_Channel9_tubevalue;
    QTextBrowser *textBrowser_Channel_9_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_9;
    QWidget *display_channel_10;
    QHBoxLayout *horizontalLayout_11;
    QTextBrowser *textBrowser_Channel10_tubevalue;
    QTextBrowser *textBrowser_Channel_10_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_10;
    QWidget *display_channel_11;
    QHBoxLayout *horizontalLayout_12;
    QTextBrowser *textBrowser_Channel11_tubevalue;
    QTextBrowser *textBrowser_Channel_11_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_11;
    QWidget *display_channel_12;
    QHBoxLayout *horizontalLayout_13;
    QTextBrowser *textBrowser_Channel12_tubevalue;
    QTextBrowser *textBrowser_Channel_12_addwatervalue;
    QTextBrowser *textBrowser_Addreagent_12;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *spinBox_tubenum;
    QToolButton *moveTube2Channel;
    QToolButton *Addwater;
    QToolButton *toolButton_addreagent;
    QComboBox *comboBox_Throw;
    QToolButton *toolButton_back;
    QComboBox *comboBox_Pause;
    QLabel *label_2;
    QSpinBox *spinBox_Absorbul;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *CalibrationPosition)
    {
        if (CalibrationPosition->objectName().isEmpty())
            CalibrationPosition->setObjectName(QStringLiteral("CalibrationPosition"));
        CalibrationPosition->setWindowModality(Qt::WindowModal);
        CalibrationPosition->resize(1409, 800);
        CalibrationPosition->setMinimumSize(QSize(1409, 800));
        CalibrationPosition->setMaximumSize(QSize(999999, 99999));
        CalibrationPosition->setContextMenuPolicy(Qt::NoContextMenu);
        verticalLayout = new QVBoxLayout(CalibrationPosition);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(CalibrationPosition);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font.setPointSize(12);
        tabWidget->setFont(font);
        display_channel_1 = new QWidget();
        display_channel_1->setObjectName(QStringLiteral("display_channel_1"));
        horizontalLayout_2 = new QHBoxLayout(display_channel_1);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        textBrowser_ChannelTubeValue = new QTextBrowser(display_channel_1);
        textBrowser_ChannelTubeValue->setObjectName(QStringLiteral("textBrowser_ChannelTubeValue"));

        horizontalLayout_2->addWidget(textBrowser_ChannelTubeValue);

        textBrowser_ChannelWaterValue = new QTextBrowser(display_channel_1);
        textBrowser_ChannelWaterValue->setObjectName(QStringLiteral("textBrowser_ChannelWaterValue"));

        horizontalLayout_2->addWidget(textBrowser_ChannelWaterValue);

        textBrowser_Addreagent_1 = new QTextBrowser(display_channel_1);
        textBrowser_Addreagent_1->setObjectName(QStringLiteral("textBrowser_Addreagent_1"));

        horizontalLayout_2->addWidget(textBrowser_Addreagent_1);

        tabWidget->addTab(display_channel_1, QString());
        display_channel_2 = new QWidget();
        display_channel_2->setObjectName(QStringLiteral("display_channel_2"));
        horizontalLayout_3 = new QHBoxLayout(display_channel_2);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_3 = new QLabel(display_channel_2);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setMinimumSize(QSize(0, 20));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\351\273\221\344\275\223"));
        label_3->setFont(font1);
        label_3->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(label_3);

        textBrowser_Channel2_tubevalue = new QTextBrowser(display_channel_2);
        textBrowser_Channel2_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel2_tubevalue"));

        verticalLayout_2->addWidget(textBrowser_Channel2_tubevalue);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_4 = new QLabel(display_channel_2);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setMinimumSize(QSize(0, 20));
        label_4->setFont(font1);
        label_4->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(label_4);

        textBrowser_Channel_2_addwatervalue = new QTextBrowser(display_channel_2);
        textBrowser_Channel_2_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_2_addwatervalue"));

        verticalLayout_3->addWidget(textBrowser_Channel_2_addwatervalue);


        horizontalLayout_3->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_5 = new QLabel(display_channel_2);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setMinimumSize(QSize(100, 20));
        label_5->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(label_5);

        textBrowser_Addreagent_2 = new QTextBrowser(display_channel_2);
        textBrowser_Addreagent_2->setObjectName(QStringLiteral("textBrowser_Addreagent_2"));

        verticalLayout_4->addWidget(textBrowser_Addreagent_2);


        horizontalLayout_3->addLayout(verticalLayout_4);

        tabWidget->addTab(display_channel_2, QString());
        display_channel_3 = new QWidget();
        display_channel_3->setObjectName(QStringLiteral("display_channel_3"));
        horizontalLayout_4 = new QHBoxLayout(display_channel_3);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        textBrowser_Channel3_tubevalue = new QTextBrowser(display_channel_3);
        textBrowser_Channel3_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel3_tubevalue"));

        horizontalLayout_4->addWidget(textBrowser_Channel3_tubevalue);

        textBrowser_Channel_3_addwatervalue = new QTextBrowser(display_channel_3);
        textBrowser_Channel_3_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_3_addwatervalue"));

        horizontalLayout_4->addWidget(textBrowser_Channel_3_addwatervalue);

        textBrowser_Addreagent_3 = new QTextBrowser(display_channel_3);
        textBrowser_Addreagent_3->setObjectName(QStringLiteral("textBrowser_Addreagent_3"));

        horizontalLayout_4->addWidget(textBrowser_Addreagent_3);

        tabWidget->addTab(display_channel_3, QString());
        display_channel_4 = new QWidget();
        display_channel_4->setObjectName(QStringLiteral("display_channel_4"));
        horizontalLayout_5 = new QHBoxLayout(display_channel_4);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        textBrowser_Channel4_tubevalue = new QTextBrowser(display_channel_4);
        textBrowser_Channel4_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel4_tubevalue"));

        horizontalLayout_5->addWidget(textBrowser_Channel4_tubevalue);

        textBrowser_Channel_4_addwatervalue = new QTextBrowser(display_channel_4);
        textBrowser_Channel_4_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_4_addwatervalue"));

        horizontalLayout_5->addWidget(textBrowser_Channel_4_addwatervalue);

        textBrowser_Addreagent_4 = new QTextBrowser(display_channel_4);
        textBrowser_Addreagent_4->setObjectName(QStringLiteral("textBrowser_Addreagent_4"));

        horizontalLayout_5->addWidget(textBrowser_Addreagent_4);

        tabWidget->addTab(display_channel_4, QString());
        display_channel_5 = new QWidget();
        display_channel_5->setObjectName(QStringLiteral("display_channel_5"));
        horizontalLayout_6 = new QHBoxLayout(display_channel_5);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        textBrowser_Channel5_tubevalue = new QTextBrowser(display_channel_5);
        textBrowser_Channel5_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel5_tubevalue"));

        horizontalLayout_6->addWidget(textBrowser_Channel5_tubevalue);

        textBrowser_Channel_5_addwatervalue = new QTextBrowser(display_channel_5);
        textBrowser_Channel_5_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_5_addwatervalue"));

        horizontalLayout_6->addWidget(textBrowser_Channel_5_addwatervalue);

        textBrowser_Addreagent_5 = new QTextBrowser(display_channel_5);
        textBrowser_Addreagent_5->setObjectName(QStringLiteral("textBrowser_Addreagent_5"));

        horizontalLayout_6->addWidget(textBrowser_Addreagent_5);

        tabWidget->addTab(display_channel_5, QString());
        display_channel_6 = new QWidget();
        display_channel_6->setObjectName(QStringLiteral("display_channel_6"));
        horizontalLayout_7 = new QHBoxLayout(display_channel_6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        textBrowser_Channel6_tubevalue = new QTextBrowser(display_channel_6);
        textBrowser_Channel6_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel6_tubevalue"));

        horizontalLayout_7->addWidget(textBrowser_Channel6_tubevalue);

        textBrowser_Channel_6_addwatervalue = new QTextBrowser(display_channel_6);
        textBrowser_Channel_6_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_6_addwatervalue"));

        horizontalLayout_7->addWidget(textBrowser_Channel_6_addwatervalue);

        textBrowser_Addreagent_6 = new QTextBrowser(display_channel_6);
        textBrowser_Addreagent_6->setObjectName(QStringLiteral("textBrowser_Addreagent_6"));

        horizontalLayout_7->addWidget(textBrowser_Addreagent_6);

        tabWidget->addTab(display_channel_6, QString());
        display_channel_7 = new QWidget();
        display_channel_7->setObjectName(QStringLiteral("display_channel_7"));
        horizontalLayout_8 = new QHBoxLayout(display_channel_7);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        textBrowser_Channel7_tubevalue = new QTextBrowser(display_channel_7);
        textBrowser_Channel7_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel7_tubevalue"));

        horizontalLayout_8->addWidget(textBrowser_Channel7_tubevalue);

        textBrowser_Channel_7_addwatervalue = new QTextBrowser(display_channel_7);
        textBrowser_Channel_7_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_7_addwatervalue"));

        horizontalLayout_8->addWidget(textBrowser_Channel_7_addwatervalue);

        textBrowser_Addreagent_7 = new QTextBrowser(display_channel_7);
        textBrowser_Addreagent_7->setObjectName(QStringLiteral("textBrowser_Addreagent_7"));

        horizontalLayout_8->addWidget(textBrowser_Addreagent_7);

        tabWidget->addTab(display_channel_7, QString());
        display_channel_8 = new QWidget();
        display_channel_8->setObjectName(QStringLiteral("display_channel_8"));
        horizontalLayout_9 = new QHBoxLayout(display_channel_8);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        textBrowser_Channel8_tubevalue = new QTextBrowser(display_channel_8);
        textBrowser_Channel8_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel8_tubevalue"));

        horizontalLayout_9->addWidget(textBrowser_Channel8_tubevalue);

        textBrowser_Channel_8_addwatervalue = new QTextBrowser(display_channel_8);
        textBrowser_Channel_8_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_8_addwatervalue"));

        horizontalLayout_9->addWidget(textBrowser_Channel_8_addwatervalue);

        textBrowser_Addreagent_8 = new QTextBrowser(display_channel_8);
        textBrowser_Addreagent_8->setObjectName(QStringLiteral("textBrowser_Addreagent_8"));

        horizontalLayout_9->addWidget(textBrowser_Addreagent_8);

        tabWidget->addTab(display_channel_8, QString());
        display_channel_9 = new QWidget();
        display_channel_9->setObjectName(QStringLiteral("display_channel_9"));
        horizontalLayout_10 = new QHBoxLayout(display_channel_9);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        textBrowser_Channel9_tubevalue = new QTextBrowser(display_channel_9);
        textBrowser_Channel9_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel9_tubevalue"));

        horizontalLayout_10->addWidget(textBrowser_Channel9_tubevalue);

        textBrowser_Channel_9_addwatervalue = new QTextBrowser(display_channel_9);
        textBrowser_Channel_9_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_9_addwatervalue"));

        horizontalLayout_10->addWidget(textBrowser_Channel_9_addwatervalue);

        textBrowser_Addreagent_9 = new QTextBrowser(display_channel_9);
        textBrowser_Addreagent_9->setObjectName(QStringLiteral("textBrowser_Addreagent_9"));

        horizontalLayout_10->addWidget(textBrowser_Addreagent_9);

        tabWidget->addTab(display_channel_9, QString());
        display_channel_10 = new QWidget();
        display_channel_10->setObjectName(QStringLiteral("display_channel_10"));
        horizontalLayout_11 = new QHBoxLayout(display_channel_10);
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        textBrowser_Channel10_tubevalue = new QTextBrowser(display_channel_10);
        textBrowser_Channel10_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel10_tubevalue"));

        horizontalLayout_11->addWidget(textBrowser_Channel10_tubevalue);

        textBrowser_Channel_10_addwatervalue = new QTextBrowser(display_channel_10);
        textBrowser_Channel_10_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_10_addwatervalue"));

        horizontalLayout_11->addWidget(textBrowser_Channel_10_addwatervalue);

        textBrowser_Addreagent_10 = new QTextBrowser(display_channel_10);
        textBrowser_Addreagent_10->setObjectName(QStringLiteral("textBrowser_Addreagent_10"));

        horizontalLayout_11->addWidget(textBrowser_Addreagent_10);

        tabWidget->addTab(display_channel_10, QString());
        display_channel_11 = new QWidget();
        display_channel_11->setObjectName(QStringLiteral("display_channel_11"));
        horizontalLayout_12 = new QHBoxLayout(display_channel_11);
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        textBrowser_Channel11_tubevalue = new QTextBrowser(display_channel_11);
        textBrowser_Channel11_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel11_tubevalue"));

        horizontalLayout_12->addWidget(textBrowser_Channel11_tubevalue);

        textBrowser_Channel_11_addwatervalue = new QTextBrowser(display_channel_11);
        textBrowser_Channel_11_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_11_addwatervalue"));

        horizontalLayout_12->addWidget(textBrowser_Channel_11_addwatervalue);

        textBrowser_Addreagent_11 = new QTextBrowser(display_channel_11);
        textBrowser_Addreagent_11->setObjectName(QStringLiteral("textBrowser_Addreagent_11"));

        horizontalLayout_12->addWidget(textBrowser_Addreagent_11);

        tabWidget->addTab(display_channel_11, QString());
        display_channel_12 = new QWidget();
        display_channel_12->setObjectName(QStringLiteral("display_channel_12"));
        horizontalLayout_13 = new QHBoxLayout(display_channel_12);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        textBrowser_Channel12_tubevalue = new QTextBrowser(display_channel_12);
        textBrowser_Channel12_tubevalue->setObjectName(QStringLiteral("textBrowser_Channel12_tubevalue"));

        horizontalLayout_13->addWidget(textBrowser_Channel12_tubevalue);

        textBrowser_Channel_12_addwatervalue = new QTextBrowser(display_channel_12);
        textBrowser_Channel_12_addwatervalue->setObjectName(QStringLiteral("textBrowser_Channel_12_addwatervalue"));

        horizontalLayout_13->addWidget(textBrowser_Channel_12_addwatervalue);

        textBrowser_Addreagent_12 = new QTextBrowser(display_channel_12);
        textBrowser_Addreagent_12->setObjectName(QStringLiteral("textBrowser_Addreagent_12"));

        horizontalLayout_13->addWidget(textBrowser_Addreagent_12);

        tabWidget->addTab(display_channel_12, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(CalibrationPosition);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 30));
        label->setMaximumSize(QSize(16777215, 30));
        label->setFont(font);

        horizontalLayout->addWidget(label);

        spinBox_tubenum = new QSpinBox(CalibrationPosition);
        spinBox_tubenum->setObjectName(QStringLiteral("spinBox_tubenum"));
        spinBox_tubenum->setMinimumSize(QSize(120, 30));
        spinBox_tubenum->setMaximumSize(QSize(16777215, 30));
        spinBox_tubenum->setFont(font);
        spinBox_tubenum->setMaximum(25);

        horizontalLayout->addWidget(spinBox_tubenum);

        moveTube2Channel = new QToolButton(CalibrationPosition);
        moveTube2Channel->setObjectName(QStringLiteral("moveTube2Channel"));
        moveTube2Channel->setMinimumSize(QSize(120, 30));
        moveTube2Channel->setMaximumSize(QSize(16777215, 30));
        moveTube2Channel->setFont(font);

        horizontalLayout->addWidget(moveTube2Channel);

        Addwater = new QToolButton(CalibrationPosition);
        Addwater->setObjectName(QStringLiteral("Addwater"));
        Addwater->setMinimumSize(QSize(120, 30));
        Addwater->setMaximumSize(QSize(16777215, 30));
        Addwater->setFont(font);

        horizontalLayout->addWidget(Addwater);

        toolButton_addreagent = new QToolButton(CalibrationPosition);
        toolButton_addreagent->setObjectName(QStringLiteral("toolButton_addreagent"));
        toolButton_addreagent->setMinimumSize(QSize(120, 30));
        toolButton_addreagent->setFont(font);

        horizontalLayout->addWidget(toolButton_addreagent);

        comboBox_Throw = new QComboBox(CalibrationPosition);
        comboBox_Throw->setObjectName(QStringLiteral("comboBox_Throw"));
        comboBox_Throw->setMinimumSize(QSize(120, 30));
        comboBox_Throw->setFont(font);

        horizontalLayout->addWidget(comboBox_Throw);

        toolButton_back = new QToolButton(CalibrationPosition);
        toolButton_back->setObjectName(QStringLiteral("toolButton_back"));
        toolButton_back->setMinimumSize(QSize(120, 30));
        toolButton_back->setFont(font);

        horizontalLayout->addWidget(toolButton_back);

        comboBox_Pause = new QComboBox(CalibrationPosition);
        comboBox_Pause->setObjectName(QStringLiteral("comboBox_Pause"));
        comboBox_Pause->setMinimumSize(QSize(100, 30));
        comboBox_Pause->setFont(font);

        horizontalLayout->addWidget(comboBox_Pause);

        label_2 = new QLabel(CalibrationPosition);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setMinimumSize(QSize(80, 30));
        label_2->setFont(font);

        horizontalLayout->addWidget(label_2);

        spinBox_Absorbul = new QSpinBox(CalibrationPosition);
        spinBox_Absorbul->setObjectName(QStringLiteral("spinBox_Absorbul"));
        spinBox_Absorbul->setMinimumSize(QSize(100, 30));
        spinBox_Absorbul->setFont(font);
        spinBox_Absorbul->setMinimum(20);
        spinBox_Absorbul->setMaximum(260);

        horizontalLayout->addWidget(spinBox_Absorbul);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(CalibrationPosition);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CalibrationPosition);
    } // setupUi

    void retranslateUi(QWidget *CalibrationPosition)
    {
        CalibrationPosition->setWindowTitle(QApplication::translate("CalibrationPosition", "Form", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_1), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(1)", nullptr));
        label_3->setText(QApplication::translate("CalibrationPosition", "\347\251\272\346\235\257\345\200\274", nullptr));
        label_4->setText(QApplication::translate("CalibrationPosition", "\346\260\264\346\240\267\345\200\274", nullptr));
        label_5->setText(QApplication::translate("CalibrationPosition", "\346\265\212\345\272\246\346\266\262", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_2), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(2)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_3), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(3)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_4), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(4)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_5), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(5)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_6), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(6)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_7), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(7)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_8), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(8)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_9), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(9)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_10), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(10)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_11), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(11)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(display_channel_12), QApplication::translate("CalibrationPosition", "\351\200\232\351\201\223(12)", nullptr));
        label->setText(QApplication::translate("CalibrationPosition", "\344\273\216\350\257\225\347\256\241\345\255\224:", nullptr));
        moveTube2Channel->setText(QApplication::translate("CalibrationPosition", "\346\212\223\345\217\226", nullptr));
        Addwater->setText(QApplication::translate("CalibrationPosition", "\345\212\240\346\260\264", nullptr));
        toolButton_addreagent->setText(QApplication::translate("CalibrationPosition", "\345\212\240\350\257\225\345\211\202", nullptr));
        toolButton_back->setText(QApplication::translate("CalibrationPosition", "Cleanning", nullptr));
        label_2->setText(QApplication::translate("CalibrationPosition", "\345\220\270\346\260\264\351\207\217", nullptr));
        spinBox_Absorbul->setSuffix(QApplication::translate("CalibrationPosition", "ul", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CalibrationPosition: public Ui_CalibrationPosition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATIONPOSITION_H
