/********************************************************************************
** Form generated from reading UI file 'tipcustomwidget.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TIPCUSTOMWIDGET_H
#define UI_TIPCUSTOMWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_tipcustomwidget
{
public:
    QHBoxLayout *horizontalLayout_2;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;

    void setupUi(QWidget *tipcustomwidget)
    {
        if (tipcustomwidget->objectName().isEmpty())
            tipcustomwidget->setObjectName(QStringLiteral("tipcustomwidget"));
        tipcustomwidget->resize(451, 87);
        horizontalLayout_2 = new QHBoxLayout(tipcustomwidget);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);
        widget = new QWidget(tipcustomwidget);
        widget->setObjectName(QStringLiteral("widget"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(5, 5, 5, 5);
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        horizontalLayout_2->addWidget(widget);


        retranslateUi(tipcustomwidget);

        QMetaObject::connectSlotsByName(tipcustomwidget);
    } // setupUi

    void retranslateUi(QWidget *tipcustomwidget)
    {
        tipcustomwidget->setWindowTitle(QApplication::translate("tipcustomwidget", "Form", nullptr));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class tipcustomwidget: public Ui_tipcustomwidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TIPCUSTOMWIDGET_H
