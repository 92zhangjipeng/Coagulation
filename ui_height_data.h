/********************************************************************************
** Form generated from reading UI file 'height_data.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HEIGHT_DATA_H
#define UI_HEIGHT_DATA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Height_Data
{
public:
    QVBoxLayout *verticalLayout_3;
    QWidget *widget_center;
    QHBoxLayout *horizontalLayout_3;
    QWidget *widget_Task;
    QVBoxLayout *verticalLayout;
    QTableWidget *Sample_Data_tablewidget;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QWidget *widget_TestHeighModel;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *checkBox_wholeblood;
    QCheckBox *checkBox_plasma;
    QSpacerItem *horizontalSpacer_5;
    QToolButton *toolButton_Delete_sel_item;
    QToolButton *AddTask;
    QToolButton *toolButtonbatchAddTask;
    QSpacerItem *horizontalSpacer;
    QToolButton *toolButton_ok;
    QToolButton *toolButton_Cancel;
    QSpacerItem *horizontalSpacer_3;
    QWidget *widget_ShowErrImage;
    QHBoxLayout *horizontalLayout_2;

    void setupUi(QWidget *Height_Data)
    {
        if (Height_Data->objectName().isEmpty())
            Height_Data->setObjectName(QStringLiteral("Height_Data"));
        Height_Data->setWindowModality(Qt::NonModal);
        Height_Data->resize(1538, 607);
        Height_Data->setStyleSheet(QString::fromUtf8("\n"
"/* \345\205\250\345\261\200\350\203\214\346\231\257\350\211\262 */\n"
"QWidget#Height_Data {\n"
"    background-color: #E8F4F8;\n"
"}\n"
"\n"
"/* \350\241\250\346\240\274\346\240\267\345\274\217 */\n"
"QTableWidget {\n"
"    background-color: #FFFFFF;\n"
"    alternate-background-color: #F2F9FC;\n"
"    gridline-color: #C8E0F0;\n"
"    font: 14px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\226\260\345\256\213\344\275\223';\n"
"    color: #1A4D6B;\n"
"    selection-background-color: #5FA8D3;\n"
"    selection-color: white;\n"
"    border: 1px solid #B8D9E6;\n"
"    border-radius: 6px;\n"
"}\n"
"\n"
"QTableWidget::item:hover {\n"
"    background-color: #D9F0F8;\n"
"}\n"
"\n"
"QTableWidget::item:selected {\n"
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
"    border-right: 1px solid #3A86A"
                        "8;\n"
"    font-weight: bold;\n"
"    font-size: 13px;\n"
"}\n"
"\n"
"QHeaderView::section:last {\n"
"    border-right: none;\n"
"}\n"
"\n"
"/* \345\272\225\351\203\250\345\267\245\345\205\267\346\240\217\345\256\271\345\231\250 */\n"
"QWidget#widget {\n"
"    border: 1px solid #B8D9E6;\n"
"    background-color: #D9EAF5;\n"
"    border-style: solid;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"/* \346\265\213\351\253\230\346\250\241\345\274\217\345\256\271\345\231\250 */\n"
"QWidget#widget_TestHeighModel {\n"
"    border: 1px solid #AACDE2;\n"
"    background-color: #E8F4F8;\n"
"    border-style: solid;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"}\n"
"\n"
"/* CheckBox \346\240\267\345\274\217 - \345\214\273\347\226\227\346\265\205\350\223\235\351\243\216\346\240\274 */\n"
"QCheckBox {\n"
"    font: 12px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    color: #1A4D6B;\n"
"    spacing: 8px;\n"
"}\n"
"\n"
"QCheckBox::indicator {\n"
"    width: "
                        "20px;\n"
"    height: 20px;\n"
"    border-radius: 4px;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked {\n"
"    background-color: #FFFFFF;\n"
"    border: 1.5px solid #AACDE2;\n"
"}\n"
"\n"
"QCheckBox::indicator:unchecked:hover {\n"
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
"QCheckBox::indicator:disabled {\n"
"    background-color: #E8F0F5;\n"
"    border: 1.5px solid #C8E0F0;\n"
"}\n"
"\n"
"/* QToolButton \346\240\267\345\274\217 - \345\214\273\347\226\227\346\265\205\350\223\235 */\n"
"QToolButton"
                        " {\n"
