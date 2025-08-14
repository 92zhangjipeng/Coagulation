#ifndef EQUIPMENTTASKACTIVE_H
#define EQUIPMENTTASKACTIVE_H

#include <QObject>
#include <ini_file.h>


typedef struct SingleCommandStatus
{
   QByteArray  Commandarry;
   QStringList  CommandStr;
   quint8   Index;
   bool     bFinished;
}SCS;

typedef struct AllActive
{
    QString SampleName;
    bool Complete;
    SCS *single_stu;

}OneActive;

enum class EquipmentMovement
{
   ReadMainBoard,
   Poweronandoffcleaning,
   Reverttotheorigin
};

class equipmentTaskActive : public QObject
{
    Q_OBJECT
public:
    explicit equipmentTaskActive(QObject *parent = nullptr);
    ~equipmentTaskActive();
public:
    void CleanNeedleCommand(const QString name, QByteArrayList codehex,const int sizenum);
    bool Destruction_CleanNeedleCommand(OneActive *);
signals:

public slots:

private:
    OneActive * m_pActivestu ;
    int m_MovementTotal;

};

#endif // EQUIPMENTTASKACTIVE_H
