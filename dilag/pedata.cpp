#pragma execution_character_set("utf-8")
#include "pedata.h"
#include "ui_pedata.h"
#include <QFile>
#include <globaldata.h>

PEdata::PEdata(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PEdata)
{
    ui->setupUi(this);

    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
      settButtonQss = QLatin1String(styleFile.readAll());
      styleFile.close();
    }
    QHash<QPushButton*, QString> pushButtonList = {
         {ui->pushButton_pushout,    tr("确定")}
    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
         QPushButton* button = it.key();
         QString text = it.value();
         // 使用button和text进行操作
         button->setStyleSheet(settButtonQss);
         button->setText(text);
    }



    GlobalData::QLableSheet(ui->label_input);
    GlobalData::QLableSheet(ui->labelviewMid);
    GlobalData::QLableSheet(ui->labeviewLowData);
	ui->label_input->setWordWrap(true);
	ui->labelviewMid->setWordWrap(true);
	ui->labeviewLowData->setWordWrap(true);
}

PEdata::~PEdata()
{
    delete ui;
}

void PEdata::insertdata(const QMap<int, QVector<double> >& datavec){
  
    // 使用范围for循环替代迭代器
    for (auto it = datavec.constBegin(); it != datavec.constEnd(); ++it) {
        const int key = it.key();
        const QVector<double>& vec = it.value();

        // 预分配内存避免多次扩容
        QString displayText;
        displayText.reserve(vec.size() * 8); // 预估每个数字约8字符

        // 直接拼接字符串避免中间容器
        for (int i = 0; i < vec.size(); ++i) {
            if (i > 0) displayText.append("  ");
            displayText.append(QString::number(vec[i]));
        }
		switch (key)
		{
		case 0: {
			ui->label_input->setText(displayText);
			mval = calculateStats(key, vec);
			break;
		}
		case 1: {
			ui->labelviewMid->setText(displayText);
			mvalMid = calculateStats(key, vec);
			break;
		}
		case 2: {
			ui->labeviewLowData->setText(displayText);
			mvalLow = calculateStats(key, vec);
			break;
		}
		default:
			break;
		}
    }
}

StatsResult PEdata::calculateStats(const int &index,const QVector<double>& data){
    StatsResult result{ NAN, NAN, NAN, NAN };
    if(data.isEmpty()) return result;


    // 极值计算
    const auto& minmax = std::minmax_element(data.constBegin(), data.constEnd());
    result.minVal = *minmax.first;
    result.maxVal = *minmax.second;

    // 平均值
    const double sum = std::accumulate(data.begin(), data.end(), 0.0);
    result.mean = sum / data.size();


    // 标准差与CV值
    if (data.size() >= 2 && std::abs(result.mean) > std::numeric_limits<double>::epsilon()) {
        double variance = 0.0;
        double varCompensation = 0.0; // Kahan求和补偿值

        for (double value : data) {
            double diff = value - result.mean;
            double term = diff * diff;
            double y = term - varCompensation;
            double t = variance + y;
            varCompensation = (t - variance) - y;
            variance = t;
        }

        variance /= (data.size() - 1);
        const double stdDev = std::sqrt(variance);
        result.cv = (stdDev / result.mean) * 100;
    }

    //updateUI
    switch(index){
        case 0:{
            ui->label_max->setText(QString("最大值:%1%").arg(result.maxVal));
            ui->label_min->setText(QString("最小值:%1%").arg(result.minVal));
            ui->label_vage->setText(QString("均值:%1%").arg(result.mean));
            ui->label_cv->setText(QString("CV值:%1%").arg(result.cv));
            break;
        }
        case 1:{
            ui->labelMidMax->setText(QString("最大值:%1%").arg(result.maxVal));
            ui->labelMidMin->setText(QString("最小值:%1%").arg(result.minVal));
            ui->labelMidVage->setText(QString("均值:%1%").arg(result.mean));
            ui->labelMidcv->setText(QString("CV值:%1%").arg(result.cv));
            break;
        }
        case 2:{
            ui->labelLowMax->setText(QString("最大值:%1%").arg(result.maxVal));
            ui->labelLowMin->setText(QString("最小值:%1%").arg(result.minVal));
            ui->labelLowvage->setText(QString("均值:%1%").arg(result.mean));
            ui->labellowcv->setText(QString("CV值:%1%").arg(result.cv));
            break;
        }
    }


    return result;

//    // 标准差与CV值CV的计算公式是标准差除以平均值再乘以100%
//    if (data.size() >= 2 && std::abs(result.mean) > 1e-9) // 修改点：避免浮点零判断
//    {
//        const double variance = std::accumulate(data.begin(), data.end(), 0.0,
//            [mean = result.mean](double acc, double val) {
//                return acc + std::pow(val - mean, 2);
//            }) / (data.size() - 1);

//        const double stdDev = std::sqrt(variance);
//        result.cv = (stdDev / result.mean) * 100;

//    }
}




void PEdata::on_pushButton_pushout_clicked()
{
    emit makesureResult(QString::number(mval.mean,'f',2),
                        QString::number(mvalMid.mean,'f',2),
                        QString::number(mvalLow.mean,'f',2));
    close();
}
