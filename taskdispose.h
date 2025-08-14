#ifndef TASKDISPOSE_H
#define TASKDISPOSE_H

#include "taskdispose_global.h"
#include <QString>
#include <QMap>
#include <QDebug>
#include <QPointF>


#define  PROJECT_1  "APD"
#define  PROJECT_2  "AA"
#define  PROJECT_3  "COL"
#define  PROJECT_4  "R"

typedef struct DLLFrom_BloodTube_To_TestTube
{
    char *Sample_num;  //样本号

    unsigned char  poorblood_hole; //贫血孔号

    unsigned char  richblood_hole; //富血孔号

    unsigned char  reagent_num;   //试剂号

    double test_height;  //测高值

    bool   bdistribution; //分配任务

}DLLFrom_BloodTube_To_TestTube_t;

typedef QVector<DLLFrom_BloodTube_To_TestTube_t*> DLLFrom_BloodTube_To_TestTubeVec;


typedef struct DLLMind_Move_Struct
{
    char *Sample_num;

    unsigned  char blood_hole;      //血样孔

    unsigned  int Testtray_hole;    //试管孔

    unsigned char  reagent_num;     //试剂号

    double test_height;             //测高值

    bool b_clearing;

}DLLMind_Move_Struct_t;

typedef QVector<DLLMind_Move_Struct_t*> DLLMind_Move_Struct_tVec;


extern "C" Q_DECL_EXPORT void __GetItemTask(QList<QString> &hadTask,
                                            QMap<QString,QString> TaskMap
                                                , DLLFrom_BloodTube_To_TestTubeVec * scan_reply);




extern "C" Q_DECL_EXPORT void __TaskConfigure(QMap<quint16, QPointF> m_Test_Tray_has_hole,
                                                  QList<quint16> &Select_tubehole,
                                                  DLLFrom_BloodTube_To_TestTubeVec  scan_reply
                                                 , DLLMind_Move_Struct_tVec * Task);




#endif // TASKDISPOSE_H
