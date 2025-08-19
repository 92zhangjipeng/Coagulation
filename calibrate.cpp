#pragma execution_character_set("utf-8")

#include "calibrate.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QDateTime>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QSqlDatabase>
#include "ui_calibrate.h"
#include "mainwindow.h"
#include "delegate.h"
#include "loginui.h"
#include "globaldata.h"
#include <QPainter>
#include <QPrinterInfo>
#include <string.h>
#include <random>
#include <iostream>
#include <ctime>
#include <QVector>
#include <QMetaType>
#include <QVariant>
#include <custom_style/widgetdelegate.h>
#include <custom_style/freezetablewidget.h>
#include <operclass/fullyautomatedplatelets.h>


Calibrate::Calibrate(QWidget *parent) :
    QWidget(parent),
    m_clickViewSampleId(-1),
    ui(new Ui::Calibrate)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);
    m_pdelegate.clear();


    qRegisterMetaType<QVariant>("QVariant");
    m_loadTableColor.setRgb(139, 134, 130);

}

Calibrate::~Calibrate()
{
    qDeleteAll(m_pdelegate);
    m_pdelegate.clear();

    qDeleteAll(m_eneditdelegate);
    m_eneditdelegate.clear();

    qDeleteAll(m_pconfigtimedelegate);
    m_pconfigtimedelegate.clear();

    delete ui;
}

void Calibrate::initstyle()
{
    InitSaveTableList(ui->saveTaskWidgetList);
}

void Calibrate::InitSaveTableList(QTableWidget * pTable)
{
    QStringList headerList{tr("样本号"),tr("添加时间"),tr("条形码"),tr("姓名"),tr("性别"),
                         tr("年龄"),tr("科室"),tr("住院号"),tr("床号"),tr("病区号")
                        ,tr("送检时间"),tr("送检医生"),tr("检验时间"),tr("检验医生")
                        ,tr("诊断"),tr("备注"),tr("审核医生")};


    pTable->setRowCount(0);  //初始化行
    pTable->setColumnCount(verifierinfo::ITEM_TOTAL); //初始化列

    pTable->setHorizontalHeaderLabels(headerList);


    QFont font ;
    font.setFamily("楷体");
    font.setPixelSize(14);
    font.setBold(true);

    pTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable->horizontalHeader()->setFixedHeight(40);

    pTable->horizontalHeader()->setFont(font);
    pTable->horizontalHeader()->setStretchLastSection(true); //设置充满表宽度
    pTable->verticalHeader()->setDefaultSectionSize(30);    //设置行高


    pTable->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    pTable->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);// 将根据整个列或行的内容自动调整区段的大小
    pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);//x先自适应宽度


    QHeaderView *header1 = pTable->verticalHeader();
    header1->setHidden(true);
    pTable->verticalHeader()->setDefaultSectionSize(30); //设置行高
    pTable->horizontalHeader()->setDefaultSectionSize(30);

    pTable->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);
    pTable->setColumnWidth(0, 140);
    pTable->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    pTable->setColumnWidth(1, 110);
    pTable->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Fixed);
    pTable->setColumnWidth(2, 140);


    for(int i = 3; i < headerList.size(); i++){
        pTable->horizontalHeader()->setSectionResizeMode(i,QHeaderView::Stretch);
        //pTable->horizontalHeader()->resizeSection(i,110); //设置表头第一列的宽度为
    }

    pTable->horizontalHeader()->setFixedHeight(30); //设置表头的高度
    pTable->horizontalHeader()->setStretchLastSection(true); //使行列头自适应宽度，所有列平均分来填充空白部分
    pTable->setAlternatingRowColors(true); //隔行换色
    pTable->verticalHeader()->show();// 显示行号



    pTable->horizontalHeader()->setStyleSheet("QHeaderView::section{background-color:#FFFFFF; font:14pt '楷体';color: black;};");
    pTable->setStyleSheet(TableWidgetCss);
    pTable->verticalScrollBar()->setStyleSheet(VScroBarCss);

         //设置水平、垂直滚动条样式
    pTable->horizontalScrollBar()->setStyleSheet("QScrollBar{background:transparent; height:10px;}"
         "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
         "QScrollBar::handle:hover{background:gray;}"
         "QScrollBar::sub-line{background:transparent;}"
         "QScrollBar::add-line{background:transparent;}");



    //设置不可编辑列
    QList<int> NoteditableColsList;
    NoteditableColsList<<SAMPLEID<<ADDSAMPLETIME<<BARCODESAMPLE;
    _setNoteditableCol(NoteditableColsList);

    updatecommboxInfo();

    TableSelTimeDelegate* pconfigtimedelegate = new TableSelTimeDelegate();
    pTable->setItemDelegateForColumn(SUBMISSIONTIME, pconfigtimedelegate);
    m_pconfigtimedelegate.push_back(pconfigtimedelegate);

    TableSelTimeDelegate* pconfigtimedelegate_other = new TableSelTimeDelegate();
    pTable->setItemDelegateForColumn(INSPECTIONTIME, pconfigtimedelegate_other);
    m_pconfigtimedelegate.push_back(pconfigtimedelegate_other);

    //导入当天的样本信息
    ImportTodayInfo();

    pTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(pTable,&QTableWidget::customContextMenuRequested,
            this, &Calibrate::showContextpup);


    connect(pTable,SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(dgvDellClick(QTableWidgetItem*)));


    return;
}

