#pragma execution_character_set("utf-8")

#include "custompppvalue.h"
#include "ui_custompppvalue.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>

// 初始化静态成员变量
QMap<quint8, quint16> customPPPValue::m_channelPPPValues;

QMap<quint8, quint16> customPPPValue::getChannelPPPValues(){
    return m_channelPPPValues;
}

quint16 customPPPValue::getChannelPPPValues(quint8 channel){
    if (m_channelPPPValues.contains(channel) && channel >= 1 && channel <= 12) {
            return m_channelPPPValues[channel];
    }
    return 0; // 如果通道不存在或超出范围，返回默认值0
}

customPPPValue::customPPPValue(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::customPPPValue)
{
    ui->setupUi(this);

    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
          QString qssStyle = QLatin1String(styleFile.readAll());
          ui->pushButtonsycn->setStyleSheet(qssStyle);
          ui->pushButtonsycn->setText("同步保存");
          styleFile.close();
    }

    connect(ui->pushButtonsycn,&QPushButton::clicked,this,&customPPPValue::onSaveButtonClicked);

    initTableWidget();

    loadSycninitPPPvalue();
}


customPPPValue::~customPPPValue()
{
    delete ui;
}


void customPPPValue::initTableWidget(){
    // 获取或创建 tableWidget
    ui->tableWidget->setRowCount(12);
    ui->tableWidget->setColumnCount(2);

    // 隐藏行号（垂直表头）
    ui->tableWidget->verticalHeader()->setVisible(false);

    // 设置表头
    QStringList headers;
    headers << "通道编号" << "初始PPP值";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 创建字体对象，设置较大的字号
    QFont cellFont;
    cellFont.setPointSize(11); // 设置较大的字号
    cellFont.setBold(false);

    // 填充数据
    for (int i = 0; i < 12; ++i) {
        quint8 channelNumber = i + 1;

        // 通道编号 - 不可编辑
        QTableWidgetItem *channelItem = new QTableWidgetItem(QString("通道%1").arg(i + 1));
        channelItem->setTextAlignment(Qt::AlignCenter);
        channelItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable); // 仅启用和可选，不可编辑
        channelItem->setFont(cellFont); // 设置字体大小
        ui->tableWidget->setItem(i, 0, channelItem);

        int pppValue = m_channelPPPValues.contains(channelNumber)?
                        m_channelPPPValues[channelNumber] : 0;

        // 初始PPP值 - 可编辑
        QTableWidgetItem *pppItem = new QTableWidgetItem(QString::number(pppValue));
        pppItem->setTextAlignment(Qt::AlignCenter);
        pppItem->setFont(cellFont); // 设置字体大小
        ui->tableWidget->setItem(i, 1, pppItem);
    }



    // 设置表格样式
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setAlternatingRowColors(true);

    // 设置表头样式 - 更美观
    ui->tableWidget->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                               stop:0 #6c6c6c, stop:1 #4a4a4a);"
        "   color: white;"
        "   padding: 8px;"
        "   border: 1px solid #3a3a3a;"
        "   font-weight: bold;"
        "   font-size: 10pt;"
        "}"
        "QHeaderView::section:first {"
        "   border-left: 1px solid #3a3a3a;"
        "}"
        "QHeaderView::section:last {"
        "   border-right: 1px solid #3a3a3a;"
        "}"
    );

    // 设置表头粗体
    QFont headerFont = ui->tableWidget->horizontalHeader()->font();
    headerFont.setBold(true);
    headerFont.setPointSize(10);
    ui->tableWidget->horizontalHeader()->setFont(headerFont);

    // 设置所有单元格文字居中
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // 设置行高
    for (int i = 0; i < 12; ++i) {
        ui->tableWidget->setRowHeight(i, 35);
    }

    // 设置列宽比例（第一列稍窄，第二列自适应）
    ui->tableWidget->setColumnWidth(0, 100);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 设置代理以实现编辑时文字居中
    ui->tableWidget->setItemDelegate(new AlignDelegate(this));
}


QString customPPPValue::getCSVFilePath()
{
    // 获取应用程序所在目录
    QString appDir = QCoreApplication::applicationDirPath();
    // 构建完整的文件路径
    return appDir + "/SycnTestpppValue.csv";
}


void customPPPValue::loadSycninitPPPvalue(){
    QString filePath = getCSVFilePath();
    QFile file(filePath);

    // 如果文件不存在，创建默认文件
    if (!file.exists()) {
        createDefaultCSVFile();
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + filePath);
        return;
    }

    QTextStream in(&file);
    m_channelPPPValues.clear(); // 清空原有数据

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || !line.startsWith("#Channel")) {
            continue;
        }

        // 解析格式: #Channel1:  value
        QStringList parts = line.split(":");
        if (parts.size() != 2) {
            continue;
        }

        // 提取通道号
        QString channelStr = parts[0].trimmed();
        if (!channelStr.startsWith("#Channel")) {
            continue;
        }

        QString channelNumStr = channelStr.mid(8); // 去掉 "#Channel"
        bool ok;
        int channelNumber = channelNumStr.toInt(&ok);
        if (!ok || channelNumber < 1 || channelNumber > 12) {
            continue;
        }

        // 提取PPP值
        QString valueStr = parts[1].trimmed();
        double pppValue = valueStr.toDouble(&ok);
        if (!ok) {
            continue;
        }

        // 保存到全局容器
        m_channelPPPValues[channelNumber] = pppValue;
    }

    file.close();

    // 更新表格显示
    updateTableFromGlobalData();
}

void customPPPValue::createDefaultCSVFile()
{
    QString filePath = getCSVFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法创建默认文件: " + filePath);
        return;
    }

    QTextStream out(&file);
    for (int i = 1; i <= 12; ++i) {
        out << "#Channel" << i << ": 0\n";
        m_channelPPPValues[i] = 0.0; // 同时初始化全局容器
    }

    file.close();

    // 更新表格显示
    updateTableFromGlobalData();
}

void customPPPValue::updateTableFromGlobalData()
{
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        int channelNumber = row + 1;
        double pppValue = m_channelPPPValues.contains(channelNumber) ?
                         m_channelPPPValues[channelNumber] : 0.0;

        QTableWidgetItem *item = ui->tableWidget->item(row, 1);
        if (item) {
            item->setText(QString::number(pppValue));
        }
    }
}

void customPPPValue::saveToCSVFile()
{
    // 先从表格读取最新数据到全局容器
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        int channelNumber = row + 1;
        QTableWidgetItem *item = ui->tableWidget->item(row, 1);
        if (item) {
            bool ok;
            double pppValue = item->text().toDouble(&ok);
            if (ok) {
                m_channelPPPValues[channelNumber] = pppValue;
            }
        }
    }

    QString filePath = getCSVFilePath();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "错误", "无法保存文件: " + filePath);
        return;
    }

    QTextStream out(&file);
    for (int i = 1; i <= 12; ++i) {
        double pppValue = m_channelPPPValues.contains(i) ? m_channelPPPValues[i] : 0.0;
        out << "#Channel" << i << ": " << pppValue << "\n";
    }

    file.close();
    QMessageBox::information(this, "成功", "数据已成功保存");
}

void customPPPValue::onSaveButtonClicked()
{
    saveToCSVFile();
}










