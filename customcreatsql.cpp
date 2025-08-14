#pragma execution_character_set("utf-8")
#include "customcreatsql.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QFile>
#include <QTableWidget>
#include <QMessageBox>
#include <QDateTime>
#include <QImageReader>
#include <QBuffer>
#include "globaldata.h"
#include "ini_file.h"
#include "loadequipmentpos.h"
#include "quiutils.h"
#include <operclass/fullyautomatedplatelets.h>
#include <stdlib.h>



struct TableConfig {
    QString tableName;
    std::function<void()> initializer;
    QString logMessage;
};

QMutex sql_mutex;

CustomCreatSql::CustomCreatSql(QObject *parent) : QObject(parent)
{

    m_MachineTubeState = "EmptyTubeState";    //空试管状态
    m_TestProjectTable = "ReagenstProject";
    m_SuppLiesTable = "SuppliesControl";
    m_UserTable = "UserPassword";
    m_ReferenceTable = "ReferenceValueTable";
    m_HospitalInfoTable = "HospitalInfo";

    m_RemainingTasks = "RemainingTasks"; //机器任务表
    m_statisticstablename = "teststatistics"; //测试统计
}

CustomCreatSql::~CustomCreatSql()
{
    CloseAllDb();
}

bool CustomCreatSql::connectMyDB(QSqlDatabase &database,const QString dataPathDb)
{
    if(!database.isValid() || !database.isOpen())
    {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(dataPathDb);
        return database.open();
    }
    return true;
}


void CustomCreatSql::initializeSQLTable()
{
    QUIUtils::isDirExist(QUIUtils::getappload("SQLFile"));

    //仪器参数保存到数据库
    judgment_table_db_exist(tr("MachineConfig"), tr("MachineConfigSQL"), m_MachineConfigdb);


    CreatMachineTableData();


    SQL_PATH = QDir(QCoreApplication::applicationDirPath()).filePath("SQLFile") + "/"+ "SWData" + ".db";
    QFile   file(SQL_PATH);
    if(!file.exists())
    {
        file.open(QIODevice::WriteOnly);
        file.close();

        if(!connectMyDB(m_database,SQL_PATH)){
            return;
        }
        QSqlQuery sql_query(m_database);
        //测试数据表
        creatTestCurveData(sql_query);

        //测试结果
        creatTestResultTable(sql_query);

        //性能检测表
        creatPETable(sql_query);

        //性能测试输出保存
        ceratPEviewTable(sql_query);

    }

    //机器执行任务数据库
    judgment_table_db_exist(tr("MachineperformsTask"),tr("MachineperformsTaskSQL"),m_MachineperformsTaskdb);

    //统计表的指引 数据表
    judgment_table_db_exist("statisticsReagent",m_statisticstablename ,m_statistics_table);

    //创建统计表格
    creat_statisticstable(m_statistics_table,m_statisticstablename,STATS);

    createTable();

    return;
}


bool CustomCreatSql::judgment_table_db_exist(const QString db_name,
                                               const QString table_name,
                                               QSqlDatabase& db_addres)
{

    // 1. 创建数据库目录
    QString dbDir = QDir(QCoreApplication::applicationDirPath()).filePath("SQLFile");
    if (!QDir().mkpath(dbDir)) {
        QLOG_DEBUG() << "Create directory failed:" << dbDir;
        return false;
    }

    // 2. 管理数据库连接
    if (QSqlDatabase::contains(db_name)) {
        db_addres = QSqlDatabase::database(db_name);
    } else {
        db_addres = QSqlDatabase::addDatabase("QSQLITE", db_name);
        QString Pathaddress = QDir(dbDir).filePath(QString("%1.db").arg(table_name));
        db_addres.setDatabaseName(Pathaddress);
    }

    // 3. 打开数据库连接
    if (!db_addres.isValid()) {
        QLOG_DEBUG() << "Invalid database driver";
        return false;
    }

    if (!db_addres.isOpen() && !db_addres.open()) {
        QLOG_DEBUG() << "Open failed:" << db_addres.lastError().text();
        return false;
    }

    // 4. 检查表是否存在
     bool tableExists = db_addres.tables(QSql::Tables)
                          .contains(table_name, Qt::CaseInsensitive);

     // 5. 日志输出
    QLOG_DEBUG() << "Table" << table_name
                << (tableExists ? "exists" : "not exists");

    return tableExists;
}



/*创建机器参数表*/
bool CustomCreatSql::CreatParameterTable(const int Index,const QString Tablename,QSqlDatabase DB_Address)
{
    bool CreatTableSucceed = false;
    QSqlQuery sqlQuery(DB_Address);
    QString CreatStr;

    const bool isTableEmptyTube = isTableExist(Index,Tablename);
    if(!isTableEmptyTube){
        if (Tablename == m_MachineTubeState) {
                CreatStr = QString("CREATE TABLE IF NOT EXISTS '%1' ("
                    "TubeNum INTEGER PRIMARY KEY AUTOINCREMENT, "
                    "State INT, TypeReagent VARCHAR, SampleNum INT)").arg(Tablename);  // 试管状态表
        } else if (Tablename == m_TestProjectTable) {
            CreatStr = QString("CREATE TABLE IF NOT EXISTS '%1' ("
                "ReagentGroup INT PRIMARY KEY, TypeReagent VARCHAR)").arg(Tablename);  // 项目数表
        } else if (m_SuppLiesTable == Tablename) {
            CreatStr = QString("CREATE TABLE IF NOT EXISTS '%1' ("
                "TypeSupplies VARCHAR PRIMARY KEY, SuppliesAllowance INT)").arg(Tablename);  // 耗材表
        } else if (m_UserTable == Tablename) {
            CreatStr = QString("CREATE TABLE IF NOT EXISTS '%1' ("
                "UserKey VARCHAR PRIMARY KEY, Password VARCHAR, SVIP INT)").arg(Tablename);  // 用户表
        } else if (m_ReferenceTable == Tablename) {
            CreatStr = QString("CREATE TABLE IF NOT EXISTS '%1' ("
                "ReagendType VARCHAR PRIMARY KEY, TestTime VARCHAR, "
                "ManleLow INT, ManleHeigh INT, WomanLow INT, WomanHeigh INT, UnitBom VARCHAR)").arg(Tablename);  // 聚集率表
        } else if (m_HospitalInfoTable == Tablename) {
            CreatStr = QString("CREATE TABLE IF NOT EXISTS '%1' ("
                "TypeSave VARCHAR PRIMARY KEY, TypeInfo VARCHAR, Abridge VARCHAR)").arg(Tablename);  // 医院信息表
        } else {
            QLOG_DEBUG() << "未知表名：" << Tablename;
            return false;
        }
	}
    if (!sqlQuery.exec(CreatStr)) {
        if(!isTableEmptyTube)
            QLOG_DEBUG() << Tablename << "创建失败：" << sqlQuery.lastError().text() << __FILE__ << __LINE__;
        CreatTableSucceed = false;
    } else {
        QLOG_DEBUG() << Tablename << "创建成功" << __FILE__ << __LINE__;
        CreatTableSucceed = true;
    }
    return CreatTableSucceed;
}

/*机器剩余任务,创建空表格*/
void CustomCreatSql::CreatRemainderTaskTable(const int Index,const QString TodayPath,QSqlDatabase DB_Address)
{
    QString creat_sql_table_;
    bool table_already_exists = false;
    QSqlQuery sqlQuery;
    sqlQuery = QSqlQuery(DB_Address);
    table_already_exists = isTableExist(Index,TodayPath);
    if(table_already_exists)
    {
        QLOG_DEBUG()<< TodayPath<<__FUNCTION__<<__LINE__<<"已存在!"<<endl;
    }
    else
    {
        creat_sql_table_ =  QString("CREATE TABLE '%1'(\
                            样本号 varchar PRIMARY KEY NOT NULL,\
                            贫血孔号 int NOT NULL,\
                            富血孔号 int NOT NULL,\
                            富血下针高度 int NOT NULL,\
                            贫血在试管号 int NOT NULL,\
                            富血在试管号 varchar NOT NULL,\
                            测试项目     varchar NOT NULL,\
                            条形码 varchar NOT NULL)").arg(TodayPath);
        sqlQuery.prepare(creat_sql_table_);
        if(!sqlQuery.exec())
        {
            QLOG_DEBUG()<<TodayPath <<"创建失败: "<<sqlQuery.lastError()<<endl;
        }
    }
    sqlQuery.finish();
	sqlQuery.clear();
    return;
}


bool CustomCreatSql::insert_task_to_sqltable(QMap<quint8,QVariant> SingleTaskmapsql)
{
    if (SingleTaskmapsql.size() != 8) {
        QLOG_DEBUG() << "参数错误: 期望8个元素，实际" << SingleTaskmapsql.size() << endl;
        return false;
    }

    QVariant sampleId = SingleTaskmapsql.value(0); // 假设样本号是第一个参数
    QSqlQuery checkQuery(m_MachineperformsTaskdb);
    checkQuery.prepare(QString("SELECT COUNT(1) FROM %1 WHERE 样本号 = ?").arg(m_RemainingTasks));
    checkQuery.addBindValue(sampleId);
    if (checkQuery.exec() && checkQuery.next() && checkQuery.value(0).toInt() > 0) {
        QLOG_DEBUG() << "样本号已存在：" << sampleId.toString();
        return false;
    }
    m_MachineperformsTaskdb.transaction();

    QSqlQuery query(m_MachineperformsTaskdb);
    QString sql = QString("INSERT OR IGNORE INTO '%1' (样本号, 贫血孔号, 富血孔号, 富血下针高度, 贫血在试管号, 富血在试管号, 测试项目, 条形码) "
                          "VALUES (:样本号, :贫血孔号, :富血孔号, :富血下针高度, :贫血在试管号, :富血在试管号, :测试项目, :条形码)")
                      .arg(m_RemainingTasks);
    query.prepare(sql);
    for (int i = 0; i < BIND_NAMES.size(); ++i) {
        query.bindValue(BIND_NAMES[i], SingleTaskmapsql.value(i));
    }

    if (!query.exec()) {
        if(query.lastError().text().contains("UNIQUE")) {  // [[11]]
            QLOG_DEBUG() << "样本号冲突（事务内）";
        }
        m_MachineperformsTaskdb.rollback();
        return false;
    }
    m_MachineperformsTaskdb.commit();
    return true;
}


/*机器做完把样本删除*/
void CustomCreatSql::Delete_Single_Task(const QString SampleNum)
{
    QString  inquresql;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineperformsTaskdb);
    inquresql = QString("DELETE FROM '%1'where 样本号 = '%2'").arg(m_RemainingTasks).arg(SampleNum);
    query_sql.exec(inquresql);
    query_sql.finish();
	query_sql.clear();
    return;
}


/*删除非当天的任务*/
void CustomCreatSql::DeleteOtherDaysTask(const QString tadaydate)
{
    // 1. 参数验证
    if (tadaydate.isEmpty()) {
        QLOG_DEBUG() << "无效的日期参数";
        return;
    }

    // 2. 使用事务确保原子操作
    QSqlDatabase::database().transaction();

    try {
        // 3. 直接执行批量删除
        QSqlQuery query(m_MachineperformsTaskdb);
        const QString sql = QString(
            "DELETE FROM '%1' "
            "WHERE SUBSTR(样本号, 1, INSTR(样本号 || '-', '-') - 1) <> ?")
            .arg(m_RemainingTasks);

        query.prepare(sql);
        query.addBindValue(tadaydate);

        if (!query.exec()) {
            throw std::runtime_error(
                QString("删除失败: %1\nSQL: %2")
                .arg(query.lastError().text())
                .arg(sql).toStdString());
        }

        // 4. 提交事务
        QSqlDatabase::database().commit();

        QLOG_DEBUG() << "成功删除" << query.numRowsAffected()
                    << "条非当日任务记录";

    } catch (const std::exception& e) {
        // 5. 异常处理
        QSqlDatabase::database().rollback();
        QLOG_ERROR() << "数据库操作异常:" << e.what();
    }


//    QVector<QString> ShapTask;
//    ShapTask.clear();
//    QString   Query_statement = QString("select * from '%1'").arg(m_RemainingTasks);
//    QSqlQuery query_sql(Query_statement,m_MachineperformsTaskdb);
//    QSqlRecord rec = query_sql.record();
//    while(query_sql.next())
//    {
//        rec = query_sql.record();
//        int Samplenum_col = rec.indexOf("样本号");
//        ShapTask.push_back(query_sql.value(Samplenum_col).toString());
//    }
//    QSqlQuery query_sql_del;
//    query_sql_del = QSqlQuery(m_MachineperformsTaskdb);
//    auto iter = ShapTask.constBegin();
//    while(iter != ShapTask.constEnd())
//    {
//        QString Samplename = (*iter);
//        QString Sampledate = Samplename.split("-").at(0);
//        if(Sampledate != tadaydate){
//           Query_statement = QString("DELETE FROM '%1'where 样本号 = '%2'").arg(m_RemainingTasks).arg(Samplename);
//           query_sql_del.exec(Query_statement);
//        }
//        iter++;
//    }
//    query_sql.finish();
//	query_sql.clear();
//    query_sql_del.finish();
//	query_sql_del.clear();
//    return;
}


/*查询单个样本信息*/
void CustomCreatSql::SearchOneSampleInfo(const int IndexNum,QMap<int,QVariant> &SingleTask)
{
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString datenum = QString("%1-%2").arg(current_date_time.toString("yyyy/MM/dd")).arg(IndexNum);
    QString   Query_statement = QString("select * from '%1' where 样本号 = '%2'").arg(m_RemainingTasks).arg(datenum);
    QSqlQuery query_sql(Query_statement,m_MachineperformsTaskdb);
    QSqlRecord rec = query_sql.record();
    while(query_sql.next())
    {
        rec = query_sql.record();
        QVariantList ShapTask;
        ShapTask.clear();
        int Samplenum_col = rec.indexOf("样本号");
        SingleTask.insert(0,query_sql.value(Samplenum_col));
        int Richbloodhole_col = rec.indexOf("富血孔号");
        SingleTask.insert(1,query_sql.value(Richbloodhole_col));
        int Richblooddownmm_col = rec.indexOf("富血下针高度");
        SingleTask.insert(2,query_sql.value(Richblooddownmm_col));
        int TestProject_col = rec.indexOf("测试项目");
        SingleTask.insert(3,query_sql.value(TestProject_col));
        int Barcode_col = rec.indexOf("条形码");
        SingleTask.insert(4,query_sql.value(Barcode_col));
    }
    query_sql.finish();
	query_sql.clear();
    return;
}
/*查找机器未做完剩余的任务(表只剩余项)*/
void CustomCreatSql::RemainingNotCompleted(QVector<QVariantList> &NotCompletedTask){

    QString   Query_statement = QString("select * from '%1'").arg(m_RemainingTasks);
    QSqlQuery query_sql(Query_statement,m_MachineperformsTaskdb);
    QSqlRecord rec = query_sql.record();
    while(query_sql.next())
    {
        rec = query_sql.record();
        QVariantList ShapTask;
        ShapTask.clear();
        int Samplenum_col = rec.indexOf("样本号");
        ShapTask.push_back(query_sql.value(Samplenum_col));
        int Poorbloodhole_col = rec.indexOf("贫血孔号");
        ShapTask.push_back(query_sql.value(Poorbloodhole_col));
        int Richbloodhole_col = rec.indexOf("富血孔号");
        ShapTask.push_back(query_sql.value(Richbloodhole_col));
        int Richblooddownmm_col = rec.indexOf("富血下针高度");
        ShapTask.push_back(query_sql.value(Richblooddownmm_col));
        int Poorbloodinhole_col = rec.indexOf("贫血在试管号");
        ShapTask.push_back(query_sql.value(Poorbloodinhole_col));
        int Richbloodinhole_col = rec.indexOf("富血在试管号");
        ShapTask.push_back(query_sql.value(Richbloodinhole_col));
        int TestProject_col = rec.indexOf("测试项目");
        ShapTask.push_back(query_sql.value(TestProject_col));
        int Barcode_col = rec.indexOf("条形码");
        ShapTask.push_back(query_sql.value(Barcode_col));
        NotCompletedTask.push_back(ShapTask);
    }
    query_sql.finish();
	query_sql.clear();
    return;
}


