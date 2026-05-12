#include "coordinatepposit.h"
#include "ui_coordinatepposit.h"
#include "opencvFindRBC/customtitlebar.h"
#include "QsLog/include/QsLog.h"
#include <QResizeEvent>
#include <QScrollArea>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <loadequipmentpos.h>
#include "cglobal.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

CoordinatepPosit::CoordinatepPosit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CoordinatepPosit),
    m_titleBar(nullptr),
    m_tabWidget(nullptr),
    m_mainLayout(nullptr),
    m_tubeCoordinates(240, QPointF(0, 0)) // 初始化240个试管的坐标为(0,0)
{
    ui->setupUi(this);

    // 设置窗口属性，去掉系统标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // 设置主布局 - 覆盖原有的布局
    // 先删除原有的布局
    delete layout();

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 设置标题栏
    setupTitleBar();

    // 设置 TabWidget
    setupTabWidget();

    // 应用医疗浅蓝样式
    applyMedicalStyle();

    // 设置窗口背景
    this->setStyleSheet("QWidget#CoordinatepPosit { background-color: #F0F8FF; }");
}

CoordinatepPosit::~CoordinatepPosit()
{
    delete ui;
}

void CoordinatepPosit::setupTitleBar()
{
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle("坐标定位系统");

    // 连接标题栏信号
    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &CoordinatepPosit::onCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &CoordinatepPosit::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &CoordinatepPosit::onMaximizeRequested);

    m_mainLayout->addWidget(m_titleBar);
}

void CoordinatepPosit::setupTabWidget()
{
    // 创建 TabWidget
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setMovable(true);
    m_tabWidget->setTabsClosable(false);
    m_tabWidget->setDocumentMode(false);

    // 创建各个标签页的内容
    // 1. 特殊区
    QWidget *specialWidget = createSpecialTab();
    m_tabWidget->addTab(specialWidget, "特殊区");

    // 2. 通道区
    QWidget *channelWidget = createChannelTab();
    m_tabWidget->addTab(channelWidget, "通道区");

    // 3. 试管区
    QWidget *tubeWidget = createTubeTab();
    m_tabWidget->addTab(tubeWidget, "试管区");

    // 4. 试剂区
    QWidget *reagentWidget = createReagentTab();
    m_tabWidget->addTab(reagentWidget, "试剂区");

    // 5. 血样区
    QWidget *bloodWidget = createBloodTab();
    m_tabWidget->addTab(bloodWidget, "血样区");

    // 添加顶部控制区域
    QWidget *topControlWidget = new QWidget(this);
    QHBoxLayout *topControlLayout = new QHBoxLayout(topControlWidget);
    topControlLayout->setContentsMargins(20, 10, 20, 10);
    topControlLayout->setSpacing(15);

    // 测试Z轴选项
    QLabel *testZLabel = new QLabel("测试Z轴:");
    testZLabel->setStyleSheet("font-size: 14px; color: #2C5F8A;");
    QComboBox *toolTypeCombo = new QComboBox();
    toolTypeCombo->addItems({ "抓手", "试剂针" });
    toolTypeCombo->setFixedWidth(100);
    toolTypeCombo->setStyleSheet("font-size: 14px;");

    // 下降高度选项
    QLabel *dropHeightLabel = new QLabel("下降高度:");
    dropHeightLabel->setStyleSheet("font-size: 14px; color: #2C5F8A;");
    QComboBox *dropHeightCombo = new QComboBox();
    dropHeightCombo->addItems({ "顶部", "中部", "底部", "自定义" });
    dropHeightCombo->setFixedWidth(100);
    dropHeightCombo->setStyleSheet("font-size: 14px;");

    // 自定义下降高度输入框
    QLineEdit *customDropHeightEdit = new QLineEdit();
    customDropHeightEdit->setPlaceholderText("mm");
    customDropHeightEdit->setFixedWidth(80);
    customDropHeightEdit->setStyleSheet("font-size: 14px;");

    // 按钮区域
    QPushButton *saveButton = new QPushButton("保存", this);
    saveButton->setFixedSize(100, 35);
    saveButton->setStyleSheet("QPushButton { background-color: #5BB8E8; border: none; border-radius: 6px; color: white; padding: 8px 16px; font-weight: bold; font-size: 14px; } QPushButton:hover { background-color: #3A9BD4; } QPushButton:pressed { background-color: #2C7FAC; }");

    QPushButton *resetButton = new QPushButton("复位", this);
    resetButton->setFixedSize(100, 35);
    resetButton->setStyleSheet("QPushButton { background-color: #FF6B6B; border: none; border-radius: 6px; color: white; padding: 8px 16px; font-weight: bold; font-size: 14px; } QPushButton:hover { background-color: #FF5252; } QPushButton:pressed { background-color: #E53935; }");

    // 添加控件到顶部布局
    topControlLayout->addWidget(testZLabel);
    topControlLayout->addWidget(toolTypeCombo);
    topControlLayout->addSpacing(10);
    topControlLayout->addWidget(dropHeightLabel);
    topControlLayout->addWidget(dropHeightCombo);
    topControlLayout->addWidget(customDropHeightEdit);
    topControlLayout->addStretch();
    topControlLayout->addWidget(saveButton);
    topControlLayout->addWidget(resetButton);

    m_mainLayout->addWidget(topControlWidget);
    m_mainLayout->addWidget(m_tabWidget);
}

