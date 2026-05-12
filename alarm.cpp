#include "alarm.h"
#include "ui_alarm.h"
#include "quiutils.h"
#include "operclass/fullyautomatedplatelets.h"
#include <mainwindow.h>
#include <QBitmap>
#include <QMouseEvent>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QDesktopWidget>
#include "QsLog/include/QsLog.h"


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

#define TITLE_BACK_COLOR  QColor(135, 206, 250)

bool Alarm::m_SoundAlarm = false;

// 医疗淡蓝色主题色系
namespace MedicalColors {
   const QString PRIMARY_LIGHT = "#E8F4FD";   // 主背景色 - 极浅蓝
   const QString PRIMARY_SOFT = "#B8DFF7";    // 辅助背景
   const QString PRIMARY_MEDIUM = "#7EC8F0";  // 中间色
   const QString PRIMARY_ACTIVE = "#5BB8E8";  // 按钮激活色
   const QString PRIMARY_DARK = "#3A9BD4";    // 按钮深色
   const QString TEXT_DARK = "#2C5F8A";       // 深色文字
   const QString TEXT_LIGHT = "#4A7FA3";      // 浅色文字
   const QString DIVIDER = "#D4EAF5";         // 分割线
   const QString WHITE = "#FFFFFF";           // 白色背景
   const QString BG_DARK = "#F5F9FC";         // 内容区域背景 - 浅灰蓝
   const QString LOG_BG = "#FFFFFF";          // 日志区域 - 纯白
   const QString LOG_TEXT_NORMAL = "#1A1A1A"; // 正常日志 - 深灰
   const QString LOG_TEXT_PROMPT = "#E6A017"; // 提示日志 - 橙黄
   const QString LOG_TEXT_ERROR = "#E53935";  // 异常日志 - 鲜红
   const QString LOG_TEXT_ACTION = "#1E88E5"; // 操作日志 - 亮蓝
}

Alarm::Alarm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Alarm),
    m_titleBar(nullptr),
    m_mainLayout(nullptr),
    m_centerWidget(nullptr),
    m_centerLayout(nullptr),
    m_iconLabel(nullptr),
    m_textBrowser(nullptr),
    m_hornBtn(nullptr),
    m_confirmBtn(nullptr),
    m_isMaximized(false)
{
    ui->setupUi(this);

    // 隐藏默认标题栏
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowOpacity(0.95);

    // 初始化UI
    initUI();
    initStyle();
    setupMediaPlayer();
}

Alarm::~Alarm()
{
    delete ui;

    if(pAction){
        delete pAction;
        pAction = nullptr;
    }

    if(DisAction){
       delete DisAction;
       DisAction = nullptr;
    }
}

void Alarm::initUI()
{
    // 设置窗口大小
    setFixedSize(OUT_UI_SIZE_WIDTH, OUT_UI_SIZE_HEIGHT);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建标题栏
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle(tr("仪器通用信息"));
    m_mainLayout->addWidget(m_titleBar);

    // 创建内容区域容器
    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName("centerWidget");
    m_centerLayout = new QVBoxLayout(m_centerWidget);
    m_centerLayout->setContentsMargins(1, 1, 1, 1);
    m_centerLayout->setSpacing(1);

    // 文本浏览区域
    m_textBrowser = new QTextBrowser(m_centerWidget);
    m_textBrowser->setObjectName("textBrowser");
    m_textBrowser->document()->setMaximumBlockCount(500);

    // 底部按钮区域容器
    QWidget *bottomWidget = new QWidget(m_centerWidget);
    bottomWidget->setObjectName("bottomWidget");
    bottomWidget->setFixedHeight(50);

    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);
    bottomLayout->setContentsMargins(10, 5, 10, 5);
    bottomLayout->setSpacing(10);

    // 喇叭按钮
    m_hornBtn = new QPushButton(bottomWidget);
    m_hornBtn->setObjectName("hornBtn");
    m_hornBtn->setFixedSize(32, 32);
    m_hornBtn->setCursor(Qt::PointingHandCursor);
    m_hornBtn->setToolTip(tr("开关蜂鸣器"));

    // 创建喇叭图标Action
    QIcon horn(":/Picture/SetPng/Horn.png");
    QIcon dishorn(":/Picture/SetPng/DisHorn.png");
    pAction = new QAction(horn, "", this);
    DisAction = new QAction(dishorn, "", this);

    // 设置喇叭按钮图标
    if (m_SoundAlarm) {
        m_hornBtn->setIcon(horn);
    } else {
        m_hornBtn->setIcon(dishorn);
    }
    m_hornBtn->setIconSize(QSize(24, 24));

    // 确认按钮
    m_confirmBtn = new QPushButton(tr("确认信息"), bottomWidget);
    m_confirmBtn->setObjectName("confirmBtn");
    m_confirmBtn->setFixedSize(100, 34);
    m_confirmBtn->setCursor(Qt::PointingHandCursor);

    bottomLayout->addWidget(m_hornBtn);
    bottomLayout->addStretch();
    bottomLayout->addWidget(m_confirmBtn);

    // 添加到布局
    m_centerLayout->addWidget(m_textBrowser, 1);
    m_centerLayout->addWidget(bottomWidget);

    m_mainLayout->addWidget(m_centerWidget);

    // 连接信号
    if (m_titleBar) {
        connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &Alarm::onMinimizeRequested);
        connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &Alarm::onMaximizeRequested);
        connect(m_titleBar, &CustomTitleBar::closeRequested, this, &Alarm::onCloseRequested);
    }

    connect(m_hornBtn, &QPushButton::clicked, this, &Alarm::onHornClicked);
    connect(m_confirmBtn, &QPushButton::clicked, this, &Alarm::onConfirmClicked);
}

