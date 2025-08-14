/********************************************************************************
** Form generated from reading UI file 'logshow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGSHOW_H
#define UI_LOGSHOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LogShow
{
public:
    QVBoxLayout *mianLyout;
    QWidget *Head_text;

    QHBoxLayout *horizontalLayout_3;
    QLabel *labeicon;
    QLabel *Title_TEXT;
    QSpacerItem *horizontalSpacer_3;







    void setupUi(QWidget *LogShow)
    {
        if (LogShow->objectName().isEmpty())
            LogShow->setObjectName(QStringLiteral("LogShow"));
        LogShow->resize(685, 477);
        LogShow->setFocusPolicy(Qt::ClickFocus);
        mianLyout = new QVBoxLayout(LogShow);
        mianLyout->setSpacing(5);


        QMetaObject::connectSlotsByName(LogShow);
    } // setupUi

    void retranslateUi(QWidget *LogShow)
    {

    } // retranslateUi

};

namespace Ui {
    class LogShow: public Ui_LogShow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGSHOW_H
