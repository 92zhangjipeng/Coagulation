/********************************************************************************
** Form generated from reading UI file 'controlthemoduletemp.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTROLTHEMODULETEMP_H
#define UI_CONTROLTHEMODULETEMP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ControltheModuletemp
{
public:
    QVBoxLayout *verticalLayout_module_widget;
    QWidget *widget_top;
    QHBoxLayout *horizontalLayout;
    QLabel *labelICON;
    QLabel *label_title;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_close;
    QWidget *widget_fucn_;
    QGridLayout *gridLayout;
    QLabel *labeltips_failed;
    QToolButton *toolButtonexit;
    QToolButton *toolButtonok;

    void setupUi(QWidget *ControltheModuletemp)
    {
        if (ControltheModuletemp->objectName().isEmpty())
            ControltheModuletemp->setObjectName(QStringLiteral("ControltheModuletemp"));
        ControltheModuletemp->resize(421, 216);
        ControltheModuletemp->setStyleSheet(QStringLiteral("background-color: qlineargradient(spread:pad, x1:0.500273, y1:0.438, x2:0.500727, y2:0.909, stop:0 rgba(190, 154, 124, 255), stop:0.971591 rgba(96, 71, 62, 255));"));
        verticalLayout_module_widget = new QVBoxLayout(ControltheModuletemp);
        verticalLayout_module_widget->setSpacing(0);
        verticalLayout_module_widget->setObjectName(QStringLiteral("verticalLayout_module_widget"));
        verticalLayout_module_widget->setContentsMargins(0, 0, 0, 0);
        widget_top = new QWidget(ControltheModuletemp);
        widget_top->setObjectName(QStringLiteral("widget_top"));
        widget_top->setMinimumSize(QSize(0, 36));
        widget_top->setMaximumSize(QSize(16777215, 36));
        widget_top->setStyleSheet(QLatin1String("background-color: rgba(198,197,193);\n"
"border-color: rgb(180, 180, 180);\n"
"border: opx solid rgba(198,197,193);"));
        horizontalLayout = new QHBoxLayout(widget_top);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        labelICON = new QLabel(widget_top);
        labelICON->setObjectName(QStringLiteral("labelICON"));
        labelICON->setMinimumSize(QSize(32, 32));
        labelICON->setMaximumSize(QSize(36, 36));
        labelICON->setScaledContents(false);

        horizontalLayout->addWidget(labelICON);

        label_title = new QLabel(widget_top);
        label_title->setObjectName(QStringLiteral("label_title"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_title->sizePolicy().hasHeightForWidth());
        label_title->setSizePolicy(sizePolicy);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(16);
        label_title->setFont(font);
        label_title->setStyleSheet(QLatin1String("background-color: rgba(198,197,193);\n"
"border-color: rgb(180, 180, 180);\n"
"border: opx solid rgba(198,197,193);"));

        horizontalLayout->addWidget(label_title);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton_close = new QToolButton(widget_top);
        toolButton_close->setObjectName(QStringLiteral("toolButton_close"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/Picture/icon_title/\345\205\263\351\227\255.png"), QSize(), QIcon::Normal, QIcon::Off);
        toolButton_close->setIcon(icon);
        toolButton_close->setIconSize(QSize(32, 32));

        horizontalLayout->addWidget(toolButton_close);


        verticalLayout_module_widget->addWidget(widget_top);

        widget_fucn_ = new QWidget(ControltheModuletemp);
        widget_fucn_->setObjectName(QStringLiteral("widget_fucn_"));
        widget_fucn_->setMinimumSize(QSize(0, 43));
        widget_fucn_->setStyleSheet(QLatin1String("background-color: rgba(188,187,183);\n"
"border-top-color: rgb(0, 0, 0);\n"
"border: opx solid rgba(198,197,193);"));
        gridLayout = new QGridLayout(widget_fucn_);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(0);
        gridLayout->setVerticalSpacing(5);
        gridLayout->setContentsMargins(0, 0, 0, 10);
        labeltips_failed = new QLabel(widget_fucn_);
        labeltips_failed->setObjectName(QStringLiteral("labeltips_failed"));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font1.setPointSize(18);
        font1.setBold(false);
        font1.setItalic(false);
        font1.setWeight(9);
        labeltips_failed->setFont(font1);
        labeltips_failed->setStyleSheet(QString::fromUtf8("font: 75 18pt '\346\245\267\344\275\223';\n"
"background-color: rgba(188,187,183);\n"
"border-radius:5px;padding:2px 4px;\n"
"color: rgb(0, 0, 0);\n"
"border-style: flat;background: transparent;"));
        labeltips_failed->setAlignment(Qt::AlignCenter);
        labeltips_failed->setWordWrap(true);

        gridLayout->addWidget(labeltips_failed, 0, 0, 1, 2);

        toolButtonexit = new QToolButton(widget_fucn_);
        toolButtonexit->setObjectName(QStringLiteral("toolButtonexit"));
        toolButtonexit->setMinimumSize(QSize(120, 30));
        toolButtonexit->setMaximumSize(QSize(120, 35));
        toolButtonexit->setStyleSheet(QString::fromUtf8("QToolButton#toolButtonexit{  \n"
"     border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButtonexit:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButtonexit:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
"\n"
"\n"
""));

        gridLayout->addWidget(toolButtonexit, 2, 1, 1, 1);

        toolButtonok = new QToolButton(widget_fucn_);
        toolButtonok->setObjectName(QStringLiteral("toolButtonok"));
        toolButtonok->setMinimumSize(QSize(120, 30));
        toolButtonok->setMaximumSize(QSize(120, 35));
        toolButtonok->setStyleSheet(QString::fromUtf8("QToolButton#toolButtonok{  \n"
"     border: 1px solid  rgb(200, 200, 200);\n"
"    background-color: qlineargradient(spread:pad, x1:0, 		y1:0, x2:1, y2:0, stop:0 rgba(8,8, 35, 255), stop:1 		rgba(6, 28, 102, 255));\n"
"    border-style: solid;  \n"
"    border-radius:10px;  \n"
"    width: 130px;  \n"
"    height:30px;  \n"
"    padding:0 10px;  \n"
"	font-family:'\346\245\267\344\275\223';\n"
"	font-size:20px;\n"
"    color:white;\n"
"}  \n"
"QToolButton#toolButtonok:hover{     \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(18,18, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}  \n"
"QToolButton#toolButtonok:pressed{  \n"
"    background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(118,118, 135, 255), stop:1 rgba(16, 28, 142, 255));\n"
"}\n"
"\n"
""));

        gridLayout->addWidget(toolButtonok, 2, 0, 1, 1);


        verticalLayout_module_widget->addWidget(widget_fucn_);


        retranslateUi(ControltheModuletemp);

        QMetaObject::connectSlotsByName(ControltheModuletemp);
    } // setupUi

    void retranslateUi(QWidget *ControltheModuletemp)
    {
        ControltheModuletemp->setWindowTitle(QApplication::translate("ControltheModuletemp", "Form", nullptr));
        labelICON->setText(QString());
        label_title->setText(QApplication::translate("ControltheModuletemp", "TextLabel", nullptr));
        toolButton_close->setText(QApplication::translate("ControltheModuletemp", "...", nullptr));
        labeltips_failed->setText(QApplication::translate("ControltheModuletemp", "TextLabel", nullptr));
        toolButtonexit->setText(QApplication::translate("ControltheModuletemp", "\345\217\226\346\266\210", nullptr));
        toolButtonok->setText(QApplication::translate("ControltheModuletemp", "\347\241\256\345\256\232", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ControltheModuletemp: public Ui_ControltheModuletemp {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTROLTHEMODULETEMP_H
