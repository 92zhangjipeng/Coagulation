// 在文件最开头添加宏定义
#define ACCESS_MASK ACCESS_MASK

#include "height_data.h"
#include "ui_height_data.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QFile>
#include <QMenu>
#include <algorithm>
#include "crc/include/vld.h"
#include <QCoreApplication>
#include "globaldata.h"
#include "loginui.h"
#include "testing.h"
#include <operclass/fullyautomatedplatelets.h>
#include <custom_style/custommessagebox.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

// 全血模式行背景色（与 addSampleIdToRow 中保持一致）
#define WHOLEBLOOD_BG_COLOR   QColor(160, 82, 45)// 棕色
// 血浆模式行背景色
#define PLASMA_BG_COLOR       QColor(135, 206, 250)// 浅蓝色

// 医疗淡蓝色主题色系常量
namespace MedicalTableColors {
    const QString BG_LIGHT = "#F0F7FC";      // 窗口背景色
    const QString TABLE_BG = "#FFFFFF";       // 表格背景色
    const QString HEADER_BG = "#E8F4FD";      // 表头背景色
    const QString HEADER_TEXT = "#2C5F8A";    // 表头文字色
    const QString ROW_HOVER = "#E8F4FD";      // 行悬停色
    const QString ROW_SELECTED = "#7EC8F0";   // 行选中色
    const QString ROW_SELECTED_HOVER = "#5BB8E8"; // 选中行悬停色
    const QString GRID_COLOR = "#D4EAF5";     // 网格线色
    const QString BORDER_FOCUS = "#5BB8E8";   // 聚焦边框色
}

Height_Data::Height_Data(QWidget *parent) : QWidget(parent),
    ui(new Ui::Height_Data)
    ,m_Barcodestr("")
    ,m_repTestOpencvId("")
    ,m_isreplaceopencv(false)
    ,m_titleBar(nullptr)
{
    ui->setupUi(this);
    this->installEventFilter(this);
    // 确保控件可以获得焦点
    this->setFocusPolicy(Qt::StrongFocus);
    this->setFocus();


    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    // 设置窗口背景色
    setStyleSheet(QString("QWidget { background-color: %1; }").arg(MedicalTableColors::BG_LIGHT));

    // 初始化自定义标题栏
    initCustomTitleBar();

    setWindowTitle(tr("任务列表"));
    this->setMinimumWidth(800);
    this->setMinimumHeight(450);
    ui->widget_ShowErrImage->show();
    m_selbloodholetemp.clear();

    //测高模式
    mtestmodebox = new QButtonGroup(this);
    mtestmodebox->setExclusive(true); //设置是否互斥
    mtestmodebox->addButton(ui->checkBox_wholeblood,0);
    mtestmodebox->addButton(ui->checkBox_plasma,1);
    bool wholeblood = INI_File().GetWholeBloodModel();
    (wholeblood == true)? ui->checkBox_wholeblood->setChecked(true): ui->checkBox_plasma->setChecked(true);
    connect(mtestmodebox,SIGNAL(buttonClicked(int)),this,SLOT(clickBloodmode(int)));

    m_sampleCreateTime.clear();

    InitTablewidget();

    initLoadOpencvTestImag();

    ui->Sample_Data_tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);

    GlobalData::QCheckboxSheet(ui->checkBox_wholeblood,tr("全血模式"));
    GlobalData::QCheckboxSheet(ui->checkBox_plasma,tr("血浆模式"));

    // 急诊优先：与模式勾选同区，开测时 SortemergencyattheTop 置顶
    if (ui->checkBox_plasma && ui->checkBox_plasma->parentWidget()) {
        m_checkBoxEmergency = new QCheckBox(tr("急诊优先"), ui->checkBox_plasma->parentWidget());
        m_checkBoxEmergency->setMinimumHeight(30);
        if (auto *layout = qobject_cast<QVBoxLayout*>(ui->checkBox_plasma->parentWidget()->layout())) {
            layout->addWidget(m_checkBoxEmergency);
        }
        GlobalData::QCheckboxSheet(m_checkBoxEmergency, tr("急诊优先"));
        m_checkBoxEmergency->setToolTip(tr(
            "勾选：把当前选中行标为急诊（没有选中则标全部行）。\n"
            "取消勾选：按同样范围取消急诊。\n"
            "开测时急诊样本插到待吸队列最前；测试中再加急诊也会插队。"));
        connect(m_checkBoxEmergency, &QCheckBox::toggled, this, [this](bool on) {
            applyEmergencyToSelectedOrAll(on);
        });
    }

    QString toolBtnQss;
    QFile styleFileToolBtn(":/Picture/SetPng/wholeToolButton.qss");
    if(styleFileToolBtn.open(QIODevice::ReadOnly)) {
          toolBtnQss = QLatin1String(styleFileToolBtn.readAll());
          styleFileToolBtn.close();
    }
    QHash<QToolButton*, QString> toolButtonList = {
         {ui->toolButton_Delete_sel_item,    tr("删除选中")},
         {ui->AddTask, tr("添加任务")},
         {ui->toolButton_ok, tr("确定")},
         {ui->toolButton_Cancel, tr("取消")},
         {ui->toolButtonbatchAddTask,tr("批量添加")}
    };
    for (auto it = toolButtonList.cbegin(); it != toolButtonList.cend(); ++it) {
         QToolButton* toolbutton = it.key();
         QString text = it.value();
         toolbutton->setStyleSheet(toolBtnQss);
         toolbutton->setText(text);
    }

}

Height_Data::~Height_Data()
{
    QLOG_DEBUG()<<__FUNCTION__<<__LINE__<<"析构加样本对象"<<endl;

    cleanupThread();

    delete m_threadaddsample;
    m_threadaddsample = nullptr;

    delete ui;
}


void Height_Data::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QTableWidget *table = ui ? ui->Sample_Data_tablewidget : nullptr;
    if (!table) {
        return;
    }
    for (int row = 0; row < table->rowCount(); ++row) {
        QWidget *widget = table->cellWidget(row, RICHBLOOD_HOLE);
        auto *box = qobject_cast<QComboBox *>(widget);
        if (!box) {
            continue;
        }
        const int hole = box->currentText().toInt();
        if (hole > 0) {
            Q_EMIT reminderPPPandPRPputHole(static_cast<quint8>(hole));
        }
    }
}



void Height_Data::clickBloodmode(int clickid)
{
   auto &ini = INI_File();
   QWidget* pshowImage = ui->widget_ShowErrImage;
   QCheckBox *btn = qobject_cast<QCheckBox*> (mtestmodebox->button(clickid));
   btn->setChecked(true);

   if (clickid == 0) {  // 全血模式
       ini.SetWholeBloodModel(true);
       pshowImage->show();

       // 关键修复：强制设置图片区域固定宽度，防止被压缩
       pshowImage->setFixedWidth(360);
       pshowImage->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

       // 计算所需最小窗口宽度（表格最小宽度 + 图片宽度 + 布局间距和边距）
       int tableMinWidth = ui->Sample_Data_tablewidget->minimumSize().width(); // 应为1050
       int imageWidth = pshowImage->width(); // 固定360
       int spacing = ui->horizontalLayout_3->spacing();
       int margins = ui->horizontalLayout_3->contentsMargins().left() +
                     ui->horizontalLayout_3->contentsMargins().right();
       int totalMinWidth = tableMinWidth + imageWidth + spacing + margins;

       setMinimumWidth(totalMinWidth);
       if (width() < totalMinWidth) {
           resize(totalMinWidth, height());
       }

       // 强制刷新布局
       pshowImage->raise();
       ui->horizontalLayout_3->invalidate();
       ui->horizontalLayout_3->activate();

   } else {  // 血浆模式
       ini.SetWholeBloodModel(false);
       pshowImage->hide();

       // 恢复图片控件尺寸策略（避免影响后续显示）
       pshowImage->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

       // 恢复窗口最小宽度为表格所需宽度
       int tableMinWidth = ui->Sample_Data_tablewidget->minimumSize().width();
       setMinimumWidth(tableMinWidth);
       if (width() < tableMinWidth) {
           resize(tableMinWidth, height());
       }
   }
   ui->widget_Task->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
   adjustTableColumnWidths();
   updateTableHeaderText();

   this->adjustSize();
   this->update();
   emit requestAltimeterCamera(clickid == 0);
}

void Height_Data::initcreat()
{
    if (m_actionsBound) {
        return;
    }
    m_actionsBound = true;

    initNumAnaemiaHole();

    FullyAutomatedPlatelets::pinstanceTestproject()->initstyle();

    QObject::connect(FullyAutomatedPlatelets::pinstanceTestproject(),
                     &ConfigureProjectItem::setallsampletestproject,this,[=](QString text){
        QList<int> item_List;
        obtainSelectedSample(item_List);
        for(int i = 0 ; i< item_List.size() ;i++){
            int rows = item_List.at(i);
            SetColumnText(rows,PROJECT_ITEM,text);
        }
    });

    QObject::connect(FullyAutomatedPlatelets::pinstanceTestproject(),&ConfigureProjectItem::setsinglesampleproject,
                     this,[=](int rows,int clos,QString itemtext){
       SetColumnText(rows,clos,itemtext);
    });

    QObject::connect(ui->AddTask,&QToolButton::clicked,this,[=]()
    {
        Addtasksmanually(); //手动按钮添加单个任务
    });

    QObject::connect(ui->toolButton_ok,&QToolButton::clicked,this,[=]()
    {
        savewaitTestSample(); //保存任务
    });

    QObject::connect(ui->toolButton_Delete_sel_item,&QToolButton::clicked,this,[=]()
    {
        deleteSelectedSample(); //删除选中待测样本
    });

    QObject::connect(ui->toolButtonbatchAddTask,&QToolButton::clicked,this,[=]()
    {
        batchAddTask();
    });

    if(!m_threadaddsample)
    {
        m_threadaddsample = new mythreadaddsample();
        qRegisterMetaType<WAIT_TEST_SAMPLE_DATA>("WAIT_TEST_SAMPLE_DATA");
        qRegisterMetaType<QList<quint8> > ("QList<quint8>");

        connect(this,&Height_Data::sycnwaittestsampledata,
                m_threadaddsample,&mythreadaddsample::waittestsampledata);

        connect(m_threadaddsample,&mythreadaddsample::addprogress,
                this,&Height_Data::updateaddprogress);

        connect(m_threadaddsample,&mythreadaddsample::updatetestui,
                this,&Height_Data::slotupdatetestui);

        connect(m_threadaddsample,&mythreadaddsample::_sycnPaintentInfo,
                FullyAutomatedPlatelets::mainWindow(),&MainWindow::slotsycnPaintentInfo,
                Qt::QueuedConnection);

        connect(m_threadaddsample, &mythreadaddsample::suckPosInvalid,
                this, [this](const QString& msg) {
            emit ReminderTextOut(ERRORLOG, msg);
            CustomMessageBox::warning(this, tr("吸样位置错误"), msg);
        }, Qt::QueuedConnection);

        m_threadaddsample->Start();
    }
}

