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
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TestOpcv
{
public:
    QVBoxLayout *verticalLayoutOpencv;
    QLabel *label_ratio;
    QWidget *widgetShowImag;
    QVBoxLayout *verticalLayout;
    QLabel *label_showimage;
    QWidget *widget_bar;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_loadpath;
    QPushButton *pushButton_test;

    void setupUi(QWidget *TestOpcv)
    {
        if (TestOpcv->objectName().isEmpty())
            TestOpcv->setObjectName(QStringLiteral("TestOpcv"));
        TestOpcv->resize(493, 791);
        verticalLayoutOpencv = new QVBoxLayout(TestOpcv);
        verticalLayoutOpencv->setSpacing(0);
        verticalLayoutOpencv->setObjectName(QStringLiteral("verticalLayoutOpencv"));
        verticalLayoutOpencv->setContentsMargins(5, 0, 5, 0);
        label_ratio = new QLabel(TestOpcv);
        label_ratio->setObjectName(QStringLiteral("label_ratio"));
        label_ratio->setMinimumSize(QSize(120, 1));
        label_ratio->setMaximumSize(QSize(16777215, 150));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(12);
        label_ratio->setFont(font);
        label_ratio->setFrameShape(QFrame::NoFrame);
        label_ratio->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

        verticalLayoutOpencv->addWidget(label_ratio);

        widgetShowImag = new QWidget(TestOpcv);
        widgetShowImag->setObjectName(QStringLiteral("widgetShowImag"));
        widgetShowImag->setStyleSheet(QLatin1String("QWidget#widgetShowImag{  \n"
"    border: 1px solid rgba(220,220,220,1);\n"
"	background-color: rgb(188,187,183);\n"
"    border-style: solid;  \n"
"    border-radius:0px;  \n"
"    padding:0 0px;  	\n"
"}"));
        verticalLayout = new QVBoxLayout(widgetShowImag);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        label_showimage = new QLabel(widgetShowImag);
        label_showimage->setObjectName(QStringLiteral("label_showimage"));
        label_showimage->setFrameShape(QFrame::Box);
        label_showimage->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_showimage);


        verticalLayoutOpencv->addWidget(widgetShowImag);

        widget_bar = new QWidget(TestOpcv);
        widget_bar->setObjectName(QStringLiteral("widget_bar"));
        widget_bar->setMinimumSize(QSize(0, 30));
        widget_bar->setMaximumSize(QSize(16777215, 30));
        horizontalLayout_2 = new QHBoxLayout(widget_bar);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 0, 5, 0);
        pushButton_loadpath = new QPushButton(widget_bar);
        pushButton_loadpath->setObjectName(QStringLiteral("pushButton_loadpath"));
        pushButton_loadpath->setMinimumSize(QSize(0, 28));

        horizontalLayout_2->addWidget(pushButton_loadpath);

        pushButton_test = new QPushButton(widget_bar);
        pushButton_test->setObjectName(QStringLiteral("pushButton_test"));
        pushButton_test->setMinimumSize(QSize(0, 28));

        horizontalLayout_2->addWidget(pushButton_test);


        verticalLayoutOpencv->addWidget(widget_bar);


        retranslateUi(TestOpcv);

        QMetaObject::connectSlotsByName(TestOpcv);
    } // setupUi

    void retranslateUi(QWidget *TestOpcv)
    {
        TestOpcv->setWindowTitle(QApplication::translate("TestOpcv", "Form", nullptr));
        label_ratio->setText(QString());
        label_showimage->setText(QString());
        pushButton_loadpath->setText(QApplication::translate("TestOpcv", "\345\257\274\345\205\245\345\233\276\347\211\207", nullptr));
        pushButton_test->setText(QApplication::translate("TestOpcv", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TestOpcv: public Ui_TestOpcv {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TESTOPCV_H
