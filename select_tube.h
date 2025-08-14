#ifndef SELECT_TUBE_H
#define SELECT_TUBE_H


#include <QCloseEvent>
#include <QMap>
#include <QWidget>

namespace Ui {
class Select_Tube;
}

class Select_Tube : public QWidget
{
    Q_OBJECT

public:
    explicit Select_Tube(QWidget *parent = 0);
    ~Select_Tube();

private:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *watched, QEvent *event);
    void Init_Tube_Pos(); //初始化试管得位置
    void Draw_Tube();
public slots:
    void SelTube_data(QMap<int, bool> select_item);
private slots:
    void on_OK_Sure_clicked();
    void on_Cancel_clicked();
signals:
    void Synchronous_Tbue(QList<int>); //同步试管
    void Synchronous_save_Tbue(QList<int>);
private:
    Ui::Select_Tube *ui;
    QPainter *m_paint;
    QMap<int,QPoint> m_Tube_Pos;
    QMap<int , QVector<QPoint>> m_cover_tube;
    QList<int> m_sel_tube;

    QMap<int, bool> m_Seltct_item;

};

#endif // SELECT_TUBE_H
