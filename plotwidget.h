#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H
#include <QWidget>
namespace Ui {
class PlotWidget;
}

class PlotWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PlotWidget(QWidget *parent = 0);
    ~PlotWidget();

private slots:

signals:
   void MovePos(QPoint);
   void SendDateMove(int);
private:

private:
    Ui::PlotWidget *ui;

};

#endif // PLOTWIDGET_H
