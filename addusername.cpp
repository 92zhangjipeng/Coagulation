#pragma execution_character_set("utf-8")
#include "addusername.h"
#include "loginui.h"
#include "ui_addusername.h"

#include <operclass/fullyautomatedplatelets.h>

AddUserName::AddUserName(char UserSet, QWidget *parent ):
    QWidget(parent),
    ui(new Ui::AddUserName)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
    m_show = UserSet;
    ui->Enter_Usar->setText(tr("输入用户名: "));
    ui->pushButton_Add->setText(tr("添 加 "));
    ui->pushButton_Cancel->setText(tr("取 消 "));
    ui->label_2->setText(tr("旧密码:"));
    ui->label_3->setText(tr("新密码: "));
    ui->label_4->setText(tr("确认新密码: "));
    ui->OK->setText(tr("确 认 "));
    ui->Cancel->setText(tr("取 消 "));
    InitUi();
}

AddUserName::~AddUserName()
{
    delete ui;
}
void AddUserName::InitUi()
{
    if(m_show == 0)
    {
        setWindowTitle(tr("添加用户"));
        ui->widget_changepassword->hide();
        this->setMaximumSize(400,240);
        QRegExp regx("[a-zA-Z0-9]+$");
        QValidator *validator = new QRegExpValidator(regx);
        ui->lineEdit_Setpassword->setValidator(validator);
        ui->lineEdit_Setpassword->setAttribute(Qt::WA_InputMethodEnabled, false);
        qInfo()<<tr("打开添加用户界面");
    }
    else
    {
        setWindowTitle(tr("修改密码"));
        QRegExp regx("[a-zA-Z0-9]+$");
        QValidator *validator = new QRegExpValidator(regx);
        ui->oldpassword->setValidator(validator);
        ui->oldpassword->setAttribute(Qt::WA_InputMethodEnabled, false);

        ui->newpassword->setValidator(validator);
        ui->newpassword->setAttribute(Qt::WA_InputMethodEnabled, false);

        ui->makesurepassword->setValidator(validator);
        ui->makesurepassword->setAttribute(Qt::WA_InputMethodEnabled, false);
        ui->widget_Adduser->hide();
        qInfo()<<tr("打开修改密码界面");
    }
}

void AddUserName::ModifyUsername_Slot(QString name)
{
    QString text = QString(tr("修改用户%1密码")).arg(name);
    m_NotifyUsername = name;
    ui->label->setText(text);
    return;
}

void AddUserName::on_pushButton_Add_clicked()
{
	QString user_ = ui->lineEdit_Enter_Usar->text().trimmed();
	QString setpassword_ = ui->lineEdit_Setpassword->text().trimmed();
    if(user_.isEmpty() || user_.isNull())
    {
       QMessageBox::critical(this,"错误提醒",tr("用户为空操作失败!"));
       return;
    }
    if(setpassword_.isEmpty())
    {
        QMessageBox::critical(this,"错误提醒",tr("密码未设置操作失败!"));
        return;
    }
    QString User_str = ui->lineEdit_Enter_Usar->text();
    QString Password = ui->lineEdit_Setpassword->text();
    bool AddVip = ui->checkBox_addvip->isChecked();
    emit AddUser(User_str,Password,AddVip);
    close();
    return;
}
void AddUserName::on_pushButton_Cancel_clicked()
{
    close();    
}

void AddUserName::closeEvent(QCloseEvent *event)
{
    switch(m_show)
    {
     case 0: qInfo()<<tr("关闭添加用户界面"); break;
     case 1: qInfo()<<tr("关闭修改密码界面"); break;
     default: break;
    }
    event->accept();
}

//修改密码确定
void AddUserName::on_OK_clicked()
{
    QString username = m_NotifyUsername;
    QString oldInput = ui->oldpassword->text();
    QString newInput = ui->newpassword->text();
    QString confirmInput = ui->makesurepassword->text();

    // 输入合法性检查
    if (oldInput.isEmpty() || newInput.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("密码不能为空"));
        return;
    }

    if(oldInput == newInput && oldInput == confirmInput){
        QMessageBox::information(this, tr("用户密码"), tr("修改重复密码无效!"));
        return;
    }

    // 获取数据库存储的旧密码（假设已加密）
    QString dbOldPassword = FullyAutomatedPlatelets::pinstancesqlData()->FindPassword(username);
    // 验证新密码一致性
    if (oldInput != dbOldPassword) {
        QMessageBox::information(this, tr("失败"), tr("初始密码不一致"));
        return;
    }
    else if(newInput != confirmInput){
        QMessageBox::information(this, tr("修改失败"), tr("新密码与确认密码不一致!"));
        return;
    }else if(newInput == confirmInput){
         FullyAutomatedPlatelets::pinstancesqlData()->NotifyPassword(username,newInput);
         QMessageBox::information(this, tr("用户密码"), tr("新密码修改成功!"));
         close();
    }
    return;
}

//取消修改密码
void AddUserName::on_Cancel_clicked()
{
    close();
}
