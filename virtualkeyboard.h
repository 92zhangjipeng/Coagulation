#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H


#pragma once
#pragma execution_character_set("utf-8")
#include <QWidget>
#include <QLineEdit>
#include <QTimer>
#include <QPushButton>
#include <QMouseEvent>
#include <QDesktopWidget>

namespace Ui {
class Virtualkeyboard;
}

class Virtualkeyboard : public QWidget
{
    Q_OBJECT

public:
    explicit Virtualkeyboard(QWidget *parent = 0 );
    ~Virtualkeyboard();

signals:


public:
    //单例模式，保证一个程序只存在一个输入法实例
    static Virtualkeyboard * Instance()
    {
        if(!_instance)
        {
            _instance = new Virtualkeyboard;
        }

        return _instance;
    }

    static void Release()
    {
        if(_instance != NULL)
        {
            delete _instance;

            _instance = NULL;
        }
    }

    void Init(QString style, int fontSize);

protected:
    bool eventFilter(QObject *obj, QEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);

private slots:

    void focusChanged(QWidget *oldWidget, QWidget *nowWidget);

    void btn_clicked();

    void changeStyle(QString topColor, QString bottomColor,
                     QString borderColor, QString textColor);

    void reClicked();
public slots:
    void CloseWidget();

    void ShowPointBtnSlot(bool);
private:
    Ui::Virtualkeyboard *ui;

    static Virtualkeyboard *_instance;       //实例对象

    QPoint mouseStartPoint;

    bool isPressBackBtn;            //是否长按退格键
    bool isFirst;                   //是否首次加载

    QPushButton *btnPress;          //长按按钮
    QTimer *backBtnTimert;          //退格键定时器
    QWidget *currentWidget;         //当前焦点的对象
    QLineEdit *currentLineEdit;     //当前焦点的单行文本框

    QString currentEditType;        //当前焦点控件的类型
    QString currentStyle;           //当前小键盘样式
    int currentFontSize;            //当前输入法面板字体大小

    bool checkPress();              //校验当前长按的按钮//初始化属性
    void ChangeStyle(QString currentStyle);             //改变样式

    void insertValue(QString value);//插入值到当前焦点控件
    void deleteValue();             //删除当前焦点控件的一个字符
    void clearValue();             //clear当前焦点控件的一个字符
};

#endif // VIRTUALKEYBOARD_H


