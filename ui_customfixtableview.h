/********************************************************************************
** Form generated from reading UI file 'customfixtableview.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMFIXTABLEVIEW_H
#define UI_CUSTOMFIXTABLEVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_CustomFixTableView
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widgetTop;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonBack;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelname;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButtonNext;
    QTableWidget *tableWidget;
    QWidget *widgetShowInfo;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_4;
    QCustomPlot *widgetCurveShow;
    QSpacerItem *horizontalSpacer_5;
    QLabel *labelbrief;
    QWidget *widgetbottom;
    QHBoxLayout *horizontalLayout;
    QWidget *widgetViewtitle;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelAAColor;
    QLabel *labelAAcurve;
    QLabel *labelADPColor;
    QLabel *labelADpcurve;
    QLabel *labelEPIColor;
    QLabel *labelEPIcurve;
    QLabel *labelcolColor;
    QLabel *labelcolcurve;
    QLabel *labelrisColor;
    QLabel *labelriscurve;
    QSpacerItem *horizontalSpacer;
    QCheckBox *checkBoxSmooth;

    void setupUi(QWidget *CustomFixTableView)
    {
        if (CustomFixTableView->objectName().isEmpty())
            CustomFixTableView->setObjectName(QStringLiteral("CustomFixTableView"));
        CustomFixTableView->resize(1141, 680);
        CustomFixTableView->setMinimumSize(QSize(400, 0));
        verticalLayout = new QVBoxLayout(CustomFixTableView);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        widgetTop = new QWidget(CustomFixTableView);
        widgetTop->setObjectName(QStringLiteral("widgetTop"));
        widgetTop->setMinimumSize(QSize(0, 30));
        horizontalLayout_2 = new QHBoxLayout(widgetTop);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 0, 5, 0);
        pushButtonBack = new QPushButton(widgetTop);
        pushButtonBack->setObjectName(QStringLiteral("pushButtonBack"));
        pushButtonBack->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-style: none; /* \346\210\226 border-width: 0px; */\n"