QWidget* CoordinatepPosit::createSpecialTab()
{
    QWidget *widget = new QWidget();
    QScrollArea *scrollArea = new QScrollArea(widget);
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // 添加坐标表格 - 特殊区
    QGroupBox *coordGroup = new QGroupBox("坐标设置");
    QGridLayout *gridLayout = new QGridLayout(coordGroup);
    gridLayout->setHorizontalSpacing(15);
    gridLayout->setVerticalSpacing(10);

    // 表头
    QLabel *headerLabel = new QLabel("坐标点",this);
    QLabel *headerX = new QLabel("X",this);
    QLabel *headerY = new QLabel("Y",this);
    QLabel *headerValidate = new QLabel("校验",this);
    QLabel *headerSave = new QLabel("保存",this);

    headerLabel->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerX->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerY->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerValidate->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerSave->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");

    gridLayout->addWidget(headerLabel, 0, 0);
    gridLayout->addWidget(headerX, 0, 1);
    gridLayout->addWidget(headerY, 0, 2);
    gridLayout->addWidget(headerValidate, 0, 3);
    gridLayout->addWidget(headerSave, 0, 4);

    // 第1行：原点坐标
    QLabel *originLabel = new QLabel("原点坐标：", this);
    originLabel->setStyleSheet("font-size: 14px;");
    QLineEdit *originXEdit = new QLineEdit(this);
    originXEdit->setPlaceholderText("X坐标");
    originXEdit->setFixedWidth(80);
    originXEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QLineEdit *originYEdit = new QLineEdit(this);
    originYEdit->setPlaceholderText("Y坐标");
    originYEdit->setFixedWidth(80);
    originYEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QPushButton *originValidateBtn = new QPushButton("校验", this);
    originValidateBtn->setFixedWidth(60);
    QPushButton *originSaveBtn = new QPushButton("保存", this);
    originSaveBtn->setFixedWidth(60);

    gridLayout->addWidget(originLabel, 1, 0);
    gridLayout->addWidget(originXEdit, 1, 1);
    gridLayout->addWidget(originYEdit, 1, 2);
    gridLayout->addWidget(originValidateBtn, 1, 3);
    gridLayout->addWidget(originSaveBtn, 1, 4);

    //载入原点坐标
    {
        QPoint originpos(0,0);
        SingletonAxis::GetInstance()->originPos(READ_OPERRAT,originpos);
        originXEdit->setText(QString::number(originpos.x() ));
        originYEdit->setText(QString::number(originpos.y() ));
        connect(originValidateBtn,&QPushButton::clicked,[this, originXEdit, originYEdit](){

        });

        connect(originSaveBtn,&QPushButton::clicked,[this, originXEdit, originYEdit](){
            QLOG_DEBUG()<<"保存原点坐标X"<<originXEdit->text();
            QPoint savePos(originXEdit->text().toInt(),originYEdit->text().toInt());
            SingletonAxis::GetInstance()->originPos(WRITE_OPERAT,savePos);
        });

    }

    // 第2行：弃杯孔坐标
    QLabel *cupLabel = new QLabel("弃杯孔坐标：", this);
    cupLabel->setStyleSheet("font-size: 14px;");
    QLineEdit *cupXEdit = new QLineEdit(this);
    cupXEdit->setPlaceholderText("X坐标");
    cupXEdit->setFixedWidth(80);
    cupXEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QLineEdit *cupYEdit = new QLineEdit(this);
    cupYEdit->setPlaceholderText("Y坐标");
    cupYEdit->setFixedWidth(80);
    cupYEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QPushButton *cupValidateBtn = new QPushButton("校验", this);
    cupValidateBtn->setFixedWidth(60);
    QPushButton *cupSaveBtn = new QPushButton("保存", this);
    cupSaveBtn->setFixedWidth(60);

    gridLayout->addWidget(cupLabel, 2, 0);
    gridLayout->addWidget(cupXEdit, 2, 1);
    gridLayout->addWidget(cupYEdit, 2, 2);
    gridLayout->addWidget(cupValidateBtn, 2, 3);
    gridLayout->addWidget(cupSaveBtn, 2, 4);

    //载入弃杯坐标
    {
        QPoint throwTubepos(0,0);
        SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT,throwTubepos);
        cupXEdit->setText(QString::number(throwTubepos.x()));
        cupYEdit->setText(QString::number(throwTubepos.y()));

        connect(cupValidateBtn,&QPushButton::clicked,[this, cupXEdit, cupYEdit](){
            // 校验弃杯孔坐标逻辑
            QLOG_DEBUG() << "校验弃杯孔坐标 X:" << cupXEdit->text() << " Y:" << cupYEdit->text();
        });

        connect(cupSaveBtn,&QPushButton::clicked,[this, cupXEdit, cupYEdit](){
            QLOG_DEBUG() << "保存弃杯孔坐标 X:" << cupXEdit->text();
            QPoint savePos(cupXEdit->text().toInt(), cupYEdit->text().toInt());
            SingletonAxis::GetInstance()->throwTubeHolePos(WRITE_OPERAT, savePos);
        });
    }


    // 第3行：清洗位off样本针
    QLabel *bloodLabel = new QLabel("清洗位off样本针：", this);
    bloodLabel->setStyleSheet("font-size: 14px;");
    QLineEdit *bloodXEdit = new QLineEdit(this);
    bloodXEdit->setPlaceholderText("X坐标");
    bloodXEdit->setFixedWidth(80);
    bloodXEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QLineEdit *bloodYEdit = new QLineEdit(this);
    bloodYEdit->setPlaceholderText("Y坐标");
    bloodYEdit->setFixedWidth(80);
    bloodYEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QPushButton *bloodValidateBtn = new QPushButton("校验", this);
    bloodValidateBtn->setFixedWidth(60);
    QPushButton *bloodSaveBtn = new QPushButton("保存", this);
    bloodSaveBtn->setFixedWidth(60);

    gridLayout->addWidget(bloodLabel, 3, 0);
    gridLayout->addWidget(bloodXEdit, 3, 1);
    gridLayout->addWidget(bloodYEdit, 3, 2);
    gridLayout->addWidget(bloodValidateBtn, 3, 3);
    gridLayout->addWidget(bloodSaveBtn, 3, 4);

    //载入清洗位off样本针坐标
    {
        QPoint loadpos(0,0);
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX,loadpos);
        bloodXEdit->setText(QString::number(loadpos.x()));
        bloodYEdit->setText(QString::number(loadpos.y()));

        connect(bloodValidateBtn,&QPushButton::clicked,[this, bloodXEdit, bloodYEdit](){
            // 校验清洗位off样本针坐标逻辑
            QLOG_DEBUG() << "校验清洗位off样本针坐标 X:" << bloodXEdit->text() << " Y:" << bloodYEdit->text();
        });

        connect(bloodSaveBtn,&QPushButton::clicked,[this, bloodXEdit, bloodYEdit](){
            QLOG_DEBUG() << "保存清洗位off样本针坐标 X:" << bloodXEdit->text();
            QPoint savePos(bloodXEdit->text().toInt(), bloodYEdit->text().toInt());
            SingletonAxis::GetInstance()->cleanZoneAxisPos(WRITE_OPERAT, MOTOR_BLOOD_INDEX, savePos);
        });
    }


    // 第4行：清洗位off试剂针
    QLabel *reagentLabel = new QLabel("清洗位off试剂针：", this);
    reagentLabel->setStyleSheet("font-size: 14px;");
    QLineEdit *reagentXEdit = new QLineEdit(this);
    reagentXEdit->setPlaceholderText("X坐标");
    reagentXEdit->setFixedWidth(80);
    reagentXEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QLineEdit *reagentYEdit = new QLineEdit(this);
    reagentYEdit->setPlaceholderText("Y坐标");
    reagentYEdit->setFixedWidth(80);
    reagentYEdit->setValidator(new QIntValidator(this)); // 只能输入整数
    QPushButton *reagentValidateBtn = new QPushButton("校验", this);
    reagentValidateBtn->setFixedWidth(60);
    QPushButton *reagentSaveBtn = new QPushButton("保存", this);
    reagentSaveBtn->setFixedWidth(60);

    //载入清洗位off试剂针坐标
    {
        QPoint loadpos(0,0);
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX,loadpos);
        reagentXEdit->setText(QString::number(loadpos.x()));
        reagentYEdit->setText(QString::number(loadpos.y()));

        connect(reagentValidateBtn,&QPushButton::clicked,[this, reagentXEdit, reagentYEdit](){
            // 校验清洗位off试剂针坐标逻辑
            QLOG_DEBUG() << "校验清洗位off试剂针坐标 X:" << reagentXEdit->text() << " Y:" << reagentYEdit->text();
        });

        connect(reagentSaveBtn,&QPushButton::clicked,[this, reagentXEdit, reagentYEdit](){
            QLOG_DEBUG() << "保存清洗位off试剂针坐标 X:" << reagentXEdit->text();
            QPoint savePos(reagentXEdit->text().toInt(), reagentYEdit->text().toInt());
            SingletonAxis::GetInstance()->cleanZoneAxisPos(WRITE_OPERAT, MOTOR_REAGNET_INDEX, savePos);
        });
    }


    gridLayout->addWidget(reagentLabel, 4, 0);
    gridLayout->addWidget(reagentXEdit, 4, 1);
    gridLayout->addWidget(reagentYEdit, 4, 2);
    gridLayout->addWidget(reagentValidateBtn, 4, 3);
    gridLayout->addWidget(reagentSaveBtn, 4, 4);

    layout->addWidget(coordGroup);
    layout->addStretch();

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(scrollArea);

    return widget;
}