"    border: 1px solid #5FA8D3;\n"
"    background-color: #5FA8D3;\n"
"    border-radius: 8px;\n"
"    padding: 0 15px;\n"
"    font: 14px '\345\276\256\350\275\257\351\233\205\351\273\221', '\346\245\267\344\275\223';\n"
"    color: white;\n"
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
"/* \345\217\263\344\276\247\345\233\276\347\211\207\346\230\276\347\244\272\345\214\272\345\237\237 */\n"
"QWidget#widget_ShowErrImage {\n"
"    border: 1px solid #B8D9E6;\n"
"    background-color: #D9EAF5;\n"
"    border-style: solid;\n"
"    border-radius: 8px;\n"
"    padding: 5px;\n"
"}\n"
"   "));
        verticalLayout_3 = new QVBoxLayout(Height_Data);
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        widget_center = new QWidget(Height_Data);
        widget_center->setObjectName(QStringLiteral("widget_center"));
        horizontalLayout_3 = new QHBoxLayout(widget_center);
        horizontalLayout_3->setSpacing(5);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        widget_Task = new QWidget(widget_center);
        widget_Task->setObjectName(QStringLiteral("widget_Task"));
        widget_Task->setMinimumSize(QSize(800, 0));
        verticalLayout = new QVBoxLayout(widget_Task);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        Sample_Data_tablewidget = new QTableWidget(widget_Task);
        Sample_Data_tablewidget->setObjectName(QStringLiteral("Sample_Data_tablewidget"));
        Sample_Data_tablewidget->setMinimumSize(QSize(1050, 0));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221,\346\226\260\345\256\213\344\275\223"));
        font.setBold(false);
        font.setItalic(false);
        font.setWeight(50);
        Sample_Data_tablewidget->setFont(font);

        verticalLayout->addWidget(Sample_Data_tablewidget);

        widget = new QWidget(widget_Task);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(0, 60));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\346\226\260\345\256\213\344\275\223"));
        font1.setPointSize(14);
        widget->setFont(font1);
        horizontalLayout = new QHBoxLayout(widget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        widget_TestHeighModel = new QWidget(widget);
        widget_TestHeighModel->setObjectName(QStringLiteral("widget_TestHeighModel"));
        widget_TestHeighModel->setMinimumSize(QSize(140, 0));
        verticalLayout_2 = new QVBoxLayout(widget_TestHeighModel);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(5, 0, 5, 0);
        checkBox_wholeblood = new QCheckBox(widget_TestHeighModel);
        checkBox_wholeblood->setObjectName(QStringLiteral("checkBox_wholeblood"));
        checkBox_wholeblood->setMinimumSize(QSize(0, 30));
        checkBox_wholeblood->setAutoExclusive(true);

        verticalLayout_2->addWidget(checkBox_wholeblood);

        checkBox_plasma = new QCheckBox(widget_TestHeighModel);
        checkBox_plasma->setObjectName(QStringLiteral("checkBox_plasma"));
        checkBox_plasma->setMinimumSize(QSize(0, 30));
        checkBox_plasma->setAutoExclusive(true);

        verticalLayout_2->addWidget(checkBox_plasma);


        horizontalLayout->addWidget(widget_TestHeighModel);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_5);

        toolButton_Delete_sel_item = new QToolButton(widget);
        toolButton_Delete_sel_item->setObjectName(QStringLiteral("toolButton_Delete_sel_item"));
        toolButton_Delete_sel_item->setMinimumSize(QSize(0, 50));
        toolButton_Delete_sel_item->setMaximumSize(QSize(170, 16777215));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221,\346\245\267\344\275\223"));
        font2.setBold(false);
        font2.setItalic(false);
        font2.setWeight(50);
        toolButton_Delete_sel_item->setFont(font2);
        toolButton_Delete_sel_item->setIconSize(QSize(32, 32));
        toolButton_Delete_sel_item->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_Delete_sel_item);

        AddTask = new QToolButton(widget);
        AddTask->setObjectName(QStringLiteral("AddTask"));
        AddTask->setMinimumSize(QSize(0, 50));
        AddTask->setMaximumSize(QSize(170, 16777215));
        AddTask->setFont(font2);
        AddTask->setIconSize(QSize(32, 32));
        AddTask->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(AddTask);

        toolButtonbatchAddTask = new QToolButton(widget);
        toolButtonbatchAddTask->setObjectName(QStringLiteral("toolButtonbatchAddTask"));
        toolButtonbatchAddTask->setMinimumSize(QSize(60, 50));
        toolButtonbatchAddTask->setMaximumSize(QSize(170, 16777215));

        horizontalLayout->addWidget(toolButtonbatchAddTask);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        toolButton_ok = new QToolButton(widget);
        toolButton_ok->setObjectName(QStringLiteral("toolButton_ok"));
        toolButton_ok->setMinimumSize(QSize(60, 50));
        toolButton_ok->setMaximumSize(QSize(170, 16777215));
        toolButton_ok->setFont(font2);
        toolButton_ok->setIconSize(QSize(32, 32));
        toolButton_ok->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_ok);

        toolButton_Cancel = new QToolButton(widget);
        toolButton_Cancel->setObjectName(QStringLiteral("toolButton_Cancel"));
        toolButton_Cancel->setMinimumSize(QSize(60, 50));
        toolButton_Cancel->setMaximumSize(QSize(170, 16777215));
        toolButton_Cancel->setFont(font2);
        toolButton_Cancel->setIconSize(QSize(32, 32));
        toolButton_Cancel->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        horizontalLayout->addWidget(toolButton_Cancel);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addWidget(widget);


        horizontalLayout_3->addWidget(widget_Task);

        widget_ShowErrImage = new QWidget(widget_center);
        widget_ShowErrImage->setObjectName(QStringLiteral("widget_ShowErrImage"));
        widget_ShowErrImage->setMinimumSize(QSize(360, 600));
        widget_ShowErrImage->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout_2 = new QHBoxLayout(widget_ShowErrImage);
        horizontalLayout_2->setSpacing(5);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(5, 5, 5, 5);

        horizontalLayout_3->addWidget(widget_ShowErrImage);


        verticalLayout_3->addWidget(widget_center);


        retranslateUi(Height_Data);

        QMetaObject::connectSlotsByName(Height_Data);
    } // setupUi

    void retranslateUi(QWidget *Height_Data)
    {
        Height_Data->setWindowTitle(QApplication::translate("Height_Data", "Form", nullptr));
        checkBox_wholeblood->setText(QApplication::translate("Height_Data", "\345\205\250\350\241\200\346\250\241\345\274\217", nullptr));
#ifndef QT_NO_TOOLTIP
        checkBox_plasma->setToolTip(QApplication::translate("Height_Data", "\351\273\230\350\256\244PRP\351\253\230\345\272\246\344\270\2720mm,\344\270\213\351\222\210\345\210\260\346\234\200\345\272\225\351\203\250", nullptr));
#endif // QT_NO_TOOLTIP
        checkBox_plasma->setText(QApplication::translate("Height_Data", "\350\241\200\346\265\206\346\250\241\345\274\217", nullptr));
        toolButton_Delete_sel_item->setText(QApplication::translate("Height_Data", "\345\210\240\351\231\244\351\200\211\344\270\255", nullptr));
        AddTask->setText(QApplication::translate("Height_Data", "\346\267\273\345\212\240\344\273\273\345\212\241", nullptr));
        toolButtonbatchAddTask->setText(QApplication::translate("Height_Data", "...", nullptr));
        toolButton_ok->setText(QApplication::translate("Height_Data", "\344\277\235\345\255\230", nullptr));
        toolButton_Cancel->setText(QApplication::translate("Height_Data", "\345\217\226\346\266\210", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Height_Data: public Ui_Height_Data {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HEIGHT_DATA_H
