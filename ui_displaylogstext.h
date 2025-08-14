/********************************************************************************
** Form generated from reading UI file 'displaylogstext.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYLOGSTEXT_H
#define UI_DISPLAYLOGSTEXT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <custom_style/animationprocessbar.h>

QT_BEGIN_NAMESPACE

class Ui_displayLogsText
{
public:
    QVBoxLayout *verticalLayout_2;
    QWidget *Head_text;
    QHBoxLayout *horizontalLayout;
    QLabel *label_showImage;
    QLabel *Title_TEXT;
    QSpacerItem *horizontalSpacer_3;
    QToolButton *toolButton_showmax;
    QToolButton *Close_Log;
    QWidget *widget_showLogDisPlay;
    QVBoxLayout *verticalLayout;
    QWidget *widget_loading;
    QHBoxLayout *horizontalLayout_loding;
	AnimationProcessBar *progressBar_loading;
    QTextBrowser *textBrowser_showlog;
    QWidget *widget_fucn;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *toolButton_update;
    QLabel *label_showtime;
    QToolButton *toolButton_clean;

    void setupUi(QWidget *displayLogsText)
    {
        if (displayLogsText->objectName().isEmpty())
            displayLogsText->setObjectName(QStringLiteral("displayLogsText"));
        displayLogsText->resize(427, 568);
        displayLogsText->setStyleSheet(QStringLiteral(""));
        verticalLayout_2 = new QVBoxLayout(displayLogsText);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        Head_text = new QWidget(displayLogsText);
        Head_text->setObjectName(QStringLiteral("Head_text"));
        Head_text->setMinimumSize(QSize(0, 36));
        Head_text->setMaximumSize(QSize(16777215, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(14);
        Head_text->setFont(font);
        Head_text->setStyleSheet(QLatin1String("QWidget#Head_text{  \n"
"    border: 2px solid  rgb(224, 224, 223);\n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;   	\n"
"} "));
        horizontalLayout = new QHBoxLayout(Head_text);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 0, 5, 0);
        label_showImage = new QLabel(Head_text);
        label_showImage->setObjectName(QStringLiteral("label_showImage"));
        label_showImage->setMinimumSize(QSize(32, 32));
        label_showImage->setMaximumSize(QSize(32, 32));

        horizontalLayout->addWidget(label_showImage);

        Title_TEXT = new QLabel(Head_text);
        Title_TEXT->setObjectName(QStringLiteral("Title_TEXT"));
        Title_TEXT->setMinimumSize(QSize(150, 32));
        Title_TEXT->setMaximumSize(QSize(16777215, 32));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(14);
        Title_TEXT->setFont(font1);

        horizontalLayout->addWidget(Title_TEXT);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        toolButton_showmax = new QToolButton(Head_text);
        toolButton_showmax->setObjectName(QStringLiteral("toolButton_showmax"));
        toolButton_showmax->setMinimumSize(QSize(16, 16));
        toolButton_showmax->setStyleSheet(QLatin1String("QToolButton{\n"
"   min-width:16px;\n"
"   min-height:16px;\n"
"   border-style:outset; \n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\346\234\200\345\244\247\345\214\226.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_showmax->setIcon(icon);
        toolButton_showmax->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolButton_showmax);

        Close_Log = new QToolButton(Head_text);
        Close_Log->setObjectName(QStringLiteral("Close_Log"));
        Close_Log->setMinimumSize(QSize(16, 16));
        QFont font2;
        Close_Log->setFont(font2);
        Close_Log->setStyleSheet(QLatin1String("QToolButton{\n"
"   min-width:16px;\n"
"   min-height:16px;\n"
"   border-style:outset; \n"
"}"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        Close_Log->setIcon(icon1);
        Close_Log->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(Close_Log);


        verticalLayout_2->addWidget(Head_text);

        widget_showLogDisPlay = new QWidget(displayLogsText);
        widget_showLogDisPlay->setObjectName(QStringLiteral("widget_showLogDisPlay"));
        widget_showLogDisPlay->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(widget_showLogDisPlay);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(10, 5, 10, 5);
        widget_loading = new QWidget(widget_showLogDisPlay);
        widget_loading->setObjectName(QStringLiteral("widget_loading"));
        widget_loading->setMinimumSize(QSize(0, 30));
        widget_loading->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_loding = new QHBoxLayout(widget_loading);
        horizontalLayout_loding->setSpacing(0);
        horizontalLayout_loding->setObjectName(QStringLiteral("horizontalLayout_loding"));
        horizontalLayout_loding->setContentsMargins(10, 0, 0, 0);
        progressBar_loading = new AnimationProcessBar(widget_loading);
        progressBar_loading->setObjectName(QStringLiteral("progressBar_loading"));
        progressBar_loading->setValue(24);

        horizontalLayout_loding->addWidget(progressBar_loading);


        verticalLayout->addWidget(widget_loading);

        textBrowser_showlog = new QTextBrowser(widget_showLogDisPlay);
        textBrowser_showlog->setObjectName(QStringLiteral("textBrowser_showlog"));
        QFont font3;
        font3.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font3.setPointSize(12);
        textBrowser_showlog->setFont(font3);
        textBrowser_showlog->setStyleSheet(QLatin1String("QTextBrowser#textBrowser_showlog{  \n"
"    border: 1px solid  rgb(224, 224, 223);\n"
"	background-color: rgba(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:1px;  \n"
"    padding:0 0px;    	\n"
"} "));

        verticalLayout->addWidget(textBrowser_showlog);

        widget_fucn = new QWidget(widget_showLogDisPlay);
        widget_fucn->setObjectName(QStringLiteral("widget_fucn"));
        widget_fucn->setMinimumSize(QSize(0, 50));
        widget_fucn->setMaximumSize(QSize(16777215, 50));
        widget_fucn->setFont(font3);
        horizontalLayout_2 = new QHBoxLayout(widget_fucn);
        horizontalLayout_2->setSpacing(20);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(10, 0, 10, 0);
        toolButton_update = new QToolButton(widget_fucn);
        toolButton_update->setObjectName(QStringLiteral("toolButton_update"));
        toolButton_update->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_update{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_update:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_update:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout_2->addWidget(toolButton_update);

        label_showtime = new QLabel(widget_fucn);
        label_showtime->setObjectName(QStringLiteral("label_showtime"));
        label_showtime->setMaximumSize(QSize(200, 16777215));
        label_showtime->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout_2->addWidget(label_showtime);

        toolButton_clean = new QToolButton(widget_fucn);
        toolButton_clean->setObjectName(QStringLiteral("toolButton_clean"));
        toolButton_clean->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_clean{  \n"
"    border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:23px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButton_clean:hover{     \n"
"  \n"
"	background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_clean:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout_2->addWidget(toolButton_clean);


        verticalLayout->addWidget(widget_fucn);


        verticalLayout_2->addWidget(widget_showLogDisPlay);


        retranslateUi(displayLogsText);

        QMetaObject::connectSlotsByName(displayLogsText);
    } // setupUi

    void retranslateUi(QWidget *displayLogsText)
    {
        displayLogsText->setWindowTitle(QApplication::translate("displayLogsText", "Form", nullptr));
        label_showImage->setText(QString());
        Title_TEXT->setText(QString());
        toolButton_showmax->setText(QString());
        Close_Log->setText(QString());
        toolButton_update->setText(QApplication::translate("displayLogsText", "\346\233\264\346\226\260", nullptr));
        label_showtime->setText(QString());
        toolButton_clean->setText(QApplication::translate("displayLogsText", "\346\270\205\347\251\272LOG", nullptr));
    } // retranslateUi

};

namespace Ui {
    class displayLogsText: public Ui_displayLogsText {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYLOGSTEXT_H