QWidget* CoordinatepPosit::createChannelTab()
{
    QWidget *widget = new QWidget();
    QScrollArea *scrollArea = new QScrollArea(widget);
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // 通道选择区域 - 水平布局（12个通道一行显示）
    QGroupBox *channelGroup = new QGroupBox("通道选择");
    QHBoxLayout *channelLayout = new QHBoxLayout(channelGroup);
    channelLayout->setSpacing(15);
    channelLayout->setAlignment(Qt::AlignCenter);

    // 存储所有环形圆控件的指针，用于互斥控制
    QVector<CircleWidget*> allCircleWidgets;

    // 创建12个通道（水平布局）
    for (int i = 0; i < 12; i++) {
        int channelNumber = i + 1;
        CircleWidget *circleWidget = new CircleWidget(channelNumber);
        allCircleWidgets.append(circleWidget);
        channelLayout->addWidget(circleWidget);
    }

    // 为所有通道连接点击信号（使用统一的互斥处理）
    for (int i = 0; i < 12; i++) {
        CircleWidget *circleWidget = allCircleWidgets[i];
        int channelNumber = i + 1;

        connect(circleWidget, &CircleWidget::clicked, this, [this, allCircleWidgets, channelNumber]() {
            // 互斥点击处理：只有一个通道可以被激活
            for (CircleWidget *widget : allCircleWidgets) {
                if (widget->channelNumber() == channelNumber) {
                    widget->setActive(true);
                }
                else {
                    widget->setActive(false);
                }
            }
            onCircleClicked(channelNumber);
        });
    }

    layout->addWidget(channelGroup);

    // 通道坐标设置区域 - 根据激活的通道显示对应坐标
    QGroupBox *coordGroup = new QGroupBox("通道坐标设置");
    QGridLayout *coordLayout = new QGridLayout(coordGroup);
    coordLayout->setHorizontalSpacing(15);
    coordLayout->setVerticalSpacing(10);

    // 表头
    QLabel *headerChannel = new QLabel("通道");
    QLabel *headerX = new QLabel("X坐标");
    QLabel *headerY = new QLabel("Y坐标");
    QLabel *headerSave = new QLabel("保存");

    headerChannel->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerX->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerY->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");
    headerSave->setStyleSheet("font-weight: bold; color: #2C5F8A; font-size: 14px;");

    coordLayout->addWidget(headerChannel, 0, 0);
    coordLayout->addWidget(headerX, 0, 1);
    coordLayout->addWidget(headerY, 0, 2);
    coordLayout->addWidget(headerSave, 0, 3);

    // 创建12行通道坐标设置（显示所有通道）
    for (int i = 0; i < 12; i++) {
        QLabel *channelLabel = new QLabel(QString("通道 %1").arg(i + 1));
        channelLabel->setStyleSheet("font-size: 14px;");

        QLineEdit *xEdit = new QLineEdit();
        xEdit->setPlaceholderText("X坐标");
        xEdit->setFixedWidth(80);

        QLineEdit *yEdit = new QLineEdit();
        yEdit->setPlaceholderText("Y坐标");
        yEdit->setFixedWidth(80);

        QPushButton *saveBtn = new QPushButton("保存");
        saveBtn->setFixedWidth(60);

        coordLayout->addWidget(channelLabel, i + 1, 0);
        coordLayout->addWidget(xEdit, i + 1, 1);
        coordLayout->addWidget(yEdit, i + 1, 2);
        coordLayout->addWidget(saveBtn, i + 1, 3);
    }

    layout->addWidget(coordGroup);
    layout->addStretch();

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(scrollArea);

    return widget;
}