void Alarm::initStyle()
{
    QString styleSheet = QString(R"(
            Alarm {
                background-color: %1;
                border-radius: 8px;
            }

            QWidget#centerWidget {
                background-color: %2;
                border-radius: 0 0 8px 8px;
            }

            QWidget#bottomWidget {
                background-color: %2;
                border-radius: 0 0 8px 8px;
            }

            QTextBrowser#textBrowser {
                background-color: %3;
                color: %4;
                border: 1px solid %5;
                border-radius: 6px;
                font-family: 'Consolas', 'Monaco', 'Microsoft YaHei';
                font-size: 13px;
                padding: 12px;
            }

            QPushButton#hornBtn {
                background-color: transparent;
                border: none;
                border-radius: 16px;
            }

            QPushButton#hornBtn:hover {
                background-color: %5;
            }

            QPushButton#confirmBtn {
                background-color: %6;
                color: white;
                border: none;
                border-radius: 17px;
                font-size: 13px;
                font-weight: 500;
                font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
                min-width: 100px;
            }

            QPushButton#confirmBtn:hover {
                background-color: %7;
            }

            QPushButton#confirmBtn:pressed {
                background-color: %8;
            }

            QScrollBar:vertical {
                background: %3;
                width: 10px;
                margin: 0px;
                border-radius: 5px;
            }

            QScrollBar::handle:vertical {
                background: %6;
                min-height: 20px;
                border-radius: 5px;
            }

            QScrollBar::handle:vertical:hover {
                background: %7;
            }

            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0px;
            }
        )").arg(MedicalColors::PRIMARY_LIGHT)      // 主背景
          .arg(MedicalColors::WHITE)                // 内容区域背景
          .arg(MedicalColors::LOG_BG)               // 日志区域背景
          .arg(MedicalColors::LOG_TEXT_NORMAL)      // 日志文字颜色
          .arg(MedicalColors::DIVIDER)              // 分隔线颜色
          .arg(MedicalColors::PRIMARY_ACTIVE)       // 按钮背景
          .arg(MedicalColors::PRIMARY_MEDIUM)       // 按钮悬停
          .arg(MedicalColors::PRIMARY_DARK);        // 按钮按下

    this->setStyleSheet(styleSheet);
}

void Alarm::setupMediaPlayer()
{
    m_play.reset(new QMediaPlaylist);
    m_playSound.reset(new QMediaPlayer);

    if (m_play && m_playSound) {
        m_play->addMedia(QUrl("qrc:/Picture/SetPng/warm.wav"));
        m_play->setCurrentIndex(0);
        m_play->setPlaybackMode(QMediaPlaylist::Loop);
        m_playSound->setPlaylist(m_play.data());

        if (m_SoundAlarm) {
            m_playSound->play();
        }
    }
}

