#pragma execution_character_set("utf-8")

#include "height_data.h"
#include "ui_height_data.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDateTime>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QStyledItemDelegate>
#include <QTextStream>
#include <QTextCodec>
#include <string>
#include <QFile>
#include <QDir>
#include "crc/include/vld.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QtConcurrent>
#include <QFuture>
#include "globaldata.h"
#include "loginui.h"
#include "testing.h"
#include <operclass/fullyautomatedplatelets.h>
#include <QMetaType>

#define WHOLEBLOODMODE  QColor(160, 82, 45) //全血模式



Height_Data::Height_Data(QWidget *parent) : QWidget(parent),
    ui(new Ui::Height_Data)
   ,m_Barcodestr("")
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint& ~Qt::WindowMinimizeButtonHint | Qt::WindowStaysOnTopHint
                         | Qt::WindowDoesNotAcceptFocus);
    this->setWindowTitle(tr("任务列表"));
    this->setWindowIcon(QIcon(":/Picture/suowei.png"));
    this->setMinimumWidth(800);
    this->setMinimumHeight(440);
    ui->widget_ShowErrImage->show();
    m_selbloodholetemp.clear();


    //测高模式
    mtestmodebox = new QButtonGroup;
    mtestmodebox->setExclusive(true); //设置是否互斥
    mtestmodebox->addButton(ui->checkBox_wholeblood,0);
    mtestmodebox->addButton(ui->checkBox_plasma,1);
    bool wholeblood_ = INI_File().GetWholeBloodModel();
    (wholeblood_ == true)? ui->checkBox_wholeblood->setChecked(true): ui->checkBox_plasma->setChecked(true);
    connect(mtestmodebox,SIGNAL(buttonClicked(int)),this,SLOT(_clickBloodmode(int)));

    mCreatTime.clear();

    InitTablewidget();

    initshowimg();

    ui->Sample_Data_tablewidget->setContextMenuPolicy(Qt::CustomContextMenu);


    GlobalData::QCheckboxSheet(ui->checkBox_wholeblood,tr("全血模式"));
    GlobalData::QCheckboxSheet(ui->checkBox_plasma,tr("血浆模式"));


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

    if(!m_correctdata)
        m_correctdata =  new Correct_Data(0, 0 ,"");
    connect(m_correctdata,&Correct_Data::ConfigureData,
            this,&Height_Data::Slot_ConfigureData);

    //重新测试高度
    connect(m_correctdata,&Correct_Data::repTestHeight,this,[=](const QString sampleid){
        FullyAutomatedPlatelets::pinstanceTesting()->repPrpheight(sampleid,true);
    });
    connect(m_correctdata, &Correct_Data::NotifyTestHeight,
            this,&Height_Data::SlotNotifyTestHeight);//修改测高值




}

Height_Data::~Height_Data()
{
    QLOG_DEBUG()<<__FUNCTION__<<__LINE__<<"析构加样本对象"<<endl;

    if(mtestmodebox)
    {
        delete mtestmodebox;
        mtestmodebox = nullptr;
    }

    delete m_correctdata;
    m_correctdata = nullptr;



    delete m_threadaddsample;
    m_threadaddsample = nullptr;

    delete ui;
}

bool Height_Data::eventFilter(QObject *obj, QEvent *event)
{
    //屏蔽 spinbox 和 combobox 的滚轮事件
     if(obj->inherits("QAbstractSpinBox") || obj->inherits("QComboBox")) {
            if(event->type() == QEvent::Wheel)
                return true;
    }
    return false;
}

void  Height_Data::_clickBloodmode(int clickid_)
{
    bool wholeBloodmodel = false;
    QCheckBox *btn = qobject_cast<QCheckBox*> (mtestmodebox->button(clickid_));
    btn->setChecked(true);
    (clickid_ == 0)? wholeBloodmodel = true : wholeBloodmodel = false;
    INI_File().SetWholeBloodModel(wholeBloodmodel);
    return;
}

void  Height_Data::_initcreat()
{
    _initNumAnaemiaHole();

    FullyAutomatedPlatelets::pinstanceTestproject()->_initstyle();

    QObject::connect(FullyAutomatedPlatelets::pinstanceTestproject(),
                     &ConfigureProjectItem::_setallsampletestproject,this,[=](QString text){
        QList<int> item_List;
        _obtainSelectedSample(item_List);
        for(int i = 0 ; i< item_List.size() ;i++){
            int rows = item_List.at(i);
            SetColumnText(rows,PROJECT_ITEM,text);
        }
    });

    QObject::connect(FullyAutomatedPlatelets::pinstanceTestproject(),&ConfigureProjectItem::_setsinglesampleproject,
                     this,[=](int rows,int clos,QString itemtext){
       SetColumnText(rows,clos,itemtext);
    });

    QObject::connect(ui->AddTask,&QToolButton::clicked,this,[=]()
    {
       _Addtasksmanually(); //手动按钮添加单个任务
    });

    QObject::connect(ui->toolButton_ok,&QToolButton::clicked,this,[=]()
    {
        _savewaitTestSample(); //保存任务
    });

    QObject::connect(ui->toolButton_Delete_sel_item,&QToolButton::clicked,this,[=]()
    {
        _deleteSelectedSample(); //删除选中待测样本
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

        connect(this,&Height_Data::_sycnwaittestsampledata,
                m_threadaddsample,&mythreadaddsample::_waittestsampledata);

        connect(m_threadaddsample,&mythreadaddsample::_addprogress,
                this,&Height_Data::_updateaddprogress);

        connect(m_threadaddsample,&mythreadaddsample::_updatetestui,
                this,&Height_Data::_slotupdatetestui);

        connect(m_threadaddsample,&mythreadaddsample::_sycnPaintentInfo,
                FullyAutomatedPlatelets::mainWindow(),&MainWindow::_slotsycnPaintentInfo);

        m_threadaddsample->Start();
    }

}


void  Height_Data::_initNumAnaemiaHole()
{
    m_pressedhole.clear();
    quint8 totalHole_ = 0;
    quint8 kindEquipment_= 0;
    SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT,kindEquipment_);
    switch(kindEquipment_){
    case    KS600:  totalHole_ = 30; break;
    case    KS800:  totalHole_ = 45; break;
    case    KS1200: totalHole_ = 60; break;
    default: totalHole_ = 60; break;
    }
    for(int n = 0 ; n < totalHole_; n++)
        m_pressedhole.insert(QString::number(2 * n + 1), false);

    m_selbloodholetemp.clear();
    return;
}

