#pragma execution_character_set("utf-8")
#include "aboutmachine.h"
#include "ui_aboutmachine.h"
#include <QCryptographicHash>
#include <QFile>
#include <QPushbutton>
#include <QsLog/include/QsLog.h>
#include <QCryptographicHash>
#include <unordered_map>


AboutMachine::AboutMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutMachine)
{
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags()&~Qt::WindowMaximizeButtonHint&~Qt::WindowMinimizeButtonHint);
    this->setWindowTitle(tr("关于"));
    setWindowIcon(QIcon(":/Picture/suowei.png"));
    QPixmap pixmap(":/Picture/test_hover.png");
    QCursor cursor(pixmap);
    this->setCursor(cursor);


    ui->About_Company->setText(OUR_COMPANY_NAME);
    m_Version = VERSION_RELEASE;
    initStyleAbout();
}

AboutMachine::~AboutMachine()
{
    delete ui;
}


void AboutMachine::initStyleAbout()
{
    //使用链式arg()优化字符串拼接（符合Qt风格）
    const QString portName = cglobal::gserialPortName.isEmpty() ? tr("未连接") : cglobal::gserialPortName;

    ui->About_Version->setText(tr("版本: %1").arg(m_Version));
    ui->About_Machine->setText(tr("型号: %1").arg(m_Machine));
    ui->About_connection->setText(tr("连接端口: %1").arg(portName)); // 修复国际化缺失
    ui->About_Module->setText(tr("发布日期: %1").arg(tr("2022年09月27日")));

    //使用Lambda引用捕获避免冗余拷贝
    connect(ui->pushButtonok, &QPushButton::clicked, this, [this] {
        this->close(); //直接使用close()无需lambda时可用QPointer防护
    });
}


void AboutMachine::sycnMd5Value(const QString &exePath)
{
    QLOG_DEBUG()<<".exe路径"<<exePath;
    QLabel *pshowMd5Val = ui->labelHaxi;
    const QString showText = "哈希值: %1";
    QFile file(exePath);
    if (!file.open(QIODevice::ReadOnly)){
       pshowMd5Val->setText(showText.arg(" "));
       return;
    }


    QCryptographicHash hash(QCryptographicHash::Md5);
    const qint64 bufferSize = 8192; // 8KB缓冲区（可调整）
    while (!file.atEnd()) {
        hash.addData(file.read(bufferSize)); // 分块添加数据
    }
    file.close();
    pshowMd5Val->setText(showText.arg(QString(hash.result().toHex())));
    return; // 返回最终MD5
}


void AboutMachine::sycnEquipment(quint8 types)
{
    // 定义设备类型到字符串的静态映射表，提升可维护性
    static const std::unordered_map<quint8, QString> machineMap = {
        {KS600, KS600STR},
        {KS800, KS800STR},
        {KS1200, KS1200STR}
    };

    // 查找映射，处理未知类型
    auto it = machineMap.find(types);
    if (it != machineMap.end()) {
        m_Machine = it->second;
    } else {
        m_Machine = tr("未识别机型");
    }

    // 优化字符串连接
    ui->About_Machine->setText(tr("型号: ") + m_Machine);
}

void AboutMachine::sycnVersion(const QString version)
{
   ui->About_Version->setText(tr("版本: ")+ version);
}

void AboutMachine::sycnSerialname(QString _serialname)
{
    const QString portName = _serialname.isEmpty() ? tr("未连接") : _serialname;
    ui->About_connection->setText(tr("连接端口: %1").arg(portName));
}

