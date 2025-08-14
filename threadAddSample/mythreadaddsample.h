#ifndef MYTHREADADDSAMPLE_H
#define MYTHREADADDSAMPLE_H

#include <QObject>
#include <QThread>
#include <QVariant>
#include "QsLog/include/QsLog.h"

struct  WAIT_TEST_SAMPLE_DATA
{
    QString  _sampleid;
    double _testheighvalue;
    quint8  _CurrRichHole;
    QString _testproject;
    QString _barcode;
    QString _addtime;
    bool  wholeBloodMode;
};
Q_DECLARE_METATYPE(WAIT_TEST_SAMPLE_DATA)
typedef QList<WAIT_TEST_SAMPLE_DATA* > AddSampleDataList;

struct EmptyTestTubeInfo
{
    quint8 UpdateHole;
    quint8 TubeState;
    QString ReagentNanme;
    int IdSample;
};

enum TaskColumns {
    COL_SAMPLE_NUMBER = 0, COL_POOR_HOLE, COL_RICH_HOLE, COL_BLOOD_HIGH,
    COL_ANEMIA_TUBE, COL_BLOODY_IN_TUBE, COL_DETECTION_PROJECT, COL_BARCODE
};

static const QStringList BIND_NAMES = {
    ":样本号", ":贫血孔号", ":富血孔号", ":富血下针高度",
    ":贫血在试管号", ":富血在试管号", ":测试项目", ":条形码"
};


class mythreadaddsample : public QObject
{
    Q_OBJECT

    QThread m_thread;

public:
    explicit mythreadaddsample(QObject *parent = nullptr);

    ~mythreadaddsample();

    void Start();

signals:
   void _addprogress(int index,int total_);
   void _updatetestui(QList<quint8> marktube,QString sample_name,quint8 anemiahole,int index_add,int all_add_task);
   void _sycnPaintentInfo(QString,QString,QString,QString);

private slots:
    void Tmain();

public slots:
    void _waittestsampledata(QString samplename, QString savedtime, QString barcode_str,
                              double bottomBloodHeight, QString project_, quint8 CurrRichHole,
                              int _total, bool insertWholeBloodMode);

private:
    void   SycnAddTaskTestHoleAndCommder(int total_);

    quint8 BackPutOutMinHoleEmptyHole(QMap<quint8, bool >& EmptySQLTubeState);


    /*添加的任务任务详细存到数据库表*/
    void insert_waittest_into_sql(QString sample_name,quint8 suck_anemia,quint8 suck_bloody,
                                    int down_pin,quint8 anemia_tube,
                                    QString spitbloodhole,QString index_reag,QString barcode);




private:
    AddSampleDataList m_WaitTestStuList;
    int m_totalnum;
};

#endif // MYTHREADADDSAMPLE_H
