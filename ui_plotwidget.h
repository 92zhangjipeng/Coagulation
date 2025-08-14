/********************************************************************************
** Form generated from reading UI file 'plotwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLOTWIDGET_H
#define UI_PLOTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PlotWidget
{
public:

    void setupUi(QWidget *PlotWidget)
    {
        if (PlotWidget->objectName().isEmpty())
            PlotWidget->setObjectName(QStringLiteral("PlotWidget"));
        PlotWidget->resize(1163, 770);
        PlotWidget->setStyleSheet(QLatin1String("QWidget#PlotWidget{  \n"
"    border: 1px solid #C0C0C0;  \n"
"	background-color: rgb(230, 230, 230);  \n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"} "));

        retranslateUi(PlotWidget);

        QMetaObject::connectSlotsByName(PlotWidget);
    } // setupUi

    void retranslateUi(QWidget *PlotWidget)
    {
        PlotWidget->setWindowTitle(QApplication::translate("PlotWidget", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlotWidget: public Ui_PlotWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLOTWIDGET_H