void CustomCreatSql::creat_statisticstable(QSqlDatabase db_addres,QString table_name,const int exits_)
{
    QString creat_;
    QSqlQuery sql_query(db_addres);
    if(!isTableExist(exits_,table_name))
    {
        creat_ = QString("CREATE TABLE '%1'(测试日期 varchar(60) PRIMARY KEY,AA  varchar(10),ADP varchar(10),EPI varchar(10),COL varchar(10),RIS varchar(10))").arg(table_name);
        sql_query.prepare(creat_);
        if(!sql_query.exec())
            QLOG_TRACE()<<"生成统计数据表失败"<< sql_query.lastError()<<endl;
    }
    sql_query.finish();
	sql_query.clear();
    return;
}

// 辅助函数：尝试更新试剂计数（原子操作）
bool CustomCreatSql::tryUpdateReagent(const QString& date_, quint8 index_reag)
{
    QSqlQuery query_update(m_statistics_table);
    QString field;

    // 根据试剂类型选择更新字段
    switch(index_reag) {
        case AA_REAGENT:  field = "AA"; break;
        case ADP_REAGENT: field = "ADP"; break;
        case EPI_REAGENT: field = "EPI"; break;
        case COL_REAGENT: field = "COL"; break;
        case RIS_REAGENT: field = "RIS"; break;
        default: return false; // 无效类型
    }

    // 构建更新语句（原子操作）
    QString update_sql = QString("UPDATE '%1' SET %2 = %2 + 1 "
                                "WHERE 测试日期 = '%3'")
                         .arg(m_statisticstablename)
                         .arg(field)
                         .arg(date_);

    // 执行更新
    if (!query_update.exec(update_sql)) {
        QLOG_WARN() << "更新执行失败：" << date_ << query_update.lastError();
        query_update.finish();
        return false;
    }

    // 检查是否实际更新了记录
    bool updated = (query_update.numRowsAffected() > 0);
    query_update.finish();

    if (!updated) {
        QLOG_WARN() << "更新未生效（记录不存在）：" << date_;
    }

    return updated;
}
void CustomCreatSql::testendAddStasReagent(const QString date_,const quint8 index_reag)
{
    QString sampleDate;
    int sampleId;
    GlobalData::apartSampleId(date_,sampleDate,sampleId);


    //先尝试更新记录（原子操作）
    if (tryUpdateReagent(date_, index_reag)) {
        return; // 更新成功直接返回
    }

    //更新失败（记录不存在），准备插入新记录
    int reagent_add[5] = {0};
    switch(index_reag) {
        case AA_REAGENT:  reagent_add[0] = 1; break;
        case ADP_REAGENT: reagent_add[1] = 1; break;
        case EPI_REAGENT: reagent_add[2] = 1; break;
        case COL_REAGENT: reagent_add[3] = 1; break;
        case RIS_REAGENT: reagent_add[4] = 1; break;
        default: break;
    }

    //执行插入操作
    QSqlQuery query_insert(m_statistics_table);
    QString sql = QString("INSERT INTO '%1'(测试日期,AA,ADP,EPI,COL,RIS) "
                             "VALUES('%2','%3','%4','%5','%6','%7')")
                  .arg(m_statisticstablename)
                  .arg(date_)
                  .arg(reagent_add[0])
                  .arg(reagent_add[1])
                  .arg(reagent_add[2])
                  .arg(reagent_add[3])
                  .arg(reagent_add[4]);

    bool binsert = query_insert.exec(sql);

    if (binsert) {
        // 插入成功
        query_insert.finish();
        return;
    }

    //处理插入失败
	QSqlError error = query_insert.lastError();
	if (error.isValid()) {
		int errCode = error.nativeErrorCode().toInt();  // 替代已弃用的number()
		if (errCode == 19) {
			QLOG_WARN() << "唯一约束冲突，尝试更新操作。错误详情：" << error.text() << "[[3]]";
			if (!tryUpdateReagent(date_, index_reag)) {
				QLOG_WARN() << "更新失败：" << error.text() << "[[19]]";
			}
		}
		else {
			QLOG_WARN() << "插入失败（错误码：" << errCode << "）" << error.text() << "[[5]]";
		}
	}
	query_insert.clear();
}




void CustomCreatSql::inquire_single_stas_total(const QString date_,QMap<quint8,int> &used_map)
{
   used_map.clear();

   // 构建动态列名字符串
   QStringList columns;
   for (const auto& reagent : REAGENT_MAP) {
       columns.append(reagent.first);
   }
   QString columnStr = columns.join(",");

    // 执行参数化查询
    QSqlQuery query(m_statistics_table);
    QString sql = QString("SELECT %1 FROM '%2' WHERE 测试日期 = :date")
                        .arg(columnStr).arg(m_statisticstablename);

    query.prepare(sql);
    query.bindValue(":date", date_);

    // 添加错误处理
    if (!query.exec()) {
        QLOG_WARN() << "Query failed:" << query.lastError().text()
                   << "\nSQL:" << sql;
        return;
    }

    if (query.next()) {
        QSqlRecord rec = query.record();
        for (const auto& reagent : REAGENT_MAP) {
            int colIndex = rec.indexOf(reagent.first);
            if (colIndex != -1) {
                    used_map.insert(
                        static_cast<quint8>(reagent.second),
                        query.value(colIndex).toInt());
            } else {
                    QLOG_WARN() << "Column missing:" << reagent.first;
            }
        }
    }
    else {
        QLOG_WARN() << "No record found for date:" << date_;
    }
}

void CustomCreatSql::inquire_all_stats_reagent(QMap<quint8,int> &all_reagent)
{
    all_reagent.clear();
    QSqlQuery query(m_statistics_table);
    QString sql = QString("SELECT SUM(AA) as sum_aa, SUM(ADP) as sum_adp,"
                          " SUM(EPI) as sum_epi, SUM(COL) as sum_col,"
                          " SUM(RIS) as sum_ris FROM '%1'").arg(m_statisticstablename);

    if (!query.exec(sql)) {
        qDebug() << "Query failed:" << query.lastError().text();
        return; // or handle error
    }
    // 定义试剂映射：列别名、枚举键
    QList<QPair<QString, quint8>> reagentMap = {
        {"sum_aa", AA_REAGENT},
        {"sum_adp", ADP_REAGENT},
        {"sum_epi", EPI_REAGENT},
        {"sum_col", COL_REAGENT},
        {"sum_ris", RIS_REAGENT}
    };

    if (query.next()) {
        QSqlRecord rec = query.record();
        for (const auto &reagent : reagentMap) {
            QString columnAlias = reagent.first;
            quint8 key = reagent.second;
            int index = rec.indexOf(columnAlias);
            if (index != -1) {
                int value = query.value(index).toInt();
                all_reagent.insert(key, value);
            } else {
                qWarning() << "Column not found:" << columnAlias;
            }
        }
    }

    if (query.next()) {
        QSqlRecord rec = query.record();
        int sum_aa = query.value(rec.indexOf("sum_aa")).toInt();
        int sum_adp = query.value(rec.indexOf("sum_adp")).toInt();
        int sum_epi = query.value(rec.indexOf("sum_epi")).toInt();
        int sum_col = query.value(rec.indexOf("sum_col")).toInt();
        int sum_ris = query.value(rec.indexOf("sum_ris")).toInt();

        all_reagent.insert(AA_REAGENT, sum_aa);
        all_reagent.insert(ADP_REAGENT, sum_adp);
        all_reagent.insert(EPI_REAGENT, sum_epi);
        all_reagent.insert(COL_REAGENT, sum_col);
        all_reagent.insert(RIS_REAGENT, sum_ris);
    }
    return;
}


void CustomCreatSql::inquire_thismonth_stats_reagent(const QString this_date_, QVector<double> &out_data_)
{
    int usedreagent[5] = {0};
    QSqlQuery query_(m_statistics_table);
    QString sql = QString("select * from '%1'").arg(m_statisticstablename);
    query_.exec(sql);
    QSqlRecord rec = query_.record();
    while(query_.next())
    {
        rec = query_.record();
        int test_date = rec.indexOf("测试日期");
        QString finde_date = query_.value(test_date).toString();
        //QLOG_DEBUG()<<"当月信息"<<finde_date.left(6)<<"=="<<this_date_;
        if(finde_date.left(6) == this_date_)
        {
            usedreagent[0] = usedreagent[0] + query_.value("AA").toInt();
            usedreagent[1] = usedreagent[1] + query_.value("ADP").toInt();
            usedreagent[2] = usedreagent[2] + query_.value("EPI").toInt();
            usedreagent[3] = usedreagent[3] + query_.value("COL").toInt();
            usedreagent[4] = usedreagent[4] + query_.value("RIS").toInt();
        }
    }
    for(int i = 0 ; i < 5 ; i++)
        out_data_.push_back((double)usedreagent[i]);

	query_.finish();
	query_.clear();
    return;
}


/*
* 创建数据表
* 并读物机器保存的坐标重新赋值
*/
void CustomCreatSql::createTable()
{

    CreatRemainderTaskTable(SQL_TASKREMAINDER,m_RemainingTasks,m_MachineperformsTaskdb);


//    /*空试管孔状态*/
//    if(CreatParameterTable(SQL_MACHINE_SETTING,m_MachineTubeState,m_MachineConfigdb))
//    {
//        //Initial_configurationEmptyTube();
//        //QLOG_TRACE()<<"空试管孔状态";
//    }
//    if(CreatParameterTable(SQL_MACHINE_SETTING,m_TestProjectTable,m_MachineConfigdb))
//    {
//        QLOG_TRACE()<<"创建测试项目保存的数据表[完成]";
//    }
//    if(CreatParameterTable(SQL_MACHINE_SETTING,m_SuppLiesTable,m_MachineConfigdb))
//    {
//         QLOG_TRACE()<<"创建耗材管理参数数据表[完成]";
//    }
//    if(CreatParameterTable(SQL_MACHINE_SETTING,m_UserTable,m_MachineConfigdb))
//    {
//        AddUserName("admin","admin",INTENDANT_CONTROL_VIP);
//        QLOG_TRACE()<<"admin**创建成功"<<__FILE__<<__LINE__;
//    }
//     if(CreatParameterTable(SQL_MACHINE_SETTING,m_ReferenceTable,m_MachineConfigdb))
//    {
//        initReerenceTable();
//        QLOG_TRACE()<<"创建聚集率表格[完成]";
//    }
//    if(CreatParameterTable(SQL_MACHINE_SETTING,m_HospitalInfoTable,m_MachineConfigdb))
//    {
//        QLOG_TRACE()<<"医院信息表和缩写[完成]";
//    }
    return;
}





//初始化配置机器试管孔状态(如果测试表格不存在创建初始化全部已丢杯)
void CustomCreatSql::Initial_configurationEmptyTube()
{
    int IndexHole = 0;
    quint8 indexEquipment = KS1200;
    switch(indexEquipment)
    {
        case KS600:  IndexHole = ONETRAY_TOTALTUBE*2; break;
        case KS800:  IndexHole = ONETRAY_TOTALTUBE*3; break;
        case KS1200: IndexHole = ONETRAY_TOTALTUBE*4; break;
        default: IndexHole = ONETRAY_TOTALTUBE*4;     break;
    }
    for(int n = 0; n < IndexHole ; n++)
        InsertEmptyTube(n, TESTTUBES_FREETIME,"Noone",0);  //TESTTUBES_CLIPPEDAWAY
    return;
}





//医院信息表操作
void CustomCreatSql::InsertInfoHospital(const QString KeyInfo, const QString Kindname, const QString AbridgeInfo)
{
    QSqlQuery query(m_MachineConfigdb);
    QString quer = QObject::tr("INSERT INTO '%1' (TypeSave, TypeInfo, Abridge) VALUES (:TypeSave, :TypeInfo, :Abridge)").arg(m_HospitalInfoTable);
    query.prepare(quer);
    query.bindValue(0, KeyInfo);
    query.bindValue(1, Kindname);
    query.bindValue(2, AbridgeInfo);
    query.exec();
    query.finish();
	query.clear();
    return;
}

void CustomCreatSql::DelInfoHospital(const QString KeyInfo)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString cmd_Data ;
    cmd_Data = QString("delete from '%1' where TypeSave = '%2' ").arg(m_HospitalInfoTable).arg(KeyInfo);
    if(query_sql.exec(cmd_Data))
        QLOG_TRACE()<<"删医院信息"<< m_HospitalInfoTable <<"成功";
    else
        QLOG_TRACE()<<"删医院信息"<< m_HospitalInfoTable <<"失败";
    query_sql.finish();
	query_sql.clear();
    return;
}

void CustomCreatSql::_obtainPatientInfo(const QString _keywords, QStringList& info_)
{
    info_.clear();
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString querystr = QString("select *from '%1'").arg(m_HospitalInfoTable);
    if (query_sql.exec(querystr)){
        while(query_sql.next()){
          QString _obtain =  query_sql.value(0).toString();
          if(_obtain.contains(_keywords,Qt::CaseSensitive))
          {
              QString outinfo_ = query_sql.value(2).toString();
              info_.push_back(outinfo_);
          }
        }
    }
    query_sql.clear();
    query_sql.finish();
    return;
}


void CustomCreatSql::SeekHospitalInfo(QVector< QVariantList> &HospitalInfo)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString temp_str = QString("select *from '%1'").arg(m_HospitalInfoTable);
    if (query_sql.exec(temp_str))
    {
        while(query_sql.next())
        {
            QVariantList Tmpinfo;
            Tmpinfo.clear();
            Tmpinfo.push_back(query_sql.value(0).toString());
            Tmpinfo.push_back(query_sql.value(1).toString());
            Tmpinfo.push_back(query_sql.value(2).toString());
            HospitalInfo.push_back(Tmpinfo);
        }
    }
    query_sql.finish();
	query_sql.clear();
    return;
}