void Alarm::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    // 不需要绘制背景，因为样式表已经处理
}

void Alarm::appendLog(const QString &reminderHead, const QString &detailedDescription, const QString &color)
{
    QString logMessage = QString("<font color=\"%1\">%2 %3</font>")
                         .arg(color, reminderHead, detailedDescription);

    {
        QMutexLocker locker(&m_textMutex);
        m_textBrowser->append(logMessage);
    }

    logToFile(logMessage);
}

void Alarm::logToFile(const QString &logMessage)
{
    QMutexLocker locker(&m_fileMutex);

    QFile file("alarm_log.txt");
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
            << " " << logMessage << "\n";
        file.close();
    }
}

void Alarm::InsertText(quint8 ReminderKind, const QString detailedDescription)
{

	// 使用 invokeMethod 确保在主线程执行 UI 更新
	QMetaObject::invokeMethod(this, [this, ReminderKind, detailedDescription]() {
		// 原 InsertText 的整个逻辑放这里
		QString currentTime = QDateTime::currentDateTime().toString("hh:mm:ss");
		QString reminderHead;
		QString color;
		
		switch (ReminderKind) {
		case NORMALLOG:
			reminderHead = tr("[%1]").arg(currentTime);
			color = MedicalColors::LOG_TEXT_NORMAL;
			emit AlarmIconState(NORMALLOG);
			break;
		case PROMPTLOG:
			reminderHead = tr("[%1 提示]").arg(currentTime);
			color = MedicalColors::LOG_TEXT_PROMPT;
			emit AlarmIconState(NORMALLOG);
			break;
		case ERRORLOG:
			reminderHead = tr("[%1 异常]").arg(currentTime);
			color = MedicalColors::LOG_TEXT_ERROR;
			emit AlarmIconState(ERRORLOG);
			break;
		case USERACTIONS:
			reminderHead = tr("[%1 操作]").arg(currentTime);
			color = MedicalColors::LOG_TEXT_ACTION;
			emit AlarmIconState(NORMALLOG);
			break;
		case TESTINGLOG:
			reminderHead = tr("[%1 测试流程]").arg(currentTime);
			color = MedicalColors::LOG_TEXT_NORMAL;
			emit AlarmIconState(NORMALLOG);
			break;
		default:
			return;
		}

		appendLog(reminderHead, detailedDescription, color);
		m_textBrowser->moveCursor(QTextCursor::End);
	}, Qt::AutoConnection);

}

void Alarm::handleReminderText(quint8 reminderKind, const QString outtext)
{
    if (reminderKind == TESTINGLOG) {
        QString currentDate = "[" + QDateTime::currentDateTime().toString("hh:mm:ss") + tr(" Test System]");
        appendLog(currentDate, outtext, MedicalColors::LOG_TEXT_NORMAL);
    }
    m_textBrowser->moveCursor(QTextCursor::End);
}

void Alarm::OnOffSound(bool open)
{
    m_SoundAlarm = open;

    if (m_playSound) {
        if (open) {
            m_playSound->play();
            if (m_hornBtn) {
                m_hornBtn->setIcon(pAction->icon());
            }
        } else {
            m_playSound->stop();
            if (m_hornBtn) {
                m_hornBtn->setIcon(DisAction->icon());
            }
        }
    }
}

void Alarm::onHornClicked()
{
    m_SoundAlarm = !m_SoundAlarm;

    if (m_SoundAlarm) {
        m_hornBtn->setIcon(pAction->icon());
        if (m_playSound) {
            m_playSound->play();
        }
    } else {
        m_hornBtn->setIcon(DisAction->icon());
        if (m_playSound) {
            m_playSound->stop();
        }
    }
}

void Alarm::onConfirmClicked()
{
    QLOG_DEBUG() << "确认报警";
    if (m_playSound) {
        m_playSound->stop();
    }
    close();
}

void Alarm::onMinimizeRequested()
{
    showMinimized();
}

void Alarm::onMaximizeRequested()
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

void Alarm::onCloseRequested()
{
    close();
}

void Alarm::closeEvent(QCloseEvent *event)
{
    if (m_playSound) {
        m_playSound->stop();
    }
    event->accept();
}
