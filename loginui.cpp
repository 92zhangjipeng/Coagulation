#pragma execution_character_set("utf-8")
#include "loginui.h"
#include "ui_loginui.h"
#include "instrumentcoordinatetable.h"
#include  <QtConcurrent>
#include  <QDateTime>
#include <operclass/fullyautomatedplatelets.h>

#define TITLE_BACK_COLOR  QColor(205, 206, 200)

LoginUi::LoginUi(QWidget *parent) :
    QWidget(parent),
    mViewPassword(false),
    mAxisFinished(false),
    ui(new Ui::LoginUi)
{
    ui->setupUi(this);
    ui->progressBar_readAxis->hide();
    ui->label_version->setText(VERSION_RELEASE);
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->lineEdit_passwod->installEventFilter(this);
    ui->comboBoxUser->installEventFilter(this);
    setFocusPolicy(Qt::StrongFocus);
    ui->TitleName->setAttribute(Qt::WA_TranslucentBackground);


    QImage img;
    img.load(":/Picture/suowei.png");
    ui->Titleicon->setPixmap(QPixmap::fromImage(img));
    //QUIUtils::QLabeldisplayIcon(ui->TitleName,":/Picture/suowei.png","登录",0);
    //QUIUtils::QLabeldisplayIcon(ui->label_User,":/Picture/SetPng/user.png","登录用户:",0);
    //QUIUtils::QLabeldisplayIcon(ui->label_Password, ":/Picture/SetPng/PassWord.png", "登录密码:",0);
    ui->comboBoxUser->setEditable(false);
    //输入密码添加可视化按钮
    QRegExp rx("[a-zA-Z0-9]+$");
    QValidator *validator = new QRegExpValidator(rx, this);
    ui->lineEdit_passwod->setValidator(validator);
    ui->lineEdit_passwod->setAttribute(Qt::WA_InputMethodEnabled,false); //禁止输入法
    ui->lineEdit_passwod->setEchoMode(QLineEdit::Password);
    ui->lineEdit_passwod->setPlaceholderText("请输入密码");
    delete validator;
    validator = nullptr;


    if(mpLoadPos == nullptr)
    {
       mpLoadPos = new loadEquipmentPos;


       //connect(mpLoadPos,&loadEquipmentPos::SigEquipmentType,this,&LoginUi::slotEquipmentType);




       //发送数据
       connect(this,&LoginUi::writedataToEquip, mpLoadPos,&loadEquipmentPos::slotwritedataToEquip);

       emit this->sig_start();
    }

    //初始化试剂耗材
    //ConsumablesOper::GetpInstance()->iterateOverEquipmentConsumables();

    //异步加载数据库文件
    CustomCreatSql* sqldata = FullyAutomatedPlatelets::pinstancesqlData();
    QtConcurrent::run(sqldata, &CustomCreatSql::initializeSQLTable);

    Init_login();
}

LoginUi::~LoginUi()
{
    if(m_pmainControlApp)
	{
        delete m_pmainControlApp;
        m_pmainControlApp = nullptr;
	}
	if (mpLoadPos)
	{
		mpLoadPos->CloseSerial();
		delete mpLoadPos;
		mpLoadPos = NULL;
	}
	closeReminder();

    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    SingletonAxis::GetInstance()->deleteInstance();

    //初始化试剂耗材
    //ConsumablesOper *pReagentTotal = ConsumablesOper::GetpInstance();
    ConsumablesOper::GetpInstance()->del_Instance();

    delete ui;
}

void LoginUi::slotShowProg(bool bshow)
{
    if(!bshow)
    {
        CreatReminderWidget(0,"同步坐标失败","仪器未连接请连接!");
        ui->label_text->setText("未联机");
        mtimerconnect = this->startTimer(1000);
    }
    else
    {
       this->killTimer(mtimerconnect);
       ui->progressBar_readAxis->show();
    }
    return;
}
void LoginUi::timerEvent(QTimerEvent *event)
{
    if(this->mtimerconnect == event->timerId())
    {
        emit this->sig_start();
    }
}

