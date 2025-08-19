#pragma execution_character_set("utf-8")


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
    setWindowTitle(tr("登录"));
    setMouseTracking(true);
    setWindowFlags(Qt::FramelessWindowHint);


    ui->label_icon->setPixmap(QPixmap(":/Picture/suowei.png"));
    ui->label_reminder->hide();



    // 统一文件检查逻辑 判断坐标文件是否存在 判断参数配置文件是否存在
    if(checkFileExist(QCoreApplication::applicationDirPath() + "/coordinatefile.ini")){

    }
    m_bparaexit =      checkFileExist(QCoreApplication::applicationDirPath() + "/Config.ini");

    setConnectBtn();

    m_myborder = new MyBorderContainer(this);

    InitStyle();

    ui->lineEdit_password->setFocus(Qt::ActiveWindowFocusReason);
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
	SingletonAxis::GetInstance()->deleteInstance(); //del坐标

	ConsumablesOper::GetpInstance()->del_Instance(); //del试剂耗材

    QLOG_DEBUG()<<"析构登录界面"<<endl;
}

void loginmaininterface::setConnectBtn()
{
    connect(ui->toolButton_max, SIGNAL(clicked()), this, SLOT(onBtnMenuMaxClicked()));
    connect(ui->toolButton_min, SIGNAL(clicked()), this, SLOT(onBtnMenuMinClicked()));
    connect(ui->toolButton_close, SIGNAL(clicked()), this, SLOT(onBtnMenuCloseClicked()));
}



void loginmaininterface::initTitleBar()
{
    // 标题栏事件过滤器
    ui->widget_title->installEventFilter(this);

    // 设置窗口图标
    QPixmap pixmap(":/Picture/suowei.png");
    pixmap = pixmap.scaled(ui->label_icon->size(),
                          Qt::KeepAspectRatio,
                          Qt::SmoothTransformation);
    ui->label_icon->setPixmap(pixmap);
    ui->label_icon->setScaledContents(true);

    // 版本号
    ui->label_version_number->setText(
        QString("版本信息: %1").arg(VERSION_RELEASE)
    );
}

void loginmaininterface::initPasswordField()
{
    // 密码输入限制
    static QRegularExpression rx("[a-zA-Z0-9]+");
    QScopedPointer<QValidator> validator(
        new QRegularExpressionValidator(rx, this)
    );
    ui->lineEdit_password->setValidator(validator.take());
    ui->lineEdit_password->setPlaceholderText("Password");
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
    ui->lineEdit_password->setAttribute(Qt::WA_InputMethodEnabled, false);
}

void loginmaininterface::asyncInitDatabase()
{
    //初始化试剂耗材
    SingletonAxis::GetInstance()->GetpStruct();
    ConsumablesOper::GetpInstance()->iterateOverEquipmentConsumables();

    //异步加载数据库文件
    CustomCreatSql* sqldata = FullyAutomatedPlatelets::pinstancesqlData();
    QFuture<void> fut1 =  QtConcurrent::run(sqldata, &CustomCreatSql::initializeSQLTable);
    fut1.waitForFinished();
}

void loginmaininterface::initHardware()
{
    // 初始化设备坐标加载器
    if (!mLoadcoordinates) {
        mLoadcoordinates = new loadEquipmentPos(this); // 设置父对象自动管理内存

        // 使用现代Qt连接语法
        connect(this, &loginmaininterface::sig_start,
                mLoadcoordinates, &loadEquipmentPos::StatrLoad);

        connect(mLoadcoordinates, &loadEquipmentPos::closetimercon,
                this, &loginmaininterface::slotclosetimercon);


        //仪器未配置类型
        connect(mLoadcoordinates,&loadEquipmentPos::setEquipmentIndex,
                this,&loginmaininterface::slotsetEquipmentIndex);

        //仪器有型号直接读取坐标
        connect(mLoadcoordinates,&loadEquipmentPos::_whiletoReadEquipPosAixs,
                this,&loginmaininterface::ToReadtEquipmentTypePos);

        connect(this,&loginmaininterface::makesureequipment,
                mLoadcoordinates,&loadEquipmentPos::writeEquipmenttyped);

        //读取写入返回进度
        connect(mLoadcoordinates,&loadEquipmentPos::sendUpdateProgressshow,
                this,&loginmaininterface::slotProgressshow);

        connect(this,&loginmaininterface::sycnParaConfigFileSatte,
                mLoadcoordinates,&loadEquipmentPos::_sycnobtainEquipmenttyped);

        connect(mLoadcoordinates, &loadEquipmentPos::progresstotal, this, [=](int totalnum) {
            mtotalcommed = totalnum;
            //读参数配置文件存在状态和路径
            emit sycnParaConfigFileSatte(m_bparaexit,_parasettingPath);
        });

        emit sig_start();
    }
}



