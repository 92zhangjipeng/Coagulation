#include "CameraWindow.h"
#include "quiutils.h"
#include "ini_file.h"
#include <QMessageBox>
#include <QIntValidator>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyleOption>
#include <QPainter>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif


CameraWindow::CameraWindow(QWidget *parent)
    : QWidget(parent), timer(nullptr), currentCameraId(0),
      frameWidth(640), frameHeight(480), currentRotation(0),
      m_titleBar(nullptr), m_contentWidget(nullptr), m_mainLayout(nullptr)
{
    // 设置窗口标志为无边框窗口
    setWindowFlags(Qt::FramelessWindowHint);
    // 注意：不要设置 WA_TranslucentBackground，否则标题栏会透明
    setAttribute(Qt::WA_StyledBackground);  // 启用样式表背景

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
    timer->start(30);
}

CameraWindow::~CameraWindow()
{
    if (timer) timer->stop();
    if (cap.isOpened()) cap.release();
}

void CameraWindow::setupUI()
{
    // 设置窗口基础大小（稍微增大尺寸，减少拥挤感）
    setFixedSize(820, 760);

    // ========== 设置窗口整体背景（深色医疗蓝，与标题栏呼应） ==========
    this->setStyleSheet(R"(
        CameraWindow {
            background-color: #3a8ab8;
        }
    )");

    // ========== 创建主布局（垂直布局，无边框） ==========
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // ========== 创建自定义标题栏 ==========
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle("医疗级摄像头控制系统 - PRP智能采集");
    m_titleBar->setFixedHeight(48);  // 稍微增高标题栏

    // 连接标题栏信号
    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &CameraWindow::onCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &CameraWindow::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &CameraWindow::onMaximizeRequested);

    // ========== 创建内容区域容器（白色圆角卡片） ==========
    m_contentWidget = new QWidget(this);
    m_contentWidget->setObjectName("contentWidget");
    m_contentWidget->setStyleSheet(R"(
        QWidget#contentWidget {
            background-color: #f5f8fc;
            border-radius: 0 0 16px 16px;
        }
    )");

    // ========== 内容区域布局（增加内边距，让内容更宽松） ==========
    QVBoxLayout *contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setSpacing(24);
    contentLayout->setContentsMargins(30, 24, 30, 30);

    // 视频显示区域（带医疗风格边框和阴影效果）
    QWidget *videoContainer = new QWidget(m_contentWidget);
    videoContainer->setStyleSheet(R"(
        QWidget {
            background-color: #eaf3fa;
            border-radius: 16px;
            border: 1px solid #c8dde8;
        }
    )");

    QVBoxLayout *videoContainerLayout = new QVBoxLayout(videoContainer);
    videoContainerLayout->setContentsMargins(12, 12, 12, 12);

    videoLabel = new QLabel(videoContainer);
    videoLabel->setObjectName("videoLabel");
    videoLabel->setFixedSize(640, 480);
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setStyleSheet(R"(
        QLabel#videoLabel {
            background-color: #dcecf5;
            border-radius: 12px;
        }
    )");

    videoContainerLayout->addWidget(videoLabel, 0, Qt::AlignCenter);

    // ========== 控制面板（白色卡片，增加内边距） ==========
    QWidget *controlPanel = new QWidget(m_contentWidget);
    controlPanel->setObjectName("controlPanel");
    controlPanel->setStyleSheet(R"(
        QWidget#controlPanel {
            background-color: white;
            border-radius: 16px;
            border: 1px solid #d8e6f0;
        }
    )");

    QVBoxLayout *panelLayout = new QVBoxLayout(controlPanel);
    panelLayout->setSpacing(20);
    panelLayout->setContentsMargins(24, 20, 24, 20);

    // ========== 第一行：摄像头选择和旋转 ==========
    QHBoxLayout *row1Layout = new QHBoxLayout;
    row1Layout->setSpacing(20);

    QLabel *cameraLabel = new QLabel("📷 摄像头", controlPanel);
    cameraLabel->setStyleSheet("color: #2c6e9e; font-weight: bold; font-size: 13px;");
    cameraCombo = new QComboBox(controlPanel);
    cameraCombo->setFixedHeight(34);
    cameraCombo->setStyleSheet(R"(
        QComboBox {
            background-color: #f8fbfe;
            border: 1px solid #c8dde8;
            border-radius: 8px;
            padding: 6px 14px;
            min-width: 140px;
            color: #2c6e9e;
            font-size: 12px;
        }
        QComboBox:hover {
            border-color: #7ab8d8;
            background-color: white;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
    )");

    QLabel *rotationLabel = new QLabel("🔄 画面旋转", controlPanel);
    rotationLabel->setStyleSheet("color: #2c6e9e; font-weight: bold; font-size: 13px;");
    rotationCombo = new QComboBox(controlPanel);
    rotationCombo->setFixedHeight(34);
    rotationCombo->addItem("0°", 0);
    rotationCombo->addItem("90°", 90);
    rotationCombo->addItem("180°", 180);
    rotationCombo->addItem("270°", 270);
    rotationCombo->setStyleSheet(cameraCombo->styleSheet());

    row1Layout->addWidget(cameraLabel);
    row1Layout->addWidget(cameraCombo);
    row1Layout->addStretch();
    row1Layout->addWidget(rotationLabel);
    row1Layout->addWidget(rotationCombo);
    row1Layout->addStretch();

    // ========== 第二行：参数输入区域 ==========
    QHBoxLayout *row2Layout = new QHBoxLayout;
    row2Layout->setSpacing(20);

    // X 坐标
    QWidget *xWidget = new QWidget(controlPanel);
    QVBoxLayout *xLayout = new QVBoxLayout(xWidget);
    xLayout->setContentsMargins(0, 0, 0, 0);
    xLayout->setSpacing(6);
    QLabel *xLabel = new QLabel("X 坐标 (mm)", xWidget);
    xLabel->setStyleSheet("color: #5a8aaa; font-size: 11px;");
    xEdit = new QLineEdit(xWidget);
    xEdit->setText("210");
    xEdit->setFixedHeight(36);
    xEdit->setValidator(new QIntValidator(this));
    xEdit->setStyleSheet(R"(
        QLineEdit {
            background-color: #f8fbfe;
            border: 1px solid #c8dde8;
            border-radius: 8px;
            padding: 8px 12px;
            min-width: 100px;
            color: #2c6e9e;
            font-size: 13px;
        }
        QLineEdit:focus {
            border-color: #5aa9d0;
            background-color: white;
        }
    )");
    xLayout->addWidget(xLabel);
    xLayout->addWidget(xEdit);

    // Y 坐标
    QWidget *yWidget = new QWidget(controlPanel);
    QVBoxLayout *yLayout = new QVBoxLayout(yWidget);
    yLayout->setContentsMargins(0, 0, 0, 0);
    yLayout->setSpacing(6);
    QLabel *yLabel = new QLabel("Y 坐标 (mm)", yWidget);
    yLabel->setStyleSheet("color: #5a8aaa; font-size: 11px;");
    yEdit = new QLineEdit(yWidget);
    yEdit->setText("3140");
    yEdit->setFixedHeight(36);
    yEdit->setValidator(new QIntValidator(this));
    yEdit->setStyleSheet(xEdit->styleSheet());
    yLayout->addWidget(yLabel);
    yLayout->addWidget(yEdit);

    // 下针高度
    QWidget *heightWidget = new QWidget(controlPanel);
    QVBoxLayout *heightLayout = new QVBoxLayout(heightWidget);
    heightLayout->setContentsMargins(0, 0, 0, 0);
    heightLayout->setSpacing(6);
    QLabel *heightLabel = new QLabel("下针高度", heightWidget);
    heightLabel->setStyleSheet("color: #5a8aaa; font-size: 11px;");
    heightSpin = new QDoubleSpinBox(heightWidget);
    heightSpin->setFixedHeight(36);
    heightSpin->setRange(0, 100);
    heightSpin->setValue(84.8);
    heightSpin->setDecimals(1);
    heightSpin->setSuffix(" mm");
    heightSpin->setStyleSheet(R"(
        QDoubleSpinBox {
            background-color: #f8fbfe;
            border: 1px solid #c8dde8;
            border-radius: 8px;
            padding: 6px 12px;
            min-width: 110px;
            color: #2c6e9e;
            font-size: 13px;
        }
        QDoubleSpinBox:focus {
            border-color: #5aa9d0;
            background-color: white;
        }
    )");
    heightLayout->addWidget(heightLabel);
    heightLayout->addWidget(heightSpin);

    // 项目数
    QWidget *timesWidget = new QWidget(controlPanel);
    QVBoxLayout *timesLayout = new QVBoxLayout(timesWidget);
    timesLayout->setContentsMargins(0, 0, 0, 0);
    timesLayout->setSpacing(6);
    QLabel *timesLabel = new QLabel("项目数", timesWidget);
    timesLabel->setStyleSheet("color: #5a8aaa; font-size: 11px;");
    suckTimes = new QSpinBox(timesWidget);
    suckTimes->setFixedHeight(36);
    suckTimes->setRange(1, 5);
    suckTimes->setValue(1);
    suckTimes->setSuffix(" 项");
    suckTimes->setStyleSheet(R"(
        QSpinBox {
            background-color: #f8fbfe;
            border: 1px solid #c8dde8;
            border-radius: 8px;
            padding: 6px 12px;
            min-width: 90px;
            color: #2c6e9e;
            font-size: 13px;
        }
        QSpinBox:focus {
            border-color: #5aa9d0;
            background-color: white;
        }
    )");
    timesLayout->addWidget(timesLabel);
    timesLayout->addWidget(suckTimes);

    row2Layout->addWidget(xWidget);
    row2Layout->addWidget(yWidget);
    row2Layout->addWidget(heightWidget);
    row2Layout->addWidget(timesWidget);
    row2Layout->addStretch();

    // ========== 第三行：按钮区域 ==========
    QHBoxLayout *row3Layout = new QHBoxLayout;
    row3Layout->setSpacing(20);
    row3Layout->addStretch();

    // 医疗风格按钮样式
    QString medicalBtnStyle = R"(
        QPushButton {
            background-color: #4a9ecf;
            border: none;
            border-radius: 10px;
            color: white;
            font-weight: bold;
            font-size: 14px;
            padding: 10px 32px;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #3a8ab8;
        }
        QPushButton:pressed {
            background-color: #2c6e9e;
        }
    )";

    QString dangerBtnStyle = R"(
        QPushButton {
            background-color: #e87a6e;
            border: none;
            border-radius: 10px;
            color: white;
            font-weight: bold;
            font-size: 14px;
            padding: 10px 32px;
            min-width: 120px;
        }
        QPushButton:hover {
            background-color: #d46558;
        }
        QPushButton:pressed {
            background-color: #c05044;
        }
    )";

    applyBtn = new QPushButton("✓ 应用设置", controlPanel);
    applyBtn->setCursor(Qt::PointingHandCursor);
    applyBtn->setStyleSheet(medicalBtnStyle);

    suckPrpBtn = new QPushButton("💉 吸Prp", controlPanel);
    suckPrpBtn->setCursor(Qt::PointingHandCursor);
    suckPrpBtn->setStyleSheet(medicalBtnStyle);

    resetBtn = new QPushButton("↺ 复位", controlPanel);
    resetBtn->setCursor(Qt::PointingHandCursor);
    resetBtn->setStyleSheet(dangerBtnStyle);

    row3Layout->addWidget(applyBtn);
    row3Layout->addWidget(suckPrpBtn);
    row3Layout->addWidget(resetBtn);
    row3Layout->addStretch();

    // 组装面板布局
    panelLayout->addLayout(row1Layout);
    panelLayout->addLayout(row2Layout);
    // 添加分隔线
    QFrame *line = new QFrame(controlPanel);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #e0ecf3; max-height: 1px;");
    panelLayout->addWidget(line);
    panelLayout->addLayout(row3Layout);

    // 组装内容布局
    contentLayout->addWidget(videoContainer, 0, Qt::AlignCenter);
    contentLayout->addWidget(controlPanel);

    // 添加标题栏和内容区域到主布局
    m_mainLayout->addWidget(m_titleBar);
    m_mainLayout->addWidget(m_contentWidget);

    // 信号槽连接
    connect(cameraCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraWindow::onCameraChanged);
    connect(applyBtn, &QPushButton::clicked, this, &CameraWindow::onApplySettings);
    connect(rotationCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &CameraWindow::onRotationChanged);
    connect(resetBtn, &QPushButton::clicked, this, &CameraWindow::onResetClicked);
    connect(suckPrpBtn, &QPushButton::clicked, this, &CameraWindow::onTestSuckPRP);
}