void LoginUi::CreatReminderWidget(char index,QString titleStr,QString reminderStr)
{
   if (nullptr == m_pReminderExceptional)
    {
        m_pReminderExceptional = new CommandExceptional(index,titleStr,this);
        m_pReminderExceptional->setAttribute(Qt::WA_DeleteOnClose);
        connect(m_pReminderExceptional,&CommandExceptional::closeDel,this,&LoginUi::closeReminder);
        if(index == 1)
        {

        }
        else
        {
            m_pReminderExceptional->setErrInfo(0,reminderStr);
        }
        if(index == 4 || index == 1)
            m_pReminderExceptional->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        m_pReminderExceptional->show();
		update();
    }
    return;
}

void LoginUi::closeReminder()
{
    if(m_pReminderExceptional)
    {
        delete m_pReminderExceptional;
        m_pReminderExceptional = nullptr;
    }
}

/*切换用户按钮*/
void LoginUi::Init_login(void)
{
    QStringList AddItems;
    AddItems.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllUsername(AddItems);
    QStringList tmpList = AddItems;
    for(auto itemstr : tmpList)
    {
        for(int i = 0 ; i < ui->comboBoxUser->count(); i++)
        {
           QString itemstrtmp = ui->comboBoxUser->itemText(i);
           if(itemstrtmp == itemstr)
           {
               AddItems.removeOne(itemstrtmp);
           }
        }
    }
    for(auto itemuser : AddItems)
    {
        if(itemuser != "hospital_name")
           ui->comboBoxUser->addItem(itemuser);
    }
    ui->comboBoxUser->setCurrentIndex(0);
    return;
}




void LoginUi::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    QColor colorBackGround = TITLE_BACK_COLOR;
    p.setRenderHint(QPainter::Antialiasing);//抗锯齿
    p.setBrush(colorBackGround);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(0, 0, width()- 1, height() - 1, 2, 2);
}

void LoginUi::on_toolClose_clicked()
{
    this->close();
}

void LoginUi::on_pushButtonCancel_clicked()
{
    this->close();
}

//进入主界面
void LoginUi::enterMainWidget()
{
    this->killTimer(mtimerconnect);
    if (mpLoadPos){
        mpLoadPos->CloseSerial();
        delete mpLoadPos;
        mpLoadPos = NULL;
    }

    bool sycnflag = false;
    SingletonAxis::GetInstance()->sycnAxisState(READ_OPERRAT,sycnflag);
    if(!sycnflag)
    {
        //未同步坐标就使用本地文件坐标
        QString defaultpath = QApplication::applicationDirPath() + "/coordinateFile.txt";//默认路径
		QFileInfo filepath(defaultpath);
        if(filepath.isFile())
        {
            bool bsycn = SingletonAxis::GetInstance()->importtCoordinate(defaultpath);
            SingletonAxis::GetInstance()->sycnAxisState(WRITE_OPERAT,bsycn);
        }
        else
        {
           QString errstr = QString("%1%2").arg("坐标文件不存在").arg(defaultpath);
           CreatReminderWidget(4,"登入失败",errstr);
           return;
        }
    }


    if(m_pmainControlApp == nullptr)
    {
        quint8 equipmentKind = 0;
        QString equipmentTypeStr = "";
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmentKind);
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,equipmentTypeStr);
        if(equipmentKind > KS1200) equipmentKind = KS1200;
        m_pmainControlApp = new MainWindow();
		connect(m_pmainControlApp, SIGNAL(ShowPassWord()), this, SLOT(ShowIt()));
        connect(m_pmainControlApp, SIGNAL(CloseSignals()), this, SLOT(Deleetmain()));
        QLOG_TRACE()<<"仪器类型"<<equipmentTypeStr<<endl;
        m_pmainControlApp->show();
    }
    this->close();
    return;
}

