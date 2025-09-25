/********************************************************************************
** Form generated from reading UI file 'custompppvalue.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMPPPVALUE_H
#define UI_CUSTOMPPPVALUE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_customPPPValue
{
public:
    QVBoxLayout *verticalLayout;
    QTableWidget *tableWidget;
    QTableWidget *tableWidgetoutresult;
    QWidget *widgetfucn;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButtonsycn;

    void setupUi(QWidget *customPPPValue)
    {
        if (customPPPValue->objectName().isEmpty())
            customPPPValue->setObjectName(QStringLiteral("customPPPValue"));
        customPPPValue->resize(622, 580);
        verticalLayout = new QVBoxLayout(customPPPValue);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tableWidget = new QTableWidget(customPPPValue);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setMinimumSize(QSize(0, 300));

        verticalLayout->addWidget(tableWidget);

        tableWidgetoutresult = new QTableWidget(customPPPValue);
        tableWidgetoutresult->setObjectName(QStringLiteral("tableWidgetoutresult"));
        tableWidgetoutresult->setMaximumSize(QSize(16777215, 180));

        verticalLayout->addWidget(tableWidgetoutresult);

        widgetfucn = new QWidget(customPPPValue);
        widgetfucn->setObjectName(QStringLiteral("widgetfucn"));
        widgetfucn->setMinimumSize(QSize(0, 40));
        horizontalLayout = new QHBoxLayout(widgetfucn);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 5, 0);
        pushButtonsycn = new QPushButton(widgetfucn);
        pushButtonsycn->setObjectName(QStringLiteral("pushButtonsycn"));

        horizontalLayout->addWidget(pushButtonsycn);


        verticalLayout->addWidget(widgetfucn);


        retranslateUi(customPPPValue);

        QMetaObject::connectSlotsByName(customPPPValue);
    } // setupUi

    void retranslateUi(QWidget *customPPPValue)
    {
        customPPPValue->setWindowTitle(QApplication::translate("customPPPValue", "Form", nullptr));
        pushButtonsycn->setText(QApplication::translate("customPPPValue", "\344\277\235\345\255\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class customPPPValue: public Ui_customPPPValue {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMPPPVALUE_H
