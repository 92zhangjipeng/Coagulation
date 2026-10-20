#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

#include "QsLog/include/QsLog.h"
#include "QsLog/include/QsLogDest.h"
#include "QsLog/include/QsLogLevel.h"
#include "dilag/loginmaininterface.h"
#include <loginui.h>
#include <QApplication>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QMessageBox>
#include <QDir>
#include <QDateTime>
#include <memory>
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
#include <LIS/lisapiservice.h>
#include <LIS/curveimagegenerator.h>
#endif

using namespace QsLogging;

// 日志配置常量
namespace LogConfig {
	constexpr qint64 MAX_LOG_SIZE_BYTES = 512 * 1024 * 1024;  // 512MB
	constexpr int MAX_LOG_FILES = 1;
	const QString LOG_DIRECTORY_NAME = "suoweiLogFile";
	const QString LOG_FILE_SUFFIX = "logs.txt";
}

// 辅助函数：安全的调试输出
inline void safeOutputDebugString(const char* message) {
#ifdef Q_OS_WIN
	OutputDebugStringA(message);
#else
	Q_UNUSED(message);
#endif
}

inline void safeOutputDebugString(const QString& message) {
#ifdef Q_OS_WIN
	OutputDebugStringW(reinterpret_cast<LPCWSTR>(message.utf16()));
#else
	Q_UNUSED(message);
#endif
}

// RAII包装器：自动管理Mutex资源
class MutexHandle {
private:
	HANDLE m_handle = nullptr;

public:
	MutexHandle() = default;
	~MutexHandle() { release(); }

	bool create(const QString& name) {
		release();
#ifdef Q_OS_WIN
		std::wstring wName = name.toStdWString();
		m_handle = CreateMutexW(nullptr, TRUE, wName.c_str());
		return m_handle != nullptr && GetLastError() != ERROR_ALREADY_EXISTS;
#else
		Q_UNUSED(name);
		return true;
#endif
	}

	bool isExists() const {
#ifdef Q_OS_WIN
		return m_handle != nullptr && GetLastError() == ERROR_ALREADY_EXISTS;
#else
		return false;
#endif
	}

	void release() {
#ifdef Q_OS_WIN
		if (m_handle) {
			CloseHandle(m_handle);
			m_handle = nullptr;
		}
#endif
	}

	MutexHandle(const MutexHandle&) = delete;
	MutexHandle& operator=(const MutexHandle&) = delete;
};

// 日志系统管理器（使用原始指针，因为Logger不是QObject）
class LogManager {
private:
	Logger* m_logger = nullptr;
	bool m_isInitialized = false;

	bool ensureLogDirectoryExists(const QString& folderPath, QString& out_todayLogFileName) {
		try {
			QDir dir(folderPath);
			if (!dir.exists() && !dir.mkpath(folderPath)) {
				outputErrorMessage("Failed to create log directory: " + folderPath);
				return false;
			}

			QDateTime dateTime = QDateTime::currentDateTime();
			if (!dateTime.isValid()) {
				outputErrorMessage("Invalid current date/time");
				return false;
			}

			out_todayLogFileName = dateTime.toString("yyyyMMdd");
			return true;
		}
		catch (const std::exception& e) {
			outputErrorMessage(QString("Exception in ensureLogDirectoryExists: %1").arg(e.what()));
			return false;
		}
		catch (...) {
			outputErrorMessage("Unknown exception in ensureLogDirectoryExists");
			return false;
		}
	}

	void outputErrorMessage(const QString& message) {
		safeOutputDebugString(message);
		std::cerr << qPrintable(message) << std::endl;
	}

public:
	LogManager() = default;
	~LogManager() { shutdown(); }

	bool initialize(const QString& appPath) {
		if (m_isInitialized) {
			return true;
		}

		QString logDate;
		QString logFolder = QDir(appPath).filePath(LogConfig::LOG_DIRECTORY_NAME);

		if (!ensureLogDirectoryExists(logFolder, logDate)) {
			QMessageBox::critical(nullptr,
				QString::fromUtf8("\xe7\xb3\xbb\xe7\xbb\x9f\xe9\x94\x99\xe8\xaf\xaf"),
				QString::fromUtf8("\xe6\x97\xa0\xe6\xb3\x95\xe5\x88\x9b\xe5\xbb\xba\xe6\x97\xa5\xe5\xbf\x97\xe7\x9b\xae\xe5\xbd\x95"));
			return false;
		}

		QString logPath = QString("%1/%2%3").arg(logFolder).arg(logDate).arg(LogConfig::LOG_FILE_SUFFIX);

		try {
			// 直接获取Logger实例引用，不使用QPointer
			m_logger = &Logger::instance();
			// Release：Info 即可。Trace + 多路同步输出会把启动拖得很卡
#ifdef QT_NO_DEBUG
			m_logger->setLoggingLevel(QsLogging::InfoLevel);
#else
			m_logger->setLoggingLevel(QsLogging::TraceLevel);
#endif

			// 创建日志输出目标（文件为主）
			DestinationPtr fileDestination(DestinationFactory::MakeFileDestination(
				logPath,
				EnableLogRotation,
				MaxSizeBytes(LogConfig::MAX_LOG_SIZE_BYTES),
				MaxOldLogCount(LogConfig::MAX_LOG_FILES)));
			m_logger->addDestination(fileDestination);

#ifndef QT_NO_DEBUG
			// Debug 构建才挂调试输出；Release 避免每条日志同步写控制台/OutputDebugString
			DestinationPtr debugDestination(DestinationFactory::MakeDebugOutputDestination());
			m_logger->addDestination(debugDestination);
#endif

			m_isInitialized = true;
			QLOG_INFO() << "Log system initialized successfully";
			return true;
		}
		catch (const std::exception& e) {
			outputErrorMessage(QString("Failed to initialize logging: %1").arg(e.what()));
			return false;
		}
		catch (...) {
			outputErrorMessage("Unknown error initializing logging");
			return false;
		}
	}

