/********************************************************************************
** Form generated from reading UI file 'qprintpreviewdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QPRINTPREVIEWDIALOG_H
#define UI_QPRINTPREVIEWDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QPrintPreviewDialog
{
public:

    void setupUi(QWidget *QPrintPreviewDialog)
    {
        if (QPrintPreviewDialog->objectName().isEmpty())
            QPrintPreviewDialog->setObjectName(QStringLiteral("QPrintPreviewDialog"));
        QPrintPreviewDialog->resize(400, 300);

        retranslateUi(QPrintPreviewDialog);

        QMetaObject::connectSlotsByName(QPrintPreviewDialog);
    } // setupUi

    void retranslateUi(QWidget *QPrintPreviewDialog)
    {
        QPrintPreviewDialog->setWindowTitle(QApplication::translate("QPrintPreviewDialog", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QPrintPreviewDialog: public Ui_QPrintPreviewDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QPRINTPREVIEWDIALOG_H
