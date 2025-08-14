/********************************************************************************
** Form generated from reading UI file 'calibrate.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CALIBRATE_H
#define UI_CALIBRATE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Calibrate
{
public:
    QGridLayout *gridLayout;
    QTableWidget *saveTaskWidgetList;

    void setupUi(QWidget *Calibrate)
    {
        if (Calibrate->objectName().isEmpty())
            Calibrate->setObjectName(QStringLiteral("Calibrate"));
        Calibrate->resize(1546, 750);
        Calibrate->setStyleSheet(QLatin1String("border: 1px solid rgb(220, 220, 220);\n"
"background-color:rgba(255,255,255);"));
        gridLayout = new QGridLayout(Calibrate);
        gridLayout->setSpacing(1);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(1, 1, 1, 1);
        saveTaskWidgetList = new QTableWidget(Calibrate);
        saveTaskWidgetList->setObjectName(QStringLiteral("saveTaskWidgetList"));
        saveTaskWidgetList->setMinimumSize(QSize(450, 0));
        saveTaskWidgetList->setMaximumSize(QSize(999999, 16777215));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(12);
        saveTaskWidgetList->setFont(font);
        saveTaskWidgetList->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(saveTaskWidgetList, 0, 0, 1, 2);


        retranslateUi(Calibrate);

        QMetaObject::connectSlotsByName(Calibrate);
    } // setupUi

    void retranslateUi(QWidget *Calibrate)
    {
        Calibrate->setWindowTitle(QApplication::translate("Calibrate", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Calibrate: public Ui_Calibrate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CALIBRATE_H