void LoginUi::on_pushButtonLogin_clicked()
{
    QString UserName = ui->comboBoxUser->currentText();     /*用户名*/
    QString EditPassword = ui->lineEdit_passwod->text();    /*密码*/
    cglobal::g_UserName_str = UserName;                     /*登录的账户*/
    QLOG_ERROR()<<"登录用户账号["<<cglobal::g_UserName_str<<"]"<<endl;
    QString SQLPassword =  FullyAutomatedPlatelets::pinstancesqlData()->FindPassword(UserName);
    if(cglobal::g_UserName_str.isEmpty())
    {
        CreatReminderWidget(0,"登入失败","账户名为空!");
    }
    else if(SQLPassword.isEmpty())
    {
        ui->lineEdit_passwod->setPlaceholderText("密码错误!");
    }
    if(EditPassword == SQLPassword)
    {
        enterMainWidget();
    }
    else
    {
        ui->lineEdit_passwod->setPlaceholderText("密码错误!");
        ui->lineEdit_passwod->clear();
        QLOG_TRACE() << "登入账号输入密码错误";
    }
    return;
}

void LoginUi::Deleetmain()
{
    if(m_pmainControlApp != nullptr)
    {
        delete m_pmainControlApp;
        m_pmainControlApp = nullptr;
    }
    return;
}

void LoginUi::ShowIt()
{
    Init_login();
    this->show();
    return;
}
void LoginUi::mousePressEvent(QMouseEvent *event)
{
    this->windowsPos = this->pos();       // 获得部件当前位置
    this->mousePos = event->globalPos();  // 获得鼠标位置
    this->dPos = mousePos - windowsPos;   // 移动后部件所在的位置
}
void LoginUi::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

void LoginUi::on_comboBoxUser_currentTextChanged(const QString &/*arg1*/)
{
    ui->lineEdit_passwod->clear();
}

void LoginUi::keyPressEvent(QKeyEvent *event)
{
    //if (event->modifiers() == Qt::ControlModifier)////如果按下了ctrl键
    if (event->key() == Qt::Key_Return)
    {
        on_pushButtonLogin_clicked();
    }
}


void LoginUi::on_pushButton_show_clicked()
{
    VisualizationPassword();
}

/*可视化密码*/
void LoginUi::VisualizationPassword()
{
    if(false == mViewPassword)
    {
        mViewPassword = true;
        ui->pushButton_show->setIcon(QIcon(":/Picture/SetPng/passwodCanSee.png"));
        ui->lineEdit_passwod->setEchoMode(QLineEdit::Normal);
    }
    else
    {
         mViewPassword = false;
         ui->pushButton_show->setIcon(QIcon(":/Picture/SetPng/passwodNotSee.png"));
         ui->lineEdit_passwod->setEchoMode(QLineEdit::Password);
    }
}

bool LoginUi::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->lineEdit_passwod)
    {
        if(QEvent::FocusIn == event->type())
        {

        }
    }
    if(event->type() == QEvent::MouseButtonPress)
    {
        if(obj == ui->comboBoxUser)
        {
            //处理鼠标点击事件
            QComboBox * combobox = qobject_cast<QComboBox *>(obj);
            combobox->clear();
            QStringList AddItems;
            AddItems.clear();
            FullyAutomatedPlatelets::pinstancesqlData()->FindAllUsername(AddItems);
            AddItems.removeOne("hospital_name");
            combobox->addItems(AddItems);
        }
    }
    return QWidget::eventFilter(obj, event);
}





