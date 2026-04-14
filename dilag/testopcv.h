#ifndef TESTOPCV_H
#define TESTOPCV_H

#pragma once

// Windows 宏冲突处理
#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef ACCESS_MASK
#endif

// 先包含OpenCV头文件
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cmath>

#include <QMap>
#include <QVariantMap>
#include <QWidget>

// 前向声明
struct BinaryParameters;

// 配置参数类 - 管理硬编码参数
class BloodAnalysisConfig {
public:
    // 图像处理参数
    static constexpr int DEFAULT_GROOVE_WIDTH = 180;
    static constexpr int MIN_CONTOUR_AREA = 100;
    static constexpr int MORPH_KERNEL_SIZE = 5;
    static constexpr double REFERENCE_HEIGHT_MM = 50.0;

    // 颜色检测参数
    static constexpr double HEMOLYSIS_THRESHOLD = 0.01;
    static constexpr int ENVIRONMENT_BRIGHTNESS_THRESHOLD = 80;

    // 参照物颜色范围（双颜色检测）
    static constexpr int REF_COLOR1_H_MIN = 94;
    static constexpr int REF_COLOR1_H_MAX = 114;
    static constexpr int REF_COLOR2_H_MIN = 83;
    static constexpr int REF_COLOR2_H_MAX = 103;

    // 二值化参数默认值 - 改为静态方法声明
    static BinaryParameters getDefaultBinaryParams();
};

// 二值化参数结构体（放在BloodAnalysisConfig之后）
struct BinaryParameters {
    // 红色参数
    int redHMin1 = 0;
    int redHMax1 = 10;
    int redHMin2 = 160;
    int redHMax2 = 180;
    int redSMin = 70;
    int redVMin = 50;

    // 暗红色参数
    int darkRedHMin1 = 0;
    int darkRedHMax1 = 10;
    int darkRedHMin2 = 160;
    int darkRedHMax2 = 180;
    int darkRedSMin = 50;
    int darkRedVMin = 30;
    int darkRedSMax = 180;
    int darkRedVMax = 150;

    // 黑色参数
    int blackHMin = 0;
    int blackHMax = 180;
    int blackSMax = 255;
    int blackVMax = 50;

    // 形态学操作参数
    int morphSize = 5;
    int morphType = cv::MORPH_ELLIPSE;
};

// 外接矩形结果结构体
struct BoundingRectResult {
    double redArea = 0.0;
    double darkRedArea = 0.0;
    double blackArea = 0.0;

    double redHeight = 0.0;
    double darkRedHeight = 0.0;
    double blackHeight = 0.0;

    cv::Rect redRect;
    cv::Rect darkRedRect;
    cv::Rect blackRect;

    bool redFound = false;
    bool darkRedFound = false;
    bool blackFound = false;
};

// 最大矩形信息结构体
struct MaxRectInfo {
    std::string colorType;
    double area = 0.0;
    double height = 0.0;
    double top = 0.0;
    cv::Rect rect;
    bool found = false;
};

// 检测结果结构体
struct InspectionResult {
    QString result;
    double confidence = 0.0;
    double hemolysisIndex = 0.0;
    QMap<QString, double> topRegionHSV;

    QVariantMap toVariantMap() const {
        QVariantMap map;
        map["result"] = result;
        map["confidence"] = confidence;
        map["hemolysis_index"] = hemolysisIndex;

        QVariantMap hsvMap;
        hsvMap["h"] = topRegionHSV.value("h", 0.0);
        hsvMap["s"] = topRegionHSV.value("s", 0.0);
        hsvMap["v"] = topRegionHSV.value("v", 0.0);
        map["top_region_hsv"] = hsvMap;

        return map;
    }
};

namespace Ui {
class TestOpcv;
}

class TestOpcv : public QWidget
{
    Q_OBJECT

public:
    explicit TestOpcv(QWidget *parent = 0);
    ~TestOpcv();
    void handleSycnOpendcvImage(const QString &imagePath);

signals:
    void imageoutResult(const QString redBloodCellHeigh);

private:
    void initsignal();
    void showImage(cv::Mat destImage);
    void trayfindImg();
    void initshowimg();

    bool detectEnvironmentBrightness(const cv::Mat& image);
    bool findReferenceObjectWithDualColor(const cv::Mat& image);
    bool extractAndAnalyzeGrooveRegion(const cv::Mat& image);

    cv::Mat findReferenceObject(cv::Mat& image, cv::Scalar lowerBound, cv::Scalar upperBound);
    double calculatePixelToCmRatio(cv::Mat& referenceMask, double realHeightCm);
    cv::Mat extractGrooveRegion(cv::Mat& image, cv::Mat& referenceMask, int grooveWidth);
    cv::Mat findTubeByMultiFeatures(cv::Mat& inputImage);

    void markResultsOnOriginalImage(cv::Mat& originalImage, const cv::Point& interfacePoint, int rbcHeight, const cv::Rect& referenceRect);
    void displayResults(const double &khemolysisIndex);
    cv::Rect findReferenceObjectRect(cv::Mat& image, cv::Scalar lowerBound, cv::Scalar upperBound);
    cv::Rect findReferenceObjectRectDualColor(cv::Mat& image, cv::Scalar lowerBound1, cv::Scalar upperBound1, cv::Scalar lowerBound2, cv::Scalar upperBound2);
    cv::Mat findReferenceObjectDualColor(cv::Mat& image, cv::Scalar lowerBound1, cv::Scalar upperBound1, cv::Scalar lowerBound2, cv::Scalar upperBound2);

    void calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio);
    double calculateReferenceToBottomDistance(const cv::Rect& referenceRect);

    void tubeBinaryProcessingWithParams(const cv::Mat& inputImage,
                                      cv::Mat& redBinary,
                                      cv::Mat& darkRedBinary,
                                      cv::Mat& blackBinary,
                                      const BinaryParameters& params = BinaryParameters());

    BoundingRectResult calculateMaxBoundingRect(const cv::Mat& redBinary,
                                              const cv::Mat& darkRedBinary,
                                              const cv::Mat& blackBinary,
                                              double minArea = 100.0);

    void drawAndDisplayResults(const cv::Mat& originalImage,
                              const BoundingRectResult& result,
                              const cv::Mat& redBinary,
                              const cv::Mat& darkRedBinary,
                              const cv::Mat& blackBinary);

    MaxRectInfo getMaxRectangleInfo(const BoundingRectResult& result);
    cv::Mat IdentifyWidthOfTheTestTube(cv::Mat & grooveTube, const double &left, const double& widthpx);
    InspectionResult inspectPRP(const cv::Mat &testTubeImage, double topRegionRatio);

private:
    Ui::TestOpcv *ui;

    cv::Mat imageOrinin;
    cv::Mat processedImage;
    cv::Mat referenceMask;  // 添加这个成员变量
    int redBloodCellHeight = 0;
    cv::Point detectedInterface;
    double pixelToMmRatio = 0.0;
    cv::Rect referenceObjectRect;

    double redBloodCellHeightMm = 0.0;
    double maxNeedleDropHeight = 0.0;
    double referenceToBottomDistance = 0.0;

    std::string maxInfoColor;

    struct ColorRange {
        int lowH, highH;
        int lowS, highS;
        int lowV, highV;
    };

    ColorRange m_lowerRed1{0, 15, 20, 255, 50, 255};
    ColorRange m_lowerRed2{165, 180, 20, 255, 50, 255};
    double m_hemolysisThreshold = 0.01;
};

#endif // TESTOPCV_H
