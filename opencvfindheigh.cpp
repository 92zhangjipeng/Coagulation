#include "opencvfindheigh.h"
#include "QsLog/include/QsLog.h"
#include "cglobal.h"
#include "globaldata.h"
#include "ini_file.h"
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include <QTimer>
#include <QMutexLocker>
#include <QElapsedTimer>
#include <QVector>
#include <QDateTime>
#include <QCameraInfo>
#include <core/types.hpp>

#ifdef _MSC_VER
#pragma execution_character_set("utf-8")
#endif

// 静态常量定义
const QString opencvfindHeigh::DEFAULT_IMAGE_SUBDIR = "FindHighImage";
static QString s_cameraDevicePath;

/*
 * 构造函数 - 初始化测高系统
 */
opencvfindHeigh::opencvfindHeigh(QObject *parent) : QObject(parent)
    , m_isreptestheigh(false)
    , m_openCameraPosition(-1)
    , m_liveTimer(nullptr)
    , m_lastSavedHash(0)
{
    m_cameraWanted.store(INI_File().GetWholeBloodModel());
    createImageStorageDirectory();
}

/*
 * 析构函数 - 清理资源
 */
opencvfindHeigh::~opencvfindHeigh()
{
    m_abort.store(true);
    m_cameraWanted.store(false);
    if (m_liveTimer) {
        m_liveTimer->stop();
    }
    releaseCapture();
    QLOG_DEBUG() << "退出测高opencv实例";
}

/*
 * 根据设备路径查找摄像头索引（Windows平台）
 *
 * @param targetDevicePath 目标设备路径
 * @return 摄像头索引，-1表示未找到
 */
int opencvfindHeigh::findCameraIndexByDevicePathWindows(const QString &targetDevicePath)
{
    QLOG_DEBUG() << "查找设备路径:" << targetDevicePath;

    // 获取可用摄像头列表
    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();

    // 遍历所有摄像头进行匹配
    for (int i = 0; i < cameras.size(); i++) {
        QLOG_DEBUG() << "摄像头" << i << ":" << cameras[i].deviceName()
                    << "描述:" << cameras[i].description();

        // 检查是否匹配目标设备（不区分大小写）
        if (cameras[i].deviceName().contains(targetDevicePath, Qt::CaseInsensitive)) {
            QLOG_DEBUG() << "找到匹配的摄像头，尝试索引:" << i;

            // 验证索引是否有效
            cv::VideoCapture cap(i, cv::CAP_DSHOW);
            if (cap.isOpened()) {
                QLOG_DEBUG() << "索引" << i << "验证成功";
                cap.release();
                return i;
            }
            cap.release(); // 确保资源释放
        }
    }

    return -1; // 未找到匹配的摄像头
}

/*
 * 创建图像存储目录
 *
 * @return true-创建成功，false-创建失败
 */
bool opencvfindHeigh::createImageStorageDirectory()
{
    // 获取应用路径
    const QString appDir = QCoreApplication::applicationDirPath();
    if (appDir.isEmpty()) {
        QLOG_ERROR() << "无法获取应用程序路径";
        return false;
    }

    // 构建完整路径
    m_imagePath = QDir::cleanPath(appDir + QDir::separator() + DEFAULT_IMAGE_SUBDIR);

    // 检查写入权限（提前失败）
    QFileInfo appDirInfo(appDir);
    if (!appDirInfo.isWritable()) {
        QLOG_ERROR() << "应用程序目录无写入权限:"
                     << QDir::toNativeSeparators(appDir);
        return false;
    }

    // 高效创建目录（如果已存在则跳过）
    QDir dir;
    if (dir.exists(m_imagePath)) {
        QLOG_DEBUG() << "测高图片文件夹已存在，路径:"
                    << QDir::toNativeSeparators(m_imagePath);
        return true;
    }

    if (!dir.mkpath(m_imagePath)) {
        QLOG_ERROR() << "创建测高图片文件夹失败，路径: "
                    << QDir::toNativeSeparators(m_imagePath);
        m_imagePath.clear(); // 失败时清空路径
        return false;
    }

    QLOG_INFO() << "测高图片文件夹创建成功，路径:"
               << QDir::toNativeSeparators(m_imagePath);
    return true;
}

/*
 * 图像旋转函数
 *
 * @param inputImage 输入图像
 * @param degree 旋转角度（度）
 * @param outputImage 输出图像
 * @return true-旋转成功，false-旋转失败
 */
