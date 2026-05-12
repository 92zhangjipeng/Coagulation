/********************************************************************************
** Form generated from reading UI file 'addusername.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ADDUSERNAME_H
#define UI_ADDUSERNAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AddUserName
{
public:

    void setupUi(QWidget *AddUserName)
    {
        if (AddUserName->objectName().isEmpty())
            AddUserName->setObjectName(QStringLiteral("AddUserName"));
        AddUserName->resize(400, 320);

        retranslateUi(AddUserName);

        QMetaObject::connectSlotsByName(AddUserName);
    } // setupUi

    void retranslateUi(QWidget *AddUserName)
    {
        AddUserName->setWindowTitle(QApplication::translate("AddUserName", "\347\224\250\346\210\267\347\256\241\347\220\206", nullptr));
    } // retranslateUi

};

namespace Ui {
    class AddUserName: public Ui_AddUserName {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ADDUSERNAME_H
