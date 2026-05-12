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


#if defined(_MSC_VER) && (_MSC_VER >= 1900)
#pragma execution_character_set("utf-8")
#endif


/**
* @brief 比较两个工具按钮的索引大小
* @param holenum 第一个工具按钮
* @param endhole 第二个工具按钮
* @return 如果第一个按钮索引小于第二个按钮索引，返回true；否则返回false
*/
bool compairtool(QToolButton *holenum, QToolButton *endhole)
{
	// 空指针安全检查
	if (!holenum || !endhole) return false;

	// Lambda函数：从对象名中提取数字索引
	auto getIndex = [](const QToolButton* btn) {
		return QUIUtils::StringFindintnum(btn->objectName());
	};

	return getIndex(holenum) < getIndex(endhole);
}

/**
* @brief 根据对象名中的数字索引对两个QWidget进行排序比较
* @param fWidget 第一个QWidget
* @param eWidget 第二个QWidget
* @return 如果第一个Widget的索引小于第二个，返回true；否则返回false
*/
bool sortWidgetNumLoc(QWidget *fWidget, QWidget *eWidget)
{
	// 空指针安全检查
	if (!fWidget || !eWidget) return false;

	// 从对象名中提取数字索引
	int index_s = QUIUtils::StringFindintnum(fWidget->objectName());
	int index_e = QUIUtils::StringFindintnum(eWidget->objectName());

	return index_s < index_e;
}


/**
* @brief CustomPlot构造函数
* @param parent 父窗口指针
*
* 初始化自定义坐标调整界面，设置窗口属性、字体、LED指示灯等
*/
CustomPlot::CustomPlot(QWidget *parent) :
	QWidget(parent),
	ui(std::make_unique<Ui::CustomPlot>()),  // 使用智能指针管理UI资源
	m_downhigh(0),                           ///< 抓手下降高度初始化为0
	mcodeNum(0),                             ///< 代码编号初始化为0
	m_CalibrationArea(-1),                   ///< 校准区域初始化为-1（未选择）
	mbstopTestadjust(false)                  ///< 测试调整停止标志初始化为false
{
	QLOG_INFO() << "CustomPlot构造函数开始";

	// 初始化UI界面
	ui->setupUi(this);

	// 设置窗口标题
	this->setWindowTitle(tr("坐标位置微调"));

	// 设置主字体
	mfont.setFamily("楷体");
	mfont.setPointSize(10);  ///< 设置文字大小

							 // 设置自定义字体
	mcustFont.setFamily("楷体");
	mcustFont.setPointSize(12);

	// 设置LED指示灯
	auto* led = ui->widget_displayled;
	led->setColors(QSimpleLed::CUSTOM);
	led->setFixedSize(32, 32);
	led->setCustomOnColor0(QColor(0, 127, 0));   ///< 暗绿色
	led->setCustomOnColor1(QColor(0, 255, 0));   ///< 亮绿色
	led->setCustomOffColor0(QColor(127, 0, 0));  ///< 暗红色
	led->setCustomOffColor1(QColor(255, 0, 0));  ///< 亮红色
	led->setStates(QSimpleLed::LEDSTATES::OFF);  ///< 初始状态为关闭

												 // 初始化SpinBox样式
	initSpinBoxStyle();

	QLOG_INFO() << "CustomPlot构造函数完成";
}

/**
* @brief CustomPlot析构函数
*
* 清理资源，断开所有信号连接，智能指针自动管理UI资源
*/
CustomPlot::~CustomPlot()
{
	QLOG_DEBUG() << "CustomPlot析构函数开始";

	// 断开所有可能引起问题的信号连接
	disconnect(this, nullptr, nullptr, nullptr);

	// 智能指针自动管理资源，无需手动删除
	// ui和mpTestCaseRun由unique_ptr自动管理
	// m_CheckGroupBox和mbloodBtnGroupbox是子对象，由Qt的父子机制管理

	QLOG_DEBUG() << "CustomPlot析构完成";
}

// ============================================================================
// 公共按钮配置方法
// ============================================================================

/**
* @brief 配置单个工具按钮
* @param button 按钮指针
* @param text 按钮文本
* @param clickHandler 点击事件处理函数
*/
void CustomPlot::configureToolButton(QToolButton* button, const QString& text,
	const std::function<void()>& clickHandler)
{
	if (!button) {
		QLOG_WARN() << "configureToolButton: 按钮指针为空";
		return;
	}

	button->setText(text);
	button->setFont(mfont);
	button->setCheckable(true);
	button->setAutoExclusive(true);
	button->setStyleSheet(m_Cssbtn);

	// 安全建立连接（避免重复）
    disconnect(button, &QToolButton::clicked, nullptr, nullptr);
	if (clickHandler) {
		connect(button, &QToolButton::clicked, this, clickHandler, Qt::UniqueConnection);
	}
}

/**
* @brief 配置按钮组中的按钮
* @param button 按钮指针
* @param group 按钮组指针
* @param clickHandler 点击事件处理函数
*/
void CustomPlot::configureButtonGroup(QToolButton* button, QButtonGroup* group,
	const std::function<void()>& clickHandler)
{
	if (!button || !group) {
		QLOG_WARN() << "configureButtonGroup: 按钮或按钮组指针为空";
		return;
	}

	// 先配置按钮
	configureToolButton(button, button->text(), clickHandler);

	// 安全添加到按钮组
	if (group->buttons().contains(button)) {
		group->removeButton(button);
	}
	group->addButton(button);
}

// ============================================================================
// 设备配置辅助方法
// ============================================================================

/**
* @brief 获取血样区显示数量
* @param model 设备型号
* @return 显示数量
*/
int CustomPlot::getBloodZoneShowCount(quint8 model) const
{
	static const QHash<quint8, int> kModelToShowNum = {
		{ KS600, 6 },
		{ KS800, 9 },
		{ KS1200, 12 }
	};

	const int showNum = kModelToShowNum.value(model, 12);
	if (!kModelToShowNum.contains(model)) {
		QLOG_WARN() << "getBloodZoneShowCount: 未知的设备型号" << model
			<< ", 使用默认值12";
	}

	return showNum;
}

/**
* @brief 获取试管托盘显示数量
* @param model 设备型号
* @return 显示数量
*/
int CustomPlot::getTubeTrayShowCount(quint8 model) const
{
	static const QHash<quint8, int> kModelShowNum = {
		{ KS600, 2 },
		{ KS800, 3 },
		{ KS1200, 4 }
	};

	const int showNum = kModelShowNum.value(model, 4);
	if (!kModelShowNum.contains(model)) {
		QLOG_WARN() << "getTubeTrayShowCount: 未知的设备型号" << model
			<< ", 使用默认值4";
	}

	return showNum;
}

void CustomPlot::initSpinBoxStyle()
{
	// X坐标SpinBox样式 - 现代化设计
	m_SpinboxsheetX =
		"QSpinBox {"
		"   background: linear-gradient(to bottom, #ffffff, #f8f9fa);"
        "   border: 1px solid #e0e6ed;"
        "   border-radius: 2px;"
		"   text-align: center;"
        "   padding: 2px 4px;"
        "   min-width: 40px;"
        "   max-width: 110px;"
		"   font: 14px 'Segoe UI', 'Microsoft YaHei';"
		"   color: #2d3748;"
		"   selection-background-color: #4299e1;"
		"}"
		"QSpinBox:hover {"
		"   border-color: #4299e1;"
		"   background: linear-gradient(to bottom, #ffffff, #edf2f7);"
		"}"
		"QSpinBox:focus {"
		"   border-color: #3182ce;"
		"   border-width: 2px;"
		"   background: white;"
		"}";

	// Y坐标SpinBox样式 - 使用不同的颜色区分
	m_SpinboxsheetY =
		"QSpinBox {"
		"   background: linear-gradient(to bottom, #ffffff, #fff5f5);"
        "   border: 1px solid #fed7d7;"
        "   border-radius: 3px;"
		"   text-align: center;"
        "   padding: 2px 4px;"
        "   min-width: 40px;"
        "   max-width: 110px;"
		"   font: 14px 'Segoe UI', 'Microsoft YaHei';"
		"   color: #2d3748;"
		"   selection-background-color: #e53e3e;"
		"}"
		"QSpinBox:hover {"
		"   border-color: #fc8181;"
		"   background: linear-gradient(to bottom, #ffffff, #fed7d7);"
		"}"
		"QSpinBox:focus {"
		"   border-color: #e53e3e;"
		"   border-width: 2px;"
		"   background: white;"
		"}";
}

