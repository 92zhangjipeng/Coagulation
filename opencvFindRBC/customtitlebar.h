#ifndef CUSTOMTITLEBAR_H
#define CUSTOMTITLEBAR_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);
    ~CustomTitleBar();

    // 设置标题文本
    void setTitle(const QString &title);
    // 设置窗口图标（可选）
    void setIcon(const QPixmap &icon);

    void updateMaximizeButton(bool isMaximized);

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

private slots:
    void onMaximizeClicked();

private:


    QLabel      *m_titleLabel;
    QPushButton *m_minimizeBtn;
    QPushButton *m_maximizeBtn;
    QPushButton *m_closeBtn;
    QHBoxLayout *m_layout;

    QPoint      m_dragStartPos;
    bool        m_dragging;
    bool        m_isMaximized;      // 记录窗口是否最大化（用于按钮样式切换）
};

#endif // CUSTOMTITLEBAR_H
