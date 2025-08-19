#pragma execution_character_set("utf-8")
#include "testopcv.h"
#include "ui_testopcv.h"
#include <QFileDialog>
#include <QGraphicsScene>
#include <QPushButton>
#include <QsLog/include/QsLog.h>
#include <opencv2/opencv.hpp>
#include <QMessagebox>
#include <globaldata.h>
#include "cglobal.h"
#include "ini_file.h"
 #include <opencv2/dnn.hpp>


using namespace cv;
using namespace dnn;

cv::Mat ReferenceImage;
cv::Mat OriginPrpImage;
cv::Mat hsvImage; //参照物
int lowerH = 61, lowerS = 28, lowerV = 62;
int upperH = 121, upperS = 255, upperV = 215;


cv::Mat hsvImageYellow;

bool detectLabelRegion(const Mat& hsv);


TestOpcv::TestOpcv(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TestOpcv)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("校准测高参数"));
    m_imgratio = 0.00f;
    m_prporiginHeight = 0.0f;
    initsignal();


    QFile stylespinBox(":/Picture/SetPng/wholeSpinBox.qss");
    if(stylespinBox.open(QIODevice::ReadOnly)) {
          QString setspinBoxQss = QLatin1String(stylespinBox.readAll());
          ui->spinBox->setStyleSheet(setspinBoxQss);
          stylespinBox.close();
    } else {
        QLOG_WARN() << "QSS load error:" << stylespinBox.errorString();
    }

    QString settButtonQss;
    QFile styleFile(":/Picture/SetPng/wholePushbutton.qss");
    if(styleFile.open(QIODevice::ReadOnly)) {
      settButtonQss = QLatin1String(styleFile.readAll());
      styleFile.close();
    }
    QHash<QPushButton*, QString> pushButtonList = {
         {ui->pushButton_loadpath,    tr("导入图片")},
         {ui->pushButton_prp, tr("测试找试管")},
         {ui->pushButton_test, tr("测试识别")}
    };

    for (auto it = pushButtonList.cbegin(); it != pushButtonList.cend(); ++it) {
         QPushButton* button = it.key();
         QString text = it.value();
         // 使用button和text进行操作
         button->setStyleSheet(settButtonQss);
         button->setText(text);
    }
}

TestOpcv::~TestOpcv()
{
    delete ui;
}


