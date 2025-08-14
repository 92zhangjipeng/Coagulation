/********************************************************************************
** Form generated from reading UI file 'pmessagebox.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PMESSAGEBOX_H
#define UI_PMESSAGEBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PMessageBox
{
public:
    QVBoxLayout *verticalLayout_traylayout;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_showicon;
    QLabel *label_titlename;
    QSpacerItem *horizontalSpacer;
    QWidget *widget_TrayShow;
    QLabel *label_remindertext;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QToolButton *toolButton_OK;
    QToolButton *toolButton_Cancel;

    void setupUi(QDialog *PMessageBox)
    {
        if (PMessageBox->objectName().isEmpty())
            PMessageBox->setObjectName(QStringLiteral("PMessageBox"));
        PMessageBox->resize(402, 675);
        PMessageBox->setMinimumSize(QSize(401, 0));
        PMessageBox->setMaximumSize(QSize(402, 16777215));
        PMessageBox->setStyleSheet(QString::fromUtf8("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border-top-color: rgb(0, 0, 0);\n"
"font: 20 12pt \"\346\245\267\344\275\223\";"));
        verticalLayout_traylayout = new QVBoxLayout(PMessageBox);
        verticalLayout_traylayout->setSpacing(0);
        verticalLayout_traylayout->setObjectName(QStringLiteral("verticalLayout_traylayout"));
        verticalLayout_traylayout->setContentsMargins(0, 0, 0, 10);
        widget_title = new QWidget(PMessageBox);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMinimumSize(QSize(0, 36));
        widget_title->setMaximumSize(QSize(400, 36));
        widget_title->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border: 2px solid rgba(220,220,220);"));
        horizontalLayout_2 = new QHBoxLayout(widget_title);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_showicon = new QLabel(widget_title);
        label_showicon->setObjectName(QStringLiteral("label_showicon"));
        label_showicon->setMinimumSize(QSize(32, 32));
        label_showicon->setMaximumSize(QSize(32, 32));
        label_showicon->setStyleSheet(QString::fromUtf8("font: 20 17pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:2px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));

        horizontalLayout_2->addWidget(label_showicon);

        label_titlename = new QLabel(widget_title);
        label_titlename->setObjectName(QStringLiteral("label_titlename"));
        label_titlename->setMinimumSize(QSize(190, 30));
        label_titlename->setMaximumSize(QSize(220, 32));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(17);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(2);
        label_titlename->setFont(font);
        label_titlename->setStyleSheet(QString::fromUtf8("font: 20 17pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:2px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));
        label_titlename->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_titlename);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_traylayout->addWidget(widget_title);

        widget_TrayShow = new QWidget(PMessageBox);
        widget_TrayShow->setObjectName(QStringLiteral("widget_TrayShow"));
        widget_TrayShow->setMinimumSize(QSize(400, 460));
        widget_TrayShow->setMaximumSize(QSize(400, 16777215));
        widget_TrayShow->setStyleSheet(QString::fromUtf8("background-color: rgba(188,187,183);\n"
"border-color: rgb(0, 0, 0);\n"
"border-top-color: rgb(0, 0, 0);\n"
"font: 20 12pt \"\346\245\267\344\275\223\";\n"
"\n"
""));

        verticalLayout_traylayout->addWidget(widget_TrayShow);

        label_remindertext = new QLabel(PMessageBox);
        label_remindertext->setObjectName(QStringLiteral("label_remindertext"));
        label_remindertext->setMinimumSize(QSize(0, 32));
        label_remindertext->setMaximumSize(QSize(16777215, 32));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(15);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(2);
        label_remindertext->setFont(font1);
        label_remindertext->setStyleSheet(QString::fromUtf8("font: 20 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;\n"
""));
        label_remindertext->setAlignment(Qt::AlignCenter);

        verticalLayout_traylayout->addWidget(label_remindertext);

        widget = new QWidget(PMessageBox);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 40));
        widget->setMaximumSize(QSize(16777215, 45));
        widget->setStyleSheet(QString::fromUtf8("font: 20 15pt '\346\245\267\344\275\223';\n"
"background-color:rgb(190, 154, 124, 255);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;\n"
"background: transparent;"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 5, -1, 10);
        toolButton_OK = new QToolButton(widget);
        toolButton_OK->setObjectName(QStringLiteral("toolButton_OK"));
        toolButton_OK->setMinimumSize(QSize(105, 35));
        toolButton_OK->setMaximumSize(QSize(105, 60));
        toolButton_OK->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_OK{  \n"
"   border: 1px solid  rgb(200, 200, 200);\n"
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
"QToolButton#toolButton_OK:hover{     \n"
"  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_OK:pressed{  \n"
"    border: 1px solid #C0C0C0; \n"
"    background-color:#33ccff;  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 80px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:16px;\n"
"	color:white;\n"
"}"));
        toolButton_OK->setIconSize(QSize(32, 32));
        toolButton_OK->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_OK);

        toolButton_Cancel = new QToolButton(widget);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMinimumSize(QSize(105, 35));
        toolButton_Cancel->setMaximumSize(QSize(105, 60));
        toolButton_Cancel->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_Cancel{  \n"
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
"QToolButton#toolButton_Cancel:hover{     \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButton_Cancel:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));
        toolButton_Cancel->setIconSize(QSize(32, 32));
        toolButton_Cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_Cancel);


        verticalLayout_traylayout->addWidget(widget);


        retranslateUi(PMessageBox);

        QMetaObject::connectSlotsByName(PMessageBox);
    } // setupUi

    void retranslateUi(QDialog *PMessageBox)
    {
        PMessageBox->setWindowTitle(QApplication::translate("PMessageBox", "Dialog", nullptr));
        label_showicon->setText(QString());
        label_titlename->setText(QString());
        label_remindertext->setText(QString());
        toolButton_OK->setText(QApplication::translate("PMessageBox", "\347\241\256\345\256\232", nullptr));
        toolButton_Cancel->setText(QApplication::translate("PMessageBox", "\345\277\275\347\225\245", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PMessageBox: public Ui_PMessageBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PMESSAGEBOX_H