	void shutdown() {
		if (m_logger && m_isInitialized) {
			QLOG_TRACE() << QString::fromUtf8("[\xe7\xa8\x8b\xe5\xba\x8f\xe9\x80\x80\xe5\x87\xba]");
			Logger::destroyInstance();
			m_logger = nullptr;
			m_isInitialized = false;
		}
	}

	bool isInitialized() const { return m_isInitialized; }

	// 获取Logger指针，用于需要日志输出的地方
	Logger* logger() const { return m_logger; }
};

// 崩溃处理初始化器
class CrashHandler {
public:
	static bool initialize() {
#ifdef Q_OS_WIN
		if (!CCreateDump::Instance()->DeclarDumpFile("dumpfile")) {
			safeOutputDebugString("Warning: Failed to initialize dump file creation\n");
			return false;
		}
		return true;
#else
		return true;
#endif
	}
};

// 应用程序初始化和运行器
class ApplicationRunner {
private:
	std::unique_ptr<FullyAutomatedPlatelets> m_app;
	MutexHandle m_mutex;
	LogManager m_logManager;
	bool m_initialized = false;

	bool checkAndCreateSingleInstance() {
		QString appName = QCoreApplication::applicationName();
		if (appName.isEmpty()) {
			appName = "FullyAutomatedPlatelets";
			QCoreApplication::setApplicationName(appName);
		}

		if (!m_mutex.create(appName)) {
			if (m_mutex.isExists()) {
				QMessageBox::warning(nullptr,
					QString::fromUtf8("\xe7\xb3\xbb\xe7\xbb\x9f\xe6\x8f\x90\xe7\xa4\xba"),
					QString::fromUtf8("\xe5\xb7\xb2\xe6\x9c\x89\xe4\xb8\x80\xe4\xb8\xaa\xe7\x9b\xb8\xe5\x90\x8c\xe7\xa8\x8b\xe5\xba\x8f\xe8\xbf\x90\xe8\xa1\x8c\xe4\xb8\xad\x2e\x2e\x2e"));
			}
			return false;
		}
		return true;
	}

	bool setupApplication(int argc, char* argv[]) {
		try {
			m_app = std::make_unique<FullyAutomatedPlatelets>(argc, argv);
			QApplication::addLibraryPath("./plugins");
			m_app->setWindowIcon(QIcon(":/Picture/logo.ico"));

			m_app->restoreOverrideCursor();

			return true;
		}
		catch (const std::exception& e) {
			QMessageBox::critical(nullptr,
				QString::fromUtf8("\xe7\xb3\xbb\xe7\xbb\x9f\xe9\x94\x99\xe8\xaf\xaf"),
				QString::fromUtf8("\xe5\xba\x94\xe7\x94\xa8\xe7\xa8\x8b\xe5\xba\x8f\xe5\x88\x9d\xe5\xa7\x8b\xe5\x8c\x96\xe5\xa4\xb1\xe8\xb4\xa5\x3a\x20\x25\x31").arg(e.what()));
			return false;
		}
	}

