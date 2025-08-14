/********************************************************************************
** Form generated from reading UI file 'batchaddsample.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BATCHADDSAMPLE_H
#define UI_BATCHADDSAMPLE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_batchAddSample
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_id;
    QLineEdit *lineEditAddFrom;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_id2;
    QLineEdit *lineEditAaddEnd;
    QVBoxLayout *verticalLayout_4;
    QLabel *label_pindown;
    QDoubleSpinBox *doubleSpinBox_pindown;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_project;
    QComboBox *comboBox_testproject;
    QWidget *widget;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_save;
    QPushButton *pushButton_cancel;

    void setupUi(QWidget *batchAddSample)
    {
        if (batchAddSample->objectName().isEmpty())
            batchAddSample->setObjectName(QStringLiteral("batchAddSample"));
        batchAddSample->resize(352, 374);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(12);
        batchAddSample->setFont(font);
        verticalLayout = new QVBoxLayout(batchAddSample);
        verticalLayout->setSpacing(10);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 10, 5, 5);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        label_id = new QLabel(batchAddSample);
        label_id->setObjectName(QStringLiteral("label_id"));
        label_id->setMinimumSize(QSize(0, 20));
        label_id->setMaximumSize(QSize(90, 30));
        label_id->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_2->addWidget(label_id);

        lineEditAddFrom = new QLineEdit(batchAddSample);
        lineEditAddFrom->setObjectName(QStringLiteral("lineEditAddFrom"));
        lineEditAddFrom->setMinimumSize(QSize(247, 30));
        lineEditAddFrom->setMaximumSize(QSize(16777215, 16777215));
        lineEditAddFrom->setAlignment(Qt::AlignCenter);

        verticalLayout_2->addWidget(lineEditAddFrom);


        verticalLayout->addLayout(verticalLayout_2);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_id2 = new QLabel(batchAddSample);
        label_id2->setObjectName(QStringLiteral("label_id2"));
        label_id2->setMinimumSize(QSize(0, 20));
        label_id2->setMaximumSize(QSize(90, 30));
        label_id2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_3->addWidget(label_id2);

        lineEditAaddEnd = new QLineEdit(batchAddSample);
        lineEditAaddEnd->setObjectName(QStringLiteral("lineEditAaddEnd"));
        lineEditAaddEnd->setMinimumSize(QSize(247, 30));
        lineEditAaddEnd->setMaximumSize(QSize(16777215, 16777215));
        lineEditAaddEnd->setAlignment(Qt::AlignCenter);

        verticalLayout_3->addWidget(lineEditAaddEnd);


        verticalLayout->addLayout(verticalLayout_3);

        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(5);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        label_pindown = new QLabel(batchAddSample);
        label_pindown->setObjectName(QStringLiteral("label_pindown"));
        label_pindown->setMinimumSize(QSize(0, 30));
        label_pindown->setMaximumSize(QSize(90, 30));
        label_pindown->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_4->addWidget(label_pindown);

        doubleSpinBox_pindown = new QDoubleSpinBox(batchAddSample);
        doubleSpinBox_pindown->setObjectName(QStringLiteral("doubleSpinBox_pindown"));
        doubleSpinBox_pindown->setMinimumSize(QSize(0, 30));
        doubleSpinBox_pindown->setMaximumSize(QSize(16777215, 30));
        doubleSpinBox_pindown->setAlignment(Qt::AlignCenter);

        verticalLayout_4->addWidget(doubleSpinBox_pindown);


        verticalLayout->addLayout(verticalLayout_4);

        verticalLayout_5 = new QVBoxLayout();
        verticalLayout_5->setSpacing(5);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_project = new QLabel(batchAddSample);
        label_project->setObjectName(QStringLiteral("label_project"));
        label_project->setMinimumSize(QSize(0, 30));
        label_project->setMaximumSize(QSize(90, 30));
        label_project->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        verticalLayout_5->addWidget(label_project);

        comboBox_testproject = new QComboBox(batchAddSample);
        comboBox_testproject->setObjectName(QStringLiteral("comboBox_testproject"));
        comboBox_testproject->setMinimumSize(QSize(0, 30));
        comboBox_testproject->setMaximumSize(QSize(16777215, 30));

        verticalLayout_5->addWidget(comboBox_testproject);


        verticalLayout->addLayout(verticalLayout_5);

        widget = new QWidget(batchAddSample);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 55));
        horizontalLayout_5 = new QHBoxLayout(widget);
        horizontalLayout_5->setSpacing(5);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_5->setContentsMargins(5, 5, 5, 5);
        pushButton_save = new QPushButton(widget);
        pushButton_save->setObjectName(QStringLiteral("pushButton_save"));
        pushButton_save->setMinimumSize(QSize(0, 40));
        pushButton_save->setMaximumSize(QSize(16777215, 40));

        horizontalLayout_5->addWidget(pushButton_save);

        pushButton_cancel = new QPushButton(widget);
        pushButton_cancel->setObjectName(QStringLiteral("pushButton_cancel"));
        pushButton_cancel->setMinimumSize(QSize(0, 40));
        pushButton_cancel->setMaximumSize(QSize(16777215, 40));

        horizontalLayout_5->addWidget(pushButton_cancel);


        verticalLayout->addWidget(widget);


        retranslateUi(batchAddSample);

        QMetaObject::connectSlotsByName(batchAddSample);
    } // setupUi

    void retranslateUi(QWidget *batchAddSample)
    {
        batchAddSample->setWindowTitle(QApplication::translate("batchAddSample", "Form", nullptr));
        label_id->setText(QApplication::translate("batchAddSample", "\350\265\267\345\247\213\346\240\267\346\234\254id:", nullptr));
        label_id2->setText(QApplication::translate("batchAddSample", "\347\273\223\346\235\237\346\240\267\346\234\254id:", nullptr));
        label_pindown->setText(QApplication::translate("batchAddSample", "\350\241\200\346\265\206\351\253\230\345\272\246:", nullptr));
        label_project->setText(QApplication::translate("batchAddSample", "\346\265\213\350\257\225\351\241\271\347\233\256:", nullptr));
        pushButton_save->setText(QApplication::translate("batchAddSample", "PushButton", nullptr));
        pushButton_cancel->setText(QApplication::translate("batchAddSample", "PushButton", nullptr));
    } // retranslateUi

};

namespace Ui {
    class batchAddSample: public Ui_batchAddSample {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BATCHADDSAMPLE_H
