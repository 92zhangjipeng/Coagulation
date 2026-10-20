/********************************************************************************
** Form generated from reading UI file 'alarm.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ALARM_H
#define UI_ALARM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Alarm
{
public:

    void setupUi(QWidget *Alarm)
    {
        if (Alarm->objectName().isEmpty())
            Alarm->setObjectName(QStringLiteral("Alarm"));
        Alarm->resize(650, 400);

        retranslateUi(Alarm);

        QMetaObject::connectSlotsByName(Alarm);
    } // setupUi

    void retranslateUi(QWidget *Alarm)
    {
        Alarm->setWindowTitle(QApplication::translate("Alarm", "\344\273\252\345\231\250\346\266\210\346\201\257", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Alarm: public Ui_Alarm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ALARM_H
