#include "displaylogstext.h"
#include <QApplication>
#include <QDateTime>
#include <QDesktopWidget>
#include <QDir>
#include <QElapsedTimer>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QQueue>
#include <QPalette>
#include <QPixmap>
#include "QsLog/include/QsLog.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// 医疗蓝主色调定义
#define MEDICAL_BLUE_PRIMARY    QColor(0, 112, 192)   // 主蓝色
#define MEDICAL_BLUE_LIGHT      QColor(51, 153, 255)  // 浅蓝色
#define MEDICAL_BLUE_DARK       QColor(0, 80, 150)    // 深蓝色
#define MEDICAL_BLUE_BG         QColor(235, 245, 255) // 背景蓝白
#define MEDICAL_GRAY_BORDER     QColor(200, 210, 220) // 边框灰
#define MEDICAL_WHITE           QColor(255, 255, 255) // 白色
#define MEDICAL_TEXT_DARK       QColor(30, 30, 60)    // 深色文字

displayLogsText::displayLogsText(QWidget *parent) :
    QWidget(parent),
    m_isMaximized(false)
{
    m_bgColor = MEDICAL_WHITE;

    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(500, 400);
    resize(550, 600);

    _initUI();
    _initStyle();
    _initConnections();

    QLOG_TRACE() << "日志查看器构造函数 - 医疗器械风格";

    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
}

displayLogsText::~displayLogsText()
{
    QLOG_DEBUG() << "析构日志查看对话框";
}

void displayLogsText::_initUI()
{
    // 创建控件
    m_logoLabel = new QLabel(this);
    m_logoLabel->setFixedSize(32, 32);
    m_logoLabel->setScaledContents(true);
    QPixmap pixmap(":/Picture/Login.png");
    if (!pixmap.isNull()) {
        m_logoLabel->setPixmap(pixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    m_titleLabel = new QLabel(this);
    m_titleLabel->setText(tr("系统日志查看器"));
    m_titleLabel->setFixedHeight(32);

    m_closeBtn = new QToolButton(this);
    m_closeBtn->setText("×");
    m_closeBtn->setFixedSize(32, 32);

    m_maximizeBtn = new QToolButton(this);
    m_maximizeBtn->setText("□");
    m_maximizeBtn->setFixedSize(32, 32);

    // 头部布局
    m_headerWidget = new QWidget(this);
    m_headerWidget->setFixedHeight(48);
    m_headerLayout = new QHBoxLayout(m_headerWidget);
    m_headerLayout->setContentsMargins(12, 8, 12, 8);
    m_headerLayout->setSpacing(10);
    m_headerLayout->addWidget(m_logoLabel);
    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addStretch();
    m_headerLayout->addWidget(m_maximizeBtn);
    m_headerLayout->addWidget(m_closeBtn);

    // 进度条
    m_progressBar = new QProgressBar(this);
    m_progressBar->setFixedHeight(8);
    m_progressBar->setTextVisible(false);

    m_progressWidget = new QWidget(this);
    m_progressWidget->setFixedHeight(20);
    m_progressLayout = new QHBoxLayout(m_progressWidget);
    m_progressLayout->setContentsMargins(0, 0, 0, 0);
    m_progressLayout->addWidget(m_progressBar);

    // 文本浏览区域
    m_textBrowser = new QTextBrowser(this);

    // 底部按钮
    m_updateBtn = new QToolButton(this);
    m_updateBtn->setText(tr("刷新日志"));
    m_updateBtn->setFixedSize(100, 36);

    m_clearBtn = new QToolButton(this);
    m_clearBtn->setText(tr("清空日志"));
    m_clearBtn->setFixedSize(100, 36);

    m_timeLabel = new QLabel(this);
    m_timeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_timeLabel->setFixedHeight(36);

    m_buttonWidget = new QWidget(this);
    m_buttonWidget->setFixedHeight(52);
    m_buttonLayout = new QHBoxLayout(m_buttonWidget);
    m_buttonLayout->setContentsMargins(12, 8, 12, 8);
    m_buttonLayout->setSpacing(15);
    m_buttonLayout->addWidget(m_updateBtn);
    m_buttonLayout->addStretch();
    m_buttonLayout->addWidget(m_timeLabel);
    m_buttonLayout->addWidget(m_clearBtn);

    // 内容区域布局
    m_contentWidget = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(15, 10, 15, 10);
    m_contentLayout->setSpacing(10);
    m_contentLayout->addWidget(m_progressWidget);
    m_contentLayout->addWidget(m_textBrowser, 1);
    m_contentLayout->addWidget(m_buttonWidget);

    // 主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_headerWidget);
    m_mainLayout->addWidget(m_contentWidget, 1);

    setLayout(m_mainLayout);
}

void displayLogsText::_initStyle()
{
    // 设置全局样式表 - 医疗器械风格
    QString styleSheet = QString(
        "QWidget {"
        "    background-color: %1;"
        "    font-family: 'Microsoft YaHei', 'Segoe UI', '楷体';"
        "}"

        /* 头部区域 */
        "QWidget#m_headerWidget {"
        "    background-color: rgba(255, 255, 255, 0.85);"
        "    border-bottom: 1px solid %3;"
        "}"

        /* 标题标签 */
        "QLabel {"
        "    color: %4;"
        "    font-size: 14px;"
        "    font-weight: normal;"
        "}"

        /* 标题文字特殊样式 */
        "QLabel#m_titleLabel {"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: %2;"

        "}"

        /* 按钮通用样式 */
        "QToolButton {"
        "    background-color: transparent;"
        "    border: none;"
        "    border-radius: 4px;"
        "    color: %4;"
        "    font-size: 18px;"
        "    font-weight: normal;"
        "}"

        "QToolButton:hover {"
        "    background-color: rgba(0, 112, 192, 0.1);"
        "}"

        "QToolButton:pressed {"
        "    background-color: rgba(0, 112, 192, 0.2);"
        "}"

        /* 关闭按钮悬停特殊样式 */
        "QToolButton#m_closeBtn:hover {"
        "    background-color: #E81123;"
        "    color: white;"
        "}"

        /* 功能按钮样式 */
        "QToolButton#m_updateBtn, QToolButton#m_clearBtn {"
        "    background-color: %2;"
        "    border-radius: 4px;"
        "    color: white;"
        "    font-size: 13px;"
        "    font-weight: normal;"
        "    padding: 6px 12px;"
        "}"

        "QToolButton#m_updateBtn:hover, QToolButton#m_clearBtn:hover {"
        "    background-color: %5;"
        "}"

        "QToolButton#m_updateBtn:pressed, QToolButton#m_clearBtn:pressed {"
        "    background-color: %6;"
        "}"

        /* 进度条样式 - 医疗器械风格 */
        "QProgressBar {"
        "    border: none;"
        "    background-color: %7;"
        "    border-radius: 4px;"
        "    text-align: center;"
        "}"

        "QProgressBar::chunk {"
        "    background-color: %2;"
        "    border-radius: 4px;"
        "}"

        /* 文本浏览区域 */
        "QTextBrowser {"
        "    background-color: %1;"
        "    border: 1px solid %3;"
        "    border-radius: 6px;"
        "    padding: 10px;"
        "    font-size: 12px;"
        "    font-family: 'Consolas', 'Monaco', monospace;"
        "}"

        /* 时间标签 */
        "QLabel#m_timeLabel {"
        "    color: %8;"
        "    font-size: 11px;"
        "}"
    ).arg(MEDICAL_WHITE.name(),
          MEDICAL_BLUE_PRIMARY.name(),
          MEDICAL_GRAY_BORDER.name(),
          MEDICAL_TEXT_DARK.name(),
          MEDICAL_BLUE_LIGHT.name(),
          MEDICAL_BLUE_DARK.name(),
          QColor(230, 235, 240).name(),
          QColor(120, 130, 140).name());

    this->setStyleSheet(styleSheet);

    // 设置对象名以便样式表识别
    m_headerWidget->setObjectName("m_headerWidget");
    m_titleLabel->setObjectName("m_titleLabel");

    m_closeBtn->setObjectName("m_closeBtn");
    m_maximizeBtn->setObjectName("m_maximizeBtn");
    m_updateBtn->setObjectName("m_updateBtn");
    m_clearBtn->setObjectName("m_clearBtn");
    m_timeLabel->setObjectName("m_timeLabel");
}

void displayLogsText::_initConnections()
{
    connect(m_closeBtn, &QToolButton::clicked, this, &displayLogsText::close);

    connect(m_maximizeBtn, &QToolButton::clicked, this, [=]() {
        if (m_isMaximized) {
            showNormal();
            m_maximizeBtn->setText("□");
            m_isMaximized = false;
        } else {
            showMaximized();
            m_maximizeBtn->setText("❐");
            m_isMaximized = true;
        }
    });

    connect(m_updateBtn, &QToolButton::clicked, this, &displayLogsText::_loadLogFile);

    connect(m_clearBtn, &QToolButton::clicked, this, [=]() {
        QString file_path = QCoreApplication::applicationDirPath() + "/log.txt";
        QFile logFile(file_path);
        if (logFile.open(QFile::WriteOnly | QFile::Truncate)) {
            logFile.close();
        }
        m_textBrowser->clear();
        QLOG_INFO() << "日志已清空";
    });
}

void displayLogsText::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mouseStartPoint = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void displayLogsText::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        QRect desktopRc = QApplication::desktop()->availableGeometry();
        QPoint curPoint = event->globalPos() - m_mouseStartPoint;

        if (event->globalY() > desktopRc.height()) {
            curPoint.setY(desktopRc.height() - m_mouseStartPoint.y());
        }
        move(curPoint);
    }
    QWidget::mouseMoveEvent(event);
}

