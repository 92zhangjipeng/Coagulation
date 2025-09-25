#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QsLog/include/QsLog.h>
#include <QStandardPaths>
#include <QCoreApplication>

class FileManager : public QObject
{
    Q_OBJECT
public:
    explicit FileManager(QObject *parent = nullptr);

/**
    * @brief 在应用目录创建文件夹和文件
    * @param folderName 文件夹名称
    * @param fileName 文件名称
    * @param initialData 初始数据（可选）
    * @return 成功返回true，失败返回false
    */
   bool createDataFile(const QString &folderName, const QString &fileName, const QString &initialData = "");

   /**
    * @brief 获取文件的完整路径
    * @param folderName 文件夹名称
    * @param fileName 文件名称
    * @return 文件的完整路径
    */
   QString getFilePath(const QString &folderName, const QString &fileName);

   /**
    * @brief 向文件追加数据
    * @param folderName 文件夹名称
    * @param fileName 文件名称
    * @param data 要追加的数据
    * @return 成功返回true，失败返回false
    */
   bool appendToFile(const QString &folderName, const QString &fileName, const QString &data);

   /**
    * @brief 从文件读取数据
    * @param folderName 文件夹名称
    * @param fileName 文件名称
    * @return 文件内容，如果失败返回空字符串
    */
   QString readFromFile(const QString &folderName, const QString &fileName);

   /**
    * @brief 检查文件夹和文件是否存在
    * @param folderName 文件夹名称
    * @param fileName 文件名称
    * @return 存在返回true，不存在返回false
    */
   bool exists(const QString &folderName, const QString &fileName);

signals:
    void errorOccurred(const QString &errorMessage);

private:
   QString m_appPath;  // 应用路径

   /**
    * @brief 初始化应用路径
    */
   void initializeAppPath();
};

#endif // FILEMANAGER_H