QWidget* CoordinatepPosit::createTubeTab()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 子QTabWidget显示4个试管盘
    QTabWidget *tubeTabWidget = new QTabWidget(widget);
    tubeTabWidget->setStyleSheet(
        "QTabWidget::pane { border: 1px solid #B8DFF7; background-color: #F0F8FF; }"
        "QTabBar::tab { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #E8F4FD, stop:1 #D4EAF5); color: #2C5F8A; font-size: 13px; font-weight: 600; padding: 8px 16px; margin-right: 2px; min-width: 100px; border-top-left-radius: 4px; border-top-right-radius: 4px; border: 1px solid #B8DFF7; border-bottom: none; }"
        "QTabBar::tab:selected { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #5BB8E8, stop:1 #3A9BD4); color: white; border: 1px solid #3A9BD4; border-bottom: none; }"
        "QTabBar::tab:hover:!selected { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #C5E8FA, stop:1 #A8DCF5); color: #2C5F8A; }"
        );

    // 创建4个试管盘的标签页
    for (int plate = 1; plate <= 4; plate++) {
        QWidget *plateWidget = createTubePlateTab(plate);
        tubeTabWidget->addTab(plateWidget, QString("试管盘 %1").arg(plate));
    }

    mainLayout->addWidget(tubeTabWidget);

    return widget;
}

