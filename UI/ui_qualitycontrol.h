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
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QualityControl
{
public:
    QToolButton *toolButton;

    void setupUi(QWidget *QualityControl)
    {
        if (QualityControl->objectName().isEmpty())
            QualityControl->setObjectName(QStringLiteral("QualityControl"));
        QualityControl->resize(400, 300);
        toolButton = new QToolButton(QualityControl);
        toolButton->setObjectName(QStringLiteral("toolButton"));
        toolButton->setGeometry(QRect(130, 120, 37, 101));

        retranslateUi(QualityControl);

        QMetaObject::connectSlotsByName(QualityControl);
    } // setupUi

    void retranslateUi(QWidget *QualityControl)
    {
        QualityControl->setWindowTitle(QApplication::translate("QualityControl", "Form", nullptr));
        toolButton->setText(QApplication::translate("QualityControl", "...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QualityControl: public Ui_QualityControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QUALITYCONTROL_H
