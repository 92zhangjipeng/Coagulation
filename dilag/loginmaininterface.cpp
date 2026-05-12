#include "loginmaininterface.h"
#include "ui_loginmaininterface.h"
#include <QDesktopWidget>
#include "cglobal.h"
#include "quiutils.h"
#include "operclass/fullyautomatedplatelets.h"
#include "custom_style/animationprocessbar.h"
#include <QtConcurrent>
#include <mainwindow.h>
#include <QCryptographicHash>
#include <warn_interface.h>
#include <verifycoordinates.h>
#include <StyledComparisonDialog.h>


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

bool checkFileExist(const QString& path) {
    QFile file(path);
    bool exists = file.exists();
    if (!exists) {
        if (file.open(QIODevice::Append)) {
            QLOG_WARN() << "文件不存在,创建:" << path << endl;
            file.close();
        }
    }
    return exists;
}

loginmaininterface::loginmaininterface(QWidget *parent) :
    QDialog(parent),
    mtotalcommed(0),
    m_gotcompleted(0),
    m_bparaexit(false),
    m_TimerRunning(false),
    ui(new Ui::loginmaininterface)
{
    ui->setupUi(this);

    // 设置窗口属性
    setWindowTitle(tr("登录"));
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(400, 430);  // 稍微增加高度以容纳新标题栏

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setAlignment(Qt::AlignTop);  // 顶部对齐

    // ==================== 使用 CustomTitleBar ====================
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);  // 水平拉伸，高度固定
    m_titleBar->setTitle(tr("用户登录"));
    mainLayout->addWidget(m_titleBar);

    // ==================== 主内容区域 ====================
    QFrame *frameMain = createMainFrame();
    frameMain->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(frameMain);

    // 设置内容区域的背景和边框样式（医疗器械风格）
    frameMain->setStyleSheet(
        "QFrame {"
        "    background-color: #f0f4f8;"
        "    border: 0px solid #c0d0e0;"
        "    border-top: none;"
        "    border-radius: 0 0 8px 8px;"
        "}"
    );

    m_myborder = new MyBorderContainer(this);

    // 连接标题栏信号
    initConnections();

    InitStyle();

    if (m_lineEdit_password) {
        m_lineEdit_password->setFocus(Qt::ActiveWindowFocusReason);
    }
}

// 连接标题栏信号
void loginmaininterface::initConnections()
{
    if (!m_titleBar) return;

    connect(m_titleBar, &CustomTitleBar::closeRequested,
            this, &loginmaininterface::onTitleBarCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested,
            this, &loginmaininterface::onTitleBarMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested,
            this, &loginmaininterface::onTitleBarMaximizeRequested);
}

// 标题栏关闭按钮槽函数
void loginmaininterface::onTitleBarCloseRequested()
{
    this->close();
}

// 标题栏最小化按钮槽函数
void loginmaininterface::onTitleBarMinimizeRequested()
{
    this->showMinimized();
}

// 标题栏最大化/还原按钮槽函数
void loginmaininterface::onTitleBarMaximizeRequested()
{
    if (max)
    {
        this->setGeometry(location);
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(false);
        }
    }
    else
    {
        QDesktopWidget* desktop = QApplication::desktop();
        int N = desktop->screenCount();
        if (N == 2)
        {
            location = this->geometry();
            this->setGeometry(desktop->screenGeometry(1));
        }
        else
        {
            location = this->geometry();
            this->setGeometry(qApp->desktop()->availableGeometry());
        }
        if (m_titleBar) {
            m_titleBar->updateMaximizeButton(true);
        }
    }
    max = !max;
}

