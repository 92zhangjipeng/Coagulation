/********************************************************************************
** Form generated from reading UI file 'canceltaskconfigure.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CANCELTASKCONFIGURE_H
#define UI_CANCELTASKCONFIGURE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_CancelTaskConfigure
{
public:

    void setupUi(QDialog *CancelTaskConfigure)
    {
        if (CancelTaskConfigure->objectName().isEmpty())
            CancelTaskConfigure->setObjectName(QStringLiteral("CancelTaskConfigure"));
        CancelTaskConfigure->resize(400, 450);

        retranslateUi(CancelTaskConfigure);

        QMetaObject::connectSlotsByName(CancelTaskConfigure);
    } // setupUi

    void retranslateUi(QDialog *CancelTaskConfigure)
    {
        CancelTaskConfigure->setWindowTitle(QApplication::translate("CancelTaskConfigure", "\345\217\226\346\266\210\346\240\267\346\234\254\344\273\273\345\212\241", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CancelTaskConfigure: public Ui_CancelTaskConfigure {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CANCELTASKCONFIGURE_H
