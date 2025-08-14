#ifndef TUBECONTINUEDOING_H
#define TUBECONTINUEDOING_H

#include <QMouseEvent>
#include <QWidget>

namespace Ui {
class TubeContinueDoing;
}

class TubeContinueDoing : public QWidget
{
    Q_OBJECT

public:
    explicit TubeContinueDoing(QWidget *parent = 0);
    ~TubeContinueDoing();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_toolButton_runTest_clicked();


    void on_toolButton_close_clicked();

    void on_toolButton_PPPok_clicked();

    void on_toolButton_cruveCreat_clicked();

private:
    void AllZaxisBack();
    QByteArrayList creatAbsorbReagentCommand(); /*构造吸试剂命令*/

    QByteArrayList creatAnaemiaCommand();

signals:
    void scynTestCommand(int,const QByteArrayList);

    void sendcode(QByteArrayList);
private:
    Ui::TubeContinueDoing *ui;
    QColor bgmcColor;
    QPoint mouseStartPoint;
    QByteArrayList mStarArry;
};

#endif // TUBECONTINUEDOING_H
