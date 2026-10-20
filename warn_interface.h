#ifndef WARN_INTERFACE_H
#define WARN_INTERFACE_H

#include <QMouseEvent>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "opencvFindRBC/customtitlebar.h"

namespace Ui {
class warn_interface;
}

class warn_interface : public QWidget
{
    Q_OBJECT

public:
    explicit warn_interface(QString titlestr, QString warmtext, QWidget *parent = 0);
    ~warn_interface();

    // 新增静态方法封装弹窗逻辑
    static void showTransientWarning(const QString& title, const QString& message);
    static void showTimeTransientWarning(const QString& title, const QString& message, int autoCloseMs = 3000);

    void settitle(QString title_);
    void setremtext(QString outputText);
    void replaceSupplyIndex(const int Index);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    virtual void closeEvent(QCloseEvent* event) override;

private slots:
    void onConfirmClicked();
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();

private:
    void initUI();
    void initStyle();

    Ui::warn_interface *ui;
    CustomTitleBar *m_titleBar;

    QWidget *m_centerWidget;
    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_centerLayout;

    QLabel *m_warmTextLabel;
    QPushButton *m_confirmBtn;

    int m_indexSuppiles;
    bool m_isMaximized;
    QRect m_normalGeometry;

signals:
    void makesure(int, QString);
};

#endif // WARN_INTERFACE_H
