/********************************************************************************
** Form generated from reading UI file 'qualitycontrol.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QUALITYCONTROL_H
#define UI_QUALITYCONTROL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QualityControl
{
public:

    void setupUi(QWidget *QualityControl)
    {
        if (QualityControl->objectName().isEmpty())
            QualityControl->setObjectName(QStringLiteral("QualityControl"));
        QualityControl->resize(1857, 1190);
        QualityControl->setMinimumSize(QSize(240, 0));
        QualityControl->setMaximumSize(QSize(999999, 16777215));
        QualityControl->setStyleSheet(QLatin1String("border: 0px;\n"
"background-color:rgba(188,187,183);"));

        retranslateUi(QualityControl);

        QMetaObject::connectSlotsByName(QualityControl);
    } // setupUi

    void retranslateUi(QWidget *QualityControl)
    {
        QualityControl->setWindowTitle(QApplication::translate("QualityControl", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QualityControl: public Ui_QualityControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUALITYCONTROL_H