QWidget* CoordinatepPosit::createTubePlateTab(int plateNumber)
{
    QWidget *widget = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(15);

    // 左侧：试管盘绘制
    QGroupBox *plateGroup = new QGroupBox(QString("试管盘 %1").arg(plateNumber));
    QVBoxLayout *plateLayout = new QVBoxLayout(plateGroup);

    QGridLayout *gridLayout = new QGridLayout();
    gridLayout->setHorizontalSpacing(5);
    gridLayout->setVerticalSpacing(5);

    QVector<CircleWidget*> plateCircles;

    // 创建6列10行的圆形环 - 显示连续编号1-240
    for (int row = 0; row < 10; row++) {
        for (int col = 0; col < 6; col++) {
            int holeNumber = (plateNumber - 1) * 60 + row * 6 + col + 1;
            CircleWidget *circleWidget = new CircleWidget(holeNumber);
            plateCircles.append(circleWidget);
            gridLayout->addWidget(circleWidget, row, col);
        }
    }

    plateLayout->addLayout(gridLayout);
    plateLayout->addStretch();

    // 为当前试管盘的圆形环添加互斥点击功能
    for (int i = 0; i < 60; i++) {
        CircleWidget *circleWidget = plateCircles[i];
        int holeNum = i + 1;

        connect(circleWidget, &CircleWidget::clicked, this, [this, plateCircles, plateNumber, holeNum]() {
            // 互斥点击处理：当前试管盘内只有一个孔可以被激活
            for (int h = 0; h < 60; h++) {
                CircleWidget *widget = plateCircles[h];
                if (h + 1 == holeNum) {
                    widget->setActive(true);
                }
                else {
                    widget->setActive(false);
                }
            }
            onHoleClicked(plateNumber, holeNum);
        });
    }

    layout->addWidget(plateGroup);

    // 右侧：坐标表格
    QGroupBox *coordGroup = new QGroupBox("试管坐标设置");
    QVBoxLayout *coordLayout = new QVBoxLayout(coordGroup);

    QTableWidget *coordTable = new QTableWidget(60, 4); // 当前试管盘的60个试管（增加轴选择列）
    coordTable->setHorizontalHeaderLabels({"试管编号", "轴选择", "X坐标", "Y坐标"});
    coordTable->horizontalHeader()->setStyleSheet("QHeaderView::section { background-color: #2C5F8A; color: white; font-weight: bold; font-size: 14px; padding: 8px; border: none; }");
    coordTable->setStyleSheet(
        "QTableWidget { "
        "    font-size: 14px; "
        "    background-color: white; "
        "    border: 1px solid #B8DFF7; "
        "    border-radius: 6px; "
        "    gridline-color: #D4EAF5; "
        "} "
        "QTableWidget::item { "
        "    padding: 6px; "
        "    border-bottom: 1px solid #E8F4FD; "
        "} "
        "QTableWidget::item:selected { "
        "    background-color: #5BB8E8; "
        "    color: white; "
        "} "
        "QTableWidget::item:!selected:hover { "
        "    background-color: #F0F8FF; "
        "}"
    );
    coordTable->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    // 设置列宽度
    coordTable->setColumnWidth(0, 100); // 试管编号列
    coordTable->setColumnWidth(1, 120); // 轴选择列
    coordTable->setColumnWidth(2, 120); // X坐标列
    coordTable->setColumnWidth(3, 120); // Y坐标列

                                        // 填充表格数据 - 试管编号从1到240
    for (int hole = 0; hole < 60; hole++) {
        int tubeNumber = (plateNumber - 1) * 60 + hole + 1; // 试管编号从1到240

        // 试管编号列
        QTableWidgetItem *tubeItem = new QTableWidgetItem(QString("试管 %1").arg(tubeNumber));
        coordTable->setItem(hole, 0, tubeItem);

        // 轴选择列 - 只在索引0行显示
        if (hole == 0) {
            // 创建组合框用于轴选择
            QComboBox *axisCombo = new QComboBox();
            axisCombo->addItems({"抓手", "样本针"});
            axisCombo->setStyleSheet("QComboBox { font-size: 14px; border: 1px solid #B8DFF7; border-radius: 4px; padding: 4px; }");
            coordTable->setCellWidget(hole, 1, axisCombo);
        } else {
            // 其他行显示空内容
            QTableWidgetItem *axisItem = new QTableWidgetItem("");
            axisItem->setFlags(axisItem->flags() & ~Qt::ItemIsEditable);
            coordTable->setItem(hole, 1, axisItem);
        }

        // X坐标列 - 只有索引0的行可以编辑
        QTableWidgetItem *xItem = new QTableWidgetItem();
        if (hole == 0) {
            xItem->setFlags(xItem->flags() | Qt::ItemIsEditable);
            // 设置初始值
            if (m_tubeCoordinates[tubeNumber - 1].x() != 0) {
                xItem->setText(QString::number(m_tubeCoordinates[tubeNumber - 1].x()));
            }
        }
        else {
            xItem->setFlags(xItem->flags() & ~Qt::ItemIsEditable);
            // 根据索引0的值进行偏移计算
            if (m_tubeCoordinates[tubeNumber - 1].x() != 0) {
                xItem->setText(QString::number(m_tubeCoordinates[tubeNumber - 1].x(), 'f', 2));
            }
            else {
                xItem->setText("自动计算");
            }
        }
        coordTable->setItem(hole, 2, xItem);

        // Y坐标列 - 只有索引0的行可以编辑
        QTableWidgetItem *yItem = new QTableWidgetItem();
        if (hole == 0) {
            yItem->setFlags(yItem->flags() | Qt::ItemIsEditable);
            // 设置初始值
            if (m_tubeCoordinates[tubeNumber - 1].y() != 0) {
                yItem->setText(QString::number(m_tubeCoordinates[tubeNumber - 1].y()));
            }
        }
        else {
            yItem->setFlags(yItem->flags() & ~Qt::ItemIsEditable);
            // 根据索引0的值进行偏移计算
            if (m_tubeCoordinates[tubeNumber - 1].y() != 0) {
                yItem->setText(QString::number(m_tubeCoordinates[tubeNumber - 1].y(), 'f', 2));
            }
            else {
                yItem->setText("自动计算");
            }
        }
        coordTable->setItem(hole, 3, yItem);
    }

    // 连接单元格变化信号
    connect(coordTable, &QTableWidget::cellChanged, this, [this, plateNumber, coordTable](int row, int column) {
        onTubeCoordinateChanged(plateNumber, row, column, coordTable);
    });

    coordLayout->addWidget(coordTable);

    // 保存按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setAlignment(Qt::AlignRight);

    QPushButton *saveButton = new QPushButton("保存坐标");
    saveButton->setFixedSize(100, 35);
    saveButton->setStyleSheet("QPushButton { background-color: #5BB8E8; border: none; border-radius: 6px; color: white; padding: 8px 16px; font-weight: bold; font-size: 14px; } QPushButton:hover { background-color: #3A9BD4; } QPushButton:pressed { background-color: #2C7FAC; }");

    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);

    coordLayout->addLayout(buttonLayout);
    layout->addWidget(coordGroup);

    return widget;
}