bool opencvfindHeigh::pinwheel(const cv::Mat& inputImage, int degree, cv::Mat& outputImage)
{
    // 输入验证
    if (inputImage.empty()) {
        QLOG_DEBUG() << "输入图像为空";
        return false;
    }

    // 计算旋转后的包围盒尺寸
    const double angle_rad = degree * CV_PI / 180.0;
    const int width = inputImage.cols;
    const int height = inputImage.rows;

    // 精确计算旋转后的图像尺寸
    const double cos_theta = fabs(std::cos(angle_rad));
    const double sin_theta = fabs(std::sin(angle_rad));
    const int new_width = cvRound(width * cos_theta + height * sin_theta);
    const int new_height = cvRound(width * sin_theta + height * cos_theta);

    // 生成旋转矩阵 (自动计算中心点平移)
    const cv::Point2f center(width / 2.0f, height / 2.0f);
    cv::Mat rotationMatrix = cv::getRotationMatrix2D(center, degree, 1.0);

    // 调整平移分量使图像居中
    rotationMatrix.at<double>(0, 2) += (new_width - width) / 2.0;
    rotationMatrix.at<double>(1, 2) += (new_height - height) / 2.0;

    // 执行旋转变换
    cv::warpAffine(
        inputImage,
        outputImage,
        rotationMatrix,
        cv::Size(new_width, new_height),
        cv::INTER_LINEAR,
        cv::BORDER_CONSTANT,
        cv::Scalar()
    );

    return true;
}

/*
 * 启动测高线程
 */
void opencvfindHeigh::Start()
{
    QLOG_DEBUG() << "启动测高线程:" << QThread::currentThreadId();
    if (!m_liveTimer) {
        m_liveTimer = new QTimer(this);
        m_liveTimer->setInterval(33);
        connect(m_liveTimer, &QTimer::timeout, this, &opencvfindHeigh::drainLiveFrame);
    }
    if (m_cameraWanted.load()) {
        setCameraPower(true);
    }
}

void opencvfindHeigh::recvfindCameraIndexByDevicePath(const QString &devicePath)
{
    s_cameraDevicePath = devicePath;
    m_openCameraPosition = findCameraIndexByDevicePathWindows(devicePath);
    QLOG_DEBUG() << "测高摄像头索引:" << m_openCameraPosition;
    if (m_cameraWanted.load() && m_openCameraPosition >= 0) {
        setCameraPower(true);
    }
}

quint64 opencvfindHeigh::hashFrame(const cv::Mat &frame) const
{
    if (frame.empty() || !frame.data) {
        return 0;
    }
    quint64 h = (static_cast<quint64>(frame.cols) << 32)
            | static_cast<quint64>(frame.rows);
    const int channels = frame.channels();
    const int step = qMax(1, (frame.rows * frame.cols * channels) / 2500);
    for (int y = 0; y < frame.rows; y += 4) {
        const uchar *row = frame.ptr<uchar>(y);
        const int rowBytes = frame.cols * channels;
        for (int i = 0; i < rowBytes; i += step) {
            h = h * 131ull + row[i];
        }
    }
    return h;
}

void opencvfindHeigh::drainLiveFrame()
{
    if (m_abort.load() || !m_cameraWanted.load()) {
        return;
    }
    if (!m_capture.isOpened()) {
        return;
    }
    cv::Mat f;
    if (!m_capture.read(f) || f.empty()) {
        return;
    }
    QMutexLocker locker(&m_frameMutex);
    m_liveFrame = f;
}

void opencvfindHeigh::setCameraPower(bool enable)
{
    m_cameraWanted.store(enable);
    if (!enable) {
        if (m_liveTimer) {
            m_liveTimer->stop();
        }
        releaseCapture();
        {
            QMutexLocker locker(&m_frameMutex);
            m_liveFrame.release();
        }
        m_lastSavedHash = 0;
        QLOG_INFO() << "[HeightCam] 血浆模式，摄像头已关闭";
        return;
    }

    if (!m_liveTimer) {
        m_liveTimer = new QTimer(this);
        m_liveTimer->setInterval(33);
        connect(m_liveTimer, &QTimer::timeout, this, &opencvfindHeigh::drainLiveFrame);
    }
    if (!ensureCaptureOpen()) {
        QLOG_WARN() << "[HeightCam] 全血模式打开摄像头失败";
        return;
    }
    m_liveTimer->start();
    drainLiveFrame();
    QLOG_INFO() << "[HeightCam] 全血模式，摄像头已打开并持续取流";
}

