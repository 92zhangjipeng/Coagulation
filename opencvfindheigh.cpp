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

const QString opencvfindHeigh::DEFAULT_IMAGE_SUBDIR = "FindHighImage";
opencvfindHeigh::opencvfindHeigh(QObject *parent) : QObject(parent)
  ,m_isreptestheigh(false)
{
    createImageStorageDirectory();

}
opencvfindHeigh::~opencvfindHeigh()
{
    QLOG_DEBUG()<<"退出测高opencv实例";
}


/* 判断打开摄像头内、外部*/
void  opencvfindHeigh::RecvOpenInstrumentCamera(const quint8 CameraPositioning)
{
    switch(CameraPositioning)
    {
        case OPEN_DEFAULT:
                m_OpenCameraPosition = OPEN_DEFAULT;
                QLOG_DEBUG()<<"仪器电脑只有一个摄像头在线==打开默认摄像头";
        break;
        case OPEN_EXTERNAL:
                m_OpenCameraPosition = OPEN_EXTERNAL;
                QLOG_DEBUG()<<"仪器电脑大于一个摄像头在线==打开外置摄像头";
        break;
    default:
         m_OpenCameraPosition = OPEN_DEFAULT;
        break;
    }
    return;
}

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
        QLOG_ERROR()
            << "创建测高图片文件夹失败，路径: "
            << QDir::toNativeSeparators(m_imagePath)
            << "，错误:";
        m_imagePath.clear(); // 失败时清空路径
        return false;
    }

    QLOG_INFO() << "测高图片文件夹创建成功，路径:"
               << QDir::toNativeSeparators(m_imagePath);
    return true;
}


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



void opencvfindHeigh::Start()
{
    QLOG_DEBUG() <<"启动测高线程:"<< QThread::currentThreadId()<<endl;
    return;
}


bool opencvfindHeigh::Extractthecolorblockontheleft(cv::Mat img,double &imgraito)
{
    int width = img.cols;
    int height = img.rows;

    //1 先截取图片
    cv::Rect rect(0, 0, width/3 - 5, height);
    cv::Mat image = img(rect);

    // 2. 转换到HSV颜色空间
    cv::Mat hsv;
    cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);

    // 3. 定义蓝色色范围（示例值，需根据实际调整）
    cv::Scalar lower_green(64, 55, 62);
    cv::Scalar upper_green(132, 255, 213);

    // 4. 创建二值掩膜
    cv::Mat mask;
    cv::inRange(hsv, lower_green, upper_green, mask);

    // 5. 形态学处理（可选）
    cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
    cv::morphologyEx(mask, mask, cv::MORPH_CLOSE, kernel);

    // 6. 查找轮廓
    std::vector<std::vector<cv::Point>> contours;
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

        // 可视化绘制
        //rectangle(image, bbox, Scalar(0,255,0), 2);
        cv::Point2f vertices[4];
        minRect.points(vertices);
        for (int j = 0; j < 4 ; j++) {
            cv::line(img, vertices[j], vertices[(j+1)%4], cv::Scalar(0,255,255), 2);
        }

        return true;
    }
    else {
        QLOG_DEBUG() << "识别参照物失败!" << endl;
        return false;
    }
    return true;

}