void CameraWindow::onRotationChanged(int index)
{
    Q_UNUSED(index);
    currentRotation = rotationCombo->currentData().toInt();
}

void CameraWindow::onResetClicked()
{
    Q_EMIT backOrigin();
}

void CameraWindow::onCloseRequested()
{
    close();
}

void CameraWindow::onMinimizeRequested()
{
    showMinimized();
}

void CameraWindow::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        m_titleBar->updateMaximizeButton(false);
    } else {
        showMaximized();
        m_titleBar->updateMaximizeButton(true);
    }
}

void CameraWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(isMaximized());
        }
    }
    QWidget::changeEvent(event);
}

void CameraWindow::openCamera(int cameraId)
{
    if (cap.isOpened()) cap.release();
    cap.open(cameraId);
    if (!cap.isOpened()) {
        videoLabel->setText(QString("无法打开摄像头 %1").arg(cameraId));
        return;
    }

    cv::Mat frame;
    cap >> frame;
    if (!frame.empty()) {
        frameWidth = frame.cols;
        frameHeight = frame.rows;
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

    cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

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
        frame = rotated;
    }

    QImage qimg(frame.data, frame.cols, frame.rows,
                static_cast<int>(frame.step), QImage::Format_RGB888);

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
    bool okX, okY;
    int x = xEdit->text().toInt(&okX);
    int y = yEdit->text().toInt(&okY);
    if (!okX || !okY) {
        QMessageBox::warning(this, "输入错误", "坐标必须为整数");
        return;
    }

    double height = heightSpin->value();
    Q_EMIT moveTestHeigh(x, y, height);
}

void CameraWindow::adjustWindowSize()
{
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
    QPoint targAxis(x, y);
    QPoint thrpoPRP(0, 0);

    int times = suckTimes->value();

    SingletonAxis::GetInstance()->originPos(READ_OPERRAT, thrpoPRP);

    QList<QPoint> suckList;
    suckList.reserve(times);
    for (int k = 0; k < times; ++k)
        suckList.append(thrpoPRP);
    QUIUtils::SuckPRPandSpitoutPRP(sendArrList, height, targAxis, suckList);

    Q_EMIT testSuckPrpAct(sendArrList);
}
