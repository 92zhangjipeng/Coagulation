#include "equipmentmainwidget.h"
#include "ui_equipmentmainwidget.h"



EquipmentMainWidget::EquipmentMainWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EquipmentMainWidget)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    LoadPushBtnImagAndText(ui->pushButton_sampleTest,"样本测试",":/Picture/test.png"); //样本测试
}

EquipmentMainWidget::~EquipmentMainWidget()
{
    delete ui;
}


void EquipmentMainWidget::LoadPushBtnImagAndText(QPushButton* pFucnBtn,QString SetTextStr,QString LoadPathImage)
{
    QLabel* label = new QLabel();
    QLabel* label2 = new QLabel();
    label2->setStyleSheet(QString("border:1px solid red;"));
    label->setStyleSheet(QString("border:1px solid red;"));
    label2->setPixmap(QPixmap(LoadPathImage));
    label->setText(SetTextStr);
    label->setFixedWidth(80);
    QHBoxLayout* myLayout = new QHBoxLayout();
    myLayout->addSpacing(10);
    myLayout->addWidget(label2);
    myLayout->addSpacing(30);
    myLayout->addWidget(label);
    myLayout->addStretch();
    pFucnBtn->setLayout(myLayout);
}
