/********************************************************************************
** Form generated from reading UI file 'configureprojectitem.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGUREPROJECTITEM_H
#define UI_CONFIGUREPROJECTITEM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigureProjectItem
{
public:
    QVBoxLayout *verticalLayout_3;
    QWidget *widgetFucn;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox_group;
    QVBoxLayout *verticalLayout;
    QRadioButton *radioButton_AA;
    QRadioButton *radioButton_ADP;
    QRadioButton *radioButton_EPI;
    QRadioButton *radioButton_COL;
    QRadioButton *radioButton_RIS;
    QTableWidget *tableWidget_group;
    QWidget *widget_btn;
    QHBoxLayout *horizontalLayout_btn;
    QSpacerItem *horizontalSpacer_left;
    QToolButton *toolButtonSaved;
    QToolButton *toolButtonCancel;
    QToolButton *toolButton_addtable;
    QToolButton *toolButton_LossGroup;
    QSpacerItem *horizontalSpacer_right;

    void setupUi(QWidget *ConfigureProjectItem)
    {
        if (ConfigureProjectItem->objectName().isEmpty())
            ConfigureProjectItem->setObjectName(QStringLiteral("ConfigureProjectItem"));
        ConfigureProjectItem->setWindowModality(Qt::ApplicationModal);
        ConfigureProjectItem->resize(600, 420);
        ConfigureProjectItem->setMinimumSize(QSize(550, 380));
        ConfigureProjectItem->setMaximumSize(QSize(600, 500));
        ConfigureProjectItem->setStyleSheet(QString::fromUtf8("\n"
"/* \345\205\250\345\261\200\346\240\267\345\274\217 - \345\214\273\347\226\227\346\265\205\350\223\235\350\211\262\347\263\273 */\n"
"QWidget#ConfigureProjectItem {\n"
"    background-color: #E8F4F8;\n"
"}\n"
"\n"
"/* \346\240\207\351\242\230\346\240\217\346\240\267\345\274\217 */\n"
"QWidget#widget_title {\n"
"    background-color: #5FA8D3;\n"
"    border: none;\n"
"    border-top-left-radius: 8px;\n"
"    border-top-right-radius: 8px;\n"
"    min-height: 40px;\n"
"    max-height: 40px;\n"
"}\n"
"\n"
"QLabel#label_title {\n"
"    color: white;\n"
"    font: bold 16px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    background: transparent;\n"
"    border: none;\n"
"}\n"
"\n"
"QPushButton#pushButton_Close {\n"
"    background: transparent;\n"
"    border: none;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QPushButton#pushButton_Close:hover {\n"
"    background-color: rgba(255, 255, 255, 0.2);\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"/* GroupBox \346\240\267\345\274\217"
                        " */\n"
"QGroupBox#groupBox_group {\n"
"    border: 2px solid #AACDE2;\n"
"    border-radius: 10px;\n"
"    margin-top: 2ex;\n"
"    font: bold 14px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    color: #005A8C;\n"
"    background-color: rgba(240, 249, 255, 0.6);\n"
"}\n"
"\n"
"QGroupBox#groupBox_group::title {\n"
"    subcontrol-origin: margin;\n"
"    subcontrol-position: top center;\n"
"    padding: 0 8px;\n"
"    color: #0077B6;\n"
"}\n"
"\n"
"/* RadioButton \346\240\267\345\274\217 - \345\214\273\347\226\227\346\265\205\350\223\235\351\243\216\346\240\274 */\n"
"QRadioButton {\n"
"    font: 14px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    color: #1A4D6B;\n"
"    spacing: 8px;\n"
"}\n"
"\n"
"QRadioButton::indicator {\n"
"    width: 20px;\n"
"    height: 20px;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked {\n"
"    background-color: #FFFFFF;\n"
"    border: 1.5px solid #AACDE2;\n"
"    border-"
                        "radius: 10px;\n"
"}\n"
"\n"
"QRadioButton::indicator:unchecked:hover {\n"
"    border: 1.5px solid #5FA8D3;\n"
"    background-color: #F0F9FF;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked {\n"
"    background-color: #5FA8D3;\n"
"    border: 1.5px solid #5FA8D3;\n"
"    border-radius: 10px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked:after {\n"
"    content: '\342\227\217';\n"
"    color: white;\n"
"    font-size: 12px;\n"
"    display: block;\n"
"    text-align: center;\n"
"    line-height: 17px;\n"
"}\n"
"\n"
"QRadioButton::indicator:checked:hover {\n"
"    background-color: #3A86A8;\n"
"    border: 1.5px solid #3A86A8;\n"
"}\n"
"\n"
"/* \350\241\250\346\240\274\346\240\267\345\274\217 */\n"
"QTableWidget#tableWidget_group {\n"
"    border: 1px solid #B8D9E6;\n"
"    background-color: #FFFFFF;\n"
"    border-style: solid;\n"
"    border-radius: 8px;\n"
"    padding: 0px;\n"
"    alternate-background-color: #F2F9FC;\n"
"    gridline-color: #C8E0F0;\n"
"}\n"
"\n"
"QTableWidget#tableWidget_group::item:hover"
                        " {\n"
