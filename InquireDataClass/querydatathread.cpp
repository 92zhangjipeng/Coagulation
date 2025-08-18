#ifdef WIN32
#pragma execution_character_set("utf-8")
#endif

#include "querydatathread.h"
#include <operclass/fullyautomatedplatelets.h>
#include <globaldata.h>

QueryDataThread::QueryDataThread(QObject *parent) : QObject(parent)
{


}
QueryDataThread::~QueryDataThread()
{

}

void QueryDataThread::_startSycnData()
{

}

void QueryDataThread::InquierCurveViewEnd(const QString& findId)
{
    QMap<quint8,QString> CurvedataList;
    QVector<QString> curveData(5);

    if(auto sqlInstance = FullyAutomatedPlatelets::pinstancesqlData()){
        sqlInstance->getOneIDAllCurveData(findId, CurvedataList);
        for(auto it = CurvedataList.constBegin(); it != CurvedataList.constEnd(); ++it) {
            if (it.key() >= 1 && it.key() <= 5) { // 添加边界检查
                curveData[it.key() - 1] = it.value();
            }
        }

        emit sendCurveData(curveData); // 发送QVector而不是原始指针
    }
    return;
}



void QueryDataThread::slotFindModuleStyle(const quint8 indexStyles)
{
     emit this->clearTableWidget(); //先清空
     if(indexStyles == INQUIRE_TODAY_SQL)
     {
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString todayint = current_date_time.toString("yyyyMMdd");
        SycnDayDataResult(todayint); //每一天的结果
     }
     else if(INQUIRE_MONTH_SQL == indexStyles)
     {
        QDateTime current_date_time = QDateTime::currentDateTime();
        QString systemtime_month = current_date_time.toString("yyyyMM");
        SycnMonthDataResult(systemtime_month);
     }
     else if(INQUIRE_ALL_SQL == indexStyles)
     {
        SycnAllDataResult(); 
     }
     return;
}


void QueryDataThread::slotFindspecifiedData(QDateEdit *pdataStart,const int days)
{
    emit this->clearTableWidget(); //先清空
    QDateTime timeFrom = pdataStart->dateTime();
    QList<QString> findDaysList;
    findDaysList.clear();
    for (int i = 0; i <= days; i++)
    {
        QString need_days = timeFrom.addDays(i).toString("yyyyMMdd");
        findDaysList.push_back(need_days);
    }
    SycnDesignateDayDataResult(findDaysList); //同步查询指定的结果
    return;
}

void QueryDataThread::SycnDesignateDayDataResult(QList<QString> daysList)
{
    int numTotal;
    int n = 1;
    QVector<PatientInformationStu *> painterInfovec;
    painterInfovec.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->getDesignateResultData(daysList,painterInfovec);

    numTotal = painterInfovec.size();
    if(numTotal == 0){
        emit this->InquireEmpty();
        return;
    }
    auto iter = painterInfovec.constBegin();
    while(iter != painterInfovec.constEnd())
    {
        PatientInformationStu *pinquered = (*iter);
        InqueryDatastu_t* pFindedResult = new InqueryDatastu_t;
        insertStructData(pFindedResult, 0, pinquered->sampleId);
        insertStructData(pFindedResult, 1, pinquered->AAResult);
        insertStructData(pFindedResult, 2, pinquered->ADPResult);
        insertStructData(pFindedResult, 3, pinquered->EPIResult);
        insertStructData(pFindedResult, 4, pinquered->COLResult);
        insertStructData(pFindedResult, 5, pinquered->RISResult);
        insertStructData(pFindedResult,6,pinquered->sex_);
        insertStructData(pFindedResult,7,pinquered->department_);
        insertStructData(pFindedResult,8,pinquered->refertithedoctor_);
        insertStructData(pFindedResult,9,pinquered->bednumber_);
        insertStructData(pFindedResult,10,QString::number(pinquered->agesample));
        insertStructData(pFindedResult,11,pinquered->AddsampleTime);
        insertStructData(pFindedResult,12,pinquered->SampleName);
        insertStructData(pFindedResult,13,pinquered->barcodesample);
        emit this->LoadInquierdata(numTotal,n,pFindedResult);
        n++;
        iter++;
    }
    return;
}

void QueryDataThread::SycnDayDataResult(const QString Dateit)
{
	QVector<QSharedPointer<PatientInformationStu>> painterInfovec;
    FullyAutomatedPlatelets::pinstancesqlData()->getOneDayTestResultData(Dateit,painterInfovec);
    int numTotal = painterInfovec.size();
    if(numTotal == 0){
        emit InquireEmpty();
        return;
    }

	for (int n = 0; n < numTotal; ++n)
	{
		// 正确访问智能指针管理的对象
		const auto& patientInfo = painterInfovec[n];

		InqueryDatastu_t* pFindedResult = new InqueryDatastu_t;

		// 直接通过智能指针访问成员
        insertStructData(pFindedResult, 0, patientInfo->sampleId);
		insertStructData(pFindedResult, 1, patientInfo->AAResult);
		insertStructData(pFindedResult, 2, patientInfo->ADPResult);
		insertStructData(pFindedResult, 3, patientInfo->EPIResult);
		insertStructData(pFindedResult, 4, patientInfo->COLResult);
		insertStructData(pFindedResult, 5, patientInfo->RISResult);
		insertStructData(pFindedResult, 6, patientInfo->sex_);
		insertStructData(pFindedResult, 7, patientInfo->department_);
		insertStructData(pFindedResult, 8, patientInfo->refertithedoctor_);
		insertStructData(pFindedResult, 9, patientInfo->bednumber_);
		insertStructData(pFindedResult, 10, QString::number(patientInfo->agesample));
		insertStructData(pFindedResult, 11, patientInfo->AddsampleTime);
        insertStructData(pFindedResult, 12, patientInfo->SampleName);
		insertStructData(pFindedResult, 13, patientInfo->barcodesample);

		// 发射信号（n+1是为了保持原计数方式）
		emit LoadInquierdata(numTotal, n + 1, pFindedResult);
	}
}



void QueryDataThread::SycnMonthDataResult(const QString findDate)
{
    QVector<PatientInformationStu *> painterInfovec;
    painterInfovec.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->getThisMonthResultData(findDate,painterInfovec);
    int numTotal = painterInfovec.size();
    if(numTotal == 0){
        emit InquireEmpty();
        return;
    }
	int n = 1;
    auto iter = painterInfovec.constBegin();
    while(iter != painterInfovec.constEnd())
    {
        PatientInformationStu *pinquered = (*iter);
        InqueryDatastu_t* pFindedResult = new InqueryDatastu_t;
        insertStructData(pFindedResult, 0, pinquered->sampleId);
        insertStructData(pFindedResult, 1, pinquered->AAResult);
        insertStructData(pFindedResult, 2, pinquered->ADPResult);
        insertStructData(pFindedResult, 3, pinquered->EPIResult);
        insertStructData(pFindedResult, 4, pinquered->COLResult);
        insertStructData(pFindedResult, 5, pinquered->RISResult);
        insertStructData(pFindedResult,6,pinquered->sex_);
        insertStructData(pFindedResult,7,pinquered->department_);
        insertStructData(pFindedResult,8,pinquered->refertithedoctor_);
        insertStructData(pFindedResult,9,pinquered->bednumber_);
        insertStructData(pFindedResult,10,QString::number(pinquered->agesample));
        insertStructData(pFindedResult,11,pinquered->AddsampleTime);
        insertStructData(pFindedResult,12,pinquered->SampleName);
        insertStructData(pFindedResult,13,pinquered->barcodesample);
        emit this->LoadInquierdata(numTotal,n,pFindedResult);
        n++;
        iter++;
    }
    return;
}



