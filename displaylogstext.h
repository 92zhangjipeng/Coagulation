#ifndef DISPLAYLOGSTEXT_H
#define DISPLAYLOGSTEXT_H

#include <QMouseEvent>
#include <QWidget>

namespace Ui {
class displayLogsText;
}

class displayLogsText : public QWidget
{
    Q_OBJECT

public:
    explicit displayLogsText(QWidget *parent = 0);
    ~displayLogsText();
protected:
    void mousePressEvent(QMouseEvent *event); //拖拽窗口
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);

private slots:


private:
    void _initStyle();

    void _loadLogFile(); //载入日志文件

    void _appendtextshow(const QString);

signals:

private:
    Ui::displayLogsText *ui;
    QPoint mouseStartPoint;
    QColor bgmcColor;
    bool _mminshow;
};

#endif // DISPLAYLOGSTEXT_H