void configAxisPoint(QPoint &input,uint x_,uint y_)
{
    input.setX(x_);
    input.setY(y_);
}
//配置仪器类型
void LoginUi::slotEquipmentType(bool bhadTyped, quint8 kindType, QString saveTimes)
{
    bool bsycnfinish = false;
    QString equipmentTypeStr = "";
    switch(kindType)
    {
        case KS600:  equipmentTypeStr = KS600STR; break;
        case KS800:  equipmentTypeStr = KS800STR; break;
        case KS1200: equipmentTypeStr = KS1200STR; break;
        default:     equipmentTypeStr = KS1200STR; break;
    }
    ui->label_text->setWordWrap(true);
    ui->label_text->setText(equipmentTypeStr);
    update();

    SingletonAxis::GetInstance()->paraAxisSaveTime(WRITE_OPERAT,saveTimes);
    SingletonAxis::GetInstance()->equipmentKind(WRITE_OPERAT,equipmentTypeStr);
    SingletonAxis::GetInstance()->equipmentKind(WRITE_OPERAT,kindType);
    SingletonAxis::GetInstance()->sycnAxisState(WRITE_OPERAT,bsycnfinish); //坐标同步完成标志
    if(bhadTyped == false)
    {
        QLOG_DEBUG()<<"配置仪器仪器类型后配置坐标:"<<kindType<<endl;
        initEquipmentAxix(kindType);
    }
    else
    {
        mtotalcommed = 16;//读取坐标
        emit this->start_ReadAxispos();
    }
}
//读写返回进度
void LoginUi::slotBackAxisProg(bool bwritr,quint8 Num)
{
    if(Num == 0)
    {
        ui->progressBar_readAxis->show();
        ui->progressBar_readAxis->setFormat(QString("仪器坐标进度:%1%").arg(0));
        ui->progressBar_readAxis->setValue(0);
    }
    else
    {
        uint provalue = (Num +1)*100/mtotalcommed;
        ui->progressBar_readAxis->setValue(provalue);
        ui->progressBar_readAxis->setFormat(QString("仪器坐标进度:%1%").arg(provalue));
        if(provalue == 100)
        {
            if(bwritr == READ_OPERRAT)
            {
                LoginUi::mAxisFinished = true;
            }
            else
            {
                configLocPosdata();
            }
            bool bsycnfinish = true;
            SingletonAxis::GetInstance()->sycnAxisState(WRITE_OPERAT,bsycnfinish); //设置本地坐标同步完成标志

            //QString defaultpath = QApplication::applicationDirPath() + "/coordinateFile.txt";




            //SingletonAxis::GetInstance()->coordinateWriteToFile(defaultpath);     //更新默认文件坐标

        }
    }
}



//读取到仪器内坐标配置到内存本地发散
void LoginUi::configLocPosdata()
{
    QPoint firstpos(0,0);
    quint8 Equipkind = 0;
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    //试剂
    QMap<quint8,QPoint> ReagentZoneOffsetKitsPin;
    ReagentZoneOffsetKitsPin.clear();
	SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,0,firstpos);
    QUIUtils::CreatReagArsOtherAxis(firstpos,ReagentZoneOffsetKitsPin);
    auto itmap = ReagentZoneOffsetKitsPin.begin();
    while(itmap != ReagentZoneOffsetKitsPin.end())
    {
		SingletonAxis::GetInstance()->reagetZoneAxisPos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }
    //血样区
    QMap<quint8,QPoint> bloodSampleZone;
    bloodSampleZone.clear();
	SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,Equipkind);
	SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,0,firstpos);
    QUIUtils::creatBloodSampleAxis(Equipkind, firstpos, bloodSampleZone);
    itmap = bloodSampleZone.begin();
    while(itmap != bloodSampleZone.end())
    {
		SingletonAxis::GetInstance()->bloodSampleZonePos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }
    //测试杯区
    if(Equipkind == KS600)
    {
        for(int i = 0; i < 2; i++)
        {
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_HANDS_INDEX,testTubeZoneoffsetHands[i]);
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_BLOOD_INDEX,testTubeZoneoffsetBloodPin[i]);
        }
    }
    else if(Equipkind == KS800)
    {
        for(int i = 0; i < 3; i++)
        {
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_HANDS_INDEX,testTubeZoneoffsetHands[i]);
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_BLOOD_INDEX,testTubeZoneoffsetBloodPin[i]);
        }
    }
    else if(Equipkind == KS1200)
    {
        for(int i = 0; i < 4; i++)
        {
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_HANDS_INDEX,testTubeZoneoffsetHands[i]);
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i+(i*59),MOTOR_BLOOD_INDEX,testTubeZoneoffsetBloodPin[i]);
        }
    }
    QMap<quint8,QPoint> testtubeZoneAxisVec_offset_Hands,testtubeZoneAxisVec_offset_BloodPin;
    testtubeZoneAxisVec_offset_Hands.clear();
    testtubeZoneAxisVec_offset_BloodPin.clear();
    QUIUtils::creatTeatTubeAxiis(MOTOR_HANDS_INDEX,Equipkind,testtubeZoneAxisVec_offset_Hands,testTubeZoneoffsetHands,4);
    QUIUtils::creatTeatTubeAxiis(MOTOR_BLOOD_INDEX,Equipkind,testtubeZoneAxisVec_offset_BloodPin,testTubeZoneoffsetBloodPin,4);
    itmap = testtubeZoneAxisVec_offset_Hands.begin();
    while(itmap != testtubeZoneAxisVec_offset_Hands.end())
    {
        if(itmap.key()%60 != 0)
        {
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_HANDS_INDEX,itmap.value());
        }
        itmap++;
    }

    itmap = testtubeZoneAxisVec_offset_BloodPin.begin();
    while(itmap != testtubeZoneAxisVec_offset_BloodPin.end())
    {
        if(itmap.key()%60 != 0)
        {
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_BLOOD_INDEX,itmap.value());
        }
        itmap++;
    }
    LoginUi::mAxisFinished = true;
    return;
}