void loginmaininterface::InitStyle()
{
    max = false;
    mousePressed = false;
    this->location = this->geometry();
    ui->progressBar_readAxis->hide();
    ui->label_icon->setFixedSize(32,32);

    //// 初始化标题栏
    initTitleBar();

    //// 初始化密码输入框
    initPasswordField();

    //// 异步初始化数据库
    asyncInitDatabase();

    //// 加载用户数据
    _loaduser();
    ui->comboBox_user->setEditable(false);

    //// 初始化硬件设备
    initHardware();
}

//关闭 最小化 最大化 三个按钮的槽函数
void loginmaininterface::onBtnMenuCloseClicked()
{
   this->close();
}

//max location
void loginmaininterface::onBtnMenuMaxClicked()
{
    if (max)
    {
        this->setGeometry(location);
        ui->toolButton_max->setToolTip("最大化");
        ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/最大化.png"));
    }
    else
    {
        QDesktopWidget* desktop = QApplication::desktop();
        int N = desktop->screenCount();
        if (N == 2)
        {
            location = this->geometry();
            this->setGeometry(desktop->screenGeometry(1));
            ui->toolButton_max->setToolTip("还原");
            ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));

        }
        else
        {
            location = this->geometry();
            this->setGeometry(qApp->desktop()->availableGeometry());
            ui->toolButton_max->setIcon(QIcon(":/Picture/icon_title/已经最大化.png"));
            ui->toolButton_max->setToolTip("还原");
        }
    }
    max = !max;
}

bool loginmaininterface::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        this->onBtnMenuMaxClicked();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void loginmaininterface::mouseMoveEvent(QMouseEvent *e)
{
    if (mousePressed && (e->buttons() && Qt::LeftButton) && !max) {
        this->move(e->globalPos() - mousePoint);
        e->accept();
    }
}

void loginmaininterface::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        mousePressed = true;
        mousePoint = e->globalPos() - this->pos();
        e->accept();
    }
}

void loginmaininterface::mouseReleaseEvent(QMouseEvent *)
{
    mousePressed = false;
}

void loginmaininterface::onBtnMenuMinClicked()
{
    this->showMinimized();
}

//导入账户
void loginmaininterface::_loaduser()
{
    QStringList newUsers;
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllUsername(newUsers);

    // 构建现有项哈希表
    QSet<QString> existingItems;
    for(int i= 0; i < ui->comboBox_user->count(); ++i)
        existingItems.insert(ui->comboBox_user->itemText(i));

    // 过滤新项
    QStringList toAdd;
    foreach(const QString& user, newUsers) {
       if(!existingItems.contains(user) && user != "hospital_name")
           toAdd.append(user);
    }

    // 批量添加
   if(!toAdd.isEmpty()) {
       ui->comboBox_user->addItems(toAdd); // [[3,5,11]]
       ui->comboBox_user->setCurrentIndex(0);
   }
}

void loginmaininterface::on_toolButton_enter_clicked()
{
    QString user = ui->comboBox_user->currentText().trimmed();
    QString password = ui->lineEdit_password->text();

    // 校验用户名
    if (user.isEmpty()) {
        warn_interface::showTransientWarning(tr("登入失败"), tr("账户名为空!")); // 封装警告弹窗
        ui->comboBox_user->setFocus();
        return;
    }

    if (password.isEmpty()) {
        warn_interface::showTransientWarning(tr("登入失败"), tr("密码不能为空!"));
        ui->lineEdit_password->setFocus();
        return;
    }

    // 查询数据库密码
    QString dbPassword = FullyAutomatedPlatelets::pinstancesqlData()->FindPassword(user);
    if (dbPassword.isNull()) {
       warn_interface::showTransientWarning(tr("登入失败"), tr("用户不存在!"));
       return;
    }

    // 校验密码
    if (password.toUtf8() !=  dbPassword.toUtf8()) {
          ui->lineEdit_password->clear();
          ui->lineEdit_password->setPlaceholderText(tr("密码错误，请重新输入！"));
          ui->lineEdit_password->setFocus();
          return;
    }


    // 登录成功逻辑
    cglobal::g_UserName_str = user;


    // 初始化设备类型
    quint8 equipmentKind;
    QString equipmentTypeStr;
    auto *pdata = SingletonAxis::GetInstance();
    pdata->equipmentKind(READ_OPERRAT, equipmentTypeStr);
    pdata->equipmentKind(READ_OPERRAT, equipmentKind);
    if (equipmentKind < KS600 || equipmentKind > KS1200 || equipmentTypeStr.isEmpty()) {
        equipmentKind = KS1200;
        equipmentTypeStr = KS1200STR;
        pdata->equipmentKind(WRITE_OPERAT, equipmentTypeStr);
        pdata->equipmentKind(WRITE_OPERAT, equipmentKind);
    }
    QLOG_DEBUG() << "登录仪器类型:" << equipmentTypeStr << __FUNCTION__ << __LINE__;

    // 关闭登录窗口并初始化主界面
    QDialog::accept();
    FullyAutomatedPlatelets::mainWindow()->init_style_all();
    return;
}


