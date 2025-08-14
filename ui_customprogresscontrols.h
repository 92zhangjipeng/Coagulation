/********************************************************************************
** Form generated from reading UI file 'customprogresscontrols.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMPROGRESSCONTROLS_H
#define UI_CUSTOMPROGRESSCONTROLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_customProgresscontrols
{
public:
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QLabel *label_ratio;
    QProgressBar *progressBar_ratio;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *customProgresscontrols)
    {
        if (customProgresscontrols->objectName().isEmpty())
            customProgresscontrols->setObjectName(QStringLiteral("customProgresscontrols"));
        customProgresscontrols->resize(400, 160);
        customProgresscontrols->setMaximumSize(QSize(400, 160));
        customProgresscontrols->setStyleSheet(QString::fromUtf8("QWidget#customProgresscontrols\n"
"{  \n"
"  border: 1px solid gray;\n"
"  border-radius:10px;\n"
"  margin-top:0ex;\n"
"  font-family:\346\245\267\344\275\223;\n"
"  font:bold 18px;	\n"
"} \n"
""));
        verticalLayout = new QVBoxLayout(customProgresscontrols);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 10, 5, 5);
        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        label_ratio = new QLabel(customProgresscontrols);
        label_ratio->setObjectName(QStringLiteral("label_ratio"));
        label_ratio->setMinimumSize(QSize(0, 35));
        label_ratio->setMaximumSize(QSize(16777215, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(20);
        label_ratio->setFont(font);
        label_ratio->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label_ratio);

        progressBar_ratio = new QProgressBar(customProgresscontrols);
        progressBar_ratio->setObjectName(QStringLiteral("progressBar_ratio"));
        progressBar_ratio->setMinimumSize(QSize(0, 40));
        progressBar_ratio->setMaximumSize(QSize(16777215, 40));
        progressBar_ratio->setValue(24);

        verticalLayout->addWidget(progressBar_ratio);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        retranslateUi(customProgresscontrols);

        QMetaObject::connectSlotsByName(customProgresscontrols);
    } // setupUi

    void retranslateUi(QWidget *customProgresscontrols)
    {
        customProgresscontrols->setWindowTitle(QApplication::translate("customProgresscontrols", "Form", nullptr));
        label_ratio->setText(QApplication::translate("customProgresscontrols", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class customProgresscontrols: public Ui_customProgresscontrols {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMPROGRESSCONTROLS_H
