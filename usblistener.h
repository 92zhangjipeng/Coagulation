#ifndef USBLISTENER_H
#define USBLISTENER_H

#pragma once
#include <QObject>

#include <initguid.h>

#include <QAbstractNativeEventFilter>
#include <Windows.h>
#include <QWidget>
#include <dbt.h>
#include <atomic>

#include <usbiodef.h>
#include <hidclass.h>  // 添加HID设备支持

#include <windows.h>
#include <setupapi.h>

#include <devguid.h>

#include <vector>
#include <string>
#include <tchar.h>

#include <winusb.h>
#include <cfgmgr32.h>

#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "cfgmgr32.lib")

#pragma execution_character_set("utf-8")





// USB 设备信息结构体
struct usbDevice {
    uint16_t VID;           // 供应商ID
    uint16_t PID;           // 产品ID
    QString description;    // 设备描述
	QString type;  // 添加type成员
};


class USBListener : public QObject, public QAbstractNativeEventFilter{
    Q_OBJECT
public:
    explicit USBListener(uint16_t targetVID, uint16_t targetPID, QObject* parent = nullptr);
    ~USBListener();

    void registerDevice(HWND pwid);//注册USB设备

    // 核心监听方法
    bool startListening();
    void stopListening();

  
protected:
    // Windows 消息过滤器
    bool nativeEventFilter(const QByteArray& eventType, void* message, long* result) override;

signals:
    // USB 设备连接/断开信号
    void deviceConnected(usbDevice dev);
    void deviceDisconnected(usbDevice dev);

    //状态信号
    void listenerStarted();
    void listenerStopped();
    void usbPowerContl(QString message,const quint8 istate);
    void errorOccurred(QString message);

private:

	void  initDisAblePower();

	bool createMessageWindow();
	bool registerDeviceNotifications();

    // 解析设备信息
    usbDevice parseDeviceInfo(const QString& devicePath);

    // 检查是否目标设备
    bool isTargetDevice(uint16_t vid, uint16_t pid);

private:
    const uint16_t m_targetVID;      // 目标设备VID
    const uint16_t m_targetPID;      // 目标设备PID



    HDEVNOTIFY m_usbNotify = nullptr;
    HDEVNOTIFY m_diskNotify = nullptr;
    HDEVNOTIFY m_hidNotify = nullptr;


    HWND m_hwnd = nullptr;            // 接收消息的窗口句柄
    std::atomic<bool> m_listening{false}; // 监听状态标志


	bool DisableSelectiveSuspendForDevice(const wchar_t* deviceInstanceId, QString &outFailed);
};
#endif // USBLISTENER_H
