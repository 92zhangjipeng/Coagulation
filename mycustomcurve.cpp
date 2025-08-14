#pragma execution_character_set("utf-8")
#include "mycustomcurve.h"
#include "QsLog/include/QsLog.h"
#include <QRandomGenerator>
#include <QTime>

MyCustomCurve::MyCustomCurve(QObject *parent) : QObject(parent)
{

}
double getOutDoubleRandomData(double min,double max,uint num = 1)
{
    QStringList outData;
    outData.clear();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(uint i= 0; i<num; ++i)
    {
        double tmp;
        tmp = min + qrand()/(double)(RAND_MAX/(max-min));
        outData<<QString::number(tmp);
    }
   QString  valueout =  outData[outData.size() - 1];
   double   randdata = valueout.toDouble();
   return randdata;
}
QStringList getOutDoubleRandomDataList(double min,double max,uint num = 1)
{
    QStringList outData;
    outData.clear();
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(uint i= 0; i<num; ++i)
    {
        double tmp;
        tmp = min + qrand()/(double)(RAND_MAX/(max-min));
        outData<<QString::number(tmp);
    }
   return  outData;
}

void MyCustomCurve::Topsixtydataoffset(QVector<double> &_origindata)
{
    double mind = -5.00, maxd =  5.00;
    QStringList offset_origindata = getOutDoubleRandomDataList(mind,maxd,59); //原始60个数据偏移随机量
    for(int i = 1; i < 60; i++)
    {
       double tmp =  _origindata.at(i);
       double rand_tmp = QString(offset_origindata.at(i - 1)).toDouble()/100.00;
       double configdata = tmp + rand_tmp*tmp;
       _origindata.replace(i,configdata);
    }
    return;
}

void MyCustomCurve::Topsixty_OneHundredEightydataoffset(QVector<double> &_origindata)
{
    double mind = -3.00, maxd =  3.00;
    QStringList offset_origindata = getOutDoubleRandomDataList(mind,maxd,120); //原始60-180数据偏移随机量
    for(int i = 60; i < 180; i++)
    {
       double tmp =  _origindata.at(i);
       double rand_tmp = QString(offset_origindata.at(i - 60)).toDouble()/100.00;
       double configdata = tmp + rand_tmp*tmp;
       _origindata.replace(i,configdata);
    }
}

void MyCustomCurve::OneHundredEighty_2_enddataoffset(QVector<double> &_origindata)
{
    double mind = -3.00, maxd = 3.00;
    QStringList offset_origindata = getOutDoubleRandomDataList(mind,maxd,120); //原始60-180数据偏移随机量
    for(int i = 180; i < 300; i++)
    {
       double tmp =  _origindata.at(i);
       double rand_tmp = QString(offset_origindata.at(i - 180)).toDouble()/100.00;
       double configdata = tmp + rand_tmp*tmp;
       _origindata.replace(i,configdata);
    }
}

