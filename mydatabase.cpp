#include "mydatabase.h"
#include <QApplication>
#include <QDebug>
#include "QsLog/include/QsLog.h"

//#pragma execution_character_set("utf-8")

MyDataBase * MyDataBase::m_pInstance(NULL);
ParamListVec paramListVec;

MyDataBase::MyDataBase()
{

}
MyDataBase::~MyDataBase()
{
    qDeleteAll(paramListVec.begin(), paramListVec.end());
    paramListVec.clear();
    this->GetDatabase().close();
}
MyDataBase * MyDataBase::GetInstance()
{
    if(NULL == m_pInstance)
    {
        static QMutex mutex;
        mutex.lock();
        if( NULL == m_pInstance)
        {
            static MyDataBase myDB;
            m_pInstance = &myDB;
        }
        mutex.unlock();
    }

    return m_pInstance;
}
bool MyDataBase::ConnectAccessDB(const QString &strDBName, const QString &strUser, const QString &strPwd) const
{
    QSqlDatabase db;
    //QLOG_ERROR() << QSqlDatabase::drivers();
    if(QSqlDatabase::contains("MyAccessDB"))
    {
        db = QSqlDatabase::database("MyAccessDB",false);
    }
    else
    {
        db = QSqlDatabase::addDatabase("QODBC", "MyAccessDB");
    }

    const QString strName(QString("DRIVER={Microsoft Access Driver (*.mdb,)};FIL={MS Access};DBQ=%1;Uid=%2;Pwd=%3")
                      .arg(strDBName)
                      .arg(strUser)
                      .arg(strPwd));
    db.setDatabaseName(strName);
    if (!db.isValid())
    {
        return false;
    }

    if (db.isOpen())
    {
        return true;
    }

    if (db.open())
    {
        return true;
    }
    else
    {
        QLOG_ERROR() << db.lastError().text();
        return false;
    }
}


void MyDataBase::GetDataOfDBParameter()
{
    QSqlDatabase db = this->GetDatabase();
    if(!db.isOpen())
    {
        return;
    }
    QStringList tables;
    QString tabName,sqlString;
    tables = db.tables(QSql::Tables);
    paramListVec.clear();
    for (int i = 0; i < tables.size(); ++i)//读表中记录
    {
        tabName = tables.at(i);//表名
        if(tabName != "TestTable"){
            continue ;
        }
        sqlString = "select * from " + tabName;
        QSqlQuery q(sqlString,db);
        QSqlRecord rec = q.record();
        int fieldCount = rec.count();//表列数
        QString fieldName;
        for(int j = 0; j < fieldCount; j++) //列的名字
        {
            fieldName = rec.fieldName(j);
        }
        while(q.next())//每一行的数据
        {
            CDBPARAMETER* data = new CDBPARAMETER;
            data->id = q.value("ID").toString();
            data->project = q.value("RgCode").toString();
            data->maxvalue = q.value("Rate_Max").toDouble();
            paramListVec.push_back(data);
            QLOG_DEBUG()<<"id"<<data->id<<"项目"<<data->project<<"Max"<<data->maxvalue<<endl;
        }
    }
}

bool MyDataBase::WriteTestFile(const QString filePath)
{
    bool bscuss = false;
    QFile file;
    if(QString::compare(file.fileName(),filePath) != 0)
    {
        file.setFileName(filePath);
    }
    if(file.exists() && file.isOpen()){
        return bscuss;
    }
    if(file.open(QIODevice::Text | QIODevice::ReadWrite /*| QIODevice::Append*/))
    {
        QTextStream out(&file);
        auto it = paramListVec.begin();
        while(it != paramListVec.end())
        {
            CDBPARAMETER* p_para = *it;
            QString pwrite = QString("ID:%1   Pro:%2   MaxValue:%3\r\n").arg(p_para->id).arg(p_para->project).arg(p_para->maxvalue);
            out<< pwrite;
            it++;
        }
        bscuss = true;
    }
    return bscuss;
}
quint8 mapping(QString pro)
{
    quint8 indexpro = 0;
    if(pro == "AA")
        indexpro = 1;
    else if(pro == "ADP")
    {
        indexpro = 2;
    }
    else if(pro == "ADR")
    {
        indexpro = 3;
    }
    else if(pro == "COL")
    {
        indexpro = 4;
    }
    else if(pro == "RIS")
    {
        indexpro = 5;
    }
    return indexpro;
}

