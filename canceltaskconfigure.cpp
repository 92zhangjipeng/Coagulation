#include "canceltaskconfigure.h"
#include "globaldata.h"
#include "ui_canceltaskconfigure.h"
#include <QHBoxLayout>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// 医疗淡蓝色主题色系
namespace MedicalColors {
    const QString PRIMARY_LIGHT = "#E8F4FD";
    const QString PRIMARY_SOFT = "#B8DFF7";
    const QString PRIMARY_MEDIUM = "#7EC8F0";
    const QString PRIMARY_ACTIVE = "#5BB8E8";
    const QString PRIMARY_DARK = "#3A9BD4";
    const QString TEXT_DARK = "#2C5F8A";
    const QString TEXT_LIGHT = "#4A7FA3";
    const QString DIVIDER = "#D4EAF5";
    const QString WHITE = "#FFFFFF";
    const QString TABLE_HEADER_BG = "#ECEFF1";
    const QString TABLE_ROW_HOVER = "#E3F2FD";
    const QString TABLE_ROW_SELECT = "#BBDEFB";
}

CancelTaskConfigure::CancelTaskConfigure(QWidget *parent, QList<int> click_sampleid) :
    QDialog(parent),
    ui(new Ui::CancelTaskConfigure),
    m_titleBar(nullptr),
    m_mainLayout(nullptr),
    m_centerWidget(nullptr),
    m_centerLayout(nullptr),
    m_tableWidget(nullptr),
    m_cancelBtn(nullptr),
    m_okBtn(nullptr),
    mclickHole(click_sampleid),
    m_isMaximized(false)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    // 隐藏默认标题栏
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    deleteitem.clear();

    // 初始化UI
    initUI();
    initStyle();
    initTable();
}

CancelTaskConfigure::~CancelTaskConfigure()
{
    delete ui;
}

void CancelTaskConfigure::initUI()
{
    // 设置窗口大小
    setFixedSize(400, 450);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建标题栏
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle(tr("取消样本任务"));
    m_mainLayout->addWidget(m_titleBar);

    // 创建内容区域容器
    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName("centerWidget");
    m_centerLayout = new QVBoxLayout(m_centerWidget);
    m_centerLayout->setContentsMargins(10, 10, 10, 10);
    m_centerLayout->setSpacing(10);

    // 创建表格
    m_tableWidget = new QTableWidget(m_centerWidget);
    m_tableWidget->setObjectName("taskTable");

    // 底部按钮区域
    QWidget *buttonWidget = new QWidget(m_centerWidget);
    buttonWidget->setObjectName("buttonWidget");
    buttonWidget->setFixedHeight(50);

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setContentsMargins(10, 5, 10, 5);
    buttonLayout->setSpacing(20);

    m_cancelBtn = new QPushButton(tr("取消&&退出"), buttonWidget);
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedSize(120, 34);
    m_cancelBtn->setCursor(Qt::PointingHandCursor);

    m_okBtn = new QPushButton(tr("确定&&退出"), buttonWidget);
    m_okBtn->setObjectName("okBtn");
    m_okBtn->setFixedSize(120, 34);
    m_okBtn->setCursor(Qt::PointingHandCursor);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelBtn);
    buttonLayout->addWidget(m_okBtn);
    buttonLayout->addStretch();

    // 添加到布局
    m_centerLayout->addWidget(m_tableWidget, 1);
    m_centerLayout->addWidget(buttonWidget);

    m_mainLayout->addWidget(m_centerWidget);

    // 连接信号
    if (m_titleBar) {
        connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &CancelTaskConfigure::onMinimizeRequested);
        connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &CancelTaskConfigure::onMaximizeRequested);
        connect(m_titleBar, &CustomTitleBar::closeRequested, this, &CancelTaskConfigure::onCloseRequested);
    }

    connect(m_cancelBtn, &QPushButton::clicked, this, &CancelTaskConfigure::onCancelClicked);
    connect(m_okBtn, &QPushButton::clicked, this, &CancelTaskConfigure::onOkClicked);
    connect(m_tableWidget, &QTableWidget::itemClicked, this, &CancelTaskConfigure::delenter);
}

