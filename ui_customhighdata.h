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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CustomHighData
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *Remindertext;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_ok;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *CustomHighData)
    {
        if (CustomHighData->objectName().isEmpty())
            CustomHighData->setObjectName(QStringLiteral("CustomHighData"));
        CustomHighData->setWindowModality(Qt::WindowModal);
        CustomHighData->resize(355, 187);
        CustomHighData->setStyleSheet(QLatin1String("QWidget#CustomHighData{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(230, 230, 230);  \n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    padding:0 0px;  	\n"
"} "));
        verticalLayout = new QVBoxLayout(CustomHighData);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        Remindertext = new QLabel(CustomHighData);
        Remindertext->setObjectName(QStringLiteral("Remindertext"));
        Remindertext->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(Remindertext);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton_ok = new QToolButton(CustomHighData);
        toolButton_ok->setObjectName(QStringLiteral("toolButton_ok"));
        toolButton_ok->setStyleSheet(QString::fromUtf8("QToolButton\n"
"{\n"
"    color:#ffffff; /*\346\226\207\345\255\227\351\242\234\350\211\262*/\n"
"    background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #696969, stop: 1 #696969);/*\350\203\214\346\231\257\350\211\262*/\n"
"    border-style:outset; /*\350\276\271\346\241\206\351\243\216\346\240\274*/\n"
"    border-width:2px;/*\350\276\271\346\241\206\345\256\275\345\272\246*/\n"
"    border-color:#0055ff; /*\350\276\271\346\241\206\351\242\234\350\211\262*/\n"
"    border-radius:10px; /*\350\276\271\346\241\206\345\200\222\350\247\222*/\n"
"    font: 16px; /*\345\255\227\344\275\223*/\n"
"    font-family: Segoe UI;\n"
"    min-width:100px;/*\346\216\247\344\273\266\346\234\200\345\260\217\345\256\275\345\272\246*/\n"
"    min-height:20px;/*\346\216\247\344\273\266\346\234\200\345\260\217\351\253\230\345\272\246*/\n"
"    padding:4px;/*\345\206\205\350\276\271\350\267\235*/\n"
"}\n"
" \n"
"QToolButton:hover\n"
"{\n"
"    color:#ffffff; /*\346\226\207\345\255\227\351\242\234\350\211\262*/\n"
" "
                        "   background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #ADD8E6, stop: 1 #1296db);/*\350\203\214\346\231\257\350\211\262*/\n"
"    border-style:outset; /*\350\276\271\346\241\206\351\243\216\346\240\274*/\n"
"    border-width:2px;/*\350\276\271\346\241\206\345\256\275\345\272\246*/\n"
"    border-color:#0055ff; /*\350\276\271\346\241\206\351\242\234\350\211\262*/\n"
"    border-radius:10px; /*\350\276\271\346\241\206\345\200\222\350\247\222*/\n"
"    font: 16px; /*\345\255\227\344\275\223*/\n"
"    font-family: Segoe UI;\n"
"    min-width:100px;/*\346\216\247\344\273\266\346\234\200\345\260\217\345\256\275\345\272\246*/\n"
"    min-height:20px;/*\346\216\247\344\273\266\346\234\200\345\260\217\351\253\230\345\272\246*/\n"
"    padding:4px;/*\345\206\205\350\276\271\350\267\235*/\n"
"}\n"
"QToolButton:pressed\n"
"{\n"
"    color:#ffffff; /*\346\226\207\345\255\227\351\242\234\350\211\262*/\n"
"    background-color:qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop:0 #ff55ff, stop: 1 #aa00ff);/*\350"
                        "\203\214\346\231\257\350\211\262*/\n"
"    border-style:outset; /*\350\276\271\346\241\206\351\243\216\346\240\274*/\n"
"    border-width:2px;/*\350\276\271\346\241\206\345\256\275\345\272\246*/\n"
"    border-color:#0055ff; /*\350\276\271\346\241\206\351\242\234\350\211\262*/\n"
"    border-radius:10px; /*\350\276\271\346\241\206\345\200\222\350\247\222*/\n"
"    font: 16px; /*\345\255\227\344\275\223*/\n"
"    font-family: Segoe UI;\n"
"    min-width:100px;/*\346\216\247\344\273\266\346\234\200\345\260\217\345\256\275\345\272\246*/\n"
"    min-height:20px;/*\346\216\247\344\273\266\346\234\200\345\260\217\351\253\230\345\272\246*/\n"
"    padding:4px;/*\345\206\205\350\276\271\350\267\235*/\n"
"}"));

        horizontalLayout->addWidget(toolButton_ok);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout);


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
