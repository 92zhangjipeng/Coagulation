#pragma execution_character_set("utf-8")
#include "batchaddsample.h"
#include "ui_batchaddsample.h"
#include <globaldata.h>

#include <operclass/fullyautomatedplatelets.h>

batchAddSample::batchAddSample(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::batchAddSample)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("批量添加样本"));
    this->setWindowIcon(QIcon(":/Picture/suowei.png"));



    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
      settButtonQss = QLatin1String(styleFile.readAll());
      styleFile.close();
    }
    QHash<QPushButton*, QString> pushButtonList = {
         {ui->pushButton_save,    tr("保存")},
         {ui->pushButton_cancel,tr("取消")}
    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
         QPushButton* button = it.key();
         QString text = it.value();
         button->setStyleSheet(settButtonQss);
         button->setText(text);
    }

    QFile styleDoublespinBox(":/Picture/SetPng/wholedoubleSpinBox.qss");
    if(styleDoublespinBox.open(QIODevice::ReadOnly)) {
          QString mdoubleSpinboxqss = QLatin1String(styleDoublespinBox.readAll());
          ui->doubleSpinBox_pindown->setStyleSheet(mdoubleSpinboxqss);
          styleDoublespinBox.close();
    } else {
        QLOG_WARN() << "QSS load error:" << styleDoublespinBox.errorString();
    }

    GlobalData::QCommboxSheet(ui->comboBox_testproject);
    GlobalData::QLineEditSheet(ui->lineEditAddFrom);
    GlobalData::QLineEditSheet(ui->lineEditAaddEnd);

}

batchAddSample::~batchAddSample()
{
    delete ui;
}

void batchAddSample::closeEvent(QCloseEvent *event)
{
   emit exitWidget();
   event->accept();//默认情况下接收关闭信号，关闭窗体 
}


void batchAddSample::obtainFirstSampleId(QTableWidget *pSampleIdTable){
    QString  todayLast = GlobalData::ObatinCreatSampleTime();
    int rows = pSampleIdTable->rowCount();
    if(rows == 0){
        int maxid = FullyAutomatedPlatelets::pinstancesqlData()->getMaxSampleID(todayLast);
        ui->lineEditAddFrom->setText(QString::number(maxid + 1));
        ui->lineEditAaddEnd->setText(QString::number(maxid + 1));

    }else{
        QString datefind;
        int setsampleNum = 0;
        QString sampleid = pSampleIdTable->item(rows - 1, SAMLPE_NAME)->text();
        GlobalData::apartSampleId(sampleid,datefind,setsampleNum);
        ui->lineEditAddFrom->setText(QString::number(setsampleNum + 1));
        ui->lineEditAaddEnd->setText(QString::number(setsampleNum + 1));
    }
    return;
}

void batchAddSample::obtainSelectTestProject()
{
    QStringList ItemProgect;
    ItemProgect<<"AA"<<"ADP"<<"EPI"<<"COL"<<"RIS";
    QMap<int ,QString> HadGroup;
    FullyAutomatedPlatelets::pinstancesqlData()->GetTestReagentGroup(HadGroup);
    auto mapreag = HadGroup.constBegin();
    while(mapreag != HadGroup.constEnd())
    {
       ItemProgect.push_back(mapreag.value());
       mapreag++;
    }

    QListWidget* listWidget = new QListWidget(this);
    for(int i = 0 ; i < ItemProgect.count(); ++i){
        QListWidgetItem *item = new QListWidgetItem(ItemProgect.at(i));
        item->setTextAlignment(Qt::AlignCenter);
        listWidget->addItem(item);
    }
    ui->comboBox_testproject->setModel(listWidget->model());
    ui->comboBox_testproject->setView(listWidget);
    ui->comboBox_testproject->setCurrentIndex(-1);

    const quint8 downBottom = INI_File().GetAbsorbTubeBottom();
    ui->doubleSpinBox_pindown->setValue(downBottom);

    return;
}

void batchAddSample::on_pushButton_cancel_clicked()
{
    close();
}

void batchAddSample::on_pushButton_save_clicked()
{
    int idstart = QString(ui->lineEditAddFrom->text()).toInt();
    int idend = QString(ui->lineEditAaddEnd->text()).toInt();
    int k = 0;
    for(;idstart <= idend; idstart++){
       SAMPLEIDINFO addSample;
       addSample.idsample =  QString::number(idstart);
       addSample.pindownHeight = QString::number(ui->doubleSpinBox_pindown->value());
       addSample.testProtect = ui->comboBox_testproject->currentText();
       QVariant DataVar;
       DataVar.setValue(addSample);
       emit send_askData(DataVar);
       k++;
    }
    close();
    return;
}