void QueryDataThread::SycnAllDataResult()//查询全部
{
    int numTotal;
    int n = 1;
    QVector<PatientInformationStu *> painterInfovec;
    painterInfovec.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->getAllTestResultData(painterInfovec);
    numTotal = painterInfovec.size();
    if(numTotal == 0){
        emit this->InquireEmpty();
        return;
    }
    auto iter = painterInfovec.constBegin();
    while(iter != painterInfovec.constEnd())
    {
        PatientInformationStu *pinquered = (*iter);
        InqueryDatastu_t* pFindedResult = new InqueryDatastu_t;
        insertStructData(pFindedResult, 0, pinquered->sampleId);
        insertStructData(pFindedResult, 1, pinquered->AAResult);
        insertStructData(pFindedResult, 2, pinquered->ADPResult);
        insertStructData(pFindedResult, 3, pinquered->EPIResult);
        insertStructData(pFindedResult, 4, pinquered->COLResult);
        insertStructData(pFindedResult, 5, pinquered->RISResult);
        insertStructData(pFindedResult,6,pinquered->sex_);
        insertStructData(pFindedResult,7,pinquered->department_);
        insertStructData(pFindedResult,8,pinquered->refertithedoctor_);
        insertStructData(pFindedResult,9,pinquered->bednumber_);
        insertStructData(pFindedResult,10,QString::number(pinquered->agesample));
        insertStructData(pFindedResult,11,pinquered->AddsampleTime);
        insertStructData(pFindedResult,12,pinquered->SampleName);
        insertStructData(pFindedResult,13,pinquered->barcodesample);
        emit this->LoadInquierdata(numTotal,n,pFindedResult);
        n++;
        iter++;
    }
    return;
}

void QueryDataThread::insertStructData(InqueryDatastu_t* pFindedResult,quint8 stuId,QString data)
{
    char* chdata;
	QByteArray ba = data.toLocal8Bit(); // must
    chdata = ba.data();
    switch(stuId)
    {
        case  0: strcpy(pFindedResult->Sample_Num,   chdata);   break; //Id
        case  1: strcpy(pFindedResult->ReagAAresult, chdata);  break; //AA
        case  2: strcpy(pFindedResult->ReagADPresult, chdata); break; //ADP
        case  3: strcpy(pFindedResult->ReagEPIresult, chdata); break; //EPI
        case  4: strcpy(pFindedResult->ReagCOLresult, chdata); break; //COL
        case  5: strcpy(pFindedResult->ReagRISresult, chdata); break; //RIS

        case  6: strcpy(pFindedResult->Sample_Sex, chdata); break; //性别
        case  7: strcpy(pFindedResult->Sample_kebie, chdata); break; //科别
        case  8: strcpy(pFindedResult->Sample_doctor, chdata); break; //送检医生
        case  9: strcpy(pFindedResult->Sample_BedNum, chdata); break; //床号
        case  10: strcpy(pFindedResult->Sample_Age, chdata); break; //年龄

        case  11: strcpy(pFindedResult->AddTime, chdata);       break;
        case  12: strcpy(pFindedResult->Sample_Name, chdata); break;
        case  13: strcpy(pFindedResult->BarCode, chdata); break;
    }
    return;
}

