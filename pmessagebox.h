#ifndef PMESSAGEBOX_H
#define PMESSAGEBOX_H

#include "qsimpleled.h"

#include <QObject>
#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QMap>


typedef struct TubeInfo{
   QPoint Posloc;
   bool   UsedTube;     //使用状态
   quint8 bIgRadius;    //大半径
   quint8 SmallRadius;  //小半径
}TubeInfo;

namespace Ui {
class PMessageBox;
}


class PMessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit PMessageBox(QWidget *parent = nullptr, int indexTray = 0);
    ~PMessageBox();

    void setindexTray(int index_);

    bool isPartiallyVisible(QDialog* pwidget);
protected:
    //拖拽窗口
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
    //void closeEvent(QCloseEvent *event);

private slots:
    void on_toolButton_OK_clicked();
    void on_toolButton_Cancel_clicked();

signals:
    void _makesureupdateTraytube(int index);  //确定试管盘脱机

    void _cancelupdateTraytube(int index); //取消、忽略试管盘脱机

private:
    void testTubeTrayLastNum(const int alarmIndex);
    void creatTrayPointLoc(QWidget *widgetRect, QList<int> &notUsedTubeList, const int indexhole);

private:
    Ui::PMessageBox *ui;
    QSimpleLed *m_pdelegatesTube = nullptr;
    QColor bgmcColor;
    QPoint mouseStartPoint;
    int mIndextubeTray = 0;
    int mTimeShowSameone;
    bool mshowtube = false;
    QMap<int,TubeInfo*> mTubePosVec;
    QList<int> mnotuseTube; //未使用的试管号
    bool mbtubeNothing = false; //是否存在未用试管
    bool mbreminderChangeNew = false;//全使用完更换新的
};

#endif // PMESSAGEBOX_H