// 创建主内容区域（医疗器械风格优化）
QFrame* loginmaininterface::createMainFrame()
{
    QFrame *frameMain = new QFrame(this);
    frameMain->setFrameShape(QFrame::StyledPanel);
    frameMain->setFrameShadow(QFrame::Raised);


    // 使用QVBoxLayout作为主内容区域的主布局
    QVBoxLayout *mainContentLayout = new QVBoxLayout(frameMain);
	mainContentLayout->setSpacing(12);
	mainContentLayout->setContentsMargins(40, 30, 40, 30);  // 增加左右边距
	mainContentLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);  // 顶部居中

    // 系统名称 - 医疗器械风格
    m_label_sysname = new QLabel(tr("全自动血小板聚集分析系统"), frameMain);
    m_label_sysname->setFixedHeight(50);
    m_label_sysname->setMinimumWidth(300);
    QFont sysFont("Microsoft YaHei", 16);
    sysFont.setWeight(QFont::Bold);
    m_label_sysname->setFont(sysFont);
    m_label_sysname->setStyleSheet(
        "QLabel {"
        "    color: #2c6fb8;"
        "    background-color: transparent;"
        "    border-radius: 5px;"
        "    padding: 5px;"
        "}"
    );
    m_label_sysname->setAlignment(Qt::AlignCenter);
    mainContentLayout->addWidget(m_label_sysname);

    // 分隔线
    QFrame *line = new QFrame(frameMain);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #c0d0e0; max-height: 1px;");
    mainContentLayout->addWidget(line);

    // 版本信息
    m_label_version_number = new QLabel(frameMain);
    m_label_version_number->setFixedHeight(28);
    QFont versionFont("Microsoft YaHei", 10);
    versionFont.setWeight(QFont::Light);
    m_label_version_number->setFont(versionFont);
    m_label_version_number->setStyleSheet(
        "QLabel {"
        "    color: #5a6a7a;"
        "    background-color: transparent;"
        "    border-radius: 5px;"
        "    padding: 2px 4px;"
        "}"
    );
    m_label_version_number->setAlignment(Qt::AlignCenter);
    mainContentLayout->addWidget(m_label_version_number);

    mainContentLayout->addSpacing(10);

    // 用户名区域 - 医疗器械风格
    QHBoxLayout *userLayout = new QHBoxLayout();
    userLayout->setSpacing(12);

    QLabel *label_user = new QLabel(tr("用户名"), frameMain);
    label_user->setFixedWidth(80);
    label_user->setFixedHeight(36);
    QFont labelFont("Microsoft YaHei", 11);
    labelFont.setWeight(QFont::Medium);
    label_user->setFont(labelFont);
    label_user->setStyleSheet(
        "QLabel {"
        "    color: #2c3e50;"
        "    background-color: transparent;"
        "}"
    );
    label_user->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    userLayout->addWidget(label_user);

    m_comboBox_user = new QComboBox(frameMain);
    m_comboBox_user->setFixedHeight(36);
    m_comboBox_user->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_comboBox_user->setEditable(true);
    m_comboBox_user->setIconSize(QSize(32, 32));
    m_comboBox_user->setStyleSheet(
        "QComboBox {"
        "    border-radius: 4px;"
        "    padding: 6px 8px;"
        "    font: 11pt 'Microsoft YaHei';"
        "    border: 1px solid #b0c0d0;"
        "    background-color: white;"
        "    min-height: 24px;"
        "}"
        "QComboBox:focus {"
        "    border: 1px solid #4a90e2;"
        "}"
        "QComboBox::drop-down {"
        "    subcontrol-origin: padding;"
        "    subcontrol-position: top right;"
        "    width: 24px;"
        "    border: none;"
        "}"
        "QComboBox::down-arrow {"
        "    height: 12px;"
        "    width: 12px;"
        "    image: url(:/Picture/minus.png);"
        "}"
        "QComboBox QAbstractItemView {"
        "    background: white;"
        "    border: 1px solid #b0c0d0;"
        "    border-radius: 4px;"
        "    font: 11pt 'Microsoft YaHei';"
        "    outline: 0px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "    height: 32px;"
        "    color: #2c3e50;"
        "    padding-left: 12px;"
        "    background-color: white;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "    background-color: #e8f0f8;"
        "    color: #2c6fb8;"
        "}"
        "QComboBox QAbstractItemView::item:selected {"
        "    background-color: #4a90e2;"
        "    color: white;"
        "}"
    );
    userLayout->addWidget(m_comboBox_user);
    mainContentLayout->addLayout(userLayout);

    // 密码区域
    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->setSpacing(12);

    QLabel *label_password = new QLabel(tr("密码"), frameMain);
    label_password->setFixedWidth(80);
    label_password->setFixedHeight(36);
    label_password->setFont(labelFont);
    label_password->setStyleSheet(
        "QLabel {"
        "    color: #2c3e50;"
        "    background-color: transparent;"
        "}"
    );
    label_password->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    passwordLayout->addWidget(label_password);

    m_lineEdit_password = new QLineEdit(frameMain);
    m_lineEdit_password->setFixedHeight(36);
    m_lineEdit_password->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_lineEdit_password->setEchoMode(QLineEdit::Password);
    m_lineEdit_password->setStyleSheet(
        "QLineEdit {"
        "    border-radius: 4px;"
        "    padding: 6px 8px;"
        "    font: 11pt 'Microsoft YaHei';"
        "    border: 1px solid #b0c0d0;"
        "    background-color: white;"
        "    min-height: 24px;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #4a90e2;"
        "}"
    );
    passwordLayout->addWidget(m_lineEdit_password);
    mainContentLayout->addLayout(passwordLayout);

    mainContentLayout->addSpacing(15);

    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(20);
    buttonLayout->setContentsMargins(10, 0, 10, 0);

    m_toolButton_enter = createActionButton("", tr("登 录"), "toolButton_enter");
    m_toolButton_enter->setFixedHeight(42);
    m_toolButton_enter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_toolButton_enter, &QToolButton::clicked, this, &loginmaininterface::on_toolButton_enter_clicked);
    buttonLayout->addWidget(m_toolButton_enter);

    m_toolButton_exit = createActionButton("", tr("退 出"), "toolButton_exit");
    m_toolButton_exit->setFixedHeight(42);
    m_toolButton_exit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(m_toolButton_exit, &QToolButton::clicked, this, &loginmaininterface::on_toolButton_exit_clicked);
    buttonLayout->addWidget(m_toolButton_exit);
    mainContentLayout->addLayout(buttonLayout);

    mainContentLayout->addSpacing(15);

    // 仪器类型和提醒区域
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->setSpacing(10);

    m_label_equipmentkind = new QLabel(tr("仪器类型: --"), frameMain);
    m_label_equipmentkind->setFixedHeight(32);
    m_label_equipmentkind->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QFont infoFont("Microsoft YaHei", 9);
    infoFont.setWeight(QFont::Light);
    m_label_equipmentkind->setFont(infoFont);
    m_label_equipmentkind->setStyleSheet(
        "QLabel {"
        "    color: #5a6a7a;"
        "    background-color: #e8f0f8;"
        "    border-radius: 4px;"
        "    padding: 4px 10px;"
        "}"
    );
    infoLayout->addWidget(m_label_equipmentkind);

    m_label_reminder = new QLabel(frameMain);
    m_label_reminder->setFixedHeight(32);
    m_label_reminder->setFont(infoFont);
    m_label_reminder->setStyleSheet(
        "QLabel {"
        "    color: #2c6fb8;"
        "    background-color: #e8f0f8;"
        "    border-radius: 4px;"
        "    padding: 4px 10px;"
        "}"
    );
    m_label_reminder->hide();
    infoLayout->addWidget(m_label_reminder);
    infoLayout->addStretch();
    mainContentLayout->addLayout(infoLayout);

    // 进度条 - 医疗器械风格
    m_progressBar_readAxis = new AnimationProcessBar(frameMain);
    m_progressBar_readAxis->setFixedHeight(28);
    m_progressBar_readAxis->setValue(0);
    m_progressBar_readAxis->setTextVisible(true);
    m_progressBar_readAxis->setStyleSheet(
        "QProgressBar {"
        "    border: 1px solid #b0c0d0;"
        "    border-radius: 4px;"
        "    background-color: #f0f4f8;"
        "    text-align: center;"
        "    color: #2c3e50;"
        "    font: 9pt 'Microsoft YaHei';"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #4a90e2;"
        "    border-radius: 3px;"
        "}"
    );
    m_progressBar_readAxis->hide();
    mainContentLayout->addWidget(m_progressBar_readAxis);

    return frameMain;
}

