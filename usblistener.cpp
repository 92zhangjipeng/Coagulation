#include "USBListener.h"
#include <QsLog/include/QsLog.h>
#include <QCoreApplication>
#include <QDebug>
#include "cglobal.h"
// 确保包含必要的头文件
#include <hidsdi.h>
#include <hidpi.h>



// 兼容旧版 SDK
#ifndef DN_POWER_SUSPENDED
#define DN_POWER_SUSPENDED 0x00000040
#endif



USBListener::USBListener(uint16_t targetVID, uint16_t targetPID,  QObject* parent)
    : QObject(parent), m_targetVID(targetVID), m_targetPID(targetPID){

	// 初始化成员变量
	m_hwnd = nullptr;
	m_listening = false;
}

USBListener::~USBListener() {
    stopListening();

}


QString getDeviceInstancePathFromVidPid(DWORD vid, DWORD pid) {
    // 初始化设备信息集（DIGCF_ALLCLASSES | DIGCF_PRESENT 表示所有已连接设备）
    HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        return QString(); // 失败返回空字符串
    }

    SP_DEVINFO_DATA devInfoData;
    devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD memberIndex = 0;
    QString resultPath;

    // 枚举设备
    while (SetupDiEnumDeviceInfo(hDevInfo, memberIndex, &devInfoData)) {
        TCHAR instanceId[MAX_DEVICE_ID_LEN];
        DWORD requiredSize = 0;

        // 获取设备实例ID（即设备实例路径）
        if (SetupDiGetDeviceInstanceId(hDevInfo, &devInfoData, instanceId, MAX_DEVICE_ID_LEN, &requiredSize)) {
            // 将VID和PID转换为目标字符串格式（如 "VID_0461&PID_4D15"）
            TCHAR targetVidPid[50];
            _stprintf_s(targetVidPid, _T("VID_%04X&PID_%04X"), vid, pid);

            // 检查实例路径是否包含目标VID和PID
            if (_tcsstr(instanceId, targetVidPid) != NULL) {
                resultPath = QString::fromWCharArray(instanceId);
                break; // 找到匹配项，退出循环
            }
        }
        memberIndex++;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo); // 释放资源
    return resultPath;
}




//bool  USBListener::DisableSelectiveSuspendForDevice(const wchar_t* deviceInstanceId,QString &outFailed) {
//	DWORD err;
//	QString formattedErr;
//	// 1. 获取设备信息集（枚举所有设备）
//	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
//    if (hDevInfo == INVALID_HANDLE_VALUE) {
//		err = GetLastError();
//		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
//		QLOG_WARN() << "USB电源禁用失败(SetupDiGetClassDevs): 0x"<< formattedErr;
//        return false;
//    }
//	m_hDevInfo = hDevInfo;
//
//
//    // 2. 枚举设备，查找匹配的实例ID
//	SP_DEVINFO_DATA devInfoData  = { sizeof(SP_DEVINFO_DATA) };
//    DWORD devIndex = 0;
//    bool deviceFound = false;
//
//    while (SetupDiEnumDeviceInfo(hDevInfo, devIndex, &devInfoData)) {
//        // 获取设备实例ID
//        wchar_t instanceId[MAX_DEVICE_ID_LEN] = {0};
//        if (CM_Get_Device_ID(devInfoData.DevInst, instanceId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
//            if (wcsstr(instanceId, deviceInstanceId) != nullptr) {
//                deviceFound = true;
//                break;
//            }
//        }
//        devIndex++;
//    }
//
//    if (!deviceFound) {
//		QLOG_WARN() << "USB电源禁用失败(未找到设备): " << QString::fromWCharArray(deviceInstanceId);
//        SetupDiDestroyDeviceInfoList(hDevInfo);
//        return false;
//    }
//
//    // 3. 禁用设备的电源管理（选择性暂停）
//    SP_PROPCHANGE_PARAMS propChangeParams = {0};
//    propChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
//    propChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
//    propChangeParams.StateChange = DICS_DISABLE;  // 禁用设备电源管理
//    propChangeParams.Scope = DICS_FLAG_CONFIGSPECIFIC; // 仅针对当前配置
//    propChangeParams.HwProfile = 0;
//
//    // 设置类安装参数
//    if (!SetupDiSetClassInstallParams(hDevInfo, &devInfoData,
//                                    (SP_CLASSINSTALL_HEADER*)&propChangeParams,
//                                    sizeof(propChangeParams))) {
//		err = GetLastError();
//		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
//		QLOG_WARN() << "USB电源禁用失败(SetupDiSetClassInstallParams): 0x"<< formattedErr;
//        SetupDiDestroyDeviceInfoList(hDevInfo);
//        return false;
//    }
//
//    // 应用更改
//    if (!SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &devInfoData)) {
//		err = GetLastError();
//		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
//		QLOG_WARN() << "USB电源禁用失败(SetupDiCallClassInstaller): 0x"<< formattedErr;
//        const int outCode = formattedErr.toInt();
//        switch (outCode) {
//            case 5: outFailed = "权限不足";  break;  //- 0x5 (ACCESS_DENIED): 权限不足
//            case 2: outFailed = "设备不存在";break; // - 0x2 (ERROR_FILE_NOT_FOUND): 设备不存在
//        default:
//            outFailed ="未知异常";
//            break;
//        }
//        SetupDiDestroyDeviceInfoList(hDevInfo);
//        return false;
//    }
//	m_devInfoData = devInfoData;
//    SetupDiDestroyDeviceInfoList(hDevInfo);
//
//	m_deviceWasDisabled = true;
//    return true;
//}

