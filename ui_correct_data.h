/********************************************************************************
** Form generated from reading UI file 'correct_data.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CORRECT_DATA_H
#define UI_CORRECT_DATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Correct_Data
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_text;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QLabel *label_Text;
    QLineEdit *lineEdit_changetext;
    QLabel *Info_error;
    QSpacerItem *horizontalSpacer_2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *toolButton_Retest;
    QToolButton *toolButton_Ok;
    QToolButton *toolButton_Cancel;

    void setupUi(QWidget *Correct_Data)
    {
        if (Correct_Data->objectName().isEmpty())
            Correct_Data->setObjectName(QStringLiteral("Correct_Data"));
        Correct_Data->setWindowModality(Qt::ApplicationModal);
        Correct_Data->resize(400, 198);
        Correct_Data->setMaximumSize(QSize(400, 200));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(14);
        Correct_Data->setFont(font);
        Correct_Data->setFocusPolicy(Qt::ClickFocus);
        verticalLayout = new QVBoxLayout(Correct_Data);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_text = new QWidget(Correct_Data);
        widget_text->setObjectName(QStringLiteral("widget_text"));
        horizontalLayout = new QHBoxLayout(widget_text);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_Text = new QLabel(widget_text);
        label_Text->setObjectName(QStringLiteral("label_Text"));
        label_Text->setMinimumSize(QSize(0, 30));
        label_Text->setMaximumSize(QSize(16777215, 40));

        gridLayout->addWidget(label_Text, 0, 0, 1, 1);

        lineEdit_changetext = new QLineEdit(widget_text);
        lineEdit_changetext->setObjectName(QStringLiteral("lineEdit_changetext"));
        lineEdit_changetext->setMinimumSize(QSize(120, 30));
        lineEdit_changetext->setMaximumSize(QSize(100, 40));

        gridLayout->addWidget(lineEdit_changetext, 0, 1, 1, 1);

        Info_error = new QLabel(widget_text);
        Info_error->setObjectName(QStringLiteral("Info_error"));
        Info_error->setMaximumSize(QSize(16777215, 40));
        Info_error->setStyleSheet(QStringLiteral("color: rgb(255, 0, 0);"));

        gridLayout->addWidget(Info_error, 1, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_text);

        widget = new QWidget(Correct_Data);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setStyleSheet(QLatin1String("QWidget#widget{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(205, 200, 205);  \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"} "));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        toolButton_Retest = new QToolButton(widget);
        toolButton_Retest->setObjectName(QStringLiteral("toolButton_Retest"));
        toolButton_Retest->setMaximumSize(QSize(100, 50));
        QFont font1;
        font1.setFamily(QStringLiteral("Microsoft YaHei"));
        toolButton_Retest->setFont(font1);
        toolButton_Retest->setStyleSheet(QLatin1String("QToolButton#toolButton_Retest{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_Retest:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_Retest:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  "));

        horizontalLayout_2->addWidget(toolButton_Retest);

        toolButton_Ok = new QToolButton(widget);
        toolButton_Ok->setObjectName(QStringLiteral("toolButton_Ok"));
        toolButton_Ok->setMaximumSize(QSize(100, 50));
        toolButton_Ok->setFont(font1);
        toolButton_Ok->setStyleSheet(QLatin1String("QToolButton#toolButton_Ok{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_Ok:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_Ok:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  "));

        horizontalLayout_2->addWidget(toolButton_Ok);

        toolButton_Cancel = new QToolButton(widget);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMaximumSize(QSize(100, 50));
        toolButton_Cancel->setFont(font1);
        toolButton_Cancel->setStyleSheet(QLatin1String("QToolButton#toolButton_Cancel{  \n"
"    border: 1px solid  #C0C0C0;  \n"
"    background-color:#606060;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_Cancel:hover{     \n"
"     border: 1px solid #C0C0C0;  \n"
"    background-color:#161616;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  \n"
"QToolButton#toolButton_Cancel:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 0px;  \n"
"	font-family:'Microsoft YaHei';\n"
"	font-size:14px;\n"
"	color:white;\n"
"}  "));

        horizontalLayout_2->addWidget(toolButton_Cancel);


        verticalLayout->addWidget(widget);


        retranslateUi(Correct_Data);

        QMetaObject::connectSlotsByName(Correct_Data);
    } // setupUi

    void retranslateUi(QWidget *Correct_Data)
    {
        Correct_Data->setWindowTitle(QString());
        label_Text->setText(QApplication::translate("Correct_Data", "TextLabel", nullptr));
        Info_error->setText(QString());
        toolButton_Retest->setText(QApplication::translate("Correct_Data", "\351\207\215\346\265\213", nullptr));
        toolButton_Ok->setText(QApplication::translate("Correct_Data", "\347\241\256\345\256\232", nullptr));
        toolButton_Cancel->setText(QApplication::translate("Correct_Data", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Correct_Data: public Ui_Correct_Data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CORRECT_DATA_H
