#include "addusername.h"
#include "loginui.h"
#include "ui_addusername.h"
#include <operclass/fullyautomatedplatelets.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QRegExpValidator>
#include <QRegExp>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// 医疗淡蓝色主题色系 (与customtitlebar保持一致)
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
    const QString INPUT_BG = "#F5F8FA";
}

AddUserName::AddUserName(char UserSet, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddUserName),
    m_titleBar(nullptr),
    m_mainLayout(nullptr),
    m_centerWidget(nullptr),
    m_centerLayout(nullptr),
    m_addUserWidget(nullptr),
    m_addUserLayout(nullptr),
    m_enterUserLabel(nullptr),
    m_userLineEdit(nullptr),
    m_setPasswordLabel(nullptr),
    m_passwordLineEdit(nullptr),
    m_vipCheckBox(nullptr),
    m_addBtn(nullptr),
    m_addCancelBtn(nullptr),
    m_changePwdWidget(nullptr),
    m_changePwdLayout(nullptr),
    m_titleLabel(nullptr),
    m_oldPwdLabel(nullptr),
    m_oldPwdLineEdit(nullptr),
    m_newPwdLabel(nullptr),
    m_newPwdLineEdit(nullptr),
    m_confirmPwdLabel(nullptr),
    m_confirmPwdLineEdit(nullptr),
    m_okBtn(nullptr),
    m_cancelBtn(nullptr),
    m_show(UserSet),
    m_isMaximized(false)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // 隐藏默认标题栏
    setWindowFlags(Qt::FramelessWindowHint);

    // 初始化UI
    initUI();
    initStyle();

    // 根据模式设置界面
    if (m_show == 0) {
        setupAddUserMode();
        qInfo() << tr("打开添加用户界面");
    } else {
        setupChangePasswordMode();
        qInfo() << tr("打开修改密码界面");
    }
}

AddUserName::~AddUserName()
{
    delete ui;
}

void AddUserName::initUI()
{
    // 设置窗口大小
    setFixedSize(400, 320);

    // 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 创建标题栏
    m_titleBar = new CustomTitleBar(this);
    m_mainLayout->addWidget(m_titleBar);

    // 创建内容区域容器
    m_centerWidget = new QWidget(this);
    m_centerWidget->setObjectName("centerWidget");
    m_centerLayout = new QVBoxLayout(m_centerWidget);
    m_centerLayout->setContentsMargins(20, 20, 20, 20);
    m_centerLayout->setSpacing(0);

    m_mainLayout->addWidget(m_centerWidget);

    // 连接信号
    if (m_titleBar) {
        connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &AddUserName::onMinimizeRequested);
        connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &AddUserName::onMaximizeRequested);
        connect(m_titleBar, &CustomTitleBar::closeRequested, this, &AddUserName::onCloseRequested);
    }
}

void AddUserName::initStyle()
{
    QString styleSheet = QString(R"(
        AddUserName {
            background-color: %1;
            border-radius: 8px;
        }

        QWidget#centerWidget {
            background-color: %2;
            border-radius: 0 0 8px 8px;
        }

        QLabel {
            color: %3;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            background-color: transparent;
        }

        QLineEdit {
            background-color: %4;
            border: 1px solid %5;
            border-radius: 6px;
            padding: 6px 10px;
            font-size: 13px;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            color: %3;
        }

        QLineEdit:focus {
            border: 1px solid %6;
        }

        QCheckBox {
            color: %3;
            font-size: 13px;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            background-color: transparent;
        }

        QCheckBox::indicator {
            width: 18px;
            height: 18px;
        }

        QPushButton {
            background-color: %6;
            color: white;
            border: none;
            border-radius: 17px;
            font-size: 13px;
            font-weight: 500;
            font-family: 'Segoe UI', 'Microsoft YaHei', 'PingFang SC';
            padding: 6px 0;
        }

        QPushButton:hover {
            background-color: %7;
        }

        QPushButton:pressed {
            background-color: %8;
        }
    )").arg(MedicalColors::PRIMARY_LIGHT)      // 主背景
      .arg(MedicalColors::WHITE)                // 内容区域背景
      .arg(MedicalColors::TEXT_DARK)            // 文字颜色
      .arg(MedicalColors::INPUT_BG)             // 输入框背景
      .arg(MedicalColors::DIVIDER)              // 输入框边框
      .arg(MedicalColors::PRIMARY_ACTIVE)       // 按钮背景/焦点边框
      .arg(MedicalColors::PRIMARY_MEDIUM)       // 按钮悬停
      .arg(MedicalColors::PRIMARY_DARK);        // 按钮按下

    this->setStyleSheet(styleSheet);
}

