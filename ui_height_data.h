/********************************************************************************
** Form generated from reading UI file 'height_data.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HEIGHT_DATA_H
#define UI_HEIGHT_DATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Height_Data
{
public:
    QHBoxLayout *horizontalLayout_3;
    QWidget *widget_Task;
    QVBoxLayout *verticalLayout;
    QTableWidget *Sample_Data_tablewidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_TestHeighModel;
    QCheckBox *checkBox_wholeblood;
    QCheckBox *checkBox_plasma;
    QSpacerItem *horizontalSpacer_5;
    QToolButton *toolButton_Delete_sel_item;
    QToolButton *AddTask;
    QToolButton *toolButtonbatchAddTask;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_ok;
    QToolButton *toolButton_Cancel;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widget_ShowErrImage;
    QHBoxLayout *horizontalLayout_2;
    QLabel *Imageshow;

    void setupUi(QWidget *Height_Data)
    {
        if (Height_Data->objectName().isEmpty())
            Height_Data->setObjectName(QStringLiteral("Height_Data"));
        Height_Data->setWindowModality(Qt::NonModal);
        Height_Data->resize(1480, 650);
        horizontalLayout_3 = new QHBoxLayout(Height_Data);
        horizontalLayout_3->setSpacing(10);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 5, 5, 5);
        widget_Task = new QWidget(Height_Data);
        widget_Task->setObjectName(QStringLiteral("widget_Task"));
        widget_Task->setMinimumSize(QSize(1100, 0));
        verticalLayout = new QVBoxLayout(widget_Task);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        Sample_Data_tablewidget = new QTableWidget(widget_Task);
        Sample_Data_tablewidget->setObjectName(QStringLiteral("Sample_Data_tablewidget"));
        Sample_Data_tablewidget->setMinimumSize(QSize(1050, 0));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(14);
        Sample_Data_tablewidget->setFont(font);

        verticalLayout->addWidget(Sample_Data_tablewidget);

        widget = new QWidget(widget_Task);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 100));
        widget->setFont(font);
        widget->setStyleSheet(QLatin1String("QWidget#widget{  \n"
"    border: 1px solid rgb(200,200,200);  \n"
"	background-color: rgb(188, 187, 186);  \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"}"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        widget_TestHeighModel = new QWidget(widget);
        widget_TestHeighModel->setObjectName(QStringLiteral("widget_TestHeighModel"));
        widget_TestHeighModel->setMinimumSize(QSize(140, 0));
        widget_TestHeighModel->setStyleSheet(QLatin1String("QWidget#widget_TestHeighModel{  \n"
"    border: 1px solid rgb(200,200,200);  \n"
"	background-color: rgb(188, 187, 186);  \n"
"    border-style: solid;  \n"
"    border-radius:1px;  \n"
"    padding:0 0px;  	\n"
"}"));
        checkBox_wholeblood = new QCheckBox(widget_TestHeighModel);
        checkBox_wholeblood->setObjectName(QStringLiteral("checkBox_wholeblood"));
        checkBox_wholeblood->setGeometry(QRect(10, 10, 121, 30));
        checkBox_wholeblood->setMinimumSize(QSize(0, 30));
        checkBox_wholeblood->setStyleSheet(QStringLiteral(""));
        checkBox_wholeblood->setAutoExclusive(true);
        checkBox_plasma = new QCheckBox(widget_TestHeighModel);
        checkBox_plasma->setObjectName(QStringLiteral("checkBox_plasma"));
        checkBox_plasma->setGeometry(QRect(10, 50, 121, 30));
        checkBox_plasma->setMinimumSize(QSize(0, 30));
        checkBox_plasma->setStyleSheet(QStringLiteral(""));
        checkBox_plasma->setAutoExclusive(true);

        horizontalLayout->addWidget(widget_TestHeighModel);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        toolButton_Delete_sel_item = new QToolButton(widget);
        toolButton_Delete_sel_item->setObjectName(QStringLiteral("toolButton_Delete_sel_item"));
        toolButton_Delete_sel_item->setMinimumSize(QSize(0, 50));
        toolButton_Delete_sel_item->setMaximumSize(QSize(170, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        toolButton_Delete_sel_item->setFont(font1);
        toolButton_Delete_sel_item->setStyleSheet(QStringLiteral(""));
        toolButton_Delete_sel_item->setIconSize(QSize(32, 32));
        toolButton_Delete_sel_item->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_Delete_sel_item);

        AddTask = new QToolButton(widget);
        AddTask->setObjectName(QStringLiteral("AddTask"));
        AddTask->setMinimumSize(QSize(0, 50));
        AddTask->setMaximumSize(QSize(170, 16777215));
        AddTask->setFont(font1);
        AddTask->setStyleSheet(QStringLiteral(""));
        AddTask->setIconSize(QSize(32, 32));
        AddTask->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(AddTask);

        toolButtonbatchAddTask = new QToolButton(widget);
        toolButtonbatchAddTask->setObjectName(QStringLiteral("toolButtonbatchAddTask"));
        toolButtonbatchAddTask->setMinimumSize(QSize(60, 50));
        toolButtonbatchAddTask->setMaximumSize(QSize(170, 16777215));

        horizontalLayout->addWidget(toolButtonbatchAddTask);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton_ok = new QToolButton(widget);
        toolButton_ok->setObjectName(QStringLiteral("toolButton_ok"));
        toolButton_ok->setMinimumSize(QSize(60, 50));
        toolButton_ok->setMaximumSize(QSize(170, 16777215));
        toolButton_ok->setFont(font1);
        toolButton_ok->setStyleSheet(QStringLiteral(""));
        toolButton_ok->setIconSize(QSize(32, 32));
        toolButton_ok->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_ok);

        toolButton_Cancel = new QToolButton(widget);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMinimumSize(QSize(60, 50));
        toolButton_Cancel->setMaximumSize(QSize(170, 16777215));
        toolButton_Cancel->setFont(font1);
        toolButton_Cancel->setStyleSheet(QStringLiteral(""));
        toolButton_Cancel->setIconSize(QSize(32, 32));
        toolButton_Cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_Cancel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(widget);


        horizontalLayout_3->addWidget(widget_Task);

        widget_ShowErrImage = new QWidget(Height_Data);
        widget_ShowErrImage->setObjectName(QStringLiteral("widget_ShowErrImage"));
        widget_ShowErrImage->setMinimumSize(QSize(360, 600));
        widget_ShowErrImage->setMaximumSize(QSize(16777215, 16777215));
        widget_ShowErrImage->setStyleSheet(QLatin1String("QWidget#widget_ShowErrImage{  \n"
"    border: 1px solid rgba(220,220,220,1);\n"
"	background-color: rgb(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"}\n"
"\n"
"\n"
""));
        horizontalLayout_2 = new QHBoxLayout(widget_ShowErrImage);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        Imageshow = new QLabel(widget_ShowErrImage);
        Imageshow->setObjectName(QStringLiteral("Imageshow"));
        Imageshow->setMinimumSize(QSize(340, 590));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font2.setPointSize(22);
        Imageshow->setFont(font2);
        Imageshow->setStyleSheet(QLatin1String("border: 1px solid rgba(220,220,220,1);\n"
"background-color: rgb(188,187,183); \n"
"border-style: solid;  \n"
"border-radius:0px;  \n"
"padding:0 0px; "));
        Imageshow->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(Imageshow);


        horizontalLayout_3->addWidget(widget_ShowErrImage);


        retranslateUi(Height_Data);

        QMetaObject::connectSlotsByName(Height_Data);
    } // setupUi

    void retranslateUi(QWidget *Height_Data)
    {
        Height_Data->setWindowTitle(QApplication::translate("Height_Data", "Form", nullptr));
        checkBox_wholeblood->setText(QApplication::translate("Height_Data", "\345\205\250\350\241\200\346\250\241\345\274\217", nullptr));
#ifndef QT_NO_TOOLTIP
        checkBox_plasma->setToolTip(QApplication::translate("Height_Data", "\351\273\230\350\256\244PRP\351\253\230\345\272\246\344\270\2720mm,\344\270\213\351\222\210\345\210\260\346\234\200\345\272\225\351\203\250", nullptr));
#endif // QT_NO_TOOLTIP
        checkBox_plasma->setText(QApplication::translate("Height_Data", "\350\241\200\346\265\206\346\250\241\345\274\217", nullptr));
        toolButton_Delete_sel_item->setText(QApplication::translate("Height_Data", "\345\210\240\351\231\244\351\200\211\344\270\255", nullptr));
        AddTask->setText(QApplication::translate("Height_Data", "\346\267\273\345\212\240\344\273\273\345\212\241", nullptr));
        toolButtonbatchAddTask->setText(QApplication::translate("Height_Data", "...", nullptr));
        toolButton_ok->setText(QApplication::translate("Height_Data", "\344\277\235\345\255\230", nullptr));
        toolButton_Cancel->setText(QApplication::translate("Height_Data", "\345\217\226\346\266\210", nullptr));
        Imageshow->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class Height_Data: public Ui_Height_Data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HEIGHT_DATA_H
