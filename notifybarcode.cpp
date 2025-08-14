#pragma execution_character_set("utf-8")
#include "notifybarcode.h"
#include "ui_notifybarcode.h"
#include <QCloseEvent>

NotifyBarCode::NotifyBarCode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotifyBarCode)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
    this->setFixedSize(360,130);
    this->setWindowIcon(QIcon(":/Picture/suowei.png"));
    this->setWindowTitle("修改条形码");
    QPixmap pixmap(":/Picture/test_hover.png");
    QCursor cursor(pixmap);
    this->setCursor(cursor);


    //输入密码添加可视化按钮
    QRegExp rx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_barcode->setValidator(validator);
    ui->lineEdit_barcode->setAttribute(Qt::WA_InputMethodEnabled,false); //禁止输入法
    delete validator;
    validator = nullptr;
}

NotifyBarCode::~NotifyBarCode()
{
    delete ui;
}

void NotifyBarCode::changerowAndCol(const int rows,const int Column,const QString changeData)
{
    mrows = rows;
    mcols = Column;
    ui->lineEdit_barcode->setText(changeData);
    return;
}
void NotifyBarCode::on_toolButton_save_clicked()
{
    if(ui->lineEdit_barcode->text().isEmpty() || ui->lineEdit_barcode->text().isNull())
    {
        return;
    }
    emit savebarcode(mrows,mcols,ui->lineEdit_barcode->text());
    close();
}

void NotifyBarCode::on_toolButton_cancel_clicked()
{
    close();
}
void NotifyBarCode::closeEvent(QCloseEvent *event)
{
    event->accept();
}
