#ifndef CONNECTTOINSTRUMENT_H
#define CONNECTTOINSTRUMENT_H

#include "mybordercontainer.h"
#include <QDialog>
#include <QTimer>

namespace Ui {
class ConnectToInstrument;
}

class ConnectToInstrument : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectToInstrument(QString titlestr, QString warmtext, QWidget *parent = 0);
    ~ConnectToInstrument();

    void settitle(QString title_);
    void setremtext(QString outputText);
    void notifybuttonDisplayName(QString surename, QString cancelname);

signals:
    void giveupTestSample();

    void tryAgainTestSample();

    void pendingTimeout();

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

    virtual void closeEvent(QCloseEvent* event) override;


    void keyPressEvent(QKeyEvent *event){
        if (event->key() == Qt::Key_Return){

        }
    }
private slots:
    void on_pushButton_ok_clicked();
    void onBtnMenuMaxClicked();
    void onTimeout();

private:
    void InitStyle();
    void setConnectBtn();

private:
    Ui::ConnectToInstrument *ui;
    MyBorderContainer *m_myborder;
    bool max;
    bool mousePressed;
    QPoint mousePoint;
    QRect location;

    QTimer *m_timer;
    int remainingSeconds = 60; // 初始60秒
};

#endif // CONNECTTOINSTRUMENT_H
