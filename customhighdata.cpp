#pragma execution_character_set("utf-8")

#include "customhighdata.h"
#include "ui_customhighdata.h"
#include <QTimer>
#include "QsLog/include/QsLog.h"


CustomHighData::CustomHighData(QString Reminder, int index, int Types, unsigned short Value, QWidget *parent) :
    QWidget(parent),
    mcloseTimerId(0),
    ui(new Ui::CustomHighData)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
    setWindowTitle(tr("充值提示"));

    setAttribute(Qt::WA_DeleteOnClose);

    ui->Remindertext->setText(Reminder);
    Supplies_type = Types;
    Supplies_AddNum = Value;
    Supplies_state = index;
    mcloseTimerId = startTimer(2000);

}

CustomHighData::~CustomHighData()
{
    delete ui;
    QLOG_TRACE() << "析构耗材充值提示框";
}

void CustomHighData::on_toolButton_ok_clicked()
{
    onTimeOutOK();
    return;
}
void CustomHighData::onTimeOutOK()
{
    killTimer(mcloseTimerId);
    close();
    emit closeDelWidget();

}
void  CustomHighData::closeEvent(QCloseEvent* /*event*/)
{
    emit AddSupplies(Supplies_type, Supplies_AddNum);
    return;
}

void CustomHighData::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == mcloseTimerId)
    {
       onTimeOutOK();
    }

}