"}\n"
"/* \346\202\254\345\201\234\347\212\266\346\200\201\357\274\232\346\233\264\346\215\242\345\233\276\347\211\207 */\n"
"QPushButton:hover {\n"
"    padding-left: 4px;\n"
"    padding-top: 4px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    padding-left: 5px;\n"
"    padding-top: 6px;\n"
"}\n"
"QPushButton:focus {\n"
"    outline: none;\n"
"    margin: 0px;\n"
"}"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Picture/SetPng/upTable.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonBack->setIcon(icon);
        pushButtonBack->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButtonBack);

        horizontalSpacer_2 = new QSpacerItem(293, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        labelname = new QLabel(widgetTop);
        labelname->setObjectName(QStringLiteral("labelname"));
        labelname->setMinimumSize(QSize(0, 20));
        labelname->setMaximumSize(QSize(16777215, 25));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(14);
        labelname->setFont(font);
        labelname->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(labelname);

        horizontalSpacer_3 = new QSpacerItem(293, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        pushButtonNext = new QPushButton(widgetTop);
        pushButtonNext->setObjectName(QStringLiteral("pushButtonNext"));
        pushButtonNext->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"    border-style: none; /* \346\210\226 border-width: 0px; */\n"
"}\n"
"/* \346\202\254\345\201\234\347\212\266\346\200\201\357\274\232\346\233\264\346\215\242\345\233\276\347\211\207 */\n"
"QPushButton:hover {\n"
"    padding-left: 4px;\n"
"    padding-top: 4px;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    padding-left: 5px;\n"
"    padding-top: 6px;\n"
"}\n"
"QPushButton:focus {\n"
"    outline: none;\n"
"    margin: 0px;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Picture/SetPng/downTable.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonNext->setIcon(icon1);
        pushButtonNext->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(pushButtonNext);


        verticalLayout->addWidget(widgetTop);

        tableWidget = new QTableWidget(CustomFixTableView);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setMinimumSize(QSize(0, 260));
        tableWidget->setStyleSheet(QStringLiteral(""));

        verticalLayout->addWidget(tableWidget);

        widgetShowInfo = new QWidget(CustomFixTableView);
        widgetShowInfo->setObjectName(QStringLiteral("widgetShowInfo"));
        widgetShowInfo->setMinimumSize(QSize(0, 300));
        widgetShowInfo->setStyleSheet(QLatin1String("background-color: #1e3a5f;\n"
"border: 2px solid #2d5278;\n"
"border-radius: 8px;\n"
"color: #ffffff;\n"
"font-family: \"Segoe UI\", \"Microsoft YaHei\";\n"
""));
        horizontalLayout_4 = new QHBoxLayout(widgetShowInfo);
        horizontalLayout_4->setSpacing(20);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(50, 5, 50, 5);
        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        widgetCurveShow = new QCustomPlot(widgetShowInfo);
        widgetCurveShow->setObjectName(QStringLiteral("widgetCurveShow"));
        widgetCurveShow->setMinimumSize(QSize(700, 260));
        widgetCurveShow->setMaximumSize(QSize(16777215, 11111));

        horizontalLayout_4->addWidget(widgetCurveShow);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        labelbrief = new QLabel(widgetShowInfo);
        labelbrief->setObjectName(QStringLiteral("labelbrief"));
        labelbrief->setMinimumSize(QSize(200, 0));
        labelbrief->setMaximumSize(QSize(210, 16777215));
        QFont font1;
        font1.setFamily(QStringLiteral("Segoe UI,Microsoft YaHei"));
        labelbrief->setFont(font1);
        labelbrief->setAlignment(Qt::AlignCenter);

        horizontalLayout_4->addWidget(labelbrief);


        verticalLayout->addWidget(widgetShowInfo);

        widgetbottom = new QWidget(CustomFixTableView);
        widgetbottom->setObjectName(QStringLiteral("widgetbottom"));
        widgetbottom->setMinimumSize(QSize(0, 25));
        widgetbottom->setMaximumSize(QSize(16777215, 25));
        horizontalLayout = new QHBoxLayout(widgetbottom);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        widgetViewtitle = new QWidget(widgetbottom);
        widgetViewtitle->setObjectName(QStringLiteral("widgetViewtitle"));
        widgetViewtitle->setMinimumSize(QSize(0, 15));
        horizontalLayout_3 = new QHBoxLayout(widgetViewtitle);
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        labelAAColor = new QLabel(widgetViewtitle);
        labelAAColor->setObjectName(QStringLiteral("labelAAColor"));
        labelAAColor->setMinimumSize(QSize(20, 0));
        labelAAColor->setMaximumSize(QSize(60, 16777215));
        labelAAColor->setStyleSheet(QStringLiteral("background-color: rgb(86, 147, 198);"));

        horizontalLayout_3->addWidget(labelAAColor);

        labelAAcurve = new QLabel(widgetViewtitle);
        labelAAcurve->setObjectName(QStringLiteral("labelAAcurve"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font2.setPointSize(12);
        labelAAcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelAAcurve);

        labelADPColor = new QLabel(widgetViewtitle);
        labelADPColor->setObjectName(QStringLiteral("labelADPColor"));
        labelADPColor->setMinimumSize(QSize(20, 0));
        labelADPColor->setMaximumSize(QSize(60, 16777215));
        labelADPColor->setStyleSheet(QStringLiteral("background-color: rgb(114, 106, 176);"));

        horizontalLayout_3->addWidget(labelADPColor);

        labelADpcurve = new QLabel(widgetViewtitle);
        labelADpcurve->setObjectName(QStringLiteral("labelADpcurve"));
        labelADpcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelADpcurve);

        labelEPIColor = new QLabel(widgetViewtitle);
        labelEPIColor->setObjectName(QStringLiteral("labelEPIColor"));
        labelEPIColor->setMinimumSize(QSize(20, 0));
        labelEPIColor->setMaximumSize(QSize(60, 16777215));
        labelEPIColor->setStyleSheet(QStringLiteral("background-color: rgb(234, 42, 42);"));

        horizontalLayout_3->addWidget(labelEPIColor);

        labelEPIcurve = new QLabel(widgetViewtitle);
        labelEPIcurve->setObjectName(QStringLiteral("labelEPIcurve"));
        labelEPIcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelEPIcurve);

        labelcolColor = new QLabel(widgetViewtitle);
        labelcolColor->setObjectName(QStringLiteral("labelcolColor"));
        labelcolColor->setMinimumSize(QSize(20, 0));
        labelcolColor->setMaximumSize(QSize(60, 16777215));
        labelcolColor->setStyleSheet(QStringLiteral("background-color: rgb(147, 153, 156);"));

        horizontalLayout_3->addWidget(labelcolColor);

        labelcolcurve = new QLabel(widgetViewtitle);
        labelcolcurve->setObjectName(QStringLiteral("labelcolcurve"));
        labelcolcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelcolcurve);

        labelrisColor = new QLabel(widgetViewtitle);
        labelrisColor->setObjectName(QStringLiteral("labelrisColor"));
        labelrisColor->setMinimumSize(QSize(20, 0));
        labelrisColor->setMaximumSize(QSize(60, 16777215));
        labelrisColor->setStyleSheet(QStringLiteral("background-color: rgb(188, 143, 143);"));

        horizontalLayout_3->addWidget(labelrisColor);

        labelriscurve = new QLabel(widgetViewtitle);
        labelriscurve->setObjectName(QStringLiteral("labelriscurve"));
        labelriscurve->setFont(font2);

        horizontalLayout_3->addWidget(labelriscurve);


        horizontalLayout->addWidget(widgetViewtitle);

        horizontalSpacer = new QSpacerItem(710, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        checkBoxSmooth = new QCheckBox(widgetbottom);
        checkBoxSmooth->setObjectName(QStringLiteral("checkBoxSmooth"));
        checkBoxSmooth->setMinimumSize(QSize(0, 25));
        checkBoxSmooth->setMaximumSize(QSize(16777215, 25));
        checkBoxSmooth->setFont(font2);

        horizontalLayout->addWidget(checkBoxSmooth);


        verticalLayout->addWidget(widgetbottom);


        retranslateUi(CustomFixTableView);

        QMetaObject::connectSlotsByName(CustomFixTableView);
    } // setupUi

    void retranslateUi(QWidget *CustomFixTableView)
    {
        CustomFixTableView->setWindowTitle(QApplication::translate("CustomFixTableView", "Form", nullptr));
        pushButtonBack->setText(QString());
        labelname->setText(QApplication::translate("CustomFixTableView", "TextLabel", nullptr));
        pushButtonNext->setText(QString());
        labelbrief->setText(QApplication::translate("CustomFixTableView", "\345\276\205\345\210\206\346\236\220", nullptr));
        labelAAColor->setText(QString());
        labelAAcurve->setText(QApplication::translate("CustomFixTableView", "AA", nullptr));
        labelADPColor->setText(QString());
        labelADpcurve->setText(QApplication::translate("CustomFixTableView", "ADP", nullptr));
        labelEPIColor->setText(QString());
        labelEPIcurve->setText(QApplication::translate("CustomFixTableView", "EPI", nullptr));
        labelcolColor->setText(QString());
        labelcolcurve->setText(QApplication::translate("CustomFixTableView", "COL", nullptr));
        labelrisColor->setText(QString());
        labelriscurve->setText(QApplication::translate("CustomFixTableView", "RIS", nullptr));
        checkBoxSmooth->setText(QApplication::translate("CustomFixTableView", "CheckBox", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomFixTableView: public Ui_CustomFixTableView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMFIXTABLEVIEW_H
