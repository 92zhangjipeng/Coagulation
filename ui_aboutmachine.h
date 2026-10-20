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
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AboutMachine
{
public:

    void setupUi(QWidget *AboutMachine)
    {
        if (AboutMachine->objectName().isEmpty())
            AboutMachine->setObjectName(QStringLiteral("AboutMachine"));
        AboutMachine->resize(420, 380);

        retranslateUi(AboutMachine);

        QMetaObject::connectSlotsByName(AboutMachine);
    } // setupUi

    void retranslateUi(QWidget *AboutMachine)
    {
        AboutMachine->setWindowTitle(QApplication::translate("AboutMachine", "\345\205\263\344\272\216", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AboutMachine: public Ui_AboutMachine {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUTMACHINE_H