QWidget* CoordinatepPosit::createReagentTab()
{
    QWidget *widget = new QWidget();
    QScrollArea *scrollArea = new QScrollArea(widget);
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // 试剂区内容
    QGroupBox *reagentGroup = new QGroupBox("试剂管理");
    QFormLayout *formLayout = new QFormLayout(reagentGroup);

    QComboBox *reagentType = new QComboBox();
    reagentType->addItems({ "抗原检测试剂", "抗体检测试剂", "缓冲液", "染色液" });
    formLayout->addRow("试剂类型:", reagentType);

    QLineEdit *batchEdit = new QLineEdit();
    batchEdit->setPlaceholderText("批号");
    formLayout->addRow("批号:", batchEdit);

    QLineEdit *expiryEdit = new QLineEdit();
    expiryEdit->setPlaceholderText("有效期");
    formLayout->addRow("有效期:", expiryEdit);

    QPushButton *checkBtn = new QPushButton("检查库存");
    formLayout->addRow("", checkBtn);

    layout->addWidget(reagentGroup);
    layout->addStretch();

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(scrollArea);

    return widget;
}

QWidget* CoordinatepPosit::createBloodTab()
{
    QWidget *widget = new QWidget();
    QScrollArea *scrollArea = new QScrollArea(widget);
    QWidget *contentWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(contentWidget);

    // 血样区内容
    QGroupBox *bloodGroup = new QGroupBox("血样信息");
    QFormLayout *formLayout = new QFormLayout(bloodGroup);

    QLineEdit *patientIdEdit = new QLineEdit();
    patientIdEdit->setPlaceholderText("患者ID");
    formLayout->addRow("患者ID:", patientIdEdit);

    QLineEdit *patientNameEdit = new QLineEdit();
    patientNameEdit->setPlaceholderText("患者姓名");
    formLayout->addRow("姓名:", patientNameEdit);

    QComboBox *bloodType = new QComboBox();
    bloodType->addItems({ "A型", "B型", "O型", "AB型" });
    formLayout->addRow("血型:", bloodType);

    QLineEdit *sampleIdEdit = new QLineEdit();
    sampleIdEdit->setPlaceholderText("样本编号");
    formLayout->addRow("样本编号:", sampleIdEdit);

    QPushButton *registerBtn = new QPushButton("登记血样");
    QPushButton *printBtn = new QPushButton("打印标签");
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(registerBtn);
    btnLayout->addWidget(printBtn);
    formLayout->addRow("操作:", btnLayout);

    layout->addWidget(bloodGroup);
    layout->addStretch();

    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->addWidget(scrollArea);

    return widget;
}