// 创建操作按钮（登入/退出）- 医疗器械风格
QToolButton* loginmaininterface::createActionButton(const QString& iconPath, const QString& text, const QString& objectName)
{
    Q_UNUSED(iconPath);

    QToolButton *btn = new QToolButton();
    btn->setObjectName(objectName);
    btn->setMinimumWidth(120);
    btn->setText(text);
    btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
        QString("QToolButton#%1 {"
        "    background-color: #4a90e2;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 20px;"
        "    font-family: 'Microsoft YaHei';"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "    color: white;"
        "}"
        "QToolButton#%1:hover {"
        "    background-color: #5a9ae2;"
        "}"
        "QToolButton#%1:pressed {"
        "    background-color: #3a80d2;"
        "}").arg(objectName)
    );
    return btn;
}

loginmaininterface::~loginmaininterface()
{
    delete ui;

    closeReminder();

    if (mLoadcoordinates)
    {
        mLoadcoordinates->CloseSerial();
        delete mLoadcoordinates;
        mLoadcoordinates = NULL;
    }
    SingletonAxis::GetInstance()->deleteInstance();

    ConsumablesOper::GetpInstance()->del_Instance();

    QLOG_DEBUG()<<"析构登录界面"<<endl;
}

void loginmaininterface::initPasswordField()
{
    if (!m_lineEdit_password) return;

    static QRegularExpression rx("[a-zA-Z0-9]+");
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(rx, this);
    m_lineEdit_password->setValidator(validator);
    m_lineEdit_password->setPlaceholderText(tr("请输入密码"));
    m_lineEdit_password->setAttribute(Qt::WA_InputMethodEnabled, false);
}