void CustomPlot::initstyle(const quint8 equipmentType)
{
	// 前置条件检查
	Q_ASSERT(ui != nullptr);
	Q_ASSERT(ui->widget_BloodArea != nullptr);
	Q_ASSERT(ui->widget_trayall != nullptr);

	QLOG_INFO() << "开始初始化样式，设备类型:" << equipmentType;

	try {
		initCommboxView(equipmentType);

		if (m_CheckGroupBox) {
			delete m_CheckGroupBox;
			m_CheckGroupBox = nullptr;
		}
		m_CheckGroupBox = new  QButtonGroup(this);
		m_CheckGroupBox->setExclusive(true); //互斥
		m_CheckGroupBox->addButton(ui->checkBox_downTop, TheGripperDrops::GipperDown_top);
		m_CheckGroupBox->addButton(ui->checkBox_downmind, TheGripperDrops::GipperDown_mid);
		m_CheckGroupBox->addButton(ui->checkBox_downbottom, TheGripperDrops::GipperDown_bottom);
		m_CheckGroupBox->addButton(ui->checkBox_downcustom, TheGripperDrops::GipperDown_custom);
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
		auto ReagentsArea = ui->widget_ReagentsArea->findChildren<QToolButton*>();
		std::sort(ReagentsArea.begin(), ReagentsArea.end(), compairtool);

		// 使用现代C++范围for循环和lambda表达式
		for (auto* reagentTube : ReagentsArea) {
			configureToolButton(reagentTube, reagentTube->text(), [this] {
				ClickReagentsTube();
			});
		}

		// 配置特殊按钮
        configureButtonGroup(ui->toolButton_throwTube, mbloodBtnGroupbox, nullptr); //弃杯孔
		configureButtonGroup(ui->toolButton_origin, mbloodBtnGroupbox, nullptr);    //原点
        configureButtonGroup(ui->toolButton_Clean_1, mbloodBtnGroupbox, nullptr);

        //添加弃杯孔连接
        connect(ui->toolButton_throwTube, &QToolButton::clicked,
                this, [=](){
            uesrClickThrowHole();
        });

        // 添加清洗孔连接
        connect(ui->toolButton_Clean_1, &QToolButton::clicked,
                this, [=](){
            userClickCleanHole();
        });

		//点击原点校验位置
		connect(ui->toolButton_origin, &QToolButton::clicked, this, [=]() {
			userClickOriginAxis();
		});

	}
	catch (const std::exception& e) {
		QLOG_ERROR() << "initstyle初始化失败:" << e.what();
		throw; // 重新抛出异常
	}
	catch (...) {
		QLOG_ERROR() << "initstyle初始化失败: 未知异常";
		throw;
	}

	QLOG_INFO() << "样式初始化完成";
}





void CustomPlot::hideRowsByEquipmentType(quint8 equipmentType)
{
	auto* view = qobject_cast<QListView*>(ui->comboBox_calibrationAarea->view());
	if (!view) return;

	switch (equipmentType)
	{
	case KS600:
		hideRows(view, { EmptyTube_3_BloodNeedle, EmptyTube_4_BloodNeedle,
			EmptyTube_3_Hands, EmptyTube_4_Hands });
		break;
	case KS800:
		hideRows(view, { EmptyTube_4_BloodNeedle, EmptyTube_4_Hands });
		break;
	default:
		break;
	}
}

void CustomPlot::hideRows(QListView* view, const std::vector<int>& rows)
{
	// 参数验证和边界检查
	if (!view || !view->model()) {
		QLOG_WARN() << "hideRows: 无效的view或model指针";
		return;
	}

	const int rowCount = view->model()->rowCount();
	for (int row : rows)
	{
		// 边界检查
		if (row >= 0 && row < rowCount) {
			view->setRowHidden(row, true);
		}
		else {
			QLOG_WARN() << "hideRows: 无效的行索引" << row
				<< ", 有效范围: 0 -" << (rowCount - 1);
		}
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
        { Origin_bloodNeedle,            "原点" },
        { CleanLinque_bloodNeedle,       "[样本针]清洗液S1" },
		{ Bloodsample_BloodNeedle,       "[样本针]样本区" },
		{ EmptyTube_1_BloodNeedle,       "[样本针]试杯区1" },
		{ EmptyTube_2_BloodNeedle,       "[样本针]试杯区2" },
		{ EmptyTube_3_BloodNeedle,       "[样本针]试杯区3" },
		{ EmptyTube_4_BloodNeedle,       "[样本针]试杯区4" },
        { CleanLinque_ReagentNeedle,     "[试剂针]清洗液S1" },
		{ ReagentLinque_ReagentNeedle,   "[试剂针]试剂区" },
		{ TestChannel_ReagentNeedle,     "[试剂针]测试区" },
		{ TestChannel_Hands,             "[抓手]测试区" },
		{ ThrowCup_Hands,                "[抓手]弃杯孔" },
		{ EmptyTube_1_Hands,             "[抓手]试杯区1" },
		{ EmptyTube_2_Hands,             "[抓手]试杯区2" },
		{ EmptyTube_3_Hands,             "[抓手]试杯区3" },
		{ EmptyTube_4_Hands,             "[抓手]试杯区4" }
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
	QElapsedTimer timer;
	timer.start();

	// 前置条件检查（调试期）
	Q_ASSERT(ui != nullptr);
	Q_ASSERT(ui->widget_BloodArea != nullptr);

	// 参数验证（运行时）
	if (!ui->widget_BloodArea) {
		QLOG_ERROR() << "initBloodZoneNum: BloodArea widget未找到";
		return;
	}

	// 使用辅助方法获取显示数量
	const int showNum = getBloodZoneShowCount(indexModels);
	constexpr int MAX_BLOOD_ZONE = CustomPlotConfig::kMaxBloodZone;

	// 确保按钮组存在
	if (!mbloodBtnGroupbox) {
		mbloodBtnGroupbox = new QButtonGroup(this);
	}

	// 批量处理所有血区
	for (int i = 0; i < MAX_BLOOD_ZONE; ++i)
	{
		const auto bloodWidget = ui->widget_BloodArea->findChild<QWidget*>(
			QStringLiteral("widget_Bloodchild_%1").arg(i + 1)
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

		// 配置按钮 - 使用现代C++特性
		int subIndex = 1;
		for (const auto& btn : buttons)
		{
			configureButtonGroup(btn, mbloodBtnGroupbox, [this] {
				ClickBloodTube();
			});
			btn->setText(QString::number(subIndex + i * 10));
			++subIndex;
		}
	}

	update();

	// 性能统计
	QLOG_DEBUG() << "initBloodZoneNum completed in" << timer.elapsed() << "ms";
}




void CustomPlot::initEmptyTubeHole(quint8 indexType)
{
	// 前置条件检查（调试期）
	Q_ASSERT(ui != nullptr);
	Q_ASSERT(ui->widget_trayall != nullptr);

	// 参数验证（运行时）
	if (!ui->widget_trayall) {
		QLOG_ERROR() << "initEmptyTubeHole: trayall widget未找到";
		return;
	}

	// 使用辅助方法获取显示数量
	const int showNum = getTubeTrayShowCount(indexType);
	constexpr int MAX_TUBE_TRAY = CustomPlotConfig::kMaxTubeTray;

	// 确保按钮组存在
	if (!mbloodBtnGroupbox) {
		mbloodBtnGroupbox = new QButtonGroup(this);
	}

	// 处理每个试管托盘
	for (int i = 0; i < MAX_TUBE_TRAY; ++i)
	{
		QString findKey = QStringLiteral("widget_testtubetray_%1").arg(i + 1);
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
			configureButtonGroup(btn, mbloodBtnGroupbox, [this]() {
				ClickEmptyTube();
			});
			btn->setText(QString::number(tubeNumber));

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
		{ KS600, 4 },
		{ KS800, 8 },
		{ KS1200, 12 }  // KS1200 不限制，12是最大值
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
            connect(btn,    &QToolButton::clicked, this, &CustomPlot::ClickTestChannelTube);
		}
	}

	update();
}


void CustomPlot::InitdisplayPointTablewidget(QTableWidget* Tablewidgetpos)
{
	if (!Tablewidgetpos) return;

	// 设置表头
	QStringList header = { tr("编号"), tr("X坐标"), tr("Y坐标"), tr("保存") };
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

	// 设置滚动条策略，确保大量数据时可滚动
	Tablewidgetpos->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	Tablewidgetpos->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);

	// 行高和列宽（确保每行有足够的空间显示内容）
	Tablewidgetpos->verticalHeader()->setDefaultSectionSize(50);  // 增加行高
	Tablewidgetpos->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);  // 固定行高
    Tablewidgetpos->horizontalHeader()->setFixedHeight(35);  // 降低标题高度
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
		"   border-radius: 4px;"  // 进一步降低圆角
		"   min-height: 25px;"     // 降低最小高度
		"   max-height: 25px;"     // 降低最大高度
		"}"
		"QHeaderView::section {"
		"   background: transparent;"
		"   color: #ecf0f1;"
		"   font: bold 12px 'Segoe UI', 'Microsoft YaHei';"  // 进一步降低字体大小
		"   padding: 4px 8px;"     // 进一步减少内边距
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

	// 设置表格尺寸策略，使用Interactive模式允许手动调整，最后一列拉伸
    //Tablewidgetpos->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
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
		{ TheGripperDrops::GipperDown_top, 10 },
		{ TheGripperDrops::GipperDown_mid, 30 },
		{ TheGripperDrops::GipperDown_bottom, 50 }
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
void CustomPlot::displayThrowHole(bool bFindShow, bool bWrite_x, int notifyValue)
{
	auto* axis = SingletonAxis::GetInstance();
	if (bFindShow) {
		// 显示弃杯孔位置
		QPoint throwHolePoint;
		axis->throwTubeHolePos(READ_OPERRAT, throwHolePoint);
		QMap<quint8, QPoint> points = { { 0, throwHolePoint } };
		notifyShowInstrumentPoint(points, "弃杯孔(抓手)微调");
	}
	else
	{
		axis->oper_ThrowTubeHolePos(bWrite_x, notifyValue);
	}
}