//参考值表格操作 表格已创建初始化
void CustomCreatSql::initReerenceTable()
{
    QStringList KeyList;
    KeyList.clear();
    QUIUtils::itemReferenceTestValue(KeyList);

    // 去重：确保 KeyList 无重复
    KeyList = KeyList.toSet().toList();

    for (int i = 0; i < KeyList.size(); i++) {
        QString key_ = KeyList.at(i);
        QString testTime;
        switch (i % 4) {
            case 0: testTime = "90"; break;
            case 1: testTime = "180"; break;
            case 2: testTime = "300"; break;
            default: testTime = "Max"; break;
        }

        // 插入时跳过冲突（或根据需求替换）
        ReferencevalueInsert(key_, testTime, 0, 0, 0, 0, "0");
    }
}
void CustomCreatSql::ReferencevalueInsert(const QString Reagentkind,
                                            const QString Timemm,
                                            const int Malelowvalue,
                                            const int MaleHeighvalue,
                                            const int WomanLowvalue,
                                            const int WomanHeighValue,
                                            const QString uintbomit)
{

    QSqlQuery query_sql(m_MachineConfigdb);
    // 使用 INSERT OR REPLACE 替换重复值
    QString sql = QString("INSERT OR REPLACE INTO %1 (ReagendType, TestTime, ManleLow, ManleHeigh, WomanLow, WomanHeigh, UnitBom) "
                            "VALUES (:ReagendType, :TestTime, :ManleLow, :ManleHeigh, :WomanLow, :WomanHeigh, :UnitBom)")
                      .arg(m_ReferenceTable);

    query_sql.prepare(sql);
    query_sql.bindValue(":ReagendType", Reagentkind);
    query_sql.bindValue(":TestTime", Timemm);
    query_sql.bindValue(":ManleLow", Malelowvalue);
    query_sql.bindValue(":ManleHeigh", MaleHeighvalue);
    query_sql.bindValue(":WomanLow", WomanLowvalue);
    query_sql.bindValue(":WomanHeigh", WomanHeighValue);
    query_sql.bindValue(":UnitBom", uintbomit);

    if (!query_sql.exec()) {
        QLOG_ERROR() << "插入失败：" << query_sql.lastError().text()
                     << "，SQL语句：" << query_sql.executedQuery();
    }
    query_sql.finish();
    return;
}


void CustomCreatSql::FindReeferenceValue(const QString Key ,QVariantList &DataList)
{
    QString  findKey;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    findKey = QString("select *from '%1' where ReagendType = '%2' ").arg(m_ReferenceTable).arg(Key);
    if(query_sql.exec(findKey) == false)
    {
        QLOG_ERROR()<<"查找参考值失败key="<<findKey<<endl;
    }
    else
    {
         QSqlRecord rec;
         while(query_sql.next())
         {
            rec = query_sql.record();
            DataList.push_back(  query_sql.value(rec.indexOf("ManleLow")).toInt());
            DataList.push_back(  query_sql.value(rec.indexOf("ManleHeigh")).toInt());
            DataList.push_back(  query_sql.value(rec.indexOf("WomanLow")).toInt());
            DataList.push_back(  query_sql.value(rec.indexOf("WomanHeigh")).toInt());
            DataList.push_back(  query_sql.value(rec.indexOf("UnitBom")).toString());
         }
    }
    query_sql.clear();
    query_sql.finish();
    return ;
}

void CustomCreatSql::_updateReferenceValue(QString key_,QString updatekey_,QString data_)
{
    QSqlQuery query_sql(m_MachineConfigdb);
    QString inquresql = QString("update '%1' set '%2' ='%3' where ReagendType = '%4'").arg(m_ReferenceTable).arg(updatekey_).arg(data_).arg(key_);
    if (query_sql.exec(inquresql) == false)
    {
        QLOG_ERROR() << "更新参考值失败" << inquresql;
    }
    query_sql.clear();
    query_sql.finish();
}

//找男同学 && 女同学 参考值
void CustomCreatSql::_obtainPersondata_(QString key_, QString& outMandata_, QString &outWomandata_)
{
    QString  findKey;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    findKey = QString("select *from '%1' where ReagendType = '%2' ").arg(m_ReferenceTable).arg(key_);
    if(query_sql.exec(findKey) == false)
    {
        QLOG_ERROR()<<"查找参考值失败key="<<findKey<<"line="<<__LINE__<<endl;
    }
    else
    {
         QSqlRecord rec;
         while(query_sql.next())
         {
            rec = query_sql.record();
            QString uint_ = query_sql.value(rec.indexOf("UnitBom")).toString();
            int low_man = query_sql.value(rec.indexOf("ManleLow")).toInt();
            int heigh_man = query_sql.value(rec.indexOf("ManleHeigh")).toInt();
            outMandata_  = QString::number(low_man ) + "-" + QString::number(heigh_man ) +"(" + uint_ +")";
            int low_woman =  query_sql.value(rec.indexOf("WomanLow")).toInt();
            int heigh_woman = query_sql.value(rec.indexOf("WomanHeigh")).toInt();
            outWomandata_ = QString::number(low_woman ) + "-" + QString::number(heigh_woman ) +"(" + uint_ +")";
         }
    }
    query_sql.clear();
    query_sql.finish();
    return;
}



//操作用户密码表
void CustomCreatSql::AddUserName(const QString usernameStr,const QString PasswordStr ,const int bvip)
{
    QSqlQuery query(m_MachineConfigdb);
    QString quer = QObject::tr("INSERT INTO '%1' (UserKey, Password ,SVIP) VALUES (:UserKey, :Password, :SVIP)").arg(m_UserTable);
    query.prepare(quer);
    query.bindValue(0, usernameStr);
    query.bindValue(1, PasswordStr);
    query.bindValue(2, bvip);
    query.exec();
    query.finish();
	query.clear();
    return;
}


/*用户权限控制*/
void CustomCreatSql::SelectUserPermissionsControl(QString Loginuser, bool &superVip)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString findKey = QString("select *from '%1' where UserKey = '%2' ").arg(m_UserTable).arg(Loginuser);
    query_sql.exec(findKey);
    while(query_sql.next()){
         superVip = query_sql.value(2).toBool();
    }
    query_sql.finish();
	query_sql.clear();
    return;
}

void CustomCreatSql::DeleteUserChar(const QString usernameStr)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString cmd_Data ;
    cmd_Data = QString("delete from '%1' where UserKey = '%2' ").arg(m_UserTable).arg(usernameStr);
    if(query_sql.exec(cmd_Data))
        QLOG_TRACE()<<"删除任务"<< usernameStr <<"成功";
    else
        QLOG_TRACE()<<"删除用户"<< usernameStr <<"失败";
    query_sql.finish();
	query_sql.clear();
    return;
}
void CustomCreatSql::NotifyPassword(const QString usernameStr,const QString Passwordchar)
{
    QString ReminderText;
    QSqlError lastError;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString cmd = QString("update '%1' set Password ='%2' where UserKey = '%3'").arg(m_UserTable).arg(Passwordchar).arg(usernameStr);
    bool sucess = query_sql.exec(cmd);
    if (!sucess)
    {
        lastError = query_sql.lastError();
        QLOG_TRACE() << lastError << __LINE__ << lastError.driverText();
        ReminderText = QString("%1%2失败!").arg(tr("修改用户:")).arg(usernameStr);
        QLOG_TRACE() << ReminderText<<__LINE__<<__FILE__;
    }
    query_sql.finish();
	query_sql.clear();
    return;
}
QString CustomCreatSql::FindPassword(const QString usernameStr)
{
    QString Passwordchar = "";
    QString  findKey;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    findKey = QString("select *from '%1' where UserKey = '%2' ").arg(m_UserTable).arg(usernameStr);
    query_sql.exec(findKey);
    while(query_sql.next())
    {
         Passwordchar = query_sql.value(1).toString();
    }
    query_sql.finish();
	query_sql.clear();
    return Passwordchar;
}

void CustomCreatSql::FindAllUsername(QStringList &AllUsername)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString temp_str = QString("select *from '%1'").arg(m_UserTable);
    if (query_sql.exec(temp_str))
    {
        while(query_sql.next()){
            AllUsername.push_back(query_sql.value(0).toString());
        }
    }
    query_sql.finish();
	query_sql.clear();
    return;
}

bool CustomCreatSql::HostipalAlreadyHad(QString findKey)
{
    bool bHadHostipal = false;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString querystr = QString("select *from '%1' where UserKey = '%2'").arg(m_UserTable).arg(findKey);
    query_sql.exec(querystr);
    QString NameHad;
    while (query_sql.next())
    {
        NameHad = query_sql.value(0).toString();
    }
    if (!NameHad.isEmpty())
    {
        bHadHostipal = true;
    }
    query_sql.finish();
	query_sql.clear();
    return  bHadHostipal;
}


//判断表是否存在
bool CustomCreatSql::isTableExist(const int index ,QString tableName)
{
    QSqlDatabase database;
    switch(index)
    {
        case SQL_SAMPLE_DATA:     database = QSqlDatabase::database("TestData"); break;
        case SQL_MACHINE_SETTING: database = QSqlDatabase::database("MachineConfig"); break;
        case SQL_QUALITY_CONTROL: database = QSqlDatabase::database("QualityControlData"); break;
        case SQL_TASKREMAINDER:   database = QSqlDatabase::database("MachineperformsTask");  break;
        //case SQL_REPORTSHEETREPORT: database = QSqlDatabase::database("ReportSheetInfo");  break;

        case STATS:    database = QSqlDatabase::database("statisticsReagent");  break;
        default:
                break;
    }
    if(database.tables().contains(tableName))
    {
        return true;
    }
    return false;
}

/*关闭数据库*/
void CustomCreatSql::CloseAllDb()
{
    m_database.close();


    m_MachineConfigdb.close(); //机器配置



    if(m_MachineperformsTaskdb.isOpen())
        m_MachineperformsTaskdb.close();

    return;
}




void CustomCreatSql::GetDataBase(const int index,QSqlDatabase &db)
{
    switch(index)
    {
        case SQL_SAMPLE_DATA:   db = m_database;          break;
        case SQL_MACHINE_SETTING: db = m_MachineConfigdb; break;
        default:
        break;
    }
    return;
}

//管理耗材表 == 耗材添加余量
void CustomCreatSql::SuppliesAddAllowance(const QString TypeSupplies,const int Allowance)
{
    QSqlQuery query(m_MachineConfigdb);
    QString quer = QObject::tr("INSERT INTO '%1' (TypeSupplies, SuppliesAllowance) VALUES (:TypeSupplies, :SuppliesAllowance)").arg(m_SuppLiesTable);
    query.prepare(quer);
    query.bindValue(0, TypeSupplies);
    query.bindValue(1, Allowance);
    query.exec();
    query.finish();
	query.clear();
    return;
}
//耗材已存在
bool CustomCreatSql::SuppliesTypeHad(const QString SuppliesName)
{
    bool bHadSuppliesName = false;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString querystr = QString("select *from '%1' where TypeSupplies = '%2'").arg(m_SuppLiesTable).arg(SuppliesName);
    query_sql.exec(querystr);
    QString  kind = "";
    while (query_sql.next())
    {
        kind = query_sql.value(0).toString();
    }
    if (!kind.isEmpty())
    {
        bHadSuppliesName = true;
    }
    query_sql.finish();
	query_sql.clear();
    return  bHadSuppliesName;
}
//每个试剂的报警限
int CustomCreatSql::TypeSuppliesAddAllowance(const QString TypeSupplies)
{
    uint SuppliesAllowance = 0;
    QString  findKey;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    findKey = QString("select *from '%1' where TypeSupplies = '%2' ").arg(m_SuppLiesTable).arg(TypeSupplies);
    query_sql.exec(findKey);
    while(query_sql.next())
    {
         SuppliesAllowance = query_sql.value(1).toUInt();
    }
    query_sql.finish();
	query_sql.clear();
    return SuppliesAllowance;
}
void CustomCreatSql::UpdateSuppliesAddAllowance(const QString TypeSupplies,const int Allowance)
{
    QString ReminderText;
    QSqlError lastError;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString cmd = QString("update '%1' set SuppliesAllowance ='%2' where TypeSupplies = '%3'").arg(m_SuppLiesTable).arg(Allowance).arg(TypeSupplies);
    bool sucess = query_sql.exec(cmd);
    if (!sucess)
    {
        lastError = query_sql.lastError();
        QLOG_DEBUG() << lastError << __LINE__ << lastError.driverText();
        ReminderText = QString("%1%2失败!").arg(tr("更新耗材库表:")).arg(m_SuppLiesTable);
        QLOG_DEBUG() << ReminderText<<__LINE__<<__FILE__<<endl;
    }
    query_sql.finish();
	query_sql.clear();
    return;
}



//++测试试剂组合
void CustomCreatSql::AddTestReagentGroup(const int Index,const QString AddReagents)
{
    QSqlQuery query(m_MachineConfigdb);

    QString quer = QString("INSERT INTO %1 (ReagentGroup, TypeReagent) "
                            "VALUES (:ReagentGroup, :TypeReagent)")
                            .arg(m_TestProjectTable);

    if (!query.prepare(quer)) {
        QLOG_DEBUG() << "Prepare error:" << query.lastError().text();
        return;
    }

    // 使用命名参数绑定（更清晰安全）
    query.bindValue(":ReagentGroup", Index);
    query.bindValue(":TypeReagent", AddReagents);

    if (!query.exec()) {
        QLOG_DEBUG() << "Execute error:" << query.lastError().text() << "| SQL:" << quer;
        return;
    }
}

//删除测试试剂组合
void CustomCreatSql::DeleteTestReagentGroup(const QString Index)
{

    QSqlQuery query(m_MachineConfigdb);

    // 构建 SQL 模板（表名直接拼接，WHERE 值使用命名参数）
    QString sql = QString("DELETE FROM %1 WHERE TypeReagent = :TypeReagentValue")
                         .arg(m_TestProjectTable);  // 注意：确保 m_TestProjectTable 可信

    // 预编译 SQL 语句
    if (!query.prepare(sql))
    {
        QLOG_DEBUG() << "[SQL Prepare Error]" << query.lastError().text()
                 << "| Query:" << sql;
        return;
    }

    // 绑定命名参数（自动处理引号和转义）
    query.bindValue(":TypeReagentValue", Index);

    // 执行并检查结果
    if (!query.exec())
    {
        QLOG_DEBUG() << "[SQL Execute Error]" << query.lastError().text()
                 << "| Query:" << sql;
        return;
    }
    return;
}
//删除后更新
void CustomCreatSql::UpdateGroupReagent(const int rows,const QString GroupReagent)
{

    QSqlQuery query(m_MachineConfigdb);
    // 使用参数化查询，避免SQL注入（核心安全改进）
    const QString sql = QString("UPDATE %1 SET ReagentGroup = :newReagentGroup "
                                    "WHERE TypeReagent = :typeReagentValue")
                               .arg(m_TestProjectTable); // 表名动态拼接需确保来源可信

    // 预编译阶段错误检查（关键可靠性增强）
    if (!query.prepare(sql)) {
        const QSqlError error = query.lastError();
        QLOG_DEBUG() << "[SQL Prepare Error]" << error.text()
                    << "| Driver Error:" << error.driverText()
                    << "| SQL:" << sql << __LINE__;
        return;
    }

    // 类型安全绑定参数（自动处理类型转换）
    query.bindValue(":newReagentGroup", rows);    // 绑定整型参数
    query.bindValue(":typeReagentValue", GroupReagent);  // 绑定字符串参数

    // 执行阶段错误处理
    if (!query.exec()) {
        const QSqlError error = query.lastError();
        QLOG_DEBUG() << "[SQL Execute Error]" << error.text()
                    << "| Driver Error:" << error.driverText()
                    << "| SQL:" << sql << __LINE__;
        const QString errorMsg = QString("试剂组更新失败: %1").arg(error.text());
        QLOG_DEBUG() << errorMsg << __FILE__;
    }
}




//获取试剂组合表格
void CustomCreatSql::GetTestReagentGroup(QMap<int ,QString> &GroupReagents)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString temp_str = QString("select *from '%1'").arg(m_TestProjectTable);
    if (query_sql.exec(temp_str))
    {
        while(query_sql.next())
        {
            GroupReagents.insert(query_sql.value(0).toInt(),query_sql.value(1).toString());
        }
    }
    query_sql.finish();
    query_sql.clear();
    return;
}


/**  插入机器配置数据库空试管区状态
 * @brief CustomCreatSql::InsertEmptyTube
 * @param Tubenum
 * @param State
 * @param TypeReagent
 * @param SampleNum
 */
void CustomCreatSql::InsertEmptyTube(const int Tubenum ,const int State,const QString TypeReagent,const int SampleNum)
{
    QSqlDatabase db = m_MachineConfigdb.database();
        db.transaction(); // 开启事务

        QSqlQuery checkQuery(m_MachineConfigdb);
        // 查询是否存在相同TubeNum
        const QString checkSql = QString("SELECT 1 FROM `%1` WHERE TubeNum = :tubeNum").arg(m_MachineTubeState);
        if (!checkQuery.prepare(checkSql)) {
            QLOG_WARN() << "Check SQL Prepare error:" << checkQuery.lastError().text();
            return;
        }
        checkQuery.bindValue(":tubeNum", Tubenum);
        if (!checkQuery.exec() || !checkQuery.next()) {
            // 不存在记录，执行插入
            QSqlQuery insertQuery(m_MachineConfigdb);
            const QString insertSql = QString("INSERT INTO `%1` (TubeNum, State, TypeReagent, SampleNum) "
                                            "VALUES (:tubeNum, :state, :typeReagent, :sampleNum)")
                                       .arg(m_MachineTubeState);
            if (!insertQuery.prepare(insertSql)) {
                QLOG_WARN() << "Insert SQL Prepare error:" << insertQuery.lastError().text();
                db.rollback();
                return;
            }
            insertQuery.bindValue(":tubeNum", Tubenum);
            insertQuery.bindValue(":state", State);
            insertQuery.bindValue(":typeReagent", TypeReagent);
            insertQuery.bindValue(":sampleNum", SampleNum);
            if (!insertQuery.exec()) {
                QLOG_WARN() << "Insert SQL Execute error:" << insertQuery.lastError().text()
                          << "| Query:" << insertSql
                          << "| Params:" << Tubenum << State << TypeReagent << SampleNum;
                db.rollback();
                return;
            }
        }
        else {
            // 存在记录，执行更新
            QSqlQuery updateQuery(m_MachineConfigdb);
            const QString updateSql = QString("UPDATE `%1` SET State = :state, TypeReagent = :typeReagent, "
                                            "SampleNum = :sampleNum WHERE TubeNum = :tubeNum")
                                       .arg(m_MachineTubeState);
            if (!updateQuery.prepare(updateSql)) {
                QLOG_WARN() << "Update SQL Prepare error:" << updateQuery.lastError().text();
                db.rollback();
                return;
            }
            updateQuery.bindValue(":tubeNum", Tubenum);
            updateQuery.bindValue(":state", State);
            updateQuery.bindValue(":typeReagent", TypeReagent);
            updateQuery.bindValue(":sampleNum", SampleNum);
            if (!updateQuery.exec()) {
                QLOG_WARN() << "Update SQL Execute error:" << updateQuery.lastError().text()
                          << "| Query:" << updateSql
                          << "| Params:" << Tubenum << State << TypeReagent << SampleNum;
                db.rollback();
                return;
            }
    }
    db.commit(); // 提交事务
    return;
}

/*更换测试试管盘状态(空闲)*/
// 专用错误日志函数
void CustomCreatSql::logUpdateError(const QSqlError& error, int trayIndex, int hole)
{
    const QString msg = QString("更新试管盘%1失败 (孔位%2) - 驱动错误: %3 | 数据库错误: %4")
                       .arg(trayIndex).arg(hole)
                       .arg(error.driverText()).arg(error.text());
    QLOG_ERROR() << msg;
}

void CustomCreatSql::UpadteTrayState(const quint8 WitchTray)
{
    QSqlQuery query(m_MachineConfigdb);
    const int index = WitchTray + 1;
    // 校验托盘编号合法性
    if (index < TEST_THE_TUBEREEL_1 || index > TEST_THE_TUBEREEL_4) {
        QLOG_ERROR() << "Invalid tray index:" << index;
        return;
    }
    // 计算孔位范围（优化数学逻辑）
    const int tubesPerTray = ONETRAY_TOTALTUBE;
    const int startHole = (index - 1) * tubesPerTray;
    const int endHole = index * tubesPerTray;


    // 使用事务提升性能
    m_MachineConfigdb.transaction();

    // 使用参数化查询（核心安全改进）
    const QString sql = QString("UPDATE %1 SET State = ?, TypeReagent = 'noone', SampleNum = 0 "
                               "WHERE TubeNum = ?")
                       .arg(m_MachineTubeState);

    if (!query.prepare(sql)) {
        QLOG_ERROR() << "Prepare error:" << query.lastError().text();
        return;
    }

    bool hasError = false;
    for (int hole = startHole; hole < endHole; ++hole) {
        query.addBindValue(TESTTUBES_FREETIME);  // 状态值
        query.addBindValue(hole);               // 孔位编号

        if (!query.exec()) {
            logUpdateError(query.lastError(), index, hole);
            hasError = true;
            break;  // 根据需求选择 break 或 continue
        }
    }

    // 事务提交/回滚
    hasError ? m_MachineConfigdb.rollback() : m_MachineConfigdb.commit();
}



void CustomCreatSql::UpdateEmptyTube_State(const int tubeNum , const int state )
{
    // 参数化查询防注入
    static const QString updateTemplate =
        "UPDATE %1 SET State = :state WHERE TubeNum = :tubeNum";

    QSqlDatabase::database().transaction();  // 开启事务
    QSqlQuery query(m_MachineConfigdb);
    query.prepare(updateTemplate.arg(m_MachineTubeState));

    //类型安全绑定
    query.bindValue(":state", state);
    query.bindValue(":tubeNum", tubeNum);
    // 执行与错误处理
    if (!query.exec()) {
        QLOG_ERROR() << "SQL更新失败 | 试管:" << tubeNum
            << " | 错误:" << query.lastError().text()
            << " | SQL:" << query.lastQuery();
        QSqlDatabase::database().rollback();  // 失败回滚
        return;
    }

    // 更新结果校验
    if (query.numRowsAffected() == 0) {
        QLOG_WARN() << "未找到匹配试管记录:" << tubeNum;
    }

    QSqlDatabase::database().commit();  // 成功提交
    // 注意：移除冗余的finish/clear（RAII自动管理）
    return;
}

void  CustomCreatSql::UpdateEmptyTube_TypeReagent(const int Tubenum, const QString Type)
{
	//参数化查询防注入 
	static const QString updateTemplate =
		"UPDATE %1 SET TypeReagent = :typeReagent WHERE TubeNum = :tubeNum";

	//事务保障原子性
	QSqlDatabase::database().transaction();  // 开启事务
	QSqlQuery query(m_MachineConfigdb);
	query.prepare(updateTemplate.arg(m_MachineTubeState));

	//类型安全绑定 
	query.bindValue(":typeReagent", Type);   // 自动转义特殊字符
	query.bindValue(":tubeNum", Tubenum);    // 整型直接绑定
	// 执行与错误处理 
	if (!query.exec()) {
		QLOG_ERROR() << "SQL更新失败 | 试管:" << Tubenum
			<< " | 错误:" << query.lastError().text()
			<< " | SQL:" << query.lastQuery();
		QSqlDatabase::database().rollback();  // 失败回滚
		return;
	}

	// 更新结果校验 
	if (query.numRowsAffected() == 0) {
		QLOG_WARN() << "未找到匹配试管记录:" << Tubenum;
	}

	QSqlDatabase::database().commit();  // 成功提交
	// 注意：移除冗余的finish/clear（RAII自动管理）
    return;
}

void  CustomCreatSql::UpdateEmptyTube_SampleNum(const int Tubenum, const int SampleNum)
{
	static const QString updateTemplate =
		"UPDATE %1 SET SampleNum = :sampleNum WHERE TubeNum = :tubeNum";

	QSqlDatabase::database().transaction(); // 开启事务 

	QSqlQuery query(m_MachineConfigdb);
	query.prepare(updateTemplate.arg(m_MachineTubeState));
	
	query.bindValue(":sampleNum", SampleNum);
	query.bindValue(":tubeNum", Tubenum);

	if (!query.exec()) {
		QLOG_ERROR() << "SQL update failed for tube:" << Tubenum
			<< "| Error:" << query.lastError().text();
		QSqlDatabase::database().rollback();
		return;
	}

	if (query.numRowsAffected() == 0) {
		QLOG_WARN() << "No rows updated for tube:" << Tubenum;
	}

	QSqlDatabase::database().commit(); // 提交事务 
    return;
}



void  CustomCreatSql::FindAllEmptyTube(QVariantList &tubeInfo )
{
    // 优化点1：使用编译时常量定义列名（避免重复拼接字符串）
    static const QString sqlTemplate = "SELECT %1, %2, %3, %4 FROM %5";
    static const QString tubeNumsCol = "TubeNum";
    static const QString tubeStusCol = "State";
    static const QString tubeReagentCol = "TypeReagent";
    static const QString tubeSampleCol = "SampleNum";

    // 优化点2：使用作用域限定自动清理资源
    QSqlQuery query(m_MachineConfigdb);
    const QString sql = sqlTemplate.arg(tubeNumsCol, tubeStusCol, tubeReagentCol,
                                        tubeSampleCol, m_MachineTubeState);

    if (!query.exec(sql)) {
        // 优化点3：添加详细的错误处理
        QLOG_WARN() << "SQL query failed:" << query.lastError().text()
                   << "\nQuery:" << sql;
        return;
    }

    // 优化点4：预分配内存（减少动态扩容开销）
    const int estimatedRowCount = query.size() > 0 ? query.size() : 240;
    tubeInfo.reserve(tubeInfo.size() + estimatedRowCount);

    // 优化点5：消除临时变量拷贝
    while (query.next()) {
        AllTubeInfo info;
        info.TubeNumbers = query.value(tubeNumsCol).toInt();
        info.TubeStatus = query.value(tubeStusCol).toInt();
        info.KindReagent = query.value(tubeReagentCol).toString();
        info.SamoleNum = query.value(tubeSampleCol).toInt();
        tubeInfo.append(QVariant::fromValue(info));
    }
}

void CustomCreatSql::findEmptyTubesInRange(int start, int end, QVariantList &result){
    QSqlQuery querysql(m_MachineConfigdb);
    // 使用参数化查询避免SQL注入 [[4]]
    QString sql = QString("SELECT * FROM '%1' WHERE number BETWEEN ? AND ?").arg(m_MachineTubeState);
    querysql.prepare(sql);
    querysql.addBindValue(start);      // 绑定起始值
    querysql.addBindValue(end - 1);    // 绑定结束值

    if (querysql.exec()) {
       while (querysql.next()) {  // 遍历所有结果
           AllTubeInfo info;
           // 安全转换：检查字段有效性 [[2]]
           info.TubeNumbers = querysql.value(TubeNums).isValid() ? querysql.value(TubeNums).toInt() : 0;
           info.TubeStatus = querysql.value(TubeStus).isValid() ? querysql.value(TubeStus).toInt() : -1;
           info.KindReagent = querysql.value(TubeReagent).isValid() ? querysql.value(TubeReagent).toString() : "";
           info.SamoleNum = querysql.value(TubeSample).isValid() ? querysql.value(TubeSample).toInt() : 0;

           result.push_back(QVariant::fromValue(info));
       }
    } else {
        QLOG_WARN() << "SQL Error:" << querysql.lastError().text();  // 输出错误信息
    }
    querysql.clear();  // 清理资源
}


int CustomCreatSql::update_test_tube_status(quint8 index,int status_,QString index_reag,int sampleid)
{
    QSqlQuery query_(m_MachineConfigdb);

    // 使用参数化查询防止SQL注入
    QString directives_str = QString("UPDATE `%1` SET SampleNum=?, State=?, TypeReagent=? WHERE TubeNum=?")
                                 .arg(m_MachineTubeState);

    if (!query_.prepare(directives_str)) {
        qWarning() << "Prepare failed:" << query_.lastError().text();
        return -1;
    }

    // 绑定参数并指定数据类型
    query_.addBindValue(sampleid);        // 整型
    query_.addBindValue(status_);         // 整型
    query_.addBindValue(index_reag);      // 字符串
    query_.addBindValue(index);           // 整型

    if (!query_.exec()) {
        qWarning() << "Update failed:" << query_.lastError().text()
                   << "\nQuery:" << directives_str;
        return -2;
    }

    return 0; // 保持与原函数兼容性

//    QSqlQuery query_;
//    query_ = QSqlQuery(m_MachineConfigdb);
//    QString directives_str = QString("update '%1' set SampleNum ='%2',State ='%3',TypeReagent ='%4' where TubeNum = '%5'").arg(m_MachineTubeState)
//                                .arg(sampleid).arg(status_).arg(index_reag).arg(index);
//    query_.setForwardOnly(true); //每次只存储一条记录
//    query_.exec(directives_str);
//    query_.finish();
//	query_.clear();
//    return 0;
}

void CustomCreatSql::UpdateTestTubeStateInfo(quint8 index,int status_,QString index_reag,int sampleid)
{
    QSqlQuery query;
    query = QSqlQuery(m_MachineConfigdb);
    query.prepare("UPDATE EmptyTubeState SET State = ?, SampleNum = ?,TypeReagent = ? WHERE TubeNum = ?");
    query.addBindValue(status_);
    query.addBindValue(sampleid);
    query.addBindValue(index_reag);
    query.addBindValue(index);
    query.exec();
    query.finish();
    query.clear();
    return;
}


quint8 CustomCreatSql::BackEmptyTubeNum()
{
	quint8 HadEmptyTube = 0;
	QString  queryString = QString("select * from '%1' where State = '%2'").arg(m_MachineTubeState).arg(TESTTUBES_FREETIME);
	QSqlQuery sqlQuery(m_MachineConfigdb);
	bool bquer = sqlQuery.exec(queryString);
	if (bquer) {
		while (sqlQuery.next()) {
			HadEmptyTube++;
		}
	}
	sqlQuery.finish();
	sqlQuery.clear();
	return HadEmptyTube;
}

QMap<quint8, bool> CustomCreatSql::BackEmptyTubeNumMap()
{
	QMap<quint8, bool> HadEmptyTube;
	HadEmptyTube.clear();
	QSqlQuery query(m_MachineConfigdb);
	QString  queryString = QString("select * from '%1' where State = '%2'").arg(m_MachineTubeState).arg(TESTTUBES_FREETIME);
	bool bquer = query.exec(queryString);
	if (bquer) {
		while (query.next()) {
			quint8 indexTube = query.value(TubeNums).toUInt();
			HadEmptyTube.insert(indexTube, true);
		}
	}
	query.finish();
	query.clear();
	return HadEmptyTube;
}


quint8 CustomCreatSql::BackFirstEmptyHole()
{
	quint8 BackHole = 0;
	QVector<quint8> HadEmptyTubeList;
	HadEmptyTubeList.clear();
	QSqlQuery query(m_MachineConfigdb);
	QString  queryString = QString("select * from '%1' where State = '%2'").arg(m_MachineTubeState).arg(TESTTUBES_FREETIME);
	bool bquer = query.exec(queryString);
	if (bquer) {
		while (query.next()) {
			quint8 indexTube = query.value(TubeNums).toUInt();
			HadEmptyTubeList.push_back(indexTube);
		}
	}
	std::sort(HadEmptyTubeList.begin(), HadEmptyTubeList.end());
	if (!HadEmptyTubeList.isEmpty())
	{
		auto min = std::min_element(std::begin(HadEmptyTubeList), std::end(HadEmptyTubeList));
		auto positionmin = std::distance(std::begin(HadEmptyTubeList), min);
		BackHole = positionmin;
	}
	query.finish();
	query.clear();
	return BackHole;
}




int CustomCreatSql::inquire_test_free_tube_status(quint8 &back_first_free_loc)
{
    QVector<quint8> free_test_tubevec;
    QSqlQuery query_;
    query_ = QSqlQuery(m_MachineConfigdb);
    QString query_str = QString("select *from '%1'").arg(m_MachineTubeState);
    if(query_.exec(query_str))
    {
        while (query_.next())
        {
            int status_tube = query_.value(TubeStus).toInt();
            quint8 indexTube = query_.value(TubeNums).toUInt();
            if(status_tube == TESTTUBES_FREETIME)
                free_test_tubevec.push_back(indexTube);
        }
    }
    int size_free = free_test_tubevec.size();
    std::sort(free_test_tubevec.begin(),free_test_tubevec.end());
        if(!free_test_tubevec.isEmpty())
            back_first_free_loc = free_test_tubevec.first();
        else
            back_first_free_loc = 0;
    query_.finish();
    query_.clear();
    return size_free;
}

//开机初始化后找试管区有加血样未测试得血样
void CustomCreatSql::TubeHadBloodNotTesting(QVariantList &NotTubeNum)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString temp_str = QString("select *from '%1'").arg(m_MachineTubeState);
    if (query_sql.exec(temp_str))
    {
        while(query_sql.next())
        {
            uint StateTube = query_sql.value(TubeStus).toUInt();
            if(StateTube == 2)
            {
                QVariant temp1;
                tytemp info;
                info.SamoleNum =  query_sql.value(TubeSample).toInt();  //样本号
                info.TubeNumbers = query_sql.value(TubeNums).toUInt();
                info.KindReagent = query_sql.value(TubeReagent).toString(); //试剂类型
                temp1 = QVariant::fromValue(info);
                NotTubeNum.push_back(temp1);
            }
        }
    }
    query_sql.finish();
	query_sql.clear();
    return;
}

//试管已存在？
bool CustomCreatSql::TubeIdHad(const int IdTube)
{
    bool bHadTube = false;
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString querystr = QString("select *from '%1' where TubeNum = '%2'").arg(m_MachineTubeState).arg(IdTube);
    query_sql.exec(querystr);
    int  kind = -1;
    while (query_sql.next())
    {
        kind = query_sql.value(TubeNums).toInt();
    }
    if (kind >= 0)
    {
        bHadTube = true;
    }
    query_sql.finish();
	query_sql.clear();
    return  bHadTube;
}
/*找样本号试管的信息*/
void CustomCreatSql::TheSameSampleNumInfo(const int SampleNum,QVariantList &NotTubeNum)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString querystr = QString("select *from '%1' where SampleNum = '%2'").arg(m_MachineTubeState).arg(SampleNum);
    query_sql.exec(querystr);
    while(query_sql.next()){
        QVariant temp1;
        tytemp info;
        info.SamoleNum =  query_sql.value(TubeSample).toUInt();
        info.TubeNumbers = query_sql.value(TubeNums).toUInt();
        info.KindReagent = query_sql.value(TubeReagent).toString();
        info.TestTubeState = query_sql.value(TubeStus).toUInt();
        temp1 = QVariant::fromValue(info);
        NotTubeNum.push_back(temp1);
    }
    query_sql.finish();
	query_sql.clear();
    return;
}


void CustomCreatSql::FindOneEmptyTube(const int TubeNum,  int &State ,QString &Reagent ,int &Samplenum)
{
	QSqlQuery query_sql = QSqlQuery(m_MachineConfigdb);
	QString findKey = QString("select *from '%1' where TubeNum = '%2' ").arg(m_MachineTubeState).arg(TubeNum);
    query_sql.exec(findKey);
    while(query_sql.next())
    {
         State = query_sql.value(TubeStus).toInt();
         Reagent = query_sql.value(TubeReagent).toString();
         Samplenum = query_sql.value(TubeSample).toInt();
    }
	query_sql.finish();
	query_sql.clear();
    return;
}










//删除数据表
void CustomCreatSql::deleteTable(QSqlDatabase db, QString& tableName)
{
    QSqlQuery sqlQuery(db);
    sqlQuery.exec(QString("DROP TABLE %1").arg(tableName));
    if(sqlQuery.exec())
    {
        qDebug() << sqlQuery.lastError();
    }
    else
    {
        qDebug() << "deleted table success";
    }
    sqlQuery.finish();
	sqlQuery.clear();
}









void CustomCreatSql::synchronizeStandardValues(const quint8 Index,const bool bfindMan,QVariant &HighValue,QVariant &LowValue,QString &uintbom)
{
    QSqlQuery query_sql;
    query_sql = QSqlQuery(m_MachineConfigdb);
    QString keystr ="";
    switch(Index)
    {
        case 0: keystr = "AA聚集率-1";     break;
        case 1: keystr = "AA聚集率-2";     break;
        case 2: keystr = "AA聚集率-3";     break;
        case 3: keystr = "AA聚集率-Max";   break;
        case 4: keystr = "ADP聚集率-1";     break;
        case 5: keystr = "ADP聚集率-2";     break;
        case 6: keystr = "ADP聚集率-3";     break;
        case 7: keystr = "ADP聚集率-Max";   break;
        case 8: keystr = "EPI聚集率-1";     break;
        case 9: keystr = "EPI聚集率-2";     break;
        case 10: keystr = "EPI聚集率-3";     break;
        case 11: keystr = "EPI聚集率-Max";   break;
        case 12: keystr = "COL聚集率-1";     break;
        case 13: keystr = "COL聚集率-2";     break;
        case 14: keystr = "COL聚集率-3";     break;
        case 15: keystr = "COL聚集率-Max";   break;
        case 16: keystr = "RIS聚集率-1";     break;
        case 17: keystr = "RIS聚集率-2";     break;
        case 18: keystr = "RIS聚集率-3";     break;
        case 19: keystr = "RIS聚集率-Max";   break;
    default:
        break;
    }
    if(keystr.isEmpty())
    {
        QLOG_WARN()<<"同步查询聚集率值失败,查询键为空";
        return;
    }
    QString findKey = QString("select *from '%1' where ReagendType = '%2'").arg(m_ReferenceTable).arg(keystr);
    query_sql.exec(findKey);
    while(query_sql.next())
   {
       if(bfindMan)
       {
            HighValue =  query_sql.value("MaleHeigh");
            LowValue =   query_sql.value("MaleLow");
       }
       else
       {
           HighValue =  query_sql.value("WomanHeigh");
           LowValue =   query_sql.value("WomanLow");
       }
       uintbom = query_sql.value("UnitBom").toString();
   }
   query_sql.finish();
   query_sql.clear();
   return ;
}











//所有样本信息插入到表中
void SetColumnText(QTableWidget * tablewiget,int row,int col,QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    QFont font;
    font.setBold(true);
    item->setFont(font);
    QBrush nullColor(QColor(120,120,120));
    item->setBackground(nullColor);
    tablewiget->setItem(row,col,item);
}

void SetColumnTextGroup(QTableWidget * tablewiget, int row, int col, QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tablewiget->setSpan(row*4, col, 4, 1);
    QFont font;
    font.setBold(true);
    //font.setPointSizeF(12);
    item->setFont(font);
    tablewiget->setItem(row*4, col, item);
}

//插入集聚率
void  insertTableAgglomeration_rate(QTableWidget *Table,int &row,QStringList OutResult_AA,QStringList OutResult_ADP,
                              QStringList OutResult_ADR,QStringList OutResult_COL,QStringList OutResult_RIS )
{
    Table->insertRow(row);

    SetColumnText(Table,row,Inquire_AA, QString("%1  [60S]").arg(OutResult_AA.at(0)));
    SetColumnText(Table, row, Inquire_ADP, QString("%1  [60S]").arg(OutResult_ADP.at(0)));
    SetColumnText(Table, row, Inquire_EPI, QString("%1  [60S]").arg(OutResult_ADR.at(0)));
    SetColumnText(Table, row, Inquire_COL, QString("%1  [60S]").arg(OutResult_COL.at(0)));
    SetColumnText(Table, row, Inquire_RIS, QString("%1  [60S]").arg(OutResult_RIS.at(0)));
    row++;
    Table->insertRow(row);
    SetColumnText(Table, row, Inquire_AA, QString("%1  [180S]").arg(OutResult_AA.at(1)));
    SetColumnText(Table, row, Inquire_ADP, QString("%1  [180S]").arg(OutResult_ADP.at(1)));
    SetColumnText(Table, row, Inquire_EPI, QString("%1  [180S]").arg(OutResult_ADR.at(1)));
    SetColumnText(Table, row, Inquire_COL, QString("%1  [180S]").arg(OutResult_COL.at(1)));
    SetColumnText(Table, row, Inquire_RIS, QString("%1  [180S]").arg(OutResult_RIS.at(1)));

    row++;
    Table->insertRow(row);

    SetColumnText(Table, row, Inquire_AA, QString("%1  [300S]").arg(OutResult_AA.at(2)));
    SetColumnText(Table, row, Inquire_ADP, QString("%1  [300S]").arg(OutResult_ADP.at(2)));
    SetColumnText(Table, row, Inquire_EPI, QString("%1  [300S]").arg(OutResult_ADR.at(2)));
    SetColumnText(Table, row, Inquire_COL, QString("%1  [300S]").arg(OutResult_COL.at(2)));
    SetColumnText(Table, row, Inquire_RIS, QString("%1  [300S]").arg(OutResult_RIS.at(2)));

    row++;
    Table->insertRow(row);

    SetColumnText(Table, row, Inquire_AA, QString("%1  [Max]").arg(OutResult_AA.at(3)));
    SetColumnText(Table, row, Inquire_ADP, QString("%1  [Max]").arg(OutResult_ADP.at(3)));
    SetColumnText(Table, row, Inquire_EPI, QString("%1  [Max]").arg(OutResult_ADR.at(3)));
    SetColumnText(Table, row, Inquire_COL, QString("%1  [Max]").arg(OutResult_COL.at(3)));
    SetColumnText(Table, row, Inquire_RIS, QString("%1  [Max]").arg(OutResult_RIS.at(3)));

    return;
}






















/*创建测试曲线数据数据表格*/
bool CustomCreatSql::creatTestCurveData(QSqlQuery &sql_query)
{
    const QString SQL_TABLE_CREAT = "CREATE TABLE CurveData (样本号 varchar PRIMARY KEY,测试通道 int,"
                                    "AA varchar, ADP varchar, EPI varchar, COL varchar,RIS varchar)";
    sql_query.prepare(SQL_TABLE_CREAT);
    if(!sql_query.exec()){
       QLOG_DEBUG()<<"测试曲线数据表创建失败: "<<sql_query.lastError()<<endl;
    }
    sql_query.finish();
    sql_query.clear();
    return true;
}

bool CustomCreatSql::creatTestResultTable(QSqlQuery &sql_query)
{
    const QString SQL_TABLE_CREAT = QString(
            "CREATE TABLE IF NOT EXISTS ResultData ("
            "样本号 TEXT  PRIMARY KEY, "
            "样本日期 TEXT, "
            "样本编号 INTEGER, "
            "添加时间 TEXT , "
            "条形编码号 TEXT, "
            "姓名 TEXT, "
            "性别 TEXT CHECK(性别 IN ('男', '女', '未知')), "
            "年龄 INTEGER CHECK(年龄 BETWEEN 0 AND 120), "
            "科别 TEXT, "
            "住院号 TEXT, "
            "病区 TEXT, "
            "床号 TEXT, "
            "临床诊断 TEXT, "
            "送检医生 TEXT, "
            "检验医生 TEXT, "
            "备注 TEXT, "
            "审核医生 TEXT, "
            "送检时间 DATETIME, "
            "检验时间 DATETIME, "
            "AA TEXT, "
            "ADP TEXT, "
            "EPI TEXT, "
            "COL TEXT, "
            "RIS TEXT"
            ")"
    );
    if (!sql_query.exec(SQL_TABLE_CREAT)) {
        QLOG_DEBUG() << "测试结果数据表创建失败: " << sql_query.lastError();
        return false;
    }
    return true;
}



//性能测试表操作
bool CustomCreatSql::creatPETable(QSqlQuery &sql_query){
    static const QString PETABLE = QStringLiteral(
           "CREATE TABLE IF NOT EXISTS PETest ("
           "    PEid      VARCHAR(64) NOT NULL, "
           "    TestCannel INTEGER, "
           "    TestRatioH  VARCHAR(64), "
           "    TestRatioM  VARCHAR(64), "
           "    TestRatioL  VARCHAR(64), "
           "    PRIMARY KEY (PEid)"
           ")"
       );

   if (Q_UNLIKELY(!sql_query.exec(PETABLE))) {
       QLOG_WARN() << Q_FUNC_INFO << "创建表失败:"
                  << sql_query.lastError().text()
                  << "\n执行语句:" << PETABLE;
       return false;
   }
   return true;
}

