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


#include <QWidget>


using namespace cv;
using namespace std;

namespace Ui {
class TestOpcv;
}

class TestOpcv : public QWidget
{
    Q_OBJECT

public:
    explicit TestOpcv(QWidget *parent = 0);
    ~TestOpcv();

    int getRedBloodCellHeight() const { return redBloodCellHeight; }



private:
   void initsignal();

   void showImage(Mat destImage);

   void trayfindImg();

   void initshowimg();

   // 图像处理相关方法
   Mat findReferenceObject(Mat& image, Scalar lowerBound, Scalar upperBound);
   double calculatePixelToCmRatio(Mat& referenceMask, double realHeightCm);
   Mat extractGrooveRegion(Mat& image, Mat& referenceMask, int grooveWidth);
   void findTubeopencv(Mat imageOrinin);
   Point findPlasmaRBCInterface(const Mat& tubeImage);
   void drawInterfaceResult(Mat& image, int interfaceY, int rbcHeight);
   vector<double> calculateVerticalGradient(const Mat& image);
   vector<double> smoothGradient(const vector<double>& gradients, int windowSize = 3);
   bool isBottomOverexposed(const Mat& tubeImage, int bottomRegionHeight = 50);

   double calculateRegionContrast(const Mat& image, int yPosition);


   string toString(double value, int precision);
   int calculateRedBloodCellHeight(const Mat& tubeImage, int interfaceY);
   int findValidBottom(const Mat& tubeImage, int interfaceY);
   Mat findTubeByMultiFeatures(Mat& inputImage);

   // 新增方法：在原图中标记结果
   void markResultsOnOriginalImage(Mat& originalImage, const Point& interfacePoint, int rbcHeight, const Rect& referenceRect);
   void displayResults();
   Rect findReferenceObjectRect(Mat& image, Scalar lowerBound, Scalar upperBound);

   // 新增方法：计算实际高度和下针深度
   void calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio);
   double calculateReferenceToBottomDistance(const Rect& referenceRect);


   // 在 calculateVerticalGradient 方法后添加颜色特征检测
   vector<double> calculateColorFeatures(const Mat& tubeImage);
   //添加溶血检测和处理
   bool isHemolyzed(const Mat& tubeImage, int interfaceY);

   // 在现有代码基础上增加暗色环境专用特征
   vector<double> calculateDarkEnvironmentFeatures(const Mat& tubeImage);

   // 专门检测分界面附近的微弱颜色变化
   vector<double>calculateInterfaceColorFeatures(const Mat& tubeImage);

   bool isLikelyAirInterface(const Mat& tubeImage, int candidateY);



signals:

   void imageoutResult(const QString redBloodCellHeigh);

public:
   void handleSycnOpendcvImage(const QString &imagePath);

private:
    Ui::TestOpcv *ui;


    Mat imageOrinin;
    Mat processedImage; // 处理后的图像
    int redBloodCellHeight; // 红细胞区域高度
    Point detectedInterface; // 检测到的分界面位置
    double pixelToMmRatio; // 像素到毫米的比例
    Rect referenceObjectRect; // 参照物矩形框

    // 新增参数
    double redBloodCellHeightMm; // 红细胞区域高度（毫米）
    double maxNeedleDropHeight; // 下针最大下降高度（毫米）
    double referenceToBottomDistance; // 参照物顶部到原点的距离（毫米）
    const double REFERENCE_TO_NEEDLEBOTTOM = 49.9; // 参照物顶部到针底部原点的距离（毫米）

};

#endif // TESTOPCV_H



