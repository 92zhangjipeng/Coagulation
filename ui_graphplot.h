/********************************************************************************
** Form generated from reading UI file 'graphplot.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHPLOT_H
#define UI_GRAPHPLOT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE

class Ui_GraphPlot
{
public:
    QVBoxLayout *verticalLayout_4;
    QWidget *widget_CurveInner;
    QWidget *aisle_widget_1;
    QVBoxLayout *verticalLayout;
    QCustomPlot *Plot_aisle_1_result;
    QWidget *aisle_widget_2;
    QVBoxLayout *verticalLayout_5;
    QCustomPlot *Plot_aisle_2_result;
    QWidget *aisle_widget_3;
    QVBoxLayout *verticalLayout_7;
    QCustomPlot *Plot_aisle_3_result;
    QWidget *aisle_widget_4;
    QVBoxLayout *verticalLayout_11;
    QCustomPlot *Plot_aisle_4_result;
    QWidget *aisle_widget_5;
    QVBoxLayout *verticalLayout_2;
    QCustomPlot *Plot_aisle_5_result;
    QWidget *aisle_widget_6;
    QVBoxLayout *verticalLayout_6;
    QCustomPlot *Plot_aisle_6_result;
    QWidget *aisle_widget_7;
    QVBoxLayout *verticalLayout_8;
    QCustomPlot *Plot_aisle_7_result;
    QWidget *aisle_widget_8;
    QVBoxLayout *verticalLayout_12;
    QCustomPlot *Plot_aisle_8_result;
    QWidget *aisle_widget_9;
    QVBoxLayout *verticalLayout_3;
    QCustomPlot *Plot_aisle_9_result;
    QWidget *aisle_widget_10;
    QVBoxLayout *verticalLayout_10;
    QCustomPlot *Plot_aisle_10_result;
    QWidget *aisle_widget_11;
    QVBoxLayout *verticalLayout_9;
    QCustomPlot *Plot_aisle_11_result;
    QWidget *aisle_widget_12;
    QVBoxLayout *verticalLayout_13;
    QCustomPlot *Plot_aisle_12_result;

    void setupUi(QWidget *GraphPlot)
    {
        if (GraphPlot->objectName().isEmpty())
            GraphPlot->setObjectName(QStringLiteral("GraphPlot"));
        GraphPlot->resize(1607, 461);
        GraphPlot->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(250,250,250);"));
        verticalLayout_4 = new QVBoxLayout(GraphPlot);
        verticalLayout_4->setSpacing(5);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        widget_CurveInner = new QWidget(GraphPlot);
        widget_CurveInner->setObjectName(QStringLiteral("widget_CurveInner"));
        widget_CurveInner->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(120, 120, 120);  \n"
"border-radius: 1px;  \n"
"width: 80px;  \n"
"height:40px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:24px;\n"
"background: transparent;\n"
""));
        aisle_widget_1 = new QWidget(widget_CurveInner);
        aisle_widget_1->setObjectName(QStringLiteral("aisle_widget_1"));
        aisle_widget_1->setGeometry(QRect(20, 30, 90, 91));
        aisle_widget_1->setMinimumSize(QSize(0, 0));
        aisle_widget_1->setSizeIncrement(QSize(440, 420));
        aisle_widget_1->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout = new QVBoxLayout(aisle_widget_1);
        verticalLayout->setSpacing(5);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_1_result = new QCustomPlot(aisle_widget_1);
        Plot_aisle_1_result->setObjectName(QStringLiteral("Plot_aisle_1_result"));
        Plot_aisle_1_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout->addWidget(Plot_aisle_1_result);

        aisle_widget_2 = new QWidget(widget_CurveInner);
        aisle_widget_2->setObjectName(QStringLiteral("aisle_widget_2"));
        aisle_widget_2->setGeometry(QRect(140, 30, 141, 91));
        aisle_widget_2->setMinimumSize(QSize(0, 0));
        aisle_widget_2->setSizeIncrement(QSize(440, 420));
        aisle_widget_2->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_5 = new QVBoxLayout(aisle_widget_2);
        verticalLayout_5->setSpacing(5);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        verticalLayout_5->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_2_result = new QCustomPlot(aisle_widget_2);
        Plot_aisle_2_result->setObjectName(QStringLiteral("Plot_aisle_2_result"));
        Plot_aisle_2_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_5->addWidget(Plot_aisle_2_result);

        aisle_widget_3 = new QWidget(widget_CurveInner);
        aisle_widget_3->setObjectName(QStringLiteral("aisle_widget_3"));
        aisle_widget_3->setGeometry(QRect(320, 30, 101, 91));
        aisle_widget_3->setMinimumSize(QSize(0, 0));
        aisle_widget_3->setSizeIncrement(QSize(440, 420));
        aisle_widget_3->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_7 = new QVBoxLayout(aisle_widget_3);
        verticalLayout_7->setSpacing(5);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        verticalLayout_7->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_3_result = new QCustomPlot(aisle_widget_3);
        Plot_aisle_3_result->setObjectName(QStringLiteral("Plot_aisle_3_result"));
        Plot_aisle_3_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_7->addWidget(Plot_aisle_3_result);

        aisle_widget_4 = new QWidget(widget_CurveInner);
        aisle_widget_4->setObjectName(QStringLiteral("aisle_widget_4"));
        aisle_widget_4->setGeometry(QRect(440, 20, 108, 101));
        aisle_widget_4->setMinimumSize(QSize(0, 0));
        aisle_widget_4->setSizeIncrement(QSize(440, 420));
        aisle_widget_4->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_11 = new QVBoxLayout(aisle_widget_4);
        verticalLayout_11->setSpacing(5);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        verticalLayout_11->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_4_result = new QCustomPlot(aisle_widget_4);
        Plot_aisle_4_result->setObjectName(QStringLiteral("Plot_aisle_4_result"));
        Plot_aisle_4_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_11->addWidget(Plot_aisle_4_result);

        aisle_widget_5 = new QWidget(widget_CurveInner);
        aisle_widget_5->setObjectName(QStringLiteral("aisle_widget_5"));
        aisle_widget_5->setGeometry(QRect(10, 140, 92, 111));
        aisle_widget_5->setMinimumSize(QSize(0, 0));
        aisle_widget_5->setSizeIncrement(QSize(440, 420));
        aisle_widget_5->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_2 = new QVBoxLayout(aisle_widget_5);
        verticalLayout_2->setSpacing(5);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_5_result = new QCustomPlot(aisle_widget_5);
        Plot_aisle_5_result->setObjectName(QStringLiteral("Plot_aisle_5_result"));
        Plot_aisle_5_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_2->addWidget(Plot_aisle_5_result);

        aisle_widget_6 = new QWidget(widget_CurveInner);
        aisle_widget_6->setObjectName(QStringLiteral("aisle_widget_6"));
        aisle_widget_6->setGeometry(QRect(160, 150, 108, 111));
        aisle_widget_6->setMinimumSize(QSize(0, 0));
        aisle_widget_6->setSizeIncrement(QSize(440, 420));
        aisle_widget_6->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_6 = new QVBoxLayout(aisle_widget_6);
        verticalLayout_6->setSpacing(5);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        verticalLayout_6->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_6_result = new QCustomPlot(aisle_widget_6);
        Plot_aisle_6_result->setObjectName(QStringLiteral("Plot_aisle_6_result"));
        Plot_aisle_6_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_6->addWidget(Plot_aisle_6_result);

        aisle_widget_7 = new QWidget(widget_CurveInner);
        aisle_widget_7->setObjectName(QStringLiteral("aisle_widget_7"));
        aisle_widget_7->setGeometry(QRect(320, 140, 108, 121));
        aisle_widget_7->setMinimumSize(QSize(0, 0));
        aisle_widget_7->setSizeIncrement(QSize(440, 420));
        aisle_widget_7->setStyleSheet(QString::fromUtf8("border:1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_8 = new QVBoxLayout(aisle_widget_7);
        verticalLayout_8->setSpacing(5);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_7_result = new QCustomPlot(aisle_widget_7);
        Plot_aisle_7_result->setObjectName(QStringLiteral("Plot_aisle_7_result"));
        Plot_aisle_7_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_8->addWidget(Plot_aisle_7_result);

        aisle_widget_8 = new QWidget(widget_CurveInner);
        aisle_widget_8->setObjectName(QStringLiteral("aisle_widget_8"));
        aisle_widget_8->setGeometry(QRect(460, 130, 108, 141));
        aisle_widget_8->setMinimumSize(QSize(0, 0));
        aisle_widget_8->setSizeIncrement(QSize(440, 420));
        aisle_widget_8->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_12 = new QVBoxLayout(aisle_widget_8);
        verticalLayout_12->setSpacing(5);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        verticalLayout_12->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_8_result = new QCustomPlot(aisle_widget_8);
        Plot_aisle_8_result->setObjectName(QStringLiteral("Plot_aisle_8_result"));
        Plot_aisle_8_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_12->addWidget(Plot_aisle_8_result);

        aisle_widget_9 = new QWidget(widget_CurveInner);
        aisle_widget_9->setObjectName(QStringLiteral("aisle_widget_9"));
        aisle_widget_9->setGeometry(QRect(10, 300, 111, 141));
        aisle_widget_9->setMinimumSize(QSize(0, 0));
        aisle_widget_9->setSizeIncrement(QSize(440, 420));
        aisle_widget_9->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_3 = new QVBoxLayout(aisle_widget_9);
        verticalLayout_3->setSpacing(5);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_9_result = new QCustomPlot(aisle_widget_9);
        Plot_aisle_9_result->setObjectName(QStringLiteral("Plot_aisle_9_result"));
        Plot_aisle_9_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_3->addWidget(Plot_aisle_9_result);

        aisle_widget_10 = new QWidget(widget_CurveInner);
        aisle_widget_10->setObjectName(QStringLiteral("aisle_widget_10"));
        aisle_widget_10->setGeometry(QRect(160, 300, 108, 141));
        aisle_widget_10->setMinimumSize(QSize(0, 0));
        aisle_widget_10->setSizeIncrement(QSize(440, 420));
        aisle_widget_10->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_10 = new QVBoxLayout(aisle_widget_10);
        verticalLayout_10->setSpacing(5);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        verticalLayout_10->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_10_result = new QCustomPlot(aisle_widget_10);
        Plot_aisle_10_result->setObjectName(QStringLiteral("Plot_aisle_10_result"));
        Plot_aisle_10_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_10->addWidget(Plot_aisle_10_result);

        aisle_widget_11 = new QWidget(widget_CurveInner);
        aisle_widget_11->setObjectName(QStringLiteral("aisle_widget_11"));
        aisle_widget_11->setGeometry(QRect(310, 300, 108, 141));
        aisle_widget_11->setMinimumSize(QSize(0, 0));
        aisle_widget_11->setSizeIncrement(QSize(440, 420));
        aisle_widget_11->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_9 = new QVBoxLayout(aisle_widget_11);
        verticalLayout_9->setSpacing(5);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        verticalLayout_9->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_11_result = new QCustomPlot(aisle_widget_11);
        Plot_aisle_11_result->setObjectName(QStringLiteral("Plot_aisle_11_result"));
        Plot_aisle_11_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_9->addWidget(Plot_aisle_11_result);

        aisle_widget_12 = new QWidget(widget_CurveInner);
        aisle_widget_12->setObjectName(QStringLiteral("aisle_widget_12"));
        aisle_widget_12->setGeometry(QRect(450, 310, 108, 121));
        aisle_widget_12->setMinimumSize(QSize(0, 0));
        aisle_widget_12->setSizeIncrement(QSize(440, 420));
        aisle_widget_12->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(225, 225, 225);  \n"
"border-radius: 10px;  \n"
"width: 80px;  \n"
"height:60px;  \n"
"padding:0 0px;  \n"
"font-family:'\346\245\267\344\275\223';\n"
"font-size:20px;\n"
"background: transparent;"));
        verticalLayout_13 = new QVBoxLayout(aisle_widget_12);
        verticalLayout_13->setSpacing(5);
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        verticalLayout_13->setContentsMargins(5, 5, 5, 5);
        Plot_aisle_12_result = new QCustomPlot(aisle_widget_12);
        Plot_aisle_12_result->setObjectName(QStringLiteral("Plot_aisle_12_result"));
        Plot_aisle_12_result->setStyleSheet(QLatin1String("border: 2px solid rgb(220, 220, 220);\n"
"background-color:rgba(188,187,183);"));

        verticalLayout_13->addWidget(Plot_aisle_12_result);


        verticalLayout_4->addWidget(widget_CurveInner);


        retranslateUi(GraphPlot);

        QMetaObject::connectSlotsByName(GraphPlot);
    } // setupUi

    void retranslateUi(QWidget *GraphPlot)
    {
        GraphPlot->setWindowTitle(QApplication::translate("GraphPlot", "Form", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GraphPlot: public Ui_GraphPlot {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHPLOT_H
