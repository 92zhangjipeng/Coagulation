#include "plotwidget.h"
#include "ui_plotwidget.h"

PlotWidget::PlotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setMouseTracking(true);

}

PlotWidget::~PlotWidget()
{
    delete ui;
}

