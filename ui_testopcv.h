/********************************************************************************
** Form generated from reading UI file 'testopcv.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TESTOPCV_H
#define UI_TESTOPCV_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TestOpcv
{
public:
    QGridLayout *gridLayout;
    QWidget *widget_bar;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QSpinBox *spinBox;
    QSpacerItem *horizontalSpacer;
    QLabel *label_ratio;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QWidget *widget_2;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_loadpath;
    QPushButton *pushButton_prp;
    QPushButton *pushButton_test;
    QLabel *label_showimage;

    void setupUi(QWidget *TestOpcv)
    {
        if (TestOpcv->objectName().isEmpty())
            TestOpcv->setObjectName(QStringLiteral("TestOpcv"));
        TestOpcv->resize(1020, 670);
        gridLayout = new QGridLayout(TestOpcv);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        widget_bar = new QWidget(TestOpcv);
        widget_bar->setObjectName(QStringLiteral("widget_bar"));
        widget_bar->setMinimumSize(QSize(0, 30));
        widget_bar->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_2 = new QHBoxLayout(widget_bar);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 0, 5, 0);
        label = new QLabel(widget_bar);
        label->setObjectName(QStringLiteral("label"));
        label->setMinimumSize(QSize(0, 25));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(12);
        label->setFont(font);

        horizontalLayout_2->addWidget(label);

        spinBox = new QSpinBox(widget_bar);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimumSize(QSize(140, 25));
        spinBox->setFont(font);
        spinBox->setAlignment(Qt::AlignCenter);
        spinBox->setMinimum(-360);
        spinBox->setMaximum(360);
        spinBox->setSingleStep(90);

        horizontalLayout_2->addWidget(spinBox);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        label_ratio = new QLabel(widget_bar);
        label_ratio->setObjectName(QStringLiteral("label_ratio"));
        label_ratio->setMinimumSize(QSize(120, 0));
        label_ratio->setFont(font);
        label_ratio->setFrameShape(QFrame::NoFrame);

        horizontalLayout_2->addWidget(label_ratio);


        gridLayout->addWidget(widget_bar, 1, 0, 1, 2);

        widget = new QWidget(TestOpcv);
        widget->setObjectName(QStringLiteral("widget"));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        widget_2 = new QWidget(widget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMinimumSize(QSize(0, 90));
        widget_2->setMaximumSize(QSize(16777215, 90));
        gridLayout_2 = new QGridLayout(widget_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        pushButton_loadpath = new QPushButton(widget_2);
        pushButton_loadpath->setObjectName(QStringLiteral("pushButton_loadpath"));
        pushButton_loadpath->setMinimumSize(QSize(0, 28));

        gridLayout_2->addWidget(pushButton_loadpath, 0, 0, 1, 1);

        pushButton_prp = new QPushButton(widget_2);
        pushButton_prp->setObjectName(QStringLiteral("pushButton_prp"));
        pushButton_prp->setMinimumSize(QSize(0, 28));

        gridLayout_2->addWidget(pushButton_prp, 0, 2, 1, 1);

        pushButton_test = new QPushButton(widget_2);
        pushButton_test->setObjectName(QStringLiteral("pushButton_test"));
        pushButton_test->setMinimumSize(QSize(0, 28));

        gridLayout_2->addWidget(pushButton_test, 1, 0, 1, 1);


        verticalLayout->addWidget(widget_2);

        label_showimage = new QLabel(widget);
        label_showimage->setObjectName(QStringLiteral("label_showimage"));
        label_showimage->setFrameShape(QFrame::Box);
        label_showimage->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_showimage);


        gridLayout->addWidget(widget, 0, 0, 1, 2);


        retranslateUi(TestOpcv);

        QMetaObject::connectSlotsByName(TestOpcv);
    } // setupUi

    void retranslateUi(QWidget *TestOpcv)
    {
        TestOpcv->setWindowTitle(QApplication::translate("TestOpcv", "Form", nullptr));
        label->setText(QApplication::translate("TestOpcv", "\346\227\213\350\275\254\350\247\222\345\272\246:", nullptr));
        label_ratio->setText(QString());
        pushButton_loadpath->setText(QApplication::translate("TestOpcv", "\345\257\274\345\205\245\345\233\276\347\211\207", nullptr));
        pushButton_prp->setText(QApplication::translate("TestOpcv", "PRP\351\230\210\345\200\274", nullptr));
        pushButton_test->setText(QApplication::translate("TestOpcv", "PushButton", nullptr));
        label_showimage->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TestOpcv: public Ui_TestOpcv {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTOPCV_H
