#include "customtitlebar.h"
#include <QMouseEvent>
#include <QApplication>
#include <QPainter>
#include <QLinearGradient>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// 医疗淡蓝色主题色系
namespace MedicalColors {
    const QString PRIMARY_LIGHT = "#E8F4FD";      // 最浅的背景色
    const QString PRIMARY_SOFT = "#B8DFF7";       // 柔和淡蓝
    const QString PRIMARY_MEDIUM = "#7EC8F0";     // 中等淡蓝
    const QString PRIMARY_ACTIVE = "#5BB8E8";     // 激活状态
    const QString PRIMARY_DARK = "#3A9BD4";       // 深蓝用于边框
    const QString TEXT_DARK = "#2C5F8A";          // 深色文字
    const QString TEXT_LIGHT = "#4A7FA3";         // 浅色文字
    const QString DIVIDER = "#D4EAF5";            // 分割线颜色
}

CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent)
    , m_dragging(false)
    , m_isMaximized(false)
    , m_isActive(true)
    , m_isHovering(false)
    , m_highlightOpacity(0)
    , m_hoverTimer(nullptr)
	, m_highlightAnimation(nullptr)
{
    setFixedHeight(44);
    setAttribute(Qt::WA_StyledBackground, true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // 创建控件
    m_titleLabel = new QLabel("医疗影像管理系统", this);
    m_titleIcon = new QLabel(this);
    m_hoverTimer = new QTimer(this);
    m_hoverTimer->setSingleShot(true);
    m_hoverTimer->setInterval(100);

	// 创建高亮动画
	m_highlightAnimation = new QPropertyAnimation(this, "highlightOpacity");
	m_highlightAnimation->setDuration(200);
	m_highlightAnimation->setEasingCurve(QEasingCurve::OutCubic);

    // 设置图标
    m_titleIcon->setFixedSize(24, 24);
    m_titleIcon->setAlignment(Qt::AlignCenter);

    QPixmap iconPixmap(":/Picture/suowei.png");
    if (!iconPixmap.isNull()) {
        m_titleIcon->setPixmap(iconPixmap.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        // 医疗风格的默认图标 - 十字标志
        m_titleIcon->setText("🏥");
        m_titleIcon->setStyleSheet("color: #3A9BD4; font-size: 18px; background-color: transparent;");
    }

    // 标题标签样式
    m_titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_titleLabel->setStyleSheet(
        "color: #2C5F8A;"
        "font-weight: 500;"
        "font-size: 13px;"
        "font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';"
        "background-color: transparent;"
        "letter-spacing: 0.5px;"
    );

    // 创建按钮
    m_minimizeBtn = new QPushButton("−", this);
    m_maximizeBtn = new QPushButton("□", this);
    m_closeBtn = new QPushButton("✕", this);

    // 更新样式
    updateStyleSheet();

    // 布局
    QHBoxLayout *m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(12, 0, 12, 0);
    m_layout->setSpacing(8);
    m_layout->addWidget(m_titleIcon);
    m_layout->addWidget(m_titleLabel, 1);
    m_layout->addWidget(m_minimizeBtn);
    m_layout->addWidget(m_maximizeBtn);
    m_layout->addWidget(m_closeBtn);

    // 连接信号槽
    connect(m_minimizeBtn, &QPushButton::clicked, this, &CustomTitleBar::minimizeRequested);
    connect(m_closeBtn, &QPushButton::clicked, this, &CustomTitleBar::closeRequested);
    connect(m_maximizeBtn, &QPushButton::clicked, this, &CustomTitleBar::onMaximizeClicked);
    connect(m_hoverTimer, &QTimer::timeout, this, &CustomTitleBar::onHoverTimeout);
}

CustomTitleBar::~CustomTitleBar()
{
}

void CustomTitleBar::setTitle(const QString &title)
{
    if (m_titleLabel) {
        m_titleLabel->setText(title);
    }
}

void CustomTitleBar::setIcon(const QPixmap &icon)
{
    if (m_titleIcon && !icon.isNull()) {
        m_titleIcon->setPixmap(icon.scaled(24, 24, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_titleIcon->setText("");
    }
}

void CustomTitleBar::setActive(bool active)
{
    m_isActive = active;
    updateStyleSheet();
    update();
}


void CustomTitleBar::updateStyleSheet()
{
    // 按钮样式
    if (m_minimizeBtn) {
        m_minimizeBtn->setStyleSheet(getButtonStyle(MedicalColors::PRIMARY_MEDIUM, MedicalColors::PRIMARY_DARK));
    }
    if (m_maximizeBtn) {
        m_maximizeBtn->setStyleSheet(getButtonStyle(MedicalColors::PRIMARY_MEDIUM, MedicalColors::PRIMARY_DARK));
    }
    if (m_closeBtn) {
        m_closeBtn->setStyleSheet(getButtonStyle("#E8746C", "#D9534F", true));
    }
}

QString CustomTitleBar::getButtonStyle(const QString &hoverColor, const QString &pressedColor, bool isCloseBtn)
{
    QString baseStyle = R"(
        QPushButton {
            background-color: transparent;
            color: %1;
            border: none;
            font-size: 14px;
            font-weight: normal;
            width: 32px;
            height: 32px;
            border-radius: 6px;
            font-family: 'Segoe UI', 'Microsoft YaHei';
        }
        QPushButton:hover {
            background-color: %2;
            color: %3;
        }
        QPushButton:pressed {
            background-color: %4;
            color: %5;
        }
    )";

    QString textColor = MedicalColors::TEXT_DARK;
    QString hoverTextColor = isCloseBtn ? "white" : MedicalColors::TEXT_DARK;
    QString pressedTextColor = isCloseBtn ? "white" : MedicalColors::TEXT_DARK;

    return baseStyle
        .arg(textColor)
        .arg(hoverColor)
        .arg(hoverTextColor)
        .arg(pressedColor)
        .arg(pressedTextColor);
}

void CustomTitleBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制渐变背景
    QLinearGradient gradient(0, 0, width(), 0);
    gradient.setColorAt(0, QColor(MedicalColors::PRIMARY_LIGHT));
    gradient.setColorAt(0.5, QColor(MedicalColors::PRIMARY_SOFT));
    gradient.setColorAt(1, QColor(MedicalColors::PRIMARY_LIGHT));

    painter.fillRect(rect(), gradient);

    // 绘制底部高光线
    if (m_isHovering && m_highlightOpacity > 0) {
        QLinearGradient highlightGrad(0, height() - 2, 0, height());
        highlightGrad.setColorAt(0, QColor(255, 255, 255, m_highlightOpacity));
        highlightGrad.setColorAt(1, QColor(255, 255, 255, 0));
        painter.fillRect(0, height() - 2, width(), 2, highlightGrad);
    }

    // 绘制底部边框线
    painter.setPen(QPen(QColor(MedicalColors::DIVIDER), 1));
    painter.drawLine(0, height() - 1, width(), height() - 1);

    // 绘制激活指示条（窗口激活时显示）
    if (m_isActive) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(MedicalColors::PRIMARY_ACTIVE));
        painter.drawRoundedRect(0, height() - 2, width(), 2, 1, 1);
    }
}
void CustomTitleBar::setHighlightOpacity(int opacity)
{
	if (m_highlightOpacity != opacity) {
		m_highlightOpacity = opacity;
		update();  // 触发重绘
	}
}