void loginmaininterface::asyncInitDatabase()
{
    SingletonAxis::GetInstance()->GetpStruct();
    ConsumablesOper::GetpInstance()->iterateOverEquipmentConsumables();

    CustomCreatSql* sqldata = FullyAutomatedPlatelets::pinstancesqlData();
    QFuture<void> fut1 = QtConcurrent::run(sqldata, &CustomCreatSql::initializeSQLTable);
    fut1.waitForFinished();
}

void loginmaininterface::initHardware()
{
    if (!mLoadcoordinates) {
        mLoadcoordinates = new loadEquipmentPos(this);

        connect(this, &loginmaininterface::signalStart,
                mLoadcoordinates, &loadEquipmentPos::StatrLoad);

        connect(mLoadcoordinates, &loadEquipmentPos::closetimercon,
                this, &loginmaininterface::slotclosetimercon);

        connect(mLoadcoordinates
                ,&loadEquipmentPos::setEquipmentIndex,
                this,
                &loginmaininterface::slotsetEquipmentIndex);

        connect(mLoadcoordinates,&loadEquipmentPos::equipmentHadPosAixs,
                this,&loginmaininterface::ToReadtEquipmentTypePos);

        connect(this,&loginmaininterface::configuredModel,
                mLoadcoordinates,
                &loadEquipmentPos::onconfiguredModel);

        connect(mLoadcoordinates,&loadEquipmentPos::sendUpdateProgressshow,
                this,&loginmaininterface::slotProgressshow);

        connect(this,&loginmaininterface::sycnParaConfigFileSatte,
                mLoadcoordinates,&loadEquipmentPos::_sycnobtainEquipmenttyped);

        connect(mLoadcoordinates, &loadEquipmentPos::progresstotal,
                this, [=](int totalnum)
        {
            mtotalcommed = totalnum;
            emit sycnParaConfigFileSatte(m_bparaexit,_parasettingPath);
        });

        emit signalStart();
    }
}

