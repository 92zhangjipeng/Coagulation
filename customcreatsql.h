#ifndef CUSTOMCREATSQL_H
#define CUSTOMCREATSQL_H

#include <QObject>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QStringList>
#include <QMessageBox>
#include <QSqlQuery>
#include <QTableWidget>
#include <QStringList>
#include <QQueue>
#include "cglobal.h"
#include "instrumentcoordinatetable.h"

struct tytemp
{
    uint TubeNumbers;
    uint SamoleNum;
    uint TestTubeState;
    QString KindReagent;
};
Q_DECLARE_METATYPE(tytemp)

struct AllTubeInfo
{
    uint TubeNumbers;
    uint TubeStatus;
    QString KindReagent;
    uint SamoleNum;
};
Q_DECLARE_METATYPE(AllTubeInfo)


struct QualityDataStu
{
    int IndexNum;
    QString ResultQualityControl;
};
Q_DECLARE_METATYPE(QualityDataStu)


//患者结构数据
typedef struct PatientInformationStu
{
   QString sampleId;            //样本号
   QString DateSample;          //样本时间
   int SampleNum;           //样本编号
   QString AddsampleTime;       //添加时间
   QString barcodesample;       //条形码
   QString SampleName;          //姓名
   QString sex_;                //性别
   unsigned int agesample;      //年龄
   QString department_;         //科室

   QString hospitalzationnumber_;       //住院号
   QString bednumber_;                  //床号
   QString wardcode_;                   //病区号
   QString submissiontime_;             //送检时间
   QString refertithedoctor_;           //送检医生
   QString inspectiontime_;              //检验时间
   QString medicalexaminer_;            //检验医生
   QString diagnosis_ ;                   //诊断
   QString remark_;                      //备注
   QString reviewdoctors_;               //审核医生

   QString AAResult;
   QString ADPResult;
   QString EPIResult;
   QString COLResult;
   QString RISResult;
}PInfTable;


//查询列表的Index
enum InquireIndex{Inquire_Sample = 0,AddTask_time,Inquire_Bar_code,Inquire_Name,Inquire_Sex,Inquire_Age,
                  Inquire_Section,Inquire_Bednun,Inquire_sendingdoctor,Inquire_AA,Inquire_ADP,Inquire_EPI,Inquire_COL,Inquire_RIS};

enum TubeState{TubeNums = 0,
               TubeStus,
               TubeReagent,
               TubeSample
               }; //试管信息下标



// 结构体封装查询结果
struct PETestEntry {
    QString PEid;
    int TestCannel;
    QString TestRatioH;
    QString TestRatioM;
    QString TestRatioL;

	PETestEntry() = default;

	// 必须的构造函数
    PETestEntry(const QString& id, int channel, const QString& ratioH,const QString& ratioM,const QString& ratioL)
        : PEid(id), TestCannel(channel), TestRatioH(ratioH), TestRatioM(ratioM), TestRatioL(ratioL) {}
};
struct QueryResult {

	QueryResult() = default;

    bool success = false;
    PETestEntry entry;
    QString errorMessage;

	QueryResult(bool s) : success(s) {}
};

class CustomCreatSql : public QObject
{
    Q_OBJECT
public:
    explicit CustomCreatSql(QObject *parent = nullptr);
    ~CustomCreatSql();

private:
    bool connectMyDB(QSqlDatabase &database,const QString dataPathDb);

public:
    /**
     * 登录界面初始化
    */
    void initializeSQLTable();

    /* 创建数据表并读物机器保存的坐标重新赋值*/
    void     createTable();

    /*关闭数据库*/
    void     CloseAllDb();  //关闭所有数据库


public:
    /** 建表
    * 创建测试数据数据表格 添加记录
    ***/
    bool  creatTestCurveData(QSqlQuery &sql_query);
    bool  creatTestResultTable(QSqlQuery &sql_query);
    bool  creatPETable(QSqlQuery &sql_query);
    bool  ceratPEviewTable(QSqlQuery &sql_query);



    bool insertPETestEntry(const QString& PEid, int testChannel,
                            const QString& testRatioH,
                            const QString &testRatioM,
                            const QString &testRatioL);
    bool insertPEDayResult(const QString& PEDate, const QString& resultH, const QString &resultM, const QString &resultL);


    bool deletePETestEntry(const QString& PEid);


    bool updatePETestEntry(const QString& PEid, int newTestChannel,
                           const QString& newTestRatioH,
                           const QString &newTestRatioM,
                           const QString &newTestRatioL);


    QueryResult  selectPETestEntry(const QString& PEid);
    QList<PETestEntry> selectAllPETestEntries();
    QList<PETestEntry> selectByDatePattern(const QString& PEid, int datePartLength);
    // 查询当天数据（PEid前8位为年月日）
    QList<PETestEntry> selectPETestToday(const QString& PEid);
    // 查询当月数据（PEid前6位为年月）
    QList<PETestEntry> selectPETestThisMonth(const QString& PEid);
    // 查询当年数据（PEid前4位为年份）
    QList<PETestEntry> selectPETestThisYear(const QString& PEid);


    bool selectByResultDate(const QString& date, int datePartLength,
                            QVector<double>& dayx,
                            QVector<double>& resultyH,
                            QVector<double> &resultyM,
                            QVector<double> &resultyL);

    bool selectPEResultToday(const QString& date, QVector<double>& dayx,
                             QVector<double>& resultyH,
                             QVector<double> &resultyM,
                             QVector<double> &resultyL);

    bool selectPEResultThisMonth(const QString& date, QVector<double>& dayx,
                                 QVector<double>& resultyH,
                                 QVector<double> &resultyM,
                                 QVector<double> &resultyL);

    /**  ++
     * @brief insertCurveRecord
     * @param id
     * @return
     */
    bool insertCurveRecord(QString id);   //插入测试曲线数据
    bool insertAddSampleData(QString id,QString addTime, QString barcode); //插入添加样本的信息 包括测试结果


    /**   删除测试数据 和 结果表
     * @brief deleteTestCurveDataTale
     * @param sql_query
     * @return
     */
    bool  deleteTestCurveDataTale(QString delid);
    bool  deleteTestResultTable(const QString &sampleId);

    /**   更新 测试曲线数据
     * @brief updateTestCurveDataTale
     * @param id
     * @param indexReag   试剂号
     * @param updateVal   更新值
     * @return
     */
    bool  updateTestCurveDataTale(QString id, quint8 indexReag,QString updateVal);


    //获取样本基础数据和结果
    bool getTestResultTabledata(QString id, PInfTable &PainterListInfo);

    bool getOneDayTestResultData(const QString targetdate,
                                 QVector<QSharedPointer<PatientInformationStu> > &painterInfovec);

    bool getAllTestResultData(QVector<PatientInformationStu *> &painterInfovec);

    bool getThisMonthResultData(QString MonthStr,QVector<PatientInformationStu *> &painterInfovec);

    bool getDesignateResultData(QList<QString> daysList,QVector<PatientInformationStu *> &painterInfovec);//指定的日期查询

    bool getDesignateTestResultData(QList<QString> keyList,QVector<PatientInformationStu *> &painterInfovec);//指定字段查询

    /**  更新结果字段
     * @brief updateTestResultTable
     * @param id
     * @param updateKey
     * @param val
     * @return
     */
    bool  updateTestResultTable(QString id, QString updateKey, QString val);


    /**  遍历找当天的最大号样本
     * @brief getMaxSampleID
     * @param datetoday
     * @return
     */
    int getMaxSampleID(const QString datetoday);

    /** 对比当天所有样本
     * @brief getTodayAllSampleid
     * @param datetoday
     * @return
     */
    QVector<int> getTodayAllSampleid(const QString datetoday);




    /**  查找试剂曲线的数据
     * @brief getCurveData
     * @param id
     * @param indexReag
     * @return
     */
    QVector<double> getCurveData(QString id,quint8 indexReag);

    /**  查询一个样本的所有曲线数据
     * @brief getOneIDAllCurveData
     * @param id
     * @param dataList
     * @return
     */
    bool getOneIDAllCurveData(const QString id,QMap<quint8,QString> &dataList);


