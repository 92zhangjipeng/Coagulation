/********************************************************************************
** Form generated from reading UI file 'alarm.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALARM_H
#define UI_ALARM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Alarm
{
public:
    QGridLayout *gridLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_puticon;
    QLabel *titleicon;
    QSpacerItem *horizontalSpacer_2;
    QToolButton *Close;
    QTextBrowser *textBrowser;
    QWidget *widget_2;
    QHBoxLayout *horizontalLayout;
    QToolButton *Horn;
    QSpacerItem *horizontalSpacer;
    QPushButton *makesureAlarm;

    void setupUi(QWidget *Alarm)
    {
        if (Alarm->objectName().isEmpty())
            Alarm->setObjectName(QStringLiteral("Alarm"));
        Alarm->setWindowModality(Qt::NonModal);
        Alarm->resize(371, 350);
        Alarm->setMinimumSize(QSize(300, 350));
        Alarm->setMaximumSize(QSize(1111111, 1111111));
        gridLayout = new QGridLayout(Alarm);
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(0, 0, 0, 0);
        widget = new QWidget(Alarm);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 42));
        widget->setMaximumSize(QSize(16777215, 42));
        widget->setStyleSheet(QLatin1String(" border:0.5px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:1px;  \n"
"    padding:0 0px;  "));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        label_puticon = new QLabel(widget);
        label_puticon->setObjectName(QStringLiteral("label_puticon"));
        label_puticon->setMinimumSize(QSize(32, 32));
        label_puticon->setMaximumSize(QSize(32, 32));
        label_puticon->setStyleSheet(QLatin1String("border: 0px ;  \n"
"background: transparent;"));

        horizontalLayout_2->addWidget(label_puticon);

        titleicon = new QLabel(widget);
        titleicon->setObjectName(QStringLiteral("titleicon"));
        titleicon->setMinimumSize(QSize(200, 0));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        titleicon->setFont(font);
        titleicon->setStyleSheet(QString::fromUtf8("border: 0px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:22px;\n"
"background: transparent;"));
        titleicon->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(titleicon);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        Close = new QToolButton(widget);
        Close->setObjectName(QStringLiteral("Close"));
        Close->setMinimumSize(QSize(16, 16));
        Close->setStyleSheet(QLatin1String("QToolButton{\n"
"   min-width:16px;\n"
"   min-height:16px;\n"
"   border-style:outset; \n"
"	border: 0px ;  \n"
"	background: transparent;\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        Close->setIcon(icon);
        Close->setIconSize(QSize(32, 32));

        horizontalLayout_2->addWidget(Close);


        gridLayout->addWidget(widget, 0, 0, 1, 1);

        textBrowser = new QTextBrowser(Alarm);
        textBrowser->setObjectName(QStringLiteral("textBrowser"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font1.setPointSize(14);
        textBrowser->setFont(font1);
        textBrowser->setStyleSheet(QLatin1String("QTextBrowser#textBrowser{  \n"
"    border: 1px solid  rgb(224, 224, 223);  \n"
"	background-color:rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:1px;  \n"
"    padding:0 0px;  	\n"
"} "));
        textBrowser->setFrameShape(QFrame::Panel);

        gridLayout->addWidget(textBrowser, 1, 0, 1, 1);

        widget_2 = new QWidget(Alarm);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMinimumSize(QSize(0, 42));
        widget_2->setStyleSheet(QLatin1String("border: 1px solid  rgb(224, 224, 223);  \n"
"background-color:rgba(188,187,183);\n"
"border-style: solid;  \n"
"border-radius:1px;  \n"
"padding:0 0px;  "));
        horizontalLayout = new QHBoxLayout(widget_2);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(10, 0, 5, 0);
        Horn = new QToolButton(widget_2);
        Horn->setObjectName(QStringLiteral("Horn"));
        Horn->setStyleSheet(QLatin1String("min-width:16px;\n"
"min-height:16px;\n"
"border-style:outset; \n"
"border: 0px ;  \n"
"background: transparent;"));
        Horn->setIconSize(QSize(16, 16));

        horizontalLayout->addWidget(Horn);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        makesureAlarm = new QPushButton(widget_2);
        makesureAlarm->setObjectName(QStringLiteral("makesureAlarm"));
        makesureAlarm->setFont(font);
        makesureAlarm->setStyleSheet(QString::fromUtf8("QPushButton#makesureAlarm\n"
"{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
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
"QPushButton#makesureAlarm:hover\n"
"{     \n"
" background-color: qlineargradient(spread:pad, x1:0,   y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#makesureAlarm:pressed\n"
"{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout->addWidget(makesureAlarm);


        gridLayout->addWidget(widget_2, 2, 0, 1, 1);


        retranslateUi(Alarm);

        QMetaObject::connectSlotsByName(Alarm);
    } // setupUi

    void retranslateUi(QWidget *Alarm)
    {
        Alarm->setWindowTitle(QApplication::translate("Alarm", "Form", nullptr));
        label_puticon->setText(QString());
        titleicon->setText(QApplication::translate("Alarm", "TextLabel", nullptr));
#ifndef QT_NO_TOOLTIP
        Close->setToolTip(QApplication::translate("Alarm", "\345\205\263\351\227\255", nullptr));
#endif // QT_NO_TOOLTIP
        Close->setText(QString());
#ifndef QT_NO_TOOLTIP
        Horn->setToolTip(QApplication::translate("Alarm", "\345\274\200\345\205\263\350\234\202\351\270\243\345\231\250", nullptr));
#endif // QT_NO_TOOLTIP
        Horn->setText(QString());
#ifndef QT_NO_TOOLTIP
        makesureAlarm->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        makesureAlarm->setText(QApplication::translate("Alarm", "\347\241\256\350\256\244\346\212\245\350\255\246", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Alarm: public Ui_Alarm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALARM_H