void loginmaininterface::InitStyle()
{
    max = false;
    mousePressed = false;
    this->location = this->geometry();

    if (m_progressBar_readAxis) {
        m_progressBar_readAxis->hide();
    }

    // 设置版本信息
    if (m_label_version_number) {
        m_label_version_number->setText(
            QString("版本信息: %1").arg(VERSION_RELEASE)
        );
    }

    initPasswordField();
    asyncInitDatabase();
    loaduser();

    if (m_comboBox_user) {
        m_comboBox_user->setEditable(false);
    }

    initHardware();
}

bool loginmaininterface::eventFilter(QObject *obj, QEvent *event)
{
    // 标题栏双击最大化由 CustomTitleBar 处理，这里不再需要
    return QObject::eventFilter(obj, event);
}

void loginmaininterface::mouseMoveEvent(QMouseEvent *e)
{
    // 拖动功能由 CustomTitleBar 处理，这里不再需要
    Q_UNUSED(e);
}

void loginmaininterface::mousePressEvent(QMouseEvent *e)
{
    // 拖动功能由 CustomTitleBar 处理，这里不再需要
    Q_UNUSED(e);
}

void loginmaininterface::mouseReleaseEvent(QMouseEvent *)
{
    // 拖动功能由 CustomTitleBar 处理，这里不再需要
}

void loginmaininterface::loaduser()
{
    if (!m_comboBox_user) return;

    QStringList newUsers;
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllUsername(newUsers);

    QSet<QString> existingItems;
    for(int i = 0; i < m_comboBox_user->count(); ++i)
        existingItems.insert(m_comboBox_user->itemText(i));

    QStringList toAdd;
    foreach(const QString& user, newUsers) {
       if(!existingItems.contains(user) && user != "hospital_name")
           toAdd.append(user);
    }

   if(!toAdd.isEmpty()) {
       m_comboBox_user->addItems(toAdd);
       m_comboBox_user->setCurrentIndex(0);
   }
}

void loginmaininterface::on_toolButton_enter_clicked()
{
    const int closeTimeLegth = 1000;
    const QString widgetTitle = "登入失败";

    if (!m_comboBox_user || !m_lineEdit_password) return;

    QString user = m_comboBox_user->currentText().trimmed();
    QString password = m_lineEdit_password->text();

    if (user.isEmpty()) {
        warn_interface::showTimeTransientWarning(widgetTitle, tr("账户名为空!"), closeTimeLegth);
        m_comboBox_user->setFocus();
        return;
    }

    if (password.isEmpty()) {
        warn_interface::showTimeTransientWarning(widgetTitle, tr("密码不能为空!"), closeTimeLegth);
        m_lineEdit_password->setFocus();
        return;
    }

    QString dbPassword = FullyAutomatedPlatelets::pinstancesqlData()->FindPassword(user);
    if (dbPassword.isNull()) {
       warn_interface::showTimeTransientWarning(widgetTitle, tr("用户不存在!"), closeTimeLegth);
       return;
    }

    if (password.toUtf8() != dbPassword.toUtf8()) {
          m_lineEdit_password->clear();
          m_lineEdit_password->setPlaceholderText(tr("密码错误，请重新输入！"));
          m_lineEdit_password->setFocus();
          return;
    }

    killTimer(mtimerconnect);
    mtimerconnect = 0;

    cglobal::g_UserName_str = user;

    quint8 equipmentKind;
    QString equipmentTypeStr;
    auto *pdata = SingletonAxis::GetInstance();
    pdata->equipmentKind(READ_OPERRAT, equipmentTypeStr);
    pdata->equipmentKind(READ_OPERRAT, equipmentKind);
    if (equipmentKind < KS600 || equipmentKind > KS1200 || equipmentTypeStr.isEmpty()) {
        equipmentKind = KS1200;//KS1200; //默认的机型
        equipmentTypeStr = KS1200STR;
        pdata->equipmentKind(WRITE_OPERAT, equipmentTypeStr);
        pdata->equipmentKind(WRITE_OPERAT, equipmentKind);
    }
    QLOG_DEBUG() << "登录仪器类型:" << equipmentTypeStr << __FUNCTION__ << __LINE__;

    QDialog::accept();
    FullyAutomatedPlatelets::mainWindow()->initStyleAll();
    return;
}

