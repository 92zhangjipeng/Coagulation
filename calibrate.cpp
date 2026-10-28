
#include "calibrate.h"
#include "pairframetablewidget.h"
#include <QCoreApplication>
#include <QDateTime>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QShowEvent>
#include <QSqlDatabase>
#include <algorithm>
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
#include <QMap>
#include <QTimer>
#include <QStyleOptionViewItem>
#include <custom_style/widgetdelegate.h>
#include <custom_style/freezetablewidget.h>
#include <operclass/fullyautomatedplatelets.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

const QString Calibrate::VScroBarCss =
"QScrollBar:vertical {"
"    width: 14px;"
"    background: #e8eef4;"
"    margin: 2px 2px 2px 2px;"
"    border: none;"
"    border-radius: 7px;"
"}"
"QScrollBar::groove:vertical {"
"    background: #e8eef4;"
"    border-radius: 7px;"
"}"
"QScrollBar::handle:vertical {"
"    background: #7a93ab;"
"    min-height: 48px;"
"    border-radius: 6px;"
"    margin: 2px;"
"}"
"QScrollBar::handle:vertical:hover {"
"    background: #4f6f8f;"
"}"
"QScrollBar::handle:vertical:pressed {"
"    background: #2c3e50;"
"}"
"QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
"    height: 0px;"
"    width: 0px;"
"    background: transparent;"
"    border: none;"
"}"
"QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
"    background: transparent;"
"}"
"QScrollBar:horizontal {"
"    height: 12px;"
"    background: #e8eef4;"
"    margin: 2px 2px 2px 2px;"
"    border: none;"
"    border-radius: 6px;"
"}"
"QScrollBar::groove:horizontal {"
"    background: #e8eef4;"
"    border-radius: 6px;"
"}"
"QScrollBar::handle:horizontal {"
"    background: #7a93ab;"
"    min-width: 48px;"
"    border-radius: 5px;"
"    margin: 2px;"
"}"
"QScrollBar::handle:horizontal:hover {"
"    background: #4f6f8f;"
"}"
"QScrollBar::handle:horizontal:pressed {"
"    background: #2c3e50;"
"}"
"QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {"
"    height: 0px;"
"    width: 0px;"
"    background: transparent;"
"    border: none;"
"}"
"QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {"
"    background: transparent;"
"}"
"QScrollBar::corner {"
"    background: #e8eef4;"
"}";

const QString Calibrate::TableWidgetCss =
"QTableWidget {"
"   background-color: white;"
"   alternate-background-color: #f5f7fa;"
"   gridline-color: #d0d7de;"
"   font: 10pt 'Segoe UI', 'Microsoft YaHei';"
"   selection-background-color: #cce5ff;"
"   selection-color: #1f2d3d;"
"}"
"QTableWidget::item {"
"   padding: 5px;"
"   border-bottom: 1px solid #e9ecef;"
"}"
"QTableWidget::item:hover {"
"   background-color: #e6f4ff;"
"}"
"QTableWidget::item:selected {"
"   background-color: #cce5ff;"
"   color: #1f2d3d;"
"}";

Calibrate::Calibrate(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Calibrate),
	m_loadTableColor(200, 255, 200),
	m_clickViewSampleId(-1)
{
	ui->setupUi(this);
	this->setWindowFlags(Qt::FramelessWindowHint);

	// 配对样本框由 ui->saveTaskWidgetList (已提升为 PairFrameTableWidget) 在其 paintEvent 中绘制。
	// 不再使用 overlay 子 widget:overlay 与 viewport 的几何同步会形成 Move/Resize 循环,
	// 导致切到结果预览后主线程卡死、内存持续上涨(日志中 T13/T14 事件数飙升即此循环)。
	qRegisterMetaType<QVariant>("QVariant");
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
    connect(ui->saveTaskWidgetList->verticalScrollBar(), &QScrollBar::valueChanged,
            this, [this](){
                ui->saveTaskWidgetList->viewport()->update();
            });

    //导入当天的样本信息
    ImportTodayInfo();

}