void AddUserName::setupAddUserMode()
{
    m_titleBar->setTitle(tr("添加用户"));
    setFixedSize(400, 280);

    // 创建添加用户控件
    m_addUserWidget = new QWidget(m_centerWidget);
    m_addUserLayout = new QGridLayout(m_addUserWidget);
    m_addUserLayout->setSpacing(12);
    m_addUserLayout->setContentsMargins(0, 0, 0, 0);

    // 用户名
    m_enterUserLabel = new QLabel(tr("输入用户名:"), m_addUserWidget);
    m_enterUserLabel->setMinimumWidth(90);
    m_enterUserLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_userLineEdit = new QLineEdit(m_addUserWidget);
    m_userLineEdit->setPlaceholderText(tr("请输入用户名"));

    // 密码
    m_setPasswordLabel = new QLabel(tr("设置密码:"), m_addUserWidget);
    m_setPasswordLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_passwordLineEdit = new QLineEdit(m_addUserWidget);
    m_passwordLineEdit->setPlaceholderText(tr("请输入密码"));
    m_passwordLineEdit->setEchoMode(QLineEdit::Password);

    // 限制只能输入字母和数字
    QRegExp regx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    m_passwordLineEdit->setValidator(validator);
    m_passwordLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    // 管理员复选框
    m_vipCheckBox = new QCheckBox(tr("添加为管理员用户"), m_addUserWidget);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);

    m_addBtn = new QPushButton(tr("添 加"), m_addUserWidget);
    m_addBtn->setFixedSize(90, 34);

    m_addCancelBtn = new QPushButton(tr("取 消"), m_addUserWidget);
    m_addCancelBtn->setFixedSize(90, 34);

    buttonLayout->addWidget(m_addBtn);
    buttonLayout->addWidget(m_addCancelBtn);

    // 添加到布局
    m_addUserLayout->addWidget(m_enterUserLabel, 0, 0);
    m_addUserLayout->addWidget(m_userLineEdit, 0, 1);
    m_addUserLayout->addWidget(m_setPasswordLabel, 1, 0);
    m_addUserLayout->addWidget(m_passwordLineEdit, 1, 1);
    m_addUserLayout->addWidget(m_vipCheckBox, 2, 0, 1, 2);
    m_addUserLayout->addLayout(buttonLayout, 3, 0, 1, 2);

    m_centerLayout->addWidget(m_addUserWidget);

    // 连接信号
    connect(m_addBtn, &QPushButton::clicked, this, &AddUserName::onAddClicked);
    connect(m_addCancelBtn, &QPushButton::clicked, this, &AddUserName::onCancelClicked);
}

void AddUserName::setupChangePasswordMode()
{
    m_titleBar->setTitle(tr("修改密码"));
    setFixedSize(400, 340);

    // 创建修改密码控件
    m_changePwdWidget = new QWidget(m_centerWidget);
    m_changePwdLayout = new QGridLayout(m_changePwdWidget);
    m_changePwdLayout->setSpacing(12);
    m_changePwdLayout->setContentsMargins(0, 0, 0, 0);

    // 标题
    m_titleLabel = new QLabel(m_changePwdWidget);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2C5F8A;");

    // 旧密码
    m_oldPwdLabel = new QLabel(tr("旧密码:"), m_changePwdWidget);
    m_oldPwdLabel->setMinimumWidth(90);
    m_oldPwdLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_oldPwdLineEdit = new QLineEdit(m_changePwdWidget);
    m_oldPwdLineEdit->setPlaceholderText(tr("请输入旧密码"));
    m_oldPwdLineEdit->setEchoMode(QLineEdit::Password);

    // 新密码
    m_newPwdLabel = new QLabel(tr("新密码:"), m_changePwdWidget);
    m_newPwdLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_newPwdLineEdit = new QLineEdit(m_changePwdWidget);
    m_newPwdLineEdit->setPlaceholderText(tr("请输入新密码"));
    m_newPwdLineEdit->setEchoMode(QLineEdit::Password);

    // 确认新密码
    m_confirmPwdLabel = new QLabel(tr("确认新密码:"), m_changePwdWidget);
    m_confirmPwdLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_confirmPwdLineEdit = new QLineEdit(m_changePwdWidget);
    m_confirmPwdLineEdit->setPlaceholderText(tr("请再次输入新密码"));
    m_confirmPwdLineEdit->setEchoMode(QLineEdit::Password);

    // 限制只能输入字母和数字
    QRegExp regx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(regx, this);
    m_oldPwdLineEdit->setValidator(validator);
    m_oldPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_newPwdLineEdit->setValidator(validator);
    m_newPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);
    m_confirmPwdLineEdit->setValidator(validator);
    m_confirmPwdLineEdit->setAttribute(Qt::WA_InputMethodEnabled, false);

    // 按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    buttonLayout->setAlignment(Qt::AlignCenter);

    m_okBtn = new QPushButton(tr("确 认"), m_changePwdWidget);
    m_okBtn->setFixedSize(90, 34);

    m_cancelBtn = new QPushButton(tr("取 消"), m_changePwdWidget);
    m_cancelBtn->setFixedSize(90, 34);

    buttonLayout->addWidget(m_okBtn);
    buttonLayout->addWidget(m_cancelBtn);

    // 添加到布局
    m_changePwdLayout->addWidget(m_titleLabel, 0, 0, 1, 2);
    m_changePwdLayout->addWidget(m_oldPwdLabel, 1, 0);
    m_changePwdLayout->addWidget(m_oldPwdLineEdit, 1, 1);
    m_changePwdLayout->addWidget(m_newPwdLabel, 2, 0);
    m_changePwdLayout->addWidget(m_newPwdLineEdit, 2, 1);
    m_changePwdLayout->addWidget(m_confirmPwdLabel, 3, 0);
    m_changePwdLayout->addWidget(m_confirmPwdLineEdit, 3, 1);
    m_changePwdLayout->addLayout(buttonLayout, 4, 0, 1, 2);

    m_centerLayout->addWidget(m_changePwdWidget);

    // 连接信号
    connect(m_okBtn, &QPushButton::clicked, this, &AddUserName::onOkClicked);
    connect(m_cancelBtn, &QPushButton::clicked, this, &AddUserName::onCancelPwdClicked);
}

