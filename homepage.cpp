#include "homepage.h"
#include "ui_homepage.h"

HomePage::HomePage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HomePage)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
}

HomePage::~HomePage()
{
    delete ui;
}
