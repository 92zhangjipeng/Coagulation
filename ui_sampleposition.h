/********************************************************************************
** Form generated from reading UI file 'sampleposition.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SAMPLEPOSITION_H
#define UI_SAMPLEPOSITION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SamplePosition
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer;
    QWidget *widget_titale;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label_text;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QWidget *SamplePosition)
    {
        if (SamplePosition->objectName().isEmpty())
            SamplePosition->setObjectName(QStringLiteral("SamplePosition"));
        SamplePosition->resize(579, 382);
        SamplePosition->setMinimumSize(QSize(320, 360));
        SamplePosition->setMaximumSize(QSize(111111, 1111111));
        QFont font;
        font.setPointSize(14);
        SamplePosition->setFont(font);
        SamplePosition->setStyleSheet(QStringLiteral(""));
        verticalLayout = new QVBoxLayout(SamplePosition);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 15);
        verticalSpacer = new QSpacerItem(20, 319, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        widget_titale = new QWidget(SamplePosition);
        widget_titale->setObjectName(QStringLiteral("widget_titale"));
        widget_titale->setMaximumSize(QSize(16777214, 40));
        widget_titale->setStyleSheet(QStringLiteral(""));
        horizontalLayout = new QHBoxLayout(widget_titale);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_text = new QLabel(widget_titale);
        label_text->setObjectName(QStringLiteral("label_text"));
        label_text->setStyleSheet(QString::fromUtf8("font-family: \"\346\226\260\345\256\213\344\275\223\";\n"
"font-size: 14px;\n"
"font-style: italic;\n"
"font-weight: bold;\n"
"color: #FF0000;\n"
"font:  24px \"\346\226\260\345\256\213\344\275\223\";"));

        horizontalLayout->addWidget(label_text);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);


        verticalLayout->addWidget(widget_titale);


        retranslateUi(SamplePosition);

        QMetaObject::connectSlotsByName(SamplePosition);
    } // setupUi

    void retranslateUi(QWidget *SamplePosition)
    {
        SamplePosition->setWindowTitle(QApplication::translate("SamplePosition", "Form", nullptr));
        label_text->setText(QApplication::translate("SamplePosition", "\350\241\200\346\240\267\347\233\230-A", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SamplePosition: public Ui_SamplePosition {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SAMPLEPOSITION_H
