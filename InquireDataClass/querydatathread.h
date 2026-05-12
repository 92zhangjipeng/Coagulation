#ifndef QUERYDATATHREAD_H
#define QUERYDATATHREAD_H

#include <QDateEdit>
#include <QObject>
#include <QProgressBar>
#include <QSqlDatabase>
#include <QTableWidget>

typedef  struct
{
    char Sample_Num[50];
    char AddTime[50];
    char BarCode[50];
    char Sample_Name[50];
    char Sample_Sex[50];
    char Sample_Age[50];
    char Sample_kebie[50];
    char Sample_BedNum[50];
    char Sample_doctor[50];
    char ReagAAresult[120];
    char ReagADPresult[120];
    char ReagEPIresult[120];
    char ReagCOLresult[120];
    char ReagRISresult[120];
}InqueryDatastu_t;
Q_DECLARE_METATYPE(InqueryDatastu_t)//注册结构体



class QueryDataThread : public QObject
{
    Q_OBJECT
public:
    explicit QueryDataThread(QObject *parent = nullptr);
    ~QueryDataThread();

signals:
    void LoadInquierdata(int numtotal, int n_ing,InqueryDatastu_t *pdata); //查询到的结果输出到界面

    void clearTableWidget(); //查询前先清空查询界面

    void InquireEmpty();

    void sendCurveData(const QVector<QString>& data);

    void outPDFPara(InqueryDatastu_t *pdata);

private:
    void SycnDesignateDayDataResult(QList<QString> daysList); //指定查询

    void SycnDayDataResult(const QString Dateit);  //查询一天的结果数据

    void SycnMonthDataResult(const QString findDate); //查询每月的数据结果

    void SycnAllDataResult();//查询全部

    void insertStructData(InqueryDatastu_t* pFindedResult,quint8 stuId,QString data);

public slots:
    void _startSycnData();

    void slotFindspecifiedData(QDateEdit *pdataStart, const int days);

    void slotFindModuleStyle(const quint8 indexStyles);

    void InquierCurveViewEnd(const QString& findId);

    void ObatinCreatPdfPara(QString idSample);//要输出到PDF的参数

    void slotLocatethelookup(QString inquierId, QString senddoctor, QString PatientsName,
                               QString PatientsAge, QString sectionkind, QString bbednum); //精确查找
private:



     QMap<QString,InqueryDatastu_t* > minqueryedData; //查询到的数据
};

#endif // QUERYDATATHREAD_H
