#ifndef TESTHEIGHT_MODULE_H
#define TESTHEIGHT_MODULE_H

#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <io.h>
#include <string>
#include <QRect>
#include <QDebug>


#define REFERENCE_LENGTH    20.0 

#define  NUMBER_MID_VALUE  25
#define  TUBE_WIDTH  110   //试管宽度
#define  TUBE_TOTAL_HEIGHT  75.3 // 试管总高度
#define  PHY_LEN     8.2  //参照物较长边边长的物理长度

class TestHeight_Module : public QObject
{
    Q_OBJECT
public:
    explicit TestHeight_Module(QObject *parent = nullptr);
	~TestHeight_Module();
private:
    void Find_Scalefactor(cv::Mat, double &, cv::Point2f &center_pos);
	void FindBluebenchmark(cv::Mat , double &Imagescale);
	void ReferencesFind(const cv::Mat& image, cv::Mat &out);//找基准物
signals:

public slots:
    void ImageLoad(QString);
    void SlotOpenCapture();
	void SlotCloseCapture();
    void SlotTakePhoto(QString);
private:
    double m_Scalefactor = 0; //比例因子
    cv::VideoCapture  mcap;
    QString folerPath;
public:
    static bool takePhotoFlag; //拍照标志
    static bool closeFlag;     //关闭标志
    static QString SampleName;

};

#endif // TESTHEIGHT_MODULE_H
