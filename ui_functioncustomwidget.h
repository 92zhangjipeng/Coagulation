/********************************************************************************
** Form generated from reading UI file 'functioncustomwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTIONCUSTOMWIDGET_H
#define UI_FUNCTIONCUSTOMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FunctionCustomWidget
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout;
    QLabel *label_icon;
    QLabel *label_name;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_min;
    QToolButton *toolButton_max;
    QToolButton *toolButton_close;
    QWidget *widget_outtext;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_warmtext;
    QWidget *widget_button;
    QHBoxLayout *horizontalLayout_funcbtn;
    QToolButton *toolButton_func_1;
    QToolButton *toolButton_func_2;
    QToolButton *toolButton_func_3;

    void setupUi(QWidget *FunctionCustomWidget)
    {
        if (FunctionCustomWidget->objectName().isEmpty())
            FunctionCustomWidget->setObjectName(QStringLiteral("FunctionCustomWidget"));
        FunctionCustomWidget->resize(515, 202);
        FunctionCustomWidget->setStyleSheet(QLatin1String("border: 0px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); \n"
"border-style: solid;  \n"
"border-radius:1px;  \n"
"padding:0 0px;"));
        verticalLayout = new QVBoxLayout(FunctionCustomWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_title = new QWidget(FunctionCustomWidget);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMaximumSize(QSize(16777215, 32));
        widget_title->setStyleSheet(QLatin1String("background-color: rgba(198,197,193);\n"
"border-color: rgb(180, 180, 180);\n"
"border: 1 px solid rgba(198,197,193);"));
        horizontalLayout = new QHBoxLayout(widget_title);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_icon = new QLabel(widget_title);
        label_icon->setObjectName(QStringLiteral("label_icon"));
        label_icon->setMinimumSize(QSize(32, 32));
        label_icon->setMaximumSize(QSize(32, 32));
        label_icon->setStyleSheet(QStringLiteral("border-style: flat;background: transparent;"));

        horizontalLayout->addWidget(label_icon);

        label_name = new QLabel(widget_title);
        label_name->setObjectName(QStringLiteral("label_name"));
        label_name->setMinimumSize(QSize(120, 32));
        label_name->setMaximumSize(QSize(16777215, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(16);
        label_name->setFont(font);
        label_name->setStyleSheet(QLatin1String("border-style: flat;\n"
"background: transparent;"));

        horizontalLayout->addWidget(label_name);

        horizontalSpacer = new QSpacerItem(110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton_min = new QToolButton(widget_title);
        toolButton_min->setObjectName(QStringLiteral("toolButton_min"));
        toolButton_min->setMinimumSize(QSize(32, 32));
        toolButton_min->setMaximumSize(QSize(32, 32));
        toolButton_min->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\346\234\200\345\260\217\345\214\226.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_min->setIcon(icon);
        toolButton_min->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolButton_min);

        toolButton_max = new QToolButton(widget_title);
        toolButton_max->setObjectName(QStringLiteral("toolButton_max"));
        toolButton_max->setMinimumSize(QSize(32, 32));
        toolButton_max->setMaximumSize(QSize(32, 32));
        toolButton_max->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Picture/icon_title/\346\234\200\345\244\247\345\214\226.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_max->setIcon(icon1);
        toolButton_max->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolButton_max);

        toolButton_close = new QToolButton(widget_title);
        toolButton_close->setObjectName(QStringLiteral("toolButton_close"));
        toolButton_close->setMinimumSize(QSize(32, 32));
        toolButton_close->setMaximumSize(QSize(32, 32));
        toolButton_close->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_close->setIcon(icon2);
        toolButton_close->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolButton_close);


        verticalLayout->addWidget(widget_title);

        widget_outtext = new QWidget(FunctionCustomWidget);
        widget_outtext->setObjectName(QStringLiteral("widget_outtext"));
        widget_outtext->setStyleSheet(QLatin1String("border: 0px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); \n"
"border-style: solid;  \n"
"border-radius:1px;  \n"
"padding:0 0px;  "));
        horizontalLayout_2 = new QHBoxLayout(widget_outtext);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_warmtext = new QLabel(widget_outtext);
        label_warmtext->setObjectName(QStringLiteral("label_warmtext"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(18);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(9);
        label_warmtext->setFont(font1);
        label_warmtext->setStyleSheet(QString::fromUtf8("font: 75 18pt '\346\245\267\344\275\223';\n"
"background-color: rgba(188,187,183);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0,0);\n"
"border-style: flat;\n"
"background: transparent;"));
        label_warmtext->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_warmtext);


        verticalLayout->addWidget(widget_outtext);

        widget_button = new QWidget(FunctionCustomWidget);
        widget_button->setObjectName(QStringLiteral("widget_button"));
        widget_button->setMinimumSize(QSize(0, 50));
        widget_button->setMaximumSize(QSize(16777215, 50));
        widget_button->setStyleSheet(QLatin1String("border: 0px solid rgba(220,220,220);\n"
"background-color:rgba(188,187,183); \n"
"border-style: solid;  \n"
"border-radius:1px;  \n"
"padding:0 0px;    "));
        horizontalLayout_funcbtn = new QHBoxLayout(widget_button);
        horizontalLayout_funcbtn->setSpacing(10);
        horizontalLayout_funcbtn->setObjectName(QStringLiteral("horizontalLayout_funcbtn"));
        horizontalLayout_funcbtn->setContentsMargins(5, 5, 5, 10);
        toolButton_func_1 = new QToolButton(widget_button);
        toolButton_func_1->setObjectName(QStringLiteral("toolButton_func_1"));
        toolButton_func_1->setStyleSheet(QString::fromUtf8("QToolButton{  \n"
"border: 1px solid  rgb(200, 200, 200);\n"
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
"QToolButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                            sto"
                        "p: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QToolButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  "));

        horizontalLayout_funcbtn->addWidget(toolButton_func_1);

        toolButton_func_2 = new QToolButton(widget_button);
        toolButton_func_2->setObjectName(QStringLiteral("toolButton_func_2"));
        toolButton_func_2->setStyleSheet(QString::fromUtf8("QToolButton{  \n"
"border: 1px solid  rgb(200, 200, 200);\n"
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
"QToolButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                            sto"
                        "p: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QToolButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  "));

        horizontalLayout_funcbtn->addWidget(toolButton_func_2);

        toolButton_func_3 = new QToolButton(widget_button);
        toolButton_func_3->setObjectName(QStringLiteral("toolButton_func_3"));
        toolButton_func_3->setStyleSheet(QString::fromUtf8("QToolButton{  \n"
"border: 1px solid  rgb(200, 200, 200);\n"
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
"QToolButton:hover{  /*\351\274\240\346\240\207\346\224\276\344\270\212\345\220\216*/  \n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton:pressed{ /*\346\214\211\344\270\213\346\214\211\351\222\256\345\220\216*/  \n"
"color:rgb(255, 255, 255);  \n"
"min-height:20;  \n"
"border-style:solid;  \n"
"border-top-left-radius:2px;  \n"
"border-top-right-radius:2px;  \n"
"background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 rgb(226,236,241),   \n"
"                            st"
                        "op: 0.3 rgb(190,190,190),  \n"
"                              stop: 1 rgb(160,160,160));  \n"
"border:1px;  \n"
"border-radius:5px;padding:2px 4px;  \n"
"}  \n"
"QToolButton:checked{    /*\351\200\211\344\270\255\345\220\216*/  \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  "));

        horizontalLayout_funcbtn->addWidget(toolButton_func_3);


        verticalLayout->addWidget(widget_button);


        retranslateUi(FunctionCustomWidget);

        QMetaObject::connectSlotsByName(FunctionCustomWidget);
    } // setupUi

    void retranslateUi(QWidget *FunctionCustomWidget)
    {
        FunctionCustomWidget->setWindowTitle(QApplication::translate("FunctionCustomWidget", "Form", nullptr));
        label_icon->setText(QString());
        label_name->setText(QString());
        toolButton_min->setText(QString());
        toolButton_max->setText(QString());
        toolButton_close->setText(QString());
        label_warmtext->setText(QApplication::translate("FunctionCustomWidget", "TextLabel", nullptr));
        toolButton_func_1->setText(QString());
        toolButton_func_2->setText(QString());
        toolButton_func_3->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class FunctionCustomWidget: public Ui_FunctionCustomWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTIONCUSTOMWIDGET_H
