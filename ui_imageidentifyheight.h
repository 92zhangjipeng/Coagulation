/********************************************************************************
** Form generated from reading UI file 'imageidentifyheight.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGEIDENTIFYHEIGHT_H
#define UI_IMAGEIDENTIFYHEIGHT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include "qvideowidget.h"

QT_BEGIN_NAMESPACE

class Ui_ImageiDentifyHeight
{
public:
    QVideoWidget *widget;
    QHBoxLayout *horizontalLayout_2;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QComboBox *cameraType;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QDialog *ImageiDentifyHeight)
    {
        if (ImageiDentifyHeight->objectName().isEmpty())
            ImageiDentifyHeight->setObjectName(QStringLiteral("ImageiDentifyHeight"));
        ImageiDentifyHeight->resize(940, 694);
        widget = new QVideoWidget(ImageiDentifyHeight);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(10, 50, 931, 641));
        widget->setMinimumSize(QSize(900, 600));
        horizontalLayout_2 = new QHBoxLayout(widget);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        layoutWidget = new QWidget(ImageiDentifyHeight);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 10, 841, 32));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        cameraType = new QComboBox(layoutWidget);
        cameraType->setObjectName(QStringLiteral("cameraType"));
        cameraType->setMinimumSize(QSize(0, 30));

        horizontalLayout->addWidget(cameraType);

        pushButton = new QPushButton(layoutWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(0, 30));
        pushButton->setMaximumSize(QSize(80, 30));

        horizontalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(layoutWidget);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setMinimumSize(QSize(0, 30));
        pushButton_2->setMaximumSize(QSize(80, 30));

        horizontalLayout->addWidget(pushButton_2);


        retranslateUi(ImageiDentifyHeight);

        QMetaObject::connectSlotsByName(ImageiDentifyHeight);
    } // setupUi

    void retranslateUi(QDialog *ImageiDentifyHeight)
    {
        ImageiDentifyHeight->setWindowTitle(QApplication::translate("ImageiDentifyHeight", "Dialog", nullptr));
        pushButton->setText(QApplication::translate("ImageiDentifyHeight", "\347\241\256\345\256\232", nullptr));
        pushButton_2->setText(QApplication::translate("ImageiDentifyHeight", "\344\277\235\345\255\230", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ImageiDentifyHeight: public Ui_ImageiDentifyHeight {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGEIDENTIFYHEIGHT_H
