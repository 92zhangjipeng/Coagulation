/*
 * opencvfindheigh.h - 测高系统摄像头图像采集模块头文件
 *
 * 类定义：opencvfindHeigh
 * 功能：负责摄像头图像采集、旋转处理和保存
 *
 * 主要特性：
 * - 摄像头设备查找和初始化
 * - 图像采集和旋转处理
 * - 图像保存和路径管理
 * - 错误处理和重试机制
 */

#ifndef OPENCVFINDHEIGH_H
#define OPENCVFINDHEIGH_H

// Windows平台特定包含
#ifdef Q_OS_WINDOWS
#include <windows.h>
#include <dshow.h>
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ole32.lib")
#endif

#pragma execution_character_set("utf-8")

// Qt核心库
#include <QObject>
#include <QMessageBox>
#include <QRect>

// OpenCV库
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

// 标准库
#include <string>
#include <io.h>

// 项目自定义头文件
#include "correct_data.h"

/*
 * opencvfindHeigh类 - 测高系统摄像头图像采集模块
 *
 * 继承自QObject，支持Qt信号槽机制
 * 负责管理摄像头设备、图像采集和处理流程
 */
class opencvfindHeigh : public QObject
{
    Q_OBJECT

public:
    /*
     * 构造函数
     * @param parent 父对象指针
     */
    explicit opencvfindHeigh(QObject *parent = nullptr);

    /*
     * 析构函数
     */
    ~opencvfindHeigh();

    // 静态常量定义
    static const QString DEFAULT_IMAGE_SUBDIR; ///< 默认图像存储子目录名称

    /*
     * 创建图像存储目录
     * @return true-创建成功，false-创建失败
     */
    bool createImageStorageDirectory();

    /*
     * 获取图像存储路径
     * @return 图像存储路径字符串引用
     */
    const QString& getImagePath() const { return m_imagePath; }

public slots:
    /*
     * 启动测高线程
     */
    void Start();

    /*
     * 接收摄像头设备路径并设置索引
     * @param devicePath 摄像头设备路径
     */
    void recvfindCameraIndexByDevicePath(const QString& devicePath);

    /*
     * 开始拍照测高 - 主处理函数
     */
    void handleTriggerTestHeight();

    /*
     * 处理PRP高度替换请求
     * @param numid 替换ID
     * @param replflag 是否替换标志
     */
    void handleRepPrpheight(const QString &numid, bool replflag);

private:
    /*
     * 根据设备路径查找摄像头索引（Windows平台）
     * @param targetDevicePath 目标设备路径
     * @return 摄像头索引，-1表示未找到
     */
    int findCameraIndexByDevicePathWindows(const QString &targetDevicePath);

    /*
     * 图像旋转函数
     * @param inputImage 输入图像
     * @param degree 旋转角度（度）
     * @param outputImage 输出图像
     * @return true-旋转成功，false-旋转失败
     */
    bool pinwheel(const cv::Mat& inputImage, int degree, cv::Mat& outputImage);

signals:
    /*
     * 获取PRP图像信号
     * @param pathImage 图像文件路径
     */
    void obtainPRPImage(const QString &pathImage);

    /*
     * 重新打开PRP图像信号（替换模式）
     * @param reId 替换ID
     * @param pathImage 图像文件路径
     */
    void reOpencvImageTubePRP(const QString &reId, const QString &pathImage);

    /*
     * 测高完成信号
     * @param success 是否成功完成
     */
    void Testheightfinish(const bool success);

    /*
     * 查找失败信号
     * @param errorType 错误类型
     * @param errorMessage 错误消息
     */
    void FindFailed(QString errorType, QString errorMessage);

private:
    // 图像存储路径
    QString m_imagePath;

    // 替换相关变量
    QString m_replaceid;      ///< 替换ID
    bool m_isreptestheigh;    ///< 是否替换模式标志

    // 摄像头相关变量
    int m_openCameraPosition; ///< 打开的摄像头索引

    // 图像尺寸常量
    const int imageWidth = 480;   ///< 图像宽度（像素）
    const int imageHeight = 640;  ///< 图像高度（像素）
};

#endif // OPENCVFINDHEIGH_H