// 检测标签区域（示例：蓝色标签）
bool detectLabelRegion(const Mat& hsv) {
	Mat label_mask;
	// 定义标签颜色范围（示例为蓝色）
	inRange(hsv, Scalar(90, 70, 50), Scalar(130, 255, 255), label_mask);

	std::vector<std::vector<cv::Point>> contours;
	findContours(label_mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	if (!contours.empty()) {
		return true;
	}
	return false;
}


// 图像预处理（增强对比度）
cv::Mat preprocessPlasmaRegion(Mat& frame) {
    // CLAHE光照均衡化（Lab空间）
    Mat lab, processed;
    cvtColor(frame, lab, COLOR_BGR2Lab);
    std::vector<Mat> channels;
    split(lab, channels);

    Ptr<CLAHE> clahe = createCLAHE();
    clahe->setClipLimit(3);
    clahe->apply(channels[0], channels[0]);

    merge(channels, lab);
    cvtColor(lab, processed, COLOR_Lab2BGR);
    return processed;
}







/** 显示图片**/
void displayImage(cv::Mat cvImage,QLabel *pshow)
{
    pshow->clear();
    // 将 BGR 转换为 RGB
    cvtColor(cvImage, cvImage, cv::COLOR_BGR2RGB);
    // 将 cv::Mat 转换为 QImage
    QImage qImage(cvImage.data, static_cast<int>(cvImage.cols), static_cast<int>(cvImage.rows), static_cast<int>(cvImage.step), QImage::Format_RGB888);

    // 将 QImage 转换为 QPixmap 并显示在 QLabel 中
    QPixmap pixmap = QPixmap::fromImage(qImage);
    QLabel *label = pshow;
    label->setPixmap(pixmap);
    label->show();
}

bool TestOpcv::hasTextTraditional(cv::Mat img)
{
	// 1. 灰度化
	cv::Mat gray, binary, processed;
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

	// 2. 二值化
	cv::threshold(gray, binary, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

	// 3. 形态学操作（膨胀+腐蚀）
	cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::dilate(binary, processed, kernel);
	cv::erode(processed, processed, kernel);

	// 4. 检测轮廓
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(processed, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// 5. 筛选候选区域
	for (auto& contour : contours) {
		cv::Rect rect = cv::boundingRect(contour);
		if (rect.area() > 100 && rect.width > 10 && rect.height > 10) {
			cv::rectangle(img, rect, Scalar(0, 255, 0), 2);
			imshow("11", img);
			waitKey(0);
			return true; // 存在文字区域
		}
	}
	return false;
}

bool TestOpcv::Extractthecolorblockontheleft(cv::Mat img,double &imgraito)
{
    int width = img.cols;
    int height = img.rows;

    //1 先截取图片
    cv::Rect rect(0, 0, width/3 - 5, height);
    cv::Mat image = img(rect);

    // 2. 转换到HSV颜色空间
    cv::Mat hsv;
    cv::cvtColor(image, hsv, COLOR_BGR2HSV);

    // 3. 定义蓝色色范围（示例值，需根据实际调整）
    cv::Scalar lower_green(64, 55, 62);
    cv::Scalar upper_green(132, 255, 213);

    // 4. 创建二值掩膜
    cv::Mat mask;
    cv::inRange(hsv, lower_green, upper_green, mask);

    // 5. 形态学处理（可选）
    cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, Size(5,5));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // 6. 查找轮廓
    std::vector<std::vector<Point>> contours;
    cv::findContours(mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 7. 筛选最大轮廓
    if (!contours.empty()) {
        // 按面积降序排序
        std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& c1, const std::vector<cv::Point>& c2){
            return contourArea(c1) > contourArea(c2);
        });

        //获取旋转矩形（更精确）
        cv::RotatedRect minRect = cv::minAreaRect(contours[0]);
        cv::Size2f rect_size = minRect.size;
        float rect_height = cv::max(rect_size.width, rect_size.height);

        // 输出高度
        QLOG_DEBUG() << "最小外接矩形高度: " << rect_height << " 像素" << endl;
        imgraito = (REFERENCE_HEIGHT*100.00 / rect_height)/100.00;
        m_imgratio = imgraito;
        QString dataratio = QString::number(imgraito,'f',4);
        ui->label_ratio->setText(dataratio + "mm/dpi");

        // 可视化绘制
        //rectangle(image, bbox, Scalar(0,255,0), 2);
        cv::Point2f vertices[4];
        minRect.points(vertices);
        for (int j = 0; j < 4 ; j++) {
			cv::line(img, vertices[j], vertices[(j+1)%4], Scalar(0,255,255), 2);
        }
       
        return true;
    }
    else {
        QLOG_DEBUG() << "未检测到色块" << endl;
        return false;
    }
    return true;
}



void TestOpcv::initsignal()
{
    connect(ui->pushButton_loadpath,&QPushButton::clicked,this,[=]()
    {
        QString OpenFile = QFileDialog::getOpenFileName(this,tr("请选取图片文件"),"",
                                "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)");
        if(!OpenFile.isEmpty())
        {
            ui->spinBox->setValue(0);
            OpenFile.replace("/", "\\");
            std::string cstrPath = std::string((const char *)OpenFile.toLocal8Bit());
            Mat img = imread(cstrPath);
            imageOrinin = img.clone(); //原图
            displayImage(img,ui->label_showimage);
        }


    });


    connect(ui->pushButton_prp,&QPushButton::clicked, this, [=]() {
        double bloodHeight = 0.00f;
		IdentifyRBCHeight(imageOrinin,m_imgratio,bloodHeight);
    });

   connect(ui->pushButton_test,&QPushButton::clicked, this, [=]() { trayfindImg(); });
}

void TestOpcv::on_spinBox_valueChanged(int arg1)
{
    wheeltoImage(arg1);
}
bool TestOpcv::matImagewheel(cv::Mat cutimage, int degree ,cv::Mat &OutMat)
{
    if (cutimage.cols == 0 || cutimage.rows == 0)
    {
        QMessageBox::about(this,"导入异常","图片路径或有中文!");
        return false;
    }
    cv::Mat dstimg;
    cv::Mat rotimg;
    int row, col;
    cv::Point2f srcTri[3];
    cv::Point2f dstTri[3];
    cv::Mat warpMat(2, 3, CV_32FC1);
    cv::Mat srcimg = cutimage;
    if (!srcimg.data)
    {
        QMessageBox::about(this,"旋转失败","读取错误!");
        return false;
    }
    row = srcimg.rows;
    col = srcimg.cols;

    srcTri[0] = cv::Point2f(0, 0);
    srcTri[1] = cv::Point2f(col - 1, 0);
    srcTri[2] = cv::Point2f(0, row - 1);

    dstTri[0] = cv::Point2f(col*0.0, row*0.33);
    dstTri[1] = cv::Point2f(col*0.85, row*0.25);
    dstTri[2] = cv::Point2f(col*0.15, row*0.7);

    warpMat = cv::getAffineTransform(srcTri, dstTri);

    cv::warpAffine(srcimg, dstimg, warpMat, dstimg.size());

    cv::Point2f center(col / 2, row / 2);
    double angle = degree*CV_PI / 180; //转换弧度
    double scale = 1;
    double a = sin(angle);
    double b = cos(angle);
    int width = col;
    int height = row;

    int rewidth = int(height*fabs(a) + width*fabs(b));
    int reheight = int(width*fabs(a) + height*fabs(b));
    cv::Mat rot = cv::getRotationMatrix2D(center, degree, scale);
    double *p = (double*)rot.data;
    p[2] += (rewidth - width) / 2;
    p[5] += (reheight - height) / 2;
    cv::warpAffine(srcimg, rotimg, rot, cv::Size(rewidth, reheight), 0, 0, 0);
    OutMat = rotimg;
    return true;
}

void TestOpcv::wheeltoImage(int wheel)
{
   matImagewheel(ReferenceImage,wheel,ReferenceImage);
   // label 显示图像
   ui->label_showimage->clear();
   QImage disImage = QImage((const unsigned char*)(ReferenceImage.data), ReferenceImage.cols, ReferenceImage.rows,
                            QImage::Format_RGB888);
   ui->label_showimage->setPixmap(QPixmap::fromImage(disImage.scaled(ui->label_showimage->size(), Qt::KeepAspectRatio)));
   return;
}

bool TestOpcv::BuffyCoatTopDpi(cv::Mat origimImag,cv::Mat tubeImage)
{
    // --- 步骤1: 图像预处理 ---
    cv::Mat processed = preprocessPlasmaRegion(tubeImage);

    // 转换到YCrCb颜色空间（更适合血浆检测）
    Mat ycrcb;
    cvtColor(processed, ycrcb, COLOR_BGR2YCrCb);

    // 定义血浆颜色范围（根据样本调整）
    Mat plasma_mask;
    inRange(ycrcb, Scalar(150, 85, 85), Scalar(255, 135, 135), plasma_mask);

    // 形态学优化
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7,7));
    morphologyEx(plasma_mask, plasma_mask, MORPH_CLOSE, kernel);

    // --- 步骤4: 确定血浆底部边界 ---
    std::vector<std::vector<cv::Point>> contours;
    findContours(plasma_mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    if (contours.empty()) {
        QLOG_ERROR() << "未检测到血浆区域!" << endl;
        return false;
    }

    // 找最大连通域
    int max_area_idx = -1;
    double max_area = 0;
    for (size_t i = 0; i < contours.size(); i++) {
        double area = contourArea(contours[i]);
        if (area > max_area) {
            max_area = area;
            max_area_idx = static_cast<int> (i);
        }
    }

    // 获取分界线的Y坐标（血浆底部）
    Rect plasma_rect = boundingRect(contours[max_area_idx]);
    int plasma_bottom_y = plasma_rect.y + plasma_rect.height;

    // --- 结果计算 ---
    int plasma_height = plasma_bottom_y - 0;
    QLOG_DEBUG() << "血浆高度像素值: " << plasma_height << endl;

    // 可视化标注
    line(tubeImage, Point(0, plasma_rect.y), Point(tubeImage.cols, plasma_rect.y), Scalar(255,0,0), 2);
    rectangle(tubeImage, plasma_rect, Scalar(0,200,200), 2);
   

    imshow("Result", tubeImage);

	return true;
}

