#ifndef FLOORPLANOFINTERFACE_H
#define FLOORPLANOFINTERFACE_H

#include <QtCore/qglobal.h>
#include <QObject>
#include <QPoint>
#include <QCloseEvent>

//#include "floorplanofinterface_global.h"

#if defined(FLOORPLANOFINTERFACE_LIBRARY)
#  define FLOORPLANOFINTERFACESHARED_EXPORT Q_DECL_EXPORT
#else
#  define FLOORPLANOFINTERFACESHARED_EXPORT Q_DECL_IMPORT
#endif

class FLOORPLANOFINTERFACESHARED_EXPORT FloorPlanofInterface : public QObject
{
    Q_OBJECT
public:
    FloorPlanofInterface();
    ~FloorPlanofInterface();
public:
    void showInterface();
    void closeEvent(QCloseEvent *event);
signals:
    void RetuenPoint(int Areas, int IndexTube,int IndexZ,int DownMm,int offsetX ,int offsetY);
    void CompleteAdjust();
    void ConfigureOffset(int,int,int,int);
    void Machinereposition(); //机器复位

private slots:
    void MoveTestChannel(int,int Index,int ,int );
    void SLOTSaveCleanOriginoffset(int,int,int,int);
    void CloseOffsetUiSave();
    void signalsreposition();

public slots:
   void SlotGetOffset(QVector<QPoint> , QVector<QPoint>, QVector<QPoint>, QPoint origin);

   void SlotGetMovedPoint(QPoint pos);
private:


};

#endif // FLOORPLANOFINTERFACE_H

//FLOORPLANOFINTERFACESHARED_EXPORT  void show();
