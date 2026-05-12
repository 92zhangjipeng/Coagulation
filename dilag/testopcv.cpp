// 在文件最开头添加宏定义
#define ACCESS_MASK ACCESS_MASK

#pragma execution_character_set("utf-8")

#include "testopcv.h"
#include "ui_testopcv.h"
#include <QFileDialog>
#include <QGraphicsScene>
#include <QPushButton>
#include <QsLog/include/QsLog.h>
#include <opencv2/opencv.hpp>
#include <QMessageBox>
#include <globaldata.h>
#include "cglobal.h"
#include "ini_file.h"
#include "ini_file.h"
#include <opencv2/dnn.hpp>

using namespace cv;
using namespace dnn;


TestOpcv::TestOpcv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestOpcv),
    redBloodCellHeight(0),
    pixelToMmRatio(0.0),
    referenceObjectRect(0,0,0,0),
    redBloodCellHeightMm(0.0),
    maxNeedleDropHeight(0.0),
    referenceToBottomDistance(0.0)
{
    ui->setupUi(this);
    initsignal();
    initshowimg();

    ui->widget_bar->hide();

    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
        settButtonQss = QLatin1String(styleFile.readAll());
        styleFile.close();
    }

    QHash<QPushButton*, QString> pushButtonList = {
        {ui->pushButton_loadpath, tr("导入图片")},
        {ui->pushButton_test, tr("测试识别")}
    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
        it.key()->setStyleSheet(settButtonQss);
        it.key()->setText(it.value());
    }
}


TestOpcv::~TestOpcv()
{
    delete ui;
}


void debugImshow(std::string name,Mat & image){
    cv::namedWindow(name, cv::WINDOW_NORMAL);
    // 设置窗口大小（宽度、高度）
    cv::resizeWindow(name, 120, 640);
    // 显示图像
    cv::imshow(name, image);
}

void TestOpcv::initshowimg()
{
    QFont font;
    font.setFamily("楷体");
    font.setPixelSize(14);
    font.setBold(true);
    ui->label_ratio->setText("检测就绪:");

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, QColor(28, 134, 238));
    ui->widgetShowImag->setAutoFillBackground(true);
    ui->widgetShowImag->setPalette(sample_palette);

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    ui->label_showimage->setPalette(pe);

    font.setPixelSize(25);
    ui->label_showimage->setFont(font);
    ui->label_showimage->setWordWrap(true);
    ui->label_showimage->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    QString text = "暂无图片";
    ui->label_showimage->setText(text.split("", QString::SkipEmptyParts).join("\n"));
}

/** 显示图片**/
void displayImage(cv::Mat cvImage,QLabel *pshow)
{
    pshow->clear();
    if (cvImage.empty()) return;
    cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
    QImage qImage(cvImage.data, cvImage.cols, cvImage.rows,
                static_cast<int>(cvImage.step), QImage::Format_RGB888);

    QPixmap pixmap = QPixmap::fromImage(qImage);
    if (pixmap.isNull()) return ;
    QSize targetSize = pshow->size();
    QPixmap scaledPix = pixmap.scaled(targetSize,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);
    pshow->setScaledContents(true);
    pshow->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    pshow->setPixmap(scaledPix);
    pshow->update();
    return;
}

void TestOpcv::initsignal()
{
    connect(ui->pushButton_loadpath, &QPushButton::clicked, this, [=]() {
            QString OpenFile = QFileDialog::getOpenFileName(this, tr("请选取图片文件"), "",
                                    "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
            if(!OpenFile.isEmpty()) {
                std::string cstrPath = OpenFile.toLocal8Bit().constData();
                Mat img = imread(cstrPath);
                if (img.empty()) {
                    QMessageBox::warning(this, "错误", "无法加载图像文件");
                    return;
                }

                handleSycnOpendcvImage(OpenFile);
            }
    });

    connect(ui->pushButton_test, &QPushButton::clicked, this, [=]() {
        if (!imageOrinin.empty()) {
            trayfindImg();
        } else {
            QMessageBox::warning(this, "提示", "请先导入图片");
        }
    });
}

void TestOpcv::showImage(Mat destImage){
   displayImage(destImage, ui->label_showimage);
}

Rect TestOpcv::findReferenceObjectRect(Mat& image, Scalar lowerBound, Scalar upperBound)
{
    Mat hsv, mask;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, lowerBound, upperBound, mask);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);

    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return Rect(0, 0, 0, 0);
    }

    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    return boundingRect(*largestContour);
}

Rect TestOpcv::findReferenceObjectRectDualColor(Mat& image, Scalar lowerBound1, Scalar upperBound1, Scalar lowerBound2, Scalar upperBound2)
{
    Mat hsv, mask1, mask2, mask;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, lowerBound1, upperBound1, mask1);
    inRange(hsv, lowerBound2, upperBound2, mask2);
    mask = mask1 | mask2;

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);

    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return Rect(0, 0, 0, 0);
    }

    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    return boundingRect(*largestContour);
}

Mat TestOpcv::findReferenceObjectDualColor(Mat& image, Scalar lowerBound1, Scalar upperBound1, Scalar lowerBound2, Scalar upperBound2) {
    Mat hsv, mask1, mask2, mask;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, lowerBound1, upperBound1, mask1);
    inRange(hsv, lowerBound2, upperBound2, mask2);
    mask = mask1 | mask2;

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);

    return mask;
}

