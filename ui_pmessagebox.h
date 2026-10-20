/********************************************************************************
** Form generated from reading UI file 'pmessagebox.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PMESSAGEBOX_H
#define UI_PMESSAGEBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PMessageBox
{
public:
    QVBoxLayout *verticalLayout_traylayout;
    QWidget *widget_title;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_showicon;
    QLabel *label_titlename;
    QSpacerItem *horizontalSpacer;
    QWidget *widget_TrayShow;
    QLabel *label_remindertext;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QPushButton *toolButton_OK;
    QPushButton *toolButton_Cancel;

    void setupUi(QDialog *PMessageBox)
    {
        if (PMessageBox->objectName().isEmpty())
            PMessageBox->setObjectName(QStringLiteral("PMessageBox"));
        PMessageBox->resize(450, 600);
        PMessageBox->setMinimumSize(QSize(450, 600));
        PMessageBox->setMaximumSize(QSize(450, 600));
        PMessageBox->setStyleSheet(QString::fromUtf8("QDialog#PMessageBox {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #f8fafc, stop:1 #e2e8f0);\n"
"    border: 2px solid #cbd5e1;\n"
"    border-radius: 12px;\n"
"    font-family: \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"}"));
        verticalLayout_traylayout = new QVBoxLayout(PMessageBox);
        verticalLayout_traylayout->setSpacing(8);
        verticalLayout_traylayout->setObjectName(QStringLiteral("verticalLayout_traylayout"));
        verticalLayout_traylayout->setContentsMargins(10, 10, 10, 10);
        widget_title = new QWidget(PMessageBox);
        widget_title->setObjectName(QStringLiteral("widget_title"));
        widget_title->setMinimumSize(QSize(0, 45));
        widget_title->setMaximumSize(QSize(16777215, 45));
        widget_title->setStyleSheet(QLatin1String("QWidget#widget_title {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #1e40af, stop:1 #1e3a8a);\n"
"    border: 1px solid #1e3a8a;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"}"));
        horizontalLayout_2 = new QHBoxLayout(widget_title);
        horizontalLayout_2->setSpacing(10);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(10, 5, 10, 5);
        label_showicon = new QLabel(widget_title);
        label_showicon->setObjectName(QStringLiteral("label_showicon"));
        label_showicon->setMinimumSize(QSize(32, 32));
        label_showicon->setMaximumSize(QSize(32, 32));
        label_showicon->setStyleSheet(QLatin1String("QLabel {\n"
"    background: transparent;\n"
"    border: none;\n"
"}"));

        horizontalLayout_2->addWidget(label_showicon);

        label_titlename = new QLabel(widget_title);
        label_titlename->setObjectName(QStringLiteral("label_titlename"));
        label_titlename->setMinimumSize(QSize(0, 35));
        label_titlename->setMaximumSize(QSize(16777215, 35));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        label_titlename->setFont(font);
        label_titlename->setStyleSheet(QLatin1String("QLabel {\n"
"    color: white;\n"
"    background: transparent;\n"
"    padding: 5px;\n"
"    font-weight: bold;\n"
"}"));
        label_titlename->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label_titlename);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_traylayout->addWidget(widget_title);

        widget_TrayShow = new QWidget(PMessageBox);
        widget_TrayShow->setObjectName(QStringLiteral("widget_TrayShow"));
        widget_TrayShow->setMinimumSize(QSize(430, 400));
        widget_TrayShow->setMaximumSize(QSize(430, 400));
        widget_TrayShow->setStyleSheet(QLatin1String("QWidget#widget_TrayShow {\n"
"    background: white;\n"
"    border: 2px solid #e2e8f0;\n"
"    border-radius: 8px;\n"
"    padding: 10px;\n"
"}"));

        verticalLayout_traylayout->addWidget(widget_TrayShow);

        label_remindertext = new QLabel(PMessageBox);
        label_remindertext->setObjectName(QStringLiteral("label_remindertext"));
        label_remindertext->setMinimumSize(QSize(0, 40));
        label_remindertext->setMaximumSize(QSize(16777215, 40));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font1.setPointSize(12);
        font1.setBold(true);
        font1.setWeight(75);
        label_remindertext->setFont(font1);
        label_remindertext->setStyleSheet(QLatin1String("QLabel {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #fef3c7, stop:1 #fde68a);\n"
"    border: 1px solid #f59e0b;\n"
"    border-radius: 6px;\n"
"    color: #92400e;\n"
"    padding: 8px;\n"
"    text-align: center;\n"
"}"));
        label_remindertext->setAlignment(Qt::AlignCenter);
        label_remindertext->setWordWrap(true);

        verticalLayout_traylayout->addWidget(label_remindertext);

        widget = new QWidget(PMessageBox);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 50));
        widget->setMaximumSize(QSize(16777215, 50));
        widget->setStyleSheet(QLatin1String("QWidget {\n"
"    background: transparent;\n"
"}"));
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setSpacing(20);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(40, 5, 40, 5);
        toolButton_OK = new QPushButton(widget);
        toolButton_OK->setObjectName(QStringLiteral("toolButton_OK"));
        toolButton_OK->setMinimumSize(QSize(120, 40));
        toolButton_OK->setMaximumSize(QSize(120, 40));
        toolButton_OK->setFont(font1);
        toolButton_OK->setStyleSheet(QLatin1String("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #10b981, stop:1 #059669);\n"
"    border: 2px solid #047857;\n"
"    border-radius: 8px;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"    padding: 8px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #34d399, stop:1 #10b981);\n"
"    border: 2px solid #10b981;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #047857, stop:1 #065f46);\n"
"    padding-left: 10px;\n"
"    padding-top: 10px;\n"
"}\n"
"\n"
"QPushButton:focus {\n"
"    outline: none;\n"
"}"));

        horizontalLayout->addWidget(toolButton_OK);

        toolButton_Cancel = new QPushButton(widget);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMinimumSize(QSize(120, 40));
        toolButton_Cancel->setMaximumSize(QSize(120, 40));
        toolButton_Cancel->setFont(font1);
        toolButton_Cancel->setStyleSheet(QLatin1String("QPushButton {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #ef4444, stop:1 #dc2626);\n"
"    border: 2px solid #b91c1c;\n"
"    border-radius: 8px;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"    padding: 8px;\n"
"}\n"
"\n"
"QPushButton:hover {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #f87171, stop:1 #ef4444);\n"
"    border: 2px solid #ef4444;\n"
"}\n"
"\n"
"QPushButton:pressed {\n"
"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,\n"
"                                stop:0 #b91c1c, stop:1 #991b1b);\n"
"    padding-left: 10px;\n"
"    padding-top: 10px;\n"
"}\n"
"\n"
"QPushButton:focus {\n"
"    outline: none;\n"
"}"));

        horizontalLayout->addWidget(toolButton_Cancel);


        verticalLayout_traylayout->addWidget(widget);


        retranslateUi(PMessageBox);

        QMetaObject::connectSlotsByName(PMessageBox);
    } // setupUi

    void retranslateUi(QDialog *PMessageBox)
    {
        PMessageBox->setWindowTitle(QApplication::translate("PMessageBox", "\350\257\225\347\256\241\347\233\230\347\212\266\346\200\201\346\217\220\347\244\272", nullptr));
        label_showicon->setText(QString());
        label_titlename->setText(QApplication::translate("PMessageBox", "\350\257\225\347\256\241\347\233\230\347\212\266\346\200\201\346\217\220\347\244\272", nullptr));
        label_remindertext->setText(QApplication::translate("PMessageBox", "\346\265\213\350\257\225\346\235\257\346\235\277,\346\234\211\346\234\252\344\275\277\347\224\250\350\257\225\346\235\257\345\205\250\351\203\250\345\274\203\347\224\250?", nullptr));
        toolButton_OK->setText(QApplication::translate("PMessageBox", "\347\241\256\345\256\232", nullptr));
        toolButton_Cancel->setText(QApplication::translate("PMessageBox", "\345\277\275\347\225\245", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PMessageBox: public Ui_PMessageBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PMESSAGEBOX_H
