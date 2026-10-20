#ifndef CUSTOMPROGRESSCONTROLS_H
#define CUSTOMPROGRESSCONTROLS_H

#include <QPaintEvent>
#include <QWidget>
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QTimer>

namespace Ui {
class customProgresscontrols;
}

class HexPlateProgressBar;

class customProgresscontrols : public QWidget
{
    Q_OBJECT

public:
    explicit customProgresscontrols(QWidget *parent = 0);
    ~customProgresscontrols();

    void    _settitle(const QString title);
    void    _setprogressvalue(const QString info, int numstep);
    void    _setprogressRand(int min,int max);
    void    _setprogresstotalnum(int numtotal);
    void    _setlabletext(const QString reminder);
    void    setBusyWait(const QString &hint);
    void    _setMinvalue(int val);
    void    _setMaxvalue(int val);
    int     _getprogressdata();

protected:
    void    paintEvent(QPaintEvent *event)override;
    void    showEvent(QShowEvent *event) override;  // 重写显示事件
    void    closeEvent(QCloseEvent *event) override; // 重写关闭事件
    void    mousePressEvent(QMouseEvent *event) override; // 添加鼠标事件支持拖动
    void    mouseMoveEvent(QMouseEvent *event)override;
private:

private:
    Ui::customProgresscontrols *ui;
    HexPlateProgressBar *m_capsuleBar;
    int m_totalnum;

    QScopedPointer <QPropertyAnimation> m_showAnimation;
    QScopedPointer <QPropertyAnimation> m_closeAnimation;

    //QPropertyAnimation *m_showAnimation ;  // 显示动画
    //QPropertyAnimation *m_closeAnimation; // 关闭动画
    bool m_isAnimating = false;          // 动画状态标志
    QPoint m_dragPosition; // 拖动位置记录
};

#endif // CUSTOMPROGRESSCONTROLS_H
