#include "usblistenerthread.h"
#include <QCoreApplication>
#include <QDebug>
#include <QsLog/include/QsLog.h>
#include <QElapsedTimer>
#include <Windows.h>

USBListenerThread::USBListenerThread(uint16_t targetVID, uint16_t targetPID,  QObject* parent)
    : QThread(parent), m_targetVID(targetVID), m_targetPID(targetPID) {
    qRegisterMetaType<usbDevice>("usbDevice"); // 注册自定义类型用于跨线程信号
}

USBListenerThread::~USBListenerThread() {
	
	if (!isRunning()) return;

	//请求停止
	requestStop();

	//安全释放窗口资源
	m_hwnd = nullptr;
	
	QLOG_DEBUG() << "USB监听线程已销毁";
}


void USBListenerThread::requestStop() {
    m_stopRequested.store(true);
	quit();
}

bool USBListenerThread::isListening() const {
    return m_listening;
}

void USBListenerThread::registerDevice(HWND pwid)//注册usb设备到系统
{
    m_hwnd = pwid;
}



void USBListenerThread::run() {

	QMutexLocker locker(&m_mutex);

	m_nativeThreadId.store(GetCurrentThreadId(), std::memory_order_release);

	QScopedPointer<USBListener> listener(new USBListener(m_targetVID, m_targetPID));
	listener->registerDevice(m_hwnd);

	connect(listener.data(), &USBListener::deviceConnected,
		this, &USBListenerThread::deviceConnected, Qt::QueuedConnection);
	connect(listener.data(), &USBListener::deviceDisconnected,
		this, &USBListenerThread::deviceDisconnected, Qt::QueuedConnection);

    // 启动监听
    if (!listener->startListening()) {
        emit errorOccurred("无法启动USB监听");
        return;
    }

	// 4. 安全状态更新
	{
		//QMutexLocker lock(&m_mutex);
		m_listening = true;
	}
	emit listenerStarted();  
	QLOG_DEBUG() << "USB监听线程已启动";

	// 5. 安全的消息循环
	MSG msg;
	while (!m_stopRequested.load(std::memory_order_acquire)) {
		if (m_stopRequested.load()) 
			break;  // 优先检查退出标志

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		QThread::msleep(10);  // 避免CPU占用过高
	}

	// 安全清理
	listener->stopListening();
	{
		//QMutexLocker lock(&m_mutex);
		m_listening = false;
	}
	emit listenerStopped();
	QLOG_DEBUG() << "USB listener thread stopped";
}
