#ifndef MYDATABASE_H
#define MYDATABASE_H
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
    QString   id;
    QString   project;
    float    maxvalue;
    float    sixtyValue;
    float    OneHandEightValue;
    float    ThreeHandValue;
    int       maxtime;
}CDBPARAMETER;

typedef std::vector<CDBPARAMETER*> ParamListVec;

class MyDataBase
{
public:
    MyDataBase();
    ~MyDataBase();

private:
    MyDataBase(const MyDataBase &);
    MyDataBase operator =(const MyDataBase &);

private:
    static MyDataBase *m_pInstance;

public:
    static MyDataBase * GetInstance();
    bool ConnectAccessDB(const QString &strDBName,const QString &strUser,const QString &strPwd) const;
    void GetDataOfDBParameter();
    bool WriteTestFile(const QString);
    float readDBData(QString Sample, quint8 IndexReagent, quint8 witchValue);
    void   ReadFileConfigValue();
public:
    QSqlDatabase GetDatabase() const
    {
        return QSqlDatabase::database("MyAccessDB");
    }

    QSqlQuery GetSqlQuery() const
    {
        static QSqlQuery query(m_pInstance->GetDatabase());
        return query;
    }

    bool IsValid() const
    {
        return this->GetDatabase().isValid();
    }
    bool IsConnected() const
    {
        return this->GetDatabase().isOpen();
    }
};

#endif // MYDATABASE_H
