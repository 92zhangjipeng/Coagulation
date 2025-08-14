/********************************************************************************
** Form generated from reading UI file 'patient.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PATIENT_H
#define UI_PATIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Patient
{
public:
    QHBoxLayout *paitentLayout;
    QTableWidget *tableWidget_Paitent;

    void setupUi(QWidget *Patient)
    {
        if (Patient->objectName().isEmpty())
            Patient->setObjectName(QStringLiteral("Patient"));
        Patient->resize(931, 663);
        paitentLayout = new QHBoxLayout(Patient);
        paitentLayout->setSpacing(0);
        paitentLayout->setObjectName(QStringLiteral("paitentLayout"));
        paitentLayout->setContentsMargins(0, 0, 0, 0);
        tableWidget_Paitent = new QTableWidget(Patient);
        tableWidget_Paitent->setObjectName(QStringLiteral("tableWidget_Paitent"));
        QFont font;
        font.setPointSize(12);
        tableWidget_Paitent->setFont(font);
        tableWidget_Paitent->setFrameShape(QFrame::Box);
        tableWidget_Paitent->setFrameShadow(QFrame::Sunken);

        paitentLayout->addWidget(tableWidget_Paitent);


        retranslateUi(Patient);

        QMetaObject::connectSlotsByName(Patient);
    } // setupUi

    void retranslateUi(QWidget *Patient)
    {
        Patient->setWindowTitle(QApplication::translate("Patient", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Patient: public Ui_Patient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PATIENT_H
