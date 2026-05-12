#ifndef FUNCTIONCUSTOMWIDGET_H
#define FUNCTIONCUSTOMWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "opencvFindRBC/customtitlebar.h"

namespace Ui {
class FunctionCustomWidget;
}

class FunctionCustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionCustomWidget(QString titlestr, QString warmtext, QWidget *parent = 0);
    ~FunctionCustomWidget();

    void setTextbtnfunction(const int index_, const QString text_);
    void setremimdertext(QString reminder_);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *) override;
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onMinimizeRequested();
    void onMaximizeRequested();
    void onCloseRequested();
    void onFunc1Clicked();
    void onFunc2Clicked();
    void onFunc3Clicked();

private:
    void initUI();
    void initStyle();

    Ui::FunctionCustomWidget *ui;
    CustomTitleBar *m_titleBar;

    QWidget *m_centerWidget;
    QVBoxLayout *m_mainLayout;
    QVBoxLayout *m_centerLayout;

    QLabel *m_warmTextLabel;
    QPushButton *m_funcBtn1;
    QPushButton *m_funcBtn2;
    QPushButton *m_funcBtn3;

    QHBoxLayout *m_buttonLayout;

    bool m_isMaximized;
    QRect m_normalGeometry;

signals:
    void sender_1function_();
    void sender_2function_();
    void sender_3function_();
};

#endif // FUNCTIONCUSTOMWIDGET_H
