#ifndef OPENCVFINDHEIGH_H
#define OPENCVFINDHEIGH_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <dshow.h>
#ifdef _MSC_VER
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "ole32.lib")
#endif
#endif

#include <QObject>
#include <QMessageBox>
#include <QRect>
#include <QMutex>
#include <QTimer>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <string>
#include <io.h>
#include <atomic>

#include "correct_data.h"

class opencvfindHeigh : public QObject
{
    Q_OBJECT

public:
    explicit opencvfindHeigh(QObject *parent = nullptr);
    ~opencvfindHeigh();

    static const QString DEFAULT_IMAGE_SUBDIR;

    bool createImageStorageDirectory();
    const QString& getImagePath() const { return m_imagePath; }
    bool hasOpenedCamera() const { return m_openCameraPosition >= 0; }

public slots:
    void Start();
    void recvfindCameraIndexByDevicePath(const QString& devicePath);
    void handleTriggerTestHeight();
    void handleRepPrpheight(const QString &numid, bool replflag);
    void setCameraPower(bool enable);

private:
    int findCameraIndexByDevicePathWindows(const QString &targetDevicePath);
    bool pinwheel(const cv::Mat& inputImage, int degree, cv::Mat& outputImage);
    bool ensureCaptureOpen();
    void releaseCapture();
    void applyWindowsCameraLikeParams();
    void warmupCaptureFrames();
    void logCaptureParams(const char *backendName);
    void drainLiveFrame();
    quint64 hashFrame(const cv::Mat &frame) const;
    bool takeFreshFrame(cv::Mat &outFrame);

signals:
    void obtainPRPImage(const QString &pathImage);
    void reOpencvImageTubePRP(const QString &reId, const QString &pathImage);
    void Testheightfinish(const bool success);
    void FindFailed(QString errorType, QString errorMessage);

private:
    QString m_imagePath;
    QString m_replaceid;
    bool m_isreptestheigh;
    int m_openCameraPosition;
    std::atomic<bool> m_abort{false};
    std::atomic<bool> m_cameraWanted{false};
    cv::VideoCapture m_capture;
    QTimer *m_liveTimer;
    QMutex m_frameMutex;
    cv::Mat m_liveFrame;
    quint64 m_lastSavedHash;
    const int imageWidth = 480;
    const int imageHeight = 640;
};

#endif