//配置仪器类型
void loginmaininterface::ToReadtEquipmentTypePos(quint8 kindType, QString saveTimes)
{
    // 定义类型映射表
    static const QMap<quint8, QPair<QString, QString>> typeMap = {
        {KS600,  {KS600STR,  tr("仪器类型: KS-600")}},
        {KS800,  {KS800STR,  tr("仪器类型: KS-800")}},
        {KS1200, {KS1200STR, tr("仪器类型: KS-1200")}}
    };

    // 获取设备类型信息，默认KS1200
    auto it = typeMap.constFind(kindType);
    if (it == typeMap.constEnd()) {
        qWarning() << "未知设备类型：" << kindType << "，使用默认值KS-1200";
        it = typeMap.constFind(KS1200);
    }

    QString equipmentTypeStr  = it.value().first;
    ui->label_equipmentkind->setText(it.value().second);


    // 通过单例保存参数
    auto axis = SingletonAxis::GetInstance();
    axis->paraAxisSaveTime(WRITE_OPERAT, saveTimes);
    axis->equipmentKind(WRITE_OPERAT, equipmentTypeStr);
    axis->equipmentKind(WRITE_OPERAT, kindType);
    return;
}



//读写返回进度
void loginmaininterface::slotProgressshow(bool bWrite)
{
    // 边界条件保护
    if (mtotalcommed <= 0) {
        qWarning() << "Invalid total commands:" << mtotalcommed;
        ui->progressBar_readAxis->setValue(0);
        ui->label_reminder->setText(tr("Error: Invalid task count"));
        QTimer::singleShot(3000, ui->label_reminder, &QLabel::hide);
        return;
    }

    // 原子操作保证进度计数安全
    QAtomicInt safeCompleted = m_gotcompleted.fetchAndAddRelaxed(1) + 1;

    // 精确浮点进度计算
    const double progressValue = qBound(0.0,
            (static_cast<double>(safeCompleted) * 100.0) / mtotalcommed,
            100.0); // 使用qBound防止溢出

    // 界面更新组
    QProgressBar *progressBar = ui->progressBar_readAxis;
    QLabel *statusLabel = ui->label_reminder;

    // 首次显示时初始化
    if (!progressBar->isVisible()) {
        progressBar->show();
        statusLabel->show();
    }

    // 平滑进度动画（可选）
    QPropertyAnimation *animation = new QPropertyAnimation(progressBar, "value");
    animation->setDuration(200);
    animation->setStartValue(progressBar->value());
    animation->setEndValue(progressValue);
    animation->start(QAbstractAnimation::DeleteWhenStopped);


    // 状态文本生成
    const QString progressText = QString::number(progressValue, 'f', 2);
    const QString modeString = bWrite ? tr("Writing Progress:") : tr("Reading Progress:");
    statusLabel->setText(QString("%1 %2%").arg(modeString).arg(progressText));



    // 完成状态处理
    if (safeCompleted >= mtotalcommed) {
        // 异步清理策略
        QMetaObject::invokeMethod(this, [this]() {
            // 状态同步
			bool finish = true;
            SingletonAxis::GetInstance()->sycnAxisState(WRITE_OPERAT, finish);

            // 资源安全释放
            if (mLoadcoordinates) {
                mLoadcoordinates->CloseSerial();
                delete mLoadcoordinates;
                mLoadcoordinates = nullptr;
            }

            // 定时器安全停止
            //if (this->timerId() == mtimerconnect)
			{
                killTimer(mtimerconnect);
                mtimerconnect = 0;
            }


            // 完成反馈
            //Q_EMIT progressCompleted(bWrite);
           /* QTimer::singleShot(1000, [=](){
                ui->progressBar_readAxis->hide();
                ui->label_reminder->hide();
            });*/

            }, Qt::QueuedConnection);
        }
}


void loginmaininterface::slotsetEquipmentIndex()
{
    CreatReminderWidget(1,"配置仪器类型","请先选择配置仪器类型?");
    return;
}

void loginmaininterface::CreatReminderWidget(char index,QString titleStr,QString reminderStr)
{
   if (nullptr == m_pReminderExceptional)
    {
        m_pReminderExceptional = new CommandExceptional(index,titleStr,this);
        m_pReminderExceptional->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_pReminderExceptional,&CommandExceptional::closeDel,this,&loginmaininterface::closeReminder);
        if(index == 1)
        {
            connect(m_pReminderExceptional,&CommandExceptional::configEquipType,this,[=](quint8 indexEquip)
            {
                //先设置仪器型号再写坐标
                emit makesureequipment(indexEquip,m_bparaexit,_parasettingPath);

                closeReminder();
                return;
            });
        }
        else
        {
            m_pReminderExceptional->setErrInfo(0,reminderStr);
        }
        if(index == 4 || index == 1)
            m_pReminderExceptional->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
        m_pReminderExceptional->show();
        update();
    }
    return;
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
        emit sig_start();
    }
}

void loginmaininterface::slotclosetimercon(bool _conned)
{
    if(_conned)
    {
		m_TimerRunning = false;
        killTimer(this->mtimerconnect);
        ui->progressBar_readAxis->show();
    }
    else
    {
       ui->label_equipmentkind->setText(QString("仪器类型: %1").arg("未联机请连接"));
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
