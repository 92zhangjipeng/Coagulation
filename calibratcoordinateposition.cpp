#include "calibratcoordinateposition.h"
#include "ui_calibratcoordinateposition.h"

CalibratCoordinatePosition::CalibratCoordinatePosition(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CalibratCoordinatePosition)
{
    ui->setupUi(this);
}

CalibratCoordinatePosition::~CalibratCoordinatePosition()
{
    delete ui;
}
