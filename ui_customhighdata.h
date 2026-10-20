/********************************************************************************
** Form generated from reading UI file 'customhighdata.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMHIGHDATA_H
#define UI_CUSTOMHIGHDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CustomHighData
{
public:
    QLabel *Remindertext;
    QToolButton *toolButton_ok;

    void setupUi(QWidget *CustomHighData)
    {
        if (CustomHighData->objectName().isEmpty())
            CustomHighData->setObjectName(QStringLiteral("CustomHighData"));
        CustomHighData->setWindowModality(Qt::WindowModal);
        CustomHighData->resize(355, 187);
        CustomHighData->setStyleSheet(QLatin1String("QWidget#CustomHighData{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgba(188,187,183); \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        Remindertext = new QLabel(CustomHighData);
        Remindertext->setObjectName(QStringLiteral("Remindertext"));
        Remindertext->setGeometry(QRect(5, 9, 341, 131));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(18);
        Remindertext->setFont(font);
        Remindertext->setStyleSheet(QStringLiteral("background-color: rgba(188,187,183);"));
        Remindertext->setAlignment(Qt::AlignCenter);
        Remindertext->setWordWrap(true);
        toolButton_ok = new QToolButton(CustomHighData);
        toolButton_ok->setObjectName(QStringLiteral("toolButton_ok"));
        toolButton_ok->setGeometry(QRect(100, 143, 155, 35));
        toolButton_ok->setStyleSheet(QString::fromUtf8("QToolButton\n"
"{\n"
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
"}\n"
" \n"
"QToolButton:hover\n"
"{\n"
"     background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"QToolButton:pressed\n"
"{\n"
"   background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}"));

        retranslateUi(CustomHighData);

        QMetaObject::connectSlotsByName(CustomHighData);
    } // setupUi

    void retranslateUi(QWidget *CustomHighData)
    {
        CustomHighData->setWindowTitle(QApplication::translate("CustomHighData", "\350\207\252\345\256\232\344\271\211\346\265\213\351\253\230", nullptr));
        Remindertext->setText(QString());
        toolButton_ok->setText(QApplication::translate("CustomHighData", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomHighData: public Ui_CustomHighData {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMHIGHDATA_H
