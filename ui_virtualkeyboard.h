/********************************************************************************
** Form generated from reading UI file 'virtualkeyboard.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIRTUALKEYBOARD_H
#define UI_VIRTUALKEYBOARD_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Virtualkeyboard
{
public:
    QGridLayout *gridLayout;
    QPushButton *Btn1;
    QPushButton *Btn2;
    QPushButton *Btn3;
    QPushButton *Btn0;
    QPushButton *Btn4;
    QPushButton *Btn5;
    QPushButton *Btn6;
    QPushButton *Btndelete;
    QPushButton *Btn7;
    QPushButton *Btn8;
    QPushButton *Btn9;
    QPushButton *Btnclear;
    QPushButton *Btnpoint;

    void setupUi(QWidget *Virtualkeyboard)
    {
        if (Virtualkeyboard->objectName().isEmpty())
            Virtualkeyboard->setObjectName(QStringLiteral("Virtualkeyboard"));
        Virtualkeyboard->resize(336, 276);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(14);
        Virtualkeyboard->setFont(font);
        gridLayout = new QGridLayout(Virtualkeyboard);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        Btn1 = new QPushButton(Virtualkeyboard);
        Btn1->setObjectName(QStringLiteral("Btn1"));
        Btn1->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn1, 0, 0, 1, 1);

        Btn2 = new QPushButton(Virtualkeyboard);
        Btn2->setObjectName(QStringLiteral("Btn2"));
        Btn2->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn2, 0, 1, 1, 1);

        Btn3 = new QPushButton(Virtualkeyboard);
        Btn3->setObjectName(QStringLiteral("Btn3"));
        Btn3->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn3, 0, 2, 1, 1);

        Btn0 = new QPushButton(Virtualkeyboard);
        Btn0->setObjectName(QStringLiteral("Btn0"));
        Btn0->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn0, 0, 3, 1, 1);

        Btn4 = new QPushButton(Virtualkeyboard);
        Btn4->setObjectName(QStringLiteral("Btn4"));
        Btn4->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn4, 1, 0, 1, 1);

        Btn5 = new QPushButton(Virtualkeyboard);
        Btn5->setObjectName(QStringLiteral("Btn5"));
        Btn5->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn5, 1, 1, 1, 1);

        Btn6 = new QPushButton(Virtualkeyboard);
        Btn6->setObjectName(QStringLiteral("Btn6"));
        Btn6->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn6, 1, 2, 1, 1);

        Btndelete = new QPushButton(Virtualkeyboard);
        Btndelete->setObjectName(QStringLiteral("Btndelete"));
        Btndelete->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btndelete, 1, 3, 1, 1);

        Btn7 = new QPushButton(Virtualkeyboard);
        Btn7->setObjectName(QStringLiteral("Btn7"));
        Btn7->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn7, 2, 0, 1, 1);

        Btn8 = new QPushButton(Virtualkeyboard);
        Btn8->setObjectName(QStringLiteral("Btn8"));
        Btn8->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn8, 2, 1, 1, 1);

        Btn9 = new QPushButton(Virtualkeyboard);
        Btn9->setObjectName(QStringLiteral("Btn9"));
        Btn9->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btn9, 2, 2, 1, 1);

        Btnclear = new QPushButton(Virtualkeyboard);
        Btnclear->setObjectName(QStringLiteral("Btnclear"));
        Btnclear->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btnclear, 2, 3, 1, 1);

        Btnpoint = new QPushButton(Virtualkeyboard);
        Btnpoint->setObjectName(QStringLiteral("Btnpoint"));
        Btnpoint->setMinimumSize(QSize(0, 60));

        gridLayout->addWidget(Btnpoint, 3, 0, 1, 4);


        retranslateUi(Virtualkeyboard);

        QMetaObject::connectSlotsByName(Virtualkeyboard);
    } // setupUi

    void retranslateUi(QWidget *Virtualkeyboard)
    {
        Virtualkeyboard->setWindowTitle(QApplication::translate("Virtualkeyboard", "Form", nullptr));
        Btn1->setText(QApplication::translate("Virtualkeyboard", "1", nullptr));
        Btn2->setText(QApplication::translate("Virtualkeyboard", "2", nullptr));
        Btn3->setText(QApplication::translate("Virtualkeyboard", "3", nullptr));
        Btn0->setText(QApplication::translate("Virtualkeyboard", "0", nullptr));
        Btn4->setText(QApplication::translate("Virtualkeyboard", "4", nullptr));
        Btn5->setText(QApplication::translate("Virtualkeyboard", "5", nullptr));
        Btn6->setText(QApplication::translate("Virtualkeyboard", "6", nullptr));
        Btndelete->setText(QApplication::translate("Virtualkeyboard", "\360\237\221\210", nullptr));
        Btn7->setText(QApplication::translate("Virtualkeyboard", "7", nullptr));
        Btn8->setText(QApplication::translate("Virtualkeyboard", "8", nullptr));
        Btn9->setText(QApplication::translate("Virtualkeyboard", "9", nullptr));
        Btnclear->setText(QApplication::translate("Virtualkeyboard", "\346\270\205\347\251\272", nullptr));
        Btnpoint->setText(QApplication::translate("Virtualkeyboard", ".", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Virtualkeyboard: public Ui_Virtualkeyboard {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIRTUALKEYBOARD_H
