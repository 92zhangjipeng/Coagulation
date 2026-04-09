#pragma execution_character_set("utf-8")
#include "customplot.h"
#include "ui_customplot.h"
#include "loginui.h"
#include "verifycoordinates.h"
#include "QsLog/include/QsLog.h"
#include <QAbstractItemView>
#include <qDebug>
#include <QSpinBox>
#include <QHeaderView>
#include <QStyledItemDelegate>
#include <QTime>
#include <QElapsedTimer>
#include <QThread>
#include <QStandardItemModel>
#include <QVector>
#include <QTextCodec>
#include <algorithm>
#include <numeric>
#include <random>



bool compairtool(QToolButton *holenum, QToolButton *endhole)
{
   if (!holenum || !endhole) return false;

   auto getIndex = [](const QToolButton* btn) {
       return QUIUtils::StringFindintnum(btn->objectName());
   };

   return getIndex(holenum) < getIndex(endhole);
}

bool sortWidgetNumLoc(QWidget *fWidget,QWidget *eWidget)
{
    // 添加空指针检查
    if (!fWidget || !eWidget) return false;

    int index_s = QUIUtils::StringFindintnum(fWidget->objectName());
    int index_e = QUIUtils::StringFindintnum(eWidget->objectName());

    return index_s < index_e;
}


CustomPlot::CustomPlot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CustomPlot),
    m_downhigh(0),
    mcodeNum(0),
    m_CalibrationArea(-1),
    mbstopTestadjust(false)
 {
    ui->setupUi(this);
    this->setWindowTitle(tr("坐标位置微调"));
    mfont.setFamily("楷体");
    mfont.setPointSize(10);//设置文字大小

    mcustFont.setFamily("楷体");
    mcustFont.setPointSize(12);

    // 设置LED指示灯
    auto* led = ui->widget_displayled;
    led->setColors(QSimpleLed::CUSTOM);
    led->setFixedSize(32, 32);
    led->setCustomOnColor0(QColor(0, 127, 0));   // 暗绿
    led->setCustomOnColor1(QColor(0, 255, 0));   // 亮绿
    led->setCustomOffColor0(QColor(127, 0, 0));  // 暗红
    led->setCustomOffColor1(QColor(255, 0, 0));  // 亮红
    led->setStates(QSimpleLed::LEDSTATES::OFF);

    ui->pushButton_TrayHands->hide();
    initSpinBoxStyle();
}

CustomPlot::~CustomPlot()
{
    // 断开所有可能引起问题的连接
    disconnect(this, nullptr, nullptr, nullptr);

    if(mpTestCaseRun != nullptr)
    {
        delete mpTestCaseRun;
        mpTestCaseRun = nullptr;
    }
    // m_CheckGroupBox and mbloodBtnGroupbox are child objects with parent 'this'
    // They will be automatically deleted by Qt's parent-child mechanism
    // No need to delete them manually to avoid double deletion
    delete ui;
}

void CustomPlot::initSpinBoxStyle()
{
    // 优化X坐标SpinBox样式 - 加减号变小并居中
    m_SpinboxsheetX =
        "QSpinBox {"
           "   background: white;"
           "   border: 1px solid #dcdfe6;"
           "   border-radius: 4px;"
           "   text-align: center;"
           "   padding: 2px 4px;"
           "   min-width: 100px;"
           "   max-width: 120px;"
           "}"
           "QSpinBox::up-button, QSpinBox::down-button {"
           "   width: 14px;"
           "   height: 12px;"
           "}"
           "QSpinBox::up-arrow, QSpinBox::down-arrow {"
           "   width: 5px;"
           "   height: 5px;"
           "}";

    // Y坐标SpinBox样式（同样优化）
    m_SpinboxsheetY = m_SpinboxsheetX;
}

void CustomPlot::initstyle(const quint8 equipmentType)
{
    initCommboxView(equipmentType);

    if (m_CheckGroupBox) {
        delete m_CheckGroupBox;
        m_CheckGroupBox = nullptr;
    }
    m_CheckGroupBox = new  QButtonGroup(this);
    m_CheckGroupBox->setExclusive(true); //互斥
    m_CheckGroupBox->addButton(ui->checkBox_downTop,TheGripperDrops::GipperDown_top);
    m_CheckGroupBox->addButton(ui->checkBox_downmind,TheGripperDrops::GipperDown_mid);
    m_CheckGroupBox->addButton(ui->checkBox_downbottom,TheGripperDrops::GipperDown_bottom);
    m_CheckGroupBox->addButton(ui->checkBox_downcustom,TheGripperDrops::GipperDown_custom);
    connect(m_CheckGroupBox, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(CheckGipperDownHigh(QAbstractButton*)));

    InitdisplayPointTablewidget(ui->tableWidget_displayPos);

    if (mbloodBtnGroupbox) {
        delete mbloodBtnGroupbox;
        mbloodBtnGroupbox = nullptr;
    }
    mbloodBtnGroupbox = new QButtonGroup(this);
    mbloodBtnGroupbox->setExclusive(true);

    initBloodZoneNum(equipmentType);  //初始化血样区

    initEmptyTubeHole(equipmentType); //空试管区

    initTestChnHole(equipmentType);  //测试通道

   //试剂区
   QList<QToolButton *> ReagentsArea =  ui->widget_ReagentsArea->findChildren<QToolButton* >();
   std::sort(ReagentsArea.begin(),ReagentsArea.end(),compairtool);
   for(QToolButton* Reagentstube : ReagentsArea)
   {
       Reagentstube->setStyleSheet(m_Cssbtn);
       QObject::connect(Reagentstube,&QToolButton::clicked,this,&CustomPlot::ClickReagentsTube);
   }
   ui->toolButton_throwTube->setStyleSheet(m_Cssbtn);
   ui->toolButton_origin->setStyleSheet(m_Cssbtn);
   ui->toolButton_Clean_1->setStyleSheet(m_Cssbtn);

   mbloodBtnGroupbox->addButton(ui->toolButton_throwTube); //弃杯孔
   mbloodBtnGroupbox->addButton(ui->toolButton_origin);    //原点
   mbloodBtnGroupbox->addButton(ui->toolButton_Clean_1);
   //mbloodBtnGroupbox->setExclusive(true);// 设置是否互斥

   //点击原点校验位置
   connect(ui->toolButton_origin,&QToolButton::clicked,this,[=](){
       userClickOriginAxis();
   });

}





void CustomPlot::hideRowsByEquipmentType(quint8 equipmentType)
   {
       auto* view = qobject_cast<QListView*>(ui->comboBox_calibrationAarea->view());
       if (!view) return;

       switch (equipmentType)
       {
       case KS600:
           hideRows(view, {EmptyTube_3_BloodNeedle, EmptyTube_4_BloodNeedle,
                           EmptyTube_3_Hands, EmptyTube_4_Hands});
           break;
       case KS800:
           hideRows(view, {EmptyTube_4_BloodNeedle, EmptyTube_4_Hands});
           break;
       default:
           break;
       }
   }

void CustomPlot::hideRows(QListView* view, const std::vector<int>& rows)
    {
        for (int row : rows)
        {
            view->setRowHidden(row, true);
        }
    }

void CustomPlot::setItemBackgroundColors()
    {
        auto* model = qobject_cast<QStandardItemModel*>(ui->comboBox_calibrationAarea->model());
        if (!model) return;

        // 样本针区域背景色
        const QColor sampleNeedleColor(190, 190, 190);
        for (int i = Origin_bloodNeedle; i < CleanLinque_ReagentNeedle; ++i)
        {
            if (auto* item = model->item(i))
                item->setBackground(sampleNeedleColor);
        }

        // 抓手区域背景色
        const QColor handsColor(211, 211, 211);
        for (int i = TestChannel_Hands; i <= EmptyTube_4_Hands; ++i)
        {
            if (auto* item = model->item(i))
                item->setBackground(handsColor);
        }
    }

void CustomPlot::initCommboxView(quint8 equipmentType)
{
    // 使用初始化列表构建映射表
    const QMap<int, QString> instrumentAdjust = {
        {Origin_bloodNeedle,            "清洗区原点"},
        {CleanLinque_bloodNeedle,       "[样本针]清洗液"},
        {Bloodsample_BloodNeedle,       "[样本针]样本区"},
        {EmptyTube_1_BloodNeedle,       "[样本针]试杯区1"},
        {EmptyTube_2_BloodNeedle,       "[样本针]试杯区2"},
        {EmptyTube_3_BloodNeedle,       "[样本针]试杯区3"},
        {EmptyTube_4_BloodNeedle,       "[样本针]试杯区4"},
        {CleanLinque_ReagentNeedle,     "[试剂针]清洗液"},
        {ReagentLinque_ReagentNeedle,   "[试剂针]试剂区"},
        {TestChannel_ReagentNeedle,     "[试剂针]测试区"},
        {TestChannel_Hands,             "[抓手]测试区"},
        {ThrowCup_Hands,                "[抓手]弃杯孔"},
        {EmptyTube_1_Hands,             "[抓手]试杯区1"},
        {EmptyTube_2_Hands,             "[抓手]试杯区2"},
        {EmptyTube_3_Hands,             "[抓手]试杯区3"},
        {EmptyTube_4_Hands,             "[抓手]试杯区4"}
    };

    ui->comboBox_calibrationAarea->blockSignals(true);

    // 插入所有项目
   for (auto it = instrumentAdjust.begin(); it != instrumentAdjust.end(); ++it)
   {
       ui->comboBox_calibrationAarea->insertItem(it.key(), it.value());
   }


   // 根据设备类型隐藏特定行
   hideRowsByEquipmentType(equipmentType);

  // 设置不同区域背景色
  setItemBackgroundColors();

  // 设置行高代理
  ui->comboBox_calibrationAarea->setItemDelegate(new MyQStyledItemDelegate(30, this));
  ui->comboBox_calibrationAarea->setCurrentIndex(-1);
  ui->comboBox_calibrationAarea->blockSignals(false);
}








void CustomPlot::initBloodZoneNum(quint8 indexModels)
{
    // 型号对应的显示数量映射
    static const QHash<quint8, int> modelToShowNum = {
        {KS600, 6},
        {KS800, 9},
        {KS1200, 12}
    };
    const int showNum = modelToShowNum.value(indexModels, 12);
    constexpr int MAX_BLOOD_ZONE = 12;

    // 确保按钮组存在
    if (!mbloodBtnGroupbox) {
        mbloodBtnGroupbox = new QButtonGroup(this);
    }

    // 批量处理所有血区
    for (int i = 0; i < MAX_BLOOD_ZONE; ++i)
    {
        QWidget* const bloodWidget = ui->widget_BloodArea->findChild<QWidget*>(
            QString("widget_Bloodchild_%1").arg(i + 1)
        );

        if (!bloodWidget) continue;

        // 超出显示数量：隐藏并跳过
        const bool shouldHide = (i >= showNum);
        bloodWidget->setVisible(!shouldHide);
        if (shouldHide) continue;

        // 获取并排序按钮
        auto buttons = bloodWidget->findChildren<QToolButton*>();
        if (buttons.isEmpty()) continue;

        std::sort(buttons.begin(), buttons.end(), compairtool);

        // 配置按钮
        int subIndex = 1;
        for (auto* btn : buttons)
        {
            btn->setText(QString::number(subIndex + i * 10));
            btn->setFont(mfont);
            btn->setCheckable(true);
            btn->setAutoExclusive(true);
            btn->setStyleSheet(m_Cssbtn);

            // 安全建立连接（避免重复）
            disconnect(btn, &QToolButton::clicked, this, &CustomPlot::ClickBloodTube);
            connect(btn, &QToolButton::clicked, this, &CustomPlot::ClickBloodTube);

            // 安全添加到按钮组
            if (mbloodBtnGroupbox->buttons().contains(btn)) {
                mbloodBtnGroupbox->removeButton(btn);
            }
            mbloodBtnGroupbox->addButton(btn);

            ++subIndex;
        }
    }

    update();
}




void CustomPlot::initEmptyTubeHole(quint8 indexType)
{
    // 型号对应的显示数量映射
   static const QHash<quint8, int> kModelShowNum = {
       {KS600, 2},
       {KS800, 3},
       {KS1200, 4}
   };
   const int showNum = kModelShowNum.value(indexType, 4);
   constexpr int MAX_TUBE_TRAY = 4;

   // 确保按钮组存在
   if (!mbloodBtnGroupbox) {
       mbloodBtnGroupbox = new QButtonGroup(this);
   }

   // 处理每个试管托盘
   for (int i = 0; i < MAX_TUBE_TRAY; ++i)
   {
       QString findKey = QString("widget_testtubetray_%1").arg(i + 1);
       QWidget* trayWidget = ui->widget_trayall->findChild<QWidget*>(findKey);

       if (!trayWidget) continue;

       // 超出显示数量：隐藏并跳过
       const bool shouldHide = (i >= showNum);
       trayWidget->setVisible(!shouldHide);
       if (shouldHide) continue;

       // 获取并排序按钮
       auto buttons = trayWidget->findChildren<QToolButton*>();
       if (buttons.isEmpty()) continue;

       std::sort(buttons.begin(), buttons.end(), compairtool);

       // 配置按钮
       int subIndex = 1;
       for (auto* btn : buttons)
       {
           const int tubeNumber = subIndex + 60 * i;
           btn->setText(QString::number(tubeNumber));
           btn->setFont(mfont);
           btn->setCheckable(true);
           btn->setAutoExclusive(true);
           btn->setStyleSheet(m_Cssbtn);

           // 安全建立连接（避免重复）
           disconnect(btn, &QToolButton::clicked, this, &CustomPlot::ClickEmptyTube);
           connect(btn, &QToolButton::clicked, this, &CustomPlot::ClickEmptyTube);

           // 安全添加到按钮组
           if (mbloodBtnGroupbox->buttons().contains(btn)) {
               mbloodBtnGroupbox->removeButton(btn);
           }
           mbloodBtnGroupbox->addButton(btn);

           ++subIndex;
       }
   }

   update();  // 如果需要刷新界面
}



void CustomPlot::initTestChnHole(quint8 indexType)
{
    // 获取所有测试通道按钮
    auto testChannelArea = ui->widget_testchannelArea->findChildren<QToolButton*>();
    if (testChannelArea.isEmpty()) return;

    std::sort(testChannelArea.begin(), testChannelArea.end(), compairtool);

    // 型号对应的最大通道数映射
    static const QHash<quint8, int> kModelMaxChannel = {
        {KS600, 4},
        {KS800, 8},
        {KS1200, 12}  // KS1200 不限制，12是最大值
    };
    const int maxChannel = kModelMaxChannel.value(indexType, 12);

    // 配置每个通道按钮
    for (auto* btn : testChannelArea)
    {
        int index = QUIUtils::StringFindintnum(btn->objectName());

        // 判断是否需要隐藏
        bool shouldHide = (maxChannel > 0 && index > maxChannel);

        if (shouldHide)
        {
            btn->hide();
        }
        else
        {
            btn->show();  // 确保之前隐藏的按钮重新显示
            btn->setStyleSheet(m_Cssbtn);

            // 避免重复连接
            disconnect(btn, &QToolButton::clicked, this, &CustomPlot::ClickTestChannelTube);
            connect(btn, &QToolButton::clicked, this, &CustomPlot::ClickTestChannelTube);
        }
    }

    update();
}


