/*
 * opencvfindheigh.cpp - 测高系统摄像头图像采集模块
 *
 * 主要功能：
 * 1. 摄像头设备查找和初始化
 * 2. 图像采集和旋转处理
 * 3. 图像保存和路径管理
 * 4. 错误处理和重试机制
 */

#pragma execution_character_set("utf-8")

#include "opencvfindheigh.h"
#include "QsLog/include/QsLog.h"
#include "cglobal.h"
#include "globaldata.h"
#include "ini_file.h"
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include <QVector>
#include <QDateTime>
#include <QCameraInfo>
#include <core/types.hpp>

// 静态常量定义
const QString opencvfindHeigh::DEFAULT_IMAGE_SUBDIR = "FindHighImage";

/*
 * 构造函数 - 初始化测高系统
 */
opencvfindHeigh::opencvfindHeigh(QObject *parent) : QObject(parent)
    , m_isreptestheigh(false)
    , m_openCameraPosition(-1) // 初始化为无效索引
{
    // 创建图像存储目录
    createImageStorageDirectory();
}

/*
 * 析构函数 - 清理资源
 */
opencvfindHeigh::~opencvfindHeigh()
{
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
 * 接收摄像头设备路径并设置索引
 */
void opencvfindHeigh::recvfindCameraIndexByDevicePath(const QString &devicePath)
{
    m_openCameraPosition = findCameraIndexByDevicePathWindows(devicePath);
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
    return;
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
void opencvfindHeigh::handleTriggerTestHeight()
{
    const int MAX_CAMERA_RETRY = 3;
    const int RETRY_DELAY_MS = 100;
    bool isOpened = false;
    cv::VideoCapture capture;

    QLOG_DEBUG() << "尝试打开摄像头，索引:" << m_openCameraPosition;

    // 摄像头打开重试机制
    for (int retry = 0; retry < MAX_CAMERA_RETRY && !isOpened; ++retry) {
        // 尝试使用DirectShow后端打开摄像头
        capture.open(m_openCameraPosition, cv::CAP_DSHOW);

        if (capture.isOpened()) {
            // 设置摄像头参数
            capture.set(cv::CAP_PROP_FRAME_WIDTH, imageWidth);
            capture.set(cv::CAP_PROP_FRAME_HEIGHT, imageHeight);
            capture.set(cv::CAP_PROP_FPS, 30);
            capture.set(cv::CAP_PROP_AUTOFOCUS, 0); // 关闭自动对焦

            // 验证分辨率设置
            double actualWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH);
            double actualHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
            double actualFps = capture.get(cv::CAP_PROP_FPS);

            QLOG_DEBUG() << "摄像头打开成功 - 实际分辨率:"
                        << actualWidth << "x" << actualHeight
                        << "FPS:" << actualFps;

            // 检查分辨率是否匹配
            if (qAbs(actualWidth - imageWidth) > 1 || qAbs(actualHeight - imageHeight) > 1) {
                QLOG_WARN() << "分辨率设置不匹配，期望:"
                            << imageWidth << "x" << imageHeight
                            << "实际:" << actualWidth << "x" << actualHeight;

                // 尝试重新设置分辨率
                capture.set(cv::CAP_PROP_FRAME_WIDTH, imageWidth);
                capture.set(cv::CAP_PROP_FRAME_HEIGHT, imageHeight);
            }
            isOpened = true;
        } else {
            QLOG_DEBUG() << "摄像头打开失败，重试:" << retry + 1 << "/" << MAX_CAMERA_RETRY;

            // 如果是最后一次重试，尝试其他后端
            if (retry == MAX_CAMERA_RETRY - 1) {
                QLOG_DEBUG() << "尝试使用其他后端打开摄像头";
                capture.open(m_openCameraPosition, cv::CAP_ANY);

                if (capture.isOpened()) {
                    isOpened = true;
                    QLOG_DEBUG() << "使用通用后端打开摄像头成功";
                }
            }

            QThread::msleep(RETRY_DELAY_MS);
        }
    }

    // 摄像头打开失败处理
    if (!isOpened) {
        QLOG_ERROR() << "摄像头最终打开失败，索引:" << m_openCameraPosition;

        // 检查摄像头索引是否有效
        if (m_openCameraPosition < 0) {
            emit FindFailed(tr("配置错误"),
                           tr("摄像头索引无效: %1").arg(m_openCameraPosition));
            return;
        }

        emit FindFailed(tr("硬件错误"), tr("无法打开摄像头设备"));
        return;
    }

    // 捕获帧允许重试
    cv::Mat frame;
    int maxRetry = 5;
    bool frameCaptured = false;

    while (maxRetry-- > 0 && !frameCaptured) {
        frameCaptured = capture.read(frame);

        if (!frameCaptured) {
            QLOG_DEBUG() << "帧捕获失败，剩余重试次数:" << maxRetry;

            // 添加不同类型的等待策略
            if (maxRetry > 2) {
                // 前几次重试使用较短等待
                cv::waitKey(1);
            } else {
                // 后几次重试使用较长等待，让摄像头有时间恢复
                QThread::msleep(50);
                cv::waitKey(10);
            }

            // 检查摄像头是否仍然打开
            if (!capture.isOpened()) {
                QLOG_WARN() << "摄像头在捕获过程中意外关闭";
                break;
            }
        }
    }

    // 确保释放摄像头资源
    if (capture.isOpened()) {
        capture.release();
        QLOG_DEBUG() << "摄像头资源已释放";
    }

    // 检查帧是否有效
    if (frame.empty() || !frameCaptured) {
        QLOG_WARN() << "无法获取有效帧，重试次数耗尽";
        emit FindFailed(tr("采集错误"), tr("摄像头画面获取超时"));
        return;
    }

    // 验证帧的尺寸和格式
    if (frame.cols <= 0 || frame.rows <= 0) {
        QLOG_WARN() << "获取到无效尺寸的帧:" << frame.cols << "x" << frame.rows;
        emit FindFailed(tr("采集错误"), tr("摄像头返回无效图像尺寸"));
        return;
    }

    QLOG_DEBUG() << "成功捕获帧，尺寸:" << frame.cols << "x" << frame.rows
                << "类型:" << frame.type() << "通道:" << frame.channels();

    // 图像旋转处理
    cv::Mat rotatedFrame;
    try {
        pinwheel(frame, 90, rotatedFrame); // 旋转90度

        if (rotatedFrame.empty()) {
            QLOG_ERROR() << "旋转后图像为空";
            emit FindFailed(tr("处理错误"), tr("图像旋转后为空"));
            return;
        }
    } catch(const cv::Exception& e) {
        QLOG_ERROR() << "图像旋转失败:" << e.what();
        emit FindFailed(tr("处理错误"), tr("图像旋转操作异常"));
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
        emit FindFailed(tr("保存错误"), tr("图像保存失败"));
    } else {
        loadImageState = true;

        QLOG_DEBUG() << "检测路径" << tempSavePath;
        // 根据是否替换模式发送不同信号
        if(m_isreptestheigh) {
            emit reOpencvImageTubePRP(m_replaceid, tempSavePath);
        } else {
            emit obtainPRPImage(tempSavePath);
        }
    }

    emit Testheightfinish(loadImageState);
    return;
}
