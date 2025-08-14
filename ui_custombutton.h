/********************************************************************************
** Form generated from reading UI file 'custombutton.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMBUTTON_H
#define UI_CUSTOMBUTTON_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_customButton
{
public:

    void setupUi(QWidget *customButton)
    {
        if (customButton->objectName().isEmpty())
            customButton->setObjectName(QStringLiteral("customButton"));
        customButton->resize(156, 155);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(18);
        customButton->setFont(font);
        customButton->setMouseTracking(true);
        customButton->setTabletTracking(true);
        customButton->setFocusPolicy(Qt::ClickFocus);
        customButton->setStyleSheet(QLatin1String("QWidget#customButton{\n"
"   background:rgb(255, 255, 255);\n"
"   background-color: rgb(255, 255, 255); \n"
"   border:1px;\n"
"   border-radius:2px;\n"
"   color:rgb(255, 255, 255);\n"
"   color:black;\n"
"}\n"
""));

        retranslateUi(customButton);

        QMetaObject::connectSlotsByName(customButton);
    } // setupUi

    void retranslateUi(QWidget *customButton)
    {
        customButton->setWindowTitle(QString());
    } // retranslateUi

};

namespace Ui {
    class customButton: public Ui_customButton {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMBUTTON_H
