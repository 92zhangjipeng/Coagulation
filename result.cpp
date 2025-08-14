#pragma execution_character_set("utf-8")

#include "result.h"
#include "ui_result.h"
#include <QMessageBox>
#include <QPainter>
#include <QPrinterInfo>


Result::Result(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Result)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
}

Result::~Result()
{
    delete ui;
}
void Result::Slot_PrintResult()
{
    QPrinter printerPixmap;
    QPixmap pix = QPixmap(800,600);
    createPix(&pix);
    pix.save("234");
    printerPixmap.setOrientation(QPrinter::Landscape);
    printerPixmap.setPageSize(QPrinter::A4);
    QPainter painterPixmap;
    printerPixmap.setPrinterName("33");
    painterPixmap.begin(&printerPixmap);
    painterPixmap.drawPixmap(0,0,pix);
    painterPixmap.end();

}
void Result::Slot_PrintSetup()
{
    QPrinterInfo info;
    QStringList list = info.availablePrinterNames();
}
void Result::Slot_PrintView()
{
    QPrinter printer(QPrinter::HighResolution);
    //自定义纸张
    printer.setPageSize(QPrinter::Custom);
    printer.setPaperSize(QSizeF(600,800),QPrinter::Point);
    QPrintPreviewDialog preview(&printer ,this);
    preview.setMinimumSize(1000,600);
    connect(&preview,SIGNAL(paintRequested(QPrinter*)),this,SLOT(printPreview(QPrinter*)));
    preview.exec();
}
void Result::printPreview(QPrinter* printer)
{
    QPixmap pix = QPixmap(800,600);
    createPix(&pix);
    pix.save("byy");
    printer->setOrientation(QPrinter::Landscape);
    //获取界面图片
    QPainter painterPixmap(this);
    painterPixmap.begin(printer);
    QRect rect = painterPixmap.viewport();
    int x = rect.width() / pix.width();
    int y = rect.height() / pix.height();
    //设置图像长宽是原图的多少倍
    painterPixmap.scale(x,y);
    painterPixmap.drawPixmap(0,0,pix);
    painterPixmap.end();
}

void Result::createPix(QPixmap *pix)
{
    QPainter *painter = new QPainter(this);
    painter->begin(pix);
    painter->setRenderHint(QPainter::Antialiasing);
    //设置画笔 宽度
    painter->setPen(QPen(QColor(255,255,255),2));
    //设置画刷颜色
    painter->setBrush(QColor(255,255,255));
    QRect rect(0,0,800,600);
    //白底
    painter->setPen(QPen(QColor(0,0,0),1));
    painter->drawRect(rect);
    //数据部分线条
    painter->setPen(QPen(QColor(0,0,0),1));
    QVector<QLine> lines;
    lines.append(QLine(QPoint(50,50),QPoint(750,50)));//上边
    lines.append(QLine(QPoint(750,50),QPoint(750,550)));//右边
    lines.append(QLine(QPoint(50,550),QPoint(750,550)));//下边
    lines.append(QLine(QPoint(50,50),QPoint(50,550)));//左边
    lines.append(QLine(QPoint(50,120),QPoint(750,120)));//名称下边
    lines.append(QLine(QPoint(50,180),QPoint(750,180)));//信息下边
    lines.append(QLine(QPoint(50,200),QPoint(750,200)));//条目下边
    lines.append(QLine(QPoint(50,490),QPoint(750,490)));//数据下边
    lines.append(QLine(QPoint(120,180),QPoint(120,200)));//类型分割边
    lines.append(QLine(QPoint(190,180),QPoint(190,200)));//类型分割边
    lines.append(QLine(QPoint(260,180),QPoint(260,200)));//类型分割边
    lines.append(QLine(QPoint(330,180),QPoint(330,200)));//类型分割边
    lines.append(QLine(QPoint(400,180),QPoint(400,490)));//结果和曲线分割边
    painter->drawLines(lines);
    QFont font;
    font.setPointSize(14);
    font.setFamily("宋体");
    font.setItalic(true);
    painter->setFont(font);
    //第一部分
    painter->drawText(50,50,700,40,Qt::AlignCenter,"纵驰");//单位名称
    painter->drawText(QPoint(650,110),"No:");
    font.setPointSize(20);
    painter->setFont(font);
    painter->drawText(50,80,700,40,Qt::AlignCenter,"凝血测试报告单");//报告名称
    font.setPointSize(10);
    painter->setFont(font);
    //第二部分
    painter->drawText(50,120,700,60,Qt::AlignVCenter,QString("样本ID： ")+QString("12"));
    painter->drawText(210,120,700,60,Qt::AlignVCenter,QString("标本类型： ")+QString("血"));
    painter->drawText(370,120,700,60,Qt::AlignVCenter,QString("检测方法： "));
    painter->drawText(600,120,700,60,Qt::AlignVCenter,QString("仪器： "));
    //第三部分
    painter->drawText(50,180,70,20,Qt::AlignCenter,"序号");
    painter->drawText(120,180,70,20,Qt::AlignCenter,"检测项目");
    painter->drawText(190,180,70,20,Qt::AlignCenter,"Ct");
    painter->drawText(260,180,70,20,Qt::AlignCenter,"检测下限");
    painter->drawText(330,180,70,20,Qt::AlignCenter,"结果");
    //第四部分
    painter->drawText(50,210,70,20,Qt::AlignCenter,"1");
    painter->drawText(120,210,70,20,Qt::AlignCenter,"我");
    painter->drawText(190,210,70,20,Qt::AlignCenter,"真的");
    painter->drawText(260,210,70,20,Qt::AlignCenter,"不知道");
    painter->drawText(330,210,70,20,Qt::AlignCenter,"写啥");
    //第五部分
    painter->drawText(50,490,700,30,Qt::AlignVCenter,QString("送检者： "));
    painter->drawText(230,490,700,30,Qt::AlignVCenter,QString("代码： ")/*+ui->lE_code->text()*/);
    painter->drawText(410,490,700,30,Qt::AlignVCenter,QString("检验者： "));
    painter->drawText(590,490,700,30,Qt::AlignVCenter,QString("审核者： "));
    painter->drawText(50,520,700,30,Qt::AlignVCenter,QString("送检日期： "));
    painter->drawText(410,520,700,30,Qt::AlignVCenter,QString("报告日期： "));
    //painter->setRenderHint(QPainter::TextAntialiasing);
    painter->end();

}



