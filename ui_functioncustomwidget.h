/********************************************************************************
** Form generated from reading UI file 'functioncustomwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FUNCTIONCUSTOMWIDGET_H
#define UI_FUNCTIONCUSTOMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FunctionCustomWidget
{
public:

    void setupUi(QWidget *FunctionCustomWidget)
    {
        if (FunctionCustomWidget->objectName().isEmpty())
            FunctionCustomWidget->setObjectName(QStringLiteral("FunctionCustomWidget"));
        FunctionCustomWidget->resize(480, 260);

        retranslateUi(FunctionCustomWidget);

        QMetaObject::connectSlotsByName(FunctionCustomWidget);
    } // setupUi

    void retranslateUi(QWidget *FunctionCustomWidget)
    {
        FunctionCustomWidget->setWindowTitle(QApplication::translate("FunctionCustomWidget", "\346\217\220\347\244\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FunctionCustomWidget: public Ui_FunctionCustomWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FUNCTIONCUSTOMWIDGET_H
