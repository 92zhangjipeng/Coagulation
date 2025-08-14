/********************************************************************************
** Form generated from reading UI file 'instrumentalarmprompt.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INSTRUMENTALARMPROMPT_H
#define UI_INSTRUMENTALARMPROMPT_H

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

class Ui_instrumentAlarmPrompt
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_titlebar;
    QHBoxLayout *horizontalLayout;
    QLabel *label_image;
    QLabel *label_alarminfo;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_close;
    QLabel *label_reminderText;
    QWidget *widgetputbutton;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *toolButton_closeReminder;

    void setupUi(QWidget *instrumentAlarmPrompt)
    {
        if (instrumentAlarmPrompt->objectName().isEmpty())
            instrumentAlarmPrompt->setObjectName(QStringLiteral("instrumentAlarmPrompt"));
        instrumentAlarmPrompt->resize(455, 266);
        instrumentAlarmPrompt->setStyleSheet(QLatin1String("QWidget#instrumentAlarmPrompt{  \n"
"  background-color: rgba(188,187,183);\n"
"border-top-color: rgb(0, 0, 0);\n"
"border: 1px solid  rgb(224, 224, 223);\n"
"} "));
        verticalLayout = new QVBoxLayout(instrumentAlarmPrompt);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_titlebar = new QWidget(instrumentAlarmPrompt);
        widget_titlebar->setObjectName(QStringLiteral("widget_titlebar"));
        widget_titlebar->setMaximumSize(QSize(16777215, 36));
        widget_titlebar->setStyleSheet(QLatin1String("QWidget#widget_titlebar\n"
"{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color:rgba(188,187,183); \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"} "));
        horizontalLayout = new QHBoxLayout(widget_titlebar);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        label_image = new QLabel(widget_titlebar);
        label_image->setObjectName(QStringLiteral("label_image"));
        label_image->setMinimumSize(QSize(32, 32));
        label_image->setMaximumSize(QSize(32, 32));

        horizontalLayout->addWidget(label_image);

        label_alarminfo = new QLabel(widget_titlebar);
        label_alarminfo->setObjectName(QStringLiteral("label_alarminfo"));
        label_alarminfo->setMinimumSize(QSize(140, 32));
        label_alarminfo->setMaximumSize(QSize(200, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(12);
        label_alarminfo->setFont(font);

        horizontalLayout->addWidget(label_alarminfo);

        horizontalSpacer = new QSpacerItem(280, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton_close = new QToolButton(widget_titlebar);
        toolButton_close->setObjectName(QStringLiteral("toolButton_close"));
        toolButton_close->setMinimumSize(QSize(16, 16));
        toolButton_close->setMaximumSize(QSize(32, 32));
        toolButton_close->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_close->setIcon(icon);
        toolButton_close->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolButton_close);


        verticalLayout->addWidget(widget_titlebar);

        label_reminderText = new QLabel(instrumentAlarmPrompt);
        label_reminderText->setObjectName(QStringLiteral("label_reminderText"));
        label_reminderText->setMinimumSize(QSize(0, 50));
        label_reminderText->setMaximumSize(QSize(16777215, 9999));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(18);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(9);
        label_reminderText->setFont(font1);
        label_reminderText->setStyleSheet(QString::fromUtf8("font: 75 18pt '\346\245\267\344\275\223';\n"
"background-color: rgba(188,187,183);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;background: transparent;"));
        label_reminderText->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        label_reminderText->setWordWrap(true);

        verticalLayout->addWidget(label_reminderText);

        widgetputbutton = new QWidget(instrumentAlarmPrompt);
        widgetputbutton->setObjectName(QStringLiteral("widgetputbutton"));
        widgetputbutton->setMinimumSize(QSize(0, 35));
        widgetputbutton->setMaximumSize(QSize(16777215, 48));
        horizontalLayout_2 = new QHBoxLayout(widgetputbutton);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(308, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        toolButton_closeReminder = new QToolButton(widgetputbutton);
        toolButton_closeReminder->setObjectName(QStringLiteral("toolButton_closeReminder"));
        toolButton_closeReminder->setMinimumSize(QSize(120, 32));
        toolButton_closeReminder->setMaximumSize(QSize(120, 35));
        toolButton_closeReminder->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_closeReminder{  \n"
"     border: 1px solid  rgb(200, 200, 200);\n"
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
"QToolButton#toolButton_closeReminder:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_closeReminder:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));
        toolButton_closeReminder->setIconSize(QSize(32, 32));
        toolButton_closeReminder->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_2->addWidget(toolButton_closeReminder);


        verticalLayout->addWidget(widgetputbutton);


        retranslateUi(instrumentAlarmPrompt);

        QMetaObject::connectSlotsByName(instrumentAlarmPrompt);
    } // setupUi

    void retranslateUi(QWidget *instrumentAlarmPrompt)
    {
        instrumentAlarmPrompt->setWindowTitle(QApplication::translate("instrumentAlarmPrompt", "Form", nullptr));
        label_image->setText(QString());
        label_alarminfo->setText(QString());
        toolButton_close->setText(QApplication::translate("instrumentAlarmPrompt", "...", nullptr));
        label_reminderText->setText(QString());
        toolButton_closeReminder->setText(QApplication::translate("instrumentAlarmPrompt", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class instrumentAlarmPrompt: public Ui_instrumentAlarmPrompt {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INSTRUMENTALARMPROMPT_H