void CancelTaskConfigure::initStyle()
{
    QString styleSheet = QString(R"(
        CancelTaskConfigure {
            background-color: %1;
            border-radius: 8px;
        }

        QWidget#centerWidget {
            background-color: %2;
            border-radius: 0 0 8px 8px;
        }

        QWidget#buttonWidget {
            background-color: %2;
            border-radius: 0 0 8px 8px;
        }

        QTableWidget#taskTable {
            background-color: %2;
            border: 1px solid %3;
            border-radius: 6px;
            gridline-color: %3;
            font-family: 'Segoe UI', 'Microsoft YaHei';
            font-size: 13px;
            outline: 0px;
        }

        QTableWidget#taskTable::item {
            padding: 8px;
            border-bottom: 1px solid %3;
        }

        QTableWidget#taskTable::item:hover {
            background-color: %4;
        }

        QTableWidget#taskTable::item:selected {
            background-color: %5;
            color: %6;
        }

        QHeaderView::section {
            background-color: %7;
            color: %6;
            font-weight: bold;
            font-size: 13px;
            padding: 8px;
            border: none;
            border-bottom: 1px solid %3;
            font-family: 'Segoe UI', 'Microsoft YaHei';
        }

        QPushButton#cancelBtn, QPushButton#okBtn {
            background-color: %8;
            color: white;
            border: none;
            border-radius: 17px;
            font-size: 13px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
        }

        QPushButton#cancelBtn:hover, QPushButton#okBtn:hover {
            background-color: %9;
        }

        QPushButton#cancelBtn:pressed, QPushButton#okBtn:pressed {
            background-color: %10;
        }

        QScrollBar:vertical {
            background: %2;
            width: 10px;
            margin: 0px;
        }

        QScrollBar::handle:vertical {
            background: %3;
            min-height: 20px;
            border-radius: 5px;
        }

        QScrollBar::handle:vertical:hover {
            background: %8;
        }

        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }

        QScrollBar:horizontal {
            background: %2;
            height: 10px;
            margin: 0px;
        }

        QScrollBar::handle:horizontal {
            background: %3;
            min-width: 20px;
            border-radius: 5px;
        }

        QScrollBar::handle:horizontal:hover {
            background: %8;
        }

        QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
            width: 0px;
        }
    )").arg(MedicalColors::PRIMARY_LIGHT)      // 主背景
      .arg(MedicalColors::WHITE)                // 内容区域背景
      .arg(MedicalColors::DIVIDER)              // 边框颜色
      .arg(MedicalColors::TABLE_ROW_HOVER)      // 行悬停颜色
      .arg(MedicalColors::TABLE_ROW_SELECT)     // 行选中颜色
      .arg(MedicalColors::TEXT_DARK)            // 文字颜色
      .arg(MedicalColors::TABLE_HEADER_BG)      // 表头背景
      .arg(MedicalColors::PRIMARY_ACTIVE)       // 按钮背景
      .arg(MedicalColors::PRIMARY_MEDIUM)       // 按钮悬停
      .arg(MedicalColors::PRIMARY_DARK);        // 按钮按下

    this->setStyleSheet(styleSheet);
}

void CancelTaskConfigure::initTable()
{
    // 设置表格列
    m_tableWidget->setColumnCount(2);
    QStringList header{tr("样本号"), tr("删除")};
    m_tableWidget->setHorizontalHeaderLabels(header);

    // 表格基本设置
    m_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);
    m_tableWidget->horizontalHeader()->setFixedHeight(40);
    m_tableWidget->verticalHeader()->setDefaultSectionSize(40);

    // 设置列宽
    m_tableWidget->setColumnWidth(0, 150);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    // 加载删除图标
    QPixmap litterPix(":/Picture/SetPng/Litter.png");
    if (litterPix.isNull()) {
        // 如果图标不存在，创建一个简单的叉号图标
        litterPix = QPixmap(24, 24);
        litterPix.fill(Qt::transparent);
        QPainter painter(&litterPix);
        painter.setPen(QPen(Qt::red, 2));
        painter.drawLine(4, 4, 20, 20);
        painter.drawLine(20, 4, 4, 20);
    }
    QIcon deleteIcon(litterPix);

    // 填充数据
    for (int sampleId : mclickHole) {
        int iRow = m_tableWidget->rowCount();
        m_tableWidget->insertRow(iRow);

        // 样本号列
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(sampleId));
        item->setTextAlignment(Qt::AlignCenter);
        m_tableWidget->setItem(iRow, 0, item);

        // 删除列 - 使用按钮而不是图标
        QPushButton *deleteBtn = new QPushButton();
        deleteBtn->setIcon(deleteIcon);
        deleteBtn->setIconSize(QSize(24, 24));
        deleteBtn->setFixedSize(32, 32);
        deleteBtn->setCursor(Qt::PointingHandCursor);
        deleteBtn->setProperty("sampleId", sampleId);
        deleteBtn->setStyleSheet(
            "QPushButton {"
            "   background-color: transparent;"
            "   border: none;"
            "   border-radius: 16px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #FFCDD2;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #EF9A9A;"
            "}"
        );

        // 连接删除按钮信号
        connect(deleteBtn, &QPushButton::clicked, this, [this, deleteBtn, iRow]() {
            int sampleId = deleteBtn->property("sampleId").toInt();
            deleteitem.append(sampleId);
            m_tableWidget->removeRow(iRow);
        });

        m_tableWidget->setCellWidget(iRow, 1, deleteBtn);
    }
}

void CancelTaskConfigure::SetColumnText(QTableWidget *tablewiget, int row, int col, QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);
    tablewiget->setItem(row, col, item);
}

void CancelTaskConfigure::delenter(QTableWidgetItem *item)
{
    if (item && item->column() == 0) {
        QString delstr = item->text();
        if (!delstr.isEmpty()) {
            deleteitem.append(delstr.toInt());
            m_tableWidget->removeRow(item->row());
        }
    }
}

void CancelTaskConfigure::onOkClicked()
{
    emit makesureCacelIdList(deleteitem);
    close();
}

void CancelTaskConfigure::onCancelClicked()
{
    close();
}

void CancelTaskConfigure::onMinimizeRequested()
{
    showMinimized();
}

void CancelTaskConfigure::onMaximizeRequested()
{
    if (m_isMaximized) {
        setGeometry(m_normalGeometry);
        m_isMaximized = false;
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(false);
        }
    } else {
        m_normalGeometry = geometry();
        QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        setGeometry(availableGeometry);
        m_isMaximized = true;
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(true);
        }
    }
}

void CancelTaskConfigure::onCloseRequested()
{
    close();
}

void CancelTaskConfigure::closeEvent(QCloseEvent *event)
{
    event->accept();
}
