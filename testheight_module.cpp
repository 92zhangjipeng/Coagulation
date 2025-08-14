#pragma execution_character_set("utf-8")

#include "testheight_module.h"
#include <QString>
#include <QCoreApplication>
#include <QDir>
#include <QThread>
#include "cglobal.h"

bool TestHeight_Module::takePhotoFlag = false;
bool TestHeight_Module::closeFlag = false;
QString TestHeight_Module::SampleName = "2022/05/30-1";


void AlreadyFilebox(QString path)
{
	QDir dir(path);
	if (!dir.exists()) {
		bool ismkdir = QDir().mkdir(path);
		if (!ismkdir)
            qDebug() << "Create path fail" << endl<<__FILE__<<__LINE__;
		else
            qDebug() << "Create fullpath success" << endl<<__FILE__<<__LINE__;
	}
	else {
        qDebug() << "fullpath exist" << endl<<__FILE__<<__LINE__;
	}
}
TestHeight_Module::TestHeight_Module(QObject *parent) : QObject(parent)
{
     folerPath = QCoreApplication::applicationDirPath() + "/FindHighImage";
     // 判断文件夹是否存在，不存在则创建
	 AlreadyFilebox(folerPath);

}
TestHeight_Module::~TestHeight_Module()
{
	//delete this;
}
int fsbh(cv::Mat cutimage, int degree ,cv::Mat &OutMat)
{
	cv::Mat dstimg;
	cv::Mat rotimg;
	int row, col;
	cv::Point2f srcTri[3];
	cv::Point2f dstTri[3];
	cv::Mat warpMat(2, 3, CV_32FC1);
	cv::Mat srcimg = cutimage;
	if (!srcimg.data) {
		qDebug() << "读取错误" << endl;
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

	cv::imshow("旋转后", rotimg);
	OutMat = rotimg;
	return true;
}

void  TestHeight_Module::SlotTakePhoto(QString SampleIndex)
{
    TestHeight_Module::takePhotoFlag = true;
    TestHeight_Module::SampleName = SampleIndex;
}
void  TestHeight_Module::SlotCloseCapture()
{
	TestHeight_Module::closeFlag = true;
	return;
}
void  TestHeight_Module::SlotOpenCapture()
{
    cv::Mat readImage; //打开图片
    mcap.open(1);  //电脑摄像头默认是 0
    //设置采集图像大小；具体大小根据相机设置
    mcap.set(cv::CAP_PROP_FRAME_HEIGHT,640);
    mcap.set(cv::CAP_PROP_FRAME_WIDTH,480);
    //判断相机是否打开成功
    if(!mcap.isOpened())
        return ;

    cv::Mat rotatingimage;
    double Imagescale = 0;
	std::string savePath;
    while(true)
    {
         mcap >> readImage ;
         cv::imshow("outImage",readImage);
         QThread::sleep(1);
        // TestHeight_Module::takePhotoFlag = false;
       savePath = QString("%1/%2.jpg").arg(folerPath).arg("temp").toStdString();

        fsbh(readImage,-90, rotatingimage);
        cv::imwrite(savePath, rotatingimage);

        mcap.release();
        cv::waitKey(30);
        break;
    }
    ImageLoad(QString::fromStdString(savePath));
    /*FindBluebenchmark(rotatingimage, Imagescale); //找到蓝色基准块，把比例算出来
         if (TestHeight_Module::closeFlag)
         {
             mcap.release();
             break;
         }
         cv::waitKey(30);
    }*/
	
	return;
}
//找基准物
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}
void TestHeight_Module::ReferencesFind(const cv::Mat& image, cv::Mat &out)
{
	int thresh = 50, N = 5;
	std::vector<std::vector<cv::Point> > squares;
	squares.clear();

	cv::Mat src, dst, gray_one, gray;

	src = image.clone();
	out = image.clone();
	gray_one = cv::Mat(src.size(), CV_8U);
	//滤波增强边缘检测
	medianBlur(src, dst, 9);
	//bilateralFilter(src, dst, 25, 25 * 2, 35);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;

	//在图像的每个颜色通道中查找矩形
	for (int c = 0; c < image.channels(); c++)
	{
		int ch[] = { c, 0 };
		//通道分离
		mixChannels(&dst, 1, &gray_one, 1, ch, 1);
		// 尝试几个阈值
		for (int l = 0; l < N; l++)
		{
			// 用canny()提取边缘
			if (l == 0)
			{
				//检测边缘
				Canny(gray_one, gray, 5, thresh, 5);
				//膨脹
				dilate(gray, gray, cv::Mat(), cv::Point(-1, -1));
				imshow("dilate", gray);
			}
			else
			{
				gray = gray_one >= (l + 1) * 255 / N;
			}

			// 轮廓查找
			//findContours(gray, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE);
			findContours(gray, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE);

			std::vector<cv::Point> approx;

			// 检测所找到的轮廓
			for (size_t i = 0; i < contours.size(); i++)
			{
				//使用图像轮廓点进行多边形拟合
				approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

				//计算轮廓面积后，得到矩形4个顶点
				if (approx.size() == 4 && fabs(cv::contourArea(cv::Mat(approx))) > 1000 && cv::isContourConvex(cv::Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						// 求轮廓边缘之间角度的最大余弦
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					if (maxCosine < 0.3)
					{
						squares.push_back(approx);
					}
				}
			}
		}
	}

	for (size_t i = 0; i < squares.size(); i++)
	{
		const cv::Point* p = &squares[i][0];

		int n = (int)squares[i].size();
		if (p->x > 3 && p->y > 3)
		{
			polylines(out, &p, &n, 1, true, cv::Scalar(0, 255, 0), 3, cv::LINE_AA);
		}
	}
	imshow("dst", out);
}


void TestHeight_Module::FindBluebenchmark(cv::Mat Image, double &Imagescale)
{
	cv::Mat srcImage_save = Image.clone();
	cv::Mat temp_image;
	//进行高斯模糊
	cv::GaussianBlur(Image, temp_image, cv::Size(5, 5), 0, 0);
	cv::GaussianBlur(temp_image, temp_image, cv::Size(5, 5),0, 0);
	cv::Mat mat_knowlength ;
	cv::cvtColor(temp_image, mat_knowlength, cv::COLOR_BGR2HSV);
	cv::imshow("111222", mat_knowlength);
	int H, S, V;
	int rows_image = mat_knowlength.rows;
	int cols_image = mat_knowlength.cols;
	for (int j = 0; j<rows_image; j++) {
		for (int i = 0; i<cols_image; i++) {
			H = mat_knowlength.at<cv::Vec3b>(j, i)[0];
			S = mat_knowlength.at<cv::Vec3b>(j, i)[1];
			V = mat_knowlength.at<cv::Vec3b>(j, i)[2];
			if ((H >= 90) && (H <= 145) && (S >= 43) && (S <= 245) && (V >= 46) && (V <= 255))
			{
				mat_knowlength.at<cv::Vec3b>(j, i)[0] = 255;
				mat_knowlength.at<cv::Vec3b>(j, i)[1] = 255;
				mat_knowlength.at<cv::Vec3b>(j, i)[2] = 255;
			}
			else
			{
				mat_knowlength.at<cv::Vec3b>(j, i)[0] = 0;
				mat_knowlength.at<cv::Vec3b>(j, i)[1] = 0;
				mat_knowlength.at<cv::Vec3b>(j, i)[2] = 0;
			}
		}
	}
	cv::namedWindow("2值图");
	cv::imshow("2值图", mat_knowlength);

	cv::Scalar ScalarL = cv::Scalar(255, 255, 255);
	cv::Scalar ScalarH = cv::Scalar(255, 255, 255);
	std::vector<cv::Mat> hsvSplit;
	cv::split(mat_knowlength, hsvSplit);
	cv::equalizeHist(hsvSplit[2], hsvSplit[2]);
	cv::merge(hsvSplit, mat_knowlength);

	cv::Mat outputImg;
	cv::inRange(mat_knowlength, ScalarL, ScalarH, outputImg);
	//开操作 (去除一些噪点)
	cv::Mat element = getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
	cv::morphologyEx(outputImg, outputImg, cv::MORPH_OPEN, element);
	//闭操作 (连接一些连通域)
	cv::morphologyEx(outputImg, outputImg, cv::MORPH_CLOSE, element);
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(outputImg, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    qDebug() << "参照物个数:" << contours.size()<<__FILE__<<__LINE__;

	if (contours.size() == 0)
	{
		return  ;
	}

	std::vector<std::vector<cv::Point>>::const_iterator iter = contours.begin();
	double Max_area = std::fabs(contourArea(*iter, true));
	iter = contours.begin() + 1;
	while (iter != contours.end())
	{
		double temp_area = fabs(contourArea(*iter, true));
		if (Max_area < temp_area)
		{
			Max_area = temp_area;
			iter = contours.erase(iter - 1);
			iter++;
		}
		else
		{
			iter = contours.erase(iter);
		}
	}
    qDebug() << "面积参照物:" << Max_area<<__FILE__<<__LINE__;

	//查看检测到的最大面积
	for (int i = 0; i < (int)contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(205, 205, 0);
		//drawContours(srcImage_save, contours, i, color, 2, 8, hierarchy, 0, cv::Point(0, 0));
		cv::RotatedRect box = minAreaRect(contours[i]);

		cv::rectangle(srcImage_save, box.boundingRect(), cv::Scalar(100, 120, 155), 2);
		cv::namedWindow("mat_knowlength");
		cv::imshow("mat_knowlength", srcImage_save);

		cv::Mat Roi = srcImage_save(cv::Rect(box.boundingRect().x, box.boundingRect().y, box.boundingRect().width, box.boundingRect().height));
		cv::namedWindow("蓝色参照物");
		cv::imshow("蓝色参照物", Roi);

		//double toppoint = box.center.y - (box.boundingRect().height / 2);
		//double bottompoint = box.center.y + (box.boundingRect().height / 2);
		//double test = bottompoint - toppoint;
		//划线
		//cv::line(srcImage_save, cv::Point(0, toppoint), cv::Point(cols_image, toppoint), color, 1);
		//cv::line(srcImage_save, cv::Point(0, bottompoint), cv::Point(cols_image, bottompoint), color, 1);

		double height = box.boundingRect().height;
		//像素比例 = mm/px; 每毫米多少像素
		Imagescale = height / REFERENCE_HEIGHT;

        qDebug() << "参照物像素总高度:" << height<<"每毫米mm =="<< Imagescale <<"像素"<<__FILE__<<__LINE__;

		//cv::namedWindow("mat_knowlength");
		//cv::imshow("mat_knowlength", srcImage_save);
		return;
	}
	return;
}



//比较轮廓周长(USB_Port_Lean用来进行轮廓排序)
bool Contour_length(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2)
{
  return cv::arcLength(contour1,true) > cv::arcLength(contour2,true);
}

//求中值
double mid(double a[],int n)
{
    int mid;
    if (n%2==0)
    mid = (a[n/2-1]+a[n/2])/2;
    else
    mid = a[(n-1)/2];
    return mid;
}

void TestHeight_Module::Find_Scalefactor(cv::Mat Image,double &Scalefactor,cv::Point2f &center_pos)
{
    cv::Mat temp_image = Image.clone();
    cv::Mat mat_knowlength;
    cv::cvtColor(Image,mat_knowlength,cv::COLOR_BGR2HSV);
    int H,S,V;
    int rows_image = mat_knowlength.rows;
    int cols_image = mat_knowlength.cols;
    for(int j = 0 ; j<rows_image; j++){
        for(int i = 0 ; i<cols_image; i++){
               H = mat_knowlength.at<cv::Vec3b>(j,i)[0];
               S = mat_knowlength.at<cv::Vec3b>(j,i)[1];
               V = mat_knowlength.at<cv::Vec3b>(j,i)[2];
               if ((H >= 0) && (H <= 180) && (S >= 0) && (S <= 255) && (V >= 0) && (V <= 46))//检测黑色
               {
                   mat_knowlength.at<cv::Vec3b>(j, i)[0] = 255;
                   mat_knowlength.at<cv::Vec3b>(j, i)[1] = 255;
                   mat_knowlength.at<cv::Vec3b>(j, i)[2] = 255;
               }else
               {
                   mat_knowlength.at<cv::Vec3b>(j, i)[0] = 0;
                   mat_knowlength.at<cv::Vec3b>(j, i)[1] = 0;
                   mat_knowlength.at<cv::Vec3b>(j, i)[2] = 0;
               }
           }
       }
    cv::namedWindow("2值图");
    cv::imshow("2值图", mat_knowlength);
    int iLowH = 255;
    int iHighH = 255;
    int iLowS = 255;
    int iHighS = 255;
    int iLowV = 255;
    int iHighV = 255;
    //因为我们读取的是彩色图，直方图均衡化需要在HSV空间做
    std::vector<cv::Mat> hsvSplit;
    cv::split(mat_knowlength, hsvSplit);
    cv:: equalizeHist(hsvSplit[2], hsvSplit[2]);
    cv::merge(hsvSplit, mat_knowlength);
    cv::Mat outputImg;
    cv::inRange(mat_knowlength, cv::Scalar(iLowH, iLowS, iLowV), cv::Scalar(iHighH, iHighS, iHighV), outputImg);
    //开操作 (去除一些噪点)
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
    cv::morphologyEx(outputImg, outputImg, cv::MORPH_OPEN, element);
    //闭操作 (连接一些连通域)
    cv::morphologyEx(outputImg, outputImg, cv::MORPH_CLOSE, element);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(outputImg, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
    if (contours.size() == 0){
        return ;
    }
    qDebug()<<"比例因子个数"<<contours.size();
    std::vector<std::vector<cv::Point>>::const_iterator iter = contours.begin();
    double Max_area= std::fabs(cv::contourArea(*iter, true));
    iter = contours.begin() + 1;
    while (iter != contours.end()) {
        double temp_area = std::fabs(cv::contourArea(*iter,true));
        if(Max_area < temp_area)
        {
            Max_area = temp_area;
            iter = contours.erase(iter -1);
            iter++;
        }else{
            iter = contours.erase(iter);
        }
    }
    qDebug()<<"面积"<<Max_area;
    //查看检测到的最大面积
    for (int i = 0; i < (int)contours.size(); i++)
    {
        cv::Scalar color = cv::Scalar(250,250,250);
        cv::drawContours(temp_image, contours, i, color, 2, 8, hierarchy, 0, cv::Point(0,0));
    }
    cv::namedWindow("图");
    cv::imshow("图", temp_image);
    //计算比例
    cv::RotatedRect box = minAreaRect(contours[0]);
    qDebug()<<"已知物体的宽:"<<box.size.width;
    qDebug()<<"已知物体的高:"<<box.size.height;
    Scalefactor = PHY_LEN/box.size.height;
    qDebug()<<"像素/毫米比例:"<<Scalefactor;
    center_pos = box.center;
}



void TestHeight_Module::ImageLoad(QString Fileload)
{
    char *chfile;
    QByteArray ba = Fileload.toLatin1(); // must
    chfile = ba.data();
    cv::Mat input_image;
    input_image = cv::imread(chfile);
    if (input_image.empty()){
       return;
    }
    //Step1--先找到基准物 算出比例因子
    cv::Point2f centerpos;
    Find_Scalefactor(input_image,m_Scalefactor,centerpos);
    qDebug()<<"重点X"<<centerpos.x<<"重点Y"<<centerpos.y;
    //定义一个Mat类型并设置ROI区域
    cv::Rect Roirect;
    Roirect.x = 0;
    Roirect.y = (input_image.cols/2 - TUBE_WIDTH);
    Roirect.width = input_image.cols;
    Roirect.height = TUBE_WIDTH;

    cv::Mat imageROI = input_image(Roirect);
    cv::namedWindow("试管区");
    cv::imshow("试管区", imageROI );
    cv::Mat show = imageROI.clone();
    cv::medianBlur(imageROI,imageROI,5);
    cv::GaussianBlur(imageROI, imageROI,cv::Size(3, 3), 0);
    cv::cvtColor(imageROI,imageROI,cv::COLOR_BGR2GRAY);
    cv::Canny(imageROI,imageROI,40,140,3,false);
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(imageROI, contours, hierarchy, cv::RETR_CCOMP, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
    std::sort(contours.begin(), contours.end(), Contour_length);
    std::vector<std::vector<cv::Point>>::const_iterator iter = contours.begin();
    double Max_length = std::fabs(cv::arcLength(*iter, true));
    iter = contours.begin() + 1;
    while (iter != contours.end()) {
        qDebug()<<"面积"<<std::fabs(cv::contourArea(*iter))<<"周长"<<cv::arcLength(*iter, true);
        double temp_length = std::fabs(cv::arcLength(*iter,true));
        if(Max_length < temp_length)
        {
            Max_length = temp_length;
            iter = contours.erase(iter -1);
            iter++;
        }else{
            iter = contours.erase(iter);
        }
    }
    //查看检测到的最大轮廓周
    for (int i = 0; i < (int)contours.size(); i++)
    {
        double max_dConlength = std::fabs(cv::arcLength(contours[i], true));
        qDebug()<<"MAX面积"<<std::fabs(cv::contourArea(contours[i]))<<"MAX周长"<<max_dConlength;
        cv::Scalar color = cv::Scalar(250,250,250);
        cv::drawContours(show, contours, i, color, 2, 8, hierarchy, 0, cv::Point(0,0));
    }
    qDebug()<<contours.size();
    cv::namedWindow("output");
    cv::imshow("output",show);
    //取 10个最小的 X点 求平均数 （防止高低不平）
    iter = contours.begin();
    std::vector<double> posx;
    posx.clear();
    for(int i = 0; i< (int)iter->size(); i++){
        posx.push_back(iter->at(i).x);
    }
    std::sort(posx.begin(),posx.end());
    //qDebug()<<"点集合:"<<posx;
    double midvalue[NUMBER_MID_VALUE];
    for(int i = 0 ; i< NUMBER_MID_VALUE ;i++){
        midvalue[i] = posx.at(i);
    }
    double bloodHeight = mid(midvalue,NUMBER_MID_VALUE);
    //计算液体高度
    double bottom2Liquidsurface = centerpos.x - bloodHeight; //液面到矩形中心点的像素高度
    qDebug()<<"图像像素高度:"<<input_image.cols<<"液面高度"<<bloodHeight<<"液体到底部像素"<<bottom2Liquidsurface<<__FILE__<<__LINE__;
    double bottom2Liquidsurface_mm = bottom2Liquidsurface * m_Scalefactor;
    qDebug()<<"针下降距离标值高度:"<<bottom2Liquidsurface_mm<<__FILE__<<__LINE__;
}


