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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_CustomFixTableView
{
public:
    QGridLayout *gridLayout;
    QWidget *widgetShowInfo;
    QHBoxLayout *horizontalLayout_4;
    QSpacerItem *horizontalSpacer_4;
    QCustomPlot *widgetCurveShow;
    QSpacerItem *horizontalSpacer_5;
    QTableWidget *tableWidget;
    QWidget *widgetTop;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButtonBack;
    QSpacerItem *horizontalSpacer_2;
    QLabel *labelname;
    QSpacerItem *horizontalSpacer_3;
    QPushButton *pushButtonNext;
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
        CustomFixTableView->resize(1450, 800);
        CustomFixTableView->setMinimumSize(QSize(1450, 800));
        CustomFixTableView->setStyleSheet(QLatin1String("QWidget#CustomFixTableView {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #f8fafc, stop:1 #e2e8f0);\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 12px;\n"
"}\n"
"\n"
"QWidget {\n"
"    font-family: \"Segoe UI\", \"Microsoft YaHei\", sans-serif;\n"
"}"));
        gridLayout = new QGridLayout(CustomFixTableView);
        gridLayout->setSpacing(8);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(10, 10, 10, 10);
        widgetShowInfo = new QWidget(CustomFixTableView);
        widgetShowInfo->setObjectName(QStringLiteral("widgetShowInfo"));
        widgetShowInfo->setMinimumSize(QSize(0, 320));
        widgetShowInfo->setMaximumSize(QSize(16777215, 320));
        widgetShowInfo->setStyleSheet(QLatin1String("QWidget#widgetShowInfo {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #ffffff, stop:1 #f8fafc);\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    padding: 8px;\n"
"}"));
        horizontalLayout_4 = new QHBoxLayout(widgetShowInfo);
        horizontalLayout_4->setSpacing(10);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(8, 8, 8, 8);
        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        widgetCurveShow = new QCustomPlot(widgetShowInfo);
        widgetCurveShow->setObjectName(QStringLiteral("widgetCurveShow"));
        widgetCurveShow->setMinimumSize(QSize(800, 280));
        widgetCurveShow->setMaximumSize(QSize(800, 280));
        widgetCurveShow->setStyleSheet(QLatin1String("QCustomPlot {\n"
"    background: white;\n"
"    border: 1px solid #e2e8f0;\n"
"    border-radius: 6px;\n"
"    padding: 5px;\n"
"}"));

        horizontalLayout_4->addWidget(widgetCurveShow);

        horizontalSpacer_5 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);


        gridLayout->addWidget(widgetShowInfo, 2, 0, 1, 1);

        tableWidget = new QTableWidget(CustomFixTableView);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setMinimumSize(QSize(0, 280));
        tableWidget->setStyleSheet(QLatin1String("QTableWidget {\n"
"    background-color: white;\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 8px;\n"
"    gridline-color: #e2e8f0;\n"
"    selection-background-color: #3b82f6;\n"
"    selection-color: white;\n"
"    font-family: \"Segoe UI\", \"Microsoft YaHei\";\n"
"    font-size: 11px;\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 6px;\n"
"    border-bottom: 1px solid #f1f5f9;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background-color: #3b82f6;\n"
"    color: white;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    color: white;\n"
"    font-weight: bold;\n"
"    padding: 8px;\n"
"    border: 1px solid #1e3a8a;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QTableCornerButton::section {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    border: 1px solid #1e3a8a;\n"
"}"));

        gridLayout->addWidget(tableWidget, 1, 0, 1, 2);

        widgetTop = new QWidget(CustomFixTableView);
        widgetTop->setObjectName(QStringLiteral("widgetTop"));
        widgetTop->setMinimumSize(QSize(0, 40));
        widgetTop->setMaximumSize(QSize(16777215, 40));
        widgetTop->setStyleSheet(QLatin1String("QWidget#widgetTop {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    border: 1px solid #1e3a8a;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"}"));
        horizontalLayout_2 = new QHBoxLayout(widgetTop);
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(10, 5, 10, 5);
        pushButtonBack = new QPushButton(widgetTop);
        pushButtonBack->setObjectName(QStringLiteral("pushButtonBack"));
        pushButtonBack->setMinimumSize(QSize(40, 30));
        pushButtonBack->setStyleSheet(QLatin1String("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #3b82f6, stop:1 #2563eb);\n"
"    border: 1px solid #1e40af;\n"
"    border-radius: 6px;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #60a5fa, stop:1 #3b82f6);\n"
"    border: 1px solid #3b82f6;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    padding-left: 2px;\n"
"    padding-top: 2px;\n"
"}\n"
"\n"
"QPushButton:focus {\n"
"    outline: none;\n"
"}"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Picture/SetPng/upTable.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonBack->setIcon(icon);
        pushButtonBack->setIconSize(QSize(20, 20));

        horizontalLayout_2->addWidget(pushButtonBack);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        labelname = new QLabel(widgetTop);
        labelname->setObjectName(QStringLiteral("labelname"));
        labelname->setMinimumSize(QSize(0, 30));
        labelname->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        labelname->setFont(font);
        labelname->setStyleSheet(QLatin1String("QLabel {\n"
"    color: white;\n"
"    background: transparent;\n"
"    padding: 5px;\n"
"}"));
        labelname->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(labelname);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);

        pushButtonNext = new QPushButton(widgetTop);
        pushButtonNext->setObjectName(QStringLiteral("pushButtonNext"));
        pushButtonNext->setMinimumSize(QSize(40, 30));
        pushButtonNext->setStyleSheet(QLatin1String("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #3b82f6, stop:1 #2563eb);\n"
"    border: 1px solid #1e40af;\n"
"    border-radius: 6px;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #60a5fa, stop:1 #3b82f6);\n"
"    border: 1px solid #3b82f6;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    padding-left: 2px;\n"
"    padding-top: 2px;\n"
"}\n"
"\n"
"QPushButton:focus {\n"
"    outline: none;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Picture/SetPng/downTable.png"), QSize(), QIcon::Normal, QIcon::Off);
        pushButtonNext->setIcon(icon1);
        pushButtonNext->setIconSize(QSize(20, 20));

        horizontalLayout_2->addWidget(pushButtonNext);


        gridLayout->addWidget(widgetTop, 0, 0, 1, 2);

        labelbrief = new QLabel(CustomFixTableView);
        labelbrief->setObjectName(QStringLiteral("labelbrief"));
        labelbrief->setMinimumSize(QSize(280, 0));
        labelbrief->setMaximumSize(QSize(280, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(10);
        labelbrief->setFont(font1);
        labelbrief->setStyleSheet(QString::fromUtf8("QLabel#labelbrief {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    border: 2px solid #1e3a8a;\n"
"    border-radius: 8px;\n"
"    color: white;\n"
"    padding: 15px;\n"
"    font-family: \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 10px;\n"
"    line-height: 1.4;\n"
"}"));
        labelbrief->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        labelbrief->setWordWrap(true);

        gridLayout->addWidget(labelbrief, 2, 1, 1, 1);

        widgetbottom = new QWidget(CustomFixTableView);
        widgetbottom->setObjectName(QStringLiteral("widgetbottom"));
        widgetbottom->setMinimumSize(QSize(0, 35));
        widgetbottom->setMaximumSize(QSize(16777215, 35));
        widgetbottom->setStyleSheet(QLatin1String("QWidget#widgetbottom {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #f1f5f9, stop:1 #e2e8f0);\n"
"    border: 1px solid #cbd5e1;\n"
"    border-radius: 6px;\n"
"    padding: 5px;\n"
"}"));
        horizontalLayout = new QHBoxLayout(widgetbottom);
        horizontalLayout->setSpacing(15);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(10, 5, 10, 5);
        widgetViewtitle = new QWidget(widgetbottom);
        widgetViewtitle->setObjectName(QStringLiteral("widgetViewtitle"));
        widgetViewtitle->setMinimumSize(QSize(0, 25));
        horizontalLayout_3 = new QHBoxLayout(widgetViewtitle);
        horizontalLayout_3->setSpacing(8);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 0, 5, 0);
        labelAAColor = new QLabel(widgetViewtitle);
        labelAAColor->setObjectName(QStringLiteral("labelAAColor"));
        labelAAColor->setMinimumSize(QSize(15, 15));
        labelAAColor->setMaximumSize(QSize(15, 15));
        labelAAColor->setStyleSheet(QLatin1String("QLabel {\n"
"    background-color: rgb(86, 147, 198);\n"
"    border: 1px solid #475569;\n"
"    border-radius: 3px;\n"
"}"));

        horizontalLayout_3->addWidget(labelAAColor);

        labelAAcurve = new QLabel(widgetViewtitle);
        labelAAcurve->setObjectName(QStringLiteral("labelAAcurve"));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font2.setPointSize(11);
        font2.setBold(true);
        font2.setWeight(75);
        labelAAcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelAAcurve);

        labelADPColor = new QLabel(widgetViewtitle);
        labelADPColor->setObjectName(QStringLiteral("labelADPColor"));
        labelADPColor->setMinimumSize(QSize(15, 15));
        labelADPColor->setMaximumSize(QSize(15, 15));
        labelADPColor->setStyleSheet(QLatin1String("QLabel {\n"
"    background-color: rgb(114, 106, 176);\n"
"    border: 1px solid #475569;\n"
"    border-radius: 3px;\n"
"}"));

        horizontalLayout_3->addWidget(labelADPColor);

        labelADpcurve = new QLabel(widgetViewtitle);
        labelADpcurve->setObjectName(QStringLiteral("labelADpcurve"));
        labelADpcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelADpcurve);

        labelEPIColor = new QLabel(widgetViewtitle);
        labelEPIColor->setObjectName(QStringLiteral("labelEPIColor"));
        labelEPIColor->setMinimumSize(QSize(15, 15));
        labelEPIColor->setMaximumSize(QSize(15, 15));
        labelEPIColor->setStyleSheet(QLatin1String("QLabel {\n"
"    background-color: rgb(234, 42, 42);\n"
"    border: 1px solid #475569;\n"
"    border-radius: 3px;\n"
"}"));

        horizontalLayout_3->addWidget(labelEPIColor);

        labelEPIcurve = new QLabel(widgetViewtitle);
        labelEPIcurve->setObjectName(QStringLiteral("labelEPIcurve"));
        labelEPIcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelEPIcurve);

        labelcolColor = new QLabel(widgetViewtitle);
        labelcolColor->setObjectName(QStringLiteral("labelcolColor"));
        labelcolColor->setMinimumSize(QSize(15, 15));
        labelcolColor->setMaximumSize(QSize(15, 15));
        labelcolColor->setStyleSheet(QLatin1String("QLabel {\n"
"    background-color: rgb(147, 153, 156);\n"
"    border: 1px solid #475569;\n"
"    border-radius: 3px;\n"
"}"));

        horizontalLayout_3->addWidget(labelcolColor);

        labelcolcurve = new QLabel(widgetViewtitle);
        labelcolcurve->setObjectName(QStringLiteral("labelcolcurve"));
        labelcolcurve->setFont(font2);

        horizontalLayout_3->addWidget(labelcolcurve);

        labelrisColor = new QLabel(widgetViewtitle);
        labelrisColor->setObjectName(QStringLiteral("labelrisColor"));
        labelrisColor->setMinimumSize(QSize(15, 15));
        labelrisColor->setMaximumSize(QSize(15, 15));
        labelrisColor->setStyleSheet(QLatin1String("QLabel {\n"
"    background-color: rgb(188, 143, 143);\n"
"    border: 1px solid #475569;\n"
"    border-radius: 3px;\n"
"}"));

        horizontalLayout_3->addWidget(labelrisColor);

        labelriscurve = new QLabel(widgetViewtitle);
        labelriscurve->setObjectName(QStringLiteral("labelriscurve"));
        labelriscurve->setFont(font2);

        horizontalLayout_3->addWidget(labelriscurve);


        horizontalLayout->addWidget(widgetViewtitle);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        checkBoxSmooth = new QCheckBox(widgetbottom);
        checkBoxSmooth->setObjectName(QStringLiteral("checkBoxSmooth"));
        checkBoxSmooth->setMinimumSize(QSize(120, 25));
        checkBoxSmooth->setMaximumSize(QSize(120, 25));
        checkBoxSmooth->setFont(font2);
        checkBoxSmooth->setStyleSheet(QLatin1String("QCheckBox {\n"
"    spacing: 5px;\n"
"    color: #1e293b;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 16px;\n"
"    height: 16px;\n"
"    border: 2px solid #94a3b8;\n"
"    border-radius: 3px;\n"
"    background: white;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #3b82f6, stop:1 #1e40af);\n"
"    border: 2px solid #1e40af;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"}"));

        horizontalLayout->addWidget(checkBoxSmooth);


        gridLayout->addWidget(widgetbottom, 3, 0, 1, 2);


        retranslateUi(CustomFixTableView);

        QMetaObject::connectSlotsByName(CustomFixTableView);
    } // setupUi

    void retranslateUi(QWidget *CustomFixTableView)
    {
        CustomFixTableView->setWindowTitle(QApplication::translate("CustomFixTableView", "\350\241\200\345\260\217\346\235\277\350\201\232\351\233\206\345\210\206\346\236\220\347\263\273\347\273\237", nullptr));
        pushButtonBack->setText(QApplication::translate("CustomFixTableView", "\344\270\212\344\270\200\346\235\241", nullptr));
        labelname->setText(QApplication::translate("CustomFixTableView", "\350\241\200\345\260\217\346\235\277\350\201\232\351\233\206\345\210\206\346\236\220\347\273\223\346\236\234", nullptr));
        pushButtonNext->setText(QApplication::translate("CustomFixTableView", "\344\270\213\344\270\200\346\235\241", nullptr));
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
        checkBoxSmooth->setText(QApplication::translate("CustomFixTableView", "\346\240\241\345\271\263\346\233\262\347\272\277", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomFixTableView: public Ui_CustomFixTableView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMFIXTABLEVIEW_H
