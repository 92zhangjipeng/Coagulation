#pragma execution_character_set("utf-8")

#include "QsLog/include/QsLog.h"
#include "QsLog/include/QsLogDest.h"
#include "QsLog/include/QsLogLevel.h"
#include "dilag/loginmaininterface.h"
#include <loginui.h>
#include <QApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include <new>
#include "mainwindow.h"
#include "operclass/fullyautomatedplatelets.h"
#include <cstdlib>
#include "Lib\VLD_SDK\vld.h"


#ifdef Q_OS_WIN
#include <Windows.h>
#include <dbghelp.h>
#include <shellapi.h>
#include "operclass/ccreatedump.h"
#endif

using namespace QsLogging;

void logFunction(const QString &message, QsLogging::Level level)
{
	std::cout << "From log function: " << qPrintable(message) << " " << static_cast<int>(level)
		<< std::endl;
}

void _boolfolderexist(QString folderPath,QString& out_todaylogfilename)
{
    if(!QDir(folderPath).exists())
    {
        QDir().mkdir(folderPath);
    }
    QDateTime dateTime = QDateTime::currentDateTime();
    out_todaylogfilename= dateTime.toString("yyyyMMdd");
}


int main(int argc, char *argv[])
{
 
#ifdef Q_OS_WIN
    //异常处理 生成dmp文件
    CCreateDump::Instance()->DeclarDumpFile("dumpfile");
#endif

    QApplication::addLibraryPath("./plugins");
	FullyAutomatedPlatelets app(argc, argv);

	QString _datelog;
	QString flod_ = QDir(app.applicationDirPath()).filePath("suoweiLogFile");
	_boolfolderexist(flod_, _datelog);
	_datelog = _datelog + "logs.txt";

	// 1. init the logging mechanism
	QString logPath = QString("%1/%2").arg(flod_).arg(_datelog);
	Logger& logger = Logger::instance();
	logger.setLoggingLevel(QsLogging::TraceLevel);
	const QString sLogPath(logPath);
	// 2. add two destinations
	DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
        sLogPath, EnableLogRotation, MaxSizeBytes(512 * 1024 * 1024), MaxOldLogCount(1)));
	DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
	DestinationPtr functorDestination(DestinationFactory::MakeFunctorDestination(&logFunction));
	logger.addDestination(debugDestination);
	logger.addDestination(functorDestination);
	logger.addDestination(fileDestination);

	HANDLE hMutex = CreateMutex(nullptr, TRUE, (LPCWSTR)qApp->applicationName().toStdWString().c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
		QMessageBox::warning(nullptr, "系统提示", "已有一个相同程序运行中...");
		CloseHandle(hMutex);
		hMutex = NULL;
		return 1;
	}

    loginmaininterface intreface;
    if(intreface.exec() == QDialog::Accepted)
    {
        app.mainWindow()->show();

        app.setQuitOnLastWindowClosed(false);

		app.mainWindow()->setAttribute(Qt::WA_QuitOnClose, true); //最后一个窗口关闭时关闭程序.

        return app.QApplication::exec();
    }
    else
    {
        return 0;
    }
	CloseHandle(hMutex);
	hMutex = NULL;
    QLOG_TRACE() <<"[程序退出]";
    return 0;
}