double TestOpcv::calculateReferenceToBottomDistance(const Rect& referenceRect)
{
    double referenceTopToImageBottom = imageOrinin.rows - referenceRect.y;
    return referenceTopToImageBottom / pixelToMmRatio;
}

void TestOpcv::calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio)
{
    if (pixelToMmRatio <= 0) {
        QLOG_ERROR() << "错误: 像素到毫米比例无效";
        return;
    }

    auto &ini = INI_File();
    //redBloodCellHeightMm = rbcHeightPixels / pixelToMmRatio; //红细胞高度
    referenceToBottomDistance = ini.GetFixedHigh(); //参照物top到针高度
    double safetyMargin = ini.GetTestDifference();  //偏移高度
    double rotb  =   ini.getRefBottomDistance();   //参照物到底部距离

    //  红细胞top与参照物top像素差（实际偏差像素）  = 红细胞TOP像素 - 参照物TOP像素
    double interfaceToReferenceTopPixels = interfaceY - referenceObjectRect.y;
    //像素差实际高度mm(像素/比列) （实际偏差高度）
    double interfaceToReferenceTopMm = interfaceToReferenceTopPixels / pixelToMmRatio;
    QLOG_DEBUG()<<"（实际偏差高度）"<<interfaceToReferenceTopMm;

    //针到参照物top的高度mm + （实际偏差高度）
    double totalDropDistance = rotb - interfaceToReferenceTopMm - RAISETHERULER; //输出血样高度
    redBloodCellHeightMm = max(0.0, totalDropDistance);
    redBloodCellHeightMm = round(redBloodCellHeightMm * 100) / 100; //保留2位小数

    maxNeedleDropHeight = referenceToBottomDistance + rotb - redBloodCellHeightMm - safetyMargin + RAISETHERULER;

    QLOG_DEBUG() << "计算参数:";
    QLOG_DEBUG() << "红细胞高度像素: " << rbcHeightPixels << "px";
    QLOG_DEBUG() << "像素到毫米比例: " << pixelToMmRatio << "px/mm";
    QLOG_DEBUG() << "红细胞高度: " << redBloodCellHeightMm << "mm";
    QLOG_DEBUG() << "分界面Y坐标: " << interfaceY << "px";
    QLOG_DEBUG() << "参照物顶部Y坐标: " << referenceObjectRect.y << "px";
    QLOG_DEBUG() << "分界面到参照物顶部距离: " << interfaceToReferenceTopPixels << "px";
    QLOG_DEBUG() << "分界面到参照物顶部距离: " << interfaceToReferenceTopMm << "mm";
    QLOG_DEBUG() << "参照物到针底部距离: " << referenceToBottomDistance << "mm";
    QLOG_DEBUG() << "下针最大下降高度: " << maxNeedleDropHeight << "mm"<<endl;
}



Mat TestOpcv::findReferenceObject(Mat& image, Scalar lowerBound, Scalar upperBound) {
    Mat hsv, mask;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, lowerBound, upperBound, mask);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);

    return mask;
}

double TestOpcv::calculatePixelToCmRatio(Mat& referenceMask, double realHeightCm) {
    vector<vector<Point>> contours;
    findContours(referenceMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return 0.0;
    }

    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    Rect boundingRect = cv::boundingRect(*largestContour);
    double pixelHeight = boundingRect.height;

    //QLOG_DEBUG()<<"宽度转换成像素比"<<boundingRect.width / 5;
    return pixelHeight / realHeightCm;
}

Mat TestOpcv::extractGrooveRegion(Mat& image, Mat& referenceMask, int grooveWidth) {
    vector<vector<Point>> contours;
    findContours(referenceMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return Mat();
    }

    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    Rect refRect = cv::boundingRect(*largestContour);
    int grooveX = refRect.x + refRect.width + 40;
    int grooveY = 0;
    int grooveHeight = image.rows;

    grooveX = min(max(grooveX, 0), image.cols - 1);
    grooveWidth = min(grooveWidth, image.cols - grooveX);

    Rect grooveRect(grooveX, grooveY, grooveWidth, grooveHeight);
    return image(grooveRect).clone();
}

