/********************************************************************************
** Form generated from reading UI file 'widgetplot.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGETPLOT_H
#define UI_WIDGETPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WidgetPlot
{
public:

    void setupUi(QWidget *WidgetPlot)
    {
        if (WidgetPlot->objectName().isEmpty())
            WidgetPlot->setObjectName(QStringLiteral("WidgetPlot"));
        WidgetPlot->resize(400, 300);

        retranslateUi(WidgetPlot);

        QMetaObject::connectSlotsByName(WidgetPlot);
    } // setupUi

    void retranslateUi(QWidget *WidgetPlot)
    {
        WidgetPlot->setWindowTitle(QApplication::translate("WidgetPlot", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WidgetPlot: public Ui_WidgetPlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGETPLOT_H