void LoginUi::slotsetEquipmentIndex()
{
    CreatReminderWidget(1,"配置仪器类型","请先选择配置仪器类型?");
    return;
}



/*** 未配置仪器类型及其坐标参数初始化坐标 ***/
void  LoginUi::initEquipmentAxix(const quint8 indexType)
{
    QPoint OriginPos(0,0); //原点坐标
    QPoint cleanZoneoffsetBloodPin(0,0);//清洗液offset血样针
    QPoint cleanZoneoffsetKttsPin(0,0); //清洗液offset试剂针
    QPoint throwsTubePos(0,0);          //弃杯孔坐标
    QPoint firstReagentZone(0,0);   //试剂区第一个坐标
    QMap<quint8,QPoint> ReagentZoneOffsetKitsPin;//试剂区到试剂针坐标
    QMap<quint8, QPoint> testChn_offset_Hands;
    QMap<quint8, QPoint> testChn_offset_KitsPin;
    QPoint firstBloodZoneAxis(0,0);
    QMap<quint8, QPoint>  BloodZoneAxisVec;

    QMap<quint8, QPoint>  testtubeZoneAxisVec_offset_Hands;
    QMap<quint8, QPoint>  testtubeZoneAxisVec_offset_BloodPin;
    switch(indexType)
    {
        case KS600:
                configAxisPoint(OriginPos,50,24);
                configAxisPoint(throwsTubePos,5245,340);
                configAxisPoint(cleanZoneoffsetBloodPin,180,634);
                configAxisPoint(cleanZoneoffsetKttsPin,170,850);
                configAxisPoint(firstReagentZone,60,1347);
                QUIUtils::CreatReagArsOtherAxis(firstReagentZone,ReagentZoneOffsetKitsPin);
                creatTestChnAixisPos(MOTOR_HANDS_INDEX,KS600,testChn_offset_Hands);
                creatTestChnAixisPos(MOTOR_REAGNET_INDEX,KS600,testChn_offset_KitsPin);
                configAxisPoint(firstBloodZoneAxis,893,2483);
                QUIUtils::creatBloodSampleAxis(KS600, firstBloodZoneAxis,BloodZoneAxisVec);
                //试管区
                configAxisPoint(testTubeZoneoffsetHands[0],895,1059);
                configAxisPoint(testTubeZoneoffsetHands[1],2097,1057);
                //configAxisPoint(testTubeZoneoffsetHands[2],3293,1055);
                //configAxisPoint(testTubeZoneoffsetHands[3],4493,1053);

                configAxisPoint(testTubeZoneoffsetBloodPin[0],895,580);
                configAxisPoint(testTubeZoneoffsetBloodPin[1],2097,580);
                //configAxisPoint(testTubeZoneoffsetBloodPin[2],3295,570);
                //configAxisPoint(testTubeZoneoffsetBloodPin[3],4485,570);
        break;
        case KS800:
                configAxisPoint(OriginPos,50,24);
                configAxisPoint(throwsTubePos,5245,340);
                configAxisPoint(cleanZoneoffsetBloodPin,180,634);
                configAxisPoint(cleanZoneoffsetKttsPin,170,850);
                configAxisPoint(firstReagentZone,60,1347);
                QUIUtils::CreatReagArsOtherAxis(firstReagentZone,ReagentZoneOffsetKitsPin);
                creatTestChnAixisPos(MOTOR_HANDS_INDEX,KS800,testChn_offset_Hands);
                creatTestChnAixisPos(MOTOR_REAGNET_INDEX,KS800,testChn_offset_KitsPin);
                configAxisPoint(firstBloodZoneAxis,893,2483);
                 QUIUtils::creatBloodSampleAxis(KS800, firstBloodZoneAxis,BloodZoneAxisVec);
                 //试管区
                 configAxisPoint(testTubeZoneoffsetHands[0],895,1059);
                 configAxisPoint(testTubeZoneoffsetHands[1],2097,1057);
                 configAxisPoint(testTubeZoneoffsetHands[2],3293,1055);
                 //configAxisPoint(testTubeZoneoffsetHands[3],4493,1053);

                 configAxisPoint(testTubeZoneoffsetBloodPin[0],895,580);
                 configAxisPoint(testTubeZoneoffsetBloodPin[1],2097,580);
                 configAxisPoint(testTubeZoneoffsetBloodPin[2],3295,570);
                 //configAxisPoint(testTubeZoneoffsetBloodPin[3],4485,570);
        break;
        case KS1200:
                configAxisPoint(OriginPos,50,24);
                configAxisPoint(throwsTubePos,5245,340);
                configAxisPoint(cleanZoneoffsetBloodPin,180,634);
                configAxisPoint(cleanZoneoffsetKttsPin,170,850);
                configAxisPoint(firstReagentZone,60,1347);

                QUIUtils::CreatReagArsOtherAxis(firstReagentZone,ReagentZoneOffsetKitsPin);

                creatTestChnAixisPos(MOTOR_HANDS_INDEX,KS1200,testChn_offset_Hands);

                creatTestChnAixisPos(MOTOR_REAGNET_INDEX,KS1200,testChn_offset_KitsPin);

                configAxisPoint(firstBloodZoneAxis,893,2483);

                QUIUtils::creatBloodSampleAxis(KS1200, firstBloodZoneAxis,BloodZoneAxisVec);
                //试管区
                configAxisPoint(testTubeZoneoffsetHands[0],895,1059);
                configAxisPoint(testTubeZoneoffsetHands[1],2097,1057);
                configAxisPoint(testTubeZoneoffsetHands[2],3293,1055);
                configAxisPoint(testTubeZoneoffsetHands[3],4493,1053);

                configAxisPoint(testTubeZoneoffsetBloodPin[0],895,580);
                configAxisPoint(testTubeZoneoffsetBloodPin[1],2097,580);
                configAxisPoint(testTubeZoneoffsetBloodPin[2],3295,570);
                configAxisPoint(testTubeZoneoffsetBloodPin[3],4485,570);
        break;
    default:break;
    }
    QUIUtils::creatTeatTubeAxiis(MOTOR_HANDS_INDEX,indexType,testtubeZoneAxisVec_offset_Hands,testTubeZoneoffsetHands,4);
    QUIUtils::creatTeatTubeAxiis(MOTOR_BLOOD_INDEX,indexType,testtubeZoneAxisVec_offset_BloodPin,testTubeZoneoffsetBloodPin,4);
    //写入到保存结构
    SingletonAxis::GetInstance()->originPos(WRITE_OPERAT,OriginPos);
    SingletonAxis::GetInstance()->throwTubeHolePos(WRITE_OPERAT,throwsTubePos);
    SingletonAxis::GetInstance()->cleanZoneAxisPos(WRITE_OPERAT,MOTOR_BLOOD_INDEX,cleanZoneoffsetBloodPin);
    SingletonAxis::GetInstance()->cleanZoneAxisPos(WRITE_OPERAT,MOTOR_REAGNET_INDEX,cleanZoneoffsetKttsPin);
    auto itmap = ReagentZoneOffsetKitsPin.begin();
    while(itmap != ReagentZoneOffsetKitsPin.end())
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }
    itmap = testChn_offset_Hands.begin();
    while(itmap != testChn_offset_Hands.end())
    {
        SingletonAxis::GetInstance()->chnZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_HANDS_INDEX,itmap.value());
        itmap++;
    }
    itmap = testChn_offset_KitsPin.begin();
    while(itmap != testChn_offset_KitsPin.end())
    {
        SingletonAxis::GetInstance()->chnZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_REAGNET_INDEX,itmap.value());
        itmap++;
    }

    itmap = BloodZoneAxisVec.begin();
    while(itmap != BloodZoneAxisVec.end())
    {
        SingletonAxis::GetInstance()->bloodSampleZonePos(WRITE_OPERAT,itmap.key(),itmap.value());
        itmap++;
    }
    itmap = testtubeZoneAxisVec_offset_Hands.begin();
    while(itmap != testtubeZoneAxisVec_offset_Hands.end())
    {
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_HANDS_INDEX,itmap.value());
        itmap++;
    }

    itmap = testtubeZoneAxisVec_offset_BloodPin.begin();
    while(itmap != testtubeZoneAxisVec_offset_BloodPin.end())
    {
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(WRITE_OPERAT,itmap.key(),MOTOR_BLOOD_INDEX,itmap.value());
        itmap++;
    }
    mtotalcommed = 16;
    emit this->axisInitComplete(6); //开始缓存坐标
    return;
}