bool opencvfindHeigh::takeFreshFrame(cv::Mat &outFrame)
{
    outFrame.release();
    QElapsedTimer wait;
    wait.start();
    quint64 h = 0;
    while (wait.elapsed() < 450) {
        drainLiveFrame();
        {
            QMutexLocker locker(&m_frameMutex);
            if (!m_liveFrame.empty()) {
                outFrame = m_liveFrame.clone();
            }
        }
        h = hashFrame(outFrame);
        if (!outFrame.empty() && h != 0 && h != m_lastSavedHash) {
            m_lastSavedHash = h;
            return true;
        }
        QThread::msleep(20);
    }
    if (!outFrame.empty()) {
        m_lastSavedHash = h;
        QLOG_WARN() << "[HeightCam] 取流超时，使用当前帧 hash=" << h;
        return true;
    }
    return false;
}



/*
 * 处理PRP高度替换请求
 */
void opencvfindHeigh::handleRepPrpheight(const QString &numid, bool replflag)
{
    m_replaceid = numid;
    m_isreptestheigh = replflag;
}

/*
 * 开始拍照测高 - 主处理函数
 *
 * 功能流程：
 * 1. 打开摄像头
 * 2. 捕获图像帧
 * 3. 图像旋转处理
 * 4. 保存图像
 * 5. 发送结果信号
 */
void opencvfindHeigh::releaseCapture()
{
    if (m_capture.isOpened()) {
        m_capture.release();
        QLOG_DEBUG() << "摄像头资源已释放";
    }
}

#ifdef _WIN32
static QString monikerDisplayName(IMoniker *moniker)
{
    if (!moniker) {
        return QString();
    }
    LPOLESTR name = NULL;
    if (FAILED(moniker->GetDisplayName(NULL, NULL, &name)) || !name) {
        return QString();
    }
    const QString text = QString::fromWCharArray(name);
    CoTaskMemFree(name);
    return text;
}

static bool cameraNameMatch(const QString &displayName, int deviceIndex)
{
    if (deviceIndex < 0) {
        return false;
    }
    if (!s_cameraDevicePath.isEmpty() && !displayName.isEmpty()) {
        if (displayName.contains(s_cameraDevicePath, Qt::CaseInsensitive)
                || s_cameraDevicePath.contains(displayName, Qt::CaseInsensitive)) {
            return true;
        }
        const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
        if (deviceIndex < cameras.size()) {
            const QString qtName = cameras[deviceIndex].deviceName();
            if (!qtName.isEmpty()
                    && (displayName.contains(qtName, Qt::CaseInsensitive)
                        || qtName.contains(displayName, Qt::CaseInsensitive))) {
                return true;
            }
        }
    }
    return false;
}

static bool setCameraControlAuto(IAMCameraControl *camCtl)
{
    long minV = 0;
    long maxV = 0;
    long step = 0;
    long defV = 0;
    long caps = 0;
    HRESULT hr = camCtl->GetRange(CameraControl_Exposure, &minV, &maxV,
                                  &step, &defV, &caps);
    QLOG_INFO() << "[HeightCam] Exposure范围 min=" << minV << " max=" << maxV
                << " def=" << defV << " caps=" << caps
                << " GetRange hr=0x" << QString::number(static_cast<quint32>(hr), 16);
    long cur = 0;
    long flags = 0;
    camCtl->Get(CameraControl_Exposure, &cur, &flags);
    QLOG_INFO() << "[HeightCam] Exposure当前值=" << cur << " flags=" << flags
                << (flags & CameraControl_Flags_Auto ? " (已是自动)" : " (手动)");

    const long autoFlags[] = {
        CameraControl_Flags_Auto,
        CameraControl_Flags_Auto | CameraControl_Flags_Manual
    };
    for (int i = 0; i < 2; ++i) {
        hr = camCtl->Set(CameraControl_Exposure, defV, autoFlags[i]);
        camCtl->Get(CameraControl_Exposure, &cur, &flags);
        QLOG_INFO() << "[HeightCam] Set自动曝光 flags=" << autoFlags[i]
                    << " hr=0x" << QString::number(static_cast<quint32>(hr), 16)
                    << " 读回 flags=" << flags << " 值=" << cur;
        if (SUCCEEDED(hr) && (flags & CameraControl_Flags_Auto)) {
            return true;
        }
    }
    return (flags & CameraControl_Flags_Auto) != 0;
}

