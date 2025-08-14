#ifndef USBLISTENERTHREAD_H
#define USBLISTENERTHREAD_H

#pragma once
#include <QThread>
#include "USBListener.h"
#include <QMutex>
#include <QObject>
#include <QWaitCondition>


class USBListenerThread : public QThread
{
    Q_OBJECT
   public:
       explicit USBListenerThread(uint16_t targetVID, uint16_t targetPID, QObject* parent = nullptr);
       ~USBListenerThread();

       void registerDevice(HWND pwid);

	   DWORD getNativeThreadId() const {
		   return m_nativeThreadId.load(std::memory_order_acquire);
	   }

       // 线程控制
       void requestStop();
       bool isListening() const;
signals:
    // USB 设备事件信号
    void deviceConnected(usbDevice dev);
    void deviceDisconnected(usbDevice dev);

    // 线程状态信号
    void listenerStarted();
    void listenerStopped();
    void errorOccurred(QString message);

protected:
    void run() override;

private:
    uint16_t m_targetVID;              // 目标设备VID
    uint16_t m_targetPID;              // 目标设备PID
  
    std::atomic<bool> m_stopRequested{false}; // 停止请求标志
    std::atomic<bool> m_listening{false};     // 监听状态标志


	QMutex m_mutex;
	HWND m_hwnd{ nullptr };

	std::atomic<DWORD> m_nativeThreadId{ 0 };

};

#endif // USBLISTENERTHREAD_H
