/********************************************************************************
** Form generated from reading UI file 'aboutmachine.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUTMACHINE_H
#define UI_ABOUTMACHINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutMachine
{
public:
    QVBoxLayout *verticalLayout_about;
    QWidget *widget_about_main;
    QGridLayout *gridLayout;
    QLabel *About_Company;
    QLabel *About_Version;
    QLabel *About_Machine;
    QLabel *About_line;
    QLabel *About_connection;
    QLabel *About_Module;
    QLabel *labelHaxi;
    QLabel *About_Info;
    QWidget *widget_about_bottom;
    QPushButton *pushButtonok;

    void setupUi(QWidget *AboutMachine)
    {
        if (AboutMachine->objectName().isEmpty())
            AboutMachine->setObjectName(QStringLiteral("AboutMachine"));
        AboutMachine->setWindowModality(Qt::ApplicationModal);
        AboutMachine->resize(378, 352);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(12);
        AboutMachine->setFont(font);
        verticalLayout_about = new QVBoxLayout(AboutMachine);
        verticalLayout_about->setSpacing(0);
        verticalLayout_about->setObjectName(QStringLiteral("verticalLayout_about"));
        verticalLayout_about->setContentsMargins(0, 0, 0, 0);
        widget_about_main = new QWidget(AboutMachine);
        widget_about_main->setObjectName(QStringLiteral("widget_about_main"));
        widget_about_main->setMaximumSize(QSize(380, 295));
        widget_about_main->setStyleSheet(QLatin1String("QWidget#widget_about_main{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(250, 250, 250);  \n"
"    border-style: solid;  \n"
"    border-radius:1px;  \n"
"    padding:0 0px;  	\n"
"} "));
        gridLayout = new QGridLayout(widget_about_main);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        About_Company = new QLabel(widget_about_main);
        About_Company->setObjectName(QStringLiteral("About_Company"));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(16);
        font1.setBold(true);
        font1.setWeight(75);
        About_Company->setFont(font1);

        gridLayout->addWidget(About_Company, 0, 0, 1, 1);

        About_Version = new QLabel(widget_about_main);
        About_Version->setObjectName(QStringLiteral("About_Version"));
        About_Version->setFont(font);

        gridLayout->addWidget(About_Version, 1, 0, 1, 1);

        About_Machine = new QLabel(widget_about_main);
        About_Machine->setObjectName(QStringLiteral("About_Machine"));
        About_Machine->setFont(font);

        gridLayout->addWidget(About_Machine, 2, 0, 1, 1);

        About_line = new QLabel(widget_about_main);
        About_line->setObjectName(QStringLiteral("About_line"));
        About_line->setFrameShape(QFrame::HLine);

        gridLayout->addWidget(About_line, 3, 0, 1, 1);

        About_connection = new QLabel(widget_about_main);
        About_connection->setObjectName(QStringLiteral("About_connection"));
        About_connection->setFont(font);

        gridLayout->addWidget(About_connection, 4, 0, 1, 1);

        About_Module = new QLabel(widget_about_main);
        About_Module->setObjectName(QStringLiteral("About_Module"));
        About_Module->setFont(font);

        gridLayout->addWidget(About_Module, 5, 0, 1, 1);

        labelHaxi = new QLabel(widget_about_main);
        labelHaxi->setObjectName(QStringLiteral("labelHaxi"));
        labelHaxi->setMinimumSize(QSize(0, 50));
        labelHaxi->setFont(font);
        labelHaxi->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        labelHaxi->setWordWrap(true);

        gridLayout->addWidget(labelHaxi, 6, 0, 1, 1);

        About_Info = new QLabel(widget_about_main);
        About_Info->setObjectName(QStringLiteral("About_Info"));
        About_Info->setMaximumSize(QSize(16777215, 16));

        gridLayout->addWidget(About_Info, 7, 0, 1, 1);


        verticalLayout_about->addWidget(widget_about_main);

        widget_about_bottom = new QWidget(AboutMachine);
        widget_about_bottom->setObjectName(QStringLiteral("widget_about_bottom"));
        widget_about_bottom->setMaximumSize(QSize(16777215, 50));
        widget_about_bottom->setStyleSheet(QLatin1String("QWidget#widget_about_bottom{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(205, 200, 205);  \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"} "));
        pushButtonok = new QPushButton(widget_about_bottom);
        pushButtonok->setObjectName(QStringLiteral("pushButtonok"));
        pushButtonok->setGeometry(QRect(270, 10, 90, 30));
        pushButtonok->setMaximumSize(QSize(90, 30));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        pushButtonok->setFont(font2);
        pushButtonok->setStyleSheet(QString::fromUtf8("QPushButton#pushButtonok\n"
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
"QPushButton#pushButtonok:hover\n"
"{     \n"
"    \n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QPushButton#pushButtonok:pressed\n"
"{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        verticalLayout_about->addWidget(widget_about_bottom);


        retranslateUi(AboutMachine);

        QMetaObject::connectSlotsByName(AboutMachine);
    } // setupUi

    void retranslateUi(QWidget *AboutMachine)
    {
        AboutMachine->setWindowTitle(QApplication::translate("AboutMachine", "Form", nullptr));
        About_Company->setText(QApplication::translate("AboutMachine", "TextLabel", nullptr));
        About_Version->setText(QApplication::translate("AboutMachine", "TextLabel", nullptr));
        About_Machine->setText(QApplication::translate("AboutMachine", "TextLabel", nullptr));
        About_line->setText(QString());
        About_connection->setText(QApplication::translate("AboutMachine", "TextLabel", nullptr));
        About_Module->setText(QApplication::translate("AboutMachine", "TextLabel", nullptr));
        labelHaxi->setText(QString());
        About_Info->setText(QString());
        pushButtonok->setText(QApplication::translate("AboutMachine", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutMachine: public Ui_AboutMachine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTMACHINE_H
