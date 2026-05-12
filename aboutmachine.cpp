#include "aboutmachine.h"
#include "ui_aboutmachine.h"
#include <QCryptographicHash>
#include <QFile>
#include <QApplication>
#include <QDesktopWidget>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// 医疗淡蓝色主题色系 (与customtitlebar保持一致)
namespace MedicalColors {
    const QString PRIMARY_LIGHT = "#E8F4FD";
    const QString PRIMARY_SOFT = "#B8DFF7";
    const QString PRIMARY_MEDIUM = "#7EC8F0";
    const QString PRIMARY_ACTIVE = "#5BB8E8";
    const QString PRIMARY_DARK = "#3A9BD4";
    const QString TEXT_DARK = "#2C5F8A";
    const QString TEXT_LIGHT = "#4A7FA3";
    const QString DIVIDER = "#D4EAF5";
    const QString WHITE = "#F8F8FF";
    const QString GRAY_BG = "#F5F5F5";
}

AboutMachine::AboutMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AboutMachine),
    m_titleBar(nullptr),
    m_centerWidget(nullptr),
    m_mainLayout(nullptr),
    m_centerLayout(nullptr),
    m_companyLabel(nullptr),
    m_versionLabel(nullptr),
    m_machineLabel(nullptr),
    m_lineFrame(nullptr),
    m_connectionLabel(nullptr),
    m_moduleLabel(nullptr),
    m_hashLabel(nullptr),
    m_confirmBtn(nullptr)
{
    ui->setupUi(this);

    // 隐藏默认标题栏，禁用最大最小化按钮
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    // 初始化UI
    initUI();
    initStyle();

    // 设置默认值
    m_Version = VERSIONALLNUM;
    if (m_companyLabel) {
        m_companyLabel->setText(OUR_COMPANY_NAME);
    }
}

AboutMachine::~AboutMachine()
{
    delete ui;
}

void AboutMachine::initUI()
{
    // 设置窗口大小
    setFixedSize(420, 380);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建标题栏
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle(tr("关于"));
    m_mainLayout->addWidget(m_titleBar);

    // 创建内容区域容器
    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName("centerWidget");
    m_centerLayout = new QVBoxLayout(m_centerWidget);
    m_centerLayout->setContentsMargins(30, 25, 30, 25);
    m_centerLayout->setSpacing(12);

    // 公司名称标签
    m_companyLabel = new QLabel(m_centerWidget);
    m_companyLabel->setObjectName("companyLabel");
    m_companyLabel->setAlignment(Qt::AlignCenter);

    // 版本标签
    m_versionLabel = new QLabel(m_centerWidget);
    m_versionLabel->setObjectName("infoLabel");
    m_versionLabel->setAlignment(Qt::AlignLeft);

    // 型号标签
    m_machineLabel = new QLabel(m_centerWidget);
    m_machineLabel->setObjectName("infoLabel");
    m_machineLabel->setAlignment(Qt::AlignLeft);

    // 分割线
    m_lineFrame = new QFrame(m_centerWidget);
    m_lineFrame->setFrameShape(QFrame::HLine);
    m_lineFrame->setFrameShadow(QFrame::Sunken);
    m_lineFrame->setObjectName("lineFrame");

    // 连接端口标签
    m_connectionLabel = new QLabel(m_centerWidget);
    m_connectionLabel->setObjectName("infoLabel");
    m_connectionLabel->setAlignment(Qt::AlignLeft);

    // 发布日期标签
    m_moduleLabel = new QLabel(m_centerWidget);
    m_moduleLabel->setObjectName("infoLabel");
    m_moduleLabel->setAlignment(Qt::AlignLeft);
    m_moduleLabel->setText(tr("发布日期: 2025年09月05日"));

    // 哈希值标签
    m_hashLabel = new QLabel(m_centerWidget);
    m_hashLabel->setObjectName("hashLabel");
    m_hashLabel->setAlignment(Qt::AlignLeft);
    m_hashLabel->setWordWrap(true);
    m_hashLabel->setMinimumHeight(50);

    // 确认按钮
    m_confirmBtn = new QPushButton(tr("确定"), m_centerWidget);
    m_confirmBtn->setObjectName("confirmBtn");
    m_confirmBtn->setFixedSize(100, 34);
    m_confirmBtn->setCursor(Qt::PointingHandCursor);

    // 添加到布局
    m_centerLayout->addWidget(m_companyLabel);
    m_centerLayout->addSpacing(8);
    m_centerLayout->addWidget(m_versionLabel);
    m_centerLayout->addWidget(m_machineLabel);
    m_centerLayout->addWidget(m_lineFrame);
    m_centerLayout->addWidget(m_connectionLabel);
    m_centerLayout->addWidget(m_moduleLabel);
    m_centerLayout->addWidget(m_hashLabel);
    m_centerLayout->addStretch();
    m_centerLayout->addWidget(m_confirmBtn, 0, Qt::AlignCenter);

    m_mainLayout->addWidget(m_centerWidget);

    // 连接信号
    if (m_titleBar) {
        connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &AboutMachine::onMinimizeRequested);
        connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &AboutMachine::onMaximizeRequested);
        connect(m_titleBar, &CustomTitleBar::closeRequested, this, &AboutMachine::onCloseRequested);
    }

    connect(m_confirmBtn, &QPushButton::clicked, this, &AboutMachine::onConfirmClicked);
}