int CustomTitleBar::getHighlightOpacity() const
{
	return m_highlightOpacity;
}

void CustomTitleBar::enterEvent(QEvent *event)
{
    m_isHovering = true;
    if (m_hoverTimer) {
        m_hoverTimer->start();
    }

	// 启动淡入动画
	if (m_highlightAnimation) {
		m_highlightAnimation->stop();
		m_highlightAnimation->setEndValue(40);
		m_highlightAnimation->start();
	}

    QWidget::enterEvent(event);
}

void CustomTitleBar::leaveEvent(QEvent *event)
{
    m_isHovering = false;

	// 启动淡出动画
	if (m_highlightAnimation) {
		m_highlightAnimation->stop();
		m_highlightAnimation->setEndValue(0);
		m_highlightAnimation->start();
	}

    //m_highlightOpacity = 0;
    //update();
    QWidget::leaveEvent(event);
}

void CustomTitleBar::onHoverTimeout()
{
	//// 这个函数现在只需要设置动画结束值
	//if (m_highlightAnimation && m_isHovering) {
	//	m_highlightAnimation->setEndValue(40);
	//	m_highlightAnimation->start();
	//}
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragStartPos = event->globalPos();
        m_dragging = true;
        event->accept();
    }
    QWidget::mousePressEvent(event);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QWidget *parentWin = window();
        if (parentWin && !parentWin->isMaximized()) {
            QPoint delta = event->globalPos() - m_dragStartPos;
            parentWin->move(parentWin->pos() + delta);
            m_dragStartPos = event->globalPos();
        }
        event->accept();
    }
    QWidget::mouseMoveEvent(event);
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
    QWidget::mouseReleaseEvent(event);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit maximizeRequested();
        event->accept();
    }
    QWidget::mouseDoubleClickEvent(event);
}

void CustomTitleBar::onMaximizeClicked()
{
    emit maximizeRequested();
}

void CustomTitleBar::updateMaximizeButton(bool isMaximized)
{
    m_isMaximized = isMaximized;
    if (m_maximizeBtn) {
        m_maximizeBtn->setText(isMaximized ? "❐" : "□");
    }
}
