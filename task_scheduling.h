#ifndef TASK_SCHEDULING_H
#define TASK_SCHEDULING_H

#include <QObject>
#include <QVariant>
#include "customcreatsql.h"
#include <QMessageBox>

#define ALL_TEST_PROJECT_TOTAL  4
class Task_Scheduling : public QObject
{
    Q_OBJECT
public:
    explicit Task_Scheduling(QObject *parent = nullptr);

signals:
    void Send_tube_task_move(QVariant);//血样到试管区得任务列表
    /*void Will_Tell_Tube_Use(QList<int>,QMap<int,int>,QMap<int,int>);
    void Will_Tell_Tube_Use_1(QList<int>,QMap<int,int>,QMap<int,int>);
    void Will_Tell_Tube_Use_2(QList<int>,QMap<int,int>,QMap<int,int>);*/
private slots:
    void Get_Start_Test_parameter(QVariant get_data);

public slots:
    void Blood_Poor_and_Rich_together_test(QList<int> select_hole_num,     //选中要测试的试管
                                              QMap<int, int> &get_poor_blood_pos,  //贫血分配的试管编号
                                              QMap<int, QList<int>> &get_rich_blood_pos, //分配任务对应的试管编号
                                              QStringList sample,  //样本号列表
                                              QMap<int,int> &out_show_text_Poor, //输出到试管界面的样本号
                                              QMap<int,int> &out_show_text_Rich);

    void Blood_Rich_test_only(QList<int> select_hole_num,
                                QMap<int, QList<int>> &get_rich_blood_pos,
                                QStringList smaple, QMap<int, int> &know_tube);

    /*从试管到测试位 && 在加入对应试剂*/
    void Poor_and_Rich_together_test(QMap<int, int> reagent_hole,
                                        QMap<int, QList<int> > rich_blood_pos,
                                        QMap<int, int> rich_blood_add_witch_reagent,
                                        QStringList smaple_num);

    void parse_Project_use(int Project);

private:
    void Sport_Machine(QList<int> select_hole_num, QStringList smaple_num);
    bool enough_tube_test(int);
private:
    QMap<int, bool> m_Project_test; //int 测试得项目号 0-3代表4个项目 bool 代表要测试
    int m_Poor_rich_test;
    /*试管区域*/
    QMap<int,bool> m_test_tube_Area;//试管区得试管数
    CustomCreatSql m_find_sql;//数据库
    QVariantList m_data_item_task;//任务列表
    QList<int>   m_Tube_already_useed;//试管区 已经用掉得了试管


    QVariantList m_task_adjust;
};

#endif // TASK_SCHEDULING_H