void loginmaininterface::ToReadtEquipmentTypePos(quint8 kindType, QString saveTimes)
{
    static const QMap<quint8, QPair<QString, QString>> typeMap = {
        {KS600,  {KS600STR,  tr("仪器类型: KS-600")}},
        {KS800,  {KS800STR,  tr("仪器类型: KS-800")}},
        {KS1200, {KS1200STR, tr("仪器类型: KS-1200")}}
    };

    auto it = typeMap.constFind(kindType);
    if (it == typeMap.constEnd()) {
        qWarning() << "未知设备类型：" << kindType << "，使用默认值KS-1200";
        it = typeMap.constFind(KS1200);
    }

    QString equipmentTypeStr  = it.value().first;
    if (m_label_equipmentkind) {
        m_label_equipmentkind->setText(it.value().second);
    }

    auto axis = SingletonAxis::GetInstance();
    axis->paraAxisSaveTime(WRITE_OPERAT, saveTimes);
    axis->equipmentKind(WRITE_OPERAT, equipmentTypeStr);
    axis->equipmentKind(WRITE_OPERAT, kindType);
    return;
}

void loginmaininterface::slotProgressshow(bool bWrite)
{
    if (!m_progressBar_readAxis || !m_label_reminder) return;

    if (mtotalcommed <= 0) {
        qWarning() << "Invalid total commands:" << mtotalcommed;
        m_progressBar_readAxis->setValue(0);
        m_label_reminder->setText(tr("Error: Invalid task count"));
        QTimer::singleShot(3000, m_label_reminder, &QLabel::hide);
        return;
    }

    QAtomicInt safeCompleted = m_gotcompleted.fetchAndAddRelaxed(1) + 1;

    const double progressValue = qBound(0.0,
            (static_cast<double>(safeCompleted) * 100.0) / mtotalcommed,
            100.0);

    if (!m_progressBar_readAxis->isVisible()) {
        m_progressBar_readAxis->show();
        m_label_reminder->show();
    }

    QPropertyAnimation *animation = new QPropertyAnimation(m_progressBar_readAxis, "value");
    animation->setDuration(200);
    animation->setStartValue(m_progressBar_readAxis->value());
    animation->setEndValue(progressValue);
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    const QString progressText = QString::number(progressValue, 'f', 2);
    const QString modeString = bWrite ? tr("初始写入进度:") : tr("初始读取进度:");
    m_label_reminder->setText(QString("%1 %2%").arg(modeString).arg(progressText));

    if (safeCompleted >= mtotalcommed){
       (bWrite)? wirteMachineParaProgress() : readMachineParaFinished();
    }
}

void loginmaininterface::closeTimerSerial()
{
    if (mLoadcoordinates) {
        mLoadcoordinates->CloseSerial();
        delete mLoadcoordinates;
        mLoadcoordinates = nullptr;
    }

    if (mtimerconnect != 0) {
        killTimer(mtimerconnect);
        mtimerconnect = 0;
    }
}

