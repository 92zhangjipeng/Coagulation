#pragma execution_character_set("utf-8")

#include "Delegate.h"
#include "globaldata.h"
#include "loginui.h"
#include "progressbar.h"
#include "qualitycontrol.h"
#include "ui_qualitycontrol.h"
#include "warn_interface.h"
#include <QPainter>
#include <QStyledItemDelegate>
#include <QScreen>         // 包含QScreen头文件
#include <QGuiApplication> // 包含QGuiApplication头文件
#include <QWidget> 
#include <math.h>       /* fmod */
#include <unordered_map>
#include <QtConcurrent>
#include <QStandardItemModel>
#include <operclass/fullyautomatedplatelets.h>
#include <suoweiFileManager/filemanager.h>
#include "custom_style/custombutton.h"

const QMap<quint8, QString> QualityControl::COLUMN_CONFIG_MAP = {
    { static_cast<quint8>(TableIndex::AA_),       "AAbottleLimit" },
    { static_cast<quint8>(TableIndex::ADP_),     "ADPbottleLimit" },
    { static_cast<quint8>(TableIndex::EPI_),      "EPIbottleLimit" },
    { static_cast<quint8>(TableIndex::COL_),      "COLbottleLimit" },
    { static_cast<quint8>(TableIndex::RIS_),     "RISbottleLimit" },
    { static_cast<quint8>(TableIndex::S1_),      "S1bottleLimit" },
    { static_cast<quint8>(TableIndex::S2_),      "S2bottleLimit" },
    { static_cast<quint8>(TableIndex::TESTCUPS_),"tubeTarybottleLimit" }
};


const QMap<int,quint8> QualityControl::ITEM_REAGENT_MAP = {
    {INDEX_AA_CONSUMABLE,  static_cast<quint8>(TableIndex::AA_)},
    {INDEX_ADP_CONSUMABLE,static_cast<quint8>(TableIndex::ADP_)},
    {INDEX_EPI_CONSUMABLE,static_cast<quint8>(TableIndex::EPI_)},
    {INDEX_COL_CONSUMABLE,static_cast<quint8>(TableIndex::COL_)},
    {INDEX_RIS_CONSUMABLE,static_cast<quint8>(TableIndex::RIS_)},
    {INDEX_CLEANLINQUE_CONSUMABLE,static_cast<quint8>(TableIndex::S1_)},
    {INDEX_TESTTUBE_CONSUMABLE,static_cast<quint8>(TableIndex::TESTCUPS_)},
    {OUTSIDE_CLEANLINQUE_S2,static_cast<quint8>(TableIndex::S2_)}
};

QualityControl::QualityControl(QWidget *parent) : QWidget(parent)
  ,ui(new Ui::QualityControl)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);

    m_reminderTitleStr.clear();
    m_pReagentLinque.clear();

    //判断耗材损耗文件是否存在
    mpathfile = QCoreApplication::applicationDirPath() + "/consumables.ini";
    QFile file(mpathfile);
    if(!file.open(QIODevice::Append)){
        QLOG_DEBUG()<<"耗材配置文件不存在即创建"<<__FUNCTION__<<endl;
    }

    m_tipswidget = new tipcustomwidget(this);
    QFont f;
    f.setFamily("楷体");
    f.setPixelSize(14);
    m_tipswidget->setContentFont(f);
    m_tipswidget->setBackColor(220,220,220, 255);
    m_tipswidget->setDirect(DIRECT::DIRECT_TOP, 0.4);

    initSupplies();

    
}

void QualityControl::initSupplies()
{
    //显示耗材表
    Capacity_display_init(ui->tableWidget_Reagent_status);

    connect(ui->Chane_tray_1,&QToolButton::clicked,this,[=]()
    {
        m_replaceindexTray = TEST_THE_TUBEREEL_1;
        ReminderReplceTestTube(TEST_THE_TUBEREEL_1);
        return;
    });

    connect(ui->Chane_tray_2,&QToolButton::clicked,this,[=](){
        m_replaceindexTray = TEST_THE_TUBEREEL_2;
        ReminderReplceTestTube(TEST_THE_TUBEREEL_2);
        return;
    });

    connect(ui->Chane_tray_3,&QToolButton::clicked,this,[=](){
        m_replaceindexTray = TEST_THE_TUBEREEL_3;
        ReminderReplceTestTube(TEST_THE_TUBEREEL_3);
        return;
    });

    connect(ui->Chane_tray_4,&QToolButton::clicked,this,[=](){
        m_replaceindexTray = TEST_THE_TUBEREEL_4;
        ReminderReplceTestTube(TEST_THE_TUBEREEL_4);
        return;
    });


}

QualityControl::~QualityControl()
{
    QLOG_DEBUG()<<"析构耗材管理界面"<<endl;
    qDeleteAll(m_testtubeHole);

    delete m_tipswidget;
    m_tipswidget = nullptr;

    delete ui;
}

void QualityControl::CreatReplaceRemindWidget(const QString Title,const QString ReminderText,
                                                const int ReplaceindexSuppiles)
{
    if(m_reminderTitleStr.contains(Title)) return;

    QPointer<warn_interface> pwarn = new warn_interface(Title, ReminderText);
	pwarn->setWindowFlags(Qt::FramelessWindowHint |
		Qt::WindowStaysOnTopHint |
		Qt::WindowDoesNotAcceptFocus);
		pwarn->setAttribute(Qt::WA_DeleteOnClose);   // 关闭时自动销毁
		pwarn->setAttribute(Qt::WA_ShowWithoutActivating); // 不抢占焦点
		pwarn->setWindowModality(Qt::NonModal);     // 非模态窗口
        pwarn->settitle(Title);
        pwarn->setremtext(ReminderText);
        pwarn->replaceSupplyIndex(ReplaceindexSuppiles);

    // 5. 信号连接：用户确认后清理标题集合
    connect(pwarn, &warn_interface::makesure, this, [this](int indexSuppiles ,QString reminderedStr) {
        m_reminderTitleStr.removeAll(reminderedStr);
        if(indexSuppiles == -1) return;

        if(indexSuppiles == INDEX_TESTTUBE_CONSUMABLE)
            updateTestTubeTrat(m_replaceindexTray); //更换试管
        else{
            updateotherSupplies(indexSuppiles); //更换除试管耗材外的其它耗材
            if(indexSuppiles == INDEX_CLEANLINQUE_CONSUMABLE)
                QMetaObject::invokeMethod(this, &QualityControl::NoinitialCleaning, Qt::QueuedConnection);//初始清洗
            //DisplaysConsumablesRemaining();//主界面文字显示更新颜色
        }
    },Qt::UniqueConnection);

    // 6. 自适应多屏幕居中（基于主窗口所在的屏幕）
	if (const QScreen* activeScreen = QGuiApplication::primaryScreen()) {
		const QRect screenGeometry = activeScreen->availableGeometry();
		pwarn->move(
			screenGeometry.center() - pwarn->rect().center()
			);
	}
    m_reminderTitleStr.append(Title);
    pwarn->show();
}