//查找原点坐标
void CustomPlot::displayOriginAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue)
{
	auto* pconfAxis = SingletonAxis::GetInstance();
	if (bFindShow) {
		QPoint originPoint;
		pconfAxis->originPos(READ_OPERRAT, originPoint);
		QMap<quint8, QPoint> points = { { 0, originPoint } };
		notifyShowInstrumentPoint(points, "微调原点坐标");
	}
	else
	{
		pconfAxis->operOriginAxis(bWrite_x, notifyValue);
	}
}

//清洗液区
void CustomPlot::displayCleanLinqueAxisPoint(bool bFindShow, quint8  indexZ, bool bWrite_x, int notifyValue)
{
	auto* pconfAxis = SingletonAxis::GetInstance();
	if (bFindShow)
	{
		QPoint cleanPoint;
		pconfAxis->cleanZoneAxisPos(READ_OPERRAT, indexZ, cleanPoint);
		QMap<quint8, QPoint> points = { { 0, cleanPoint } };

		switch (indexZ) {
		case MOTOR_BLOOD_INDEX:
			notifyShowInstrumentPoint(points, "清洗液(样本针)微调");
			break;
		case MOTOR_REAGNET_INDEX:
			notifyShowInstrumentPoint(points, "清洗液(试剂针)微调");
			break;
		default:
			return;
		}
	}
	else {
		pconfAxis->oper_CleanZonePos(bWrite_x, indexZ, notifyValue);
	}
	return;
}

//试剂区
int CustomPlot::displayReagentAxisPoint(bool bFindShow, bool bWrite_x, int notifyValue)
{
	//SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
	QPoint backPoint(0, 0);
	int offset_v = 0;
	if (bFindShow)
	{
		QMap<quint8, QPoint> readAxis;
		readAxis.clear();
		for (int i = 0; i < 10; i++)
		{
			SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT, i, backPoint);
			readAxis.insert(i, backPoint);
		}
		InsertOneChangeOthersChange(readAxis, "试剂区(试剂针)微调");
	}
	else
	{
		SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT, 0, backPoint);
		if (bWrite_x == NOTIFY_XPOINT)
			offset_v = notifyValue - backPoint.x();
		else
			offset_v = notifyValue - backPoint.y();
		for (int i = 0; i < 10; i++)
		{
			SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT, i, backPoint);
			if (bWrite_x == NOTIFY_XPOINT)
				SingletonAxis::GetInstance()->oper_ReagentZonePos(bWrite_x, i, backPoint.x() + offset_v);
			else
				SingletonAxis::GetInstance()->oper_ReagentZonePos(bWrite_x, i, backPoint.y() + offset_v);
		}
	}
	return offset_v;
}

//测试通道
void CustomPlot::displayTestChnAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ, quint8 ChnNum, int notifyValue)
{
	//SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
	quint8 Typtind = 0;
	SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, Typtind);
	if (bFindShow)
	{
		QMap<quint8, QPoint> readAxis;
		readAxis.clear();
		quint8 totalChn = 0;
		QPoint backPoint(0, 0);
		switch (Typtind)
		{
		case KS600: totalChn = 4;  break;
		case KS800: totalChn = 8;   break;
		case KS1200: totalChn = 12; break;
		default: totalChn = 12;     break;
		}
		for (int i = 0; i < totalChn; i++)
		{
			SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, i, indexZ, backPoint);
			readAxis.insert(i, backPoint);
		}
		if (indexZ == MOTOR_REAGNET_INDEX)
			notifyShowInstrumentPoint(readAxis, "测试区(试剂针)微调");
		else if (indexZ == MOTOR_HANDS_INDEX)
			notifyShowInstrumentPoint(readAxis, "测试区(抓手)微调");
	}
	else
	{
		SingletonAxis::GetInstance()->oper_TestChnZoneAxispos(bWrite_x, ChnNum, indexZ, notifyValue);
	}
	return;
}

//测试区
int CustomPlot::displayEmptyTrayAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexZ, quint8 indexTray, quint8 tubeNum, int notifyValue)
{
	//SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
	quint8 totalHole_s = tubeNum;
	int offset_v = 0;
	quint8 totalHole_e = 0;
	QPoint backPoint(0, 0);
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
	if (bFindShow)
	{
		QMap<quint8, QPoint> readAxis;
		readAxis.clear();
		for (int i = totalHole_s; i < totalHole_e; i++)
		{
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, i, indexZ, backPoint);
			readAxis.insert(i, backPoint);
		}
		if (indexZ == MOTOR_BLOOD_INDEX)
			InsertOneChangeOthersChange(readAxis, QString("[样本针]试杯区(%1)偏移微调").arg(indexTray));
		else if (MOTOR_HANDS_INDEX == indexZ)
			InsertOneChangeOthersChange(readAxis, QString("[抓手]试杯区(%1)偏移微调").arg(indexTray));
	}
	else
	{
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, totalHole_s, indexZ, backPoint);
		if (bWrite_x == NOTIFY_XPOINT)
			offset_v = notifyValue - backPoint.x();
		else
			offset_v = notifyValue - backPoint.y();
		for (int k = totalHole_s; k < totalHole_e; k++)
		{
			SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, k, indexZ, backPoint);
			if (bWrite_x == NOTIFY_XPOINT)
				SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_XPOINT, k, indexZ, backPoint.x() + offset_v);
			else
				SingletonAxis::GetInstance()->oper_TestTrayZonaPos(NOTIFY_YPOINT, k, indexZ, backPoint.y() + offset_v);
		}
	}
	return offset_v;
}

int CustomPlot::displayBloodHoleAxisPoint(bool bFindShow, bool bWrite_x, quint8 indexhole, int notifyValue)
{
	//SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
	quint8 Typtind = 0;
	int offset_v = 0;
	SingletonAxis::GetInstance()->equipmentKind(READ_OPERRAT, Typtind);
	quint8 totalHole = 0;
	QPoint backPoint(0, 0);
	switch (Typtind)
	{
	case KS600:  totalHole = 70; break;
	case KS800:  totalHole = 90; break;
	case KS1200: totalHole = 120; break;
	default: totalHole = 120;     break;
	}
	if (bFindShow)
	{
		QMap<quint8, QPoint> readAxis;
		readAxis.clear();

		for (int i = 0; i < totalHole; i++)
		{
			SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT, i, backPoint);
			readAxis.insert(i, backPoint);
		}
		InsertOneChangeOthersChange(readAxis, tr("[样本针]样本区偏移微调"));
	}
	else
	{
		SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT, indexhole, backPoint);
		if (bWrite_x == NOTIFY_XPOINT)
			offset_v = notifyValue - backPoint.x();
		else
			offset_v = notifyValue - backPoint.y();
		for (int i = 0; i < totalHole; i++)
		{
			SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT, i, backPoint);
			if (bWrite_x == NOTIFY_XPOINT)
				SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_XPOINT, i, backPoint.x() + offset_v);
			else
				SingletonAxis::GetInstance()->oper_bloodSampleZonePos(NOTIFY_YPOINT, i, backPoint.y() + offset_v);
		}
	}
	return offset_v;
}

