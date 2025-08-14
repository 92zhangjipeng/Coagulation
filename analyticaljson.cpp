#pragma execution_character_set("utf-8")

#include "analyticaljson.h"
#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>

AnalyticalJson::AnalyticalJson()
{
   m_currentPath = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(JSON_FILE_LOAD);
   m_JsonFilePath.setFileName(m_currentPath);

   m_Machine_str = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(MACHINE_CONFIG);
   m_Machine_configure_parameter.setFileName(m_Machine_str);

}
void AnalyticalJson::AnalyticalJsonFile(QString  root, QString objName, QVariant &data,QString FileNameLoad)
{
    bool FileExist = isFileExist(FileNameLoad);
    if(FileExist == true)
    {
        m_JsonFilePath.setFileName(FileNameLoad);
        if (!m_JsonFilePath.open(QIODevice::ReadOnly))
        {
            qDebug() << "json File Open Failed.";
            return;
        }
        m_Jsonfile = m_JsonFilePath.readAll();
        QJsonParseError jsonError;
        QJsonDocument doucment = QJsonDocument::fromJson(m_Jsonfile, &jsonError);
        if (jsonError.error != QJsonParseError::NoError || doucment.isNull())
        {
            //qDebug()<<"Json Parse Failed";
            return;
        }
        if (doucment.isObject())
        {
            QJsonObject obj = doucment.object();
            if (obj.contains(root))
            {
                QJsonValue value = obj.value(root);
                if (value.isObject())
                {
                    QJsonObject obj_0 = value.toObject();
                    if (obj_0.contains(objName))
                    {
                        QJsonValue value_0 = obj_0.value(objName);
                        if (value_0.isArray())
                        {
                            QJsonArray arry_0 = value_0.toArray();
                            int nSize = arry_0.size();
                            for (int i = 0; i<nSize; i++)
                            {
                                QJsonValue value = arry_0.at(i);
                                data = value;
                            }
                        }
                        else if (value_0.isDouble())
                        {
                            double value = value_0.toDouble();
                            data = value;
                        }
                        else if (value_0.isBool())
                        {
                            bool bvalue = value_0.toBool();
                            data = bvalue;
                        }
                        else if (value_0.isString())
                        {
                            QString Svalue = value_0.toString();
                            data = Svalue;
                        }
                    }
                }
            }
        }
        m_JsonFilePath.close();
    }
}

void AnalyticalJson::AnalyticalJsonFile(QString root,QString objName,QVariantList &data,QString filename)
{
    bool FileExist = isFileExist(filename);
    if(FileExist == true)
    {
        m_JsonFilePath.setFileName(filename);
        if(!m_JsonFilePath.open(QIODevice::ReadOnly))
        {
            qDebug()<<"json文件打开失败";
            return;
        }
        m_Jsonfile = m_JsonFilePath.readAll();
        QJsonParseError jsonError;
        QJsonDocument doucment = QJsonDocument::fromJson(m_Jsonfile,&jsonError);
        if(jsonError.error != QJsonParseError::NoError || doucment.isNull())
        {
            qDebug()<<"Json Parse Failed";
            return;
        }
        if(doucment.isObject())
        {
            QJsonObject obj = doucment.object();
            if(obj.contains(root))
            {
                QJsonValue value =  obj.value(root);
                if(value.isObject())
                {
                    QJsonObject obj_0 = value.toObject();
                    if(obj_0.contains(objName))
                    {
                        QJsonValue value_0 = obj_0.value(objName);
                        if(value_0.isArray())
                        {
                            QJsonArray arry_0 = value_0.toArray();
                            int nSize = arry_0.size();
                            for(int i =0;i<nSize;i++)
                            {
                                QJsonValue value = arry_0.at(i);
                                data.append(value);
                            }
                        }
                    }
                }
            }
        }
        m_JsonFilePath.close();
    }
}

bool AnalyticalJson::isFileExist(QString fullfilepath)
{
    QFileInfo fileinfo(fullfilepath);
    if(fileinfo.isFile())
        return true;
    return false;
}
QString AnalyticalJson::JsonFilePath(int path)
{
    if(path == 0)
        m_Machine_str = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(MACHINE_CONFIG);
    else if(path == 1)
        m_Machine_str = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg("患者信息配置.json");
    else if(path == 2)
        m_Machine_str = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(PROJECT_FILE);
    else if(path == 3)
        m_Machine_str = QString("%1/%2").arg(QCoreApplication::applicationDirPath()).arg(TUBE_USED);
    return m_Machine_str;
}
