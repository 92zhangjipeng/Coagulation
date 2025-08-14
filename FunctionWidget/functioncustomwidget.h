#ifndef FUNCTIONCUSTOMWIDGET_H
#define FUNCTIONCUSTOMWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <dilag/mybordercontainer.h>

namespace Ui {
class FunctionCustomWidget;
}

class FunctionCustomWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FunctionCustomWidget(QString titlestr, QString warmtext,QWidget *parent = 0);
    ~FunctionCustomWidget();

    void setTextbtnfunction(const int index_,const QString text_);

    void setremimdertext(QString reminder_);

protected:
    bool eventFilter(QObject *obj, QEvent *event);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

    void keyPressEvent(QKeyEvent *event){
        if (event->key() == Qt::Key_Return){

        }
    }
private:
    void setConnectBtn();
    void InitStyle();

private slots:
   void onBtnMenuMaxClicked();

signals:
    void sender_1function_();
    void sender_2function_();
    void sender_3function_();

private:
    Ui::FunctionCustomWidget *ui;
    MyBorderContainer *m_myborder;

    bool max;
    bool mousePressed;
    QPoint mousePoint;
    QRect location;
};

#endif // FUNCTIONCUSTOMWIDGET_H
