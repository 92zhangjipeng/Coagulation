#include "correct_data.h"
#include "globaldata.h"
#include "ini_file.h"
#include "ui_correct_data.h"
#include <QFuture>
#include <QMessageBox>
#include <QtConcurrent>
#include "QsLog/include/QsLog.h"
#pragma execution_character_set("utf-8")

bool Correct_Data::RetestHight = false;

Correct_Data::Correct_Data(unsigned int rows,
                           int Column,
                           QString changeData,
                           QWidget *parent) : QWidget(parent),ui(new Ui::Correct_Data)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
    this->setFixedSize(360,130);

    m_rows = rows;      //行
    m_Index = Column;   //列
    m_Data_str = changeData;
    ui->lineEdit_changetext->installEventFilter(this);
    ui->lineEdit_changetext->setEchoMode(QLineEdit::Normal);


    QFile styleFileToolBtn(":/Picture/SetPng/wholeToolButton.qss");
    if(styleFileToolBtn.open(QIODevice::ReadOnly)) {
          QString toolBtnQss = QLatin1String(styleFileToolBtn.readAll());
          ui->toolButton_Retest->setStyleSheet(toolBtnQss);
          ui->toolButton_Retest->setText("高度重测");

          ui->toolButton_Ok->setStyleSheet(toolBtnQss);
          ui->toolButton_Ok->setText("确定");

          ui->toolButton_Cancel->setStyleSheet(toolBtnQss);
          ui->toolButton_Cancel->setText("取消");

          styleFileToolBtn.close();
    }



    if(Column == Cols_Sample)
    {
        this->setWindowTitle(tr("修改样本编号"));
        int id;
        GlobalData::apartSampleId(changeData,m_dateSample,id);
        ui->label_Text->setText("修改样本号:");
        ui->toolButton_Retest->hide();
        ui->lineEdit_changetext->setText(QString::number(id));
        emit ShowPointBtn(false);
    }
    else
    {
        ui->toolButton_Retest->show();
        bool bwholeModel =  INI_File().GetWholeBloodModel();
        (!bwholeModel)? ui->toolButton_Retest->hide() : ui->toolButton_Retest->show();
        QString titlename = QString("%1(%2)").arg(tr("修正测高值")).arg(m_Data_str);
        ui->label_Text->setText(QString("修本样本测高值(%1)").arg(m_Data_str));
        this->setWindowTitle(titlename);
        emit ShowPointBtn(true);
    }
}

Correct_Data::~Correct_Data()
{
    delete ui;
    //delete  myFrmnum;
    //myFrmnum = nullptr;

}

void Correct_Data::setrows(const int &row){ m_rows = row;}
void Correct_Data::setcols(const int &col){ m_Index = col; }
void Correct_Data::setorigindata(const QString &Originaldata){
    m_Data_str = Originaldata;
}

void Correct_Data::on_toolButton_Ok_clicked()
{
    if(ui->lineEdit_changetext->text().isEmpty() || ui->lineEdit_changetext->text().isNull())
    {
        QMessageBox::information(this,tr("保存失败"),tr("样本号请勿配置空!"));
        return;
    }
	if (m_Index == IndexCols::Cols_Sample)
	{
        int changeId = ui->lineEdit_changetext->text().toInt();
        if(changeId <= 0 ){
            QMessageBox::information(this,tr("保存失败"),tr("样本号请勿0号!"));
            return;
        }
        emit ConfigureData(m_rows, m_Index, GlobalData::groupDateAndID(m_dateSample,changeId));
	}
	else if(m_Index == IndexCols::Cols_BloodHeight)
	{
		QString resetHeight = ui->lineEdit_changetext->text();
        if (!resetHeight.isEmpty())
        {
			emit NotifyTestHeight(m_rows, m_Index, resetHeight);
            QLOG_DEBUG() << "修改第"<< m_Index << "列";
		}
        else
        {
            QMessageBox::information(this,tr("保存失败"),tr("修正测高值请勿为空!"));
            return;
		}
	}
    //emit ClosVirtualKeyboard();  //关闭虚拟键盘
    this->close();
    return;
}

void Correct_Data::on_toolButton_Cancel_clicked()
{
    //emit ConfigureData(m_rows ,m_Index, m_Data_str);
    ui->lineEdit_changetext->clear();
    //emit ClosVirtualKeyboard();
    this->close();
}


//重测
void Correct_Data::on_toolButton_Retest_clicked()
{
    RetestHight = true;
    QString reminderText = QString("请重测%1血样管高度").arg(m_Samplename);
    QMessageBox::information(this,tr("测高重测"),reminderText);

    emit repTestHeight(m_Samplename);
    return;
}

void  Correct_Data::ClickSamplename(const QString Samplename)
{
   m_Samplename = Samplename;
}

QStringList getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.jpg" << "*.png";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);
    return files;
}



bool  Correct_Data::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEdit_changetext)
    {
        if(QEvent::FocusIn == event->type())
        {
            /*if(ui->lineEdit_changetext->echoMode() == QLineEdit::Normal)
            {
                ui->lineEdit_changetext->clear();
            }
			ui->lineEdit_changetext->setEchoMode(QLineEdit::Normal);*/
        }
    }
    // 最后将事件交给上层对话框
    return QWidget::eventFilter(watched,event);
}

void  Correct_Data::closeEvent(QCloseEvent *)
{

}