"    background-color: #D9F0F8;\n"
"}\n"
"\n"
"QTableWidget#tableWidget_group::item:selected {\n"
"    background-color: #2C7DA0;\n"
"    color: white;\n"
"}\n"
"\n"
"/* \350\241\250\345\244\264\346\240\267\345\274\217 */\n"
"QHeaderView::section {\n"
"    background-color: #5FA8D3;\n"
"    color: white;\n"
"    padding: 8px;\n"
"    border: none;\n"
"    border-right: 1px solid #3A86A8;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QHeaderView::section:last {\n"
"    border-right: none;\n"
"}\n"
"\n"
"/* \350\241\250\346\240\274\345\206\205 CheckBox \346\240\267\345\274\217 */\n"
"QCheckBox {\n"
"    font: 12px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    color: #1A4D6B;\n"
"    spacing: 8px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: 20px;\n"
"    height: 20px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    background-color: #FFFFFF;\n"
"    border: 1.5px solid #AACDE2;\n"
"}\n"
"\n"
"QCheckBo"
                        "x::indicator:unchecked:hover {\n"
"    border: 1.5px solid #5FA8D3;\n"
"    background-color: #F0F9FF;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked {\n"
"    background-color: #5FA8D3;\n"
"    border: 1.5px solid #5FA8D3;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:after {\n"
"    content: '\342\234\223';\n"
"    color: white;\n"
"    font-size: 14px;\n"
"    font-weight: bold;\n"
"    display: block;\n"
"    text-align: center;\n"
"    line-height: 18px;\n"
"}\n"
"\n"
"QCheckBox::indicator:checked:hover {\n"
"    background-color: #3A86A8;\n"
"    border: 1.5px solid #3A86A8;\n"
"}\n"
"\n"
"/* \345\272\225\351\203\250\346\214\211\351\222\256\345\256\271\345\231\250 */\n"
"QWidget#widget_btn {\n"
"    background-color: transparent;\n"
"}\n"
"\n"
"/* QToolButton \346\240\267\345\274\217 - \345\214\273\347\226\227\346\265\205\350\223\235 */\n"
"QToolButton {\n"
"    border: 1px solid #5FA8D3;\n"
"    background-color: #5FA8D3;\n"
"    border-radius: 8px;\n"
"    padding: 0 15px;\n"
"    font: 14px '\345\276\256\350"
                        "\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    color: white;\n"
"    min-width: 100px;\n"
"    min-height: 32px;\n"
"}\n"
"\n"
"QToolButton:hover {\n"
"    background-color: #3A86A8;\n"
"    border: 1px solid #3A86A8;\n"
"}\n"
"\n"
"QToolButton:pressed {\n"
"    background-color: #2C6D8A;\n"
"    border: 1px solid #2C6D8A;\n"
"}\n"
"\n"
"QToolButton:disabled {\n"
"    background-color: #B0D4E8;\n"
"    border: 1px solid #B0D4E8;\n"
"    color: #6A8EAE;\n"
"}\n"
"\n"
"/* \346\273\232\345\212\250\346\235\241\346\240\267\345\274\217 */\n"
"QScrollBar:vertical {\n"
"    background: #E0F0F8;\n"
"    width: 8px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical {\n"
"    background: #5FA8D3;\n"
"    border-radius: 4px;\n"
"    min-height: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:vertical:hover {\n"
"    background: #3A86A8;\n"
"}\n"
"\n"
"QScrollBar:horizontal {\n"
"    background: #E0F0F8;\n"
"    height: 8px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QScrollBar::handle:"
                        "horizontal {\n"
