/********************************************************************************
** Form generated from reading UI file 'waitingforui.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WAITINGFORUI_H
#define UI_WAITINGFORUI_H

#include <QRoundProgressBar.h>
#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WaitingForUi
{
public:
    QVBoxLayout *verticalLayout;
    QRoundProgressBar *Waiting;
    QPushButton *pushButton_Cancel;

    void setupUi(QWidget *WaitingForUi)
    {
        if (WaitingForUi->objectName().isEmpty())
            WaitingForUi->setObjectName(QStringLiteral("WaitingForUi"));
        WaitingForUi->setWindowModality(Qt::NonModal);
        WaitingForUi->resize(300, 300);
        WaitingForUi->setMinimumSize(QSize(300, 300));
        WaitingForUi->setMaximumSize(QSize(300, 300));
        verticalLayout = new QVBoxLayout(WaitingForUi);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        Waiting = new QRoundProgressBar(WaitingForUi);
        Waiting->setObjectName(QStringLiteral("Waiting"));
        Waiting->setMinimumSize(QSize(290, 0));

        verticalLayout->addWidget(Waiting);

        pushButton_Cancel = new QPushButton(WaitingForUi);
        pushButton_Cancel->setObjectName(QStringLiteral("pushButton_Cancel"));
        pushButton_Cancel->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(12);
        pushButton_Cancel->setFont(font);

        verticalLayout->addWidget(pushButton_Cancel);


        retranslateUi(WaitingForUi);

        QMetaObject::connectSlotsByName(WaitingForUi);
    } // setupUi

    void retranslateUi(QWidget *WaitingForUi)
    {
        WaitingForUi->setWindowTitle(QApplication::translate("WaitingForUi", "Form", nullptr));
        pushButton_Cancel->setText(QApplication::translate("WaitingForUi", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class WaitingForUi: public Ui_WaitingForUi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WAITINGFORUI_H
