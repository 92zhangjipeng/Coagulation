#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>
#include <QPropertyAnimation>

class CustomTitleBar : public QWidget
{
    Q_OBJECT
	Q_PROPERTY(int highlightOpacity READ getHighlightOpacity WRITE setHighlightOpacity)

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);
    ~CustomTitleBar();

    // 设置标题文本
    void setTitle(const QString &title);
    // 设置窗口图标（可选）
    void setIcon(const QPixmap &icon);
    // 更新最大化按钮状态
    void updateMaximizeButton(bool isMaximized);
    // 设置窗口激活状态
    void setActive(bool active);

	void setHighlightOpacity(int opacity);
	int getHighlightOpacity() const;

signals:
    // 发送窗口操作请求（供外部连接，也可以直接在内部操作父窗口）
    void closeRequested();
    void minimizeRequested();
    void maximizeRequested();

protected:
    // 实现窗口拖动
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void enterEvent(QEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onMaximizeClicked();
    void onHoverTimeout();

private:
    void updateStyleSheet();

	QPropertyAnimation *m_highlightAnimation;  // 添加动画成员
    QString getButtonStyle(const QString &hoverColor, const QString &pressedColor, bool isCloseBtn = false);

    QLabel      *m_titleLabel;
    QLabel      *m_titleIcon;
    QPushButton *m_minimizeBtn;
    QPushButton *m_maximizeBtn;
    QPushButton *m_closeBtn;
    QHBoxLayout *m_layout;

    QPoint      m_dragStartPos;
    bool        m_dragging;
    bool        m_isMaximized;
    bool        m_isActive;
    bool        m_isHovering;
    int         m_highlightOpacity;
    QTimer      *m_hoverTimer;
};

#endif // CUSTOMTITLEBAR_H