	int executeLoginAndMainWindow() {
		loginmaininterface interfaceLogin;

		if (interfaceLogin.exec() != QDialog::Accepted) {
			if (m_logManager.isInitialized()) {
				QLOG_INFO() << "User cancelled login";
			}
			return 0;  // 正常退出
		}

		QMainWindow* pMainWindow = m_app->mainWindow();
		if (!pMainWindow) {
			if (m_logManager.isInitialized()) {
				QLOG_ERROR() << "Main window is null";
			}
			QMessageBox::critical(nullptr,
				QString::fromUtf8("\xe7\xb3\xbb\xe7\xbb\x9f\xe9\x94\x99\xe8\xaf\xaf"),
				QString::fromUtf8("\xe6\x97\xa0\xe6\xb3\x95\xe5\x88\x9b\xe5\xbb\xba\xe4\xb8\xbb\xe7\xaa\x97\xe5\x8f\xa3"));
			return 1;
		}

		pMainWindow->show();
		m_app->setQuitOnLastWindowClosed(false);
		pMainWindow->setAttribute(Qt::WA_QuitOnClose, true);

		return m_app->QApplication::exec();
	}

public:
	bool initialize(int argc, char* argv[]) {
		if (m_initialized) {
			return true;
		}

		// 按顺序初始化各个组件
		if (!CrashHandler::initialize()) {
			// 崩溃处理初始化失败不影响程序运行，只记录警告
			safeOutputDebugString("Warning: Crash handler initialization failed\n");
		}

		if (!setupApplication(argc, argv)) {
			return false;
		}

#ifdef Q_OS_WIN
		// Qt/CRT 可能在 QApplication 构造时覆盖异常过滤器，启动后再挂一次
		CCreateDump::Instance()->ReinstallFilter();
#endif

		if (!m_logManager.initialize(m_app->applicationDirPath())) {
			return false;
		}

#ifdef Q_OS_WIN
		{
			const wchar_t* dumpDir = CCreateDump::Instance()->dumpDirectory();
			if (dumpDir && dumpDir[0] != L'\0') {
				QLOG_INFO() << QString::fromUtf8("崩溃转储目录:")
							<< QString::fromWCharArray(dumpDir);
			}
		}
#endif

		if (!checkAndCreateSingleInstance()) {
			return false;
		}

		m_initialized = true;
		return true;
	}

	int run() {
		if (!m_initialized) {
			if (m_logManager.isInitialized()) {
				QLOG_ERROR() << "Application not initialized";
			}
			else {
				safeOutputDebugString("Application not initialized\n");
			}
			return -1;
		}

		try {
			return executeLoginAndMainWindow();
		}
		catch (const std::exception& e) {
			if (m_logManager.isInitialized()) {
				QLOG_FATAL() << "Unhandled exception in application: " << e.what();
			}
			else {
				safeOutputDebugString(QString("Unhandled exception: %1\n").arg(e.what()).toUtf8().constData());
			}
			QMessageBox::critical(nullptr,
				QString::fromUtf8("\xe7\xb3\xbb\xe7\xbb\x9f\xe9\x94\x99\xe8\xaf\xaf"),
				QString::fromUtf8("\xe7\xa8\x8b\xe5\xba\x8f\xe5\x8f\x91\xe7\x94\x9f\xe6\x9c\xaa\xe5\xa4\x84\xe7\x90\x86\xe7\x9a\x84\xe5\xbc\x82\xe5\xb8\xb8\x3a\x20\x25\x31").arg(e.what()));
			return -1;
		}
		catch (...) {
			if (m_logManager.isInitialized()) {
				QLOG_FATAL() << "Unknown exception in application";
			}
			else {
				safeOutputDebugString("Unknown exception in application\n");
			}
			QMessageBox::critical(nullptr,
				QString::fromUtf8("\xe7\xb3\xbb\xe7\xbb\x9f\xe9\x94\x99\xe8\xaf\xaf"),
				QString::fromUtf8("\xe7\xa8\x8b\xe5\xba\x8f\xe5\x8f\x91\xe7\x94\x9f\xe6\x9c\xaa\xe7\x9f\xa5\xe5\xbc\x82\xe5\xb8\xb8"));
			return -1;
		}
	}

	~ApplicationRunner() {
		// 析构顺序很重要：先停止日志，再释放其他资源
		m_initialized = false;
		m_mutex.release();
		// m_logManager会在其析构函数中自动关闭
		m_app.reset();
	}
};

// 控制台输出辅助函数（用于日志系统初始化前的错误报告）
void outputEarlyErrorMessage(const QString& message) {
	safeOutputDebugString(message);
	fprintf(stderr, "%s\n", qPrintable(message));
	fflush(stderr);
}


int main(int argc, char* argv[])
{
	// 设置应用程序元数据（尽早设置）
	QCoreApplication::setApplicationName("FullyAutomatedPlatelets");
	QCoreApplication::setOrganizationName("SuoWei");

	// 使用RAII管理器运行应用程序
	ApplicationRunner runner;

	if (!runner.initialize(argc, argv)) {
		outputEarlyErrorMessage("Failed to initialize application");
		return 1;
	}

    QString dbPath = QDir(QCoreApplication::applicationDirPath()).filePath("SQLFile/SWData.db");
    CurveImageGenerator::instance()->init(dbPath, "./CurveImages");
    // 单独设置数据文件的保存目录（可以不同于图片目录）
    CurveImageGenerator::instance()->setDataSaveDirectory("./TestResults");

    // 创建并启动 LIS API 服务（父对象为 QApplication，确保整个生命周期存在）
    //LisApiService *lisService = new LisApiService(8080, dbPath, qApp);
    //lisService->setImageSaveDirectory("./CurveImages");
    //lisService->setImageRetentionDays(60);  // 保留60天
    //lisService->start();

	int exitCode = runner.run();

	// 确保日志刷新
	QCoreApplication::processEvents();

	return exitCode;
}
