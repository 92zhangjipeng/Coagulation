#ifndef FUNCTIONCLASS_H
#define FUNCTIONCLASS_H
#include <QMutex>   //线程保护序列化
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlIndex>
#include <QtSql/QSqlRecord>
#include <QTextStream>
#include <QStringList>
#include <QVariant>
#include <qt_windows.h>
#include <QFile>
#include <QFileDialog>

typedef struct
{
    QString  SampleId;
}TASKTESTINFO;
typedef std::vector<TASKTESTINFO*> ParamListVec;

class functionclass
{
public:
     functionclass();
     ~functionclass();
private:
     functionclass(const functionclass &);
     functionclass operator =(const functionclass &);

private:
     static functionclass *m_pInstance;
public:
     static functionclass * GetInstance();
     void   startQualityTest(QVector<int> qualityChannel);

};

#endif // FUNCTIONCLASS_H

