#ifndef OPENCVFINDHEIGH_H
#define OPENCVFINDHEIGH_H

#pragma execution_character_set("utf-8")
#include <QObject>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <opencv2/highgui/highgui_c.h>
#include <QMessageBox>
#include <io.h>
#include <string>
#include <QRect>
#include "correct_data.h"

using namespace std;

class opencvfindHeigh : public QObject
{
    Q_OBJECT
public:
    explicit opencvfindHeigh(QObject *parent = nullptr);
    ~opencvfindHeigh();

    // 将路径常量提取为静态成员，方便统一管理
    static const QString DEFAULT_IMAGE_SUBDIR;

    bool createImageStorageDirectory();
    const QString& getImagePath() const { return m_imagePath; }

public slots:
   void Start();
   void handleTriggerTestHeight();
   void RecvOpenInstrumentCamera(const quint8 CameraPositioning); /*判断打开摄像头内、外部*/
   void handleRepPrpheight(const QString &numid,bool replflag);

private:

   /*
   * 图片旋转
   */
   bool pinwheel(const cv::Mat& inputImage, int degree, cv::Mat& outputImage);



   /**  识别左侧的参照物
    * @brief Extractthecolorblockontheleft
    * @param img
    * @param imgraito
    * @return
    */
   bool Extractthecolorblockontheleft(cv::Mat img,double &imgraito);

   /** 识别红细胞高度
    * @brief IdentifyRedBloodcellHeight
    * @param img
    */
   bool IdentifyRBCHeight(cv::Mat img, double ratioimg, double &BloodHeigh);

   void IdentifyTheTubes(cv::Mat img, cv::Mat &outTuberoi, cv::Rect &roiTube);

    /**
     * @brief inputImage  识别PRP
     * @param imageOrinin
     */
    bool opencvIdentifyPrp(cv::Mat &inputImage, double &outBottomBloodHeight,QString &outErr);




signals:
   void FinishTestHigh(QString Path,double, bool , QString);
   void Testheightfinish(const bool );
   void FindFailed(QString,QString);



private:
   QString m_imagePath;

   QString m_replaceid;
   bool m_isreptestheigh;

   quint8  m_OpenCameraPosition; //打开摄像头位置

   const int Image_Width =  480;
   const int Image_Height = 640;
};

#endif // OPENCVFINDHEIGH_H
