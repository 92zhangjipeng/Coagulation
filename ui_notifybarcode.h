/********************************************************************************
** Form generated from reading UI file 'notifybarcode.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NOTIFYBARCODE_H
#define UI_NOTIFYBARCODE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_NotifyBarCode
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget_2;
    QGridLayout *gridLayout_bar;
    QLabel *label_name;
    QLineEdit *lineEdit_barcode;
    QToolButton *toolButton_cancel;
    QToolButton *toolButton_save;

    void setupUi(QWidget *NotifyBarCode)
    {
        if (NotifyBarCode->objectName().isEmpty())
            NotifyBarCode->setObjectName(QStringLiteral("NotifyBarCode"));
        NotifyBarCode->resize(360, 118);
        NotifyBarCode->setMaximumSize(QSize(360, 16777215));
        NotifyBarCode->setStyleSheet(QStringLiteral("background-color: #E8F4F8;"));
        verticalLayout = new QVBoxLayout(NotifyBarCode);
        verticalLayout->setSpacing(1);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(1, 1, 1, 1);
        widget_2 = new QWidget(NotifyBarCode);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setStyleSheet(QLatin1String("background-color: #E8F4F8;\n"
"border-color: #B8D9E8;\n"
"border-top-color: #B8D9E8;"));
        gridLayout_bar = new QGridLayout(widget_2);
        gridLayout_bar->setObjectName(QStringLiteral("gridLayout_bar"));
        label_name = new QLabel(widget_2);
        label_name->setObjectName(QStringLiteral("label_name"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(15);
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(2);
        label_name->setFont(font);
        label_name->setStyleSheet(QString::fromUtf8("font: 17 15pt '\345\276\256\350\275\257\351\233\205\351\273\221';\n"
"background-color: rgba(232, 244, 248, 255);\n"
"border-radius:5px;\n"
"padding:2px 4px;\n"
"color: #1A6B8A;\n"
"border-style: flat;\n"
"background: transparent;"));

        gridLayout_bar->addWidget(label_name, 0, 0, 1, 1);

        lineEdit_barcode = new QLineEdit(widget_2);
        lineEdit_barcode->setObjectName(QStringLiteral("lineEdit_barcode"));
        lineEdit_barcode->setStyleSheet(QString::fromUtf8("QLineEdit {\n"
"    border-radius:5px;\n"
"    padding:2px 4px;\n"
"    font: 17 15pt '\345\276\256\350\275\257\351\233\205\351\273\221';\n"
"    border: 2px solid #A0C4D9;\n"
"    background-color: #F0F8FC;\n"
"    color: #1A6B8A;\n"
"}\n"
"\n"
"QLineEdit:hover {\n"
"    border: 2px solid #6FB3D2;\n"
"    border-radius: 5px;\n"
"    background-color: #FFFFFF;\n"
"    color: #0E4A63;\n"
"    selection-background-color: #6FB3D2;\n"
"    selection-color: #FFFFFF;\n"
"}\n"
"\n"
"QLineEdit:focus {\n"
"    border: 2px solid #4A90B0;\n"
"    background-color: #FFFFFF;\n"
"}\n"
"\n"
"QLineEdit[echoMode=\"2\"] {\n"
"    lineedit-password-character: 9679;\n"
"    lineedit-password-mask-delay: 2000;\n"
"}\n"
"\n"
"QLineEdit:disabled {\n"
"    border: 1px solid #CDCDCD;\n"
"    background-color: #E0E0E0;\n"
"    color: #909090;\n"
"}\n"
"\n"
"QLineEdit:read-only {\n"
"    background-color: #D4EAF3;\n"
"    color: #6FB3D2;\n"
"}"));

        gridLayout_bar->addWidget(lineEdit_barcode, 0, 1, 1, 2);

        toolButton_cancel = new QToolButton(widget_2);
        toolButton_cancel->setObjectName(QStringLiteral("toolButton_cancel"));
        toolButton_cancel->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_cancel{\n"
"    border: 1px solid #A0C4D9;\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #6FB3D2, stop:1 #4A90B0);\n"
"    border-style: solid;\n"
"    border-radius:10px;\n"
"    width: 130px;\n"
"    height:30px;\n"
"    padding:0 10px;\n"
"    font-family:'\345\276\256\350\275\257\351\233\205\351\273\221';\n"
"    font-size:20px;\n"
"    color: white;\n"
"}\n"
"QToolButton#toolButton_cancel:hover{\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #8AC5E0, stop:1 #6FB3D2);\n"
"}\n"
"QToolButton#toolButton_cancel:pressed{\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #4A90B0, stop:1 #2C6F8F);\n"
"}"));
        toolButton_cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_bar->addWidget(toolButton_cancel, 1, 2, 1, 1);

        toolButton_save = new QToolButton(widget_2);
        toolButton_save->setObjectName(QStringLiteral("toolButton_save"));
        toolButton_save->setMaximumSize(QSize(177777, 16777215));
        toolButton_save->setStyleSheet(QString::fromUtf8("QToolButton#toolButton_save{\n"
"    border: 1px solid #A0C4D9;\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #6FB3D2, stop:1 #4A90B0);\n"
"    border-style: solid;\n"
"    border-radius:10px;\n"
"    width: 130px;\n"
"    height:30px;\n"
"    padding:0 10px;\n"
"    font-family:'\345\276\256\350\275\257\351\233\205\351\273\221';\n"
"    font-size:20px;\n"
"    color: white;\n"
"}\n"
"QToolButton#toolButton_save:hover{\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #8AC5E0, stop:1 #6FB3D2);\n"
"}\n"
"QToolButton#toolButton_save:pressed{\n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 #4A90B0, stop:1 #2C6F8F);\n"
"}"));
        toolButton_save->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        gridLayout_bar->addWidget(toolButton_save, 1, 0, 1, 1);


        verticalLayout->addWidget(widget_2);


        retranslateUi(NotifyBarCode);

        QMetaObject::connectSlotsByName(NotifyBarCode);
    } // setupUi

    void retranslateUi(QWidget *NotifyBarCode)
    {
        NotifyBarCode->setWindowTitle(QString());
        label_name->setText(QApplication::translate("NotifyBarCode", "\344\277\256\346\224\271\346\235\241\345\275\242\347\240\201:", nullptr));
        toolButton_cancel->setText(QApplication::translate("NotifyBarCode", "\345\217\226\346\266\210", nullptr));
        toolButton_save->setText(QApplication::translate("NotifyBarCode", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class NotifyBarCode: public Ui_NotifyBarCode {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NOTIFYBARCODE_H
