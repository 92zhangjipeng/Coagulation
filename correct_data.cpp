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

Correct_Data::Correct_Data(unsigned int rows,int Column,QString changeData,
                           QWidget *parent) : QWidget(parent),ui(new Ui::Correct_Data)
{
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    flags |= Qt::WindowCloseButtonHint;
    this->setWindowFlags(flags);
    this->setFixedSize(360,130);

    m_rows = rows;      //行
    m_Index = Column;   //列
    m_originclickData = changeData;
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
}

Correct_Data::~Correct_Data()
{
    delete ui;
}

void Correct_Data::setrows(const int &row){ m_rows = row;}
void Correct_Data::setcols(const int &col){ m_Index = col; }
void Correct_Data::setorigindata(const QString &Originaldata){
    m_originclickData = Originaldata;
    repLoading();
}


void Correct_Data::repLoading(){
    if (m_Index == Cols_Sample) {
        setWindowTitle(tr("修改样本编号"));

        int id;
        GlobalData::apartSampleId(m_originclickData, m_sampleDate, id);

        ui->label_Text->setText(tr("修改样本号:"));
        ui->toolButton_Retest->hide();
        ui->lineEdit_changetext->setText(QString::number(id));

    } else if (m_Index == Cols_BloodHeight) {
        // 根据全血模式决定是否显示重测按钮
        bool isWholeBloodMode = INI_File().GetWholeBloodModel();
        ui->toolButton_Retest->setVisible(isWholeBloodMode);

        // 使用 tr() 进行国际化翻译
        QString titleName = tr("修正测高值(%1)").arg(m_originclickData);
        QString labelText = tr("修本样本测高值(%1)").arg(m_originclickData);

        setWindowTitle(titleName);
        ui->label_Text->setText(labelText);
    }
}


void Correct_Data::on_toolButton_Ok_clicked()
{
    const QString inputText = ui->lineEdit_changetext->text().trimmed();

    // 通用空值检查
    if (inputText.isEmpty()) {
        showErrorMessage(m_Index);
        return;
    }

    bool processingSuccess = false;

    switch (m_Index) {
    case IndexCols::Cols_Sample:
        processingSuccess = processSampleChange(inputText);
        break;

    case IndexCols::Cols_BloodHeight:
        processingSuccess = processBloodHeightChange(inputText);
        break;

    default:
        QMessageBox::warning(this, tr("错误"), tr("未知的操作类型"));
        break;
    }

    if (processingSuccess) {
       // emit ClosVirtualKeyboard();  // 关闭虚拟键盘
	   ui->lineEdit_changetext->clear();
       close();
    }
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



bool  Correct_Data::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEdit_changetext)
    {
        if(QEvent::FocusIn == event->type()){

        }
    }
    // 最后将事件交给上层对话框
    return QWidget::eventFilter(watched,event);
}

void  Correct_Data::closeEvent(QCloseEvent *)
{

}


bool Correct_Data::processSampleChange(const QString &inputText)
{
    bool ok = false;
    int changeId = inputText.toInt(&ok);

    // 检查转换是否成功且数值有效
    if (!ok || changeId <= 0) {
        QMessageBox::information(this, tr("保存失败"), tr("样本号必须为正整数!"));
        return false;
    }

    emit ConfigureData(m_rows, m_Index, GlobalData::groupDateAndID(m_sampleDate, changeId));
    return true;
}

bool Correct_Data::processBloodHeightChange(const QString &inputText)
{
    // 血液高度值可能包含小数，进行适当的验证
    if (inputText.trimmed().isEmpty()) {
        QMessageBox::information(this, tr("保存失败"), tr("修正测高值不能为空!"));
        return false;
    }

    // 可选：添加数值范围验证
    bool isNumber = false;
    double heightValue = inputText.toDouble(&isNumber);
    if (!isNumber || heightValue < 0) {
        QMessageBox::information(this, tr("保存失败"), tr("请输入有效的数值!"));
        return false;
    }

    emit NotifyTestHeight(m_rows, m_Index, inputText);
    QLOG_DEBUG() << "修改第" << m_Index << "列，值:" << inputText;
    return true;
}

void Correct_Data::showErrorMessage(int index)
{
    switch (index) {
    case IndexCols::Cols_Sample:
        QMessageBox::information(this, tr("保存失败"), tr("样本号不能为空!"));
        break;
    case IndexCols::Cols_BloodHeight:
        QMessageBox::information(this, tr("保存失败"), tr("修正测高值不能为空!"));
        break;
    default:
        QMessageBox::warning(this, tr("错误"), tr("输入内容不能为空"));
        break;
    }
}
