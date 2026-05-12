/********************************************************************************
** Form generated from reading UI file 'loginmaininterface.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINMAININTERFACE_H
#define UI_LOGINMAININTERFACE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_loginmaininterface
{
public:

    void setupUi(QDialog *loginmaininterface)
    {
        if (loginmaininterface->objectName().isEmpty())
            loginmaininterface->setObjectName(QStringLiteral("loginmaininterface"));
        loginmaininterface->resize(500, 520);

        retranslateUi(loginmaininterface);

        QMetaObject::connectSlotsByName(loginmaininterface);
    } // setupUi

    void retranslateUi(QDialog *loginmaininterface)
    {
        loginmaininterface->setWindowTitle(QApplication::translate("loginmaininterface", "\347\231\273\345\275\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class loginmaininterface: public Ui_loginmaininterface {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINMAININTERFACE_H