void CustomPlot::InitdisplayPointTablewidget(QTableWidget* Tablewidgetpos)
{
    if (!Tablewidgetpos) return;

    // 设置表头
    QStringList header = {tr("编号"), tr("X坐标"), tr("Y坐标"), tr("保存")};
    Tablewidgetpos->setColumnCount(header.size());
    Tablewidgetpos->setHorizontalHeaderLabels(header);

    // 基础配置
    Tablewidgetpos->setFocusPolicy(Qt::NoFocus);
    Tablewidgetpos->setFrameShape(QFrame::Box);
    Tablewidgetpos->setShowGrid(true);
    Tablewidgetpos->verticalHeader()->setVisible(false);
    Tablewidgetpos->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Tablewidgetpos->setSelectionMode(QAbstractItemView::NoSelection);
    Tablewidgetpos->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 行高和列宽（进一步增加行高，确保控件完全显示）
    Tablewidgetpos->verticalHeader()->setDefaultSectionSize(45);
    Tablewidgetpos->horizontalHeader()->setFixedHeight(40);
    Tablewidgetpos->horizontalHeader()->setDefaultSectionSize(40);
    Tablewidgetpos->setColumnWidth(TableIndexPos::IndexNum, 50);
    Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_xpos, 135);
    Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_ypos, 135);
    Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_save, 105);

    // 表头样式（减小字体和padding）
    QFont font;
    font.setFamily("Microsoft YaHei");
    font.setBold(true);
    font.setPixelSize(13);  // 从16降到13
    Tablewidgetpos->horizontalHeader()->setFont(font);
    Tablewidgetpos->horizontalHeader()->setHighlightSections(false);
    Tablewidgetpos->horizontalHeader()->setStretchLastSection(true);
    Tablewidgetpos->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    Tablewidgetpos->setStyleSheet(
        "QTableWidget {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #ffffff, stop:1 #f8f9fa);"
        "   alternate-background-color: #f8f9fa;"
        "   border: 1px solid #d1d8e0;"
        "   border-radius: 6px;"
        "   gridline-color: #e2e8f0;"
        "   font: 12px 'Segoe UI', 'Microsoft YaHei';"
        "}"
        "QTableWidget::item {"
        "   padding: 2px 4px;"  // 进一步减少内边距，为控件腾出空间
        "   border-bottom: 1px solid #e9ecef;"
        "}"
        "QTableWidget QWidget {"
        "   margin: 0px;"
        "   padding: 0px;"
        "}"
        "QTableWidget::item:selected {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #007bff, stop:1 #0056b3);"
        "   color: white;"
        "   border-radius: 2px;"
        "}"
        "QHeaderView {"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "       stop:0 #1a252f, stop:1 #2c3e50);"
        "   border: none;"
        "   border-radius: 6px;"  // 从10降到6
        "   min-height: 32px;"     // 从44降到32
        "   max-height: 32px;"     // 限制最大高度
        "}"
        "QHeaderView::section {"
        "   background: transparent;"
        "   color: #ecf0f1;"
        "   font: bold 13px 'Segoe UI', 'Microsoft YaHei';"  // 从14降到13
        "   padding: 6px 12px;"     // 从10px 15px降到6px 12px
        "   border: none;"
        "   border-right: 1px solid #34495e;"
        "}"
        "QHeaderView::section:last { border-right: none; }"
        "QHeaderView::section:hover {"
        "   background: rgba(255, 255, 255, 0.1);"
        "}"
        "QScrollBar:vertical {"
        "   background: #f8f9fa;"
        "   width: 10px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #adb5bd;"
        "   border-radius: 5px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover { background: #6c757d; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar:horizontal {"
        "   background: #f8f9fa;"
        "   height: 10px;"
        "   border-radius: 5px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "   background: #adb5bd;"
        "   border-radius: 5px;"
        "   min-width: 20px;"
        "}"
        "QScrollBar::handle:horizontal:hover { background: #6c757d; }"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0px; }"
    );

    // 确保行高固定，不会自动调整
    Tablewidgetpos->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    // 设置表格尺寸策略，使用Interactive模式允许手动调整，最后一列拉伸
    Tablewidgetpos->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
    Tablewidgetpos->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    Tablewidgetpos->horizontalHeader()->setStretchLastSection(true);

    // 如果这是主显示表格，使其宽度与右侧widget保持一致
    if (Tablewidgetpos == ui->tableWidget_displayPos && ui->widget_Rightwidget) {
        // 获取右侧widget的宽度
        int rightWidgetWidth = ui->widget_Rightwidget->width();

        // 只有在widget有有效宽度时才设置
        if (rightWidgetWidth > 100) {
            // 减去边框和内边距
            int borderPadding = 8;

            // 计算表格宽度
            int tableWidth = rightWidgetWidth - borderPadding;

            // 确保宽度在合理范围内
            if (tableWidth < 420) tableWidth = 420;
            if (tableWidth > 650) tableWidth = 650;

            // 移除UI文件中设置的最大宽度限制(430)
            Tablewidgetpos->setMaximumWidth(16777215);

            // 设置表格固定宽度，使其与widget_Rightwidget匹配
            Tablewidgetpos->setFixedWidth(tableWidth);

            // 重新设置列宽（保持原有比例，最后一列会拉伸）
            Tablewidgetpos->setColumnWidth(TableIndexPos::IndexNum, 50);
            Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_xpos, 135);
            Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_ypos, 135);
            Tablewidgetpos->setColumnWidth(TableIndexPos::Instrument_save, 105);

            // 最后一列拉伸以填充剩余空间
            Tablewidgetpos->horizontalHeader()->setStretchLastSection(true);

            QLOG_DEBUG() << "调整表格宽度与widget_Rightwidget一致:"
                         << "widget宽度=" << rightWidgetWidth
                         << "表格宽度=" << tableWidth;
        }
    }
}






//下降高度
void CustomPlot::CheckGipperDownHigh(QAbstractButton *btn)
{
    if (!btn || !m_CheckGroupBox) return;

     m_indexDown = m_CheckGroupBox->id(btn);

    // 使用 constexpr 结构体数组
    static constexpr struct {
        int id;
        int height;
    } kPresets[] = {
        {TheGripperDrops::GipperDown_top, 10},
        {TheGripperDrops::GipperDown_mid, 30},
        {TheGripperDrops::GipperDown_bottom, 50}
    };

    int height = ui->spinBox_downValue->value();  // 默认值

    for (const auto& preset : kPresets) {
        if (preset.id == m_indexDown) {
            height = preset.height;
            break;
        }
    }

    m_downhigh = height;
}


//弃杯孔
void CustomPlot::displayThrowHole(bool bFindShow,bool bWrite_x,int notifyValue)
{
    auto* axis = SingletonAxis::GetInstance();
    if(bFindShow){
        // 显示弃杯孔位置
        QPoint throwHolePoint;
        axis->throwTubeHolePos(READ_OPERRAT, throwHolePoint);
        QMap<quint8, QPoint> points = {{0, throwHolePoint}};
        notifyShowInstrumentPoint(points, "弃杯孔(抓手)微调");
    }
    else
    {
        axis->oper_ThrowTubeHolePos(bWrite_x,notifyValue);
    }
}



//查找原点坐标
void CustomPlot::displayOriginAxisPoint(bool bFindShow,bool bWrite_x,int notifyValue)
{
    auto* pconfAxis = SingletonAxis::GetInstance();
    if(bFindShow){

        QPoint originPoint;
        pconfAxis->originPos(READ_OPERRAT, originPoint);
        QMap<quint8, QPoint> points = {{0, originPoint}};
        notifyShowInstrumentPoint(points, "微调原点坐标");
    }
    else
    {
        pconfAxis->operOriginAxis(bWrite_x,notifyValue);
    }
}

//清洗液区
void CustomPlot::displayCleanLinqueAxisPoint(bool bFindShow, quint8  indexZ,bool bWrite_x, int notifyValue)
{
    auto* pconfAxis = SingletonAxis::GetInstance();
    if(bFindShow)
    {
        QPoint cleanPoint;
        pconfAxis->cleanZoneAxisPos(READ_OPERRAT, indexZ, cleanPoint);

        QMap<quint8, QPoint> points = {{0, cleanPoint}};
        switch (indexZ) {
            case MOTOR_BLOOD_INDEX:
                notifyShowInstrumentPoint(points, "清洗液(样本针)微调");
                break;
            case MOTOR_REAGNET_INDEX:
                notifyShowInstrumentPoint(points, "清洗液(试剂针)微调");
                break;
        }
    }
    else{
        pconfAxis->oper_CleanZonePos(bWrite_x,indexZ,notifyValue);
    }
    return;
}

//试剂区
int CustomPlot::displayReagentAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QPoint backPoint(0,0);
    int offset_v = 0;
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();
        for(int i = 0 ;i < 10; i++)
        {
            SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,i,backPoint);
            readAxis.insert(i,backPoint);
        }
        InsertOneChangeOthersChange(readAxis,"试剂区(试剂针)微调");
    }
    else
    {
        SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,0,backPoint);
        if(bWrite_x == NOTIFY_XPOINT)
            offset_v =  notifyValue - backPoint.x();
        else
            offset_v =  notifyValue - backPoint.y();
        for(int i = 0;  i < 10 ; i++)
        {
            SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,i,backPoint);
            if(bWrite_x == NOTIFY_XPOINT)
                SingletonAxis::GetInstance()->oper_ReagentZonePos(bWrite_x,i,backPoint.x() + offset_v);
            else
                SingletonAxis::GetInstance()->oper_ReagentZonePos(bWrite_x,i,backPoint.y() + offset_v);
        }
    }
    return offset_v;
}

//测试通道
void CustomPlot::displayTestChnAxisPoint(bool bFindShow, bool bWrite_x,quint8 indexZ, quint8 ChnNum,int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    quint8 Typtind = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,Typtind);
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();
        quint8 totalChn = 0;
        QPoint backPoint(0,0);
        switch (Typtind)
        {
            case KS600: totalChn = 4;  break;
            case KS800: totalChn = 8;   break;
            case KS1200: totalChn = 12; break;
            default: totalChn = 12;     break;
        }
        for(int i = 0 ;i < totalChn; i++)
        {
            SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,i,indexZ,backPoint);
            readAxis.insert(i,backPoint);
        }
        if(indexZ == MOTOR_REAGNET_INDEX)
            notifyShowInstrumentPoint(readAxis,"测试区(试剂针)微调");
        else if(indexZ == MOTOR_HANDS_INDEX)
            notifyShowInstrumentPoint(readAxis,"测试区(抓手)微调");
    }
    else
    {
        SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(bWrite_x,ChnNum,indexZ,notifyValue);
    }
    return;
}

//测试杯区
int CustomPlot::displayEmptyTrayAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ,quint8 indexTray,quint8 tubeNum, int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    quint8 totalHole_s = tubeNum;
    int offset_v = 0;
    quint8 totalHole_e = 0;
    QPoint backPoint(0,0);
    switch (indexTray)
    {
        case 1:
            totalHole_s = 0;
            totalHole_e = 60;
        break;
        case 2:
            totalHole_s = 60;
            totalHole_e = 120;
        break;
        case 3:
            totalHole_s = 120;
            totalHole_e = 180;
        break;
        case 4:
            totalHole_s = 180;
            totalHole_e = 240;
        break;
        default:
           break;
    }
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();
        for(int i = totalHole_s; i < totalHole_e; i++)
        {
            SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,i,indexZ,backPoint);
            readAxis.insert(i,backPoint);
        }
        if(indexZ == MOTOR_BLOOD_INDEX)
            InsertOneChangeOthersChange(readAxis, QString("[样本针]试杯区(%1)偏移微调").arg(indexTray));
        else if(MOTOR_HANDS_INDEX == indexZ)
            InsertOneChangeOthersChange(readAxis, QString("[抓手]试杯区(%1)偏移微调").arg(indexTray));
    }
    else
    {
         SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,totalHole_s,indexZ,backPoint);
         if(bWrite_x == NOTIFY_XPOINT)
            offset_v = notifyValue - backPoint.x();
         else
            offset_v = notifyValue - backPoint.y();
         for(int k = totalHole_s; k < totalHole_e; k++)
         {
             SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,k,indexZ,backPoint);
             if(bWrite_x == NOTIFY_XPOINT)
                 SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_XPOINT,k,indexZ ,backPoint.x() + offset_v);
             else
                SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_YPOINT, k ,indexZ,backPoint.y() + offset_v);
         }
    }
    return offset_v;
}