void Height_Data::fillPressedHoles(int totalHole)
{
    m_pressedhole.clear();
    m_selbloodholetemp.clear();
    for (int n = 0; n < totalHole; ++n) {
        m_pressedhole.insert(QString::number(2 * n + 1), false);
    }
}

void Height_Data::initNumAnaemiaHole()
{
    try {
        quint8 totalHole = 0;
        quint8 kindEquipment = 0;
        SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, kindEquipment);
        switch (kindEquipment) {
        case KS600:
            totalHole = 30; // 6 组 × 5 对 PPP+PRP
            break;
        case KS800:
            totalHole = 40; // 8 组 × 5 对 PPP+PRP（旧值 45 会多出不存在的孔）
            break;
        case KS1200:
            totalHole = 60; // 12 组 × 5 对 PPP+PRP
            break;
        default:
            totalHole = 60;
            QLOG_DEBUG() << "Unknown equipment kind:" << kindEquipment << ", using default 60 holes";
            break;
        }
        fillPressedHoles(totalHole);
    } catch (const std::exception& e) {
        QLOG_ERROR() << "Exception in _initNumAnaemiaHole:" << e.what();
        fillPressedHoles(60);
    } catch (...) {
        QLOG_ERROR() << "Unknown exception in _initNumAnaemiaHole";
        fillPressedHoles(60);
    }
}

void Height_Data::restoreHoleOccupancy(quint8 hole)
{
    auto tubeIt = m_pressedhole.find(QString::number(hole));
    if (tubeIt == m_pressedhole.end()) {
        return;
    }
    tubeIt.value() = false;
    for (auto it = m_selbloodholetemp.begin(); it != m_selbloodholetemp.end(); ) {
        if (it.value() == hole) {
            it = m_selbloodholetemp.erase(it);
            break;
        }
        ++it;
    }
}

//补回用掉的试管孔
void Height_Data::Makeuptubenum(quint8 canceltube)
{
    if (canceltube % 2 == 0) {
        return;
    }
    QLOG_INFO() << "Recovering tube hole:" << canceltube;
    restoreHoleOccupancy(canceltube);
}

//测试界面取消 补回血样孔
void Height_Data::backcancelhole(const quint8 index)
{
    QLOG_DEBUG() << "测试界面删除样本孔号补回" << index;
    restoreHoleOccupancy(index);
    Q_EMIT reminderPPPandPRPputHoleNotify(index, 255);
}

void Height_Data::resizeEvent(QResizeEvent* event){
    QWidget::resizeEvent(event);
    // 全血模式下，强制保持图片区域不被压缩
    if (INI_File().GetWholeBloodModel()) {
        QWidget* pshowImage = ui->widget_ShowErrImage;
        if (pshowImage && pshowImage->isVisible()) {
            int desiredWidth = 360;
            if (pshowImage->width() != desiredWidth) {
                pshowImage->setFixedWidth(desiredWidth);
                ui->horizontalLayout_3->invalidate();
                ui->horizontalLayout_3->activate();
            }
        }
    }
}

void Height_Data::adjustTableColumnWidths()
{
    QTableWidget *table = ui->Sample_Data_tablewidget;
    if (!table) return;

    table->horizontalHeader()->setSectionResizeMode(CHECK_ROW, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(SAMLPE_NAME, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(HEIGHT_DATA, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(RICHBLOOD_HOLE, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(PROJECT_ITEM, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(BARCODE, QHeaderView::Stretch);
    table->horizontalHeader()->resizeSection(CHECK_ROW, 50);
    table->horizontalHeader()->update();
    table->update();
}
void Height_Data::closeEvent(QCloseEvent *event)
{
    emit Taskconfigcloe();

    if (m_titleBar && isMaximized()) {
        m_titleBar->updateMaximizeButton(false);
    }

    event->accept();
}

bool Height_Data::eventFilter(QObject *obj, QEvent *event)
{
     // 1. 全局屏蔽 SpinBox / ComboBox 的滚轮事件
     if (event->type() == QEvent::Wheel &&
         (obj->inherits("QAbstractSpinBox") || obj->inherits("QComboBox")))
     {
         return true;  // 丢弃滚轮事件
     }

     // 2. 只处理当前控件的键盘按下事件
    if (obj != this || event->type() != QEvent::KeyPress)
        return QWidget::eventFilter(obj, event);

    auto *keyEvent = static_cast<QKeyEvent*>(event);

    // 2.1 检查扫码功能是否开启（避免频繁读取配置文件）
    static bool isScanEnabled = INI_File().rConfigPara(SCANCODEBAR).toBool();
    if (!isScanEnabled)
    {
        QLOG_DEBUG() << "扫码功能未打开!";
        return true;  // 吞掉事件
    }

    // 2.2 非回车 → 累积条码字符（加长度限制，防止内存爆炸）
    if (keyEvent->key() != Qt::Key_Return)
    {
        QString inputText = keyEvent->text();
        if (!inputText.isEmpty())
        {
            constexpr int MAX_BARCODE_LEN = 1024;  // 根据实际条码最大长度调整
            if (m_Barcodestr.length() + inputText.length() <= MAX_BARCODE_LEN)
                m_Barcodestr += inputText;
            else
                QLOG_WARN() << "条码超过最大长度限制，已忽略后续输入";
        }
        return true;  // 事件已处理
    }

    // 2.3 回车 → 处理完整条码
    // 验证内容
    if (m_Barcodestr.trimmed().isEmpty())
    {
        QLOG_DEBUG() << "扫码内容为空或仅含空白字符";
        emit ReminderTextOut(ERRORLOG, tr("扫码失败: 内容为空或仅含空白字符"));
        m_Barcodestr.clear();
        return true;
    }

    // 记录日志
    QLOG_DEBUG() << "扫码QrCode结果:" << m_Barcodestr;

    // 写入表格（重构为小函数，避免 eventFilter 过长）
    writeBarcodeToTable(m_Barcodestr);

    // 清空缓存并通知完成
    m_Barcodestr.clear();
    FullyAutomatedPlatelets::mainWindow()->TestHeightFinish(true);

    return true;
}

void Height_Data::writeBarcodeToTable(const QString &barcode)
{
    int rowCount = ui->Sample_Data_tablewidget->rowCount();
    if (rowCount <= 0) return;

    int rowIndex = rowCount - 1;
    QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(rowIndex, BARCODE);
    if (!item)
    {
        item = new QTableWidgetItem();
        ui->Sample_Data_tablewidget->setItem(rowIndex, BARCODE, item);
    }
    item->setText(barcode);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    item->setTextAlignment(Qt::AlignCenter);
}



void Height_Data::updateVerticalHeaderRowNumbers()
{
    int rowCount = ui->Sample_Data_tablewidget->rowCount();
    QTableWidget* table = ui->Sample_Data_tablewidget;

    for (int row = 0; row < rowCount; ++row) {
        // 检查 verticalHeaderItem 是否存在，如果不存在则创建
        QTableWidgetItem* headerItem = table->verticalHeaderItem(row);
        if (!headerItem) {
            headerItem = new QTableWidgetItem();
            table->setVerticalHeaderItem(row, headerItem);
        }
        headerItem->setText(QString::number(row + 1));
    }

}
void Height_Data::updateTableHeaderText()
{
    if (!ui || !ui->Sample_Data_tablewidget) return;

    bool isWholeBlood = INI_File().GetWholeBloodModel();
    QString headerText = isWholeBlood ? tr("红细胞高度") : tr("吸样高度");

    QStringList headers;
    headers << tr("测试选项") << tr("样本号") << headerText
            << tr("血样孔号") << tr("测试项目") << tr("条形码");

    ui->Sample_Data_tablewidget->setHorizontalHeaderLabels(headers);
}

void Height_Data::InitTablewidget()
{
    QStringList header;
    header<<tr("测试选项")<<tr("样本号")<<tr("测高值")<<tr("血样孔号")<<tr("测试项目")<<tr("条形码");
    ui->Sample_Data_tablewidget->setColumnCount(6);
    ui->Sample_Data_tablewidget->setHorizontalHeaderLabels(header);

    // 启用垂直表头并设置行号
    ui->Sample_Data_tablewidget->verticalHeader()->setVisible(true);
    ui->Sample_Data_tablewidget->verticalHeader()->setDefaultSectionSize(60);

    // 设置行号列的宽度
    ui->Sample_Data_tablewidget->verticalHeader()->setFixedWidth(50);
    ui->Sample_Data_tablewidget->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
    // 设置行号字体样式
    QFont headerFont;
    headerFont.setPixelSize(14);
    headerFont.setFamily("Microsoft YaHei");
    ui->Sample_Data_tablewidget->verticalHeader()->setFont(headerFont);

    // 设置行号背景色（可选）
    ui->Sample_Data_tablewidget->verticalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "   background-color: #E8F4FD;"
        "   color: #2C5F8A;"
        "   font-weight: bold;"
        "   border: none;"
        "   border-right: 1px solid #D4EAF5;"
        "   padding: 0px;"
        "   text-align: center;"
        "}"
    );

    ui->Sample_Data_tablewidget->horizontalHeader()->setFont(headerFont);
    ui->Sample_Data_tablewidget->horizontalHeader()->setStretchLastSection(true);
    ui->Sample_Data_tablewidget->setFrameShape(QFrame::NoFrame);
    ui->Sample_Data_tablewidget->setShowGrid(false);

    ui->Sample_Data_tablewidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->Sample_Data_tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->Sample_Data_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->Sample_Data_tablewidget->verticalHeader()->setDefaultSectionSize(60);
    ui->Sample_Data_tablewidget->horizontalHeader()->setDefaultSectionSize(30);
    ui->Sample_Data_tablewidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed); // 固定高度
    adjustTableColumnWidths();
    ui->Sample_Data_tablewidget->horizontalHeader()->setFixedHeight(45);
    ui->Sample_Data_tablewidget->setAlternatingRowColors(true);

    QFont tableFont;
    tableFont.setPointSize(14);
    tableFont.setFamily("Microsoft YaHei");
    ui->Sample_Data_tablewidget->setFont(tableFont);
    // 应用医疗主题样式
    ui->Sample_Data_tablewidget->setStyleSheet(getMedicalTableStyle());
    ui->Sample_Data_tablewidget->horizontalHeader()->setStyleSheet(getMedicalHeaderStyle());

    // 设置滚动条样式
    ui->Sample_Data_tablewidget->horizontalScrollBar()->setStyleSheet(getMedicalScrollBarStyle());
    ui->Sample_Data_tablewidget->verticalScrollBar()->setStyleSheet(getMedicalScrollBarStyle());

    connect(ui->Sample_Data_tablewidget, SIGNAL(cellClicked(int,int)), this, SLOT(tableItemClicked(int,int)));
    connect(ui->Sample_Data_tablewidget->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(sortByColumn(int)));
    connect(ui->Sample_Data_tablewidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(on_Sample_Data_tablewidget_customContextMenuRequested(QPoint)));
    updateTableHeaderText();
    return;
}

// 医疗主题样式函数实现
QString Height_Data::getMedicalTableStyle() const
{
    static const QString kStyle = QString(
        "QTableWidget {"
        "   background-color: %1;"
        "   border: none;"
        "   border-radius: 8px;"
        "   gridline-color: %2;"
        "   outline: 0px;"
        "}"
        "QTableWidget::item {"
        "   padding: 2px;"
        "   border-bottom: 1px solid %2;"
        "   border-right: none;"
        "   font-size: 14px;"           // 添加这一行，调整字体大小
        "   font-family: 'Microsoft YaHei';"  // 可选，设置字体
        "}"
        "QTableWidget::item:hover {"
        "   background-color: %3;"
        "   color: #2C5F8A;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: %4;"
        "   color: #FFFFFF;"
        "}"
        "QTableWidget::item:selected:hover {"
        "   background-color: %5;"
        "}"
        "QTableWidget::item:!selected:hover {"
        "   background-color: %3;"
        "}"
        "QTableCornerButton::section {"
        "   background-color: %6;"
        "   border: none;"
        "   border-radius: 0px;"
        "}"
    ).arg(MedicalTableColors::TABLE_BG,
          MedicalTableColors::GRID_COLOR,
          MedicalTableColors::ROW_HOVER,
          MedicalTableColors::ROW_SELECTED,
          MedicalTableColors::ROW_SELECTED_HOVER,
          MedicalTableColors::HEADER_BG);
    return kStyle;
}

QString Height_Data::getMedicalHeaderStyle() const
{
    static const QString kStyle = QString(
        "QHeaderView::section {"
        "   background-color: %1;"
        "   color: %2;"
        "   font: 14pt 'Microsoft YaHei';"
        "   font-weight: bold;"
        "   padding: 10px;"
        "   border: none;"
        "   border-right: 1px solid %3;"
        "   border-bottom: 2px solid #7EC8F0;"
        "}"
        "QHeaderView::section:last {"
        "   border-right: none;"
        "}"
        "QHeaderView::section:hover {"
        "   background-color: #D4EAF5;"
        "}"
        "QHeaderView {"
        "   background-color: %1;"
        "   border: none;"
        "}"
    ).arg(MedicalTableColors::HEADER_BG,
          MedicalTableColors::HEADER_TEXT,
          MedicalTableColors::GRID_COLOR);
    return kStyle;
}

QString Height_Data::getMedicalScrollBarStyle() const
{
    static const QString kStyle = QString(
        "QScrollBar:horizontal {"
        "   height: 10px;"
        "   background: %1;"
        "   border-radius: 5px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:horizontal {"
        "   background: #7EC8F0;"
        "   border-radius: 5px;"
        "   min-width: 30px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "   background: #5BB8E8;"
        "}"
        "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
        "   border: none;"
        "   background: none;"
        "}"
        "QScrollBar:vertical {"
        "   width: 10px;"
        "   background: %1;"
        "   border-radius: 5px;"
        "   margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #7EC8F0;"
        "   border-radius: 5px;"
        "   min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #5BB8E8;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   border: none;"
        "   background: none;"
        "}"
    ).arg(MedicalTableColors::BG_LIGHT);
    return kStyle;
}

QString Height_Data::getMedicalComboxStyle() const
{
    static const QString kStyle = QString(
        "QComboBox {"
        "   font-family: 'Microsoft YaHei';"
        "   font-size: 14px;"
        "   color: #2C5F8A;"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #B8DFF7;"
        "   border-radius: 6px;"
        "   padding: 5px 10px;"
        "}"
        "QComboBox:hover {"
        "   border-color: #7EC8F0;"
        "   background-color: #F0F9FF;"
        "}"
        "QComboBox:focus {"
        "   border-color: #5BB8E8;"
        "}"
        "QComboBox::drop-down {"
        "   border: none;"
        "   width: 25px;"
        "}"
        "QComboBox::down-arrow {"
        "   image: url(:/Picture/SetPng/commbox.png);"
        "   width: 16px;"
        "   height: 16px;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #FFFFFF;"
        "   border: 1px solid #B8DFF7;"
        "   border-radius: 6px;"
        "   selection-background-color: #7EC8F0;"
        "   selection-color: #FFFFFF;"
        "   outline: 0px;"
        "}"
        "QComboBox QAbstractItemView::item {"
        "   height: 35px;"
        "   padding: 5px;"
        "}"
        "QComboBox QAbstractItemView::item:hover {"
        "   background-color: #E8F4FD;"
        "   color: #2C5F8A;"
        "}"
    );
    return kStyle;
}

QCheckBox *Height_Data::rowSelectCheckBox(int row) const
{
    QWidget *cellWidget = ui->Sample_Data_tablewidget->cellWidget(row, CHECK_ROW);
    if (!cellWidget) {
        return nullptr;
    }
    return qobject_cast<QCheckBox *>(cellWidget->children().value(1, nullptr));
}

void Height_Data::obtainSelectedSample(QList<int> &itemList)
{
    itemList.clear();
    itemList.reserve(ui->Sample_Data_tablewidget->rowCount());

    const int totalRows = ui->Sample_Data_tablewidget->rowCount();
    for (int row = 0; row < totalRows; ++row) {
        QCheckBox *checkbox = rowSelectCheckBox(row);
        if (checkbox && checkbox->isChecked()) {
            itemList.append(row);
        }
    }
}

bool Height_Data::shouldIgnoreClick(int col) const {
    return ui->Sample_Data_tablewidget->rowCount() == 0 ||
           col == static_cast<int>(CHECK_ROW) ||
           col == static_cast<int>(RICHBLOOD_HOLE);
}

void Height_Data::saveOriginalValue(int row, int col) {
    if (row < 0 || col < 0) {
        QLOG_WARN() << "Invalid row or column index:" << row << col;
        return;
    }

    QTableWidget* tableWidget = ui->Sample_Data_tablewidget;
    if (!tableWidget) {
        QLOG_ERROR() << "Table widget is null!";
        return;
    }

    if (row >= tableWidget->rowCount() || col >= tableWidget->columnCount()) {
        QLOG_WARN() << "Index out of range - row:" << row << "/" << tableWidget->rowCount()
                   << ", col:" << col << "/" << tableWidget->columnCount();
        return;
    }

    QTableWidgetItem* item = tableWidget->item(row, col);
    if (!item) {
        m_OriginallyValue.clear();
        return;
    }

    m_OriginallyValue = item->text();
    QLOG_DEBUG() << "Saved original value:" << m_OriginallyValue << "at [" << row << "," << col << "]";
}

void Height_Data::handleSampleOrHeightClick(int row, int col) {
    if (row < 0 || col < 0) {
        qWarning() << "Invalid row or column index:" << row << col;
        return;
    }

    QTableWidget* tableWidget = ui->Sample_Data_tablewidget;
    if (!tableWidget) {
        qCritical() << "Table widget is null!";
        return;
    }

    if (row >= tableWidget->rowCount() || col >= tableWidget->columnCount()) {
        qWarning() << "Index out of range - row:" << row << "/" << tableWidget->rowCount()
                   << ", col:" << col << "/" << tableWidget->columnCount();
        return;
    }

    if (col == static_cast<int>(SAMLPE_NAME) || col == static_cast<int>(HEIGHT_DATA)) {
        QTableWidgetItem* nameItem = tableWidget->item(row, col);
        if (!nameItem) {
            qDebug() << "No item at sample name column, row:" << row;
            return;
        }

        QString sampleName = nameItem->text().trimmed();
        if (sampleName.isEmpty()) {
            qDebug() << "Sample name is empty at row:" << row;
            return;
        }

        showCorrectDialog(row,col,sampleName);
    }

}

void Height_Data::handleProjectItemClick(int row, int col) {
    auto* projectWindow = FullyAutomatedPlatelets::pinstanceTestproject();
    projectWindow->setWindowModality(Qt::NonModal);
    projectWindow->setWindowFlags(Qt::Window |
                                  Qt::FramelessWindowHint |
                                  Qt::WindowStaysOnTopHint |
                                  Qt::WindowDoesNotAcceptFocus);
    projectWindow->Slot_ChangeSingleItem(row, col, true);
    projectWindow->show();
}

void Height_Data::handleBarcodeClick(int row, int col) {
    if (!m_Notifycode) {
        m_Notifycode = new NotifyBarCode(this);
        connect(m_Notifycode, &NotifyBarCode::savebarcode, this, &Height_Data::slotsavebarcode);
    }
    m_Notifycode->changerowAndCol(row, col, m_OriginallyValue);
    m_Notifycode->show();
}

void Height_Data::tableItemClicked(int row,int col){
    if (shouldIgnoreClick(col)) return;

    saveOriginalValue(row, col);
    ui->Sample_Data_tablewidget->removeCellWidget(row, col);

    switch (static_cast<int>(col)) {
        case SAMLPE_NAME:
        case HEIGHT_DATA:
            handleSampleOrHeightClick(row, col);
            break;
        case PROJECT_ITEM:
            handleProjectItemClick(row, col);
            break;
        case BARCODE:
            handleBarcodeClick(row, col);
            break;
        default:
            break;
    }

    return;
}

void Height_Data::slotsavebarcode(unsigned int row,quint8 cols,QString noityBarcode)
{
    QTableWidget *table = ui->Sample_Data_tablewidget;
    if (!table) {
        QLOG_ERROR() << "slotsavebarcode: table is nullptr";
        return;
    }

    // 检查行是否有效
    if (row >= (unsigned int)table->rowCount()) {
        QLOG_ERROR() << QString("slotsavebarcode: 无效行号 - row=%1, 总行数=%2").arg(row).arg(table->rowCount());
        return;
    }

    // 检查列是否有效
    if (cols >= (quint8)table->columnCount()) {
        QLOG_ERROR() << QString("slotsavebarcode: 无效列号 - col=%1, 总列数=%2").arg(cols).arg(table->columnCount());
        return;
    }

    // 获取或创建表格项
    QTableWidgetItem* item = table->item(row, cols);
    if (!item) {
        QLOG_WARN() << QString("slotsavebarcode: 创建新的表格项 - row=%1, col=%2").arg(row).arg(cols);
        item = new QTableWidgetItem();
        table->setItem(row, cols, item);
    }

    // 设置条形码文本
    item->setText(noityBarcode);
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);

    // 确保表格刷新显示
	table->viewport()->update();  // 刷新表格内容区域
    table->update();
}

void Height_Data::onNotifyTestHeight(int Rows, int cols, QString Value)
{
    SetColumnText(Rows, cols, Value);
    return;
}



bool Height_Data::validateSampleUniqueness(const QString &sampleData,const bool isfollowUp)
{
    if (!repeatData(sampleData)) {
        CustomMessageBox::warning(this,tr("修改样本id失败"),"任务列表样本号已存在,请勿重复!");
        return false;
    }

    if (SQLTheSameItem(sampleData,isfollowUp)) {
        CustomMessageBox::warning(this,tr("修改样本id失败"),"样本数据库号已存在,请勿重复!");
        return false;
    }

    return true;
}

void Height_Data::updateCreationTimeMapping(unsigned int rows,const QString &newSampleData)
{
    if (m_sampleCreateTime.contains(m_OriginallyValue)) {
        m_sampleCreateTime.remove(m_OriginallyValue);
    }

    QTableWidgetItem *sampleNameItem = ui->Sample_Data_tablewidget->item(rows, SAMLPE_NAME);
    if (!sampleNameItem) {
        QLOG_WARN() << "无法获取样本名称，行:" << rows << "列:" << SAMLPE_NAME;
        return;
    }

    QString sampleName = newSampleData;//sampleNameItem->text();
    QString currentTime = QDateTime::currentDateTime().toString("MM.dd hh:mm:ss");
    m_sampleCreateTime.insert(sampleName, currentTime);

    QLOG_DEBUG() << "更新创建时间映射 - 样本:" << sampleName << "时间:" << currentTime;
}

void Height_Data::restoreOriginalValue(unsigned int rows, int cols)
{
    SetColumnText(rows, cols, m_OriginallyValue);
}



bool Height_Data::repeatData(QString textChange)
{
   if (textChange.isEmpty()) {
       QLOG_WARN() << "重复检查: 输入文本为空";
       return false;
   }

    const int rowCount = ui->Sample_Data_tablewidget->rowCount();

   if (rowCount == 0) {
       return true;
   }

   for (int row = 0; row < rowCount; ++row) {
       QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);

       if (!item || item->text().isEmpty()) {
           continue;
       }

       if (textChange.compare(item->text(), Qt::CaseSensitive) == 0) {
           QLOG_DEBUG() << "发现重复数据 - 行:" << row << "值:" << textChange;
           return false;
       }
   }
   return true;
}

bool Height_Data::SQLTheSameItem(QString textchange, const bool isfollowUp)
{
    QString date;
    int idsample = 0;
    GlobalData::apartSampleId(textchange,date, idsample);
    QSet<QString> todaySampleid;
    todaySampleid = FullyAutomatedPlatelets::pinstancesqlData()->getTodayAllSampleid(date,isfollowUp);
    if (isfollowUp) {
        const QString reviewId = textchange.endsWith(QLatin1Char('F'))
                ? textchange : (textchange + QLatin1Char('F'));
        return todaySampleid.contains(reviewId);
    }
    return todaySampleid.contains(textchange);
}

void Height_Data::sortByColumn(int col)
{
    if (ui->Sample_Data_tablewidget->rowCount() == 0) {
        return;
    }

     if(col != PROJECT_ITEM) return;

    ConfigureProjectItem *projectWindow = FullyAutomatedPlatelets::pinstanceTestproject();
    if (!projectWindow) {
        QLOG_ERROR() << "无法获取项目选择窗口实例";
        return;
    }

    projectWindow->setWindowModality(Qt::NonModal);
    projectWindow->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |
                                 Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);

    projectWindow->show();

    projectWindow->Slot_ChangeSingleItem(0, PROJECT_ITEM, false);
}