bool checkExcessiveBlack(const cv::Mat& roi_mask, const cv::Mat& black_pixels_mask, double threshold = 0.5) {
    // 输入校验
    if (roi_mask.empty() || black_pixels_mask.empty() || roi_mask.size() != black_pixels_mask.size()) {
        return false;
    }

    // 计算ROI总像素和黑色像素数
    int total_pixels = cv::countNonZero(roi_mask);
    int black_pixels = cv::countNonZero(black_pixels_mask);

    // 避免除零
    if (total_pixels == 0) {
        return false;
    }

    // 计算比例
    double ratio = static_cast<double>(black_pixels) / total_pixels;
    return (ratio > threshold);
}
//测试识别血样管
bool opencvfindHeigh::IdentifyRBCHeight(cv::Mat img,double ratioimg,double &BloodHeigh)
{
    cv::Mat testimg = img.clone();
    cv::Mat tube_region;
    cv::Rect tuberoi;
    IdentifyTheTubes(testimg,tube_region,tuberoi);

    if(tube_region.empty()){
        emit FindFailed("试管试杯异常","ROI检测失败！返回空矩阵.");
        return false;
    }

    cv::Mat hsv, mask;
    cv::cvtColor(tube_region, hsv, cv::COLOR_BGR2HSV);

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
    cv::Mat kernel_black = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5,5));
    cv::morphologyEx(mask_black, mask_black, cv::MORPH_CLOSE, kernel_black);

    // 合并掩膜
    cv::Mat combined_mask;
    cv::bitwise_or(mask, mask_black, combined_mask);
    cv::Mat kernel_red = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(15,15));
    cv::morphologyEx(combined_mask, combined_mask, cv::MORPH_CLOSE, kernel_red);

    // 垂直投影分析
    cv::Mat ver_proj;
    cv::reduce(combined_mask, ver_proj, 1, cv::REDUCE_SUM, CV_32F);

    // 动态阈值计算
    float max_val = 0, min_val = FLT_MAX;
    for(int i=0; i<ver_proj.rows; i++) {
        float val = ver_proj.at<float>(i);
        if(val > max_val) max_val = val;
        if(val < min_val) min_val = val;
    }

    // 模式自适应
    bool is_black = (countNonZero(mask_black) > countNonZero(mask));
    //bool is_black =  checkExcessiveBlack(mask_black, mask, 0.5);
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
    cv::Scalar color = is_black ? cv::Scalar(255,255,255) : cv::Scalar(0,255,0);
    cv::line(img, cv::Point(tuberoi.x,top), cv::Point(tuberoi.width+tuberoi.x,top), color, 2);
    cv::line(img, cv::Point(tuberoi.x,bottom), cv::Point(tuberoi.width + tuberoi.x,bottom), color, 2);

    //putText(img, is_black ? "Black Blood" : "Red Blood",Point(30,30), FONT_HERSHEY_SIMPLEX, 1, color, 2);

    BloodHeigh = ratioimg * (std::abs(bottom - top));
    QString outdata = "RBC Height: " + QString::number(BloodHeigh,'f',2)+"mm";
    cv::putText(img, outdata.toStdString(), cv::Point(20, 20),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 200, 0), 1);

    auto &ini = INI_File();
    double  fixedHeigh = ini.GetFixedHigh() + REFERENCE_TO_BOTTOM;
    double  pinDownSuckHeigh = fixedHeigh - (BloodHeigh + ini.GetTestDifference());
    QLOG_DEBUG()<<"总高度:"<< fixedHeigh <<"-(测出血样高度+"<<"偏移高度)";
    QLOG_DEBUG()<<"测出血样高度"<<BloodHeigh;
    QLOG_DEBUG()<<"偏移高度"<<ini.GetTestDifference();
    QLOG_DEBUG()<<"输出高度"<<pinDownSuckHeigh;


    outdata = "Needle Drop: " + QString::number(pinDownSuckHeigh,'f',2)+"mm";
    cv::putText(img, outdata.toStdString(), cv::Point(20, 50),
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 200, 0), 1);

    cv::rectangle(img, tuberoi, cv::Scalar(250,0,0), 2);
    return true;
}

void opencvfindHeigh::IdentifyTheTubes(cv::Mat img,cv::Mat &outTuberoi,cv::Rect &roiTube)
{
    int width = img.cols;
    int height = img.rows;

    //1 先截取图片右边2/3进行试管识别
    cv::Rect rect(width/3, 0, (width*2 )/3, height);
    cv::Mat halfwayimg = img(rect);

    // 转换为灰度图并降噪
    cv::Mat gray, edges;
    cv::cvtColor(halfwayimg, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, gray, cv::Size(9, 9), 1);
    cv::equalizeHist(gray, gray);

    // 2. 定位试管区域
    cv::Canny(gray, edges, 30, 100);
    // 霍夫变换检测垂直线
    std::vector<cv::Vec4i> lines;
    cv::HoughLinesP(edges, lines, 1, CV_PI/2, 50, 50, 10);


    // 筛选最左最右垂直线
    int left = halfwayimg.cols;
    int right = 0;
    for (auto line : lines) {
        if (abs(line[0] - line[2]) < 5) { // 近似垂直线
            //left = min(left, line[0]);
            //right = max(right, line[0]);
            left = std::min(left, std::min(line[0], line[2]));
            right = std::max(right, std::max(line[0], line[2]));
        }
    }

    // 3. 验证检测结果有效性 ---------------------------
    // 强制限定left/right在截取区域范围内
    left =  std::max(0, left);
    right = std::min(halfwayimg.cols - 1, right);

    // 检查是否找到有效区域
    if (left >= right || lines.empty()) {
        // 若未找到，返回整个截取区域（或自定义错误处理）
        roiTube = rect;
        outTuberoi = img(roiTube).clone();
        return;
    }

    // 4. 计算原图坐标系下的ROI -------------------------
    // 转换为原图坐标（原图x起点是width/3）
    int x_in_original = width / 3 + left - 5;  // 左侧扩展5像素
    int w_in_original = right - left + 10;    // 宽度容差

    // 强制ROI不超出原图范围
    x_in_original = std::max(0, x_in_original);
    w_in_original = std::min(img.cols - x_in_original, w_in_original);

    // 最终ROI
    cv::Rect roi(x_in_original, 0, w_in_original, height);

    // 5. 输出结果
    if (roi.width > 0 && roi.height > 0) {
        outTuberoi = img(roi).clone();
        roiTube = roi;
    }
    else {
        // 异常情况返回空矩阵或原图
        outTuberoi = cv::Mat();
        roiTube = cv::Rect();
    }
    return;
}

