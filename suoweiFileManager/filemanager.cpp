#pragma execution_character_set("utf-8")
#include "filemanager.h"

FileManager::FileManager(QObject *parent) : QObject(parent)
{
    initializeAppPath();
}

void FileManager::initializeAppPath()
{
    // 获取应用可执行文件所在目录
    m_appPath = QCoreApplication::applicationDirPath();

    // 如果是开发环境（在build目录中），可能需要调整到项目目录
#ifdef QT_DEBUG
    QLOG_DEBUG() << "应用路径:" << m_appPath;

    // 如果路径包含build，可能向上级目录查找
    if (m_appPath.contains("build", Qt::CaseInsensitive)) {
        QDir dir(m_appPath);
        dir.cdUp(); // 向上级目录
        m_appPath = dir.absolutePath();
        QLOG_DEBUG() << "调试模式，调整路径为:" << m_appPath;
    }
#endif
}

bool FileManager::createDataFile(const QString &folderName, const QString &fileName, const QString &initialData)
{
    // 验证输入参数
    if (folderName.isEmpty() || fileName.isEmpty()) {
        emit errorOccurred("文件夹名或文件名不能为空");
        return false;
    }

    // 创建文件夹路径
    QString folderPath = m_appPath + "/" + folderName;
    QDir dir(folderPath);

    // 如果文件夹不存在，则创建
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            QString error = QString("无法创建文件夹: %1").arg(folderPath);
            emit errorOccurred(error);
            return false;
        }
        QLOG_DEBUG() << "文件夹创建成功:" << folderPath;
    }

    // 创建文件完整路径
    QString filePath = folderPath + "/" + fileName;
    QFile file(filePath);

    // 如果文件不存在，则创建并写入初始数据
    if (!file.exists()) {
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QString error = QString("无法创建文件: %1").arg(filePath);
            emit errorOccurred(error);
            return false;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8"); // 设置编码为UTF-8

        // 写入初始数据（如果有）
        if (!initialData.isEmpty()) {
            out << initialData;
        }

        file.close();
        QLOG_DEBUG() << "文件创建成功:" << filePath;
    } else {
        QLOG_DEBUG() << "文件已存在:" << filePath;
    }

    return true;
}

QString FileManager::getFilePath(const QString &folderName, const QString &fileName)
{
    return m_appPath + "/" + folderName + "/" + fileName;
}

bool FileManager::appendToFile(const QString &folderName, const QString &fileName, const QString &data)
{
    if (data.isEmpty()) {
        emit errorOccurred("要写入的数据不能为空");
        return false;
    }

    QString filePath = getFilePath(folderName, fileName);
    QFile file(filePath);

    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QString error = QString("无法打开文件进行追加: %1").arg(filePath);
        emit errorOccurred(error);
        return false;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << data << "\n"; // 追加数据并换行
    file.close();

    return true;
}

QString FileManager::readFromFile(const QString &folderName, const QString &fileName)
{
    QString filePath = getFilePath(folderName, fileName);
    QFile file(filePath);

    if (!file.exists()) {
        emit errorOccurred(QString("文件不存在: %1").arg(filePath));
        return "";
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(QString("无法打开文件进行读取: %1").arg(filePath));
        return "";
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString content = in.readAll();
    file.close();

    return content;
}

bool FileManager::exists(const QString &folderName, const QString &fileName)
{
    QString filePath = getFilePath(folderName, fileName);
    QFile file(filePath);
    return file.exists();
}