void loginmaininterface::wirteMachineParaProgress()
{
    QMetaObject::invokeMethod(this, [this]() {

    closeTimerSerial();

    QMessageBox::StandardButton reply = QMessageBox::information(
            this,
            tr("写入完成"),
            tr("初始写入进度已达到100%，需要重启软件使配置生效。是否立即重启？"),
            QMessageBox::Yes | QMessageBox::No
        );
        if (reply == QMessageBox::Yes) {
        qApp->quit();
        QProcess::startDetached(qApp->applicationFilePath(), QStringList());
    }
    }, Qt::QueuedConnection);
}

void loginmaininterface::readMachineParaFinished()
{
    bool readFinished = true;
    SingletonAxis::GetInstance()->sycnAxisState(WRITE_OPERAT, readFinished);

    QMetaObject::invokeMethod(this, [this]() {
        VerifyCoordinates::GetInstance()->startComparingCoordinates(true);
    }, Qt::QueuedConnection);

    closeTimerSerial();
}

void loginmaininterface::slotsetEquipmentIndex()
{
    CreatReminderWidget(1,"配置仪器类型","请先选择配置仪器类型?");
    return;
}

void loginmaininterface::CreatReminderWidget(char index, const QString &titleStr, const QString &reminderStr)
{
    if (m_pReminderExceptional != nullptr) {
        m_pReminderExceptional->activateWindow();
        m_pReminderExceptional->raise();
        return;
    }

   enum ReminderType : char {
       TypeConfigEquipment = 1,
       TypeInfo = 2,
       TypeWarning = 3,
       TypeTool = 4
   };

   m_pReminderExceptional = new CommandExceptional(index, titleStr, this);
   m_pReminderExceptional->setAttribute(Qt::WA_DeleteOnClose);
   connect(m_pReminderExceptional, &CommandExceptional::closeDel,
               this, &loginmaininterface::closeReminder);

    switch (static_cast<ReminderType>(index)) {
      case ReminderType::TypeConfigEquipment:
          setupConfigEquipmentReminder(reminderStr);
          break;

      case ReminderType::TypeTool:
          setupToolReminder(reminderStr);
          break;

      default:
          setupDefaultReminder(reminderStr);
          break;
    }
    if (index == TypeConfigEquipment || index == TypeTool) {
        m_pReminderExceptional->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
    }

    m_pReminderExceptional->show();
    update();
}

void loginmaininterface::setupConfigEquipmentReminder(const QString& reminderStr)
{
     Q_UNUSED(reminderStr);
    connect(m_pReminderExceptional, &CommandExceptional::configEquipType,
            this, [this](quint8 indexEquip) {
        emit configuredModel(indexEquip, m_bparaexit, _parasettingPath);
        closeReminder();
    });
}

void loginmaininterface::setupToolReminder(const QString& reminderStr)
{
    m_pReminderExceptional->setErrInfo(0, reminderStr);
}

void loginmaininterface::setupDefaultReminder(const QString& reminderStr)
{
    m_pReminderExceptional->setErrInfo(0, reminderStr);
}

void loginmaininterface::closeReminder()
{
    if (m_pReminderExceptional)
    {
        delete m_pReminderExceptional;
        m_pReminderExceptional = nullptr;
    }
}

void loginmaininterface::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Return){
        on_toolButton_enter_clicked();
    }
}

void loginmaininterface::timerEvent(QTimerEvent *event)
{
    if(this->mtimerconnect == event->timerId())
    {
        emit signalStart();
    }
}

void loginmaininterface::slotclosetimercon(bool _conned)
{
    if(_conned)
    {
        m_TimerRunning = false;
        killTimer(this->mtimerconnect);
        if (m_progressBar_readAxis) {
            m_progressBar_readAxis->show();
        }
    }
    else
    {
        if (m_label_equipmentkind) {
            m_label_equipmentkind->setText(QString("仪器类型: %1").arg("未联机请连接"));
        }
        if (!m_TimerRunning)
        {
            m_TimerRunning = true;
            mtimerconnect = this->startTimer(500);
        }
    }
}

void loginmaininterface::on_toolButton_exit_clicked()
{
    this->close();
}