//测试识别血样管
void TestOpcv::IdentifyRBCHeight(cv::Mat img,double ratioimg,double &BloodHeigh)
{
    cv::Mat testimg = img.clone();
    cv::Mat tube_region;
    Rect tuberoi;
    IdentifyTheTubes(testimg,tube_region,tuberoi);

    cv::Mat tubeimage = tube_region.clone();
    BuffyCoatTopDpi(img,tubeimage);

    cv::Mat hsv, mask;
    cv::cvtColor(tube_region, hsv, COLOR_BGR2HSV);

    // 红色检测（保持原有逻辑）
    cv::Scalar lower_red1(0, 70, 50), upper_red1(10, 255, 255);
    cv::Scalar lower_red2(160, 70, 50), upper_red2(180, 255, 255);
    cv::Mat mask1, mask2;
    cv::inRange(hsv, lower_red1, upper_red1, mask1);
    cv::inRange(hsv, lower_red2, upper_red2, mask2);
    cv::bitwise_or(mask1, mask2, mask);

    // 新增黑色检测
    cv::Scalar lower_black(0, 0, 0), upper_black(180, 255, 50);
    cv::Mat mask_black;
    cv::inRange(hsv, lower_black, upper_black, mask_black);
    cv::Mat kernel_black = getStructuringElement(MORPH_ELLIPSE, Size(5,5));
    cv::morphologyEx(mask_black, mask_black, MORPH_CLOSE, kernel_black);

    // 合并掩膜
    cv::Mat combined_mask;
    cv::bitwise_or(mask, mask_black, combined_mask);
    cv::Mat kernel_red = getStructuringElement(MORPH_ELLIPSE, Size(15,15));
    cv::morphologyEx(combined_mask, combined_mask, MORPH_CLOSE, kernel_red);

    // 垂直投影分析
    cv::Mat ver_proj;
    cv::reduce(combined_mask, ver_proj, 1, REDUCE_SUM, CV_32F);

    // 动态阈值计算
    float max_val = 0, min_val = FLT_MAX;
    for(int i= 0; i<ver_proj.rows; i++) {
        float val = ver_proj.at<float>(i);
        if(val > max_val) max_val = val;
        if(val < min_val) min_val = val;
    }

    // 模式自适应
    bool is_black = (countNonZero(mask_black) > countNonZero(mask));
    float th_rise = is_black ? max_val*0.1 : max_val*0.15;
    float th_fall = is_black ? max_val*0.05 : max_val*0.08;

    // 边界检测
    int top = 0, bottom = 0;
    bool found_top = false;
    for(int i=10; i<ver_proj.rows-10; i++) {
        float current = ver_proj.at<float>(i);
        if(!found_top && current > th_rise) {
            top = i;
            found_top = true;
        }
        if(found_top && current < th_fall) {
            bottom = i;
            break;
        }
    }

    // 可视化（新增黑色标识）
    cv::Scalar color = is_black ? Scalar(255,255,255) : Scalar(0,255,0);
    cv::line(img, Point(tuberoi.x,top), Point(tuberoi.width+tuberoi.x,top), color, 2);
    cv::line(img, Point(tuberoi.x,bottom), Point(tuberoi.width + tuberoi.x,bottom), cv::Scalar(0,0,240), 2);

    
    BloodHeigh = ratioimg * (std::abs(bottom - top));
    QString outdata = "RBC Height: " + QString::number(BloodHeigh,'f',2)+"mm";
    cv::putText(img, outdata.toStdString(), cv::Point(20, 20),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(250, 250, 250), 1);

    double  pinDownSuckHeigh = INI_File().GetFixedHigh() + (REFERENCE_TO_BOTTOM - BloodHeigh - INI_File().GetTestDifference());
    outdata = "Height of the needle drop: " + QString::number(pinDownSuckHeigh,'f',2)+"mm";
    cv::putText(img, outdata.toStdString(), cv::Point(20, 50),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 250, 250), 1);

    cv::rectangle(img, tuberoi, cv::Scalar(250,0,0), 2);
    return;
}