void AboutMachine::initStyle()
{
    QString styleSheet = QString(R"(
        AboutMachine {
            background-color: %1;
            border-radius: 8px;
        }

        QWidget#centerWidget {
            background-color: %2;
            border-radius: 0 0 8px 8px;
            border: none;
        }

        QLabel#companyLabel {
            color: %3;
            font-size: 18px;
            font-weight: bold;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            background-color: transparent;
            letter-spacing: 1px;
            border: none;
        }

        QLabel#infoLabel {
            color: %4;
            font-size: 13px;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            background-color: transparent;
            line-height: 1.8;
            border: none;
        }

        QLabel#hashLabel {
            color: %4;
            font-size: 11px;
            font-family: 'Consolas', 'Monaco', monospace;
            background-color: transparent;
            line-height: 1.5;
            border: none;
        }

        QFrame#lineFrame {
            color: %5;
            background-color: %5;
            max-height: 1px;
            min-height: 1px;
            border: none;
        }

        QPushButton#confirmBtn {
            background-color: %6;
            color: white;
            border: none;
            border-radius: 17px;
            font-size: 14px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
        }

        QPushButton#confirmBtn:hover {
            background-color: %7;
        }

        QPushButton#confirmBtn:pressed {
            background-color: %8;
        }
    )").arg(MedicalColors::PRIMARY_LIGHT)      // 主背景
      .arg(MedicalColors::WHITE)                // 内容区域背景
      .arg(MedicalColors::PRIMARY_DARK)         // 公司名称颜色
      .arg(MedicalColors::TEXT_DARK)            // 信息文字颜色
      .arg(MedicalColors::DIVIDER)              // 分割线颜色
      .arg(MedicalColors::PRIMARY_ACTIVE)       // 按钮背景
      .arg(MedicalColors::PRIMARY_MEDIUM)       // 按钮悬停
      .arg(MedicalColors::PRIMARY_DARK);        // 按钮按下

    this->setStyleSheet(styleSheet);
}

void AboutMachine::sycnMd5Value(const QString &exePath)
{
    if (!m_hashLabel) return;

    const QString showText = "哈希值: %1";
    QFile file(exePath);
    if (!file.open(QIODevice::ReadOnly)) {
        m_hashLabel->setText(showText.arg(" "));
        return;
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    const qint64 bufferSize = 8192;
    while (!file.atEnd()) {
        hash.addData(file.read(bufferSize));
    }
    file.close();
    m_hashLabel->setText(showText.arg(QString(hash.result().toHex())));
}

void AboutMachine::sycnEquipment(quint8 types)
{
    static const QMap<quint8, QString> machineMap = {
        {KS600, "KS600"},
        {KS800, "KS800"},
        {KS1200, "KS1200"}
    };

    m_Machine = machineMap.value(types, tr("未识别机型"));

    if (m_machineLabel) {
        m_machineLabel->setText(tr("型号: ") + m_Machine);
    }
}

void AboutMachine::sycnVersion(const QString version)
{
    m_Version = version;
    if (m_versionLabel) {
        m_versionLabel->setText(tr("版本: ") + version);
    }
}

void AboutMachine::sycnSerialname(QString _serialname)
{
    const QString portName = _serialname.isEmpty() ? tr("未连接") : _serialname;
    if (m_connectionLabel) {
        m_connectionLabel->setText(tr("连接端口: ") + portName);
    }
}

void AboutMachine::onMinimizeRequested()
{
    showMinimized();
}

void AboutMachine::onMaximizeRequested()
{
    // 关于窗口通常不需要最大化，这里只做简单处理
    if (isMaximized()) {
        showNormal();
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(false);
        }
    } else {
        showMaximized();
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(true);
        }
    }
}

void AboutMachine::onCloseRequested()
{
    close();
}

void AboutMachine::onConfirmClicked()
{
    close();
}

void AboutMachine::closeEvent(QCloseEvent *event)
{
    event->accept();
    deleteLater();
}