    /**
     * @brief CreatMachineTableData 仪器设置表格
     */
    void  CreatMachineTableData();


public:

    /* 插入机器配置数据库空试管区状态*/
    void     InsertEmptyTube(const int Tubenum, const int State, const QString TypeReagent, const int SampleNum);

    /*更换测试试管盘状态(空闲)*/
    void    logUpdateError(const QSqlError& error, int trayIndex, int hole) ;
    void    UpadteTrayState(const quint8 WitchTray);

    /*更改单个试管状态*/
    void     UpdateEmptyTube_State(const int tubeNum, const int state); //更新状态
    void     UpdateEmptyTube_TypeReagent(const int Tubenum, const QString Type); //更新试剂类型
    void     UpdateEmptyTube_SampleNum(const int Tubenum, const int SampleNum); //更新样本号

    //查询全部
    void     FindAllEmptyTube(QVariantList &tubeInfo);

    //查找范围
    void     findEmptyTubesInRange(int start, int end, QVariantList &result);

    void     FindOneEmptyTube(const int TubeNum,  int &State ,QString &Reagent ,int &Samplenum);

    bool     TubeIdHad(const int IdTube); //试管已添加

    void     TheSameSampleNumInfo(const int SampleNum,QVariantList &NotTubeNum);/*找样本号试管的信息*/

    //开机初始化后找试管区有加血样未测试得血样
    void     TubeHadBloodNotTesting(QVariantList &NotTubeNum);

    /**
    * @brief inquire_test_free_tube_status 查询空试管状态
    * @param back_first_free_loc  首个空的位置
    * @return                       空试管个数
    */
    int inquire_test_free_tube_status(quint8 &back_first_free_loc);

	//查询空试管剩余个数
	quint8  BackEmptyTubeNum();
	QMap<quint8, bool> BackEmptyTubeNumMap();

	quint8 BackFirstEmptyHole();


    //更新试杯状态
    int  update_test_tube_status(quint8 index,int status_,QString index_reag,int sampleid);
    void UpdateTestTubeStateInfo(quint8 index,int status_,QString index_reag,int sampleid);

public:
    void   AddTestReagentGroup(const int Index,const QString AddReagents);//++测试试剂组合
    void   UpdateGroupReagent(const int rows,const QString GroupReagent);
    void   DeleteTestReagentGroup(const QString Index); //删除测试试剂组合
    void   GetTestReagentGroup(QMap<int ,QString> &GroupReagents);
public:
    //管理耗材表 == 耗材添加余量
    void SuppliesAddAllowance(const QString TypeSupplies,const int Allowance);
    int  TypeSuppliesAddAllowance(const QString TypeSupplies);
    void UpdateSuppliesAddAllowance(const QString TypeSupplies,const int Allowance);
    bool SuppliesTypeHad(const QString SuppliesName);

public:
    //操作用户密码表
    void AddUserName(const QString usernameStr, const QString PasswordStr, const int bvip);

    /*用户权限控制*/
    void SelectUserPermissionsControl(QString Loginuser, bool &superVip);

    void DeleteUserChar(const QString usernameStr);

    void NotifyPassword(const QString usernameStr,const QString Passwordchar);

    QString FindPassword(const QString usernameStr);

    void FindAllUsername(QStringList &AllUsername);

    bool HostipalAlreadyHad(QString findKey);
public:
    void  InsertInfoHospital(const QString KeyInfo, const QString Kindname, const QString AbridgeInfo);

    void  DelInfoHospital(const QString KeyInfo);

    void  SeekHospitalInfo(QVector< QVariantList> &HospitalInfo);

    void _obtainPatientInfo(const QString _keywords, QStringList &info_); //通过关键字 或取患者相关信息

public:
    //参考值表格操作
    void    ReferencevalueInsert(const QString Reagentkind,
                                    const QString Timemm,
                                    const int Malelowvalue,
                                    const int MaleHeighvalue,
                                    const int WomanLowvalue,
                                    const int WomanHeighValue,
                                    const QString uintbomit);

    void	_updateReferenceValue(QString key_,QString updatekey_,QString data_);

