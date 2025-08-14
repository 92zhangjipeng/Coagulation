/********************************************************************************
** Form generated from reading UI file 'canceltaskconfigure.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CANCELTASKCONFIGURE_H
#define UI_CANCELTASKCONFIGURE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CancelTaskConfigure
{
public:
    QGridLayout *gridLayout_deltask;
    QWidget *widget_alltask;
    QHBoxLayout *horizontalLayout;
    QTableWidget *tableWidgetCancelTask;
    QPushButton *ok;
    QSpacerItem *horizontalSpacer;
    QPushButton *Cancel;

    void setupUi(QDialog *CancelTaskConfigure)
    {
        if (CancelTaskConfigure->objectName().isEmpty())
            CancelTaskConfigure->setObjectName(QStringLiteral("CancelTaskConfigure"));
        CancelTaskConfigure->resize(320, 416);
        gridLayout_deltask = new QGridLayout(CancelTaskConfigure);
        gridLayout_deltask->setSpacing(0);
        gridLayout_deltask->setObjectName(QStringLiteral("gridLayout_deltask"));
        gridLayout_deltask->setContentsMargins(5, 5, 1, 10);
        widget_alltask = new QWidget(CancelTaskConfigure);
        widget_alltask->setObjectName(QStringLiteral("widget_alltask"));
        widget_alltask->setStyleSheet(QString::fromUtf8("QWidget#widget_alltask{  \n"
"font: 75 18pt '\346\245\267\344\275\223';\n"
"background-color: rgba(188,187,183);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;background: transparent;	\n"
"}"));
        horizontalLayout = new QHBoxLayout(widget_alltask);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        tableWidgetCancelTask = new QTableWidget(widget_alltask);
        tableWidgetCancelTask->setObjectName(QStringLiteral("tableWidgetCancelTask"));
        tableWidgetCancelTask->setStyleSheet(QStringLiteral(""));

        horizontalLayout->addWidget(tableWidgetCancelTask);


        gridLayout_deltask->addWidget(widget_alltask, 0, 0, 1, 3);

        ok = new QPushButton(CancelTaskConfigure);
        ok->setObjectName(QStringLiteral("ok"));
        ok->setMinimumSize(QSize(120, 40));
        ok->setMaximumSize(QSize(140, 30));
        ok->setStyleSheet(QStringLiteral(""));

        gridLayout_deltask->addWidget(ok, 1, 2, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_deltask->addItem(horizontalSpacer, 1, 1, 1, 1);

        Cancel = new QPushButton(CancelTaskConfigure);
        Cancel->setObjectName(QStringLiteral("Cancel"));
        Cancel->setMinimumSize(QSize(120, 40));
        Cancel->setMaximumSize(QSize(140, 30));
        Cancel->setStyleSheet(QStringLiteral(""));

        gridLayout_deltask->addWidget(Cancel, 1, 0, 1, 1);

        QWidget::setTabOrder(Cancel, ok);

        retranslateUi(CancelTaskConfigure);

        QMetaObject::connectSlotsByName(CancelTaskConfigure);
    } // setupUi

    void retranslateUi(QDialog *CancelTaskConfigure)
    {
        CancelTaskConfigure->setWindowTitle(QApplication::translate("CancelTaskConfigure", "Dialog", nullptr));
        ok->setText(QApplication::translate("CancelTaskConfigure", "\347\241\256\345\256\232", nullptr));
        Cancel->setText(QApplication::translate("CancelTaskConfigure", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CancelTaskConfigure: public Ui_CancelTaskConfigure {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CANCELTASKCONFIGURE_H