void MyCustomCurve::CreatCurve(INDEX_CURVE  index,const float /*maxValue*/)
{
    QStringList curvedata ;
    switch(index)
    {
        case  CURVE_AA:
            mAA_Curve.clear();
            curvedata = m_AABasicData.split(",");
            mAA_Curve.push_back(0.00);
        break;
        case  CURVE_ADP:
            mADP_Curve.clear();
            curvedata = m_ADPBasicData.split(",");
            mADP_Curve.push_back(0.00);
        break;
        case  CURVE_ADR:
            mADR_Curve.clear();
            curvedata = m_ADRBasicData.split(",");
            mADR_Curve.push_back(0.00);
        break;
        case  CURVE_COL:
            mCOL_Curve.clear();
            curvedata = m_COLBasicData.split(",");
            mCOL_Curve.push_back(0.00);
        break;
    }

    double mind = -3.00, maxd =  3.00;
    QStringList offset_origindata = getOutDoubleRandomDataList(mind,maxd,299); //原始数据偏移随机量
    //整体==>随机偏移 +/- 5%
    for(int i = 1; i < curvedata.size(); i++)
    {
        double offsetrand = QString(offset_origindata.at(i - 1)).toDouble();
        double posvalue =  QString(curvedata.at(i)).toDouble() *100.00; //0.001*100
        double tmpvalue = 0.00;
        if(i%2 == 0)
        {
            tmpvalue =  (posvalue - (offsetrand*posvalue)/100.00)/100.00;
        }
        else
        {
            tmpvalue =  (posvalue + (offsetrand*posvalue)/100.00)/100.00;
        }
        //QLOG_DEBUG()<<"=====offset_rand_value=== "<<posvalue<<"+"<<offsetrand<<"="<<tmpvalue; //0.00
        switch(index)
        {
            case  CURVE_AA:  mAA_Curve.push_back(tmpvalue);   break;
            case  CURVE_ADP: mADP_Curve.push_back(tmpvalue);  break;
            case  CURVE_ADR: mADR_Curve.push_back(tmpvalue);  break;
            case  CURVE_COL: mCOL_Curve.push_back(tmpvalue);  break;
        }
    }

    switch (index)
    {
        case CURVE_AA:
            Topsixtydataoffset(mAA_Curve);                   //前 60个点偏移 +/- 5%
            Topsixty_OneHundredEightydataoffset(mAA_Curve);  //60 - 180 偏移 +/- 5%
            OneHundredEighty_2_enddataoffset(mAA_Curve);     //180 -300 偏移 +/- 5%
        break;
        case CURVE_ADP:
            Topsixtydataoffset(mADP_Curve);
            Topsixty_OneHundredEightydataoffset(mADP_Curve);
            OneHundredEighty_2_enddataoffset(mADP_Curve);
        break;
        case CURVE_ADR:
            Topsixtydataoffset(mADR_Curve);
            Topsixty_OneHundredEightydataoffset(mADR_Curve);
            OneHundredEighty_2_enddataoffset(mADR_Curve);
        break;
        case CURVE_COL:
            Topsixtydataoffset(mCOL_Curve);
            Topsixty_OneHundredEightydataoffset(mCOL_Curve);
            OneHundredEighty_2_enddataoffset(mCOL_Curve);
        break;
        default:    break;
    }

}


//取数据
void MyCustomCurve::fetchData(INDEX_CURVE  index, const int num, double &backvalue)
{
    int totalnum = 0;
    switch(index)
    {
        case CURVE_AA:
            totalnum = mAA_Curve.size() - 1;
            (totalnum >= num)? backvalue = mAA_Curve[num] : backvalue = 0.00;
        break;
        case CURVE_ADP:
            totalnum = mADP_Curve.size() - 1;
            (totalnum >= num)? backvalue = mADP_Curve[num] : backvalue = 0.00;
        break;
        case CURVE_ADR:
            totalnum = mADR_Curve.size() - 1;
            (totalnum >= num)? backvalue = mADR_Curve[num] : backvalue = 0.00;
        break;
        case CURVE_COL:
            totalnum = mCOL_Curve.size() - 1;
            (totalnum >= num)? backvalue = mCOL_Curve[num] : backvalue = 0.00;
        break;
        default:break;
    }
    //QLOG_DEBUG()<<"取值:"<<mAA_Curve<<endl;
}




//设置最大值
void MyCustomCurve::RoutineMaxValue(INDEX_CURVE  index,double maxvalue)
{
	double min_setmaxvalue = 0.00;
	double max_setmaxvalue = 0.00;
	double rel_setmaxvalue = 0.00; //实际设置最大值
    min_setmaxvalue = (maxvalue - (maxvalue *0.03)) / 100.00;
    max_setmaxvalue = (maxvalue + (maxvalue *0.03)) / 100.00;
	rel_setmaxvalue = getOutDoubleRandomData(min_setmaxvalue, max_setmaxvalue); //实际设置最大值
	
    //找到曲线最大值的区间前 20个
    switch(index)
    {
        case CURVE_AA:  configMaxValue(mAA_Curve,rel_setmaxvalue, min_setmaxvalue);  break;
        case CURVE_ADP: configMaxValue(mADP_Curve,rel_setmaxvalue, min_setmaxvalue); break;
        case CURVE_ADR: configMaxValue(mADR_Curve,rel_setmaxvalue, min_setmaxvalue); break;
        case CURVE_COL: configMaxValue(mCOL_Curve,rel_setmaxvalue, min_setmaxvalue); break;
    default: break;
    }
    return;
}

void MyCustomCurve::configMaxValue(QVector<double> &IndexReagent,double config_maxvalue,double /*min_setmaxvalue*/)
{
	double max_maxv = 0.00;
	double max_minv = 0.00;
	QStringList biggermax;
	biggermax.clear();
    quint32 seed = quint32(QDateTime::currentDateTime().toSecsSinceEpoch());
    QRandomGenerator generator(seed);
    int randloss_maxpos = generator.bounded(-20, 20); //最大值位置区间随机值
	//随机后曲线最大值的位置
	auto max = std::max_element(std::begin(IndexReagent), std::end(IndexReagent));
	auto positionmax = std::distance(std::begin(IndexReagent), max);
	int posmax = positionmax;	
	if ((positionmax + randloss_maxpos) >= 300)
	{
		posmax = positionmax - randloss_maxpos;
	}
	if (posmax >= 300)
	{
		int lock = generator.bounded(1, 20);
		posmax =  300 - lock;
	}
	max_minv = config_maxvalue - (config_maxvalue*0.05);
	if (config_maxvalue >= 85)
		max_maxv = config_maxvalue;
	else
		max_maxv = config_maxvalue + (config_maxvalue*0.03);
	
	//如果获取最大值小于 N = 30
	if (config_maxvalue <= 0.5)
	{
		int posfirst = 0;
		int num = 0;
		for (int i = 0; i < IndexReagent.size(); i++)
		{
			if (IndexReagent.at(i) >= config_maxvalue)
			{
				posfirst = i;
				num = IndexReagent.size() - i;
				break;
			}
		}
		biggermax = getOutDoubleRandomDataList(max_minv, max_maxv, num);
		for (int k = 0; k < num; k++)
		{
			int randpos = generator.bounded(0, num);
			IndexReagent.replace(k + posfirst, biggermax.at(randpos).toDouble());
		}
		//IndexReagent.replace(posmax,);
	}
	else if (config_maxvalue > 0.5 && config_maxvalue < 1)
	{
		IndexReagent.replace(posmax, config_maxvalue);
		int max_end = 300 - posmax; //最大值后面的点
		biggermax = getOutDoubleRandomDataList(max_minv, max_maxv, max_end);
		for (int f = posmax; f < 300; f++)
		{
			int randpos = generator.bounded(0, max_end);
			IndexReagent.replace(f, biggermax.at(randpos).toDouble());
		}
		//找到第一个升到升到时间 90 -125
		int posfirst = 0;
        //int num = posmax - posfirst;
		double Updatevalue = 0.00;
		posfirst = generator.bounded(70, 125);
		Updatevalue = IndexReagent.at(posfirst);
		QVector<double> upKvec;
		regression(posfirst, posmax, Updatevalue, config_maxvalue, upKvec);
        //QLOG_DEBUG() << "leng=" << upKvec.size() << "leng++==" << posmax - posfirst;
		int lsize = upKvec.size();
		int endleng = lsize + posfirst;
		for (int n = 0; n < lsize; n++)
		{
			double chanevalue = upKvec.at(n);
			IndexReagent.replace(n + posfirst, chanevalue);
		}
		if (endleng != posmax)
		{
			int indexlen = qAbs(posmax - endleng);
			auto maxk = std::max_element(std::begin(upKvec), std::end(upKvec));
			double biggest = *maxk;
			QStringList datauplist = getOutDoubleRandomDataList(biggest - (biggest * 0.07), config_maxvalue - (config_maxvalue * 0.03), indexlen);
            //QLOG_DEBUG() << "lose==" << indexlen <<"++"<< posmax <<"++=="<< endleng << "leng != " << datauplist;
			for (int i = endleng; i < posmax ; i++)
			{
				IndexReagent.replace(i, datauplist.at(i- endleng).toDouble());
                //QLOG_DEBUG()<<"补齐" << i<<"="<< datauplist.at(i-endleng).toDouble();
			}
	    
		}
        //QLOG_DEBUG() << "Max---==" << config_maxvalue;
		return;


		//QStringList datauplist = getOutDoubleRandomDataList(Updatevalue, config_maxvalue - (config_maxvalue * 0.03), num);
		//QVector <double> KvalueUp;//上升段
		//for (int a = 0; a < num; a++)
		//{
		//	int upk = generator.bounded(0, num);
		//	KvalueUp.push_back(datauplist.at(upk).toDouble());
		//}
		//std::sort(KvalueUp.begin(), KvalueUp.end());
		//auto maxk = std::max_element(std::begin(KvalueUp), std::end(KvalueUp));
		//auto mink = std::min_element(std::begin(KvalueUp), std::end(KvalueUp));
		////直接赋值表示
		//double biggest = *maxk;
		//double smallest = *mink;
		//double mindvalue = (biggest + smallest) / 2;
		//QVector <double> backKvalueUp;//上升段前半段
		//int leng = KvalueUp.size();
		//
		//int total = leng / 2;
		//int continu = 0;
		//for (int i = 0; i < total; i++)
		//{
		//	backKvalueUp.push_back(KvalueUp.at(i));
		//	KvalueUp.remove(i);
		//	IndexReagent.replace(posfirst + i, KvalueUp.at(i));
		//	continu = posfirst + i;
		//}
		////上升段后半段
		//QVector <double> EndKvalueUp; 
		//leng = KvalueUp.size();
		//QStringList enduplist = getOutDoubleRandomDataList(mindvalue, config_maxvalue - (config_maxvalue * 0.03), leng);
		//for (int k = 0; k < leng; k++)
		//{
		//	int randpos = generator.bounded(0, leng);
		//	EndKvalueUp.push_back(enduplist.at(randpos).toDouble());
		//}
		//QLOG_DEBUG() << "系数" << 0 << "斜率" << 0 << "up==" << KvalueUp;
		//int c = 0;
		//for (int n = continu; n < posmax; n++)
		//{
		//	IndexReagent.replace(n, EndKvalueUp.at(c));
		//	c++;
		//}
	}
	
		
	
    return;
}
/************************************线性回归计算中线斜率************************************/
// y = Ax+B
void MyCustomCurve::regression(int  startline, int endline, double lowvalue, double HeighValue, QVector<double> &Middle_black)
{
	double leng = endline - startline; //拉升时常
	double ratio_up = HeighValue - lowvalue; //拉升比例
    //QLOG_DEBUG() << "low==" << lowvalue<<"higmax=="<< HeighValue;
	//double switchRatio = ratio_up / leng;  //拉升时间平均每个点时间整长率
	//QStringList uplist = getOutDoubleRandomDataList(switchRatio - 2* switchRatio, switchRatio, leng);
	quint32 seed = quint32(QDateTime::currentDateTime().toSecsSinceEpoch());
	QRandomGenerator generator(seed);
	int cutquae = generator.bounded(3, 10); //分成 3 - 10段 随机

	int posnum = leng / cutquae; //每段反应时间
	double posRatio = ratio_up / cutquae; //每段反应上升的比例

    //int icut = 0;
	double minv, maxv = 0;
	QStringList *updataList = new QStringList[cutquae];
	for (int i = 0; i < cutquae ; i++)
	{
		int k = generator.bounded(0, cutquae);
		if (i == 0)
		{
			updataList[i] = getOutDoubleRandomDataList(lowvalue - lowvalue*0.05, lowvalue + posRatio, posnum);
		}
		else if (i != cutquae - 1 && i != 0)
		{
			minv = (lowvalue + posRatio*i) - (lowvalue + posRatio*i)*0.05;
			maxv = lowvalue + posRatio*(k + 1);
			if(maxv > minv)
				updataList[i] = getOutDoubleRandomDataList(minv, maxv, posnum); 
			else
				updataList[i] = getOutDoubleRandomDataList(maxv, minv, posnum);
		}
		else if( i + 1 == cutquae)
		{
			minv = lowvalue + posRatio*(cutquae - k);
			maxv = lowvalue + posRatio*(i + 1);
			if(minv < maxv)
				updataList[i] = getOutDoubleRandomDataList(minv, maxv, posnum);
			else
				updataList[i] = getOutDoubleRandomDataList(maxv, minv, posnum);
		}
        //QLOG_DEBUG() << "拉升段:" << cutquae << "数据" << updataList[i];
	}
	
	for (int i = 0; i < cutquae; i++)
	{
		int f = updataList[i].size();
		for (int z = 0; z < f; z++)
			Middle_black.push_back(updataList[i].at(z).toDouble());
	}

	updataList->clear();
	delete []updataList;
	return;
}


