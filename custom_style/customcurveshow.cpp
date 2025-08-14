#include "customcurveshow.h"
#include "ui_customcurveshow.h"

CustomCurveShow::CustomCurveShow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomCurveShow)
{
    ui->setupUi(this);
}

CustomCurveShow::~CustomCurveShow()
{
    delete ui;
}
