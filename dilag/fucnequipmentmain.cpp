#include "fucnequipmentmain.h"
#include "ui_fucnequipmentmain.h"

FucnEquipmentMain::FucnEquipmentMain(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FucnEquipmentMain)
{
    ui->setupUi(this);
}

FucnEquipmentMain::~FucnEquipmentMain()
{
    delete ui;
}