void  LoginUi::creatTestChnAixisPos(int indexZ,int equipType, QMap<quint8,QPoint> &ChnAxispos)
{
    QPoint KttsPinAxis_bac;
    QPoint ChnHandsAxis_bac;
    QPoint tmpAxis(0,0);
    if(indexZ == MOTOR_HANDS_INDEX)
    {
        switch(equipType)
        {
            case KS600:
                        configAxisPoint(ChnHandsAxis_bac,879,330);
                        for(int f = 0; f < 4; f++)
                        {
                           tmpAxis.setX(ChnHandsAxis_bac.x() + f*350);
                           tmpAxis.setY(ChnHandsAxis_bac.y());
                           ChnAxispos.insert(f,tmpAxis);
                        }
            break;
            case KS800:
                        configAxisPoint(ChnHandsAxis_bac,879,330);
                        for(int f = 0; f < 8; f++)
                        {
                           tmpAxis.setX(ChnHandsAxis_bac.x() + f*350);
                           tmpAxis.setY(ChnHandsAxis_bac.y());
                           ChnAxispos.insert(f,tmpAxis);
                        }
            break;
            case KS1200:
                        configAxisPoint(ChnHandsAxis_bac,879,330);
                        for(int f = 0; f < 12; f++)
                        {
                           tmpAxis.setX(ChnHandsAxis_bac.x() + f*350);
                           tmpAxis.setY(ChnHandsAxis_bac.y());
                           ChnAxispos.insert(f,tmpAxis);
                        }
            break;
            default:
                break;
        }
    }
    else if(indexZ == MOTOR_REAGNET_INDEX)
    {
        switch(equipType)
        {
            case KS600:
                        configAxisPoint(KttsPinAxis_bac,879,97);
                        for(int f = 0; f < 4; f++)
                        {
                           tmpAxis.setX(KttsPinAxis_bac.x() + f*350);
                           tmpAxis.setY(KttsPinAxis_bac.y());
                           ChnAxispos.insert(f,tmpAxis);
                        }
            break;
            case KS800:
                        configAxisPoint(KttsPinAxis_bac,879,97);
                        for(int f = 0; f < 8; f++)
                        {
                           tmpAxis.setX(KttsPinAxis_bac.x() + f*350);
                           tmpAxis.setY(KttsPinAxis_bac.y());
                           ChnAxispos.insert(f,tmpAxis);
                        }
            break;
            case KS1200:
                        configAxisPoint(KttsPinAxis_bac,879,97);
                        for(int f = 0; f < 12; f++)
                        {
                           tmpAxis.setX(KttsPinAxis_bac.x() + f*350);
                           tmpAxis.setY(KttsPinAxis_bac.y());
                           ChnAxispos.insert(f,tmpAxis);
                        }
            break;
            default:
                break;
        }
    }
}