int CustomPlot::displayBloodHoleAxisPoint(bool bFindShow, bool bWrite_x,quint8 indexhole, int notifyValue)
{
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    quint8 Typtind = 0;
    int offset_v = 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,Typtind);
    quint8 totalHole = 0;
    QPoint backPoint(0,0);
    switch (Typtind)
    {
        case KS600:  totalHole = 70; break;
        case KS800:  totalHole = 90; break;
        case KS1200: totalHole = 120; break;
        default: totalHole = 120;     break;
    }
    if(bFindShow)
    {
        QMap<quint8,QPoint> readAxis;
        readAxis.clear();

        for(int i = 0; i < totalHole; i++)
        {
            SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,i,backPoint);
            readAxis.insert(i,backPoint);
        }
        InsertOneChangeOthersChange(readAxis,tr("[样本针]样本区偏移微调"));
    }
    else
    {
        SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,indexhole,backPoint);
        if(bWrite_x == NOTIFY_XPOINT)
            offset_v = notifyValue - backPoint.x();
        else
            offset_v = notifyValue - backPoint.y();
        for(int i = 0; i < totalHole; i++)
        {
            SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,i,backPoint);
            if(bWrite_x == NOTIFY_XPOINT)
                SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_XPOINT,i,backPoint.x() + offset_v);
            else
               SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_YPOINT,i,backPoint.y() +  offset_v);
        }
    }
    return offset_v;
}

//区域
void CustomPlot::on_comboBox_calibrationAarea_activated(int index)
{
    if(index == -1 )     return;
    if(m_indexDown < 0)
    {
        QMessageBox msgBox;
        msgBox.setIconPixmap(QPixmap(":/Picture/SetPng/remindererroe.png"));
        msgBox.setText("选取失败,请选择下针高度");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Ok);
        int ret = msgBox.exec();
        switch (ret)
        {
            case QMessageBox::Ok:     return;  break;
            case QMessageBox::Cancel: return;  break;
            default:
            break;
        }
    }
    //先清空表内容
    DelTableText();
    m_CalibrationArea = index;
    switch(index)
    {
        case Origin_bloodNeedle:
        {
            displayOriginAxisPoint(true,false,0);
            break;
        }
        case CleanLinque_bloodNeedle:
        {
            displayCleanLinqueAxisPoint(true,MOTOR_BLOOD_INDEX,false,0);
            break;
        }
        case CleanLinque_ReagentNeedle:
        {
            displayCleanLinqueAxisPoint(true,MOTOR_REAGNET_INDEX,false,0);
            break;
        }
        case ReagentLinque_ReagentNeedle:
        {
            displayReagentAxisPoint(true, false, 0);
            break;
        }
        case TestChannel_ReagentNeedle:
        {
            displayTestChnAxisPoint(true,false,MOTOR_REAGNET_INDEX,0,0);
            break;
        }
        case TestChannel_Hands:
        {
            displayTestChnAxisPoint(true,false,MOTOR_HANDS_INDEX,0,0);
            break;
        }
        case ThrowCup_Hands:
        {
            displayThrowHole(true,false,0);
            break;
        }
        case EmptyTube_1_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,1,0, 0);

            break;
        }

        case EmptyTube_1_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,1,0, 0);
            break;
        }

        case EmptyTube_2_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,2,0, 0);
            break;
        }

        case EmptyTube_2_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,2,0, 0);
            break;
        }

        case EmptyTube_3_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,3,0, 0);
            break;
        }

        case EmptyTube_3_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,3,0, 0);
            break;
        }
        case EmptyTube_4_Hands:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX,4,0, 0);
            break;
        }
        case EmptyTube_4_BloodNeedle:
        {
            displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX,4,0, 0);
            break;
        }
        case Bloodsample_BloodNeedle:
        {
            displayBloodHoleAxisPoint(true,false,0,0);
            break;
        }
        default:
        break;
    }
    return;
}


QWidget* CustomPlot::createCenteredButton(const QString& text, int rowIndex, bool needChangeOther)
{
    Q_UNUSED(rowIndex);
    QWidget* container = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    QPushButton* btn = new QPushButton(text, container);
    btn->setFont(mcustFont);
    btn->setFixedSize(80, 28);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "   font: 12px 'Microsoft YaHei';"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3d8b40; }"
    );

    if (!needChangeOther) {
        connect(btn, &QPushButton::clicked, this, &CustomPlot::BtnClickSavePoint);
    } else {
        connect(btn, &QPushButton::clicked, this, &CustomPlot::BtnClickOthersSavePoint);
    }

    layout->addWidget(btn);
    container->setLayout(layout);

    return container;
}
void CustomPlot::AddBtnSave_backValue(int rowIndex,bool NeedChaneOther)
{
    ui->tableWidget_displayPos->setCellWidget(rowIndex, TableIndexPos::Instrument_save,
           createCenteredButton("保存微调", rowIndex, NeedChaneOther));
}



QWidget* CustomPlot::createCenteredSpinBox(int value, const QString& style)
{
    // 创建容器Widget
    QWidget* container = new QWidget(this);
    QHBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(Qt::AlignCenter);

    // 创建SpinBox
    QSpinBox* spinBox = new QSpinBox(container);
    spinBox->setStyleSheet(style);
    spinBox->setAlignment(Qt::AlignCenter);
    spinBox->setMaximum(5500);
    spinBox->setFixedHeight(30);
    spinBox->setFixedWidth(110);  // 减小宽度以适应更窄的列
    spinBox->setValue(value);

    // 设置更大的字体
    QFont font = spinBox->font();
    font.setPixelSize(15);
    spinBox->setFont(font);

    // 设置+/-按钮样式
    spinBox->setButtonSymbols(QSpinBox::PlusMinus);

    // 将SpinBox指针存储为容器属性，便于后续访问
    container->setProperty("innerSpinBox", QVariant::fromValue<QSpinBox*>(spinBox));

    layout->addWidget(spinBox);
    container->setLayout(layout);

    return container;
}

void CustomPlot::notifyShowInstrumentPoint(QMap<quint8,QPoint>& displayPoint ,const QString& tableName)
{
    ui->label_title->setText(tableName);

    // 清空表格（可选，根据需求）
    //ui->tableWidget_displayPos->clearContents();
    //ui->tableWidget_displayPos->setRowCount(0);

    for (auto iter = displayPoint.constBegin(); iter != displayPoint.constEnd(); ++iter) {
        const int row = ui->tableWidget_displayPos->rowCount();
        ui->tableWidget_displayPos->insertRow(row);

        // 序号列
        QTableWidgetItem* item = new QTableWidgetItem(QString::number(row + 1));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignCenter);
        item->setBackground(QColor(204, 204, 204));
        ui->tableWidget_displayPos->setItem(row, TableIndexPos::IndexNum, item);

        // X坐标列 - 使用容器居中
        ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_xpos,
            createCenteredSpinBox(iter.value().x(), m_SpinboxsheetX));

        // Y坐标列
        ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_ypos,
            createCenteredSpinBox(iter.value().y(), m_SpinboxsheetY));

        AddBtnSave_backValue(row, false);
    }
}