//区域
void CustomPlot::on_comboBox_calibrationAarea_activated(int index)
{
	if (index == -1)
		return;

	if (m_indexDown < 0)
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
	switch (index)
	{
	case Origin_bloodNeedle:
	{
		displayOriginAxisPoint(true, false, 0);
		break;
	}
	case CleanLinque_bloodNeedle:
	{
		displayCleanLinqueAxisPoint(true, MOTOR_BLOOD_INDEX, false, 0);
		break;
	}
	case CleanLinque_ReagentNeedle:
	{
		displayCleanLinqueAxisPoint(true, MOTOR_REAGNET_INDEX, false, 0);
		break;
	}
	case ReagentLinque_ReagentNeedle:
	{
		displayReagentAxisPoint(true, false, 0);
		break;
	}
	case TestChannel_ReagentNeedle:
	{
		displayTestChnAxisPoint(true, false, MOTOR_REAGNET_INDEX, 0, 0);
		break;
	}
	case TestChannel_Hands:
	{
		displayTestChnAxisPoint(true, false, MOTOR_HANDS_INDEX, 0, 0);
		break;
	}
	case ThrowCup_Hands:
	{
		displayThrowHole(true, false, 0);
		break;
	}
	case EmptyTube_1_Hands:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX, 1, 0, 0);

		break;
	}

	case EmptyTube_1_BloodNeedle:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX, 1, 0, 0);
		break;
	}

	case EmptyTube_2_Hands:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX, 2, 0, 0);
		break;
	}

	case EmptyTube_2_BloodNeedle:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX, 2, 0, 0);
		break;
	}

	case EmptyTube_3_Hands:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX, 3, 0, 0);
		break;
	}

	case EmptyTube_3_BloodNeedle:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX, 3, 0, 0);
		break;
	}
	case EmptyTube_4_Hands:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_HANDS_INDEX, 4, 0, 0);
		break;
	}
	case EmptyTube_4_BloodNeedle:
	{
		displayEmptyTrayAxisPoint(true, false, MOTOR_BLOOD_INDEX, 4, 0, 0);
		break;
	}
	case Bloodsample_BloodNeedle:
	{
		displayBloodHoleAxisPoint(true, false, 0, 0);
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
	}
	else {
		connect(btn, &QPushButton::clicked, this, &CustomPlot::BtnClickOthersSavePoint);
	}

	layout->addWidget(btn);
	container->setLayout(layout);

	return container;
}
void CustomPlot::AddBtnSave_backValue(int rowIndex, bool NeedChaneOther)
{
	ui->tableWidget_displayPos->setCellWidget(rowIndex, TableIndexPos::Instrument_save,
		createCenteredButton("保存微调", rowIndex, NeedChaneOther));
}



QSpinBox* CustomPlot::createStandardSpinBox(int value, const QString& style, QWidget* parent)
{
	if (!parent) parent = this;

	QSpinBox* spinBox = new QSpinBox(parent);
	spinBox->setStyleSheet(style);
	spinBox->setAlignment(Qt::AlignCenter);
	spinBox->setMaximum(5500);
    spinBox->setFixedHeight(35);
	spinBox->setFixedWidth(110);
	spinBox->setValue(value);

	QFont font = spinBox->font();
	font.setPixelSize(15);
	spinBox->setFont(font);
    spinBox->setButtonSymbols(QSpinBox::NoButtons);

	return spinBox;
}

QWidget* CustomPlot::createCenteredWidget(QWidget* contentWidget)
{
	QWidget* container = new QWidget(this);
	QGridLayout* layout = new QGridLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignCenter);  // 水平和垂直居中
    layout->addWidget(contentWidget, 0, 0, Qt::AlignCenter);

	container->setProperty("innerWidget", QVariant::fromValue<QWidget*>(contentWidget));
	return container;
}

QWidget* CustomPlot::createCenteredSpinBox(int value, const QString& style)
{
	QSpinBox* spinBox = createStandardSpinBox(value, style);
	return createCenteredWidget(spinBox);
}

void CustomPlot::notifyShowInstrumentPoint(QMap<quint8, QPoint>& displayPoint, const QString& tableName)
{
	ui->label_title->setText(tableName);

	// 清空表格
	ui->tableWidget_displayPos->clearContents();
	ui->tableWidget_displayPos->setRowCount(0);

	for (auto iter = displayPoint.constBegin(); iter != displayPoint.constEnd(); ++iter) {
		const int row = ui->tableWidget_displayPos->rowCount();
		ui->tableWidget_displayPos->insertRow(row);

        // 序号列
        setupIndexColumn(row);

		// X坐标列 - 使用容器居中
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_xpos,
			createCenteredSpinBox(iter.value().x(), m_SpinboxsheetX));

		// Y坐标列
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_ypos,
			createCenteredSpinBox(iter.value().y(), m_SpinboxsheetY));

		AddBtnSave_backValue(row, false);
	}


}



// 设置序号列（不可编辑、居中、灰色背景）
void CustomPlot::setupIndexColumn(int row)
{
    QLabel* labelIndex = new QLabel();
    labelIndex->setText(QString("%1").arg(row + 1));
    labelIndex->setAlignment(Qt::AlignCenter);
    labelIndex->setStyleSheet(
        "QLabel {"
        "   background-color: rgb(204, 204, 204);"
        "   border: none;"
        "   margin: 0px;"
        "   padding: 0px;"
        "}"
    );

    ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::IndexNum, labelIndex);
}

void CustomPlot::setupTableRow(int row, const QPoint& point, bool isEditable)
{
	setupIndexColumn(row);

	if (isEditable) {
		// 可编辑模式 - 使用 SpinBox
		QSpinBox* spinBoxX = createStandardSpinBox(point.x(), m_SpinboxsheetX);
		QWidget* containerX = createCenteredWidget(spinBoxX);
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_xpos, containerX);

		connect(spinBoxX, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
			[=](int value)
		{
			spinBoxX->setEnabled(false);
			UpdateBaseValue_x(value, spinBoxX);
			spinBoxX->setEnabled(true);

			// 同步更新其他行的显示
			refreshAllRowsDisplay();
		});

		QSpinBox* spinBoxY = createStandardSpinBox(point.y(), m_SpinboxsheetY);
		QWidget* containerY = createCenteredWidget(spinBoxY);
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_ypos, containerY);

		connect(spinBoxY, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
			[=](int value)
		{
			spinBoxY->setEnabled(false);
			UpdateBaseValue_y(value);
			spinBoxY->setEnabled(true);

			// 同步更新其他行的显示
			refreshAllRowsDisplay();
		});

		// 保存按钮
		AddBtnSave_backValue(row, true);
	}
	else {
		// 只读模式 - 使用QLabel显示
		QLabel *label_x = new QLabel(QString("%1").arg(point.x()));
		label_x->setFont(QFont("Segoe UI", 12));
		label_x->setAlignment(Qt::AlignCenter);
		label_x->setStyleSheet("color: black; background: transparent;");
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_xpos, label_x);

		QLabel *label_y = new QLabel(QString("%1").arg(point.y()));
		label_y->setFont(QFont("Segoe UI", 12));
		label_y->setAlignment(Qt::AlignCenter);
		label_y->setStyleSheet("color: black; background: transparent;");
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_ypos, label_y);

		// 保存按钮占位列
		QLabel *label_btn = new QLabel("");
		label_btn->setAlignment(Qt::AlignCenter);
		label_btn->setStyleSheet("background-color: rgb(205, 201, 201);");
		ui->tableWidget_displayPos->setCellWidget(row, TableIndexPos::Instrument_save, label_btn);
	}
}



void CustomPlot::refreshAllRowsDisplay()
{
	// 获取当前显示的标题，用于重新显示
	QString currentTitle = ui->label_title->text();

	// 根据标题判断当前显示的数据类型，并重新获取数据
	if (currentTitle.contains("原点坐标")) {
		displayOriginAxisPoint(true, false, 0);
	}
	else if (currentTitle.contains("清洗液")) {
		// 需要根据具体类型调用相应的显示函数
		// 这里需要根据实际情况实现
	}
	// 可以添加其他数据类型的判断


}



