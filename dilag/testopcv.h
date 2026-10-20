#ifndef TESTOPCV_H
#define TESTOPCV_H

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifdef ACCESS_MASK
#undef ACCESS_MASK
#endif
#endif

#include <opencv2/core.hpp>

#include <string>

#include <QString>
#include <QWidget>

class QThread;
class opencvfindHeigh;

struct InspectionResult {
    QString result;
    double hemolysisIndex = 0.0;
};

namespace Ui {
class TestOpcv;
}

class QResizeEvent;
class QShowEvent;

class TestOpcv : public QWidget
{
    Q_OBJECT

public:
    explicit TestOpcv(QWidget *parent = 0);
    ~TestOpcv();

    bool hasOpenedCamera() const;

public slots:
    void handleSycnOpendcvImage(const QString &imagePath);
    void triggerCaptureAndRecognize();
    void setCameraPower(bool enable);
    void recvfindCameraIndexByDevicePath(const QString &devicePath);
    void handleRepPrpheight(const QString &numid, bool replflag);

signals:
   void imageoutResult(const QString redBloodCellHeigh);
   void captureFailed(const QString &errorType, const QString &errorMessage);
   void captureFinished(bool success);
   void beginNewMeasure();
   void replaceSampleId(const QString &id);

protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);

private:
    void initshowimg();
    void showImage(cv::Mat destImage);
    void trayfindImg();
    void markResultsOnOriginalImage(cv::Mat& originalImage, const cv::Point& interfacePoint,
                                    int rbcHeight, const cv::Rect& referenceRect);
    void showFailureWithReference(const QString& title, const QString& reason);
    void displayResults(const double &khemolysisIndex);
    void calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio);
    void calculateNeedleDropFromPackedLength(int packedRbcPixels, double pixelToMmRatio);
    InspectionResult inspectPRP(const cv::Mat &testTubeImage, double topRegionRatio);
    InspectionResult inspectPRPAlongX(const cv::Mat &testTubeImage, int interfaceX);

private slots:
   void onCapturedImage(const QString &imagePath);
   void onRetestImage(const QString &reId, const QString &imagePath);

private:
    void startCameraPipeline();
    void stopCameraPipeline();
    Ui::TestOpcv *ui;

    cv::Mat imageOrinin;
    cv::Mat processedImage;
    int redBloodCellHeight;
    cv::Point detectedInterface;
    double pixelToMmRatio;
    cv::Rect referenceObjectRect;

    double redBloodCellHeightMm;
    double maxNeedleDropHeight;
    double referenceToBottomDistance;

    std::string maxInfoColor;
    bool m_horizontalTube;
    QString m_hemolysisStatus;
    QString m_layerFailDetail;
    double m_refTopX;

    struct ColorRange {
        int lowH, highH;
        int lowS, highS;
        int lowV, highV;
    };

    ColorRange m_lowerRed1{0, 8, 50, 255, 80, 220};
    ColorRange m_lowerRed2{170, 180, 50, 255, 80, 220};
    double m_hemolysisThreshold{0.05};

    opencvfindHeigh *m_camera;
    QThread *m_cameraThread;

    QString m_detectImagePath;
};

#endif // TESTOPCV_H
