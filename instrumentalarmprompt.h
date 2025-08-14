#ifndef INSTRUMENTALARMPROMPT_H
#define INSTRUMENTALARMPROMPT_H

#include <QWidget>

namespace Ui {
class instrumentAlarmPrompt;
}

class instrumentAlarmPrompt : public QWidget
{
    Q_OBJECT

public:
    explicit instrumentAlarmPrompt(QWidget *parent = 0,int alarmIndex = 0,const QString alarmText = "");
    ~instrumentAlarmPrompt();

    void _configalarmindex(int index_);
    void _configalarmText(QString outtext_);
protected:
    //拖拽窗口
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);
private slots:
    void on_toolButton_closeReminder_clicked();
    void on_toolButton_close_clicked();
signals:
   void outSideCleanDepleteOne();
private:
    Ui::instrumentAlarmPrompt *ui;
    QColor bgmcColor;
    QPoint mouseStartPoint;
    int malarmIndex = -1;
};

#endif // INSTRUMENTALARMPROMPT_H
