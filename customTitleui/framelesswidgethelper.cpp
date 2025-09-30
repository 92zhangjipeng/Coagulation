#pragma execution_character_set("utf-8")
#include "framelesswidgethelper.h"
#include <QGraphicsDropShadowEffect>

FramelessWidgetHelper::FramelessWidgetHelper(QWidget *parent)
    : QObject(parent)
    , m_parentWidget(parent)
    , m_borderWidth(5)
    , m_resizable(true)
    , m_movable(true)
    , m_mousePressed(false)
    , m_mousePressEdge(EdgeNone)
    , m_minimizeButton(nullptr)
    , m_maximizeButton(nullptr)
    , m_closeButton(nullptr)
{
    if (m_parentWidget) {
        m_parentWidget->setWindowFlags(m_parentWidget->windowFlags() | Qt::FramelessWindowHint);

        m_parentWidget->installEventFilter(this);

        // 连接屏幕变化信号
        connect(qApp, &QApplication::primaryScreenChanged, this, &FramelessWidgetHelper::onScreenChanged);
    }
}

QWidget* FramelessWidgetHelper::createFramelessWidget(QWidget *contentWidget,
	const QString &title,
	int borderWidth,
	bool resizable,
	bool movable,
	bool showTitleBarButtons)
{
	// 创建主窗口
	QWidget *mainWidget = new QWidget();
	mainWidget->setWindowFlags(mainWidget->windowFlags() | Qt::FramelessWindowHint);

	// 创建中心容器
	QWidget *centralWidget = new QWidget(mainWidget);
	centralWidget->setObjectName("framelessCentralWidget");
	centralWidget->setStyleSheet(QString(
		"#framelessCentralWidget {"
		"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
		"        stop:0 #ffffff, stop:0.5 #f8f9fa, stop:1 #e9ecef);"
		"    border: 1px solid #dee2e6;"
		"    border-radius: 12px;"
		"}"
		));

	// 创建布局
	QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	// 添加标题栏（如果提供了标题或需要显示按钮）
	if (!title.isEmpty() || showTitleBarButtons) {
		QWidget *titleBar = new QWidget(centralWidget);
		titleBar->setFixedHeight(35);
		titleBar->setStyleSheet(
			"QWidget {"
			"    background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
			"        stop:0 #6c757d, stop:1 #495057);"
			"    border-top-left-radius: 12px;"
			"    border-top-right-radius: 12px;"
			"    color: white;"
			"    border-bottom: 1px solid #495057;"
			"}"
			);

		QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
		titleLayout->setContentsMargins(12, 8, 12, 8); // 调整内边距

		QLabel *titleLabel = new QLabel(title);
		titleLabel->setStyleSheet("color: white; font-weight: bold; font-size: 14px;");

		titleLayout->addWidget(titleLabel);
		titleLayout->addStretch();

		// 添加窗口控制按钮
		if (showTitleBarButtons) {
			// 最小化按钮 - 使用ASCII减号
			QPushButton *minimizeButton = new QPushButton("-");
			minimizeButton->setFixedSize(20, 20);
			minimizeButton->setStyleSheet(
				"QPushButton {"
				"    background: transparent;"
				"    border: 1px solid #adb5bd;"
				"    border-radius: 3px;"
				"    color: white;"
				"    font-weight: bold;"
				"}"
				"QPushButton:hover {"
				"    background: #5a6268;"
				"}"
				);

			// 最大化/还原按钮 - 使用ASCII字符
			QPushButton *maximizeButton = new QPushButton("[]");
			maximizeButton->setFixedSize(20, 20);
			maximizeButton->setStyleSheet(
				"QPushButton {"
				"    background: transparent;"
				"    border: 1px solid #adb5bd;"
				"    border-radius: 3px;"
				"    color: white;"
				"    font-weight: bold;"
				"}"
				"QPushButton:hover {"
				"    background: #5a6268;"
				"}"
				);

			// 关闭按钮 - 使用ASCII字符
			QPushButton *closeButton = new QPushButton("X");
			closeButton->setFixedSize(20, 20);
			closeButton->setStyleSheet(
				"QPushButton {"
				"    background: transparent;"
				"    border: 1px solid #adb5bd;"
				"    border-radius: 3px;"
				"    color: white;"
				"    font-weight: bold;"
				"}"
				"QPushButton:hover {"
				"    background: #e74c3c;"
				"}"
				);

			titleLayout->addWidget(minimizeButton);
			titleLayout->addWidget(maximizeButton);
			titleLayout->addWidget(closeButton);

			// 存储按钮指针供后续使用
			FramelessWidgetHelper *helper = new FramelessWidgetHelper(mainWidget);
			helper->m_minimizeButton = minimizeButton;
			helper->m_maximizeButton = maximizeButton;
			helper->m_closeButton = closeButton;

			// 连接按钮信号
			connect(minimizeButton, &QPushButton::clicked, helper, &FramelessWidgetHelper::onMinimizeClicked);
			connect(maximizeButton, &QPushButton::clicked, helper, &FramelessWidgetHelper::onMaximizeClicked);
			connect(closeButton, &QPushButton::clicked, helper, &FramelessWidgetHelper::onCloseClicked);
		}

		mainLayout->addWidget(titleBar);
	}

	// 添加内容widget
	if (contentWidget) {
		mainLayout->addWidget(contentWidget);
	}
	else {
		// 如果没有提供内容widget，创建一个默认的
		QWidget *defaultContent = new QWidget();
		defaultContent->setMinimumSize(400, 300);
		mainLayout->addWidget(defaultContent);
	}

	// 设置中心widget的布局
	QVBoxLayout *centralLayout = new QVBoxLayout(mainWidget);
	centralLayout->setContentsMargins(borderWidth, borderWidth, borderWidth, borderWidth);
	centralLayout->addWidget(centralWidget);

	// 设置最小尺寸
	mainWidget->setMinimumSize(200, 150);

	// 应用无边框助手
	FramelessWidgetHelper *helper = mainWidget->findChild<FramelessWidgetHelper*>();
	if (!helper) {
		helper = new FramelessWidgetHelper(mainWidget);
	}
	helper->m_borderWidth = borderWidth;
	helper->m_resizable = resizable;
	helper->m_movable = movable;

	return mainWidget;
}
void FramelessWidgetHelper::applyFramelessStyle(QWidget *widget, int borderWidth, bool resizable, bool movable)
{
    if (!widget) return;

    FramelessWidgetHelper *helper = new FramelessWidgetHelper(widget);
    helper->m_borderWidth = borderWidth;
    helper->m_resizable = resizable;
    helper->m_movable = movable;
    // 设置最小尺寸避免调整大小时的问题
    widget->setMinimumSize(100, 100);
}

