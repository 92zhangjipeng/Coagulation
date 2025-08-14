/********************************************************************************
** Form generated from reading UI file 'connecttoinstrument.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTTOINSTRUMENT_H
#define UI_CONNECTTOINSTRUMENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConnectToInstrument
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
    QWidget *widget_wram;
    QGridLayout *gridLayout;
    QPushButton *pushButton_ok;
    QPushButton *pushButton_cancel;
    QPushButton *pushButton_hendingtime;
    QLabel *label_warmtext;

    void setupUi(QDialog *ConnectToInstrument)
    {
        if (ConnectToInstrument->objectName().isEmpty())
            ConnectToInstrument->setObjectName(QStringLiteral("ConnectToInstrument"));
        ConnectToInstrument->resize(371, 285);
        ConnectToInstrument->setMinimumSize(QSize(0, 200));
        verticalLayout = new QVBoxLayout(ConnectToInstrument);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        widget_title = new QWidget(ConnectToInstrument);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMinimumSize(QSize(0, 35));
        widget_title->setMaximumSize(QSize(16777215, 35));
        widget_title->setStyleSheet(QLatin1String("background-color: rgba(255,0,0);\n"
"border-color: rgb(180, 180, 180);\n"
"border: opx solid rgba(255,0,0);\n"
""));
        horizontalLayout = new QHBoxLayout(widget_title);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_icon = new QLabel(widget_title);
        label_icon->setObjectName(QStringLiteral("label_icon"));
        label_icon->setMinimumSize(QSize(32, 32));
        label_icon->setMaximumSize(QSize(32, 32));
        label_icon->setStyleSheet(QLatin1String("border-style: flat;\n"
"background: transparent;"));
        label_icon->setPixmap(QPixmap(QString::fromUtf8(":/Picture/icon_title/\346\217\220\347\244\272.png")));

        horizontalLayout->addWidget(label_icon);

        label_name = new QLabel(widget_title);
        label_name->setObjectName(QStringLiteral("label_name"));
        label_name->setMinimumSize(QSize(0, 32));
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

        widget_wram = new QWidget(ConnectToInstrument);
        widget_wram->setObjectName(QStringLiteral("widget_wram"));
        widget_wram->setStyleSheet(QLatin1String("background-color: rgba(205,200 ,200);\n"
"border-top-color: rgb(0, 0, 0);\n"
"border: opx solid rgba(255,0,0);"));
        gridLayout = new QGridLayout(widget_wram);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        pushButton_ok = new QPushButton(widget_wram);
        pushButton_ok->setObjectName(QStringLiteral("pushButton_ok"));
        pushButton_ok->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_ok{  \n"
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
"QPushButton#pushButton_ok:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButton_ok:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        gridLayout->addWidget(pushButton_ok, 1, 0, 1, 1);

        pushButton_cancel = new QPushButton(widget_wram);
        pushButton_cancel->setObjectName(QStringLiteral("pushButton_cancel"));
        pushButton_cancel->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_cancel{  \n"
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
"QPushButton#pushButton_cancel:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButton_cancel:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        gridLayout->addWidget(pushButton_cancel, 1, 1, 1, 1);

        pushButton_hendingtime = new QPushButton(widget_wram);
        pushButton_hendingtime->setObjectName(QStringLiteral("pushButton_hendingtime"));
        pushButton_hendingtime->setStyleSheet(QString::fromUtf8("QPushButton#pushButton_hendingtime{  \n"
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
"QPushButton#pushButton_hendingtime:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButton_hendingtime:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        gridLayout->addWidget(pushButton_hendingtime, 1, 2, 1, 1);

        label_warmtext = new QLabel(widget_wram);
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
"color: rgb(255, 0, 0);\n"
"border-style: flat;background: transparent;"));
        label_warmtext->setAlignment(Qt::AlignCenter);
        label_warmtext->setWordWrap(true);

        gridLayout->addWidget(label_warmtext, 0, 0, 1, 3);


        verticalLayout->addWidget(widget_wram);


        retranslateUi(ConnectToInstrument);

        QMetaObject::connectSlotsByName(ConnectToInstrument);
    } // setupUi

    void retranslateUi(QDialog *ConnectToInstrument)
    {
        ConnectToInstrument->setWindowTitle(QApplication::translate("ConnectToInstrument", "Dialog", nullptr));
        label_icon->setText(QString());
        label_name->setText(QApplication::translate("ConnectToInstrument", "TextLabel", nullptr));
        toolButton_min->setText(QString());
        toolButton_max->setText(QString());
        toolButton_close->setText(QString());
        pushButton_ok->setText(QApplication::translate("ConnectToInstrument", "\347\241\256\345\256\232", nullptr));
        pushButton_cancel->setText(QApplication::translate("ConnectToInstrument", "\345\217\226\346\266\210", nullptr));
        pushButton_hendingtime->setText(QString());
        label_warmtext->setText(QApplication::translate("ConnectToInstrument", "\346\217\220\347\244\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConnectToInstrument: public Ui_ConnectToInstrument {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTTOINSTRUMENT_H
