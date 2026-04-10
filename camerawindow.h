#ifndef CAMERAWINDOW_H
#define CAMERAWINDOW_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class CameraWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWindow(QWidget *parent = nullptr);
    ~CameraWindow();

private slots:
    void updateFrame();          // 更新视频帧
    void onCameraChanged(int index);      // 切换摄像头
    void onApplySettings();      // 应用窗口坐标和高度
    void onRotationChanged(int index);
    void onResetClicked();
    void onTestSuckPRP();

signals:
    void moveTestHeigh(const int x, const int y,const double downMm);
    void backOrigin();
    void testSuckPrpAct(const QByteArrayList);
private:
    void setupUI();              // 创建控件布局
    void openCamera(int cameraId);        // 打开指定摄像头
    void adjustWindowSize();     // 根据设置的高度调整窗口大小



    cv::VideoCapture cap;
    QTimer *timer;
    //QMutex m_frameMutex;

    // UI 控件
    QLabel *videoLabel;          // 显示视频的标签
    QComboBox *cameraCombo;      // 摄像头选择下拉框
    QLineEdit *xEdit;            // X 坐标输入框
    QLineEdit *yEdit;            // Y 坐标输入框
    QDoubleSpinBox *heightSpin;        // 窗口高度设置（像素）
    QSpinBox *suckTimes;
    QPushButton *applyBtn;
    QPushButton *resetBtn;
    QPushButton *suckPrpBtn;

    int currentCameraId;         // 当前打开的摄像头ID
    int frameWidth;              // 当前视频帧宽度
    int frameHeight;             // 当前视频帧高度

    QComboBox *rotationCombo;   // 旋转角度选择下拉框
    int currentRotation;        // 当前旋转角度（0, 90, 180, 270）
};

#endif // CAMERAWINDOW_H