void Height_Data::SetColumnText(int row,int col,QString text)
{
    QTableWidgetItem *oldItem = ui->Sample_Data_tablewidget->item(row, col);
    const QVariant oldEmergency = (oldItem && col == SAMLPE_NAME) ? oldItem->data(RoleEmergency) : QVariant();
    const QVariant oldFollowUp = (oldItem && col == SAMLPE_NAME) ? oldItem->data(RoleFollowUp) : QVariant();
    const QBrush oldBg = (oldItem && col == SAMLPE_NAME) ? oldItem->background() : QBrush();

    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignCenter);

    QFont cellFont = item->font();
    cellFont.setPointSize(14);
    cellFont.setFamily("Microsoft YaHei");
    item->setFont(cellFont);

    if (col == HEIGHT_DATA) {
        setHeightDataBackground(item, text);
    } else if (col == PROJECT_ITEM) {
        item->setTextColor(QColor(0,0,0));
    } else if (col == SAMLPE_NAME) {
        if (oldItem) {
            item->setBackground(oldBg);
            item->setData(RoleEmergency, oldEmergency);
            item->setData(RoleFollowUp, oldFollowUp);
        }
    }

    ui->Sample_Data_tablewidget->setItem(row, col, item);
    if (col == SAMLPE_NAME) {
        applySampleRowMarks(row);
    }
}

void Height_Data::applySampleRowMarks(int row)
{
    QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
    if (!item) {
        return;
    }
    const bool emergency = item->data(RoleEmergency).toBool();
    const bool followUp = item->data(RoleFollowUp).toBool();
    QString tip;
    if (emergency) {
        tip += tr("急诊优先");
    }
    if (followUp) {
        if (!tip.isEmpty()) {
            tip += tr(" / ");
        }
        tip += tr("复查（结果入库号后加F）");
    }
    item->setToolTip(tip);

    QFont font = item->font();
    font.setBold(emergency || followUp);
    item->setFont(font);
    if (emergency) {
        item->setTextColor(QColor(192, 57, 43));
    } else if (followUp) {
        item->setTextColor(QColor(41, 128, 185));
    } else {
        item->setTextColor(QColor(0, 0, 0));
    }
}

bool Height_Data::isRowEmergency(int row) const
{
    QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
    return item && item->data(RoleEmergency).toBool();
}

bool Height_Data::isRowFollowUp(int row) const
{
    QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
    return item && item->data(RoleFollowUp).toBool();
}

void Height_Data::setRowEmergency(int row, bool on)
{
    QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
    if (!item) {
        return;
    }
    item->setData(RoleEmergency, on);
    applySampleRowMarks(row);
}

void Height_Data::setRowFollowUp(int row, bool on)
{
    QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
    if (!item) {
        return;
    }
    item->setData(RoleFollowUp, on);
    applySampleRowMarks(row);
}

void Height_Data::applyEmergencyToSelectedOrAll(bool on)
{
    QList<int> rows;
    obtainSelectedSample(rows);
    if (rows.isEmpty() && ui->Sample_Data_tablewidget) {
        for (int r = 0; r < ui->Sample_Data_tablewidget->rowCount(); ++r) {
            rows.append(r);
        }
    }
    for (int row : rows) {
        setRowEmergency(row, on);
    }
}

bool Height_Data::validateSelectedRowsForSave(const QList<int> &selectedRows)
{
    for (int row : selectedRows) {
        QTableWidgetItem *sampleItem = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
        if (!sampleItem) {
            continue;
        }
        const QString sampleId = sampleItem->text().trimmed();
        const bool followUp = isRowFollowUp(row);
        if (followUp) {
            if (SQLTheSameItem(sampleId, true)) {
                CustomMessageBox::warning(this, tr("添加测试样本失败"),
                    tr("复查号已存在：%1F\n请更换样本号或确认今日尚未复查过该号。").arg(sampleId));
                return false;
            }
        } else if (SQLTheSameItem(sampleId, false)) {
            CustomMessageBox::warning(this, tr("添加测试样本失败"),
                tr("今日已有样本 %1。\n若要重测某个项目：点该样本号 → 复查 → 只勾选要重测的试剂。").arg(sampleId));
            return false;
        }
    }
    return true;
}

void Height_Data::setHeightDataBackground(QTableWidgetItem *item, const QString &text)
{
    bool conversionOk = false;
    double value = text.toDouble(&conversionOk);

    if (!conversionOk || value <= 0.0) {
        item->setBackground(QColor(255, 0, 0));
    } else {
        item->setBackground(QColor(255, 255, 255));
    }
}

bool Height_Data::SameSampleandtube(QList<int> TaskList, QTableWidget *TaskWidget)
{
    QString samplenum_;
    bool bhandsame_ = false;
    foreach (int index_row, TaskList)
    {
       QString itemtext = TaskWidget->item(index_row, SAMLPE_NAME)->text();
       if(samplenum_ != itemtext)
           samplenum_ = itemtext;
       else
       {
           CustomMessageBox::warning(this,tr("添加测试样本失败"),"保存失败样本号重复,请重设!");
           bhandsame_ = true;
           break;
       }
    }
    return  bhandsame_;
}

bool Height_Data::validateTableItems(QTableWidget* taskWidget, const QList<int>& selectedRows)
{
    if (!taskWidget) {
        qWarning() << "Task widget is null";
        return false;
    }

    for (int row : selectedRows) {
        if (row < 0 || row >= taskWidget->rowCount()) {
            qWarning() << "Invalid row number:" << row;
            return false;
        }

        for (int col = 1; col <= 5; ++col) {
            if (col == 3) continue;

            QTableWidgetItem* item = taskWidget->item(row, col);
            if (!item || item->text().trimmed().isEmpty()) {
                qDebug() << "Missing or empty item at row:" << row << "column:" << col;
                return false;
            }
        }
    }
    return true;
}

bool Height_Data::TheSameBloodHole()
{
    bool bhadSame = false;
    int  totalRows = ui->Sample_Data_tablewidget->rowCount();
    QString outtext = "";
    QMap<QString, bool> suckHoleBlood; suckHoleBlood.clear();
    for (int row = 0; row < totalRows; ++row)
    {
        QWidget* cellWidget = ui->Sample_Data_tablewidget->cellWidget(row, RICHBLOOD_HOLE);
        if(!cellWidget){
            QLOG_WARN() << "Cell widget is null at row:" << row;
            return false;
        }
        QComboBox* combox = qobject_cast<QComboBox*>(cellWidget);
        if(!combox){
            QLOG_WARN() << "Cell widget is not a QComboBox at row:" << row;
            return false;
        }
        QString holeNumber = combox->currentText().trimmed();
        if(!holeNumber.isEmpty()){
            if (!suckHoleBlood.contains(holeNumber)){
                suckHoleBlood.insert(holeNumber, true);
            }else{
                outtext += QString("同孔请检查第%1行孔号%2").arg(row + 1).arg(holeNumber) + "\n";
                bhadSame = true;
            }
        }
    }
    if (bhadSame) {
        CustomMessageBox::warning(this,tr("添加测试样本失败"),outtext);
    }
    return bhadSame;
}

bool Height_Data::NeedTubeEnouthTesting(const int hadtube,int &needtube ,QList<int> selItems)
{
    int tmpneedTube = 0;
    bool benoughtest = false;
    for(auto TaskItem : selItems)
    {
        int  Sel_Row = TaskItem;
        QString reagentItem = ui->Sample_Data_tablewidget->item(Sel_Row,PROJECT_ITEM)->text();
        QStringList ProjectList = reagentItem.split("|");
        tmpneedTube  = tmpneedTube + (ProjectList.size() + 1);
    }
    needtube = tmpneedTube;

    const QString _putlog = tr("仪器剩余试管数=%1 [加样所需试管数=%2]").arg(hadtube).arg(needtube);
    emit this->ReminderTextOut(PROMPTLOG,_putlog);
    QLOG_DEBUG()<<_putlog<<endl;

    (hadtube >= tmpneedTube)? benoughtest = true : benoughtest = false;
    return benoughtest;
}

void Height_Data::on_toolButton_Cancel_clicked()
{
    close();
}

void Height_Data::DeleteAllItems(QTableWidget * tableWidget)
{
    if (tableWidget) {
        tableWidget->setRowCount(0);
    }
}

bool Height_Data::hasAvailableSampleHole(QStringList &availableHoleList)
{
    availableHoleList.clear();

    for (auto iter = m_pressedhole.constBegin(); iter != m_pressedhole.constEnd(); ++iter) {
        if (!iter.value()) {
            availableHoleList.append(iter.key());
        }
    }

    std::sort(availableHoleList.begin(), availableHoleList.end(),
              [](const QString& s1, const QString& s2) {
                  return s1.toInt() < s2.toInt();
              });

    return !availableHoleList.isEmpty();
}



void Height_Data::updateotherinserthole(int _rows, QString index_)
{
    int total_row = ui->Sample_Data_tablewidget->rowCount();
    for(int r = 0 ; r < total_row; r++) {
        if(r == _rows) continue;

        QWidget * widget = ui->Sample_Data_tablewidget->cellWidget(r, RICHBLOOD_HOLE);
        QComboBox *combox = qobject_cast<QComboBox*>(widget);
        if (!combox) continue;
        if (combox->findText(index_) >= 0) {
            continue;
        }

        const int hole = index_.toInt();
        int insertAt = combox->count();
        for (int i = 0; i < combox->count(); ++i) {
            if (combox->itemText(i).toInt() > hole) {
                insertAt = i;
                break;
            }
        }
        combox->blockSignals(true);
        combox->insertItem(insertAt, index_);
        combox->blockSignals(false);
    }
}

void Height_Data::selectPPPholeChange(const QString& index_) {
	QComboBox *member = qobject_cast<QComboBox*>(sender());
	if (!member) {
		QLOG_ERROR() << "selectPPPholeChange: sender is not a QComboBox";
		return;
	}

	quint8 changeHole = index_.toInt();
	QTableWidget *ptable = ui->Sample_Data_tablewidget;
	if (!ptable) {
		QLOG_ERROR() << "selectPPPholeChange: ptable is nullptr";
		return;
	}

	int row = -1;
	int column = -1;
	for (int r = 0; r < ptable->rowCount(); ++r) {
		if (ptable->cellWidget(r, RICHBLOOD_HOLE) == member) {
			row = r;
			column = RICHBLOOD_HOLE;
			break;
		}
	}

	// 如果方法1失败，尝试方法2：坐标转换
	if (row == -1) {
		const QPoint globalPos = member->mapToGlobal(QPoint(0, 0));
		const QPoint tablePos = ptable->mapFromGlobal(globalPos);
		const QModelIndex index_item = ptable->indexAt(tablePos);
		row = index_item.row();
		column = index_item.column();
	}

	// 检查索引是否有效
	if (row < 0 || row >= ptable->rowCount() || column < 0 || column >= ptable->columnCount()) {
		QLOG_ERROR() << QString("selectPPPholeChange: 无效索引 - row=%1, column=%2").arg(row).arg(column);
		return;
	}

    QString sampleidChnage;
    QTableWidgetItem *item = ptable->item(row, SAMLPE_NAME);
    if(item != nullptr)
        sampleidChnage = ptable->item(row, SAMLPE_NAME)->text();

    if (column == RICHBLOOD_HOLE) {
        if(m_selbloodholetemp.contains(sampleidChnage)) {
            auto ittmp = m_selbloodholetemp.find(sampleidChnage);
			quint8 oldPPPhole = ittmp.value();

            if (m_pressedhole.contains(QString::number(oldPPPhole))) {
                m_pressedhole[QString::number(oldPPPhole)] = false;
            }

            ittmp.value() = changeHole;
            if (m_pressedhole.contains(QString::number(changeHole))) {
                m_pressedhole[QString::number(changeHole)] = true;
            }

			Q_EMIT reminderPPPandPRPputHoleNotify(oldPPPhole, changeHole);
            QLOG_DEBUG() << "孔号已更改: 行" << row
                         << "从" << oldPPPhole << "改为" << changeHole;
        }
    }
	
   
    return;
}

void Height_Data::updateTableItem(const QString& id, double heightValue)
{
    QTableWidget* table = ui->Sample_Data_tablewidget;
    const int rowCount = table->rowCount();

    for(int i = 0; i < rowCount; ++i) {
        QTableWidgetItem* idItem = table->item(i, SAMLPE_NAME);
        if (!idItem || idItem->text() != id) continue;

        QTableWidgetItem* heightItem = table->item(i, HEIGHT_DATA);
        if (!heightItem) {
            heightItem = new QTableWidgetItem();
            table->setItem(i, HEIGHT_DATA, heightItem);
        }

        heightItem->setText(QString::number(heightValue));
        heightItem->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        heightItem->setForeground((heightValue <= 0) ? Qt::red : palette().text());
        break;
    }
}

void Height_Data::handleSampleAddition(const double heightValue)
{
    QStringList availableHoles;
    if (!hasAvailableSampleHole(availableHoles)) {
        showHoleWarning(availableHoles);
        return;
    }

    addOneTestSample(INI_File().GetWholeBloodModel(), heightValue, availableHoles, "null");
}

void Height_Data::showHoleWarning(const QStringList& holes)
{
    QString errorMsg =  QString("可用孔位: %1\n%2").arg(holes.join(",")).arg("无可用加样位置");
    CustomMessageBox::warning(this,tr("添加样本失败"),errorMsg);
}

void Height_Data::on_Sample_Data_tablewidget_customContextMenuRequested(const QPoint &)
{
    QMenu menu;
    QAction *selectAll = menu.addAction(tr("全选"));
    menu.addSeparator();
    QAction *Inverse = menu.addAction(tr("反选"));
    menu.addSeparator();
    QAction *AllNotSel = menu.addAction(tr("全不选中"));

    connect(selectAll, &QAction::triggered, [=](){
       selectAllItem(true);
    });

    connect(Inverse, &QAction::triggered, [=](){
        selectInverseItem();
    });

    connect(AllNotSel, &QAction::triggered, [=](){
        selectAllItem(false);
    });
    menu.exec(QCursor::pos());
}

void Height_Data::selectAllItem(const bool bselAll)
{
    const int totalItem = ui->Sample_Data_tablewidget->rowCount();
    for (int irows = 0; irows < totalItem; ++irows) {
        if (QCheckBox *pcheckbox = rowSelectCheckBox(irows)) {
            pcheckbox->setChecked(bselAll);
        }
    }
}

void Height_Data::selectInverseItem()
{
    const int totalItem = ui->Sample_Data_tablewidget->rowCount();
    for (int irows = 0; irows < totalItem; ++irows) {
        if (QCheckBox *pcheckbox = rowSelectCheckBox(irows)) {
            pcheckbox->setChecked(!pcheckbox->isChecked());
        }
    }
}

QString Height_Data::generateSampleId(){
    QString todayLast = GlobalData::ObatinCreatSampleTime();

    int maxSeqInTable = getMaxSequenceFromTable(todayLast);
    int maxSeqInDB = FullyAutomatedPlatelets::pinstancesqlData()->getMaxSampleID(todayLast);
    int maxSeq = qMax(maxSeqInTable, maxSeqInDB);

    return GlobalData::groupDateAndID(todayLast, maxSeq + 1);
}

int Height_Data::getMaxSequenceFromTable(const QString& date) {
    int maxSeq = 0;
    int rowCount = ui->Sample_Data_tablewidget->rowCount();

    for(int row = 0; row < rowCount; row++) {
        auto* item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
        if(item && !item->text().isEmpty()) {
            QString itemDate;
            int seqNum = 0;
            GlobalData::apartSampleId(item->text(), itemDate, seqNum);
            if(itemDate == date && seqNum > maxSeq) {
                maxSeq = seqNum;
            }
        }
    }
    return maxSeq;
}

int Height_Data::addOneTestSample(const bool isWholeBloodMode, double testHeight,
                                   const QStringList &availableHoles,
                                   const QString &barcode)
{
    if (!ui || !ui->Sample_Data_tablewidget) {
        QLOG_ERROR() << "addOneTestSample: UI组件未初始化";
        return -1;
    }

    if (availableHoles.isEmpty()) {
        QLOG_ERROR() << "addOneTestSample: 可用孔位列表为空";
        return -1;
    }

    QTableWidget *sampleTable = ui->Sample_Data_tablewidget;
    int currentRow = sampleTable->rowCount();

    try{
        sampleTable->insertRow(currentRow);

        // 添加垂直表头项（这是关键！）
        QTableWidgetItem* headerItem = new QTableWidgetItem(QString::number(currentRow + 1));
        sampleTable->setVerticalHeaderItem(currentRow, headerItem);

        if (!addCheckBoxToRow(sampleTable, currentRow)) {
            throw std::runtime_error("添加复选框失败");
        }

        QString sampleId = generateSampleId();
        if (!addSampleIdToRow(sampleTable, currentRow, sampleId, isWholeBloodMode)) {
            throw std::runtime_error("添加样本号失败");
        }

        double heightValue = calculateHeightValue(isWholeBloodMode, testHeight);
        if (!addHeightValueToRow(sampleTable, currentRow, heightValue, isWholeBloodMode)) {
            throw std::runtime_error("添加测高值失败");
        }

        int selectedHole = selectDefaultHole(availableHoles);
        if (!addHoleSelectorToRow(sampleTable, currentRow, sampleId,
                                   availableHoles, selectedHole)) {
            throw std::runtime_error("添加孔号选择器失败");
        }

        if (!addBarcodeToRow(sampleTable, currentRow, barcode)) {
            throw std::runtime_error("添加条码失败");
        }

        recordSampleData(sampleId, selectedHole);

        //sampleTable->resizeRowToContents(currentRow);
        sampleTable->scrollToBottom();

        QLOG_DEBUG() << "成功添加样本:" << sampleId << "孔号:" << selectedHole;

    } catch (const std::exception &e) {
        QLOG_ERROR() << "addOneTestSample 异常:" << e.what();
        if (currentRow < sampleTable->rowCount()) {
            sampleTable->removeRow(currentRow);
        }
        return -1;
    }
    return currentRow;
}

bool Height_Data::addCheckBoxToRow(QTableWidget *table, int row)
{
    if (!table || row < 0) return false;

    QWidget *container = new QWidget(table);
    container->setStyleSheet("background: transparent;");
    QHBoxLayout *layout = new QHBoxLayout(container);

    QCheckBox *checkBox = new QCheckBox(container);
    checkBox->setChecked(true);

    static const QString styleSheet =
        "QCheckBox::indicator {"
        "   width: 20px;"
        "   height: 20px;"
        "   border-radius: 4px;"
        "   border: 2px solid #7FB3D5;"
        "   background-color: transparent;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "   background-color: transparent;"
        "}"
        "QCheckBox::indicator:checked {"
        "   background-color: #2E86C1;"
        "   border: 2px solid #2E86C1;"
        "   image: none;"
        "}"
        "QCheckBox::indicator:checked::after {"
        "   content: \"✓\";"
        "   color: white;"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   position: relative;"
        "   left: 2px;"
        "   top: -1px;"
        "}"
        "QCheckBox::indicator:hover {"
        "   border-color: #2E86C1;"
        "   background-color: rgba(46, 134, 193, 0.2);;"
        "}"
        "QCheckBox::indicator:checked:hover {"
        "   background-color: #1B6D9E;"
        "   border-color: #1B6D9E;"
        "}";
    checkBox->setStyleSheet(styleSheet);

    layout->addWidget(checkBox);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setAlignment(checkBox, Qt::AlignCenter);

    table->setCellWidget(row, CHECK_ROW, container);
    return true;
}

bool Height_Data::addSampleIdToRow(QTableWidget *table, int row,
                                    const QString &sampleId, bool isWholeBloodMode)
{
    if (!table || row < 0 || sampleId.isEmpty()) return false;

    QTableWidgetItem *item = new QTableWidgetItem(sampleId);
    item->setTextAlignment(Qt::AlignCenter);

    QColor backgroundColor = isWholeBloodMode ? WHOLEBLOOD_BG_COLOR : PLASMA_BG_COLOR;
    item->setBackground(backgroundColor);

    table->setItem(row, SAMLPE_NAME, item);
    if (m_checkBoxEmergency && m_checkBoxEmergency->isChecked()) {
        setRowEmergency(row, true);
    }
    return true;
}

bool Height_Data::addHeightValueToRow(QTableWidget *table, int row,
                                       double heightValue, bool isInvalid)
{
    if (!table || row < 0) return false;
    Q_UNUSED(isInvalid);

    QString displayText = QString::number(heightValue, 'f', 2);
    QTableWidgetItem *item = new QTableWidgetItem(displayText);
    item->setTextAlignment(Qt::AlignCenter);
    table->setItem(row, HEIGHT_DATA, item);
    return true;
}

double Height_Data::calculateHeightValue(bool isWholeBloodMode, double testHeight) const
{
    if (isWholeBloodMode) {
        QLOG_DEBUG()<<"全血模式高度["<<testHeight<<"]";
        return testHeight;
    }
    return INI_File().GetAbsorbTubeBottom();
}

bool Height_Data::addBarcodeToRow(QTableWidget *table, int row, const QString &barcode)
{
    if (!table || row < 0) return false;

    QTableWidgetItem *item = new QTableWidgetItem(barcode);
    item->setTextAlignment(Qt::AlignCenter);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

    table->setItem(row, BARCODE, item);
    return true;
}

void Height_Data::recordSampleData(const QString &sampleId, int holeNumber)
{
    if (sampleId.isEmpty() || holeNumber <= 0) return;

    QString createTime = QDateTime::currentDateTime().toString("MM-dd hh:mm:ss");
    m_sampleCreateTime.insert(sampleId, createTime);
}

int Height_Data::selectDefaultHole(const QStringList &availableHoles) const
{
    if (availableHoles.isEmpty()) return -1;

    QSet<int> usedHoles;
    const int rowCount = ui->Sample_Data_tablewidget->rowCount();
    for (int row = 0; row < rowCount; ++row) {
        QWidget* widget = ui->Sample_Data_tablewidget->cellWidget(row, RICHBLOOD_HOLE);
        if (widget) {
            QComboBox* combox = qobject_cast<QComboBox*>(widget);
            if (combox && !combox->currentText().isEmpty()) {
                usedHoles.insert(combox->currentText().toInt());
            }
        }
    }

    QList<int> availableInts;
    for (const QString& holeStr : availableHoles) {
        int hole = holeStr.toInt();
        if (!usedHoles.contains(hole)) {
            availableInts.append(hole);
        }
    }

    if (availableInts.isEmpty()) return -1;

    std::sort(availableInts.begin(), availableInts.end());
    return availableInts.first();
}

bool Height_Data::addHoleSelectorToRow(QTableWidget *table, int row,
                                        const QString &sampleId,
                                        const QStringList &availableHoles,
                                        int defaultHole)
{
    if (!table || row < 0 || sampleId.isEmpty() || availableHoles.isEmpty()) {
        return false;
    }

    QComboBox *holeSelector = new QComboBox(table);
    holeSelector->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // 可选：设置最小高度，避免过小
    holeSelector->setMinimumHeight(30);
    holeSelector->setStyleSheet(getMedicalComboxStyle());


    QStyledItemDelegate *delegate = new QStyledItemDelegate(holeSelector);
    holeSelector->setItemDelegate(delegate);
    holeSelector->installEventFilter(this);

    QStringList sortedHoles = availableHoles;
    std::sort(sortedHoles.begin(), sortedHoles.end(),
              [](const QString &a, const QString &b) {
                  return a.toInt() < b.toInt();
              });

    if (!sortedHoles.isEmpty()) {
        QString firstHole = sortedHoles.first();  // 只获取，不移除
        Q_EMIT reminderPPPandPRPputHole(firstHole.toInt());
        holeSelector->addItems(sortedHoles);  // 添加完整的列表
    }

    QString defaultHoleStr = QString::number(defaultHole);
    if (sortedHoles.contains(defaultHoleStr)) {
        holeSelector->blockSignals(true);
        holeSelector->setCurrentText(defaultHoleStr);
        holeSelector->blockSignals(false);
        m_pressedhole[defaultHoleStr] = true;
    } else if (!sortedHoles.isEmpty()) {
        holeSelector->setCurrentIndex(0);
        QString firstHole = sortedHoles.first();
        m_pressedhole[firstHole] = true;
        defaultHole = firstHole.toInt();
    }

    connect(holeSelector,
            static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
            this, &Height_Data::selectPPPholeChange);

    table->setCellWidget(row, RICHBLOOD_HOLE, holeSelector);
    //table->resizeRowToContents(row);
    table->scrollToBottom();

    m_selbloodholetemp.insert(sampleId, defaultHole);

    return true;
}

int Height_Data::Addtasksmanually()
{
    if (!ui || !ui->Sample_Data_tablewidget) {
        QLOG_ERROR() << "AddTasksManually: UI组件未初始化";
        return -1;
    }

    QStringList availableHoleList;

    if (!hasAvailableSampleHole(availableHoleList)) {
        CustomMessageBox::warning(this,tr("添加样本失败"),"血样孔已占满，无可用加样位置!");
        return -1;
    }

    bool isWholeBloodMode = INI_File().GetWholeBloodModel();
    if (isWholeBloodMode) {
        CustomMessageBox::warning(this,tr("添加样本失败"),"手动加样请先切换到血浆模式!");
        return -1;
    }

    return addOneTestSample(false, 0.0, availableHoleList, QString("null"));
}

void Height_Data::handleEmptySelection()
{
    Q_EMIT ReminderTextOut(PROMPTLOG, tr("保存添加测试样本为空"));

    close();
}

bool Height_Data::processSelectedSamples(const QList<int>& selectedRows)
{
    try {
        sycnstudata(selectedRows, ui->Sample_Data_tablewidget);
        return true;
    } catch (const std::exception& e) {
        QLOG_ERROR() << "Failed to process samples:" << e.what();
        CustomMessageBox::warning(this,tr("处理失败"),"样本数据处理过程中发生错误");
        return false;
    }
}

void Height_Data::cleanupAndClose()
{
    DeleteAllItems(ui->Sample_Data_tablewidget);
    m_sampleCreateTime.clear();
    close();
}


void Height_Data::savewaitTestSample()
{
    QList<int> selectedRows;
    obtainSelectedSample(selectedRows);

    if (selectedRows.isEmpty() || ui->Sample_Data_tablewidget->rowCount() == 0) {
        handleEmptySelection();
        return ;
    }

    // ========== 新增：模式一致性校验 ==========
    bool currentWholeBloodMode = INI_File().GetWholeBloodModel();
    for (int row : selectedRows) {
        QTableWidgetItem* sampleItem = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
        if (!sampleItem) continue;

        QColor bgColor = sampleItem->background().color();
        bool isTaskWholeBlood = (bgColor == WHOLEBLOOD_BG_COLOR);

        // 任务模式与当前模式不匹配
        if (isTaskWholeBlood != currentWholeBloodMode) {
            QString msg;
            if (currentWholeBloodMode) {
                msg = tr("当前为全血模式，但选中的任务包含血浆模式下添加的样本。\n请切换回血浆模式后再保存，或删除不匹配的任务。");
            } else {
                msg = tr("当前为血浆模式，但选中的任务包含全血模式下添加的样本。\n请切换回全血模式后再保存，或删除不匹配的任务。");
            }
            CustomMessageBox::warning(this, tr("保存任务失败"), msg);
            return;
        }
    }
    // ========== 校验结束 ==========



    if(!validateTableItems(ui->Sample_Data_tablewidget,selectedRows)){
        CustomMessageBox::warning(this,tr("添加测试样本失败"),"请检查样本配置信息!");
        return;
    }

    if (TheSameBloodHole()){
        return;
    }

    if (SameSampleandtube(selectedRows,ui->Sample_Data_tablewidget))
        return;

    int needtube = 0;
    quint8 remainder_tube =  FullyAutomatedPlatelets::pinstancesqlData()->BackEmptyTubeNum();
    bool  enoughTube = NeedTubeEnouthTesting(remainder_tube,needtube,selectedRows);
    if(!enoughTube)
    {
        CustomMessageBox::warning(this,tr("添加测试样本失败"),"剩余试管空不足!");
        return;
    }

    bool dimmingLED;
    if (auto *dim = FullyAutomatedPlatelets::pinstancedimming()) {
        dim->getdimming_status(dimmingLED);
    } else {
        dimmingLED = false;
    }
    if(!dimmingLED){
        emit ReminderTextOut(PROMPTLOG,tr("调光未完成,稍后添加!"));
        CustomMessageBox::warning(this,tr("添加测试样本失败"),tr("调光未完成,稍后添加!"));
        return;
    }

    if (!validateSelectedRowsForSave(selectedRows)) {
        return;
    }

    processSelectedSamples(selectedRows);

    QTableWidget *table = ui->Sample_Data_tablewidget;
    if (table) {
        for (int row = 0; row < table->rowCount(); ++row) {
            if (selectedRows.contains(row)) {
                continue;
            }
            QWidget *widget = table->cellWidget(row, RICHBLOOD_HOLE);
            auto *box = qobject_cast<QComboBox *>(widget);
            if (!box) {
                continue;
            }
            const quint8 hole = static_cast<quint8>(box->currentText().toInt());
            if (hole > 0) {
                Makeuptubenum(hole);
                Q_EMIT reminderPPPandPRPputHoleNotify(hole, 255);
            }
        }
    }

    cleanupAndClose();

    return;
}

void Height_Data::updateaddprogress(int index, int total)
{
    if (auto* mainWindow = FullyAutomatedPlatelets::mainWindow()) {
        mainWindow->progressBarconfig(index,total);
    }
}

void Height_Data::slotupdatetestui(QList<quint8> marktube, QString sample_name,
                                    quint8 anemiahole,int index_add, int all_add_task)
{
    QLOG_DEBUG()<<"标记试管["<<marktube<<"]";
    QString date;
    int id;
    GlobalData::apartSampleId(sample_name,date,id);
    emit updateTestTubeSatus(QString::number(id),anemiahole,marktube,index_add,all_add_task);
}

void Height_Data::sycnstudata(QList<int> TaskList,QTableWidget *TaskWidget)
{
    if (!TaskWidget || TaskList.isEmpty()) return;

    const int totalTasks = TaskList.size();
    const QString savedtime = QDateTime::currentDateTime().toString("MM.dd hh:mm:ss");

    for (int selRows : TaskList)
    {
        if (selRows < 0 || selRows >= TaskWidget->rowCount()) continue;

        QTableWidgetItem* sampleItem = TaskWidget->item(selRows, SAMLPE_NAME);
        QTableWidgetItem* barcodeItem = TaskWidget->item(selRows, BARCODE);
        QTableWidgetItem* heightItem = TaskWidget->item(selRows, HEIGHT_DATA);
        QTableWidgetItem* projectItem = TaskWidget->item(selRows, PROJECT_ITEM);

        if (!sampleItem || !barcodeItem || !heightItem || !projectItem) continue;

        bool insertWholeBloodMode = (sampleItem->background().color() == WHOLEBLOOD_BG_COLOR);

        int CurrRichHole = 0;
        QWidget* widget = TaskWidget->cellWidget(selRows, RICHBLOOD_HOLE);
        if (widget) {
            QComboBox* combox = qobject_cast<QComboBox*>(widget);
            if (combox) CurrRichHole = combox->currentText().toInt();
        }

        double needleDown = 0.0f;
        if(INI_File().GetWholeBloodModel()){
            const double wholeBloodmamode  = heightItem->text().toDouble();
            needleDown = switchWholeModeHeight(wholeBloodmamode);
			if (needleDown <= 0) {
				CustomMessageBox::warning(this, tr("添加测试样本失败"), "下针吸PRP高度异常请");
				return;
			}
            QLOG_DEBUG() << "全血模式，红细胞层高度:" << wholeBloodmamode<<"mm"<<"下针高度"<<needleDown<<"mm";
        }else{
            double Plasmamode  = heightItem->text().toDouble();
            needleDown = Plasmamode;
            QLOG_DEBUG() << "血浆模式，下针高度:" << needleDown;
        }

        emit sycnwaittestsampledata(
            sampleItem->text(),
            savedtime,
            barcodeItem->text(),
            needleDown,
            projectItem->text(),
            CurrRichHole,
            totalTasks,
            insertWholeBloodMode,
            isRowEmergency(selRows)
            );
    }
}

void Height_Data::deleteSelectedSample()
{
    QList<int> selectedRows;
    obtainSelectedSample(selectedRows);

   if (selectedRows.isEmpty()) {
       CustomMessageBox::warning(this,tr("删除任务失败"),tr("请选择要删除任务!"));
       return;
   }

   std::sort(selectedRows.begin(), selectedRows.end(), std::greater<int>());

   for (int row : selectedRows) {
       QString sampleName;
       if (auto* item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME)) {
           sampleName = item->text();
       }

       quint8 currentHole = 0;
       if (QWidget* widget = ui->Sample_Data_tablewidget->cellWidget(row, RICHBLOOD_HOLE)) {
           if (auto* combobox = qobject_cast<QComboBox*>(widget)) {
               currentHole = combobox->currentText().toInt();
               Makeuptubenum(currentHole);
               Q_EMIT reminderPPPandPRPputHoleNotify(currentHole, 255);

               // 关键修复：将释放的孔号添加回其他行的下拉列表
              if (currentHole > 0) {
                  updateotherinserthole(row, QString::number(currentHole));
              }
           }
       }

       ui->Sample_Data_tablewidget->removeRow(row);
   }

   // 删除后重新编号
    updateVerticalHeaderRowNumbers();
}

void Height_Data::batchAddTask()
{
    if(INI_File().GetWholeBloodModel()){
        CustomMessageBox::warning(this,tr("批量添加失败"),tr("请切换血浆模式添加"));
        return;
    }

    if(m_batchaddTestnumSample == nullptr){
        m_batchaddTestnumSample = new batchAddSample;
        qRegisterMetaType<QVariant>("QVariant");

        connect(m_batchaddTestnumSample.data(),
                SIGNAL(send_askData(QVariant)),
                this,SLOT(AF_DATA_REQUEST(QVariant)));

        connect(m_batchaddTestnumSample.data(),&batchAddSample::exitWidget,this,[=](){
          delete m_batchaddTestnumSample;
        });
    }
    m_batchaddTestnumSample.data()->obtainFirstSampleId(ui->Sample_Data_tablewidget);
    m_batchaddTestnumSample.data()->obtainSelectTestProject();

    if (m_batchaddTestnumSample->isMinimized()) {
        m_batchaddTestnumSample->showNormal();
        m_batchaddTestnumSample->raise();
        m_batchaddTestnumSample->activateWindow();
    }
    m_batchaddTestnumSample.data()->show();
}

void Height_Data::AF_DATA_REQUEST(QVariant sampleiddata)
{
     SAMPLEIDINFO askData;
     askData = sampleiddata.value<SAMPLEIDINFO>();
     QString  todayLast = GlobalData::ObatinCreatSampleTime();
     QString  addSampleid = GlobalData::groupDateAndID(todayLast,askData.idsample.toInt());
     int insertRows = Addtasksmanually();
     if(insertRows >= 0)
     {
         SetColumnText(insertRows,SAMLPE_NAME,addSampleid);
         SetColumnText(insertRows,HEIGHT_DATA,askData.pindownHeight);
         SetColumnText(insertRows,PROJECT_ITEM,askData.testProtect);
     }
     return;
}

void Height_Data::onImageoutResult(const QString redBloodCellHeigh){
    double outTestResult = redBloodCellHeigh.toDouble();
    const double wholeBloodHeigh = round(outTestResult * 100) / 100;

    if (m_isreplaceopencv) {
       updateTableItem(m_repTestOpencvId, wholeBloodHeigh);
       return;
    }

    handleSampleAddition(wholeBloodHeigh);
    return;
}

void Height_Data::opencvFindImageLine(const QString &pathImage){
    if(m_testFindPrpHeigh){
        m_isreplaceopencv = false;
        m_testFindPrpHeigh->handleSycnOpendcvImage(pathImage);
    }
}

void Height_Data::reTestOpencvId(const QString& reIdsample,const QString &pathImage){
    m_repTestOpencvId = reIdsample;
    if(m_testFindPrpHeigh){
        m_isreplaceopencv = true;
        m_testFindPrpHeigh->handleSycnOpendcvImage(pathImage);
    }
}

TestOpcv *Height_Data::vision() const
{
    return m_testFindPrpHeigh;
}

void Height_Data::initLoadOpencvTestImag(){
    cleanupThread();

    m_testFindPrpHeigh = new TestOpcv();
    ui->widget_ShowErrImage->layout()->addWidget(m_testFindPrpHeigh);

    connect(m_testFindPrpHeigh, &TestOpcv::imageoutResult, this,
            &Height_Data::onImageoutResult);
    connect(m_testFindPrpHeigh, &TestOpcv::beginNewMeasure, this, [this]() {
        m_isreplaceopencv = false;
    });
    connect(m_testFindPrpHeigh, &TestOpcv::replaceSampleId, this, [this](const QString &id) {
        m_repTestOpencvId = id;
        m_isreplaceopencv = true;
    });

    const bool prpMode = INI_File().GetWholeBloodModel();
    (prpMode)? ui->widget_ShowErrImage->show() : ui->widget_ShowErrImage->hide();
}

void Height_Data::cleanupThread()
{
    if (m_testFindPrpHeigh) {
        if (ui && ui->widget_ShowErrImage && ui->widget_ShowErrImage->layout()) {
            ui->widget_ShowErrImage->layout()->removeWidget(m_testFindPrpHeigh);
        }
        delete m_testFindPrpHeigh;
        m_testFindPrpHeigh = nullptr;
    }
}

double Height_Data::switchWholeModeHeight(const double redBloodHeigh)
{
    auto &ini = INI_File();
    double referenceToBottomDistance = ini.GetFixedHigh();
    double safetyMargin = ini.GetTestDifference();
    double rotb  =   ini.getRefBottomDistance();
    return referenceToBottomDistance + rotb - redBloodHeigh - safetyMargin + RAISETHERULER;
}

//=============================================================================
// 自定义标题栏相关函数
//=============================================================================

void Height_Data::initCustomTitleBar()
{
    // 获取现有的顶层布局（UI文件中设置的布局）
    QLayout *existingLayout = layout();
    if (!existingLayout) {
        // 如果没有布局，则创建垂直布局，并将现有子控件添加进去（保持原有顺序）
        QVBoxLayout *newLayout = new QVBoxLayout(this);
        newLayout->setContentsMargins(0, 0, 0, 0);
        newLayout->setSpacing(0);

        // 获取所有直接子控件（按z-order排序，尽可能保持原有视觉顺序）
        QList<QWidget*> children = this->findChildren<QWidget*>();
        // 简单按tab顺序或几何位置排序，这里假设子控件顺序大致正确
        for (QWidget* child : children) {
            if (child->parent() == this && !qobject_cast<CustomTitleBar*>(child)) {
                newLayout->addWidget(child);
            }
        }
        existingLayout = newLayout;
    }

    // 创建标题栏
    m_titleBar = new CustomTitleBar(this);
    m_titleBar->setTitle(windowTitle());

    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &Height_Data::onCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &Height_Data::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &Height_Data::onMaximizeRequested);

    // 将标题栏插入到布局的最前面（索引0）
    // 注意：QLayout::insertWidget 需要在布局中添加，但标题栏的父对象必须是 this
    // 如果 existingLayout 是 QBoxLayout（如 QVBoxLayout），可以直接 insertWidget(0, m_titleBar)
    if (QBoxLayout *boxLayout = qobject_cast<QBoxLayout*>(existingLayout)) {
        boxLayout->insertWidget(0, m_titleBar);
    } else {
        // 非 QBoxLayout 的情况（罕见），先删除原布局，再创建新布局
        QWidget().setLayout(existingLayout); // 临时转移所有权
        QVBoxLayout *newLayout = new QVBoxLayout(this);
        newLayout->addWidget(m_titleBar);
        // 将原布局中的所有控件转移过来
        while (QLayoutItem *item = existingLayout->takeAt(0)) {
            if (item->widget()) {
                newLayout->addWidget(item->widget());
            }
            delete item;
        }
        delete existingLayout;
        existingLayout = newLayout;
    }

    // 设置窗口边距，让内容区域不紧贴标题栏
    existingLayout->setContentsMargins(0, 0, 0, 0);
    existingLayout->setSpacing(0);
}

void Height_Data::onCloseRequested()
{
    close();
}

void Height_Data::onMinimizeRequested()
{
     showMinimized();
}

void Height_Data::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        m_titleBar->updateMaximizeButton(false);
    } else {
        showMaximized();
        m_titleBar->updateMaximizeButton(true);
    }
}

void Height_Data::setWindowTitle(const QString &title)
{
    QWidget::setWindowTitle(title);
    if (m_titleBar) {
        m_titleBar->setTitle(title);
    }
}

void Height_Data::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange) {
        if (m_titleBar) {
            m_titleBar->setActive(isActiveWindow());
        }
        static const QString activeSs = getMedicalTableStyle();
        static const QString inactiveSs = activeSs
                + QStringLiteral("QTableWidget::item:selected { background-color: #B8DFF7; }");
        ui->Sample_Data_tablewidget->setStyleSheet(isActiveWindow() ? activeSs : inactiveSs);
    }
    QWidget::changeEvent(event);
}

void Height_Data::showCorrectDialog(unsigned int rows, int column, const QString& changeData)
{
    // 如果已有对话框，先关闭并释放
    if (m_correctdata) {
        m_correctdata->close();
        delete m_correctdata;
    }

    // 创建新对话框，传递 this 作为 parent 确保窗口层级正确
    m_correctdata = new Correct_Data(nullptr);
    if (!m_correctdata) {
        QLOG_ERROR() << "Failed to create Correct_Data dialog";
        return;
    }

    m_correctdata->ClickSamplename(changeData);
    m_correctdata->setrows(rows);
    m_correctdata->setcols(column);
    m_correctdata->setorigindata(m_OriginallyValue);

    // 连接信号
    connect(m_correctdata, &Correct_Data::ConfigureData,
            this, &Height_Data::onDataConfigured);

    //重新测试高度
    connect(m_correctdata,&Correct_Data::repTestHeight,
            this,[=](const QString sampleid){
        if (auto* testing = FullyAutomatedPlatelets::pinstanceTesting()) {
            testing->repPrpheight(sampleid, true);
        }
    });

    //修改测高值
    connect(m_correctdata, &Correct_Data::NotifyTestHeight,
            this,&Height_Data::onNotifyTestHeight);

    // 显示对话框
    m_correctdata->setAttribute(Qt::WA_DeleteOnClose, true);
    m_correctdata->show();
}

void Height_Data::onDataConfigured(unsigned int row, int col, const QString& data,const bool isfollowUp)
{
    if (data.isEmpty()) {
        CustomMessageBox::warning(this,tr("修改样本id失败"),"样本号不能为空!");
        restoreOriginalValue(row, col);
        return;
    }

    if (!validateSampleUniqueness(data,isfollowUp)) {
        restoreOriginalValue(row, col);
        return;
    }

    updateCreationTimeMapping(row,data);

    //复查样本号
    if(isfollowUp){
        auto *paddTaskCol = FullyAutomatedPlatelets::pinstancepatientdata();
		if (paddTaskCol) {
			paddTaskCol->addfollowUpSample(data);
		}
        setRowFollowUp(row, true);
        CustomMessageBox::information(this, tr("已标记复查"),
            tr("该样本将按复查入库（样本号后加F）。\n接下来请只勾选需要重测的试剂，再点确定加入任务。"));
        handleProjectItemClick(row, PROJECT_ITEM);
    }

    SetColumnText(row, col, data);
    if (isfollowUp) {
        setRowFollowUp(row, true);
    }
}