void CustomPlot::InsertOneChangeOthersChange(QMap<quint8, QPoint> displayPoint, const QString Tablename)
{
	ui->label_title->setText(Tablename);
	int InsertRow = 0;
	auto iter = displayPoint.begin();

	while (iter != displayPoint.end())
	{
		ui->tableWidget_displayPos->insertRow(InsertRow);
		setupTableRow(InsertRow, iter.value(), InsertRow == 0);
		++iter;
		++InsertRow;
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

	// 获取按钮在 tableWidget 视口中的位置
	QPoint posInTable = ui->tableWidget_displayPos->mapFromGlobal(btn->mapToGlobal(QPoint(0, 0)));
	QModelIndex index = ui->tableWidget_displayPos->indexAt(posInTable);
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

    int config_xpos = static_cast<int>( Point_x->value());
	int config_ypos = static_cast<int>( Point_y->value() );

	switch (m_CalibrationArea)
	{

	case Origin_bloodNeedle:
		displayOriginAxisPoint(false, NOTIFY_XPOINT, config_xpos);
		displayOriginAxisPoint(false, NOTIFY_YPOINT, config_ypos);
		sendcommd.clear();
		QUIUtils::get6_7NumcodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
		QLOG_DEBUG() << QString("修改原点坐标:[%1,%2]").arg(config_xpos).arg(config_ypos) << endl;
		break;
	case CleanLinque_bloodNeedle:
		displayCleanLinqueAxisPoint(false, MOTOR_BLOOD_INDEX, NOTIFY_XPOINT, config_xpos);
		displayCleanLinqueAxisPoint(false, MOTOR_BLOOD_INDEX, NOTIFY_YPOINT, config_ypos);
		sendcommd.clear();
		QUIUtils::get6_7NumcodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
        QLOG_DEBUG() << QString("修改清洗液相对样本针坐标:[%1,%2]").arg(config_xpos).arg(config_ypos) << endl;
		break;
	case CleanLinque_ReagentNeedle:
		displayCleanLinqueAxisPoint(false, MOTOR_REAGNET_INDEX, NOTIFY_XPOINT, config_xpos);
		displayCleanLinqueAxisPoint(false, MOTOR_REAGNET_INDEX, NOTIFY_YPOINT, config_ypos);
		sendcommd.clear();
		QUIUtils::get6_7NumcodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
		QLOG_DEBUG() << QString("修改清洗液相对试剂针坐标:[%1,%2]").arg(config_xpos).arg(config_ypos) << endl;
		break;
	case TestChannel_ReagentNeedle:
		displayTestChnAxisPoint(false, NOTIFY_XPOINT, MOTOR_REAGNET_INDEX, row, config_xpos);
		displayTestChnAxisPoint(false, NOTIFY_YPOINT, MOTOR_REAGNET_INDEX, row, config_ypos);
		writeboard(MOTOR_REAGNET_INDEX, row, sendcommd);
		QLOG_DEBUG() << QString("修改测试通道%1offset试剂针坐标:[%2,%3]").arg(row + 1).arg(config_xpos).arg(config_ypos) << endl;
		break;
	case TestChannel_Hands:
		displayTestChnAxisPoint(false, NOTIFY_XPOINT, MOTOR_HANDS_INDEX, row, config_xpos);
		displayTestChnAxisPoint(false, NOTIFY_YPOINT, MOTOR_HANDS_INDEX, row, config_ypos);
		writeboard(MOTOR_HANDS_INDEX, row, sendcommd);
		QLOG_DEBUG() << QString("修改测试通道%1offset抓手坐标:[%2,%3]").arg(row + 1).arg(config_xpos).arg(config_ypos) << endl;
		break;
	case ThrowCup_Hands:
		displayThrowHole(false, NOTIFY_XPOINT, config_xpos);
		displayThrowHole(false, NOTIFY_YPOINT, config_ypos);
		sendcommd.clear();
		QLOG_DEBUG() << QString("修改弃杯孔坐标:[%1,%2]").arg(config_xpos).arg(config_ypos) << endl;
		QUIUtils::get6_7NumcodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
		break;
	default:
		break;
	}
	emit writdAxisata(sendcommd, "缓存坐标指令");
	//btn->setText("已保存");
	return;
}

void  CustomPlot::UpdateBaseValue_x(int NotifyValue_x, QSpinBox *pbox)
{
	pbox->setEnabled(false);
	int offsetValue = 0;
	switch (m_CalibrationArea)
	{
	case ReagentLinque_ReagentNeedle:   offsetValue = displayReagentAxisPoint(false, NOTIFY_XPOINT, NotifyValue_x);                             break;
	case EmptyTube_1_Hands:             offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_HANDS_INDEX, 1, 0, NotifyValue_x);     break;
	case EmptyTube_1_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_BLOOD_INDEX, 1, 0, NotifyValue_x);     break;
	case EmptyTube_2_Hands:             offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_HANDS_INDEX, 2, 60, NotifyValue_x);    break;
	case EmptyTube_2_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_BLOOD_INDEX, 2, 60, NotifyValue_x);    break;
	case EmptyTube_3_Hands:             offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_HANDS_INDEX, 3, 120, NotifyValue_x);   break;
	case EmptyTube_3_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_BLOOD_INDEX, 3, 120, NotifyValue_x);   break;
	case EmptyTube_4_Hands:             offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_HANDS_INDEX, 4, 180, NotifyValue_x);   break;
	case EmptyTube_4_BloodNeedle:       offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_XPOINT, MOTOR_BLOOD_INDEX, 4, 180, NotifyValue_x);   break;
	case Bloodsample_BloodNeedle:       offsetValue = displayBloodHoleAxisPoint(false, NOTIFY_XPOINT, 0, NotifyValue_x);                         break;
	default:
		break;
	}
	updateNotifyOffsetValue(NOTIFY_XPOINT, offsetValue);
	pbox->setEnabled(true);

}
void  CustomPlot::UpdateBaseValue_y(int NotifyValue_y)
{
	int offsetValue = 0;
	switch (m_CalibrationArea)
	{
	case ReagentLinque_ReagentNeedle:  offsetValue = displayReagentAxisPoint(false, NOTIFY_YPOINT, NotifyValue_y);                             break;
	case EmptyTube_1_Hands:            offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_HANDS_INDEX, 1, 0, NotifyValue_y);     break;
	case EmptyTube_1_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_BLOOD_INDEX, 1, 0, NotifyValue_y);     break;
	case EmptyTube_2_Hands:            offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_HANDS_INDEX, 2, 60, NotifyValue_y);    break;
	case EmptyTube_2_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_BLOOD_INDEX, 2, 60, NotifyValue_y);    break;
	case EmptyTube_3_Hands:            offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_HANDS_INDEX, 3, 120, NotifyValue_y);   break;
	case EmptyTube_3_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_BLOOD_INDEX, 3, 120, NotifyValue_y);   break;
	case EmptyTube_4_Hands:            offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_HANDS_INDEX, 4, 180, NotifyValue_y);   break;
	case EmptyTube_4_BloodNeedle:      offsetValue = displayEmptyTrayAxisPoint(false, NOTIFY_YPOINT, MOTOR_BLOOD_INDEX, 4, 180, NotifyValue_y);   break;
	case Bloodsample_BloodNeedle:      offsetValue = displayBloodHoleAxisPoint(false, NOTIFY_YPOINT, 0, NotifyValue_y);                         break;
	default:
		break;
	}
	updateNotifyOffsetValue(NOTIFY_YPOINT, offsetValue);
}

void CustomPlot::updateNotifyOffsetValue(bool bupdate_x, const int offsetValue)
{
	int rowscount = ui->tableWidget_displayPos->rowCount();
	const int xposCol = TableIndexPos::Instrument_xpos;
	const int yposCol = TableIndexPos::Instrument_ypos;

	for (int i = 0; i < rowscount; i++)
	{
		if (bupdate_x == NOTIFY_XPOINT)
		{
			// 安全获取QLabel控件（现在使用setCellWidget设置的QLabel）
			QWidget* widget = ui->tableWidget_displayPos->cellWidget(i, xposCol);
			if (widget == nullptr) {
				continue;
			}

			// 尝试转换为QLabel
			QLabel* label = qobject_cast<QLabel*>(widget);
			if (label == nullptr) {
				// 如果不是QLabel，可能是QWidget容器，尝试获取内部的QLabel
				QLabel* innerLabel = widget->findChild<QLabel*>();
				if (innerLabel == nullptr) {
					continue;
				}
				label = innerLabel;
			}

			// 更新QLabel文本
			int tablevalue = label->text().toInt();
			label->setText(QString("%1").arg(tablevalue + offsetValue));
		}
		else
		{
			// 安全获取QLabel控件
			QWidget* widget = ui->tableWidget_displayPos->cellWidget(i, yposCol);
			if (widget == nullptr) {
				continue;
			}

			// 尝试转换为QLabel
			QLabel* label = qobject_cast<QLabel*>(widget);
			if (label == nullptr) {
				// 如果不是QLabel，可能是QWidget容器，尝试获取内部的QLabel
				QLabel* innerLabel = widget->findChild<QLabel*>();
				if (innerLabel == nullptr) {
					continue;
				}
				label = innerLabel;
			}

			// 更新QLabel文本
			int tablevalue = label->text().toInt();
			label->setText(QString("%1").arg(tablevalue + offsetValue));
		}
	}
	update();
	return;
}

