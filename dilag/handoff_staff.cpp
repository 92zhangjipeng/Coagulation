#pragma execution_character_set("utf-8")

#include "handoff_staff.h"
#include "ui_handoff_staff.h"

#include <cglobal.h>
#include <customcreatsql.h>
#include <operclass/fullyautomatedplatelets.h>
//切换用户

handoff_staff::handoff_staff(QWidget *parent) :QWidget(parent),
    ui(new Ui::handoff_staff)
{
    ui->setupUi(this);
    setWindowTitle("切换用户");
    ui->label_user->setText(QString("当前登录用户:%1").arg(cglobal::g_UserName_str));
    ui->lineEdit_passwordhandof->setPlaceholderText("切换密码");

    QRegExp rx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_passwordhandof->setValidator(validator);
    ui->lineEdit_passwordhandof->setAttribute(Qt::WA_InputMethodEnabled,false); //禁止输入法
    delete validator;
    validator = nullptr;

    QStringList user_list;
    user_list.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllUsername(user_list);
    user_list.removeOne("hospital_name");
    if(user_list.contains(cglobal::g_UserName_str))
    {
        user_list.removeOne(cglobal::g_UserName_str);
    }
    for(auto itemuser : user_list){
           ui->comboBox_handofstaf->addItem(itemuser);
    }

}

handoff_staff::~handoff_staff()
{
    delete ui;
}

void handoff_staff::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void handoff_staff::setTitlename(QString titlename)
{
    m_Titlename = titlename;
}

void handoff_staff::on_toolButton_handofstaff_clicked()
{
    QString handoffuser = ui->comboBox_handofstaf->currentText();
	if (handoffuser == "" || handoffuser.isEmpty()) {
		QMessageBox::warning(this, tr("切换失败"), tr("切换账户为空!"));
		return;
	}
    QString enditpassword = ui->lineEdit_passwordhandof->text();
    QString handofPassword =  FullyAutomatedPlatelets::pinstancesqlData()->FindPassword(handoffuser);
    if(enditpassword == handofPassword)
    {
       cglobal::g_UserName_str = handoffuser;
       QLOG_DEBUG()<<"切换用户成功:"<<handoffuser<<endl;
       close();
    }
    else
    {
        ui->lineEdit_passwordhandof->clear();
        ui->lineEdit_passwordhandof->setPlaceholderText("切换密码错误");
    }
    return;
}
