#include "coordinatepposit.h"
#include "ui_coordinatepposit.h"

CoordinatepPosit::CoordinatepPosit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoordinatepPosit)
{
    ui->setupUi(this);
    ui->widgetFucn->setStyleSheet(styleSheet);


}

CoordinatepPosit::~CoordinatepPosit()
{
    delete ui;
}

// 在窗口大小变化时调整布局
void CoordinatepPosit::resizeEvent(QResizeEvent *event)
{
    CoordinatepPosit::resizeEvent(event);

    // 根据窗口大小调整字体大小
    int baseSize = qMin(width(), height()) / 80;
    QString dynamicStyle = QString("QGroupBox { font-size: %1px; }").arg(qMax(10, baseSize));
    ui->widgetFucn->setStyleSheet(ui->widgetFucn->styleSheet() + dynamicStyle);
}