void AddUserName::ModifyUsername_Slot(QString name)
{
    m_NotifyUsername = name;
    if (m_titleLabel) {
        QString text = QString(tr("修改用户 %1 密码")).arg(name);
        m_titleLabel->setText(text);
    }
}

void AddUserName::onAddClicked()
{
    QString user_ = m_userLineEdit->text().trimmed();
    QString setpassword_ = m_passwordLineEdit->text().trimmed();

    if (user_.isEmpty()) {
        QMessageBox::critical(this, tr("错误提醒"), tr("用户为空操作失败!"));
        return;
    }
    if (setpassword_.isEmpty()) {
        QMessageBox::critical(this, tr("错误提醒"), tr("密码未设置操作失败!"));
        return;
    }

    bool AddVip = m_vipCheckBox->isChecked();
    emit AddUser(user_, setpassword_, AddVip);
    close();
}

void AddUserName::onCancelClicked()
{
    close();
}

void AddUserName::onOkClicked()
{
    QString username = m_NotifyUsername;
    QString oldInput = m_oldPwdLineEdit->text();
    QString newInput = m_newPwdLineEdit->text();
    QString confirmInput = m_confirmPwdLineEdit->text();

    // 输入合法性检查
    if (oldInput.isEmpty() || newInput.isEmpty()) {
        QMessageBox::warning(this, tr("错误"), tr("密码不能为空"));
        return;
    }

    if (oldInput == newInput && oldInput == confirmInput) {
        QMessageBox::information(this, tr("用户密码"), tr("修改重复密码无效!"));
        return;
    }

    // 获取数据库存储的旧密码
    QString dbOldPassword = FullyAutomatedPlatelets::pinstancesqlData()->FindPassword(username);

    // 验证新密码一致性
    if (oldInput != dbOldPassword) {
        QMessageBox::information(this, tr("失败"), tr("初始密码不一致"));
        return;
    } else if (newInput != confirmInput) {
        QMessageBox::information(this, tr("修改失败"), tr("新密码与确认密码不一致!"));
        return;
    } else if (newInput == confirmInput) {
        FullyAutomatedPlatelets::pinstancesqlData()->NotifyPassword(username, newInput);
        QMessageBox::information(this, tr("用户密码"), tr("新密码修改成功!"));
        close();
    }
}

void AddUserName::onCancelPwdClicked()
{
    close();
}

void AddUserName::onMinimizeRequested()
{
    showMinimized();
}

void AddUserName::onMaximizeRequested()
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

void AddUserName::onCloseRequested()
{
    close();
}

void AddUserName::closeEvent(QCloseEvent *event)
{
    switch(m_show) {
        case 0:
            qInfo() << tr("关闭添加用户界面");
            break;
        case 1:
            qInfo() << tr("关闭修改密码界面");
            break;
        default:
            break;
    }
    event->accept();
}