void CustomPlot::InsertOneChangeOthersChange(QMap<quint8,QPoint> displayPoint, const QString Tablename)
{
    ui->label_title->setText(Tablename);
    int InsertRow = 0;
    auto iter = displayPoint.begin();
    while(iter != displayPoint.end())
    {
        ui->tableWidget_displayPos->insertRow(InsertRow);
        QTableWidgetItem *itemIndex = new QTableWidgetItem();
        itemIndex->setText(QString("%1").arg(InsertRow + 1));
        itemIndex->setFlags(itemIndex->flags() & (~Qt::ItemIsEditable));//不可编辑
        itemIndex->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::IndexNum, itemIndex);
        ui->tableWidget_displayPos->item(InsertRow,TableIndexPos::IndexNum)->setBackground(QBrush(QColor(204,204,204)));//改变单元格颜色
        if(InsertRow == 0)
        {
            //设置X - 使用容器实现居中
            QWidget* containerX = new QWidget(this);
            QHBoxLayout* layoutX = new QHBoxLayout(containerX);
            layoutX->setContentsMargins(0, 0, 0, 0);
            layoutX->setAlignment(Qt::AlignCenter);
            QSpinBox *DisplayXvalue = new QSpinBox(containerX);
            DisplayXvalue->setStyleSheet(m_SpinboxsheetX);
            DisplayXvalue->setAlignment(Qt::AlignCenter);
            DisplayXvalue->setMaximum(5000);
            DisplayXvalue->setFixedHeight(30);
            DisplayXvalue->setFixedWidth(110);
            DisplayXvalue->setValue(iter.value().x());

            // 设置更大的字体
            QFont fontX = DisplayXvalue->font();
            fontX.setPixelSize(15);
            DisplayXvalue->setFont(fontX);

            // 设置+/-按钮样式
            DisplayXvalue->setButtonSymbols(QSpinBox::PlusMinus);
            layoutX->addWidget(DisplayXvalue);
            // 将SpinBox指针存储为容器属性
            containerX->setProperty("innerSpinBox", QVariant::fromValue<QSpinBox*>(DisplayXvalue));
            containerX->setLayout(layoutX);
            ui->tableWidget_displayPos->setCellWidget(InsertRow,TableIndexPos::Instrument_xpos,containerX);
            connect(DisplayXvalue, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int value)
            {
                DisplayXvalue->setEnabled(false);
                UpdateBaseValue_x(value, DisplayXvalue);
                DisplayXvalue->setEnabled(true);
            });
            //设置Y - 使用容器实现居中
            QWidget* containerY = new QWidget(this);
            QHBoxLayout* layoutY = new QHBoxLayout(containerY);
            layoutY->setContentsMargins(0, 0, 0, 0);
            layoutY->setAlignment(Qt::AlignCenter);
            QSpinBox *DisplayYvalue = new QSpinBox(containerY);
            DisplayYvalue->setMaximum(5000);
            DisplayYvalue->setFixedHeight(30);
            DisplayYvalue->setFixedWidth(110);
            DisplayYvalue->setAlignment(Qt::AlignCenter);
            DisplayYvalue->setStyleSheet(m_SpinboxsheetY);
            DisplayYvalue->setValue(iter.value().y());

            // 设置更大的字体
            QFont fontY = DisplayYvalue->font();
            fontY.setPixelSize(15);
            DisplayYvalue->setFont(fontY);

            // 设置+/-按钮样式
            DisplayYvalue->setButtonSymbols(QSpinBox::PlusMinus);
            layoutY->addWidget(DisplayYvalue);
            // 将SpinBox指针存储为容器属性
            containerY->setProperty("innerSpinBox", QVariant::fromValue<QSpinBox*>(DisplayYvalue));
            containerY->setLayout(layoutY);
            ui->tableWidget_displayPos->setCellWidget(InsertRow,TableIndexPos::Instrument_ypos, containerY);
            connect(DisplayYvalue, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
                [=](int value)
            {
                DisplayYvalue->setEnabled(false);
                UpdateBaseValue_y(value);
                DisplayYvalue->setEnabled(true);
            });
            //保存按钮
            AddBtnSave_backValue(InsertRow,true);
        }
        else
        {
            QTableWidgetItem *item_x = new QTableWidgetItem();
            item_x->setFont(mcustFont);
            item_x->setText(QString("%1").arg(iter.value().x()));
            item_x->setFlags(item_x->flags() & (~Qt::ItemIsEditable));//不可编辑
            item_x->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::Instrument_xpos,item_x);

            QTableWidgetItem *item_y = new QTableWidgetItem(QString("%1").arg(iter.value().y()));
            item_y->setFont(mcustFont);
            item_y->setFlags(item_y->flags() & (~Qt::ItemIsEditable));//不可编辑
            item_y->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::Instrument_ypos,item_y);

            QTableWidgetItem *item_btn = new QTableWidgetItem();
            item_btn->setText("");
            item_btn->setFlags(item_btn->flags() & (~Qt::ItemIsEditable));//不可编辑
            item_btn->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            ui->tableWidget_displayPos->setItem(InsertRow,TableIndexPos::Instrument_save,item_btn);
            ui->tableWidget_displayPos->item(InsertRow,TableIndexPos::Instrument_save)->setBackground(QBrush(QColor(205, 201, 201)));
        }
        iter++;
        InsertRow++;
    }
    update();
}


//保存修改的单个坐标
void CustomPlot::BtnClickSavePoint()
{
    QByteArrayList sendcommd;
    QByteArray modifyParaarry_x;
    QByteArray modifyParaarry_y;
    // 安全获取按钮
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    // 获取按钮在表格中的位置
    int x = btn->mapToParent(QPoint(0,0)).x();
    int y = btn->mapToParent(QPoint(0,0)).y();
    QModelIndex index = ui->tableWidget_displayPos->indexAt(QPoint(x,y));
    if (!index.isValid()) return;

    int row = index.row();

    // 从单元格控件中安全获取SpinBox
    auto getSpinBoxFromCell = [](QWidget* cellWidget) -> QSpinBox* {
        if (!cellWidget) return nullptr;

        // 如果本身就是QSpinBox，直接返回
        if (auto* spinBox = qobject_cast<QSpinBox*>(cellWidget)) {
            return spinBox;
        }

        // 否则尝试从属性中获取
        QVariant spinBoxVar = cellWidget->property("innerSpinBox");
        if (spinBoxVar.isValid() && spinBoxVar.canConvert<QSpinBox*>()) {
            return spinBoxVar.value<QSpinBox*>();
        }

        // 最后尝试查找子控件
        return cellWidget->findChild<QSpinBox*>();
    };

    QWidget* cellWidgetX = ui->tableWidget_displayPos->cellWidget(row, TableIndexPos::Instrument_xpos);
    QWidget* cellWidgetY = ui->tableWidget_displayPos->cellWidget(row, TableIndexPos::Instrument_ypos);

    QSpinBox* Point_x = getSpinBoxFromCell(cellWidgetX);
    QSpinBox* Point_y = getSpinBoxFromCell(cellWidgetY);

    if (!Point_x || !Point_y) return;

    int config_xpos = Point_x->value();
    int config_ypos = Point_y->value();

    switch(m_CalibrationArea)
    {

        case Origin_bloodNeedle:
                displayOriginAxisPoint(false,NOTIFY_XPOINT,config_xpos);
                displayOriginAxisPoint(false,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                QLOG_DEBUG()<<QString("修改原点坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case CleanLinque_bloodNeedle:
                displayCleanLinqueAxisPoint(false,MOTOR_BLOOD_INDEX,NOTIFY_XPOINT,config_xpos);
                displayCleanLinqueAxisPoint(false,MOTOR_BLOOD_INDEX,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                QLOG_DEBUG()<<QString("修改清洗液相对血样针坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case CleanLinque_ReagentNeedle:
                displayCleanLinqueAxisPoint(false,MOTOR_REAGNET_INDEX,NOTIFY_XPOINT,config_xpos);
                displayCleanLinqueAxisPoint(false,MOTOR_REAGNET_INDEX,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                QLOG_DEBUG()<<QString("修改清洗液相对试剂针坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case TestChannel_ReagentNeedle:
                displayTestChnAxisPoint(false,NOTIFY_XPOINT,MOTOR_REAGNET_INDEX,row,config_xpos);
                displayTestChnAxisPoint(false,NOTIFY_YPOINT,MOTOR_REAGNET_INDEX,row,config_ypos);
                writeboard(MOTOR_REAGNET_INDEX,row,sendcommd);
                QLOG_DEBUG()<<QString("修改测试通道%1offset试剂针坐标:[%2,%3]").arg(row+1).arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case TestChannel_Hands:
                displayTestChnAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,row,config_xpos);
                displayTestChnAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,row,config_ypos);
                writeboard(MOTOR_HANDS_INDEX,row,sendcommd);
                QLOG_DEBUG()<<QString("修改测试通道%1offset抓手坐标:[%2,%3]").arg(row+1).arg(config_xpos).arg(config_ypos)<<endl;
        break;
        case ThrowCup_Hands:
                displayThrowHole(false,NOTIFY_XPOINT,config_xpos);
                displayThrowHole(false,NOTIFY_YPOINT,config_ypos);
                sendcommd.clear();
                QLOG_DEBUG()<<QString("修改弃杯孔坐标:[%1,%2]").arg(config_xpos).arg(config_ypos)<<endl;
                QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        default:
            break;
    }
    emit writdAxisata(sendcommd,"缓存坐标指令");
    //btn->setText("已保存");
    return;
}

void  CustomPlot::UpdateBaseValue_x(int NotifyValue_x,QSpinBox *pbox)
{
    pbox->setEnabled(false);
    int offsetValue = 0;
    switch(m_CalibrationArea)
    {
        case ReagentLinque_ReagentNeedle:   offsetValue = displayReagentAxisPoint(false,NOTIFY_XPOINT,NotifyValue_x);                             break;
        case EmptyTube_1_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,1,0,NotifyValue_x);     break;
        case EmptyTube_1_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,1,0,NotifyValue_x);     break;
        case EmptyTube_2_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,2,60,NotifyValue_x);    break;
        case EmptyTube_2_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,2,60,NotifyValue_x);    break;
        case EmptyTube_3_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,3,120,NotifyValue_x);   break;
        case EmptyTube_3_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,3,120,NotifyValue_x);   break;
        case EmptyTube_4_Hands:             offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_HANDS_INDEX,4,180,NotifyValue_x);   break;
        case EmptyTube_4_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_XPOINT,MOTOR_BLOOD_INDEX,4,180,NotifyValue_x);   break;
        case Bloodsample_BloodNeedle:       offsetValue = displayBloodHoleAxisPoint(false,NOTIFY_XPOINT,0,NotifyValue_x);                         break;
        default:
            break;
    }
    updateNotifyOffsetValue(NOTIFY_XPOINT,offsetValue);
    pbox->setEnabled(true);

}
void  CustomPlot::UpdateBaseValue_y(int NotifyValue_y)
{
    int offsetValue = 0;
    switch(m_CalibrationArea)
    {
        case ReagentLinque_ReagentNeedle:  offsetValue = displayReagentAxisPoint(false,NOTIFY_YPOINT,NotifyValue_y);                             break;
        case EmptyTube_1_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,1,0,NotifyValue_y);     break;
        case EmptyTube_1_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,1,0,NotifyValue_y);     break;
        case EmptyTube_2_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,2,60,NotifyValue_y);    break;
        case EmptyTube_2_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,2,60,NotifyValue_y);    break;
        case EmptyTube_3_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,3,120,NotifyValue_y);   break;
        case EmptyTube_3_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,3,120,NotifyValue_y);   break;
        case EmptyTube_4_Hands:            offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_HANDS_INDEX,4,180,NotifyValue_y);   break;
        case EmptyTube_4_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false,NOTIFY_YPOINT,MOTOR_BLOOD_INDEX,4,180,NotifyValue_y);   break;
        case Bloodsample_BloodNeedle:      offsetValue = displayBloodHoleAxisPoint(false,NOTIFY_YPOINT,0,NotifyValue_y);                         break;
        default:
            break;
    }
    updateNotifyOffsetValue(NOTIFY_YPOINT,offsetValue);
}

void CustomPlot::updateNotifyOffsetValue(bool bupdate_x, const int offsetValue)
{
    int rowscount = ui->tableWidget_displayPos->rowCount();
    const int xposCol = TableIndexPos::Instrument_xpos;
    const int yposCol = TableIndexPos::Instrument_ypos;

    for(int i = 0; i < rowscount; i++)
    {
        if(bupdate_x == NOTIFY_XPOINT)
        {
            // 安全获取表格项
            QTableWidgetItem* xItem = ui->tableWidget_displayPos->item(i, xposCol);
            if(xItem == nullptr || xItem->text().isEmpty())
            {
                continue;
            }
            // 使用局部变量避免重复查找
            int tablevalue = xItem->text().toInt();
            xItem->setText(QString("%1").arg(tablevalue + offsetValue));
        }
        else
        {
            // 安全获取表格项
            QTableWidgetItem* yItem = ui->tableWidget_displayPos->item(i, yposCol);
            if(yItem == nullptr || yItem->text().isEmpty())
            {
                continue;
            }
            // 使用局部变量避免重复查找
            int tablevalue = yItem->text().toInt();
            yItem->setText(QString("%1").arg(tablevalue + offsetValue));
        }
    }
    update();
    return;
}

void CustomPlot::writeboard(quint8 intdexZ,quint8 row, QByteArrayList &sendcommd)
{
    QByteArray x_arry;
    QByteArray y_arry;
    sendcommd.clear();
    if(intdexZ == MOTOR_REAGNET_INDEX)
    {
        if(row >= 0 && row < 5)
        {
             QUIUtils::get89NumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else if(row >= 5 && row < 10)
        {
            QUIUtils::get0a0bNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else
        {
            QUIUtils::get0c0dNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
    }
    else if(intdexZ == MOTOR_HANDS_INDEX)
    {
        if(row >= 0 && row < 3)
        {
             QUIUtils::get0c0dNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else if(row >= 3 && row <= 7 )
        {
             QUIUtils::get0e0fNumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
        else
        {
           QUIUtils::get1011NumberCodeArry(x_arry,AXISPOS_BUFFER,y_arry,AXISPOS_BUFFER);
        }
    }
    sendcommd.push_back(x_arry);
    sendcommd.push_back(y_arry);
}




//保存一个整体已偏移此刻发送命令就行
void CustomPlot::BtnClickOthersSavePoint()
{
    QByteArrayList sendcommd;
    QByteArray modifyParaarry_x;
    QByteArray modifyParaarry_y;
    switch(m_CalibrationArea)
    {
        case ReagentLinque_ReagentNeedle:
                    sendcommd.clear();
                    QUIUtils::get6_7NumcodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                    sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        case EmptyTube_1_Hands:
        case EmptyTube_1_BloodNeedle:
        case EmptyTube_2_Hands:
        case EmptyTube_2_BloodNeedle:
        case EmptyTube_3_Hands:
        case EmptyTube_3_BloodNeedle:
        case EmptyTube_4_Hands:
        case EmptyTube_4_BloodNeedle:
                     sendcommd.clear();
                     QUIUtils::get1213NumberCodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                     sendcommd<<modifyParaarry_x<<modifyParaarry_y;
                     QUIUtils::get1415NumberCodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                     sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        case Bloodsample_BloodNeedle:
                    sendcommd.clear();
                    QUIUtils::get1011NumberCodeArry(modifyParaarry_x,AXISPOS_BUFFER,modifyParaarry_y,AXISPOS_BUFFER);
                    sendcommd<<modifyParaarry_x<<modifyParaarry_y;
        break;
        default:
            break;
    }
    emit writdAxisata(sendcommd,"缓存坐标指令");
    return;
}



//清空列表内容
void CustomPlot::DelTableText()
{
    ui->tableWidget_displayPos->setRowCount(0);
    ui->label_movedPos->setText("校验位置: ");
}

//复位
void CustomPlot::on_toolButton_Backorigin_clicked()
{
    emit Resetmaneuver();
    auto *pconfAxis = SingletonAxis::GetInstance();
    QPoint locAxis(0,0);
    pconfAxis->originPos(READ_OPERRAT,locAxis);
    ui->label_movedPos->setText(QString("复位位置: X[%2]  Y[%3]").arg(locAxis.x()).arg(locAxis.y()));
    return;
}




void CustomPlot::ExitCoordinateSaving()
{
    quint8 indexHole = 60;
    QPoint savingAxis(0,0);
    QMap<quint8,quint16> ExitSaveCoordin;
    QByteArrayList send_data_;
    ExitSaveCoordin.clear();
    send_data_.clear();
    for(int n = 0; n < 3; n++){
        int indexTray = n + 1;
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,indexHole*indexTray,MOTOR_HANDS_INDEX,savingAxis);
        ExitSaveCoordin.insert(n,savingAxis.y());
    }
    ExitSaveCoordin.insert(3,0);
    ExitSaveCoordin.insert(4,0);

    QByteArray writeinboard = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_HANDSY,AXISPOS_WRITE,ExitSaveCoordin);
    send_data_.push_back(writeinboard);

    QLOG_DEBUG()<<"退出坐标调试保存坐标尾端"<< writeinboard.toHex(' ').trimmed().toUpper()<<"leng="<<writeinboard.size()<<endl;
    emit writdAxisata(send_data_,"保存坐标微调界面坐标");
    return;
}

bool CustomPlot::QuerytoModifyCoordinates(QMap<int,QPoint> PointGather,int FindIndexKey,QPoint &FindedCoordinates)
{
    bool bContainsKey = false;
    bContainsKey = PointGather.contains(FindIndexKey);
    if(!bContainsKey) return false;
    auto iter = PointGather.find(FindIndexKey);
    FindedCoordinates = iter.value();
    return bContainsKey;
}


void CustomPlot::SelectHoleChangebgm(bool changeColor, int selectedIndex, int indexTray)
{
    if (selectedIndex <= 0) return;

    auto updateItem = [this, changeColor](int row, int column) {
        QTableWidgetItem* item = ui->tableWidget_displayPos->item(row, column);
        if (item && !item->text().isEmpty()) {
            item->setBackground(changeColor ? QColor(255, 0, 0) : QColor(255, 255, 255));
        }
    };

    updateItem(selectedIndex, TableIndexPos::Instrument_xpos);
    updateItem(selectedIndex, TableIndexPos::Instrument_ypos);

    if (changeColor) {
        ui->tableWidget_displayPos->selectRow(selectedIndex);
        m_selectRows = selectedIndex + indexTray * 60;
    }
}

void CustomPlot::ChangeControlColors(bool changeColor,int selectedIndex)
{
    static const QColor SELECTED_COLOR(255, 0, 0);
    static const QColor DEFAULT_COLOR(204, 204, 204);

    QTableWidgetItem* item = ui->tableWidget_displayPos->item(selectedIndex, TableIndexPos::IndexNum);
    if (!item || item->text().isEmpty()) {
        return;
    }

    if (changeColor) {
        item->setBackground(SELECTED_COLOR);
        m_selectRows = selectedIndex;
    } else {
        item->setBackground(DEFAULT_COLOR);
    }
}

//血样区BtnClick
void  CustomPlot::ClickBloodTube()
{
    if(m_CalibrationArea != Bloodsample_BloodNeedle)
    {
        QMessageBox::about(this,"血样区","请选择匹配的校准区域!");
        return;
    }
    //SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
    QObject *object = QObject::sender();
    QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
    QString object_name = pushbtn->text();
    int index = object_name.toInt() - 1;
    if(m_selectRows != index && m_selectRows > 0)
    {
        SelectHoleChangebgm(false,m_selectRows,0);
    }
    mcodeNum = 0;
    mbtnClickPos.setX(0);
    mbtnClickPos.setY(0);
    SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT,index,mbtnClickPos);

    SelectHoleChangebgm(true,index,0);
    QByteArrayList modifyarry = Testing:: m_TaskDll->XYLocation(mbtnClickPos,MOTOR_BLOOD_INDEX,OFFSET_BLOODAREA,m_downhigh,mcodeNum,m_downhigh);
    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry);
    ui->label_movedPos->setText(QString("校验位置: X[%2]  Y[%3]").arg(mbtnClickPos.x()).arg(mbtnClickPos.y()));
    return;
}

//Click 试管区
void CustomPlot::ClickEmptyTube()
{
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (!button) return;

    bool ok = false;
    int index = button->text().toInt(&ok) - 1;
    if (!ok || index < 0) return;

    quint8 trayId = (index / 60) + 1;
    QPoint targetPoint;
    int needleIndex = 0;
    QString statusText;
    QString operationType;

    // 定义校准区域类型
    bool isBloodNeedleArea = (m_CalibrationArea >= EmptyTube_1_BloodNeedle &&
        m_CalibrationArea <= EmptyTube_4_BloodNeedle);
    bool isHandsArea = (m_CalibrationArea >= EmptyTube_1_Hands &&
        m_CalibrationArea <= EmptyTube_4_Hands);

    if (!isBloodNeedleArea && !isHandsArea) {
        QMessageBox::about(this, "试管区", "请选择匹配的校准区域!");
        return;
    }

    // 确定针类型和操作类型
    if (isBloodNeedleArea) {
        needleIndex = MOTOR_BLOOD_INDEX;
        operationType = "血样针";
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, index, needleIndex, targetPoint);
    }
    else {
        needleIndex = MOTOR_HANDS_INDEX;
        operationType = "抓手";
        SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, index, needleIndex, targetPoint);
    }

    // 根据枚举值计算期望的托盘ID
    int expectedTrayId = 0;
    if (isBloodNeedleArea) {
        expectedTrayId = m_CalibrationArea - EmptyTube_1_BloodNeedle + 1;
    }
    else {
        expectedTrayId = m_CalibrationArea - EmptyTube_1_Hands + 1;
    }

    // 验证托盘ID匹配
    if (trayId != expectedTrayId) {
        QMessageBox::about(this, "试管区", "请选择匹配的校准区域!");
        return;
    }

    // 生成状态文本
    statusText = QString("试杯区%1(%2)校验位置: [%3,%4]")
        .arg(trayId)
        .arg(operationType)
        .arg(targetPoint.x())
        .arg(targetPoint.y());

    // 更新选择的行
    if (m_selectRows != index && m_selectRows > 0) {
        SelectHoleChangebgm(false, m_selectRows - 60 * (trayId - 1), trayId - 1);
    }

    // 生成运动指令
    mcodeNum = 0;
    QByteArrayList modifyArray = Testing::m_TaskDll->XYLocation(targetPoint, needleIndex,
        1, m_downhigh, mcodeNum, m_downhigh);

    // 更新UI并发送信号
    ui->label_movedPos->setText(statusText);
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);
}

void CustomPlot::ReminderTable(int TableRow)
{
    static const QColor SELECTED_COLOR(220, 44, 44);
    static const QColor DEFAULT_COLOR(204, 204, 204);

    const int rowCount = ui->tableWidget_displayPos->rowCount();

    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem* item = ui->tableWidget_displayPos->item(i, TableIndexPos::IndexNum);
        if (!item) continue;

        bool ok = false;
        const int rowNum = item->text().toInt(&ok) - 1;
        if (!ok) continue;

        item->setBackground(TableRow == rowNum ? SELECTED_COLOR : DEFAULT_COLOR);
    }
}

//点击测试通道
void  CustomPlot::ClickTestChannelTube()
{
    QToolButton* button = qobject_cast<QToolButton*>(QObject::sender());
    if (!button) return;

    bool ok = false;
    int index = button->text().toInt(&ok) - 1;
    if (!ok || index < 0) return;

    if (m_selectRows != index) {
        ChangeControlColors(false, m_selectRows);
    }

    quint8 codeNum = 0;
    QPoint targetPoint;
    int motorIndex = MOTOR_HANDS_INDEX;
    QByteArrayList modifyArray;

    if (m_CalibrationArea == TestChannel_ReagentNeedle || m_CalibrationArea == TestChannel_Hands) {
        motorIndex = (m_CalibrationArea == TestChannel_ReagentNeedle) ? MOTOR_REAGNET_INDEX : MOTOR_HANDS_INDEX;
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, index, motorIndex, targetPoint);
        ChangeControlColors(true, index);
    } else {
        SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, index, MOTOR_HANDS_INDEX, targetPoint);
        motorIndex = MOTOR_HANDS_INDEX;
    }

    modifyArray = Testing::m_TaskDll->XYLocation(targetPoint, motorIndex, 0, m_downhigh, codeNum, m_downhigh);

    ui->label_movedPos->setText(QString("测试通道校验位置:[%1,%2]").arg(targetPoint.x()).arg(targetPoint.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);



//    quint8  codenum = 0%255;
//    QObject *object = QObject::sender();
//    QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
//    QString object_name = pushbtn->text();
//    int index = object_name.toInt() - 1;
//    if(m_selectRows != index)
//        ChangeControlColors(false,m_selectRows);

//    QByteArrayList modifyarry;
//    modifyarry.clear();

//    QPoint getOutPoint(0,0);
//    if(m_CalibrationArea == TestChannel_ReagentNeedle || m_CalibrationArea == TestChannel_Hands)
//    {
//        if(m_CalibrationArea == TestChannel_ReagentNeedle)
//        {
//            SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,index,MOTOR_REAGNET_INDEX,getOutPoint);
//            modifyarry = Testing:: m_TaskDll->XYLocation(getOutPoint, MOTOR_REAGNET_INDEX, 0, m_downhigh, codenum, m_downhigh);
//        }
//        else if(m_CalibrationArea == TestChannel_Hands)
//        {
//            SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,index,MOTOR_HANDS_INDEX,getOutPoint);
//            modifyarry =  Testing:: m_TaskDll->XYLocation(getOutPoint, MOTOR_HANDS_INDEX, 0, m_downhigh, codenum, m_downhigh);
//        }
//        ChangeControlColors(true,index);
//    }
//    else
//    {
//         SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT,index,MOTOR_HANDS_INDEX,getOutPoint);
//         modifyarry =  Testing:: m_TaskDll->XYLocation(getOutPoint, MOTOR_HANDS_INDEX, 0, m_downhigh, codenum, m_downhigh);
//    }
//    ui->label_movedPos->setText(QString("测试通道校验位置:[%1,%2]").arg(getOutPoint.x()).arg(getOutPoint.y()));
//    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry);
//    return;
}

//点击试剂区
void  CustomPlot::ClickReagentsTube()
{
    if(m_CalibrationArea != ReagentLinque_ReagentNeedle)
    {
        QMessageBox::about(this,"试剂区","请选择匹配的校准区域!");
        return;
    }
    quint8  codenum = 0;
    QByteArrayList  Notifyarry;
    Notifyarry.clear();
    QObject *object = QObject::sender();
    QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
    QStringList object_name = pushbtn->objectName().split("_");
    int index = object_name.last().toInt() - 20; //20起
    if(m_selectRows != index && m_selectRows > 0)
    {
        SelectHoleChangebgm(false,m_selectRows,0);
    }


    QPoint getOutPoint(0,0);
    SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT,index,getOutPoint);
    QLOG_DEBUG()<<"click试剂区"<<index<<"坐标"<<getOutPoint<<endl;

    SelectHoleChangebgm(true,index,0);
    QByteArrayList modifyarry = Testing::m_TaskDll->XYLocation(getOutPoint,MOTOR_REAGNET_INDEX, 0, m_downhigh,codenum, m_downhigh);
    ui->label_movedPos->setText(QString("试剂区校验位置:[%1,%2]").arg(getOutPoint.x()).arg(getOutPoint.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry); //试剂区
    return;
}