"    background: #5FA8D3;\n"
"    border-radius: 4px;\n"
"    min-width: 20px;\n"
"}\n"
"\n"
"QScrollBar::handle:horizontal:hover {\n"
"    background: #3A86A8;\n"
"}\n"
"   "));
        verticalLayout_3 = new QVBoxLayout(ConfigureProjectItem);
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 5);
        widgetFucn = new QWidget(ConfigureProjectItem);
        widgetFucn->setObjectName(QStringLiteral("widgetFucn"));
        horizontalLayout = new QHBoxLayout(widgetFucn);
        horizontalLayout->setSpacing(10);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(10, 10, 10, 5);
        groupBox_group = new QGroupBox(widgetFucn);
        groupBox_group->setObjectName(QStringLiteral("groupBox_group"));
        groupBox_group->setMinimumSize(QSize(140, 0));
        groupBox_group->setMaximumSize(QSize(160, 400));
        verticalLayout = new QVBoxLayout(groupBox_group);
        verticalLayout->setSpacing(12);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(20, 15, 10, 15);
        radioButton_AA = new QRadioButton(groupBox_group);
        radioButton_AA->setObjectName(QStringLiteral("radioButton_AA"));

        verticalLayout->addWidget(radioButton_AA);

        radioButton_ADP = new QRadioButton(groupBox_group);
        radioButton_ADP->setObjectName(QStringLiteral("radioButton_ADP"));

        verticalLayout->addWidget(radioButton_ADP);

        radioButton_EPI = new QRadioButton(groupBox_group);
        radioButton_EPI->setObjectName(QStringLiteral("radioButton_EPI"));

        verticalLayout->addWidget(radioButton_EPI);

        radioButton_COL = new QRadioButton(groupBox_group);
        radioButton_COL->setObjectName(QStringLiteral("radioButton_COL"));

        verticalLayout->addWidget(radioButton_COL);

        radioButton_RIS = new QRadioButton(groupBox_group);
        radioButton_RIS->setObjectName(QStringLiteral("radioButton_RIS"));

        verticalLayout->addWidget(radioButton_RIS);


        horizontalLayout->addWidget(groupBox_group);

        tableWidget_group = new QTableWidget(widgetFucn);
        tableWidget_group->setObjectName(QStringLiteral("tableWidget_group"));
        tableWidget_group->setMinimumSize(QSize(350, 250));
        tableWidget_group->setMaximumSize(QSize(9999, 400));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font.setPointSize(12);
        tableWidget_group->setFont(font);

        horizontalLayout->addWidget(tableWidget_group);


        verticalLayout_3->addWidget(widgetFucn);

        widget_btn = new QWidget(ConfigureProjectItem);
        widget_btn->setObjectName(QStringLiteral("widget_btn"));
        widget_btn->setMinimumSize(QSize(0, 60));
        widget_btn->setMaximumSize(QSize(16777215, 70));
        horizontalLayout_btn = new QHBoxLayout(widget_btn);
        horizontalLayout_btn->setSpacing(15);
        horizontalLayout_btn->setObjectName(QStringLiteral("horizontalLayout_btn"));
        horizontalLayout_btn->setContentsMargins(15, 5, 15, 5);
        horizontalSpacer_left = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_btn->addItem(horizontalSpacer_left);

        toolButtonSaved = new QToolButton(widget_btn);
        toolButtonSaved->setObjectName(QStringLiteral("toolButtonSaved"));
        toolButtonSaved->setMinimumSize(QSize(132, 34));
        toolButtonSaved->setMaximumSize(QSize(120, 40));
        toolButtonSaved->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_btn->addWidget(toolButtonSaved);

        toolButtonCancel = new QToolButton(widget_btn);
        toolButtonCancel->setObjectName(QStringLiteral("toolButtonCancel"));
        toolButtonCancel->setMinimumSize(QSize(132, 34));
        toolButtonCancel->setMaximumSize(QSize(120, 40));
        toolButtonCancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_btn->addWidget(toolButtonCancel);

        toolButton_addtable = new QToolButton(widget_btn);
        toolButton_addtable->setObjectName(QStringLiteral("toolButton_addtable"));
        toolButton_addtable->setMinimumSize(QSize(132, 34));
        toolButton_addtable->setMaximumSize(QSize(120, 40));
        toolButton_addtable->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_btn->addWidget(toolButton_addtable);

        toolButton_LossGroup = new QToolButton(widget_btn);
        toolButton_LossGroup->setObjectName(QStringLiteral("toolButton_LossGroup"));
        toolButton_LossGroup->setMinimumSize(QSize(132, 34));
        toolButton_LossGroup->setMaximumSize(QSize(120, 40));
        toolButton_LossGroup->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout_btn->addWidget(toolButton_LossGroup);

        horizontalSpacer_right = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_btn->addItem(horizontalSpacer_right);


        verticalLayout_3->addWidget(widget_btn);


        retranslateUi(ConfigureProjectItem);

        QMetaObject::connectSlotsByName(ConfigureProjectItem);
    } // setupUi

    void retranslateUi(QWidget *ConfigureProjectItem)
    {
        ConfigureProjectItem->setWindowTitle(QApplication::translate("ConfigureProjectItem", "\351\205\215\347\275\256\351\241\271\347\233\256", nullptr));
        groupBox_group->setTitle(QApplication::translate("ConfigureProjectItem", "\346\265\213\350\257\225\351\241\271\347\233\256", nullptr));
        radioButton_AA->setText(QApplication::translate("ConfigureProjectItem", "AA", nullptr));
        radioButton_ADP->setText(QApplication::translate("ConfigureProjectItem", "ADP", nullptr));
        radioButton_EPI->setText(QApplication::translate("ConfigureProjectItem", "EPI", nullptr));
        radioButton_COL->setText(QApplication::translate("ConfigureProjectItem", "COL", nullptr));
        radioButton_RIS->setText(QApplication::translate("ConfigureProjectItem", "RIS", nullptr));
        toolButtonSaved->setText(QApplication::translate("ConfigureProjectItem", "\347\241\256\345\256\232", nullptr));
        toolButtonCancel->setText(QApplication::translate("ConfigureProjectItem", "\345\217\226\346\266\210", nullptr));
        toolButton_addtable->setText(QApplication::translate("ConfigureProjectItem", "\346\267\273\345\212\240\347\273\204\345\220\210", nullptr));
        toolButton_LossGroup->setText(QApplication::translate("ConfigureProjectItem", "\345\210\240\351\231\244\347\273\204\345\220\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigureProjectItem: public Ui_ConfigureProjectItem {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGUREPROJECTITEM_H
