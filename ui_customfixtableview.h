/********************************************************************************
** Form generated from reading UI file 'customfixtableview.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMFIXTABLEVIEW_H
#define UI_CUSTOMFIXTABLEVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_CustomFixTableView
{
public:

    void setupUi(QWidget *CustomFixTableView)
    {
        if (CustomFixTableView->objectName().isEmpty())
            CustomFixTableView->setObjectName(QStringLiteral("CustomFixTableView"));
        CustomFixTableView->resize(1450, 800);

        retranslateUi(CustomFixTableView);

        QMetaObject::connectSlotsByName(CustomFixTableView);
    } // setupUi

    void retranslateUi(QWidget *CustomFixTableView)
    {
        CustomFixTableView->setWindowTitle(QApplication::translate("CustomFixTableView", "\350\241\200\345\260\217\346\235\277\350\201\232\351\233\206\345\210\206\346\236\220\347\263\273\347\273\237", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomFixTableView: public Ui_CustomFixTableView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMFIXTABLEVIEW_H
