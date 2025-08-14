/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionMin;
    QAction *actionMax;
    QWidget *centralWidget;
    QWidget *ToolBar;
    QWidget *StatusBar;
    QTabWidget *Function;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *VlayoutMain;
    QWidget *widget_tabbar;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *TabbarHlayout;
    QWidget *widget_Main;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1166, 773);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        actionMin = new QAction(MainWindow);
        actionMin->setObjectName(QStringLiteral("actionMin"));
        actionMax = new QAction(MainWindow);
        actionMax->setObjectName(QStringLiteral("actionMax"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        ToolBar = new QWidget(centralWidget);
        ToolBar->setObjectName(QStringLiteral("ToolBar"));
        ToolBar->setGeometry(QRect(180, 120, 271, 16));
        ToolBar->setStyleSheet(QStringLiteral("background-color: rgb(223, 223, 223);"));
        StatusBar = new QWidget(centralWidget);
        StatusBar->setObjectName(QStringLiteral("StatusBar"));
        StatusBar->setGeometry(QRect(160, 460, 281, 20));
        StatusBar->setStyleSheet(QStringLiteral("background-color: rgb(220, 220, 220);"));
        Function = new QTabWidget(centralWidget);
        Function->setObjectName(QStringLiteral("Function"));
        Function->setGeometry(QRect(180, 170, 281, 181));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(12);
        Function->setFont(font);
        Function->setStyleSheet(QStringLiteral(""));
        Function->setTabPosition(QTabWidget::North);
        Function->setTabShape(QTabWidget::Rounded);
        Function->setIconSize(QSize(64, 64));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(560, 90, 160, 41));
        VlayoutMain = new QVBoxLayout(verticalLayoutWidget);
        VlayoutMain->setSpacing(0);
        VlayoutMain->setContentsMargins(11, 11, 11, 11);
        VlayoutMain->setObjectName(QStringLiteral("VlayoutMain"));
        VlayoutMain->setContentsMargins(0, 0, 0, 0);
        widget_tabbar = new QWidget(centralWidget);
        widget_tabbar->setObjectName(QStringLiteral("widget_tabbar"));
        widget_tabbar->setGeometry(QRect(20, 120, 120, 301));
        label = new QLabel(widget_tabbar);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(20, 30, 54, 12));
        label_2 = new QLabel(widget_tabbar);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 70, 54, 12));
        label_3 = new QLabel(widget_tabbar);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setGeometry(QRect(30, 110, 54, 12));
        label_4 = new QLabel(widget_tabbar);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setGeometry(QRect(30, 150, 54, 12));
        label_5 = new QLabel(widget_tabbar);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(30, 180, 54, 12));
        label_6 = new QLabel(widget_tabbar);
        label_6->setObjectName(QStringLiteral("label_6"));
        label_6->setGeometry(QRect(30, 200, 54, 12));
        label_7 = new QLabel(widget_tabbar);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setGeometry(QRect(30, 230, 54, 12));
        label_8 = new QLabel(widget_tabbar);
        label_8->setObjectName(QStringLiteral("label_8"));
        label_8->setGeometry(QRect(30, 260, 54, 12));
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(540, 200, 160, 81));
        TabbarHlayout = new QHBoxLayout(horizontalLayoutWidget);
        TabbarHlayout->setSpacing(2);
        TabbarHlayout->setContentsMargins(11, 11, 11, 11);
        TabbarHlayout->setObjectName(QStringLiteral("TabbarHlayout"));
        TabbarHlayout->setContentsMargins(0, 0, 0, 0);
        widget_Main = new QWidget(centralWidget);
        widget_Main->setObjectName(QStringLiteral("widget_Main"));
        widget_Main->setGeometry(QRect(530, 350, 120, 80));
        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        Function->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "\351\200\200\345\207\272", nullptr));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("MainWindow", "\351\200\200\345\207\272\347\263\273\347\273\237", nullptr));
#endif // QT_NO_TOOLTIP
        actionMin->setText(QApplication::translate("MainWindow", "\346\234\200\345\260\217\345\214\226", nullptr));
#ifndef QT_NO_TOOLTIP
        actionMin->setToolTip(QApplication::translate("MainWindow", "\346\234\200\345\260\217\345\214\226\350\275\257\344\273\266", nullptr));
#endif // QT_NO_TOOLTIP
        actionMax->setText(QApplication::translate("MainWindow", "\346\234\200\345\244\247\345\214\226", nullptr));
#ifndef QT_NO_TOOLTIP
        actionMax->setToolTip(QApplication::translate("MainWindow", "\346\234\200\345\244\247\345\214\226\350\275\257\344\273\266", nullptr));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_3->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_4->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_5->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_6->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_7->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_8->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
