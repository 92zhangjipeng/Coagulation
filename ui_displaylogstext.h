/********************************************************************************
** Form generated from reading UI file 'displaylogstext.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DISPLAYLOGSTEXT_H
#define UI_DISPLAYLOGSTEXT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_displayLogsText
{
public:

    void setupUi(QWidget *displayLogsText)
    {
        if (displayLogsText->objectName().isEmpty())
            displayLogsText->setObjectName(QStringLiteral("displayLogsText"));
        displayLogsText->resize(427, 568);

        retranslateUi(displayLogsText);

        QMetaObject::connectSlotsByName(displayLogsText);
    } // setupUi

    void retranslateUi(QWidget *displayLogsText)
    {
        displayLogsText->setWindowTitle(QApplication::translate("displayLogsText", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class displayLogsText: public Ui_displayLogsText {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DISPLAYLOGSTEXT_H
