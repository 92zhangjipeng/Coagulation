#ifndef TESTOPCV_H
#define TESTOPCV_H

#include <QWidget>
#include <opencv2/opencv.hpp>


// 分层检测参数结构体
struct LayerConfig {
    int cell_min_height = 50;       // 血细胞层最小高度(像素)
    int plasma_max_dark = 80;       // 血浆区域最大暗度阈值
    float gradient_thresh = 0.3f;   // 分层梯度阈值
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


private slots:


    void on_spinBox_valueChanged(int arg1);

private:
    void initsignal();

    //识别左侧颜色块
    bool Extractthecolorblockontheleft(cv::Mat img, double &imgraito);


    bool hasTextTraditional(cv::Mat img);

    void wheeltoImage(int wheel);

    bool matImagewheel(cv::Mat cutimage, int degree ,cv::Mat &OutMat); //图片旋转


    /** 识别红细胞高度  RBC
     * @brief IdentifyRedBloodcellHeight
     * @param img
     */
    void IdentifyRBCHeight(cv::Mat img, double ratioimg, double &BloodHeigh);

    /** 血浆最底部
     * @brief BuffyCoatTopDpi
     * @return
     */
    bool BuffyCoatTopDpi(cv::Mat origimImag, cv::Mat tubeImage);

    void IdentifyTheTubes(cv::Mat img, cv::Mat &outTuberoi, cv::Rect &roiTube);

    void  trayfindImg();


private:
    Ui::TestOpcv *ui;

    cv::Mat imageOrinin;

    double m_imgratio;

    double m_prporiginHeight = 0.0f;
    QString m_imagePath;
};

#endif // TESTOPCV_H
