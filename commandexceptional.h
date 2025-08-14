#ifndef COMMANDEXCEPTIONAL_H
#define COMMANDEXCEPTIONAL_H

#include <QWidget>

namespace Ui {
class CommandExceptional;
}


class QMouseEvent;
class QCloseEvent;
class QKeyEvent;

class CommandExceptional : public QWidget
{
    Q_OBJECT

public:
    explicit CommandExceptional(char index, QString InfoRemid, QWidget *parent = 0);
    ~CommandExceptional();
    void setErrInfo(const quint8, const QString);
    void replaceIndexSupplies(const quint8 index,const QString Info); //更换那种耗材试管
    void replaceReagents(const quint8 indexReag,const QString Info);  //更换那种试剂
protected:
    //拖拽窗口
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
private slots:
    void on_toolButton_ok_clicked();
    void on_toolButton_close_clicked();
    void on_pushButton_KS600_clicked();

    void on_pushButton_KS800_clicked();

    void on_pushButton_KS1200_clicked();

    void on_toolButton_Cancel_clicked();

signals:
    void ReplaceSend(quint8); /*错误提示*/
    void closeDel();
    void configEquipType(quint8);
    void clossuoweiApp();
    void replaceIndexSulipp(quint8 );
    void updateRagentsKinds(quint8);
private:
    Ui::CommandExceptional *ui;
    quint8 m_indexErr = 0;
    QColor bgmcColor;
    QPoint mousePos;
    QPoint windowsPos;
    QPoint dPos;
    char mFucn;
    quint8  mindexSulip = 0;
    quint8  m_indexReag = 0;
};

#endif // COMMANDEXCEPTIONAL_H