void Calibrate::_setNoteditableCol(QList<int> cols){
    foreach (int val_, cols) {
       TableItemDelegate* peneditable = new TableItemDelegate();
       ui->saveTaskWidgetList->setItemDelegateForColumn(val_, peneditable);
       m_eneditdelegate.push_back(peneditable);
    }  
}

void Calibrate::_setCommboxDelegate(QList<int> cols)
{
    QMap<int, QString> columnMap = {
            {SEXSAMPLE, "性别"},
            {DEPARTMENT, "科别"},
            {WARDCODE, "病区"},
            {BEDNUMBER, "床号"},
            {DIAGNOSIS, "临床诊断"},
            {REFERTOTHEDOCTOR, "送检医生"},
            {REVIEWDOCTORS, "审核医生"},
            {REMARK, "备注"}
    };
    for (int col : cols) {
        if (columnMap.contains(col)) {
            QStringList dataList;
            FullyAutomatedPlatelets::pinstancesqlData()->_obtainPatientInfo(columnMap[col], dataList);
            WidgetDelegate *delegate = new WidgetDelegate(dataList);
            ui->saveTaskWidgetList->setItemDelegateForColumn(col, delegate);
            m_pdelegate.push_back(delegate);
        }
        else if(col == static_cast<int>(MEDICALEXAMINER))
        {
            //检验医生==登录账户
            QStringList Testlist;
            Testlist.push_back(cglobal::g_UserName_str);
            WidgetDelegate *_testingDelegate = new WidgetDelegate(Testlist);
            ui->saveTaskWidgetList->setItemDelegateForColumn(MEDICALEXAMINER ,_testingDelegate);
            m_pdelegate.push_back(_testingDelegate);
        }
    }
}



void  Calibrate::MarktableWidgetColor(QTableWidget* table, int row, int cols, const QString& text, const QColor& color)
{
    QTableWidgetItem * item = ui->saveTaskWidgetList->item(row,cols);
    if (!item) {
        item = new QTableWidgetItem();
        table->setItem(row, cols, item);
    }
    item->setText(text);
    item->setBackground(color);
}

void Calibrate::SetColumnText(QTableWidget * tablewiget,int row,int col,QString text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    tablewiget->setItem(row,col,item);
}


void Calibrate::showContextpup(const QPoint &pos){
    QTableWidget *ptablewidget = ui->saveTaskWidgetList;

	int clickrow = -1;
    QTableWidgetItem *item = ptablewidget->itemAt(pos); // 获取点击项
	if (item) {
		clickrow = item->row(); // 获取行号
	}
	if (clickrow < 0) {
		return;
	}

    QString clickDate,sampleSex;
    int clickIdNum;
    QString clickId = ptablewidget->item(clickrow,SAMPLEID)->text();
    GlobalData::apartSampleId(clickId,clickDate,clickIdNum);

    m_clickViewSampleId = clickIdNum;

    if(mpdatawidget.isNull()){
        mpdatawidget.reset(new CustomFixTableView());
        connect(this, &Calibrate::hideCurveUi, mpdatawidget.data(),
                &CustomFixTableView::hideWithAnimation);


    }

    QTableWidgetItem* targetItem = ptablewidget->item(clickrow,SEXSAMPLE);
    if (targetItem && !targetItem->text().isEmpty()) {
		sampleSex = targetItem->text();
    }else{
		QLOG_ERROR() << "选中样本未设置性别,查询对比值失败" << endl;
    }

	QList<QString> hanSampleID;
	hanSampleID.reserve(ptablewidget->rowCount());
	for (int n = 0; n < ptablewidget->rowCount(); ++n) {
		targetItem = ptablewidget->item(n, SAMPLEID);
		if (targetItem && !targetItem->text().isEmpty())
			hanSampleID.append(targetItem->text());
	}


    mpdatawidget->setCheckBoxState(false, sampleSex,hanSampleID);
    mpdatawidget->setClickViewId(clickId,clickIdNum);
    mpdatawidget->showResult(false);

    if (mpdatawidget->isMinimized()) {
        mpdatawidget->showNormal();  // 先恢复窗口标准状态
        mpdatawidget->raise();       // 确保窗口置顶[[12]]
        mpdatawidget->activateWindow(); // 激活窗口焦点
    }
    mpdatawidget->showWithAnimation();
 
}



