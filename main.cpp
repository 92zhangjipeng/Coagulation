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
	try {
		std::cout << "From log function: " << qPrintable(message)
			<< " " << static_cast<int>(level) << std::endl;
	}
	catch (const std::exception& e) {
		OutputDebugStringA("Log function error: ");
		OutputDebugStringA(e.what());
	}
}

bool ensureLogDirectoryExists(const QString& folderPath, QString& out_todaylogfilename)
{
	try {
		QDir dir(folderPath);
		if (!dir.exists()) {
			if (!dir.mkpath(folderPath)) {
				QLOG_ERROR() << "Failed to create log directory: " << folderPath;
				return false;
			}
		}

		QDateTime dateTime = QDateTime::currentDateTime();
		if (!dateTime.isValid()) {
			QLOG_ERROR() << "Invalid current date/time";
			return false;
		}

		out_todaylogfilename = dateTime.toString("yyyyMMdd");
		return true;
	}
	catch (const std::exception& e) {
		QLOG_ERROR() << "Exception in ensureLogDirectoryExists: " << e.what();
		return false;
	}
}

int main(int argc, char *argv[])
{
	int exitCode = 0;
	HANDLE hMutex = NULL;
	Logger* pLogger = nullptr;

	try {
#ifdef Q_OS_WIN
		// 异常处理 生成dmp文件
		if (!CCreateDump::Instance()->DeclarDumpFile("dumpfile")) {
			OutputDebugStringA("Failed to initialize dump file creation");
		}
#endif

		// 初始化应用程序
		FullyAutomatedPlatelets app(argc, argv);
		QApplication::addLibraryPath("./plugins");

		// 初始化日志系统
		QString logDate;
		QString logFolder = QDir(app.applicationDirPath()).filePath("suoweiLogFile");

		if (!ensureLogDirectoryExists(logFolder, logDate)) {
			QMessageBox::critical(nullptr, "系统错误", "无法创建日志目录");
			return 1;
		}

		QString logPath = QString("%1/%2logs.txt").arg(logFolder).arg(logDate);

		pLogger = &Logger::instance();
		pLogger->setLoggingLevel(QsLogging::TraceLevel);

		DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
			logPath, EnableLogRotation, MaxSizeBytes(512 * 1024 * 1024), MaxOldLogCount(1)));
		DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
		DestinationPtr functorDestination(DestinationFactory::MakeFunctorDestination(&logFunction));

		pLogger->addDestination(debugDestination);
		pLogger->addDestination(functorDestination);
		pLogger->addDestination(fileDestination);

		// 检查程序是否已运行
		hMutex = CreateMutex(nullptr, TRUE, (LPCWSTR)qApp->applicationName().toStdWString().c_str());
		if (hMutex == NULL || GetLastError() == ERROR_ALREADY_EXISTS) {
			QMessageBox::warning(nullptr, "系统提示", "已有一个相同程序运行中...");
			if (hMutex) {
				CloseHandle(hMutex);
				hMutex = NULL;
			}
			return 1;
		}

		// 显示登录界面
		loginmaininterface interfaceLogin;
		if (interfaceLogin.exec() == QDialog::Accepted) {
			// 登录成功，显示主窗口
			QMainWindow* pMainWindow = app.mainWindow();
			if (pMainWindow) {
				pMainWindow->show();
				app.setQuitOnLastWindowClosed(false);
				pMainWindow->setAttribute(Qt::WA_QuitOnClose, true);

				exitCode = app.QApplication::exec();
			}
			else {
				QLOG_ERROR() << "Main window is null";
				exitCode = 1;
			}
		}
	}
	catch (const std::exception& e) {
		QLOG_FATAL() << "Unhandled exception in main: " << e.what();
		QMessageBox::critical(nullptr, "系统错误", "程序发生未处理的异常");
		exitCode = -1;
	}
	catch (...) {
		QLOG_FATAL() << "Unknown exception in main";
		QMessageBox::critical(nullptr, "系统错误", "程序发生未知异常");
		exitCode = -1;
	}

	// 清理资源
	if (hMutex) {
		CloseHandle(hMutex);
		hMutex = NULL;
	}

	if (pLogger) {
		QLOG_TRACE() << "[程序退出]";
		Logger::destroyInstance();
	}

	return exitCode;
}