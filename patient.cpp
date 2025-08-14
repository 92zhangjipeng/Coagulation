#pragma execution_character_set("utf-8")

#include "patient.h"
#include "ui_patient.h"
#include <QComboBox>
#include <QDateTime>
#include <QScrollBar>
#include <qDebug>
#include "delegate.h"

Patient::Patient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Patient)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

Patient::~Patient()
{
    delete ui;
}














