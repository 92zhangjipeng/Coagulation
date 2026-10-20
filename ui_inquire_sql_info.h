/********************************************************************************
** Form generated from reading UI file 'inquire_sql_info.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INQUIRE_SQL_INFO_H
#define UI_INQUIRE_SQL_INFO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_Inquire_Sql_Info
{
public:
    QVBoxLayout *mainLayout;
    QGroupBox *widget_date_;
    QTableWidget *tableWidget_SQL_Inquire;
    QProgressBar *progressBarLoad;
    QGroupBox *widget_curve;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_curve_left;
    QCustomPlot *Inquire_curve_1;
    QCustomPlot *widget_sats_curve;
    QSpacerItem *horizontalSpacer_curve_right;

    void setupUi(QWidget *Inquire_Sql_Info)
    {
        if (Inquire_Sql_Info->objectName().isEmpty())
            Inquire_Sql_Info->setObjectName(QStringLiteral("Inquire_Sql_Info"));
        Inquire_Sql_Info->resize(1680, 1050);
        Inquire_Sql_Info->setStyleSheet(QString::fromUtf8("QWidget#Inquire_Sql_Info {\n"
"    background-color: #F5F7FA;\n"
"}\n"
"\n"
"QGroupBox {\n"
"    border: 1px solid #D0D7DE;\n"
"    border-radius: 4px;\n"
"    margin-top: 10px;\n"
"    background-color: #FFFFFF;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 12px;\n"
"    color: #1A3A4A;\n"
"    font-weight: normal;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0px 6px 0px 6px;\n"
"    color: #2B7FC4;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QLabel {\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 11px;\n"
"    color: #3A4A5A;\n"
"}\n"
"\n"
"QCheckBox {\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 11px;\n"
"    color: #2C3E50;\n"
""
                        "    spacing: 4px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 14px;\n"
"    height: 14px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    border: 1px solid #B0BEC5;\n"
"    border-radius: 2px;\n"
"    background: #FFFFFF;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:hover {\n"
"    border: 1px solid #2B7FC4;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    border: 1px solid #2B7FC4;\n"
"    border-radius: 2px;\n"
"    background: #2B7FC4;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:hover {\n"
"    border: 1px solid #1A6BAE;\n"
"    background: #1A6BAE;\n"
"}\n"
"\n"
"QLineEdit, QSpinBox, QComboBox, QDateEdit {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #D0D7DE;\n"
"    border-radius: 3px;\n"
"    padding: 0px 6px;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 11px;\n"
"    color: #2C3E50;\n"
"    selection-background-color: #2B7FC4;\n"
"    selection-color: #FFFFFF;\n"
"    min-height: 22px;\n"
"    ma"
                        "x-height: 22px;\n"
"}\n"
"\n"
"QLineEdit:focus, QSpinBox:focus, QComboBox:focus, QDateEdit:focus {\n"
"    border: 1px solid #2B7FC4;\n"
"}\n"
"\n"
"QLineEdit:hover, QSpinBox:hover, QComboBox:hover, QDateEdit:hover {\n"
"    border: 1px solid #7EB6DE;\n"
"}\n"
"\n"
"QComboBox::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    min-height: 22px;\n"
"    max-height: 22px;\n"
"    border-left: 1px solid #D0D7DE;\n"
"    border-top-right-radius: 3px;\n"
"    border-bottom-right-radius: 3px;\n"
"}\n"
"\n"
"QComboBox::down-arrow {\n"
"    image: none;\n"
"    border-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 4px solid #7A8A9A;\n"
"    width: 0;\n"
"    height: 0;\n"
"}\n"
"\n"
"QComboBox::down-arrow:hover {\n"
"    border-top: 4px solid #2B7FC4;\n"
"}\n"
"\n"
"QComboBox QAbstractItemView {\n"
"    border: 1px solid #D0D7DE;\n"
"    background: #FFFFFF;\n"
"    selection-background-color: #E3F2FD;\n"
""
                        "    selection-color: #2B7FC4;\n"
"    outline: 0;\n"
"    min-height: 22px;\n"
"}\n"
"\n"
"QDateEdit::drop-down, QSpinBox::drop-down {\n"
"    subcontrol-origin: padding;\n"
"    subcontrol-position: top right;\n"
"    width: 20px;\n"
"    min-height: 22px;\n"
"    max-height: 22px;\n"
"    border-left: 1px solid #D0D7DE;\n"
"    border-top-right-radius: 3px;\n"
"    border-bottom-right-radius: 3px;\n"
"}\n"
"\n"
"QDateEdit::down-arrow, QSpinBox::down-arrow {\n"
"    image: none;\n"
"    border-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 4px solid #7A8A9A;\n"
"    width: 0;\n"
"    height: 0;\n"
"}\n"
"\n"
"QDateEdit::down-arrow:hover, QSpinBox::down-arrow:hover {\n"
"    border-top: 4px solid #2B7FC4;\n"
"}\n"
"\n"
"QToolButton, QPushButton {\n"
"    background-color: #2B7FC4;\n"
"    border: 1px solid #2B7FC4;\n"
"    border-radius: 3px;\n"
"    padding: 0px 8px;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
""
                        "    font-size: 11px;\n"
"    color: #FFFFFF;\n"
"    font-weight: normal;\n"
"    min-height: 22px;\n"
"    max-height: 22px;\n"
"}\n"
"\n"
"QToolButton:hover, QPushButton:hover {\n"
"    background-color: #3A8ED4;\n"
"    border: 1px solid #3A8ED4;\n"
"}\n"
"\n"
"QToolButton:pressed, QPushButton:pressed {\n"
"    background-color: #1A6BAE;\n"
"    border: 1px solid #1A6BAE;\n"
"}\n"
"\n"
"QToolButton:disabled, QPushButton:disabled {\n"
"    background-color: #B0BEC5;\n"
"    border: 1px solid #B0BEC5;\n"
"    color: #EEEEEE;\n"
"}\n"
"\n"
"QTableWidget {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #D0D7DE;\n"
"    border-radius: 4px;\n"
"    gridline-color: #E8EEF4;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 12px;\n"
"    color: #2C3E50;\n"
"    selection-background-color: #E3F2FD;\n"
"    selection-color: #2B7FC4;\n"
"    alternate-background-color: #F8FAFC;\n"
"}\n"
"\n"
"QHeaderView::section {\n"
"    background-color:"
                        " #E8EEF4;\n"
"    color: #2B7FC4;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    border-right: 1px solid #D0D7DE;\n"
"    border-bottom: 2px solid #2B7FC4;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 12px;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QHeaderView::section:last {\n"
"    border-right: none;\n"
"}\n"
"\n"
"QTableWidget::item {\n"
"    padding: 4px;\n"
"    border: none;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
"    background-color: #E3F2FD;\n"
"    color: #2B7FC4;\n"
"}\n"
"\n"
"QProgressBar {\n"
"    border: 1px solid #D0D7DE;\n"
"    border-radius: 4px;\n"
"    background-color: #FFFFFF;\n"
"    text-align: center;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 11px;\n"
"    color: #2C3E50;\n"
"    height: 22px;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    background-color: #2B7FC4;\n"
"    border-radius: 3px;\n"
"}\n"
"\n"
"QSpinBox::up-button, QS"
                        "pinBox::down-button {\n"
"    width: 14px;\n"
"    background: #F5F7FA;\n"
"}\n"
"\n"
"QSpinBox::up-arrow {\n"
"    width: 0;\n"
"    height: 0;\n"
"    border-left: 3px solid transparent;\n"
"    border-right: 3px solid transparent;\n"
"    border-bottom: 3px solid #7A8A9A;\n"
"}\n"
"\n"
"QSpinBox::down-arrow {\n"
"    width: 0;\n"
"    height: 0;\n"
"    border-left: 3px solid transparent;\n"
"    border-right: 3px solid transparent;\n"
"    border-top: 3px solid #7A8A9A;\n"
"}\n"
"\n"
"QDateEdit::drop-down {\n"
"    width: 18px;\n"
"    border-left: 1px solid #D0D7DE;\n"
"}\n"
"\n"
"QDateEdit::down-arrow {\n"
"    image: none;\n"
"    border-left: 4px solid transparent;\n"
"    border-right: 4px solid transparent;\n"
"    border-top: 4px solid #7A8A9A;\n"
"    width: 0;\n"
"    height: 0;\n"
"}\n"
"\n"
"QScrollBar:vertical {\n"
"    background: #F5F7FA;\n"
"    width: 12px;\n"
"    margin: 0;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: #B0BEC5;\n"
"    border-radius: 6px;\n"
"    min-he"
                        "ight: 30px;\n"
"    margin: 2px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #2B7FC4;\n"
"}\n"
"\n"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {\n"
"    height: 0;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    background: #F5F7FA;\n"
"    height: 12px;\n"
"    margin: 0;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal {\n"
"    background: #B0BEC5;\n"
"    border-radius: 6px;\n"
"    min-width: 30px;\n"
"    margin: 2px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: #2B7FC4;\n"
"}\n"
"\n"
"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {\n"
"    width: 0;\n"
"}\n"
"\n"
"QMenu {\n"
"    background-color: #FFFFFF;\n"
"    border: 1px solid #D0D7DE;\n"
"    border-radius: 4px;\n"
"    padding: 4px 0px;\n"
"    font-family: \"Microsoft YaHei\", \"\345\276\256\350\275\257\351\233\205\351\273\221\";\n"
"    font-size: 12px;\n"
"    color: #2C3E50;\n"
"}\n"
"\n"
"QMenu::item {\n"
"    padding: 8px 24px 8px 24px;\n"
"    backgroun"
                        "d-color: transparent;\n"
"}\n"
"\n"
"QMenu::item:selected {\n"
"    background-color: #E3F2FD;\n"
"    color: #2B7FC4;\n"
"}"));
        mainLayout = new QVBoxLayout(Inquire_Sql_Info);
        mainLayout->setSpacing(6);
        mainLayout->setObjectName(QStringLiteral("mainLayout"));
        mainLayout->setContentsMargins(10, 10, 10, 10);
        widget_date_ = new QGroupBox(Inquire_Sql_Info);
        widget_date_->setObjectName(QStringLiteral("widget_date_"));
        widget_date_->setMinimumSize(QSize(0, 60));

        mainLayout->addWidget(widget_date_);

        tableWidget_SQL_Inquire = new QTableWidget(Inquire_Sql_Info);
        tableWidget_SQL_Inquire->setObjectName(QStringLiteral("tableWidget_SQL_Inquire"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(tableWidget_SQL_Inquire->sizePolicy().hasHeightForWidth());
        tableWidget_SQL_Inquire->setSizePolicy(sizePolicy);
        tableWidget_SQL_Inquire->setMinimumSize(QSize(0, 200));

        mainLayout->addWidget(tableWidget_SQL_Inquire);

        progressBarLoad = new QProgressBar(Inquire_Sql_Info);
        progressBarLoad->setObjectName(QStringLiteral("progressBarLoad"));
        progressBarLoad->setValue(0);

        mainLayout->addWidget(progressBarLoad);

        widget_curve = new QGroupBox(Inquire_Sql_Info);
        widget_curve->setObjectName(QStringLiteral("widget_curve"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(widget_curve->sizePolicy().hasHeightForWidth());
        widget_curve->setSizePolicy(sizePolicy1);
        widget_curve->setMinimumSize(QSize(0, 300));
        horizontalLayout_3 = new QHBoxLayout(widget_curve);
        horizontalLayout_3->setSpacing(12);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(12, 18, 12, 12);
        horizontalSpacer_curve_left = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_curve_left);

        Inquire_curve_1 = new QCustomPlot(widget_curve);
        Inquire_curve_1->setObjectName(QStringLiteral("Inquire_curve_1"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(Inquire_curve_1->sizePolicy().hasHeightForWidth());
        Inquire_curve_1->setSizePolicy(sizePolicy2);
        Inquire_curve_1->setMinimumSize(QSize(600, 0));
        Inquire_curve_1->setMaximumSize(QSize(700, 16777215));

        horizontalLayout_3->addWidget(Inquire_curve_1);

        widget_sats_curve = new QCustomPlot(widget_curve);
        widget_sats_curve->setObjectName(QStringLiteral("widget_sats_curve"));
        sizePolicy2.setHeightForWidth(widget_sats_curve->sizePolicy().hasHeightForWidth());
        widget_sats_curve->setSizePolicy(sizePolicy2);
        widget_sats_curve->setMinimumSize(QSize(380, 0));
        widget_sats_curve->setMaximumSize(QSize(400, 16777215));

        horizontalLayout_3->addWidget(widget_sats_curve);

        horizontalSpacer_curve_right = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_curve_right);


        mainLayout->addWidget(widget_curve);


        retranslateUi(Inquire_Sql_Info);

        QMetaObject::connectSlotsByName(Inquire_Sql_Info);
    } // setupUi

    void retranslateUi(QWidget *Inquire_Sql_Info)
    {
        Inquire_Sql_Info->setWindowTitle(QApplication::translate("Inquire_Sql_Info", "\346\243\200\351\252\214\346\225\260\346\215\256\346\237\245\350\257\242", nullptr));
        widget_date_->setTitle(QApplication::translate("Inquire_Sql_Info", "\360\237\224\215 \346\237\245\350\257\242\346\235\241\344\273\266", nullptr));
        widget_curve->setTitle(QApplication::translate("Inquire_Sql_Info", "\360\237\223\210 \350\201\232\351\233\206\347\216\207\346\233\262\347\272\277", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Inquire_Sql_Info: public Ui_Inquire_Sql_Info {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INQUIRE_SQL_INFO_H