static bool enableDirectShowExposureAuto(int deviceIndex)
{
    if (deviceIndex < 0) {
        QLOG_WARN() << "[HeightCam] DirectShow自动曝光跳过，索引无效";
        return false;
    }
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    const bool needUninit = (hr == S_OK);
    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE && hr != S_FALSE) {
        QLOG_WARN() << "[HeightCam] DirectShow COM 初始化失败 hr=0x"
                    << QString::number(static_cast<quint32>(hr), 16);
        return false;
    }

    ICreateDevEnum *devEnum = NULL;
    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
                          IID_ICreateDevEnum, reinterpret_cast<void **>(&devEnum));
    if (FAILED(hr) || !devEnum) {
        QLOG_WARN() << "[HeightCam] SystemDeviceEnum 失败 hr=0x"
                    << QString::number(static_cast<quint32>(hr), 16);
        if (needUninit) {
            CoUninitialize();
        }
        return false;
    }

    IEnumMoniker *enumMon = NULL;
    hr = devEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &enumMon, 0);
    devEnum->Release();
    if (hr != S_OK || !enumMon) {
        QLOG_WARN() << "[HeightCam] 没有 DirectShow 视频设备 hr=0x"
                    << QString::number(static_cast<quint32>(hr), 16);
        if (needUninit) {
            CoUninitialize();
        }
        return false;
    }

    IMoniker *moniker = NULL;
    int idx = 0;
    bool ok = false;
    bool matched = false;
    while (enumMon->Next(1, &moniker, NULL) == S_OK) {
        const QString display = monikerDisplayName(moniker);
        const bool byName = cameraNameMatch(display, deviceIndex);
        const bool byIndex = (idx == deviceIndex);
        QLOG_INFO() << "[HeightCam] DirectShow设备" << idx << display
                    << " 名匹配=" << byName << " 序匹配=" << byIndex;
        if (byName || byIndex) {
            matched = true;
            IBaseFilter *filter = NULL;
            hr = moniker->BindToObject(NULL, NULL, IID_IBaseFilter,
                                       reinterpret_cast<void **>(&filter));
            if (FAILED(hr) || !filter) {
                QLOG_WARN() << "[HeightCam] BindToObject 失败 hr=0x"
                            << QString::number(static_cast<quint32>(hr), 16)
                            << "（OpenCV占用设备时常见）";
            } else {
                IAMCameraControl *camCtl = NULL;
                hr = filter->QueryInterface(IID_IAMCameraControl,
                                            reinterpret_cast<void **>(&camCtl));
                if (FAILED(hr) || !camCtl) {
                    QLOG_WARN() << "[HeightCam] 无 IAMCameraControl hr=0x"
                                << QString::number(static_cast<quint32>(hr), 16);
                } else {
                    ok = setCameraControlAuto(camCtl);
                    camCtl->Release();
                }
                filter->Release();
            }
            moniker->Release();
            break;
        } else {
            moniker->Release();
        }
        ++idx;
    }
    enumMon->Release();
    if (!matched) {
        QLOG_WARN() << "[HeightCam] DirectShow 未匹配到相机 索引=" << deviceIndex
                    << " 路径=" << s_cameraDevicePath;
    }
    if (needUninit) {
        CoUninitialize();
    }
    return ok;
}
#endif

void opencvfindHeigh::applyWindowsCameraLikeParams()
{
    const int captureW = 640;
    const int captureH = 480;
    // 不要 set MJPEG：本机驱动会回报成功，实际仍是 YUY2，而且会重启 graph 冲掉自动曝光。
    const int curW = qRound(m_capture.get(cv::CAP_PROP_FRAME_WIDTH));
    const int curH = qRound(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    if (curW != captureW || curH != captureH) {
        m_capture.set(cv::CAP_PROP_FRAME_WIDTH, captureW);
        m_capture.set(cv::CAP_PROP_FRAME_HEIGHT, captureH);
    }
    m_capture.set(cv::CAP_PROP_FPS, 30);
    m_capture.set(cv::CAP_PROP_CONVERT_RGB, 1);
    m_capture.set(cv::CAP_PROP_BUFFERSIZE, 1);

    cv::Mat kick;
    m_capture.read(kick);

    const bool ae1 = m_capture.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
    const bool ae075 = m_capture.set(cv::CAP_PROP_AUTO_EXPOSURE, 0.75);
#ifdef _WIN32
    const bool dshowAe = enableDirectShowExposureAuto(m_openCameraPosition);
#else
    const bool dshowAe = false;
#endif
    QLOG_INFO() << "[HeightCam] 曝光设置 OpenCV_AE1=" << ae1
                << " OpenCV_AE0.75=" << ae075
                << " DirectShow自动曝光=" << dshowAe
                << " 开图后尺寸" << qRound(m_capture.get(cv::CAP_PROP_FRAME_WIDTH))
                << "x" << qRound(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT));
}