void QueryDataThread::ObatinCreatPdfPara(QString idSample)
{
    QVector<PatientInformationStu *> painterInfovec;
    painterInfovec.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->getAllTestResultData(painterInfovec);
    int numTotal = painterInfovec.size();
    if(numTotal == 0){
        QLOG_ERROR()<<"未找到输出PDF样本信息!"<<endl;
        return;
    }
    auto iter = painterInfovec.constBegin();
    while(iter != painterInfovec.constEnd())
    {
        PatientInformationStu *pinquered = (*iter);
        if(pinquered->sampleId == idSample)
        {
            InqueryDatastu_t* pFindedResult = new InqueryDatastu_t;
            insertStructData(pFindedResult, 0, pinquered->sampleId);
            insertStructData(pFindedResult, 1, pinquered->AAResult);
            insertStructData(pFindedResult, 2, pinquered->ADPResult);
            insertStructData(pFindedResult, 3, pinquered->EPIResult);
            insertStructData(pFindedResult, 4, pinquered->COLResult);
            insertStructData(pFindedResult, 5, pinquered->RISResult);
            insertStructData(pFindedResult,6,pinquered->sex_);
            insertStructData(pFindedResult,7,pinquered->department_);
            insertStructData(pFindedResult,8,pinquered->refertithedoctor_);
            insertStructData(pFindedResult,9,pinquered->bednumber_);
            insertStructData(pFindedResult,10,QString::number(pinquered->agesample));
            insertStructData(pFindedResult,11,pinquered->AddsampleTime);
            insertStructData(pFindedResult,12,pinquered->SampleName);
            insertStructData(pFindedResult,13,pinquered->barcodesample);
            emit this->outPDFPara(pFindedResult);
            break;
        }
        iter++;
    }
    return;
}


void QueryDataThread::slotLocatethelookup(QString inquierId,   QString senddoctor,
                                            QString PatientsName,QString PatientsAge,
                                            QString sectionkind, QString bbednum)
{
    emit this->clearTableWidget(); //先清空
    int numTotal;
    int n = 1;
    QVector<PatientInformationStu *> painterInfovec;
    painterInfovec.clear();

    QList<QString> FindKey;
    if(!inquierId.isEmpty() || !inquierId.isNull() || inquierId != "")
        FindKey.push_back(inquierId);
    if(!senddoctor.isEmpty() || !senddoctor.isNull() || senddoctor != "")
        FindKey.push_back(senddoctor);
    if(PatientsName != "")
        FindKey.push_back(PatientsName);
    if(!PatientsAge.isEmpty() || !PatientsAge.isNull() || PatientsAge != "")
        FindKey.push_back(PatientsAge);
    if(!sectionkind.isEmpty() || !sectionkind.isNull() || sectionkind != "")
        FindKey.push_back(sectionkind);
    if(bbednum != "")
        FindKey.push_back(bbednum);
    FullyAutomatedPlatelets::pinstancesqlData()->getDesignateTestResultData(FindKey,painterInfovec);
    numTotal = painterInfovec.size();
    if(numTotal == 0){
        emit this->InquireEmpty();
        return;
    }
    auto iter = painterInfovec.constBegin();
    while(iter != painterInfovec.constEnd())
    {
        PatientInformationStu *pinquered = (*iter);
        InqueryDatastu_t* pFindedResult = new InqueryDatastu_t;
        insertStructData(pFindedResult, 0, pinquered->sampleId);
        insertStructData(pFindedResult, 1, pinquered->AAResult);
        insertStructData(pFindedResult, 2, pinquered->ADPResult);
        insertStructData(pFindedResult, 3, pinquered->EPIResult);
        insertStructData(pFindedResult, 4, pinquered->COLResult);
        insertStructData(pFindedResult, 5, pinquered->RISResult);
        insertStructData(pFindedResult,6,pinquered->sex_);
        insertStructData(pFindedResult,7,pinquered->department_);
        insertStructData(pFindedResult,8,pinquered->refertithedoctor_);
        insertStructData(pFindedResult,9,pinquered->bednumber_);
        insertStructData(pFindedResult,10,QString::number(pinquered->agesample));
        insertStructData(pFindedResult,11,pinquered->AddsampleTime);
        insertStructData(pFindedResult,12,pinquered->SampleName);
        insertStructData(pFindedResult,13,pinquered->barcodesample);
        emit this->LoadInquierdata(numTotal,n,pFindedResult);
        n++;
        iter++;
    }
    return;
}
