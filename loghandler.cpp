#include "loghandler.h"


//初始化 static 变量
QMutex LogHandlerPrivate::logMutex;
QFile *LogHandlerPrivate::logfile = nullptr;
QTextStream *LogHandlerPrivate::logOut = nullptr;

LogHandlerPrivate::LogHandlerPrivate()
{
	logDir.setPath("log");
	QString logPath = logDir.absoluteFilePath("log.txt");//获取日志的路径

    // 日志文件创建的时间
    // QFileInfo::created(): On most Unix systems, this function returns the time of the last status change.
     // 所以不能运行时使用这个函数检查创建时间，因为会在运行时变化，于是在程序启动时保存下日志文件的最后修改时间，
    // 在后面判断如果不是今天则用于重命名 log.txt
    // 如果是 Qt 5.10 后，lastModified() 可以使用 birthTime() 代替

	logfilecreatedData = QFileInfo(logPath).lastModified().date();

    //打开日志文件，如果不是当天创建的，备份已有日志文件
    openAndBackupLogFile();

    //十分钟检查一次日志文件创建时间
    renameLogFileTimer.setInterval(1000 * 60* 10);
    renameLogFileTimer.start();
    QObject::connect(&renameLogFileTimer,&QTimer::timeout,[this]
                    {QMutexLocker locker(&LogHandlerPrivate::logMutex);
                    openAndBackupLogFile();//打开日志
    });

    //定时刷新日志到文件，尽快的能在日志文件里看到最新的日志
    flushLogFileTimer.setInterval(1000);
    flushLogFileTimer.start();
    QObject::connect(&flushLogFileTimer,&QTimer::timeout,[]{
                     QMutexLocker locker(&LogHandlerPrivate::logMutex);
                     if(nullptr != logOut){
                         logOut->flush();
                     }
    });
}
LogHandlerPrivate::~LogHandlerPrivate()
{
    if(nullptr != logfile)
    {
        logfile->flush();

        logfile->close();

        delete logOut;

        delete logfile;

        //因为是静态变量
        logOut = nullptr;

        logfile = nullptr;
    }
}

void LogHandlerPrivate::openAndBackupLogFile()
{
   //逻辑
   //1.程序启动 logfile 为NULLPTR,初始化logfile,有可能是同一天打开已经存在的logfile所以用APPend模式
   //2.logfileCreatDate is nullptr,说明日志文件在程序开始不存在，所以记录下创建时间
   //3.程序运行时检查如果logfile的创建日期和当前日期不相等，则使用它的创建日期重命名，然后再生成一个新的文件
   //4.检查日志文件超过 LOGLIMT_NUM个，删除最早的

    //2如果日志所在目录不存在，则创建
    makeSureLogDirectory();

    QString logPath = logDir.absoluteFilePath("log.txt");

    if(logfile == nullptr)
    {
        logfile = new QFile(logPath);

        logOut = (logfile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append )) ? new QTextStream(logfile) :NULL;

        if(logOut != nullptr)
            logOut->setCodec("UTF-8");

        //2如果文件第一次创建，则创建日期无效，把当前日期设置
        if(logfilecreatedData.isNull())
        {
            logfilecreatedData = QDate::currentDate();
        }
    }
	//3.程序启动如果创建日期不是当前日期，则使用创建日期重命名，&生成一个新的log.txt
	if(logfilecreatedData != QDate::currentDate())
    {
        logfile->flush();

        logfile->close();

        delete logOut;

        delete logfile;

        QString newLogPath = logDir.absoluteFilePath(logfilecreatedData.toString("yyyy-MM-dd.log"));

        QFile::copy(logPath,newLogPath); //  source  --> dest

        QFile::remove(logPath);

        //重新创建log.txt
        logfile =  new QFile(logPath);

        logOut = (logfile->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) ? new QTextStream(logfile) : nullptr;

        logfilecreatedData = QDate::currentDate();

        if(logOut != nullptr)
            logOut->setCodec("UTF-8");
    }
}
void LogHandlerPrivate::makeSureLogDirectory() const{
    if (!logDir.exists()) {
            logDir.mkpath(".");  // 可以递归的创建文件夹
        }
}

void LogHandlerPrivate::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   QMutexLocker locker(&LogHandlerPrivate::logMutex);

   QString level;
 
   switch(type){
   case QtDebugMsg:
        level ="DEBUG"; //调试信息
        break;
   case QtInfoMsg:
        level = "INFO";//操作消息
        break;
   case QtWarningMsg:
        level = "WARN";//系统提示
        break;
   case QtCriticalMsg:
        level = "ERROR";//操作错误
        break;
   case QtFatalMsg:
        level = "FATAL"; //致命错误
        break;
   default:
        break;
   }

#if defined(Q_OS_WIN)
    QByteArray localMsg = QTextCodec::codecForName("GB2312")->fromUnicode(msg);
#else
   QByteArray localMsg = msg.toLocal8Bit();

#endif
    std::cout<<std::string(localMsg) <<std::endl;

    if(nullptr == LogHandlerPrivate::logOut)
        return;

    //输出到日志文件
    QString filename = context.file;

    int index = filename.lastIndexOf(QDir::separator());

    filename = filename.mid(index + 1);

//    (*LogHandlerPrivate::logOut)<<QString("%1 - [%2](%3:%4, %5)==> %6\n")
//                                  .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
//                                  .arg(level)
//                                  .arg(filename)
//                                  .arg(context.line)
//                                  .arg(context.function)
//                                  .arg(msg);
    (*LogHandlerPrivate::logOut)<<QString("%2[%1]: %3\n")
                                    .arg(level)
                                    .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                                    .arg(msg);


}
/*****************************************************************************************
 *                                                                                       *
 *                                 logHandler                                            *
 *                                                                                       *
 *****************************************************************************************/
LogHandler::LogHandler(): d(nullptr)
{

}
void LogHandler::installMessageHandel()
{
    QMutexLocker locker(&LogHandlerPrivate::logMutex);
    if(nullptr == d){
        d = new LogHandlerPrivate();
        qInstallMessageHandler(LogHandlerPrivate::messageHandler);
    }
}

void LogHandler::uninstallMessageHandeler()
{
    QMutexLocker locker(&LogHandlerPrivate::logMutex);

    qInstallMessageHandler(nullptr);

    delete d;

    d = nullptr;
}





