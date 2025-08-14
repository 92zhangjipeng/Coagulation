#include "task_scheduling.h"
#include "custom_struct.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

Task_Scheduling::Task_Scheduling(QObject *parent) : QObject(parent)
{
    for(int i = 0 ;i < 360; i++)
        m_test_tube_Area.insert(i,true);//编号,贫富血
    m_data_item_task.clear();
    m_Tube_already_useed.clear();
}

void Task_Scheduling::parse_Project_use(int Project)
{
    m_Project_test.clear();
	/*16进制转10进制--case值*/
    bool bProject_use_test[ALL_TEST_PROJECT_TOTAL]{};
	memset(bProject_use_test, false, sizeof(bProject_use_test));
	switch (Project)
	{
		case 1:
			bProject_use_test[0] = true;
			break;
		case 2:
			bProject_use_test[1] = true;
			break;
		case 4:
			bProject_use_test[2] = true;
			break;
		case 8:
			bProject_use_test[3] = true;
			break;
		case 3:
			bProject_use_test[0] = true;
			bProject_use_test[1] = true;
			break;
		case 5:
			bProject_use_test[0] = true;
			bProject_use_test[2] = true;
			break;
		case 9:
			bProject_use_test[0] = true;
			bProject_use_test[3] = true;
			break;
		case 6:
			bProject_use_test[1] = true;
			bProject_use_test[2] = true;
			break;
		case 16:
			bProject_use_test[1] = true;
			bProject_use_test[3] = true;
			break;
		case 18:
			bProject_use_test[2] = true;
			bProject_use_test[3] = true;
			break;
		case 7:
			bProject_use_test[0] = true;
			bProject_use_test[1] = true;
			bProject_use_test[2] = true;
			break;
		case 17:
			bProject_use_test[0] = true;
			bProject_use_test[1] = true;
			bProject_use_test[3] = true;
			break;
		case 20:
			bProject_use_test[1] = true;
			bProject_use_test[2] = true;
			bProject_use_test[3] = true;
			break;
		case 21:
			bProject_use_test[0] = true;
			bProject_use_test[1] = true;
			bProject_use_test[2] = true;
			bProject_use_test[3] = true;
			break;
		default:
			bProject_use_test[0] = true;
			bProject_use_test[1] = true;
			bProject_use_test[2] = true;
			bProject_use_test[3] = true;
		break;
	}
	for (int i = 0; i < ALL_TEST_PROJECT_TOTAL; i++)
	{
        if(bProject_use_test[i])
            m_Project_test.insert(i,true);
	}
    return;
}