//补回用掉的试管孔
void Height_Data::Makeuptubenum(quint8 canceltube)
{
    if(canceltube %2 == 0) return;

    // 仅处理奇数孔位
    if (Q_UNLIKELY(canceltube % 2 == 0)) {
        return;
    }

    auto tubeIt = m_pressedhole.find(QString::number(canceltube));
    if(tubeIt != m_pressedhole.end()){
        QLOG_INFO() << "Recovering tube hole:" << canceltube;
        tubeIt.value() = false; // 标记为可用

        //临时用的孔号清除
        auto it = m_selbloodholetemp.begin();
        while(it != m_selbloodholetemp.end()){
            if(it.value() == canceltube){
               m_selbloodholetemp.erase(it);
               break;
            }
            it++;
        }
    }
}




//测试界面取消 补回血样孔
void Height_Data::_backcancelhole(const quint8 _index)
{
    QString backhole_ = QString::number(_index);
    if(m_pressedhole.contains(backhole_))
    {
        auto iter = m_pressedhole.find(backhole_);
        QLOG_DEBUG()<<"测试界面删除样本孔号补回"<<backhole_;
        iter.value() = false;
        auto it = m_selbloodholetemp.begin();
        while(it != m_selbloodholetemp.end()){
            if(it.value() == _index){
                m_selbloodholetemp.erase(it);
                break;
             }
            it++;
       }
    }
    return;
}


void Height_Data::initshowimg()
{
    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::white);
    sample_palette.setColor(QPalette::WindowText, QColor(28, 134 ,238));
    ui->widget_ShowErrImage->setAutoFillBackground(true);
    ui->widget_ShowErrImage->setPalette(sample_palette);

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    ui->Imageshow->setPalette(pe);
    QFont font;
    font.setFamily("楷体");//字体
    font.setPixelSize(25);
    ui->Imageshow->setFont(font);
    ui->Imageshow->setWordWrap(true);
    ui->Imageshow->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    QString text = "暂无图片";
    ui->Imageshow->setText(text.split("", QString::SkipEmptyParts).join("\n"));

}

void Height_Data::resizeEvent(QResizeEvent* event){
    QWidget::resizeEvent(event);
    if(ui->Imageshow->pixmap()) {
            ui->Imageshow->setPixmap(ui->Imageshow->pixmap()->scaled(
            ui->Imageshow->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));
    }
}
void Height_Data::closeEvent(QCloseEvent *event)
{
    emit Taskconfigcloe();
    ReminderPutBloodHole(-1);
    event->accept();
}


void Height_Data::keyPressEvent(QKeyEvent *scanData){
    if (!INI_File().rConfigPara(SCANCODEBAR).toBool()) {
        QLOG_DEBUG()<<"扫码功能未打开!";
        return;
    }

    if(scanData->key() != Qt::Key_Return){
        QString inputText  = scanData->text();
        if(!inputText.isEmpty()){
            m_Barcodestr += inputText;
            QLOG_DEBUG()<<"当前缓存:"<<m_Barcodestr;
        }
        return;
    }

    QString _output = QString("扫码QrCode结果:%1").arg(m_Barcodestr);
    QLOG_DEBUG()<<_output;

    const int rowCount = ui->Sample_Data_tablewidget->rowCount();
    if(rowCount >= 1){
        int rowIndex = rowCount - 1;
        QTableWidgetItem* itembarcode = ui->Sample_Data_tablewidget->item(rowIndex, BARCODE);
        if (!itembarcode) {
            itembarcode = new QTableWidgetItem();  // 创建新Item
            ui->Sample_Data_tablewidget->setItem(rowIndex, BARCODE, itembarcode);
        }
        // 设置文本及属性
        itembarcode->setText(m_Barcodestr);
        itembarcode->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable); // 按需选择标志位
        itembarcode->setTextAlignment(Qt::AlignCenter); // 合并水平和垂直居中
    }

    // 5. 增加空值检查和Trim处理 [[6]]
    if (m_Barcodestr.trimmed().isEmpty()) {
        emit ReminderTextOut(ERRORLOG, tr("扫码失败: 内容为空或仅含空白字符"));
        m_Barcodestr.clear(); // 清空无效输入
        return;
    }

    m_Barcodestr.clear();
    FullyAutomatedPlatelets::mainWindow()->TestHeightFinish(true);
}