void CoordinatepPosit::applyMedicalStyle()
{
    // 医疗浅蓝风格的 TabWidget 样式
    QString tabStyle = R"(
        QTabWidget::pane {
            border: 1px solid #B8DFF7;
            border-radius: 8px;
            background-color: #F8FBFF;
            top: -1px;
        }

                QTabWidget::tab-bar {
            alignment: center;
            background-color: #F0F8FF;
            border-bottom: 1px solid #B8DFF7;
        }

                QTabBar::tab {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                        stop: 0 #E8F4FD, stop: 1 #D4EAF5);
            color: #2C5F8A;
            font-size: 15px;
            font-weight: 400;
            padding: 8px 15px;
            margin-right: 2px;
            min-width: 120px;
            border-top-left-radius: 4px;
            border-top-right-radius: 4px;
            border: 1px solid #B8DFF7;
            border-bottom: none;
        }

                QTabBar::tab:selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                        stop: 0 #5BB8E8, stop: 1 #3A9BD4);
            color: white;
            border: 1px solid #3A9BD4;
            border-bottom: none;
        }

                QTabBar::tab:hover:!selected {
            background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,
                                        stop: 0 #C5E8FA, stop: 1 #A8DCF5);
            color: #2C5F8A;
        }

                QTabBar::tab:selected {
            margin-bottom: -1px;
        }

                /* 滚动区域样式 */
        QScrollArea {
            border: none;
            background-color: transparent;
        }

                QScrollBar:vertical {
            border: none;
            background: #F0F8FF;
            width: 10px;
            border-radius: 5px;
        }

                QScrollBar::handle:vertical {
            background: #7EC8F0;
            border-radius: 5px;
            min-height: 20px;
        }

                QScrollBar::handle:vertical:hover {
            background: #5BB8E8;
        }

                QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            border: none;
            background: none;
        }

                /* GroupBox 样式更新为医疗蓝 */
        QGroupBox {
            font-weight: bold;
            font-size: 15px;
            border: 2px solid #B8DFF7;
            border-radius: 10px;
            margin-top: 12px;
            padding-top: 10px;
            background-color: white;
        }

                QGroupBox::title {
            subcontrol-origin: margin;
            subcontrol-position: top center;
            padding: 0 12px;
            background-color: #5BB8E8;
            color: white;
            border-radius: 6px;
            font-size: 14px;
        }

                /* 输入框聚焦样式 */
        QLineEdit, QComboBox {
            border: 1px solid #D4EAF5;
            border-radius: 4px;
            padding: 8px;
            background-color: white;
            font-size: 14px;
        }

                QLineEdit:focus, QComboBox:focus {
            border: 2px solid #5BB8E8;
            background-color: #F8FBFF;
        }

                /* 按钮样式更新 */
        QPushButton {
            background-color: #5BB8E8;
            border: none;
            border-radius: 6px;
            color: white;
            padding: 8px 16px;
            font-weight: bold;
            font-size: 14px;
        }

                QPushButton:hover {
            background-color: #3A9BD4;
        }

                QPushButton:pressed {
            background-color: #2C7FAC;
        }
    )";

    m_tabWidget->setStyleSheet(tabStyle);
}

