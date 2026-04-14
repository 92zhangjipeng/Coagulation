#ifdef _MSC_VER
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
#endif

#include "customtitlebar.h"
#include <QMouseEvent>
#include <QApplication>
#include <QStyle>

CustomTitleBar::CustomTitleBar(QWidget *parent)
    : QWidget(parent)
    , m_dragging(false)
    , m_isMaximized(false)
{
    // 设置标题栏固定高度和背景色（医疗蓝风格）
    setFixedHeight(40);
    setStyleSheet("background-color: #4a90e2; border-bottom: 2px solid #2c6fb8;");

    // 创建控件
    m_titleLabel = new QLabel("医疗图像库", this);
    m_titleLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px; padding-left: 12px;");

    m_minimizeBtn = new QPushButton("—", this);
    m_maximizeBtn = new QPushButton("□", this);
    m_closeBtn    = new QPushButton("×", this);

    // 按钮统一样式（医疗蓝色风格）
    QString btnStyle = R"(
        QPushButton {
            background-color: transparent;
            color: white;
            border: none;
            font-size: 16px;
            font-weight: bold;
            width: 32px;
            height: 32px;
            border-radius: 4px;
        }
        QPushButton:hover {
            background-color: #5a9ae2;
        }
        QPushButton:pressed {
            background-color: #3a80d2;
        }
    )";
    m_minimizeBtn->setStyleSheet(btnStyle);
    m_maximizeBtn->setStyleSheet(btnStyle);
    m_closeBtn->setStyleSheet(btnStyle + "QPushButton:hover { background-color: #e74c3c; color: white; }");

    // 布局
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 2, 0);
    m_layout->setSpacing(2);
    m_layout->addWidget(m_titleLabel, 1);
    m_layout->addWidget(m_minimizeBtn);
    m_layout->addWidget(m_maximizeBtn);
    m_layout->addWidget(m_closeBtn);

    // 连接信号槽
    connect(m_minimizeBtn, &QPushButton::clicked, this, &CustomTitleBar::minimizeRequested);
    connect(m_closeBtn,    &QPushButton::clicked, this, &CustomTitleBar::closeRequested);
    connect(m_maximizeBtn, &QPushButton::clicked, this, &CustomTitleBar::onMaximizeClicked);
}

CustomTitleBar::~CustomTitleBar()
{
}

void CustomTitleBar::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void CustomTitleBar::setIcon(const QPixmap &icon)
{
    m_titleLabel->setPixmap(icon.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
        QWidget *parentWin = window();  // 获取顶层窗口
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
    m_maximizeBtn->setText(isMaximized ? "❐" : "□");
}