bool USBListener::DisableSelectiveSuspendForDevice(const wchar_t* deviceInstanceId, QString &outFailed) {
	DWORD err;
	QString formattedErr;

	// 1. 获取设备信息集
	HDEVINFO hDevInfo = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (hDevInfo == INVALID_HANDLE_VALUE) {
		err = GetLastError();
		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
		QLOG_WARN() << "USB电源禁用失败(SetupDiGetClassDevs): 0x" << formattedErr;
		return false;
	}

	// 2. 枚举设备，查找匹配的实例ID
	SP_DEVINFO_DATA devInfoData = { sizeof(SP_DEVINFO_DATA) };
	DWORD devIndex = 0;
	bool deviceFound = false;
	while (SetupDiEnumDeviceInfo(hDevInfo, devIndex, &devInfoData)) {
		wchar_t instanceId[MAX_DEVICE_ID_LEN] = { 0 };
		if (CM_Get_Device_ID(devInfoData.DevInst, instanceId, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS) {
			if (wcsstr(instanceId, deviceInstanceId) != nullptr) {
				deviceFound = true;
				break;
			}
		}
		devIndex++;
	}

	if (!deviceFound) {
		QLOG_WARN() << "USB电源禁用失败(未找到设备): " << QString::fromWCharArray(deviceInstanceId);
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return false;
	}

	// 3. 获取设备路径（新增关键步骤）
	SP_DEVICE_INTERFACE_DATA interfaceData = { sizeof(SP_DEVICE_INTERFACE_DATA) };
	GUID winusbGuid = GUID_DEVINTERFACE_WINUSB; // WinUSB设备接口GUID

												// 创建设备接口
	if (!SetupDiCreateDeviceInterface(hDevInfo, &devInfoData, &winusbGuid, NULL, 0, &interfaceData)) {
		err = GetLastError();
		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
		QLOG_WARN() << "USB电源禁用失败(创建设备接口): 0x" << formattedErr;
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return false;
	}

	// 获取接口详情
	DWORD requiredSize = 0;
	SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, NULL, 0, &requiredSize, NULL);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
		err = GetLastError();
		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
		QLOG_WARN() << "USB电源禁用失败(获取接口详情大小): 0x" << formattedErr;
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return false;
	}

	QByteArray detailBuffer(requiredSize, 0);
	auto pDetail = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA*>(detailBuffer.data());
	pDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

	if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, pDetail, requiredSize, NULL, NULL)) {
		err = GetLastError();
		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
		QLOG_WARN() << "USB电源禁用失败(获取设备路径): 0x" << formattedErr;
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return false;
	}

	const wchar_t* devicePath = pDetail->DevicePath;

	// 4. 打开设备并禁用电源管理
	HANDLE hDevice = CreateFile(devicePath, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
		NULL);

	if (hDevice == INVALID_HANDLE_VALUE) {
		err = GetLastError();
		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
		QLOG_WARN() << "USB电源禁用失败(打开设备): 0x" << formattedErr;
		SetupDiDestroyDeviceInfoList(hDevInfo);
		return false;
	}

	WINUSB_INTERFACE_HANDLE winusbHandle = NULL;
	bool result = false;

	if (WinUsb_Initialize(hDevice, &winusbHandle)) {
		UCHAR policyValue = FALSE; // FALSE表示禁用自动挂起
		ULONG timeout = 0; // 立即生效

						   // 禁用自动挂起（选择性暂停）
		if (WinUsb_SetPowerPolicy(winusbHandle, AUTO_SUSPEND, sizeof(policyValue), &policyValue)) {
			// 设置超时策略为0防止延迟挂起
			WinUsb_SetPowerPolicy(winusbHandle, SUSPEND_DELAY, sizeof(timeout), &timeout);
			result = true;
		}
		else {
			err = GetLastError();
			formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
			QLOG_WARN() << "USB电源禁用失败(设置电源策略): 0x" << formattedErr;
		}

		WinUsb_Free(winusbHandle);
	}
	else {
		err = GetLastError();
		formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
		QLOG_WARN() << "USB电源禁用失败(初始化WinUSB): 0x" << formattedErr;
	}

	CloseHandle(hDevice);
	SetupDiDestroyDeviceInfoList(hDevInfo);

	// 5. 错误处理
	if (!result) {
		switch (err) {
		case ERROR_ACCESS_DENIED:
			outFailed = "权限不足"; break;
		case ERROR_NOT_SUPPORTED:
			outFailed = "设备不支持电源管理"; break;
		case ERROR_DEVICE_NOT_CONNECTED:
			outFailed = "设备未连接"; break;
		case ERROR_INVALID_HANDLE:
			outFailed = "设备句柄无效"; break;
		default:
			outFailed = QString("未知错误 (0x%1)").arg(formattedErr);
			break;
		}
		return false;
	}

	QLOG_INFO() << "USB电源管理已禁用：" << QString::fromWCharArray(deviceInstanceId);
	return true;
}



