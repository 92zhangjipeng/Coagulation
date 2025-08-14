#include "calibrationposition.h"
#include "ui_calibrationposition.h"

CalibrationPosition::CalibrationPosition(QWidget *parent) :
    QWidget(parent),

    ui(new Ui::CalibrationPosition)
{
    ui->setupUi(this);
    this->setWindowTitle("模组测试");
}

CalibrationPosition::~CalibrationPosition()
{
    delete ui;
}


