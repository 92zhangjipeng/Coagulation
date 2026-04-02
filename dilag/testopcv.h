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

using namespace cv;
using namespace std;


// 二值化参数结构体
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
    int morphType = cv::MORPH_ELLIPSE; // 0:矩形, 1:十字, 2:椭圆
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
    std::string colorType;  // 颜色类型："Red", "DarkRed", "Black", "None"
    double area;           // 最大面积
    double height;         // 矩形高度
    double top;            // 矩形顶部Y坐标
    cv::Rect rect;         // 完整的矩形信息
    bool found;            // 是否找到有效矩形
};


// 检测结果结构体
struct InspectionResult {
    QString result;          // "Normal", "Hemolyzed", "Error"
    double confidence;       // 置信度 0-1
    double hemolysisIndex;   // 溶血指数
    QMap<QString, double> topRegionHSV; // 顶部区域HSV值

    // 转换为QVariantMap便于使用
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

private:
   void initsignal();
   void showImage(Mat destImage);

   // 颜色评分方法
   void     trayfindImg();
   void     initshowimg();

   // 图像处理相关方法
   Mat      findReferenceObject(Mat& image, Scalar lowerBound, Scalar upperBound);
   double   calculatePixelToCmRatio(Mat& referenceMask, double realHeightCm);
   Mat      extractGrooveRegion(Mat& image, Mat& referenceMask, int grooveWidth);
   Mat      findTubeByMultiFeatures(Mat& inputImage);

   // 在原图中标记结果
   void     markResultsOnOriginalImage(Mat& originalImage, const Point& interfacePoint, int rbcHeight, const Rect& referenceRect);
   void     displayResults(const double &khemolysisIndex);
   Rect     findReferenceObjectRect(Mat& image, Scalar lowerBound, Scalar upperBound);
   Rect     findReferenceObjectRectDualColor(Mat& image, Scalar lowerBound1, Scalar upperBound1, Scalar lowerBound2, Scalar upperBound2);
   Mat      findReferenceObjectDualColor(Mat& image, Scalar lowerBound1, Scalar upperBound1, Scalar lowerBound2, Scalar upperBound2);

   // 计算实际高度和下针深度
   void     calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio);
   double   calculateReferenceToBottomDistance(const Rect& referenceRect);


   /**
    * @brief 带参数的试管图像颜色二值化处理
    * @param inputImage 输入的试管图像
    * @param redBinary 输出的红色二值化结果
    * @param darkRedBinary 输出的暗红色二值化结果
    * @param blackBinary 输出的黑色二值化结果
    * @param params 二值化参数配置
    */
   void tubeBinaryProcessingWithParams(const cv::Mat& inputImage,
                                     cv::Mat& redBinary,
                                     cv::Mat& darkRedBinary,
                                     cv::Mat& blackBinary,
                                     const BinaryParameters& params = BinaryParameters());


   /**
    * @brief 计算二值化图像的最大外接矩形面积和高度
    * @param redBinary 红色二值化图像
    * @param darkRedBinary 暗红色二值化图像
    * @param blackBinary 黑色二值化图像
    * @param minArea 最小面积阈值，小于此面积的轮廓将被忽略
    * @return BoundingRectResult 包含面积和高度信息的结果结构体
    */
   BoundingRectResult calculateMaxBoundingRect(const cv::Mat& redBinary,
                                             const cv::Mat& darkRedBinary,
                                             const cv::Mat& blackBinary,
                                             double minArea = 100.0);

   /**
    * @brief 在图像上绘制外接矩形并显示结果
    * @param originalImage 原始图像
    * @param result 外接矩形结果
    * @param redBinary 红色二值化图像（用于显示）
    * @param darkRedBinary 暗红色二值化图像（用于显示）
    * @param blackBinary 黑色二值化图像（用于显示）
    */
   void drawAndDisplayResults(const cv::Mat& originalImage,
                             const BoundingRectResult& result,
                             const cv::Mat& redBinary,
                             const cv::Mat& darkRedBinary,
                             const cv::Mat& blackBinary);


   /**
    * @brief 从BoundingRectResult中获取最大矩形面积的信息
    * @param result BoundingRectResult结构体
    * @return MaxRectInfo 包含最大矩形的详细信息
    */
   MaxRectInfo getMaxRectangleInfo(const BoundingRectResult& result);

   Mat IdentifyWidthOfTheTestTube(Mat & grooveTube, const double &left, const double& widthpx);

   InspectionResult inspectPRP(const Mat &testTubeImage, double topRegionRatio);


signals:
   void imageoutResult(const QString redBloodCellHeigh,const double maxNeedleDropHeight);

public:
   void handleSycnOpendcvImage(const QString &imagePath);

private:
    Ui::TestOpcv *ui;

    Mat imageOrinin;
    Mat processedImage;
    int redBloodCellHeight;
    Point detectedInterface;
    double pixelToMmRatio;
    Rect referenceObjectRect;

    double redBloodCellHeightMm;
    double maxNeedleDropHeight;
    double referenceToBottomDistance;

    std::string maxInfoColor;


    // HSV颜色范围定义
    struct ColorRange {
        int lowH, highH;
        int lowS, highS;
        int lowV, highV;
    };

    ColorRange m_lowerRed1{0, 15, 20, 255, 50, 255};
    ColorRange m_lowerRed2{165, 180, 20, 255, 50, 255};
    double m_hemolysisThreshold{0.01}; // 溶血阈值2%

};

#endif // TESTOPCV_H