void opencvfindHeigh::warmupCaptureFrames()
{
    cv::Mat warm;
    for (int i = 0; i < 15 && !m_abort.load(); ++i) {
        if (!m_capture.read(warm) || warm.empty()) {
            break;
        }
    }
}

void opencvfindHeigh::logCaptureParams(const char *backendName)
{
    const int fourcc = static_cast<int>(m_capture.get(cv::CAP_PROP_FOURCC));
    char fcc[5] = {
        static_cast<char>(fourcc & 0xFF),
        static_cast<char>((fourcc >> 8) & 0xFF),
        static_cast<char>((fourcc >> 16) & 0xFF),
        static_cast<char>((fourcc >> 24) & 0xFF),
        0
    };
    QLOG_INFO() << "[HeightCam] 采集参数 后端=" << backendName
                << " 尺寸" << qRound(m_capture.get(cv::CAP_PROP_FRAME_WIDTH))
                << "x" << qRound(m_capture.get(cv::CAP_PROP_FRAME_HEIGHT))
                << " fps=" << qRound(m_capture.get(cv::CAP_PROP_FPS))
                << " fourcc=" << fcc
                << " 自动曝光=" << m_capture.get(cv::CAP_PROP_AUTO_EXPOSURE)
                << " 曝光=" << m_capture.get(cv::CAP_PROP_EXPOSURE)
                << " 增益=" << m_capture.get(cv::CAP_PROP_GAIN)
                << " 亮度=" << m_capture.get(cv::CAP_PROP_BRIGHTNESS)
                << " 自动白平衡=" << m_capture.get(cv::CAP_PROP_AUTO_WB);
}

