/********************************************************************************
** Form generated from reading UI file 'correct_data.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CORRECT_DATA_H
#define UI_CORRECT_DATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Correct_Data
{
public:

    void setupUi(QWidget *Correct_Data)
    {
        if (Correct_Data->objectName().isEmpty())
            Correct_Data->setObjectName(QStringLiteral("Correct_Data"));
        Correct_Data->resize(360, 130);

        retranslateUi(Correct_Data);

        QMetaObject::connectSlotsByName(Correct_Data);
    } // setupUi

    void retranslateUi(QWidget *Correct_Data)
    {
        Correct_Data->setWindowTitle(QApplication::translate("Correct_Data", "\346\225\260\346\215\256\344\277\256\346\255\243", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Correct_Data: public Ui_Correct_Data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CORRECT_DATA_H