void displayLogsText::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_mouseStartPoint = QCursor::pos() - frameGeometry().topLeft();
    }
    QWidget::mouseReleaseEvent(event);
}

void displayLogsText::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setBrush(m_bgColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect(), 8, 8);

    // 绘制边框
    QPainterPath path;
    path.addRoundedRect(rect(), 8, 8);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(MEDICAL_GRAY_BORDER, 1));
    p.drawPath(path);
}

void displayLogsText::closeEvent(QCloseEvent *event)
{
    event->accept();
    close();
}

void displayLogsText::_loadLogFile()
{
    m_textBrowser->clear();
    m_timeLabel->clear();
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(false);

    QDateTime datetime = QDateTime::currentDateTime();
    QString finder_file = datetime.toString("yyyyMMdd");
    QString logPath = finder_file + "logs.txt";

    QString file_path = QString("%1/%2")
        .arg(QDir(QCoreApplication::applicationDirPath()).filePath("suoweiLogFile"))
        .arg(logPath);

    QFile logFile(file_path);
    if (!logFile.exists()) {
        QLOG_ERROR() << "日志文件不存在:" << file_path;
        m_textBrowser->append("<font color=\"#E81123\">[错误] 日志文件不存在: " + file_path + "</font>");
        return;
    }

    if (!logFile.open(QIODevice::ReadOnly)) {
        QLOG_ERROR() << "读取LOG文件异常:" << logFile.errorString();
        m_textBrowser->append("<font color=\"#E81123\">[错误] 无法读取日志文件: " + logFile.errorString() + "</font>");
        return;
    }

    QElapsedTimer loadTimer;
    loadTimer.start();

    QQueue<QString> logQueue;
    QTextStream out(&logFile);
    out.setCodec("UTF-8");

    while (!out.atEnd()) {
        logQueue.enqueue(out.readLine());
    }

    int totalSize = logQueue.size();
    int currentIndex = 0;

    for (const QString &logStr : logQueue) {
        _appendtextshow(logStr);
        currentIndex++;

        int progress = static_cast<int>((currentIndex * 100.0) / totalSize);
        m_progressBar->setValue(progress);

        // 每处理100条更新一次界面，避免界面卡死
        if (currentIndex % 100 == 0) {
            QApplication::processEvents();
        }
    }

    QString loadTimeMsg = QString("加载完成 | 共 %1 条日志 | 耗时: %2 毫秒")
        .arg(totalSize)
        .arg(loadTimer.elapsed());
    m_timeLabel->setText(loadTimeMsg);

    m_progressBar->setValue(100);
    m_progressBar->setTextVisible(false);

    logFile.close();

    QLOG_INFO() << "日志加载完成，共" << totalSize << "条记录，耗时" << loadTimer.elapsed() << "毫秒";
}

void displayLogsText::_appendtextshow(const QString logdata)
{
    // 根据日志级别使用不同颜色
    QString color = "#333333";

    if (logdata.contains("ERROR", Qt::CaseInsensitive) ||
        logdata.contains("FATAL", Qt::CaseInsensitive)) {
        color = "#E81123";  // 错误红色
    } else if (logdata.contains("WARN", Qt::CaseInsensitive)) {
        color = "#FF8C00";  // 警告橙色
    } else if (logdata.contains("DEBUG", Qt::CaseInsensitive)) {
        color = "#2E8B57";  // 调试绿色
    } else if (logdata.contains("TRACE", Qt::CaseInsensitive)) {
        color = "#808080";  // 跟踪灰色
    } else if (logdata.contains("INFO", Qt::CaseInsensitive)) {
        color = MEDICAL_BLUE_PRIMARY.name();  // 信息蓝色
    }

    m_textBrowser->append("<font color=\"" + color + "\">" + logdata.toHtmlEscaped() + "</font>");
}