void Calibrate::InitSaveTableList(QTableWidget * pTable)
{
	QStringList headerList{ tr("样本号"),tr("添加时间"),tr("条形码"),tr("姓名"),tr("性别"),
		tr("年龄"),tr("科室"),tr("住院号"),tr("床号"),tr("病区号")
		,tr("送检时间"),tr("送检医生"),tr("检验时间"),tr("检验医生")
		,tr("诊断"),tr("备注"),tr("审核医生") };


	pTable->setRowCount(0);  //初始化行
	pTable->setColumnCount(verifierinfo::ITEM_TOTAL); //初始化列

	pTable->setHorizontalHeaderLabels(headerList);


	QFont headerFont;
	headerFont.setFamily("楷体");
	headerFont.setPointSize(12);
	headerFont.setBold(true);

	pTable->setSelectionMode(QAbstractItemView::ExtendedSelection);
	pTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	pTable->horizontalHeader()->setFont(headerFont);
	pTable->horizontalHeader()->setStretchLastSection(true);
	pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
	pTable->horizontalHeader()->setDefaultSectionSize(30);
	pTable->horizontalHeader()->setFixedHeight(35);

	QHeaderView *header1 = pTable->verticalHeader();
	header1->setHidden(true);

	pTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	pTable->setColumnWidth(0, 140);
	pTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	pTable->setColumnWidth(1, 110);
	pTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
	pTable->setColumnWidth(2, 140);

	for (int i = 3; i < headerList.size(); i++) {
		pTable->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
	}

	pTable->setAlternatingRowColors(true);
	pTable->setWordWrap(true);
	pTable->verticalHeader()->setDefaultSectionSize(64);
	pTable->verticalHeader()->setMinimumSectionSize(64);
	pTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	pTable->verticalHeader()->show();


	pTable->horizontalHeader()->setStyleSheet(
		"QHeaderView::section {"
		"   background-color: #2c3e50;"
		"   color: white;"
		"   font: bold 12pt 'Segoe UI', 'Microsoft YaHei';"
		"   padding: 8px 4px;"
		"   border: none;"
		"   border-right: 1px solid #1a2a3a;"
		"}"
		"QHeaderView::section:last {"
		"   border-right: none;"
		"}"
		);

	pTable->setStyleSheet(TableWidgetCss + VScroBarCss);
	pTable->verticalScrollBar()->setStyleSheet(VScroBarCss);
	pTable->horizontalScrollBar()->setStyleSheet(VScroBarCss);

	// 滚动条策略固定为 AlwaysOn,彻底消除垂直/水平滚动条的显隐振荡。
	// 之前 AsNeeded 下,内容尺寸恰处于阈值:显示垂直滚动条→viewport 宽缩 12px→
	// 横向溢出→显示水平滚动条→viewport 高缩 8px→又影响垂直判断→隐藏/显示……
	// 无限振荡(日志中 viewport resize 19s 飙到 19000 次,尺寸在 1691/1703、755/763 间反复跳)。
	pTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	pTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);



	//设置不可编辑列
	QList<int> NoteditableColsList;
	NoteditableColsList << SAMPLEID << ADDSAMPLETIME << BARCODESAMPLE;
	setNoteditableCol(NoteditableColsList);

	updatecommboxInfo();

	TableSelTimeDelegate* pconfigtimedelegate = new TableSelTimeDelegate();
	pTable->setItemDelegateForColumn(SUBMISSIONTIME, pconfigtimedelegate);
	m_pconfigtimedelegate.push_back(pconfigtimedelegate);

	TableSelTimeDelegate* pconfigtimedelegate_other = new TableSelTimeDelegate();
	pTable->setItemDelegateForColumn(INSPECTIONTIME, pconfigtimedelegate_other);
	m_pconfigtimedelegate.push_back(pconfigtimedelegate_other);


    connect(ui->saveTaskWidgetList, &QTableWidget::itemDoubleClicked,
            this, &Calibrate::onTableAboutToEdit);

	pTable->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(pTable, &QTableWidget::customContextMenuRequested,
		this, &Calibrate::showContextpup);


	connect(pTable, SIGNAL(itemChanged(QTableWidgetItem*)),
		this, SLOT(dgvDellClick(QTableWidgetItem*)));

	if (pTable->selectionModel()) {
		connect(pTable->selectionModel(), &QItemSelectionModel::selectionChanged,
			pTable, [pTable](const QItemSelection &, const QItemSelection &) {
				if (pTable->viewport())
					pTable->viewport()->update();
			});
	}

}