void Height_Data::InitTablewidget()
{

    QUIUtils::ConfigMousingPicture(ui->widget);
    QUIUtils::ConfigMousingPicture(ui->widget_Task);
    QUIUtils::ConfigMousingPicture(ui->widget_ShowErrImage);

    QStringList header;
    header<<tr("测试选项")<<tr("样本号")<<tr("测高值")<<tr("血样孔号")<<tr("测试项目")<<tr("条形码");
    ui->Sample_Data_tablewidget->setColumnCount(6); //初始化列
    ui->Sample_Data_tablewidget->setHorizontalHeaderLabels(header);
    QFont font;
    font.setPixelSize(22);
    font.setFamily("楷体");
    font.setBold(true);

    ui->Sample_Data_tablewidget->horizontalHeader()->setFont(font);
    ui->Sample_Data_tablewidget->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    ui->Sample_Data_tablewidget->setFrameShape(QFrame::Box); //设置无边框
    ui->Sample_Data_tablewidget->setShowGrid(true); //设置不显示格子线
    ui->Sample_Data_tablewidget->verticalHeader()->setVisible(false); //设置垂直头不可见//隐藏行表头(行号)
    ui->Sample_Data_tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);  //可多选（Ctrl、Shift、  Ctrl+A都可以）
    ui->Sample_Data_tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //设置选择行为时每次选择一行
    ui->Sample_Data_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置不可编辑
    ui->Sample_Data_tablewidget->verticalHeader()->setResizeContentsPrecision(QHeaderView::Stretch);
    ui->Sample_Data_tablewidget->horizontalHeader()->setHighlightSections(false);
    ui->Sample_Data_tablewidget->setFocusPolicy(Qt::NoFocus);
    ui->Sample_Data_tablewidget->horizontalHeader()->setSectionResizeMode(CHECK_ROW, QHeaderView::ResizeToContents);
    ui->Sample_Data_tablewidget->horizontalHeader()->setSectionResizeMode(SAMLPE_NAME, QHeaderView::Stretch);
    ui->Sample_Data_tablewidget->horizontalHeader()->setSectionResizeMode(HEIGHT_DATA, QHeaderView::Stretch);
    ui->Sample_Data_tablewidget->horizontalHeader()->setSectionResizeMode(RICHBLOOD_HOLE, QHeaderView::Stretch);
    ui->Sample_Data_tablewidget->horizontalHeader()->setSectionResizeMode(PROJECT_ITEM, QHeaderView::Stretch);
    QHeaderView *header1 = ui->Sample_Data_tablewidget->verticalHeader();
    header1->setHidden(true);
    ui->Sample_Data_tablewidget->verticalHeader()->setDefaultSectionSize(40); //设置行高
    ui->Sample_Data_tablewidget->horizontalHeader()->setDefaultSectionSize(30);
    ui->Sample_Data_tablewidget->horizontalHeader()->resizeSection(CHECK_ROW,30); //设置表头第一列的宽度为
    ui->Sample_Data_tablewidget->horizontalHeader()->resizeSection(SAMLPE_NAME,250);
    ui->Sample_Data_tablewidget->horizontalHeader()->resizeSection(HEIGHT_DATA,200);
    ui->Sample_Data_tablewidget->horizontalHeader()->resizeSection(RICHBLOOD_HOLE,200);
    ui->Sample_Data_tablewidget->horizontalHeader()->resizeSection(PROJECT_ITEM,100); //设置表头第一列的宽度为
    ui->Sample_Data_tablewidget->horizontalHeader()->setFixedHeight(40); //设置表头的高度
    ui->Sample_Data_tablewidget->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    ui->Sample_Data_tablewidget->setAlternatingRowColors(true); //隔行换色
    ui->Sample_Data_tablewidget->verticalHeader()->show();// 显示行号
    //ui->Sample_Data_tablewidget->setSortingEnabled(true);//排序


    ui->Sample_Data_tablewidget->setStyleSheet(InitTalbe_css);
    ui->Sample_Data_tablewidget->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:rgb(188, 187, 186);\
                                                                    font:14pt '楷体';color: black;};");
    //设置水平、垂直滚动条样式
    ui->Sample_Data_tablewidget->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
    "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
    "QScrollBar::handle:hover{background:gray;}"
    "QScrollBar::sub-line{background:transparent;}"
    "QScrollBar::add-line{background:transparent;}");

    QString VScroBarCss =
            "QScrollBar:vertical {width: 18px; background: transparent; margin: 0px,0px,0px,0px; padding-top: 18px;padding-bottom: 18px;} "
            "QScrollBar::handle:vertical {width: 18px; background: rgba(0,0,0,25%);border-radius: 4px;min-height: 20;}"
            "QScrollBar::handle:vertical:hover {width: 8px;background: rgba(0,0,0,50%);border-radius: 4px; min-height: 20;}"
            "QScrollBar::add-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
            "QScrollBar::sub-line:vertical {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
            "QScrollBar::add-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/minus.png);subcontrol-position: bottom;}"
            "QScrollBar::sub-line:vertical:hover {height: 18px;width: 18px;border-image: url(:/Picture/Quality.png);subcontrol-position: top;}"
            "QScrollBar::sub-page:vertical {background: rgb(178,180,180); border-radius: 0px;}"
            "QScrollBar::add-page:vertical {background: rgb(178,180,180); border-radius: 0px;}";

    ui->Sample_Data_tablewidget->verticalScrollBar()->setStyleSheet(VScroBarCss);

    connect(ui->Sample_Data_tablewidget, SIGNAL(cellClicked(int,int)), this, SLOT(tableItemClicked(int,int)));

    connect(ui->Sample_Data_tablewidget->horizontalHeader(),SIGNAL(sectionClicked(int )),this, SLOT(sortByColumn(int)));
    return;
}

void Height_Data::_obtainSelectedSample(QList<int> &itemList)
{
    itemList.clear();
    QCheckBox *pcheckbox = nullptr;
    quint16 totalItem = ui->Sample_Data_tablewidget->rowCount();
    quint16 irows = 0;
    for(;irows < totalItem ;irows++)
    {
        if(QWidget *pwidgetCall = ui->Sample_Data_tablewidget->cellWidget(irows, CHECK_ROW)){
            pcheckbox = qobject_cast<QCheckBox*>(pwidgetCall->children().at(1));
            if(pcheckbox->checkState() == Qt::Checked){
                itemList.append(irows);
            }
        }
    }
}

//修改单个item
// 判断是否需要忽略点击
bool Height_Data::shouldIgnoreClick(int col) const {
    return ui->Sample_Data_tablewidget->rowCount() == 0 ||
           col == static_cast<int>(CHECK_ROW) ||
           col == static_cast<int>(RICHBLOOD_HOLE);
}
// 保存原始值
void Height_Data::saveOriginalValue(int row, int col) {
    if (auto* item = ui->Sample_Data_tablewidget->item(row, col)) {
        m_OriginallyValue = item->text();
    }
}
// 处理样本名称和高度数据列点击
void Height_Data::handleSampleOrHeightClick(int row, int col) {
    //m_correctdata->setRowColData(row, col, m_OriginallyValue);
    if (auto* nameItem = ui->Sample_Data_tablewidget->item(row, static_cast<int>(SAMLPE_NAME))) {
        m_correctdata->ClickSamplename(nameItem->text());
        m_correctdata->setrows(row);
        m_correctdata->setcols(col);
        m_correctdata->setorigindata(m_OriginallyValue);
    }
    m_correctdata->show();
}
// 处理项目项点击
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

