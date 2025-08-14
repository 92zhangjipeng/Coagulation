#ifndef CONTROLTHEMODULETEMP_H
#define CONTROLTHEMODULETEMP_H

#include <QMouseEvent>
#include <QTimer>
#include <QWidget>

namespace Ui {
class ControltheModuletemp;
}

class ControltheModuletemp : public QWidget
{
    Q_OBJECT

public:
    explicit ControltheModuletemp(QWidget *parent = 0);
    ~ControltheModuletemp();

    void settipstext(const QString, const QString);


private slots:
    void on_toolButton_close_clicked();
    void on_toolButtonok_clicked();
    void on_toolButtonexit_clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent* event) override;
signals:
    void re_dimming(); //重新调光
    void disableusechn(); //直接禁用通道
    void deletedimming(QString);
private:
    Ui::ControltheModuletemp *ui;
    QColor bgmcColor;
    QPoint mouseStartPoint;
    QString m_dimmingText;

};

#endif // CONTROLTHEMODULETEMP_H
