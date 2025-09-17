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
#include <opencv2/dnn.hpp>

using namespace cv;
using namespace dnn;

cv::Mat ReferenceImage;
cv::Mat OriginPrpImage;
cv::Mat hsvImage;





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
    this->setWindowTitle(tr("校准测高参数"));
    initsignal();

    // 样式设置
    QFile stylespinBox(":/Picture/SetPng/wholeSpinBox.qss");
    if(stylespinBox.open(QIODevice::ReadOnly)) {
        QString setspinBoxQss = QLatin1String(stylespinBox.readAll());
        ui->spinBox->setStyleSheet(setspinBoxQss);
        stylespinBox.close();
    }

    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
        settButtonQss = QLatin1String(styleFile.readAll());
        styleFile.close();
    }

    QHash<QPushButton*, QString> pushButtonList = {
        {ui->pushButton_loadpath, tr("导入图片")},
        {ui->pushButton_prp, tr("测试找试管")},
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

/** 显示图片**/
void displayImage(cv::Mat cvImage,QLabel *pshow)
{
    pshow->clear();
    if (cvImage.empty()) return;

    // 将 BGR 转换为 RGB
    cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);

    // 将 cv::Mat 转换为 QImage
    QImage qImage(cvImage.data, cvImage.cols, cvImage.rows,static_cast<int>(cvImage.step), QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap 并显示在 QLabel 中
    QPixmap pixmap = QPixmap::fromImage(qImage);
    pshow->setPixmap(pixmap.scaled(pshow->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}




void TestOpcv::initsignal()
{
    connect(ui->pushButton_loadpath, &QPushButton::clicked, this, [=]() {
            QString OpenFile = QFileDialog::getOpenFileName(this, tr("请选取图片文件"), "",
                                    "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
            if(!OpenFile.isEmpty()) {
                ui->spinBox->setValue(0);
                std::string cstrPath = OpenFile.toLocal8Bit().constData();
                Mat img = imread(cstrPath);
                if (img.empty()) {
                    QMessageBox::warning(this, "错误", "无法加载图像文件");
                    return;
                }
                imageOrinin = img.clone();
                displayImage(img, ui->label_showimage);

                // 重置检测结果
                redBloodCellHeight = 0;
                detectedInterface = Point(-1, -1);
                referenceObjectRect = Rect(0,0,0,0);
                redBloodCellHeightMm = 0.0;
                maxNeedleDropHeight = 0.0;
                referenceToBottomDistance = 0.0;
            }
    });

    connect(ui->pushButton_prp, &QPushButton::clicked, this, [=]() {
        if (!imageOrinin.empty()) {
            findTubeopencv(imageOrinin);
        } else {
            QMessageBox::warning(this, "提示", "请先导入图片");
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

void TestOpcv::on_spinBox_valueChanged(int arg1)
{
    wheeltoImage(arg1);
}


bool TestOpcv::matImagewheel(cv::Mat cutimage, int degree ,cv::Mat &OutMat)
{
    if (cutimage.empty()) {
        QMessageBox::warning(this, "导入异常", "图片为空或路径包含中文!");
        return false;
    }

    Point2f center(cutimage.cols / 2.0f, cutimage.rows / 2.0f);
    Mat rot = getRotationMatrix2D(center, degree, 1.0);

    // 计算旋转后的图像边界
    double angle = degree * CV_PI / 180.0;
    double sin_val = fabs(sin(angle));
    double cos_val = fabs(cos(angle));

    int newWidth = int(cutimage.rows * sin_val + cutimage.cols * cos_val);
    int newHeight = int(cutimage.rows * cos_val + cutimage.cols * sin_val);

    // 调整旋转矩阵
    rot.at<double>(0, 2) += (newWidth - cutimage.cols) / 2.0;
    rot.at<double>(1, 2) += (newHeight - cutimage.rows) / 2.0;

    warpAffine(cutimage, OutMat, rot, Size(newWidth, newHeight));
    return true;
}

void TestOpcv::showImage(Mat destImage){
   displayImage(destImage, ui->label_showimage);
}

void TestOpcv::wheeltoImage(int wheel)
{
   Mat outImage;
   matImagewheel(ReferenceImage,wheel,outImage);//旋转
   showImage(outImage);// label 显示图像
}

Rect TestOpcv::findReferenceObjectRect(Mat& image, Scalar lowerBound, Scalar upperBound)
{
    Mat hsv, mask;
    cvtColor(image, hsv, COLOR_BGR2HSV);
    inRange(hsv, lowerBound, upperBound, mask);

    // 形态学操作
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(mask, mask, MORPH_CLOSE, kernel);
    morphologyEx(mask, mask, MORPH_OPEN, kernel);

    // 查找轮廓
    vector<vector<Point>> contours;
    findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return Rect(0, 0, 0, 0);
    }

    // 找到最大轮廓的边界矩形
    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    return boundingRect(*largestContour);
}

double TestOpcv::calculateReferenceToBottomDistance(const Rect& referenceRect)
{
    // 计算参照物顶部到图像底部的距离（像素）
    double referenceTopToImageBottom = imageOrinin.rows - referenceRect.y;

    // 转换为毫米（使用像素到毫米的比例）
    return referenceTopToImageBottom / pixelToMmRatio;
}

void TestOpcv::calculateNeedleDropParameters(int interfaceY, int rbcHeightPixels, double pixelToMmRatio)
{
    if (pixelToMmRatio <= 0) {
        QLOG_ERROR() << "错误: 像素到毫米比例无效";
        return;
    }

    // 1. 计算红细胞区域高度（毫米）
    redBloodCellHeightMm = rbcHeightPixels / pixelToMmRatio;

    // 2. 计算参照物顶部到原点的距离（毫米）
    // REFERENCE_TO_BOTTOM 是已知的固定值（49.9mm）
    referenceToBottomDistance = REFERENCE_TO_NEEDLEBOTTOM;

    // 3. 计算分界面到参照物顶部的距离（像素）
    // 参照物顶部在图像中的Y坐标是 referenceObjectRect.y
    double interfaceToReferenceTopPixels = interfaceY - referenceObjectRect.y;

    // 4. 计算分界面到参照物顶部的距离（毫米）
    double interfaceToReferenceTopMm = interfaceToReferenceTopPixels / pixelToMmRatio;

    // 5. 计算下针最大下降高度（毫米）
    // 最大下降高度 = 参照物顶部到针底部原点距离 + 分界面到参照物顶部距离 - 安全余量
    double safetyMargin = 0.0; // 5mm安全余量，避免碰到分界面

    // 针需要从原点下降到分界面上方安全位置的总距离
    double totalDropDistance = referenceToBottomDistance + interfaceToReferenceTopMm;

    // 最大下降高度（从原点开始计算）
    maxNeedleDropHeight = totalDropDistance - safetyMargin;


    // 确保下降高度不为负
    maxNeedleDropHeight = max(0.0, maxNeedleDropHeight);

    QLOG_DEBUG() << "计算参数:";
    QLOG_DEBUG() << "红细胞高度像素: " << rbcHeightPixels << "px";
    QLOG_DEBUG() << "像素到毫米比例: " << pixelToMmRatio << "px/mm";
    QLOG_DEBUG() << "红细胞高度: " << redBloodCellHeightMm << "mm";
    QLOG_DEBUG() << "分界面Y坐标: " << interfaceY << "px";
    QLOG_DEBUG() << "参照物顶部Y坐标: " << referenceObjectRect.y << "px";
    QLOG_DEBUG() << "分界面到参照物顶部距离: " << interfaceToReferenceTopPixels << "px";
    QLOG_DEBUG() << "分界面到参照物顶部距离: " << interfaceToReferenceTopMm << "mm";
    QLOG_DEBUG() << "参照物到针底部距离: " << referenceToBottomDistance << "mm";
    QLOG_DEBUG() << "下针最大下降高度: " << maxNeedleDropHeight << "mm";
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

    Scalar lowerBlue = Scalar(100, 50, 50);
    Scalar upperBlue = Scalar(140, 255, 255);

    // 查找参照物矩形框
    referenceObjectRect = findReferenceObjectRect(image, lowerBlue, upperBlue);
    if (referenceObjectRect.width == 0 || referenceObjectRect.height == 0) {
        QMessageBox::warning(this, "提示", "未找到参照物");
        return;
    }

    Mat referenceMask = findReferenceObject(image, lowerBlue, upperBlue);
    if (countNonZero(referenceMask) == 0) {
        QMessageBox::warning(this, "提示", "未找到参照物");
        return;
    }

    double realHeightCm = REFERENCE_HEIGHT;
    pixelToMmRatio = calculatePixelToCmRatio(referenceMask, realHeightCm)/* *10*/; //转换为px/mm
    QLOG_DEBUG() << "像素到毫米的比例:" << pixelToMmRatio << "像素/毫米";

    int grooveWidth = 180;
    Mat grooveRegion = extractGrooveRegion(image, referenceMask, grooveWidth);

    imshow("111",grooveRegion);


    if (grooveRegion.empty()) {
        QMessageBox::warning(this, "错误", "无法提取凹槽区域");
        return;
    }

    // 暗色环境下的特殊处理
    if (isDarkEnvironment) {
        QLOG_DEBUG() << "检测到暗色环境，应用增强预处理";

        // 使用自适应直方图均衡化增强对比度
        Mat lab;
        cvtColor(grooveRegion, lab, COLOR_BGR2Lab);
        vector<Mat> labChannels;
        split(lab, labChannels);

        // 只对亮度通道进行CLAHE
        Ptr<CLAHE> clahe = createCLAHE();
        clahe->setClipLimit(3.0);
        clahe->apply(labChannels[0], labChannels[0]);

        merge(labChannels, lab);
        cvtColor(lab, grooveRegion, COLOR_Lab2BGR);

        // 适度增加对比度
        grooveRegion.convertTo(grooveRegion, -1, 1.2, 10);
    }


    // 处理凹槽区域并找到试管
    Mat resultImage = findTubeByMultiFeatures(grooveRegion);

    imshow("222",resultImage);
    // 显示中间结果用于调试
    displayImage(resultImage, ui->label_showimage);


    findTubeopencv(resultImage);

    // 计算下针参数
    calculateNeedleDropParameters(detectedInterface.y, redBloodCellHeight, pixelToMmRatio);

    // 显示最终结果
    displayResults();

}

Mat TestOpcv::findReferenceObject(Mat& image, Scalar lowerBound, Scalar upperBound) {
    Mat hsv, mask;

    // 转换到HSV颜色空间，更容易进行颜色分割
    cvtColor(image, hsv, COLOR_BGR2HSV);

    // 根据颜色范围创建掩码
    inRange(hsv, lowerBound, upperBound, mask);

    // 形态学操作去除噪声
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

    // 找到最大的轮廓
    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    Rect boundingRect = cv::boundingRect(*largestContour);
    double pixelHeight = boundingRect.height;

    return pixelHeight / realHeightCm;
}

Mat TestOpcv::extractGrooveRegion(Mat& image, Mat& referenceMask, int grooveWidth) {
    vector<vector<Point>> contours;
    findContours(referenceMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        return Mat();
    }

    // 找到参照物的边界框
    auto largestContour = max_element(contours.begin(), contours.end(),
        [](const vector<Point>& a, const vector<Point>& b) {
            return contourArea(a) < contourArea(b);
        });

    Rect refRect = cv::boundingRect(*largestContour);

    // 假设凹槽在参照物右侧，宽度为grooveWidth，高度与图像相同
    int grooveX = refRect.x + refRect.width + 40; // 右侧偏移40像素
    int grooveY = 0;
    int grooveHeight = image.rows;

    // 确保不超出图像边界
    grooveX = min(max(grooveX, 0), image.cols - 1);
    grooveWidth = min(grooveWidth, image.cols - grooveX);

    Rect grooveRect(grooveX, grooveY, grooveWidth, grooveHeight);

    return image(grooveRect).clone();
}

void TestOpcv::markResultsOnOriginalImage(Mat& originalImage, const Point& interfacePoint,
                                          int rbcHeight, const Rect& referenceRect)
{
     // 创建叠加层用于半透明效果
     Mat overlay = originalImage.clone();

     // 1. 标记红细胞区域（半透明红色矩形）
     if (interfacePoint.y >= 0 && interfacePoint.y < originalImage.rows) {
         Rect rbcRegion(0, interfacePoint.y, originalImage.cols, rbcHeight);

         // 绘制半透明红色矩形
         rectangle(overlay, rbcRegion, Scalar(0, 0, 255), -1);
         double alpha = 0.3; // 透明度
         addWeighted(overlay, alpha, originalImage, 1 - alpha, 0, originalImage);

         // 绘制红细胞区域边界
         rectangle(originalImage, rbcRegion, Scalar(0, 0, 200), 2);
     }

     // 2. 标记参照物矩形框（绿色）
     if (referenceRect.width > 0 && referenceRect.height > 0) {
         rectangle(originalImage, referenceRect, Scalar(0, 255, 0), 3);
         putText(originalImage, "Reference Object",
                 Point(referenceRect.x, referenceRect.y - 10),
                 FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
     }

     // 3. 标记分界线（蓝色）
     if (interfacePoint.y >= 0 && interfacePoint.y < originalImage.rows) {
         line(originalImage, Point(0, interfacePoint.y),
              Point(originalImage.cols, interfacePoint.y), Scalar(255, 0, 0), 2);

         // 标记分界点
         circle(originalImage, Point(originalImage.cols / 2, interfacePoint.y),
                10, Scalar(255, 0, 0), -1);
     }

     // 4. 添加文字信息
     int fontFace = FONT_HERSHEY_SIMPLEX;
     double fontScale = 0.7;
     int thickness = 2;

     // 分界面信息
     putText(originalImage, "Plasma-RBC Interface",
             Point(10, interfacePoint.y - 20), fontFace, fontScale,
             Scalar(255, 0, 0), thickness);

     // 红细胞高度信息
     string heightText = "RBC Height: " + to_string(rbcHeight) + " px";
     putText(originalImage, heightText, Point(10, 30), fontFace, 0.6,
             Scalar(255, 255, 255), thickness);

    string heightTextMm = "RBC Height: " + toString(redBloodCellHeightMm, 1) + " mm";
    putText(originalImage, heightTextMm, Point(10, 60), fontFace, 0.6,
                 Scalar(255, 255, 255), thickness);

     // 下针深度信息
     string needleText = "Max Needle Drop: " + toString(maxNeedleDropHeight, 1) + " mm";
     putText(originalImage, needleText, Point(10, 90), fontFace, 0.6,
             Scalar(255, 255, 0), thickness);

     // 参照物信息
     if (referenceRect.width > 0) {
         string refText = "Reference: " + to_string(referenceRect.width) +
                          "x" + to_string(referenceRect.height) + " px";
         putText(originalImage, refText, Point(10, 120), fontFace, 0.5,
                 Scalar(0, 255, 0), 1);
     }
}

void TestOpcv::displayResults()
{
    if (processedImage.empty() || detectedInterface.y < 0) {
        QMessageBox::warning(this, "错误", "未找到有效结果");
        return;
    }

    // 在原始图像上标记所有结果
    Mat resultImage = imageOrinin.clone();
    markResultsOnOriginalImage(resultImage, detectedInterface, redBloodCellHeight, referenceObjectRect);

    // 显示标记后的图像
    displayImage(resultImage, ui->label_showimage);

    // 更新界面信息
    QString infoText = QString("检测完成:\n"
                              "分界面Y坐标: %1 像素\n"
                              "红细胞高度: %2 像素 (%3 mm)\n"
                              "参照物尺寸: %4x%5 像素\n"
                              "下针最大下降高度: %6 mm\n"
                              "参照物到原点距离: %7 mm")
                         .arg(detectedInterface.y)
                         .arg(redBloodCellHeight)
                         .arg(redBloodCellHeightMm, 0, 'f', 1)
                         .arg(referenceObjectRect.width)
                         .arg(referenceObjectRect.height)
                         .arg(maxNeedleDropHeight, 0, 'f', 1)
                         .arg(REFERENCE_TO_NEEDLEBOTTOM, 0, 'f', 1);

    ui->label_ratio->setText(infoText);
    ui->label_ratio->setStyleSheet("QLabel { background-color: white; color: black; padding: 5px; }");
}






















void TestOpcv::findTubeopencv(Mat imageOrinin){
    if (imageOrinin.empty()) return;

    Mat tubeImage = imageOrinin.clone();
    QLOG_DEBUG() << "试管图像尺寸:" << tubeImage.cols << "x" << tubeImage.rows;

    if (isBottomOverexposed(tubeImage)) {
        QLOG_DEBUG() << "检测到试管底部可能过曝，启用抗过曝处理...";
    }

    detectedInterface = findPlasmaRBCInterface(tubeImage);
    if (detectedInterface.y < 0 || detectedInterface.y >= tubeImage.rows) {
        QLOG_ERROR() << "错误: 分界面检测失败";
        return;
    }

    redBloodCellHeight = calculateRedBloodCellHeight(tubeImage, detectedInterface.y);
    QLOG_DEBUG() << "检测完成 - 分界面Y:" << detectedInterface.y
                 << ", 红细胞高度:" << redBloodCellHeight << "像素";

    // 保存处理后的图像
    processedImage = tubeImage.clone();
    drawInterfaceResult(processedImage, detectedInterface.y, redBloodCellHeight);

    // 显示中间结果
    displayImage(processedImage, ui->label_showimage);
}



// 检测试管底部是否过曝
bool TestOpcv::isBottomOverexposed(const Mat& tubeImage, int bottomRegionHeight) {
    if (tubeImage.rows < bottomRegionHeight * 2) {
        return false;
    }

    // 取底部区域
    Rect bottomRegion(0, tubeImage.rows - bottomRegionHeight, tubeImage.cols, bottomRegionHeight);
    Mat bottomROI = tubeImage(bottomRegion);

    // 转换为HSV颜色空间，更好地检测过曝
    Mat hsv;
    cvtColor(bottomROI, hsv, COLOR_BGR2HSV);

    // 分离V通道（亮度）
    vector<Mat> hsvChannels;
    split(hsv, hsvChannels);
    Mat valueChannel = hsvChannels[2];

    // 计算过曝像素比例（V值 > 240）
    Mat overexposedMask;
    threshold(valueChannel, overexposedMask, 240, 255, THRESH_BINARY);
    double overexposedRatio = (double)countNonZero(overexposedMask) / (bottomROI.rows * bottomROI.cols);

    // 计算底部平均亮度
    Scalar bottomMean = mean(valueChannel);
    double bottomBrightness = bottomMean[0];

    // 计算中间区域的平均亮度作为参考
    Rect middleRegion(0, tubeImage.rows / 3, tubeImage.cols, tubeImage.rows / 3);
    Mat middleROI = tubeImage(middleRegion);
    cvtColor(middleROI, hsv, COLOR_BGR2HSV);
    split(hsv, hsvChannels);
    Scalar middleMean = mean(hsvChannels[2]);
    double middleBrightness = middleMean[0];

    QLOG_DEBUG() << "底部亮度: " << bottomBrightness << ", 中间亮度: " << middleBrightness
                 << ", 过曝像素比例: " << overexposedRatio * 100 << "%" << endl;

    // 多重条件判断过曝
    bool isOverexposed = (overexposedRatio > 0.3) || // 过曝像素比例超过30%
                        (bottomBrightness > middleBrightness + 50) || // 亮度差异大
                        (bottomBrightness > 200 && overexposedRatio > 0.1); // 高亮度且有一定过曝比例

    return isOverexposed;
}

// 查找多个峰值
vector<int> TestOpcv::findMultiplePeaks(const vector<double>& gradients, int minDistance) {
    vector<int> peaks;
    vector<pair<int, double>> peakCandidates;

    // 寻找所有局部峰值
    for (int i = 1; i < gradients.size() - 1; i++) {
        if (gradients[i] > gradients[i-1] && gradients[i] > gradients[i+1] && gradients[i] > 5) {
            peakCandidates.push_back({i, gradients[i]});
        }
    }

    // 按梯度值排序
    sort(peakCandidates.begin(), peakCandidates.end(),
         [](const pair<int, double>& a, const pair<int, double>& b) {
             return a.second > b.second;
         });

    // 选择不相近的峰值
    for (const auto& candidate : peakCandidates) {
        bool tooClose = false;
        for (int existingPeak : peaks) {
            if (abs(candidate.first - existingPeak) < minDistance) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            peaks.push_back(candidate.first);
        }
        if (peaks.size() >= 3) break; // 最多取3个峰值
    }

    return peaks;
}

// 计算区域对比度
double TestOpcv::calculateRegionContrast(const Mat& image, int yPosition) {
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // 应用中值滤波减少噪声
    Mat filtered;
    medianBlur(gray, filtered, 5);

    // 分界线上方区域（血浆）
    int regionHeight = 20;
    int topY = max(0, yPosition - regionHeight);
    Rect topRegion(0, topY, filtered.cols, regionHeight);

    // 分界线下方区域（红细胞）
    int bottomY = min(filtered.rows - regionHeight, yPosition);
    Rect bottomRegion(0, bottomY, filtered.cols, regionHeight);

    // 计算标准差作为对比度指标（更好的区分度）
    Scalar topMean, topStdDev;
    Scalar bottomMean, bottomStdDev;

    meanStdDev(filtered(topRegion), topMean, topStdDev);
    meanStdDev(filtered(bottomRegion), bottomMean, bottomStdDev);

    // 使用亮度和纹理差异的综合指标
    double brightnessDiff = abs(topMean[0] - bottomMean[0]);
    double textureDiff = abs(topStdDev[0] - bottomStdDev[0]);

    return brightnessDiff + textureDiff * 0.5; // 结合亮度和纹理差异
}

// 计算垂直梯度
vector<double> TestOpcv::calculateVerticalGradient(const Mat& image) {
    Mat gray;
    cvtColor(image, gray, COLOR_BGR2GRAY);

    // 应用中值滤波去除噪声
    Mat filtered;
    medianBlur(gray, filtered, 5);

    // 计算每行的平均亮度（忽略边缘区域）
    vector<double> rowMeans(filtered.rows);
    for (int y = 0; y < filtered.rows; y++) {
        double sum = 0;
        int validPixels = 0;

        // 只使用中间70%的宽度，避免试管边缘干扰
        int startX = filtered.cols * 0.15;
        int endX = filtered.cols * 0.85;

        for (int x = startX; x < endX; x++) {
            sum += filtered.at<uchar>(y, x);
            validPixels++;
        }
        rowMeans[y] = validPixels > 0 ? sum / validPixels : 0;
    }

    // 计算梯度（一阶差分）
    vector<double> gradients(filtered.rows - 1);
    for (int y = 1; y < filtered.rows; y++) {
        gradients[y-1] = abs(rowMeans[y] - rowMeans[y-1]);
    }

    return gradients;
}

// 平滑梯度曲线
vector<double> TestOpcv::smoothGradient(const vector<double>& gradients, int windowSize) {
    vector<double> smoothed = gradients;
    int halfWindow = windowSize / 2;

    for (int i = halfWindow; i < gradients.size() - halfWindow; i++) {
        double sum = 0;
        for (int j = -halfWindow; j <= halfWindow; j++) {
            sum += gradients[i + j];
        }
        smoothed[i] = sum / windowSize;
    }

    return smoothed;
}

// 查找血浆-红细胞分界面
Point TestOpcv::findPlasmaRBCInterface(const Mat& tubeImage) {
    vector<double> gradients = calculateVerticalGradient(tubeImage);
    vector<double> colorFeatures = calculateColorFeatures(tubeImage);
    vector<double> darkEnvFeatures = calculateDarkEnvironmentFeatures(tubeImage);
    vector<double> interfaceColorFeatures = calculateInterfaceColorFeatures(tubeImage);

    if (gradients.empty() || colorFeatures.empty() || darkEnvFeatures.empty()) {
        QLOG_ERROR() << "特征计算失败，使用默认位置";
        return Point(0, tubeImage.rows / 2);
    }

    // 2. 平滑处理
    vector<double> smoothedGradients = smoothGradient(gradients, 5);
    vector<double> smoothedColor = smoothGradient(colorFeatures, 7);
    vector<double> smoothedDark = smoothGradient(darkEnvFeatures, 5);
    vector<double> smoothedInterfaceColor = smoothGradient(interfaceColorFeatures, 5);

    // 3. 环境亮度检测
    Mat gray;
    cvtColor(tubeImage, gray, COLOR_BGR2GRAY);
    Scalar meanBrightness = mean(gray);
    bool isDarkEnvironment = meanBrightness[0] < 80;
    QLOG_DEBUG() << "环境亮度: " << meanBrightness[0] << ", 暗色环境: " << isDarkEnvironment;




    // 4. 特征融合
    vector<double> combinedScores(tubeImage.rows, 0);
    for (int i = 0; i < combinedScores.size(); i++) {
        double gradientScore = (i < smoothedGradients.size()) ? smoothedGradients[i] : 0;
        double colorScore = (i < smoothedColor.size()) ? smoothedColor[i] : 0;
        double darkScore = (i < smoothedDark.size()) ? smoothedDark[i] : 0;
        double interfaceScore = (i < smoothedInterfaceColor.size()) ? smoothedInterfaceColor[i] : 0;

        // 基础分数组合
        combinedScores[i] = gradientScore * 1.0 + colorScore * 0.8 + darkScore * 0.6 + interfaceScore * 0.7;

        // 区域权重调整
        if (i < tubeImage.rows * 0.3) {
            combinedScores[i] *= 0.1; // 顶部区域大幅降低权重（避免空气界面）
        } else if (i > tubeImage.rows * 0.6 && i < tubeImage.rows * 0.9) {
            combinedScores[i] *= 2.0; // 中下部区域加倍权重
        }

        // === 新增：空气界面检测和惩罚 ===
        if (isLikelyAirInterface(tubeImage, i)) {
            QLOG_DEBUG() << "位置 " << i << " 疑似空气界面，大幅降低评分";
            combinedScores[i] *= 0.05; // 降低95%的评分
        }
    }

    // 5. 查找所有峰值
    vector<int> peaks;
    for (int i = 2; i < combinedScores.size() - 2; i++) {
        if (combinedScores[i] > combinedScores[i-1] &&
            combinedScores[i] > combinedScores[i+1] &&
            combinedScores[i] > combinedScores[i-2] &&
            combinedScores[i] > combinedScores[i+2] &&
            combinedScores[i] > 5.0) {
            peaks.push_back(i);
        }
    }

    QLOG_DEBUG() << "找到 " << peaks.size() << " 个候选峰值";

    // 6. 选择最佳峰值
    int bestPeak = -1;
    double bestScore = 0;


    for (int peak : peaks) {
        // 跳过不合理的位置
        if (peak < tubeImage.rows * 0.1 || peak > tubeImage.rows * 0.9) {
            continue;
        }

        // 如果底部过曝，跳过底部区域的峰值
        if (isBottomOverexposed(tubeImage) && peak > tubeImage.rows * 0.7) {
            continue;
        }

        double contrast = calculateRegionContrast(tubeImage, peak);
        double colorStrength = (peak < smoothedColor.size()) ? smoothedColor[peak] : 0;
        double gradientStrength = (peak < smoothedGradients.size()) ? smoothedGradients[peak] : 0;

        // 综合评分（已包含空气界面惩罚）
        double score = combinedScores[peak] * (1.0 + contrast * 0.1 + colorStrength * 0.2);

        // === 新增：溶血特异性检测 ===
        bool isLikelyHemolyzed = isHemolyzed(tubeImage, peak);

        // 溶血情况下调整评分策略
        if (isLikelyHemolyzed) {
            score = gradientStrength * 1.5 + contrast * 1.2 + colorStrength * 0.3;
        }

        QLOG_DEBUG() << "候选位置 " << peak << ": 总分=" << score<< ", 对比度=" << contrast
                     << ", 颜色强度=" << colorStrength<< ", 溶血=" << isLikelyHemolyzed;
        if (score > bestScore) {
           bestScore = score;
           bestPeak = peak;
       }
    }

    // === 新增：溶血验证和后处理 ===
    if (bestPeak != -1) {
        // 检查是否可能误检了液面顶部
        bool isHemolyzedCase = isHemolyzed(tubeImage, bestPeak);
        bool isTooHigh = (bestPeak < tubeImage.rows * 0.3);
        bool hasHighContrast = calculateRegionContrast(tubeImage, bestPeak) > 70;

        if (isHemolyzedCase && isTooHigh && hasHighContrast) {
            QLOG_DEBUG() << "警告：可能误检了溶血样本的液面顶部，尝试寻找替代位置";

            // 向下搜索更合理的位置
            int alternativePeak = -1;
            double alternativeScore = 0;

            for (int y = static_cast<int>(tubeImage.rows * 0.4);
                 y < static_cast<int>(tubeImage.rows * 0.8); y++) {

                double contrast = calculateRegionContrast(tubeImage, y);
                double gradient = (y < smoothedGradients.size()) ? smoothedGradients[y] : 0;

                // 溶血情况下偏好中等对比度和较高梯度的位置
                double altScore = gradient * 1.2 + contrast * 0.8;

                if (altScore > alternativeScore) {
                    alternativeScore = altScore;
                    alternativePeak = y;
                }
            }

            if (alternativePeak != -1 && alternativeScore > bestScore * 0.7) {
                bestPeak = alternativePeak;
                QLOG_DEBUG() << "使用替代位置: " << bestPeak << ", 评分: " << alternativeScore;
            }
        }
    }

    // 7. 备用策略
    if (bestPeak == -1) {
        QLOG_DEBUG() << "未找到合适峰值，使用备用策略";

        // 策略1: 寻找颜色特征最强的位置
        auto maxColor = max_element(smoothedColor.begin(), smoothedColor.end());
        int colorPeak = distance(smoothedColor.begin(), maxColor);

        // 策略2: 寻找对比度最大的位置
        int contrastPeak = -1;
        double maxContrast = 0;
        for (int i = tubeImage.rows * 0.3; i < tubeImage.rows * 0.8; i++) {
            double contrast = calculateRegionContrast(tubeImage, i);
            if (contrast > maxContrast) {
                maxContrast = contrast;
                contrastPeak = i;
            }
        }

        // 选择更合理的位置
        if (colorPeak >= tubeImage.rows * 0.2 && colorPeak <= tubeImage.rows * 0.8) {
            bestPeak = colorPeak;
            QLOG_DEBUG() << "使用颜色特征备用位置: " << bestPeak;
        } else if (contrastPeak != -1) {
            bestPeak = contrastPeak;
            QLOG_DEBUG() << "使用对比度备用位置: " << bestPeak;
        } else {
            bestPeak = tubeImage.rows * 0.5;
            QLOG_DEBUG() << "使用默认中间位置: " << bestPeak;
        }
    }

    // 8. 最终验证
    double finalContrast = calculateRegionContrast(tubeImage, bestPeak);
    if (finalContrast < 5.0) {
        QLOG_DEBUG() << "警告: 最终位置对比度过低(" << finalContrast << ")，尝试向上搜索";

        // 向上搜索更好的位置
        for (int y = bestPeak - 20; y > tubeImage.rows * 0.3; y--) {
            double contrast = calculateRegionContrast(tubeImage, y);
            if (contrast > finalContrast * 1.5) {
                bestPeak = y;
                finalContrast = contrast;
                QLOG_DEBUG() << "调整到更好位置: " << bestPeak << ", 对比度: " << finalContrast;
                break;
            }
        }
    }

    // 9. 确保位置合理
	int searchmax = static_cast<int>(tubeImage.rows * 0.2);
	int searchmin = std::min(bestPeak, static_cast<int>(tubeImage.rows * 0.8));
    bestPeak = std::max(searchmax, searchmin);

    QLOG_DEBUG() << "最终分界面位置: " << bestPeak
                 << ", 对比度: " << finalContrast
                 << ", 环境亮度: " << meanBrightness[0];

    return Point(0, bestPeak);
}

// 绘制分界面结果
void TestOpcv::drawInterfaceResult(Mat& image, int interfaceY,int rbcHeight) {
    // 绘制分界线
    line(image, Point(0, interfaceY), Point(image.cols, interfaceY), Scalar(0, 0, 255), 3);
    circle(image, Point(image.cols / 2, interfaceY), 8, Scalar(255, 0, 0), -1);

    // 标记红细胞区域（半透明红色矩形）
    Mat overlay = image.clone();
    Rect rbcRegion(0, interfaceY, image.cols, rbcHeight);
    rectangle(overlay, rbcRegion, Scalar(0, 0, 255), -1); // 填充红色

    // 添加透明度
    double alpha = 0.3; // 透明度
    addWeighted(overlay, alpha, image, 1 - alpha, 0, image);

    // 绘制红细胞区域边界
    rectangle(image, rbcRegion, Scalar(0, 0, 200), 2);

    // 添加文字标注
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;

    // 血浆区域标注
    putText(image, "Plasma", Point(10, interfaceY - 15),
            fontFace, fontScale, Scalar(255, 0, 0), thickness);

    // 红细胞区域标注
    putText(image, "Red Blood Cells", Point(10, interfaceY + 30),
            fontFace, fontScale, Scalar(0, 0, 255), thickness);

    // 添加高度信息
    string coordText = "Interface Y: " + to_string(interfaceY);
    putText(image, coordText, Point(10, 30), fontFace, 0.6, Scalar(255, 255, 255), thickness);

    string heightText = "RBC Height: " + to_string(rbcHeight) + " px";
    putText(image, heightText, Point(10, 60), fontFace, 0.6, Scalar(255, 255, 255), thickness);

    //string ratioText = "RBC Ratio: " + to_string((double)rbcHeight / image.rows * 100, 1) + "%";
    //putText(image, ratioText, Point(10, 90), fontFace, 0.6, Scalar(255, 255, 255), thickness);
}

// 添加辅助函数用于格式化浮点数
string TestOpcv::toString(double value, int precision) {
    stringstream stream;
    stream << fixed << setprecision(precision) << value;
    return stream.str();
}

// 备用策略：在非过曝区域寻找最强梯度
int TestOpcv::findFallbackInterface(const Mat& tubeImage, int bottomStartY) {
    vector<double> gradients = calculateVerticalGradient(tubeImage);

    // 只考虑非底部区域
    int maxGradientPos = 0;
    double maxGradient = 0;

    for (int i = 0; i < bottomStartY && i < gradients.size(); i++) {
        if (gradients[i] > maxGradient) {
            maxGradient = gradients[i];
            maxGradientPos = i;
        }
    }

    // 如果找到有效梯度，返回位置
    if (maxGradient > 10) {
        return maxGradientPos;
    }

    // 否则返回中间偏上位置
    return tubeImage.rows * 0.3;
}
// 寻找备选分界面（在当前选择的上方寻找）
int TestOpcv::findAlternativeInterface(const Mat& tubeImage, int currentPeak) {
    vector<double> gradients = calculateVerticalGradient(tubeImage);

    // 在当前峰值的上方区域（至少向上50像素）寻找次强梯度
    int searchStart = max(0, currentPeak - 100);
    int searchEnd = max(0, currentPeak - 20);

    int bestAltPeak = currentPeak;
    double bestAltGradient = 0;

    for (int i = searchStart; i < searchEnd; i++) {
        if (i < gradients.size() && gradients[i] > bestAltGradient) {
            bestAltGradient = gradients[i];
            bestAltPeak = i;
        }
    }

    return bestAltPeak;
}

// 计算红细胞区域高度
int TestOpcv::calculateRedBloodCellHeight(const Mat& tubeImage, int interfaceY) {
    // 红细胞区域从分界面到试管底部
    int rbcHeight = tubeImage.rows - interfaceY;

    // 如果底部过曝，可能需要调整高度计算
    if (isBottomOverexposed(tubeImage)) {
        // 找到有效的红细胞区域底部（避免过曝区域）
        int validBottom = findValidBottom(tubeImage, interfaceY);
        rbcHeight = validBottom - interfaceY;
    }

    return max(0, rbcHeight); // 确保高度不为负
}

// 找到有效的红细胞区域底部（避免过曝区域）
int TestOpcv::findValidBottom(const Mat& tubeImage, int interfaceY) {
    Mat gray;
    cvtColor(tubeImage, gray, COLOR_BGR2GRAY);

    // 从分界面开始向下扫描，找到过曝开始的区域
    int overexposureThreshold = 240; // 过曝阈值
    int minValidPixels = gray.cols * 0.3; // 需要至少30%的有效像素

    int validBottom = tubeImage.rows; // 默认到底部

    for (int y = interfaceY; y < tubeImage.rows; y++) {
        int overexposedCount = 0;

        // 扫描该行的中间70%区域
        int startX = gray.cols * 0.15;
        int endX = gray.cols * 0.85;

        for (int x = startX; x < endX; x++) {
            if (gray.at<uchar>(y, x) > overexposureThreshold) {
                overexposedCount++;
            }
        }

        // 如果过曝像素超过阈值，认为从此处开始无效
        double overexposedRatio = (double)overexposedCount / (endX - startX);
        if (overexposedRatio > 0.5) { // 超过50%像素过曝
            validBottom = y;
            break;
        }
    }

    return validBottom;
}





// 计算轮廓的U型多特征融合的精确定位
Mat TestOpcv::findTubeByMultiFeatures(Mat& inputImage) {
    Mat result;
    inputImage.copyTo(result);

    // 1. 环境检测和预处理
    Mat grayEnv;
    cvtColor(inputImage, grayEnv, COLOR_BGR2GRAY);
    Scalar meanBrightness = mean(grayEnv);
    bool isDarkEnvironment = meanBrightness[0] < 80;

    Mat enhanced;
    if (isDarkEnvironment) {
        // 暗色环境：增强对比度但避免过曝白色区域
        inputImage.convertTo(enhanced, CV_32F, 1.0/255.0);
        pow(enhanced, 1.5, enhanced); // 适度的伽马校正
        enhanced.convertTo(enhanced, CV_8U, 255.0);
    } else {
        enhanced = inputImage.clone();
    }

    Mat gray;
    cvtColor(enhanced, gray, COLOR_BGR2GRAY);

    // 2. 专门检测白色凹槽区域并排除
    Mat whiteMask;
    inRange(inputImage, Scalar(200, 200, 200), Scalar(255, 255, 255), whiteMask);

    // 形态学操作去除噪声
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    morphologyEx(whiteMask, whiteMask, MORPH_CLOSE, kernel);
    morphologyEx(whiteMask, whiteMask, MORPH_OPEN, kernel);

    // 3. 分析白色区域的位置特征
    vector<vector<Point>> whiteContours;
    findContours(whiteMask, whiteContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    int whiteBottomY = -1;
    for (const auto& contour : whiteContours) {
        Rect rect = boundingRect(contour);
        // 白色区域通常在图像底部，且宽度较大
        if (rect.y > gray.rows * 0.7 && rect.width > gray.cols * 0.3) {
            whiteBottomY = max(whiteBottomY, rect.y);
            rectangle(result, rect, Scalar(255, 0, 0), 2); // 蓝色标记白色凹槽
        }
    }

    // 4. 特征提取 - 重点排除白色凹槽区域
    Mat background;
    blur(gray, background, Size(31, 31));
    Mat difference;
    absdiff(gray, background, difference);

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

    // 5. 创建特征图 - 排除白色凹槽影响
    Mat featureMap = Mat::zeros(gray.size(), CV_32F);

    for (int y = 0; y < gray.rows; y++) {
        // 如果是白色凹槽区域，权重为零
        if (whiteBottomY != -1 && y >= whiteBottomY) {
            continue;
        }

        for (int x = 0; x < gray.cols; x++) {
            float diffVal = difference.at<uchar>(y, x) / 255.0f;
            float gradVal = gradY.at<uchar>(y, x) / 255.0f;

            // 暗色环境权重调整
            float darkWeight = (gray.at<uchar>(y, x) < 50) ? 2.0f : 1.0f;
            float positionWeight = (y > gray.rows * 0.6) ? 1.5f : 1.0f;

            featureMap.at<float>(y, x) = (diffVal * darkWeight + gradVal) * positionWeight;
        }
    }

    // 6. 垂直投影分析 - 限制搜索范围
    Mat verticalProj;
    reduce(featureMap, verticalProj, 1, REDUCE_SUM, CV_32F);

    // 设置合理的搜索范围，避免白色凹槽区域
    int searchStart = static_cast<int>(gray.rows * 0.4);  // 从40%高度开始搜索

    // 修正：使用 std::min 和 std::max
    int searchEnd;
    if (whiteBottomY != -1) {
        searchEnd = std::min(whiteBottomY - 10, static_cast<int>(gray.rows * 0.9));
    } else {
        searchEnd = static_cast<int>(gray.rows * 0.9);
    }
    searchEnd = std::max(searchEnd, searchStart + 50); // 确保有足够的搜索范围

    int bestY = -1;
    float bestScore = 0;

    // 7. 寻找采血管底部
    for (int y = searchStart; y < searchEnd; y++) {
        float score = verticalProj.at<float>(y);

        // 添加连续性约束
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

    // 8. 多重验证和备用策略
    if (bestY == -1 || bestScore < 5.0f) {
        QLOG_DEBUG() << "主算法未找到合适底部，使用颜色特征备用策略";

        // 备用策略：基于颜色变化寻找血浆-红细胞分界面
        vector<double> rowMeans(gray.rows);
        for (int y = 0; y < gray.rows; y++) {
            double sum = 0;
            int count = 0;
            for (int x = static_cast<int>(gray.cols * 0.2); x < static_cast<int>(gray.cols * 0.8); x++) {
                sum += gray.at<uchar>(y, x);
                count++;
            }
            rowMeans[y] = (count > 0) ? sum / count : 0;
        }

        // 寻找亮度变化最大的位置
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

    // 9. 最终合理性检查
    if (bestY != -1) {
        // 确保检测位置在合理范围内
        if (bestY < static_cast<int>(gray.rows * 0.3) || bestY > static_cast<int>(gray.rows * 0.85)) {
            QLOG_DEBUG() << "检测位置超出合理范围，使用保守估计";
            bestY = static_cast<int>(gray.rows * 0.7);
        }

        // 确保不会检测到白色凹槽区域
        if (whiteBottomY != -1 && bestY >= whiteBottomY - 20) {
            QLOG_DEBUG() << "检测位置接近白色凹槽，向上调整";
            bestY = whiteBottomY - 30;
        }

        line(result, Point(0, bestY), Point(result.cols, bestY), Scalar(0, 255, 0), 3);
        putText(result, "Tube Bottom: " + to_string(bestY),
               Point(10, bestY - 10), FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
    } else {
        bestY = static_cast<int>(gray.rows * 0.7);
        QLOG_DEBUG() << "使用默认底部位置: " << bestY;
    }

    // 10. 截取有效区域
    Rect roi(0, 0, inputImage.cols, bestY);
    Mat croppedImage = inputImage(roi).clone();

    // 在结果图像上标记白色凹槽区域
    if (whiteBottomY != -1) {
        line(result, Point(0, whiteBottomY), Point(result.cols, whiteBottomY),
             Scalar(255, 0, 0), 2);
        putText(result, "White Groove", Point(10, whiteBottomY - 10),
               FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 1);
    }

    return croppedImage;
}

// 在 calculateVerticalGradient 方法后添加颜色特征检测
vector<double> TestOpcv::calculateColorFeatures(const Mat& tubeImage) {
    vector<double> colorScores(tubeImage.rows);
    Mat hsv;
    cvtColor(tubeImage, hsv, COLOR_BGR2HSV);

    for (int y = 0; y < tubeImage.rows; y++) {
        double redScore = 0;
        int count = 0;

        // 检测中间区域
        int startX = tubeImage.cols * 0.3;
        int endX = tubeImage.cols * 0.7;

        for (int x = startX; x < endX; x++) {
            Vec3b pixel = hsv.at<Vec3b>(y, x);

            // 红色检测 (HSV空间中的红色有两个范围)
           bool isRed1 = (pixel[0] >= 0 && pixel[0] <= 10) && pixel[1] >= 50 && pixel[2] >= 30;
           bool isRed2 = (pixel[0] >= 170 && pixel[0] <= 180) && pixel[1] >= 50 && pixel[2] >= 30;

           // 暗红色检测（针对暗色红细胞）
           bool isDarkRed = (pixel[0] >= 0 && pixel[0] <= 15) &&
                          pixel[1] >= 40 && pixel[2] >= 20 && pixel[2] <= 80;

           if (isRed1 || isRed2 || isDarkRed) {
               // 加权评分：饱和度 + 亮度
               redScore += pixel[1] * 0.6 + pixel[2] * 0.4;
           }
           count++;
       }

       colorScores[y] = (count > 0) ? redScore / count : 0;
   }

   return colorScores;
}

//添加溶血检测和处理
bool TestOpcv::isHemolyzed(const Mat& tubeImage, int interfaceY) {
    if (interfaceY < 10 || interfaceY >= tubeImage.rows - 10) return false;

    Mat hsv;
    cvtColor(tubeImage, hsv, COLOR_BGR2HSV);

    // 检测血浆区域（分界面上方）和红细胞区域（分界面下方）的颜色分布
    int plasmaStart = max(0, interfaceY - 30);
    int plasmaEnd = interfaceY;
    int rbcStart = interfaceY;
    int rbcEnd = min(tubeImage.rows, interfaceY + 30);

    double plasmaRedRatio = 0;
    double rbcRedRatio = 0;

    // 分析血浆区域
    for (int y = plasmaStart; y < plasmaEnd; y++) {
        for (int x = tubeImage.cols * 0.3; x < tubeImage.cols * 0.7; x++) {
            Vec3b pixel = hsv.at<Vec3b>(y, x);

            // 检测红色成分
            bool isRed = (pixel[0] <= 15 || pixel[0] >= 170) &&
                        pixel[1] >= 30 && pixel[2] >= 30;

            if (isRed) plasmaRedRatio++;
        }
    }

    // 分析红细胞区域
    for (int y = rbcStart; y < rbcEnd; y++) {
        for (int x = tubeImage.cols * 0.3; x < tubeImage.cols * 0.7; x++) {
            Vec3b pixel = hsv.at<Vec3b>(y, x);

            bool isRed = (pixel[0] <= 15 || pixel[0] >= 170) &&
                        pixel[1] >= 30 && pixel[2] >= 30;

            if (isRed) rbcRedRatio++;
        }
    }

    // 计算红色像素比例
    plasmaRedRatio /= ((plasmaEnd - plasmaStart) * (tubeImage.cols * 0.4));
    rbcRedRatio /= ((rbcEnd - rbcStart) * (tubeImage.cols * 0.4));

    // 溶血判断：血浆中有较多红色，且与红细胞区域的颜色差异较小
    bool isHemolyzed = (plasmaRedRatio > 0.1) &&
                      (abs(plasmaRedRatio - rbcRedRatio) < 0.3);

    QLOG_DEBUG() << "溶血检测 - 位置: " << interfaceY
                 << ", 血浆红色比例: " << plasmaRedRatio * 100 << "%"
                 << ", 红细胞红色比例: " << rbcRedRatio * 100 << "%"
                 << ", 判定溶血: " << isHemolyzed;

    return isHemolyzed;
}

//增强暗色红细胞检测
vector<double> TestOpcv::calculateDarkRegionFeatures(const Mat& tubeImage) {
    vector<double> darkScores(tubeImage.rows);
    Mat gray;
    cvtColor(tubeImage, gray, COLOR_BGR2GRAY);

    for (int y = 0; y < tubeImage.rows; y++) {
        double darkness = 0;
        int count = 0;

        for (int x = tubeImage.cols * 0.3; x < tubeImage.cols * 0.7; x++) {
            // 暗色区域得分更高
            darkness += (255 - gray.at<uchar>(y, x)) / 255.0;
            count++;
        }

        darkScores[y] = (count > 0) ? darkness / count : 0;
    }

    return darkScores;
}


// 在现有代码基础上增加暗色环境专用特征
vector<double> TestOpcv::calculateDarkEnvironmentFeatures(const Mat& tubeImage) {
    vector<double> darkScores(tubeImage.rows, 0);
    Mat gray;
    cvtColor(tubeImage, gray, COLOR_BGR2GRAY);

    // 应用中值滤波减少噪声
    Mat filtered;
    medianBlur(gray, filtered, 5);

    // 计算每行的纹理特征（更适合暗色环境）
    for (int y = 1; y < filtered.rows - 1; y++) {
        double textureScore = 0;
        int count = 0;

        // 只分析中间区域，避免边缘干扰
        int startX = filtered.cols * 0.3;
        int endX = filtered.cols * 0.7;

        for (int x = startX; x < endX; x++) {
            // 计算局部纹理变化（更适合暗色区域）
            uchar center = filtered.at<uchar>(y, x);
            uchar above = filtered.at<uchar>(y-1, x);
            uchar below = filtered.at<uchar>(y+1, x);

            // 暗色环境下的纹理特征：关注垂直方向的亮度变化
            textureScore += abs(center - above) + abs(center - below);
            count += 2;
        }

        darkScores[y] = (count > 0) ? textureScore / count : 0;
    }

    return darkScores;
}


// 专门检测分界面附近的微弱颜色变化
vector<double> TestOpcv::calculateInterfaceColorFeatures(const Mat& tubeImage) {
    vector<double> interfaceScores(tubeImage.rows, 0);
    Mat hsv;
    cvtColor(tubeImage, hsv, COLOR_BGR2HSV);

    for (int y = 0; y < tubeImage.rows; y++) {
        double interfaceScore = 0;
        int count = 0;

        for (int x = tubeImage.cols * 0.3; x < tubeImage.cols * 0.7; x++) {
            Vec3b pixel = hsv.at<Vec3b>(y, x);

            // 专门检测分界面处的微弱红色特征
            // 放宽红色检测条件，适应暗色环境
            bool isFaintRed = (pixel[0] <= 15 || pixel[0] >= 170) &&
                            pixel[1] >= 20 &&  // 降低饱和度阈值
                            pixel[2] >= 15 && pixel[2] <= 100; // 关注中等亮度区域

            if (isFaintRed) {
                // 对微弱红色给予更高权重
                interfaceScore += 2.0;
            }

            // 检测亮度变化（分界面处通常有亮度跳变）
            if (y > 0) {
                Vec3b prevPixel = hsv.at<Vec3b>(y-1, x);
                double brightnessChange = abs(pixel[2] - prevPixel[2]);
                interfaceScore += brightnessChange * 0.1;
            }

            count++;
        }

        interfaceScores[y] = (count > 0) ? interfaceScore / count : 0;
    }

    return interfaceScores;
}

bool TestOpcv::isLikelyAirInterface(const Mat& tubeImage, int candidateY) {
    // 只对图像上半部分进行检测
    if (candidateY > tubeImage.rows * 0.4) {
        return false;
    }

    // 1. 定义试管材质的典型颜色范围（塑料试管的常见颜色）
    // 深灰色到黑色范围（BGR空间）
    Scalar tubeDarkMin = Scalar(20, 20, 20);   // 较暗的试管颜色
    Scalar tubeDarkMax = Scalar(80, 80, 80);   // 较亮的试管颜色

    // HSV空间的试管颜色范围（低饱和度，中等亮度）
    Scalar tubeHSVMin = Scalar(0, 0, 20);      // H:任意, S:低, V:中等
    Scalar tubeHSVMax = Scalar(180, 50, 80);   // H:任意, S:较低, V:中等偏暗

    // 2. 检测候选位置上方的区域（试管材质区域）
    int tubeRegionHeight = min(40, candidateY);
    if (tubeRegionHeight < 10) {
        return false;
    }

    Rect tubeRegion(0, 0, tubeImage.cols, candidateY);
    Mat tubeROI = tubeImage(tubeRegion);

    // 3. 检测候选位置下方的区域（液体区域）
    int liquidRegionHeight = 30;
    int liquidStart = candidateY;
    int liquidEnd = min(tubeImage.rows, candidateY + liquidRegionHeight);
    Rect liquidRegion(0, liquidStart, tubeImage.cols, liquidEnd - liquidStart);
    Mat liquidROI = tubeImage(liquidRegion);

    // 4. 计算试管区域的颜色特征
    Scalar tubeMean, tubeStdDev;
    meanStdDev(tubeROI, tubeMean, tubeStdDev);

    // 5. 转换到HSV空间进行颜色分析
    Mat tubeHSV, liquidHSV;
    cvtColor(tubeROI, tubeHSV, COLOR_BGR2HSV);
    cvtColor(liquidROI, liquidHSV, COLOR_BGR2HSV);

    Scalar tubeHSVMean, tubeHSVStdDev;
    Scalar liquidHSVMean, liquidHSVStdDev;
    meanStdDev(tubeHSV, tubeHSVMean, tubeHSVStdDev);
    meanStdDev(liquidHSV, liquidHSVMean, liquidHSVStdDev);

    // 6. 试管材质的特征判断
    bool isTubeColorBGR = (tubeMean[0] >= tubeDarkMin[0] && tubeMean[0] <= tubeDarkMax[0] &&
                          tubeMean[1] >= tubeDarkMin[1] && tubeMean[1] <= tubeDarkMax[1] &&
                          tubeMean[2] >= tubeDarkMin[2] && tubeMean[2] <= tubeDarkMax[2]);

    bool isTubeColorHSV = (tubeHSVMean[1] >= tubeHSVMin[1] && tubeHSVMean[1] <= tubeHSVMax[1] && // 低饱和度
                          tubeHSVMean[2] >= tubeHSVMin[2] && tubeHSVMean[2] <= tubeHSVMax[2]);   // 中等亮度

    bool isTubeUniform = (tubeStdDev[0] < 20 && tubeStdDev[1] < 20 && tubeStdDev[2] < 20); // 颜色均匀

    // 7. 液体区域的特征（应该与试管材质有明显区别）
    bool liquidDifferentFromTube = false;
    double brightnessDiff = abs(liquidHSVMean[2] - tubeHSVMean[2]);
    double saturationDiff = abs(liquidHSVMean[1] - tubeHSVMean[1]);

    if (brightnessDiff > 30 || saturationDiff > 20) {
        liquidDifferentFromTube = true;
    }

    // 8. 使用颜色掩码进一步验证试管材质
    Mat tubeMaskBGR, tubeMaskHSV;
    inRange(tubeROI, tubeDarkMin, tubeDarkMax, tubeMaskBGR);

    Mat tubeROI_HSV;
    cvtColor(tubeROI, tubeROI_HSV, COLOR_BGR2HSV);
    inRange(tubeROI_HSV, tubeHSVMin, tubeHSVMax, tubeMaskHSV);

    double tubeColorRatioBGR = (double)countNonZero(tubeMaskBGR) / (tubeROI.rows * tubeROI.cols);
    double tubeColorRatioHSV = (double)countNonZero(tubeMaskHSV) / (tubeROI.rows * tubeROI.cols);

    bool hasHighTubeColorRatio = (tubeColorRatioBGR > 0.6 || tubeColorRatioHSV > 0.6);

    // 9. 位置权重（越靠近顶部越可能是试管界面）
    double positionWeight = 1.0 - (double)candidateY / (tubeImage.rows * 0.4);
    bool isVeryTop = candidateY < tubeImage.rows * 0.2;

    // 10. 综合评分
    double tubeInterfaceScore = 0;

    if (isTubeColorBGR || isTubeColorHSV) tubeInterfaceScore += 3.0;
    if (isTubeUniform) tubeInterfaceScore += 2.0;
    if (liquidDifferentFromTube) tubeInterfaceScore += 2.0;
    if (hasHighTubeColorRatio) tubeInterfaceScore += 2.0;
    if (isVeryTop) tubeInterfaceScore += 2.0 * positionWeight;

    // 11. 调试信息
    if (tubeInterfaceScore > 5.0) {
        QLOG_DEBUG() << "试管界面检测 - 位置: " << candidateY
                     << ", 总分: " << tubeInterfaceScore
                     << ", 试管颜色(BGR): " << tubeMean[0] << "," << tubeMean[1] << "," << tubeMean[2]
                     << ", 试管颜色(HSV): " << tubeHSVMean[0] << "," << tubeHSVMean[1] << "," << tubeHSVMean[2]
                     << ", 颜色均匀度: " << tubeStdDev[0]
                     << ", 试管颜色比例: " << tubeColorRatioBGR * 100 << "%";
    }

    // 阈值判断：总分超过6分认为是试管材质界面
    return tubeInterfaceScore > 6.0;
}
