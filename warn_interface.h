#ifndef WARN_INTERFACE_H
#define WARN_INTERFACE_H

#include <QMouseEvent>
#include <QWidget>
#include "dilag/mybordercontainer.h"

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

    void settitle(QString title_);
    void setremtext(QString outputText);
    void replaceSupplyIndex(const int Index);

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
    void on_toolButton_clicked();
	void onBtnMenuMaxClicked();

private:
    void setConnectBtn();
    void InitStyle();

signals:
    void makesure(int, QString );
private:
    Ui::warn_interface *ui;
    MyBorderContainer *m_myborder;

    int m_indexSuppiles;

    bool max;
    bool mousePressed;
    QPoint mousePoint;
    QRect location;

};

#endif // WARN_INTERFACE_H
