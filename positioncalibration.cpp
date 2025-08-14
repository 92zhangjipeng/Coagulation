#include "positioncalibration.h"
#include "ui_positioncalibration.h"

Positioncalibration::Positioncalibration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Positioncalibration)
{
    ui->setupUi(this);   

}

Positioncalibration::~Positioncalibration()
{
    delete ui;
}