bool CustomCreatSql::ceratPEviewTable(QSqlQuery &sql_query){
    static const QString PETABLE = QStringLiteral(
           "CREATE TABLE IF NOT EXISTS PEDayResult ("
           "    PEDate  VARCHAR(64) NOT NULL, "
           "    PEResultH   VARCHAR(64), "
           "    PEResultM   VARCHAR(64), "
           "    PEResultL   VARCHAR(64), "
           "    PRIMARY KEY (PEDate)"
           ")"
       );

   if (Q_UNLIKELY(!sql_query.exec(PETABLE))) {
       QLOG_WARN() << Q_FUNC_INFO << "创建表失败:"
                  << sql_query.lastError().text()
                  << "\n执行语句:" << PETABLE;
       return false;
   }
   return true;
}

/**********************数据插入**********************/
bool CustomCreatSql::insertPETestEntry(const QString& PEid, int testChannel,
                                        const QString& testRatioH,
                                        const QString& testRatioM,
                                        const QString& testRatioL) {
    QSqlQuery query(m_database);
    const QString sql = QStringLiteral(
        "INSERT INTO PETest (PEid, TestCannel, TestRatioH , TestRatioM , TestRatioL) "
        "VALUES (:PEid, :TestCannel, :TestRatioH, :TestRatioM, :TestRatioL)"
    );

    query.prepare(sql);
    query.bindValue(":PEid", PEid);
    query.bindValue(":TestCannel", testChannel);
    query.bindValue(":TestRatioH", testRatioH);
    query.bindValue(":TestRatioM", testRatioM);
    query.bindValue(":TestRatioL", testRatioL);

    if (Q_UNLIKELY(!query.exec())) {
        QLOG_WARN() << "[Insert] 操作失败:"
                   << query.lastError().text()
                   << "\nSQL:" << sql
                   << "\n参数: PEid=" << PEid
                   << "TestCannel=" << testChannel
                   << "TestRatio=" << testRatioH;
        return false;
    }
    return true;
}

bool CustomCreatSql::insertPEDayResult(const QString& date, const QString& resultH,
                                       const QString& resultM, const QString& resultL) {
    QSqlQuery sql_query(m_database);
    QString sql;
    // 根据数据库类型选择语法
    if (m_database.driverName() == "QSQLITE") {
            sql = "INSERT OR REPLACE INTO PEDayResult (PEDate, PEResultH, PEResultM, PEResultL) VALUES (:PEDate, :PEResultH, :PEResultM, :PEResultL)";
    } else if (m_database.driverName().contains("QMYSQL")) {
            sql = "INSERT INTO PEDayResult (PEDate, PEResultH, PEResultM, PEResultL) VALUES (:PEDate, :PEResultH, :PEResultM, :PEResultL) "
              "ON DUPLICATE KEY UPDATE PEResult = VALUES(PEResult)";
    } else {
        QLOG_DEBUG() << "Unsupported database driver:" << m_database.driverName();
        return false;
    }

    if (!sql_query.prepare(sql)) {
        QLOG_DEBUG() << "Prepare failed:" << sql_query.lastError().text();
        return false;
    }

    sql_query.bindValue(":PEDate", date);
    sql_query.bindValue(":PEResultH", resultH);
    sql_query.bindValue(":PEResultM", resultM);
    sql_query.bindValue(":PEResultL", resultL);

    if (!sql_query.exec()) {
        QLOG_DEBUG() << "Insert failed:" << sql_query.lastError().text()
                     << "\nSQL:" << sql; // 输出具体错误和 SQL 语句
        return false;
    }
    return true;
}


/**********************数据删除**********************/
bool CustomCreatSql::deletePETestEntry(const QString& PEid) {
    QSqlQuery query(m_database);
    const QString sql = QStringLiteral(
        "DELETE FROM PETest WHERE PEid = :PEid"
    );

    query.prepare(sql);
    query.bindValue(":PEid", PEid);

    if (Q_UNLIKELY(!query.exec())) {
        QLOG_WARN() << "[Delete] 操作失败:"
                   << query.lastError().text()
                   << "\nSQL:" << sql
                   << "\n参数: PEid=" << PEid;
        return false;
    }
    return true;
}

/**********************数据更新**********************/
bool CustomCreatSql::updatePETestEntry(const QString& PEid,
                                        int newTestChannel,
                                        const QString& newTestRatioH,
                                        const QString& newTestRatioM,
                                        const QString& newTestRatioL) {
    QSqlQuery query(m_database);
    const QString sql = QStringLiteral(
        "UPDATE PETest "
        "SET TestCannel = :TestCannel, TestRatioH = :TestRatioH , TestRatioM = :TestRatioM, TestRatioL = :TestRatioL "
        "WHERE PEid = :PEid"
    );

    query.prepare(sql);
    query.bindValue(":PEid", PEid);
    query.bindValue(":TestCannel", newTestChannel);
    query.bindValue(":TestRatioH", newTestRatioH);
    query.bindValue(":TestRatioM", newTestRatioM);
    query.bindValue(":TestRatioL", newTestRatioL);

    if (Q_UNLIKELY(!query.exec())) {
        QLOG_WARN() << "[Update] 操作失败:"
                   << query.lastError().text()
                   << "\nSQL:" << sql
                   << "\n参数: PEid=" << PEid
                   << "TestCannel=" << newTestChannel
                   << "TestRatio=" << newTestRatioH;
        return false;
    }
    return true;
}
/**********************数据查询**********************/
QueryResult CustomCreatSql::selectPETestEntry(const QString& PEid) {
    QueryResult result;
    QSqlQuery query(m_database);
    const QString sql = QStringLiteral(
        "SELECT TestCannel, TestRatioH , TestRatioM, TestRatioL  FROM PETest WHERE PEid = :PEid"
    );

    query.prepare(sql);
    query.bindValue(":PEid", PEid);

    if (Q_UNLIKELY(!query.exec())) {
        result.errorMessage = query.lastError().text();
        return result;
    }

    if (!query.next()) {
        result.success = true;
        result.entry.PEid = PEid;
        result.entry.TestCannel = query.value("TestCannel").toInt();
        result.entry.TestRatioH = query.value("TestRatioH").toString();
        result.entry.TestRatioM = query.value("TestRatioM").toString();
        result.entry.TestRatioL = query.value("TestRatioL").toString();
    }

    return result;
}

// 通用查询函数（按日期部分匹配）
QList<PETestEntry> CustomCreatSql::selectByDatePattern(const QString& PEid, int datePartLength) {
    QList<PETestEntry> result;
    if (PEid.length() < datePartLength) {
		QLOG_ERROR() << "PEid格式错误：长度不足";
        return result;
    }

    QString pattern = PEid.left(datePartLength) + "%"; // 构造LIKE通配符模式
    QSqlQuery query(m_database);
    const QString sql =
        "SELECT PEid, TestCannel, TestRatioH, TestRatioM, TestRatioL FROM PETest WHERE PEid LIKE :pattern";

    query.prepare(sql);
    query.bindValue(":pattern", pattern);

    if (!query.exec()) {
        QLOG_ERROR()<<query.lastError().text();
        return result;
    }

    while (query.next()) { // 遍历所有结果
        result.append(PETestEntry(
            query.value("PEid").toString(),
            query.value("TestCannel").toInt(),
            query.value("TestRatioH").toString(),
            query.value("TestRatioM").toString(),
            query.value("TestRatioL").toString()
            ));
    }
    return result;
}

bool CustomCreatSql::selectByResultDate(const QString& date,
                                            int datePartLength,
                                            QVector<double>& dayx,
                                            QVector<double>& resultyH,
                                            QVector<double>& resultyM,
                                            QVector<double>& resultyL) {
    if (date.length() < datePartLength) {
        QLOG_ERROR() << "PEresult格式错误:长度不足";
        return false;
    }
    QString pattern = date.left(datePartLength) + "%"; // 构造LIKE通配符模式
    QSqlQuery query(m_database);
    const QString sql =
        "SELECT  PEDate, PEResultH, PEResultM, PEResultL FROM PEDayResult WHERE PEDate LIKE :pattern";
    query.prepare(sql);
    query.bindValue(":pattern", pattern);

    if (!query.exec()) {
        QLOG_ERROR()<<query.lastError().text();
        return false;
    }
    while (query.next()) { // 遍历所有结果
        QString dateget = query.value("PEDate").toString();
        dayx.append(dateget.right(2).toDouble());
        resultyH.append(query.value("PEResultH").toString().toDouble());
        resultyM.append(query.value("PEResultM").toString().toDouble());
        resultyL.append(query.value("PEResultL").toString().toDouble());
    }
    return true;
}
// 查询当天数据（PEid前8位为年月日）
bool CustomCreatSql::selectPEResultToday(const QString& date,QVector<double>& dayx,
                                         QVector<double>& resultyH,
                                         QVector<double>& resultyM,
                                         QVector<double>& resultyL){
    return selectByResultDate(date, 8,dayx,resultyH,resultyM,resultyL); // 示例：20231015%
}

// 查询当月数据（PEid前6位为年月）
bool CustomCreatSql::selectPEResultThisMonth(const QString& date,QVector<double>& dayx,
                                             QVector<double>& resultyH,
                                             QVector<double>& resultyM,
                                             QVector<double>& resultyL)
{
    return selectByResultDate(date, 6,dayx,resultyH,resultyM,resultyL); // 示例：202310%
}


// 查询当天数据（PEid前8位为年月日）
QList<PETestEntry> CustomCreatSql::selectPETestToday(const QString& PEid) {
	return selectByDatePattern(PEid, 8); // 示例：20231015%
}

// 查询当月数据（PEid前6位为年月）
QList<PETestEntry> CustomCreatSql::selectPETestThisMonth(const QString& PEid) {
	return selectByDatePattern(PEid, 6); // 示例：202310%
}

// 查询当年数据（PEid前4位为年份）
QList<PETestEntry> CustomCreatSql::selectPETestThisYear(const QString& PEid) {
	return selectByDatePattern(PEid, 4); // 示例：2023%
}

QList<PETestEntry> CustomCreatSql::selectAllPETestEntries() {
    QList<PETestEntry> entries;
    QSqlQuery query(m_database);
    const QString sql = QStringLiteral("SELECT * FROM PETest");

    if (Q_UNLIKELY(!query.exec(sql))) {
        QLOG_ERROR() << "[Select All] 查询失败:"
                    << query.lastError().text()
                    << "\nSQL:" << sql;
        return entries; // 返回空列表
    }

	while (query.next()) {
		entries.append(PETestEntry(
			query.value("PEid").toString(),
			query.value("TestCannel").toInt(),
            query.value("TestRatioH").toString(),
            query.value("TestRatioM").toString(),
            query.value("TestRatioL").toString()
			));
	}

    QLOG_DEBUG() << "查询到记录数:" << entries.size();
    return entries;
}
//auto result = selectPETestEntry("PE2023_001");
//if (result.success) {
    // 使用 result.entry
//} else {
//    qDebug() << "错误信息:" << result.errorMessage;
//}



//仪器的设置参数和试管状态
void CustomCreatSql::CreatMachineTableData()
{
    const TableConfig configs[] = {
            {m_MachineTubeState,   [this]{Initial_configurationEmptyTube();}, "空试管孔状态"},
            {m_TestProjectTable,   nullptr,          "创建测试项目保存的数据表[完成]"},
            {m_SuppLiesTable,      nullptr,          "创建耗材管理参数数据表[完成]"},
            {m_UserTable,          [this]{AddUserName("admin","admin",INTENDANT_CONTROL_VIP);}, "admin**创建成功"},
            {m_ReferenceTable,     [this]{initReerenceTable();}, "创建聚集率表格[完成]"},
            {m_HospitalInfoTable,  nullptr,          "医院信息表和缩写[完成]"}
        };

	for (const auto& cfg : configs) {
		if (CreatParameterTable(SQL_MACHINE_SETTING, cfg.tableName, m_MachineConfigdb)) {
			if (cfg.initializer) {
				cfg.initializer();
			}
			// 修复日志输出
			QLOG_TRACE() << cfg.logMessage
				<< (cfg.tableName == m_UserTable
					? QString("File: %1, Line: %2").arg(__FILE__).arg(__LINE__)
					: "");
		}
	}
}

/**    ++样本的时候 插入一条曲线信息
 * @brief CustomCreatSql::insertCurveRecord
 * @param id
 * @return
 */
bool CustomCreatSql::insertCurveRecord(QString id)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }

    QSqlQuery sql_query(m_database);
    sql_query.prepare(INSERT_CURVEDATA);
    sql_query.addBindValue(id);
    sql_query.addBindValue(0);
    sql_query.addBindValue("");
    sql_query.addBindValue("");
    sql_query.addBindValue("");
    sql_query.addBindValue("");
    sql_query.addBindValue("");
    if(!sql_query.exec())
    {
        QLOG_ERROR()<< QString("Error: Fail to insert data: %1").arg(sql_query.lastError().driverText());
        return false;
    }
    sql_query.finish();
    sql_query.clear();
    return true;
}

// 删除曲线的数据点
bool CustomCreatSql::deleteTestCurveDataTale(QString delid)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }

    QString DELETE_CURVEDATA_BY_ID = QString("DELETE FROM CurveData WHERE 样本号 ='%1'").arg(delid);
    QSqlQuery sql_query(m_database);
    bool bscuess = sql_query.exec(DELETE_CURVEDATA_BY_ID);
    sql_query.finish();
    sql_query.clear();
    return bscuess;
}

//更新曲线的数据点
bool CustomCreatSql::updateTestCurveDataTale(QString id, quint8 indexReag,QString updateVal)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }
    QSqlQuery sql_query(m_database);
    QString UPDATE_CURVEDATA_VAL;
    switch(indexReag)
    {
        case AA_REAGENT:
            UPDATE_CURVEDATA_VAL = QString("UPDATE CurveData SET AA = '%1' WHERE 样本号 = '%2'").arg(updateVal).arg(id);
        break;
        case ADP_REAGENT:
            UPDATE_CURVEDATA_VAL = QString("UPDATE CurveData SET ADP = '%1' WHERE 样本号 = '%2'").arg(updateVal).arg(id);
        break;
        case EPI_REAGENT:
            UPDATE_CURVEDATA_VAL = QString("UPDATE CurveData SET EPI = '%1' WHERE 样本号 = '%2'").arg(updateVal).arg(id);
        break;
        case COL_REAGENT:
            UPDATE_CURVEDATA_VAL = QString("UPDATE CurveData SET COL = '%1' WHERE 样本号 = '%2'").arg(updateVal).arg(id);
        break;
        case RIS_REAGENT:
            UPDATE_CURVEDATA_VAL = QString("UPDATE CurveData SET RIS = '%1' WHERE 样本号 = '%2'").arg(updateVal).arg(id);
        break;
        default:
            UPDATE_CURVEDATA_VAL = QString("UPDATE CurveData SET 测试通道 = '%1' WHERE 样本号 = '%2'").arg(updateVal).arg(id);
        break;
    }
    if (!sql_query.exec(UPDATE_CURVEDATA_VAL))
    {
        QLOG_ERROR()<<QString("Error: Fail to update data: %1").arg(sql_query.lastError().driverText());
        sql_query.finish();
        sql_query.clear();
        return false;
    }
    sql_query.finish();
    sql_query.clear();
    return true;
}