void Calibrate::setNoteditableCol(QList<int> cols) {
	foreach(int val_, cols) {
		TableItemDelegate* peneditable = new TableItemDelegate();
		ui->saveTaskWidgetList->setItemDelegateForColumn(val_, peneditable);
		m_eneditdelegate.push_back(peneditable);
	}
}

void Calibrate::_setCommboxDelegate(QList<int> cols)
{
	QMap<int, QString> columnMap = {
		{ SEXSAMPLE, "性别" },
		{ DEPARTMENT, "科别" },
		{ WARDCODE, "病区" },
		{ BEDNUMBER, "床号" },
		{ DIAGNOSIS, "临床诊断" },
		{ REFERTOTHEDOCTOR, "送检医生" },
		{ REVIEWDOCTORS, "审核医生" },
		{ REMARK, "备注" }
	};
	for (int col : cols) {
		if (columnMap.contains(col)) {
			QStringList dataList;
			FullyAutomatedPlatelets::pinstancesqlData()->_obtainPatientInfo(columnMap[col], dataList);
			WidgetDelegate *delegate = new WidgetDelegate(dataList);
			ui->saveTaskWidgetList->setItemDelegateForColumn(col, delegate);
			m_pdelegate.push_back(delegate);
		}
		else if (col == static_cast<int>(MEDICALEXAMINER))
		{
			//检验医生==登录账户
			QStringList Testlist;
			Testlist.push_back(cglobal::g_UserName_str);
			WidgetDelegate *_testingDelegate = new WidgetDelegate(Testlist);
			ui->saveTaskWidgetList->setItemDelegateForColumn(MEDICALEXAMINER, _testingDelegate);
			m_pdelegate.push_back(_testingDelegate);
		}
	}
}



void  Calibrate::MarktableWidgetColor(QTableWidget* table, int row, int cols, const QString& text, const QColor& color)
{
	QTableWidgetItem * item = table->item(row, cols);
	if (!item) {
		item = new QTableWidgetItem();
		table->setItem(row, cols, item);
	}
	item->setText(text);
	item->setTextAlignment(Qt::AlignCenter);
	item->setBackground(color);
}

void Calibrate::SetColumnText(QTableWidget * tablewiget, int row, int col, QString text)
{
	QTableWidgetItem *item = new QTableWidgetItem(text);
	item->setTextAlignment(Qt::AlignCenter);
	tablewiget->setItem(row, col, item);
}


void Calibrate::showContextpup(const QPoint &pos) {
	QTableWidget *ptablewidget = ui->saveTaskWidgetList;

	int clickrow = -1;
	QTableWidgetItem *item = ptablewidget->itemAt(pos); // 获取点击项
	if (item) {
		clickrow = item->row(); // 获取行号
	}
	if (clickrow < 0) {
		return;
	}

	QString clickDate, sampleSex;
	int clickIdNum;
	QTableWidgetItem *idItem = ptablewidget->item(clickrow, SAMPLEID);
	if (!idItem) return;
	QString clickId = idItem->text();
	GlobalData::apartSampleId(clickId, clickDate, clickIdNum);

	m_clickViewSampleId = clickIdNum;

	if (mpdatawidget.isNull()) {
		mpdatawidget.reset(new CustomFixTableView(this));
		if (mpdatawidget.isNull()) {
			QLOG_ERROR() << "Failed to create CustomFixTableView";
			return;
		}
		connect(this, &Calibrate::hideCurveUi, mpdatawidget.data(),
			&CustomFixTableView::hideWithAnimation);
	}

	QTableWidgetItem* targetItem = ptablewidget->item(clickrow, SEXSAMPLE);
	if (targetItem && !targetItem->text().isEmpty()) {
		sampleSex = targetItem->text();
	}
	else {
		QLOG_ERROR() << "选中样本未设置性别,查询对比值失败";
	}

	// 收集样本ID列表
	QList<QString> hanSampleID;
	hanSampleID.reserve(ptablewidget->rowCount());
	for (int row = 0; row < ptablewidget->rowCount(); ++row) {
		targetItem = ptablewidget->item(row, SAMPLEID);
		if (targetItem && !targetItem->text().isEmpty()) {
			hanSampleID.append(targetItem->text());
		}
	}


	mpdatawidget->setCheckBoxState(false, sampleSex, hanSampleID);
	mpdatawidget->setClickViewId(clickId, clickIdNum);
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

	// ========== 重新排序，使配对样本相邻 ==========
	QVector<QSharedPointer<PatientInformationStu>> sortedVec;
	sortedVec.reserve(painterInfovec.size());

	QMap<QString, QVector<QSharedPointer<PatientInformationStu>>> groupMap;
	// 按基础ID分组（去掉F后缀）
	for (const auto& data : painterInfovec) {
		QString baseId = data->sampleId;
		if (baseId.endsWith('F'))
			baseId.chop(1);   // 移除末尾的F
		groupMap[baseId].append(data);
	}

	// 输出每组：先输出不包含F的样本，再输出包含F的样本（如果有）
	for (auto it = groupMap.begin(); it != groupMap.end(); ++it) {
		const auto& group = it.value();
		QSharedPointer<PatientInformationStu> normalSample;
		QSharedPointer<PatientInformationStu> fSample;
		for (const auto& sample : group) {
			if (sample->sampleId.endsWith('F'))
				fSample = sample;
			else
				normalSample = sample;
		}
		if (normalSample) sortedVec.append(normalSample);
		if (fSample) sortedVec.append(fSample);
	}
	// 使用排序后的列表
	painterInfovec = sortedVec;
	//========== 排序结束 ==========

	// 批量设置行数（避免逐行插入）
	pSampleInfoTable->clearContents();  // 清空内容但保留表头
	pSampleInfoTable->setRowCount(painterInfovec.size());  // 一次性分配行

														   //预定义列索引常量（减少重复计算）
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
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::SEXSAMPLE, data->sex_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::AGESAMPLE, QString::number(data->agesample));
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::DEPARTMENT, data->department_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::HOSPITALIZATIONNUMBER, data->hospitalzationnumber_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::BEDNUMBER, data->bednumber_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::WARDCODE, data->wardcode_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::SUBMISSIONTIME, data->submissiontime_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::REFERTOTHEDOCTOR, data->refertithedoctor_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::INSPECTIONTIME, data->inspectiontime_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::MEDICALEXAMINER, data->medicalexaminer_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::DIAGNOSIS, data->diagnosis_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::REMARK, data->remark_);
		SetColumnText(pSampleInfoTable, tRow, verifierinfo::REVIEWDOCTORS, data->reviewdoctors_);
		applyDefaultRowHeight(pSampleInfoTable, tRow);

	}
	pSampleInfoTable->setUpdatesEnabled(true);
	rebuildPairRowsCache();
	refreshPairFrame();
	syncTableLayout();
}

void  Calibrate::addfollowUpSample(const QString sampleidF) {
	m_followUpSample.insert(sampleidF);
}

void Calibrate::addpatientsqltable(QString sampleId, QString addtime, QString barcode,
                                   QString testProject, bool isEmergency)
{
	bool  hasfollowup = hasfollowUpSample(sampleId);
	QString insertSqlid;
	if (hasfollowup) {
		insertSqlid = QString("%1F").arg(sampleId);
	}
	else {
		insertSqlid = sampleId;
	}

	// 插入样本数据（增加错误检查）
	if (!FullyAutomatedPlatelets::pinstancesqlData()->insertAddSampleData(insertSqlid, addtime, barcode)) {
		QLOG_ERROR() << "样本信息插入失败：" << insertSqlid;
		return;
	}

	// 插入曲线数据
	if (!FullyAutomatedPlatelets::pinstancesqlData()->insertCurveRecord(insertSqlid)) {
		QLOG_ERROR() << "曲线记录插入失败：" << insertSqlid;
		return;
	}

	// 获取待测试试剂列表
	QList<quint8> waitTestReagentList;
	GlobalData::sycnTestingReagnetNum(waitTestReagentList, testProject);

	// 更新曲线和结果数据
	QString waitTest = "null";
	for (int i = 0; i < waitTestReagentList.count(); i++) {
		quint8 reagent = waitTestReagentList.at(i);
		FullyAutomatedPlatelets::pinstancesqlData()->updateTestCurveDataTale(insertSqlid, reagent, waitTest);

		static const QMap<quint8, QString> REAGENT_COLUMN_MAP = {
			{ AA_REAGENT,  "AA" },
			{ ADP_REAGENT, "ADP" },
			{ EPI_REAGENT, "EPI" },
			{ COL_REAGENT, "COL" },
			{ RIS_REAGENT, "RIS" }
		};
		auto it = REAGENT_COLUMN_MAP.constFind(reagent);
		if (it != REAGENT_COLUMN_MAP.constEnd()) {
			FullyAutomatedPlatelets::pinstancesqlData()->updateTestResultTable(insertSqlid, it.value(), waitTest);
		}
	}

	// 更新UI表格
	QTableWidget *pSampleInfoTable = ui->saveTaskWidgetList;
	const QSignalBlocker blocker(pSampleInfoTable);
	int iRow = pSampleInfoTable->rowCount();

	// 如果是复查样本（以F结尾），尝试插入到原样本的下方
	if (insertSqlid.endsWith('F')) {
		QString baseId = insertSqlid.left(insertSqlid.length() - 1);
		for (int row = 0; row < pSampleInfoTable->rowCount(); ++row) {
			QTableWidgetItem *item = pSampleInfoTable->item(row, SAMPLEID);
			if (item && item->text() == baseId) {
				iRow = row + 1;  // 插入到原样本的下一行
				break;
			}
		}
	}

	pSampleInfoTable->insertRow(iRow);
	applyDefaultRowHeight(pSampleInfoTable, iRow);
	SetColumnText(pSampleInfoTable, iRow, ADDSAMPLETIME, addtime);
	SetColumnText(pSampleInfoTable, iRow, SAMPLEID, insertSqlid);
	SetColumnText(pSampleInfoTable, iRow, BARCODESAMPLE, barcode);
	SetColumnText(pSampleInfoTable, iRow, NAMESAMPLE, "");
	if (QTableWidgetItem *idCell = pSampleInfoTable->item(iRow, SAMPLEID)) {
		QString tip;
		if (hasfollowup) {
			tip += tr("复查样本（原号+F）");
			idCell->setTextColor(QColor(41, 128, 185));
		}
		if (isEmergency) {
			if (!tip.isEmpty()) {
				tip += tr(" / ");
			}
			tip += tr("急诊优先");
			idCell->setTextColor(QColor(192, 57, 43));
			QFont idFont = idCell->font();
			idFont.setBold(true);
			idCell->setFont(idFont);
		}
		idCell->setToolTip(tip);
	}
	rebuildPairRowsCache();
	refreshPairFrame();
	syncTableLayout();
}

void Calibrate::dgvDellClick(QTableWidgetItem* item) {

	if (!item) return;

	int row = item->row();
	int col = item->column();
	QTableWidgetItem *idItem = ui->saveTaskWidgetList->item(row, SAMPLEID);
	if (!idItem) return;
	QString sampleId = idItem->text();
	if (sampleId.isEmpty()) return;


	static const std::map<int, QString> columnToField = {
		{ NAMESAMPLE, "姓名" },
		{ SEXSAMPLE, "性别" },
		{ AGESAMPLE, "年龄" },
		{ DEPARTMENT, "科别" },
		{ HOSPITALIZATIONNUMBER, "住院号" },
		{ BEDNUMBER, "床号" },
		{ WARDCODE, "病区" },
		{ SUBMISSIONTIME, "送检时间" },
		{ REFERTOTHEDOCTOR, "送检医生" },
		{ INSPECTIONTIME, "检验时间" },
		{ MEDICALEXAMINER, "检验医生" },
		{ DIAGNOSIS, "临床诊断" },
		{ REMARK, "备注" },
		{ REVIEWDOCTORS, "审核医生" }
	};
	auto it = columnToField.find(col);
	if (it != columnToField.end()) {
		QString fieldName = it->second;
		FullyAutomatedPlatelets::pinstancesqlData()->updateTestResultTable(sampleId, fieldName, item->text());
	}
}

//设置下拉框代理
void Calibrate::updatecommboxInfo() {
	QList<int> DelegatecolList;
	DelegatecolList << SEXSAMPLE << DEPARTMENT << BEDNUMBER << WARDCODE
		<< REFERTOTHEDOCTOR << MEDICALEXAMINER
		<< DIAGNOSIS << REMARK << REVIEWDOCTORS;
	_setCommboxDelegate(DelegatecolList);
}


//取消任务删除患者样本
int  Calibrate::cancelSampleResultItem(const QString &cancelId)
{
	QTableWidget *table = ui->saveTaskWidgetList;
	if (!table) return 0;

	const QSignalBlocker blocker(table);
	const QString targetWithF = cancelId + "F";   // 预计算目标字符串
	int removedCount = 0;

	// 逆向遍历，安全删除多行
	for (int row = table->rowCount() - 1; row >= 0; --row) {
		QTableWidgetItem *idItem = table->item(row, SAMPLEID);
		if (!idItem) continue;

		const QString sampleId = idItem->text();
		if (sampleId == cancelId || sampleId == targetWithF) {
			table->removeRow(row);
			++removedCount;
		}
	}
	rebuildPairRowsCache();
	refreshPairFrame();
	syncTableLayout();
	return removedCount;
}

void Calibrate::onsycnDelOthertableWidgetRow(const QString delSameId) {

	cancelSampleResultItem(delSameId);
}

void Calibrate::recvSampleTestingErr(const QString& ErrSampleid, const quint8 &channelIdx)
{
	QTableWidget *pSampleInfoTable = ui->saveTaskWidgetList;
	const QSignalBlocker blocker(pSampleInfoTable);
	for (int i = pSampleInfoTable->rowCount() - 1; i >= 0; --i) {
		QTableWidgetItem *item = pSampleInfoTable->item(i, SAMPLEID);
		if (item && item->text() == ErrSampleid) {
			item->setBackground(QColor(255, 0, 0)); // 设置背景颜色为红色
			FullyAutomatedPlatelets::pinstanceTesting()->giveupSampleChannelFlash(false, channelIdx);
			return;
		}
	}
}

void Calibrate::applyDefaultRowHeight(QTableWidget *table, int row)
{
	if (!table || row < 0 || row >= table->rowCount()) {
		return;
	}
	const int rowH = qMax(64, table->verticalHeader()->defaultSectionSize());
	table->setRowHeight(row, rowH);
}

void Calibrate::syncTableLayout()
{
	QTableWidget *table = ui->saveTaskWidgetList;
	if (!table) {
		return;
	}
	const int rowH = qMax(64, table->verticalHeader()->defaultSectionSize());
	for (int row = 0; row < table->rowCount(); ++row) {
		if (table->rowHeight(row) != rowH) {
			table->setRowHeight(row, rowH);
		}
	}
	ui->saveTaskWidgetList->syncLayout();
	if (QScrollBar *vs = table->verticalScrollBar()) {
		vs->update();
	}
}

void Calibrate::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	onPreviewPageShown();
}

void Calibrate::onPreviewPageShown()
{
	QTableWidget *table = ui->saveTaskWidgetList;
	if (!table) {
		return;
	}

	QVector<QSharedPointer<PatientInformationStu>> sqlRows;
	const QString targetdate = GlobalData::ObatinCreatSampleTime();
	if (!FullyAutomatedPlatelets::pinstancesqlData()->getOneDayTestResultData(targetdate, sqlRows)) {
		syncTableLayout();
		return;
	}
	if (sqlRows.size() != table->rowCount()) {
		QLOG_WARN() << QString::fromUtf8("结果预览行数与数据库不一致，界面=%1 数据库=%2，重新加载")
					   .arg(table->rowCount()).arg(sqlRows.size());
		ImportTodayInfo();
		return;
	}
	syncTableLayout();
}

void Calibrate::refreshPairFrame() {
	// 配对行号已由 rebuildPairRowsCache 在数据变化时预算并缓存,
	// 这里把缓存推给 PairFrameTableWidget,由其 paintEvent 绘制配对框。
	// 不再使用 overlay 子 widget(会与 viewport 形成 Move/Resize 几何循环)。
	// ui->saveTaskWidgetList 已被 calibrate.ui 提升为 PairFrameTableWidget*,可直接调用。
	ui->saveTaskWidgetList->setPairRows(m_pairRowsCache);
}

void Calibrate::rebuildPairRowsCache()
{
	m_pairRowsCache.clear();
	QTableWidget *table = ui->saveTaskWidgetList;
	if (!table || table->rowCount() == 0) return;

	// 第一步:所有带F的行
	for (int row = 0; row < table->rowCount(); ++row) {
		QTableWidgetItem *item = table->item(row, SAMPLEID);
		if (item && item->text().endsWith('F'))
			m_pairRowsCache.insert(row);
	}
	// 第二步:不带F但存在配对F的行
	for (int row = 0; row < table->rowCount(); ++row) {
		QTableWidgetItem *item = table->item(row, SAMPLEID);
		if (!item || item->text().endsWith('F')) continue;
		const QString fSampleId = item->text() + 'F';
		for (int checkRow = 0; checkRow < table->rowCount(); ++checkRow) {
			QTableWidgetItem *checkItem = table->item(checkRow, SAMPLEID);
			if (checkItem && checkItem->text() == fSampleId) {
				m_pairRowsCache.insert(row);
				break;
			}
		}
	}
}

void Calibrate::drawSamplePairFrame()
{
	// 统一走 refreshPairFrame:由 PairFrameTableWidget 的 paintEvent 绘制配对框。
	// 不再使用 overlay 子 widget(会与 viewport 形成 Move/Resize 几何循环)。
	refreshPairFrame();
}


void Calibrate::onTableAboutToEdit(QTableWidgetItem *item)
{
    if (item && item->column() == NAMESAMPLE) { // 也可以对所有列生效
        if (!mpdatawidget.isNull() && mpdatawidget->isVisible()) {
            mpdatawidget->hideWithAnimation(); // 或直接 hide()
        }
    }
}
