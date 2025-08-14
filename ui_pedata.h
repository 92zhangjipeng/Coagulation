/********************************************************************************
** Form generated from reading UI file 'pedata.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEDATA_H
#define UI_PEDATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PEdata
{
public:
    QVBoxLayout *verticalLayout;
    QGroupBox *groupBoxHeigh;
    QGridLayout *gridLayout;
    QLabel *label_input;
    QLabel *label_max;
    QLabel *label_vage;
    QLabel *label_min;
    QLabel *label_cv;
    QGroupBox *groupBoxMid;
    QGridLayout *gridLayout_2;
    QLabel *labelviewMid;
    QLabel *labelMidMax;
    QLabel *labelMidVage;
    QLabel *labelMidMin;
    QLabel *labelMidcv;
    QGroupBox *groupBoxLow;
    QGridLayout *gridLayout_3;
    QLabel *labeviewLowData;
    QLabel *labelLowMax;
    QLabel *labelLowvage;
    QLabel *labelLowMin;
    QLabel *labellowcv;
    QPushButton *pushButton_pushout;

    void setupUi(QWidget *PEdata)
    {
        if (PEdata->objectName().isEmpty())
            PEdata->setObjectName(QStringLiteral("PEdata"));
        PEdata->resize(444, 536);
        PEdata->setMinimumSize(QSize(0, 400));
        PEdata->setMaximumSize(QSize(570, 550));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(14);
        PEdata->setFont(font);
        verticalLayout = new QVBoxLayout(PEdata);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        groupBoxHeigh = new QGroupBox(PEdata);
        groupBoxHeigh->setObjectName(QStringLiteral("groupBoxHeigh"));
        groupBoxHeigh->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 1px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}"));
        gridLayout = new QGridLayout(groupBoxHeigh);
        gridLayout->setSpacing(5);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setContentsMargins(5, 5, 5, 5);
        label_input = new QLabel(groupBoxHeigh);
        label_input->setObjectName(QStringLiteral("label_input"));
        label_input->setMinimumSize(QSize(0, 50));
        label_input->setMaximumSize(QSize(16777215, 100));
        label_input->setFrameShape(QFrame::Box);
        label_input->setWordWrap(true);

        gridLayout->addWidget(label_input, 0, 0, 1, 2);

        label_max = new QLabel(groupBoxHeigh);
        label_max->setObjectName(QStringLiteral("label_max"));
        label_max->setMinimumSize(QSize(0, 30));

        gridLayout->addWidget(label_max, 1, 0, 1, 1);

        label_vage = new QLabel(groupBoxHeigh);
        label_vage->setObjectName(QStringLiteral("label_vage"));

        gridLayout->addWidget(label_vage, 1, 1, 1, 1);

        label_min = new QLabel(groupBoxHeigh);
        label_min->setObjectName(QStringLiteral("label_min"));

        gridLayout->addWidget(label_min, 2, 0, 1, 1);

        label_cv = new QLabel(groupBoxHeigh);
        label_cv->setObjectName(QStringLiteral("label_cv"));

        gridLayout->addWidget(label_cv, 2, 1, 1, 1);


        verticalLayout->addWidget(groupBoxHeigh);

        groupBoxMid = new QGroupBox(PEdata);
        groupBoxMid->setObjectName(QStringLiteral("groupBoxMid"));
        groupBoxMid->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 1px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}"));
        gridLayout_2 = new QGridLayout(groupBoxMid);
        gridLayout_2->setSpacing(5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        gridLayout_2->setContentsMargins(5, 5, 5, 5);
        labelviewMid = new QLabel(groupBoxMid);
        labelviewMid->setObjectName(QStringLiteral("labelviewMid"));
        labelviewMid->setMinimumSize(QSize(0, 50));
        labelviewMid->setMaximumSize(QSize(16777215, 100));
        labelviewMid->setFrameShape(QFrame::Box);

        gridLayout_2->addWidget(labelviewMid, 0, 0, 1, 2);

        labelMidMax = new QLabel(groupBoxMid);
        labelMidMax->setObjectName(QStringLiteral("labelMidMax"));
        labelMidMax->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(labelMidMax, 1, 0, 1, 1);

        labelMidVage = new QLabel(groupBoxMid);
        labelMidVage->setObjectName(QStringLiteral("labelMidVage"));
        labelMidVage->setMinimumSize(QSize(30, 30));

        gridLayout_2->addWidget(labelMidVage, 1, 1, 1, 1);

        labelMidMin = new QLabel(groupBoxMid);
        labelMidMin->setObjectName(QStringLiteral("labelMidMin"));
        labelMidMin->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(labelMidMin, 2, 0, 1, 1);

        labelMidcv = new QLabel(groupBoxMid);
        labelMidcv->setObjectName(QStringLiteral("labelMidcv"));
        labelMidcv->setMinimumSize(QSize(0, 30));

        gridLayout_2->addWidget(labelMidcv, 2, 1, 1, 1);


        verticalLayout->addWidget(groupBoxMid);

        groupBoxLow = new QGroupBox(PEdata);
        groupBoxLow->setObjectName(QStringLiteral("groupBoxLow"));
        groupBoxLow->setStyleSheet(QString::fromUtf8("QGroupBox\n"
"{\n"
"	border: 1px solid gray;\n"
"	border-radius:10px;\n"
"	margin-top:4ex;\n"
"	font-family:\346\245\267\344\275\223;\n"
"	font:bold 18px;\n"
"} \n"
"QGroupBox::title\n"
"{\n"
"	subcontrol-origin:margin;\n"
"	subcontrol-position:top center;\n"
"	padding:1px;\n"
"}\n"
"QGroupBox::enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}\n"
"QGroupBox::!enabled\n"
"{\n"
"	border: 1px solid gray;\n"
"}"));
        gridLayout_3 = new QGridLayout(groupBoxLow);
        gridLayout_3->setSpacing(5);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        gridLayout_3->setContentsMargins(5, 5, 5, 5);
        labeviewLowData = new QLabel(groupBoxLow);
        labeviewLowData->setObjectName(QStringLiteral("labeviewLowData"));
        labeviewLowData->setMinimumSize(QSize(0, 50));
        labeviewLowData->setMaximumSize(QSize(16777215, 100));
        labeviewLowData->setFrameShape(QFrame::Box);

        gridLayout_3->addWidget(labeviewLowData, 0, 0, 1, 2);

        labelLowMax = new QLabel(groupBoxLow);
        labelLowMax->setObjectName(QStringLiteral("labelLowMax"));
        labelLowMax->setMinimumSize(QSize(0, 30));

        gridLayout_3->addWidget(labelLowMax, 1, 0, 1, 1);

        labelLowvage = new QLabel(groupBoxLow);
        labelLowvage->setObjectName(QStringLiteral("labelLowvage"));
        labelLowvage->setMinimumSize(QSize(0, 30));

        gridLayout_3->addWidget(labelLowvage, 1, 1, 1, 1);

        labelLowMin = new QLabel(groupBoxLow);
        labelLowMin->setObjectName(QStringLiteral("labelLowMin"));
        labelLowMin->setMinimumSize(QSize(0, 30));

        gridLayout_3->addWidget(labelLowMin, 2, 0, 1, 1);

        labellowcv = new QLabel(groupBoxLow);
        labellowcv->setObjectName(QStringLiteral("labellowcv"));
        labellowcv->setMinimumSize(QSize(0, 30));

        gridLayout_3->addWidget(labellowcv, 2, 1, 1, 1);


        verticalLayout->addWidget(groupBoxLow);

        pushButton_pushout = new QPushButton(PEdata);
        pushButton_pushout->setObjectName(QStringLiteral("pushButton_pushout"));
        pushButton_pushout->setMinimumSize(QSize(0, 30));

        verticalLayout->addWidget(pushButton_pushout);


        retranslateUi(PEdata);

        QMetaObject::connectSlotsByName(PEdata);
    } // setupUi

    void retranslateUi(QWidget *PEdata)
    {
        PEdata->setWindowTitle(QApplication::translate("PEdata", "Form", nullptr));
        groupBoxHeigh->setTitle(QApplication::translate("PEdata", "\351\253\230\345\200\274", nullptr));
        label_input->setText(QString());
        label_max->setText(QApplication::translate("PEdata", "\346\234\200\345\244\247\345\200\274:", nullptr));
        label_vage->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        label_min->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        label_cv->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        groupBoxMid->setTitle(QApplication::translate("PEdata", "\344\270\255\345\200\274", nullptr));
        labelviewMid->setText(QString());
        labelMidMax->setText(QApplication::translate("PEdata", "\346\234\200\345\244\247\345\200\274:", nullptr));
        labelMidVage->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        labelMidMin->setText(QApplication::translate("PEdata", "\346\234\200\345\260\217\345\200\274:", nullptr));
        labelMidcv->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        groupBoxLow->setTitle(QApplication::translate("PEdata", "\344\275\216\345\200\274", nullptr));
        labeviewLowData->setText(QString());
        labelLowMax->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        labelLowvage->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        labelLowMin->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        labellowcv->setText(QApplication::translate("PEdata", "TextLabel", nullptr));
        pushButton_pushout->setText(QApplication::translate("PEdata", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PEdata: public Ui_PEdata {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEDATA_H
