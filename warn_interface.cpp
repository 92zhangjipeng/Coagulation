#include "warn_interface.h"
#include "ui_warn_interface.h"
#include <QDesktopWidget>
#include <QTimer>
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

warn_interface::warn_interface(QString titlestr, QString warmtext, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::warn_interface),
    m_titleBar(nullptr),
    m_centerWidget(nullptr),
    m_mainLayout(nullptr),
    m_centerLayout(nullptr),
    m_warmTextLabel(nullptr),
    m_confirmBtn(nullptr),
    m_indexSuppiles(-1),
    m_isMaximized(false)
{
    ui->setupUi(this);

    // 隐藏默认标题栏
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

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

warn_interface::~warn_interface()
{
    delete ui;
}

void warn_interface::initUI()
{
    // 设置窗口大小
    setFixedSize(380, 220);

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
    m_centerLayout->setContentsMargins(20, 20, 20, 20);
    m_centerLayout->setSpacing(16);

    // 创建提示图标和文本的垂直布局 - 改为垂直居中
    QVBoxLayout *centerMessageLayout = new QVBoxLayout();
    centerMessageLayout->setSpacing(12);
    centerMessageLayout->setAlignment(Qt::AlignCenter);

    // 提示图标 - 居中
    QLabel *iconLabel = new QLabel(m_centerWidget);
    iconLabel->setFixedSize(48, 48);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setObjectName("iconLabel");

    // 尝试加载图标资源，如果没有则显示emoji
    QPixmap iconPixmap(":/Picture/icon_title/提示.png");
    if (!iconPixmap.isNull()) {
        iconLabel->setPixmap(iconPixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        iconLabel->setText("⚠️");
        iconLabel->setStyleSheet("font-size: 40px; background-color: transparent;");
    }

    // 警告文本标签 - 居中显示
    m_warmTextLabel = new QLabel(m_centerWidget);
    m_warmTextLabel->setObjectName("warmTextLabel");
    m_warmTextLabel->setAlignment(Qt::AlignCenter);
    m_warmTextLabel->setWordWrap(true);
    m_warmTextLabel->setMinimumWidth(280);

    // 确认按钮
    m_confirmBtn = new QPushButton("确 定", m_centerWidget);
    m_confirmBtn->setObjectName("confirmBtn");
    m_confirmBtn->setFixedSize(100, 32);
    m_confirmBtn->setCursor(Qt::PointingHandCursor);

    // 添加到布局
    centerMessageLayout->addWidget(iconLabel, 0, Qt::AlignCenter);
    centerMessageLayout->addWidget(m_warmTextLabel, 0, Qt::AlignCenter);

    m_centerLayout->addStretch();
    m_centerLayout->addLayout(centerMessageLayout);
    m_centerLayout->addStretch();
    m_centerLayout->addWidget(m_confirmBtn, 0, Qt::AlignCenter);

    m_mainLayout->addWidget(m_centerWidget);

    // 连接信号
    connect(m_confirmBtn, &QPushButton::clicked, this, &warn_interface::onConfirmClicked);

    if (m_titleBar) {
        connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &warn_interface::onMinimizeRequested);
        connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &warn_interface::onMaximizeRequested);
        connect(m_titleBar, &CustomTitleBar::closeRequested, this, &warn_interface::onCloseRequested);
    }
}

void warn_interface::initStyle()
{
    QString styleSheet = QString(R"(
        warn_interface {
            background-color: %1;
            border-radius: 8px;
        }

        QWidget#centerWidget {
            background-color: %1;
            border-radius: 0 0 8px 8px;
        }

        QLabel#warmTextLabel {
            color: %2;
            font-size: 16px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            background-color: transparent;
            line-height: 1.6;
        }

        QPushButton#confirmBtn {
            background-color: %3;
            color: white;
            border: none;
            border-radius: 16px;
            font-size: 14px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
        }

        QPushButton#confirmBtn:hover {
            background-color: %4;
        }

        QPushButton#confirmBtn:pressed {
            background-color: %5;
        }
    )").arg(MedicalColors::PRIMARY_LIGHT)
      .arg(MedicalColors::TEXT_DARK)
      .arg(MedicalColors::PRIMARY_ACTIVE)
      .arg(MedicalColors::PRIMARY_MEDIUM)
      .arg(MedicalColors::PRIMARY_DARK);

    this->setStyleSheet(styleSheet);
}

void warn_interface::showTransientWarning(const QString& title, const QString& message) {
    warn_interface* warn = new warn_interface(title, message);
    warn->setAttribute(Qt::WA_DeleteOnClose);
    warn->setWindowModality(Qt::NonModal);
    warn->show();
    warn->raise();
    warn->activateWindow();
}

void warn_interface::showTimeTransientWarning(const QString& title, const QString& message, int autoCloseMs) {
    warn_interface* warn = new warn_interface(title, message);
    warn->setAttribute(Qt::WA_DeleteOnClose);
    warn->setWindowModality(Qt::NonModal);

    // 设置自动关闭定时器
    QTimer::singleShot(autoCloseMs, warn, &warn_interface::close);

    warn->show();
    warn->raise();
    warn->activateWindow();
}

void warn_interface::settitle(QString title_)
{
    if (m_titleBar) {
        m_titleBar->setTitle(title_);
    }
}

void warn_interface::setremtext(QString outputText)
{
    if (m_warmTextLabel) {
        m_warmTextLabel->setText(outputText);
    }
}

void warn_interface::replaceSupplyIndex(const int Index)
{
    m_indexSuppiles = Index;
}

void warn_interface::onConfirmClicked()
{
    emit makesure(m_indexSuppiles, m_titleBar ? m_titleBar->windowTitle() : "");
    close();
}

void warn_interface::onMinimizeRequested()
{
    showMinimized();
}

void warn_interface::onMaximizeRequested()
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

void warn_interface::onCloseRequested()
{
    close();
}

void warn_interface::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
}

void warn_interface::mouseMoveEvent(QMouseEvent *e)
{
    QWidget::mouseMoveEvent(e);
}

void warn_interface::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mouseReleaseEvent(e);
}

void warn_interface::closeEvent(QCloseEvent *event)
{
    event->accept();
    deleteLater();
}
