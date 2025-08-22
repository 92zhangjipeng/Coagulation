#include "USBListener.h"
#include <QsLog/include/QsLog.h>
#include <QCoreApplication>
#include <QDebug>
#include "cglobal.h"
// 确保包含必要的头文件
#include <hidsdi.h>
#include <hidpi.h>

#include <QTimer>
#include <QMetaObject>


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



QString returnErrStr(QString formattedErr) {
    QString outFailed;
    const int outCode = formattedErr.toInt();
    switch (outCode) {
            case 5: outFailed = "权限不足";  break;  //- 0x5 (ACCESS_DENIED): 权限不足
            case 2: outFailed = "设备不存在";break; // - 0x2 (ERROR_FILE_NOT_FOUND): 设备不存在
        default:
            outFailed ="未知异常";
            break;
    }
    return outFailed;
}

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
        outFailed = "USB电源禁用失败(未找到设备)";
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return false;
    }

    // 3. 禁用选择性暂停 - 正确的方法
    HKEY hDeviceKey = SetupDiOpenDevRegKey(hDevInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_WRITE);
    if (hDeviceKey == INVALID_HANDLE_VALUE) {
        err = GetLastError();
        formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
        QLOG_WARN() << "USB电源禁用失败(打开设备注册表键): 0x" << formattedErr;
        outFailed = returnErrStr(formattedErr);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return false;
    }

    // 4. 设置注册表值来禁用选择性暂停
    DWORD disableSelectiveSuspend = 1; // 1 = 禁用选择性暂停
    LONG regResult = RegSetValueExW(hDeviceKey,
        L"DisableSelectiveSuspend",
        0,
        REG_DWORD,
        (const BYTE*)&disableSelectiveSuspend,
        sizeof(disableSelectiveSuspend));

    RegCloseKey(hDeviceKey);

    if (regResult != ERROR_SUCCESS) {
        formattedErr = QString("%1").arg(regResult, 8, 16, QChar('0')).toUpper();
        QLOG_WARN() << "USB电源禁用失败(设置注册表值): 0x" << formattedErr;
        outFailed = returnErrStr(formattedErr);
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return false;
    }

    // 5. 重启设备以使更改生效（可选但推荐）
    SP_PROPCHANGE_PARAMS propChangeParams = { 0 };
    propChangeParams.ClassInstallHeader.cbSize = sizeof(SP_CLASSINSTALL_HEADER);
    propChangeParams.ClassInstallHeader.InstallFunction = DIF_PROPERTYCHANGE;
    propChangeParams.StateChange = DICS_PROPCHANGE; // 属性更改而不是禁用
    propChangeParams.Scope = DICS_FLAG_CONFIGSPECIFIC;
    propChangeParams.HwProfile = 0;

    if (!SetupDiSetClassInstallParams(hDevInfo, &devInfoData,
        (SP_CLASSINSTALL_HEADER*)&propChangeParams,
        sizeof(propChangeParams))) {
        err = GetLastError();
        formattedErr = QString("%1").arg(err, 8, 16, QChar('0')).toUpper();
        QLOG_WARN() << "警告: 无法重启设备应用更改: 0x" << formattedErr;
    }
    else {
        SetupDiCallClassInstaller(DIF_PROPERTYCHANGE, hDevInfo, &devInfoData);
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
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
            outmessage = "设备的 USB电源 选择性暂停已禁用!";
            isstate = NORMALLOG;
            QLOG_DEBUG() << outmessage;
        }
        else {
            outmessage = "设备的 USB电源 选择性暂停禁用失败!(" + knowErr + ")";
            isstate = ERRORLOG;
            QLOG_ERROR() << outmessage;
        }
    }
    Q_EMIT usbPowerContl(outmessage, isstate);
}


bool USBListener::startListening() {
    if (m_listening) return true;
    // 使用单次定时器在后台执行耗时操作
    QTimer::singleShot(0, this, [this]() {
        this->initDisAblePower();

        // 回到主线程继续执行
        QMetaObject::invokeMethod(this, [this]() {
            if (!registerDeviceNotifications()) {
                QLOG_ERROR() << "Failed to register device notifications";
                m_hwnd = nullptr;
                emit errorOccurred("无法启动USB监听");
                return;
            }

            qApp->installNativeEventFilter(this);
            m_listening = true;
            emit listenerStarted();
        }, Qt::QueuedConnection);
    });

    return true;



    /*initDisAblePower();
    if (!registerDeviceNotifications()) {
        QLOG_ERROR() << "Failed to register device notifications";
        m_hwnd = nullptr;
        emit errorOccurred("无法启动USB监听");
        return false;
    }

    qApp->installNativeEventFilter(this);
    m_listening = true;
    emit listenerStarted();
    return true;*/
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

