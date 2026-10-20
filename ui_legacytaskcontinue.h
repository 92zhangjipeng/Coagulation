/********************************************************************************
** Form generated from reading UI file 'legacytaskcontinue.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEGACYTASKCONTINUE_H
#define UI_LEGACYTASKCONTINUE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LegacyTaskContinue
{
public:
    QWidget *widget;
    QToolButton *toolButton_continue;
    QTableWidget *tableWidget_LegacyTask;

    void setupUi(QWidget *LegacyTaskContinue)
    {
        if (LegacyTaskContinue->objectName().isEmpty())
            LegacyTaskContinue->setObjectName(QStringLiteral("LegacyTaskContinue"));
        LegacyTaskContinue->resize(1039, 667);
        widget = new QWidget(LegacyTaskContinue);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 510, 1011, 141));
        toolButton_continue = new QToolButton(widget);
        toolButton_continue->setObjectName(QStringLiteral("toolButton_continue"));
        toolButton_continue->setGeometry(QRect(170, 50, 81, 41));
        tableWidget_LegacyTask = new QTableWidget(LegacyTaskContinue);
        tableWidget_LegacyTask->setObjectName(QStringLiteral("tableWidget_LegacyTask"));
        tableWidget_LegacyTask->setGeometry(QRect(10, 10, 1011, 491));
        tableWidget_LegacyTask->setStyleSheet(QLatin1String("QTableWidget{\n"
"color:#DCDCDC;\n"
"background:#444444;\n"
"border:1px solid #242424;\n"
"alternate-background-color:#525252;\n"
"gridline-color:#242424;\n"
"font:12pt;\n"
"\n"
"}\n"
" \n"
"QTableWidget::item:selected{\n"
"color:#DCDCDC;\n"
"background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 #484848,stop:1 #383838);\n"
"}\n"
" \n"
"QTableWidget::item:hover{\n"
"background:#5B5B5B;\n"
"}\n"
"QHeaderView::section{\n"
"text-align:center;\n"
"background:#5E5E5E;\n"
"padding:3px;\n"
"margin:0px;\n"
"color:#DCDCDC;\n"
"border:1px solid #242424;\n"
"border-left-width:0;\n"
"}\n"
" \n"
"QScrollBar:vertical{\n"
"background:#484848;\n"
"padding:0px;\n"
"border-radius:6px;\n"
"max-width:12px;\n"
"}\n"
" \n"
" \n"
"QScrollBar::handle:vertical{\n"
"background:#CCCCCC;\n"
"}\n"
" \n"
"QScrollBar::handle:hover:vertical,QScrollBar::handle:pressed:vertical{\n"
"background:#A7A7A7;\n"
"}\n"
"QScrollBar::sub-page:vertical{\n"
"background:444444;\n"
"}\n"
" \n"
" \n"
"QScrollBar::add-page:vertical{\n"
"background:5"
                        "B5B5B;\n"
"}\n"
" \n"
"QScrollBar::add-line:vertical{\n"
"background:none;\n"
"}\n"
"QScrollBar::sub-line:vertical{\n"
"background:none;\n"
"}"));

        retranslateUi(LegacyTaskContinue);

        QMetaObject::connectSlotsByName(LegacyTaskContinue);
    } // setupUi

    void retranslateUi(QWidget *LegacyTaskContinue)
    {
        LegacyTaskContinue->setWindowTitle(QApplication::translate("LegacyTaskContinue", "Form", nullptr));
        toolButton_continue->setText(QApplication::translate("LegacyTaskContinue", "\347\273\247\347\273\255\344\273\273\345\212\241", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LegacyTaskContinue: public Ui_LegacyTaskContinue {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEGACYTASKCONTINUE_H
