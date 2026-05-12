#include "configureprojectitem.h"
#include "ui_configureprojectitem.h"
#include <QScrollBar>
#include <QDebug>
#include <QVBoxLayout>
#include "cglobal.h"
#include "loginui.h"
#include "testing.h"
#include <operclass/fullyautomatedplatelets.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif

ConfigureProjectItem::ConfigureProjectItem(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConfigureProjectItem),
	m_titleBar(nullptr),
	m_mainLayout(nullptr)
{
	ui->setupUi(this);

	// 隐藏默认的标题栏和边框
	setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	//setAttribute(Qt::WA_TranslucentBackground);

	// 设置窗口固定大小（保持原来的大小）
	setFixedSize(600, 420);

	// 初始化自定义标题栏
	initTitleBar();

	// 设置窗口标题
	m_titleBar->setTitle(tr("配置测试项目"));

	// 连接标题栏信号
	connect(m_titleBar, &CustomTitleBar::closeRequested, this, &ConfigureProjectItem::close);
	connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &ConfigureProjectItem::showMinimized);

	ui->groupBox_group->setTitle(tr("测试项目"));

	m_checkBoxList.clear();
	if (parent)
	{
		QRect rect = parent->geometry();
		int x = rect.x() + rect.width() / 2 - this->width() / 2;
		int y = rect.y() + rect.height() / 2 - this->height() / 2;
		this->move(x, y);
	}
	m_ptestingradioList << ui->radioButton_AA << ui->radioButton_ADP << ui->radioButton_EPI
		<< ui->radioButton_COL << ui->radioButton_RIS;

	//-组合
	QObject::connect(ui->toolButton_LossGroup, &QToolButton::clicked, this, [=]() { DeleteReagentGroup(); });

	//+组合
	QObject::connect(ui->toolButton_addtable, &QToolButton::clicked, this, [=]() { AddGroupReagentTest(); });

	//退出
	QObject::connect(ui->toolButtonCancel, &QToolButton::clicked, this, [=]() { this->close(); });

	//确定
	QObject::connect(ui->toolButtonSaved, &QToolButton::clicked, this, [=]() { ConfigTestingReagnet(); });

	// 设置窗口激活状态
	m_titleBar->setActive(true);
}

ConfigureProjectItem::~ConfigureProjectItem()
{
	qDeleteAll(m_checkBoxList);
	QLOG_DEBUG() << "析构添加任务对象" << __FUNCTION__ << endl;
	delete ui;
}