void TestOpcv::markResultsOnOriginalImage(Mat& originalImage, const Point& interfacePoint,
                                          int rbcHeight, const Rect& referenceRect)
{
     Mat overlay = originalImage.clone();

     // 1. 绘制液面水平线（血浆-红细胞分界面）
     if (interfacePoint.y >= 0 && interfacePoint.y < originalImage.rows) {

         line(originalImage, Point(0, interfacePoint.y),
         Point(originalImage.cols, interfacePoint.y), Scalar(255, 0, 0), 2);   // 蓝色线
     }

     if (referenceRect.width > 0 && referenceRect.height > 0) {
         rectangle(originalImage, referenceRect, Scalar(0, 255, 0), 3);

         // 在参照物顶部增加一条水平线（与矩形框上边缘重合，但单独绘制以示强调）
         line(originalImage, Point(0, referenceRect.y),
                Point(originalImage.cols , referenceRect.y), Scalar(0, 255, 0), 2);

         putText(originalImage, "Reference",
                 Point(referenceRect.x, referenceRect.y - 10),
                 FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
     }

     if (interfacePoint.y >= 0 && interfacePoint.y < originalImage.rows) {

        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.7;
        int thickness = 2;
        putText(originalImage, "RBC Interface",
                        Point(10, interfacePoint.y - 20), fontFace, fontScale,
                        Scalar(255, 0, 0), thickness);
     }

//     int fontFace = FONT_HERSHEY_SIMPLEX;
//     double fontScale = 0.7;
//     int thickness = 2;

//     putText(originalImage, "RBC Interface",
//             Point(10, interfacePoint.y - 20), fontFace, fontScale,
//             Scalar(255, 0, 0), thickness);
}

void TestOpcv::displayResults(const double& khemolysisIndex)
{
    if (processedImage.empty() || detectedInterface.y < 0) {
       QMessageBox::warning(this, "错误", "未找到有效结果");
       return;
   }

   Mat resultImage = imageOrinin.clone();
   markResultsOnOriginalImage(resultImage, detectedInterface, redBloodCellHeight, referenceObjectRect);
   displayImage(resultImage, ui->label_showimage);


   QString infoText = QString(
           "<div style='color: black;'>"
           "检测完成:<br>"
           "溶血指数: %1 像素<br>"
           "红细胞topY坐标: %2 像素<br>"
           "红细胞高度: %3 像素 (%4 mm)<br>"
           "图例比: %5 像素/毫米<br>"
           "样本针下降触碰到红细胞高度: %6 mm<br>"
           "样本针物理原点高度距离: %7 mm"
           "</div>")
           .arg(khemolysisIndex)
           .arg(detectedInterface.y)
           .arg(redBloodCellHeight)
           .arg(redBloodCellHeightMm, 0, 'f', 2)
           .arg(pixelToMmRatio)
           .arg(maxNeedleDropHeight, 0, 'f', 2)
           .arg(INI_File().GetFixedHigh(), 0, 'f', 1);

   // 如果识别到的颜色类型是黑色，添加红色警告文字
   if (maxInfoColor == "Black") {
       infoText += QString("<div style='color: red; font-weight: bold; margin-top: 5px;'>请检查血细胞有压积层</div>");
   }

   QString redBloodCellHeightMmstr = QString("%1").arg(redBloodCellHeightMm, 0, 'f', 2);
   emit imageoutResult(redBloodCellHeightMmstr); // 显示血的距离、和下针的高度

   // 启用富文本显示
   ui->label_ratio->setTextFormat(Qt::RichText);
   ui->label_ratio->setText(infoText);
   ui->label_ratio->setStyleSheet("QLabel { background-color: white; padding: 5px; }");
}






// 计算轮廓的U型多特征融合的精确定位
Mat TestOpcv::  findTubeByMultiFeatures(Mat& inputImage) {
    Mat result;
    inputImage.copyTo(result);

    //环境检测和预处理
    Mat grayEnv;
    cvtColor(inputImage, grayEnv, COLOR_BGR2GRAY);
    Scalar meanBrightness = mean(grayEnv);
    bool isDarkEnvironment = meanBrightness[0] < 80;

    Mat enhanced;
    if (isDarkEnvironment) {
        inputImage.convertTo(enhanced, CV_32F, 1.0/255.0);
        pow(enhanced, 1.5, enhanced);
        enhanced.convertTo(enhanced, CV_8U, 255.0);
    } else {
        enhanced = inputImage.clone();
    }

    debugImshow("0",enhanced);

    Mat gray;
    cvtColor(enhanced, gray, COLOR_BGR2GRAY);

    debugImshow("1",gray);

    // 2. 专门检测白色凹槽区域并排除
    Mat whiteMask;
    inRange(inputImage, Scalar(200, 200, 200), Scalar(255, 255, 255), whiteMask);

    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(whiteMask, whiteMask, MORPH_CLOSE, kernel);
    morphologyEx(whiteMask, whiteMask, MORPH_OPEN, kernel);

    debugImshow("2",whiteMask);

    // 3. 分析白色区域的位置特征
    vector<vector<Point>> whiteContours;
    findContours(whiteMask, whiteContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    int whiteBottomY = -1;
    Rect whiteGrooveRect;
    for (const auto& contour : whiteContours) {
        Rect rect = boundingRect(contour);
        if (rect.y > gray.rows * 0.7 && rect.width > gray.cols * 0.3) {
            whiteBottomY = max(whiteBottomY, rect.y);
            whiteGrooveRect = rect;
            //rectangle(result, rect, Scalar(255, 0, 0), 2);
        }
    }

    // 4. 检测试管边缘（新增：精确宽度检测）
    Mat edges;
    Canny(gray, edges, 50, 150);

    // 霍夫直线检测，找垂直边缘
    vector<Vec4i> lines;
    HoughLinesP(edges, lines, 1, CV_PI/180, 50, 30, 10);

    int leftEdge = inputImage.cols;
    int rightEdge = 0;

    for (const auto& line : lines) {
        int x1 = line[0], y1 = line[1], x2 = line[2], y2 = line[2];

        // 主要关注垂直方向的直线（角度接近90度）
        if (abs(x1 - x2) < 5 && abs(y1 - y2) > 20) {
            int x = (x1 + x2) / 2;
            if (x < leftEdge) leftEdge = x;
            if (x > rightEdge) rightEdge = x;

            // 在结果图像上标记检测到的边缘
            cv::line(result, Point(x1, y1), Point(x2, y2), Scalar(0, 255, 255), 2);
        }
    }

   debugImshow("3",result);

    // 如果没有检测到边缘，使用默认宽度
    if (leftEdge >= rightEdge) {
        leftEdge =  static_cast<int>(inputImage.cols * 0.1);
        rightEdge = static_cast<int>(inputImage.cols * 0.9);
    }

    int tubeWidth = rightEdge - leftEdge;
    QLOG_DEBUG() << "检测到的试管宽度: " << tubeWidth << "像素";

    // 5. 特征提取 - 只在试管宽度范围内处理
    Mat background;
    blur(gray, background, Size(31, 31));
    Mat difference;
    absdiff(gray, background, difference);

    debugImshow("4",difference);

    // 在白色凹槽区域将特征值设为零
    if (whiteBottomY != -1) {
        for (int y = whiteBottomY; y < gray.rows; y++) {
            for (int x = 0; x < gray.cols; x++) {
                difference.at<uchar>(y, x) = 0;
            }
        }
    }

    Mat gradY;
    Sobel(gray, gradY, CV_32F, 0, 1);
    convertScaleAbs(gradY, gradY);

    // 6. 创建特征图 - 限制在试管宽度范围内
    Mat featureMap = Mat::zeros(gray.size(), CV_32F);

    for (int y = 0; y < gray.rows; y++) {
        if (whiteBottomY != -1 && y >= whiteBottomY) {
            continue;
        }

        for (int x = leftEdge; x < rightEdge; x++) { // 只在试管宽度范围内处理
            if (x < 0 || x >= gray.cols) continue;

            float diffVal = difference.at<uchar>(y, x) / 255.0f;
            float gradVal = gradY.at<uchar>(y, x) / 255.0f;

            float darkWeight = (gray.at<uchar>(y, x) < 50) ? 2.0f : 1.0f;
            float positionWeight = (y > gray.rows * 0.6) ? 1.5f : 1.0f;

            featureMap.at<float>(y, x) = (diffVal * darkWeight + gradVal) * positionWeight;
        }
    }

    // 7. 垂直投影分析 - 限制搜索范围
    Mat verticalProj;
    reduce(featureMap, verticalProj, 1, REDUCE_SUM, CV_32F);

    int searchStart = static_cast<int>(gray.rows * 0.4);
    int searchEnd;
    if (whiteBottomY != -1) {
        searchEnd = std::min(whiteBottomY - 10, static_cast<int>(gray.rows * 0.9));
    } else {
        searchEnd = static_cast<int>(gray.rows * 0.9);
    }
    searchEnd = std::max(searchEnd, searchStart + 50);

    int bestY = -1;
    float bestScore = 0;

    // 8. 寻找采血管底部
    for (int y = searchStart; y < searchEnd; y++) {
        float score = verticalProj.at<float>(y);

        if (y > searchStart + 5) {
            float continuity = 0;
            int count = 0;
            for (int dy = -3; dy <= 3; dy++) {
                if (y + dy >= 0 && y + dy < verticalProj.rows) {
                    continuity += verticalProj.at<float>(y + dy);
                    count++;
                }
            }
            if (count > 0) {
                score *= (continuity / count) / std::max(verticalProj.at<float>(y), 0.1f);
            }
        }

        if (score > bestScore) {
            bestScore = score;
            bestY = y;
        }
    }

    // 9. 多重验证和备用策略
    if (bestY == -1 || bestScore < 5.0f) {
        QLOG_DEBUG() << "主算法未找到合适底部，使用颜色特征备用策略";

        vector<double> rowMeans(gray.rows);
        for (int y = 0; y < gray.rows; y++) {
            double sum = 0;
            int count = 0;
            for (int x = leftEdge; x < rightEdge; x++) { // 限制在试管宽度内
                if (x >= 0 && x < gray.cols) {
                    sum += gray.at<uchar>(y, x);
                    count++;
                }
            }
            rowMeans[y] = (count > 0) ? sum / count : 0;
        }

        double maxChange = 0;
        int searchEndTemp = std::min(searchEnd, gray.rows - 10);
        for (int y = searchStart; y < searchEndTemp; y++) {
            double change = abs(rowMeans[y] - rowMeans[y + 5]);
            if (change > maxChange) {
                maxChange = change;
                bestY = y;
            }
        }
    }

    // 10. 最终合理性检查
    if (bestY != -1) {
        if (bestY < static_cast<int>(gray.rows * 0.3) || bestY > static_cast<int>(gray.rows * 0.85)) {
            QLOG_DEBUG() << "检测位置超出合理范围，使用保守估计";
            bestY = static_cast<int>(gray.rows * 0.7);
        }

        if (whiteBottomY != -1 && bestY >= whiteBottomY - 20) {
            QLOG_DEBUG() << "检测位置接近白色凹槽，向上调整";
            bestY = whiteBottomY - 30;
        }

        line(result, Point(0, bestY), Point(result.cols, bestY), Scalar(0, 255, 0), 3);
        putText(result, "Tube Bottom: " + to_string(bestY),
               Point(10, bestY - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);

        // 标记试管左右边缘
        line(result, Point(leftEdge, 0), Point(leftEdge, result.rows), Scalar(255, 255, 0), 2);
        line(result, Point(rightEdge, 0), Point(rightEdge, result.rows), Scalar(255, 255, 0), 2);
        putText(result, "Tube Width: " + to_string(tubeWidth),
               Point(10, 30), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 0), 2);
    } else {
        bestY = static_cast<int>(gray.rows * 0.7);
        QLOG_DEBUG() << "使用默认底部位置: " << bestY;
    }

    // 11. 截取有效区域 - 使用精确的宽度
    if(tubeWidth < 120)
        tubeWidth = 120;
    Rect roi(leftEdge, 0, tubeWidth, bestY);

    // 确保ROI在图像范围内
    roi.x = max(0, roi.x);
    roi.y = max(0, roi.y);
    roi.width = min(roi.width, inputImage.cols - roi.x);
    roi.height = min(roi.height, inputImage.rows - roi.y);

    Mat croppedImage = inputImage(roi).clone();

    return croppedImage;
}



void TestOpcv::handleSycnOpendcvImage(const QString &imagePath){
    ui->label_ratio->setText("检测就绪:");

    initshowimg();

    QFile file(imagePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this, "错误", "无法加载图像文件");
        return;
    }

    QByteArray imageData = file.readAll();
    Mat img = imdecode(Mat(1, imageData.size(), CV_8UC1, imageData.data()), IMREAD_COLOR);
    if (img.empty()) {
        QMessageBox::warning(this, "错误", "无法解码图像文件");
        return;
    }

    // 目标分辨率 480x640
   int targetWidth = 480;
   int targetHeight = 640;

   if (img.cols != targetWidth || img.rows != targetHeight) {
       Mat resizedImg;
       // 注意：Size的参数是 (width, height)
       cv::resize(img, resizedImg, Size(targetWidth, targetHeight), 0, 0, INTER_LINEAR);

       imageOrinin = resizedImg.clone();
       displayImage(resizedImg, ui->label_showimage);

       QLOG_DEBUG() << "图像已从 " << img.cols << "x" << img.rows
           << " 缩放至 " << targetWidth << "x" << targetHeight;
   } else {
       // 已经是目标分辨率，直接使用
       imageOrinin = img.clone();
       displayImage(img, ui->label_showimage);
   }


    // 重置检测结果
    redBloodCellHeight = 0;
    detectedInterface = Point(-1, -1);
    referenceObjectRect = Rect(0,0,0,0);
    redBloodCellHeightMm = 0.0;
    maxNeedleDropHeight = 0.0;
    referenceToBottomDistance = 0.0;

    trayfindImg();
}




void TestOpcv::tubeBinaryProcessingWithParams(const cv::Mat& inputImage,
                                  cv::Mat& redBinary,
                                  cv::Mat& darkRedBinary,
                                  cv::Mat& blackBinary,
                                  const BinaryParameters& params )
{
    if (inputImage.empty()) {
        QLOG_WARN() << "Input image is empty!";
        return;
    }

    // 转换为HSV颜色空间，更好地处理颜色
    cv::Mat hsvImage;
    cv::cvtColor(inputImage, hsvImage, cv::COLOR_BGR2HSV);

    // 红色范围1 (0-10)
    cv::Mat redMask1;
    cv::inRange(hsvImage,
               cv::Scalar(params.redHMin1, params.redSMin, params.redVMin),
               cv::Scalar(params.redHMax1, 255, 255),
               redMask1);

    // 红色范围2 (160-180)
    cv::Mat redMask2;
    cv::inRange(hsvImage,
               cv::Scalar(params.redHMin2, params.redSMin, params.redVMin),
               cv::Scalar(params.redHMax2, 255, 255),
               redMask2);

    // 合并红色掩码
    cv::Mat redMask = redMask1 | redMask2;
    redBinary = redMask.clone();

    // 暗红色处理
    cv::Mat darkRedMask1;
    cv::inRange(hsvImage,
               cv::Scalar(params.darkRedHMin1, params.darkRedSMin, params.darkRedVMin),
               cv::Scalar(params.darkRedHMax1, params.darkRedSMax, params.darkRedVMax),
               darkRedMask1);

    cv::Mat darkRedMask2;
    cv::inRange(hsvImage,
               cv::Scalar(params.darkRedHMin2, params.darkRedSMin, params.darkRedVMin),
               cv::Scalar(params.darkRedHMax2, params.darkRedSMax, params.darkRedVMax),
               darkRedMask2);

    cv::Mat darkRedMask = darkRedMask1 | darkRedMask2;
    darkRedBinary = darkRedMask.clone();

    // 黑色处理
    cv::Mat blackMask;
    cv::inRange(hsvImage,
               cv::Scalar(params.blackHMin, 0, 0),
               cv::Scalar(params.blackHMax, params.blackSMax, params.blackVMax),
               blackMask);
    blackBinary = blackMask.clone();

    // 创建形态学操作核
    cv::Mat kernel;
    switch (params.morphType) {
        case 0: // 矩形
            kernel = cv::getStructuringElement(cv::MORPH_RECT,
                     cv::Size(params.morphSize, params.morphSize));
            break;
        case 1: // 十字
            kernel = cv::getStructuringElement(cv::MORPH_CROSS,
                     cv::Size(params.morphSize, params.morphSize));
            break;
        case 2: // 椭圆
        default:
            kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE,
                     cv::Size(params.morphSize, params.morphSize));
            break;
    }

    // 可选：进行形态学操作去除噪声
    if (params.morphSize > 0) {
        cv::morphologyEx(redBinary, redBinary, cv::MORPH_CLOSE, kernel);
        cv::morphologyEx(darkRedBinary, darkRedBinary, cv::MORPH_CLOSE, kernel);
        cv::morphologyEx(blackBinary, blackBinary, cv::MORPH_CLOSE, kernel);
    }
}