void FramelessWidgetHelper::onScreenChanged(QScreen *screen)
{
    if (m_parentWidget && screen) {
        m_parentWidget->update();
    }
}

void FramelessWidgetHelper::onCloseClicked()
{
    if (m_parentWidget) {
        m_parentWidget->close();
    }
}

void FramelessWidgetHelper::onMaximizeClicked()
{
    if (!m_parentWidget) return;

    if (m_parentWidget->isMaximized()) {
        m_parentWidget->showNormal();
    } else {
        m_parentWidget->showMaximized();
    }
    updateMaximizeButton();
}

void FramelessWidgetHelper::onMinimizeClicked()
{
    if (m_parentWidget) {
        m_parentWidget->showMinimized();
    }
}

void FramelessWidgetHelper::updateMaximizeButton()
{
    if (m_maximizeButton) {
        if (m_parentWidget->isMaximized()) {
            m_maximizeButton->setText("]["); // 还原图标 ❐
        } else {
            m_maximizeButton->setText("[]"); // 最大化图标
        }
    }
}

bool FramelessWidgetHelper::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != m_parentWidget) return QObject::eventFilter(obj, event);

    switch (event->type()) {
    case QEvent::MouseButtonPress:
        handleMousePress(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseRelease(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMove(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Paint:
        paintBorder();
        break;
    case QEvent::WindowStateChange:
        updateMaximizeButton();
        break;
    default:
        break;
    }

    return QObject::eventFilter(obj, event);
}

FramelessWidgetHelper::BorderEdge FramelessWidgetHelper::getMouseEdge(const QPoint &pos)
{
    if (!m_parentWidget) return EdgeNone;

    int x = pos.x();
    int y = pos.y();
    int width = m_parentWidget->width();
    int height = m_parentWidget->height();

    BorderEdge edge = EdgeNone;

    if (x <= m_borderWidth) edge = static_cast<BorderEdge>(edge | EdgeLeft);
    if (x >= width - m_borderWidth) edge = static_cast<BorderEdge>(edge | EdgeRight);
    if (y <= m_borderWidth) edge = static_cast<BorderEdge>(edge | EdgeTop);
    if (y >= height - m_borderWidth) edge = static_cast<BorderEdge>(edge | EdgeBottom);

    return edge;
}

QCursor FramelessWidgetHelper::getEdgeCursor(BorderEdge edge)
{
    switch (edge) {
    case EdgeTop:
    case EdgeBottom:
        return QCursor(Qt::SizeVerCursor);
    case EdgeLeft:
    case EdgeRight:
        return QCursor(Qt::SizeHorCursor);
    case EdgeTopLeft:
    case EdgeBottomRight:
        return QCursor(Qt::SizeFDiagCursor);
    case EdgeTopRight:
    case EdgeBottomLeft:
        return QCursor(Qt::SizeBDiagCursor);
    default:
        return QCursor(Qt::ArrowCursor);
    }
}

void FramelessWidgetHelper::updateCursor(const QPoint &pos)
{
    if (!m_parentWidget || !m_resizable) return;

    BorderEdge edge = getMouseEdge(pos);
    m_parentWidget->setCursor(getEdgeCursor(edge));
}

void FramelessWidgetHelper::handleMousePress(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) return;

    m_mousePressed = true;
    m_mousePressPos = e->globalPos();
    m_windowPressPos = m_parentWidget->pos();
    m_mousePressEdge = getMouseEdge(e->pos());
    m_dragStartGeometry = m_parentWidget->geometry();
}

void FramelessWidgetHelper::handleMouseMove(QMouseEvent *e)
{
    if (!m_mousePressed) {
        updateCursor(e->pos());
        return;
    }

    QPoint delta = e->globalPos() - m_mousePressPos;

    if (m_mousePressEdge == EdgeNone) {
        // 移动窗口
        if (m_movable) {
            m_parentWidget->move(m_windowPressPos + delta);
        }
    } else {
        // 调整窗口大小
        if (m_resizable) {
            QRect newGeometry = m_dragStartGeometry;

            if (m_mousePressEdge & EdgeLeft) {
                newGeometry.setLeft(m_dragStartGeometry.left() + delta.x());
                if (newGeometry.width() < m_parentWidget->minimumWidth()) {
                    newGeometry.setLeft(newGeometry.right() - m_parentWidget->minimumWidth());
                }
            }
            if (m_mousePressEdge & EdgeRight) {
                newGeometry.setRight(m_dragStartGeometry.right() + delta.x());
                if (newGeometry.width() < m_parentWidget->minimumWidth()) {
                    newGeometry.setWidth(m_parentWidget->minimumWidth());
                }
            }
            if (m_mousePressEdge & EdgeTop) {
                newGeometry.setTop(m_dragStartGeometry.top() + delta.y());
                if (newGeometry.height() < m_parentWidget->minimumHeight()) {
                    newGeometry.setTop(newGeometry.bottom() - m_parentWidget->minimumHeight());
                }
            }
            if (m_mousePressEdge & EdgeBottom) {
                newGeometry.setBottom(m_dragStartGeometry.bottom() + delta.y());
                if (newGeometry.height() < m_parentWidget->minimumHeight()) {
                    newGeometry.setHeight(m_parentWidget->minimumHeight());
                }
            }

            // 确保尺寸有效
            if (newGeometry.isValid() &&
                newGeometry.width() >= m_parentWidget->minimumWidth() &&
                newGeometry.height() >= m_parentWidget->minimumHeight()) {
                m_parentWidget->setGeometry(newGeometry);
            }
        }
    }
}

void FramelessWidgetHelper::handleMouseRelease(QMouseEvent *e)
{
    Q_UNUSED(e)
    m_mousePressed = false;
    m_mousePressEdge = EdgeNone;
}

void FramelessWidgetHelper::paintBorder()
{
    if (!m_parentWidget) return;

    QPainter painter(m_parentWidget);
    painter.setRenderHint(QPainter::Antialiasing);

    // 只在调试时绘制边框
    #ifdef QT_DEBUG
    QColor borderColor(255, 0, 0, 100); // 红色半透明边框，便于调试
    painter.setPen(QPen(borderColor, 2));
    painter.drawRect(m_parentWidget->rect().adjusted(1, 1, -1, -1));
    #endif
}
