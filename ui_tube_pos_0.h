/********************************************************************************
** Form generated from reading UI file 'tube_pos_0.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TUBE_POS_0_H
#define UI_TUBE_POS_0_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Tube_Pos_0
{
public:
    QVBoxLayout *verticalLayout;

    void setupUi(QWidget *Tube_Pos_0)
    {
        if (Tube_Pos_0->objectName().isEmpty())
            Tube_Pos_0->setObjectName(QStringLiteral("Tube_Pos_0"));
        Tube_Pos_0->resize(598, 386);
        Tube_Pos_0->setMinimumSize(QSize(320, 380));
        Tube_Pos_0->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(Tube_Pos_0);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 15);

        retranslateUi(Tube_Pos_0);

        QMetaObject::connectSlotsByName(Tube_Pos_0);
    } // setupUi

    void retranslateUi(QWidget *Tube_Pos_0)
    {
        Tube_Pos_0->setWindowTitle(QApplication::translate("Tube_Pos_0", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Tube_Pos_0: public Ui_Tube_Pos_0 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TUBE_POS_0_H