void ConfigureProjectItem::initTitleBar()
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

	// 创建自定义标题栏
	m_titleBar = new CustomTitleBar(this);
	m_titleBar->setTitle(tr("配置测试项目"));

    connect(m_titleBar, &CustomTitleBar::closeRequested, this, &ConfigureProjectItem::onCloseRequested);
    connect(m_titleBar, &CustomTitleBar::minimizeRequested, this, &ConfigureProjectItem::onMinimizeRequested);
    connect(m_titleBar, &CustomTitleBar::maximizeRequested, this, &ConfigureProjectItem::onMaximizeRequested);


	if (QBoxLayout *boxLayout = qobject_cast<QBoxLayout*>(existingLayout)) {
		boxLayout->insertWidget(0, m_titleBar);
	}
	else {
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

void ConfigureProjectItem::_initstyle()
{
	int n = 0;
	QStringList _projectname;
	_projectname << tr("AA") << tr("ADP") << tr("EPI") << tr("COL") << tr("RIS");
	foreach(QRadioButton *pvar, m_ptestingradioList)
	{
		pvar->setText(_projectname.at(n));
		pvar->setAutoExclusive(false);
		// 设置RadioButton样式 - 医疗浅蓝风格
		pvar->setStyleSheet("QRadioButton {"
			"   font: 14px '微软雅黑', '楷体';"
			"   color: #1A4D6B;"
			"   spacing: 8px;"
			"}"
			"QRadioButton::indicator {"
			"   width: 20px;"
			"   height: 20px;"
			"   border-radius: 10px;"
			"}"
			"QRadioButton::indicator:unchecked {"
			"   background-color: #FFFFFF;"
			"   border: 1.5px solid #AACDE2;"
			"}"
			"QRadioButton::indicator:unchecked:hover {"
			"   border: 1.5px solid #5FA8D3;"
			"   background-color: #F0F9FF;"
			"}"
			"QRadioButton::indicator:checked {"
			"   background-color: #5FA8D3;"
			"   border: 1.5px solid #5FA8D3;"
			"}"
			"QRadioButton::indicator:checked:after {"
			"   content: '●';"
			"   color: white;"
			"   font-size: 11px;"
			"   display: block;"
			"   text-align: center;"
			"   line-height: 17px;"
			"}"
			"QRadioButton::indicator:checked:hover {"
			"   background-color: #3A86A8;"
			"   border: 1.5px solid #3A86A8;"
			"}");
		connect(pvar, &QRadioButton::clicked, this, [=]() {
			if (pvar->isChecked() == true) {
				CancelReagentGroup();
			}
		});
		n++;
	}

	QString pattern_str_ScrollBar = "QScrollBar{background:#E0F0F8; height:8px; border-radius:4px;}"
		"QScrollBar::handle{background:#5FA8D3; border-radius:4px;}"
		"QScrollBar::handle:hover{background:#3A86A8;}"
		"QScrollBar::sub-line{background:transparent;}"
		"QScrollBar::add-line{background:transparent;}";

	QStringList TableTitleList;
	TableTitleList << tr("组合试剂") << tr("确认");
	ui->tableWidget_group->setFont(QFont("微软雅黑", 12));
	ui->tableWidget_group->setColumnCount(2);
	QFont font = ui->tableWidget_group->horizontalHeader()->font();
	font.setBold(true);
	ui->tableWidget_group->horizontalHeader()->setFont(font);
	ui->tableWidget_group->setHorizontalHeaderLabels(TableTitleList);

	ui->tableWidget_group->verticalHeader()->setVisible(false);
	ui->tableWidget_group->setSelectionMode(QAbstractItemView::ExtendedSelection);
	ui->tableWidget_group->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->tableWidget_group->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 医疗浅蓝风格表格样式
	QString TableCss = "QTableWidget{"
		"   background-color: #FFFFFF;"
		"   alternate-background-color: #F2F9FC;"
		"   gridline-color: #C8E0F0;"
		"   border: 1px solid #B8D9E6;"
		"   border-radius: 8px;"
		"}"
		"QTableWidget::item:hover{"
		"   background-color: #D9F0F8;"
		"}"
		"QTableWidget::item:selected{"
		"   background-color: #2C7DA0;"
		"   color: white;"
		"}"
		"QHeaderView::section{"
		"   background-color: #5FA8D3;"
		"   color: white;"
		"   padding: 8px;"
		"   border: none;"
		"   border-right: 1px solid #3A86A8;"
		"   font-weight: bold;"
		"   font-size: 13px;"
		"}"
		"QHeaderView::section:last{"
		"   border-right: none;"
		"}";

	ui->tableWidget_group->setStyleSheet(TableCss);
	ui->tableWidget_group->setColumnWidth((int)Table_TestReagentroup, 350);
	ui->tableWidget_group->horizontalScrollBar()->setStyleSheet(pattern_str_ScrollBar);
	ui->tableWidget_group->verticalScrollBar()->setStyleSheet(pattern_str_ScrollBar);

	//设置单元格大小
	ui->tableWidget_group->horizontalHeader()->setDefaultSectionSize(180);
	ui->tableWidget_group->verticalHeader()->setDefaultSectionSize(40);
	ui->tableWidget_group->setColumnWidth((int)Table_TestReagentroup, 350);
	ui->tableWidget_group->setColumnWidth((int)Table_CheckState, 80);
	ui->tableWidget_group->horizontalHeader()->setStretchLastSection(true);

	// 设置底部按钮样式
	QString btnStyle = "QToolButton{"
		"   border: 1px solid #5FA8D3;"
		"   background-color: #5FA8D3;"
		"   border-radius: 8px;"
		"   padding: 0 15px;"
		"   font: 14px '微软雅黑', '楷体';"
		"   color: white;"
		"   min-width: 100px;"
		"   min-height: 32px;"
		"}"
		"QToolButton:hover{"
		"   background-color: #3A86A8;"
		"   border: 1px solid #3A86A8;"
		"}"
		"QToolButton:pressed{"
		"   background-color: #2C6D8A;"
		"   border: 1px solid #2C6D8A;"
		"}";
	ui->toolButtonSaved->setStyleSheet(btnStyle);
	ui->toolButtonCancel->setStyleSheet(btnStyle);
	ui->toolButton_addtable->setStyleSheet(btnStyle);
	ui->toolButton_LossGroup->setStyleSheet(btnStyle);

	CreatTableNumTab();
	return;
}

// 以下所有函数保持不变...
void ConfigureProjectItem::CreatTableNumTab()
{
	int TotalRows = 0;
	QMap<int, QString> AllSaveReagentTask;
	AllSaveReagentTask.clear();
	QTableWidgetItem * Reagentinfo_item;
	FullyAutomatedPlatelets::pinstancesqlData()->GetTestReagentGroup(AllSaveReagentTask);
	auto iter = AllSaveReagentTask.constBegin();
	while (iter != AllSaveReagentTask.constEnd())
	{
		TotalRows = ui->tableWidget_group->rowCount();
		ui->tableWidget_group->insertRow(TotalRows);

		Reagentinfo_item = new QTableWidgetItem(iter.value());
		Reagentinfo_item->setTextAlignment(Qt::AlignCenter);
		Reagentinfo_item->setFont(QFont("微软雅黑", 12));
		ui->tableWidget_group->setItem(TotalRows, Table_TestReagentroup, Reagentinfo_item);

		AddCheckBoxInntoTable(TotalRows);

		iter++;
	}
}

void ConfigureProjectItem::RadioButtonAllNoChecked()
{
	for (auto radiobtn : m_ptestingradioList)
	{
		radiobtn->setChecked(false);
	}
}

void ConfigureProjectItem::AddCheckBoxInntoTable(int Rows)
{
	//++复选框
	QCheckBox * pCheckBox;
	QWidget * pBoxWidget;
	QHBoxLayout * hBox;

	pCheckBox = new QCheckBox();
	pBoxWidget = new QWidget(this);
	hBox = new QHBoxLayout();

	// 医疗浅蓝风格复选框样式
	pCheckBox->setStyleSheet("QCheckBox::indicator{"
		"   width: 20px;"
		"   height: 20px;"
		"   border-radius: 4px;"
		"}"
		"QCheckBox::indicator:unchecked{"
		"   background-color: #FFFFFF;"
		"   border: 1.5px solid #AACDE2;"
		"}"
		"QCheckBox::indicator:unchecked:hover{"
		"   border: 1.5px solid #5FA8D3;"
		"   background-color: #F0F9FF;"
		"}"
		"QCheckBox::indicator:checked{"
		"   background-color: #5FA8D3;"
		"   border: 1.5px solid #5FA8D3;"
		"}"
		"QCheckBox::indicator:checked:after{"
		"   content: '✓';"
		"   color: white;"
		"   font-size: 13px;"
		"   font-weight: bold;"
		"   display: block;"
		"   text-align: center;"
		"   line-height: 17px;"
		"}"
		"QCheckBox::indicator:checked:hover{"
		"   background-color: #3A86A8;"
		"   border: 1.5px solid #3A86A8;"
		"}");

	hBox->addWidget(pCheckBox);
	pCheckBox->setChecked(false);
	hBox->setMargin(0);
	hBox->setAlignment(pCheckBox, Qt::AlignCenter);
	pBoxWidget->setLayout(hBox);
	ui->tableWidget_group->setCellWidget(Rows, Table_CheckState, pBoxWidget);
	QObject::connect(pCheckBox, &QCheckBox::clicked, this, [=]() {
		bool pchecked = pCheckBox->isChecked();
		ClickCheckBox(pchecked);
	});
	return;
}

void ConfigureProjectItem::ClickCheckBox(bool pchecked)
{
	QCheckBox *pIndexCheckBox = qobject_cast<QCheckBox *>(sender());
	if (pchecked == true)
	{
		RadioButtonAllNoChecked();
		int Rows = ui->tableWidget_group->rowCount();
		for (int n = 0; n < Rows; n++)
		{
			QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
			QCheckBox* pCheck = widget->findChild<QCheckBox *>();
			if (pCheck != pIndexCheckBox)
			{
				pCheck->blockSignals(true);
				pCheck->setChecked(false);
				pCheck->blockSignals(false);
			}
		}
	}
	update();
	return;
}

void ConfigureProjectItem::CancelReagentGroup()
{
	int Rows = ui->tableWidget_group->rowCount();
	for (int n = 0; n < Rows; n++)
	{
		QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
		QCheckBox* pCheck = widget->findChild<QCheckBox *>();
		if (pCheck->isChecked())
		{
			pCheck->blockSignals(true);
			pCheck->setChecked(false);
			pCheck->blockSignals(false);
		}
	}
	update();
	return;
}

void ConfigureProjectItem::AddGroupReagentTest()
{
	QStringList AddGroupReagent;
	QString SavedGroup = "";
	AddGroupReagent.clear();
	for (auto radiobtn : m_ptestingradioList)
	{
		bool bchecked = radiobtn->isChecked();
		if (bchecked)
			AddGroupReagent.push_back(radiobtn->text());
	}
	if (AddGroupReagent.size() == 0) {
		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
		FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("组合试剂失败", "请选择要编组的试剂!");
		return;
	}
	ShowTestGroupText(AddGroupReagent, SavedGroup);
	QMap<int, QString> HadGroup;
	FullyAutomatedPlatelets::pinstancesqlData()->GetTestReagentGroup(HadGroup);
	auto  it = HadGroup.begin();
	while (it != HadGroup.end())
	{
		if (it.value() == SavedGroup)
		{
			this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
			FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder("组合试剂失败", "组合已存在!");
			return;
		}
		it++;
	}

	//添加
	int Rows = ui->tableWidget_group->rowCount();
	ui->tableWidget_group->insertRow(Rows);

	QTableWidgetItem *Reagentinfo_item = new QTableWidgetItem(SavedGroup);
	Reagentinfo_item->setTextAlignment(Qt::AlignCenter);
	Reagentinfo_item->setFont(QFont("微软雅黑", 12));
	ui->tableWidget_group->setItem(Rows, (int)Table_TestReagentroup, Reagentinfo_item);

	AddCheckBoxInntoTable(Rows);

	//保存到数据库中
	FullyAutomatedPlatelets::pinstancesqlData()->AddTestReagentGroup(ui->tableWidget_group->rowCount(), SavedGroup);

	RadioButtonAllNoChecked();
	return;
}

void ConfigureProjectItem::DeleteReagentGroup()
{
	int Rows = ui->tableWidget_group->rowCount();
	for (int n = 0; n < Rows; n++)
	{
		QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
		QCheckBox* pCheck = widget->findChild<QCheckBox *>();
		if (pCheck->isChecked() == true)
		{
			QString reagentstr = ui->tableWidget_group->item(n, Table_TestReagentroup)->text();
			FullyAutomatedPlatelets::pinstancesqlData()->DeleteTestReagentGroup(reagentstr);
			ui->tableWidget_group->removeRow(n);
			m_checkBoxList.removeOne(pCheck);
			break;
		}
	}

	Rows = ui->tableWidget_group->rowCount();
	for (int i = 0; i < Rows; i++)
	{
		int index = i + 1;
		QString reagentstr = ui->tableWidget_group->item(i, Table_TestReagentroup)->text();
		FullyAutomatedPlatelets::pinstancesqlData()->UpdateGroupReagent(index, reagentstr);
	}
	return;
}

bool ConfigureProjectItem::SelectRadittom()
{
	for (int i = 0; i < m_ptestingradioList.size(); i++)
	{
		if (m_ptestingradioList.at(i)->isChecked() == true)
			return false;
	}
	return true;
}

void ConfigureProjectItem::ShowTestGroupText(QStringList GroupReagent, QString &ShowText)
{
	for (int i = 0; i < GroupReagent.size(); i++)
	{
		if (i == GroupReagent.size() - 1)
			ShowText = ShowText + GroupReagent.at(i);
		else
			ShowText = ShowText + GroupReagent.at(i) + "|";
	}
	return;
}

void ConfigureProjectItem::ConfigTestingReagnet()
{
	QString reagent_str = "";
	QStringList  ReagnetGroup;
	ReagnetGroup.clear();
	for (auto radiobtn : m_ptestingradioList)
	{
		if (radiobtn->isChecked() == true)
			ReagnetGroup.push_back(radiobtn->text());
	}

	QStringList ReagentGroupCheck;
	ReagentGroupCheck.clear();
	int Rows = ui->tableWidget_group->rowCount();
	for (int n = 0; n < Rows; n++)
	{
		QWidget *widget = ui->tableWidget_group->cellWidget(n, Table_CheckState);
		QCheckBox* pCheck = widget->findChild<QCheckBox *>();
		if (pCheck->isChecked() == true) {
			QString reagentstr = ui->tableWidget_group->item(n, Table_TestReagentroup)->text();
			ReagentGroupCheck.push_back(reagentstr);
		}
	}

	if (ReagnetGroup.size() != 0 && ReagentGroupCheck.size() == 0)
	{
		ShowTestGroupText(ReagnetGroup, reagent_str);
	}
	else if (ReagnetGroup.size() == 0 && ReagentGroupCheck.size() == 0)
	{
		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
		FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(tr("保存试剂失败"), tr("请选择测试试剂种类!"));
	}
	else if (ReagnetGroup.size() != 0 && ReagentGroupCheck.size() != 0)
	{
		this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
		FullyAutomatedPlatelets::mainWindow()->ThreadSafeReminder(tr("保存试剂失败"), tr("请选择单项或试剂组测试种类!"));
	}
	else if (ReagnetGroup.size() == 0 && ReagentGroupCheck.size() != 0)
	{
		reagent_str = ReagentGroupCheck.first();
	}
	if (!reagent_str.isEmpty())
	{
		if (m_singleitem == false)
			emit _setallsampletestproject(reagent_str);
		else
			emit setsinglesampleproject(m_rows, m_clos, reagent_str);
		this->close();
	}

	return;
}

void ConfigureProjectItem::Slot_ChangeSingleItem(int rows, int clos, bool signal)
{
	m_rows = rows;
	m_clos = clos;
	m_singleitem = signal;
}

void ConfigureProjectItem::closeEvent(QCloseEvent *event)
{
	event->accept();
	close();
}

void ConfigureProjectItem::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Return)
	{
		this->close();
	}
}


void ConfigureProjectItem::onCloseRequested()
{
    close();
}

void ConfigureProjectItem::onMinimizeRequested()
{
    showMinimized();
}

void ConfigureProjectItem::onMaximizeRequested()
{
    if (isMaximized()) {
        showNormal();
        m_titleBar->updateMaximizeButton(false);
    } else {
        showMaximized();
        m_titleBar->updateMaximizeButton(true);
    }
}
