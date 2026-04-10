#pragma execution_character_set("utf-8")
#include "CameraWindow.h"
#include "quiutils.h"
#include  "ini_file.h"
#include <QMessageBox>
#include <QIntValidator>

CameraWindow::CameraWindow(QWidget *parent)
    : QWidget(parent), timer(nullptr), currentCameraId(0),
      frameWidth(640), frameHeight(480),currentRotation(0)
{
    setupUI();

    // 自动检测可用摄像头（0~9）
    for (int i = 0; i < 10; ++i) {
        cv::VideoCapture testCap(i);
        if (testCap.isOpened()) {
            cameraCombo->addItem(QString("摄像头 %1").arg(i), i);
            testCap.release();
        }
    }

    if (cameraCombo->count() > 0) {
        openCamera(cameraCombo->currentData().toInt());
    } else {
        videoLabel->setText("未检测到任何摄像头");
    }

    // 启动定时器刷新视频
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CameraWindow::updateFrame);
    timer->start(30); // 约33帧/秒
}

CameraWindow::~CameraWindow()
{
    if (timer) timer->stop();
    if (cap.isOpened()) cap.release();
}


void CameraWindow::setupUI()
{
    // ========== 全局样式表：医疗蓝主题 ==========
    this->setStyleSheet(R"(
        /* 主窗口背景：医疗深蓝 */
        QWidget {
            background-color: #1e3a5f;
            font-family: "Microsoft YaHei", "Segoe UI", Arial, sans-serif;
            font-size: 12px;
        }
        /* 视频显示标签 */
        QLabel#videoLabel {
            background-color: #0a1c2f;
            border-radius: 8px;
            border: 1px solid #2c5a7a;
            box-shadow: 0 2px 6px rgba(0,0,0,0.3);
        }
        /* 普通标签 */
        QLabel {
            color: #e0f0ff;
            font-weight: 500;
            padding: 4px;
        }
        /* 下拉框 */
        QComboBox {
            background-color: #f5f9ff;
            border: 1px solid #3a6e9f;
            border-radius: 6px;
            padding: 5px 8px;
            min-width: 100px;
            color: #1e3a5f;
        }
        QComboBox:hover {
            border-color: #5a9ed0;
        }
        /* 单行输入框 */
        QLineEdit {
            background-color: #f5f9ff;
            border: 1px solid #3a6e9f;
            border-radius: 6px;
            padding: 5px 8px;
            color: #1e3a5f;
            min-width: 50px;
        }
        QLineEdit:focus {
            border-color: #7ab8e0;
        }
        /* 双精度输入框 */
        QDoubleSpinBox {
            background-color: #f5f9ff;
            border: 1px solid #3a6e9f;
            border-radius: 6px;
            padding: 5px 8px;
            color: #1e3a5f;
            min-width: 70px;
        }
        QDoubleSpinBox:focus {
            border-color: #7ab8e0;
        }
        QSpinBox {
            background-color: #f5f9ff;
            border: 1px solid #3a6e9f;
            border-radius: 6px;
            padding: 5px 8px;
            color: #1e3a5f;
            min-width: 70px;
        }
        QSpinBox:focus {
            border-color: #7ab8e0;
        }
        /* 按钮 */
        QPushButton {
            background-color: #2c7da0;
            border: none;
            border-radius: 6px;
            color: white;
            font-weight: bold;
            padding: 6px 16px;
            min-width: 70px;
        }
        QPushButton:hover {
            background-color: #1f6390;
        }
        QPushButton:pressed {
            background-color: #145a7a;
        }
    )");

    // 创建控件
    videoLabel = new QLabel(this);
    videoLabel->setObjectName("videoLabel");
    videoLabel->setFixedSize(640, 480);
    videoLabel->setAlignment(Qt::AlignCenter);

    cameraCombo = new QComboBox(this);
    QLabel *cameraLabel = new QLabel("选择摄像头:", this);

    // 旋转角度下拉框
    QLabel *rotationLabel = new QLabel("画面旋转:", this);
    rotationCombo = new QComboBox(this);
    rotationCombo->addItem("0°", 0);
    rotationCombo->addItem("90°", 90);
    rotationCombo->addItem("180°", 180);
    rotationCombo->addItem("270°", 270);
    rotationCombo->setCurrentIndex(0);

    QLabel *xLabel = new QLabel("X 坐标:", this);
    xEdit = new QLineEdit(this);
    xEdit->setText("210");
    xEdit->setValidator(new QIntValidator(this));

    QLabel *yLabel = new QLabel("Y 坐标:", this);
    yEdit = new QLineEdit(this);
    yEdit->setText("3140");
    yEdit->setValidator(new QIntValidator(this));

    QLabel *heightLabel = new QLabel("下针高度:", this);
    heightSpin = new QDoubleSpinBox(this);
    heightSpin->setRange(0, 100);
    heightSpin->setValue(84.8);
    heightSpin->setDecimals(1);
    heightSpin->setSuffix(" mm");


    QLabel *timesLabel = new QLabel("项目数:", this);
    suckTimes = new QSpinBox(this);
    suckTimes->setRange(1, 5);
    suckTimes->setValue(1);
    suckTimes->setSuffix("项");

    applyBtn = new QPushButton("应用设置", this);
    applyBtn->setCursor(Qt::PointingHandCursor);

    resetBtn = new QPushButton("复位", this);
    resetBtn->setCursor(Qt::PointingHandCursor);

    suckPrpBtn = new QPushButton("吸Prp", this);
    suckPrpBtn->setCursor(Qt::PointingHandCursor);

    // 布局
    QHBoxLayout *controlLayout1 = new QHBoxLayout;
    controlLayout1->addWidget(cameraLabel);
    controlLayout1->addWidget(cameraCombo);
    controlLayout1->addWidget(rotationLabel);
    controlLayout1->addWidget(rotationCombo);
    controlLayout1->addStretch();

    // 第二行：参数输入（X、Y、高度）
    QHBoxLayout *paramLayout = new QHBoxLayout;
    paramLayout->setSpacing(12);
    paramLayout->addWidget(xLabel);
    paramLayout->addWidget(xEdit, 1);
    paramLayout->addWidget(yLabel);
    paramLayout->addWidget(yEdit, 1);
    paramLayout->addWidget(heightLabel);
    paramLayout->addWidget(heightSpin, 1);
    paramLayout->addWidget(timesLabel);
    paramLayout->addWidget(suckTimes, 1);

    // 第三行：按钮（右对齐）
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();          // 将按钮推到右侧
    buttonLayout->addWidget(applyBtn);
    buttonLayout->addWidget(suckPrpBtn);
    buttonLayout->addWidget(resetBtn);
    buttonLayout->setSpacing(12);

    // 主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(videoLabel, 0, Qt::AlignCenter);
    mainLayout->addLayout(controlLayout1);
    mainLayout->addLayout(paramLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    // 信号槽连接
    connect(cameraCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraWindow::onCameraChanged);
    connect(applyBtn, &QPushButton::clicked, this, &CameraWindow::onApplySettings);
    connect(rotationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraWindow::onRotationChanged);

    connect(resetBtn, &QPushButton::clicked, this, &CameraWindow::onResetClicked);
    connect(suckPrpBtn, &QPushButton::clicked, this, &CameraWindow::onTestSuckPRP);


    setWindowTitle("医疗级摄像头控制系统");
    setFixedSize(720, 660);
}

