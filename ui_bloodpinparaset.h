/********************************************************************************
** Form generated from reading UI file 'bloodpinparaset.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BLOODPINPARASET_H
#define UI_BLOODPINPARASET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BloodPinParaSet
{
public:
    QTableWidget *tableWidget_bloodpin;
    QWidget *widget_btn;

    void setupUi(QWidget *BloodPinParaSet)
    {
        if (BloodPinParaSet->objectName().isEmpty())
            BloodPinParaSet->setObjectName(QStringLiteral("BloodPinParaSet"));
        BloodPinParaSet->resize(778, 576);
        tableWidget_bloodpin = new QTableWidget(BloodPinParaSet);
        tableWidget_bloodpin->setObjectName(QStringLiteral("tableWidget_bloodpin"));
        tableWidget_bloodpin->setGeometry(QRect(10, 20, 741, 491));
        widget_btn = new QWidget(BloodPinParaSet);
        widget_btn->setObjectName(QStringLiteral("widget_btn"));
        widget_btn->setGeometry(QRect(20, 520, 721, 45));
        widget_btn->setMaximumSize(QSize(13333, 45));

        retranslateUi(BloodPinParaSet);

        QMetaObject::connectSlotsByName(BloodPinParaSet);
    } // setupUi

    void retranslateUi(QWidget *BloodPinParaSet)
    {
        BloodPinParaSet->setWindowTitle(QApplication::translate("BloodPinParaSet", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class BloodPinParaSet: public Ui_BloodPinParaSet {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BLOODPINPARASET_H