void TestOpcv::IdentifyTheTubes(cv::Mat img,cv::Mat &outTuberoi,Rect &roiTube)
{
    int width = img.cols;
    int height = img.rows;

    //1 先截取图片右边2/3进行试管识别
    cv::Rect rect(width/3, 0, width/3*2, height);
    cv::Mat halfwayimg = img(rect);

    // 转换为灰度图并降噪
    cv::Mat gray;
    cv::cvtColor(halfwayimg, gray, COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, Size(9, 9), 1);
    cv::equalizeHist(gray, gray);

    // 2. 定位试管区域
    cv::Mat edges;
    cv::Canny(gray, edges, 30, 100);

    // 霍夫变换检测垂直线
    std::vector<Vec4i> lines;
    cv::HoughLinesP(edges, lines, 1, CV_PI/2, 50, 50, 10);


    // 筛选最左最右垂直线
    int left = img.cols, right = 0;
    for (auto line : lines) {
        if (abs(line[0] - line[2]) < 5) { // 近似垂直线
            left = min(left, line[0]);
            right = max(right, line[0]);
        }
    }

    // 提取试管ROI（扩展5像素容差）
    Rect roi(left- 5 + width/3 , 0, right-left + 10, height);
    outTuberoi = img(roi).clone();
    roiTube = roi;
    return;
}



void TestOpcv::trayfindImg()
{
    double imgratio = 0.00f,Bloodsampleheight = 0.00f;
    bool references = Extractthecolorblockontheleft(imageOrinin,imgratio);
    if(references)
    {
       IdentifyRBCHeight(imageOrinin,imgratio,Bloodsampleheight);
       displayImage(imageOrinin,ui->label_showimage);
    }else
        return ;
    return ;
}





