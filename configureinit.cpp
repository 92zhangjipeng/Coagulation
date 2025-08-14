#include "configureinit.h"

#include <QVariant>


ConfigureInit::ConfigureInit(QObject *parent) : QObject(parent)
{

}

bool ConfigureInit::AlarmSoundInitMain()
{
    QVariant temp;
    m_AnalyJson.AnalyticalJsonFile("Alarm", "AlarmSound" ,temp, "AlarmConfigure.json");
    bool sound =    temp.toBool();
    return  sound;
}
