#include "equipmenttaskactive.h"
#include "QsLog/include/QsLog.h"
#include "loginui.h"
#include "quiutils.h"

equipmentTaskActive::equipmentTaskActive(QObject *parent) : QObject(parent)
{
    m_MovementTotal = 1;
    m_pActivestu = new OneActive[m_MovementTotal];
}
equipmentTaskActive::~equipmentTaskActive()
{
    delete[] m_pActivestu;
    m_pActivestu  = nullptr;
}

void equipmentTaskActive::CleanNeedleCommand(const QString name, QByteArrayList codehex, const int sizenum)
{
    int typeActive = (int)EquipmentMovement::Poweronandoffcleaning;
    m_pActivestu[typeActive].SampleName = name;
    m_pActivestu[typeActive].single_stu = new SCS [sizenum];
    for(int k = 0 ; k < sizenum ; k++)
    {
        m_pActivestu[typeActive].single_stu[k].bFinished = false;
        m_pActivestu[typeActive].single_stu[k].Commandarry = codehex.at(k);
        m_pActivestu[typeActive].single_stu[k].CommandStr = QUIUtils::QByteArrayToQStringList(codehex.at(k));
        m_pActivestu[typeActive].single_stu[k].Index = k;
    }
    m_pActivestu[typeActive].single_stu->bFinished = false;
    m_pActivestu[typeActive].Complete = false;
    return;
}

bool equipmentTaskActive::Destruction_CleanNeedleCommand(OneActive *delpCommand)
{
    int typeActive = (int)EquipmentMovement::Poweronandoffcleaning;
    delete[] delpCommand[typeActive].single_stu;
    delpCommand[typeActive].single_stu = nullptr;

    m_pActivestu[typeActive].SampleName = "nullstr";
    m_pActivestu[typeActive].single_stu->bFinished = false;
    m_pActivestu[typeActive].Complete = false;
    return true;
}