void QualityControl::resizeEvent(QResizeEvent *event)
{

    QWidget::resizeEvent(event);


    // 定义布局常量
    constexpr int SPACING = 5;
    constexpr int REAGENT_ROWS = 5;
    constexpr int TABLE_HEIGHT_RATIO = 2;
    constexpr int TOTAL_HEIGHT_PARTS = 7;

    // 初始化尺寸参数
    const QSize mainSize = size();
    int _SuppileWidgetHeight = mainSize.height();


    //清洗液和试剂的宽 高
    int  CleanS1Width = ui->CleanReagent_area->width();

    int bottleSetWidth = (CleanS1Width - SPACING)/2;
    int bottleSetHeight = (_SuppileWidgetHeight - SPACING)/6;

    ui->CleanReagent_area->setFixedHeight(bottleSetHeight);
    ui->Group_Reagentarea->setFixedHeight(bottleSetHeight*5);

    //初始化试剂瓶
    _initreagbottle(bottleSetWidth,bottleSetHeight);

    //清洗液Loc
    QRect S1Cleanning = ui->CleanReagent_area->rect();
    int topLeft_x = S1Cleanning.center().x() - bottleSetWidth/2;
    int topLeft_y = S1Cleanning.center().y()  -  bottleSetHeight/2;
    ui->s1CleanLinque->setGeometry(topLeft_x,topLeft_y,CleanS1Width,bottleSetHeight);

    //试剂Loc
    QRect ReagentLinqueRect = ui->Group_Reagentarea->rect();
    int AvgReagentH = ReagentLinqueRect.height()/REAGENT_ROWS;
    int AvgReagengW = ReagentLinqueRect.width()/TABLE_HEIGHT_RATIO;

    ui->AAReagentLeft->setGeometry( 0,  AvgReagentH*0 , bottleSetWidth,bottleSetHeight);
    ui->ADPReagentLeft->setGeometry(0 , AvgReagentH*1 , bottleSetWidth,bottleSetHeight);
    ui->EPIReagentLeft->setGeometry(0 , AvgReagentH*2 , bottleSetWidth,bottleSetHeight);
    ui->COLReagentLeft->setGeometry(0 , AvgReagentH*3 , bottleSetWidth,bottleSetHeight);
    ui->RISReagentLeft->setGeometry(0 , AvgReagentH*4 , bottleSetWidth,bottleSetHeight);

    ui->AAReagentRight->setGeometry(AvgReagengW , AvgReagentH*0 , bottleSetWidth,bottleSetHeight);
    ui->ADPReagentRight->setGeometry(AvgReagengW , AvgReagentH*1 , bottleSetWidth,bottleSetHeight);
    ui->EPIReagentRight->setGeometry(AvgReagengW , AvgReagentH*2 , bottleSetWidth,bottleSetHeight);
    ui->COLReagentRight->setGeometry(AvgReagengW , AvgReagentH*3 , bottleSetWidth,bottleSetHeight);
    ui->RISReagentRight->setGeometry(AvgReagengW , AvgReagentH*4 , bottleSetWidth,bottleSetHeight);



    //耗材测试管区
    int CupTestWidth = ui->Tray_testtube->width();
    int CupTestHeigth = ui->Tray_testtube->height();


    //右边  --耗材表
    // 优化高度计算：试管盘区域在上，表格在下
    const int TUBE_AREA_RATIO = 7;  // 试管盘区域占70%
    const int TABLE_AREA_RATIO = 3; // 表格区域占30%
    const int TOTAL_AREA_RATIO = TUBE_AREA_RATIO + TABLE_AREA_RATIO;
    
    // 计算各区域高度 - 增加试管盘高度，确保充分显示
    int TaryCupHeigh = qMax((CupTestHeigth - SPACING * 3) * TUBE_AREA_RATIO / TOTAL_AREA_RATIO, 450); // 试管盘最小450像素
    int TatbleHeigh = qMax((CupTestHeigth - SPACING * 3) * TABLE_AREA_RATIO / TOTAL_AREA_RATIO, 60);  // 表格最小60像素
    
    // 确保总高度不超过可用空间
    if (TatbleHeigh + TaryCupHeigh > CupTestHeigth - SPACING * 3) {
        // 按比例重新分配
        TaryCupHeigh = (CupTestHeigth - SPACING * 3) * TUBE_AREA_RATIO / TOTAL_AREA_RATIO;
        TatbleHeigh = (CupTestHeigth - SPACING * 3) * TABLE_AREA_RATIO / TOTAL_AREA_RATIO;
    }
    
    // 设置表格位置（试管盘下方）
    int tableTop = SPACING + TaryCupHeigh + SPACING;
    ui->tableWidget_Reagent_status->setFixedWidth(CupTestWidth - SPACING * 2);
    ui->tableWidget_Reagent_status->setGeometry(SPACING, tableTop,
                                                CupTestWidth - SPACING * 2, TatbleHeigh);

    int TaryCupWidth = (CupTestWidth - 5 * SPACING) / 4; // 增加一个间距用于最右侧
    int tubeTop = SPACING; // 试管盘在顶部

    quint8 equipmentType = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, equipmentType);
    // 设置frame几何位置并创建垂直布局
    switch(equipmentType)
    {
        case KS600:
        {
            ui->frame->setGeometry(TaryCupWidth/2+ SPACING, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_2->setGeometry(TaryCupWidth/2+ SPACING + TaryCupWidth*2, tubeTop, TaryCupWidth, TaryCupHeigh);
            // 创建垂直布局
            createVerticalLayout(ui->frame, ui->Testtube_tray_1, ui->TestCupe_1, ui->Chane_tray_1, TaryCupHeigh);
            createVerticalLayout(ui->frame_2, ui->Testtube_tray_2, ui->TestCupe_2, ui->Chane_tray_2, TaryCupHeigh);
            break;
        }
        case KS800:
        {
            ui->frame->setGeometry(TaryCupWidth/2+ SPACING, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_2->setGeometry(TaryCupWidth/2+ SPACING + TaryCupWidth*1, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_3->setGeometry(TaryCupWidth/2+ SPACING + TaryCupWidth*2, tubeTop, TaryCupWidth, TaryCupHeigh);
            // 创建垂直布局
            createVerticalLayout(ui->frame, ui->Testtube_tray_1, ui->TestCupe_1, ui->Chane_tray_1, TaryCupHeigh);
            createVerticalLayout(ui->frame_2, ui->Testtube_tray_2, ui->TestCupe_2, ui->Chane_tray_2, TaryCupHeigh);
            createVerticalLayout(ui->frame_3, ui->Testtube_tray_3, ui->TestCupe_3, ui->Chane_tray_3, TaryCupHeigh);
            break;
        }
        case KS1200:
        {
            ui->frame->setGeometry(SPACING, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_2->setGeometry(TaryCupWidth*1+ SPACING*1, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_3->setGeometry(TaryCupWidth*2+ SPACING*2, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_4->setGeometry(TaryCupWidth*3+ SPACING*3, tubeTop, TaryCupWidth, TaryCupHeigh);
            // 创建垂直布局
            createVerticalLayout(ui->frame, ui->Testtube_tray_1, ui->TestCupe_1, ui->Chane_tray_1, TaryCupHeigh);
            createVerticalLayout(ui->frame_2, ui->Testtube_tray_2, ui->TestCupe_2, ui->Chane_tray_2, TaryCupHeigh);
            createVerticalLayout(ui->frame_3, ui->Testtube_tray_3, ui->TestCupe_3, ui->Chane_tray_3, TaryCupHeigh);
            createVerticalLayout(ui->frame_4, ui->Testtube_tray_4, ui->TestCupe_4, ui->Chane_tray_4, TaryCupHeigh);
            break;
        }
        default:
            ui->frame->setGeometry(TaryCupWidth/2+ SPACING, tubeTop, TaryCupWidth, TaryCupHeigh);
            ui->frame_2->setGeometry(TaryCupWidth/2+ SPACING + TaryCupWidth*2, tubeTop, TaryCupWidth, TaryCupHeigh);
            // 创建垂直布局
            createVerticalLayout(ui->frame, ui->Testtube_tray_1, ui->TestCupe_1, ui->Chane_tray_1, TaryCupHeigh);
            createVerticalLayout(ui->frame_2, ui->Testtube_tray_2, ui->TestCupe_2, ui->Chane_tray_2, TaryCupHeigh);
            break;
    }

    initshowcapacity(); //初始话试剂量

	// 延迟初始化逻辑
	static bool initialized = false;
	if (!initialized) {
		initializeTheTube(equipmentType);
		initialized = true;
	}
    return;
}

void QualityControl::_initreagbottle(int w_, int h_)
{
    // 试剂配置结构体
    struct ReagentConfig {
        quint8 indexLeft;
        quint8 indexRight;
        customButton* uiLeft;
        customButton* uiRight;
        QString name;
        QColor color;
    };

    // 定义所有试剂配置
    const QVector<ReagentConfig> reagents = {
        {INDEX_CLEANLINQUE_CONSUMABLE, INDEX_CLEANLINQUE_CONSUMABLE,
         ui->s1CleanLinque, nullptr, "S1", QColor(240, 255, 255)}, // 清洗剂无右侧

        {INDEX_AA_CONSUMABLE, INDEX_AA_1_CONSUMABLE,
         ui->AAReagentLeft, ui->AAReagentRight, "AA", GlobalData::customCurveColor(AA_REAGENT)},

        {INDEX_ADP_CONSUMABLE, INDEX_ADP_1_CONSUMABLE,
         ui->ADPReagentLeft, ui->ADPReagentRight, "ADP", GlobalData::customCurveColor(ADP_REAGENT)},

        {INDEX_EPI_CONSUMABLE, INDEX_EPI_1_CONSUMABLE,
         ui->EPIReagentLeft, ui->EPIReagentRight, "EPI", GlobalData::customCurveColor(EPI_REAGENT)},

        {INDEX_COL_CONSUMABLE, INDEX_COL_1_CONSUMABLE,
         ui->COLReagentLeft, ui->COLReagentRight, "COL", GlobalData::customCurveColor(COL_REAGENT)},

        {INDEX_RIS_CONSUMABLE, INDEX_RIS_1_CONSUMABLE,
         ui->RISReagentLeft, ui->RISReagentRight, "RIS", GlobalData::customCurveColor(RIS_REAGENT)}
    };

    QMap<quint8, QString> suppliesName;
    QMap<quint8, QColor> reagentColor;

    // 统一初始化试剂按钮
    for (const auto& config : reagents) {
        m_pReagentLinque.insert(config.indexLeft, config.uiLeft);
        suppliesName.insert(config.indexLeft, QString("%1(L)").arg(config.name));

        if (config.uiRight) { // 存在右侧试剂瓶
            m_pReagentLinque.insert(config.indexRight, config.uiRight);
            suppliesName.insert(config.indexRight, QString("%1(R)").arg(config.name));

            reagentColor.insert(config.indexRight, config.color);
        }

        reagentColor.insert(config.indexLeft, config.color);
    }

    // 特殊处理清洗剂（单独命名）
    suppliesName[INDEX_CLEANLINQUE_CONSUMABLE] = "S1";

    // 配置按钮属性和信号连接
    for (auto it = m_pReagentLinque.begin(); it != m_pReagentLinque.end(); ++it) {
        const quint8 index = it.key();
        customButton* btn = it.value();

        btn->setMouseTracking(true);
        btn->_setwidgetfixsize(w_, h_);
        btn->setRange(0, 100);
        btn->setStep(1.0);
        btn->_setBottlename(suppliesName.value(index, "Unknown"));

        const QColor color = reagentColor.value(index, Qt::white);
        btn->setNormalColorStart(color);
        btn->setNormalColorEnd(color);

        // 安全捕获当前index的lambda
        QObject::connect(btn, &customButton::btninnerIndexReag, this, [this, index]() {
            if (ReaminderUsbNotConnect()) return;

            QString reagentName;
            ReagentIndexMapReagentStr(index, reagentName);

            const QString title = QString("更换试剂%1").arg(reagentName);
            const QString content = QString("确定更换%1？").arg(reagentName);
            CreatReplaceRemindWidget(title, content, index);
        });
    }

    update();
}


void QualityControl::setReagentPara(quint8 index_,bool bsetAlarm,double valuedata)
{
    if(m_pReagentLinque.contains(index_))
    {
        auto it = m_pReagentLinque.find(index_);
        if(bsetAlarm)
            it.value()->setAlarmValue(valuedata);
        else
            it.value()->setValue(valuedata);
    }
    return;
}
void QualityControl::configwarmvalue(quint8 index_,quint8 _limitarm)
{
    setReagentPara(index_,true,static_cast<double>(_limitarm));
    return;
}


bool QualityControl::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched,event);
}

QString QualityControl::mapoutreagent(quint8 index_reag, double lastratio) {
    static const QMap<quint8, QString> reagentMap = {
        {INDEX_AA_CONSUMABLE,          "AA1(左)"},
        {INDEX_ADP_CONSUMABLE,         "ADP1(左)"},
        {INDEX_EPI_CONSUMABLE,         "EPI1(左)"},
        {INDEX_COL_CONSUMABLE,         "COL1(左)"},
        {INDEX_RIS_CONSUMABLE,         "RIS1(左)"},
        {INDEX_CLEANLINQUE_CONSUMABLE, "清洗液"},
        {INDEX_TESTTUBE_CONSUMABLE,    "测试杯"},
        {INDEX_AA_1_CONSUMABLE,        "AA2(右)"},
        {INDEX_ADP_1_CONSUMABLE,       "ADP2(右)"},
        {INDEX_EPI_1_CONSUMABLE,       "EPI2(右)"},
        {INDEX_COL_1_CONSUMABLE,       "COL2(右)"},
        {INDEX_RIS_1_CONSUMABLE,       "RIS2(右)"}
    };

    QString _outprint = reagentMap.value(index_reag, "未知耗材");
    return QString("耗材%1剩余量:[%2]").arg(_outprint).arg(lastratio);
}




//仪器内部清洗液
void QualityControl::loc_clean_linque_display()
{
    quint16 clean_theremaining_amount = 0;
    ConsumablesOper::GetpInstance()->getCleanLinqueAllowance(clean_theremaining_amount);
    double displayRatio = double(double(clean_theremaining_amount)*100.00)/MAX_RATIO_SWITCT;
    setReagentPara(INDEX_CLEANLINQUE_CONSUMABLE,false,displayRatio);//清洗剂剩余量(%)
    emit SycnMainUiLosserSuppile(INDEX_CLEANLINQUE_CONSUMABLE,displayRatio);
    update();
    return;
}


//仪器内试剂
void QualityControl::loc_reagnet_type_display_val(quint8 index_reag)
{
    quint16 reagent_amount = 0;
    ConsumablesOper::GetpInstance()->updateReplaceLocRatio(READ_OPERRAT,index_reag,reagent_amount);
    double displayRatio = double(double(reagent_amount*100)/MAX_RATIO_SWITCT);
    setReagentPara( index_reag ,false,displayRatio);
    emit SycnMainUiLosserSuppile(index_reag,displayRatio);
    QLOG_DEBUG()<<mapoutreagent(index_reag,displayRatio);
    ui->Group_Reagentarea->repaint();
    return;
}


/*点击耗材按钮进入*/
void  QualityControl::initshowcapacity()
{
    loc_clean_linque_display();
    for(int i = INDEX_AA_CONSUMABLE; i <= INDEX_RIS_CONSUMABLE ; i++)
        loc_reagnet_type_display_val(i);
    for(int i = INDEX_AA_1_CONSUMABLE; i <= INDEX_RIS_1_CONSUMABLE ; i++)
        loc_reagnet_type_display_val(i);
    return;
}


void  QualityControl::ShowConsumablesLimitArm()
{
    static const QStringList keyList = {
            "AAbottleLimit", "ADPbottleLimit", "EPIbottleLimit",
            "COLbottleLimit", "RISbottleLimit", "S1bottleLimit",
            "tubeTarybottleLimit", "S2bottleLimit"
        };

    // 2. 阻塞信号以提高性能
    ui->tableWidget_Reagent_status->blockSignals(true);

    for (auto it = ITEM_REAGENT_MAP.constBegin(); it != ITEM_REAGENT_MAP.constEnd(); ++it){
        const quint8 index_ = it.key(); // 4. 明确类型和常量性
        const int col = it.value();     // 5. 缓存列号避免多次调用 value()

        // 6. 将 _limitarm 作用域限定在循环内
        quint8 _limitarm = 0;
        ConsumablesOper::GetpInstance()->updateReagentLimit(READ_OPERRAT, index_, _limitarm);

        // 7. 合并重复操作
        FilltheConsumablesTable(ui->tableWidget_Reagent_status,
                                   static_cast<int>(TableRow::LimitArm),
                                   col,
                                   QString::number(_limitarm),
                                   false);

        this->configwarmvalue(index_, _limitarm);

        // 8. 添加数组越界保护
        if (Q_UNLIKELY(index_ >= keyList.size())) {
            qWarning() << "Invalid index for keyList:" << index_;
            continue;
        }

        // 9. 避免重复创建临时 INI_File 对象
        const QString paramValue = INI_File().rConfigPara(keyList[index_]).toString();
        FilltheConsumablesTable(ui->tableWidget_Reagent_status,
                                   static_cast<int>(TableRow::LimitAllSuppile),
                                   col,
                                   paramValue,
                                   true);
    }

    ui->tableWidget_Reagent_status->blockSignals(false);

    return;
}




//显示耗材剩余整体
void QualityControl::displayConsumablesInteger()
{
    const QColor kAlertColor(255, 0, 0, 255);
    const QColor kNormalColor(0, 0, 0, 255);

    ui->tableWidget_Reagent_status->blockSignals(true);
    for (auto it = ITEM_REAGENT_MAP.constBegin(); it != ITEM_REAGENT_MAP.constEnd(); ++it) {
        quint8 bottlenum = 0;
        ConsumablesOper::GetpInstance()->syacnReagentTotalBottle(READ_OPERRAT, it.key(), bottlenum);

        QColor textColor = (bottlenum <= 10) ? kAlertColor : kNormalColor;
        FilltheConsumablesTable(ui->tableWidget_Reagent_status,
                                   static_cast<int>(TableRow::Allowance_),
                                   it.value(),
                                   QString::number(bottlenum),
                                   textColor,
                                   false);
    }
    ui->tableWidget_Reagent_status->blockSignals(false); // 恢复信号
    return;
}


void QualityControl::createVerticalLayout(QFrame* frame, QLabel* label, QWidget* testCupe, QToolButton* button, int frameHeight)
{
    // 清除frame的现有布局
    if (frame->layout()) {
        QLayout* oldLayout = frame->layout();
        oldLayout->setParent(nullptr);
        delete oldLayout;
    }
    
    // 创建垂直布局
    QVBoxLayout* verticalLayout = new QVBoxLayout(frame);
    verticalLayout->setSpacing(10); // 控件间距
    verticalLayout->setContentsMargins(10, 10, 10, 10); // 边距
    
    // 设置标签：顶部居中
    label->setFixedHeight(20);
    label->setAlignment(Qt::AlignCenter);
    verticalLayout->addWidget(label);
    
    // 设置TestCupe容器：占据主要空间
    const int testCupeHeight = frameHeight - 20 - 35 - 30; // 总高度 - 标签高度 - 按钮高度 - 间距
    testCupe->setFixedHeight(qMax(testCupeHeight, 200)); // 最小200像素
    verticalLayout->addWidget(testCupe);
    
    // 设置按钮：底部居中
    button->setFixedHeight(35);
    
    // 创建水平布局用于按钮居中
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // 左侧拉伸
    buttonLayout->addWidget(button); // 按钮居中
    buttonLayout->addStretch(); // 右侧拉伸
    
    verticalLayout->addLayout(buttonLayout);
    
    // 设置布局比例：标签和按钮固定高度，TestCupe占据剩余空间
    verticalLayout->setStretch(0, 0); // 标签不拉伸
    verticalLayout->setStretch(1, 1); // TestCupe占据主要空间
    verticalLayout->setStretch(2, 0); // 按钮不拉伸
}

void QualityControl::gridLayoutTubeTray(QVector<QSimpleLed*> tubeVector,
                                       QWidget* container,
                                       int trayId)
{

    // 定义布局常量
    constexpr int COLUMNS_PER_ROW = 6;
    constexpr int FIXED_HEIGHT = 360;
    constexpr int ROW_SPACING_DIVISOR = 11;
    constexpr int MIN_ROW_HEIGHT = 40;
    constexpr int LAYOUT_SPACING = 10;

    // 计算起始偏移量
    const int startOffset = (trayId - TEST_THE_TUBEREEL_1) * ONETRAY_TOTALTUBE;

    // 清除旧布局
    QLayout* oldLayout = container->layout();
    if (oldLayout) {
        QLayoutItem* item;
        while ((item = oldLayout->takeAt(0))) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    // 创建垂直布局
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setSpacing((container->height() - FIXED_HEIGHT) / ROW_SPACING_DIVISOR);

    // 预计算行数
    const int actualRows = qMin(tubeVector.size() / COLUMNS_PER_ROW + 1, 10);

    // 创建行布局
    QVector<QHBoxLayout*> rowLayouts;
    rowLayouts.reserve(actualRows);

    for (int i = 0; i < actualRows; ++i) {
        QHBoxLayout* hLayout = new QHBoxLayout();
        hLayout->setSpacing(LAYOUT_SPACING);

        QWidget* rowWidget = new QWidget(container);
        rowWidget->setMinimumHeight(MIN_ROW_HEIGHT);
        rowWidget->setLayout(hLayout);

        rowLayouts.append(hLayout);
        mainLayout->addWidget(rowWidget);
    }

    // 排列试管
    for (QSimpleLed* tube : tubeVector) {
        const int tubeIndex = tube->objectName().toInt() - startOffset;
        const int row = tubeIndex / COLUMNS_PER_ROW;

        if (row < rowLayouts.size()) {
            rowLayouts[row]->addWidget(tube);
        }
    }

    container->setLayout(mainLayout);
}


void QualityControl::initializeTheTube(const quint8 instrumentType)
{
    quint8 trayCount = 4;
    switch(instrumentType){
        case KS600: trayCount = 2; break;
        case KS800: trayCount = 3; break;
        case KS1200: trayCount = 4; break;
        default:    trayCount = 4; break;
    }

    ui->frame_3->setVisible(trayCount >= 3);
    ui->frame_4->setVisible(trayCount >= 4);

    const quint8 totalTube = ONETRAY_TOTALTUBE *trayCount;
    m_testtubeHole.clear();
    m_testtubeHole.reserve(totalTube);

    m_tubeStateVec_1.reserve(ONETRAY_TOTALTUBE);
    m_tubeStateVec_2.reserve(ONETRAY_TOTALTUBE);
    m_tubeStateVec_3.reserve(ONETRAY_TOTALTUBE);
    m_tubeStateVec_4.reserve(ONETRAY_TOTALTUBE);
    for(int i = 0; i < totalTube; ++i) {
        const int trayIndex = i / ONETRAY_TOTALTUBE;
        QWidget *parentWidget = nullptr;
        switch(trayIndex){
            case 0: parentWidget = ui->TestCupe_1; break;
            case 1: parentWidget = ui->TestCupe_2; break;
            case 2: parentWidget = ui->TestCupe_3; break;
            case 3: parentWidget = ui->TestCupe_4; break;
        }

        QSimpleLed* pDelegatesTube = new QSimpleLed(parentWidget);
        pDelegatesTube->setFixedSize(30, 28);
        pDelegatesTube->setObjectName(QString::number(i));
        pDelegatesTube->setColors(QSimpleLed::CUSTOM);
        pDelegatesTube->setStates(QSimpleLed::LEDSTATES::ON);
        // 按托盘索引分类存储
        switch (trayIndex) {
            case 0: m_tubeStateVec_1.append(pDelegatesTube); break;
            case 1: m_tubeStateVec_2.append(pDelegatesTube); break;
            case 2: m_tubeStateVec_3.append(pDelegatesTube); break;
            case 3: m_tubeStateVec_4.append(pDelegatesTube); break;
        }
        m_testtubeHole.append(pDelegatesTube);
    }

    // 统一布局管理（简化条件判断）
    gridLayoutTubeTray(m_tubeStateVec_1, ui->TestCupe_1, TEST_THE_TUBEREEL_1);
    gridLayoutTubeTray(m_tubeStateVec_2, ui->TestCupe_2, TEST_THE_TUBEREEL_2);
    if (trayCount >= 3) gridLayoutTubeTray(m_tubeStateVec_3, ui->TestCupe_3, TEST_THE_TUBEREEL_3);
    if (trayCount >= 4) gridLayoutTubeTray(m_tubeStateVec_4, ui->TestCupe_4, TEST_THE_TUBEREEL_4);

    // 优化数据库交互和状态更新
    QVariantList TestTubeInfo;
    FullyAutomatedPlatelets::pinstancesqlData()->FindAllEmptyTube(TestTubeInfo);

    for (const QVariant& variant : TestTubeInfo) {
        const AllTubeInfo& tubeInfo = variant.value<AllTubeInfo>();
        if (tubeInfo.TubeStatus == TESTTUBES_SAMPLED || tubeInfo.TubeStatus == TESTTUBES_CLIPPEDAWAY) {
            slotSynchronizeEmptyTube(tubeInfo.TubeNumbers);
        }
    }
    return;
}

//已使用的显示为红色(单个)
void  QualityControl::slotSynchronizeEmptyTube(quint8 indextube)
{
    constexpr int TUBES_PER_TRAY = ONETRAY_TOTALTUBE;

    // 确定目标托盘
    const int trayIndex = indextube / TUBES_PER_TRAY;
    QVector<QSimpleLed*>* targetVec = nullptr;

    switch(trayIndex) {
        case 0:  targetVec = &m_tubeStateVec_1; break;
        case 1:  targetVec = &m_tubeStateVec_2; break;
        case 2:  targetVec = &m_tubeStateVec_3; break;
        case 3:  targetVec = &m_tubeStateVec_4; break;
        default: return; // 无效索引直接返回
    }

    // 使用STL算法查找目标控件
    auto it = std::find_if(targetVec->cbegin(), targetVec->cend(),
            [indextube](const QSimpleLed* led) {
                return led->objectName().toInt() == indextube;
        });

    if (it != targetVec->cend()) {
        (*it)->setStates(QSimpleLed::LEDSTATES::OFF);
    }

    this->update();
}

void QualityControl::consumeCleaningFluid(const CleaningFluidDepleteType depleteType){
    // 单次获取配置实例[[10]]
    auto& ini = INI_File();
    ConsumablesOper* consumables = ConsumablesOper::GetpInstance();

    // 使用const防止意外修改
    const int washbloodPinVol = ini.GetAbsorbWashingfluidX2();
    const int washReagPinVol = ini.GetAbsorbWashingfluidX1();

    // 增强错误处理[[3]]
    if (washReagPinVol < 0 || washbloodPinVol < 0) {
        QLOG_ERROR() << "Invalid INI parameters: "
                     << "washReagPinVol=" << washReagPinVol
                     << "washbloodPinVol=" << washbloodPinVol;
        return;
    }

    // 使用查表法替代switch[[9]]
    const std::map<CleaningFluidDepleteType, quint16> depletionMap = {
        {REAGENT_PIN, static_cast<quint16>(washReagPinVol)},
        {BLOOD_PIN,   static_cast<quint16>(washbloodPinVol)},
        {BOTH_PINS,   static_cast<quint16>(washbloodPinVol + washReagPinVol)}
    };

    quint16 consumingCleaningSolutionS1 = depletionMap.count(depleteType)
            ? depletionMap.at(depleteType)
            : (washbloodPinVol + washReagPinVol);

    // 避免浮点运算
    double lastRatioPercent = 0;
    quint16 updateratio = 0;
    QUIUtils::CalculateTheConsumptionOfCleaningFluid(consumingCleaningSolutionS1,
                                                     lastRatioPercent, updateratio);
    // 检查阈值并触发警报
    quint8 limitRatio = 0;
    consumables->updateReagentLimit(READ_OPERRAT, INDEX_CLEANLINQUE_CONSUMABLE, limitRatio);

    // 边界值检查
	const double warmRatio = lastRatioPercent*100.00;
    if (warmRatio <= 0 || warmRatio < limitRatio) {
		if (cglobal::g_StartTesting) {
			emit consumablesLackPauses(INDEX_CLEANLINQUE_CONSUMABLE);
		}
		else {
			warn_interface::showTransientWarning(tr("耗材提示"), 
				tr("内清洗液S1不足警报限,请及时更换!"));
			return;
		}
        QLOG_WARN() << "内清洗液S1不足警报限";
        return;
    }

    // 合并更新操作
    updateCleaningFluidStatus(consumables, warmRatio, updateratio);

}


void QualityControl::updateCleaningFluidStatus(ConsumablesOper* consumables, double lastRatioPercent,
                                                quint16 updateratio)
{
    // 更新本地配置
    QUIUtils::sycnBottleRatioPara(INDEX_CLEANLINQUE_CONSUMABLE, updateratio);
    setReagentPara(INDEX_CLEANLINQUE_CONSUMABLE, false, lastRatioPercent);

    // 通知UI
    emit SycnMainUiLosserSuppile(INDEX_CLEANLINQUE_CONSUMABLE, lastRatioPercent);
    consumables->updateReplaceLocRatio(WRITE_OPERAT, INDEX_CLEANLINQUE_CONSUMABLE, updateratio);

    // 缓存操作
    quint8 totalnum = 0;
    consumables->syacnReagentTotalBottle(READ_OPERRAT, INDEX_CLEANLINQUE_CONSUMABLE, totalnum);


    QByteArray buffer;
    QUIUtils::writeSuppliesBottleControlOrder(buffer, INDEX_CLEANLINQUE_CONSUMABLE,
        INDEX_CLEANLINQUE_CONSUMABLE, totalnum, updateratio, true);
    emit sendDirectives(buffer, "缓存消耗清洗液");
}

void QualityControl::consumeReagent(const quint8 reag_kind, const quint8 index_)
{
    constexpr double PERCENTAGE_FACTOR = 100.0;
    auto *consumables = ConsumablesOper::GetpInstance();
    // 获取试剂消耗量
    const quint8 usedVolume = INI_File().getTypesReagentSuckVolume(index_);
    if (usedVolume == 0) {
        QLOG_WARN() << "试剂消耗量为0，跳过更新流程，类型:" << reag_kind;
        return;
    }

    //计算消耗清洗液 输出剩余比 显示
    double remainingRatio  = 0.0; //输出剩余比 max 1
    quint16 updatedRatio  = 0;    //剩余比例 max 512
    QUIUtils::CalculateTheConsumptionOfReagent(reag_kind,usedVolume,remainingRatio,updatedRatio );

    const double remainingPercentage = static_cast<double>(remainingRatio * PERCENTAGE_FACTOR);
    if (remainingPercentage <= 0) return;

    consumables->updateReplaceLocRatio(WRITE_OPERAT, reag_kind,updatedRatio);
    QLOG_DEBUG()<<"设置余量到内存储"<<updatedRatio;

    setReagentPara(reag_kind, false, remainingPercentage);
    emit SycnMainUiLosserSuppile(reag_kind, remainingPercentage);
    QUIUtils::sycnBottleRatioPara(reag_kind,updatedRatio);

    //缓存消耗试剂
    quint8 totalnum;
    QByteArray buffer;
    consumables->syacnReagentTotalBottle(READ_OPERRAT,reag_kind,totalnum);
    QUIUtils::writeSuppliesBottleControlOrder(buffer,reag_kind,reag_kind,
                                              totalnum,updatedRatio,true);
    emit  sendDirectives(buffer,"缓存消耗试剂");


    quint8 limitRatio  = 0;
    consumables->updateReagentLimit(READ_OPERRAT, reag_kind,limitRatio );
    const bool reagentSufficient = (remainingPercentage > limitRatio);

    if(!reagentSufficient){
        emit consumablesLackPauses(reag_kind);//软件暂停
    }
    return;
}

/*减少耗材*/
void QualityControl::LosserOneReagentul(const quint8 index_reag,const quint8 kind_deplete)
{
    // 特殊处理清洗液消耗
    if(index_reag == INDEX_CLEANLINQUE_CONSUMABLE) {
        consumeCleaningFluid(static_cast<CleaningFluidDepleteType>(kind_deplete));
        return;
    }

    // 定义试剂类型映射表
    static const ReagentMapping reagentMap[] = {
        {INDEX_AA_CONSUMABLE,   AA_REAGENT},
        {INDEX_ADP_CONSUMABLE,  ADP_REAGENT},
        {INDEX_EPI_CONSUMABLE,  EPI_REAGENT},
        {INDEX_COL_CONSUMABLE,  COL_REAGENT},
        {INDEX_RIS_CONSUMABLE,  RIS_REAGENT},
        {INDEX_AA_1_CONSUMABLE, AA_REAGENT},
        {INDEX_ADP_1_CONSUMABLE,ADP_REAGENT},
        {INDEX_EPI_1_CONSUMABLE,EPI_REAGENT},
        {INDEX_COL_1_CONSUMABLE,COL_REAGENT},
        {INDEX_RIS_1_CONSUMABLE,RIS_REAGENT}
    };

    // 遍历映射表查找匹配项
    for(size_t i = 0; i < sizeof(reagentMap)/sizeof(reagentMap[0]); ++i) {
        if(reagentMap[i].index == index_reag) {
            consumeReagent(index_reag, reagentMap[i].type);
            return;
        }
    }
    // 可选的默认处理
    QLOG_WARN() << "未知试剂索引:" << index_reag<<endl;
}




//试管盘内所有试管改变状态
void QualityControl::updatetrayAllTube(const QVector<QSimpleLed *> &ptubeTrayVec, const bool bused){
    for(auto ptube : ptubeTrayVec)
       {
           QSimpleLed *ptubeOne = ptube;
           if(bused == true)
               ptubeOne->setStates(QSimpleLed::LEDSTATES::OFF);
           else
              ptubeOne->setStates(QSimpleLed::LEDSTATES::ON);
       }
       update();
}


//更换耗材试管--确定
void  QualityControl::makesureReplace(quint8 index)
{
    if(index == INDEX_TESTTUBE_CONSUMABLE){
        updateTestTubeTrat(m_replaceindexTray); //更换试管
    }
}

void  QualityControl::handleoutSideCleanDepleteOne(){
    quint8 s2bottle = 0 ;
    auto *pinstance = ConsumablesOper::GetpInstance();
    pinstance->syacnReagentTotalBottle(READ_OPERRAT,OUTSIDE_CLEANLINQUE_S2,s2bottle);
    if(s2bottle <= 0){
        warn_interface::showTransientWarning(tr("更换S2清洗液失败"),tr("余量不足,请充值!"));
        return;
    }
    quint8 configBottle = s2bottle - 1;

    QByteArray buffer;
    pinstance->syacnReagentTotalBottle(WRITE_OPERAT,OUTSIDE_CLEANLINQUE_S2,configBottle);
    INI_File().wConfigPara("S2DiskNumber",configBottle);
    QUIUtils::writeSuppliesBottleControlOrder(buffer,OUTSIDE_CLEANLINQUE_S2,
                                              OUTSIDE_CLEANLINQUE_S2,
                                              configBottle,521,true);
    QTableWidget *pwidget = ui->tableWidget_Reagent_status;
    const int rows = static_cast<int>(TableRow::Allowance_);
    const int cols = static_cast<int>(TableIndex::S2_);
    FilltheConsumablesTable(pwidget,rows,cols,QString::number(configBottle),false);
    emit  sendDirectives(buffer,"缓存S2余量瓶");
}


//联机状态才能更换耗材
bool QualityControl::ReaminderUsbNotConnect()
{
    if(cglobal::gserialConnecStatus == false)
    {
        warn_interface::showTransientWarning(tr("更换耗材失败"), tr("仪器未连接!"));
        QLOG_ERROR()<<"更换盘失败仪器未连接"<<endl;
        return true;
    }
    else if(cglobal::gserialConnecStatus && cglobal::g_StartTesting && !cglobal::g_PauseTesting)
    {
        warn_interface::showTransientWarning(tr("更换耗材失败"), tr("仪器开始测试请先暂停!"));
        update();
        return true;
    }
    return false;
}


bool QualityControl::isHadAddSampleTube(quint8 indextray)
{
    bool _hadMarktube = true;

    const int startHole = (indextray - 1) * ONETRAY_TOTALTUBE;
    const int EndHole =   startHole +  ONETRAY_TOTALTUBE;

    QVariantList tragtubeState;
    tragtubeState.clear();
    FullyAutomatedPlatelets::pinstancesqlData()->findEmptyTubesInRange(
             startHole,EndHole,tragtubeState);

    for(const QVariant &var : tragtubeState){
        const auto &tube = var.value<AllTubeInfo>();
        const uint status = tube.TubeStatus;
        if(status == TESTTUBES_SAMPLED){
            _hadMarktube = false;
            QString outtext = QString("试管盘%1,有待测待测样本更换失败!").arg(indextray);
            warn_interface::showTransientWarning(tr("更换操作异常"), outtext);
            break;
        }
    }
    return _hadMarktube;
}

void QualityControl::TrayHadFreetube(quint8 indextray,bool &hadtube)
{
    QVariantList sqltubestate;
    sqltubestate.clear();
    const int startHole = (indextray - 1) * ONETRAY_TOTALTUBE;
    const int EndHole =   startHole +  ONETRAY_TOTALTUBE;
    FullyAutomatedPlatelets::pinstancesqlData()->findEmptyTubesInRange(
             startHole,EndHole,sqltubestate);

    for(const QVariant &var : sqltubestate){
        const auto &tube = var.value<AllTubeInfo>();
        const uint status = tube.TubeStatus;
        if(status == TESTTUBES_FREETIME){
            hadtube = false;
            break;
        }
    }
    return;
}

void QualityControl::ReminderReplceTestTube(quint8 indexTray)
{
    if(ReaminderUsbNotConnect() == true)    return; //未联机直接返回

    if(!isHadAddSampleTube(indexTray))      return; //有标记任务不能更换

    // 统一构建提示信息
    bool alltubeused = true;
    TrayHadFreetube(indexTray,alltubeused);
    const QString statusDesc = alltubeused ? tr("确定更换?") : tr("未使用完,是否确定更换?");
    const QString message = tr("测试杯盘%1%2").arg(indexTray).arg(statusDesc);
    const QString title = tr("更换试管盘%1").arg(indexTray);
    CreatReplaceRemindWidget(title,message,INDEX_TESTTUBE_CONSUMABLE);
    return;
}


//执行更换试管盘的操作
void updatetestDiskConfig(quint8 newValue, QByteArray &buffer) {
       ConsumablesOper::GetpInstance()->sycnTubeTotalTray(WRITE_OPERAT, newValue);
       INI_File().wConfigPara("testDiskNumber",newValue);
       QUIUtils::writeSuppliesBottleControlOrder(buffer,INDEX_TESTTUBE_CONSUMABLE,
                                                 INDEX_TESTTUBE_CONSUMABLE,
                                                 newValue,521,true);
}
void QualityControl::updateTestTubeTrat(quint8 replaceIndexTray)
{
    quint8 totalTarys = 0;
    QVector <QSimpleLed *> replactindex;
    ConsumablesOper::GetpInstance()->sycnTubeTotalTray(READ_OPERRAT,totalTarys);
    if(totalTarys <= 0){
        warn_interface::showTransientWarning(tr("更换试管盘失败"),
                                             tr("试管耗材不足,请刷卡充值!"));
        return;
    }
    switch(replaceIndexTray) { // 按使用频率排序
        case TEST_THE_TUBEREEL_2: replactindex = m_tubeStateVec_2; break;
        case TEST_THE_TUBEREEL_1: replactindex = m_tubeStateVec_1; break;
        case TEST_THE_TUBEREEL_3: replactindex = m_tubeStateVec_3; break;
        case TEST_THE_TUBEREEL_4: replactindex = m_tubeStateVec_4; break;
        default:
            warn_interface::showTransientWarning(tr("错误"), tr("无效索引"));
            return;
    }
    if (replactindex.isEmpty()) {
        qWarning() << "试管状态向量未初始化";
        return;
    }
    const quint8 configTestTrayNum = totalTarys - 1;
    QByteArray buffer;
    updatetestDiskConfig(configTestTrayNum,buffer);
    QTableWidget *pwidget = ui->tableWidget_Reagent_status;
    const int rows = static_cast<int>(TableRow::Allowance_);
    const int cols = static_cast<int>(TableIndex::TESTCUPS_);
    FilltheConsumablesTable(pwidget,rows,cols,QString::number(configTestTrayNum),false);

    //同步测试界面更新和更新数据库内试管状态
    updatetrayAllTube(replactindex,false); //耗材界面更新
    FullyAutomatedPlatelets::pinstanceTesting()->replacEmptyTestTary(replaceIndexTray - 1);
    emit  sendDirectives(buffer,"直接写入试管余盘");
    return;
}


bool QualityControl::RemindersuppliesEnouth(const int Typesupplies)
{
    if(Typesupplies <= 0)
    {
        warn_interface::showTransientWarning(tr("更换耗材失败"), tr("更换耗材已用尽,请尽快充值!"));
        return false;
    }
    return true;
}

void QualityControl::ReagentIndexMapReagentStr(quint8 indexReagent, QString& outPutText) {
	static const std::unordered_map<quint8, QString> reagentMap = {
		{ INDEX_AA_CONSUMABLE,         "更换AA(L)试剂" },
		{ INDEX_ADP_CONSUMABLE,        "更换ADP(L)试剂" },
		{ INDEX_EPI_CONSUMABLE,       "更换EPI(L)试剂" },
		{ INDEX_COL_CONSUMABLE,       "更换COL(L)试剂" },
		{ INDEX_RIS_CONSUMABLE,       "更换RIS(L)试剂" },
		{ INDEX_CLEANLINQUE_CONSUMABLE,"更换清洗剂S1" },
		{ INDEX_AA_1_CONSUMABLE,      "更换AA(R)试剂" },
		{ INDEX_ADP_1_CONSUMABLE,     "更换ADP(R)试剂" },
		{ INDEX_EPI_1_CONSUMABLE,     "更换EPI(R)试剂" },
		{ INDEX_COL_1_CONSUMABLE,     "更换COL(R)试剂" },
		{ INDEX_RIS_1_CONSUMABLE,     "更换RIS(R)试剂" }
	};

	auto it = reagentMap.find(indexReagent);
	if (it != reagentMap.end()) {
		outPutText = it->second;
	}
	else {
		// 可选：处理未匹配到的情况，如赋空值或默认提示
		outPutText.clear();
	}
}



void QualityControl::updateotherSupplies(const quint8 indexSupplies){
    ConsumablesOper* consumables = ConsumablesOper::GetpInstance();
    if (!consumables) {
        qWarning() << "ConsumablesOper instance is null.";
        return;
    }
    quint16 fullratio = 512;
    double fullvalue = 100.00;
    quint8 num_Bottle = 0;//剩余耗材总量
    QString Reminderstr ;
    ReagentIndexMapReagentStr(indexSupplies,Reminderstr);

    consumables->_ObtainConsumablesBalance(indexSupplies,num_Bottle);
    if(num_Bottle <= 0)
    {
        QString reminder_content = QString("%1耗材余量不足,请刷卡充值!").arg(Reminderstr);
        warn_interface::showTransientWarning(tr("更换试剂耗材"), reminder_content);
        return;
    }

    //设置试剂瓶显示比例
    setReagentPara(indexSupplies,false,fullvalue);

    //同步测试主界面
    emit SycnMainUiLosserSuppile(indexSupplies,fullvalue);

    quint8 LastBottle = num_Bottle - 1;
    consumables->syacnReagentTotalBottle(WRITE_OPERAT,indexSupplies,LastBottle);

    QUIUtils::sycnBottleWholeNum(indexSupplies,LastBottle);

    _TableLinqueLess(indexSupplies,LastBottle); //更新表显示
    consumables->updateReplaceLocRatio(WRITE_OPERAT,indexSupplies, fullratio); //更新本地耗材比例

    QByteArray buffer;
    QUIUtils::writeSuppliesBottleControlOrder(buffer,indexSupplies,indexSupplies,LastBottle,fullratio,true);
    emit  sendDirectives(buffer,"更换直接写入试剂数");
    return;
}

void QualityControl::_TableLinqueLess(const quint8 index_,const quint8 total_)
{
    static const QHash<quint8, quint8> indexMap = {
            {INDEX_AA_1_CONSUMABLE,  INDEX_AA_CONSUMABLE},
            {INDEX_ADP_1_CONSUMABLE, INDEX_ADP_CONSUMABLE},
            {INDEX_EPI_1_CONSUMABLE, INDEX_EPI_CONSUMABLE},
            {INDEX_COL_1_CONSUMABLE, INDEX_COL_CONSUMABLE},
            {INDEX_RIS_1_CONSUMABLE, INDEX_RIS_CONSUMABLE}
    };
    const quint8 index_Supplies = indexMap.value(index_, index_);
    const auto iter = ITEM_REAGENT_MAP.constFind(index_Supplies);
    if (iter != ITEM_REAGENT_MAP.constEnd()) {
            FilltheConsumablesTable(ui->tableWidget_Reagent_status,
                                  static_cast<int>(TableRow::Allowance_),
                                  iter.value(),
                                  QString::number(total_),
                                  false);
    } else {
        QLOG_ERROR() << QString::fromUtf8("更新耗材显示表失败 下标[%1->%2]")
                        .arg(index_).arg(index_Supplies)
                     << __FUNCTION__ << __LINE__;
    }
    return;
}

void QualityControl::Capacity_display_init(QTableWidget *_ptablewidget)
{
    _ptablewidget->setMouseTracking(true);  //设置鼠标追踪
    _ptablewidget->setSelectionBehavior(QAbstractItemView::SelectItems); //整行选中的方式
    _ptablewidget->setEditTriggers(QAbstractItemView::CurrentChanged); //禁止编辑
    _ptablewidget->setFrameShape(QFrame::Box);
    _ptablewidget->horizontalHeader()->setFont(QFont("楷体", 16));
    //点击表时不对表头行光亮（获取焦点）
    _ptablewidget->horizontalHeader()->setHighlightSections(false);
    //设置表头字体加粗
    QFont font = _ptablewidget->horizontalHeader()->font();
    font.setBold(true);
    _ptablewidget->horizontalHeader()->setFont(font);
    _ptablewidget->verticalHeader()->setHidden(true);       //隐藏行号列
    _ptablewidget->setWindowTitle("耗材信息表");

    QStringList headerText_,_projectname,_perunitList;
    _ptablewidget->horizontalHeader()->sectionResizeMode(QHeaderView::Stretch);
    _ptablewidget->setColumnCount(TOTALITEM);


    QHeaderView *header1 = _ptablewidget->verticalHeader();
    header1->setHidden(true);
    _ptablewidget->verticalHeader()->setDefaultSectionSize(45); //默认行高
    _ptablewidget->horizontalHeader()->setDefaultSectionSize(130);
    _ptablewidget->horizontalHeader()->resizeSection(0,150);
    _ptablewidget->horizontalHeader()->setFixedHeight(40); //设置表头的高度
    _ptablewidget->setAlternatingRowColors(true); //隔行换色

   _ptablewidget->horizontalHeader()->setStyleSheet("QHeaderView::section {"
          "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
          "                               stop:0 #6c6c6c, stop:1 #4a4a4a);"
          "   color: white;"
          "   padding: 8px;"
          "   border: 1px solid #3a3a3a;"
          "   font-weight: bold;"
          "   font-size: 12pt;"  // 增大表头字号
          "}"
          "QHeaderView::section:first {"
          "   border-left: 1px solid #3a3a3a;"
          "}"
          "QHeaderView::section:last {"
          "   border-right: 1px solid #3a3a3a;"
          "}");


    _ptablewidget->setStyleSheet(InitTalbe_css);
    //设置水平、垂直滚动条样式
    _ptablewidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
    "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");

    //设置列名
    headerText_<<tr("/")<<tr("AA")<<tr("ADP")<<tr("EPI")<<tr("COL")<<tr("RIS")<<tr("S1清洗液")<<tr("S2清洗液")<<tr("测试杯");
    _perunitList<<"瓶"<<"瓶"<<"瓶"<<"瓶"<<"瓶"<<"瓶"<<"桶"<<"盘";
    _ptablewidget->setHorizontalHeaderLabels(headerText_);


    //设置行名
    _projectname<<tr("单位")<<tr("余额")<<tr("单瓶报警限(%)")<<tr("总余量报警限");
    _ptablewidget->setRowCount(_projectname.size());
    _ptablewidget->verticalHeader()->setVisible(false);
    
    for(int n = 0; n < _projectname.size(); n++){
        QString _itemstr = _projectname.at(n);
		_ptablewidget->blockSignals(true);
        FilltheConsumablesTable(_ptablewidget,n,(int)TableIndex::PROJECT_,_itemstr,false);
		_ptablewidget->blockSignals(false);
    }

    for(int n = 0; n < _perunitList.size(); n++){
        QString _itemstr = _perunitList.at(n);
		_ptablewidget->blockSignals(true);
        FilltheConsumablesTable(_ptablewidget,(int)TableRow::preunit_,n+1,_itemstr,false);
		_ptablewidget->blockSignals(false);
    }

    //整体耗材余额
    displayConsumablesInteger();

    //显示报警线
    ShowConsumablesLimitArm();

    connect(_ptablewidget, SIGNAL(itemPressed(QTableWidgetItem*)),SLOT(tableItemRressed(QTableWidgetItem*)));
    connect(_ptablewidget,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(tableitemNotify(QTableWidgetItem*)));
    return;
}


/*修改耗材的参数*/
void QualityControl::tableitemNotify(QTableWidgetItem* changedItem) {

    constexpr int TARGET_ROW = static_cast<int>(TableRow::LimitAllSuppile);
        if (changedItem->row() != TARGET_ROW) return;

    const quint8 modifiedColumn = changedItem->column();

    // 检查是否为有效列
    if (!COLUMN_CONFIG_MAP.contains(modifiedColumn)) {
        QLOG_DEBUG() << "Ignored modification in column:" << modifiedColumn;
        return;
    }

    // 原子化获取单元格
    QTableWidgetItem* targetItem = ui->tableWidget_Reagent_status->item(
        TARGET_ROW, modifiedColumn
    );

    if (!targetItem) {
        QLOG_ERROR() << "Missing item at [Row:"
                    << TARGET_ROW << " Col:" << modifiedColumn << "]";
        return;
    }

    // 带范围检查的数值转换
    bool isValid = false;
    const quint32 rawValue = targetItem->text().toUInt(&isValid);

    if (!isValid || rawValue > std::numeric_limits<quint8>::max()) {
        QLOG_ERROR() << "Invalid value:" << targetItem->text()
                    << "at [Row:" << TARGET_ROW
                    << " Col:" << modifiedColumn << "]";
        //restorePreviousValue(targetItem);  // 恢复上次有效值
        return;
    }

    // 类型安全转换
    const quint8 configValue = static_cast<quint8>(rawValue);

    // 保存配置
    INI_File config;
    config.wConfigPara(
        COLUMN_CONFIG_MAP.value(modifiedColumn),
        configValue
    );

    // 局部刷新优化
    ui->tableWidget_Reagent_status->viewport()->update(
        ui->tableWidget_Reagent_status->visualItemRect(targetItem)
    );
    return;
}

void QualityControl::tableItemRressed(QTableWidgetItem *ptablepressed)
{
    int pressedcol_ = ptablepressed->column(); //列号
    int pressedrow_ = ptablepressed->row();    //行号

    if(pressedrow_ == TableRow::preunit_){
        // 直接使用当前鼠标位置
        QCursor cursor;
        QPoint mouseGlobalPos = cursor.pos();

        // 获取表格的全局位置
        QPoint tableGlobalPos = ui->tableWidget_Reagent_status->mapToGlobal(QPoint(0, 0));

        // 验证鼠标是否在点击的单元格内
        QRect cellRect = ui->tableWidget_Reagent_status->visualItemRect(ptablepressed);
        QPoint cellGlobalTopLeft = ui->tableWidget_Reagent_status->viewport()->mapToGlobal(cellRect.topLeft());
        QRect cellGlobalRect(cellGlobalTopLeft, cellRect.size());

        if (cellGlobalRect.contains(mouseGlobalPos)) {
            // 鼠标在单元格内，使用鼠标位置
            QPoint tipPos(mouseGlobalPos.x(), mouseGlobalPos.y() - 10); // 稍微向上偏移

            QLOG_DEBUG() << "鼠标定位 - 鼠标位置:" << mouseGlobalPos
                         << "单元格全局矩形:" << cellGlobalRect
                         << "提示位置:" << tipPos;

            outputtips(pressedcol_, tipPos);
        } else {
            // 使用单元格中心点作为备选
            QPoint center = cellGlobalRect.center();
            QLOG_DEBUG() << "使用中心点 - 单元格中心:" << center;
            outputtips(pressedcol_, center);
        }
    }
}

void QualityControl::FilltheConsumablesTable(QTableWidget *ConsumablesTable,int rows,int cols,QString itemdata,bool enable)
{
    QTableWidgetItem *pitem = new QTableWidgetItem(itemdata);
    pitem->setTextColor(QColor(0 ,0, 0));
	if (rows == (int)TableRow::LimitArm)
	{
		switch (cols)
		{
            case TableIndex::S2_:
                pitem->setText("/");
            break;
            case TableIndex::TESTCUPS_:
                pitem->setText("/");
            break;
            default:break;
		}
	}
    ConsumablesTable->setItem(rows,cols,pitem);
    if(!enable)
        pitem->setFlags(Qt::ItemIsEnabled);//设置改item不可修改;
    ConsumablesTable->item(rows,cols)->setFont(QFont( "楷体", 14, QFont::Black ));
    ConsumablesTable->item(rows,cols)->setTextAlignment(Qt::AlignCenter);
    return;
}

void QualityControl::FilltheConsumablesTable(QTableWidget *ConsumablesTable,int rows,
                                              int cols,QString itemdata,QColor textColor,bool enable)
{
    QTableWidgetItem *pitem = new QTableWidgetItem(itemdata);
    pitem->setTextColor(textColor);
    ConsumablesTable->setItem(rows,cols,pitem);
    if(!enable)
        pitem->setFlags(Qt::ItemIsEnabled);//设置改item不可修改;
    
    ConsumablesTable->item(rows,cols)->setFont(QFont( "楷体", 14, QFont::Black ));
    ConsumablesTable->item(rows,cols)->setTextAlignment(Qt::AlignCenter);
    return;
}


QString QualityControl::readBatchnumber(const quint8& index){

    // 使用静态映射表避免每次调用都重新创建
    static const QMap<quint8, quint8> indexMapping = {
        {TableIndex::AA_, INDEX_AA_CONSUMABLE},
        {TableIndex::ADP_, INDEX_ADP_CONSUMABLE},
        {TableIndex::EPI_, INDEX_EPI_CONSUMABLE},
        {TableIndex::COL_, INDEX_COL_CONSUMABLE},
        {TableIndex::RIS_, INDEX_RIS_CONSUMABLE},
        {TableIndex::S1_, INDEX_CLEANLINQUE_CONSUMABLE},
        {TableIndex::S2_, OUTSIDE_CLEANLINQUE_S2},
        {TableIndex::TESTCUPS_, INDEX_TESTTUBE_CONSUMABLE}
    };

    // 检查索引是否存在映射中
    auto it = indexMapping.constFind(index);
    if (it == indexMapping.constEnd()) {
        QLOG_WARN() << "未找到对应的耗材索引:" << index;
        return "";
    }

    quint8 indexCont = it.value();
    const QString consumableName = GenericFunctions::BiteMapingConsumablesName(indexCont) + ".txt";
    const QString filefolder = "ConsumableBatchNumber";

    // 使用静态FileManager避免重复创建（如果线程安全）
    static FileManager fileManager;

    // 提前进行文件存在性检查
    if (!fileManager.exists(filefolder, consumableName)) {
        QLOG_WARN() << "批号文件不存在:" << consumableName;
        return "";
    }

    // 读取并处理文件内容
    QString batchNumber = fileManager.readFromFile(filefolder, consumableName).trimmed();

    QLOG_DEBUG() << "读取耗材批号 - 类型:" << consumableName << "批号:" << batchNumber;

    return batchNumber;
}

void QualityControl::outputtips(int pressedcol, QPoint itempos)
{
    // 定义耗材信息映射表
   static const QMap<int, QPair<QString, QColor>> consumableMap = {
       {TableIndex::AA_,       {"AA批号",       QColor(220, 20, 60)}},     // 深红色
       {TableIndex::ADP_,      {"ADP批号",      QColor(30, 144, 255)}},    // 道奇蓝
       {TableIndex::EPI_,      {"EPI批号",      QColor(46, 139, 87)}},     // 海洋绿
       {TableIndex::COL_,      {"COL批号",      QColor(148, 0, 211)}},     // 深紫色
       {TableIndex::RIS_,      {"RIS批号",      QColor(255, 140, 0)}},     // 深橙色
       {TableIndex::S1_,       {"S1清洗液批号", QColor(70, 130, 180)}},    // 钢蓝色
       {TableIndex::S2_,       {"S2清洗液批号", QColor(178, 34, 34)}},     // 砖红色
       {TableIndex::TESTCUPS_, {"试杯批号",     QColor(160, 0, 160)}}      // 紫红色
   };

   // 检查是否为有效的耗材列
   if (!consumableMap.contains(pressedcol)) {
       return;
   }

   // 读取批号信息
   QString outBatchstr = readBatchnumber(pressedcol);
   QColor textColor = Qt::white;

   // 获取耗材信息和背景色
   auto consumableInfo = consumableMap.value(pressedcol);
   QString title = consumableInfo.first;
   QColor backgroundColor = consumableInfo.second;

   // 设置提示内容
   m_tipswidget->setContent(title + "\n" + outBatchstr, textColor, backgroundColor);

   // 计算并调整显示位置
   adjustTipPosition(itempos);

   // 显示提示框
   m_tipswidget->show();
   QTimer::singleShot(5000, this, SLOT(slot_Timerout()));
}

// 提取位置调整为独立函数
void QualityControl::adjustTipPosition(const QPoint& itempos)
{
    m_tipswidget->adjustSize();
    QSize tipSize = m_tipswidget->size();

    // 计算中心对齐的位置
    QPoint adjustedPos(itempos.x() - tipSize.width() / 2,
                      itempos.y() - tipSize.height() / 2);

    // 获取屏幕边界
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->availableGeometry();

    // 边界检查
    adjustedPos.setX(qMax(screenGeometry.left() + 10,
                         qMin(adjustedPos.x(),
                             screenGeometry.right() - tipSize.width() - 10)));

    adjustedPos.setY(qMax(screenGeometry.top() + 10,
                         qMin(adjustedPos.y(),
                             screenGeometry.bottom() - tipSize.height() - 10)));

    m_tipswidget->move(adjustedPos);
}


void QualityControl::slot_Timerout()
{
   m_tipswidget->close();
}


void QualityControl::handleRechargeSuccessful(int types, quint8 setdata)
{
    const int index_ = types;
    quint8 totalbottle = setdata;
    //同步本地总量
    ConsumablesOper::GetpInstance()->syacnReagentTotalBottle(WRITE_OPERAT, index_, totalbottle);
    if(ITEM_REAGENT_MAP.contains(index_))
    {
        auto iter = ITEM_REAGENT_MAP.find(index_);
        FilltheConsumablesTable(ui->tableWidget_Reagent_status,(int)TableRow::Allowance_,iter.value(),
                                QString::number(totalbottle),false);
    }
    QLOG_DEBUG()<<"充值成功耗材:"<<GlobalData::mapReagentNames(types)<<"充值成功总量:"<< totalbottle;
	update();
	return;
}

//刷卡充值耗材接收消息
void QualityControl::sltoAddConsumables(int types, unsigned short addvalue)
{
    // 参数校验（防御性编程）
    if (addvalue == 0) {
        QLOG_WARN() << "Invalid zero addValue for type:" << types;
        return;
    }

    // 使用智能指针管理实例
    if (!ConsumablesOper::GetpInstance()) {
        QLOG_ERROR() << "ConsumablesOper instance is null";
        return;
    }

    // 合并变量初始化
    const int mapIndex = types;
    quint8 currentValue = 0;
    QString suppliesType = GlobalData::mapReagentNames(mapIndex);

    if (suppliesType.isEmpty() || suppliesType.isNull()) {
        QLOG_ERROR() << "Failed to map consumable type:" << mapIndex;
        return;
    }

    if (!ConsumablesOper::GetpInstance()->_ObtainConsumablesBalance(mapIndex, currentValue)) {
        QLOG_ERROR() << "Failed to get balance for:" << suppliesType;
        return;
    }

    // 使用类型安全的数值计算
    const int calculatedValue = static_cast<int>(currentValue) + addvalue;
        QLOG_DEBUG() << "[" << suppliesType << "] Current:" << currentValue
                   << "Add:" << addvalue << "Total:" << calculatedValue;

    // 数值范围校验
    if (calculatedValue > MAX_CONSUMABLE_VALUE) {
        const QString message = tr("%1充值量超过最大存储值%2").arg(suppliesType)
                                .arg(MAX_CONSUMABLE_VALUE);

       //通过信号通知代替直接调用
       warn_interface::showTransientWarning(tr("读卡写入失败"), message);
       QLOG_WARN() << "Overflow: " << message;
       return;
    }

    // 指令生成模块化
    QByteArray command;
    QUIUtils::ConfirmReadthecardRecharge(mapIndex, EVERYTIME_READ_MAINSTATE,
                                         static_cast<quint8>(calculatedValue),
                                         command);


    // 发送指令并记录
    static const QString CONFIRM_CMD = tr("确认充卡");
    emit sendDirectives(command, CONFIRM_CMD);
    QLOG_INFO() << "Command sent for:" << suppliesType << "Value:" << calculatedValue;
    return;
}


void QualityControl::recv_updateTrayUsed(int IndexTray)
{
    switch(IndexTray)
    {
        case equipmentTipInfo::TestTubeTrayI:    updatetrayAllTube(m_tubeStateVec_1,true);
        break;
        case equipmentTipInfo::TestTubeTrayII:   updatetrayAllTube(m_tubeStateVec_2,true);
        break;
        case equipmentTipInfo::TestTubeTrayIII:  updatetrayAllTube(m_tubeStateVec_3,true);
        break;
        case equipmentTipInfo::TestTubeTrayIIII: updatetrayAllTube(m_tubeStateVec_4,true);
        break;
    }
    update();
}


void QualityControl::TotalConsumablesAlarm()
{
    static const QList<quint8> indexConsumablesList = {
        INDEX_AA_CONSUMABLE, INDEX_ADP_CONSUMABLE, INDEX_EPI_CONSUMABLE,
        INDEX_COL_CONSUMABLE, INDEX_RIS_CONSUMABLE, INDEX_CLEANLINQUE_CONSUMABLE,
        INDEX_TESTTUBE_CONSUMABLE, OUTSIDE_CLEANLINQUE_S2
    };

    QStringList alarmList;
    QString consumableName;

    for (quint8 index : indexConsumablesList) {
        bool isAlarm = false; // 每次循环重置状态
        if (auto* oper = ConsumablesOper::GetpInstance()) {
            oper->TotalConsumablesAlarm(index, isAlarm);
        }
        consumableName = GlobalData::mapReagentNames(index);
        if (isAlarm) {
            alarmList.append(consumableName);
        }
    }

    if (!alarmList.isEmpty() && cglobal::gserialConnecStatus) {
        const QString message = tr("请充值耗材: %1。").arg(alarmList.join(tr(", ")));
        warn_interface::showTransientWarning(tr("耗材报警"), message);
    }
}