bool opencvfindHeigh::ensureCaptureOpen()
{
    if (m_capture.isOpened()) {
        return true;
    }
    if (m_openCameraPosition < 0) {
        return false;
    }

    const int MAX_CAMERA_RETRY = 3;
    const int RETRY_DELAY_MS = 100;
    qputenv("OPENCV_VIDEOIO_MSMF_ENABLE_HW_TRANSFORMS", "0");
#ifdef _WIN32
    enableDirectShowExposureAuto(m_openCameraPosition);
#endif
    const int backends[] = { cv::CAP_MSMF, cv::CAP_DSHOW };
    const char *backendNames[] = { "MSMF", "DSHOW" };
    for (int retry = 0; retry < MAX_CAMERA_RETRY; ++retry) {
        if (m_abort.load()) {
            releaseCapture();
            return false;
        }
        for (int b = 0; b < 2; ++b) {
            m_capture.open(m_openCameraPosition, backends[b]);
            if (!m_capture.isOpened()) {
                QLOG_INFO() << "[HeightCam] 打开失败 后端=" << backendNames[b]
                            << " 索引=" << m_openCameraPosition;
                continue;
            }
            applyWindowsCameraLikeParams();
            warmupCaptureFrames();
            logCaptureParams(backendNames[b]);
            QLOG_DEBUG() << "摄像头打开成功，索引:" << m_openCameraPosition
                         << " 后端:" << backendNames[b];
            return true;
        }
        if (retry == MAX_CAMERA_RETRY - 1) {
            m_capture.open(m_openCameraPosition, cv::CAP_ANY);
            if (m_capture.isOpened()) {
                applyWindowsCameraLikeParams();
                warmupCaptureFrames();
                logCaptureParams("ANY");
                QLOG_DEBUG() << "摄像头打开成功，索引:" << m_openCameraPosition
                             << " 后端: ANY";
                return true;
            }
        }
        QLOG_DEBUG() << "摄像头打开失败，重试:" << retry + 1 << "/" << MAX_CAMERA_RETRY;
        QThread::msleep(RETRY_DELAY_MS);
    }
    return false;
}
void opencvfindHeigh::handleTriggerTestHeight()
{
    m_abort.store(false);

    if (!m_cameraWanted.load()) {
        emit FindFailed(tr("模式限制"), tr("血浆模式不支持测高"));
        return;
    }

    QLOG_DEBUG() << "尝试打开摄像头，索引:" << m_openCameraPosition;
    if (!ensureCaptureOpen()) {
        QLOG_ERROR() << "摄像头最终打开失败，索引:" << m_openCameraPosition;
        if (m_openCameraPosition < 0) {
            emit FindFailed(tr("配置错误"),
                           tr("摄像头索引无效: %1").arg(m_openCameraPosition));
        } else {
            emit FindFailed(tr("硬件错误"), tr("无法打开摄像头设备"));
        }
        return;
    }
    if (m_liveTimer && !m_liveTimer->isActive()) {
        m_liveTimer->start();
    }

    cv::Mat frame;
    if (!takeFreshFrame(frame)) {
        QLOG_WARN() << "无法获取有效帧";
        emit FindFailed(tr("采集错误"), tr("摄像头画面获取超时"));
        return;
    }

    if (m_abort.load()) {
        return;
    }

    if (frame.cols <= 0 || frame.rows <= 0) {
        QLOG_WARN() << "获取到无效尺寸的帧:" << frame.cols << "x" << frame.rows;
        emit FindFailed(tr("采集错误"), tr("摄像头返回无效图像尺寸"));
        return;
    }

    QLOG_INFO() << "[HeightCam] 原始帧" << frame.cols << "x" << frame.rows
                << " 通道" << frame.channels();

    cv::Mat rotatedFrame = frame;
    try {
        if (frame.rows > frame.cols) {
            cv::Mat landscape;
            cv::transpose(frame, landscape);
            cv::flip(landscape, landscape, 1);
            rotatedFrame = landscape;
            QLOG_INFO() << "[HeightCam] 竖图转横图" << frame.cols << "x" << frame.rows
                        << " -> " << rotatedFrame.cols << "x" << rotatedFrame.rows;
        } else {
            QLOG_INFO() << "[HeightCam] 已是横图，不旋转" << rotatedFrame.cols << "x" << rotatedFrame.rows
                        << " (宽>高=" << (rotatedFrame.cols > rotatedFrame.rows) << ")";
        }
        if (rotatedFrame.empty()) {
            QLOG_ERROR() << "[HeightCam] 输出图像为空";
            emit FindFailed(tr("处理错误"), tr("图像处理后为空"));
            return;
        }
    } catch(const cv::Exception& e) {
        QLOG_ERROR() << "[HeightCam] 图像处理失败:" << e.what();
        emit FindFailed(tr("处理错误"), tr("图像处理异常"));
        return;
    }

    // 验证处理后的图像
    if(rotatedFrame.empty() || rotatedFrame.channels() != 3) {
        QLOG_WARN() << "无效的输出图像，通道数:"
                  << (rotatedFrame.empty() ? 0 : rotatedFrame.channels());
        emit FindFailed(tr("数据异常"), tr("图像格式不正确"));
        return;
    }

    // 输出拍照的图像
    bool loadImageState = false;
    const QString tempSavePath = QString("%1/opencvPRP.jpg").arg(m_imagePath);

    std::string outpath = tempSavePath.toLocal8Bit().toStdString();
    if(!cv::imwrite(outpath, rotatedFrame)) {
        QLOG_WARN() << "图像保存失败" << tempSavePath;
        loadImageState = false;
        if (!m_abort.load()) {
            emit FindFailed(tr("保存错误"), tr("图像保存失败"));
        }
    } else {
        loadImageState = true;

        QLOG_INFO() << "[HeightCam] 已保存" << tempSavePath
                    << rotatedFrame.cols << "x" << rotatedFrame.rows;
        if (!m_abort.load()) {
            // 根据是否替换模式发送不同信号
            if(m_isreptestheigh) {
                emit reOpencvImageTubePRP(m_replaceid, tempSavePath);
            } else {
                emit obtainPRPImage(tempSavePath);
            }
        }
    }

    if (!m_abort.load()) {
        emit Testheightfinish(loadImageState);
    }
    return;
}