void MyDataBase::ReadFileConfigValue()
{
    qDeleteAll(paramListVec.begin(), paramListVec.end());
    paramListVec.clear();

    QString filePathw = QApplication::applicationDirPath() + "/dbdata.txt";
    QFile file(filePathw);
    bool isok = file.open(QIODevice::ReadOnly); //只读模式打开
    if(isok == true)
    {
        QByteArray array;
        while(file.atEnd() == false)
        {
            array = file.readLine();
            CDBPARAMETER* pdatastu = new CDBPARAMETER;
            QByteArrayList linedata = array.split('|');
            int index = 0;
            for (QByteArray  child : linedata)
            {
                QByteArrayList data = child.split(':');
                if (data.size() == 2)
                {
                    QByteArray getvalue = data.at(1);
                    QString bytes = getvalue;
                    bytes = bytes.simplified();
                    switch(index)
                    {
                        case 0: pdatastu->id = bytes;       break;
                        case 1: pdatastu->project = bytes;  break;
                        case 2: pdatastu->maxvalue = bytes.toDouble(); break;
                        case 3: pdatastu->sixtyValue = bytes.toDouble(); break;
                        case 4: pdatastu->OneHandEightValue = bytes.toDouble(); break;
                        case 5: pdatastu->ThreeHandValue = bytes.toDouble(); break;
                        case 6: pdatastu->maxtime = bytes.toInt(); break;
                    default: break;
                    }
                }
                index++;
            }
            //QLOG_DEBUG()<<"id"<< pdatastu->id<<"project:"<< pdatastu->project<<"Max:"<< pdatastu->maxvalue<<endl;
           // QLOG_DEBUG()<<"60s"<< pdatastu->sixtyValue<<"180s:"<< pdatastu->OneHandEightValue<<"300s:"<< pdatastu->ThreeHandValue
                      // <<"maxtime:"<<pdatastu->maxtime<<endl;
            paramListVec.push_back(pdatastu);
        }
    }
    file.close();
}

float MyDataBase::readDBData(QString Sample,quint8 IndexReagent,quint8 witchValue)
{
    //2023/06/28-2
    float MaxValueget = 0.00;
    auto it = paramListVec.begin();
    while(it != paramListVec.end())
    {
        CDBPARAMETER* p_para = *it;
        int index = p_para->id.right(4).toInt();
        int index_date = p_para->id.left(8).toInt();
        int findindex = QString(Sample.split("-").at(1)).toInt() - 100;
        QString finddate =  QString(Sample.split("-").at(0));
        QString valuedate;
        for(int j = 0; j < finddate.length(); j++)
        {
            if(finddate[j] >= '0' && finddate<= '9')
            valuedate.append(finddate[j]);
        }
        int finddate_ = valuedate.toInt();
        if(index == findindex && index_date == finddate_)
        {
            quint8 indexRegt = mapping(p_para->project);
            if(IndexReagent == indexRegt)
            {
                switch(witchValue)
                {
                    case 0: MaxValueget = p_para->maxvalue; break;
                    case 1: MaxValueget = p_para->sixtyValue; break;
                    case 2: MaxValueget = p_para->OneHandEightValue; break;
                    case 3: MaxValueget = p_para->ThreeHandValue; break;
                    case 4: MaxValueget = p_para->maxtime; break;
                default:
                    break;
                }
            }
        }
        it++;
    }
    return MaxValueget;
}