//查曲线的数据&&输出
QVector<double> CustomCreatSql::getCurveData(QString id,quint8 indexReag)
{
    QVector<double> returnCurvadata;

    if(!connectMyDB(m_database,SQL_PATH))
    {
        return returnCurvadata;
    }
    QString valdata;
    QSqlQuery sql_query(m_database);
    QString  GET_CURVEDATA_VAL = QString("SELECT * FROM CurveData WHERE 样本号 = '%1'").arg(id);
    sql_query.exec(GET_CURVEDATA_VAL);
    while(sql_query.next())
    {
        switch(indexReag)
        {
            case AA_REAGENT:  valdata = sql_query.value("AA").toString();    break;
            case ADP_REAGENT: valdata = sql_query.value("ADP").toString();   break;
            case EPI_REAGENT: valdata = sql_query.value("EPI").toString();   break;
            case COL_REAGENT: valdata = sql_query.value("COL").toString();   break;
            case RIS_REAGENT: valdata = sql_query.value("RIS").toString();    break;
            default: break;
        }
    }
    valdata = valdata.simplified();
    QStringList valdataList = valdata.split(",");
    for(int k = 0; k < valdataList.size(); k++)
    {
        double data_ = strtod(valdataList[k].toLocal8Bit().data(),nullptr) *100.00;
        returnCurvadata.push_back(data_);
    }
    return returnCurvadata;
}


bool CustomCreatSql::getOneIDAllCurveData(const QString id,QMap<quint8,QString> &dataList)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }
    QSqlQuery sql_query(m_database);
    QString  GET_CURVEDATA_VAL = QString("SELECT * FROM CurveData WHERE 样本号 = '%1'").arg(id);
    sql_query.exec(GET_CURVEDATA_VAL);
    while(sql_query.next())
    {
        dataList.insert(AA_REAGENT,sql_query.value("AA").toString());
        dataList.insert(ADP_REAGENT,sql_query.value("ADP").toString());
        dataList.insert(EPI_REAGENT,sql_query.value("EPI").toString());
        dataList.insert(COL_REAGENT,sql_query.value("COL").toString());
        dataList.insert(RIS_REAGENT,sql_query.value("RIS").toString());
    }
    return true;
}



/**  ++样本的时候插入一行填充结果和基础信息
 * @brief CustomCreatSql::insertAddSampleData
 * @param id
 */
bool CustomCreatSql::insertAddSampleData(QString id, QString addTime, QString barcode)
{
    // 使用现有连接而不是每次重新连接
    if(!m_database.isOpen() && !connectMyDB(m_database, SQL_PATH)){
            return false;
    }

    QSqlQuery sql_query(m_database);

    // 明确指定插入字段，避免表结构变更导致错误
    const QString INSERT_SQL =
        "INSERT INTO ResultData (样本号,样本日期,样本编号,添加时间,条形编码号) "
        "VALUES (?, ?, ?, ?, ?)";

    sql_query.prepare(INSERT_SQL);
    sql_query.addBindValue(id);

    QString datestr;
    int sampleNum = 0;
    GlobalData::apartSampleId(id,datestr,sampleNum);
	QDate date = QDate::fromString(datestr, "yyyyMMdd"); // 指定原始格式
	QString formattedDate = date.toString("yyyy/MM/dd");
    sql_query.addBindValue(formattedDate);
    sql_query.addBindValue(sampleNum);

    sql_query.addBindValue(addTime);
    sql_query.addBindValue(barcode);

    if(!sql_query.exec()) {
        QLOG_ERROR() << "插入样本数据失败: "
                     << sql_query.lastError().text()
                     << " - SQL: " << INSERT_SQL;
        return false;
    }

    return true;
}

//删除结果和基础信息
bool CustomCreatSql::deleteTestResultTable(const QString &sampleId)
{
    if(!connectMyDB(m_database,SQL_PATH)){
        return false;
    }

    QString DELETE_TESTRESULTDATA_BY_ID = QString("DELETE FROM ResultData WHERE 样本号 ='%1'").arg(sampleId);
    QSqlQuery sql_query(m_database);
    bool bscuess = sql_query.exec(DELETE_TESTRESULTDATA_BY_ID);
    sql_query.finish();
    sql_query.clear();
    return bscuess;
}

bool CustomCreatSql::getOneDayTestResultData(const QString targetdate,
                                             QVector<QSharedPointer<PatientInformationStu>>& painterInfovec)
{
    if(!connectMyDB(m_database,SQL_PATH))   return false;

    const QString sql = R"(
            SELECT 添加时间,样本日期,样本编号, 年龄, 条形编码号, 床号, 科别, 临床诊断, 住院号, 检验时间,
                   检验医生, 送检医生, 备注, 审核医生, 姓名, 性别, 送检时间, 病区,
                   AA, ADP, EPI, COL, RIS, 样本号
            FROM ResultData
            WHERE SUBSTR(样本号, 1, 8) = :datePrefix
        )";

    QSqlQuery sql_query(m_database);
    sql_query.prepare(sql);
    sql_query.bindValue(":datePrefix", targetdate.left(8).replace("-", "")); // 假设样本号前缀为YYYYMMDD

    if (!sql_query.exec()) {
        QLOG_ERROR() << "查询失败:" << sql_query.lastError().text();
        return false;
    }

    const QSqlRecord rec = sql_query.record();
    const int idxSampleId = rec.indexOf("样本号");
    const int idxDateSample = rec.indexOf("样本日期");
    const int idSamplenNum = rec.indexOf("样本编号");
    const int idxAddsampleTime = rec.indexOf("添加时间");
    const int idxAge = rec.indexOf("年龄");
    const int idxBarcode = rec.indexOf("条形编码号");
    const int idxBedNumber = rec.indexOf("床号");
    const int idxDepartment = rec.indexOf("科别");
    const int idxDiagnosis = rec.indexOf("临床诊断");
    const int idxHospitalization = rec.indexOf("住院号");
    const int idxInspectionTime = rec.indexOf("检验时间");
    const int idxMedicalExaminer = rec.indexOf("检验医生");
    const int idxReferDoctor = rec.indexOf("送检医生");
    const int idxRemark = rec.indexOf("备注");
    const int idxReviewDoctors = rec.indexOf("审核医生");
    const int idxSampleName = rec.indexOf("姓名");
    const int idxSex = rec.indexOf("性别");
    const int idxSubmissionTime = rec.indexOf("送检时间");
    const int idxWardCode = rec.indexOf("病区");
    const int idxAA = rec.indexOf("AA");
    const int idxADP = rec.indexOf("ADP");
    const int idxEPI = rec.indexOf("EPI");
    const int idxCOL = rec.indexOf("COL");
    const int idxRIS = rec.indexOf("RIS");
    while (sql_query.next()) {
        auto patientInfo = QSharedPointer<PatientInformationStu>::create();

        patientInfo->sampleId = sql_query.value(idxSampleId).toString();
        patientInfo->DateSample = sql_query.value(idxDateSample).toString();
        patientInfo->SampleNum = sql_query.value(idSamplenNum).toInt();
        patientInfo->AddsampleTime = sql_query.value(idxAddsampleTime).toString();
        patientInfo->agesample = sql_query.value(idxAge).toInt();
        patientInfo->barcodesample = sql_query.value(idxBarcode).toString();
        patientInfo->bednumber_ = sql_query.value(idxBedNumber).toString();
        patientInfo->department_ = sql_query.value(idxDepartment).toString();
        patientInfo->diagnosis_ = sql_query.value(idxDiagnosis).toString();
        patientInfo->hospitalzationnumber_ = sql_query.value(idxHospitalization).toString();
        patientInfo->inspectiontime_ = sql_query.value(idxInspectionTime).toString();
        patientInfo->medicalexaminer_ = sql_query.value(idxMedicalExaminer).toString();
        patientInfo->refertithedoctor_ = sql_query.value(idxReferDoctor).toString();
        patientInfo->remark_ = sql_query.value(idxRemark).toString();
        patientInfo->reviewdoctors_ = sql_query.value(idxReviewDoctors).toString();
        patientInfo->SampleName = sql_query.value(idxSampleName).toString();
        patientInfo->sex_ = sql_query.value(idxSex).toString();
        patientInfo->submissiontime_ = sql_query.value(idxSubmissionTime).toString();
        patientInfo->wardcode_ = sql_query.value(idxWardCode).toString();
        patientInfo->AAResult = sql_query.value(idxAA).toString();
        patientInfo->ADPResult = sql_query.value(idxADP).toString();
        patientInfo->EPIResult = sql_query.value(idxEPI).toString();
        patientInfo->COLResult = sql_query.value(idxCOL).toString();
        patientInfo->RISResult = sql_query.value(idxRIS).toString();

        painterInfovec.append(patientInfo);
    }
    return true;
}

bool CustomCreatSql::getAllTestResultData(QVector<PatientInformationStu *> &painterInfovec)
{
    if(!connectMyDB(m_database,SQL_PATH)){
        return false;
    }

    const QString queryStr = "SELECT * FROM ResultData";
    QSqlQuery sql_query(m_database);

    if (!sql_query.exec(queryStr)) {
        QLOG_ERROR() << "查询失败:" << sql_query.lastError().text();
        return false;
    }

    const QSqlRecord rec = sql_query.record();
    const int colCount = rec.count();
    QVector<int> columnIndexes = {
        rec.indexOf("样本号"),    rec.indexOf("添加时间"),
        rec.indexOf("样本日期"),    rec.indexOf("样本编号"),
        rec.indexOf("年龄"),      rec.indexOf("条形编码号"),
        rec.indexOf("床号"),      rec.indexOf("科别"),
        rec.indexOf("临床诊断"),  rec.indexOf("住院号"),
        rec.indexOf("检验时间"),  rec.indexOf("检验医生"),
        rec.indexOf("送检医生"),  rec.indexOf("备注"),
        rec.indexOf("审核医生"),  rec.indexOf("姓名"),
        rec.indexOf("性别"),      rec.indexOf("送检时间"),
        rec.indexOf("病区"),      rec.indexOf("AA"),
        rec.indexOf("ADP"),      rec.indexOf("EPI"),
        rec.indexOf("COL"),      rec.indexOf("RIS")
    };

    if (std::any_of(columnIndexes.begin(), columnIndexes.end(), [](int idx) {
            return idx == -1;
    })) {
        QLOG_ERROR() << "检测到无效列索引，表结构可能已变更";
        return false;
    }

    painterInfovec.reserve(painterInfovec.size() + sql_query.size());

    while (sql_query.next()) {
        auto patientInfo = new PatientInformationStu();

        // 使用索引数组按顺序赋值
        patientInfo->sampleId = sql_query.value(columnIndexes[0]).toString();
        patientInfo->AddsampleTime = sql_query.value(columnIndexes[1]).toString();
        patientInfo->DateSample = sql_query.value(columnIndexes[2]).toString();
        patientInfo->SampleNum = sql_query.value(columnIndexes[3]).toInt();


        patientInfo->agesample = sql_query.value(columnIndexes[4]).toInt();
        patientInfo->barcodesample = sql_query.value(columnIndexes[5]).toString();
        patientInfo->bednumber_ = sql_query.value(columnIndexes[6]).toString();
        patientInfo->department_ = sql_query.value(columnIndexes[7]).toString();
        patientInfo->diagnosis_ = sql_query.value(columnIndexes[8]).toString();
        patientInfo->hospitalzationnumber_ = sql_query.value(columnIndexes[9]).toString();
        patientInfo->inspectiontime_ = sql_query.value(columnIndexes[10]).toString();
        patientInfo->medicalexaminer_ = sql_query.value(columnIndexes[11]).toString();
        patientInfo->refertithedoctor_ = sql_query.value(columnIndexes[12]).toString();
        patientInfo->remark_ = sql_query.value(columnIndexes[13]).toString();
        patientInfo->reviewdoctors_ = sql_query.value(columnIndexes[14]).toString();
        patientInfo->SampleName = sql_query.value(columnIndexes[15]).toString();
        patientInfo->sex_ = sql_query.value(columnIndexes[16]).toString();
        patientInfo->submissiontime_ = sql_query.value(columnIndexes[17]).toString();
        patientInfo->wardcode_ = sql_query.value(columnIndexes[18]).toString();
        patientInfo->AAResult = sql_query.value(columnIndexes[19]).toString();
        patientInfo->ADPResult = sql_query.value(columnIndexes[20]).toString();
        patientInfo->EPIResult = sql_query.value(columnIndexes[21]).toString();
        patientInfo->COLResult = sql_query.value(columnIndexes[22]).toString();
        patientInfo->RISResult = sql_query.value(columnIndexes[23]).toString();

        painterInfovec.push_back(patientInfo);
    }
    return true;
}