void  Calibrate::ImportTodayInfo()
{
    QTableWidget *pSampleInfoTable = ui->saveTaskWidgetList;

    pSampleInfoTable->setUpdatesEnabled(false);
    const QSignalBlocker blocker(pSampleInfoTable);

    QVector<QSharedPointer<PatientInformationStu>> painterInfovec;
    QString targetdate = GlobalData::ObatinCreatSampleTime();
    FullyAutomatedPlatelets::pinstancesqlData()->getOneDayTestResultData(targetdate, painterInfovec);

    // 3. 批量设置行数（避免逐行插入）
    pSampleInfoTable->clearContents();  // 清空内容但保留表头
    pSampleInfoTable->setRowCount(painterInfovec.size());  // 一次性分配行

    // 4. 预定义列索引常量（减少重复计算）
    constexpr int colTime = verifierinfo::ADDSAMPLETIME;
    constexpr int colId = verifierinfo::SAMPLEID;
    constexpr int colBarcode = verifierinfo::BARCODESAMPLE;
    constexpr int colName = verifierinfo::NAMESAMPLE;

    for (int tRow = 0; tRow < painterInfovec.size(); ++tRow)
       {
           const auto& data = painterInfovec[tRow];
           MarktableWidgetColor(pSampleInfoTable, tRow, colTime, data->AddsampleTime, m_loadTableColor);
           MarktableWidgetColor(pSampleInfoTable, tRow, colId, data->sampleId, m_loadTableColor);
           MarktableWidgetColor(pSampleInfoTable, tRow, colBarcode, data->barcodesample, m_loadTableColor);
           MarktableWidgetColor(pSampleInfoTable, tRow, colName, data->SampleName, m_loadTableColor);

           // 非高亮列快速设置（无需颜色标记）
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::SEXSAMPLE,    data->sex_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::AGESAMPLE,    QString::number(data->agesample));
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::DEPARTMENT,    data->department_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::HOSPITALIZATIONNUMBER,    data->hospitalzationnumber_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::BEDNUMBER,    data->bednumber_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::WARDCODE,    data->wardcode_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::SUBMISSIONTIME,    data->submissiontime_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::REFERTOTHEDOCTOR,    data->refertithedoctor_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::INSPECTIONTIME,    data->inspectiontime_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::MEDICALEXAMINER,    data->medicalexaminer_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::DIAGNOSIS,    data->diagnosis_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::REMARK,    data->remark_);
           SetColumnText(pSampleInfoTable, tRow, verifierinfo::REVIEWDOCTORS,    data->reviewdoctors_);

       }
    // 恢复更新并触发渲染
    pSampleInfoTable->setUpdatesEnabled(true);  //
    pSampleInfoTable->resizeRowsToContents();  // 如需自适应行高
    return;
}

void Calibrate::_addpatientsqltable(QString id_,QString addtime,QString barcode_,QString testProject)
{
    // 插入样本数据（增加错误检查）
    if (!FullyAutomatedPlatelets::pinstancesqlData()->insertAddSampleData(id_, addtime, barcode_)) {
        QLOG_ERROR() << "样本信息插入失败：" << id_;
        return;
    }

    // 插入曲线数据
    if (!FullyAutomatedPlatelets::pinstancesqlData()->insertCurveRecord(id_)) {
        QLOG_ERROR() << "曲线记录插入失败：" << id_;
        return;
    }

    // 获取待测试试剂列表
    QList<quint8> waitTestReagentList;
    GlobalData::sycnTestingReagnetNum(waitTestReagentList, testProject);

    // 定义试剂类型与列名的映射
    const QMap<quint8, QString> REAGENT_COLUMN_MAP = {
        {AA_REAGENT,  "AA"},
        {ADP_REAGENT, "ADP"},
        {EPI_REAGENT, "EPI"},
        {COL_REAGENT, "COL"},
        {RIS_REAGENT, "RIS"}
    };

    // 更新曲线和结果数据
    QString _waitTest_ = "null";  // 空值表示NULL
    for (int i = 0; i < waitTestReagentList.count(); i++) {
        quint8 reagent = waitTestReagentList.at(i);
        FullyAutomatedPlatelets::pinstancesqlData()->updateTestCurveDataTale(id_, reagent, _waitTest_);

        if (REAGENT_COLUMN_MAP.contains(reagent)) {
            QString column = REAGENT_COLUMN_MAP[reagent];
            FullyAutomatedPlatelets::pinstancesqlData()->updateTestResultTable(id_, column, _waitTest_);
        }
    }

    // 更新UI表格
    QTableWidget *pSampleInfoTable = ui->saveTaskWidgetList;
    const QSignalBlocker blocker(pSampleInfoTable);
    int iRow = pSampleInfoTable->rowCount();
    pSampleInfoTable->insertRow(iRow);
    SetColumnText(pSampleInfoTable, iRow, ADDSAMPLETIME, addtime);
    SetColumnText(pSampleInfoTable, iRow, SAMPLEID, id_);
    SetColumnText(pSampleInfoTable, iRow, BARCODESAMPLE, barcode_);
    SetColumnText(pSampleInfoTable, iRow, NAMESAMPLE, "");  // 使用空字符串
    return;
}

void Calibrate::dgvDellClick(QTableWidgetItem* item){

    if (!item) return;

    int row = item->row();
    int col = item->column();
    QString sampleId = ui->saveTaskWidgetList->item(row, SAMPLEID)->text();
    if (sampleId.isEmpty()) return;


    static const std::map<int, QString> columnToField = {
            {NAMESAMPLE, "姓名"},
            {SEXSAMPLE, "性别"},
            {AGESAMPLE, "年龄"},
            {DEPARTMENT, "科别"},
            {HOSPITALIZATIONNUMBER, "住院号"},
            {BEDNUMBER, "床号"},
            {WARDCODE, "病区"},
            {SUBMISSIONTIME, "送检时间"},
            {REFERTOTHEDOCTOR, "检验医生"},
            {INSPECTIONTIME, "检验时间"},
            {MEDICALEXAMINER, "检验医生"}, // Note: duplicate field, might be intentional
            {DIAGNOSIS, "临床诊断"},
            {REMARK, "备注"},
            {REVIEWDOCTORS, "审核医生"}
        };
    auto it = columnToField.find(col);
    if (it != columnToField.end()) {
        QString fieldName = it->second;
        FullyAutomatedPlatelets::pinstancesqlData()->updateTestResultTable(sampleId, fieldName, item->text());
    }
}

//设置下拉框代理
void Calibrate::updatecommboxInfo(){
    QList<int> DelegatecolList;
    DelegatecolList<<SEXSAMPLE<<DEPARTMENT<<BEDNUMBER<<WARDCODE
                  <<REFERTOTHEDOCTOR<<MEDICALEXAMINER
                  <<DIAGNOSIS<<REMARK<<REVIEWDOCTORS;
    _setCommboxDelegate(DelegatecolList);
}


//取消任务删除患者样本
void  Calibrate::cancelSampleResultItem(const QString &cancelId)
{
    QTableWidget *pSampleInfoTable = ui->saveTaskWidgetList;
    // 使用RAII模式确保信号阻塞/恢复的异常安全
    const QSignalBlocker blocker(pSampleInfoTable);  // 自动管理信号阻塞

    // 逆向遍历避免索引错位问题
    for (int i = pSampleInfoTable->rowCount() - 1; i >= 0; --i) {
        if (pSampleInfoTable->item(i, SAMPLEID)->text() == cancelId) {
            pSampleInfoTable->removeRow(i);  // 找到目标后立即删除
            return;  // 直接返回避免无效循环
        }
    }
}

void Calibrate::recvSampleTestingErr(const QString& ErrSampleid,const quint8 &channelIdx)
{
    QTableWidget *pSampleInfoTable = ui->saveTaskWidgetList;
    const QSignalBlocker blocker(pSampleInfoTable);
    for (int i = pSampleInfoTable->rowCount() - 1; i >= 0; --i) {
       QTableWidgetItem *item = pSampleInfoTable->item(i, SAMPLEID);
       if (item && item->text() == ErrSampleid) {
           item->setBackground(QColor(255, 0, 0)); // 设置背景颜色为红色
           FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleChannelFlash(false,channelIdx);
           return;
        }
    }
}
