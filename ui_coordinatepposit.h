/********************************************************************************
** Form generated from reading UI file 'coordinatepposit.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COORDINATEPPOSIT_H
#define UI_COORDINATEPPOSIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CoordinatepPosit
{
public:
    QHBoxLayout *horizontalLayoutAll;

    void setupUi(QWidget *CoordinatepPosit)
    {
        if (CoordinatepPosit->objectName().isEmpty())
            CoordinatepPosit->setObjectName(QStringLiteral("CoordinatepPosit"));
        CoordinatepPosit->resize(784, 530);
        horizontalLayoutAll = new QHBoxLayout(CoordinatepPosit);
        horizontalLayoutAll->setSpacing(5);
        horizontalLayoutAll->setObjectName(QStringLiteral("horizontalLayoutAll"));
        horizontalLayoutAll->setContentsMargins(5, 5, 5, 5);

        retranslateUi(CoordinatepPosit);

        QMetaObject::connectSlotsByName(CoordinatepPosit);
    } // setupUi

    void retranslateUi(QWidget *CoordinatepPosit)
    {
        CoordinatepPosit->setWindowTitle(QApplication::translate("CoordinatepPosit", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CoordinatepPosit: public Ui_CoordinatepPosit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COORDINATEPPOSIT_H