void USBListener::registerDevice(HWND pwid)//注册usb设备到系统
{
    m_hwnd = pwid;
}

bool USBListener::registerDeviceNotifications()
{
	// 1. 注册标准USB设备
	DEV_BROADCAST_DEVICEINTERFACE usbFilter = { 0 };
	usbFilter.dbcc_size = sizeof(usbFilter);
	usbFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	usbFilter.dbcc_classguid = GUID_DEVINTERFACE_USB_DEVICE;
	m_usbNotify = RegisterDeviceNotification(m_hwnd, &usbFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

	if (!m_usbNotify) {
		QLOG_ERROR() << "All device notifications failed to register";
		return false;
	}
	return true;
}

void USBListener::initDisAblePower() {

	QString outmessage, knowErr;
	quint8  isstate = 0;
	QString path = getDeviceInstancePathFromVidPid(m_targetVID, m_targetPID);
	if (path.isEmpty() || path.isNull()) {
		isstate = PROMPTLOG;
		outmessage = "设备路径识别失败(未连接)!禁用USB选择性暂停失败";
		QLOG_WARN() << outmessage;
	}
	else {
		//禁用特定 USB 设备的电源管理（需替换为实际设备实例 ID）
		const wchar_t* deviceId = reinterpret_cast<const wchar_t*>(path.utf16());
		if (DisableSelectiveSuspendForDevice(deviceId, knowErr)) {
			outmessage = "设备的 USB 选择性暂停已禁用!";
			isstate = NORMALLOG;
			QLOG_DEBUG() << outmessage;
		}
		else {
			outmessage = "设备的 USB 选择性暂停禁用失败!(" + knowErr + ")";
			isstate = ERRORLOG;
			QLOG_ERROR() << outmessage;
		}
	}
	Q_EMIT usbPowerContl(outmessage, isstate);
}


bool USBListener::startListening() {
	if (m_listening) return true;

	initDisAblePower();


	if (!registerDeviceNotifications()) {
        QLOG_ERROR() << "Failed to register device notifications";
		m_hwnd = nullptr;
        emit errorOccurred("无法启动USB监听");
		return false;
	}

	qApp->installNativeEventFilter(this);
	m_listening = true;
    emit listenerStarted();
	return true;
}

void USBListener::stopListening() {
    
	if (!m_listening) return;

	m_hwnd = nullptr;

	if (m_usbNotify) {
		UnregisterDeviceNotification(m_usbNotify);
		m_usbNotify = nullptr;
	}

	qApp->removeNativeEventFilter(this);
    emit listenerStopped();
	m_listening = false;
}



bool USBListener::nativeEventFilter(const QByteArray& eventType, void* message, long* result) {
	Q_UNUSED(result);

#ifdef Q_OS_WIN
	if (eventType != "windows_generic_MSG" && eventType != "windows_dispatcher_MSG") {
		return false;
	}
#else
	return false;
#endif

	MSG* msg = static_cast<MSG*>(message);
	if (msg->message != WM_DEVICECHANGE) {
		return false;
	}

	if (msg->wParam != DBT_DEVICEARRIVAL && msg->wParam != DBT_DEVICEREMOVECOMPLETE) {
		return false;
	}

	if (!msg->lParam) {
		qWarning() << "WM_DEVICECHANGE with null lParam";
		return false;
	}

	PDEV_BROADCAST_HDR broadcastHdr = reinterpret_cast<PDEV_BROADCAST_HDR>(msg->lParam);
	QString devicePath;
	usbDevice deviceInfo;

	switch (broadcastHdr->dbch_devicetype) {
	case DBT_DEVTYP_DEVICEINTERFACE: {
		PDEV_BROADCAST_DEVICEINTERFACE devInterface =
			reinterpret_cast<PDEV_BROADCAST_DEVICEINTERFACE>(broadcastHdr);
		devicePath = QString::fromWCharArray(devInterface->dbcc_name);
		deviceInfo = parseDeviceInfo(devicePath);

		// 设置设备类型
		if (IsEqualGUID(devInterface->dbcc_classguid, GUID_DEVINTERFACE_USB_DEVICE)) {
			deviceInfo.type = "USB";
		}
		else if (IsEqualGUID(devInterface->dbcc_classguid, GUID_DEVINTERFACE_DISK)) {
			deviceInfo.type = "USB_DISK";
		}
		else if (IsEqualGUID(devInterface->dbcc_classguid, GUID_DEVINTERFACE_HID)) {
			deviceInfo.type = "USB_HID";
		}
		else {
			return false; // 不处理其他类型
		}
		break;
	}
	default:
		return false;
	}
    if (!isTargetDevice(deviceInfo.VID, deviceInfo.PID)) {
		return false;
    }

	switch (msg->wParam) {
	case DBT_DEVICEARRIVAL:
		emit deviceConnected(deviceInfo);
		QLOG_INFO() << "Device connected:" << deviceInfo.description;
		break;
	case DBT_DEVICEREMOVECOMPLETE:
		emit deviceDisconnected(deviceInfo);
		QLOG_INFO() << "Device disconnected:" << deviceInfo.description;
		break;
	}

	return true;
}


usbDevice USBListener::parseDeviceInfo(const QString& devicePath) {
    usbDevice dev;
    dev.description = devicePath;

    // 解析VID/PID (格式: VID_XXXX&PID_XXXX)
    int vidPos = devicePath.indexOf("VID_", 0, Qt::CaseInsensitive);
    int pidPos = devicePath.indexOf("PID_", 0, Qt::CaseInsensitive);

    if (vidPos != -1 && pidPos != -1) {
        bool ok;
        dev.VID = devicePath.mid(vidPos + 4, 4).toUShort(&ok, 16);
        dev.PID = devicePath.mid(pidPos + 4, 4).toUShort(&ok, 16);
    }

    return dev;
}

bool USBListener::isTargetDevice(uint16_t vid, uint16_t pid) {
    return (vid == m_targetVID && pid == m_targetPID);
}



