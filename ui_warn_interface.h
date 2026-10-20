/********************************************************************************
** Form generated from reading UI file 'warn_interface.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WARN_INTERFACE_H
#define UI_WARN_INTERFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_warn_interface
{
public:

    void setupUi(QWidget *warn_interface)
    {
        if (warn_interface->objectName().isEmpty())
            warn_interface->setObjectName(QStringLiteral("warn_interface"));
        warn_interface->resize(380, 220);

        retranslateUi(warn_interface);

        QMetaObject::connectSlotsByName(warn_interface);
    } // setupUi

    void retranslateUi(QWidget *warn_interface)
    {
        warn_interface->setWindowTitle(QApplication::translate("warn_interface", "\346\217\220\347\244\272", nullptr));
    } // retranslateUi

};

namespace Ui {
    class warn_interface: public Ui_warn_interface {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WARN_INTERFACE_H
