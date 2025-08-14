#include <QDebug>
#include <QFile>
#include <QIODevice>
#include "json.h"
#include <QObject>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

JSON::JSON()
{

}
QJsonObject JSON::getJson()
{
    return json;
}
QJsonObject JSON::loadJson(const QString& filepath)
{
    QFile loadFile(filepath);
    if (!loadFile.open(QIODevice::ReadOnly)){
        QString error = QString("%1====%2").arg(QObject::tr("打开Json文件失败!")).arg(filepath);
        qWarning()<<error;
    }
    QByteArray allData = loadFile.readAll();
    loadFile.close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData, &json_error));

    if (json_error.error != QJsonParseError::NoError)
    {
        QString error = QString("%1====%2").arg(QObject::tr("Json文件错误!")).arg(filepath);
        qWarning()<<error;
    }
    QJsonObject rootObj = jsonDoc.object();
    return rootObj;
}
// NOTE: implicit conversion turns string literal into bool
void JSON::writeJson(const QString key, bool value)
{
    json.insert(key, value);
    QString info = QString("插入键:%1 插入值:%2(布尔)").arg(key).arg(value);
    qInfo()<<info;
}
void JSON::writeJson(const QString key, int value)
{
    json.insert(key, value);
    QString info = QString("插入键:%1 插入值:%2(整形)").arg(key).arg(value);
    qInfo()<<info;
}
void JSON::writeJson(const QString key, double value)
{
    json.insert(key, value);
    QString info = QString("插入键:%1 插入值:%2(双整形)").arg(key).arg(value);
    qInfo()<<info;
}
// value only support QString
void JSON::writeJson(const QString key, QString value)
{
    json.insert(key, QString(value));
    QString info = QString("插入键:%1 插入值:%2(字符串)").arg(key).arg(value);
    qInfo()<<info;
}
void JSON::writeJson(const QString key, bool* array, int length)
{
    QJsonArray arr;
    for (int i = 0; i < length; i++)
        arr.append(array[i]);
    json.insert(key, arr);
    qInfo()<<QObject::tr("插入键:")<<key<<QObject::tr("插入值(数组--布尔):")<<arr;
}
void JSON::writeJson(const QString key, int* array, int length)
{
    QJsonArray arr;
    for (int i = 0; i < length; i++)
        arr.append(array[i]);
    json.insert(key, arr);
    qInfo()<<QObject::tr("插入键:")<<key<<QObject::tr("插入值(数组--整型):")<<arr;
}
void JSON::writeJson(const QString key, double* array, int length)
{
    QJsonArray arr;
    for (int i = 0; i < length; i++)
        arr.append(array[i]);
    json.insert(key, arr);
    qInfo()<<QObject::tr("插入键:")<<key<<QObject::tr("插入值(数组--双精度整型):")<<arr;
}
void JSON::writeJson(const QString key, QJsonObject object)
{
    json.insert(key, object);
    qInfo()<<QObject::tr("插入键:")<<key<<QObject::tr("插入值(数组--对象):")<<object;
}

bool JSON::saveJson(const QString& filepath)
{
    QJsonDocument document;
    document.setObject(json);
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly )) {
        QString error = QString("%1====%2").arg(QObject::tr("保存Json文件失败!")).arg(filepath);
        qWarning()<<error;
        return false;
    }
    file.write(document.toJson());
    return true;
}

QString JSON::toString()
{
    QJsonDocument document;
    document.setObject(json);
    QByteArray byteArray = document.toJson(QJsonDocument::Compact);
    QString str(byteArray);
    return str;
}
