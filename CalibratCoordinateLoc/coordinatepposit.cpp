#include "coordinatepposit.h"
#include "ui_coordinatepposit.h"

CoordinatepPosit::CoordinatepPosit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoordinatepPosit)
{
    ui->setupUi(this);
}

CoordinatepPosit::~CoordinatepPosit()
{
    delete ui;
}