bool CustomCreatSql::getDesignateResultData(QList<QString> daysList,QVector<PatientInformationStu *> &painterInfovec)
{

	QString minDate = *std::min_element(daysList.begin(), daysList.end());
	QString maxDate = *std::max_element(daysList.begin(), daysList.end());
    QDate datemin = QDate::fromString(minDate, "yyyyMMdd"); // 指定原始格式
    QString formattedDateMin = datemin.toString("yyyy/MM/dd");
    QDate datemax = QDate::fromString(maxDate, "yyyyMMdd");
    QString formattedDateMax = datemax.toString("yyyy/MM/dd");

    if(!connectMyDB(m_database,SQL_PATH)) return false;

    QString queryStr = "SELECT * FROM ResultData WHERE 样本日期 BETWEEN ? AND ?";
	QSqlQuery sql_query(m_database);
	sql_query.prepare(queryStr);
    sql_query.addBindValue(formattedDateMin);
    sql_query.addBindValue(formattedDateMax);

	if (!sql_query.exec()) {
		QLOG_ERROR() << "查询失败:" << sql_query.lastError().text()
			<< "\nSQL:" << sql_query.lastQuery();
		return false;
	}

    const QSqlRecord rec = sql_query.record();
    const std::vector<int> columnIndexes = {
        rec.indexOf("样本号"),    rec.indexOf("添加时间"),
        rec.indexOf("样本日期"),    rec.indexOf("样本编号"),
        rec.indexOf("年龄"),      rec.indexOf("条形编码号"),
        rec.indexOf("床号"),      rec.indexOf("科别"),
        rec.indexOf("临床诊断"),  rec.indexOf("住院号"),
        rec.indexOf("检验时间"),  rec.indexOf("检验医生"),
        rec.indexOf("送检医生"),  rec.indexOf("备注"),
        rec.indexOf("审核医生"),  rec.indexOf("姓名"),
        rec.indexOf("性别"),      rec.indexOf("送检时间"),
        rec.indexOf("病区"),      rec.indexOf("AA"),
        rec.indexOf("ADP"),      rec.indexOf("EPI"),
        rec.indexOf("COL"),      rec.indexOf("RIS")
    };

    painterInfovec.reserve(sql_query.size());
    while(sql_query.next()) {
        auto info = std::make_unique<PatientInformationStu>();

        info->sampleId = sql_query.value(columnIndexes[0]).toString();
        info->AddsampleTime = sql_query.value(columnIndexes[1]).toString();
        info->DateSample = sql_query.value(columnIndexes[2]).toString();
        info->SampleNum = sql_query.value(columnIndexes[3]).toInt();

        info->agesample = sql_query.value(columnIndexes[4]).toInt();
        info->barcodesample = sql_query.value(columnIndexes[5]).toString();
        info->bednumber_ = sql_query.value(columnIndexes[6]).toString();
        info->department_ = sql_query.value(columnIndexes[7]).toString();
        info->diagnosis_ = sql_query.value(columnIndexes[8]).toString();
        info->hospitalzationnumber_ = sql_query.value(columnIndexes[9]).toString();
        info->inspectiontime_ = sql_query.value(columnIndexes[10]).toString();
        info->medicalexaminer_ = sql_query.value(columnIndexes[11]).toString();
        info->refertithedoctor_ = sql_query.value(columnIndexes[12]).toString();
        info->remark_ = sql_query.value(columnIndexes[13]).toString();
        info->reviewdoctors_ = sql_query.value(columnIndexes[14]).toString();
        info->SampleName = sql_query.value(columnIndexes[15]).toString();
        info->sex_ = sql_query.value(columnIndexes[16]).toString();
        info->submissiontime_ = sql_query.value(columnIndexes[17]).toString();
        info->wardcode_ = sql_query.value(columnIndexes[18]).toString();
        info->AAResult = sql_query.value(columnIndexes[19]).toString();
        info->ADPResult = sql_query.value(columnIndexes[20]).toString();
        info->EPIResult = sql_query.value(columnIndexes[21]).toString();
        info->COLResult = sql_query.value(columnIndexes[22]).toString();
        info->RISResult = sql_query.value(columnIndexes[23]).toString();

        painterInfovec.push_back(info.release());
    }

    return true;
}




bool CustomCreatSql::getDesignateTestResultData(QList<QString> keyList,QVector<PatientInformationStu *> &painterInfovec)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }

    QString GETSAMPLEBASICRESULT_DATA = QString("SELECT * FROM ResultData");
    QSqlQuery sql_query(m_database);
    if(!sql_query.exec(GETSAMPLEBASICRESULT_DATA))
    {
        QLOG_ERROR()<<"查找患者数据测试结果失败"<<endl;
        sql_query.clear();
        sql_query.finish();
        return false;
    }

    QSqlRecord rec;
    QString findDate;
    int findid;
    while(sql_query.next())
    {
        rec = sql_query.record();
        QString sampleid = sql_query.value(rec.indexOf("样本号")).toString();
		QString sendDoctor = sql_query.value(rec.indexOf("送检医生")).toString();
		QString names = sql_query.value(rec.indexOf("姓名")).toString();
		QString Ages = sql_query.value(rec.indexOf("年龄")).toString();
		QString  kebie = sql_query.value(rec.indexOf("科别")).toString();
		QString  bennum = sql_query.value(rec.indexOf("床号")).toString();
        GlobalData::apartSampleId(sampleid,findDate,findid);
        if(keyList.contains(QString::number(findid)) || keyList.contains(sendDoctor) ||keyList.contains(names) ||
           keyList.contains(Ages) ||keyList.contains(kebie) || keyList.contains(bennum) )
        {
            PatientInformationStu *PainterListInfo = new PatientInformationStu;
            PainterListInfo->AddsampleTime = sql_query.value(rec.indexOf("添加时间")).toString();
            PainterListInfo->agesample = sql_query.value(rec.indexOf("年龄")).toInt();
            PainterListInfo->barcodesample = sql_query.value(rec.indexOf("条形编码号")).toString();
            PainterListInfo->bednumber_ = sql_query.value(rec.indexOf("床号")).toString();
            PainterListInfo->department_ = sql_query.value(rec.indexOf("科别")).toString();
            PainterListInfo->diagnosis_ = sql_query.value(rec.indexOf("临床诊断")).toString();
            PainterListInfo->hospitalzationnumber_ = sql_query.value(rec.indexOf("住院号")).toString();
            PainterListInfo->inspectiontime_ = sql_query.value(rec.indexOf("检验时间")).toString();
            PainterListInfo->medicalexaminer_ = sql_query.value(rec.indexOf("检验医生")).toString();
            PainterListInfo->refertithedoctor_ = sql_query.value(rec.indexOf("送检医生")).toString();
            PainterListInfo->remark_ = sql_query.value(rec.indexOf("备注")).toString();
            PainterListInfo->reviewdoctors_ = sql_query.value(rec.indexOf("审核医生")).toString();
            PainterListInfo->SampleName = sql_query.value(rec.indexOf("姓名")).toString();
            PainterListInfo->sex_ = sql_query.value(rec.indexOf("性别")).toString();
            PainterListInfo->submissiontime_ = sql_query.value(rec.indexOf("送检时间")).toString();
            PainterListInfo->wardcode_ = sql_query.value(rec.indexOf("病区")).toString();
            PainterListInfo->AAResult = sql_query.value(rec.indexOf("AA")).toString();
            PainterListInfo->ADPResult = sql_query.value(rec.indexOf("ADP")).toString();
            PainterListInfo->EPIResult = sql_query.value(rec.indexOf("EPI")).toString();
            PainterListInfo->COLResult = sql_query.value(rec.indexOf("COL")).toString();
            PainterListInfo->RISResult = sql_query.value(rec.indexOf("RIS")).toString();
            PainterListInfo->sampleId = sampleid;
            painterInfovec.push_back(PainterListInfo);
        }
    }
    sql_query.clear();
    sql_query.finish();
    return true;
}




bool CustomCreatSql::getThisMonthResultData(QString MonthStr,QVector<PatientInformationStu *> &painterInfovec)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }

    QString GETSAMPLEBASICRESULT_DATA = QString("SELECT * FROM ResultData");
    QSqlQuery sql_query(m_database);
    if(!sql_query.exec(GETSAMPLEBASICRESULT_DATA))
    {
        QLOG_ERROR()<<"查找患者数据测试结果失败"<<endl;
        sql_query.clear();
        sql_query.finish();
        return false;
    }

    QSqlRecord rec;
    QString dateStr;
    int idnum;
    while(sql_query.next())
    {
        rec = sql_query.record();
        QString sampleid = sql_query.value(rec.indexOf("样本号")).toString();
        GlobalData::apartSampleId(sampleid,dateStr,idnum);
        QString findDate = dateStr.left(6);
        if(findDate == MonthStr)
        {
            PatientInformationStu *PainterListInfo = new PatientInformationStu;
            PainterListInfo->AddsampleTime = sql_query.value(rec.indexOf("添加时间")).toString();
            PainterListInfo->agesample = sql_query.value(rec.indexOf("年龄")).toInt();
            PainterListInfo->barcodesample = sql_query.value(rec.indexOf("条形编码号")).toString();
            PainterListInfo->bednumber_ = sql_query.value(rec.indexOf("床号")).toString();
            PainterListInfo->department_ = sql_query.value(rec.indexOf("科别")).toString();
            PainterListInfo->diagnosis_ = sql_query.value(rec.indexOf("临床诊断")).toString();
            PainterListInfo->hospitalzationnumber_ = sql_query.value(rec.indexOf("住院号")).toString();
            PainterListInfo->inspectiontime_ = sql_query.value(rec.indexOf("检验时间")).toString();
            PainterListInfo->medicalexaminer_ = sql_query.value(rec.indexOf("检验医生")).toString();
            PainterListInfo->refertithedoctor_ = sql_query.value(rec.indexOf("送检医生")).toString();
            PainterListInfo->remark_ = sql_query.value(rec.indexOf("备注")).toString();
            PainterListInfo->reviewdoctors_ = sql_query.value(rec.indexOf("审核医生")).toString();
            PainterListInfo->SampleName = sql_query.value(rec.indexOf("姓名")).toString();
            PainterListInfo->sex_ = sql_query.value(rec.indexOf("性别")).toString();
            PainterListInfo->submissiontime_ = sql_query.value(rec.indexOf("送检时间")).toString();
            PainterListInfo->wardcode_ = sql_query.value(rec.indexOf("病区")).toString();
            PainterListInfo->AAResult = sql_query.value(rec.indexOf("AA")).toString();
            PainterListInfo->ADPResult = sql_query.value(rec.indexOf("ADP")).toString();
            PainterListInfo->EPIResult = sql_query.value(rec.indexOf("EPI")).toString();
            PainterListInfo->COLResult = sql_query.value(rec.indexOf("COL")).toString();
            PainterListInfo->RISResult = sql_query.value(rec.indexOf("RIS")).toString();
            PainterListInfo->sampleId = sampleid;
            painterInfovec.push_back(PainterListInfo);
        }
    }
    sql_query.clear();
    sql_query.finish();
    return true;
}

//查询样本结果和基础信息
bool CustomCreatSql::getTestResultTabledata(QString id,PInfTable& PainterListInfo)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return false;
    }

    QString GETSAMPLEBASICRESULT_DATA = QString("SELECT * FROM ResultData WHERE 样本号 = '%1'").arg(id);
    QSqlQuery sql_query(m_database);
    if(sql_query.exec(GETSAMPLEBASICRESULT_DATA))
    {
        QSqlRecord rec;
        while(sql_query.next())
        {
            rec = sql_query.record();
            PainterListInfo.sampleId = sql_query.value(rec.indexOf("样本号")).toString();

            //QByteArray ba = sql_query.value(rec.indexOf("样本号")).toString().toLatin1(); // must
            //char *chdataid = ba.data();
            //strcpy(PainterListInfo->sample_id,chdataid);

            PainterListInfo.AddsampleTime = sql_query.value(rec.indexOf("添加时间")).toString();
            PainterListInfo.agesample = sql_query.value(rec.indexOf("年龄")).toInt();
            PainterListInfo.barcodesample = sql_query.value(rec.indexOf("条形编码号")).toString();
            PainterListInfo.bednumber_ = sql_query.value(rec.indexOf("床号")).toString();
            PainterListInfo.department_ = sql_query.value(rec.indexOf("科别")).toString();
            PainterListInfo.diagnosis_ = sql_query.value(rec.indexOf("临床诊断")).toString();
            PainterListInfo.hospitalzationnumber_ = sql_query.value(rec.indexOf("住院号")).toString();
            PainterListInfo.inspectiontime_ = sql_query.value(rec.indexOf("检验时间")).toString();
            PainterListInfo.medicalexaminer_ = sql_query.value(rec.indexOf("检验医生")).toString();
            PainterListInfo.refertithedoctor_ = sql_query.value(rec.indexOf("送检医生")).toString();
            PainterListInfo.remark_ = sql_query.value(rec.indexOf("备注")).toString();
            PainterListInfo.reviewdoctors_ = sql_query.value(rec.indexOf("审核医生")).toString();
            PainterListInfo.SampleName = sql_query.value(rec.indexOf("姓名")).toString();

            PainterListInfo.sex_ = sql_query.value(rec.indexOf("性别")).toString();
            PainterListInfo.submissiontime_ = sql_query.value(rec.indexOf("送检时间")).toString();
            PainterListInfo.wardcode_ = sql_query.value(rec.indexOf("病区")).toString();
            PainterListInfo.AAResult = sql_query.value(rec.indexOf("AA")).toString();
            PainterListInfo.ADPResult = sql_query.value(rec.indexOf("ADP")).toString();
            PainterListInfo.EPIResult = sql_query.value(rec.indexOf("EPI")).toString();
            PainterListInfo.COLResult = sql_query.value(rec.indexOf("COL")).toString();
            PainterListInfo.RISResult = sql_query.value(rec.indexOf("RIS")).toString();
        }
    }
    sql_query.clear();
    sql_query.finish();
    return true;
}


//更新结果和基础信息
bool CustomCreatSql::updateTestResultTable(QString id,QString updateKey,QString val)
{
    if(!connectMyDB(m_database,SQL_PATH)) return false;


    QString sql = QString("UPDATE ResultData SET %1 = ? WHERE 样本号 = ?").arg(updateKey);
    QSqlQuery sql_query(m_database);
    sql_query.prepare(sql);
    sql_query.addBindValue(val.isEmpty() ? QVariant() : val); // 自动处理NULL
    sql_query.addBindValue(id);

    if (!sql_query.exec()) {
        QLOG_ERROR() << "更新失败：" << sql_query.lastError().text() << " SQL:" << sql;
        return false;
    }
    return true;
}




//找单天最大的样本号
int CustomCreatSql::getMaxSampleID(const QString datetoday)
{
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return 0;
    }
    QVector<int> idVec;
    int biggestId = 0;
    QString GETSAMPLEBASICRESULT_ID = QString("SELECT  *FROM ResultData");
    QSqlQuery sql_query(m_database);
    if(sql_query.exec(GETSAMPLEBASICRESULT_ID))
    {
        QSqlRecord rec;
        QString Finddate;
        int findid;
        while(sql_query.next())
        {
            rec = sql_query.record();
            QString id = sql_query.value(rec.indexOf("样本号")).toString();
            GlobalData::apartSampleId(id,Finddate,findid);
            if(datetoday == Finddate)
                idVec.push_back(findid);
        }
    }
    if(idVec.size() != 0){
        auto max = std::max_element(std::begin(idVec), std::end(idVec));
        biggestId = *max;
    }
    sql_query.clear();
    sql_query.finish();
    return biggestId;
}

QVector<int> CustomCreatSql::getTodayAllSampleid(const QString datetoday)
{
	QVector<int> outtodaySample;
	outtodaySample.clear();
    if(!connectMyDB(m_database,SQL_PATH))
    {
        return outtodaySample;
    }

    QString GETSAMPLEBASICRESULT_ID = QString("SELECT *FROM ResultData");
    QSqlQuery sql_query(m_database);
    if(sql_query.exec(GETSAMPLEBASICRESULT_ID))
    {
        QSqlRecord rec;
        QString Finddate;
        int findid;
        while(sql_query.next())
        {
            rec = sql_query.record();
            QString id = sql_query.value(rec.indexOf("样本号")).toString();
            GlobalData::apartSampleId(id,Finddate,findid);
            if(datetoday == Finddate)
                outtodaySample.push_back(findid);
        }
    }
    return outtodaySample;
}



