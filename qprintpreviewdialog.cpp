#include "qprintpreviewdialog.h"
#include "ui_qprintpreviewdialog.h"

QPrintPreviewDialog::QPrintPreviewDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QPrintPreviewDialog)
{
    ui->setupUi(this);
}

QPrintPreviewDialog::~QPrintPreviewDialog()
{
    delete ui;
}