void Task_Scheduling::Get_Start_Test_parameter(QVariant get_data)
{
  
   return;
}
/*贫富同测*/
void Task_Scheduling::Blood_Poor_and_Rich_together_test(QList<int> select_hole_num
                                                            ,QMap<int, int> &get_poor_blood_pos
                                                            ,QMap<int, QList<int>> &get_rich_blood_pos
                                                            ,QStringList sample
                                                            ,QMap<int,int> &out_show_text_Poor
                                                            ,QMap<int,int> &out_show_text_Rich)
{

    QList<int> sample_int;
    for(int i = 0 ; i < sample.size();i++){
        QString num_ = sample.at(i);
        QString temp = num_.split("-").at(1);
        sample_int.append(temp.toInt());
    }
    QMap<int ,int> show_text_poor;
    show_text_poor.clear();
    QMap<int ,int> show_text_rich;
    show_text_rich.clear();
    QMap<int,int> show_text_poor_out;
    show_text_poor_out.clear();
    QMap<int,int> show_text_rich_out;
    show_text_rich_out.clear();
    int num_sample_poor = 0;
    int num_sample_rich = 0;

    QMap<int,bool>::iterator iter;
    QList<int> blood_hole_poor;
    blood_hole_poor.clear();
    QList<int> blood_hole_rich;
    blood_hole_rich.clear();
    QMap<int, int> poor_blood_pos;        //贫血： key:血样区得孔号  value:试管区得位置
    QMap<int, QList<int>> rich_blood_pos; //富血：key:血样区得孔号  value:试管区得位置list
    int count_blood = select_hole_num.size()/2; //贫富同测 2个血样看成一个整体
    int project_num = m_Project_test.size(); //项目数
    //需要试管总数
    int need_tube = (1+project_num) *count_blood;
    bool enough = enough_tube_test(need_tube);
    if(enough == false)
    {
        QMessageBox::about(NULL, "测试失败", "剩余试管不够本次测试,请更换试管盘!");
        return;
    }
    for(int i = 0 ; i < count_blood; i++ )
    {
        blood_hole_poor.append(select_hole_num.at(i*2));
        show_text_poor.insert(select_hole_num.at(i*2),sample_int.at(i));
        blood_hole_rich.append(select_hole_num.at(i*2+1));
        show_text_rich.insert(select_hole_num.at(i*2+1),sample_int.at(i));
    }
    poor_blood_pos.clear();
    rich_blood_pos.clear();
    iter = m_test_tube_Area.begin();//试管区得起始位置
    int tube_start = iter.key(); //试管区得起始位置--孔号
    for(int k = 0 ; k < blood_hole_poor.size();k++)
    {
        /*贫血的孔号对应*/
        int hole_poor = blood_hole_poor.at(k);
        if(show_text_poor.contains(hole_poor))
        {
            QMap<int,int>::iterator temp_it = show_text_poor.find(hole_poor);
            num_sample_poor = temp_it.value();
        }
        int tube_hole_poor = tube_start +(1 + project_num) *k; //贫血在试管A 得位置
        m_Tube_already_useed.append(tube_hole_poor);
        poor_blood_pos.insert(hole_poor,tube_hole_poor); //贫血得孔位号 --试管区放贫血得孔位号
        show_text_poor_out.insert(tube_hole_poor,num_sample_poor);
        /*富血的孔号对应*/
        QList<int> list_rich_tube;
        list_rich_tube.clear();
        int hole_rich = blood_hole_rich.at(k);
        for(int j = 1 ;j <= project_num; j++)
        {

            if(show_text_rich.contains(hole_rich))
            {
                QMap<int,int>::iterator temp_it = show_text_rich.find(hole_rich);
                num_sample_rich = temp_it.value();
            }
            int tube_hole_rich = tube_hole_poor + j;
            m_Tube_already_useed.append(tube_hole_rich);
            list_rich_tube.append(tube_hole_rich);
            show_text_rich_out.insert(tube_hole_rich,num_sample_rich);
        }
        rich_blood_pos.insert(hole_rich,list_rich_tube);
    }
    get_poor_blood_pos = poor_blood_pos;
    get_rich_blood_pos = rich_blood_pos;
    out_show_text_Poor = show_text_poor_out;
    out_show_text_Rich = show_text_rich_out;
    return;
}
/*仅测富*/
void Task_Scheduling::Blood_Rich_test_only(QList<int> select_hole_num,
                                            QMap<int, QList<int>> &get_rich_blood_pos,
                                            QStringList smaple,
                                            QMap<int,int> &know_tube)
{
    QList<int> sample_int;
    for(int i =0 ; i <smaple.size();i++){
        QString num_ = smaple.at(i);
        QString temp = num_.split("-").at(1);
        sample_int.append(temp.toInt());
    }
    int need_tube;
    QMap<int,int> xueyangyangben; //血样样本号
    xueyangyangben.clear();
    QMap<int,int> show_tube_num;
    show_tube_num.clear();

    QMap<int,bool>::iterator iter;
    QList<int> blood_hole_rich;
    blood_hole_rich.clear();
    QMap<int, QList<int>> rich_blood_pos; //富血：key:血样区得孔号  value:试管区得位置list

    int count_blood = select_hole_num.size();
    int project_num = m_Project_test.size(); //项目数
    need_tube = project_num * count_blood; //需要得试管数
    bool enough = enough_tube_test(need_tube);
    if(enough == false)
    {
        QMessageBox::about(NULL, "测试失败", "剩余试管不够本次测试,请更换试管盘!");
        return;
    }
    for(int i = 0 ; i < count_blood ;i++)
    {
        blood_hole_rich.append(select_hole_num.at(i));
        xueyangyangben.insert(select_hole_num.at(i),sample_int.at(i));
    }
    rich_blood_pos.clear();
    iter = m_test_tube_Area.begin();
    int tube_start = iter.key();
    for(int k = 0 ; k < blood_hole_rich.size();k++)
    {
        /*富血的孔号对应*/
        QList<int> list_rich_tube;
        list_rich_tube.clear();
        int hole_rich = blood_hole_rich.at(k); //富血在血样区得孔号
        for(int j = 0 ;j < project_num; j++)
        {
            int num_sample;
            if(xueyangyangben.contains(hole_rich))
            {
               QMap<int,int>::iterator temp_it = xueyangyangben.find(hole_rich);
               num_sample = temp_it.value();
            }
            int tube_hole_rich = tube_start + j;
            m_Tube_already_useed.append(tube_hole_rich);
            list_rich_tube.append(tube_hole_rich);
            show_tube_num.insert(tube_hole_rich,num_sample);
        }
        /*删除掉已经调度过的孔*/
        for (int i = 0; i <m_Tube_already_useed.size(); i++)
        {
            int hole = m_Tube_already_useed.at(i);//试管区已经分配有样本得试管 总数
            if(m_test_tube_Area.contains(hole))
                m_test_tube_Area.remove(hole);
        }
        iter = m_test_tube_Area.begin();
        tube_start = iter.key();
        rich_blood_pos.insert(hole_rich,list_rich_tube);
    }
    get_rich_blood_pos = rich_blood_pos;
    know_tube = show_tube_num;
    return;
}
bool Task_Scheduling::enough_tube_test(int need_tube)
{
    int remainder_tube = m_test_tube_Area.size();
    if(remainder_tube >= need_tube )
        return true;
    else
       return false;
    return true;
}
void Task_Scheduling::Sport_Machine(QList<int> select_hole_num, QStringList smaple_num)
{
    /*贫富同测输出的样本号*/
    QMap<int,int> out_poor_text;
    out_poor_text.clear();
    QMap<int,int> out_rich_text;
    out_rich_text.clear();

	QMap<int, int> poor_blood_pos;//贫血： key:血样区得孔号  value:试管区得位置
	QMap<int, QList<int>> rich_blood_pos; //富血：key:血样区得孔号  value:试管区得位置list
	if (m_Poor_rich_test == 0) //贫富同测 --那血样摆放就是 贫富-贫富-贫富轮询
	{
        poor_blood_pos.clear();
        rich_blood_pos.clear();
        Blood_Poor_and_Rich_together_test(select_hole_num,
                                          poor_blood_pos,
                                          rich_blood_pos,
                                          smaple_num,
                                          out_poor_text,
                                          out_rich_text);
        /*删除掉已经调度过的孔*/
        for(int i = 0 ; i <m_Tube_already_useed.size();i++)
        {
            int hole = m_Tube_already_useed.at(i);//试管区已经分配有样本得试管总数
            if(m_test_tube_Area.contains(hole))
                m_test_tube_Area.remove(hole);
        } 

	}
	else if (m_Poor_rich_test == 1) //仅测富
	{
        rich_blood_pos.clear();
        Blood_Rich_test_only(select_hole_num,rich_blood_pos, smaple_num, out_rich_text);
	}
	/*试管区绑定要加得试剂种类*/
	QMap<int, int> rich_blood_add_witch_reagent;
	rich_blood_add_witch_reagent.clear();
	QMap<int, bool>::iterator it_reagent;
	QMap<int, QList<int>>::iterator it_rich;
	for (it_rich = rich_blood_pos.begin(); it_rich != rich_blood_pos.end(); it_rich++)
	{
		QList<int> number_hole = it_rich.value();
		int size_length = number_hole.size();
		it_reagent = m_Project_test.begin();
		for (int i = 0; i< size_length; i++)
		{
			int Pos = number_hole.at(i);
			int value = it_reagent.key();
			rich_blood_add_witch_reagent.insert(Pos, value);
			it_reagent++;
		}
	}
	/*组结构数据发送到串口处理*/
	qRegisterMetaType<From_blood_Move_Tube>("From_blood_Move_Tube");
	QVariant data;
	From_blood_Move_Tube move_readyTest;
	move_readyTest.Poor_Blood = poor_blood_pos;
	move_readyTest.Rich_Blood = rich_blood_pos;
	move_readyTest.Rich_blood_add_witch_reagent = rich_blood_add_witch_reagent;
	move_readyTest.Sample_num = smaple_num;
    move_readyTest.Project_num =  m_Project_test.size();
	data.setValue(move_readyTest);
	emit Send_tube_task_move(data);
    ///*试管区要用得变颜色*/
    //emit Will_Tell_Tube_Use(m_Tube_already_useed ,out_poor_text,out_rich_text);
    //emit Will_Tell_Tube_Use_1(m_Tube_already_useed,out_poor_text,out_rich_text);
    //emit Will_Tell_Tube_Use_2(m_Tube_already_useed,out_poor_text,out_rich_text);


    Poor_and_Rich_together_test(poor_blood_pos,rich_blood_pos,rich_blood_add_witch_reagent,smaple_num);
	return;
}

void Task_Scheduling::Poor_and_Rich_together_test( QMap<int, int> poor_blood_pos,
                                                     QMap<int, QList<int>> rich_blood_pos,
                                                     QMap<int, int> rich_blood_add_witch_reagent,
                                                     QStringList smaple_num)
{
    QList<int> sample_patient;
    sample_patient.clear();
    for(int i =0 ; i <smaple_num.size();i++){
        QString num_ = smaple_num.at(i);
        QString temp = num_.split("-").at(1);
        sample_patient.append(temp.toInt());
    }
   QVariant sinagl_task;
   qRegisterMetaType<Sinagl_data>("Sinagl_data");
   QMap<int,int>::iterator iter_poor;
   QMap<int,QList<int> >::iterator iter_rich;
   int count_patient = sample_patient.size();
   QList<int> temp;
   iter_poor = poor_blood_pos.begin();
   iter_rich = rich_blood_pos.begin();
   for(int i = 0 ; i < count_patient ; i++)
   {
       sinagl_task.clear();
       Sinagl_data Tube_move_test;
       Tube_move_test.Sample_num = sample_patient.at(i);
       Tube_move_test.Poor_hole_tube = iter_poor.value();
       iter_poor++;
       temp.clear();
       temp = iter_rich.value();
       for(int j =0 ;j<temp.size();j++)
       {
           int tube_hole = temp.at(j);
           if(rich_blood_add_witch_reagent.contains(tube_hole))
           {
               QMap<int, int>::iterator temp_it = rich_blood_add_witch_reagent.find(tube_hole);
               int reagent = temp_it.value();
               Tube_move_test.Poor_hole_tube_reagent.insert(tube_hole,reagent);
           }
       }
       sinagl_task.setValue(Tube_move_test);
       m_task_adjust.append(sinagl_task);
	   iter_rich++;
   }
   return;
}


