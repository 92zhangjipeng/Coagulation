#include "functioncustomwidget.h"
#include "ui_functioncustomwidget.h"
#include <QDesktopWidget>
#include <QApplication>

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
    const QString WHITE = "#FFFFFF";
}

FunctionCustomWidget::FunctionCustomWidget(QString titlestr, QString warmtext, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FunctionCustomWidget),
    m_titleBar(nullptr),
    m_centerWidget(nullptr),
    m_mainLayout(nullptr),
    m_centerLayout(nullptr),
    m_warmTextLabel(nullptr),
    m_funcBtn1(nullptr),
    m_funcBtn2(nullptr),
    m_funcBtn3(nullptr),
    m_buttonLayout(nullptr),
    m_isMaximized(false)
{
    ui->setupUi(this);

    // 隐藏默认标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // 初始化UI
    initUI();
    initStyle();

    // 设置内容
    if (m_titleBar) {
        m_titleBar->setTitle(titlestr);
    }
    if (m_warmTextLabel) {
        m_warmTextLabel->setText(warmtext);
    }
}

FunctionCustomWidget::~FunctionCustomWidget()
{
    delete ui;
}

void FunctionCustomWidget::initUI()
{
    // 设置窗口大小
    setFixedSize(480, 260);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建标题栏
    m_titleBar = new CustomTitleBar(this);
    m_mainLayout->addWidget(m_titleBar);

    // 创建内容区域容器
    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName("centerWidget");
    m_centerLayout = new QVBoxLayout(m_centerWidget);
    m_centerLayout->setContentsMargins(24, 20, 24, 20);
    m_centerLayout->setSpacing(16);

    // 提示文本标签 - 居中显示
    m_warmTextLabel = new QLabel(m_centerWidget);
    m_warmTextLabel->setObjectName("warmTextLabel");
    m_warmTextLabel->setAlignment(Qt::AlignCenter);
    m_warmTextLabel->setWordWrap(true);
    m_warmTextLabel->setMinimumWidth(400);

    // 按钮布局
    m_buttonLayout = new QHBoxLayout();
    m_buttonLayout->setSpacing(20);
    m_buttonLayout->setAlignment(Qt::AlignCenter);

    // 创建3个功能按钮
    m_funcBtn1 = new QPushButton("", m_centerWidget);
    m_funcBtn1->setObjectName("funcBtn");
    m_funcBtn1->setFixedSize(120, 36);
    m_funcBtn1->setCursor(Qt::PointingHandCursor);
    m_funcBtn1->setVisible(false);

    m_funcBtn2 = new QPushButton("", m_centerWidget);
    m_funcBtn2->setObjectName("funcBtn");
    m_funcBtn2->setFixedSize(120, 36);
    m_funcBtn2->setCursor(Qt::PointingHandCursor);
    m_funcBtn2->setVisible(false);

    m_funcBtn3 = new QPushButton("", m_centerWidget);
    m_funcBtn3->setObjectName("funcBtn");
    m_funcBtn3->setFixedSize(120, 36);
    m_funcBtn3->setCursor(Qt::PointingHandCursor);
    m_funcBtn3->setVisible(false);

    m_buttonLayout->addWidget(m_funcBtn1);
    m_buttonLayout->addWidget(m_funcBtn2);
    m_buttonLayout->addWidget(m_funcBtn3);

    // 添加到布局
    m_centerLayout->addStretch();
    m_centerLayout->addWidget(m_warmTextLabel);
    m_centerLayout->addSpacing(8);
    m_centerLayout->addLayout(m_buttonLayout);
    m_centerLayout->addStretch();

    m_mainLayout->addWidget(m_centerWidget);

    // 连接信号
    if (m_titleBar) {
        connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &FunctionCustomWidget::onMinimizeRequested);
        connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &FunctionCustomWidget::onMaximizeRequested);
        connect(m_titleBar, &CustomTitleBar::closeRequested, this, &FunctionCustomWidget::onCloseRequested);
    }

    connect(m_funcBtn1, &QPushButton::clicked, this, &FunctionCustomWidget::onFunc1Clicked);
    connect(m_funcBtn2, &QPushButton::clicked, this, &FunctionCustomWidget::onFunc2Clicked);
    connect(m_funcBtn3, &QPushButton::clicked, this, &FunctionCustomWidget::onFunc3Clicked);
}

void FunctionCustomWidget::initStyle()
{
    QString styleSheet = QString(R"(
        FunctionCustomWidget {
            background-color: %1;
            border-radius: 8px;
        }

        QWidget#centerWidget {
            background-color: %1;
            border-radius: 0 0 8px 8px;
        }

        QLabel#warmTextLabel {
            color: %2;
            font-size: 15px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            background-color: transparent;
            line-height: 1.6;
        }

        QPushButton#funcBtn {
            background-color: %3;
            color: white;
            border: none;
            border-radius: 18px;
            font-size: 14px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
        }

        QPushButton#funcBtn:hover {
            background-color: %4;
        }

        QPushButton#funcBtn:pressed {
            background-color: %5;
        }
    )").arg(MedicalColors::PRIMARY_LIGHT)
      .arg(MedicalColors::TEXT_DARK)
      .arg(MedicalColors::PRIMARY_ACTIVE)
      .arg(MedicalColors::PRIMARY_MEDIUM)
      .arg(MedicalColors::PRIMARY_DARK);

    this->setStyleSheet(styleSheet);
}

void FunctionCustomWidget::setremimdertext(QString reminder_)
{
    if (m_warmTextLabel) {
        m_warmTextLabel->setText(reminder_);
    }
}

void FunctionCustomWidget::setTextbtnfunction(const int index_, const QString text_)
{
    switch(index_)
    {
        case 0:
        {
            if (m_funcBtn1) {
                m_funcBtn1->setText(text_);
                m_funcBtn1->setVisible(true);
            }
            break;
        }
        case 1:
        {
            if (m_funcBtn2) {
                m_funcBtn2->setText(text_);
                m_funcBtn2->setVisible(true);
            }
            break;
        }
        case 2:
        {
            if (m_funcBtn3) {
                m_funcBtn3->setText(text_);
                m_funcBtn3->setVisible(true);
            }
            break;
        }
        default: break;
    }
}

void FunctionCustomWidget::onFunc1Clicked()
{
    emit sender_1function_();
    close();
}

void FunctionCustomWidget::onFunc2Clicked()
{
    emit sender_2function_();
    close();
}

void FunctionCustomWidget::onFunc3Clicked()
{
    emit sender_3function_();
    close();
}

void FunctionCustomWidget::onMinimizeRequested()
{
    showMinimized();
}

void FunctionCustomWidget::onMaximizeRequested()
{
    if (m_isMaximized) {
        setGeometry(m_normalGeometry);
        m_isMaximized = false;
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(false);
        }
    } else {
        m_normalGeometry = geometry();
        QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        setGeometry(availableGeometry);
        m_isMaximized = true;
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(true);
        }
    }
}

void FunctionCustomWidget::onCloseRequested()
{
    close();
}

void FunctionCustomWidget::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
}

void FunctionCustomWidget::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
}

void FunctionCustomWidget::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
}

void FunctionCustomWidget::closeEvent(QCloseEvent *event)
{
    event->accept();
    deleteLater();
}