void CustomPlot::writeboard(quint8 intdexZ, quint8 row, QByteArrayList &sendcommd)
{
	QByteArray x_arry;
	QByteArray y_arry;
	sendcommd.clear();
	if (intdexZ == MOTOR_REAGNET_INDEX)
	{
		if (row >= 0 && row < 5)
		{
			QUIUtils::get89NumberCodeArry(x_arry, AXISPOS_BUFFER, y_arry, AXISPOS_BUFFER);
		}
		else if (row >= 5 && row < 10)
		{
			QUIUtils::get0a0bNumberCodeArry(x_arry, AXISPOS_BUFFER, y_arry, AXISPOS_BUFFER);
		}
		else
		{
			QUIUtils::get0c0dNumberCodeArry(x_arry, AXISPOS_BUFFER, y_arry, AXISPOS_BUFFER);
		}
	}
	else if (intdexZ == MOTOR_HANDS_INDEX)
	{
		if (row >= 0 && row < 3)
		{
			QUIUtils::get0c0dNumberCodeArry(x_arry, AXISPOS_BUFFER, y_arry, AXISPOS_BUFFER);
		}
		else if (row >= 3 && row <= 7)
		{
			QUIUtils::get0e0fNumberCodeArry(x_arry, AXISPOS_BUFFER, y_arry, AXISPOS_BUFFER);
		}
		else
		{
			QUIUtils::get1011NumberCodeArry(x_arry, AXISPOS_BUFFER, y_arry, AXISPOS_BUFFER);
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
	switch (m_CalibrationArea)
	{
	case ReagentLinque_ReagentNeedle:
		sendcommd.clear();
		QUIUtils::get6_7NumcodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
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
		QUIUtils::get1213NumberCodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
		QUIUtils::get1415NumberCodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
		break;
	case Bloodsample_BloodNeedle:
		sendcommd.clear();
		QUIUtils::get1011NumberCodeArry(modifyParaarry_x, AXISPOS_BUFFER, modifyParaarry_y, AXISPOS_BUFFER);
		sendcommd << modifyParaarry_x << modifyParaarry_y;
		break;
	default:
		break;
	}
	emit writdAxisata(sendcommd, "缓存坐标指令");
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
	backMachinOrigin();
	return;
}

void CustomPlot::backMachinOrigin()
{
	QPoint locAxis(0, 0);
	auto *pconfAxis = SingletonAxis::GetInstance();
	if (!pconfAxis)
	{
		QLOG_WARN() << "调试界面复位坐标系为NULL";
	}
	pconfAxis->originPos(READ_OPERRAT, locAxis);
	QByteArrayList cmd = QUIUtils::backZAxisandmoveOrigin(locAxis);
	if (!cmd.isEmpty())
        sendCommder(AXIS_BACKRESET, cmd,"复位位置",locAxis);
}


void CustomPlot::sendCommder(const int indexAct,const QByteArrayList cmd,QString info,QPoint locAAxis)
{
    Q_EMIT testMachineAxis(indexAct,cmd);
    auto plabel = ui->label_movedPos;
    plabel->setText(info + QString("%1  %2").arg(locAAxis.x()).arg(locAAxis.y()));
}



void CustomPlot::ExitCoordinateSaving()
{
	quint8 indexHole = 60;
	QPoint savingAxis(0, 0);
	QMap<quint8, quint16> ExitSaveCoordin;
	QByteArrayList send_data_;
	ExitSaveCoordin.clear();
	send_data_.clear();
	for (int n = 0; n < 3; n++) {
		int indexTray = n + 1;
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, indexHole*indexTray, MOTOR_HANDS_INDEX, savingAxis);
		ExitSaveCoordin.insert(n, savingAxis.y());
	}
	ExitSaveCoordin.insert(3, 0);
	ExitSaveCoordin.insert(4, 0);

	QByteArray writeinboard = QUIUtils::bufferCodeAxis(AXIS_TRAY_OFFSET_HANDSY, AXISPOS_WRITE, ExitSaveCoordin);
	send_data_.push_back(writeinboard);

	QLOG_DEBUG() << "退出坐标调试保存坐标尾端" << writeinboard.toHex(' ').trimmed().toUpper() << "leng=" << writeinboard.size() << endl;
	emit writdAxisata(send_data_, "保存坐标微调界面坐标");
	return;
}

bool CustomPlot::QuerytoModifyCoordinates(QMap<int, QPoint> PointGather, int FindIndexKey, QPoint &FindedCoordinates)
{
	bool bContainsKey = false;
	bContainsKey = PointGather.contains(FindIndexKey);
	if (!bContainsKey) return false;
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

void CustomPlot::ChangeControlColors(bool changeColor, int selectedIndex)
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
	}
	else {
		item->setBackground(DEFAULT_COLOR);
	}
}

//血样区BtnClick
void  CustomPlot::ClickBloodTube()
{
	if (m_CalibrationArea != Bloodsample_BloodNeedle) {
		QMessageBox::about(this, "血样区", "请选择匹配的校准区域!");
		return;
	}

	//SingletonAxis *pconfAxis = SingletonAxis::GetInstance();
	QObject *object = QObject::sender();
	QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
	QString object_name = pushbtn->text();
	int index = object_name.toInt() - 1;
	if (m_selectRows != index && m_selectRows > 0)
	{
		SelectHoleChangebgm(false, m_selectRows, 0);
	}

	mcodeNum = 0;
	mbtnClickPos.setX(0);
	mbtnClickPos.setY(0);
	SingletonAxis::GetInstance()->bloodSampleZonePos(READ_OPERRAT, index, mbtnClickPos);

	SelectHoleChangebgm(true, index, 0);

	QByteArrayList modifyarry = QUIUtils::TargCleanLinqueHole(MOTOR_BLOOD_INDEX, m_downhigh,
		mbtnClickPos, GRIPPERNORMAL);

    sendCommder(AXIS_BLOODPIN_OFF_BLOOD,modifyarry,"样本针off样本区",mbtnClickPos);

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

    QByteArrayList modifyArray;
    int indexAct = 0;
    mcodeNum = 0;
	// 确定针类型和操作类型
	if (isBloodNeedleArea) {
		needleIndex = MOTOR_BLOOD_INDEX;
        operationType = "样本针offset";
        indexAct =  AXIS_BlOODPIN_OFF_TARY;
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, index, needleIndex, targetPoint);
        modifyArray = QUIUtils::TargCleanLinqueHole(needleIndex, m_downhigh, targetPoint, GRIPPERNORMAL);
	}
	else {
		needleIndex = MOTOR_HANDS_INDEX;
        operationType = "抓手offset";
        indexAct =  AXIS_HANDS_OFF_TARY;
		SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, index, needleIndex, targetPoint);
        modifyArray = QUIUtils::TargCleanLinqueHole(needleIndex, m_downhigh, targetPoint, GRABCUPWITHGRIPPER);
        quint8 indexCode = modifyArray.size(); //抓手就抬起来
        QUIUtils::allZAxisBackOrigin(modifyArray, indexCode);
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

	// 更新选择的行
	if (m_selectRows != index && m_selectRows > 0) {
		SelectHoleChangebgm(false, m_selectRows - 60 * (trayId - 1), trayId - 1);
	}

	// 更新UI并发送信号
    QString actname = operationType + QString::number(trayId) + "号试管盘";
    sendCommder(indexAct,modifyArray,actname,targetPoint);
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

	QPoint targetPoint;
	int motorIndex = MOTOR_HANDS_INDEX;
	QByteArrayList modifyArray;

	if (m_CalibrationArea == TestChannel_ReagentNeedle || m_CalibrationArea == TestChannel_Hands) {
		motorIndex = (m_CalibrationArea == TestChannel_ReagentNeedle) ? MOTOR_REAGNET_INDEX : MOTOR_HANDS_INDEX;
		SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, index, motorIndex, targetPoint);
		ChangeControlColors(true, index);
	}
	else {
		SingletonAxis::GetInstance()->chnZoneAxisPos(READ_OPERRAT, index, MOTOR_HANDS_INDEX, targetPoint);
		motorIndex = MOTOR_HANDS_INDEX;
	}


    bool sucktube = motorIndex ? MOTOR_HANDS_INDEX : MOTOR_REAGNET_INDEX;
    modifyArray = QUIUtils::TargCleanLinqueHole(motorIndex,m_downhigh,targetPoint,sucktube);
    if(motorIndex == MOTOR_HANDS_INDEX){
        quint8 indexCode = modifyArray.size(); //抓手就抬起来
        QUIUtils::allZAxisBackOrigin(modifyArray, indexCode);
    }
    sendCommder(AXIS_OFF_CHANNEL,modifyArray,"测试通道校验位置：",targetPoint);
}