bool opencvfindHeigh::opencvIdentifyPrp(cv::Mat& inputImage, double& outBottomBloodHeight, QString &outErr)
{
    // 输入校验
    if (inputImage.empty() || inputImage.channels() != 3) {
         QLOG_ERROR()  << "Invalid input: Empty image or incorrect color channels (Expected BGR).";
        return false;
    }

    // 初始化输出参数
    outBottomBloodHeight = 0.0;
    double imageRatio = 0.0;
    double bloodSampleHeight = 0.0;

    // Step 1: 提取左侧颜色块
    const bool isColorBlockValid = Extractthecolorblockontheleft(inputImage, imageRatio);
    if (!isColorBlockValid) {
        QLOG_ERROR() << "Failed to extract color reference block.";
        outErr = "提取参照物失败";
        return false;
    }

    // Step 2: 计算红细胞高度
    const bool isRbcHeightValid = IdentifyRBCHeight(inputImage, imageRatio, bloodSampleHeight);
    if (!isRbcHeightValid) {
		QLOG_ERROR() << "Failed to detect RBC height.";
        outErr = "计算红细胞高度失败";
        return false;
    }

    // 输出最终结果
    outBottomBloodHeight = bloodSampleHeight;
    return true;
}

void opencvfindHeigh::handleRepPrpheight(const QString &numid,bool replflag){
    m_replaceid = numid;
    m_isreptestheigh = replflag;
}

/*开始解析图片*/
void opencvfindHeigh::handleTriggerTestHeight()
{
    const int MAX_CAMERA_RETRY = 3;
    //QString titiestr, reminderstr;
    bool isOpened = false;
    cv::VideoCapture capture;

    for (int retry = 0; retry < MAX_CAMERA_RETRY && !isOpened; ++retry) {
        capture.open(m_OpenCameraPosition, cv::CAP_DSHOW);
        if (capture.isOpened()) {
            // 验证分辨率设置
            double actualWidth = capture.get(cv::CAP_PROP_FRAME_WIDTH);
            double actualHeight = capture.get(cv::CAP_PROP_FRAME_HEIGHT);
            if (actualWidth != Image_Height || actualHeight != Image_Width) {
                QLOG_WARN() << "分辨率设置失败，实际分辨率："
                            << actualWidth << "x" << actualHeight;
            }
            isOpened = true;
        } else {
            QLOG_DEBUG() << "摄像头打开失败，重试次数：" << retry;
            QThread::msleep(100);
        }
    }

    if (!isOpened) {
        QLOG_WARN() << "摄像头最终打开失败，索引:" << m_OpenCameraPosition;
        emit FindFailed(tr("硬件错误"), tr("摄像头初始化失败 (设备号:%1)").arg(m_OpenCameraPosition));
        return;
    }

    // 捕获帧（允许重试）
    cv::Mat frame;
    int maxRetry = 5;
    while (maxRetry-- > 0 && !capture.read(frame)) {
       QLOG_DEBUG() << "帧捕获失败，剩余重试次数:" << maxRetry;
       cv::waitKey(1); // 短暂等待
    }

    if (frame.empty()) {
       QLOG_WARN() << "无法获取有效帧";
       emit FindFailed(tr("采集错误"), tr("摄像头画面获取超时"));
       return;
    }
    if (capture.isOpened()){
        capture.release();
    }

    // 图像旋转处理
    cv::Mat rotatedFrame;
    try {
        pinwheel(frame, 90, rotatedFrame); // 旋转-90度
        if (rotatedFrame.empty()) {
            QLOG_ERROR()<<"旋转后图像为空";
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

    // 执行核心识别逻辑
    double height = 0.0;
    QString outErr;
    bool success = opencvIdentifyPrp(rotatedFrame, height,outErr);

    //蜂鸣器响1|3声
    emit Testheightfinish(success);


    // 保存结果图像
    const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd");
    const QString savePath = QString("%1/PRP_%2.jpg").arg(m_imagePath).arg(timestamp);
    string outpath = savePath.toLocal8Bit().toStdString();

    if(!cv::imwrite(outpath, rotatedFrame)) {
        QLOG_WARN() << "图像保存失败:" << savePath;
        outErr = outErr + "图像保存失败:"+ savePath;
    }

    if(!success)
    {
        QLOG_ERROR() << outErr;
        emit FindFailed(tr("识别异常"), outErr);
        return;
    }else{
        emit FinishTestHigh(savePath, height,m_isreptestheigh,m_replaceid);
        m_isreptestheigh = false;
        m_replaceid = "";
    }
    return;
}
