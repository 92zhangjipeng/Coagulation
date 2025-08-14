#ifndef ANALYTICALJSON_H
#define ANALYTICALJSON_H

#include <QObject>
#include <QWidget>
#include <QJsonDocument>
#include <QFile>

#define  JSON_FILE_LOAD "SetConfigure.json"
#define  MACHINE_CONFIG "Machine_configure_parameter.json"
#define  PROJECT_FILE   "TestingGroup.json"
#define  TUBE_USED      "Tube_Already_used.json"

class AnalyticalJson
{
public:
    AnalyticalJson();
    /*解析json文件*/
    void AnalyticalJsonFile(QString  root,  QString objName, QVariant &data, QString FileNameLoad);
    void AnalyticalJsonFile(QString  root,  QString objName, QVariantList&data,QString filename);
public slots:
    QString JsonFilePath(int);
private slots:
    bool isFileExist(QString fullfilepath);
private:
	QFile m_JsonFilePath;
    QFile m_Machine_configure_parameter; //机器配置参数
	QByteArray  m_Jsonfile;
	QString m_currentPath;
    QString m_Machine_str;

};

#endif // ANALYTICALJSON_H
