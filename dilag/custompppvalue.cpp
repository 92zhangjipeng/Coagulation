#pragma execution_character_set("utf-8")

#include "custompppvalue.h"
#include "ui_custompppvalue.h"

#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <ini_file.h>

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

    initParaini();
}


customPPPValue::~customPPPValue()
{
    delete ui;
}




void customPPPValue::insertOneItem(QTableWidget* ptableWidget,QString text,int row,int col, bool isedited,QFont font){
    QTableWidgetItem *pItem = new QTableWidgetItem(text);
    pItem->setTextAlignment(Qt::AlignCenter);
    // 设置编辑权限
    pItem->setFlags(isedited ?
        (Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable) : // 可编辑
        (Qt::ItemIsEnabled | Qt::ItemIsSelectable)                         // 不可编辑
    );
    pItem->setFont(font);
    ptableWidget->setItem(row, col, pItem);
}

void customPPPValue::applyTableStyling(QTableWidget* table) {
    if (!table) return;

    // 设置表头样式
    QString headerStyle = R"(
        QHeaderView::section {
            background: qlineargradient(x1:0, y1:0, x2:0, y2:1,
                                       stop:0 #6c6c6c, stop:1 #4a4a4a);
            color: white;
            padding: 8px;
            border: 1px solid #3a3a3a;
            font-weight: bold;
            font-size: 10pt;
        }
        QHeaderView::section:first {
            border-left: 1px solid #3a3a3a;
        }
        QHeaderView::section:last {
            border-right: 1px solid #3a3a3a;
        }
    )";

    table->horizontalHeader()->setStyleSheet(headerStyle);
    table->horizontalHeader()->setFont(m_headerFont);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);

    // 设置表格通用样式
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setAlternatingRowColors(true);
}

void customPPPValue::initTableStructures() {
    // 设置表格结构
    ui->tableWidget->setRowCount(12);
    ui->tableWidget->setColumnCount(2);

    ui->tableWidgetoutresult->setRowCount(4);
    ui->tableWidgetoutresult->setColumnCount(6);

    // 隐藏行号
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidgetoutresult->verticalHeader()->setVisible(false);
}

void customPPPValue::initHeaders() {
    // 设置表头标签
    QStringList channelHeaders = { "通道编号", "初始PPP值" };
    ui->tableWidget->setHorizontalHeaderLabels(channelHeaders);

    QStringList resultHeaders = { "最小K值", "Kmin值", "最大K值", "Kmax值", "校准率", "R值" };
    ui->tableWidgetoutresult->setHorizontalHeaderLabels(resultHeaders);
}

void customPPPValue::initTableStyles() {
    // 创建统一的字体
    m_cellFont.setPointSize(11);
    m_cellFont.setBold(false);

    m_headerFont.setBold(true);
    m_headerFont.setPointSize(10);
}

void customPPPValue::initResultTableContent() {
    // 初始化结果表格内容
    static const QStringList labels = { "K1", "K2", "K3", "K4" };

    for (int i = 0; i < 4; ++i) {
        // 标签列 - 不可编辑
        insertOneItem(ui->tableWidgetoutresult, QString("%1>= Min").arg(labels[i]), i, 0, false, m_cellFont);
        insertOneItem(ui->tableWidgetoutresult, QString("%1<= Max").arg(labels[i]), i, 2, false, m_cellFont);
        insertOneItem(ui->tableWidgetoutresult, "校准率", i, 4, false, m_cellFont);

        // 数值列 - 可编辑
        insertOneItem(ui->tableWidgetoutresult, "0", i, 1, true, m_cellFont);
        insertOneItem(ui->tableWidgetoutresult, "0", i, 3, true, m_cellFont);
        insertOneItem(ui->tableWidgetoutresult, "0", i, 5, true, m_cellFont);

        // 设置特定单元格的背景色
        if (i == 0) {
            // 第0行，第1列
            QTableWidgetItem* item = ui->tableWidgetoutresult->item(0, 1);
            if (item) {
                item->setBackground(Qt::red); // 浅灰色
                item->setText("/");
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            }
        }
        else if (i == 3) {
            // 第3行，第3列
            QTableWidgetItem* item = ui->tableWidgetoutresult->item(3, 3);
            if (item) {
                item->setBackground(Qt::red); // 浅灰色
                item->setText("/");
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            }
        }
        else if(i == 1){
            QTableWidgetItem* item = ui->tableWidgetoutresult->item(1, 5);
            if (item) {
                item->setBackground(Qt::red); // 浅灰色
                item->setText("/");
				item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            }
        }

    }
}

