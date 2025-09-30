#ifndef FRAMELESSWIDGETHELPER_H
#define FRAMELESSWIDGETHELPER_H

#include <QObject>
#include <QWidget>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QPushButton>

class FramelessWidgetHelper : public QObject
{
    Q_OBJECT

public:
    enum BorderEdge {
        EdgeNone = 0,
        EdgeTop = 1,
        EdgeBottom = 2,
        EdgeLeft = 4,
        EdgeRight = 8,
        EdgeTopLeft = EdgeTop | EdgeLeft,
        EdgeTopRight = EdgeTop | EdgeRight,
        EdgeBottomLeft = EdgeBottom | EdgeLeft,
        EdgeBottomRight = EdgeBottom | EdgeRight
    };

    explicit FramelessWidgetHelper(QWidget *parent = nullptr);

    // 封装函数：创建带自定义边框的无边框窗口
    static QWidget* createFramelessWidget(QWidget *contentWidget = nullptr,
                                         const QString &title = QString(),
                                         int borderWidth = 5,
                                         bool resizable = true,
                                         bool movable = true,
                                         bool showTitleBarButtons = true);

    // 为现有窗口应用无边框样式
    static void applyFramelessStyle(QWidget *widget,
                                   int borderWidth = 5,
                                   bool resizable = true,
                                   bool movable = true);

private slots:
    void onScreenChanged(QScreen *screen);
    void onCloseClicked();
    void onMaximizeClicked();
    void onMinimizeClicked();

private:
    QWidget *m_parentWidget;
    int m_borderWidth;
    bool m_resizable;
    bool m_movable;
    bool m_mousePressed;
    QPoint m_mousePressPos;
    QPoint m_windowPressPos;
    BorderEdge m_mousePressEdge;
    QRect m_dragStartGeometry;
    QPushButton *m_minimizeButton;
    QPushButton *m_maximizeButton;
    QPushButton *m_closeButton;

    bool eventFilter(QObject *obj, QEvent *event) override;
    BorderEdge getMouseEdge(const QPoint &pos);
    QCursor getEdgeCursor(BorderEdge edge);
    void updateCursor(const QPoint &pos);
    void handleMousePress(QMouseEvent *e);
    void handleMouseMove(QMouseEvent *e);
    void handleMouseRelease(QMouseEvent *e);
    void paintBorder();
    void updateMaximizeButton();
};


#endif // FRAMELESSWIDGETHELPER_H
