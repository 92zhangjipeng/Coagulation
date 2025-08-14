/********************************************************************************
** Form generated from reading UI file 'testcupdraw.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTCUPDRAW_H
#define UI_TESTCUPDRAW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>
#include "ringsmapprogressbar.h"

QT_BEGIN_NAMESPACE

class Ui_TestCupDraw
{
public:
    RingsMapProgressbar *bar_test_1;
    RingsMapProgressbar *bar_test_2;
    RingsMapProgressbar *bar_test_3;
    RingsMapProgressbar *bar_test_4;
    RingsMapProgressbar *bar_test_7;
    RingsMapProgressbar *bar_test_12;
    RingsMapProgressbar *bar_test_11;
    RingsMapProgressbar *bar_test_5;
    RingsMapProgressbar *bar_test_8;
    RingsMapProgressbar *bar_test_10;
    RingsMapProgressbar *bar_test_9;
    RingsMapProgressbar *bar_test_6;

    void setupUi(QWidget *TestCupDraw)
    {
        if (TestCupDraw->objectName().isEmpty())
            TestCupDraw->setObjectName(QStringLiteral("TestCupDraw"));
        TestCupDraw->resize(2214, 278);
        TestCupDraw->setMinimumSize(QSize(0, 120));
        TestCupDraw->setMaximumSize(QSize(12222, 460));
        bar_test_1 = new RingsMapProgressbar(TestCupDraw);
        bar_test_1->setObjectName(QStringLiteral("bar_test_1"));
        bar_test_1->setGeometry(QRect(10, 30, 125, 125));
        bar_test_1->setMinimumSize(QSize(125, 125));
        bar_test_1->setMaximumSize(QSize(125, 125));
        bar_test_2 = new RingsMapProgressbar(TestCupDraw);
        bar_test_2->setObjectName(QStringLiteral("bar_test_2"));
        bar_test_2->setGeometry(QRect(150, 30, 125, 125));
        bar_test_2->setMinimumSize(QSize(125, 125));
        bar_test_2->setMaximumSize(QSize(125, 125));
        bar_test_3 = new RingsMapProgressbar(TestCupDraw);
        bar_test_3->setObjectName(QStringLiteral("bar_test_3"));
        bar_test_3->setGeometry(QRect(300, 30, 125, 125));
        bar_test_3->setMinimumSize(QSize(125, 125));
        bar_test_3->setMaximumSize(QSize(125, 125));
        bar_test_4 = new RingsMapProgressbar(TestCupDraw);
        bar_test_4->setObjectName(QStringLiteral("bar_test_4"));
        bar_test_4->setGeometry(QRect(450, 10, 125, 125));
        bar_test_4->setMinimumSize(QSize(125, 125));
        bar_test_4->setMaximumSize(QSize(125, 125));
        bar_test_7 = new RingsMapProgressbar(TestCupDraw);
        bar_test_7->setObjectName(QStringLiteral("bar_test_7"));
        bar_test_7->setGeometry(QRect(610, 20, 125, 125));
        bar_test_7->setMinimumSize(QSize(125, 125));
        bar_test_7->setMaximumSize(QSize(125, 125));
        bar_test_12 = new RingsMapProgressbar(TestCupDraw);
        bar_test_12->setObjectName(QStringLiteral("bar_test_12"));
        bar_test_12->setGeometry(QRect(750, 20, 125, 125));
        bar_test_12->setMinimumSize(QSize(125, 125));
        bar_test_12->setMaximumSize(QSize(125, 125));
        bar_test_11 = new RingsMapProgressbar(TestCupDraw);
        bar_test_11->setObjectName(QStringLiteral("bar_test_11"));
        bar_test_11->setGeometry(QRect(970, 60, 125, 125));
        bar_test_11->setMinimumSize(QSize(125, 125));
        bar_test_11->setMaximumSize(QSize(125, 125));
        bar_test_5 = new RingsMapProgressbar(TestCupDraw);
        bar_test_5->setObjectName(QStringLiteral("bar_test_5"));
        bar_test_5->setGeometry(QRect(1320, 240, 125, 125));
        bar_test_5->setMinimumSize(QSize(125, 125));
        bar_test_5->setMaximumSize(QSize(125, 125));
        bar_test_8 = new RingsMapProgressbar(TestCupDraw);
        bar_test_8->setObjectName(QStringLiteral("bar_test_8"));
        bar_test_8->setGeometry(QRect(1540, 270, 125, 125));
        bar_test_8->setMinimumSize(QSize(125, 125));
        bar_test_8->setMaximumSize(QSize(125, 125));
        bar_test_10 = new RingsMapProgressbar(TestCupDraw);
        bar_test_10->setObjectName(QStringLiteral("bar_test_10"));
        bar_test_10->setGeometry(QRect(1730, 260, 125, 125));
        bar_test_10->setMinimumSize(QSize(125, 125));
        bar_test_10->setMaximumSize(QSize(125, 125));
        bar_test_9 = new RingsMapProgressbar(TestCupDraw);
        bar_test_9->setObjectName(QStringLiteral("bar_test_9"));
        bar_test_9->setGeometry(QRect(1910, 270, 125, 125));
        bar_test_9->setMinimumSize(QSize(125, 125));
        bar_test_9->setMaximumSize(QSize(125, 125));
        bar_test_6 = new RingsMapProgressbar(TestCupDraw);
        bar_test_6->setObjectName(QStringLiteral("bar_test_6"));
        bar_test_6->setGeometry(QRect(2070, 270, 125, 125));
        bar_test_6->setMinimumSize(QSize(125, 125));
        bar_test_6->setMaximumSize(QSize(125, 125));

        retranslateUi(TestCupDraw);

        QMetaObject::connectSlotsByName(TestCupDraw);
    } // setupUi

    void retranslateUi(QWidget *TestCupDraw)
    {
        TestCupDraw->setWindowTitle(QApplication::translate("TestCupDraw", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TestCupDraw: public Ui_TestCupDraw {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTCUPDRAW_H