// 处理条码点击
void Height_Data::handleBarcodeClick(int row, int col) {
    if (!m_Notifycode) {
        m_Notifycode = new NotifyBarCode(this);  // 设置父对象自动管理内存
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
    ui->Sample_Data_tablewidget->item(row,cols)->setText(noityBarcode);
}

//修改测高值手动
void Height_Data::SlotNotifyTestHeight(int Rows, int cols, QString Value)
{
    SetColumnText(Rows, cols, Value);
    return;
}

//修改样本重复校验
void Height_Data::Slot_ConfigureData(unsigned int Rows,int cols,QString data)
{
    QString remindertext;
    if(!repeatData(data))
    {
        remindertext = QString("%1").arg(tr("任务列表样本号已存在,请勿重复!"));
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("修改样本id失败",remindertext);
        SetColumnText(Rows, cols, m_OriginallyValue);
        return;
    }

    if(SQLTheSameItem(data))
    {
        remindertext = QString("%1").arg(tr("样本数据库号已存在,请勿重复!"));
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("修改样本id失败",remindertext);
        SetColumnText(Rows, cols, m_OriginallyValue);
        return;
    }

    //创建时间 --不显示在任务栏内
    if(mCreatTime.contains(m_OriginallyValue))
        mCreatTime.remove(m_OriginallyValue); //删除修改前的
    QString current_date = QDateTime::currentDateTime().toString("MM.dd hh:mm:ss");
    QString exchangesamplename = ui->Sample_Data_tablewidget->item(Rows, SAMLPE_NAME)->text();
    mCreatTime.insert(exchangesamplename, current_date);
    SetColumnText(Rows, cols, data);
    return;
}

//修改样本号 遍历表任务中是否有相同号
bool Height_Data::repeatData(QString textchange)
{
    int countrows = ui->Sample_Data_tablewidget->rowCount();
    for(int k = 0 ; k < countrows ;k++)
    {
        QString itemstr = ui->Sample_Data_tablewidget->item(k,SAMLPE_NAME)->text();
        if(textchange == itemstr)
          return false;
    }
    return true;
}

//修改样本号 遍历数据库表任务中是否有相同号
bool Height_Data::SQLTheSameItem(QString textchange)
{
    QString date;
    int idsample = 0;
    GlobalData::apartSampleId(textchange,date, idsample);
    QVector<int> todaySampleid;
    todaySampleid = FullyAutomatedPlatelets::pinstancesqlData()->getTodayAllSampleid(date);
    return todaySampleid.contains(idsample);
}



//点击表头
void Height_Data::sortByColumn(int col)
{
    if(ui->Sample_Data_tablewidget->rowCount() == 0 ) return;
    if(col == PROJECT_ITEM)
    {
        FullyAutomatedPlatelets::pinstanceTestproject()->Slot_ChangeSingleItem(0,col,false);
        FullyAutomatedPlatelets::pinstanceTestproject()->setWindowModality(Qt::NonModal);
        FullyAutomatedPlatelets::pinstanceTestproject()->setWindowFlags(Qt::Window | Qt::FramelessWindowHint |Qt::WindowStaysOnTopHint | Qt::WindowDoesNotAcceptFocus);
        FullyAutomatedPlatelets::pinstanceTestproject()->show();
    }
}


void Height_Data::SetColumnText(int row,int col,QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    if(HEIGHT_DATA == col)
    {
        double val = text.toDouble();
        if(val <= 0)
            item ->setBackgroundColor(QColor(255,0,0));
    }
    item->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    ui->Sample_Data_tablewidget->setItem(row,col,item);
    return;
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
           FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加测试样本失败","保存失败样本号重复,请重设!");
           bhandsame_ = true;
           break;
       }
    }
    return  bhandsame_;
}

bool Height_Data::WidetItemNUll(QTableWidget *TaskWidget, QList<int> selRows)
{
    for (auto Rows : selRows)
    {
        for (int i = 1; i < 6; i++)
        {
            if (i != 3)
            {
                if (TaskWidget->item(Rows, i) == nullptr || TaskWidget->item(Rows, i)->text().isEmpty())
                    return false;
            }
        }

    }
    return true;
}

//添加样本的血样孔号存在相同??
bool Height_Data::TheSameBloodHole()
{
    bool bhadSame = false;
    int  totalRows = ui->Sample_Data_tablewidget->rowCount();
    QString outtext_ = "";
    QMap<QString, bool> suckHoleBlood; suckHoleBlood.clear();
    for (int i = 0; i < totalRows; i++)
    {
        QWidget * widget = ui->Sample_Data_tablewidget->cellWidget(i, RICHBLOOD_HOLE);//获得widget
        QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
        if (!suckHoleBlood.contains(combox->currentText()))
        {
            suckHoleBlood.insert(combox->currentText(), true);
        }
        else
        {
            outtext_ += QString("同孔请检查第%1行孔号%2").arg(i + 1).arg(combox->currentText()) + "\n";
            bhadSame = true;
        }
    }
    if (bhadSame) {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加测试样本失败", outtext_);
    }
    return bhadSame;
}


//++样本试管是否够
bool Height_Data::NeedTubeEnouthTesting(const int hadtube,int &needtube ,QList<int> selItems)
{
    int tmpneedTube = 0;
    bool benoughtest = false;
    for(auto TaskItem : selItems)
    {
        int  Sel_Row = TaskItem;
        QString reagentItem = ui->Sample_Data_tablewidget->item(Sel_Row,PROJECT_ITEM)->text();
        QStringList ProjectList = reagentItem.split("|");
        tmpneedTube  = tmpneedTube + (ProjectList.size() + 1); //富血 + 贫血
    }
    needtube = tmpneedTube;

    QString _putlog = QString("仪器剩余试管数=%1 [加样所需试管数=%2]").arg(hadtube).arg(needtube);
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
    while (tableWidget->rowCount() > 0)
    {
        tableWidget->removeRow(0);
    }
    return;
}



//返回是否还有待测试血样的血样位置
bool Height_Data::_enoughAddSampleHole(QStringList &canselholeList_)
{
    bool fullEnoughHole = false;
    QMap<QString,bool>::iterator iter = m_pressedhole.begin();
    while(iter != m_pressedhole.end())
    {
        if(!iter.value())
            canselholeList_.push_back(iter.key());
        iter++;
    }
    bool fullselectHole = canselholeList_.isEmpty();
    (fullselectHole)? fullEnoughHole = false : fullEnoughHole = true;
    std::sort(canselholeList_.begin(), canselholeList_.end(), [](const QString& s1, const QString& s2){
                return s1.toInt() < s2.toInt(); });
    return fullEnoughHole;
}



void  Height_Data::ReminderPutBloodHole(int RichHolenum)
{
    emit ReminderHole(RichHolenum);
    return;
}

void Height_Data::_updateotherinserthole(int _rows, QString index_)
{
    int total_row = ui->Sample_Data_tablewidget->rowCount();
    for(int r = 0 ; r < total_row; r++) {
        if(r == _rows) continue;

        QWidget * widget = ui->Sample_Data_tablewidget->cellWidget(r, RICHBLOOD_HOLE);
        QComboBox *combox = qobject_cast<QComboBox*>(widget);
        if (!combox) continue;

        QString selectedText = combox->currentText();


        // 1. 获取可选项
        QStringList items;
        for(int i = 0; i < combox->count(); ++i) {
            items.append(combox->itemText(i));
        }

		if (!items.contains(index_)) {
			items.append(index_);

            // 自定义排序（按数值升序）
            std::sort(items.begin(), items.end(), [](const QString &a, const QString &b) {
                return a.toInt() < b.toInt();  // 数值升序
            });

            // 4. 重新填充
            combox->blockSignals(true);
            combox->clear();
            combox->addItems(items);

            // 查找原选中项的新位置
            int newIndex = combox->findText(selectedText);
            if (newIndex != -1) {
                combox->setCurrentIndex(newIndex);
            } else {
                // 可选：若原项不存在，设置默认选中项（如第0项）
                combox->setCurrentIndex(0);
            }
            combox->blockSignals(false);
        }
    }
    update();
}

void Height_Data::selectPPPholeChange(const QString& index_) {
    
	// 1. 坐标转换优化
	QComboBox *member = (QComboBox*)sender();
    const QPoint globalPos = member->mapToGlobal(QPoint(0, 0));
    const QPoint tablePos = ui->Sample_Data_tablewidget->mapFromGlobal(globalPos);
    const QModelIndex index_item = ui->Sample_Data_tablewidget->indexAt(tablePos);

	quint8 changeHole = index_.toInt();
   
    // 2. 获取行列数据
    const int row = index_item.row();
    const int column = index_item.column();

    //原值
    QString sampleidChnage;
	QTableWidgetItem *item = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME);
	if(item != nullptr)
		sampleidChnage = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME)->text();


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

            QSignalBlocker blocker(member);
            _updateotherinserthole(row,QString::number(oldPPPhole)); //更新其它行的富血孔项
		}

	}
	else {
		if (m_pressedhole.contains(index_)) {
			auto it = m_pressedhole.find(index_);
			if (it.value() == false) {
				it.value() = true;
				QLOG_DEBUG() << "使用孔" << index_ << "提示血样孔" << index_.toInt();
			}
		}
	}

	//提醒放置的血样孔
	emit ReminderHole(changeHole);
	return;
}



////////扫码&&测高完成/////
void  Height_Data::slotShowTestImageTube(QString Imagepath,
                                            double value,
                                            const bool &isreplace,
                                            const QString& idnum){
    // 模块1: 图像加载与显示
    if (!loadAndDisplayImage(Imagepath)) {
        QLOG_ERROR() << "Failed to load image:" << Imagepath;
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("图像错误", "测试管图像加载失败");
        return;
    }

    // 模块2: 表格数据替换模式
    if (isreplace) {
        updateTableItem(idnum, value);
        return;
    }

    // 模块3: 样本添加逻辑
    handleSampleAddition(value);

    //QPixmap *pixmap = new QPixmap(Imagepath);
    //pixmap->scaled(QSize(480, 640), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    //ui->Imageshow->setScaledContents(true);
    //ui->Imageshow->setPixmap(*pixmap);
    //ui->Imageshow->update();

    //if(isreplace){
    //    QTableWidget *paddsampleTable = ui->Sample_Data_tablewidget;
    //    int countRows = paddsampleTable->rowCount();
    //    for(int i = 0 ; i < countRows; i++){
    //        QTableWidgetItem *existingKeyItem = ui->tableWidget->item(i, SAMLPE_NAME);
    //        if (existingKeyItem && existingKeyItem->text() == idnum) {
    //            QTableWidgetItem *existingHeighItem  = ui->tableWidget->item(i, HEIGHT_DATA);
    //            if(existingHeighItem)
    //                existingHeighItem->setText(QString::number(value));
    //            break;
    //        }
    //    }
    //    return;
    //}
    //QStringList canaddholeList;
    //bool  bwholeBlood = INI_File().GetWholeBloodModel();
    //bool  benoughHole = _enoughAddSampleHole(canaddholeList);
    //if(!benoughHole)
    //{
    //    FullyAutomatedPlatelets::mainWindow()->_ReminderInfo("添加样本失败","血样孔已占满无加样位置!");
    //    QLOG_DEBUG()<<"测高加样可选血样孔:"<<canaddholeList<<endl;
    //    return;
    //}
    //bool ScanningOpen = INI_File().loadParameter("ScanCodetodo",false).toBool();
    //if(ScanningOpen && m_scanning)
    //{
    //    int rowcount = ui->Sample_Data_tablewidget->rowCount() - 1;
    //    QTableWidgetItem *itemTestHeight = ui->Sample_Data_tablewidget->item(rowcount,HEIGHT_DATA);
    //    itemTestHeight->setText(QString::number(value));
    //    if(itemTestHeight->text().toDouble() <= 0)
    //        itemTestHeight->setTextColor(QColor(255,0,0));
    //    itemTestHeight->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    //    m_scanning = false;
    //}
    //else if(!m_scanning)
    //{
    //    AddOneTestSample(bwholeBlood,value,canaddholeList,"null",true); //插入数据
    //    m_DetectionAltimetry = true;
    //}
    return;
}

/**
 * @brief 加载并显示图像
 * @param path 图像路径
 * @return 是否加载成功
 */
bool Height_Data::loadAndDisplayImage(const QString& path)
{
//	QPixmap *pixmap = new QPixmap(path);
//	pixmap->scaled(QSize(480, 640), Qt::KeepAspectRatio, Qt::SmoothTransformation);
//	ui->Imageshow->setScaledContents(true);
//	ui->Imageshow->setPixmap(*pixmap);
//	ui->Imageshow->update();
//	delete pixmap;
//	return true;


    QScopedPointer<QPixmap> pixmap(new QPixmap(path));
    if (pixmap->isNull()) return false;

    // 获取QLabel的当前可用尺寸
    QSize targetSize = ui->Imageshow->size();

    // 动态缩放，保持宽高比
    QPixmap scaledPix = pixmap->scaled(targetSize,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

    // 启用QLabel自动缩放内容
    ui->Imageshow->setScaledContents(true);  // [[9]]

    // 显示图片
    ui->Imageshow->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    ui->Imageshow->setPixmap(scaledPix);
    ui->Imageshow->update();
    return true;
}

/**
 * @brief 更新表格项
 * @param id 样本ID
 * @param heightValue 高度值
 */
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

/**
 * @brief 处理样本添加逻辑
 * @param heightValue 测量高度值
 */
void Height_Data::handleSampleAddition(double heightValue)
{
    // 检查加样孔位状态
    QStringList availableHoles;
    if (!_enoughAddSampleHole(availableHoles)) {
        showHoleWarning(availableHoles);
        return;
    }

    // 常规添加模式
    addNewSample(heightValue, availableHoles);
}

void Height_Data::showHoleWarning(const QStringList& holes)
{
    QLOG_DEBUG() << "可用加样孔位:" << holes;
    FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(
        "添加样本失败",
        QString("可用孔位: %1\n%2").arg(holes.join(",")).arg("无可用加样位置")
    );
}



void Height_Data::addNewSample(double value, const QStringList& holes)
{
    const bool wholeBloodMode = INI_File().GetWholeBloodModel();
    AddOneTestSample(wholeBloodMode, value, holes, "null", true);
}


//菜单栏
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

//全选
void Height_Data::selectAllItem(const bool bselAll)
{
    quint16 totalItem = ui->Sample_Data_tablewidget->rowCount();
    quint16 irows = 0;
    QCheckBox *pcheckbox = nullptr;
    for(;irows < totalItem ;irows++)
    {
       if(QWidget *pwidgetCall = ui->Sample_Data_tablewidget->cellWidget(irows, CHECK_ROW))
       {
           pcheckbox = qobject_cast<QCheckBox*>(pwidgetCall->children().at(1));
           if(pcheckbox){
               pcheckbox->setChecked(bselAll);
           }
       }
    }
}
//反选
void Height_Data::selectInverseItem()
{
    quint16 totalItem = ui->Sample_Data_tablewidget->rowCount();
    quint16 irows = 0;
    QCheckBox *pcheckbox = nullptr;
    for(;irows < totalItem ;irows++)
    {
       if(QWidget *pwidgetCall = ui->Sample_Data_tablewidget->cellWidget(irows, CHECK_ROW))
       {
           pcheckbox = qobject_cast<QCheckBox*>(pwidgetCall->children().at(1));
           if(pcheckbox){
               if(pcheckbox->checkState() == Qt::Checked)
                   pcheckbox->setChecked(false);
               else if(pcheckbox->checkState() == Qt::Unchecked)
                   pcheckbox->setChecked(true);
           }
       }
    }
}


//判断测高值为 0？
bool Height_Data::testhightValueNotZero(QList<int> checklist)
{
    bool bheihevalnotZero = true;
    QList<int>::iterator it = checklist.begin();
    while (it != checklist.end())
    {
        int index = *it;
        QString heighValue = ui->Sample_Data_tablewidget->item(index,HEIGHT_DATA)->text();
        double val = heighValue.toDouble();
        if(val <= 0)
        {
            bheihevalnotZero = false;
            FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加测试样本失败","保存任务选中测高值项为[0]");
            update();
            break;
        }
        it++;
    }
    return bheihevalnotZero;
}


//添加一个样本
int Height_Data::AddOneTestSample(const bool addSampleMode,double TestHeight,
                                    QStringList Canselecthole,QString barcodestr,
                                    bool viewImage)
{
    QTableWidget *paddsampleTable = ui->Sample_Data_tablewidget;
    int countRows = paddsampleTable->rowCount();
    paddsampleTable->insertRow(countRows);

    QCheckBox* pCheckBox = new QCheckBox();
    QWidget* pBoxWidget = new QWidget();
    QHBoxLayout* hBox = new QHBoxLayout();

    //调整框的大小,添加选中图片
    pCheckBox->setStyleSheet("QCheckBox::indicator{width:40px;height:40px}"
                             "QCheckBox::indicator:unchecked{image: url(:/Picture/SetPng/checkbox-blank.png);width:40px;height:40px}"
                             "QCheckBox::indicator:checked {image: url(:/Picture/SetPng/checkbox-fill.png); width: 40px;height:40px}");
    hBox->addWidget(pCheckBox);
    pCheckBox->setChecked(true);//设置默认选中状态
    hBox->setMargin(0);
    hBox->setAlignment(pCheckBox,Qt::AlignCenter);
    pBoxWidget->setLayout(hBox);
    paddsampleTable->setCellWidget(countRows,CHECK_ROW,pBoxWidget);

    /*样本号*/
    QString addSampleid,datefind;
    int setsampleNum = 0;
    QString  todayLast = GlobalData::ObatinCreatSampleTime();
    if(countRows == 0)
    {
        //遍历数据库内样本信息和结果表当天的最大样本号
        int maxid = FullyAutomatedPlatelets::pinstancesqlData()->getMaxSampleID(todayLast);
        addSampleid = GlobalData::groupDateAndID(todayLast,maxid + 1);
    }else{
        QString sampleid = paddsampleTable->item(countRows - 1, SAMLPE_NAME)->text();
        GlobalData::apartSampleId(sampleid,datefind,setsampleNum);
        setsampleNum = setsampleNum + 1;
        addSampleid = GlobalData::groupDateAndID(todayLast,setsampleNum);
    }
    QTableWidgetItem *itemSampleNum = new QTableWidgetItem(addSampleid);
    itemSampleNum->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    if (!addSampleMode)
        itemSampleNum->setBackgroundColor(QColor(135, 206, 250));
    else
        itemSampleNum->setBackgroundColor(WHOLEBLOODMODE);
    paddsampleTable->setItem(countRows,SAMLPE_NAME, itemSampleNum);

    /*测高值*/
    QTableWidgetItem *itemTestHeight = nullptr;
    double bloodHeight = 0.00f,suckPrpDownPinHeight = 0.00f;
    if(addSampleMode)
    {
        bloodHeight = TestHeight;
        itemTestHeight = new QTableWidgetItem(QString("%1").arg(bloodHeight));
        if(bloodHeight <= 0)
            itemTestHeight->setTextColor(QColor(255,0,0));
    }
    else
    {
        suckPrpDownPinHeight = INI_File().GetAbsorbTubeBottom();
        itemTestHeight = new QTableWidgetItem(QString("%1").arg(suckPrpDownPinHeight));
    }
    itemTestHeight->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    paddsampleTable->setItem(countRows,HEIGHT_DATA,itemTestHeight);

    /*血样框孔号*/
    QComboBox *bloodHoleSel = new QComboBox(this);
    GlobalData::QCommboxSheet(bloodHoleSel);
    //bloodHoleSel->setEditable(true);
    //bloodHoleSel->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
    bloodHoleSel->setFixedHeight(40);
    connect(bloodHoleSel,
            static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentTextChanged),
            this,
            &Height_Data::selectPPPholeChange);



    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate(this);
    bloodHoleSel->setItemDelegate(itemDelegate);
    bloodHoleSel->installEventFilter(this);
    bloodHoleSel->addItems(Canselecthole);

    QVector<quint8> HoleList;
    foreach (QString BloodHole, Canselecthole)
        HoleList.push_back(BloodHole.toInt());
    auto min_ = std::min_element(std::begin(HoleList), std::end(HoleList));
    quint8 min_hole = *min_;
    QString min_str = QString::number(min_hole);
    bloodHoleSel->blockSignals(true);
    bloodHoleSel->setCurrentText(min_str);
    bloodHoleSel->blockSignals(false);

    paddsampleTable->setCellWidget(countRows,RICHBLOOD_HOLE, bloodHoleSel);
    paddsampleTable->resizeRowToContents(countRows);
    paddsampleTable->scrollToBottom();
    m_selbloodholetemp.insert(addSampleid,bloodHoleSel->currentText().toInt());

    //创建时间 --不显示在任务栏内
    QString current_date = QDateTime::currentDateTime().toString("MM dd hh:mm:ss");
    mCreatTime.insert(addSampleid,current_date);

    /*条码*/
    QTableWidgetItem *itembarcode = nullptr;
    itembarcode = new QTableWidgetItem(barcodestr);
    itembarcode->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    itembarcode->setFlags((Qt::ItemFlag)63);// 设置单元格可编辑
    paddsampleTable->setItem(countRows, BARCODE, itembarcode);

    if(!viewImage)
        initshowimg();

    return countRows;
}


//++手动按钮添加
int Height_Data::_Addtasksmanually()
{
    QStringList canaddholeList;
    bool  bwholeBlood = INI_File().GetWholeBloodModel();
    bool  benoughHole = _enoughAddSampleHole(canaddholeList);
    if(!benoughHole)
    {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加样本失败","血样孔已占满无加样位置!");
        QLOG_DEBUG()<<"手动加样可选血样孔:"<<canaddholeList<<endl;
        return -1;
    }
   if(bwholeBlood)
    {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加样本失败","手动加样请切换血浆模式!");
        return -1;
    }
    return AddOneTestSample(bwholeBlood,0,canaddholeList,"null",false);
}


void Height_Data::_savewaitTestSample()
{
    QList<int> selecteditem_;
    _obtainSelectedSample(selecteditem_);
    QTableWidget *pSampleTable = ui->Sample_Data_tablewidget;

    int selectedTotal =  selecteditem_.size();
    int totalrows     =  pSampleTable->rowCount();

    if(totalrows == 0 || selectedTotal == 0)
    {
        ReminderPutBloodHole(-1); //提示要放的孔号
        QLOG_DEBUG()<<"保存添加测试样本为空";
        emit this->ReminderTextOut(PROMPTLOG,tr("保存添加测试样本为空"));
        close();
        return;
    }

    //校验+样本的配置信息
    if(!WidetItemNUll(pSampleTable, selecteditem_))
    {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加测试样本失败","请检查样本配置信息!");
        return;
    }

    //校验+样本 血样孔有相同
    if (TheSameBloodHole()) return;

    //校验测高样本的值=0
    //if(!testhightValueNotZero(selecteditem_)) return;

    //校验相同样本号
    if (SameSampleandtube(selecteditem_,pSampleTable))
        return;

    int needtube = 0; //所需试杯
    quint8 remainder_tube =  FullyAutomatedPlatelets::pinstancesqlData()->BackEmptyTubeNum();
    bool  enoughTube = NeedTubeEnouthTesting(remainder_tube,needtube,selecteditem_);
    if(!enoughTube)
    {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加测试样本失败","剩余试管不足添加新样本!");
        return;
    }

    bool _dimmingLED;
    FullyAutomatedPlatelets::pinstancedimming()->getdimming_status(_dimmingLED);
    if(!_dimmingLED){
        emit ReminderTextOut(PROMPTLOG,tr("调光未完成,稍后添加!"));
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("添加测试样本失败","调光未完成,稍后添加!");
        return;
    }

    _sycnstudata(selecteditem_, pSampleTable);

    //配置成功删除任务行和关闭对话框
    DeleteAllItems(pSampleTable);
    mCreatTime.clear();
    this->close();
    return;
}

void Height_Data::_updateaddprogress(int index_,int _total)
{
   FullyAutomatedPlatelets::mainWindow()->_progressBarconfig(index_,_total);
}



void Height_Data::_slotupdatetestui(QList<quint8> marktube, QString sample_name,
                                    quint8 anemiahole,int index_add, int all_add_task)
{
    QLOG_DEBUG()<<"标记试管["<<marktube<<"]";
    QString date;
    int id;
    GlobalData::apartSampleId(sample_name,date,id);
    FullyAutomatedPlatelets::pinstanceTesting()->sycn_changeui_status(QString::number(id),anemiahole,
                                                                      marktube,index_add,all_add_task);
    return;
}


void Height_Data::_sycnstudata(QList<int> TaskList,QTableWidget *TaskWidget)
{
	if (!TaskWidget || TaskList.isEmpty()) return;

	const int totalTasks = TaskList.size();
	const QString savedtime = QDateTime::currentDateTime().toString("MM.dd hh:mm:ss");

	for (int Sel_Row : TaskList) // 范围循环提升性能
	{
		// 1. 行索引有效性检查
		if (Sel_Row < 0 || Sel_Row >= TaskWidget->rowCount()) continue;

		// 2. 集中获取单元格项，避免重复调用
		QTableWidgetItem* sampleItem = TaskWidget->item(Sel_Row, SAMLPE_NAME);
		QTableWidgetItem* barcodeItem = TaskWidget->item(Sel_Row, BARCODE);
		QTableWidgetItem* heightItem = TaskWidget->item(Sel_Row, HEIGHT_DATA);
		QTableWidgetItem* projectItem = TaskWidget->item(Sel_Row, PROJECT_ITEM);

		// 3. 空指针检查
		if (!sampleItem || !barcodeItem || !heightItem || !projectItem) continue;

		// 4. 简化背景色逻辑
		bool insertWholeBloodMode = (sampleItem->background().color() == WHOLEBLOODMODE);

		// 5. 安全获取QComboBox数据
		int CurrRichHole = 0;
		QWidget* widget = TaskWidget->cellWidget(Sel_Row, RICHBLOOD_HOLE);
		if (widget) {
			QComboBox* combox = qobject_cast<QComboBox*>(widget); // 安全类型转换 [[5]]
			if (combox) CurrRichHole = combox->currentText().toInt(); // 直接获取文本 [[1]]
		}
		// 6. 发射信号（使用预计算的totalTasks和savedtime）
		emit _sycnwaittestsampledata(
			sampleItem->text(),
			savedtime,
			barcodeItem->text(),
			heightItem->text().toDouble(), // 减少临时QString创建
			projectItem->text(),
			CurrRichHole,
			totalTasks,
			insertWholeBloodMode
			);
	}


    //for(auto TaskItem : TaskList)
    //{
    //    int Sel_Row = TaskItem;
    //    bool insertWholeBloodMode = false;
    //    QString samplename = TaskWidget->item(Sel_Row,SAMLPE_NAME)->text();
    //    QColor bgmcolor = TaskWidget->item(Sel_Row,SAMLPE_NAME)->background().color();
    //    if(bgmcolor == WHOLEBLOODMODE)
    //        insertWholeBloodMode = true;
    //    else
    //        insertWholeBloodMode = false;
    //    QString barcode_str = TaskWidget->item(Sel_Row,BARCODE)->text();
    //    double done_pin =  QString(TaskWidget->item(Sel_Row,HEIGHT_DATA)->text()).toDouble();
    //    QString project_ = TaskWidget->item(Sel_Row,PROJECT_ITEM)->text();
    //    QString savedtime = QDateTime::currentDateTime().toString("MM.dd hh:mm:ss");
    //    QWidget * widget = TaskWidget->cellWidget(Sel_Row, RICHBLOOD_HOLE);//获得widget
    //    QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
    //    int CurrRichHole  = combox->currentText().toInt();  //添加富血对应血样区显示孔
    //    emit _sycnwaittestsampledata(samplename, savedtime, barcode_str, done_pin, project_,
    //                                       CurrRichHole,TaskList.size(),insertWholeBloodMode);
    //}
}




//删除样本
void Height_Data::_deleteSelectedSample()
{
    QList<int> checklist;
    checklist.clear();
    _obtainSelectedSample(checklist);
    if(checklist.count() <= 0)
    {
        FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("删除任务失败","请选择要删除任务!");
        return;
    }
    QList<QTableWidgetItem *> selItem;
    QList<int>::iterator it = checklist.begin();
    while (it != checklist.end())
    {
        int select_ = *it;
        selItem.append(ui->Sample_Data_tablewidget->item(select_,SAMLPE_NAME)) ;
        it++;
    }
    for(int i = 0 ;i < selItem.count();i++)
    {
        int row = ui->Sample_Data_tablewidget->row(selItem.at(i));
        //删除任务--同步数据库任务删除样本号
        //QString Samplename = ui->Sample_Data_tablewidget->item(row, SAMLPE_NAME)->text();

        //删除列表同步显示的结果界面
        QWidget * widget = ui->Sample_Data_tablewidget->cellWidget(row, RICHBLOOD_HOLE);//获得widget
        QComboBox *combox = (QComboBox*)widget;//强制转化为QComboBox
        quint8 curr  = combox->currentText().toInt();
        Makeuptubenum(curr);

        delete selItem.at(i);
        if(row != -1)
            ui->Sample_Data_tablewidget->removeRow(row);
    }
    return;
}


//批量添加任务
void Height_Data::batchAddTask()
{
    if(INI_File().GetWholeBloodModel()){
        QMessageBox::warning(this,tr("批量添加失败"),tr("请切换血浆模式添加"));
        return;
    }

    if(m_batchaddTestnumSample == nullptr){
        m_batchaddTestnumSample = new batchAddSample;
        qRegisterMetaType<QVariant>("QVariant"); //写在构造函数里

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
        m_batchaddTestnumSample->showNormal();  // 先恢复窗口标准状态
        m_batchaddTestnumSample->raise();       // 确保窗口置顶
        m_batchaddTestnumSample->activateWindow(); // 激活窗口焦点
    }
    m_batchaddTestnumSample.data()->show();
}

 void Height_Data::AF_DATA_REQUEST(QVariant sampleiddata)
 {
     SAMPLEIDINFO askData;
     askData = sampleiddata.value<SAMPLEIDINFO>();
     QString  todayLast = GlobalData::ObatinCreatSampleTime();
     QString addSampleid = GlobalData::groupDateAndID(todayLast,askData.idsample.toInt());
     int insertRows = _Addtasksmanually();
     if(insertRows >= 0)
     {
         SetColumnText(insertRows,SAMLPE_NAME,addSampleid);
         SetColumnText(insertRows,HEIGHT_DATA,askData.pindownHeight);
         SetColumnText(insertRows,PROJECT_ITEM,askData.testProtect);
     }
     return;
 }

