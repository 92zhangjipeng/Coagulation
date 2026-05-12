#pragma execution_character_set("utf-8")
#include "notifybarcode.h"
#include "ui_notifybarcode.h"
#include <QCloseEvent>
#include <QMessageBox>
#include <custom_style/custommessagebox.h>

NotifyBarCode::NotifyBarCode(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NotifyBarCode)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
    this->setFixedSize(360,130);
    this->setWindowTitle("修改条形码");


    //输入密码添加可视化按钮
    QRegExp rx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_barcode->setValidator(validator);
    ui->lineEdit_barcode->setAttribute(Qt::WA_InputMethodEnabled,false); //禁止输入法

    // 监听回车键（扫码枪通常以回车结束）
    connect(ui->lineEdit_barcode, &QLineEdit::returnPressed,
            this, &NotifyBarCode::onLineEditReturnPressed);
}

NotifyBarCode::~NotifyBarCode()
{
    delete ui;
}


void NotifyBarCode::onLineEditReturnPressed()
{
    QString barcode = ui->lineEdit_barcode->text();
    if(!barcode.isEmpty() && !barcode.trimmed().isEmpty()) {
        emit savebarcode(mrows, mcols, barcode);
        close();
    } else if(barcode.isEmpty()) {
        CustomMessageBox::warning(this, "提示", "条形码不能为空！");
    }
}

void NotifyBarCode::changerowAndCol(const int rows,const int Column,const QString changeData)
{
    Q_UNUSED(changeData);
    mrows = rows;
    mcols = Column;

    // 清空并准备接收新条码
    ui->lineEdit_barcode->clear();
    ui->lineEdit_barcode->setFocus();
    ui->lineEdit_barcode->setPlaceholderText("请扫描新的条形码");
}

void NotifyBarCode::on_toolButton_save_clicked()
{
    QString barcode = ui->lineEdit_barcode->text();
    if(barcode.isEmpty() || barcode.trimmed().isEmpty())
    {
        // 提示用户不能保存空条码
        CustomMessageBox::warning(this, "提示", "条形码不能为空！");
        return;
    }
    emit savebarcode(mrows, mcols, barcode);
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