//点击试剂区
void  CustomPlot::ClickReagentsTube()
{
	if (m_CalibrationArea != ReagentLinque_ReagentNeedle)
	{
		QMessageBox::about(this, "试剂区", "请选择匹配的校准区域!");
		return;
	}

	QByteArrayList  Notifyarry;
	Notifyarry.clear();
	QObject *object = QObject::sender();
	QToolButton *pushbtn = qobject_cast<QToolButton *>(object);
	QStringList object_name = pushbtn->objectName().split("_");
	int index = object_name.last().toInt() - 20; //20起
	if (m_selectRows != index && m_selectRows > 0)
	{
		SelectHoleChangebgm(false, m_selectRows, 0);
	}


	QPoint getOutPoint(0, 0);
	SingletonAxis::GetInstance()->reagetZoneAxisPos(READ_OPERRAT, index, getOutPoint);
    //QLOG_DEBUG() << "click试剂区" << index << "坐标" << getOutPoint << endl;

	SelectHoleChangebgm(true, index, 0);

	QByteArrayList modifyarry;
	modifyarry = QUIUtils::TargCleanLinqueHole(MOTOR_REAGNET_INDEX, m_downhigh,
		getOutPoint, GRIPPERNORMAL);

    sendCommder(AXIS_REAGENTPIN, modifyarry,"试剂区校验：",getOutPoint);
	return;
}


void CustomPlot::_sendmoveActiveToEquipment(AreasCalibration indexZone, QPoint MovingAxis, quint8 indexZ)
{
	QString outputText = "";
	QByteArrayList modifyarry;
	modifyarry.clear();

	switch (indexZone)
	{
	case origin:
		outputText = "原点校验位置:";
		QUIUtils::_Locatetheorigin(modifyarry, MovingAxis, m_downhigh);
        sendCommder(AXIS_ORIGIN,modifyarry,outputText,MovingAxis);
		break;
	case detergent:
		outputText = "清洗剂校验位置:";
		modifyarry = QUIUtils::TargCleanLinqueHole(indexZ, m_downhigh, MovingAxis, GRIPPERNORMAL);
        sendCommder(AXIS_OFF_CLEANING,modifyarry,outputText,MovingAxis);
		break;
	}
	return;
}


//点击原点
void CustomPlot::userClickOriginAxis()
{
	if (m_CalibrationArea != Origin_bloodNeedle)
	{
        QMessageBox::about(this, "原点", "请选择匹配的校准区域!");
		return;
	}
	QPoint getOutPoint(0, 0);
	SingletonAxis::GetInstance()->originPos(READ_OPERRAT, getOutPoint);

	ChangeControlColors(true, 0);

	_sendmoveActiveToEquipment(origin, getOutPoint, 0);
	return;
}

/*
*点击清洗孔
* @brief CustomPlot::userClickCleanHole
*/
void CustomPlot::userClickCleanHole()
{
    if (CleanLinque_bloodNeedle != m_CalibrationArea && CleanLinque_ReagentNeedle != m_CalibrationArea)
    {
        QMessageBox::about(this, "清洗液区", "请选择匹配的校准区域!");
        return;
    }
    quint8  indexZ = 0;
    QPoint  getOutPoint(0, 0);
    if (CleanLinque_bloodNeedle == m_CalibrationArea)
    {
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT, MOTOR_BLOOD_INDEX, getOutPoint);
        indexZ = MOTOR_BLOOD_INDEX;
    }
    else
    {
        SingletonAxis::GetInstance()->cleanZoneAxisPos(READ_OPERRAT, MOTOR_REAGNET_INDEX, getOutPoint);
        indexZ = MOTOR_REAGNET_INDEX;
    }

    ChangeControlColors(true, 0);

    _sendmoveActiveToEquipment(detergent, getOutPoint, indexZ);

    return;
}

/*
* 点击弃杯孔
* @brief CustomPlot::uesrClickThrowHole
*/
void CustomPlot::uesrClickThrowHole()
{
    QPoint throwPoint;
    SingletonAxis::GetInstance()->throwTubeHolePos(READ_OPERRAT, throwPoint);

    QByteArrayList modifyArray;

    if (ThrowCup_Hands != m_CalibrationArea) {
        const int throwCupsDown = INI_File()._gethandsdownthrowcpus();
        modifyArray = QUIUtils::moveThrowCupCommands(throwPoint, throwCupsDown);

    }
    else {
        const int downMM = m_downhigh;
        quint8 codeNum = 0;
        QUIUtils::allZAxisBackOrigin(modifyArray, codeNum);
        codeNum = modifyArray.size();
        QByteArray cmd1 = QUIUtils::outPutxyMoveAxisComd(throwPoint, codeNum);
        QByteArray cmd2 = QUIUtils::outPutHandsLocComd(downMM, GRIPPERNORMAL, codeNum);
        modifyArray << cmd1 << cmd2;
        ChangeControlColors(true, 0);
    }

    sendCommder(AXIS_OFF_THROW,modifyArray,"弃杯孔位置:",throwPoint);
}









