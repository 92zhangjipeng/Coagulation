#include "channel_1.h"

Channel_1::Channel_1()
{
    m_Typainducer = new  WaitingReagentType;
    m_channelRunning = false;
    m_saveinitpoorblood = false;
    m_saveinitRichBlood = false;
    m_Reagent_type = 0;
    m_startcalculating = false; //开始己算 -->富血值拿到后
//    m_TestingValue_RIS.reserve(300);
//    m_TestingValue_COL.reserve(300);
//    m_TestingValue_ADR.reserve(300);
//    m_TestingValue_ADP.reserve(300);
//    m_TestingValue_AA.reserve(300);
    m_TestingValue_RIS.clear();
    m_TestingValue_COL.clear();
    m_TestingValue_ADR.clear();
    m_TestingValue_ADP.clear();
    m_TestingValue_AA.clear();
}

Channel_1::~Channel_1()
{
    qDebug()<<"退出通道一完成测试";
}

void Channel_1::SetChannelRunning()
{
    m_channelRunning = true;
}

bool Channel_1::GetChannelRunning()
{
    qDebug()<<"通道状态1"<< m_channelRunning ;
    return m_channelRunning;
}

void Channel_1::Synchronize_Sample(const QString num, const QString name, const QString barcode)
{
    m_number = num;
    m_name = name;     //样本姓名
    m_barcode = barcode;
    qDebug()<<"样本编号:"<<num<<name<<barcode;

}

void Channel_1::ExitChannel()
{


    m_channelRunning = false;

}

void Channel_1::TestingRuuningValue(const double value)
{
    if(m_saveinitpoorblood)
    {
        m_initpoorblood =  value;
        SavePoorBlood(false);
    }
    else if(m_saveinitRichBlood)
    {
        m_initrichblood = value;
        SaveRichBlood(false);
        m_startcalculating = true; //开始计算 集聚率
    }
    else if(m_startcalculating)
    {
        //己算公式
        float molecules,Segmentation = 0;  //分子、分母
        int valuerich = (value - m_initrichblood);
        molecules = tr("%1").arg(valuerich *100).toFloat(); //当前测试值 - 富血初始值
        Segmentation = tr("%1").arg(std::abs(m_initpoorblood - m_initrichblood)).toFloat();
        if (Segmentation == 0)  Segmentation = 1;
        float resultValue = (molecules/Segmentation)/100;

        switch(m_Reagent_type)
        {
        case AA_REAGENT:
            m_TestingValue_AA.push_back(resultValue);
            if(m_TestingValue_AA.size() == 300)
            {
                m_startcalculating = false;
                m_Typainducer->AA_inducer = false;
            }
        break;
        case ADP_REAGENT:
            m_TestingValue_ADP.push_back(resultValue);
            if(m_TestingValue_ADP.size() == 300)
            {
                m_startcalculating = false;
                m_Typainducer->ADP_inducer = false;
            }
        break;
        case ADR_REAGENT:
            m_TestingValue_ADR.push_back(resultValue);
            if(m_TestingValue_ADR.size() == 300)
            {
                m_startcalculating = false;
                m_Typainducer->ADR_inducer = false;
            }
            break;
        case COL_REAGENT:
            m_TestingValue_COL.push_back(resultValue);
            if(m_TestingValue_COL.size() == 300)
            {
                m_startcalculating = false;
                m_Typainducer->COL_inducer = false;
            }
        break;
        case RIS_REAGENT:
            m_TestingValue_RIS.push_back(resultValue);
            if(m_TestingValue_RIS.size() == 300)
            {
                m_startcalculating = false;
                m_Typainducer->RIS_inducer = false;
            }
            break;
        default:
            break;
        }
    }

    if(!m_Typainducer->AA_inducer && !m_Typainducer->ADP_inducer && !m_Typainducer->ADR_inducer &&
            !m_Typainducer->COL_inducer && !m_Typainducer->RIS_inducer)
    {
         //qDebug()<<"通道一完成测试改变running标志";
    }

    //qDebug()<<"通道1值:"<<  value;
}

void Channel_1::WillTestReagentType(const QList<int> TypesReagents)
{
    for(int  i = 0 ;TypesReagents.size(); i++)
    {
        int kind = TypesReagents.at(i);
        if(kind == AA_REAGENT)
        {
            m_Typainducer->AA_inducer = true;
        }
        else if(kind == ADP_REAGENT)
        {
            m_Typainducer->ADP_inducer = true;
        }
        else if(kind == ADR_REAGENT)
        {
            m_Typainducer->ADR_inducer = true;
        }
        else if(kind == COL_REAGENT)
        {
            m_Typainducer->COL_inducer = true;
        }
        else if(kind == RIS_REAGENT)
        {
            m_Typainducer->RIS_inducer = true;
        }
    }

}
void Channel_1::SavePoorBlood(bool open)
{
    m_saveinitpoorblood = open;

}

void Channel_1::SaveRichBlood(bool open)
{
    m_saveinitRichBlood = open;
}

void Channel_1::TestingReagentType(quint8 Types)
{
    m_Reagent_type = Types;
}