    void    _obtainPersondata_(QString key_, QString &outMandata_,QString& outWomandata_);

    void    initReerenceTable();

    void    FindReeferenceValue(const QString Key ,QVariantList &DataList);

    /*查找参考值表*/
    void    synchronizeStandardValues(const quint8 Index,const bool bfindMan,QVariant &HighValue,QVariant &LowValue,QString &uintbom);
public:

     //判断数据库中某个数据表是否存在
     bool isTableExist(const int index, QString tableName);

     //删除数据表
     void deleteTable(QSqlDatabase db ,QString& tableName);

     //获取BD
     void GetDataBase(const int index,QSqlDatabase &db);

public:
    //++新任务插入到数据库表
    bool insert_task_to_sqltable(QMap<quint8,QVariant> SingleTaskmapsql);

    //样本删除任务信息
    void   Delete_Single_Task(const QString SampleNum);

    //删除非当天的任务
    void   DeleteOtherDaysTask(const QString tadaydate);

    //查找机器未做完剩余的任务(表只剩余项)
    void   RemainingNotCompleted(QVector<QVariantList> &NotCompletedTask);

    //查询单个样本信息
    void   SearchOneSampleInfo(const int IndexNum,QMap<int,QVariant> &SingleTask);

    //样本试剂测试完统计试剂消耗+1
    bool   tryUpdateReagent(const QString& date_, quint8 index_reag);
    void   testendAddStasReagent(const QString date_,const quint8 index_reag);

    void   inquire_single_stas_total(const QString date_,QMap<quint8,int> &used_map);

    //全部试剂
    void   inquire_all_stats_reagent(QMap<quint8,int> &all_reagent);

    void   inquire_thismonth_stats_reagent(const QString this_date_, QVector<double> &out_data_);

    /* 初始化配置机器试管孔状态(如果测试表格不存在创建初始化全部已丢杯)*/
    void  Initial_configurationEmptyTube();
public:

    /**
    * @brief judgment_table_db_exist 判断数据库DB 是否存在 不存在就创建并打开
    * @param db_name                 db名字
    * @param table_name              表名
    * @param db_addres               db地址
    */
    bool judgment_table_db_exist(const QString db_name,const QString table_name, QSqlDatabase& db_addres);

    /*
    * 创建机器参数表
    */
    bool  CreatParameterTable(const int Index,const QString Tablename,QSqlDatabase DB_Address);
    /*
    * 机器剩余任务
    */
    void  CreatRemainderTaskTable(const int Index,const QString TodayPath,QSqlDatabase DB_Address);

    //void  CreatReportSheetInfoTalbe(const int Index,const QString Tablename,QSqlDatabase DB_Address);/*创建报道单信息*/

    void  creat_statisticstable(QSqlDatabase db_addres, QString table_name, const int exits_);

private:
    QString SQL_PATH;
    const QString QSQLLITENAME = "QSQLITE";
    QSqlDatabase m_database;        //测试数据表和患者信息结果表
    QSqlDatabase m_MachineConfigdb; //机器配置

    const QString INSERT_CURVEDATA = "INSERT INTO CurveData VALUES(?,?,?,?,?,?,?);";


    QSqlDatabase m_statistics_table; //统计表
    QString  m_statisticstablename;  //统计表

    //机器执行任务
    QSqlDatabase m_MachineperformsTaskdb;
    QString m_RemainingTasks;

    QString m_MachineTubeState; //机器空试管区孔状态表名
    QString m_TestProjectTable; //测试试剂项目表
    QString m_SuppLiesTable;    //耗材参数表
    QString m_UserTable;        //用户 密码
    QString m_ReferenceTable;   //聚集率参考值
    QString m_HospitalInfoTable; //医院信息表

    const QVector<QPair<QString, int>> REAGENT_MAP = {
            {"AA", AA_REAGENT},
            {"ADP", ADP_REAGENT},
            {"EPI", EPI_REAGENT},
            {"COL", COL_REAGENT},
            {"RIS", RIS_REAGENT}
    };
};

#endif // CUSTOMCREATSQL_H