void CustomPlot::_sendmoveActiveToEquipment(AreasCalibration indexZone, QPoint MovingAxis,quint8 indexZ)
{
    QString outputText ="";
    quint8 codenum = 0;
    QByteArrayList modifyarry;
    modifyarry.clear();

    switch(indexZone)
    {
        case origin:
            outputText = "原点校验位置:";
            QUIUtils::_Locatetheorigin(modifyarry, MovingAxis ,m_downhigh);
        break;
        case detergent:
            outputText = "清洗剂校验位置:";
            modifyarry = Testing::m_TaskDll->XYLocation(MovingAxis, indexZ, 0, m_downhigh, codenum, m_downhigh);

        break;
    }
    ui->label_movedPos->setText(QString("%1[%2,%3]").arg(outputText).arg(MovingAxis.x()).arg(MovingAxis.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST,modifyarry);
    return;
}


//点击原点
void CustomPlot::userClickOriginAxis()
{
    if(m_CalibrationArea != Origin_bloodNeedle)
    {
        QMessageBox::about(this,"清洗区原点","请选择匹配的校准区域!");
        return;
    }
    QPoint getOutPoint(0,0);
    SingletonAxis::GetInstance()->originPos(READ_OPERRAT,getOutPoint);

    ChangeControlColors(true,0);

    _sendmoveActiveToEquipment(origin,getOutPoint,0);
    return;
}

//点击清洗孔
void CustomPlot::on_toolButton_Clean_1_clicked()
{
    if(CleanLinque_bloodNeedle != m_CalibrationArea && CleanLinque_ReagentNeedle != m_CalibrationArea)
    {
        QMessageBox::about(this,"清洗液区","请选择匹配的校准区域!");
        return;
    }
    quint8  indexZ = 0;
    QPoint  getOutPoint(0,0);
    if(CleanLinque_bloodNeedle == m_CalibrationArea)
    {
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_BLOOD_INDEX,getOutPoint);
        indexZ = MOTOR_BLOOD_INDEX;
    }
    else
    {
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT,MOTOR_REAGNET_INDEX,getOutPoint);
        indexZ = MOTOR_REAGNET_INDEX;
    }

    ChangeControlColors(true,0);

    _sendmoveActiveToEquipment(detergent,getOutPoint,indexZ);

    return;

}

//点击弃杯孔
void CustomPlot::on_toolButton_throwTube_clicked()
{
    quint8 codeNum = 0;
    QPoint throwPoint;
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT, throwPoint);

    QByteArrayList modifyArray;
    QString statusText;

    if (ThrowCup_Hands != m_CalibrationArea) {
        int throwCupsDown = INI_File()._gethandsdownthrowcpus();
        modifyArray = Testing::m_TaskDll->AdjustModeThrowTube(throwPoint, MOTOR_HANDS_INDEX, 0, throwCupsDown, codeNum);
        statusText = QString("弃杯:[%1,%2]").arg(throwPoint.x()).arg(throwPoint.y());
    } else {
        modifyArray = Testing::m_TaskDll->XYLocation(throwPoint, MOTOR_HANDS_INDEX, 0, m_downhigh, codeNum, m_downhigh);
        ChangeControlColors(true, 0);
        statusText = QString("弃杯校验位置:[%1,%2]").arg(throwPoint.x()).arg(throwPoint.y());
    }

    ui->label_movedPos->setText(statusText);
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);
}


void CustomPlot::Recv_CalibrationMoved()
{
    QString text = ui->label_movedPos->text();
    if (text.indexOf("完成") == -1) {
        ui->label_movedPos->setText(text + " [完成]");
    }
}





void Generate_random_numbers(QVector<int> & Randomnum, int spacevalue)
{
    // 使用现代C++随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());

    // 创建0到59的序列
    QVector<int> numbers(60);
    std::iota(numbers.begin(), numbers.end(), 0);

    // 打乱序列
    std::shuffle(numbers.begin(), numbers.end(), gen);

    // 添加偏移量
    Randomnum.clear();
    Randomnum.reserve(numbers.size());
    for (int num : numbers) {
        Randomnum.append(num + spacevalue);
    }
}


void CustomPlot::recv_scynTestCommand(int index, const QByteArrayList commdArry)
{
     switch(index)
     {
         case 0: QLOG_DEBUG()<<"测试吸试剂"; break;
         case 1: QLOG_DEBUG()<<"测试吸PPP"; break;
     }
     emit this->SportActive(COORDINATE_FINE_TUNING_TEST,commdArry);
     QThread::sleep(1);
     emit Resetmaneuver(); //调试界面机器复位
     return;
}

/*测试抓手*/
void CustomPlot::on_pushButton_TrayHands_clicked()
{
    if(mpTestCaseRun == nullptr)
    {
        mpTestCaseRun = new TubeContinueDoing;
        connect(mpTestCaseRun,&TubeContinueDoing::scynTestCommand,this,&CustomPlot::recv_scynTestCommand);
        connect(mpTestCaseRun,&TubeContinueDoing::sendcode,this,&CustomPlot::slotsendcode);
    }
    mpTestCaseRun->show();
    return;
}
void CustomPlot::slotsendcode(QByteArrayList data_)
{
     emit writdAxisata(data_,"吸吐");
     //QLOG_DEBUG()<<"=============";
}

void CustomPlot::on_spinBox_downValue_valueChanged(int arg1)
{
    m_downhigh = arg1;
    update();
}

void CustomPlot::on_pushButton_TrayHands_2_clicked()
{
    if(m_indexDown < 0 ){
        QMessageBox::about(this,tr("测试失败"),tr("请配试抓手下降高度!"));
        return;
    }
    mfromtray = ui->spinBox_from_tray->value() - 1;
    mendtray =  ui->spinBox_endtray->value() - 1;
    if(mfromtray != mendtray){
        mfromHole.clear();
        mendHole.clear();
        mtest_catch_put_commad.clear();
        for(int i = 0; i < total_tube_num ; i++){
            mfromHole.push_back(mfromtray*total_tube_num + i);
            mendHole.push_back(mendtray*total_tube_num + i);
        }
        ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::BLINK);
        //打乱抓取试管盘的试管号(from)
        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        std::shuffle(mfromHole.begin(),mfromHole.end(),std::default_random_engine(seed));
        //从小到大放置孔号
        std::sort(mendHole.begin(),mendHole.end());
        //输出抓取孔号放到托盘孔号
        int index_ = 0;
        auto it = mfromHole.constBegin();
        while(it != mfromHole.constEnd()){
            int hole_catch = *it;
            // 边界检查，防止容器大小不匹配导致的越界
            if (index_ >= mendHole.size()) {
                QLOG_WARN() << "索引越界: index_" << index_ << "mendHole大小:" << mendHole.size();
                break;
            }
            int hole_put = mendHole.at(index_);
            test_catch_putdown_cup(hole_catch,hole_put,mtest_catch_put_commad);
            index_++;
            it++;
        }
        send_test_cups_accurate();
    }else{
        QMessageBox::about(this,tr("测试失败"),tr("相同试管盘请勿移动!"));
    }
    return;
}

void CustomPlot::test_catch_putdown_cup(const int form_,const int end_,QMap<int,QByteArrayList> &group_directives)
{
    QByteArrayList once_directives;
    QPoint from_tube_axis(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,form_,MOTOR_HANDS_INDEX,from_tube_axis);
    QPoint end_tube_axis(0,0);
    SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT,end_,MOTOR_HANDS_INDEX,end_tube_axis);
    QUIUtils::test_hands_catchtube_precise(from_tube_axis,end_tube_axis,m_downhigh,once_directives);
    group_directives.insert(end_,once_directives);
    return;
}

void CustomPlot::send_test_cups_accurate()
{
    foreach(int insex_ ,mendHole)
    {
        if(mtest_catch_put_commad.contains(insex_))
        {
             QMap<int, QByteArrayList>::iterator iter = mtest_catch_put_commad.find(insex_);
             int key = iter.key();
             QByteArrayList value = iter.value();
             //测试抓被准确性
             emit this->TrayMoveTest(value,true,value.size());
             mtest_catch_put_commad.remove(key);
        }
    }
    if(mtest_catch_put_commad.isEmpty()){
        ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::OFF);
        emit Resetmaneuver();
    }
    return;
}


//移动到摄像头区域下样本针
void CustomPlot::on_toolButton_video_clicked()
{
    static const QPoint locVideo(190, 3140);
    quint8 codeNum = 0;

    auto modifyArray = Testing::m_TaskDll->XYLocation(locVideo, IndexZ::Blood_z, 2, m_downhigh, codeNum, m_downhigh);

    ui->label_movedPos->setText(QString("信息录入坐标:[%1,%2]").arg(locVideo.x()).arg(locVideo.y()));
    emit SportActive(COORDINATE_FINE_TUNING_TEST, modifyArray);
}



void CustomPlot::closeEvent(QCloseEvent* event)
{
    //使用更清晰的对话框标题和文本
    QString title = tr("确认关闭");
    QString message = tr("确定要关闭窗口吗？");

   //如果串口正忙，给出额外警告
   if (cglobal::gserialConnecStatus && cglobal::g_StartTesting) {
       message = tr("串口正在测试中，关闭可能导致数据丢失。\n确定要继续吗？");
   }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, title, message,
                                     QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
           if (cglobal::gserialConnecStatus) {
               QLOG_DEBUG() << "关闭校准坐标，重置操作并保存坐标...";

               //发射信号前检查连接状态
               if (receivers(SIGNAL(Resetmaneuver())) > 0) {
                   emit Resetmaneuver();
               }

               //确保保存操作完成（如果可能是异步的，需要等待）
               ExitCoordinateSaving();

               //可选 - 等待操作完成
               //QCoreApplication::processEvents();
           }

           VerifyCoordinates::GetInstance()->startComparingCoordinates(false);

           event->accept();  // ✅ 接受关闭事件

   } else {
       event->ignore();  // ✅ 忽略关闭事件
   }
}
