#pragma execution_character_set("utf-8")

#include "mythreadaddsample.h"

#include <customcreatsql.h>

#include <custom_struct.h>

#include <QUIUtils.h>

#include <mainwindow.h>

#include <globaldata.h>
#include <loadequipmentpos.h>

#include <operclass/fullyautomatedplatelets.h>

mythreadaddsample::mythreadaddsample(QObject *parent) : QObject(parent)
{
    moveToThread(&m_thread);

    QObject::connect(&m_thread,&QThread::started,this,&mythreadaddsample::Tmain);

    m_totalnum = 0;
}

mythreadaddsample::~mythreadaddsample()
{
    if(m_thread.isRunning()){
        m_thread.quit();
        m_thread.wait();
    }
}

void mythreadaddsample::Tmain()
{
    m_WaitTestStuList.clear();
    QLOG_DEBUG()<<"mythreadaddsample开始线程"<<QThread::currentThreadId();
}

void mythreadaddsample::Start()
{
    if(!m_thread.isRunning())
    {
        m_thread.start();
    }

}

void  mythreadaddsample::_waittestsampledata(QString samplename, QString savedtime,
                                               QString barcode_str,
                                               double bottomBloodHeight, QString project_,
                                               quint8 CurrRichHole, int _total,bool insertWholeBloodMode)
{
    m_totalnum = _total;
    WAIT_TEST_SAMPLE_DATA *waitTestSapleInfoStu = new WAIT_TEST_SAMPLE_DATA;
    waitTestSapleInfoStu->_addtime      = savedtime;
    waitTestSapleInfoStu->_barcode      = barcode_str;
    waitTestSapleInfoStu->_CurrRichHole = CurrRichHole;
    waitTestSapleInfoStu->_sampleid     = samplename;
    if(insertWholeBloodMode)
    {
       double totalHeigh = INI_File().GetFixedHigh() + REFERENCE_TO_BOTTOM;
	   double offsetmm = INI_File().GetTestDifference();
       double PindownMM = std::abs(totalHeigh - bottomBloodHeight - offsetmm);
       waitTestSapleInfoStu->_testheighvalue = PindownMM;
    }
    else
    {
         waitTestSapleInfoStu->_testheighvalue = bottomBloodHeight;
    }
    waitTestSapleInfoStu->_testproject = project_;
    m_WaitTestStuList.append(waitTestSapleInfoStu);

    //同步到患者界面+样本信息+曲线数据数据库
    emit _sycnPaintentInfo(samplename,savedtime,barcode_str,project_);

    QLOG_DEBUG()<<"添加任务数"<<m_totalnum<<"结构体存储数"<<m_WaitTestStuList.size();

    if(m_totalnum == m_WaitTestStuList.size())
    {
        SycnAddTaskTestHoleAndCommder(m_totalnum);
    }
    return;
}


void mythreadaddsample::SycnAddTaskTestHoleAndCommder(int total_)
{
	//先获取SQL 试管状态
    QString sampledate;
	QMap<quint8, bool > SQLTubeState;
	SQLTubeState = FullyAutomatedPlatelets::pinstancesqlData()->BackEmptyTubeNumMap();

    QList<EmptyTestTubeInfo* > _UpdateTubeState;
    _UpdateTubeState.clear();

    int index = 1,sampleId = 0 ;


	auto iter = m_WaitTestStuList.begin();
	while (iter != m_WaitTestStuList.end())
	{
		DATASAMPLESTRUCT *pSampleBasicInfo = new DATASAMPLESTRUCT;
		WAIT_TEST_SAMPLE_DATA *data_stu = *iter;
		QString SampleNum = data_stu->_sampleid;

        GlobalData::apartSampleId(SampleNum,sampledate,sampleId);

		pSampleBasicInfo->samplestr = SampleNum;
		pSampleBasicInfo->bstartTesting = false;
		pSampleBasicInfo->sample_num = sampleId;
		pSampleBasicInfo->bemergency = false;
		pSampleBasicInfo->_bthisSampleaddData = false;
		pSampleBasicInfo->indexTestingChn = -1;
		pSampleBasicInfo->fcus_in = false;
		pSampleBasicInfo->initAnemiaValue = 0;
		pSampleBasicInfo->index_fcus_Active = FOCUS_ADDING_SAMPLE; //每个样本第一步总是吸样
		pSampleBasicInfo->CompleteAspirationState = NOT_ADD_SAMPLE;
		pSampleBasicInfo->anemiaoffHandsAxis.setX(0);     //贫血抓手坐标
		pSampleBasicInfo->anemiaoffHandsAxis.setY(0);
		pSampleBasicInfo->bcleanbloody_state = false;     //清洗血样针状态
		pSampleBasicInfo->bcleanDoublePin_state = false;  //清洗双针状态

		pSampleBasicInfo->tadd_sample_active.clear();         //加样动作

		pSampleBasicInfo->t_catchAnemiaTest.clear();          //抓贫血测试
        pSampleBasicInfo->suctionPPPreset = 0;
        pSampleBasicInfo->spitPPPreset = 0;

		pSampleBasicInfo->t_throw_anemiacups_active.clear();  //丢杯动作
        pSampleBasicInfo->throwPPPsuckBacknum = 0;
        pSampleBasicInfo->throwPPPsplitBacknum = 0;

		pSampleBasicInfo->ptestReagent_active.clear();

        pSampleBasicInfo->bhandsErr = false;
        pSampleBasicInfo->bgivesample = false;
        pSampleBasicInfo->bPendingtimeSample = false;

        pSampleBasicInfo->prePatchActions.clear();


		//确定吸样的试管
		QString out_reag_name;
		quint8 suckbloody_hole = data_stu->_CurrRichHole;
		QList<quint8> testbloody_project;
		quint8 suckAenmia_hole = data_stu->_CurrRichHole - 1;
		quint8 anemia_suck_to_hole = 0;
		quint8 bloody_suck_tohole = 0;
		QList<quint8> mark_tube;
		//inquire_test_free_tube_status(anemia_suck_to_hole);


        out_reag_name = GlobalData::mapIndexReagentnames(ANEMIA);
        //遍历返回空试管个数 获取第一个空试管孔号
        anemia_suck_to_hole = BackPutOutMinHoleEmptyHole(SQLTubeState);

        EmptyTestTubeInfo *pAnemiaTestTubeState  = new EmptyTestTubeInfo;
        pAnemiaTestTubeState->IdSample = sampleId;
        pAnemiaTestTubeState->ReagentNanme = out_reag_name;
        pAnemiaTestTubeState->UpdateHole = anemia_suck_to_hole;
        pAnemiaTestTubeState->TubeState = TESTTUBES_MARK;
        _UpdateTubeState.append(pAnemiaTestTubeState);


		//吸贫血在血样区孔号+坐标
		pSampleBasicInfo->suckanaemia_hole = suckAenmia_hole;
		QPoint AnaemiAaxis(0, 0);
		SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT, suckAenmia_hole, AnaemiAaxis);
		pSampleBasicInfo->suckanaemia_axis = AnaemiAaxis;

		//吐贫血在试杯区+坐标
		pSampleBasicInfo->spitanaemia_hole = anemia_suck_to_hole;
		mark_tube.push_back(anemia_suck_to_hole);
		QPoint AnaemiinEmptyAaxis(0, 0);
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, anemia_suck_to_hole, MOTOR_BLOOD_INDEX, AnaemiinEmptyAaxis);
		pSampleBasicInfo->spitanaemia_axis = AnaemiinEmptyAaxis;

		//吸富血在血样区孔号+坐标
		pSampleBasicInfo->suckbloody_hole = suckbloody_hole;
		QPoint suckbloodyAxis(0, 0);
		SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT, suckbloody_hole, suckbloodyAxis);
		pSampleBasicInfo->suckbloody_axis = suckbloodyAxis;

		//吐富血在试杯区的孔号+坐标
		QString richHole_ = "";
        GlobalData::sycnTestingReagnetNum(testbloody_project, data_stu->_testproject);
		int total_test_project = testbloody_project.size();
		QPoint spit_bloody_axis(0, 0);
		QList<QPoint> spit_bloody_poslist;
		foreach(quint8 index_, testbloody_project)
		{
			BLOODY_INFO Bloodydata;
            out_reag_name = GlobalData::mapIndexReagentnames(index_);
            bloody_suck_tohole = BackPutOutMinHoleEmptyHole(SQLTubeState);

            EmptyTestTubeInfo *pRichBloodTestTubeState  = new EmptyTestTubeInfo;
            pRichBloodTestTubeState->IdSample = sampleId;
            pRichBloodTestTubeState->ReagentNanme = out_reag_name;
            pRichBloodTestTubeState->UpdateHole = bloody_suck_tohole;
            pRichBloodTestTubeState->TubeState = TESTTUBES_MARK;
            _UpdateTubeState.append(pRichBloodTestTubeState);
           
			Bloodydata.bloody_hole = bloody_suck_tohole; //富血孔号
			mark_tube.push_back(bloody_suck_tohole);
			Bloodydata.index_reagent = index_;//测试试剂
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, bloody_suck_tohole, MOTOR_BLOOD_INDEX, spit_bloody_axis);
			Bloodydata.bloody_axis = spit_bloody_axis; //吐富血坐标
			spit_bloody_poslist.push_back(spit_bloody_axis);
            QLOG_DEBUG() << "富血孔" << bloody_suck_tohole << GlobalData::mapIndexReagentnames(index_)
                         << "吐富血坐标" << spit_bloody_axis;

			richHole_ = QString("%1|%2").arg(richHole_).arg(bloody_suck_tohole);
			pSampleBasicInfo->bloody_tube.push_back(Bloodydata);
		}

		//加样动作
		QByteArrayList out_directives_anemia;
        QUIUtils::suckPPPEndSplitPPP(out_directives_anemia, AnaemiAaxis, AnaemiinEmptyAaxis);
        QUIUtils::SuckPRPandSpitoutPRP(out_directives_anemia, data_stu->_testheighvalue, suckbloodyAxis, spit_bloody_poslist);
		StructInstance::getInstance()->creataddSampleCommands(out_directives_anemia, pSampleBasicInfo);

		//清洗血样针
		QByteArrayList cleanbloodpin;
		QUIUtils::CleanBloodPinActionCommd(total_test_project, cleanbloodpin);
		StructInstance::getInstance()->creat_cleanBloodyPin(false, cleanbloodpin, pSampleBasicInfo);

		//清洗双针
        QByteArrayList _rinseddoublepin;
        _rinseddoublepin.clear();
        QUIUtils::cleaningDoubleNeedleAction(_rinseddoublepin, total_test_project);
		StructInstance::getInstance()->creat_cleanBloodyPin(true, _rinseddoublepin, pSampleBasicInfo);
        StructInstance::getInstance()->addSamplebloodInfo(pSampleBasicInfo);


		//插入到数据表--待测任务(下针高度,贫富血孔号信息)
        insert_waittest_into_sql(data_stu->_sampleid,
                                 suckAenmia_hole,
                                 suckbloody_hole,
                                 data_stu->_testheighvalue,
                                 anemia_suck_to_hole,
                                 richHole_,
                                 data_stu->_testproject,
                                 data_stu->_barcode);

        emit _addprogress(index, total_);
        emit _updatetestui(mark_tube,
                           data_stu->_sampleid,
                           suckAenmia_hole,
                           index,
                           total_);

		index++;
		iter++;
	}

    for(auto it = m_WaitTestStuList.begin(); it != m_WaitTestStuList.end(); ++it)
	{
		if (*it != nullptr) {
			delete *it;
			*it = nullptr;
		}
	}
	m_WaitTestStuList.clear();

    auto iterHole =   _UpdateTubeState.begin();
    while(iterHole != _UpdateTubeState.end())
    {
        FullyAutomatedPlatelets::pinstancesqlData()->UpdateTestTubeStateInfo((*iterHole)->UpdateHole,
                                                                             (*iterHole)->TubeState,
                                                                             (*iterHole)->ReagentNanme,
                                                                             (*iterHole)->IdSample);
		iterHole++;
    }
	qDeleteAll(_UpdateTubeState);
	return;
}



quint8 mythreadaddsample::BackPutOutMinHoleEmptyHole(QMap<quint8, bool >& EmptySQLTubeState)
{
	quint8 outputhole = 0;
	auto iter = EmptySQLTubeState.begin();
	while (iter != EmptySQLTubeState.end())
	{
		if (iter.value() == true){
			outputhole = iter.key();
			iter.value() = false;
			break;
		}
		iter++;
	}
	return outputhole;
}


void mythreadaddsample::insert_waittest_into_sql(QString sample_name,
                                                    quint8 suck_anemia,
                                                    quint8 suck_bloody,
                                                    int down_pin,
                                                    quint8 anemia_tube,
                                                    QString spitbloodhole,
                                                    QString index_reag,
                                                    QString barcode){
    QMap<quint8, QVariant> SingleTaskmap;
    SingleTaskmap.insert(COL_SAMPLE_NUMBER, sample_name);
    SingleTaskmap.insert(COL_POOR_HOLE,  suck_anemia);   // suck_anemia 对应 贫血孔
    SingleTaskmap.insert(COL_RICH_HOLE, suck_bloody);   // suck_bloody 对应 富血孔
    SingleTaskmap.insert(COL_BLOOD_HIGH, down_pin);
    SingleTaskmap.insert(COL_ANEMIA_TUBE, anemia_tube);
    SingleTaskmap.insert(COL_BLOODY_IN_TUBE, spitbloodhole);
    SingleTaskmap.insert(COL_DETECTION_PROJECT, index_reag);
    SingleTaskmap.insert(COL_BARCODE, barcode);
    if (!FullyAutomatedPlatelets::pinstancesqlData()->insert_task_to_sqltable(SingleTaskmap)) {
        QLOG_DEBUG() << "插入任务失败" << endl;
    }




//   QMap<quint8,QVariant> SingleTaskmap;
//   SingleTaskmap.clear();
//   QVariant  Sample_number = sample_name;
//   QVariant  Sample_RichHole = suck_anemia;
//   QVariant  Sample_PoorHole = suck_bloody;
//   QVariant  Sample_BloodHigh =  down_pin;
//   QVariant  bloodyintotube = spitbloodhole;
//   QVariant  Sample_Barcode  = barcode;
//   QVariant  Detection_Project = index_reag;
//   SingleTaskmap.insert(0,Sample_number);
//   SingleTaskmap.insert(1,Sample_PoorHole);
//   SingleTaskmap.insert(2,Sample_RichHole);
//   SingleTaskmap.insert(3,Sample_BloodHigh);
//   SingleTaskmap.insert(4,anemia_tube);
//   SingleTaskmap.insert(5,bloodyintotube);
//   SingleTaskmap.insert(6,Detection_Project);
//   SingleTaskmap.insert(7,Sample_Barcode);
//   FullyAutomatedPlatelets::pinstancesqlData()->insert_task_to_sqltable(SingleTaskmap);
   return ;
}