void CustomPlot::recvCalibrationMoved(const int finishAct)
{
    QString outdata = "";

    // 根据不同的动作设置对应的提示信息
    switch(finishAct)
    {
    case AXIS_BACKRESET:
        outdata = "复位完成!";
        break;
    case AXIS_ORIGIN:
        outdata = "原点位置校准完成!";
        break;
    case AXIS_BLOODPIN_OFF_BLOOD:
        outdata = "样本针-样本区校准完成!";
        break;
    case AXIS_HANDS_OFF_TARY:
        outdata = "抓手-试管盘校准完成!";
        break;
    case AXIS_BlOODPIN_OFF_TARY:
        outdata = "样本针-试管盘校准完成!";
        break;
    case AXIS_REAGENTPIN:
        outdata = "试剂针-试剂区校准完成!";
        break;
    case AXIS_OFF_CHANNEL:
        outdata = "通道位置校准完成!";
        break;
    case AXIS_OFF_CLEANING:
        outdata = "清洗液S1校准完成!";
        break;
    case AXIS_OFF_THROW:
        outdata = "弃杯区校准完成!";
        break;
    case AXIS_OFF_CAMERA:
        outdata = "摄像头位置校准完成!";
        break;
    case AXIS_TEST_SUCKPRP:
        outdata = "测试吸PRP位置校准完成!";
        break;
    case AXIS_TESTCATCHCUPS:
        outdata = "完成 单个抓杯丢杯动作!";
        continueCatchCups();
        break;
    default:
        outdata = QString("动作 %1 完成!").arg(finishAct);
        break;
    }

    ui->label_movedPos->setText(outdata);
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





void CustomPlot::slotsendcode(QByteArrayList data_)
{
	emit writdAxisata(data_, "吸吐");
}

void CustomPlot::on_spinBox_downValue_valueChanged(int arg1)
{
	m_downhigh = arg1;
	update();
}

void CustomPlot::on_pushButton_TrayHands_2_clicked()
{
	QElapsedTimer timer;
	timer.start();
	QLOG_INFO() << "开始执行试管移动测试";

	// 测试状态管理
	if (m_testInProgress) {
		QMessageBox::warning(this, tr("测试进行中"), tr("请等待当前测试完成"));
		QLOG_WARN() << "测试进行中，拒绝新请求";
		return;
	}
	m_testInProgress = true;

	// 前置条件检查
	if (m_indexDown < 0) {
		QMessageBox::about(this, tr("测试失败"), tr("请配试抓手下降高度!"));
		QLOG_ERROR() << "抓手下降高度未配置";
		m_testInProgress = false;
		return;
	}

	// 获取并验证托盘参数
	mfromtray = ui->spinBox_from_tray->value() - 1;
    mendtray =  ui->spinBox_endtray->value() - 1;

	// 参数验证
	if (mfromtray < 0 || mfromtray >= CustomPlotConfig::kMaxTubeTray ||
        mendtray < 0 ||  mendtray >= CustomPlotConfig::kMaxTubeTray) {
		QLOG_ERROR() << "无效的托盘索引: from=" << mfromtray << ", end=" << mendtray;
		QMessageBox::critical(this, tr("参数错误"), tr("托盘索引超出有效范围"));
		m_testInProgress = false;
		return;
	}

	if (mfromtray != mendtray) {
		mfromHole.clear();
		mendHole.clear();
		mtest_catch_put_commad.clear();

		// 生成试管编号
		for (int i = 0; i < CustomPlotConfig::kTotalTubeNum; i++) {
			mfromHole.push_back(mfromtray * CustomPlotConfig::kTotalTubeNum + i);
			mendHole.push_back(mendtray * CustomPlotConfig::kTotalTubeNum + i);
		}

		// 验证容器大小匹配
		if (mfromHole.size() != mendHole.size()) {
			QLOG_ERROR() << "试管数量不匹配: from=" << mfromHole.size()
				<< ", end=" << mendHole.size();
			QMessageBox::critical(this, tr("系统错误"), tr("试管数量不匹配，请检查配置"));
			m_testInProgress = false;
			return;
		}

		ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::BLINK);

		// 改进随机种子生成机制
		std::random_device rd;
		unsigned seed = rd();
		std::shuffle(mfromHole.begin(), mfromHole.end(), std::default_random_engine(seed));
		//从小到大放置孔号
		std::sort(mendHole.begin(), mendHole.end());
		// 生成移动指令
		int index_ = 0;
		auto it = mfromHole.constBegin();
		bool generationSuccess = true;

		while (it != mfromHole.constEnd()) {
			int hole_catch = *it;

			// 边界检查，防止容器大小不匹配导致的越界
			if (index_ >= mendHole.size()) {
				QLOG_ERROR() << "索引越界: index_" << index_ << "mendHole大小:" << mendHole.size();
				generationSuccess = false;
				break;
			}

			int hole_put = mendHole.at(index_);

			// 增强错误处理
			try {
				testCatchPutDownCup(hole_catch, hole_put, mtest_catch_put_commad);

			}
			catch (const std::exception& e) {
				QLOG_ERROR() << "生成移动指令失败: " << e.what()
					<< " hole_catch=" << hole_catch << " hole_put=" << hole_put;
				generationSuccess = false;
				break;
			}

			index_++;
			it++;
		}

		// 检查指令生成是否成功
		if (!generationSuccess) {
			QLOG_ERROR() << "移动指令生成失败，中止测试";
			QMessageBox::critical(this, tr("指令生成失败"), tr("移动指令生成失败，请检查系统配置"));
			m_testInProgress = false;
			return;
		}

		// 发送测试指令
		try {
            sednCatchCups(mtest_catch_put_commad);
		}
		catch (const std::exception& e) {
			QLOG_ERROR() << "发送测试指令失败: " << e.what();
			QMessageBox::critical(this, tr("发送失败"), tr("测试指令发送失败"));
			m_testInProgress = false;
			return;
		}
	}
	else {
		QMessageBox::about(this, tr("测试失败"), tr("相同试管盘请勿移动!"));
		QLOG_WARN() << "起始托盘和结束托盘相同: " << mfromtray;
		m_testInProgress = false;
	}

	// 性能监控
	QLOG_INFO() << "试管移动测试完成，耗时: " << timer.elapsed() << "ms";

	return;
}

void CustomPlot::testCatchPutDownCup(const int form, const int end, QMap<int, QByteArrayList> &groupDirectives)
{
	QByteArrayList once_directives;
	QPoint from_tube_axis(0, 0);
	SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, form, MOTOR_HANDS_INDEX, from_tube_axis);
	QPoint end_tube_axis(0, 0);
	SingletonAxis::GetInstance()->testTaryZoneAxisPos(READ_OPERRAT, end, MOTOR_HANDS_INDEX, end_tube_axis);
	once_directives = QUIUtils::testHandsCatchtubePrecise(from_tube_axis, end_tube_axis, m_downhigh);
	groupDirectives.insert(end, once_directives);
	return;
}

QByteArrayList CustomPlot::takeFirstFromMap(QMap<int, QByteArrayList>& map)
{
    if (map.isEmpty()) {
        return QByteArrayList(); // 返回空列表
    }

    // 获取第一个元素的迭代器（QMap 自动按 key 排序）
    auto it = map.begin();

    // 取出第一个 QByteArrayList
    QByteArrayList result = it.value();

    // 删除第一个元素
    map.erase(it);

    return result;
}

bool CustomPlot::sednCatchCups(QMap<int, QByteArrayList>& catchMap)
{
    const QByteArrayList valueArry = takeFirstFromMap(catchMap);
    if(!valueArry.isEmpty())
         sendCommder(AXIS_TESTCATCHCUPS, valueArry,"抓杯测试..",QPoint(0,0));
    else
        return true;
    return false;
}

void CustomPlot::continueCatchCups()
{
   const bool isFinished = sednCatchCups(mtest_catch_put_commad);
   if(isFinished)
   {
       ui->widget_displayled->setStates(QSimpleLed::LEDSTATES::OFF);

       //调试界面机器复位
       backMachinOrigin();

       // 重置测试状态
       m_testInProgress = false;
       QLOG_INFO() << "所有测试抓杯指令发送完成，测试状态已重置";
   }

}






//移动到摄像头区域下样本针
void CustomPlot::on_toolButton_video_clicked()
{
	if (!mCameraWindow.isNull()) {
		mCameraWindow->show();
		mCameraWindow->raise();
		mCameraWindow->activateWindow();
		return;
	}

	CameraWindow *win = new CameraWindow(nullptr);
	win->setAttribute(Qt::WA_DeleteOnClose);   // 关闭时自动 delete

	connect(win, &CameraWindow::moveTestHeigh,
		this, &CustomPlot::onMoveTestHeigh);

	connect(win, &CameraWindow::backOrigin,
		this, &CustomPlot::onbackOrigin);

	connect(win, &CameraWindow::testSuckPrpAct,
		this, &CustomPlot::onTestSuckPrpAct);


	connect(win, &QObject::destroyed, this, [this]() {
		mCameraWindow.clear();  // 清空 QPointer（其实不是必须，isNull() 已能检测）
	});

	// 窗口居中
	if (QWidget *parentWindow = this->window()) {
		win->adjustSize();
		QPoint center = parentWindow->geometry().center();
		QRect winRect = win->geometry();
		win->move(center.x() - winRect.width() / 2,
			center.y() - winRect.height() / 2);
	}

	mCameraWindow = win;   // QPointer 接管观察
	win->show();
}


void CustomPlot::onMoveTestHeigh(const int x, const int y, const double downMm)
{
	QPoint locVideo(x, y); //200 3140

	ui->label_movedPos->setText(QString("测高位置:[%1,%2]").arg(x).arg(y));
	QByteArrayList modifyArray = QUIUtils::backZAxisandmoveOrigin(QPoint(0, 0));
	quint8 codeNum = modifyArray.size();
	QByteArray cmd0 = QUIUtils::outPutxyMoveAxisComd(locVideo, codeNum);
	QByteArray cmd1 = QUIUtils::outPutBloodPinLocComd(downMm, codeNum);
	modifyArray << cmd0 << cmd1;

    sendCommder(AXIS_OFF_CAMERA,modifyArray,"测高摄像头坐标:",locVideo);

}



void CustomPlot::onbackOrigin()
{
	backMachinOrigin();
}

void CustomPlot::onTestSuckPrpAct(const QByteArrayList data)
{
    sendCommder(AXIS_TEST_SUCKPRP,data,"测试吸PRP...",QPoint(0,0));
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

			//调试界面机器复位
			backMachinOrigin();

			//确保保存操作完成（如果可能是异步的，需要等待）
			ExitCoordinateSaving();

			//可选 - 等待操作完成
			//QCoreApplication::processEvents();
		}

		VerifyCoordinates::GetInstance()->startComparingCoordinates(false);

		event->accept();  // ✅ 接受关闭事件

	}
	else {
		event->ignore();  // ✅ 忽略关闭事件
	}
}
