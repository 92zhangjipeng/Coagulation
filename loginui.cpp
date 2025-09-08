#pragma execution_character_set("utf-8")
#include "loginui.h"
#include "ui_loginui.h"
#include "instrumentcoordinatetable.h"
#include <QtConcurrent>
#include <QDateTime>
#include <operclass/fullyautomatedplatelets.h>


LoginUi::LoginUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginUi)
{
    ui->setupUi(this);

}

LoginUi::~LoginUi()
{

    delete ui;
}