void CameraWindow::onRotationChanged(int index)
{
    Q_UNUSED(index);
    currentRotation = rotationCombo->currentData().toInt();
    // 不需要其他操作，updateFrame 会使用新的旋转角度
}

void CameraWindow::onResetClicked()
{
    Q_EMIT backOrigin();
}

void CameraWindow::openCamera(int cameraId)
{
    if (cap.isOpened()) cap.release();
    cap.open(cameraId);
    if (!cap.isOpened()) {
        videoLabel->setText(QString("无法打开摄像头 %1").arg(cameraId));
        return;
    }

    // 获取一帧以确定实际尺寸
    cv::Mat frame;
    cap >> frame;
    if (!frame.empty()) {
        frameWidth = frame.cols;
        frameHeight = frame.rows;
        // 按比例缩放显示标签大小（如果画面太大）
        videoLabel->setFixedSize(frameWidth > 640 ? 640 : frameWidth,
                                 frameHeight > 480 ? 480 : frameHeight);
    }
    currentCameraId = cameraId;
}

void CameraWindow::updateFrame()
{
    if (!cap.isOpened()) return;

    cv::Mat frame;
    cap >> frame;
    if (frame.empty()) return;

    // 转换颜色格式：BGR -> RGB (Qt 显示需要)
    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);
    // 根据当前旋转角度旋转图像
    if (currentRotation != 0) {
       cv::Mat rotated;
       switch (currentRotation) {
       case 90:
           cv::rotate(frame, rotated, cv::ROTATE_90_CLOCKWISE);
           break;
       case 180:
           cv::rotate(frame, rotated, cv::ROTATE_180);
           break;
       case 270:
           cv::rotate(frame, rotated, cv::ROTATE_90_COUNTERCLOCKWISE);
           break;
       default:
           rotated = frame;
           break;
       }
       frame = rotated;  // 使用旋转后的图像
    }

    QImage qimg(frame.data, frame.cols, frame.rows,
                static_cast<int>(frame.step), QImage::Format_RGB888);

    // 缩放图像以适应 videoLabel 的大小（保持比例）
    QPixmap pix = QPixmap::fromImage(qimg);
    pix = pix.scaled(videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    videoLabel->setPixmap(pix);
}

void CameraWindow::onCameraChanged(int index)
{
    if (index < 0) return;
    int newId = cameraCombo->currentData().toInt();
    if (newId != currentCameraId) {
        openCamera(newId);
    }
}

void CameraWindow::onApplySettings()
{
    // 获取坐标
    bool okX, okY;
    int x = xEdit->text().toInt(&okX);
    int y = yEdit->text().toInt(&okY);
    if (!okX || !okY) {
        QMessageBox::warning(this, "输入错误", "坐标必须为整数");
        return;
    }

    double height = heightSpin->value();

    Q_EMIT moveTestHeigh(x,y,height);
}

void CameraWindow::adjustWindowSize()
{
    // 此函数已被 onApplySettings 代替，保留以备他用
    onApplySettings();
}


void CameraWindow::onTestSuckPRP()
{
    QByteArrayList sendArrList;
    sendArrList.clear();

    bool okX, okY;
    int x = xEdit->text().toInt(&okX);
    int y = yEdit->text().toInt(&okY);
    if (!okX || !okY) {
        QMessageBox::warning(this, "输入错误", "坐标必须为整数");
        return;
    }

    double height = heightSpin->value();
    QPoint targAxis(x,y);
    QPoint thrpoPRP(0,0);

    int times = suckTimes->value();

    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,thrpoPRP);

	QList<QPoint> suckList;
	suckList.reserve(times);
	for (int k = 0; k < times; ++k)
		suckList.append(thrpoPRP);
    QUIUtils::SuckPRPandSpitoutPRP(sendArrList,height,targAxis,suckList);

    Q_EMIT testSuckPrpAct(sendArrList);
}