void CoordinatepPosit::onCloseRequested()
{
    close();
}

void CoordinatepPosit::onMinimizeRequested()
{
    showMinimized();
}

void CoordinatepPosit::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(false);
        }
    }
    else {
        showMaximized();
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(true);
        }
    }
}

void CoordinatepPosit::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    // 移除了对 ui->widgetFucn 的引用
}


void CoordinatepPosit::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::ActivationChange) {
        if (m_titleBar) {
            m_titleBar->setActive(isActiveWindow());
        }
    }
}



/** 通道区
* @brief CoordinatepPosit::onCircleClicked
* @param channelNumber
*/
void CoordinatepPosit::onCircleClicked(int channelNumber)
{
    // 处理环形圆点击事件
    QLOG_DEBUG() << "通道" << channelNumber << "被点击";

    // 这里可以添加更复杂的逻辑，比如切换通道状态、显示详细信息等
    // 示例：显示通道信息对话框或更新界面状态
}

/** 使馆盘区
* @brief CoordinatepPosit::onHoleClicked
* @param plateNumber
* @param holeNumber
*/
void CoordinatepPosit::onHoleClicked(int plateNumber, int holeNumber)
{
    // 处理使馆盘孔位点击事件
    QLOG_DEBUG() << "试馆盘" << plateNumber << "孔位" << holeNumber << "被点击";


}

/** 试管坐标变化处理
 * @brief CoordinatepPosit::onTubeCoordinateChanged
 * @param plateNumber
 * @param row
 * @param column
 * @param table
 */
void CoordinatepPosit::onTubeCoordinateChanged(int plateNumber, int row, int column, QTableWidget* table)
{
    // 只处理X坐标和Y坐标列的变化（列索引现在是2和3）
    if (column != 2 && column != 3) return;

    // 只处理索引0的行的编辑
    if (row != 0) return;

    int tubeNumber = (plateNumber - 1) * 60 + row + 1; // 当前编辑的试管编号
    QTableWidgetItem *item = table->item(row, column);
    if (!item) return;

    QString text = item->text();
    bool ok;
    double value = text.toDouble(&ok);

    if (!ok) {
        // 如果不是有效数字，恢复原值
        if (column == 2) {
            item->setText(QString::number(m_tubeCoordinates[tubeNumber - 1].x()));
        } else {
            item->setText(QString::number(m_tubeCoordinates[tubeNumber - 1].y()));
        }
        return;
    }

    // 更新坐标数据
    if (column == 2) {
        m_tubeCoordinates[tubeNumber - 1].setX(value);
    } else {
        m_tubeCoordinates[tubeNumber - 1].setY(value);
    }

    // 计算并更新其他试管的坐标（基于偏移）
    updateTubeCoordinates(plateNumber, table);
}

/** 更新试管坐标（基于偏移计算）
* @brief CoordinatepPosit::updateTubeCoordinates
* @param plateNumber
* @param table
*/
void CoordinatepPosit::updateTubeCoordinates(int plateNumber, QTableWidget* table)
{
    // 获取第一个试管的坐标作为基准
    int baseTubeNumber = (plateNumber - 1) * 60;
    double baseX = m_tubeCoordinates[baseTubeNumber].x();
    double baseY = m_tubeCoordinates[baseTubeNumber].y();

    // 假设每个试管之间的间距为固定值（可以根据需要调整）
    double xSpacing = 10.0; // X方向间距
    double ySpacing = 10.0; // Y方向间距

                            // 更新当前试管盘的所有试管坐标
    for (int hole = 1; hole < 60; hole++) {
        int tubeNumber = baseTubeNumber + hole;

        // 计算行列位置
        int row = hole / 6;
        int col = hole % 6;

        // 计算偏移坐标
        double x = baseX + col * xSpacing;
        double y = baseY + row * ySpacing;

        // 更新坐标数据
        m_tubeCoordinates[tubeNumber].setX(x);
        m_tubeCoordinates[tubeNumber].setY(y);

        // 更新表格显示（列索引现在是2和3）
        QTableWidgetItem *xItem = table->item(hole, 2);
        QTableWidgetItem *yItem = table->item(hole, 3);

        if (xItem) xItem->setText(QString::number(x, 'f', 2));
        if (yItem) yItem->setText(QString::number(y, 'f', 2));
    }
}