BoundingRectResult TestOpcv::calculateMaxBoundingRect(const cv::Mat& redBinary,
                                          const cv::Mat& darkRedBinary,
                                          const cv::Mat& blackBinary,
                                          double minArea)
{
    BoundingRectResult result;

    // 处理红色二值化图像
    if (!redBinary.empty()) {
        std::vector<std::vector<cv::Point>> redContours;
        cv::findContours(redBinary.clone(), redContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if (!redContours.empty()) {
            // 找到最大面积的轮廓
            auto maxRedContour = *std::max_element(redContours.begin(), redContours.end(),
                [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                    return cv::contourArea(a) < cv::contourArea(b);
                });

            double area = cv::contourArea(maxRedContour);
            if (area >= minArea) {
                result.redRect = cv::boundingRect(maxRedContour);
                result.redArea = area;
                result.redHeight = result.redRect.height;
                result.redFound = true;
            }
        }
    }

    // 处理暗红色二值化图像
    if (!darkRedBinary.empty()) {
        std::vector<std::vector<cv::Point>> darkRedContours;
        cv::findContours(darkRedBinary.clone(), darkRedContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if (!darkRedContours.empty()) {
            auto maxDarkRedContour = *std::max_element(darkRedContours.begin(), darkRedContours.end(),
                [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                    return cv::contourArea(a) < cv::contourArea(b);
                });

            double area = cv::contourArea(maxDarkRedContour);
            if (area >= minArea) {
                result.darkRedRect = cv::boundingRect(maxDarkRedContour);
                result.darkRedArea = area;
                result.darkRedHeight = result.darkRedRect.height;
                result.darkRedFound = true;
            }
        }
    }

    // 处理黑色二值化图像
    if (!blackBinary.empty()) {
        std::vector<std::vector<cv::Point>> blackContours;
        cv::findContours(blackBinary.clone(), blackContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

        if (!blackContours.empty()) {
            auto maxBlackContour = *std::max_element(blackContours.begin(), blackContours.end(),
                [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
                    return cv::contourArea(a) < cv::contourArea(b);
                });

            double area = cv::contourArea(maxBlackContour);
            cv::Rect bbox = cv::boundingRect(maxBlackContour);

            // 检查是否应该舍弃该轮廓
            bool shouldDiscard = false;

            // 检查是否在图像底部（假设底部定义为图像高度的95%以下）
            double bottomThreshold = blackBinary.rows * 0.95;
            if (bbox.y + bbox.height > bottomThreshold) {
                shouldDiscard = true;
            }

            // 检查宽度是否大于高度
           if (bbox.width > bbox.height) {
               shouldDiscard = true;
           }


            if (area >= minArea && !shouldDiscard) {
                result.blackRect = cv::boundingRect(maxBlackContour);
                result.blackArea = area;
                result.blackHeight = result.blackRect.height;
                result.blackFound = true;
            } else {
                // 如果需要，可以在这里添加逻辑来寻找下一个符合条件的轮廓
                // 例如，遍历所有轮廓，找到第一个满足条件的
                for (const auto& contour : blackContours) {
                    area = cv::contourArea(contour);
                    if (area < minArea) continue;

                    bbox = cv::boundingRect(contour);

                    // 检查是否应该舍弃
                    shouldDiscard = false;
                    if (bbox.y + bbox.height > bottomThreshold) {
                        shouldDiscard = true;
                    }
                    if (bbox.width > bbox.height) {
                        shouldDiscard = true;
                    }

                    if (!shouldDiscard) {
                        result.blackRect = bbox;
                        result.blackArea = area;
                        result.blackHeight = result.blackRect.height;
                        result.blackFound = true;
                        break;
                    }
                }
            }
        }
    }

    return result;
}




void TestOpcv::drawAndDisplayResults(const cv::Mat& originalImage,
                                      const BoundingRectResult& result,
                                      const cv::Mat& redBinary,
                                      const cv::Mat& darkRedBinary,
                                      const cv::Mat& blackBinary)
{
    // 创建副本用于绘制
    cv::Mat displayImage = originalImage.clone();
    cv::Mat redDisplay = redBinary.clone();
    cv::Mat darkRedDisplay = darkRedBinary.clone();
    cv::Mat blackDisplay = blackBinary.clone();

    // 在原始图像上绘制矩形
    if (result.redFound) {
        cv::rectangle(displayImage, result.redRect, cv::Scalar(0, 0, 255), 2);
        cv::rectangle(redDisplay, result.redRect, cv::Scalar(255), 2);
        cv::putText(displayImage,
                   "Red: " + std::to_string(int(result.redArea)) + "px², H: " + std::to_string(int(result.redHeight)),
                   cv::Point(result.redRect.x, result.redRect.y - 10),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
    }

    if (result.darkRedFound) {
        cv::rectangle(displayImage, result.darkRedRect, cv::Scalar(0, 165, 255), 2); // 橙红色
        cv::rectangle(darkRedDisplay, result.darkRedRect, cv::Scalar(255), 2);
        cv::putText(displayImage,
                   "DarkRed: " + std::to_string(int(result.darkRedArea)) + "px², H: " + std::to_string(int(result.darkRedHeight)),
                   cv::Point(result.darkRedRect.x, result.darkRedRect.y - 10),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 165, 255), 1);
    }

    if (result.blackFound) {
        cv::rectangle(displayImage, result.blackRect, cv::Scalar(255, 255, 255), 2); // 白色
        cv::rectangle(blackDisplay, result.blackRect, cv::Scalar(255), 2);
        cv::putText(displayImage,
                   "Black: " + std::to_string(int(result.blackArea)) + "px², H: " + std::to_string(int(result.blackHeight)),
                   cv::Point(result.blackRect.x, result.blackRect.y - 10),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
    }

    // 显示结果
    cv::imshow("Original with Bounding Boxes", displayImage);
    cv::imshow("Red Binary with Box", redDisplay);
    cv::imshow("Dark Red Binary with Box", darkRedDisplay);
    cv::imshow("Black Binary with Box", blackDisplay);

    // 输出结果到控制台
    QLOG_DEBUG() << "=== Bounding Rectangle Results ===";
    if (result.redFound) {
        QLOG_DEBUG() << "Red - Area:" << result.redArea << "px², Height:" << result.redHeight << "px";
    } else {
        QLOG_DEBUG() << "Red - No significant contour found";
    }

    if (result.darkRedFound) {
        QLOG_DEBUG() << "DarkRed - Area:" << result.darkRedArea << "px², Height:" << result.darkRedHeight << "px";
    } else {
        QLOG_DEBUG() << "DarkRed - No significant contour found";
    }

    if (result.blackFound) {
        QLOG_DEBUG() << "Black - Area:" << result.blackArea << "px², Height:" << result.blackHeight << "px";
    } else {
        QLOG_DEBUG() << "Black - No significant contour found";
    }
}


MaxRectInfo TestOpcv::getMaxRectangleInfo(const BoundingRectResult& result)
{
    MaxRectInfo maxInfo;
    maxInfo.found = false;
    maxInfo.area = 0.0;
    maxInfo.colorType = "None";

    // 比较三个颜色的面积，找出最大的
    if (result.redFound && result.redArea > maxInfo.area) {
        maxInfo.area = result.redArea;
        maxInfo.height = result.redHeight;
        maxInfo.top = result.redRect.y;
        maxInfo.rect = result.redRect;
        maxInfo.colorType = "Red";
        maxInfo.found = true;
    }

    if (result.darkRedFound && result.darkRedArea > maxInfo.area) {
        maxInfo.area = result.darkRedArea;
        maxInfo.height = result.darkRedHeight;
        maxInfo.top = result.darkRedRect.y;
        maxInfo.rect = result.darkRedRect;
        maxInfo.colorType = "DarkRed";
        maxInfo.found = true;
    }

    if (result.blackFound && result.blackArea > maxInfo.area) {
        maxInfo.area = result.blackArea;
        maxInfo.height = result.blackHeight;
        maxInfo.top = result.blackRect.y;
        maxInfo.rect = result.blackRect;
        maxInfo.colorType = "Black";
        maxInfo.found = true;
    }

    return maxInfo;
}



void TestOpcv::trayfindImg()
{
    if (imageOrinin.empty()) {
        QMessageBox::warning(this, "错误", "没有可处理的图像");
        return;
    }

    Mat image = imageOrinin.clone();

    // 检测环境亮度
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);
    Scalar meanBrightness = mean(gray);
    QLOG_DEBUG() << "环境亮度: " << meanBrightness[0];

    bool isDarkEnvironment = meanBrightness[0] < 80;
    if (isDarkEnvironment) {
        QLOG_DEBUG() << "检测到暗色环境，启用暗色优化模式";
    }

	// 第一个颜色范围: hsv(208, 69%, 68%) -> H:104, S:176, V:173//原来的蓝色
    Scalar lowerColor1 = Scalar(94, 100, 100);
    Scalar upperColor1 = Scalar(114, 255, 255);

    // 第二个颜色范围: hsv(187, 64%, 74%) -> H:93, S:163, V:189 (±10范围)
    Scalar lowerColor2 = Scalar(83, 100, 100);
    Scalar upperColor2 = Scalar(103, 255, 255);

    // 查找参照物矩形框（同时检测两个颜色）
    referenceObjectRect = findReferenceObjectRectDualColor(image, lowerColor1, upperColor1, lowerColor2, upperColor2);
    if (referenceObjectRect.width == 0 || referenceObjectRect.height == 0) {
        QMessageBox::warning(this, "提示", "未找到参照物");
        return;
    }

    Mat referenceMask = findReferenceObjectDualColor(image, lowerColor1, upperColor1, lowerColor2, upperColor2);
    if (countNonZero(referenceMask) == 0) {
        QMessageBox::warning(this, "提示", "未找到参照物");
        return;
    }

    double realHeightCm = REFERENCE_HEIGHT;
    pixelToMmRatio = calculatePixelToCmRatio(referenceMask, realHeightCm);
    QLOG_DEBUG() << "像素到毫米的比例(高转换):" << pixelToMmRatio << "像素/毫米";

    int grooveWidth = 180;
    Mat grooveRegion = extractGrooveRegion(image, referenceMask, grooveWidth);

    if (grooveRegion.empty()) {
        QMessageBox::warning(this, "错误", "无法提取凹槽区域");
        return;
    }



    // 创建自定义参数
    BinaryParameters params;
    params.redSMin = 80;        // 提高红色饱和度阈值
    params.redVMin = 60;        // 提高红色亮度阈值
    params.darkRedVMax = 120;   // 调整暗红色最大亮度
    params.blackVMax = 40;      // 降低黑色最大亮度
    params.morphSize = 3;       // 使用较小的形态学核


    //在凹槽带试管区域识别出试管的宽度
    cv::Mat redBinarytmp, darkRedBinarytmp, blackBinarytmp;
    tubeBinaryProcessingWithParams(grooveRegion, redBinarytmp, darkRedBinarytmp, blackBinarytmp, params);

    // 计算外接矩形
    BoundingRectResult resulttmp = calculateMaxBoundingRect(redBinarytmp, darkRedBinarytmp, blackBinarytmp, 100.0);

    MaxRectInfo maxInfotmp = getMaxRectangleInfo(resulttmp);

    Mat testTubeMat = IdentifyWidthOfTheTestTube(grooveRegion,
                                                 static_cast<double>(maxInfotmp.rect.x)   ,
                                                 static_cast<double>(maxInfotmp.rect.width));
    // 计算下针参数
    detectedInterface = Point(0, maxInfotmp.top);
    redBloodCellHeight = maxInfotmp.height;
    maxInfoColor = maxInfotmp.colorType;
    calculateNeedleDropParameters(detectedInterface.y,redBloodCellHeight, pixelToMmRatio);

    // 显示最终结果
    processedImage = testTubeMat.clone();

    //检查PRP 也就是血浆状态离心后的血浆正常情况下应该是
    //淡黄色、清澈透明的。如果出现泛红（呈淡红色、粉红色或洗肉水样），这确实是一个异常现象，需要引起注意。
    //医学检验上被称为 “溶血”
    InspectionResult resultPrp = inspectPRP(testTubeMat,static_cast<double>(maxInfotmp.top));

    displayResults(resultPrp.hemolysisIndex);

    return;

    // 处理凹槽区域并找到试管
//    Mat resultImage = testTubeMat; //findTubeByMultiFeatures(grooveRegion);
}

Mat TestOpcv::IdentifyWidthOfTheTestTube(Mat & grooveTube,const double &left,const double& widthpx){
    // 截取试管区域（保持原始高度）
     Rect tubeROI(left, 0, widthpx, grooveTube.rows);
     Mat testTubeMat = grooveTube(tubeROI).clone();
     return testTubeMat;
}



InspectionResult TestOpcv::inspectPRP(const Mat &testTubeImage, double topRegionRatio)
{
    InspectionResult result;

    // 提取顶部区域（从topRegionRatio位置向上50PX）
    int height = testTubeImage.rows;
    int width = testTubeImage.cols;

    // 计算顶分层处 处朝上试管口40px
    int startY = max(0,static_cast<int>(topRegionRatio - 100));
    int regionHeight =  startY + 70;
    // 确保区域高度有效
    if (regionHeight >= height) {
        result.result = "InvalidRegion";
        result.confidence = 0.0;
        return result;
    }

    Rect topRegion(0, startY , width, 70); //留了30
    Mat topRegionMat = testTubeImage(topRegion);

    // 转换到HSV颜色空间
    Mat hsvImg;
    cvtColor(topRegionMat, hsvImg, COLOR_BGR2HSV);

    // 高斯模糊去噪
    Mat blurredImg;
    GaussianBlur(hsvImg, blurredImg, Size(5, 5), 0);

    // 创建红色掩膜
    Mat maskRed1, maskRed2, redMask;
    inRange(blurredImg,
            Scalar(m_lowerRed1.lowH, m_lowerRed1.lowS, m_lowerRed1.lowV),
            Scalar(m_lowerRed1.highH, m_lowerRed1.highS, m_lowerRed1.highV),
            maskRed1);

    inRange(blurredImg,
            Scalar(m_lowerRed2.lowH, m_lowerRed2.lowS, m_lowerRed2.lowV),
            Scalar(m_lowerRed2.highH, m_lowerRed2.highS, m_lowerRed2.highV),
            maskRed2);

    bitwise_or(maskRed1, maskRed2, redMask);

    // 计算红色像素比例
    int totalPixels = static_cast<int>( redMask.total());
    int redPixels = countNonZero(redMask);
    double redRatio = static_cast<double>(redPixels) / totalPixels;

    // 计算平均HSV值
    Scalar meanHSV = mean(blurredImg);
    double avgH = meanHSV[0];
    double avgS = meanHSV[1];
    double avgV = meanHSV[2];

    // 保存HSV值到QMap
    result.topRegionHSV["h"] = avgH;
    result.topRegionHSV["s"] = avgS;
    result.topRegionHSV["v"] = avgV;

    // 决策逻辑
    if (redRatio > m_hemolysisThreshold) {
        result.result = "Hemolyzed";
        result.confidence = qMin(redRatio * 5, 1.0);
    } else {
        result.result = "Normal";
        result.confidence = 1.0 - redRatio;
    }

    result.hemolysisIndex = redRatio;

    return result;
}
