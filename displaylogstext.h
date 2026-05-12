#ifndef DISPLAYLOGSTEXT_H
#define DISPLAYLOGSTEXT_H

#include <QMouseEvent>
#include <QWidget>
#include <QLabel>
#include <QTextBrowser>
#include <QProgressBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class displayLogsText : public QWidget
{
    Q_OBJECT

public:
    explicit displayLogsText(QWidget *parent = 0);
    ~displayLogsText();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);

private:
    void _initUI();
    void _initStyle();
    void _initConnections();
    void _loadLogFile();
    void _appendtextshow(const QString logdata);

private:
    // 控件指针
    QLabel *m_titleLabel;
    QLabel *m_logoLabel;
    QLabel *m_timeLabel;
    QTextBrowser *m_textBrowser;
    QProgressBar *m_progressBar;
    QToolButton *m_closeBtn;
    QToolButton *m_maximizeBtn;
    QToolButton *m_updateBtn;
    QToolButton *m_clearBtn;

    // 布局
    QVBoxLayout *m_mainLayout;
    QWidget *m_headerWidget;
    QHBoxLayout *m_headerLayout;
    QWidget *m_contentWidget;
    QVBoxLayout *m_contentLayout;
    QWidget *m_progressWidget;
    QHBoxLayout *m_progressLayout;
    QWidget *m_buttonWidget;
    QHBoxLayout *m_buttonLayout;

    // 窗口拖拽相关
    QPoint m_mouseStartPoint;
    QColor m_bgColor;
    bool m_isMaximized;
};

#endif // DISPLAYLOGSTEXT_H