void customPPPValue::initChannelTableContent() {
    // 初始化通道表格内容
    for (int i = 0; i < 12; ++i) {
        quint8 channelNumber = i + 1;

        // 通道编号 - 不可编辑
        insertOneItem(ui->tableWidget, QString("通道%1").arg(channelNumber), i, 0, false, m_cellFont);

        // 初始PPP值 - 可编辑
        int pppValue = m_channelPPPValues.value(channelNumber, 0);
        insertOneItem(ui->tableWidget, QString::number(pppValue), i, 1, true, m_cellFont);
    }
}

void customPPPValue::applyFinalStyling() {
    // 应用表格样式
    applyTableStyling(ui->tableWidget);
    applyTableStyling(ui->tableWidgetoutresult);

    // 设置特定的尺寸调整
    ui->tableWidget->setColumnWidth(0, 100);
    ui->tableWidget->setRowHeight(0, 35); // 设置第一行高度，其他行会自动跟随

    // 设置代理以实现编辑时文字居中
    ui->tableWidget->setItemDelegate(new AlignDelegate(this));
    ui->tableWidgetoutresult->setItemDelegate(new AlignDelegate(this));
}

void customPPPValue::initTableWidget(){
    initTableStructures();
    initHeaders();
    initTableStyles();
    initResultTableContent();
    initChannelTableContent();
    applyFinalStyling();
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
    //QMessageBox::information(this, "成功", "数据已成功保存");
}

void customPPPValue::onSaveButtonClicked()
{
    saveToCSVFile();
    saveParaini();
}



void customPPPValue::initParaini(){
     static INI_File config;
     //K1
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(MAXK1).toDouble(), 0, 'f', 1),
                    0, 1, true, m_cellFont);
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(RATIOK1).toDouble(), 0, 'f', 1),
                    0, 5, true, m_cellFont);
     //K2
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(MINK2).toDouble(), 0, 'f', 1),
                    1, 1, true, m_cellFont);
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(MAXK2).toDouble(), 0, 'f', 1),
                    1, 3, true, m_cellFont);

     //K3
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(MINK3).toDouble(), 0, 'f', 1),
                    2, 1, true, m_cellFont);
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(MAXK3).toDouble(), 0, 'f', 1),
                    2, 3, true, m_cellFont);
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(RATIOK3).toDouble(), 0, 'f', 1),
                    2, 5, true, m_cellFont);

     //K4
     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(MINK4).toDouble(), 0, 'f', 1),
                    3, 1, true, m_cellFont);

     insertOneItem(ui->tableWidgetoutresult, QString("%1").arg(config.rConfigPara(RATIOK4).toDouble(), 0, 'f', 1),
                    3, 5, true, m_cellFont);
}


QString customPPPValue::getItemText(QTableWidget* table, int row, int column)
{
    QTableWidgetItem* item = table->item(row, column);
    if (item) {
        return item->text();
    }
    return "";
}
void customPPPValue::saveParaini()
{
    static INI_File config;
    bool dataValid = true;

    auto getValidatedValue = [&](int row, int col, const QString& paramName) -> double {
        QString text = getItemText(ui->tableWidgetoutresult, row, col);
        text = text.replace("%", "").trimmed();

        bool ok;
        double value = text.toDouble(&ok);

        if (!ok) {
            QMessageBox::warning(this, "数据错误",
                                QString("参数 %1 的值无效: %2").arg(paramName).arg(text));
            dataValid = false;
            return 0.0;
        }

        return value;
    };

    // K1
    double maxK1 = getValidatedValue(0, 1, "MAXK1");
    double ratiok1 = getValidatedValue(0, 5, "RATIOK1");
    if (dataValid){
        config.wConfigPara(MAXK1, maxK1);
        config.wConfigPara(RATIOK1, ratiok1);
    }

    // K2
    double minK2 = getValidatedValue(1, 1, "MINK2");
    double maxK2 = getValidatedValue(1, 3, "MAXK2");
    if (dataValid) {
        config.wConfigPara(MINK2, minK2);
        config.wConfigPara(MAXK2, maxK2);
    }

    // K3
    double minK3 = getValidatedValue(2, 1, "MINK3");
    double maxK3 = getValidatedValue(2, 3, "MAXK3");
    double ratioK3 = getValidatedValue(2, 5, "RATIOK3");
    if (dataValid) {
        config.wConfigPara(MINK3, minK3);
        config.wConfigPara(MAXK3, maxK3);
        config.wConfigPara(RATIOK3, ratioK3);
    }

    // K4
    double minK4 = getValidatedValue(3, 1, "MINK4");
    double ratioK4 = getValidatedValue(3, 5, "RATIOK4");
    if (dataValid) {
        config.wConfigPara(MINK4, minK4);
        config.wConfigPara(RATIOK4, ratioK4);
    }

    if (dataValid) {
       QMessageBox::information(this, "成功", "配置参数保存成功！"); 
    }
}




