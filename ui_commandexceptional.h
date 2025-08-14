/********************************************************************************
** Form generated from reading UI file 'commandexceptional.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMANDEXCEPTIONAL_H
#define UI_COMMANDEXCEPTIONAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CommandExceptional
{
public:
    QVBoxLayout *verticalLayout_main;
    QWidget *widget_head;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labelimage;
    QLabel *label_titlename;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_close;
    QWidget *widget_2;
    QVBoxLayout *verticalLayout_2;
    QWidget *widget_showfucn;
    QVBoxLayout *verticalLayout;
    QLabel *label_ErrInfo;
    QWidget *widgetConfigEquipment;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_KS600;
    QPushButton *pushButton_KS800;
    QPushButton *pushButton_KS1200;
    QWidget *widgetBtn;
    QHBoxLayout *horizontalLayout_2;
    QToolButton *toolButton_ok;
    QToolButton *toolButton_Cancel;

    void setupUi(QWidget *CommandExceptional)
    {
        if (CommandExceptional->objectName().isEmpty())
            CommandExceptional->setObjectName(QStringLiteral("CommandExceptional"));
        CommandExceptional->resize(496, 175);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(12);
        CommandExceptional->setFont(font);
        CommandExceptional->setWindowOpacity(0.95);
        CommandExceptional->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border-top-color: rgb(0, 0, 0);"));
        verticalLayout_main = new QVBoxLayout(CommandExceptional);
        verticalLayout_main->setSpacing(0);
        verticalLayout_main->setObjectName(QStringLiteral("verticalLayout_main"));
        verticalLayout_main->setContentsMargins(0, 0, 0, 0);
        widget_head = new QWidget(CommandExceptional);
        widget_head->setObjectName(QStringLiteral("widget_head"));
        widget_head->setMaximumSize(QSize(16777215, 36));
        widget_head->setFont(font);
        widget_head->setStyleSheet(QLatin1String("background-color: rgba(198,197,193);\n"
"border-color: rgb(180, 180, 180);\n"
"border: 1 px solid rgba(198,197,193);"));
        horizontalLayout_3 = new QHBoxLayout(widget_head);
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(5, 0, 0, 0);
        labelimage = new QLabel(widget_head);
        labelimage->setObjectName(QStringLiteral("labelimage"));
        labelimage->setMinimumSize(QSize(32, 32));
        labelimage->setMaximumSize(QSize(32, 32));
        labelimage->setStyleSheet(QLatin1String("border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_3->addWidget(labelimage);

        label_titlename = new QLabel(widget_head);
        label_titlename->setObjectName(QStringLiteral("label_titlename"));
        label_titlename->setMinimumSize(QSize(180, 0));
        label_titlename->setMaximumSize(QSize(16777215, 34));
        label_titlename->setFont(font);
        label_titlename->setStyleSheet(QLatin1String("border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_3->addWidget(label_titlename);

        horizontalSpacer = new QSpacerItem(131, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        toolButton_close = new QToolButton(widget_head);
        toolButton_close->setObjectName(QStringLiteral("toolButton_close"));
        toolButton_close->setMinimumSize(QSize(0, 0));
        toolButton_close->setStyleSheet(QLatin1String("QToolButton\n"
"{\n"
"	border-style: flat;\n"
"	background: transparent;\n"
"}"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_close->setIcon(icon);
        toolButton_close->setIconSize(QSize(32, 32));

        horizontalLayout_3->addWidget(toolButton_close);


        verticalLayout_main->addWidget(widget_head);

        widget_2 = new QWidget(CommandExceptional);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(180, 180, 180);\n"
"border: 0 px solid rgba(198,197,193);"));
        verticalLayout_2 = new QVBoxLayout(widget_2);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(5, 5, 5, 5);
        widget_showfucn = new QWidget(widget_2);
        widget_showfucn->setObjectName(QStringLiteral("widget_showfucn"));
        verticalLayout = new QVBoxLayout(widget_showfucn);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label_ErrInfo = new QLabel(widget_showfucn);
        label_ErrInfo->setObjectName(QStringLiteral("label_ErrInfo"));
        label_ErrInfo->setMinimumSize(QSize(0, 25));
        label_ErrInfo->setMaximumSize(QSize(16777215, 25));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(18);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(9);
        label_ErrInfo->setFont(font1);
        label_ErrInfo->setStyleSheet(QString::fromUtf8("font: 75 18pt '\346\245\267\344\275\223';\n"
"background-color: rgba(188,187,183);\n"
"border-radius:1px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;background: transparent;"));
        label_ErrInfo->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_ErrInfo);


        verticalLayout_2->addWidget(widget_showfucn);

        widgetConfigEquipment = new QWidget(widget_2);
        widgetConfigEquipment->setObjectName(QStringLiteral("widgetConfigEquipment"));
        widgetConfigEquipment->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border-top-color: rgb(0, 0, 0);"));
        horizontalLayout = new QHBoxLayout(widgetConfigEquipment);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_KS600 = new QPushButton(widgetConfigEquipment);
        pushButton_KS600->setObjectName(QStringLiteral("pushButton_KS600"));
        pushButton_KS600->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_KS600{  \n"
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
"QPushButton#pushButton_KS600:hover{     \n"
" background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButton_KS600:pressed{  \n"
"      background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout->addWidget(pushButton_KS600);

        pushButton_KS800 = new QPushButton(widgetConfigEquipment);
        pushButton_KS800->setObjectName(QStringLiteral("pushButton_KS800"));
        pushButton_KS800->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_KS800{  \n"
"  border: 1px solid  rgb(200, 200, 200);\n"
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
"QPushButton#pushButton_KS800:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButton_KS800:pressed{  \n"
"      background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout->addWidget(pushButton_KS800);

        pushButton_KS1200 = new QPushButton(widgetConfigEquipment);
        pushButton_KS1200->setObjectName(QStringLiteral("pushButton_KS1200"));
        pushButton_KS1200->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_KS1200{  \n"
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
"QPushButton#pushButton_KS1200:hover{     \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButton_KS1200:pressed{  \n"
"      background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        horizontalLayout->addWidget(pushButton_KS1200);


        verticalLayout_2->addWidget(widgetConfigEquipment);

        widgetBtn = new QWidget(widget_2);
        widgetBtn->setObjectName(QStringLiteral("widgetBtn"));
        widgetBtn->setMinimumSize(QSize(0, 35));
        widgetBtn->setMaximumSize(QSize(16777215, 45));
        horizontalLayout_2 = new QHBoxLayout(widgetBtn);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        toolButton_ok = new QToolButton(widgetBtn);
        toolButton_ok->setObjectName(QStringLiteral("toolButton_ok"));
        toolButton_ok->setMinimumSize(QSize(100, 0));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        toolButton_ok->setFont(font2);
        toolButton_ok->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_ok{  \n"
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
"QToolButton#toolButton_ok:hover{     \n"
"     background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_ok:pressed{  \n"
"     background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        toolButton_ok->setIconSize(QSize(32, 32));
        toolButton_ok->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_2->addWidget(toolButton_ok);

        toolButton_Cancel = new QToolButton(widgetBtn);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMinimumSize(QSize(100, 0));
        toolButton_Cancel->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_Cancel{  \n"
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
"QToolButton#toolButton_Cancel:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_Cancel:pressed{  \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        toolButton_Cancel->setIconSize(QSize(32, 32));
        toolButton_Cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_2->addWidget(toolButton_Cancel);


        verticalLayout_2->addWidget(widgetBtn);


        verticalLayout_main->addWidget(widget_2);


        retranslateUi(CommandExceptional);

        QMetaObject::connectSlotsByName(CommandExceptional);
    } // setupUi

    void retranslateUi(QWidget *CommandExceptional)
    {
        CommandExceptional->setWindowTitle(QString());
        labelimage->setText(QString());
        label_titlename->setText(QString());
        toolButton_close->setText(QString());
        label_ErrInfo->setText(QString());
        pushButton_KS600->setText(QApplication::translate("CommandExceptional", "KS-600", nullptr));
        pushButton_KS800->setText(QApplication::translate("CommandExceptional", "KS-800", nullptr));
        pushButton_KS1200->setText(QApplication::translate("CommandExceptional", "KS-1200", nullptr));
        toolButton_ok->setText(QApplication::translate("CommandExceptional", "\347\241\256\345\256\232", nullptr));
        toolButton_Cancel->setText(QApplication::translate("CommandExceptional", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CommandExceptional: public Ui_CommandExceptional {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMANDEXCEPTIONAL_H
