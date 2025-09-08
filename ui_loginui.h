/********************************************************************************
** Form generated from reading UI file 'loginui.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINUI_H
#define UI_LOGINUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginUi
{
public:
    QVBoxLayout *verticalLayout_mian;

    void setupUi(QWidget *LoginUi)
    {
        if (LoginUi->objectName().isEmpty())
            LoginUi->setObjectName(QStringLiteral("LoginUi"));
        LoginUi->setWindowModality(Qt::ApplicationModal);
        LoginUi->resize(1155, 665);
        LoginUi->setMinimumSize(QSize(400, 340));
        LoginUi->setMaximumSize(QSize(8888, 8888));
        LoginUi->setFocusPolicy(Qt::ClickFocus);
        verticalLayout_mian = new QVBoxLayout(LoginUi);
        verticalLayout_mian->setSpacing(0);
        verticalLayout_mian->setObjectName(QStringLiteral("verticalLayout_mian"));
        verticalLayout_mian->setContentsMargins(0, 0, 0, 0);

        retranslateUi(LoginUi);

        QMetaObject::connectSlotsByName(LoginUi);
    } // setupUi

    void retranslateUi(QWidget *LoginUi)
    {
        LoginUi->setWindowTitle(QApplication::translate("LoginUi", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LoginUi: public Ui_LoginUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINUI_H
